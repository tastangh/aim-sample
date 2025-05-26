/* SPDX-FileCopyrightText: 2009-2024 AIM GmbH <info@aim-online.com> */
/* SPDX-License-Identifier: MIT  */

/* SmartLib.h */

#pragma once

#include "Ai_cdef.h"

#define SL_LOGFILE_NAME      "smart.log"

#define SL_RETVAL_NO_BATCH                0xFFFFFFFF
#define SL_RETVAL_BATCH_OK                0
#define SL_RETVAL_BATCH_ARG_WRONG         1
#define SL_RETVAL_BATCH_ARG_OUT_OF_RANGE  2
#define SL_MAX_BATCH_SN                   32

#define SL_BATCHMODE_UPDATE               0
#define SL_BATCHMODE_MODE_SWITCH          1
#define SL_BATCHMODE_UPDATE_CHECKSUM      2
#define SL_BATCHMODE_HELP                 3
#define SL_BATCHMODE_NVR_DUMP             4

#define SL_INSTR_HEADER      1
#define SL_INSTR_FILE        2
#define SL_INSTR_NVR         3
#define SL_INSTR_COLDBOOT    4
#define SL_INSTR_COLDBOOT_R  5  /* cold boot and restart of SMART required*/
#define SL_INSTR_REBOOT      6
#define SL_INSTR_REBOOT_R    7  /* reboot and restart of SMART required*/
#define SL_INSTR_ABORT       8
#define SL_INSTR_IF          9
#define SL_INSTR_EXEC_REBOOT 10
#define SL_INSTR_COMMENT     11
#define SL_INSTR_CSM         12
#define SL_INSTR_COMPONENT   14
#define SL_INSTR_PACKAGE     15
#define SL_INSTR_NVR_CHK     16
#define SL_INSTR_BIFI        17


#define SL_MIN_INSTR         SL_INSTR_HEADER
#define SL_MAX_INSTR         SL_INSTR_BIFI


#define SL_POST_UPDATE_NONE           0 


/*! \enum sl_post_update_actions

    Enumeration of flags that identify
    actions to be taken after successful update of board
*/
enum sl_post_update_actions
{
    SL_POST_UPDATE_REBOOT = 1 << 0,         /*!< System has to be rebooted */
    SL_POST_UPDATE_POWEROFF = 1 << 1,       /*!< System poweroff is neccessary */
    SL_POST_UPDATE_SMART_RESTART = 1 << 2,  /*!< SMART should be automatically restarted after system reboot/poweroff */
};


typedef struct
{
  AiUInt8  b_BatchExec;
  AiUInt8  b_Force;
  AiUInt32 ul_NrOfSn;
  AiUInt32 ul_BatchMode;
  AiUInt32 aul_Sn[SL_MAX_BATCH_SN];
  AiChar   s_BatchUpdFile[255];
  AiChar   s_BatchServer[80];
  AiChar   s_BatchLicenceKey[80];
  AiUInt32 ul_ModeSwitchID;
  AiUInt32 aul_LicenceKey[4];
  int      argc;
  char     **argv;
  AiUInt8  b_autoboot;
} TY_SMARTCON_SETTINGS;

/* module SmartLib.c */
_API_EXTERN void _API_DLL_FUNC v_SLLogFileTime               ( FILE *p_LogFile );
_API_EXTERN void _API_DLL_FUNC v_SLLogFile                   ( char *sz_Text, 
                                                               char uc_PrintTime,
                                                               char uc_PrintOnConsole );
_API_EXTERN AiUInt16 _API_DLL_FUNC uw_SLDownloadSREDirectly  ( AiUInt32 ul_ModuleHandle,
                                                               char *sz_FileName,
                                                               AiUInt8 uc_Sector,
                                                               AiUInt32 ul_Addr,
                                                               AiUInt32 ul_Offset );
_API_EXTERN AiUInt16 _API_DLL_FUNC uw_SLUpdateComponent      ( AiUInt32 ul_ModuleHandle,
                                                               AiUInt32 ul_ComponentId,
                                                               const char *sz_FilePrefix,
                                                               AiUInt32 ul_NovRamAddr,
                                                               const char *sz_FilePath,
                                                               AiUInt32 ul_Force,
                                                               enum sl_post_update_actions* post_update_actions);
_API_EXTERN AiUInt16 _API_DLL_FUNC uw_SLUpdateBitfile(AiUInt32 ul_ModuleHandle,
                                                                AiUInt32 ul_FlashPartition,
                                                                const char *sz_FilePrefix,
                                                                AiUInt32 ul_NovRamAddr,
                                                                const char* sz_Suffix,
                                                                const char* sz_FilePath,
                                                                AiUInt32 ul_Force,
                                                                enum sl_post_update_actions* post_update_actions);
_API_EXTERN AiUInt16 _API_DLL_FUNC uw_SLUpdatePackage(AiUInt32 ul_ModuleHandle,
                                                               const char *sz_FilePrefix,
                                                               const char *sz_FilePath,
                                                               AiUInt32 ul_Force);
_API_EXTERN AiUInt16 _API_DLL_FUNC uw_SLUpdateBoard          ( AiUInt32 ul_ModuleHandle,
                                                               AiChar *sz_SrvName,
                                                               AiChar *sz_FileName,
                                                               AiUInt32 ul_Force,
                                                               enum sl_post_update_actions* post_update_actions);
_API_EXTERN AiUInt16 _API_DLL_FUNC uw_SLGetAllModuleInfos    ( AiUChar *ac_SrvName,
                                                               TY_MODULE_INFO *px_ModuleInfo,
                                                               AiUInt32 *pul_ReturnedModules );
_API_EXTERN AiUInt32 _API_DLL_FUNC uw_SLGetAmountOfModuleInfos(AiChar *ac_SrvName);
_API_EXTERN AiUInt16 _API_DLL_FUNC uw_SLGetModuleInfo        ( AiChar *ac_SrvName,
                                                               AiUInt32 ul_ModuleId,
                                                               TY_MODULE_INFO *px_ModuleInfo );
_API_EXTERN AiUInt32 _API_DLL_FUNC ul_SLSwitchBoard(AiUInt32 ul_ModuleHandle, AiUInt8 *puc_SwitchResult);

_API_EXTERN AiUInt32 _API_DLL_FUNC ul_SLNvrDump(AiUInt32 ul_ModuleHandle);

_API_EXTERN void _API_DLL_FUNC  SLShutdown(enum sl_post_update_actions shutdown_flags);
_API_EXTERN void _API_DLL_FUNC  SLRegisterRestart(char * command_line);

/* module SmartUpd.c */
_API_EXTERN AiUInt16 _API_DLL_FUNC uw_SLAnalyzeUPDFile       ( AiChar *ac_FileName, 
                                                               AiUInt32 *pul_Result,
                                                               AiUInt32 *pul_ErrorLine );
_API_EXTERN AiUInt16 _API_DLL_FUNC uw_SLUpdateCSM            ( AiChar *ac_FileName );

_API_EXTERN void _API_DLL_FUNC v_SLShowUsage                 ( AiChar *ac_Version );

