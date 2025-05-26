/* SPDX-FileCopyrightText: 2001-2022 AIM GmbH <info@aim-online.com> */
/* SPDX-License-Identifier: MIT OR GPL-2.0-or-later */

/*! \file UsbListSynchWin.c

   File that contains implementations for Transfer and RT buffer list functions
*/
#include "UsbListSynchWin.h"
#include "Debug.h"
#include "NetchipUsbIo.h"
#include "DriverMemory.h"



/* Synchronizes all transfers, that are in the transfer list, between global memory and its mirror */
AiInt32 UsbSynchronizeTransfers(TY_API_DEV* p_api_dev, short uc_Biu, enum SYNCH_DIRECTION direction)
{
    AiInt32 Status = STATUS_SUCCESS;
    PDEVICE_CONTEXT p_DeviceContext;
    ULONG ul_TransferCount;
    ULONG i;
    WDFCOLLECTION TransferList;
    WDFOBJECT TransferObject;
    PTRANSFER_LIST_CONTEXT p_TransferListContext;
    PTRANSFER_CONTEXT p_TransferContext;

    TRACE("UsbSynchronizeTransfers on BIU %d with direction %d\n", uc_Biu, direction);

    p_DeviceContext = (PDEVICE_CONTEXT) p_api_dev->p_DeviceContext;
    TransferList = p_DeviceContext->TransferList[uc_Biu];

    if(TransferList == NULL)
    {
        ERROR("No transfer list available for this BIU\n");
        return STATUS_UNSUCCESSFUL;
    }

    p_TransferListContext = GetTransferListContext(TransferList);
    WdfWaitLockAcquire(p_TransferListContext->Lock, NULL);

    ul_TransferCount = WdfCollectionGetCount(TransferList);


    i = 0;

    while(i < ul_TransferCount && Status == STATUS_SUCCESS)
    {
        TransferObject = WdfCollectionGetItem(TransferList, i);
        p_TransferContext = GetTransferContext(TransferObject);
        Status = UsbSynchronizeTransfer(p_api_dev, p_TransferContext, TRUE, direction);

        i++;

    }

    WdfWaitLockRelease(p_TransferListContext->Lock);

    if(!NT_SUCCESS(Status))
    {
        ERROR("Synchronization of Transfers failed\n");
    }

    return Status;
}


/* Synchronizes all RT buffers of the buffer list of a specific BIU 
   between global memory and its mirror */
AiInt32 UsbSynchronizeRTBuffers(TY_API_DEV* p_api_dev, short uc_Biu, 
                                enum SYNCH_DIRECTION direction)
{
    AiInt32 Status = STATUS_SUCCESS;
    PDEVICE_CONTEXT p_DeviceContext;
    ULONG ul_BufferCount;
    ULONG i;
    WDFCOLLECTION RTBufferList;
    WDFOBJECT RTBufferObject;
    PRT_BUFFER_CONTEXT p_BufferContext;

    TRACE("UsbSynchronizeRTBuffers on BIU %d with direction %d\n", uc_Biu, direction);

    p_DeviceContext = (PDEVICE_CONTEXT) p_api_dev->p_DeviceContext;
    RTBufferList = p_DeviceContext->RTBufferList[uc_Biu];

    if(RTBufferList == NULL)
    {
        ERROR("No RT Buffer List list available for this BIU\n");
        return STATUS_UNSUCCESSFUL;
    }

   
    ul_BufferCount = WdfCollectionGetCount(RTBufferList);

    i = 0;

    while(i < ul_BufferCount && Status == STATUS_SUCCESS)
    {
        RTBufferObject = WdfCollectionGetItem(RTBufferList, i);
        p_BufferContext = GetRtBufferContext(RTBufferObject);
        Status = UsbSynchronizeRTBuffer(p_api_dev, p_BufferContext, direction);

        i++;

    }

    if(!NT_SUCCESS(Status))
    {
        ERROR("Synchronization of RT Buffers failed\n");
    }

    return Status;
}


/* Synchronizes one specific transfer between global memory and its mirror */
AiInt32 UsbSynchronizeTransfer(TY_API_DEV* p_api_dev, PTRANSFER_CONTEXT p_TransferContext, 
                                BOOLEAN b_IncludeBufferData, enum SYNCH_DIRECTION direction)
{
    AiInt32 Status = STATUS_UNSUCCESSFUL;
    struct ty_api_bc_bh_desc* p_BufferHeader;
    struct ty_api_bc_xfer_desc* p_TransferDescriptor;
    unsigned char uc_QueueSize;
    unsigned short us_NumBuffers;
    size_t RelativeBufferOffset;
    PDEVICE_CONTEXT p_DeviceContext;
    TY_USB_MEM_INFO* p_DeviceMemory;
    size_t RelativeBufferHeaderOffset;
    size_t RelativeEventQueueOffset;
    size_t EventQueueSize;
    unsigned long StatusQueueMode;
    size_t StatusQueueSize;
    size_t RelativeStatusQueueOffset;
    size_t RelativeTransferDescriptorOffset;
    TY_SCATTERLIST_ENTRY p_Scatterlist[5];
    TY_SCATTERLIST_ENTRY* p_CurrentScatterListEntry;
    size_t GlobalMemoryBase;
    AiUInt32 ui_NumEntries = 0;
    size_t TotalSize = 0;
    WDFMEMORY Data;
    AiUInt8* puc_Data;
    AiUInt8* puc_MemoryMirrorData;
    size_t DataOffset = 0;

    TRACE("Synchronizing Transfer with direction %d\n", direction);

    p_DeviceContext = (PDEVICE_CONTEXT) p_api_dev->p_DeviceContext;
    p_DeviceMemory = &p_DeviceContext->DeviceMemory;
    GlobalMemoryBase = p_DeviceMemory->x_Gram.ul_Base;

    p_TransferDescriptor     = p_TransferContext->p_TransferDescriptor;


    if(    (p_TransferDescriptor->bhd_ptr <  p_api_dev->glb_mem.biu[ p_api_dev->biu ].base.bc_bh_area)
        || (p_TransferDescriptor->bhd_ptr > (p_api_dev->glb_mem.biu[ p_api_dev->biu ].base.bc_bh_area + p_api_dev->glb_mem.biu[ p_api_dev->biu ].size.bc_bh_area) ) )
    {
        /*@TODO: Remove if mirror is stable */
        ERROR("BH out of range 0x%08X\n", p_TransferDescriptor->bhd_ptr );
        return STATUS_INVALID_DEVICE_STATE;
    }


    p_BufferHeader = (struct ty_api_bc_bh_desc*) ((ptrdiff_t) p_DeviceContext->p_GlobalMemoryMirror + 
                     (ptrdiff_t) p_TransferDescriptor->bhd_ptr);


    uc_QueueSize = (p_BufferHeader->bh_ctrl >> 14) & 0xF;
    us_NumBuffers = 1 << uc_QueueSize;

    p_CurrentScatterListEntry = p_Scatterlist;

    /* Synchronize Event queue */ 
    RelativeEventQueueOffset = p_BufferHeader->eq_addr;
    EventQueueSize = MAX_API_BC_EQ_DESC_SIZE;

    p_CurrentScatterListEntry->Configuration.Settings.ByteCount = EventQueueSize;
    p_CurrentScatterListEntry->ui_PciAddress = GlobalMemoryBase + RelativeEventQueueOffset;
    p_CurrentScatterListEntry++;
    ui_NumEntries++;
    TotalSize += EventQueueSize;

    /* Synchronize Status queue */
    RelativeStatusQueueOffset = p_BufferHeader->sq_addr;
    StatusQueueMode = (p_BufferHeader->bh_ctrl >> 12) & 0x3L;

    if(StatusQueueMode == API_BC_BH_STS_QSIZE_SQ)
    {
        StatusQueueSize = us_NumBuffers * MAX_API_BC_SQ_DESC_SIZE;
    }
    else
    {
        StatusQueueSize = MAX_API_BC_SQ_DESC_SIZE;
    }

    p_CurrentScatterListEntry->Configuration.Settings.ByteCount = StatusQueueSize;
    p_CurrentScatterListEntry->ui_PciAddress = GlobalMemoryBase + RelativeStatusQueueOffset;
    p_CurrentScatterListEntry++;
    ui_NumEntries++;
    TotalSize += StatusQueueSize;

    /* Synchronize Buffer header */
    RelativeBufferHeaderOffset = p_TransferDescriptor->bhd_ptr;

    p_CurrentScatterListEntry->Configuration.Settings.ByteCount = MAX_API_BC_BH_DESC_SIZE;
    p_CurrentScatterListEntry->ui_PciAddress = GlobalMemoryBase + RelativeBufferHeaderOffset;
    p_CurrentScatterListEntry++;
    ui_NumEntries++;
    TotalSize += MAX_API_BC_BH_DESC_SIZE;

    /* Synchronize transfer descriptor */
    RelativeTransferDescriptorOffset = (ptrdiff_t) p_TransferDescriptor - 
                                       (ptrdiff_t) p_DeviceContext->p_GlobalMemoryMirror;

    p_CurrentScatterListEntry->Configuration.Settings.ByteCount = MAX_API_BC_XFER_DESC_SIZE;
    p_CurrentScatterListEntry->ui_PciAddress = GlobalMemoryBase + RelativeTransferDescriptorOffset;
    p_CurrentScatterListEntry++;
    ui_NumEntries++;
    TotalSize += MAX_API_BC_XFER_DESC_SIZE;

    if(b_IncludeBufferData)
    {
        RelativeBufferOffset = p_BufferHeader->bp_addr;
    
        p_CurrentScatterListEntry->Configuration.Settings.ByteCount = us_NumBuffers * MAX_API_BUF_SIZE;
        p_CurrentScatterListEntry->ui_PciAddress = GlobalMemoryBase + RelativeBufferOffset;
        ui_NumEntries++;
        TotalSize += us_NumBuffers * MAX_API_BUF_SIZE;
    }

    Status = WdfMemoryCreate(WDF_NO_OBJECT_ATTRIBUTES, NonPagedPool, '1553', TotalSize, &Data, &puc_Data);
    if(!NT_SUCCESS(Status))
    {
        ERROR("Allocation of data buffer for Scatter/Gather transfer failed\n");
        return Status;
    }

    if(direction == In)
    {
        Status = ReadUsbPciDataByScatterList(p_DeviceContext, p_Scatterlist, ui_NumEntries, puc_Data, TotalSize);
        if(!NT_SUCCESS(Status))
        {
            ERROR("Scatter/Gather Transfer failed\n");
            WdfObjectDelete(Data);
            return Status;
        }

        CopyScatterDataToMirror(p_DeviceContext, p_Scatterlist, ui_NumEntries, puc_Data);
    }
    else if(direction == Out)
    {
        CopyScatterDataToBuffer(p_DeviceContext, p_Scatterlist, ui_NumEntries, puc_Data);

        Status = WriteUsbPciDataByScatterList(p_DeviceContext, p_Scatterlist, ui_NumEntries, puc_Data, 
                                              TotalSize);
        if(!NT_SUCCESS(Status))
        {
            ERROR("Scatter/Gather Transfer failed\n");
            WdfObjectDelete(Data);
            return Status;
        }
    }

    WdfObjectDelete(Data);


    if(    (p_TransferDescriptor->bhd_ptr <  p_api_dev->glb_mem.biu[ p_api_dev->biu ].base.bc_bh_area)
        || (p_TransferDescriptor->bhd_ptr > (p_api_dev->glb_mem.biu[ p_api_dev->biu ].base.bc_bh_area + p_api_dev->glb_mem.biu[ p_api_dev->biu ].size.bc_bh_area) ) )
    {
        /*@TODO: Remove if mirror is stable */
        ERROR("BH out of range 0x%08X\n", p_TransferDescriptor->bhd_ptr );
        return STATUS_INVALID_DEVICE_STATE;
    }

    return Status;
}


/* Synchronizes one RT buffer between global memory and its mirror */
AiInt32 UsbSynchronizeRTBuffer(TY_API_DEV* p_api_dev, PRT_BUFFER_CONTEXT p_BufferContext, 
                                enum SYNCH_DIRECTION direction)
{
    AiInt32 Status = STATUS_UNSUCCESSFUL;
    struct ty_api_rt_bh_desc* p_BufferHeader;
    unsigned char uc_QueueSize;
    unsigned short us_NumBuffers;
    size_t RelativeBufferOffset;
    PDEVICE_CONTEXT p_DeviceContext;
    size_t RelativeBufferHeaderOffset;
    size_t RelativeEventQueueOffset;
    size_t EventQueueSize;
    size_t RelativeStatusQueueOffset;
    size_t StatusQueueSize;
    size_t GlobalMemoryBase;
    size_t TotalSize = 0;
    unsigned long StatusQueueMode;
    TY_SCATTERLIST_ENTRY p_ScatterList[4];
    TY_SCATTERLIST_ENTRY* p_CurrentScatterListEntry;
    AiUInt8* puc_Data;
    size_t ScatterListEntriesCount;
    
    TRACE("Synchronizing RT BUffer with direction %d\n", direction);

    p_BufferHeader = p_BufferContext->p_BufferHeader;

    uc_QueueSize = (p_BufferHeader->bh_ctrl >> 14) & 0xF;
    us_NumBuffers = 1 << uc_QueueSize;

    p_DeviceContext = (PDEVICE_CONTEXT) p_api_dev->p_DeviceContext;
    GlobalMemoryBase = p_DeviceContext->DeviceMemory.x_Gram.ul_Base;
    p_CurrentScatterListEntry = &p_ScatterList[0];
    ScatterListEntriesCount = sizeof(p_ScatterList) / sizeof(TY_SCATTERLIST_ENTRY);

    /* Synchronize the buffer data */
    RelativeBufferOffset = p_BufferHeader->bp_addr;

    p_CurrentScatterListEntry->Configuration.Settings.ByteCount = us_NumBuffers * MAX_API_BUF_SIZE;
    p_CurrentScatterListEntry->ui_PciAddress = RelativeBufferOffset + GlobalMemoryBase;
    TotalSize += p_CurrentScatterListEntry->Configuration.Settings.ByteCount;
    p_CurrentScatterListEntry++;

    /* Synchronize the Event queue */
    RelativeEventQueueOffset = p_BufferHeader->eq_addr;
    EventQueueSize = MAX_API_RT_EQ_DESC_SIZE;

    p_CurrentScatterListEntry->Configuration.Settings.ByteCount = EventQueueSize;
    p_CurrentScatterListEntry->ui_PciAddress = RelativeEventQueueOffset + GlobalMemoryBase;
    TotalSize += p_CurrentScatterListEntry->Configuration.Settings.ByteCount;
    p_CurrentScatterListEntry++;

    /* Synchronize the Status queue*/
    RelativeStatusQueueOffset = p_BufferHeader->sq_addr;
    StatusQueueMode = (p_BufferHeader->bh_ctrl >> 12) & 0x3L;

    if(StatusQueueMode == API_BC_BH_STS_QSIZE_SQ)
    {
        StatusQueueSize = us_NumBuffers * MAX_API_BC_SQ_DESC_SIZE;
    }
    else
    {
        StatusQueueSize = MAX_API_BC_SQ_DESC_SIZE;
    }

    p_CurrentScatterListEntry->Configuration.Settings.ByteCount = StatusQueueSize;
    p_CurrentScatterListEntry->ui_PciAddress = RelativeStatusQueueOffset + GlobalMemoryBase;
    TotalSize += p_CurrentScatterListEntry->Configuration.Settings.ByteCount;
    p_CurrentScatterListEntry++;

    /* Synchronize the buffer header */
    RelativeBufferHeaderOffset = (ptrdiff_t) p_BufferHeader - 
                                 (ptrdiff_t) p_DeviceContext->p_GlobalMemoryMirror;

    p_CurrentScatterListEntry->Configuration.Settings.ByteCount = MAX_API_BC_BH_DESC_SIZE;
    p_CurrentScatterListEntry->ui_PciAddress = RelativeBufferHeaderOffset + GlobalMemoryBase;
    TotalSize += p_CurrentScatterListEntry->Configuration.Settings.ByteCount;
    
    puc_Data = AllocateDriverMemory(TotalSize, MEM_NON_PAGEABLE);
    if(puc_Data == NULL)
    {
        ERROR("Allocation of driver memory for scatter/gather transfer failed\n");
        return STATUS_UNSUCCESSFUL;
    }

    if(direction == In)
    {
        Status = ReadUsbPciDataByScatterList(p_DeviceContext, p_ScatterList, ScatterListEntriesCount, 
                                             puc_Data, TotalSize);

        CopyScatterDataToMirror(p_DeviceContext, p_ScatterList, ScatterListEntriesCount, puc_Data);
    }
    else if(direction == Out)
    {
        CopyScatterDataToBuffer(p_DeviceContext, p_ScatterList, ScatterListEntriesCount, puc_Data);

        Status = WriteUsbPciDataByScatterList(p_DeviceContext, p_ScatterList, ScatterListEntriesCount, 
                                              puc_Data, TotalSize);
    }

    if(!NT_SUCCESS(Status))
    {
        ERROR("Scatter/Gather Transfer failed\n");
    }

    FreeDriverMemory(puc_Data);

    return Status;
}


/* Adds one transfer to the BIU specific transfer list */
void UsbRTBufferListAdd(TY_API_DEV* p_api_dev, short us_Biu, unsigned short us_HeaderID,
                        size_t RelativeBufferHeaderOffset)
{
    PDEVICE_CONTEXT p_DeviceContext;
    WDF_OBJECT_ATTRIBUTES RTBufferObjectAttributes;
    WDFOBJECT RTBufferObject;
    AiInt32 Status = STATUS_UNSUCCESSFUL;
    PRT_BUFFER_CONTEXT p_RtBufferContext;
    uintptr_t AbsoluteBufferHeaderAddress;
    unsigned long ul_BufferCount;
    BOOLEAN b_BufferFound = FALSE;
    unsigned int i;

    p_DeviceContext = (PDEVICE_CONTEXT) p_api_dev->p_DeviceContext;

    if(p_DeviceContext->RTBufferList[us_Biu] == NULL)
    {
        ERROR("No RT Buffer list for this BIU available\n");
        return;
    }

    ul_BufferCount = WdfCollectionGetCount(p_DeviceContext->RTBufferList[us_Biu]);
    i = 0;

    /* check if rt buffer is already contained in list */
    while(i < ul_BufferCount && !b_BufferFound)
    {
        RTBufferObject = WdfCollectionGetItem(p_DeviceContext->RTBufferList[us_Biu], i);
        p_RtBufferContext = GetRtBufferContext(RTBufferObject);

        if(p_RtBufferContext->us_HeaderID == us_HeaderID)
            b_BufferFound = TRUE;
        else
            i++;
    }

    if(b_BufferFound)
    {
        AbsoluteBufferHeaderAddress = (ptrdiff_t) p_DeviceContext->p_GlobalMemoryMirror + 
                                       RelativeBufferHeaderOffset;
        p_RtBufferContext->p_BufferHeader = (struct ty_api_rt_bh_desc*) AbsoluteBufferHeaderAddress;
    }
    else
    {
        /* RT buffer is not in list yet, so create a new RT buffer object */
        WDF_OBJECT_ATTRIBUTES_INIT(&RTBufferObjectAttributes);
        WDF_OBJECT_ATTRIBUTES_SET_CONTEXT_TYPE(&RTBufferObjectAttributes, RT_BUFFER_CONTEXT);
        RTBufferObjectAttributes.ParentObject = p_DeviceContext->RTBufferList[us_Biu];

        Status = WdfObjectCreate(&RTBufferObjectAttributes, &RTBufferObject);
        if(!NT_SUCCESS(Status))
        {
            ERROR("Creation of RT Buffer Object failed\n");
            return;
        }

        p_RtBufferContext = GetRtBufferContext(RTBufferObject);
    
        p_RtBufferContext->us_HeaderID = us_HeaderID;
        AbsoluteBufferHeaderAddress = (ptrdiff_t) p_DeviceContext->p_GlobalMemoryMirror + 
                                  RelativeBufferHeaderOffset;
        p_RtBufferContext->p_BufferHeader = (struct ty_api_rt_bh_desc*) AbsoluteBufferHeaderAddress;

        Status = WdfCollectionAdd(p_DeviceContext->RTBufferList[us_Biu], RTBufferObject);
        if(!NT_SUCCESS(Status))
        {
            ERROR("Adding of RT Buffer Object to Collection failed\n");
        }
    }
}


/* Removes all transfers from the BIU specific RT buffer list */
void UsbRTBufferListClear(TY_API_DEV* p_api_dev, short us_Biu)
{
    PDEVICE_CONTEXT p_DeviceContext;
    WDFCOLLECTION RTBufferList;
    WDFOBJECT CurrentRTBufferObject;

    TRACE("Clearing RT Buffer List of BIU: %d\n", us_Biu);

    p_DeviceContext = (PDEVICE_CONTEXT) p_api_dev->p_DeviceContext;

    if(p_DeviceContext->RTBufferList[us_Biu] == NULL)
    {
        ERROR("No RT Buffer List for this BIU available\n");
        return;
    }

    RTBufferList = p_DeviceContext->RTBufferList[us_Biu];

    CurrentRTBufferObject = WdfCollectionGetFirstItem(RTBufferList);

    while(CurrentRTBufferObject != NULL)
    {
        WdfCollectionRemove(RTBufferList, CurrentRTBufferObject);

        WdfObjectDelete(CurrentRTBufferObject);

        CurrentRTBufferObject = WdfCollectionGetFirstItem(RTBufferList);
    }
    
}


/* Adds one transfer to the BIU specific transfer list */
void UsbTransferListAdd(TY_API_DEV* p_api_dev, short us_Biu, unsigned short us_TransferID, 
                        struct ty_api_bc_xfer_desc* p_TransferDescriptor, BOOLEAN b_SynchOut)
{
    PDEVICE_CONTEXT p_DeviceContext;
    WDF_OBJECT_ATTRIBUTES TransferObjectAttributes;
    WDFOBJECT TransferObject;
    AiInt32 Status = STATUS_UNSUCCESSFUL;
    PTRANSFER_CONTEXT p_TransferContext;
    WDFCOLLECTION TransferList;
    unsigned int ui_NumTransfers;
    unsigned int i;
    BOOLEAN b_Found = FALSE;
    PTRANSFER_LIST_CONTEXT p_TransferListContext;

    p_DeviceContext = (PDEVICE_CONTEXT) p_api_dev->p_DeviceContext;
    TransferList = p_DeviceContext->TransferList[us_Biu];

    if(TransferList == NULL)
    {
        ERROR("No transfer list for this BIU available\n");
        return;
    }

    p_TransferListContext = GetTransferListContext(TransferList);
    WdfWaitLockAcquire(p_TransferListContext->Lock, NULL);

    ui_NumTransfers = WdfCollectionGetCount(TransferList);
    i = 0;

    /* Check if transfer is already contained in the list */
    while(i < ui_NumTransfers && !b_Found)
    {
        TransferObject = WdfCollectionGetItem(TransferList, i);
        p_TransferContext = GetTransferContext(TransferObject);

        if(p_TransferContext->us_TransferID == us_TransferID)
        {
            b_Found = TRUE;
        }

        i++;
    }

    if(!b_Found)
    {
        /* Transfer was not in the list, so create a new transfer object */
        WDF_OBJECT_ATTRIBUTES_INIT(&TransferObjectAttributes);
        WDF_OBJECT_ATTRIBUTES_SET_CONTEXT_TYPE(&TransferObjectAttributes, TRANSFER_CONTEXT);
        TransferObjectAttributes.ParentObject = p_DeviceContext->TransferList[us_Biu];

        Status = WdfObjectCreate(&TransferObjectAttributes, &TransferObject);
        if(!NT_SUCCESS(Status))
        {
            ERROR("Creation of Transfer Object failed\n");
            return;
        }

        p_TransferContext = GetTransferContext(TransferObject);
        p_TransferContext->us_TransferID = us_TransferID;
        p_TransferContext->p_TransferDescriptor = p_TransferDescriptor;

        Status = WdfCollectionAdd(p_DeviceContext->TransferList[us_Biu], TransferObject);
        if(!NT_SUCCESS(Status))
        {
            ERROR("Adding of Transfer Object to Collection failed\n");
        }
    }
    else
    {
        p_TransferContext->p_TransferDescriptor = p_TransferDescriptor;
    }

    WdfWaitLockRelease(p_TransferListContext->Lock);

    if(b_SynchOut)
    {
        UsbSynchronizeTransfer(p_api_dev, p_TransferContext, FALSE, Out);
    }
}


/* Removes all transfers from the BIU specific transfer list */
void UsbTransferListClear(TY_API_DEV* p_api_dev, short us_Biu)
{
    PDEVICE_CONTEXT p_DeviceContext;
    WDFCOLLECTION TransferList;
    WDFOBJECT CurrentTransferObject;
    PTRANSFER_LIST_CONTEXT p_TransferListContext;

    TRACE("Clearing Transfer List of BIU: %d\n", us_Biu);

    p_DeviceContext = (PDEVICE_CONTEXT) p_api_dev->p_DeviceContext;

    if(p_DeviceContext->TransferList[us_Biu] == NULL)
    {
        ERROR("No transfer list for this BIU available\n");
        return;
    }

    TransferList = p_DeviceContext->TransferList[us_Biu];

    p_TransferListContext = GetTransferListContext(TransferList);
    WdfWaitLockAcquire(p_TransferListContext->Lock, NULL);

    CurrentTransferObject = WdfCollectionGetFirstItem(TransferList);

    while(CurrentTransferObject != NULL)
    {
        WdfCollectionRemove(TransferList, CurrentTransferObject);

        WdfObjectDelete(CurrentTransferObject);

        CurrentTransferObject = WdfCollectionGetFirstItem(TransferList);
    }

    WdfWaitLockRelease(p_TransferListContext->Lock);
    
}


/* Synchronizes one specific transfer determined by its transfer ID 
   between global memory and its mirror. The transfer list of the given BIU will be searched for
   the transfer. */
AiInt32 UsbSynchronizeTransferByID(TY_API_DEV* p_api_dev, short uc_Biu, unsigned short us_TransferID, 
                                    BOOLEAN b_IncludeBufferData, enum SYNCH_DIRECTION direction)
{
    AiInt32 Status = STATUS_UNSUCCESSFUL;
    PDEVICE_CONTEXT p_DeviceContext;
    WDFCOLLECTION TransferList;
    unsigned int ui_TransferCount;
    unsigned int i;
    WDFOBJECT TransferObject;
    BOOLEAN b_Found = FALSE;
    PTRANSFER_CONTEXT p_TransferContext;
    PTRANSFER_LIST_CONTEXT p_TransferListContext;

    TRACE("UsbSynchronizeTransferByID on BIU %d with ID %d and direction %d\n", uc_Biu, us_TransferID,
          direction);

    p_DeviceContext = (PDEVICE_CONTEXT) p_api_dev->p_DeviceContext;
    TransferList = p_DeviceContext->TransferList[uc_Biu];

    if(TransferList == NULL)
    {
        ERROR("No transfer list available for this BIU\n");
        return STATUS_UNSUCCESSFUL;
    }

    p_TransferListContext = GetTransferListContext(TransferList);
    WdfWaitLockAcquire(p_TransferListContext->Lock, NULL);

    ui_TransferCount = WdfCollectionGetCount(TransferList);

    i = 0;

    while(i < ui_TransferCount && !b_Found)
    {
        TransferObject = WdfCollectionGetItem(TransferList, i);
        p_TransferContext = GetTransferContext(TransferObject);

        if(p_TransferContext->us_TransferID == us_TransferID)
        {
            b_Found = TRUE;
            
            Status = UsbSynchronizeTransfer(p_api_dev, p_TransferContext, b_IncludeBufferData, direction); 
        }

        i++;
    }

    WdfWaitLockRelease(p_TransferListContext->Lock);

    return Status;
}

