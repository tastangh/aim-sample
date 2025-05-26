/* SPDX-FileCopyrightText: 2001-2022 AIM GmbH <info@aim-online.com> */
/* SPDX-License-Identifier: MIT OR GPL-2.0-or-later */

/*! \file UsbGlobalMemoryManagement.h
 *
 *  Header file with function declarations for USB Global Memory Mirror
 *
 */

#ifndef _USBGLOBALMEMORYMANAGEMENT_H
#define _USBGLOBALMEMORYMANAGEMENT_H

#include "mil_tsw_includes.h"
#include "mil_usb_io.h"
#include "Ai_container.h"

/* enumeration for determining direction of memory synchronization */
enum SYNCH_DIRECTION
{
    In = 0,
    Out,
};

#define READ_GLOBAL_U32(p_AbsoluteMirrorAddress) UsbSynchAndReadLWord(p_api_dev, p_AbsoluteMirrorAddress)


#define SYSTEM_CONTROL_BLOCK_SIZE 0x100
#define INTERRUPT_LOGLIST_SIZE 0x400

#define READ_GLOBAL_U32(p_AbsoluteMirrorAddress) UsbSynchAndReadLWord(p_api_dev, p_AbsoluteMirrorAddress)




/* Creates a Transfer List for each possible BIU of the given device */
AiInt32 CreateTransferLists(TY_API_DEV* p_api_dev);


/* Creates an RT Buffer List for each BIU of the given Device */
AiInt32 CreateRTBufferLists(TY_API_DEV* p_api_dev);


/* Synchronizes one longword between global memory and its mirror
   Parameters: TY_API_DEV - pointer to target vars structure
               size_t - relative memory offset to synchronize
               enum SYNCH_DIRECTION - direction to synchronize
   Return: return 0, if synch was successful */
AiInt32 UsbSynchronizeLWord(TY_API_DEV* p_api_dev, size_t ul_RelativeOffset, 
                            enum SYNCH_DIRECTION direction);

/* Synchronizes the System Control Block of a sepcific BIU between global memory and its mirror
   Parameters: TY_API_DEV - pointer to target vars structure
               unsigned char - ID of BIU to synchronize
               enum SYNCH_DIRECTION - direction to synchronize
   Return: return 0, if synch was successful */
AiInt32 UsbSynchronizeSystemControlBlock(TY_API_DEV* p_api_dev, short uc_Biu, 
                                         enum SYNCH_DIRECTION direction);

/* Synchronizes the BC relative data between global memory and its mirror
   Parameters: TY_API_DEV - pointer to target vars structure
               unsigned char - ID of BIU to synchronize on
               enum SYNCH_DIRECTION - direction to synchronize
   Return: return 0, if synch was successful */
AiInt32 UsbSynchronizeBC(TY_API_DEV* p_api_dev, short uc_Biu, enum SYNCH_DIRECTION direction);

/* Synchronizes the RT relative data between global memory and its mirror
   Parameters: TY_API_DEV - pointer to target vars structure
               unsigned char - ID of BIU to synchronize on
               enum SYNCH_DIRECTION - direction to synchronize
   Return: return 0, if synch was successful */
AiInt32 UsbSynchronizeRT(TY_API_DEV* p_api_dev, short uc_Biu, enum SYNCH_DIRECTION direction);

/* Synchronizes instruction lists of a specific BIU between global memory and its mirror
   Parameters: TY_API_DEV - pointer to target vars structure
               unsigned char - ID of BIU
               enum SYNCH_DIRECTION - direction to synchronize
   Return: return 0, if synch was successful */
AiInt32 UsbSynchronizeInstructionLists(TY_API_DEV* p_api_dev, short uc_Biu, 
                                       enum SYNCH_DIRECTION direction);

/* Synchronizes BC data area of System Control Block between global memory and its mirror
   Parameters: TY_API_DEV - pointer to target vars structure
               unsigned char - ID of BIU
               enum SYNCH_DIRECTION - direction to synchronize
   Return: return 0, if synch was successful */
AiInt32 UsbSynchronizeBCSystemControlBlock(TY_API_DEV* p_api_dev, short uc_Biu, 
                                           enum SYNCH_DIRECTION direction);

/* Synchronizes a specific memory range between global memory and its mirror
   Parameters: TY_API_DEV - pointer to target vars structure
               size_t - relative global memory offset to start synchronization
               size_t - size of synchronization in Bytes
               enum SYNCH_DIRECTION - direction to synchronize
   Return: return 0, if synch was successful */
AiInt32 UsbSynchronizeMemoryArea(TY_API_DEV* p_api_dev, size_t RelativeOffset, size_t Size, 
                                 enum SYNCH_DIRECTION direction);

/* Synchronizes acyclic instruction list of a specific BIU between global memory and its mirror
   Parameters: TY_API_DEV - pointer to target vars structure
               unsigned char - ID of BIU
               enum SYNCH_DIRECTION - direction to synchronize
   Return: return 0, if synch was successful */
AiInt32 UsbSynchronizeAcyclicInstructionList(TY_API_DEV* p_api_dev, short uc_Biu, 
                                             enum SYNCH_DIRECTION direction);

/* Synchronizes the RT descriptor area of a specific BIU between global memory and its mirror
   Parameters: TY_API_DEV - pointer to target vars structure
               unsigned char - ID of BIU
               enum SYNCH_DIRECTION - direction to synchronize
   Return: return 0, if synch was successful */
AiInt32 UsbSynchronizeRTDescriptorArea(TY_API_DEV* p_api_dev, short uc_Biu, 
                                       enum SYNCH_DIRECTION direction);

/* Synchronizes the RT descriptor of a specific RT between global memory and its mirror
   Parameters: TY_API_DEV - pointer to target vars structure
               unsigned char - ID of BIU the RT belongs to
               unsigned char - ID of RT to synchronize
               enum SYNCH_DIRECTION - direction to synchronize
   Return: return 0, if synch was successful */
AiInt32 UsbSynchronizeRTDescriptor(TY_API_DEV* p_api_dev, short uc_Biu, unsigned char uc_RT, 
                                   enum SYNCH_DIRECTION direction);

/* Synchronizes the RT subaddress descriptor area of a specific BIU between global memory and its mirror
   Parameters: TY_API_DEV - pointer to target vars structure
               unsigned char - ID of BIU
               enum SYNCH_DIRECTION - direction to synchronize
   Return: return 0, if synch was successful */
AiInt32 UsbSynchronizeRTSubAddressDescriptorArea(TY_API_DEV* p_api_dev, short uc_Biu, 
                                                 enum SYNCH_DIRECTION direction);

/* Synchronizes the RT area of the System Control Block of a specific BIU 
   between global memory and its mirror
   Parameters: TY_API_DEV - pointer to target vars structure
               unsigned char - ID of BIU
               enum SYNCH_DIRECTION - direction to synchronize
   Return: return 0, if synch was successful */
AiInt32 UsbSynchronizeRTSystemControlBlock(TY_API_DEV* p_api_dev, short uc_Biu, 
                                           enum SYNCH_DIRECTION direction);

/* Synchronizes the bus monitor specific data between global memory and its mirror
   Parameters: TY_API_DEV - pointer to target vars structure
               unsigned char - ID of BIU to synchronize on
               enum SYNCH_DIRECTION - direction to synchronize
   Return: return 0, if synch was successful */
AiInt32 UsbSynchronizeBM(TY_API_DEV* p_api_dev, short uc_Biu, enum SYNCH_DIRECTION direction);

/* Synchronizes the monitor data buffer of a specific BIU between global memory and its mirror
   Parameters: TY_API_DEV - pointer to target vars structure
               unsigned char - ID of BIU
               enum SYNCH_DIRECTION - direction to synchronize
   Return: return 0, if synch was successful */
AiInt32 UsbSynchronizeMonitorDataBuffer(TY_API_DEV* p_api_dev, short uc_Biu, 
                                        enum SYNCH_DIRECTION direction);

/* Synchronizes the monitor activity/filtering page of a specific BIU 
   between global memory and its mirror
   Parameters: TY_API_DEV - pointer to target vars structure
               unsigned char - ID of BIU
               enum SYNCH_DIRECTION - direction to synchronize
   Return: return 0, if synch was successful */
AiInt32 UsbSynchronizeMonitorActivityFilteringPage(TY_API_DEV* p_api_dev, short uc_Biu, 
                                                   enum SYNCH_DIRECTION direction);

/* Synchronizes the monitor activity/filtering page section of a specific RT 
   between global memory and its mirror
   Parameters: TY_API_DEV - pointer to target vars structure
               unsigned char - ID of BIU
               unsigned char - ID of RT
               enum SYNCH_DIRECTION - direction to synchronize
   Return: return 0, if synch was successful */
AiInt32 UsbSynchronizeMonitorActivityPageOfRT(TY_API_DEV* p_api_dev, short uc_Biu, unsigned char uc_Rt, 
                                              enum SYNCH_DIRECTION direction);

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
                                              enum SYNCH_DIRECTION direction);

/* Synchronizes the monitor trigger control block of a specific BIU between global memory and its mirror
   Parameters: TY_API_DEV - pointer to target vars structure
               unsigned char - ID of BIU
               enum SYNCH_DIRECTION - direction to synchronize
   Return: return 0, if synch was successful */
AiInt32 UsbSynchronizeTriggerControlBlockArea(TY_API_DEV* p_api_dev, short uc_Biu, 
                                              enum SYNCH_DIRECTION direction);

/* Synchronizes the monitor specific system control block registers of a specific BIU 
   between global memory and its mirror
   Parameters: TY_API_DEV - pointer to target vars structure
               unsigned char - ID of BIU
               enum SYNCH_DIRECTION - direction to synchronize
   Return: return 0, if synch was successful */
AiInt32 UsbSynchronizeBMSystemControlBlock(TY_API_DEV* p_api_dev, short uc_Biu, 
                                           enum SYNCH_DIRECTION direction);

/* Synchronizes the interrupt loglist of a specific BIU between global memory and its mirror
   Parameters: TY_API_DEV - pointer to target vars structure
               unsigned char - ID of BIU
               enum SYNCH_DIRECTION - direction to synchronize
   Return: return 0, if synch was successful */
AiInt32 UsbSynchronizeInterruptLoglist(TY_API_DEV* p_api_dev, short uc_Biu, 
                                       enum SYNCH_DIRECTION direction);

/* Synchronizes replay specific data between global memory and its mirror
   Parameters: TY_API_DEV - pointer to target vars structure
               unsigned char - ID of BIU
               enum SYNCH_DIRECTION - direction to synchronize
   Return: return 0, if synch was successful */
AiInt32 UsbSynchronizeReplay(TY_API_DEV* p_api_dev, short uc_Biu, 
                             enum SYNCH_DIRECTION direction);

/* Synchronizes replay area of the System Control Block between global memory and its mirror
   Parameters: TY_API_DEV - pointer to target vars structure
               unsigned char - ID of BIU
               enum SYNCH_DIRECTION - direction to synchronize
   Return: return 0, if synch was successful */
AiInt32 UsbSynchronizeReplaySystemControlBlock(TY_API_DEV* p_api_dev, short uc_Biu, 
                                               enum SYNCH_DIRECTION direction);

/* Synchronizes the replay data buffer of a specific BIU between global memory and its mirror
   Parameters: TY_API_DEV - pointer to target vars structure
               unsigned char - ID of BIU
               enum SYNCH_DIRECTION - direction to synchronize
   Return: return 0, if synch was successful */
AiInt32 UsbSynchronizeReplayBuffer(TY_API_DEV* p_api_dev, short uc_Biu, 
                                   enum SYNCH_DIRECTION direction);

/* Synchronizes all active memory blocks, that lie in the given memory range
   between global memory and its mirror
   Parameters: TY_API_DEV - pointer to target vars structure
               size_t - relative start offset of the memory range
               size_t - size of memory range in bytes
               enum SYNCH_DIRECTION - direction to synchronize
   Return: return 0, if synch was successful */
AiInt32 UsbSynchronizeActiveMemoryBlocks(TY_API_DEV* p_api_dev, size_t RelativeOffset, 
                                         size_t Size, enum SYNCH_DIRECTION direction);

/* Checks if part of a given memory range lies in BIU specific System Control Block.
   If this is the case and the corresponding BIU is active, the memory will be synchronized between
   global memory and its mirror
   Parameters: TY_API_DEV - pointer to target vars structure
               size_t - relative start of memory range to check
               size_t - size in Bytes of memory range to check
               enum SYNCH_DIRECTION - direction to synchronize
               unsigned char* - number of remaining memory intervals that lie not in SCB. (At most 2)
               size_t[2] - start offsets of remaining memory intervals
               size_t[2] - sizes of remaining memory intervals
   Return: return 0, if part of memory range lay in SCB */
AiInt32 UsbSynchronizeActiveBiuMemory(TY_API_DEV* p_api_dev, size_t RelativeOffset, size_t Size, 
                                      enum SYNCH_DIRECTION direction, 
                                      unsigned char* puc_NumRemainingIntervals, 
                                      size_t RemainingStartOffsets[2], size_t RemainingSizes[2]);

/* Checks if part of a given memory range lies in BIU specific interrupt loglist.
   If this is the case and the corresponding BIU is active, the memory will be synchronized between
   global memory and its mirror
   Parameters: TY_API_DEV - pointer to target vars structure
               size_t - relative start of memory range to check
               size_t - size in Bytes of memory range to check
               enum SYNCH_DIRECTION - direction to synchronize
               unsigned char* - number of remaining memory intervals that lie not in 
                                interrupt loglist. (At most 2)
               size_t[2] - start offsets of remaining memory intervals
               size_t[2] - sizes of remaining memory intervals
   Return: return 0, if part of memory range lay in interrupt loglist */
AiInt32 UsbSynchronizeActiveInterruptLoglistMemory(TY_API_DEV* p_api_dev, size_t RelativeOffset, 
                                                   size_t Size, enum SYNCH_DIRECTION direction, 
                                                   unsigned char* puc_NumRemainingIntervals, 
                                                   size_t RemainingStartOffsets[2], 
                                                   size_t RemainingSizes[2]);

/* Checks if part of a given memory range lies in monitor activity/filtering page.
   If this is the case and the monitor is active, the memory will be synchronized between
   global memory and its mirror
   Parameters: TY_API_DEV - pointer to target vars structure
               size_t - relative start of memory range to check
               size_t - size in Bytes of memory range to check
               enum SYNCH_DIRECTION - direction to synchronize
               unsigned char* - number of remaining memory intervals that lie not 
                                in activity page. (At most 2)
               size_t[2] - start offsets of remaining memory intervals
               size_t[2] - sizes of remaining memory intervals
   Return: return 0, if part of memory range lay in activity page */
AiInt32 UsbSynchronizeActiveMonitorActivityMemory(TY_API_DEV* p_api_dev, size_t RelativeOffset, 
                                                  size_t Size, enum SYNCH_DIRECTION direction, 
                                                  unsigned char* puc_NumRemainingIntervals, 
                                                  size_t RemainingStartOffsets[2], 
                                                  size_t RemainingSizes[2]);

/* Checks if part of a given memory range lies in RT buffer header area.
   If this is the case and the corresponding RT is active, the memory will be synchronized between
   global memory and its mirror
   Parameters: TY_API_DEV - pointer to target vars structure
               size_t - relative start of memory range to check
               size_t - size in Bytes of memory range to check
               enum SYNCH_DIRECTION - direction to synchronize
               unsigned char* - number of remaining memory intervals that lie not in 
                                RT buffer header area. (At most 2)
               size_t[2] - start offsets of remaining memory intervals
               size_t[2] - sizes of remaining memory intervals
   Return: return 0, if part of memory range lay in RT buffer header area */
AiInt32 UsbSynchronizeActiveRTBufferHeaderMemory(TY_API_DEV* p_api_dev, size_t RelativeOffset, 
                                                 size_t Size, enum SYNCH_DIRECTION direction, 
                                                 unsigned char* puc_NumRemainingIntervals, 
                                                 size_t RemainingStartOffsets[2], 
                                                 size_t RemainingSizes[2]);

/* Checks if part of a given memory range lies in RT status queue area.
   If this is the case and the corresponding RT is active, the memory will be synchronized between
   global memory and its mirror
   Parameters: TY_API_DEV - pointer to target vars structure
               size_t - relative start of memory range to check
               size_t - size in Bytes of memory range to check
               enum SYNCH_DIRECTION - direction to synchronize
               unsigned char* - number of remaining memory intervals that lie not in 
                                RT status queue area. (At most 2)
               size_t[2] - start offsets of remaining memory intervals
               size_t[2] - sizes of remaining memory intervals
   Return: return 0, if part of memory range lay in RT status queue area */
AiInt32 UsbSynchronizeActiveRTStatusQueueMemory(TY_API_DEV* p_api_dev, size_t RelativeOffset, 
                                                size_t Size, enum SYNCH_DIRECTION direction, 
                                                unsigned char* puc_NumRemainingIntervals, 
                                                size_t RemainingStartOffsets[2], 
                                                size_t RemainingSizes[2]);

/* Checks if part of a given memory range lies in RT event queue area.
   If this is the case and the corresponding RT is active, the memory will be synchronized between
   global memory and its mirror
   Parameters: TY_API_DEV - pointer to target vars structure
               size_t - relative start of memory range to check
               size_t - size in Bytes of memory range to check
               enum SYNCH_DIRECTION - direction to synchronize
               unsigned char* - number of remaining memory intervals that lie not in 
                                RT event queue area. (At most 2)
               size_t[2] - start offsets of remaining memory intervals
               size_t[2] - sizes of remaining memory intervals
   Return: return 0, if part of memory range lay in RT event queue area */
AiInt32 UsbSynchronizeActiveRTEventQueueMemory(TY_API_DEV* p_api_dev, size_t RelativeOffset, 
                                               size_t Size, enum SYNCH_DIRECTION direction, 
                                               unsigned char* puc_NumRemainingIntervals, 
                                               size_t RemainingStartOffsets[2], 
                                               size_t RemainingSizes[2]);

/* Checks if part of a given memory range lies in BC buffer header area.
   If this is the case and the corresponding BC is active, the memory will be synchronized between
   global memory and its mirror
   Parameters: TY_API_DEV - pointer to target vars structure
               size_t - relative start of memory range to check
               size_t - size in Bytes of memory range to check
               enum SYNCH_DIRECTION - direction to synchronize
               unsigned char* - number of remaining memory intervals that lie not in 
                                BC buffer header area. (At most 2)
               size_t[2] - start offsets of remaining memory intervals
               size_t[2] - sizes of remaining memory intervals
   Return: return 0, if part of memory range lay in BC buffer header area */
AiInt32 UsbSynchronizeActiveBCBufferHeaderMemory(TY_API_DEV* p_api_dev, size_t RelativeOffset, 
                                                 size_t Size, enum SYNCH_DIRECTION direction, 
                                                 unsigned char* puc_NumRemainingIntervals, 
                                                 size_t RemainingStartOffsets[2], 
                                                 size_t RemainingSizes[2]);

/* Checks if part of a given memory range lies in BC status queue area.
   If this is the case and the corresponding BC is active, the memory will be synchronized between
   global memory and its mirror
   Parameters: TY_API_DEV - pointer to target vars structure
               size_t - relative start of memory range to check
               size_t - size in Bytes of memory range to check
               enum SYNCH_DIRECTION - direction to synchronize
               unsigned char* - number of remaining memory intervals that lie not in 
                                BC status queue area. (At most 2)
               size_t[2] - start offsets of remaining memory intervals
               size_t[2] - sizes of remaining memory intervals
   Return: return 0, if part of memory range lay in BC status queue area */
AiInt32 UsbSynchronizeActiveBCStatusQueueMemory(TY_API_DEV* p_api_dev, size_t RelativeOffset, 
                                                size_t Size, enum SYNCH_DIRECTION direction, 
                                                unsigned char* puc_NumRemainingIntervals, 
                                                size_t RemainingStartOffsets[2], 
                                                size_t RemainingSizes[2]);

/* Checks if part of a given memory range lies in BC event queue area.
   If this is the case and the corresponding BC is active, the memory will be synchronized between
   global memory and its mirror
   Parameters: TY_API_DEV - pointer to target vars structure
               size_t - relative start of memory range to check
               size_t - size in Bytes of memory range to check
               enum SYNCH_DIRECTION - direction to synchronize
               unsigned char* - number of remaining memory intervals that lie not in 
                                BC event queue area. (At most 2)
               size_t[2] - start offsets of remaining memory intervals
               size_t[2] - sizes of remaining memory intervals
   Return: return 0, if part of memory range lay in BC event queue area */
AiInt32 UsbSynchronizeActiveBCEventQueueMemory(TY_API_DEV* p_api_dev, size_t RelativeOffset, 
                                               size_t Size, enum SYNCH_DIRECTION direction, 
                                               unsigned char* puc_NumRemainingIntervals, 
                                               size_t RemainingStartOffsets[2], 
                                               size_t RemainingSizes[2]);

/* Checks if part of a given memory range lies in monitor buffer area.
   If this is the case and the corresponding monitor is active, the memory will be synchronized between
   global memory and its mirror
   Parameters: TY_API_DEV - pointer to target vars structure
               size_t - relative start of memory range to check
               size_t - size in Bytes of memory range to check
               enum SYNCH_DIRECTION - direction to synchronize
               unsigned char* - number of remaining memory intervals that lie not in 
                                monitor buffer area. (At most 2)
               size_t[2] - start offsets of remaining memory intervals
               size_t[2] - sizes of remaining memory intervals
   Return: return 0, if part of memory range lay in BC event queue area */
AiInt32 UsbSynchronizeActiveMonitorBufferMemory(TY_API_DEV* p_api_dev, size_t RelativeOffset, 
                                                size_t Size, enum SYNCH_DIRECTION direction, 
                                                unsigned char* puc_NumRemainingIntervals, 
                                                size_t RemainingStartOffsets[2], 
                                                size_t RemainingSizes[2]);

/* Checks if part of a given memory range lies in simulation buffer area.
   If this is the case and the corresponding BC or RT is active, the memory will be synchronized between
   global memory and its mirror
   Parameters: TY_API_DEV - pointer to target vars structure
               size_t - relative start of memory range to check
               size_t - size in Bytes of memory range to check
               enum SYNCH_DIRECTION - direction to synchronize
               unsigned char* - number of remaining memory intervals that lie not in 
                                simulation buffer area. (At most 2)
               size_t[2] - start offsets of remaining memory intervals
               size_t[2] - sizes of remaining memory intervals
   Return: return 0, if part of memory range lay in simulation buffer area */
AiInt32 UsbSynchronizeActiveSimulationBufferMemory(TY_API_DEV* p_api_dev, size_t RelativeOffset, 
                                                   size_t Size, enum SYNCH_DIRECTION direction, 
                                                   unsigned char* puc_NumRemainingIntervals, 
                                                   size_t RemainingStartOffsets[2], 
                                                   size_t RemainingSizes[2]);

/* Synchronizes the BC misc block of a specific BIU between global memory and its mirror
   Parameters: TY_API_DEV - pointer to target vars structure
               unsigned char - ID of BIU
               enum SYNCH_DIRECTION - direction to synchronize
   Return: return 0, if synch was successful */
AiInt32 UsbSynchronizeBCMiscBlock(TY_API_DEV* p_api_dev, short uc_Biu, 
                                  enum SYNCH_DIRECTION direction);

/* Synchronizes one longword from the board's global memory and returns the synchronized value.
   Parameters: TY_API_DEV - pointer to target vars structure
               void* - pointer to longword in global memory mirror
   Return: returns the synchronized value */
unsigned int UsbSynchAndReadLWord(TY_API_DEV* p_api_dev, void* p_AbsoluteAddress);





/******************************************************************************
*
* Function from UsbListSyncWin.h
*
*****************************************************************************/



/* struct that defines the transfer context for a transfer list entry */
typedef struct _TRANSFER_CONTEXT
{
    unsigned short us_TransferID;
    struct ty_api_bc_xfer_desc* p_TransferDescriptor;
    struct ai_list_head list;
}TRANSFER_CONTEXT, *PTRANSFER_CONTEXT;


/* context for an RT buffer in the RT buffer list */
typedef struct _RT_BUFFER_CONTEXT
{
    unsigned short us_HeaderID;
    struct ty_api_rt_bh_desc* p_BufferHeader;
    struct ai_list_head list;
}RT_BUFFER_CONTEXT, *PRT_BUFFER_CONTEXT;



/* Synchronizes all transfers, that are in the transfer list, between global memory and its mirror
Parameters: TY_API_DEV - pointer to target vars structure
unsigned char - ID of BIU to synchronize on
enum SYNCH_DIRECTION - direction to synchronize
Return: return 0, if synch was successful */
AiInt32 UsbSynchronizeTransfers(TY_API_DEV* p_api_dev, short uc_Biu,
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
struct ty_api_bc_xfer_desc* p_TransferDescriptor, AiBoolean b_SynchOut);


/* Synchronizes one specific transfer determined by its transfer ID
between global memory and its mirror. The transfer list of the given BIU will be searched for
the transfer.
Parameters: TY_API_DEV - pointer to target vars structure
unsigned char - ID of BIU the transfer belongs to
unsigned short - ID of transfer to synchronize
AiBoolean - if true, the buffer data of the transfer will also be synchronized
enum SYNCH_DIRECTION - direction to synchronize
Return: return 0, if synch was successful */
AiInt32 UsbSynchronizeTransferByID(TY_API_DEV* p_api_dev, short uc_Biu,
    unsigned short us_TransferID, AiBoolean b_IncludeBufferData,
enum SYNCH_DIRECTION direction);


/* Synchronizes one specific transfer between global memory and its mirror
Parameters: TY_API_DEV - pointer to target vars structure
WDFOBJECT - Transfer object to synchronize
AiBoolean - if true, the buffer data of the transfer will also be synchronized
enum SYNCH_DIRECTION - direction to synchronize
Return: return 0, if synch was successful */
AiInt32 UsbSynchronizeTransfer(TY_API_DEV* p_api_dev, PTRANSFER_CONTEXT p_TransferContext,
    AiBoolean b_IncludeBufferData, enum SYNCH_DIRECTION direction);


/* Synchronizes one RT buffer between global memory and its mirror
Parameters: TY_API_DEV - pointer to target vars structure
PRT_BUFFER_CONTEXT  - RT buffer to synchronize
enum SYNCH_DIRECTION - direction to synchronize
Return: return 0, if synch was successful */
AiInt32 UsbSynchronizeRTBuffer(TY_API_DEV* p_api_dev, PRT_BUFFER_CONTEXT p_BufferContext,
enum SYNCH_DIRECTION direction);


#endif

