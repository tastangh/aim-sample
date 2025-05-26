/* SPDX-FileCopyrightText: 2015-2024 AIM GmbH <info@aim-online.com> */
/* SPDX-License-Identifier: MIT */

#ifndef __MIL_UTIL_H__
#define __MIL_UTIL_H__

#include "AiOs.h"
#include "Api1553.h"



/* -- Defines --- */
/*   Buffer Id
     Buffer Ids are shared between BC and RT simulation.
     This means that the same physical buffer can be used
     for RT and BC transmission. For most cases this is however
     not neccessary. To avoid id conflicts we start rt buffers at
     index BID_OFFSET_RT. The maximum number of available buffers
     depends on the hardware. This value might need to be adjusted
     to fit the requirements and hardware conditions.
     */
#define BID_OFFSET_BC 0
#define BID_OFFSET_RT 1024


/* Reset ids */
void MilUtilResetIds();
/* BC buffer header ids are uniqe for each stream */
AiUInt32 MilUtilNextBCHid();
/* RT buffer header ids are uniqe for each stream */
AiUInt32 MilUtilNextRTHid();
/* Buffer Ids are shared between RT,BC and between all streams on one module */
AiUInt32 MilUtilNextBid(AiUInt32 ulModHandle, AiUInt32 qSize);


/* Use the AIM_ERROR_MESSAGE_x defines to handle return values directly returned by AIM API functions */
/* Print the error code and the corresponding AIM error message */
#define AIM_ERROR_MESSAGE( x ) if( x != 0 ){ \
                                    fprintf( stderr, "\n"); \
                                    fprintf( stderr, "ERROR: %s (0x%04X)\n", ApiGetErrorMessage(x), x ); \
                                    fprintf( stderr, "%s:%d\n", __FILE__, __LINE__ ); \
                                    fprintf( stderr, "\n"); \
                              };

#define AIM_ERROR_MESSAGE_RETURN( x ) if( x != 0 ){ \
                                                    AIM_ERROR_MESSAGE( x ); \
                                                    return x; \
                                                  };

#define AIM_ERROR_MESSAGE_BREAK( x ) if( x != 0 ){ \
                                                    AIM_ERROR_MESSAGE( x ); \
                                                    break; \
                                                 };

/* Use this define to return on an high level sample function error  */
#define AIM_RETURN( x ) if( x != 0 ){ \
                                        return x; \
                                    };

/* Use this define to return on an high level sample function error and trace where the function call occured */
#define AIM_TRACE_RETURN( x ) if( x != 0 ){ \
                                                fprintf( stderr, "%s:%d\n", __FILE__, __LINE__ ); \
                                                return x; \
                                          };

/* Use this define to return on an high level sample function error and trace where the function call occured */
#define AIM_TRACE_BREAK( x ) if( x != 0 ){ \
                                                fprintf( stderr, "%s:%d\n", __FILE__, __LINE__ ); \
                                                break; \
                                         };




AiUInt32 MilUtilIsBcActive(AiUInt32 ulModHandle);
AiUInt32 MilUtilIsBcActiveCheckCount(AiUInt32 ulModHandle, AiUInt32 ulTransferCount);
AiReturn MilUtilConfigureHardware(AiUInt32 ulModHandle, AiUInt8 coupling);



AiReturn MilUtilEnableRT(AiUInt32 ulModHandle,
    AiUInt8 ucRt, 
    AiFloat resp_time);

AiReturn MilUtilEnableMailboxRT(AiUInt32 ulModHandle,
    AiUInt8 ucRt);

AiReturn MilUtilEnableRTSABufferQueue(AiUInt32 ulModHandle,
    AiUInt8  ucRt,
    AiUInt8  ucSa,
    AiUInt8  ucType,
    AiUInt16 uwHeaderId,
    AiUInt8  ucQueueSize);

AiReturn MilUtilEnableRTSA(AiUInt32 ulModHandle,
    AiUInt8 ucRt,
    AiUInt8 ucSa,
    AiUInt8 ucType,
    AiUInt16 uwHeaderId);

AiReturn MilUtilBCCreateTransfer(AiUInt32 ulModHandle,
    AiUInt16  uwTransferId,
    AiUInt8   ucType,
    AiUInt8   ucTransmitRt,
    AiUInt8   ucTransmitSa,
    AiUInt8   ucReceiveRt,
    AiUInt8   ucReceiveSa,
    AiUInt8   ucWordCount,
    AiUInt16  uwHeaderId,
    AiUInt8   ucQueueSize,
    AiUInt32  ulInterruptControl);

AiReturn MilUtilBCCreateTransferWithGap(AiUInt32 ulModHandle,
    AiUInt16  uwTransferId,
    AiUInt8   ucType,
    AiUInt8   ucTransmitRt,
    AiUInt8   ucTransmitSa,
    AiUInt8   ucReceiveRt,
    AiUInt8   ucReceiveSa,
    AiUInt8   ucWordCount,
    AiUInt16  uwHeaderId,
    AiUInt8   ucQueueSize,
    AiUInt32  ulInterruptControl,
    AiUInt8   gap_mode,
    AiUInt16  gap);

AiReturn MilUtilBCCreateSingleFrameFraming(AiUInt32 ulModHandle, AiUInt16 aulTransferIds[], AiUInt32 ulTransferIdsCount);


AiReturn MilUtilSetData(AiUInt32 ulModHandle,
    AiUInt16  uwHeaderId,
    AiUInt8   ucBufferType,
    AiUInt16  uwIndex,
    AiUInt8   ucWordCount,
    AiUInt16  auwMessageData[],
    AiUInt32  *ulOutAddr);

AiReturn MilUtilGetData(AiUInt32 ulModHandle,
    AiUInt16  uwHeaderId,
    AiUInt8   ucBufferType,
    AiUInt16  uwBufferId,
    AiUInt8   ucWordCount,
    AiUInt16  auwMessageData[]);


#define MIL_UTIL_BM_INIT_ITERATOR( iterator, start, size ) { (iterator)->iterator_position = (AiUInt32*)start; (iterator)->buffer_end = (AiUInt32*)((AiUInt8*)start + size); }

typedef struct mil_util_bm_iterator
{
    AiUInt32 * iterator_position;
    AiUInt32 * buffer_end;
} MIL_UTIL_BM_ITERATOR;


AiReturn MilUtilBMLsInterpretData(AiUInt32 ulModHandle, MIL_UTIL_BM_ITERATOR * pIterator, TY_API_QUEUE_BUF_EX * pInterpreted);

AiReturn MilUtilBMLsSaveInterpretedData(AiUInt32 ulModHandle, FILE* pFileBm, TY_API_QUEUE_BUF_EX* pInterpreted);

AiReturn MilUtilBMLsQueueBufShow(AiUInt32 ulModHandle, TY_API_QUEUE_BUF_EX * pInterpreted);

AiUInt32 MilUtilBMGetLogicalBiuFromStream(AiUInt32 ulModHandle, AiUInt32 ulStream, AiUInt32 ulHsBiu);

AiReturn MilUtilBMLsShowData(AiUInt32 ulModHandle, MIL_UTIL_BM_ITERATOR * pIterator);

#endif /* __MIL_UTIL_H__ */







