/* SPDX-FileCopyrightText: 2018-2022 AIM GmbH <info@aim-online.com> */
/* SPDX-License-Identifier: MIT OR GPL-2.0-or-later */

/*! \file api_ir.h
 *
 *  Header information of the 1553 Driver Software. Definition of variables, arrays and structures, external references.   
 *
 */

#ifndef __API_IR_H__
  #define __API_IR_H__

#include "mil_tsw_includes.h"

  #define MAX_ISR_BIU               ((p_api_dev->chns == 2) ? 2 : 1) 
  #define MAX_ISR_BIU_IR            ((px_api_dev->chns > 2) ? ((px_api_dev->chns == 4) ? 4 : 3) : ((px_api_dev->chns == 2) ? 2 : 1) ) 
  #define API_ISR_CB_ADDR           ((dest_biu == 0) ? p_api_dev->glb_mem.biu[0].cb_addr       : p_api_dev->glb_mem.biu[1].cb_addr) 
  #define API_ISR_LOG_ADDR          ((dest_biu == 0) ? p_api_dev->glb_mem.biu[BIU_1-1].ir_log_addr  : p_api_dev->glb_mem.biu[BIU_2-1].ir_log_addr)
  #define API_ISR_BC_XFER_DESC_ADDR ((dest_biu == 0) ? p_api_dev->glb_mem.biu[BIU_1-1].base.bc_xfer_desc    : p_api_dev->glb_mem.biu[BIU_2-1].base.bc_xfer_desc)
  #define API_ISR_BC_BH_BASE_ADDR   ((dest_biu == 0) ? p_api_dev->glb_mem.biu[BIU_1-1].base.bc_bh_area : p_api_dev->glb_mem.biu[BIU_2-1].base.bc_bh_area)
  #define API_ISR_RT_SA_DESC_ADDR   ((dest_biu == 0) ? p_api_dev->glb_mem.biu[BIU_1-1].base.rt_sa_desc      : p_api_dev->glb_mem.biu[BIU_2-1].base.rt_sa_desc)
  #define API_ISR_RT_BH_BASE_ADDR   ((dest_biu == 0) ? p_api_dev->glb_mem.biu[BIU_1-1].base.rt_bh_area : p_api_dev->glb_mem.biu[BIU_2-1].base.rt_bh_area)
  #define API_ISR_BM_BUF_BASE_ADDR  ((dest_biu == 0) ? p_api_dev->glb_mem.biu[BIU_1-1].size.bm_buf     : p_api_dev->glb_mem.biu[BIU_2-1].size.bm_buf)


/* Structure Definition as Bit Fields for the GT-64011 Interrupt Cause Register */
typedef union
{
  L_WORD all;
  struct
  {
    L_WORD res_1: 21;
    L_WORD bits: 5;
    L_WORD res_2: 5;
    L_WORD sum: 1;
  } cpu2pci;
  struct
  {
    L_WORD res_1: 27;
    L_WORD bits: 4;
    L_WORD sum: 1;
  } pci2cpu;
}

GT_INTERRUPT_CAUSE;


NAMESPACE1553_START

/* local prototypes */
void api_write_int_loli_gen_int(TY_API_DEV *p_api_dev, WORD uw_IntType, L_WORD ul_w1, L_WORD ul_w2, L_WORD ul_w3, L_WORD ul_w4);

NAMESPACE1553_END


#endif /* __API_IR_H__ */
