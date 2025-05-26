/* SPDX-FileCopyrightText: 2001-2022 AIM GmbH <info@aim-online.com> */
/* SPDX-License-Identifier: MIT */

/*! \file ApiBwts.h
 *
 * Prototypes for Windows 32 Bit specific I/O routines.
 * 
 */


#include "AiOs.h"
#include "Ai_cdef.h"
#ifdef _API3910
#include "Ai3910i_def.h"
#include "Ai3910i_fnc.h"
#include "Ai1553_Internal_def.h"
#include "Ai3910_Internal_def.h"
#else
#include "Ai1553i_def.h"
#include "Ai1553i_fnc.h"
#include "Ai1553_Internal_def.h"
#endif
#include "BWTS_API.H"
#include "DEFS_V.H"

extern AiInt16 _ApiCmdSetSysDbgLevel(AiUInt32 bModule, AiUInt32 ul_DbgLevel);



//***************************************************************************
//***
//***            A P I 1 5 5 3 - D S    F U N C T I O N S
//***
//***************************************************************************

//***************************************************************************
//***       B W T S     C O M M A N D S
//***************************************************************************
//***************************************************************************
//
//   Module : APC_API                  Submodule : ApiCmdIPAddrInit
//
//   Author : Marco Motz               Project   : API1553-DS
//
//---------------------------------------------------------------------------
//   Create : 01.02.99   Update : 01.02.99
//---------------------------------------------------------------------------
//    Descriptions
//    ------------
//    Inputs    :
//
//    Outputs   : Instruction execution status [status]
//
//    Description :
//
//***************************************************************************

AI_LIB_FUNC AiReturn AI_CALL_CONV ApiCmdIPAddrInit(AiUInt32 bModule, AiUInt8 ip_type, AiUInt8 io_type, AiUInt8 chn, AiUInt8 res, AiUInt32 addr)
{
    AiInt16 status;
    AiUInt8 info[8];

    info[0] = ip_type;
    info[1] = io_type;
    info[2] = chn;
    info[3] = res;
    info[4] = (AiUInt8)(addr >> 24);
    info[5] = (AiUInt8)(addr >> 16);
    info[6] = (AiUInt8)(addr >> 8);
    info[7] = (AiUInt8)(addr);

    status = ApiIo(bModule & ~API_STREAM_MASK, 0, TG_API_IP_ADDR_INIT, TG_ACK,
        &info[0], 8, w_p, 0,
        b_p, &tg_os, w_p, &tg_os, lw_p, &tg_os);

    return(status);

} /* end: ApiCmdIPAddrInit */

//***************************************************************************
//
//   Module : APC_API                  Submodule : ApiCmdIPOutChnWrite
//
//   Author : Marco Motz               Project   : API1553-DS
//
//---------------------------------------------------------------------------
//   Create : 01.02.99   Update : 01.02.99
//---------------------------------------------------------------------------
//    Descriptions
//    ------------
//    Inputs    :
//
//    Outputs   : Instruction execution status [status]
//
//    Description :
//
//***************************************************************************

AI_LIB_FUNC AiReturn AI_CALL_CONV ApiCmdIPOutChnWrite(AiUInt32 bModule, AiUInt8 ip_type, AiUInt8 chn, AiUInt32 res, AiUInt32 val)
{
    AiInt16 status;
    AiUInt8 info[10];

    info[0] = ip_type;
    info[1] = chn;
    info[2] = (AiUInt8)(res >> 24);
    info[3] = (AiUInt8)(res >> 16);
    info[4] = (AiUInt8)(res >> 8);
    info[5] = (AiUInt8)(res);
    info[6] = (AiUInt8)(val >> 24);
    info[7] = (AiUInt8)(val >> 16);
    info[8] = (AiUInt8)(val >> 8);
    info[9] = (AiUInt8)(val);

    status = ApiIo(bModule & ~API_STREAM_MASK, 0, TG_API_IP_OUT_CHN_WRITE, TG_ACK,
        &info[0], 10, w_p, 0,
        b_p, &tg_os, w_p, &tg_os, lw_p, &tg_os);

    return(status);

} // end: ApiCmdIPOutChnWrite

//***************************************************************************
//
//   Module : APC_API                  Submodule : ApiCmdIPInChnRead
//
//   Author : Marco Motz               Project   : API1553-DS
//
//---------------------------------------------------------------------------
//   Create : 01.02.99   Update : 01.02.99
//---------------------------------------------------------------------------
//    Descriptions
//    ------------
//    Inputs    :
//
//    Outputs   : Instruction execution status [status]
//
//    Description :
//
//***************************************************************************

AI_LIB_FUNC AiReturn AI_CALL_CONV ApiCmdIPInChnRead(AiUInt32 bModule, AiUInt8 ip_type, AiUInt8 chn, AiUInt32 ctl, AiUInt32 *val)
{
    AiInt16 status;
    AiUInt8 info[6];
    AiUInt32 ret_val;
    AiInt16 in_lword_size;

    in_lword_size = 0;

    info[0] = ip_type;
    info[1] = chn;
    info[2] = (AiUInt8)(ctl >> 24);
    info[3] = (AiUInt8)(ctl >> 16);
    info[4] = (AiUInt8)(ctl >> 8);
    info[5] = (AiUInt8)(ctl);

    status = ApiIo(bModule & ~API_STREAM_MASK, 0, TG_API_IP_IN_CHN_READ, TG_ACK2,
        &info[0], 6, w_p, 0,
        b_p, &tg_os, w_p, &tg_os, &ret_val, &in_lword_size);

    *val = ret_val;

    return(status);

} // end: ApiCmdIPInChnRead

//***************************************************************************
//
//   Module : APC_API                  Submodule : ApiCmdSysRecSS
//
//   Author : Marco Motz               Project   : API1553-DS
//
//---------------------------------------------------------------------------
//   Create : 01.02.99   Update : 01.02.99
//---------------------------------------------------------------------------
//    Descriptions
//    ------------
//    Inputs    :
//
//    Outputs   : Instruction execution status [status]
//
//    Description :
//
//***************************************************************************

AI_LIB_FUNC AiReturn AI_CALL_CONV ApiCmdSysRecSS(AiUInt32 bModule, AiUInt8 rec_src, AiUInt8 con, AiUInt8 mode, AiUInt8 res, AiUInt32 mode_val)
{
    AiInt16 status;
    AiUInt8 info[8];

    info[0] = rec_src;
    info[1] = con;
    info[2] = mode;
    info[3] = res;
    info[4] = (AiUInt8)(mode_val >> 24);
    info[5] = (AiUInt8)(mode_val >> 16);
    info[6] = (AiUInt8)(mode_val >> 8);
    info[7] = (AiUInt8)(mode_val);

    status = ApiIo(bModule & ~API_STREAM_MASK, 0, TG_API_SYS_REC_SS, TG_ACK,
        &info[0], 8, w_p, 0,
        b_p, &tg_os, w_p, &tg_os, lw_p, &tg_os);

    return(status);

} // end: ApiCmdSysRecSS

//***************************************************************************
//
//   Module : APC_API                  Submodule : ApiCmdSysRecCnf
//
//   Author : Marco Motz               Project   : API1553-DS
//
//---------------------------------------------------------------------------
//   Create : 01.02.99   Update : 01.02.99
//---------------------------------------------------------------------------
//    Descriptions
//    ------------
//    Inputs    :
//
//    Outputs   : Instruction execution status [status]
//
//    Description :
//
//***************************************************************************

AI_LIB_FUNC AiReturn AI_CALL_CONV ApiCmdSysRecCnf(AiUInt32 bModule, AiUInt8 rec_src, AiUInt8 con, AiUInt8 chn_mask, AiUInt8 res1, AiUInt32 fsize, AiUInt32 res2)
{
    AiInt16 status;
    AiUInt8 info[12];

    info[0] = rec_src;
    info[1] = con;
    info[2] = chn_mask;
    info[3] = res1;
    info[4] = (AiUInt8)(fsize >> 24);
    info[5] = (AiUInt8)(fsize >> 16);
    info[6] = (AiUInt8)(fsize >> 8);
    info[7] = (AiUInt8)(fsize);
    info[8] = (AiUInt8)(res2 >> 24);
    info[9] = (AiUInt8)(res2 >> 16);
    info[10]= (AiUInt8)(res2 >> 8);
    info[11]= (AiUInt8)(res2);

    status = ApiIo(bModule & ~API_STREAM_MASK, 0, TG_API_SYS_REC_CNF, TG_ACK,
        &info[0], 12, w_p, 0,
        b_p, &tg_os, w_p, &tg_os, lw_p, &tg_os);

    return(status);

} // end: ApiCmdSysRecCnf

//***************************************************************************
//
//   Module : APC_API                  Submodule : ApiCmdSysRecStatus
//
//   Author : Marco Motz               Project   : API1553-DS
//
//---------------------------------------------------------------------------
//   Create : 01.02.99   Update : 01.02.99
//---------------------------------------------------------------------------
//    Descriptions
//    ------------
//    Inputs    :
//
//    Outputs   : Instruction execution status [status]
//
//    Description :
//
//***************************************************************************

AI_LIB_FUNC AiReturn AI_CALL_CONV ApiCmdSysRecStatus(AiUInt32 bModule, AiUInt8 rec_src, TY_API_BM_REC *prec)
{
    AiInt16 status;
    AiUInt8 info[1];
    AiInt16 in_byte_size;
    AiInt16 in_lword_size;

    in_byte_size = 0;
    in_lword_size = 0;

    info[0] = rec_src;

    status = ApiIo(bModule & ~API_STREAM_MASK, 0, TG_API_SYS_REC_STATUS, TG_ACK2,
        &info[0], 1, w_p, 0,
        &prec->status, &in_byte_size, w_p, &tg_os,
        &prec->hfi_cnt, &in_lword_size);

    return(status);

} // end: ApiCmdSysRecStatus

//***************************************************************************
//
//   Module : APC_API                  Submodule : ApiCmdSysRecCopy
//
//   Author : Marco Motz               Project   : API1553-DS
//
//---------------------------------------------------------------------------
//   Create : 01.02.99   Update : 01.02.99
//---------------------------------------------------------------------------
//    Descriptions
//    ------------
//    Inputs    :
//
//    Outputs   : Instruction execution status [status]
//
//    Description :
//
//***************************************************************************

AI_LIB_FUNC AiReturn AI_CALL_CONV ApiCmdSysRecCopy(AiUInt32 bModule, AiUInt8 rec_src, TY_API_BM_REC *prec)
{
    AiInt16 status;
    AiUInt8 info[2];
    AiUInt16 infow[4];

    info[0] = rec_src;
    info[1] = 0;

    infow[0] = (AiUInt16)(prec->saddr >> 16);
    infow[1] = (AiUInt16)(prec->saddr);
    infow[2] = (AiUInt16)(prec->size >> 16);
    infow[3] = (AiUInt16)(prec->size);

    status = ApiIo(bModule & ~API_STREAM_MASK, 0, TG_API_SYS_REC_COPY, TG_ACK,
        &info[0], 2, &infow[0], 4,
        b_p, &tg_os, w_p, &tg_os, lw_p, &tg_os);

    return(status);

} // end: ApiCmdSysRecCopy

//***************************************************************************
//
//   Module : APC_API                  Submodule : ApiCmdSysSTaskCtrl
//
//   Author : Marco Motz               Project   : API1553-DS
//
//---------------------------------------------------------------------------
//   Create : 01.02.99   Update : 01.02.99
//---------------------------------------------------------------------------
//    Descriptions
//    ------------
//    Inputs    :
//
//    Outputs   : Instruction execution status [status]
//
//    Description :
//
//***************************************************************************

AI_LIB_FUNC AiReturn AI_CALL_CONV ApiCmdSysSTaskCtrl(AiUInt32 bModule, AiUInt8 tmr, AiUInt8 con, AiUInt32 rate, AiUInt32 rsync, AiUInt32 ustep)
{
    AiInt16 status;
    AiUInt8 info[14];

    info[0]  = tmr;
    info[1]  = con;
    info[2]  = (AiUInt8)(rate >> 24);
    info[3]  = (AiUInt8)(rate >> 16);
    info[4]  = (AiUInt8)(rate >> 8);
    info[5]  = (AiUInt8)(rate);
    info[6]  = (AiUInt8)(rsync >> 24);
    info[7]  = (AiUInt8)(rsync >> 16);
    info[8]  = (AiUInt8)(rsync >> 8);
    info[9]  = (AiUInt8)(rsync);
    info[10] = (AiUInt8)(ustep >> 24);
    info[11] = (AiUInt8)(ustep >> 16);
    info[12] = (AiUInt8)(ustep >> 8);
    info[13] = (AiUInt8)(ustep);

    status = ApiIo(bModule & ~API_STREAM_MASK, 0, TG_API_SYS_STASK_CTRL, TG_ACK,
        &info[0], 14, w_p, 0,
        b_p, &tg_os, w_p, &tg_os, lw_p, &tg_os);

    return(status);

} // end: ApiCmdSysSTaskCtrl

//***************************************************************************
//
//   Module : APC_API                  Submodule : ApiExtCmdIni
//
//   Author : Marco Motz               Project   : API1553-DS
//
//---------------------------------------------------------------------------
//   Create : 01.02.99   Update : 01.02.99
//---------------------------------------------------------------------------
//    Descriptions
//    ------------
//    Inputs    :
//
//    Outputs   :
//
//    Description :
//
//***************************************************************************

AI_LIB_FUNC AiReturn AI_CALL_CONV ApiExtCmdIni(AiUInt32 bModule, AiUInt32 *pini)
{
    AiInt16 status;
    AiInt16 in_lword_size;

    in_lword_size = 0;

    status = ApiExtIo(bModule, 0, TG_EXT_PCI_INI, TG_ACK2,
        b_p, 0, w_p, 0,
        b_p, &tg_os, w_p, &tg_os, pini, &in_lword_size);

    return(status);

} // end: ApiExtCmdIni

//***************************************************************************
//
//   Module : APC_API                  Submodule : ApiExtCmdRamWrite
//
//   Author : Marco Motz               Project   : API1553-DS
//
//---------------------------------------------------------------------------
//   Create : 01.02.99   Update : 01.02.99
//---------------------------------------------------------------------------
//    Descriptions
//    ------------
//    Inputs    :
//
//    Outputs   :
//
//    Description :
//
//***************************************************************************

AI_LIB_FUNC AiReturn AI_CALL_CONV ApiExtCmdRamWrite(AiUInt32 bModule, AiUInt8 access, AiUInt32 addr, AiUInt32 val)
{
    AiInt16 status;
    AiUInt8 info[9];

    info[0] = access;
    info[1] = (AiUInt8)(addr >> 24);
    info[2] = (AiUInt8)(addr >> 16);
    info[3] = (AiUInt8)(addr >> 8);
    info[4] = (AiUInt8)(addr);
    info[5] = (AiUInt8)(val >> 24);
    info[6] = (AiUInt8)(val >> 16);
    info[7] = (AiUInt8)(val >> 8);
    info[8] = (AiUInt8)(val);

    status = ApiExtIo(bModule, 0, TG_EXT_PCI_IP_LW_WRITE, TG_ACK,
        &info[0], 9, w_p, 0,
        b_p, &tg_os, w_p, &tg_os, lw_p, &tg_os);

    return(status);

} // end: ApiExtCmdRamWrite

//***************************************************************************
//
//   Module : APC_API                  Submodule : ApiExtCmdRamRead
//
//   Author : Marco Motz               Project   : API1553-DS
//
//---------------------------------------------------------------------------
//   Create : 01.02.99   Update : 01.02.99
//---------------------------------------------------------------------------
//    Descriptions
//    ------------
//    Inputs    :
//
//    Outputs   :
//
//    Description :
//
//***************************************************************************

AI_LIB_FUNC AiReturn AI_CALL_CONV ApiExtCmdRamRead(AiUInt32 bModule, AiUInt8 access, AiUInt32 addr, AiUInt32 *val)
{
    AiInt16 status;
    AiUInt8 info[5];
    AiInt16 in_lword_size;

    in_lword_size = 0;

    info[0] = access;
    info[1] = (AiUInt8)(addr >> 24);
    info[2] = (AiUInt8)(addr >> 16);
    info[3] = (AiUInt8)(addr >> 8);
    info[4] = (AiUInt8)(addr);

    status = ApiExtIo(bModule, 0, TG_EXT_PCI_IP_LW_READ, TG_ACK2,
        &info[0], 5, w_p, 0,
        b_p, &tg_os, w_p, &tg_os, val, &in_lword_size);

    return(status);

} // end: ApiExtCmdRamRead

//***************************************************************************
//
//   Module : APC_API                  Submodule : ApiExtCmdAllocMem
//
//   Author : Marco Motz               Project   : API1553-DS
//
//---------------------------------------------------------------------------
//   Create : 01.02.99   Update : 01.02.99
//---------------------------------------------------------------------------
//    Descriptions
//    ------------
//    Inputs    :
//
//    Outputs   :
//
//    Description :
//
//***************************************************************************

AI_LIB_FUNC AiReturn AI_CALL_CONV ApiExtCmdAllocMem(AiUInt32 bModule, AiUInt8 bar_no, AiUInt8 kbyte, AiUInt8 *st)
{
    AiInt16 status;
    AiUInt8 info[2];
    AiInt16 in_byte_size;

    in_byte_size = 0;

    info[0] = (AiUInt8)(bar_no & 0x3);
    info[1] = kbyte;

    status = ApiExtIo(bModule, 0, TG_EXT_PCI_IP_ALLOC_MEM, TG_ACK2,
        &info[0], 2, w_p, 0,
        st, &in_byte_size, w_p, &tg_os, lw_p, &tg_os);

    return(status);

} // end: ApiExtCmdAllocMem

