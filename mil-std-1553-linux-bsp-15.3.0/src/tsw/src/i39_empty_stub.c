/* SPDX-FileCopyrightText: 2015-2022 AIM GmbH <info@aim-online.com> */
/* SPDX-License-Identifier: MIT OR GPL-2.0-or-later */

/*! \file i39_empty_stub.c
    Empty 3910 function stub for 1553 only TSW.
*/

#include "mil_tsw_includes.h"


void enter_drv3910(                      TY_API_DEV *p_api_dev)                                                             { return; }
void enter_drv_set_mem_layout_hs(        TY_API_DEV *p_api_dev, L_WORD ul_MemoryBank, TY_MEM_DEFINITION ax_MemDef[MAX_BIU] ){ return; }


L_WORD api39_bc_ini(TY_API_DEV *p_api_dev)                                    { return API_NAK; }
BYTE api39_sys_read_sw_vers_legacy(      TY_API_DEV *p_api_dev)                                     { return API_NAK; }
BYTE api39_sys_reset_legacy(             TY_API_DEV *p_api_dev, BYTE rc, BYTE uc_mbfs)              { return API_NAK; }
BYTE api_gpio(                           TY_API_DEV *p_api_dev, L_WORD cmd, WORD size, BYTE *info_p){ return API_NAK; }
BYTE api39_sys_get_mem_partition_legacy( TY_API_DEV *p_api_dev, BYTE uc_mode)                       { return API_NAK; }
BYTE api39_ini_legacy(                   TY_API_DEV *p_api_dev, BYTE uc_mode)                       { return API_NAK; }

WORD api39_bite(                         TY_API_DEV *p_api_dev, BYTE mode) { return 0; }

L_WORD api39_rep_start(                  TY_API_DEV *p_api_dev)                         { return API_ERR_PLATTFORM_NOT_SUPPORTED; }
L_WORD api39_sys_reset(                  TY_API_DEV *p_api_dev, BYTE rc, BYTE uc_mbfs)  { return API_ERR_PLATTFORM_NOT_SUPPORTED; }
L_WORD api39_rt_mode(                    TY_API_DEV *p_api_dev, BYTE uc_con)            { return API_ERR_PLATTFORM_NOT_SUPPORTED; }
L_WORD api39_rep_halt(                   TY_API_DEV *p_api_dev)                         { return API_ERR_PLATTFORM_NOT_SUPPORTED; }
L_WORD api39_sys_set_mem_partition (     TY_API_DEV *p_api_dev, L_WORD mode,
                                         TY_MEM_DEFINITION ax_MemDef[MAX_BIU],
                                         L_WORD * ret_status, L_WORD ret_mem_used[2])   { return API_ERR_PLATTFORM_NOT_SUPPORTED; }
L_WORD api39_bc_start(                   TY_API_DEV *p_api_dev, BYTE uc_smod)           { return API_ERR_PLATTFORM_NOT_SUPPORTED; }
L_WORD api39_rt_ini(                     TY_API_DEV *p_api_dev,
                                         BYTE uc_RtAddr, BYTE uc_Con)                   { return API_ERR_PLATTFORM_NOT_SUPPORTED; }
L_WORD api39_sys_get_mem_partition(      TY_API_DEV *p_api_dev, L_WORD mode,
                                         TY_API_GET_MEM_INFO * mem_info )               { return API_ERR_PLATTFORM_NOT_SUPPORTED; }
L_WORD api39_bc_halt(                    TY_API_DEV *p_api_dev)                         { return API_ERR_PLATTFORM_NOT_SUPPORTED; }
L_WORD api_x_bm_mode(                    TY_API_DEV *p_api_dev, BYTE uc_Con)            { return API_ERR_PLATTFORM_NOT_SUPPORTED; }
L_WORD api39_bm_ini(                     TY_API_DEV *p_api_dev)                         { return API_ERR_PLATTFORM_NOT_SUPPORTED; }
L_WORD api39_sys_foddl(                  TY_API_DEV *p_api_dev, BYTE con)               { return API_ERR_PLATTFORM_NOT_SUPPORTED; }
L_WORD api39_ini(                        TY_API_DEV *p_api_dev, BYTE uc_mode)           { return API_ERR_PLATTFORM_NOT_SUPPORTED; }
L_WORD api39_rt_sa_con(                  TY_API_DEV *p_api_dev, BYTE rt_addr,
                                         BYTE sa_mc, WORD hid, BYTE sa_type,
                                         BYTE con, BYTE rmod, BYTE smod, WORD swm)      { return API_ERR_PLATTFORM_NOT_SUPPORTED; }
L_WORD api39_rt_con(                     TY_API_DEV *p_api_dev,
                                         BYTE uc_RtAddr, BYTE uc_Con)                   { return API_ERR_PLATTFORM_NOT_SUPPORTED; }
L_WORD api39_bc_xfer_ctrl(               TY_API_DEV *p_api_dev,
                                         WORD uw_xid, BYTE uc_mode)                     { return API_ERR_PLATTFORM_NOT_SUPPORTED; }

void api39_io_rt_ena_dis_set_operation(  TY_API_DEV *p_api_dev, BYTE RtAddr, 
                                         BYTE con, BYTE new_mode)                       { }
void api39_io_rt_ena_dis(                TY_API_DEV *p_api_dev, BYTE RtAddr, BYTE con)  { }

void api39_ir                         (TY_API_DEV *p_api_dev) {}
