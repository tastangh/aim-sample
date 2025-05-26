/* SPDX-FileCopyrightText: 2001-2022 AIM GmbH <info@aim-online.com> */
/* SPDX-License-Identifier: MIT OR GPL-2.0-or-later */

/*! \file UsbListSynchWin.h

   Header file that contains declarations for Transfer and RT buffer list functions
*/

#ifndef _USBLISTSYNCHWIN_H
#define _USBLISTSYNCHWIN_H

#include "Driver.h"
#include "UsbGlobalMemoryManagement.h"

/* struct that defines the transfer context for a transfer list entry */
typedef struct _TRANSFER_CONTEXT
{
    unsigned short us_TransferID;
    struct ty_api_bc_xfer_desc* p_TransferDescriptor;
}TRANSFER_CONTEXT, *PTRANSFER_CONTEXT;


/* context for an RT buffer in the RT buffer list */
typedef struct _RT_BUFFER_CONTEXT
{
    unsigned short us_HeaderID;
    struct ty_api_rt_bh_desc* p_BufferHeader;
}RT_BUFFER_CONTEXT, *PRT_BUFFER_CONTEXT;


WDF_DECLARE_CONTEXT_TYPE_WITH_NAME(TRANSFER_CONTEXT, GetTransferContext);
WDF_DECLARE_CONTEXT_TYPE_WITH_NAME(RT_BUFFER_CONTEXT, GetRtBufferContext);


/* Synchronizes all transfers, that are in the transfer list, between global memory and its mirror
   Parameters: TY_API_DEV - pointer to target vars structure
               unsigned char - ID of BIU to synchronize on
               enum SYNCH_DIRECTION - direction to synchronize
   Return: return 0, if synch was successful */
AiInt32 UsbSynchronizeTransfers(TY_API_DEV* p_api_dev,short uc_Biu,
                                enum SYNCH_DIRECTION direction);


/* Synchronizes all RT buffers of the buffer list of a specific BIU between global memory and its mirror
   Parameters: TY_API_DEV - pointer to target vars structure
               unsigned char - ID of BIU
               enum SYNCH_DIRECTION - direction to synchronize
   Return: return 0, if synch was successful */
AiInt32 UsbSynchronizeRTBuffers(TY_API_DEV* p_api_dev, short uc_Biu,
                                enum SYNCH_DIRECTION direction);


/* Removes all transfers from the BIU specific RT buffer list
   Parameters: TY_API_DEV* - pointer to target vars structure
               unsigned short - ID of BIU */
void UsbRTBufferListClear(TY_API_DEV* p_api_dev, short us_Biu);


/* Adds one transfer to the BIU specific RT buffer list
   Parameters: TY_API_DEV* - pointer to target vars structure
               unsigned short - ID of BIU
               unsigned short - ID of RT buffer header
               size_t - relative buffer header offset in global memory */
void UsbRTBufferListAdd(TY_API_DEV* p_api_dev, short us_Biu, unsigned short us_HeaderID,
                        size_t RelativeBufferHeaderOffset);


/* Removes all transfers from the BIU specific transfer list
   Parameters: TY_API_DEV* - pointer to target vars structure
               unsigned short - ID of BIU */
void UsbTransferListClear(TY_API_DEV* p_api_dev, short us_Biu);


/*! \brief  Adds one transfer to the BIU specific transfer list
    Optionally sets up transfer and corresponding buffer on board immediately
    \param p_api_dev pointer to target vars structure
    \param us_Biu ID of BIU
    \param us_TransferID ID of transfer
    \param p_TransferDescriptor pointer to transfer descriptor in global memory mirror
    \param b_SynchOut if true, transfer, as well as buffer header, will be transferred to board immediately */
void UsbTransferListAdd(TY_API_DEV* p_api_dev, short us_Biu, unsigned short us_TransferID,
                        struct ty_api_bc_xfer_desc* p_TransferDescriptor, BOOLEAN b_SynchOut);


/* Synchronizes one specific transfer determined by its transfer ID
   between global memory and its mirror. The transfer list of the given BIU will be searched for
   the transfer.
   Parameters: TY_API_DEV - pointer to target vars structure
               unsigned char - ID of BIU the transfer belongs to
               unsigned short - ID of transfer to synchronize
               BOOLEAN - if true, the buffer data of the transfer will also be synchronized
               enum SYNCH_DIRECTION - direction to synchronize
   Return: return 0, if synch was successful */
AiInt32 UsbSynchronizeTransferByID(TY_API_DEV* p_api_dev, short uc_Biu,
                                   unsigned short us_TransferID, BOOLEAN b_IncludeBufferData,
                                   enum SYNCH_DIRECTION direction);


/* Synchronizes one specific transfer between global memory and its mirror
   Parameters: TY_API_DEV - pointer to target vars structure
               WDFOBJECT - Transfer object to synchronize
               BOOLEAN - if true, the buffer data of the transfer will also be synchronized
               enum SYNCH_DIRECTION - direction to synchronize
   Return: return 0, if synch was successful */
AiInt32 UsbSynchronizeTransfer(TY_API_DEV* p_api_dev, PTRANSFER_CONTEXT p_TransferContext,
                               BOOLEAN b_IncludeBufferData, enum SYNCH_DIRECTION direction);


/* Synchronizes one RT buffer between global memory and its mirror
   Parameters: TY_API_DEV - pointer to target vars structure
               PRT_BUFFER_CONTEXT  - RT buffer to synchronize
               enum SYNCH_DIRECTION - direction to synchronize
   Return: return 0, if synch was successful */
AiInt32 UsbSynchronizeRTBuffer(TY_API_DEV* p_api_dev, PRT_BUFFER_CONTEXT p_BufferContext,
                               enum SYNCH_DIRECTION direction);


#endif

