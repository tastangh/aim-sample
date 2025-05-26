/* SPDX-FileCopyrightText: 2001-2022 AIM GmbH <info@aim-online.com> */
/* SPDX-License-Identifier: MIT */

/*! \file BWTS_API.h
 *
 * BWTS Header information of the API/APC Application Interface
 * 
 */

/******************************************/
/***  API1553-DS Instruction Codes      ***/
/******************************************/

/* 01.02.99 : BWTS Commands */
#define TG_API_BUF_BWTS_LD_MSG         0x5C 
#define TG_API_BUF_RAS_LD_MSG          0x5C /* 11.11.99 */
#define TG_API_IP_ADDR_INIT            0xE7
#define TG_API_IP_OUT_CHN_WRITE        0xE0
#define TG_API_IP_OUT_CHN_READ         0xE8 /* 01.04.99 */
#define TG_API_IP_IN_CHN_READ          0xE1
#define TG_API_SYS_REC_SS              0xE2
#define TG_API_SYS_REC_CNF             0xE3
#define TG_API_SYS_REC_STATUS          0xE4
#define TG_API_SYS_REC_COPY            0xE5
#define TG_API_SYS_STASK_CTRL          0xE6
#define TG_API_RAS_DSET_INIT           0xE9 /* 05.07.00 */
#define TG_API_RAS_DSET_WRITE          0xEA /* 05.07.00 */
#define TG_API_RAS_DSET_STATUS         0xEB /* 05.07.00 */

/******************************************/
/***  Ext Instruction Codes             ***/
/******************************************/

/* 01.02.99 : BWTS Commands */
#define TG_EXT_PCI_INI          1
#define TG_EXT_PCI_IP_LW_WRITE  2
#define TG_EXT_PCI_IP_LW_READ   3
#define TG_EXT_PCI_IP_ALLOC_MEM 4


/******************************************/
/***  01.02.99 : BWTS DEFs              ***/
/******************************************/

// Dig-out bit mask
#define API_BWTS_DOUT_RT_ADDR1       0x00000100L
#define API_BWTS_DOUT_RT_ADDR2       0x00000200L
#define API_BWTS_DOUT_RT_ADDR3       0x00000400L
#define API_BWTS_DOUT_RT_ADDR4       0x00000800L
#define API_BWTS_DOUT_RT_ADDR5       0x00001000L
#define API_BWTS_DOUT_RT_PARITY      0x00002000L
#define API_BWTS_DOUT_REL_CONSENT    0x00004000L
#define API_BWTS_DOUT_AC_OUT         0x00008000L
#define API_BWTS_DOUT_DC1_OUT        0x00010000L
#define API_BWTS_DOUT_DC2_OUT        0x00020000L
#define API_BWTS_DOUT_INC_PHASE      0x00040000L

#define API_BWTS_DOUT_CASE_RTADDR1   9
#define API_BWTS_DOUT_CASE_RTADDR2  10
#define API_BWTS_DOUT_CASE_RTADDR3  11
#define API_BWTS_DOUT_CASE_RTADDR4  12
#define API_BWTS_DOUT_CASE_RTADDR5  13
#define API_BWTS_DOUT_CASE_RTPAR    14
#define API_BWTS_DOUT_CASE_RELCON   15
#define API_BWTS_DOUT_CASE_ACOUT    16
#define API_BWTS_DOUT_CASE_DC1OUT   17
#define API_BWTS_DOUT_CASE_DC2OUT   18
#define API_BWTS_DOUT_CASE_INCPHS   19 
// ... TBDs ...                     32 
// set all RT discretes at once
#define API_BWTS_DOUT_CASE_RT_GRP  (32+1)

#define API_BWTS_DO_NOT_INVERT_SIGNAL 0L
#define API_BWTS_INVERT_SIGNAL        1L

// Dig-In bit mask
#define API_BWTS_DIN_REL_CONSENT     0x00000001
#define API_BWTS_DIN_INTERLOCK       0x00000002
#define API_BWTS_DIN_AC_IN_PHASE     0x00000004
#define API_BWTS_DIN_AC_OUT_PHASE    0x00000008
#define API_BWTS_DIN_DC1_PRESENT     0x00000010
#define API_BWTS_DIN_DC2_PRESENT     0x00000020

// IP Type
#define API_IP_APX_TYPE  1
#define API_IP_DIG_TYPE  2
#define API_IP_ANA_TYPE  3
#define API_IP_RS_TYPE   4

// Recording
#define API_REC_SRC_ALL  0
#define API_REC_SRC_MIL1 1
#define API_REC_SRC_MIL2 2
#define API_REC_SRC_AD   3
#define API_REC_SRC_RS   4

// BC Transfer Type
#define API_BWTS_XFER_TYPE_CYCL  0
#define API_BWTS_XFER_TYPE_ACYC  1
#define MAX_API_BWTS_MSG_QUEUE  64

//***************************************************************************
//  API1553-DS Functions - External References
//***************************************************************************
// Buffer Commands 
AI_LIB_FUNC AiReturn AI_CALL_CONV ApiCmdBufBWTSMsgLoad(AiUInt32 bModule, AiUInt8 biu, AiUInt16 bid,
                                       AiUInt8 xfer_mode, AiUInt8 res, AiUInt16 *data, AiUInt32 *st); /* 01.02.99 */

/* 25.05.00 : RAS extensions */
#ifdef _CMPL4RAS
AI_LIB_FUNC AiReturn AI_CALL_CONV ApiCmdRASDatasetInit(AiUInt32 bModule, AiUInt8 biu, AiUInt8 con, AiUInt16 dsid, AiUInt16 bid, AiUInt16 res);
AI_LIB_FUNC AiReturn AI_CALL_CONV ApiCmdRASDatasetWrite(AiUInt32 bModule, AiUInt8 biu, AiUInt16 dsid,AiUInt16 cnt, AiUInt16 res, AiUInt16 *data);
AI_LIB_FUNC AiReturn AI_CALL_CONV ApiCmdRASDatasetStatus(AiUInt32 bModule, AiUInt8 biu, AiUInt16 dsid,AiUInt16 *hfi_cnt, AiUInt16 *act_rdq, AiUInt16 *act_wrq, AiUInt16 *act_stat);

AI_LIB_FUNC AiReturn AI_CALL_CONV ApiRasReadRecData(AiUInt rec_src,AiUInt8 biu,
                                  TY_API_BM_REC *api_rec_stat, LPVOID lpBuf,AiUInt32 *lBytesRead);
#endif

/****************************************************************
 **    HATS related Functions
 ****************************************************************/
#ifdef _CMPL4HATS
AI_LIB_FUNC AiReturn AI_CALL_CONV ApiCmdTpmcIni           (AiUInt32 bModule, AiUInt8 mode);
AI_LIB_FUNC AiReturn AI_CALL_CONV ApiCmdTpmcDinRead       (AiUInt32 bModule, AiUInt16 con_in, AiUInt16 con_out, 
                                                           AiUInt16 *val_in, AiUInt16 *val_out_rdbk);
AI_LIB_FUNC AiReturn AI_CALL_CONV ApiCmdTpmcDoutWrite     (AiUInt32 bModule, AiUInt16 con, AiUInt16 mask, AiUInt16 val_out);
AI_LIB_FUNC AiReturn AI_CALL_CONV ApiCmdTpmcLedCon        (AiUInt32 bModule, AiUInt8 led, AiUInt8 con);
AI_LIB_FUNC AiReturn AI_CALL_CONV ApiCmdTpmcDacWrite      (AiUInt32 bModule, AiUInt16 con, AiUInt16 mask, AiInt16 *val_out);
AI_LIB_FUNC AiReturn AI_CALL_CONV ApiCmdTpmcDacRead       (AiUInt32 bModule, AiUInt16 con, AiInt16 *val_out_rdbk);
AI_LIB_FUNC AiReturn AI_CALL_CONV ApiCmdTpmcAdcRead       (AiUInt32 bModule, AiUInt16 con, AiInt16 *val_in);
AI_LIB_FUNC AiReturn AI_CALL_CONV ApiCmdTpmcTest          (AiUInt32 bModule, AiUInt8 con, AiUInt8 fct, AiUInt16 mask);
AI_LIB_FUNC AiReturn AI_CALL_CONV ApiCmdLvdtIni           (AiUInt32 bModule, AiUInt8 mode);
AI_LIB_FUNC AiReturn AI_CALL_CONV ApiCmdLvdtOutWrite      (AiUInt32 bModule, AiUInt16 con, AiUInt16 mask, AiInt16 *val_out);
AI_LIB_FUNC AiReturn AI_CALL_CONV ApiCmdLvdtOutRead       (AiUInt32 bModule, AiUInt16 con, AiInt16 *val_out_rdbk);
AI_LIB_FUNC AiReturn AI_CALL_CONV ApiCmdLvdtInRead        (AiUInt32 bModule, AiUInt16 con, AiInt16 *val_in);
#endif
/****************************************************************
 **    System related Functions (e.g. BWTS)
 ****************************************************************/
AI_LIB_FUNC AiReturn AI_CALL_CONV ApiCmdIPAddrInit(AiUInt32 bModule, AiUInt8 ip_type, AiUInt8 io_type, AiUInt8 chn, AiUInt8 res, AiUInt32 addr);
AI_LIB_FUNC AiReturn AI_CALL_CONV ApiCmdIPOutChnWrite(AiUInt32 bModule, AiUInt8 ip_type, AiUInt8 chn, AiUInt32 res, AiUInt32 val);
AI_LIB_FUNC AiReturn AI_CALL_CONV ApiCmdIPInChnRead(AiUInt32 bModule, AiUInt8 ip_type, AiUInt8 chn, AiUInt32 ctl, AiUInt32 *val);
/*AI_LIB_FUNC AiReturn AI_CALL_CONV ApiCmdIPOutChnRead(AiUInt32 bModule, AiUInt8 ip_type, AiUInt8 chn, AiUInt32 ctl, AiUInt32 *val);*/

AI_LIB_FUNC AiReturn AI_CALL_CONV ApiCmdSysRecSS(AiUInt32 bModule, AiUInt8 rec_src, AiUInt8 con, AiUInt8 mode, AiUInt8 res, AiUInt32 mode_val);
AI_LIB_FUNC AiReturn AI_CALL_CONV ApiCmdSysRecCnf(AiUInt32 bModule, AiUInt8 rec_src, AiUInt8 con, AiUInt8 chn_mask, AiUInt8 res1, AiUInt32 fsize, AiUInt32 res2);
AI_LIB_FUNC AiReturn AI_CALL_CONV ApiCmdSysRecStatus(AiUInt32 bModule, AiUInt8 rec_src, TY_API_BM_REC *prec);
AI_LIB_FUNC AiReturn AI_CALL_CONV ApiCmdSysRecCopy(AiUInt32 bModule, AiUInt8 rec_src, TY_API_BM_REC *prec);
AI_LIB_FUNC AiReturn AI_CALL_CONV ApiCmdSysSTaskCtrl(AiUInt32 bModule, AiUInt8 tmr, AiUInt8 con, AiUInt32 rate, AiUInt32 rsync, AiUInt32 ustep);


