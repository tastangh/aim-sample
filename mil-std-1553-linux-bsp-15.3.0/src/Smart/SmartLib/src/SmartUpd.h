/* SPDX-FileCopyrightText: 2011-2024 AIM GmbH <info@aim-online.com> */
/* SPDX-License-Identifier: MIT  */

/*!
SmartUpd.h
Contains the interface function prototypes for the SmartUpd module.
*/
#pragma once

#define SL_MAX_SECTION_ENTRIES    50
#define SL_MAX_SECTION_ENTRY_SIZE 150
#define SL_MAX_PARAMS             7
#define SL_MAX_PARAM_SIZE         150

#define SL_INSTR_NAME_HEADER      "{"
#define SL_INSTR_NAME_FILE        "FILE"
#define SL_INSTR_NAME_NVR_CHK     "NVR_CHK"
#define SL_INSTR_NAME_NVR         "NVR"
#define SL_INSTR_NAME_COLDBOOT    "COLDBOOT"
#define SL_INSTR_NAME_COLDBOOT_R  "COLDBOOT_RESTART"
#define SL_INSTR_NAME_REBOOT      "REBOOT"
#define SL_INSTR_NAME_REBOOT_R    "REBOOT_RESTART"
#define SL_INSTR_NAME_ABORT       "ABORT"
#define SL_INSTR_NAME_IF          "IF"
#define SL_INSTR_NAME_EXEC_REBOOT "EXEC_REBOOT"
#define SL_INSTR_NAME_COMMENT     ";"
#define SL_INSTR_NAME_CSM         "CSM"
#define SL_INSTR_NAME_COMPONENT   "COMPONENT"
#define SL_INSTR_NAME_PACKAGE     "PACKAGE"
#define SL_INSTR_NAME_BIFI        "BIFI"

#define SL_IF_OPERATION_LESS_THAN                         1
#define SL_IF_OPERATION_GREATER_THAN                      2
#define SL_IF_OPERATION_EQUAL                             3
#define SL_IF_OPERATION_NOT_EQUAL                         4

#define SL_IF_VALIDATE_TSWVER                             0
#define SL_IF_VALIDATE_BIU1VER                            1
#define SL_IF_VALIDATE_BIU2VER                            2
#define SL_IF_VALIDATE_BIU3VER                            3
#define SL_IF_VALIDATE_BIU4VER                            4
#define SL_IF_VALIDATE_IOLCA1VER                          5
#define SL_IF_VALIDATE_IOLCA2VER                          6
#define SL_IF_VALIDATE_IOLCA3VER                          7
#define SL_IF_VALIDATE_IOLCA4VER                          8
#define SL_IF_VALIDATE_PCILCAVER                          9
#define SL_IF_VALIDATE_TCPVER                             10
#define SL_IF_VALIDATE_MONVER                             11

#define SL_MAX_IF_VALIDATIONS                             (SL_IF_VALIDATE_MONVER+1)

#define SL_IF_VALIDATE_TSWVER_NAME                        "TSWVER"
#define SL_IF_VALIDATE_BIU1VER_NAME                       "BIU1VER"
#define SL_IF_VALIDATE_BIU2VER_NAME                       "BIU2VER"
#define SL_IF_VALIDATE_BIU3VER_NAME                       "BIU3VER"
#define SL_IF_VALIDATE_BIU4VER_NAME                       "BIU4VER"
#define SL_IF_VALIDATE_IOLCA1VER_NAME                     "IOLCA1VER"
#define SL_IF_VALIDATE_IOLCA2VER_NAME                     "IOLCA2VER"
#define SL_IF_VALIDATE_IOLCA3VER_NAME                     "IOLCA3VER"
#define SL_IF_VALIDATE_IOLCA4VER_NAME                     "IOLCA4VER"
#define SL_IF_VALIDATE_PCILCAVER_NAME                     "PCILCAVER"
#define SL_IF_VALIDATE_TCPVER_NAME                        "TCPVER"
#define SL_IF_VALIDATE_MONVER_NAME                        "MONVER"

#define SL_RETVAL_HEADER_INSTR_IS_VALID_FOR_BOARD         100
#define SL_RETVAL_HEADER_INSTR_IS_INVALID_FOR_BOARD       101
#define SL_RETVAL_IF_INSTR_IS_TRUE                        102
#define SL_RETVAL_IF_INSTR_IS_FALSE                       103

#define SL_RETVAL_ERROR_PARAM1_IS_NULL                    90
#define SL_RETVAL_ERROR_PARAM2_IS_NULL                    91
#define SL_RETVAL_ERROR_PARAM3_IS_NULL                    92
#define SL_RETVAL_ERROR_NO_VALID_INSTR                    99
#define SL_RETVAL_ERROR_FILE_INSTR_PARAM1                 200
#define SL_RETVAL_ERROR_FILE_INSTR_PARAM2                 201
#define SL_RETVAL_ERROR_FILE_INSTR_PARAM3                 202
#define SL_RETVAL_ERROR_FILE_INSTR_PARAM4                 203
#define SL_RETVAL_ERROR_FILE_INSTR_MISSING_PARAMS         204
#define SL_RETVAL_ERROR_FILE_INSTR_TOO_MANY_PARAMS        205
#define SL_RETVAL_ERROR_COMPONENT_INSTR_PARAM1            250
#define SL_RETVAL_ERROR_COMPONENT_INSTR_PARAM2            251
#define SL_RETVAL_ERROR_COMPONENT_INSTR_PARAM3            252
#define SL_RETVAL_ERROR_COMPONENT_INSTR_MISSING_PARAMS    253
#define SL_RETVAL_ERROR_COMPONENT_INSTR_TOO_MANY_PARAMS   254
#define SL_RETVAL_ERROR_COMPONENT_INSTR_PARAM4            255
#define SL_RETVAL_ERROR_NVR_INSTR_PARAM1                  300
#define SL_RETVAL_ERROR_NVR_INSTR_PARAM2                  301
#define SL_RETVAL_ERROR_NVR_INSTR_MISSING_PARAMS          302
#define SL_RETVAL_ERROR_NVR_INSTR_TOO_MANY_PARAMS         303
#define SL_RETVAL_ERROR_COLDBOOT_INSTR_TOO_MANY_PARAMS    400
#define SL_RETVAL_ERROR_REBOOT_INSTR_TOO_MANY_PARAMS      500
#define SL_RETVAL_ERROR_ABORT_INSTR_TOO_MANY_PARAMS       600
#define SL_RETVAL_ERROR_IF_INSTR_LEFT_PART                700
#define SL_RETVAL_ERROR_IF_INSTR_OPERATION                701
#define SL_RETVAL_ERROR_IF_INSTR_MAJ_VER                  702
#define SL_RETVAL_ERROR_IF_INSTR_MIN_VER                  703
#define SL_RETVAL_ERROR_IF_INSTR_BUILD_NR                 704
#define SL_RETVAL_ERROR_IF_INSTR_PARAM_CONSTRUCT          705
#define SL_RETVAL_ERROR_EXEC_REBOOT_INSTR_TOO_MANY_PARAMS 800

/******************************************/
/* functions related to checksum handling */
/******************************************/
_API_EXTERN AiUInt16 _API_DLL_FUNC uw_SLAnalyzeUPDFile       ( AiChar *ac_FileName, 
                                                               AiUInt32 *pul_Result,
                                                               AiUInt32 *pul_ErrorLine );
_API_EXTERN AiUInt16 _API_DLL_FUNC uw_SLCalculateCSM         ( AiChar *ac_FileName, 
                                                               AiBoolean b_RefreshCheckSumInFile,
                                                               AiUInt32 *pul_CalculatedCheckSum, 
                                                               AiUInt32 *pul_LastCheckSumFoundInFile);
_API_EXTERN AiUInt16 _API_DLL_FUNC uw_SLVerifyCSM            ( AiChar *ac_FileName );
_API_EXTERN AiUInt16 _API_DLL_FUNC uw_SLUpdateCSM            ( AiChar *ac_FileName );


/***********************************************/
/* functions related to board section handling */
/***********************************************/
_API_EXTERN AiUInt16 _API_DLL_FUNC uw_SLSearchBoardSection   ( AiChar *ac_FileName,
                                                               TY_MODULE_INFO x_ModuleInfo );
_API_EXTERN AiUInt16 _API_DLL_FUNC uw_SLReadBoardSection     ( AiChar *ac_FileName,
                                                               TY_MODULE_INFO x_ModuleInfo, 
                                                               AiBoolean b_SearchOnly,
                                                               AiChar ppc_Section[SL_MAX_SECTION_ENTRIES+1][SL_MAX_SECTION_ENTRY_SIZE],
                                                               AiUInt32 *pul_NbOfSectionEntries );


/***************************************************/
/* functions related to instruction interpretation */
/***************************************************/
void v_SeparateString                                        ( char *auc_Str, char c_Seperator, 
                                                               char auc_Params[SL_MAX_PARAMS][SL_MAX_PARAM_SIZE], 
                                                               unsigned char *puc_NbOfParams);
_API_EXTERN AiUInt16 _API_DLL_FUNC uw_SLCheckHeaderInstr     ( TY_MODULE_INFO *px_ModuleInfo,
                                                               AiChar *puc_SectionEntry,
                                                               AiUInt32 *pul_Info );
_API_EXTERN AiUInt16 _API_DLL_FUNC uw_SLCheckFileInstr       ( AiChar *pc_SectionEntry,
                                                               AiChar ac_FileName[20],
                                                               AiUInt32 aul_Params[3] );
_API_EXTERN AiUInt16 _API_DLL_FUNC uw_SLCheckComponentInstr  ( AiChar *pc_SectionEntry,
                                                               AiChar ac_FileName[20],
                                                               AiUInt32 aul_Params[2] );
_API_EXTERN AiUInt16 _API_DLL_FUNC uw_SLCheckBitfileInstr(AiChar *pc_SectionEntry,
                                                                AiChar ac_FileName[20],
                                                                AiUInt32 aul_Params[2],
                                                                AiChar ac_Suffix[20]);
_API_EXTERN AiUInt16 _API_DLL_FUNC uw_SLCheckPackageInstr(AiChar *pc_SectionEntry,
                                                               AiChar ac_FileName[150] );
_API_EXTERN AiUInt16 _API_DLL_FUNC uw_SLCheckNvrInstr        ( AiChar *puc_SectionEntry,
                                                               AiUInt32 aul_Params[2] );
_API_EXTERN AiUInt16 _API_DLL_FUNC uw_SLCheckNvrChkInstr     ( AiChar *puc_SectionEntry,
                                                               AiUInt32 aul_Params[3] );
_API_EXTERN AiUInt16 _API_DLL_FUNC uw_SLCheckInstrWithTxtOut ( AiChar *puc_SectionEntry,
                                                               AiChar *puc_InstrName,
                                                               AiChar ac_OutTxt[SL_MAX_PARAM_SIZE] );
_API_EXTERN AiUInt16 _API_DLL_FUNC uw_SLCheckExecRebootInstr ( AiChar *puc_SectionEntry,
                                                               AiChar ac_OutTxt[SL_MAX_PARAM_SIZE] );
_API_EXTERN AiUInt16 _API_DLL_FUNC uw_SLCheckIfInstr         ( AiChar *puc_SectionEntry,
                                                               AiUInt32 aul_Params[5] );
_API_EXTERN AiUInt16 _API_DLL_FUNC uw_SLExecuteNvrChk        ( TY_MODULE_INFO *px_ModuleInfo, AiChar ac_Section[SL_MAX_SECTION_ENTRY_SIZE]);
_API_EXTERN AiUInt16 _API_DLL_FUNC uw_SLExecuteIfInstr       ( TY_MODULE_INFO *px_ModuleInfo,
                                                               AiChar ac_Section[SL_MAX_SECTION_ENTRY_SIZE]);
_API_EXTERN AiUInt16 _API_DLL_FUNC uw_SLInterpreteUPDEntry   ( TY_MODULE_INFO *px_ModuleInfo,
                                                               AiChar *puc_SectionEntry );
