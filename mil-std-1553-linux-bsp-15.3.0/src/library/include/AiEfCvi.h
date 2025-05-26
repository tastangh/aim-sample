/* SPDX-FileCopyrightText: 2001-2022 AIM GmbH <info@aim-online.com> */
/* SPDX-License-Identifier: MIT */

/*! \file AiEfCvi.h
 *
 *  CVI Wrapper DLL for EFEx Application Interface. Header file with function prototypes
 *
 */
#ifndef CVI_EFEX
#define CVI_EFEX

#include "Api1553Cvi.h"
#include "Api3910Cvi.h"

//***************************************************************************
//  API3910Xp Functions - External References
//***************************************************************************

//###########################################################################
//    API3910Xp System Commands
//###########################################################################
AI_LIB_FUNC AiReturn AI_CALL_CONV Api3910CmdEfTimeOutDefCvi(AiUInt32 bModule, AiInt16 w_EfaResponseTOOffset, AiUInt16 uw_EfexResponseTOVal);

AI_LIB_FUNC AiReturn AI_CALL_CONV Api3910CmdEfSwitchEfaEfexCvi(AiUInt32 bModule, AiUInt8 uc_SwitchType,
                                                               AiUInt32 ul_LicenseKey1, AiUInt32 ul_LicenseKey2,
                                                               AiUInt32 ul_LicenseKey3, AiUInt32 ul_LicenseKey4,
                                                               AiUInt8  *puc_SwitchResult );

//###############################################################
//    API3910Xp Buffer Commands
//###############################################################
AI_LIB_FUNC AiReturn AI_CALL_CONV Api3910HsBHModifyCvi(AiUInt32 bModule,
    AiUInt32 ul_BHAddr, AiUInt32 ul_DataBufferStoreMode, AiUInt32 ul_WordCountMode,
    AiUInt32 ul_BufferSize, AiUInt32 ul_BufferQueueMode, AiUInt32 ul_BufferQueueSize,
    AiUInt32 ul_StatusQueueSize, AiUInt32 ul_EventQueueSize, AiUInt32 ul_CurrentBufferIndex);

//###############################################################
//    API3910Xp BC Commands
//###############################################################

AI_LIB_FUNC AiReturn AI_CALL_CONV Api3910CmdEfBCXferDefCvi(AiUInt32 bModule,
        AiUInt16    uw_XferId,        AiUInt16    uw_HeaderId,      AiUInt8      uc_XferType,
        AiUInt8      uc_XferHalt,      AiUInt8      uc_MsgSize,          AiUInt8      uc_Padding1,
        AiUInt16    uw_Rt,            AiUInt16    uw_Mid,              AiUInt8      uc_StatusWordException,
        AiUInt8      uc_RetryControl,  AiUInt16    uw_Padding2,      AiUInt16  uw_WCount,
    AiUInt16  uw_EfexMCWordOne, AiUInt16  uw_EfexMCWordTwo,    AiUInt32    ul_FirstXferGap,
        AiUInt32    ul_SecondXferGap,    AiUInt32    ul_LsChnIr,          AiUInt8      uc_LsErrType,
        AiUInt8      uc_LsErrSync,        AiUInt8      uc_LsErrContig,      AiUInt8      uc_LsErrPadding1,
        AiUInt32    ul_LsErrSpec,        AiUInt32    ul_HsChnIr,          AiUInt32    ul_HsXmt,
        AiUInt8      uc_HsErrType,        AiUInt8      uc_HsPadding1,    AiUInt16    uw_HsPadding2,
        AiUInt32    ul_HsErrSpec );
AI_LIB_FUNC AiReturn AI_CALL_CONV Api3910CmdEfBCXferDefExCvi(AiUInt32 bModule,
        AiUInt16    uw_XferId,            AiUInt16    uw_HeaderId,        AiUInt8      uc_XferType,
        AiUInt8        uc_XferHalt,        AiUInt8        uc_MsgSize,            AiUInt8      uc_Padding1,
        AiUInt16	uw_Rt,                AiUInt16    uw_Mid,                AiUInt8      uc_StatusWordException,
        AiUInt8        uc_RetryControl,    AiUInt16    uw_Padding2,        AiUInt16  uw_WCount,
        AiUInt16    uw_EfexMCWordOne,    AiUInt16    uw_EfexMCWordTwo,    AiUInt32    ul_FirstXferGap,
        AiUInt32    ul_SecondXferGap,    AiUInt32    ul_LsChnIr,            AiUInt8      uc_LsErrType,
        AiUInt8        uc_LsErrSync,        AiUInt8        uc_LsErrContig,          AiUInt8      uc_LsErrPadding1,
        AiUInt32    ul_LsErrSpec,        AiUInt32    ul_HsChnIr,            AiUInt32    ul_HsXmt,
        AiUInt8        uc_HsErrType,        AiUInt8        uc_HsPadding1,        AiUInt16    uw_HsPadding2,
        AiUInt32    ul_HsErrSpec,        AiUInt32 *pul_LsXferDescAddr1,    AiUInt32 *pul_LsXferDescAddr2,
        AiUInt32    *pul_HsXferDescAddr );

AI_LIB_FUNC AiReturn AI_CALL_CONV Api3910CmdEfBCGetXferDefCvi(AiUInt32 bModule, AiUInt16 uw_XferId,
    AiUInt16 *puw_HeaderId,      AiUInt8  *puc_XferType,              AiUInt8  *puc_XferHalt,
    AiUInt8  *puc_MsgSize,       AiUInt8  *puc_Padding1,              AiUInt16 *puw_Rt,
    AiUInt16 *puw_Mid,           AiUInt8  *puc_StatusWordException,   AiUInt8  *puc_RetryControl,
    AiUInt16 *puw_Padding2,      AiUInt16 *puw_WCount,                AiUInt16 *puw_EfexMCWordOne,
    AiUInt16 *puw_EfexMCWordTwo, AiUInt32 *pul_x_FirstXferGap,        AiUInt32 *pul_x_SecondXferGap,
    AiUInt32 *pul_LsChnIr,       AiUInt8  *puc_LsErrType,             AiUInt8  *puc_LsErrSync,
    AiUInt8  *puc_LsErrContig,   AiUInt8  *puc_LsErrPadding1,         AiUInt32 *pul_LsErrSpec,
    AiUInt32 *pul_HsChnIr,       AiUInt32 *pul_HsXmt,                 AiUInt8  *puc_HsErrType,
    AiUInt8  *puc_HsErrPadding1, AiUInt16 *puw_HsErrPadding2,         AiUInt32 *pul_HsErrSpec );

AI_LIB_FUNC AiReturn AI_CALL_CONV Api3910CmdEfBCGetXferDefExCvi(AiUInt32 bModule, AiUInt16 uw_XferId,
    AiUInt16 *puw_HeaderId,      AiUInt8  *puc_XferType,              AiUInt8  *puc_XferHalt,
    AiUInt8  *puc_MsgSize,       AiUInt8  *puc_Padding1,              AiUInt16 *puw_Rt,
    AiUInt16 *puw_Mid,           AiUInt8  *puc_StatusWordException,   AiUInt8  *puc_RetryControl,
    AiUInt16 *puw_Padding2,      AiUInt16 *puw_WCount,                AiUInt16 *puw_EfexMCWordOne,
    AiUInt16 *puw_EfexMCWordTwo, AiUInt32 *pul_x_FirstXferGap,        AiUInt32 *pul_x_SecondXferGap,
    AiUInt32 *pul_LsChnIr,       AiUInt8  *puc_LsErrType,             AiUInt8  *puc_LsErrSync,
    AiUInt8  *puc_LsErrContig,   AiUInt8  *puc_LsErrPadding1,         AiUInt32 *pul_LsErrSpec,
    AiUInt32 *pul_HsChnIr,       AiUInt32 *pul_HsXmt,                 AiUInt8  *puc_HsErrType,
    AiUInt8  *puc_HsErrPadding1, AiUInt16 *puw_HsErrPadding2,         AiUInt32 *pul_HsErrSpec,
    AiUInt32 *pul_LsXferDescAddr1, AiUInt32 *pul_LsXferDescAddr2,     AiUInt32 *pul_HsXferDescAddr );


AI_LIB_FUNC AiReturn AI_CALL_CONV Api3910CmdEfBCXferReadCvi(AiUInt32 bModule, AiUInt16 uw_XferId,
    AiUInt32 *pul_TransDescControl,     AiUInt32 *pul_TransStatus,          AiUInt32 *pul_ErrSpec,
    AiUInt32 *pul_TransCount,           AiUInt32 *pul_TransErrCount,        AiUInt16 *puw_CCDestAddress,
    AiUInt16 *puw_CCFrameControl,       AiUInt16 *puw_CCWCountMCModeCom,    AiUInt16 *puw_MCModeCom,
    AiUInt16 *puw_SFrameMessTag,        AiUInt16 *puw_Reserved,             AiUInt16 *puw_SFrameADW,
    AiUInt16 *puw_SFrameControl,        AiUInt16 *puw_SDFrameMessTag,       AiUInt16 *puw_SDFrameWCount,
    AiUInt16 *puw_SDFrameDestAddr,      AiUInt16 *puw_SDFrameControl,       AiUInt32 *pul_TimeTag,
    AiUInt16 *puw_SFrameSatusWExcMask,  AiUInt16 *puw_SDFrameSatusWExcMask, AiUInt16 *puw_BCTransWaitT,
    AiUInt16 *puw_Reserved1,            AiUInt16 *puw_Reserved2,            AiUInt16 *puw_Reserved3 );

AI_LIB_FUNC AiReturn AI_CALL_CONV Api3910CmdEfBCXferConCvi(AiUInt32 bModule, AiUInt16 uw_XferId, AiUInt8 uc_Enable );

AI_LIB_FUNC AiReturn AI_CALL_CONV Api3910CmdEfBCRespSimDefCvi(AiUInt32 bModule, 
    AiUInt32 ul_XferId,                    AiUInt32 ul_HeaderId,
    AiUInt32 ul_SDFrameBus,                AiUInt32 ul_SDFrameErrorWordPos,    AiUInt32 ul_SDFrameErrorType,
    AiUInt32 ul_SDFrameBitCntOrPos,        AiUInt32 ul_SDFrameResponseTime,    AiUInt32 ul_SDFrameFCPA,
    AiUInt32 ul_SDFrameDestAddr,        AiUInt32 ul_SDFrameWordCount,
    AiUInt32 ul_SFrameBus,                AiUInt32 ul_SFrameErrorWordPos,        AiUInt32 ul_SFrameErrorType,
    AiUInt32 ul_SFrameBitCntOrPos,        AiUInt32 ul_SFrameResponseTime,        AiUInt32 ul_SFrameFCPA,
    AiUInt32 ul_SFrameADW);

AI_LIB_FUNC AiReturn AI_CALL_CONV Api3910CmdEfBCRespSimCtrlCvi(AiUInt32 bModule, 
      AiUInt32 ul_XferId,               AiUInt32 ul_ResponseMode,    AiUInt32 ul_FastMode,
    AiUInt32 ul_ForceResponseOnError);

AI_LIB_FUNC AiReturn AI_CALL_CONV Api3910CmdEfBCInternalDefCvi(AiUInt32 bModule, 
    AiUInt32 ul_XferId,                    AiUInt32 ul_HeaderId,
    AiUInt32 ul_BCInternalFCPA,            AiUInt32 ul_BCInternalDestAddr,        AiUInt32 ul_BCInternalWordCount,
    AiUInt32 ul_BCInternalModeCodeWords);

AI_LIB_FUNC AiReturn AI_CALL_CONV Api3910CmdEfBCInternalCtrlCvi(AiUInt32 bModule, 
      AiUInt32 ul_XferId,               AiUInt32 ul_ForceLocalResponseOnError, AiUInt32 ul_SuppressLocalCCError,
    AiUInt32 ul_UseBCInternalValidation);

AI_LIB_FUNC AiReturn AI_CALL_CONV Api3910CmdEfBCSyncTimeDefCvi(AiUInt32 bModule, AiUInt32 pul_SyncTimeOffset);

AI_LIB_FUNC AiReturn AI_CALL_CONV Api3910CmdEfBCSyncTimeGetCvi(AiUInt32 bModule, 
      AiUInt32 *pul_SyncTimeOffset, AiUInt32 *pul_LastRcvSyncTime, AiUInt32 *pul_LastSyncTime);

//###############################################################
//    API3910Xp RT Commands
//###############################################################

AI_LIB_FUNC AiReturn AI_CALL_CONV Api3910CmdEfRTStatusReadCvi(AiUInt32 bModule, AiUInt8 uc_RtAddr,
      AiUInt32 *pul_RtTransDescControl, AiUInt32 *pul_RtTransStatus, AiUInt32 *pul_RtTransCount,
      AiUInt32 *pul_RtTransErrCount,    AiUInt16 *puw_NextStatus,    AiUInt16 *puw_LastStatus,
      AiUInt16 *puw_NextADW,            AiUInt16 *puw_LastADW,       AiUInt16 *puw_CCMCDestAdd,
      AiUInt16 *puw_CCMCFrameCont,      AiUInt16 *puw_CCMCMessTag,   AiUInt16 *puw_CCMCWCount,
      AiUInt16 *puw_MCDataW2,           AiUInt16 *puw_MCDataW1,      AiUInt32 *pul_Res1 );

AI_LIB_FUNC AiReturn AI_CALL_CONV Api3910CmdEfRTIniCvi(AiUInt32 bModule, AiUInt8 uc_RtAddr,
      AiUInt8  uc_RtMode, AiFloat f_RespEfex, 
      AiUInt32 *ul_RtTransDescControl, AiUInt32 *ul_RtTransStatus, AiUInt32 *ul_RtTransCount,
      AiUInt32 *ul_RtTransErrCount,    AiUInt16 *uw_NextStatus,    AiUInt16 *uw_LastStatus,
      AiUInt16 *uw_NextADW,            AiUInt16 *uw_LastADW,       AiUInt16 *uw_CCMCDestAdd,
      AiUInt16 *uw_CCMCFrameCont,      AiUInt16 *uw_CCMCMessTag,   AiUInt16 *uw_CCMCWCount,
      AiUInt16 *uw_MCDataW2,           AiUInt16 *uw_MCDataW1,      AiUInt32 *ul_Res1 );
                                                      

AI_LIB_FUNC AiReturn AI_CALL_CONV Api3910CmdEfRTModeCodeDefCvi(AiUInt32 bModule,  AiUInt8 uc_RtAddr, AiUInt8 uc_ModeCode,
                                                               AiUInt8 uc_ModeCodeTx, AiUInt8 uc_ModeCodeBr );
AI_LIB_FUNC AiReturn AI_CALL_CONV Api3910CmdEfRTModeCodeDefExCvi(AiUInt32 bModule,  AiUInt32 ul_RtAddr, AiUInt32 ul_ModeCode,
                                                                 AiUInt32 ul_ModeCodeTx, AiUInt32 ul_ModeCodeBr, AiUInt32 ul_ModeCodeRx, AiUInt32 ul_StdIllegalHandling);

//###############################################################
//    API3910Xp BM Commands
//###############################################################

AI_LIB_FUNC AiReturn AI_CALL_CONV Api3910CmdEfBMTrigTypeCvi(AiUInt32 bModule,  AiUInt8 uc_SHdrTriggerSel, AiUInt8 uc_CWTriggerSel );
AI_LIB_FUNC AiReturn AI_CALL_CONV Api3910CmdEfBMTrgConCvi(AiUInt32 bModule, AiUInt16 uw_WordLocation, AiUInt16 uw_TrgCon,
    AiUInt32    ul_ErrTrgWord,
    AiUInt32    ul_MonDataWord,
    AiUInt32    ul_MonCmdWord,
    AiUInt32    ul_MonCmdMask,
    AiUInt32    ul_MonHdrWord,
    AiUInt32    ul_MonHdrMask);

//###############################################################
//    API3910Xp Queueing Commands
//###############################################################


//###############################################################
//    API3910Xp Calibration Commands
//###############################################################
AI_LIB_FUNC AiReturn AI_CALL_CONV Api3910CmdEfCalRxModeCvi(AiUInt32 bModule, AiUInt32 ul_RxBusSelect, AiUInt32 ul_FFlt);


//###############################################################
//    API3910Xp Replay Commands
//###############################################################

#endif
// end: AiEfCvi.h
