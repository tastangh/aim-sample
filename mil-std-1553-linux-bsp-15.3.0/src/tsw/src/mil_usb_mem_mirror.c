/* SPDX-FileCopyrightText: 2001-2022 AIM GmbH <info@aim-online.com> */
/* SPDX-License-Identifier: MIT OR GPL-2.0-or-later */

/*! \file mil_usb_mem_mirror.c

   File with function implementations for USB Global Memory Mirror
*/

#include "mil_usb_mem_mirror.h"
#include "mil_usb_io.h"
#include "Ai_container.h"


#define USB_DMA_THRESHOLD 16


/* Creates a Transfer List for each possible BIU of the given device */
AiInt32 CreateTransferLists(TY_API_DEV* p_api_dev)
{
    int i;

    /* Create a transfer list for each BIU */
    for (i = 0; i < MAX_BIU; i++)
    {
        /*Initialize and create the lock of the list */
        AI_LIST_INIT(p_api_dev->TransferList[i]);

        p_api_dev->TransferListLock[i] = ai_tsw_os_lock_create();
    }

    return API_OK;
}


/* Creates an RT Buffer List for each BIU of the given Device */
AiInt32 CreateRTBufferLists(TY_API_DEV* p_api_dev)
{
    int i;

    /* Create a buffer list for each possible BIU */
    for (i = 0; i < MAX_BIU; i++)
    {
        /*Initialize and create the lock of the list */
        AI_LIST_INIT(p_api_dev->RTBufferList[i]);
        p_api_dev->RTBufferListLock[i] = ai_tsw_os_lock_create();
    }


    return API_OK;
}



/* This function checks if two given memory intervals intersect. It delivers the intersection of these
   memory intervals and also the up to 2 remaining memory intervals that form the symmetric 
   difference quantity
   Parameters: size_t - start of first memory interval
               size_t - end of first memory interval
               size_t - start of second memory interval
               size_t - end of second memory interval
               size_t* - start of resulting intersection
               size_t* - size of resulting intersection
               unsigned char* - number of remaining memory intervals that are not contained in 
                                the intersection (at most 2)
               size_t[2] - start offsets of remaining intervals
               size_t[2] - sizes of remaining intervals
   Return: returns TRUE, if the intervals have a non-empty intersection */
AiBoolean IntersectMemoryInterval(size_t FirstIntervalStart, size_t FirstIntervalEnd, 
                                size_t SecondIntervalStart, size_t SecondIntervalEnd, 
                                size_t* IntersectionStart, size_t* IntersectionSize, 
                                unsigned char* uc_NumRemainingIntervals,
                                size_t RemainingStartIntervals[2], size_t RemainingIntervalSizes[2]);


/* Synchronizes one longword between global memory and its mirror */
AiInt32 UsbSynchronizeLWord(TY_API_DEV* p_api_dev, size_t offset, enum SYNCH_DIRECTION direction)
{
    return UsbSynchronizeMemoryArea(p_api_dev, offset, sizeof(AiUInt32), direction);
}


/* Synchronizes the System Control Block of a sepcific BIU between global memory and its mirror */
AiInt32 UsbSynchronizeSystemControlBlock(TY_API_DEV* p_api_dev, short uc_Biu, 
                                          enum SYNCH_DIRECTION direction)
{
    AiInt32 Status;
    size_t RelativeSystemControlBlockOffset;
    size_t SystemControlBlockSize;

    RelativeSystemControlBlockOffset = p_api_dev->glb_mem.biu[uc_Biu].cb_addr;
    SystemControlBlockSize = SYSTEM_CONTROL_BLOCK_SIZE;
              
    Status = UsbSynchronizeMemoryArea(p_api_dev, RelativeSystemControlBlockOffset, 
                                      SystemControlBlockSize, direction);

    return Status;
}


/* Synchronizes one longword from the board's global memory and returns the synchronized value. */
unsigned int UsbSynchAndReadLWord(TY_API_DEV* p_api_dev, void* p_AbsoluteMirrorAddress)
{
    AiInt32 Status;
    size_t RelativeOffset;

    RelativeOffset = API_GLB_MEM_ADDR_REL(p_AbsoluteMirrorAddress);

    Status = UsbSynchronizeLWord(p_api_dev, RelativeOffset, In);

    if(Status)
    {
        return API_ERR;
    }

    return *((unsigned int*) p_AbsoluteMirrorAddress);
}



/* Synchronizes the BC relative data between global memory and its mirror */
AiInt32 UsbSynchronizeBC(TY_API_DEV* p_api_dev, short uc_Biu, enum SYNCH_DIRECTION direction)
{
    AiInt32 Status;

    Status = UsbSynchronizeTransfers(p_api_dev, uc_Biu, direction);
    if(Status)
    {
        return Status;
    }

    Status = UsbSynchronizeInstructionLists(p_api_dev, uc_Biu, direction);
    if(Status)
    {
        return Status;
    }

    Status = UsbSynchronizeBCMiscBlock(p_api_dev, uc_Biu, direction);
    if(Status)
    {
        return Status;
    }

    Status = UsbSynchronizeBCSystemControlBlock(p_api_dev, uc_Biu, direction);
    if(Status)
    {
        return Status;
    }

    return Status;
}


/* Synchronizes the RT area of the System Control Block of a specific BIU 
   between global memory and its mirror */
AiInt32 UsbSynchronizeRTSystemControlBlock(TY_API_DEV* p_api_dev, short uc_Biu, 
                                            enum SYNCH_DIRECTION direction)
{
    AiInt32 Status;
    size_t RelativeOffset;
    size_t Size;

    RelativeOffset = p_api_dev->glb_mem.biu[uc_Biu].cb_addr + 0x4C;
    Size = 8;

    Status = UsbSynchronizeMemoryArea(p_api_dev, RelativeOffset, Size, direction);

    return Status;
}


/* Synchronizes the RT relative data between global memory and its mirror */
AiInt32 UsbSynchronizeRT(TY_API_DEV* p_api_dev, short uc_Biu, enum SYNCH_DIRECTION direction)
{
    AiInt32 Status;

    Status = UsbSynchronizeRTBuffers(p_api_dev, uc_Biu, direction);
    if(Status)
    {
        return Status;
    }

    Status = UsbSynchronizeRTSubAddressDescriptorArea(p_api_dev, uc_Biu, direction);
    if(Status)
    {
        return Status;
    }

    Status = UsbSynchronizeRTDescriptorArea(p_api_dev, uc_Biu, direction);
    if(Status)
    {
        return Status;
    }

    Status = UsbSynchronizeRTSystemControlBlock(p_api_dev, uc_Biu, direction);
    if(Status)
    {
        return Status;
    }

    return Status;
}


/* Synchronizes instruction lists of a specific BIU between global memory and its mirror */
AiInt32 UsbSynchronizeInstructionLists(TY_API_DEV* p_api_dev, short uc_Biu, 
                                        enum SYNCH_DIRECTION direction)
{
    AiInt32 Status;
    size_t RelativeInstructionListOffset;
    size_t InstructionListSize;

    /* Synchronize high priority instruction list */
    RelativeInstructionListOffset = p_api_dev->glb_mem.biu[uc_Biu].base.bc_hip_instr;
    InstructionListSize = p_api_dev->glb_mem.biu[uc_Biu].size.bc_hip_instr;
   
    Status = UsbSynchronizeMemoryArea(p_api_dev, RelativeInstructionListOffset, InstructionListSize, 
                                      direction);
    if(Status)
    {
        return Status;
    }

    /* Synchronize low priority instruction list */
    RelativeInstructionListOffset = p_api_dev->glb_mem.biu[uc_Biu].base.bc_lip_instr;
    InstructionListSize = p_api_dev->glb_mem.biu[uc_Biu].size.bc_lip_instr;
 
    Status = UsbSynchronizeMemoryArea(p_api_dev, RelativeInstructionListOffset, InstructionListSize, 
                                      direction);
    if(Status)
    {
        return Status;
    }

    /* Synchronize acyclic priority instruction list */
    Status = UsbSynchronizeAcyclicInstructionList(p_api_dev, uc_Biu, direction);
    if(Status)
    {
        return Status;
    }

    return Status;

}


/* Synchronizes BC data area of System Control Block between global memory and its mirror */
AiInt32 UsbSynchronizeBCSystemControlBlock(TY_API_DEV* p_api_dev, short uc_Biu, 
                                            enum SYNCH_DIRECTION direction)
{
    AiInt32 Status;
    size_t RelativeGlobalMemoryOffset;
    size_t Size;

    /* Synchronize BC registers */
    RelativeGlobalMemoryOffset = p_api_dev->glb_mem.biu[uc_Biu].cb_addr + 0x30;
    Size = 24;

    Status = UsbSynchronizeMemoryArea(p_api_dev, RelativeGlobalMemoryOffset, Size, direction);

    if(Status)
    {
        return Status;
    }

    /* Synchronize global BC transfer/error counter */
    RelativeGlobalMemoryOffset = p_api_dev->glb_mem.biu[uc_Biu].cb_addr + 0x54;
    Size = 8;
    
    Status = UsbSynchronizeMemoryArea(p_api_dev, RelativeGlobalMemoryOffset, Size, direction);

    if(Status)
    {
        return Status;
    }

    return Status;
}


/* Synchronizes a specific memory range between global memory and its mirror */
AiInt32 UsbSynchronizeMemoryArea(TY_API_DEV* p_api_dev, size_t offset, size_t size, enum SYNCH_DIRECTION direction)
{
    AiInt32 status = API_ERR;
    void* memory_mirror;

    memory_mirror = (void*)((ptrdiff_t)p_api_dev->GlobalRAMBase + offset);

    if (direction == In)
    {
        status = usb_global_mem_read(p_api_dev->p_DeviceContext, offset, memory_mirror, size);
    }
    else
    {
        status = usb_global_mem_write(p_api_dev->p_DeviceContext, offset, memory_mirror, size);
    }

    return status;
}


/* Synchronizes acyclic instruction list of a specific BIU between global memory and its mirror */
AiInt32 UsbSynchronizeAcyclicInstructionList(TY_API_DEV* p_api_dev, short uc_Biu, 
                                              enum SYNCH_DIRECTION direction)
{
    AiInt32 Status;
    size_t RelativeListOffset;
    size_t ListSize;

    RelativeListOffset = p_api_dev->glb_mem.biu[uc_Biu].base.bc_acyc_instr;
    ListSize = p_api_dev->glb_mem.biu[uc_Biu].size.bc_acyc_instr;
    
    Status = UsbSynchronizeMemoryArea(p_api_dev, RelativeListOffset, ListSize, direction);

    return Status;
}




/* Synchronizes the RT descriptor area of a specific BIU between global memory and its mirror */
AiInt32 UsbSynchronizeRTDescriptorArea(TY_API_DEV* p_api_dev, short uc_Biu, 
                                        enum SYNCH_DIRECTION direction)
{
    AiInt32 Status;
    size_t RelativeRTDescriptorOffset;
    size_t RTDescriptorSize;

    RelativeRTDescriptorOffset = p_api_dev->glb_mem.biu[uc_Biu].rt_desc_addr;
    RTDescriptorSize = MAX_API_RT_DESC_SIZE * MAX_API_RT;

    Status = UsbSynchronizeMemoryArea(p_api_dev, RelativeRTDescriptorOffset, RTDescriptorSize, 
                                      direction);

    return Status;
}


/* Synchronizes the RT descriptor of a specific RT between global memory and its mirror
   Parameters: TY_API_DEV - pointer to target vars structure
               unsigned char - ID of BIU the RT belongs to
               unsigned char - ID of RT to synchronize
               enum SYNCH_DIRECTION - direction to synchronize
   Return: return 0, if synch was successful */
AiInt32 UsbSynchronizeRTDescriptor(TY_API_DEV* p_api_dev, short uc_Biu, unsigned char uc_RT, 
                                   enum SYNCH_DIRECTION direction)
{
    AiInt32 Status;
    size_t RelativeRTDescriptorOffset;
    size_t RTDescriptorSize;

    RelativeRTDescriptorOffset  = API_GLB_MEM_ADDR_REL(&(*p_api_dev->rt_desc_p[uc_Biu])[uc_RT]);
    RTDescriptorSize = sizeof(struct ty_api_rt_desc);

    Status = UsbSynchronizeMemoryArea(p_api_dev, RelativeRTDescriptorOffset, RTDescriptorSize, direction);

    return Status;
}


/* Synchronizes the RT subaddress descriptor area of a specific BIU 
   between global memory and its mirror*/
AiInt32 UsbSynchronizeRTSubAddressDescriptorArea(TY_API_DEV* p_api_dev, short uc_Biu, 
                                                  enum SYNCH_DIRECTION direction)
{
    AiInt32 Status;
    size_t RelativeRTSubAddressDescriptorOffset;
    size_t RTSubAddressDescriptorSize;

    RelativeRTSubAddressDescriptorOffset = p_api_dev->glb_mem.biu[uc_Biu].base.rt_sa_desc;
    RTSubAddressDescriptorSize = p_api_dev->glb_mem.biu[uc_Biu].size.rt_sa_desc;

    Status = UsbSynchronizeMemoryArea(p_api_dev, RelativeRTSubAddressDescriptorOffset, 
                                      RTSubAddressDescriptorSize, direction);

    return Status;
}


/* Synchronizes the bus monitor specific data between global memory and its mirror */
AiInt32 UsbSynchronizeBM(TY_API_DEV* p_api_dev, short uc_Biu, enum SYNCH_DIRECTION direction)
{
    AiInt32 Status;

    Status = UsbSynchronizeMonitorDataBuffer(p_api_dev, uc_Biu, direction);
    if(Status)
    {
        return Status;
    }

    Status = UsbSynchronizeMonitorActivityFilteringPage(p_api_dev, uc_Biu, direction);
    if(Status)
    {
        return Status;
    }

    Status = UsbSynchronizeTriggerControlBlockArea(p_api_dev, uc_Biu, direction);
    if(Status)
    {
        return Status;
    }

    Status = UsbSynchronizeBMSystemControlBlock(p_api_dev, uc_Biu, direction);
    if(Status)
    {
        return Status;
    }

    return Status;
}


/* Synchronizes the monitor data buffer of a specific BIU between global memory and its mirror */
AiInt32 UsbSynchronizeMonitorDataBuffer(TY_API_DEV* p_api_dev, short uc_Biu, 
                                         enum SYNCH_DIRECTION direction)
{
    AiInt32 Status;
    size_t RelativeBufferOffset;
    size_t BufferSize;

    RelativeBufferOffset = p_api_dev->glb_mem.biu[uc_Biu].base.bm_buf;
    BufferSize = p_api_dev->glb_mem.biu[uc_Biu].size.bm_buf;

    Status = UsbSynchronizeMemoryArea(p_api_dev, RelativeBufferOffset, BufferSize, direction);

    return Status;
}


/* Synchronizes the monitor activity/filtering page of a specific BIU */
AiInt32 UsbSynchronizeMonitorActivityFilteringPage(TY_API_DEV* p_api_dev, short uc_Biu, 
                                                   enum SYNCH_DIRECTION direction)
{
    AiInt32 Status;
    size_t RelativeOffset;
    size_t ActivityFilteringPageSize;

    RelativeOffset = p_api_dev->glb_mem.biu[uc_Biu].bm_act_addr;

    if( p_api_dev->ulUseEnhancedBmActivityPage )
        ActivityFilteringPageSize = API_BM_ACT_ENH_PAGE_SIZE;
    else
        ActivityFilteringPageSize = API_BM_ACT_PAGE_SIZE;


    Status = UsbSynchronizeMemoryArea(p_api_dev, RelativeOffset, ActivityFilteringPageSize, direction);

    return Status;
}


/* Synchronizes the monitor activity/filtering page section of a specific RT 
   between global memory and its mirror
   Parameters: TY_API_DEV - pointer to target vars structure
               unsigned char - ID of BIU
               unsigned char - ID of RT
               enum SYNCH_DIRECTION - direction to synchronize
   Return: return 0, if synch was successful */
AiInt32 UsbSynchronizeMonitorActivityPageOfRT(TY_API_DEV* p_api_dev, short uc_Biu, unsigned char uc_Rt, 
                                              enum SYNCH_DIRECTION direction)
{
    AiInt32 Status;
    AiUInt32 * pActStart = NULL;
    size_t RelativeStartOffset;
    size_t Size;

    /* Get relative offset of RT's section in the monitor activity page */
    pActStart = (AiUInt32 *)GET_BM_ACTIVITY_DESC_BY_BIU(p_api_dev, uc_Biu, uc_Rt, 0, 0);
    RelativeStartOffset =  API_GLB_MEM_ADDR_REL(pActStart);


    if(p_api_dev->ulUseEnhancedBmActivityPage)
        Size = API_BM_ACT_ENH_PAGE_SIZE / 32;
    else
        Size = API_BM_ACT_PAGE_SIZE / 32;


    Status = UsbSynchronizeMemoryArea(p_api_dev, RelativeStartOffset, Size, direction);

    return Status;
}


/* Synchronizes the monitor activity/filtering page section of a specific sub-address 
   between global memory and its mirror
   Parameters: TY_API_DEV - pointer to target vars structure
               unsigned char - ID of BIU
               unsigned char - ID of RT
               unsigned char - ID of sub-address
               unsigned char - Type of sub-address
               enum SYNCH_DIRECTION - direction to synchronize
   Return: return 0, if synch was successful */
AiInt32 UsbSynchronizeMonitorActivityPageOfSA(TY_API_DEV* p_api_dev, short uc_Biu, unsigned char uc_Rt,
                                              unsigned char uc_Subaddress, unsigned char uc_SubaddressType,
                                              enum SYNCH_DIRECTION direction)
{
    AiInt32 Status;
    AiUInt32 * pActStart = NULL;
    size_t RelativeStartOffset;
    size_t Size;

    /* Get offsets of different SA type sections and prepare scatterlist */

    /* Determine offset of section to synchronize */
    if( uc_SubaddressType == API_RT_SA_TYPE_ALL )
    {
        Status = UsbSynchronizeMonitorActivityPageOfSA( p_api_dev, uc_Biu, uc_Rt, uc_Subaddress, API_RT_SA_TYPE_RCV, direction );
        if( Status != 0 ) return Status;

        Status = UsbSynchronizeMonitorActivityPageOfSA( p_api_dev, uc_Biu, uc_Rt, uc_Subaddress, API_RT_SA_TYPE_XMT, direction );
        if( Status != 0 ) return Status;

        Status = UsbSynchronizeMonitorActivityPageOfSA( p_api_dev, uc_Biu, uc_Rt, uc_Subaddress, API_RT_MC_TYPE_RCV, direction );
        if( Status != 0 ) return Status;

        Status = UsbSynchronizeMonitorActivityPageOfSA( p_api_dev, uc_Biu, uc_Rt, uc_Subaddress, API_RT_MC_TYPE_XMT, direction );
        if( Status != 0 ) return Status;

    }
    else
    {
        /* Get relative offset of SA section in the monitor activity page */
        pActStart = (AiUInt32*)GET_BM_ACTIVITY_DESC_BY_BIU(p_api_dev, uc_Biu, uc_Rt, uc_Subaddress, uc_SubaddressType);
        RelativeStartOffset =  API_GLB_MEM_ADDR_REL(pActStart);

        if( p_api_dev->ulUseEnhancedBmActivityPage )
            Size = 0x14;
        else
            Size = 0xC;

        /* Just one SA type section to read, no scatter/gather DMA necessary */
        Status = UsbSynchronizeMemoryArea(p_api_dev, RelativeStartOffset, Size, direction);
    }

    return Status;
}



/* Synchronizes the monitor trigger control block of a specific BIU 
   between global memory and its mirror */
AiInt32 UsbSynchronizeTriggerControlBlockArea(TY_API_DEV* p_api_dev, short uc_Biu, 
                                              enum SYNCH_DIRECTION direction)
{
    AiInt32 Status;
    size_t RelativeOffset;
    size_t TriggerControlBlockSize;

    RelativeOffset = p_api_dev->glb_mem.biu[uc_Biu].bm_tcb_addr;
    TriggerControlBlockSize = sizeof(TY_API_BM_TCB_DESC);

    Status = UsbSynchronizeMemoryArea(p_api_dev, RelativeOffset, TriggerControlBlockSize, direction);

    return Status;
}


/* Synchronizes the monitor specific system control block registers of a specific BIU 
   between global memory and its mirror */
AiInt32 UsbSynchronizeBMSystemControlBlock(TY_API_DEV* p_api_dev, short uc_Biu, 
                                            enum SYNCH_DIRECTION direction)
{
    AiInt32 Status = API_ERR;
    size_t MonitorSystemControlBlockOffset;
    size_t MonitorSystemControlBlockSize;
    size_t BusSpecificCounterAreaOffset;
    size_t BusSpecificCountSize;

    /* Relative offset of Monitor specific registers in system control block */
    MonitorSystemControlBlockOffset = API_GLB_MEM_ADDR_REL(&p_api_dev->cb_p[uc_Biu]->mbba);

    /* Size of Monitor specific area in System control block */
    MonitorSystemControlBlockSize = (ptrdiff_t) &p_api_dev->cb_p[uc_Biu]->bm_act_addr -
                                    (ptrdiff_t) &p_api_dev->cb_p[uc_Biu]->mbba;

    /* Prepare first scatter list entry with bus monitor specific system control block area */
    /* SGL DMA is not supported by Linux USB driver, so we read one block after the other */
    Status = UsbSynchronizeMemoryArea(p_api_dev, MonitorSystemControlBlockOffset, MonitorSystemControlBlockSize, direction);

    if( Status != API_OK )
    {
        return Status;
    }

    /* Relative offset of Bus specific counter area in system control block */
    BusSpecificCounterAreaOffset = API_GLB_MEM_ADDR_REL(&p_api_dev->cb_p[uc_Biu]->bm_xfer_cnt_sec);

    /* Size of Bus specific counter area in system control block */
    BusSpecificCountSize = 0x10;

    /* Prepare second scatter list entry with bus specific counter area */
    /* SGL DMA is not supported by Linux USB driver, so we read one block after the other */
    Status = UsbSynchronizeMemoryArea(p_api_dev, BusSpecificCounterAreaOffset, BusSpecificCountSize, direction);

    return Status;
}


/* Synchronizes the interrupt loglist of a specific BIU between global memory and its mirror */
AiInt32 UsbSynchronizeInterruptLoglist(TY_API_DEV* p_api_dev, short uc_Biu, 
                                        enum SYNCH_DIRECTION direction)
{
    AiInt32 Status;
    size_t RelativeOffset;
    size_t InterruptLoglistSize;

    RelativeOffset = p_api_dev->glb_mem.biu[uc_Biu].ir_log_addr;
    InterruptLoglistSize = p_api_dev->glb_mem.biu[uc_Biu].rt_desc_addr - RelativeOffset;

    Status = UsbSynchronizeMemoryArea(p_api_dev, RelativeOffset, InterruptLoglistSize, 
                                      direction);

    return Status;
}


/* Synchronizes replay specific data between global memory and its mirror */
AiInt32 UsbSynchronizeReplay(TY_API_DEV* p_api_dev, short uc_Biu, 
                              enum SYNCH_DIRECTION direction)
{
    AiInt32 Status;

    Status = UsbSynchronizeReplayBuffer(p_api_dev, uc_Biu, direction);
    if(Status)
    {
        return Status;
    }

    Status = UsbSynchronizeReplaySystemControlBlock(p_api_dev, uc_Biu, direction);
    if(Status)
    {
        return Status;

    }

    return Status;
}


/* Synchronizes replay area of the System Control Block between global memory and its mirror */
AiInt32 UsbSynchronizeReplaySystemControlBlock(TY_API_DEV* p_api_dev, short uc_Biu, 
                                                enum SYNCH_DIRECTION direction)
{
    AiInt32 Status;
    size_t RelativeOffset;
    size_t Size;

    RelativeOffset = API_GLB_MEM_ADDR_REL(&p_api_dev->cb_p[uc_Biu]->rcw);

    Size = (ptrdiff_t) &(p_api_dev->cb_p[uc_Biu]->res4[0]) - (ptrdiff_t) &p_api_dev->cb_p[uc_Biu]->rcw;

    Status = UsbSynchronizeMemoryArea(p_api_dev, RelativeOffset, Size, direction);

    return Status;
}


/* Synchronizes the replay data buffer of a specific BIU between global memory and its mirror */
AiInt32 UsbSynchronizeReplayBuffer(TY_API_DEV* p_api_dev, short uc_Biu, 
                                    enum SYNCH_DIRECTION direction)
{
    AiInt32 Status;
    size_t RelativeReplayBufferOffset;
    size_t ReplayBufferSize;

    RelativeReplayBufferOffset = p_api_dev->glb_mem.biu[uc_Biu].base.rep_buf;
    ReplayBufferSize = p_api_dev->glb_mem.biu[uc_Biu].size.rep_buf;

    Status = UsbSynchronizeMemoryArea(p_api_dev, RelativeReplayBufferOffset, 
                                      ReplayBufferSize, direction);

    return Status;
}


/* Synchronizes all active memory blocks, that lie in the given memory range
   between global memory and its mirror */
AiInt32 UsbSynchronizeActiveMemoryBlocks(TY_API_DEV* p_api_dev, size_t RelativeOffset, 
                                          size_t Size, enum SYNCH_DIRECTION direction)
{
    AiInt32 Status;
    size_t RemainingStartOffsets[2];
    size_t RemainingSizes[2];
    unsigned char uc_NumRemainingIntervals;
    int i;

    if(Size == 0)
    {
        return API_OK;
    }

    /* check all available memory areas for intersection */
    Status = UsbSynchronizeActiveBiuMemory(p_api_dev, RelativeOffset, Size, direction,
                                           &uc_NumRemainingIntervals, RemainingStartOffsets,
                                           RemainingSizes);
    if(Status == API_OK)
        goto exit;

    Status = UsbSynchronizeActiveInterruptLoglistMemory(p_api_dev, RelativeOffset, Size, direction,
                                                        &uc_NumRemainingIntervals,
                                                        RemainingStartOffsets, RemainingSizes);
    if (Status == API_OK)
        goto exit;

    Status = UsbSynchronizeActiveMonitorActivityMemory(p_api_dev, RelativeOffset, Size, direction,
                                                       &uc_NumRemainingIntervals,
                                                       RemainingStartOffsets, RemainingSizes);
    if (Status == API_OK)
        goto exit;

    Status = UsbSynchronizeActiveRTBufferHeaderMemory(p_api_dev, RelativeOffset, Size, direction,
                                                      &uc_NumRemainingIntervals,
                                                      RemainingStartOffsets, RemainingSizes);
    if (Status == API_OK)
        goto exit;

    Status = UsbSynchronizeActiveRTStatusQueueMemory(p_api_dev, RelativeOffset, Size, direction,
                                                     &uc_NumRemainingIntervals,
                                                     RemainingStartOffsets, RemainingSizes);
    if (Status == API_OK)
        goto exit;

    Status = UsbSynchronizeActiveRTEventQueueMemory(p_api_dev, RelativeOffset, Size, direction,
                                                    &uc_NumRemainingIntervals,
                                                    RemainingStartOffsets, RemainingSizes);
    if (Status == API_OK)
        goto exit;

    Status = UsbSynchronizeActiveBCBufferHeaderMemory(p_api_dev, RelativeOffset, Size, direction,
                                                      &uc_NumRemainingIntervals,
                                                      RemainingStartOffsets, RemainingSizes);
    if (Status == API_OK)
        goto exit;

    Status = UsbSynchronizeActiveBCStatusQueueMemory(p_api_dev, RelativeOffset, Size, direction,
                                                     &uc_NumRemainingIntervals,
                                                     RemainingStartOffsets, RemainingSizes);
    if (Status == API_OK)
        goto exit;

    Status = UsbSynchronizeActiveBCEventQueueMemory(p_api_dev, RelativeOffset, Size, direction,
                                                    &uc_NumRemainingIntervals,
                                                    RemainingStartOffsets, RemainingSizes);
    if (Status == API_OK)
        goto exit;

    Status = UsbSynchronizeActiveMonitorBufferMemory(p_api_dev, RelativeOffset, Size, direction,
                                                     &uc_NumRemainingIntervals,
                                                     RemainingStartOffsets, RemainingSizes);
    if (Status == API_OK)
        goto exit;

    Status = UsbSynchronizeActiveSimulationBufferMemory(p_api_dev, RelativeOffset, Size, direction,
                                                        &uc_NumRemainingIntervals, 
                                                        RemainingStartOffsets, RemainingSizes);
    if (Status == API_OK)
        goto exit;
    
exit:

    if(Status)
    {
        return Status;
    }
    else
    {
        for(i = 0; i < uc_NumRemainingIntervals; i++)
        {
            /* Recursive call to synchronize the rest of the memory block */
            UsbSynchronizeActiveMemoryBlocks(p_api_dev, RemainingStartOffsets[i], RemainingSizes[i], 
                                             direction);
        }

        return Status;
    }

}


/* Checks if part of a given memory range lies in BIU specific System Control Block.
   If this is the case and the corresponding BIU is active, the memory will be synchronized between
   global memory and its mirror */
AiInt32 UsbSynchronizeActiveBiuMemory(TY_API_DEV* p_api_dev, size_t RelativeOffset, size_t Size, 
                                       enum SYNCH_DIRECTION direction, 
                                       unsigned char* puc_NumRemainingIntervals, 
                                       size_t RemainingStartOffsets[2], size_t RemainingSizes[2])
{
    AiInt32 Status = API_ERR;
    unsigned char uc_Biu = 0;
    size_t IntersectionStart;
    size_t IntersectionSize;

    do
    {
        if(IntersectMemoryInterval(p_api_dev->glb_mem.biu[uc_Biu].cb_addr, 
                                   p_api_dev->glb_mem.biu[uc_Biu].cb_addr + SYSTEM_CONTROL_BLOCK_SIZE, 
                                   RelativeOffset, RelativeOffset + Size, &IntersectionStart, 
                                   &IntersectionSize, puc_NumRemainingIntervals, RemainingStartOffsets, 
                                   RemainingSizes))
        {
            Status = API_OK;
        }
        else
        {
            uc_Biu++;
        }
        
    }while(Status != API_OK && uc_Biu < p_api_dev->chns);


    if(Status == API_OK)
    {
        if(IntersectionStart <= p_api_dev->glb_mem.biu[uc_Biu].cb_addr + 0x2C 
           && p_api_dev->b_BiuEnabled[uc_Biu])
        {
            Status = UsbSynchronizeMemoryArea(p_api_dev, IntersectionStart, IntersectionSize, 
                                              direction);
        }
        else if(IntersectionStart > p_api_dev->glb_mem.biu[uc_Biu].cb_addr + 0x2C 
                && IntersectionStart <= p_api_dev->glb_mem.biu[uc_Biu].cb_addr + 0x7C 
                && (p_api_dev->bc_status[uc_Biu] == API_BUSY || p_api_dev->rt_status[uc_Biu] == API_BUSY))
        {
            Status = UsbSynchronizeMemoryArea(p_api_dev, IntersectionStart, IntersectionSize, 
                                              direction);
        }
        else if(IntersectionStart > p_api_dev->glb_mem.biu[uc_Biu].cb_addr + 0x7C 
                && IntersectionStart <= p_api_dev->glb_mem.biu[uc_Biu].cb_addr + 0xBC 
                && p_api_dev->bm_status[uc_Biu] == API_BUSY)
        {
            Status = UsbSynchronizeMemoryArea(p_api_dev, IntersectionStart, IntersectionSize, 
                                          direction);
        }
        else if(IntersectionStart > p_api_dev->glb_mem.biu[uc_Biu].cb_addr + 0xBC 
                && IntersectionStart <= p_api_dev->glb_mem.biu[uc_Biu].cb_addr + 0xEC 
                && p_api_dev->rep_status[uc_Biu] == API_REP_BUSY)
        {
            Status = UsbSynchronizeMemoryArea(p_api_dev, IntersectionStart, IntersectionSize, 
                                              direction);
        }
    }

    return Status;
}


/* Checks if part of a given memory range lies in BIU specific interrupt loglist.
   If this is the case and the corresponding BIU is active, the memory will be synchronized between
   global memory and its mirror */
AiInt32 UsbSynchronizeActiveInterruptLoglistMemory(TY_API_DEV* p_api_dev, size_t RelativeOffset, 
                                                    size_t Size, enum SYNCH_DIRECTION direction, 
                                                    unsigned char* puc_NumRemainingIntervals, 
                                                    size_t RemainingStartOffsets[2], 
                                                    size_t RemainingSizes[2])
{
    AiInt32 Status = API_ERR;
    unsigned char uc_Biu = 0;
    size_t IntersectionStart;
    size_t IntersectionSize;

    do
    {
        if(IntersectMemoryInterval(p_api_dev->glb_mem.biu[uc_Biu].ir_log_addr, 
                                   p_api_dev->glb_mem.biu[uc_Biu].ir_log_addr + INTERRUPT_LOGLIST_SIZE, 
                                   RelativeOffset, RelativeOffset + Size, &IntersectionStart, 
                                   &IntersectionSize, puc_NumRemainingIntervals, RemainingStartOffsets, 
                                   RemainingSizes))
        {
            Status = API_OK;
        }
        else
        {
            uc_Biu++;
        }
        
    } while (Status != API_OK && uc_Biu < p_api_dev->chns);

    if (Status == API_OK && p_api_dev->b_BiuEnabled[uc_Biu])
    {
        Status = UsbSynchronizeMemoryArea(p_api_dev, IntersectionStart, IntersectionSize, 
                                          direction);
    }

    return Status;
}


/* Checks if part of a given memory range lies in monitor activity/filtering page.
   If this is the case and the monitor is active, the memory will be synchronized between
   global memory and its mirror */
AiInt32 UsbSynchronizeActiveMonitorActivityMemory(TY_API_DEV* p_api_dev, size_t RelativeOffset, 
                                                   size_t Size, enum SYNCH_DIRECTION direction, 
                                                   unsigned char* puc_NumRemainingIntervals, 
                                                   size_t RemainingStartOffsets[2], 
                                                   size_t RemainingSizes[2])
{
    AiInt32 Status = API_ERR;
    unsigned char uc_Biu = 0;
    size_t IntersectionStart;
    size_t IntersectionSize;
    size_t BmActPageSize;


    if( p_api_dev->ulUseEnhancedBmActivityPage )
        BmActPageSize = API_BM_ACT_ENH_PAGE_SIZE;
    else
        BmActPageSize = API_BM_ACT_PAGE_SIZE;

    do
    {
        if(IntersectMemoryInterval(p_api_dev->glb_mem.biu[uc_Biu].bm_act_addr, 
                                   p_api_dev->glb_mem.biu[uc_Biu].bm_act_addr + 
                                   BmActPageSize, RelativeOffset, 
                                   RelativeOffset + Size, &IntersectionStart, &IntersectionSize, 
                                   puc_NumRemainingIntervals, RemainingStartOffsets, RemainingSizes))
        {
            Status = API_OK;
        }
        else
        {
            uc_Biu++;
        }
        
    } while (Status != API_OK && uc_Biu < p_api_dev->chns);

    if (Status == API_OK && p_api_dev->bm_status[uc_Biu] == API_BUSY)
    {
        Status = UsbSynchronizeMemoryArea(p_api_dev, IntersectionStart, IntersectionSize, 
                                          direction);
    }

    return Status;
}


/* Checks if part of a given memory range lies in RT buffer header area.
   If this is the case and the corresponding RT is active, the memory will be synchronized between
   global memory and its mirror */
AiInt32 UsbSynchronizeActiveRTBufferHeaderMemory(TY_API_DEV* p_api_dev, size_t RelativeOffset, 
                                                  size_t Size, enum SYNCH_DIRECTION direction, 
                                                  unsigned char* puc_NumRemainingIntervals, 
                                                  size_t RemainingStartOffsets[2], 
                                                  size_t RemainingSizes[2])
{
    AiInt32 Status = API_ERR;
    unsigned char uc_Biu = 0;
    size_t IntersectionStart;
    size_t IntersectionSize;

    do
    {
        if(IntersectMemoryInterval(p_api_dev->glb_mem.biu[uc_Biu].base.rt_bh_area, 
                                   p_api_dev->glb_mem.biu[uc_Biu].base.rt_bh_area + 
                                   p_api_dev->glb_mem.biu[uc_Biu].size.rt_bh_area, RelativeOffset, 
                                   RelativeOffset + Size, &IntersectionStart, &IntersectionSize, 
                                   puc_NumRemainingIntervals, RemainingStartOffsets, RemainingSizes))
        {
            Status = API_OK;
        }
        else
        {
            uc_Biu++;
        }
        
    } while (Status != API_OK && uc_Biu < p_api_dev->chns);

    if (Status == API_OK && p_api_dev->rt_status[uc_Biu] == API_BUSY)
    {
        Status = UsbSynchronizeMemoryArea(p_api_dev, IntersectionStart, IntersectionSize, 
                                          direction);
    }

    return Status;
}



/* Checks if part of a given memory range lies in RT status queue area.
   If this is the case and the corresponding RT is active, the memory will be synchronized between
   global memory and its mirror */
AiInt32 UsbSynchronizeActiveRTStatusQueueMemory(TY_API_DEV* p_api_dev, size_t RelativeOffset, 
                                                 size_t Size, enum SYNCH_DIRECTION direction, 
                                                 unsigned char* puc_NumRemainingIntervals, 
                                                 size_t RemainingStartOffsets[2], 
                                                 size_t RemainingSizes[2])
{
    AiInt32 Status = API_ERR;
    unsigned char uc_Biu = 0;
    size_t IntersectionStart;
    size_t IntersectionSize;

    do
    {
        if(IntersectMemoryInterval(p_api_dev->glb_mem.biu[uc_Biu].base.rt_sq_area, 
                                   p_api_dev->glb_mem.biu[uc_Biu].base.rt_sq_area + 
                                   p_api_dev->glb_mem.biu[uc_Biu].size.rt_sq_area, RelativeOffset, 
                                   RelativeOffset + Size, &IntersectionStart, &IntersectionSize, 
                                   puc_NumRemainingIntervals, RemainingStartOffsets, RemainingSizes))
        {
            Status = API_OK;
        }
        else
        {
            uc_Biu++;
        }
        
    } while (Status != API_OK && uc_Biu < p_api_dev->chns);

    if (Status == API_OK && p_api_dev->rt_status[uc_Biu] == API_BUSY)
    {
        Status = UsbSynchronizeMemoryArea(p_api_dev, IntersectionStart, IntersectionSize, 
                                          direction);
    }

    return Status;
}



/* Checks if part of a given memory range lies in RT event queue area.
   If this is the case and the corresponding RT is active, the memory will be synchronized between
   global memory and its mirror */
AiInt32 UsbSynchronizeActiveRTEventQueueMemory(TY_API_DEV* p_api_dev, size_t RelativeOffset, 
                                                size_t Size, enum SYNCH_DIRECTION direction, 
                                                unsigned char* puc_NumRemainingIntervals, 
                                                size_t RemainingStartOffsets[2], 
                                                size_t RemainingSizes[2])
{
    AiInt32 Status = API_ERR;
    unsigned char uc_Biu = 0;
    size_t IntersectionStart;
    size_t IntersectionSize;

    do
    {
        if(IntersectMemoryInterval(p_api_dev->glb_mem.biu[uc_Biu].base.rt_eq_area, 
                                   p_api_dev->glb_mem.biu[uc_Biu].base.rt_eq_area + 
                                   p_api_dev->glb_mem.biu[uc_Biu].size.rt_eq_area, RelativeOffset, 
                                   RelativeOffset + Size, &IntersectionStart, &IntersectionSize, 
                                   puc_NumRemainingIntervals, RemainingStartOffsets, RemainingSizes))
        {
            Status = API_OK;
        }
        else
        {
            uc_Biu++;
        }
        
    } while (Status != API_OK && uc_Biu < p_api_dev->chns);

    if (Status == API_OK && p_api_dev->rt_status[uc_Biu] == API_BUSY)
    {
        Status = UsbSynchronizeMemoryArea(p_api_dev, IntersectionStart, IntersectionSize, 
                                          direction);
    }

    return Status;
}


/* Checks if part of a given memory range lies in BC buffer header area.
   If this is the case and the corresponding BC is active, the memory will be synchronized between
   global memory and its mirror */
AiInt32 UsbSynchronizeActiveBCBufferHeaderMemory(TY_API_DEV* p_api_dev, size_t RelativeOffset, 
                                                  size_t Size, enum SYNCH_DIRECTION direction, 
                                                  unsigned char* puc_NumRemainingIntervals, 
                                                  size_t RemainingStartOffsets[2], 
                                                  size_t RemainingSizes[2])
{
    AiInt32 Status = API_ERR;
    unsigned char uc_Biu = 0;
    size_t IntersectionStart;
    size_t IntersectionSize;

    do
    {
        if(IntersectMemoryInterval(p_api_dev->glb_mem.biu[uc_Biu].base.bc_bh_area, 
                                   p_api_dev->glb_mem.biu[uc_Biu].base.bc_bh_area + 
                                   p_api_dev->glb_mem.biu[uc_Biu].size.bc_bh_area, RelativeOffset, 
                                   RelativeOffset + Size, &IntersectionStart, &IntersectionSize, 
                                   puc_NumRemainingIntervals, RemainingStartOffsets, RemainingSizes))
        {
            Status = API_OK;
        }
        else
        {
            uc_Biu++;
        }
        
    } while (Status != API_OK && uc_Biu < p_api_dev->chns);

    if (Status == API_OK && p_api_dev->bc_status[uc_Biu] == API_BUSY)
    {
        Status = UsbSynchronizeMemoryArea(p_api_dev, IntersectionStart, IntersectionSize, 
                                          direction);
    }

    return Status;
}


/* Checks if part of a given memory range lies in BC status queue area.
   If this is the case and the corresponding BC is active, the memory will be synchronized between
   global memory and its mirror */
AiInt32 UsbSynchronizeActiveBCStatusQueueMemory(TY_API_DEV* p_api_dev, size_t RelativeOffset, 
                                                 size_t Size, enum SYNCH_DIRECTION direction, 
                                                 unsigned char* puc_NumRemainingIntervals, 
                                                 size_t RemainingStartOffsets[2], 
                                                 size_t RemainingSizes[2])
{
    AiInt32 Status = API_ERR;
    unsigned char uc_Biu = 0;
    size_t IntersectionStart;
    size_t IntersectionSize;

    do
    {
        if(IntersectMemoryInterval(p_api_dev->glb_mem.biu[uc_Biu].base.bc_sq_area, 
                                   p_api_dev->glb_mem.biu[uc_Biu].base.bc_sq_area + 
                                   p_api_dev->glb_mem.biu[uc_Biu].size.bc_sq_area, RelativeOffset, 
                                   RelativeOffset + Size, &IntersectionStart, &IntersectionSize, 
                                   puc_NumRemainingIntervals, RemainingStartOffsets, RemainingSizes))
        {
            Status = API_OK;
        }
        else
        {
            uc_Biu++;
        }
        
    } while (Status != API_OK && uc_Biu < p_api_dev->chns);

    if (Status == API_OK && p_api_dev->bc_status[uc_Biu] == API_BUSY)
    {
        Status = UsbSynchronizeMemoryArea(p_api_dev, IntersectionStart, IntersectionSize, 
                                          direction);
    }

    return Status;
}


/* Checks if part of a given memory range lies in BC event queue area.
   If this is the case and the corresponding BC is active, the memory will be synchronized between
   global memory and its mirror */
AiInt32 UsbSynchronizeActiveBCEventQueueMemory(TY_API_DEV* p_api_dev, size_t RelativeOffset, 
                                                size_t Size, enum SYNCH_DIRECTION direction, 
                                                unsigned char* puc_NumRemainingIntervals, 
                                                size_t RemainingStartOffsets[2], 
                                                size_t RemainingSizes[2])
{
    AiInt32 Status = API_ERR;
    unsigned char uc_Biu = 0;
    size_t IntersectionStart;
    size_t IntersectionSize;

    do
    {
        if(IntersectMemoryInterval(p_api_dev->glb_mem.biu[uc_Biu].base.bc_eq_area, 
                                   p_api_dev->glb_mem.biu[uc_Biu].base.bc_eq_area + 
                                   p_api_dev->glb_mem.biu[uc_Biu].size.bc_eq_area, RelativeOffset, 
                                   RelativeOffset + Size, &IntersectionStart, &IntersectionSize, 
                                   puc_NumRemainingIntervals, RemainingStartOffsets, RemainingSizes))
        {
            Status = API_OK;
        }
        else
        {
            uc_Biu++;
        }
        
    } while (Status != API_OK && uc_Biu < p_api_dev->chns);

    if (Status == API_OK && p_api_dev->bc_status[uc_Biu] == API_BUSY)
    {
        Status = UsbSynchronizeMemoryArea(p_api_dev, IntersectionStart, IntersectionSize, 
                                          direction);
    }

    return Status;
}


/* Checks if part of a given memory range lies in monitor buffer area.
   If this is the case and the corresponding monitor is active, the memory will be synchronized between
   global memory and its mirror */
AiInt32 UsbSynchronizeActiveMonitorBufferMemory(TY_API_DEV* p_api_dev, size_t RelativeOffset, 
                                                 size_t Size, enum SYNCH_DIRECTION direction, 
                                                 unsigned char* puc_NumRemainingIntervals, 
                                                 size_t RemainingStartOffsets[2], 
                                                 size_t RemainingSizes[2])
{
    AiInt32 Status = API_ERR;
    unsigned char uc_Biu = 0;
    size_t IntersectionStart;
    size_t IntersectionSize;

    do
    {
        if(IntersectMemoryInterval(p_api_dev->glb_mem.biu[uc_Biu].base.bm_buf, 
                                   p_api_dev->glb_mem.biu[uc_Biu].base.bm_buf + 
                                   p_api_dev->glb_mem.biu[uc_Biu].size.bm_buf, RelativeOffset, 
                                   RelativeOffset + Size, &IntersectionStart, &IntersectionSize, 
                                   puc_NumRemainingIntervals, RemainingStartOffsets, RemainingSizes))
        {
            Status = API_OK;
        }
        else
        {
            uc_Biu++;
        }
        
    } while (Status != API_OK && uc_Biu < p_api_dev->chns);

    if (Status == API_OK && p_api_dev->bm_status[uc_Biu] == API_BUSY)
    {
        Status = UsbSynchronizeMemoryArea(p_api_dev, IntersectionStart, IntersectionSize, 
                                          direction);
    }

    return Status;
}


/* Checks if part of a given memory range lies in simulation buffer area.
   If this is the case and the corresponding BC or RT is active, the memory will be synchronized between
   global memory and its mirror */
AiInt32 UsbSynchronizeActiveSimulationBufferMemory(TY_API_DEV* p_api_dev, size_t RelativeOffset, 
                                                    size_t Size, enum SYNCH_DIRECTION direction, 
                                                    unsigned char* puc_NumRemainingIntervals, 
                                                    size_t RemainingStartOffsets[2], 
                                                    size_t RemainingSizes[2])
{
    AiInt32 Status = API_ERR;
    unsigned char uc_Biu = 0;
    size_t IntersectionStart;
    size_t IntersectionSize;
    AiBoolean b_Synchronized;

    do
    {
        if(IntersectMemoryInterval(p_api_dev->glb_mem.sim_buf_base_addr[uc_Biu], 
                                   p_api_dev->glb_mem.sim_buf_base_addr[uc_Biu] + 
                                   p_api_dev->glb_mem.sim_buf_size[uc_Biu], RelativeOffset, 
                                   RelativeOffset + Size, &IntersectionStart, &IntersectionSize, 
                                   puc_NumRemainingIntervals, RemainingStartOffsets, RemainingSizes))
        {
            Status = API_OK;
        }
        else
        {
            uc_Biu++;
        }
        
    } while (Status != API_OK && uc_Biu < p_api_dev->chns);

    if (Status == API_OK)
    {
        /* Check all streams for beeing active, as simulator area is shared between all BIUs */
        uc_Biu = 0;
        b_Synchronized = FALSE;

        while(uc_Biu < p_api_dev->chns && !b_Synchronized)
        {
            if(p_api_dev->bc_status[uc_Biu] == API_BUSY || p_api_dev->rt_status[uc_Biu] == API_BUSY)
            {
                Status = UsbSynchronizeMemoryArea(p_api_dev, IntersectionStart, IntersectionSize, 
                                                  direction);

                b_Synchronized = TRUE;
            }

            uc_Biu++;
        }
    }

    return Status;
}



/* This function checks if two given memory intervals intersect. It delivers the intersection of these
   memory intervals and also the up to 2 remaining memory intervals that form the symmetric 
   difference quantity */
AiBoolean IntersectMemoryInterval(size_t FirstIntervalStart, size_t FirstIntervalEnd, 
                                size_t SecondIntervalStart, size_t SecondIntervalEnd, 
                                size_t* IntersectionStart, size_t* IntersectionSize, 
                                unsigned char* uc_NumRemainingIntervals,
                                size_t RemainingStartIntervals[2], size_t RemainingIntervalSizes[2])
{
    AiBoolean b_IsIntersect = FALSE;
    
    *uc_NumRemainingIntervals = 0;

    if(SecondIntervalStart >= FirstIntervalStart && SecondIntervalStart < FirstIntervalEnd)
    {
        /* The second interval starts in the first one */
        b_IsIntersect = TRUE;

        *IntersectionStart = SecondIntervalStart;
        *IntersectionSize = SecondIntervalEnd - SecondIntervalStart; 

        if(SecondIntervalEnd > FirstIntervalEnd)
        {
            /* The second interval reaches beyond the end of the first one */
            *IntersectionSize = FirstIntervalEnd - SecondIntervalStart;
            RemainingStartIntervals[0] = FirstIntervalEnd;
            RemainingIntervalSizes[0] = SecondIntervalEnd - FirstIntervalEnd;
            *uc_NumRemainingIntervals = 1;
        }

       
    }
    else if(SecondIntervalEnd > FirstIntervalStart && SecondIntervalEnd <= FirstIntervalEnd)
    {
        /* The second interval ends in the first one */
        b_IsIntersect = TRUE;

        *IntersectionStart = FirstIntervalStart;
        *IntersectionSize = SecondIntervalEnd - FirstIntervalStart; 

        if(SecondIntervalStart < FirstIntervalStart)
        {
            /* The second interval begins before the first interval */
            RemainingStartIntervals[0] = SecondIntervalStart;
            RemainingIntervalSizes[0] = FirstIntervalStart - SecondIntervalStart;
            *uc_NumRemainingIntervals = 1;
        }
    }
    else if(SecondIntervalStart < FirstIntervalStart && SecondIntervalEnd >= FirstIntervalEnd)
    {
        /* The first interval is a subset of the second one */
        b_IsIntersect = TRUE;

        *IntersectionStart = FirstIntervalStart;
        *IntersectionSize = FirstIntervalEnd - FirstIntervalStart; 

        RemainingStartIntervals[0] = SecondIntervalStart;
        RemainingIntervalSizes[0] = FirstIntervalStart - SecondIntervalStart;
        RemainingStartIntervals[1] = FirstIntervalEnd;
        RemainingIntervalSizes[1] = SecondIntervalEnd - FirstIntervalEnd;
        *uc_NumRemainingIntervals = 2;
    }

    return b_IsIntersect;
}


/* Synchronizes the BC misc block of a specific BIU between global memory and its mirror */
AiInt32 UsbSynchronizeBCMiscBlock(TY_API_DEV* p_api_dev, short uc_Biu, 
                                   enum SYNCH_DIRECTION direction)
{
    AiInt32 Status;
    size_t RelativeBCMiscBlockOffset;
    size_t BCMiscBlockSize;

    RelativeBCMiscBlockOffset = p_api_dev->glb_mem.biu[uc_Biu].cb_addr + 0x100;
    BCMiscBlockSize = 0x300;

    Status = UsbSynchronizeMemoryArea(p_api_dev, RelativeBCMiscBlockOffset, BCMiscBlockSize,
                                      direction);


    return Status;
}













/******************************************************************************
*
* Function from UsbListSyncWin.c
*
*****************************************************************************/


/* Synchronizes all transfers, that are in the transfer list, between global memory and its mirror */
AiInt32 UsbSynchronizeTransfers(TY_API_DEV* p_api_dev, short biu, enum SYNCH_DIRECTION direction)
{
    AiInt32 Status = API_OK;
    PTRANSFER_CONTEXT current_transfer;

    ai_tsw_os_lock_aquire(p_api_dev->TransferListLock[biu]);

    ai_list_for_each_entry(current_transfer, &(p_api_dev->TransferList[biu]), TRANSFER_CONTEXT, list)
    {
        Status = UsbSynchronizeTransfer(p_api_dev, current_transfer, AiTrue, direction);

        if( Status != API_OK )
            break;
    }

    ai_tsw_os_lock_release(p_api_dev->TransferListLock[biu]);

    return Status;
}


/* Synchronizes all RT buffers of the buffer list of a specific BIU
between global memory and its mirror */
AiInt32 UsbSynchronizeRTBuffers(TY_API_DEV* p_api_dev, short biu, enum SYNCH_DIRECTION direction)
{
    AiInt32 Status = API_OK;
    PRT_BUFFER_CONTEXT current_buffer;

    ai_tsw_os_lock_aquire(p_api_dev->RTBufferListLock[biu]);

    ai_list_for_each_entry(current_buffer, &(p_api_dev->RTBufferList[biu]), RT_BUFFER_CONTEXT, list)
    {
        Status = UsbSynchronizeRTBuffer(p_api_dev, current_buffer, direction);

        if (Status != API_OK)
            break;
    }

    ai_tsw_os_lock_release(p_api_dev->RTBufferListLock[biu]);

    return Status;
}


/* Synchronizes one specific transfer between global memory and its mirror */
AiInt32 UsbSynchronizeTransfer(TY_API_DEV* p_api_dev, PTRANSFER_CONTEXT p_TransferContext, AiBoolean b_IncludeBufferData, enum SYNCH_DIRECTION direction)
{
    AiInt32 Status = API_ERR;
    struct ty_api_bc_bh_desc* p_BufferHeader;
    struct ty_api_bc_xfer_desc* p_TransferDescriptor;
    unsigned char uc_QueueSize;
    unsigned short us_NumBuffers;
    size_t RelativeTransferDescriptorOffset;
    size_t StatusQueueSize;
    unsigned long StatusQueueMode;

    p_TransferDescriptor     = p_TransferContext->p_TransferDescriptor;

    if ((p_TransferDescriptor->bhd_ptr <  p_api_dev->glb_mem.biu[p_api_dev->biu].base.bc_bh_area)
     || (p_TransferDescriptor->bhd_ptr >(p_api_dev->glb_mem.biu[p_api_dev->biu].base.bc_bh_area + p_api_dev->glb_mem.biu[p_api_dev->biu].size.bc_bh_area)))
    {
        /*@TODO: Remove if mirror is stable */
        return API_ERR;
    }

    p_BufferHeader = (struct ty_api_bc_bh_desc*) API_GLB_MEM_ADDR_ABS(p_TransferDescriptor->bhd_ptr);


    uc_QueueSize = (p_BufferHeader->bh_ctrl >> 14) & 0xF;
    us_NumBuffers = 1 << uc_QueueSize;

    /* Synchronize Event queue */
    Status = UsbSynchronizeMemoryArea(p_api_dev, p_BufferHeader->eq_addr, MAX_API_BC_EQ_DESC_SIZE, direction);

    if( Status != API_OK )
    {
        return Status;
    }


    /* Synchronize Status queue */
    StatusQueueMode = (p_BufferHeader->bh_ctrl >> 12) & 0x3L;

    if (StatusQueueMode == API_BC_BH_STS_QSIZE_SQ)
    {
        StatusQueueSize = us_NumBuffers * MAX_API_BC_SQ_DESC_SIZE;
    }
    else
    {
        StatusQueueSize = MAX_API_BC_SQ_DESC_SIZE;
    }

    Status = UsbSynchronizeMemoryArea(p_api_dev, p_BufferHeader->sq_addr, StatusQueueSize, direction);

    if (Status)
    {
        return Status;
    }


    /* Synchronize Buffer header */
    Status = UsbSynchronizeMemoryArea(p_api_dev, p_TransferDescriptor->bhd_ptr, MAX_API_BC_BH_DESC_SIZE, direction);

    if (Status)
    {
        return Status;
    }

    /* Synchronize transfer descriptor */
    RelativeTransferDescriptorOffset = API_GLB_MEM_ADDR_REL(p_TransferDescriptor);

    Status = UsbSynchronizeMemoryArea(p_api_dev, RelativeTransferDescriptorOffset, MAX_API_BC_XFER_DESC_SIZE, direction);

    if (Status)
    {
        return Status;
    }

    if (b_IncludeBufferData)
    {
        Status = UsbSynchronizeMemoryArea(p_api_dev, p_BufferHeader->bp_addr, us_NumBuffers * MAX_API_BUF_SIZE, direction);

        if (Status)
        {
            return Status;
        }
    }


    if ((p_TransferDescriptor->bhd_ptr <  p_api_dev->glb_mem.biu[p_api_dev->biu].base.bc_bh_area)
     || (p_TransferDescriptor->bhd_ptr >(p_api_dev->glb_mem.biu[p_api_dev->biu].base.bc_bh_area + p_api_dev->glb_mem.biu[p_api_dev->biu].size.bc_bh_area)))
    {
        /*@TODO: Remove if mirror is stable */
        return API_ERR;
    }

    return Status;
}


/* Synchronizes one RT buffer between global memory and its mirror */
AiInt32 UsbSynchronizeRTBuffer(TY_API_DEV* p_api_dev, PRT_BUFFER_CONTEXT p_BufferContext, enum SYNCH_DIRECTION direction)
{
    AiInt32 Status = API_ERR;
    struct ty_api_rt_bh_desc* p_BufferHeader;
    unsigned char uc_QueueSize;
    unsigned short us_NumBuffers;
    size_t RelativeBufferHeaderOffset;
    size_t StatusQueueSize;
    unsigned long StatusQueueMode;

    p_BufferHeader = p_BufferContext->p_BufferHeader;

    uc_QueueSize = (p_BufferHeader->bh_ctrl >> 14) & 0xF;
    us_NumBuffers = 1 << uc_QueueSize;

    /* Synchronize the buffer data */
    Status = UsbSynchronizeMemoryArea(p_api_dev, p_BufferHeader->bp_addr, us_NumBuffers * MAX_API_BUF_SIZE, direction);

    if (Status)
    {
        return Status;
    }

    /* Synchronize the Event queue */
    Status = UsbSynchronizeMemoryArea(p_api_dev, p_BufferHeader->eq_addr, MAX_API_RT_EQ_DESC_SIZE, direction);

    if (Status)
    {
        return Status;
    }

    /* Synchronize the Status queue*/
    StatusQueueMode = (p_BufferHeader->bh_ctrl >> 12) & 0x3L;

    if (StatusQueueMode == API_BC_BH_STS_QSIZE_SQ)
    {
        StatusQueueSize = us_NumBuffers * MAX_API_BC_SQ_DESC_SIZE;
    }
    else
    {
        StatusQueueSize = MAX_API_BC_SQ_DESC_SIZE;
    }

    Status = UsbSynchronizeMemoryArea(p_api_dev, p_BufferHeader->sq_addr, StatusQueueSize, direction);

    if (Status)
    {
        return Status;
    }

    /* Synchronize the buffer header */
    RelativeBufferHeaderOffset = API_GLB_MEM_ADDR_REL(p_BufferHeader);

    Status = UsbSynchronizeMemoryArea(p_api_dev, RelativeBufferHeaderOffset, MAX_API_BC_BH_DESC_SIZE, direction);

    if (Status)
    {
        return Status;
    }

    return Status;
}


/* Adds one transfer to the BIU specific transfer list */
void UsbRTBufferListAdd(TY_API_DEV* p_api_dev, short biu, unsigned short hid, size_t RelativeBufferHeaderOffset)
{
    PRT_BUFFER_CONTEXT current_buffer;
    struct ty_api_rt_bh_desc* AbsoluteBufferHeaderAddress;
    AiBoolean found = AiFalse;


    ai_tsw_os_lock_aquire(p_api_dev->RTBufferListLock[biu]);

    ai_list_for_each_entry(current_buffer, &(p_api_dev->RTBufferList[biu]), RT_BUFFER_CONTEXT, list)
    {
        if (current_buffer->us_HeaderID == hid)
        {
            found = AiTrue;
            break;
        }
    }

    AbsoluteBufferHeaderAddress = (struct ty_api_rt_bh_desc*)API_GLB_MEM_ADDR_ABS(RelativeBufferHeaderOffset);

    if (found)
    {
        current_buffer->p_BufferHeader =  AbsoluteBufferHeaderAddress;
    }
    else
    {
        /* RT buffer is not in list yet, so create a new RT buffer object */

        current_buffer = ai_tsw_os_malloc(sizeof(RT_BUFFER_CONTEXT));

        if (current_buffer)
        {
            AI_LIST_INIT(current_buffer->list);

            current_buffer->us_HeaderID    = hid;
            current_buffer->p_BufferHeader = AbsoluteBufferHeaderAddress;

            ai_list_add_tail(&current_buffer->list, &(p_api_dev->RTBufferList[biu]));
        }
    }

    ai_tsw_os_lock_release(p_api_dev->RTBufferListLock[biu]);
}


/* Removes all transfers from the BIU specific RT buffer list */
void UsbRTBufferListClear(TY_API_DEV* p_api_dev, short biu)
{
    PRT_BUFFER_CONTEXT current_buffer;
    PRT_BUFFER_CONTEXT next_buffer;

    ai_tsw_os_lock_aquire(p_api_dev->RTBufferListLock[biu]);

    ai_list_for_each_entry_safe(current_buffer, next_buffer, &(p_api_dev->RTBufferList[biu]), RT_BUFFER_CONTEXT, list)
    {
        ai_list_del(&current_buffer->list);
        ai_tsw_os_free(current_buffer);
    }

    ai_tsw_os_lock_release(p_api_dev->RTBufferListLock[biu]);
}


/* Adds one transfer to the BIU specific transfer list */
void UsbTransferListAdd(TY_API_DEV* p_api_dev, short biu, unsigned short xid, struct ty_api_bc_xfer_desc* p_TransferDescriptor, AiBoolean b_SynchOut)
{
    PTRANSFER_CONTEXT current_transfer;
    AiBoolean found = AiFalse;

    ai_tsw_os_lock_aquire(p_api_dev->TransferListLock[biu]);

    ai_list_for_each_entry(current_transfer, &(p_api_dev->TransferList[biu]), TRANSFER_CONTEXT, list)
    {
        if (current_transfer->us_TransferID == xid)
        {
            found = AiTrue;
        }
    }

    if (found)
    {
        current_transfer->p_TransferDescriptor = p_TransferDescriptor;
    }
    else
    {
        /* RT buffer is not in list yet, so create a new RT buffer object */

        current_transfer = ai_tsw_os_malloc(sizeof(TRANSFER_CONTEXT));

        if (current_transfer)
        {
            AI_LIST_INIT(current_transfer->list);

            current_transfer->us_TransferID        = xid;
            current_transfer->p_TransferDescriptor = p_TransferDescriptor;

            ai_list_add_tail(&current_transfer->list, &(p_api_dev->TransferList[biu]));
        }
    }

    if (b_SynchOut)
    {
        UsbSynchronizeTransfer(p_api_dev, current_transfer, AiFalse, Out);
    }

    ai_tsw_os_lock_release(p_api_dev->TransferListLock[biu]);
}


/* Removes all transfers from the BIU specific transfer list */
void UsbTransferListClear(TY_API_DEV* p_api_dev, short biu)
{
    PTRANSFER_CONTEXT current_transfer;
    PTRANSFER_CONTEXT next_transfer;

    ai_tsw_os_lock_aquire(p_api_dev->TransferListLock[biu]);

    ai_list_for_each_entry_safe(current_transfer, next_transfer, &(p_api_dev->TransferList[biu]), TRANSFER_CONTEXT, list)
    {
        ai_list_del(&current_transfer->list);
        ai_tsw_os_free(current_transfer);
    }

    ai_tsw_os_lock_release(p_api_dev->TransferListLock[biu]);

}


/* Synchronizes one specific transfer determined by its transfer ID
between global memory and its mirror. The transfer list of the given BIU will be searched for
the transfer. */
AiInt32 UsbSynchronizeTransferByID(TY_API_DEV* p_api_dev, short biu, unsigned short xid, AiBoolean b_IncludeBufferData, enum SYNCH_DIRECTION direction)
{
    AiInt32 status = API_ERR;
    PTRANSFER_CONTEXT current_transfer;

    ai_tsw_os_lock_aquire(p_api_dev->TransferListLock[biu]);

    ai_list_for_each_entry(current_transfer, &(p_api_dev->TransferList[biu]), TRANSFER_CONTEXT, list)
    {
        if (current_transfer->us_TransferID == xid)
        {
            status = UsbSynchronizeTransfer(p_api_dev, current_transfer, b_IncludeBufferData, direction);
            break;
        }
    }

    ai_tsw_os_lock_release(p_api_dev->TransferListLock[biu]);

    return status;
}
