/* SPDX-FileCopyrightText: 2016-2024 AIM GmbH <info@aim-online.com> */
/* SPDX-License-Identifier: MIT */

/*! \file AiMapDllToTsw.h
 *
 * This files can be used to map the Library calls directly to TSW function calls in cases where the TSW is part of the Library.
 * This feature is used by some customers to speed up the function calls and to ease the debugging on VME VxWorks Systems.
 */

/* #define API_CMD_STRUCT_RT_INI(x,y,z)   api_rt_ini( api_dev[x&MODULE_MASK], y, z ); */

#define API_CMD_STRUCT_RT_INI(x,y,z)                ApiIoStruct( x, (TY_MIL_COM*)y, (TY_MIL_COM_ACK*)z );
#define API_CMD_STRUCT_RT_CON(x,y,z)                ApiIoStruct( x, (TY_MIL_COM*)y, (TY_MIL_COM_ACK*)z );
#define API_CMD_STRUCT_RT_SA_CON(x,y,z)             ApiIoStruct( x, (TY_MIL_COM*)y, (TY_MIL_COM_ACK*)z );
#define API_CMD_STRUCT_RT_INI_RESP(x,y,z)           ApiIoStruct( x, (TY_MIL_COM*)y, (TY_MIL_COM_ACK*)z );
#define API_CMD_STRUCT_RT_INI_NXW(x,y,z)            ApiIoStruct( x, (TY_MIL_COM*)y, (TY_MIL_COM_ACK*)z );
#define API_CMD_STRUCT_RT_INI_LCW(x,y,z)            ApiIoStruct( x, (TY_MIL_COM*)y, (TY_MIL_COM_ACK*)z );
#define API_CMD_STRUCT_RT_INI_LSW(x,y,z)            ApiIoStruct( x, (TY_MIL_COM*)y, (TY_MIL_COM_ACK*)z );
#define API_CMD_STRUCT_RT_BH_DEF(x,y,z)             ApiIoStruct( x, (TY_MIL_COM*)y, (TY_MIL_COM_ACK*)z );
#define API_CMD_STRUCT_RT_BH_GET(x,y,z)             ApiIoStruct( x, (TY_MIL_COM*)y, (TY_MIL_COM_ACK*)z );
#define API_CMD_STRUCT_RT_SA_ERR_CON(x,y,z)         ApiIoStruct( x, (TY_MIL_COM*)y, (TY_MIL_COM_ACK*)z );
#define API_CMD_STRUCT_RT_SA_ERR_GET(x,y,z)         ApiIoStruct( x, (TY_MIL_COM*)y, (TY_MIL_COM_ACK*)z );
#define API_CMD_STRUCT_RT_DYTAG_DEF(x,y,z)          ApiIoStruct( x, (TY_MIL_COM*)y, (TY_MIL_COM_ACK*)z );
#define API_CMD_STRUCT_RT_DYTAG_GET(x,y,z)          ApiIoStruct( x, (TY_MIL_COM*)y, (TY_MIL_COM_ACK*)z );
#define API_CMD_STRUCT_RT_START(x,y,z)              ApiIoStruct( x, (TY_MIL_COM*)y, (TY_MIL_COM_ACK*)z );
#define API_CMD_STRUCT_RT_HALT(x,y,z)               ApiIoStruct( x, (TY_MIL_COM*)y, (TY_MIL_COM_ACK*)z );
#define API_CMD_STRUCT_RT_STATUS_READ(x,y,z)        ApiIoStruct( x, (TY_MIL_COM*)y, (TY_MIL_COM_ACK*)z );
#define API_CMD_STRUCT_RT_MSG_READ(x,y,z)           ApiIoStruct( x, (TY_MIL_COM*)y, (TY_MIL_COM_ACK*)z );
#define API_CMD_STRUCT_RT_MSG_READ_ALL(x,y,z)       ApiIoStruct( x, (TY_MIL_COM*)y, (TY_MIL_COM_ACK*)z );
#define API_CMD_STRUCT_RT_SA_MSG_READ(x,y,z)        ApiIoStruct( x, (TY_MIL_COM*)y, (TY_MIL_COM_ACK*)z );
#define API_CMD_STRUCT_RT_GLOBAL_CON(x,y,z)         ApiIoStruct( x, (TY_MIL_COM*)y, (TY_MIL_COM_ACK*)z );
#define API_CMD_STRUCT_RT_GET_SIM_INFO(x,y,z)       ApiIoStruct( x, (TY_MIL_COM*)y, (TY_MIL_COM_ACK*)z );
#define API_CMD_STRUCT_RT_SA_BH_GET(x,y,z)          ApiIoStruct( x, (TY_MIL_COM*)y, (TY_MIL_COM_ACK*)z );
#define API_CMD_STRUCT_RT_MODE_CTRL(x,y,z)          ApiIoStruct( x, (TY_MIL_COM*)y, (TY_MIL_COM_ACK*)z );
#define API_CMD_STRUCT_RT_DWC_SET(x,y,z)            ApiIoStruct( x, (TY_MIL_COM*)y, (TY_MIL_COM_ACK*)z );
#define API_CMD_STRUCT_RT_DWC_GET(x,y,z)            ApiIoStruct( x, (TY_MIL_COM*)y, (TY_MIL_COM_ACK*)z );

#define API_CMD_STRUCT_HS_RT_BH_DEF(x,y,z)          ApiIoStruct( x, (TY_MIL_COM*)y, (TY_MIL_COM_ACK*)z );
#define API_CMD_STRUCT_HS_RT_BH_GET(x,y,z)          ApiIoStruct( x, (TY_MIL_COM*)y, (TY_MIL_COM_ACK*)z );
#define API_CMD_STRUCT_HS_RT_MID_DEF(x,y,z)         ApiIoStruct( x, (TY_MIL_COM*)y, (TY_MIL_COM_ACK*)z );
#define API_CMD_STRUCT_HS_RT_MID_ERR_DEF(x,y,z)     ApiIoStruct( x, (TY_MIL_COM*)y, (TY_MIL_COM_ACK*)z );
#define API_CMD_STRUCT_HS_RT_MID_ERR_GET(x,y,z)     ApiIoStruct( x, (TY_MIL_COM*)y, (TY_MIL_COM_ACK*)z );
#define API_CMD_STRUCT_HS_RT_DYTAG_DEF(x,y,z)       ApiIoStruct( x, (TY_MIL_COM*)y, (TY_MIL_COM_ACK*)z );
#define API_CMD_STRUCT_HS_RT_DYTAG_GET(x,y,z)       ApiIoStruct( x, (TY_MIL_COM*)y, (TY_MIL_COM_ACK*)z );
#define API_CMD_STRUCT_HS_RT_STATUS_GET(x,y,z)      ApiIoStruct( x, (TY_MIL_COM*)y, (TY_MIL_COM_ACK*)z );
#define API_CMD_STRUCT_HS_RT_MSG_GET(x,y,z)         ApiIoStruct( x, (TY_MIL_COM*)y, (TY_MIL_COM_ACK*)z );
#define API_CMD_STRUCT_HS_RT_MID_BH_GET(x,y,z)      ApiIoStruct( x, (TY_MIL_COM*)y, (TY_MIL_COM_ACK*)z );
#define API_CMD_STRUCT_HS_RT_INI_START_DELAY(x,y,z) ApiIoStruct( x, (TY_MIL_COM*)y, (TY_MIL_COM_ACK*)z );
#define API_CMD_STRUCT_HS_RT_INI_PREAMBLES(x,y,z)   ApiIoStruct( x, (TY_MIL_COM*)y, (TY_MIL_COM_ACK*)z );
#define API_CMD_STRUCT_HS_RT_INI_LSW(x,y,z)         ApiIoStruct( x, (TY_MIL_COM*)y, (TY_MIL_COM_ACK*)z );
#define API_CMD_STRUCT_HS_RT_INI_LAW(x,y,z)         ApiIoStruct( x, (TY_MIL_COM*)y, (TY_MIL_COM_ACK*)z );
#define API_CMD_STRUCT_HS_RT_MODE_CONTROL(x,y,z)    ApiIoStruct( x, (TY_MIL_COM*)y, (TY_MIL_COM_ACK*)z );

#define API_CMD_STRUCT_LS_BC_XFER_ERROR(x,y,z)      ApiIoStruct( x, (TY_MIL_COM*)y, (TY_MIL_COM_ACK*)z );
#define API_CMD_STRUCT_LS_BC_XFER_DESC_GET(x,y,z)   ApiIoStruct( x, (TY_MIL_COM*)y, (TY_MIL_COM_ACK*)z );
#define API_CMD_STRUCT_LS_BC_XFER_DESC_MOD(x,y,z)   ApiIoStruct( x, (TY_MIL_COM*)y, (TY_MIL_COM_ACK*)z );
#define API_CMD_STRUCT_LS_BC_INI(x,y,z)             ApiIoStruct( x, (TY_MIL_COM*)y, (TY_MIL_COM_ACK*)z );
#define API_CMD_STRUCT_LS_BC_BH_DEF(x,y,z)          ApiIoStruct( x, (TY_MIL_COM*)y, (TY_MIL_COM_ACK*)z );
#define API_CMD_STRUCT_LS_BC_XFER_DEF(x,y,z)        ApiIoStruct( x, (TY_MIL_COM*)y, (TY_MIL_COM_ACK*)z );
#define API_CMD_STRUCT_LS_BC_XFER_CTRL(x,y,z)       ApiIoStruct( x, (TY_MIL_COM*)y, (TY_MIL_COM_ACK*)z );
#define API_CMD_STRUCT_LS_BC_START(x,y,z)           ApiIoStruct( x, (TY_MIL_COM*)y, (TY_MIL_COM_ACK*)z );
#define API_CMD_STRUCT_LS_BC_HALT(x,y,z)            ApiIoStruct( x, (TY_MIL_COM*)y, (TY_MIL_COM_ACK*)z );
#define API_CMD_STRUCT_LS_BC_MODIFY_DEF(x,y,z)      ApiIoStruct( x, (TY_MIL_COM*)y, (TY_MIL_COM_ACK*)z );
#define API_CMD_STRUCT_LS_BC_XFER_DEF_GET(x,y,z)    ApiIoStruct( x, (TY_MIL_COM*)y, (TY_MIL_COM_ACK*)z );
#define API_CMD_STRUCT_LS_BC_BH_READ(x,y,z)         ApiIoStruct( x, (TY_MIL_COM*)y, (TY_MIL_COM_ACK*)z );
#define API_CMD_STRUCT_LS_BC_BH_INFO(x,y,z)         ApiIoStruct( x, (TY_MIL_COM*)y, (TY_MIL_COM_ACK*)z );
#define API_CMD_STRUCT_LS_BC_MAJOR_FRAME_GET(x,y,z) ApiIoStruct( x, (TY_MIL_COM*)y, (TY_MIL_COM_ACK*)z );
#define API_CMD_STRUCT_LS_BC_MINOR_FRAME_GET(x,y,z) ApiIoStruct( x, (TY_MIL_COM*)y, (TY_MIL_COM_ACK*)z );
#define API_CMD_STRUCT_LS_BC_DYTAG_DEF(x,y,z)       ApiIoStruct( x, (TY_MIL_COM*)y, (TY_MIL_COM_ACK*)z );
#define API_CMD_STRUCT_LS_BC_DYTAG_DEF_GET(x,y,z)   ApiIoStruct( x, (TY_MIL_COM*)y, (TY_MIL_COM_ACK*)z );
#define API_CMD_STRUCT_LS_BC_MINOR_FRAME_DEF(x,y,z) ApiIoStruct( x, (TY_MIL_COM*)y, (TY_MIL_COM_ACK*)z );
#define API_CMD_STRUCT_LS_BC_MAJOR_FRAME_DEF(x,y,z) ApiIoStruct( x, (TY_MIL_COM*)y, (TY_MIL_COM_ACK*)z );
#define API_CMD_STRUCT_LS_BC_STATUS_READ(x,y,z)     ApiIoStruct( x, (TY_MIL_COM*)y, (TY_MIL_COM_ACK*)z );
#define API_CMD_STRUCT_LS_BC_XFER_READ(x,y,z)       ApiIoStruct( x, (TY_MIL_COM*)y, (TY_MIL_COM_ACK*)z );
#define API_CMD_STRUCT_LS_BC_SRVW_REQ_READ(x,y,z)   ApiIoStruct( x, (TY_MIL_COM*)y, (TY_MIL_COM_ACK*)z );
#define API_CMD_STRUCT_LS_BC_SRVW_REQ_CON(x,y,z)    ApiIoStruct( x, (TY_MIL_COM*)y, (TY_MIL_COM_ACK*)z );
#define API_CMD_STRUCT_LS_BC_ACYC_PREP(x,y,z)       ApiIoStruct( x, (TY_MIL_COM*)y, (TY_MIL_COM_ACK*)z );
#define API_CMD_STRUCT_LS_BC_ACYC_SEND(x,y,z)       ApiIoStruct( x, (TY_MIL_COM*)y, (TY_MIL_COM_ACK*)z );
#define API_CMD_STRUCT_LS_BC_ACYC_PREP_AND_SEND_TRANSFER(x,y,z)       ApiIoStruct( x, (TY_MIL_COM*)y, (TY_MIL_COM_ACK*)z );
#define API_CMD_STRUCT_LS_BC_MODE_CTRL(x,y,z)       ApiIoStruct( x, (TY_MIL_COM*)y, (TY_MIL_COM_ACK*)z );
#define API_CMD_STRUCT_LS_BC_TRAFFIC_OVERRIDE_CONTROL(x,y,z)       ApiIoStruct( x, (TY_MIL_COM*)y, (TY_MIL_COM_ACK*)z );
#define API_CMD_STRUCT_LS_BC_TRAFFIC_OVERRIDE_GET(x,y,z)           ApiIoStruct( x, (TY_MIL_COM*)y, (TY_MIL_COM_ACK*)z );


#define API_CMD_STRUCT_HS_BC_XFER_ERROR(x,y,z)      ApiIoStruct( x, (TY_MIL_COM*)y, (TY_MIL_COM_ACK*)z );
#define API_CMD_STRUCT_HS_BC_BH_DEF(x,y,z)          ApiIoStruct( x, (TY_MIL_COM*)y, (TY_MIL_COM_ACK*)z );
#define API_CMD_STRUCT_HS_BC_BH_READ(x,y,z)         ApiIoStruct( x, (TY_MIL_COM*)y, (TY_MIL_COM_ACK*)z );
#define API_CMD_STRUCT_HS_BC_XFER_DEF(x,y,z)        ApiIoStruct( x, (TY_MIL_COM*)y, (TY_MIL_COM_ACK*)z );
#define API_CMD_STRUCT_HS_BC_XFER_DEF_GET(x,y,z)    ApiIoStruct( x, (TY_MIL_COM*)y, (TY_MIL_COM_ACK*)z );
#define API_CMD_STRUCT_HS_BC_DYTAG_DEF(x,y,z)       ApiIoStruct( x, (TY_MIL_COM*)y, (TY_MIL_COM_ACK*)z );
#define API_CMD_STRUCT_HS_BC_DYTAG_GET(x,y,z)       ApiIoStruct( x, (TY_MIL_COM*)y, (TY_MIL_COM_ACK*)z );
#define API_CMD_STRUCT_HS_BC_XFER_READ(x,y,z)       ApiIoStruct( x, (TY_MIL_COM*)y, (TY_MIL_COM_ACK*)z );
#define API_CMD_STRUCT_HS_BC_STATUS_READ(x,y,z)     ApiIoStruct( x, (TY_MIL_COM*)y, (TY_MIL_COM_ACK*)z );
#define API_CMD_STRUCT_HS_BC_MODE_CTRL(x,y,z)       ApiIoStruct( x, (TY_MIL_COM*)y, (TY_MIL_COM_ACK*)z );

#define API_CMD_STRUCT_EF_BC_RESP_SIM_DEF(x,y,z)    ApiIoStruct( x, (TY_MIL_COM*)y, (TY_MIL_COM_ACK*)z );
#define API_CMD_STRUCT_EF_BC_INTERNAL_DEF(x,y,z)    ApiIoStruct( x, (TY_MIL_COM*)y, (TY_MIL_COM_ACK*)z );


#define API_CMD_STRUCT_BM_INI(x,y,z)                ApiIoStruct( x, (TY_MIL_COM*)y, (TY_MIL_COM_ACK*)z );
#define API_CMD_STRUCT_BM_INI_INTR(x,y,z)           ApiIoStruct( x, (TY_MIL_COM*)y, (TY_MIL_COM_ACK*)z );
#define API_CMD_STRUCT_BM_INI_CAP(x,y,z)            ApiIoStruct( x, (TY_MIL_COM*)y, (TY_MIL_COM_ACK*)z );
#define API_CMD_STRUCT_BM_INI_SWXM(x,y,z)           ApiIoStruct( x, (TY_MIL_COM*)y, (TY_MIL_COM_ACK*)z );
#define API_CMD_STRUCT_BM_START(x,y,z)              ApiIoStruct( x, (TY_MIL_COM*)y, (TY_MIL_COM_ACK*)z );
#define API_CMD_STRUCT_BM_HALT(x,y,z)               ApiIoStruct( x, (TY_MIL_COM*)y, (TY_MIL_COM_ACK*)z );
#define API_CMD_STRUCT_BM_STATUS_READ(x,y,z)        ApiIoStruct( x, (TY_MIL_COM*)y, (TY_MIL_COM_ACK*)z );
#define API_CMD_STRUCT_BM_ACT_READ(x,y,z)           ApiIoStruct( x, (TY_MIL_COM*)y, (TY_MIL_COM_ACK*)z );
#define API_CMD_STRUCT_BM_STACK_PTR_READ(x,y,z)     ApiIoStruct( x, (TY_MIL_COM*)y, (TY_MIL_COM_ACK*)z );
#define API_CMD_STRUCT_BM_STACK_ENTRY_READ(x,y,z)   ApiIoStruct( x, (TY_MIL_COM*)y, (TY_MIL_COM_ACK*)z );
#define API_CMD_STRUCT_BM_STACK_ENTRY_FIND(x,y,z)   ApiIoStruct( x, (TY_MIL_COM*)y, (TY_MIL_COM_ACK*)z );
#define API_CMD_STRUCT_BM_REC_STATUS_READ(x,y,z)    ApiIoStruct( x, (TY_MIL_COM*)y, (TY_MIL_COM_ACK*)z );
#define API_CMD_STRUCT_BM_COPY_REC_DAT(x,y,z)       ApiIoStruct( x, (TY_MIL_COM*)y, (TY_MIL_COM_ACK*)z );
#define API_CMD_STRUCT_BM_INI_MSG_FLT(x,y,z)        ApiIoStruct( x, (TY_MIL_COM*)y, (TY_MIL_COM_ACK*)z );
#define API_CMD_STRUCT_BM_INI_MSG_ILL(x,y,z)        ApiIoStruct( x, (TY_MIL_COM*)y, (TY_MIL_COM_ACK*)z );
#define API_CMD_STRUCT_BM_STACK_ENTRY_READ(x,y,z)   ApiIoStruct( x, (TY_MIL_COM*)y, (TY_MIL_COM_ACK*)z );
#define API_CMD_STRUCT_BM_INI_FTW(x,y,z)            ApiIoStruct( x, (TY_MIL_COM*)y, (TY_MIL_COM_ACK*)z );
#define API_CMD_STRUCT_BM_INI_TIW(x,y,z)            ApiIoStruct( x, (TY_MIL_COM*)y, (TY_MIL_COM_ACK*)z );
#define API_CMD_STRUCT_BM_INI_TCI(x,y,z)            ApiIoStruct( x, (TY_MIL_COM*)y, (TY_MIL_COM_ACK*)z );
#define API_CMD_STRUCT_BM_INI_TCB(x,y,z)            ApiIoStruct( x, (TY_MIL_COM*)y, (TY_MIL_COM_ACK*)z );
#define API_CMD_STRUCT_BM_RT_ACT_READ(x,y,z)        ApiIoStruct( x, (TY_MIL_COM*)y, (TY_MIL_COM_ACK*)z );
#define API_CMD_STRUCT_BM_RT_SA_ACT_READ(x,y,z)     ApiIoStruct( x, (TY_MIL_COM*)y, (TY_MIL_COM_ACK*)z );
#define API_CMD_STRUCT_BM_INI_MSG_REC_FLT(x,y,z)    ApiIoStruct( x, (TY_MIL_COM*)y, (TY_MIL_COM_ACK*)z );
#define API_CMD_STRUCT_BM_MSG_REC_FLT(x,y,z)        ApiIoStruct( x, (TY_MIL_COM*)y, (TY_MIL_COM_ACK*)z );
#define API_CMD_STRUCT_BM_DYNTAG_MON_DEF(x,y,z)     ApiIoStruct( x, (TY_MIL_COM*)y, (TY_MIL_COM_ACK*)z );
#define API_CMD_STRUCT_BM_DYNTAG_MON_READ(x,y,z)    ApiIoStruct( x, (TY_MIL_COM*)y, (TY_MIL_COM_ACK*)z );

#define API_CMD_STRUCT_HS_BM_INI_INTR(x,y,z)        ApiIoStruct( x, (TY_MIL_COM*)y, (TY_MIL_COM_ACK*)z );
#define API_CMD_STRUCT_HS_BM_INI_CAP(x,y,z)         ApiIoStruct( x, (TY_MIL_COM*)y, (TY_MIL_COM_ACK*)z );
#define API_CMD_STRUCT_HS_BM_INI_FLT(x,y,z)         ApiIoStruct( x, (TY_MIL_COM*)y, (TY_MIL_COM_ACK*)z );
#define API_CMD_STRUCT_HS_BM_STATUS_READ(x,y,z)     ApiIoStruct( x, (TY_MIL_COM*)y, (TY_MIL_COM_ACK*)z );
#define API_CMD_STRUCT_HS_BM_TRG_CON(x,y,z)         ApiIoStruct( x, (TY_MIL_COM*)y, (TY_MIL_COM_ACK*)z );
#define API_CMD_STRUCT_HS_BM_ACT_READ(x,y,z)        ApiIoStruct( x, (TY_MIL_COM*)y, (TY_MIL_COM_ACK*)z );
#define API_CMD_STRUCT_HS_BM_RT_ACT_READ(x,y,z)     ApiIoStruct( x, (TY_MIL_COM*)y, (TY_MIL_COM_ACK*)z );
#define API_CMD_STRUCT_HS_BM_RT_MID_ACT_READ(x,y,z) ApiIoStruct( x, (TY_MIL_COM*)y, (TY_MIL_COM_ACK*)z );
#define API_CMD_STRUCT_HS_BM_PTR_READ(x,y,z)        ApiIoStruct( x, (TY_MIL_COM*)y, (TY_MIL_COM_ACK*)z );
#define API_CMD_STRUCT_HS_BM_STACK_READ(x,y,z)      ApiIoStruct( x, (TY_MIL_COM*)y, (TY_MIL_COM_ACK*)z );
#define API_CMD_STRUCT_HS_BM_STACK_FIND(x,y,z)      ApiIoStruct( x, (TY_MIL_COM*)y, (TY_MIL_COM_ACK*)z );
#define API_CMD_STRUCT_HS_BM_REC_STATUS(x,y,z)      ApiIoStruct( x, (TY_MIL_COM*)y, (TY_MIL_COM_ACK*)z );
#define API_CMD_STRUCT_HS_BM_COPY_REC_DAT(x,y,z)    ApiIoStruct( x, (TY_MIL_COM*)y, (TY_MIL_COM_ACK*)z );
#define API_CMD_STRUCT_HS_BM_DYNTAG_DEF(x,y,z)      ApiIoStruct( x, (TY_MIL_COM*)y, (TY_MIL_COM_ACK*)z );
#define API_CMD_STRUCT_HS_BM_DYNTAG_READ(x,y,z)     ApiIoStruct( x, (TY_MIL_COM*)y, (TY_MIL_COM_ACK*)z );

#define API_CMD_STRUCT_BM_DATAQUEUE_OPEN(x,y,z)     ApiIoStruct( x, (TY_MIL_COM*)y, (TY_MIL_COM_ACK*)z );
#define API_CMD_STRUCT_BM_DATAQUEUE_CONTROL(x,y,z)  ApiIoStruct( x, (TY_MIL_COM*)y, (TY_MIL_COM_ACK*)z );
#define API_CMD_STRUCT_BM_DATAQUEUE_CLOSE(x,y,z)    ApiIoStruct( x, (TY_MIL_COM*)y, (TY_MIL_COM_ACK*)z );



#define API_CMD_STRUCT_SYS_CAL_SIG_CON(x,y,z)       ApiIoStruct( x, (TY_MIL_COM*)y, (TY_MIL_COM_ACK*)z );
#define API_CMD_STRUCT_SYS_CAL_CPL_CON(x,y,z)       ApiIoStruct( x, (TY_MIL_COM*)y, (TY_MIL_COM_ACK*)z );
#define API_CMD_STRUCT_SYS_CAL_XMT_CON(x,y,z)       ApiIoStruct( x, (TY_MIL_COM*)y, (TY_MIL_COM_ACK*)z );
#define API_CMD_STRUCT_SYS_CAL_TRANS(x,y,z)         ApiIoStruct( x, (TY_MIL_COM*)y, (TY_MIL_COM_ACK*)z );

#define API_CMD_STRUCT_HS_SYS_CAL_CON(x,y,z)            ApiIoStruct( x, (TY_MIL_COM*)y, (TY_MIL_COM_ACK*)z );
#define API_CMD_STRUCT_HS_SYS_CAL_CPL_CON(x,y,z)        ApiIoStruct( x, (TY_MIL_COM*)y, (TY_MIL_COM_ACK*)z );
#define API_CMD_STRUCT_HS_SYS_CAL_FOP_CON(x,y,z)        ApiIoStruct( x, (TY_MIL_COM*)y, (TY_MIL_COM_ACK*)z );
#define API_CMD_STRUCT_HS_SYS_CAL_FOFE_DISCRETE(x,y,z)  ApiIoStruct( x, (TY_MIL_COM*)y, (TY_MIL_COM_ACK*)z );



#define API_CMD_STRUCT_SYS_PING(x,y,z)                  ApiIoStruct( x, (TY_MIL_COM*)y, (TY_MIL_COM_ACK*)z );
#define API_CMD_STRUCT_SYS_MALLOC(x,y,z)                ApiIoStruct( x, (TY_MIL_COM*)y, (TY_MIL_COM_ACK*)z );
#define API_CMD_STRUCT_SYS_FREE(x,y,z)                  ApiIoStruct( x, (TY_MIL_COM*)y, (TY_MIL_COM_ACK*)z );
#define API_CMD_STRUCT_SYS_GET_BOARD_INFO(x,y,z)        ApiIoStruct( x, (TY_MIL_COM*)y, (TY_MIL_COM_ACK*)z );
#define API_CMD_STRUCT_SYS_GET_MEM_LOCATION(x,y,z)      ApiIoStruct( x, (TY_MIL_COM*)y, (TY_MIL_COM_ACK*)z );
#define API_CMD_STRUCT_SYS_UTIL(x,y,z)                  ApiIoStruct( x, (TY_MIL_COM*)y, (TY_MIL_COM_ACK*)z );
#define API_CMD_STRUCT_SYS_MEM_PART_SET(x,y,z)          ApiIoStruct( x, (TY_MIL_COM*)y, (TY_MIL_COM_ACK*)z );
#define API_CMD_STRUCT_SYS_MEM_PART_GET(x,y,z)          ApiIoStruct( x, (TY_MIL_COM*)y, (TY_MIL_COM_ACK*)z );
#define API_CMD_STRUCT_SYS_SYSTAG_CON(x,y,z)            ApiIoStruct( x, (TY_MIL_COM*)y, (TY_MIL_COM_ACK*)z );
#define API_CMD_STRUCT_SYS_SYSTAG_DEF(x,y,z)            ApiIoStruct( x, (TY_MIL_COM*)y, (TY_MIL_COM_ACK*)z );
#define API_CMD_STRUCT_SYS_MILBUS_PROT(x,y,z)           ApiIoStruct( x, (TY_MIL_COM*)y, (TY_MIL_COM_ACK*)z );
#define API_CMD_STRUCT_SYS_RESP_TOUT_DEF(x,y,z)         ApiIoStruct( x, (TY_MIL_COM*)y, (TY_MIL_COM_ACK*)z );
#define API_CMD_STRUCT_SYS_PXI_CON(x,y,z)               ApiIoStruct( x, (TY_MIL_COM*)y, (TY_MIL_COM_ACK*)z );
#define API_CMD_STRUCT_SYS_PXI_GEO_ADDR(x,y,z)          ApiIoStruct( x, (TY_MIL_COM*)y, (TY_MIL_COM_ACK*)z );
#define API_CMD_STRUCT_SYS_DISCRETES_READ(x,y,z)        ApiIoStruct( x, (TY_MIL_COM*)y, (TY_MIL_COM_ACK*)z );
#define API_CMD_STRUCT_SYS_DISCRETES_CONFIG_READ(x,y,z) ApiIoStruct( x, (TY_MIL_COM*)y, (TY_MIL_COM_ACK*)z );
#define API_CMD_STRUCT_SYS_DISCRETES_WRITE(x,y,z)       ApiIoStruct( x, (TY_MIL_COM*)y, (TY_MIL_COM_ACK*)z );
#define API_CMD_STRUCT_SYS_DISCRETES_INIT(x,y,z)        ApiIoStruct( x, (TY_MIL_COM*)y, (TY_MIL_COM_ACK*)z );
#define API_CMD_STRUCT_SYS_DISCRETES_REC_INIT(x,y,z)    ApiIoStruct( x, (TY_MIL_COM*)y, (TY_MIL_COM_ACK*)z );
#define API_CMD_STRUCT_SYS_SYNC_COUNTER_SET(x,y,z)      ApiIoStruct( x, (TY_MIL_COM*)y, (TY_MIL_COM_ACK*)z );
#define API_CMD_STRUCT_SYS_SYNC_COUNTER_GET(x,y,z)      ApiIoStruct( x, (TY_MIL_COM*)y, (TY_MIL_COM_ACK*)z );
#define API_CMD_STRUCT_SYS_IRIG_GET(x,y,z)              ApiIoStruct( x, (TY_MIL_COM*)y, (TY_MIL_COM_ACK*)z );
#define API_CMD_STRUCT_SYS_IRIG_STATUS_GET(x,y,z)       ApiIoStruct( x, (TY_MIL_COM*)y, (TY_MIL_COM_ACK*)z );
#define API_CMD_STRUCT_SYS_IRIG_SET(x,y,z)              ApiIoStruct( x, (TY_MIL_COM*)y, (TY_MIL_COM_ACK*)z );
#define API_CMD_STRUCT_SYS_IRIG_SOURCE_SET(x,y,z)       ApiIoStruct( x, (TY_MIL_COM*)y, (TY_MIL_COM_ACK*)z );
#define API_CMD_STRUCT_SYS_RESET(x,y,z)                 ApiIoStruct( x, (TY_MIL_COM*)y, (TY_MIL_COM_ACK*)z );
#define API_CMD_STRUCT_SYS_RESET_INFO(x,y,z)            ApiIoStruct( x, (TY_MIL_COM*)y, (TY_MIL_COM_ACK*)z );
#define API_CMD_SYS_TRIGGER_EDGE_INPUT_SET(x,y,z)       ApiIoStruct( x, (TY_MIL_COM*)y, (TY_MIL_COM_ACK*)z );
#define API_CMD_SYS_TRIGGER_EDGE_INPUT_GET(x,y,z)       ApiIoStruct( x, (TY_MIL_COM*)y, (TY_MIL_COM_ACK*)z );
#define API_CMD_STRUCT_SYS_DISCRETES_INFO(x,y,z)        ApiIoStruct( x, (TY_MIL_COM*)y, (TY_MIL_COM_ACK*)z );
#define API_CMD_SYS_TRIGGER_DIGITAL_LOOP_CON(x,y,z)     ApiIoStruct( x, (TY_MIL_COM*)y, (TY_MIL_COM_ACK*)z );
#define API_CMD_SYS_TRIGGER_DIGITAL_LOOP_GET(x,y,z)     ApiIoStruct( x, (TY_MIL_COM*)y, (TY_MIL_COM_ACK*)z );





#define API_CMD_STRUCT_HS_SYS_SYSTAG_CON(x,y,z)     ApiIoStruct( x, (TY_MIL_COM*)y, (TY_MIL_COM_ACK*)z );
#define API_CMD_STRUCT_HS_SYS_SYSTAG_DEF(x,y,z)     ApiIoStruct( x, (TY_MIL_COM*)y, (TY_MIL_COM_ACK*)z );
#define API_CMD_STRUCT_HS_SYS_RX_TIMEOUT(x,y,z)     ApiIoStruct( x, (TY_MIL_COM*)y, (TY_MIL_COM_ACK*)z );







#define API_CMD_STRUCT_SYS_TRACK_DEF(x,y,z)         ApiIoStruct( x, (TY_MIL_COM*)y, (TY_MIL_COM_ACK*)z );
#define API_CMD_STRUCT_SYS_TRACK_READ(x,y,z)        ApiIoStruct( x, (TY_MIL_COM*)y, (TY_MIL_COM_ACK*)z );
#define API_CMD_STRUCT_SYS_TRACK_SCAN(x,y,z)        ApiIoStruct( x, (TY_MIL_COM*)y, (TY_MIL_COM_ACK*)z );
#define API_CMD_STRUCT_SYS_TRACK_ALLOC(x,y,z)       ApiIoStruct( x, (TY_MIL_COM*)y, (TY_MIL_COM_ACK*)z );



#define API_CMD_STRUCT_HS_SYS_TRACK_DEF(x,y,z)      ApiIoStruct( x, (TY_MIL_COM*)y, (TY_MIL_COM_ACK*)z );
#define API_CMD_STRUCT_HS_SYS_TRACK_READ(x,y,z)     ApiIoStruct( x, (TY_MIL_COM*)y, (TY_MIL_COM_ACK*)z );
#define API_CMD_STRUCT_HS_SYS_TRACK_SCAN(x,y,z)     ApiIoStruct( x, (TY_MIL_COM*)y, (TY_MIL_COM_ACK*)z );
#define API_CMD_STRUCT_HS_SYS_TRACK_ALLOC(x,y,z)    ApiIoStruct( x, (TY_MIL_COM*)y, (TY_MIL_COM_ACK*)z );


#define API_CMD_STRUCT_SYS_FIFO_INI(x,y,z)          ApiIoStruct( x, (TY_MIL_COM*)y, (TY_MIL_COM_ACK*)z );
#define API_CMD_STRUCT_SYS_FIFO_WRITE(x,y,z)        ApiIoStruct( x, (TY_MIL_COM*)y, (TY_MIL_COM_ACK*)z );
#define API_CMD_STRUCT_SYS_FIFO_STATUS(x,y,z)       ApiIoStruct( x, (TY_MIL_COM*)y, (TY_MIL_COM_ACK*)z );
#define API_CMD_STRUCT_SYS_FIFO_RT_ASSIGN(x,y,z)    ApiIoStruct( x, (TY_MIL_COM*)y, (TY_MIL_COM_ACK*)z );
#define API_CMD_STRUCT_SYS_FIFO_BC_ASSIGN(x,y,z)    ApiIoStruct( x, (TY_MIL_COM*)y, (TY_MIL_COM_ACK*)z );


#define API_CMD_STRUCT_SYS_EXEC(x,y,z)              ApiIoStruct( x, (TY_MIL_COM*)y, (TY_MIL_COM_ACK*)z );
#define API_CMD_STRUCT_SYS_BITE(x,y,z)              ApiIoStruct( x, (TY_MIL_COM*)y, (TY_MIL_COM_ACK*)z );
#define API_CMD_STRUCT_SYS_INI(x,y,z)               ApiIoStruct( x, (TY_MIL_COM*)y, (TY_MIL_COM_ACK*)z );


#define API_CMD_STRUCT_HS_BUF_WRITE(x,y,z)          ApiIoStruct( x, (TY_MIL_COM*)y, (TY_MIL_COM_ACK*)z );
#define API_CMD_STRUCT_HS_BUF_WRITE_BLOCK(x,y,z)    ApiIoStruct( x, (TY_MIL_COM*)y, (TY_MIL_COM_ACK*)z );
#define API_CMD_STRUCT_HS_BUF_HOST_CONTROL(x,y,z)   ApiIoStruct( x, (TY_MIL_COM*)y, (TY_MIL_COM_ACK*)z );
#define API_CMD_STRUCT_HS_BUF_READ(x,y,z)           ApiIoStruct( x, (TY_MIL_COM*)y, (TY_MIL_COM_ACK*)z );
#define API_CMD_STRUCT_HS_BUF_DEF(x,y,z)            ApiIoStruct( x, (TY_MIL_COM*)y, (TY_MIL_COM_ACK*)z );


#define API_CMD_STRUCT_REP_INI(x,y,z)               ApiIoStruct( x, (TY_MIL_COM*)y, (TY_MIL_COM_ACK*)z );
#define API_CMD_STRUCT_REP_START(x,y,z)             ApiIoStruct( x, (TY_MIL_COM*)y, (TY_MIL_COM_ACK*)z );
#define API_CMD_STRUCT_REP_HALT(x,y,z)              ApiIoStruct( x, (TY_MIL_COM*)y, (TY_MIL_COM_ACK*)z );
#define API_CMD_STRUCT_REP_RT_CON(x,y,z)            ApiIoStruct( x, (TY_MIL_COM*)y, (TY_MIL_COM_ACK*)z );
#define API_CMD_STRUCT_REP_STATUS(x,y,z)            ApiIoStruct( x, (TY_MIL_COM*)y, (TY_MIL_COM_ACK*)z );

#define API_CMD_STRUCT_HS_REP_INI(x,y,z)            ApiIoStruct( x, (TY_MIL_COM*)y, (TY_MIL_COM_ACK*)z );
#define API_CMD_STRUCT_HS_REP_RT_CON(x,y,z)         ApiIoStruct( x, (TY_MIL_COM*)y, (TY_MIL_COM_ACK*)z );
#define API_CMD_STRUCT_HS_REP_STATUS(x,y,z)         ApiIoStruct( x, (TY_MIL_COM*)y, (TY_MIL_COM_ACK*)z );


#define API_CMD_STRUCT_LS_BUF_DEF(x,y,z)            ApiIoStruct( x, (TY_MIL_COM*)y, (TY_MIL_COM_ACK*)z );
#define API_CMD_STRUCT_LS_BUF_READ(x,y,z)           ApiIoStruct( x, (TY_MIL_COM*)y, (TY_MIL_COM_ACK*)z );
#define API_CMD_STRUCT_LS_BUF_WRITE(x,y,z)          ApiIoStruct( x, (TY_MIL_COM*)y, (TY_MIL_COM_ACK*)z );
#define API_CMD_STRUCT_LS_BUF_C1760_CON(x,y,z)      ApiIoStruct( x, (TY_MIL_COM*)y, (TY_MIL_COM_ACK*)z );
#define API_CMD_STRUCT_LS_BUF_DATASET_WRITE(x,y,z)  ApiIoStruct( x, (TY_MIL_COM*)y, (TY_MIL_COM_ACK*)z );
#define API_CMD_STRUCT_LS_BUF_DATASET_READ(x,y,z)   ApiIoStruct( x, (TY_MIL_COM*)y, (TY_MIL_COM_ACK*)z );
#define API_CMD_STRUCT_LS_BUF_WRITE_BLOCK(x,y,z)    ApiIoStruct( x, (TY_MIL_COM*)y, (TY_MIL_COM_ACK*)z );
#define API_CMD_STRUCT_LS_BUF_HOST_CONTROL(x,y,z)   ApiIoStruct( x, (TY_MIL_COM*)y, (TY_MIL_COM_ACK*)z );





#define API_CMD_STRUCT_CUSTOM_DTS_GEN_IO_TEST_SEQ(x,y,z)   ApiIoStruct( x, (TY_MIL_COM*)y, (TY_MIL_COM_ACK*)z );
