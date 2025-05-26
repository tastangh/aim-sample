/* SPDX-FileCopyrightText: 2001-2022 AIM GmbH <info@aim-online.com> */
/* SPDX-License-Identifier: MIT */

/*! \file Api1553Cvi.h
 *
 *  CVI Wrapper DLL for 3910 Application Interface. Header file with function prototypes
 *
 */
#ifndef CVI_3910
#define CVI_3910

#include "Api1553Cvi.h"

//***************************************************************************
//  API3910 Functions - External References
//***************************************************************************

//###########################################################################
//    API3910 System Commands
//###########################################################################
AI_LIB_FUNC AiReturn AI_CALL_CONV Api3910CmdHsResetCvi(AiUInt32 bModule, AiUInt8 uc_ResCtrl,
                                AiUInt8 uc_MBufSize,
  AiUInt8        *uc_LsMBufs,
  AiUInt8        *uc_LsSBufs,
  AiUInt32    *ul_LsMonAddr,
  AiUInt32    *ul_LsSimAddr,
  AiUInt8        *uc_HsMBufs,
  AiUInt8        *uc_HsSBufs,
  AiUInt32    *ul_HsMonAddr,
  AiUInt32    *ul_HsSimAddr);
AI_LIB_FUNC AiReturn AI_CALL_CONV Api3910CmdHsResetInfoCvi(AiUInt32 bModule,
  AiUInt8        *uc_LsMBufs,
  AiUInt8        *uc_LsSBufs,
  AiUInt32    *ul_LsMonAddr,
  AiUInt32    *ul_LsSimAddr,
  AiUInt8        *uc_HsMBufs,
  AiUInt8        *uc_HsSBufs,
  AiUInt32    *ul_HsMonAddr,
  AiUInt32    *ul_HsSimAddr);
AI_LIB_FUNC AiReturn AI_CALL_CONV Api3910CmdHsRxInitTimeOutDefCvi(AiUInt32 bModule,
  AiUInt16 uw_TimeOut);
AI_LIB_FUNC AiReturn AI_CALL_CONV Api3910CmdHsSystagDefCvi(AiUInt32 bModule,
                                  AiUInt8 uc_Id, AiUInt8 uc_Con, AiUInt8 uc_Mode,
    AiUInt16 uw_XidRtMid,
    AiUInt16 uw_Fct,
    AiUInt16 uw_Min,
    AiUInt16 uw_Max,
    AiUInt16 uw_Step,
    AiUInt16 uw_WPos,
    AiUInt16 uw_BPos);
AI_LIB_FUNC AiReturn AI_CALL_CONV Api3910CmdHsSystagConCvi(AiUInt32 bModule,
                                  AiUInt8 uc_Id, AiUInt8 uc_Con);

AI_LIB_FUNC AiReturn AI_CALL_CONV Api3910HsReadRecDataCvi(AiUInt32 bModule,
    AiUInt8  *uc_Status,
    AiUInt8  *uc_Padding1,
    AiUInt16 *uw_Padding2,
    AiUInt32 *ul_HfiCnt,
    AiUInt32 *ul_SAddr,
    AiUInt32 *ul_Size,
    void *lpBuf,                                               // changed from LPVOID MR 31.10.00
    AiUInt32 *pul_BytesRead);
AI_LIB_FUNC AiReturn AI_CALL_CONV Api3910HsWriteRepDataCvi(AiUInt32 bModule,
    AiUInt8  *uc_Status,
    AiUInt8  *uc_Padding1,
    AiUInt16 *uw_Padding2,
    AiUInt32 *ul_RpiCnt,
    AiUInt32 ul_SAddr,
    AiUInt32 ul_Size,
    AiUInt32 *ul_MsgCnt,
    AiAddr pl_Buf,
    AiUInt32 *pul_BytesWritten);
AI_LIB_FUNC AiReturn AI_CALL_CONV Api3910CmdHsTrackDefCvi(AiUInt32 bModule, AiUInt8 uc_TrackId, AiUInt8 uc_BT,
                                        AiUInt16 uw_XidRtMid, AiUInt8 uc_Mode, AiUInt16 uw_TrackStartPos,
                                        AiUInt8 uc_TrackBPos, AiUInt8 uc_TrackBLen, AiUInt16 uw_TrackLen,
                                        AiUInt16 uw_MultiplexedTrackNb, AiUInt16 uw_ContinousTrackNb,
                                        AiUInt32 *pul_TrackBufferStartAddr);
AI_LIB_FUNC AiReturn AI_CALL_CONV Api3910CmdHsTrackDefExCvi(AiUInt32 bModule, AiUInt32 ul_TrackId,
                                        AiUInt32 ul_BT,
                                        AiUInt32 ul_XidRtMid, AiUInt32 ul_Mode, AiUInt32 ul_TrackStartPos,
                                        AiUInt32 ul_TrackBPos, AiUInt32 ul_TrackBLen, AiUInt32 ul_TrackLen,
                                        AiUInt32 ul_MultiplexedTrackNb, AiUInt32 ul_ContinousTrackNb,
                                        AiInt32 l_Offset, AiUInt32 *pul_TrackBufferStartAddr);
AI_LIB_FUNC AiReturn AI_CALL_CONV Api3910CmdHsTrackReadCvi(AiUInt32 bModule, AiUInt8 uc_TrackId, AiUInt16 uw_MultiplexedTrackIndex,
                                         AiUInt8 uc_ReadMode, AiUInt8 *puc_DataValid, AiUInt16 *puw_TrackDataWords);
AI_LIB_FUNC AiReturn AI_CALL_CONV Api3910CmdHsTrackReadExCvi(AiUInt32 bModule, AiUInt32 ul_TrackId,
                                         AiUInt32 ul_MultiplexedTrackIndex,
                                         AiUInt32 ul_ReadMode, AiUInt32 *pul_DataValid, AiUInt32 *pul_LastTT,
                                         AiUInt16 *puw_TrackDataWords);
AI_LIB_FUNC AiReturn AI_CALL_CONV Api3910CmdHsTrackScanCvi(AiUInt32 bModule, AiUInt32 ul_TrackId,
                                         AiUInt32 ul_Chunk, AiUInt32 ul_ChunkSize, AiUInt32 *pul_NumberOfReturnedStates,
                                         AiUInt32 *pul_MoreData,
                                         AiUInt32 *pul_ReturnedStates);

//###############################################################
//    API3910 Buffer Commands
//###############################################################

AI_LIB_FUNC AiReturn AI_CALL_CONV Api3910CmdHsBufDefCvi(AiUInt32 bModule, /*AiUInt8 uc_AllocMode,*/
    AiUInt8 uc_BufType, AiUInt16 uw_HeaderId,
    AiUInt16 uw_BufferId, AiUInt16 uw_BufSize,
    AiUInt16 uw_WStartIndex, AiUInt16 uw_WriteSize,
    AiUInt16 *puw_Data, AiUInt16 *puw_AllocBufNr,
    AiUInt16 *puw_AllocInfo, AiUInt32 *pul_BufAddr);
AI_LIB_FUNC AiReturn AI_CALL_CONV Api3910CmdHsBufHostControlCvi(AiUInt32 bModule,
    AiUInt8 uc_BufType,
    AiUInt16 uw_HeaderId,
    AiUInt32 ul_Con);
AI_LIB_FUNC AiReturn AI_CALL_CONV Api3910CmdHsBufReadCvi(AiUInt32 bModule, AiUInt8 uc_BufType,
    AiUInt16 uw_HeaderId, AiUInt16 uw_BufferId,
    AiUInt16 uw_RStartIndex, AiUInt16 uw_ReadSize,
    AiUInt16 *puw_BufNr, AiUInt32 *pul_BufAddr,
    AiUInt16 *puw_Data );
AI_LIB_FUNC AiReturn AI_CALL_CONV Api3910CmdHsBufWriteCvi(AiUInt32 bModule, AiUInt8 uc_BufType, AiUInt16 uw_HeaderId,
    AiUInt16 uw_BufferId, AiUInt16 uw_DataPos, AiUInt16 uw_DataWordMask, AiUInt16 uw_DataWord);
AI_LIB_FUNC AiReturn AI_CALL_CONV Api3910CmdHsBufWriteBlockCvi(AiUInt32 bModule,
    AiUInt8 uc_BufType, AiUInt16 uw_HeaderId,
    AiUInt16 uw_BufferId, AiUInt16 uw_WStartIndex,
    AiUInt16 uw_WriteSize, AiUInt16* puw_Data);
AI_LIB_FUNC AiReturn AI_CALL_CONV ApiBHModifyCvi(AiUInt32 bModule, AiUInt32 ul_BHAddr, AiUInt32 ul_DataBufferStoreMode, AiUInt32 ul_WordCountMode,
                                                      AiUInt32 ul_BufferSize, AiUInt32 ul_BufferQueueMode, AiUInt32 ul_BufferQueueSize,
                                                      AiUInt32 ul_StatusQueueSize, AiUInt32 ul_EventQueueSize, AiUInt32 ul_CurrentBufferIndex);

//###############################################################
//    API3910 BC Commands
//###############################################################

AI_LIB_FUNC AiReturn AI_CALL_CONV Api3910CmdHsBCBHDefCvi(AiUInt32 bModule, AiUInt16 uw_HeaderId,
    AiUInt16 uw_Res, AiUInt16 uw_SqId,     // changed 08.01.01 pg/mr
    AiUInt16 uw_EqId, AiUInt8 uc_QSize,    // changed 08.01.01 pg/mr
    AiUInt8 uc_Bqm, AiUInt8 uc_Bsm,
    AiUInt8 uc_Sqm, AiUInt8 uc_Eqm,        // changed 08.01.01 pg/mr
    AiUInt8 uc_Dbm, AiUInt8 uc_Wcm);                       // changed 08.01.01 pg/mr
AI_LIB_FUNC AiReturn AI_CALL_CONV Api3910CmdHsBCBHReadCvi(AiUInt32 bModule, AiUInt16 uw_HeaderId,
    AiUInt16 *uw_SqId,                     // changed 08.01.01 pg/mr
    AiUInt16 *uw_EqId,                     // changed 08.01.01 pg/mr
    AiUInt16 *uw_NBufs,
    AiUInt32 *ul_Mem,
    AiUInt32 *ul_HIdAddr,
    AiUInt32 *ul_BqAddr,
    AiUInt32 *ul_SqAddr,
    AiUInt32 *ul_EqAddr,
    AiUInt32 *ul_HeaderId2,
    AiUInt32 *ul_LsHIdAddr1,
    AiUInt32 *ul_LsBqAddr1,
    AiUInt32 *ul_LsSqAddr1,
    AiUInt32 *ul_LsEqAddr1,
    AiUInt32 *ul_LsHIdAddr2,
    AiUInt32 *ul_LsBqAddr2,
    AiUInt32 *ul_LsSqAddr2,
    AiUInt32 *ul_LsEqAddr2);
// AJR 2 new functions to replace original Api3910CmdHsBCXferDefCvi
AI_LIB_FUNC AiReturn AI_CALL_CONV Api3910CmdHsBCXferDefCvi(AiUInt32 bModule,
        AiUInt16    uw_XferId,
        AiUInt16    uw_HeaderId,
        AiUInt8    uc_XferType,
        AiUInt8    uc_XferHalt,
        AiUInt8    uc_MsgSize,
        AiUInt8    uc_Padding1,
        AiUInt16    uw_Rt,
        AiUInt16    uw_Mid,
        AiUInt8    uc_StatusWordException,
        AiUInt8    uc_RetryControl,
        AiUInt16    uw_Padding2,
        AiUInt32    ul_FirstXferGap,
        AiUInt32    ul_SecondXferGap,
        AiUInt32    ul_LsChnIr,
        AiUInt8    uc_LsErrType,
        AiUInt8    uc_LsErrSync,
        AiUInt8    uc_LsErrContig,
        AiUInt8    uc_LsErrPadding1,
        AiUInt32    ul_LsErrSpec,
        AiUInt32    ul_HsChnIr,
        AiUInt32    ul_HsXmt,
        AiUInt8    uc_HsErrType,
        AiUInt8    uc_HsPadding1,
        AiUInt16    uw_HsPadding2,
        AiUInt32    ul_HsErrSpec);
AI_LIB_FUNC AiReturn AI_CALL_CONV Api3910CmdHsBCXferDefErrCvi(AiUInt32 ulModule,
        AiUInt16    uw_XferId,
        AiUInt8        uc_HsErrType,
        AiUInt8        uc_HsPadding1,
        AiUInt16    uw_HsPadding2,
        AiUInt32    ul_HsErrSpec);
AI_LIB_FUNC AiReturn AI_CALL_CONV Api3910CmdHsBCXferDefExCvi(AiUInt32 bModule,
        AiUInt16    uw_XferId,            AiUInt16    uw_HeaderId,        AiUInt8      uc_XferType,
        AiUInt8        uc_XferHalt,        AiUInt8        uc_MsgSize,            AiUInt8      uc_Padding1,
        AiUInt16    uw_Rt,                AiUInt16    uw_Mid,                AiUInt8      uc_StatusWordException,
        AiUInt8        uc_RetryControl,    AiUInt16    uw_Padding2,        AiUInt32    ul_FirstXferGap,
        AiUInt32    ul_SecondXferGap,    AiUInt32    ul_LsChnIr,            AiUInt8      uc_LsErrType,
        AiUInt8        uc_LsErrSync,        AiUInt8        uc_LsErrContig,          AiUInt8      uc_LsErrPadding1,
        AiUInt32    ul_LsErrSpec,        AiUInt32    ul_HsChnIr,            AiUInt32    ul_HsXmt,
        AiUInt8        uc_HsErrType,        AiUInt8        uc_HsPadding1,        AiUInt16    uw_HsPadding2,
        AiUInt32    ul_HsErrSpec,        AiUInt32 *pul_LsXferDescAddr1,    AiUInt32 *pul_LsXferDescAddr2,
        AiUInt32    *pul_HsXferDescAddr );

AI_LIB_FUNC AiReturn AI_CALL_CONV Api3910CmdHsBCGetXferDefCvi (AiUInt32 Module, AiUInt16 uw_XferId,
        AiUInt16    *puw_HeaderId,
        AiUInt8    *puc_XferType,
        AiUInt8    *puc_XferHalt,
        AiUInt8    *puc_MsgSize,
        AiUInt8    *puc_Padding1,
        AiUInt16    *puw_Rt,
        AiUInt16    *puw_Mid,
        AiUInt8    *puc_StatusWordException,
        AiUInt8    *puc_RetryControl,
        AiUInt16    *puw_Padding2,
        AiUInt32    *pul_FirstXferGap,
        AiUInt32    *pul_SecondXferGap,
        AiUInt32    *pul_LsChnIr,
        AiUInt8    *puc_LsErrType,
        AiUInt8    *puc_LsErrSync,
        AiUInt8    *puc_LsErrContig,
        AiUInt8    *puc_LsErrPadding1,
        AiUInt32    *pul_LsErrSpec,
        AiUInt32    *pul_HsChnIr,
        AiUInt32    *pul_HsXmt,
        AiUInt8    *puc_HsErrType,
        AiUInt8    *puc_HsPadding1,
        AiUInt16    *puw_HsPadding2,
        AiUInt32    *pul_HsErrSpec
);

AI_LIB_FUNC AiReturn AI_CALL_CONV Api3910CmdHsBCGetXferDefExCvi (AiUInt32 Module, AiUInt16 uw_XferId,
        AiUInt16    *puw_HeaderId,
        AiUInt8    *puc_XferType,
        AiUInt8    *puc_XferHalt,
        AiUInt8    *puc_MsgSize,
        AiUInt8    *puc_Padding1,
        AiUInt16    *puw_Rt,
        AiUInt16    *puw_Mid,
        AiUInt8    *puc_StatusWordException,
        AiUInt8    *puc_RetryControl,
        AiUInt16    *puw_Padding2,
        AiUInt32    *pul_FirstXferGap,
        AiUInt32    *pul_SecondXferGap,
        AiUInt32    *pul_LsChnIr,
        AiUInt8    *puc_LsErrType,
        AiUInt8    *puc_LsErrSync,
        AiUInt8    *puc_LsErrContig,
        AiUInt8    *puc_LsErrPadding1,
        AiUInt32    *pul_LsErrSpec,
        AiUInt32    *pul_HsChnIr,
        AiUInt32    *pul_HsXmt,
        AiUInt8    *puc_HsErrType,
        AiUInt8    *puc_HsPadding1,
        AiUInt16    *puw_HsPadding2,
        AiUInt32    *pul_HsErrSpec,
        AiUInt32    *pul_LsXferDescAddr1,
        AiUInt32    *pul_LsXferDescAddr2,
        AiUInt32    *pul_HsXferDescAddr
);

/* MR REMOVED 31.10.2000
AI_LIB_FUNC AiReturn AI_CALL_CONV Api3910CmdHsBCErrInjCvi(AiUInt32 bModule,
        AiUInt16    uw_XferId,
        AiUInt8    uc_LsType,
        AiUInt8    uc_LsWPos,
        AiUInt8    uc_LsBPos,
        AiUInt8    uc_LsSync,
        AiUInt8    uc_LsContig,
        AiUInt8    uc_LsBcBits,
        // HS
        AiUInt8    uc_HsType,
        AiUInt8    uc_HsWPos,
        AiUInt8    uc_HsBPos,
        AiUInt8    uc_HsBcBits);
*/
/* AJR REMOVED 27-MAR-2000
AI_LIB_FUNC AiReturn AI_CALL_CONV Api3910CmdHsBCXferDefCvi(AiUInt32 bModule,
        // Common
        AiUInt16    uw_XferId,
        AiUInt8    uc_XferType,
        AiUInt8    uc_XferHalt,
        AiUInt8    uc_MsgSize,
        AiUInt8    uc_XmtRt,
        AiUInt8    uc_RcvRt,
        AiUInt8    uc_XmtMid,
        AiUInt8    uc_RcvMid,
        AiUInt16    uw_Res,
        AiUInt16    uw_FirstXferHeaderId,
        AiUInt8    uc_FirstXferGapMode,
        AiUInt16    uw_FirstXferGap,
        AiUInt16    uw_SecondXferHeaderId,
        AiUInt8    uc_SecondGapMode,
        AiUInt16    uw_SecondGap,
        // LS
        AiUInt8    uc_LsXferIr,
        AiUInt8    uc_LsChn,
        AiUInt8    uc_LsType,
        AiUInt8    uc_LsWPos,
        AiUInt8    uc_LsBPos,
        AiUInt8    uc_LsSync,
        AiUInt8    uc_LsContig,
        AiUInt8    uc_LsBcBits,
        // HS
        AiUInt8    uc_HsXferIr,
        AiUInt8    uc_HsChn,
        AiUInt16    uw_HsXmtStartDel,
        AiUInt8    uc_HsXmtNoPre,
        AiUInt8    uc_HsType,
        AiUInt8    uc_HsWPos,
        AiUInt8    uc_HsBPos,
        AiUInt8    uc_HsBcBits);
*/
AI_LIB_FUNC AiReturn AI_CALL_CONV Api3910CmdHsBCDytagDefCvi(AiUInt32 bModule, AiUInt8 uc_Con,
    AiUInt16 uw_HeaderId,
    AiUInt8  uc_DytagWordMode[4],
    AiUInt8  uc_Mode[4],
    AiUInt16 uw_FctWord[4],
    AiUInt16 uw_WPos[4]);
AI_LIB_FUNC AiReturn AI_CALL_CONV Api3910CmdHsBCGetDytagDefCvi(AiUInt32 bModule,
    AiUInt16 uw_HeaderId,
    AiUInt8  uc_DytagWordMode[4],
    AiUInt8  uc_Mode[4],
    AiUInt16 uw_FctWord[4],
    AiUInt16 uw_WPos[4]);
AI_LIB_FUNC AiReturn AI_CALL_CONV Api3910CmdHsBCXferReadCvi(AiUInt32 bModule, AiUInt16 uw_XferId,
    AiUInt16 uw_Clr,
    AiUInt16 *uw_FirstCmdWord,
    AiUInt16 *uw_FirstStatusWord,
    AiUInt16 *uw_FirstXferRepWord,
    AiUInt16 *uw_FirstActionWord,
    AiUInt32 *ul_FirstTimeTag,
    AiUInt32 *ul_FirstXferCnt,
    AiUInt32 *ul_FirstErrCnt,
    AiUInt32 *ul_FirstLastVectorWord,
    AiUInt32 *ul_FirstServiceRequestCnt,
    AiUInt16 *uw_SecondCmdWord,
    AiUInt16 *uw_SecondStatusWord,
    AiUInt16 *uw_SecondXferRepWord,
    AiUInt16 *uw_SecondActionWord,
    AiUInt32 *ul_SecondTimeTag,
    AiUInt32 *ul_SecondXferCnt,
    AiUInt32 *ul_SecondErrCnt,
    AiUInt32 *ul_SecondLastVectorWord,
    AiUInt32 *ul_SecondServiceRequestCnt,
    AiUInt16 *uw_SqDa,
    AiUInt16 *uw_SqFrmCtrlWord,
    AiUInt16 *uw_SqWCntWord,
    AiUInt16 *uw_SqMsgTagWord,
    AiUInt16 *uw_SqEntryCtrlWord,
    AiUInt32 *ul_SqActBufPtr,
    AiUInt32 *ul_SqTimeTag);
AI_LIB_FUNC AiReturn AI_CALL_CONV Api3910CmdHsBCXferReadExCvi(AiUInt32 bModule, AiUInt32 ul_XferId,
    AiUInt32 ul_Clr,
    AiUInt32 ul_Mode,
    AiUInt32 *pul_BufferQueueSize,
    AiUInt32 *pul_LsXf1_ActBufferId,
    AiUInt32 *pul_LsXf1_ActionWord,
    AiUInt32 *pul_LsXf1_XferCnt,
    AiUInt32 *pul_LsXf1_ErrCnt,
    AiUInt32 *pul_LsXf1_LastVectorWord,
    AiUInt32 *pul_LsXf1_ServiceRequestCnt,
    AiUInt32 *pul_LsXf2_ActBufferId,
    AiUInt32 *pul_LsXf2_ActionWord,
    AiUInt32 *pul_LsXf2_XferCnt,
    AiUInt32 *pul_LsXf2_ErrCnt,
    AiUInt32 *pul_LsXf2_LastVectorWord,
    AiUInt32 *pul_LsXf2_ServiceRequestCnt,
    AiUInt32 *pul_HsXf_ActBufferId,
    AiUInt32 *pul_HsXf_XferCnt,
    AiUInt32 *pul_HsXf_ErrCnt,
    AiUInt32 aul_LsXf1_SqCtrlWord[256],
    AiUInt32 aul_LsXf1_SqStatusWords[256],
    AiUInt32 aul_LsXf1_SqActBufPtr[256],
    AiUInt32 aul_LsXf1_SqTimeTag[256],
    AiUInt32 aul_LsXf2_SqCtrlWord[256],
    AiUInt32 aul_LsXf2_SqStatusWords[256],
    AiUInt32 aul_LsXf2_SqActBufPtr[256],
    AiUInt32 aul_LsXf2_SqTimeTag[256],
    AiUInt32 aul_HsXf_SqCtrlWord[256],
    AiUInt32 aul_HsXf_SqCmdActWord[256],
    AiUInt32 aul_HsXf_SqActBufPtr[256],
    AiUInt32 aul_HsXf_SqFCPADAWord[256],
    AiUInt32 aul_HsXf_SqWCntMsgTagWord[256],
    AiUInt32 aul_HsXf_SqTimeTag[256]);

AI_LIB_FUNC AiReturn AI_CALL_CONV Api3910CmdHsBCStatusReadCvi( AiUInt32 bModule, AiUInt32 *pul_GlbMsgCnt, AiUInt32 *pul_GlbErrCnt );
AI_LIB_FUNC AiReturn AI_CALL_CONV Api3910CmdHsBCModeCtrlCvi(AiUInt32 Module, AiUInt32 ul_BcMode,
                                                          AiUInt32 ul_Ctrl, AiUInt32 ul_Param1,
                                                          AiUInt32 ul_Param2, AiUInt32 ul_Param3);

//###############################################################
//    API3910 RT Commands
//###############################################################

AI_LIB_FUNC AiReturn AI_CALL_CONV Api3910CmdHsRTLSWCvi(AiUInt32 bModule, AiUInt8 uc_RtAddr,
     AiUInt16 uw_StatusWord);
AI_LIB_FUNC AiReturn AI_CALL_CONV Api3910CmdHsRTLAWCvi(AiUInt32 bModule, AiUInt8 uc_RtAddr,
     AiUInt16 uw_ActionWord);
AI_LIB_FUNC AiReturn AI_CALL_CONV Api3910CmdHsRTXmtStartDelCvi(AiUInt32 bModule, AiUInt8 uc_RtAddr,
    AiUInt8 uc_RtMode, AiUInt16 uw_StartDelay);
AI_LIB_FUNC AiReturn AI_CALL_CONV Api3910CmdHsRTXmtPreNbCvi(AiUInt32 bModule, AiUInt8 uc_RtAddr,
     AiUInt8 uc_PreambleNb );
AI_LIB_FUNC AiReturn AI_CALL_CONV Api3910CmdHsRTBHDefCvi(AiUInt32 bModule, AiUInt16 uw_HeaderId,
    AiUInt16 uw_Rsv, AiUInt16 uw_SqId,
    AiUInt16 uw_EqId, AiUInt8 uc_QSize,
    AiUInt8 uc_Bqm, AiUInt8 uc_Bsm,   // changed 08.01.01 pg/mr
    AiUInt8 uc_Sqm, AiUInt8 uc_Eqm,   // changed 08.01.01 pg/mr
    AiUInt8 uc_Dbm, AiUInt8 uc_Wcm);                  // changed 08.01.01 pg/mr
AI_LIB_FUNC AiReturn AI_CALL_CONV Api3910CmdHsRTBHReadCvi(AiUInt32 bModule, AiUInt16 uw_HeaderId,
    AiUInt32 *pul_SimMem,
    AiUInt16 *uw_SqId,
    AiUInt16 *uw_EqId,
    AiUInt16 *uw_NBufs,
    AiUInt32 *ul_BhAddr,
    AiUInt32 *ul_BqAddr,
    AiUInt32 *ul_SqAddr,
    AiUInt32 *ul_EqAddr);
AI_LIB_FUNC AiReturn AI_CALL_CONV Api3910CmdHsRTMIDDefCvi(AiUInt32 bModule, AiUInt8 uc_RtAddr,
     AiUInt8 uc_MsgId, AiUInt16 uw_HeaderId,
     AiUInt8 uc_MsgIdType, AiUInt8 uc_MsgIdCon,
     AiUInt8 uc_MsgIdInt);
AI_LIB_FUNC AiReturn AI_CALL_CONV Api3910CmdHsRTErrInjCvi(AiUInt32 bModule, AiUInt8 uc_RtAddr,
     AiUInt8 uc_MsgId, AiUInt8 uc_Type,
     AiUInt8 uc_Padding1, AiUInt16 uw_Padding2,
     AiUInt32 ul_ErrSpec);
AI_LIB_FUNC AiReturn AI_CALL_CONV Api3910CmdHsRTGetErrInjCvi(AiUInt32 bModule, AiUInt8 uc_RtAddr,
     AiUInt8 uc_MsgId, AiUInt8 *puc_Type,
     AiUInt8 *puc_Padding1, AiUInt16 *puw_Padding2,
     AiUInt32 *pul_ErrSpec);
AI_LIB_FUNC AiReturn AI_CALL_CONV Api3910CmdHsRTDytagDefCvi(AiUInt32 bModule, AiUInt8 uc_DyDatCon,
    AiUInt16 uw_HeaderId,
    AiUInt8  uc_DytagWordMode[4],
    AiUInt8  uc_Mode[4],
    AiUInt16 uw_FctWord[4],
    AiUInt16 uw_WPos[4]);
AI_LIB_FUNC AiReturn AI_CALL_CONV Api3910CmdHsRTGetDytagDefCvi(AiUInt32 bModule,
    AiUInt16 uw_HeaderId,
    AiUInt8  uc_DytagWordMode[4],
    AiUInt8  uc_Mode[4],
    AiUInt16 uw_FctWord[4],
    AiUInt16 uw_WPos[4]);
AI_LIB_FUNC AiReturn AI_CALL_CONV Api3910CmdHsRTStatusCvi(AiUInt32 bModule, AiUInt8 uc_RtAddr,
    AiUInt16 *uw_HsStatusWord,
    AiUInt16 *uw_HsActionWord,
    AiUInt32 *ul_HsMsgCnt,
    AiUInt32 *ul_HsErrCnt);
AI_LIB_FUNC AiReturn AI_CALL_CONV Api3910CmdHsRTMIDReadCvi(AiUInt32 bModule, AiUInt8 uc_RtAddr,
    AiUInt8 uc_MsgId, AiUInt8 uc_MsgIdType,
    AiUInt8 uc_ClrBufStat,
    AiUInt16 *uw_HsStatusRepWord,
    AiUInt16 *uw_HsSqDa,
    AiUInt16 *uw_HsSqFrmCtrlWord,
    AiUInt16 *uw_HsSqWCntWord,
    AiUInt16 *uw_HsSqMsgTag,
    AiUInt16 *uw_HsActionWord,
    AiUInt32 *ul_HsCurBufPtr,
    AiUInt32 *ul_HsCurTimeTag);
AI_LIB_FUNC AiReturn AI_CALL_CONV Api3910CmdHsRTMIDReadExCvi(AiUInt32 bModule, AiUInt32 ul_RtAddr,
    AiUInt32 ul_MsgId, AiUInt32 ul_MsgIdType,
    AiUInt32 ul_ClrBufStat, AiUInt32 ul_Mode,
    AiUInt32 *pul_BufferQueueSize,
    AiUInt32 *pul_LsXf_ActBufferId,
    AiUInt32 *pul_LsXf_ActionWord,
    AiUInt32 *pul_LsXf_XferCnt,
    AiUInt32 *pul_LsXf_ErrCnt,
    AiUInt32 *pul_HsXf_ActBufferId,
    AiUInt32 *pul_HsXf_XferCnt,
    AiUInt32 *pul_HsXf_ErrCnt,
    AiUInt32 aul_LsXf_SqCtrlWord[256],
    AiUInt32 aul_LsXf_SqStatusWords[256],
    AiUInt32 aul_LsXf_SqActBufPtr[256],
    AiUInt32 aul_LsXf_SqTimeTag[256],
    AiUInt32 aul_HsXf_SqCtrlWord[256],
    AiUInt32 aul_HsXf_SqCmdActWord[256],
    AiUInt32 aul_HsXf_SqActBufPtr[256],
    AiUInt32 aul_HsXf_SqFCPADAWord[256],
    AiUInt32 aul_HsXf_SqWCntMsgTagWord[256],
    AiUInt32 aul_HsXf_SqTimeTag[256]);
AI_LIB_FUNC AiReturn AI_CALL_CONV Api3910CmdHsRTGetMidBufferHeaderInfoCvi (AiUInt32 Module, AiUInt8 RT, AiUInt8 Type,
                                AiUInt8 MID_MC, AiUInt32 *BufHeaderIndex, AiUInt32 *BufHeaderAddr);
AI_LIB_FUNC AiReturn AI_CALL_CONV Api3910CmdHsRTModeCtrlCvi(AiUInt32 Module, AiUInt32 ul_RtMode,
                                                          AiUInt32 ul_Ctrl, AiUInt32 ul_Param1,
                                                          AiUInt32 ul_Param2, AiUInt32 ul_Param3);


//###############################################################
//    API3910 BM Commands
//###############################################################

AI_LIB_FUNC AiReturn AI_CALL_CONV Api3910CmdHsBMIntModeCvi(AiUInt32 bModule, AiUInt8 uc_IMod,
                                                         AiUInt8 uc_SMod, AiUInt8 uc_Res);
AI_LIB_FUNC AiReturn AI_CALL_CONV Api3910CmdHsBMCapModeCvi(AiUInt32 bModule, AiUInt8 uc_CapMode,
                                                         AiUInt32 ul_Tat);
AI_LIB_FUNC AiReturn AI_CALL_CONV Api3910CmdHsBMFilterIniCvi(AiUInt32 bModule, AiUInt8 uc_RtAddr,
    AiUInt32 ul_MceRxMid1,
    AiUInt32 ul_MceRxMid2,
    AiUInt32 ul_MceRxMid3,
    AiUInt32 ul_MceRxMid4,
    AiUInt32 ul_MceRxMid5,
    AiUInt32 ul_MceRxMid6,
    AiUInt32 ul_MceRxMid7,
    AiUInt32 ul_MceRxMid8,
    AiUInt32 ul_MceTxMid1,
    AiUInt32 ul_MceTxMid2,
    AiUInt32 ul_MceTxMid3,
    AiUInt32 ul_MceTxMid4,
    AiUInt32 ul_MceTxMid5,
    AiUInt32 ul_MceTxMid6,
    AiUInt32 ul_MceTxMid7,
    AiUInt32 ul_MceTxMid8);
AI_LIB_FUNC AiReturn AI_CALL_CONV Api3910CmdHsBMStatusReadCvi(AiUInt32 bModule,
    AiUInt8  *uc_Men,
    AiUInt32 *ul_Msw,
    AiUInt32 *ul_GlbMsgCnt,
    AiUInt32 *ul_GlbErrCnt,
    AiUInt32 *ul_GlbMsgCntA,
    AiUInt32 *ul_GlbMsgCntB,
    AiUInt32 *ul_GlbErrCntA,
    AiUInt32 *ul_GlbErrCntB,
    AiUInt32 *ul_BusLoadA,
    AiUInt32 *ul_BusLoadB,
    AiUInt32 *ul_BusLoadAAvg,
    AiUInt32 *ul_BusLoadBAvg );
AI_LIB_FUNC AiReturn AI_CALL_CONV Api3910CmdHsBMTrgConCvi(AiUInt32 bModule, AiUInt16 uw_WordLocation, AiUInt16 uw_TrgCon,
    AiUInt32    ul_ErrTrgWord,
    AiUInt32    ul_MonDataWord,
    AiUInt32    ul_MonCwActWord,
    AiUInt32    ul_MonCwActMask,
    AiUInt32    ul_MonFcPaWord,
    AiUInt32    ul_MonFcPaMask);
AI_LIB_FUNC AiReturn AI_CALL_CONV Api3910CmdHsBMActReadCvi(AiUInt32 bModule, AiUInt16 uw_EntryNr,
      AiUInt8  *uc_Fnd,
    AiUInt8  *uc_Rt,
    AiUInt8  *uc_Mid,
    AiUInt8  *uc_Type,
    AiUInt32 *ul_MsgCnt,
    AiUInt32 *ul_ErrCnt,
    AiUInt32 *ul_ErrTypeWord);
AI_LIB_FUNC AiReturn AI_CALL_CONV Api3910CmdHsBMRTActReadCvi(AiUInt32 bModule, AiUInt8 uc_RtAddr,
       AiUInt32 *ul_MsgCnt,
    AiUInt32 *ul_ErrCnt,
    AiUInt32 *ul_ErrTypeWord);
AI_LIB_FUNC AiReturn AI_CALL_CONV Api3910CmdHsBMRTMIDActReadCvi(AiUInt32 bModule, AiUInt8 uc_RtAddr, AiUInt8 uc_Mid,
    AiUInt8 uc_MidType,
    AiUInt32 *ul_MsgCnt,
    AiUInt32 *ul_ErrCnt,
    AiUInt32 *ul_ErrTypeWord);
AI_LIB_FUNC AiReturn AI_CALL_CONV Api3910CmdHsBMStackpReadCvi(AiUInt32 bModule,
    AiUInt8  *uc_Status,
    AiUInt8  *uc_Padding1,
    AiUInt16 *uw_StpCtpCnt,
    AiUInt16 *uw_CtpEtpCnt,
    AiUInt16 *uw_EntryCnt,
    AiUInt32 *ul_Stp,
    AiUInt32 *ul_Ctp,
    AiUInt32 *ul_Etp);
AI_LIB_FUNC AiReturn AI_CALL_CONV Api3910CmdHsBMStackEntryReadCvi(AiUInt32 bModule, AiUInt8 uc_PtrType, AiUInt8 uc_OffsetSign, AiUInt32 ul_Offset,    AiUInt16 uw_ReadStartIndex, AiUInt16 uw_EntryReadSize,
    AiUInt16 *uw_TxInitTime,
    AiUInt16 *uw_HsBufTagWord,
    AiUInt32 *ul_HsTimetagHigh,
    AiUInt32 *ul_HsTimetagLow,
    AiUInt16 *uw_LsCmdWord,
    AiUInt16 *uw_LsActWord,
    AiUInt32 *ul_LsCtrlWord,
    AiUInt16 *uw_LsCmdWord2,
    AiUInt16 *uw_LsActWord2,
    AiUInt32 *ul_LsCtrlWord2,
    AiUInt32 *ul_FcPaDa,
    AiUInt16 *uw_WCntWord,
    AiUInt16 *uw_EntryAmount,
    AiUInt16 auw_Data[API_HS_MAX_DATA_WORDS]);
AI_LIB_FUNC AiReturn AI_CALL_CONV Api3910CmdHsBMStackEntryFindCvi(AiUInt32 bModule, AiUInt8 uc_PtrType,
            AiUInt8 uc_OffsetSign, AiUInt16 uw_EntryType,AiUInt16 uw_Offset, AiUInt32 ul_EntryWord, AiUInt32 ul_EntryMask,
    AiUInt8    *uc_EntryFnd,
    AiUInt8    *uc_Eos,
    AiUInt16    *uw_EntryNr,
    AiUInt16    *uw_WordNr);
/* removed 29.11.2000 MR
AI_LIB_FUNC AiReturn AI_CALL_CONV Api3910CmdHsBMRecStatusReadCvi( AiUInt32 bModule,
    AiUInt8  *uc_Status,
    AiUInt32 *ul_HfiCnt,
    AiUInt32 *ul_SAddr,
    AiUInt32 *ul_Size);
*/
AI_LIB_FUNC AiReturn AI_CALL_CONV Api3910CmdHsBMDytagMonDefCvi(AiUInt32 bModule, AiUInt8 uc_biu, AiUInt8 uc_Id,
                                                         AiUInt8 uc_Con, AiUInt8 uc_RtAddr, AiUInt8 uc_SubAddrMsgId, AiUInt8 uc_SubAddrMsgIdType,
                                                         AiUInt16 uw_DytagType, AiUInt16 uw_DytagWPos, AiUInt16 uw_DytagBPos,
                                                         AiUInt16 uw_DytagBLen, AiUInt16 uw_DytagStep);
AI_LIB_FUNC AiReturn AI_CALL_CONV Api3910CmdHsBMDytagMonReadCvi(AiUInt32 bModule, AiUInt8 uc_biu, AiUInt8 uc_Id,
                                                          AiUInt32 *ul_Stale, AiUInt32 *ul_Bad, AiUInt32 *ul_Good);

//###############################################################
//    API3910 Queueing Commands
//###############################################################


//###############################################################
//    API3910 Calibration Commands
//###############################################################

AI_LIB_FUNC AiReturn AI_CALL_CONV Api3910CmdHsCalSigConCvi(AiUInt32 bModule, AiUInt8 uc_Channel,
    AiUInt8 uc_Con );
AI_LIB_FUNC AiReturn AI_CALL_CONV Api3910CmdHsCalCplConCvi(AiUInt32 bModule, AiUInt8 uc_Mode);
AI_LIB_FUNC AiReturn AI_CALL_CONV Api3910CmdHsCalXmtConCvi(AiUInt32 bModule, AiUInt8 uc_Channel,
    AiUInt8 uc_Power);
AI_LIB_FUNC AiReturn AI_CALL_CONV Api3910CmdHsCalRxModeCvi(AiUInt32 bModule, AiUInt32 ul_RxBusSelect);


//###############################################################
//    API3910 Replay Commands
//###############################################################

AI_LIB_FUNC AiReturn AI_CALL_CONV Api3910CmdHsReplayIniCvi(AiUInt32 bModule, AiUInt8 uc_Nct,
                                AiUInt8 uc_Cet, AiUInt8 uc_RInt, AiUInt32 ul_HsMsgCnt);
AI_LIB_FUNC AiReturn AI_CALL_CONV Api3910CmdHsReplayStatusCvi(AiUInt32 bModule,
    AiUInt8  *uc_Status,
    AiUInt8  *uc_Padding1,
    AiUInt16 *uw_Padding2,
    AiUInt32 *ul_RpiCnt,
    AiUInt32 *ul_SAddr,
    AiUInt32 *ul_Size,
    AiUInt32 *ul_MsgCnt);
AI_LIB_FUNC AiReturn AI_CALL_CONV Api3910CmdHsReplayRTCvi(AiUInt32 bModule, AiUInt8 uc_Con,
                                                        AiUInt8 uc_Mode, AiUInt8 uc_Rt);

//###############################################################
//    API3910 GPIO Commands
//###############################################################

AI_LIB_FUNC AiReturn AI_CALL_CONV Api3910CmdHsGpioResetCvi       ( AiUInt32 bModule, AiUInt8 *puc_DeviceCount,
                                                                   AiUInt8 auc_DeviceId[API_HS_MAX_GPIO_DEVICES], 
                                                                   AiUInt8 auc_DeviceType[API_HS_MAX_GPIO_DEVICES],
                                                                   AiUInt16 auw_Padding1[API_HS_MAX_GPIO_DEVICES]);
AI_LIB_FUNC AiReturn AI_CALL_CONV Api3910CmdHsGpioResetInfoCvi   ( AiUInt32 bModule, AiUInt8 *puc_DeviceCount,
                                                                   AiUInt8 auc_DeviceId[API_HS_MAX_GPIO_DEVICES], 
                                                                   AiUInt8 auc_DeviceType[API_HS_MAX_GPIO_DEVICES],
                                                                   AiUInt16 auw_Padding1[API_HS_MAX_GPIO_DEVICES]);
AI_LIB_FUNC AiReturn AI_CALL_CONV Api3910CmdHsGpioBrWriteCmdCvi  ( AiUInt32 bModule, AiUInt8 uc_Command);
AI_LIB_FUNC AiReturn AI_CALL_CONV Api3910CmdHsGpioBrWriteRegCvi  ( AiUInt32 bModule, AiUInt8 uc_Address, AiUInt8 uc_Data);
AI_LIB_FUNC AiReturn AI_CALL_CONV Api3910CmdHsGpioReadRegCvi     ( AiUInt32 bModule, AiUInt8 uc_Device, AiUInt8 uc_Address, AiUInt8 *puc_Data);
AI_LIB_FUNC AiReturn AI_CALL_CONV Api3910CmdHsGpioReadRegBlockCvi( AiUInt32 bModule, AiUInt8 uc_Device, AiUInt8 uc_Address,    
                                                                    AiUInt8 uc_Count, AiUInt8 *puc_Data);
AI_LIB_FUNC AiReturn AI_CALL_CONV Api3910CmdHsGpioReadStatusCvi  ( AiUInt32 bModule, AiUInt8 uc_Device, AiUInt8 *puc_Status);
AI_LIB_FUNC AiReturn AI_CALL_CONV Api3910CmdHsGpioWriteCmdCvi    ( AiUInt32 bModule, AiUInt8 uc_Device, AiUInt8 uc_Command);
AI_LIB_FUNC AiReturn AI_CALL_CONV Api3910CmdHsGpioWriteRegCvi    ( AiUInt32 bModule, AiUInt8 uc_Device,
                                                                   AiUInt8 uc_Address, AiUInt8 uc_Data);
AI_LIB_FUNC AiReturn AI_CALL_CONV Api3910CmdHsGpioWriteRegBlockCvi( AiUInt32 bModule, AiUInt8 uc_Device, AiUInt8 uc_Address,    
                                                                    AiUInt8 uc_Count, AiUInt8 *puc_Data);
AI_LIB_FUNC AiReturn AI_CALL_CONV Api3910HsCtxResetCvi           ( AiUInt32 bModule, AiUInt8 *puc_CtxDevCount,
                                                                   AiUInt8 auc_DeviceId[API_HS_MAX_GPIO_DEVICES],
                                                                   AiUInt8 auc_DeviceType[API_HS_MAX_GPIO_DEVICES],
                                                                   AiUInt16 auw_Padding1[API_HS_MAX_GPIO_DEVICES]);
AI_LIB_FUNC AiReturn AI_CALL_CONV Api3910HsCtxBiteCvi            ( AiUInt32 bModule, AiUInt8 uc_Device, AiUInt8 *puc_Result, AiUInt8 *puc_FailInfo);
AI_LIB_FUNC AiReturn AI_CALL_CONV Api3910HsCtxReadVersionCvi     ( AiUInt32 bModule, AiUInt8 uc_Device, AiUInt8 *puc_Version );
AI_LIB_FUNC AiReturn AI_CALL_CONV Api3910HsCtxReadStatusCvi      ( AiUInt32 bModule, AiUInt8 uc_Device, AiUInt8 *puc_Status);
AI_LIB_FUNC AiReturn AI_CALL_CONV Api3910HsCtxModeConCvi         ( AiUInt32 bModule, AiUInt8 uc_Device, AiUInt8 uc_Mode );
AI_LIB_FUNC AiReturn AI_CALL_CONV Api3910HsCtxDacConCvi          ( AiUInt32 bModule, AiUInt8 uc_Device, AiFloat f_Pulse,
                                                                   AiFloat f_Rise, AiFloat f_Fall, AiFloat f_OvrShoot);
AI_LIB_FUNC AiReturn AI_CALL_CONV Api3910HsCtxReadRegCvi         ( AiUInt32 bModule, AiUInt8 uc_Device, AiUInt8 uc_Address, AiUInt8 *puc_Data);
AI_LIB_FUNC AiReturn AI_CALL_CONV Api3910HsCtxWriteRegCvi        ( AiUInt32 bModule, AiUInt8 uc_Device, AiUInt8 uc_Address, AiUInt8 uc_Data);

#endif
// end: cvi3910.h
