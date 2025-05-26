/* SPDX-FileCopyrightText: 2001-2022 AIM GmbH <info@aim-online.com> */
/* SPDX-License-Identifier: MIT OR GPL-2.0-or-later */

/*! \file api_bm.c
Bus Monitor (BM) module of the 1553 ASP Driver SW. This module comprises
the handler routines for the BM instructions.
*/

#include "mil_tsw_includes.h"

NAMESPACE1553_START

TY_MIL_COM_TABLE_ENTRY axTargetLsBmCommands[] = {
   /* LS BM Functions */
  {MIL_COM_LS_BM_INI,             (TY_MIL_COM_FUNC_PTR)api_bm_ini_cmd,            "api_bm_ini_cmd",            sizeof(TY_MIL_COM),                             sizeof(TY_MIL_COM_ACK)  },
  {MIL_COM_LS_BM_INI_INTR,        (TY_MIL_COM_FUNC_PTR)api_bm_ini_intr_cmd,       "api_bm_ini_intr_cmd",       sizeof(TY_MIL_COM_LS_BM_INI_INTR_INPUT),        sizeof(TY_MIL_COM_ACK)  },
  {MIL_COM_LS_BM_INI_CAP,         (TY_MIL_COM_FUNC_PTR)api_bm_ini_cap_cmd,        "api_bm_ini_cap_cmd",        sizeof(TY_MIL_COM_LS_BM_INI_CAP_INPUT),         sizeof(TY_MIL_COM_ACK)  },
  {MIL_COM_LS_BM_INI_SWXM,        (TY_MIL_COM_FUNC_PTR)api_bm_ini_swxm,           "api_bm_ini_swxm",           sizeof(TY_MIL_COM_LS_BM_INI_SWXM_INPUT),        sizeof(TY_MIL_COM_ACK)  },
  {MIL_COM_LS_BM_START,           (TY_MIL_COM_FUNC_PTR)api_bm_start,              "api_bm_start",              sizeof(TY_MIL_COM),                             sizeof(TY_MIL_COM_ACK)  },
  {MIL_COM_LS_BM_HALT,            (TY_MIL_COM_FUNC_PTR)api_bm_halt,               "api_bm_halt",               sizeof(TY_MIL_COM),                             sizeof(TY_MIL_COM_ACK)  },
  {MIL_COM_LS_BM_STATUS_READ,     (TY_MIL_COM_FUNC_PTR)api_bm_status_read,        "api_bm_status_read",        sizeof(TY_MIL_COM),                             sizeof(TY_MIL_COM_LS_BM_STATUS_OUTPUT)  },
  {MIL_COM_LS_BM_ACT_READ,        (TY_MIL_COM_FUNC_PTR)api_bm_act_read,           "api_bm_act_read",           sizeof(TY_MIL_COM_LS_BM_ACT_READ_INPUT),        sizeof(TY_MIL_COM_LS_BM_ACT_READ_OUTPUT)  },
  {MIL_COM_LS_BM_STACK_PTR_READ,  (TY_MIL_COM_FUNC_PTR)api_bm_stack_ptr_read,     "api_bm_stack_ptr_read",     sizeof(TY_MIL_COM),                             sizeof(TY_MIL_COM_LS_BM_STACK_PTR_READ_OUTPUT)  },
  {MIL_COM_LS_BM_STACK_ENTRY_READ,(TY_MIL_COM_FUNC_PTR)api_bm_stack_entry_read,   "api_bm_stack_entry_read",   sizeof(TY_MIL_COM_LS_BM_STACK_ENTRY_READ_INPUT),sizeof(TY_MIL_COM_LS_BM_STACK_ENTRY_READ_OUTPUT)  },
  {MIL_COM_LS_BM_STACK_ENTRY_FIND,(TY_MIL_COM_FUNC_PTR)api_bm_stack_entry_find,   "api_bm_stack_entry_find",   sizeof(TY_MIL_COM_LS_BM_STACK_ENTRY_FIND_INPUT),sizeof(TY_MIL_COM_LS_BM_STACK_ENTRY_FIND_OUTPUT)  },
  {MIL_COM_LS_BM_REC_STATUS_READ, (TY_MIL_COM_FUNC_PTR)api_bm_rec_status_read,    "api_bm_rec_status_read",    sizeof(TY_MIL_COM),                             sizeof(TY_MIL_COM_LS_BM_REC_STATUS_OUTPUT)  },
  {MIL_COM_LS_BM_COPY_REC_DAT,    (TY_MIL_COM_FUNC_PTR)api_bm_copy_rec_dat,       "api_bm_copy_rec_dat",       sizeof(TY_MIL_COM_LS_BM_COPY_REC_DAT_INPUT),    sizeof(TY_MIL_COM_ACK)  },
  {MIL_COM_LS_BM_INI_MSG_FLT,     (TY_MIL_COM_FUNC_PTR)api_bm_ini_msg_flt,        "api_bm_ini_msg_flt",        sizeof(TY_MIL_COM_LS_BM_INI_MSG_FLT_INPUT),     sizeof(TY_MIL_COM_ACK)  },
  {MIL_COM_LS_BM_INI_MSG_ILL,     (TY_MIL_COM_FUNC_PTR)api_bm_ini_msg_ill,        "api_bm_ini_msg_ill",        sizeof(TY_MIL_COM_LS_BM_INI_MSG_ILL_INPUT),     sizeof(TY_MIL_COM_ACK)  },
  {MIL_COM_LS_BM_INI_FTW,         (TY_MIL_COM_FUNC_PTR)api_bm_ini_ftw,            "api_bm_ini_ftw",            sizeof(TY_MIL_COM_LS_BM_INI_FTW_INPUT),         sizeof(TY_MIL_COM_ACK)  },
  {MIL_COM_LS_BM_INI_TIW,         (TY_MIL_COM_FUNC_PTR)api_bm_ini_tiw,            "api_bm_ini_tiw",            sizeof(TY_MIL_COM_LS_BM_INI_TIW_INPUT),         sizeof(TY_MIL_COM_ACK)  },
  {MIL_COM_LS_BM_INI_TCI,         (TY_MIL_COM_FUNC_PTR)api_bm_ini_tci,            "api_bm_ini_tci",            sizeof(TY_MIL_COM_LS_BM_INI_TCI_INPUT),         sizeof(TY_MIL_COM_ACK)  },
  {MIL_COM_LS_BM_INI_TCB,         (TY_MIL_COM_FUNC_PTR)api_bm_ini_tcb,            "api_bm_ini_tcb",            sizeof(TY_MIL_COM_LS_BM_INI_TCB_INPUT),         sizeof(TY_MIL_COM_ACK)  },
  {MIL_COM_LS_BM_RT_ACT_READ,     (TY_MIL_COM_FUNC_PTR)api_bm_rt_act_read,        "api_bm_rt_act_read",        sizeof(TY_MIL_COM_LS_BM_RT_ACT_READ_INPUT),     sizeof(TY_MIL_COM_LS_BM_RT_ACT_READ_OUTPUT)  },
  {MIL_COM_LS_BM_RT_SA_ACT_READ,  (TY_MIL_COM_FUNC_PTR)api_bm_rt_sa_act_read_cmd, "api_bm_rt_sa_act_read_cmd", sizeof(TY_MIL_COM_LS_BM_RT_SA_ACT_READ_INPUT),  sizeof(TY_MIL_COM_LS_BM_RT_SA_ACT_READ_OUTPUT)  },
  {MIL_COM_LS_BM_INI_MSG_REC_FLT, (TY_MIL_COM_FUNC_PTR)api_bm_ini_msg_flt_rec,    "api_bm_ini_msg_flt_rec",    sizeof(TY_MIL_COM_LS_BM_INI_MSG_FLT_REC_INPUT), sizeof(TY_MIL_COM_ACK)  },
  {MIL_COM_LS_BM_MSG_REC_FLT,     (TY_MIL_COM_FUNC_PTR)api_bm_read_msg_flt_rec,   "api_bm_read_msg_flt_rec",   sizeof(TY_MIL_COM_LS_BM_MSG_FLT_REC_INPUT),     sizeof(TY_MIL_COM_LS_BM_MSG_FLT_REC_OUTPUT)  },
  {MIL_COM_LS_BM_DYNTAG_MON_DEF,  (TY_MIL_COM_FUNC_PTR)api_bm_dyntag_mon_def,     "api_bm_dyntag_mon_def",     sizeof(TY_MIL_COM_LS_BM_DYNTAG_DEF_INPUT),      sizeof(TY_MIL_COM_ACK)  },
  {MIL_COM_LS_BM_DYNTAG_MON_READ, (TY_MIL_COM_FUNC_PTR)api_bm_dyntag_mon_read,    "api_bm_dyntag_mon_read",    sizeof(TY_MIL_COM_LS_BM_DYNTAG_MON_READ_INPUT), sizeof(TY_MIL_COM_LS_BM_DYNTAG_MON_READ_OUTPUT)  },

};

void api_ls_bm_command_table( TY_MIL_COM_TABLE_ENTRY ** table, AiUInt32 * size )
{
    *size = sizeof(axTargetLsBmCommands) / sizeof(TY_MIL_COM_TABLE_ENTRY);
    *table = axTargetLsBmCommands;
}


/*****************************************************************************/
/*                                                                           */
/*   Module : API_BM                   Submodule : API_BM_INI                */
/*                                                                           */
/*   Author : Marco Motz               Project   : API1553-DS                */
/*                                                                           */
/*   Source : C                        Tools     : PC/AT; Norton Editor;     */
/*                                                 CYGNUS, GNU-C, As, and LD */
/*                                                 IDT-C 5.1 Toolkit         */
/*---------------------------------------------------------------------------*/
/*   Create : 01.05.98   Update : 11.11.05                                   */
/*---------------------------------------------------------------------------*/
/*    Descriptions                                                           */
/*    ------------                                                           */
/*    Inputs    : none                                                       */
/*                                                                           */
/*    Outputs   : Instruction acknowledge type [ackfl]                       */
/*                                                                           */
/*    Description :                                                          */
/*    This function handles the 'API_BM_INI' instruction to initialize the   */
/*    API1553-DS BM mode.                                                    */
/*                                                                           */
/*****************************************************************************/
L_WORD api_bm_ini_cmd(TY_API_DEV *p_api_dev, TY_MIL_COM * in, TY_MIL_COM_ACK * out )
{
    return api_bm_ini( p_api_dev );
}

L_WORD api_bm_ini(TY_API_DEV *p_api_dev)
{
    WORD   i      = 0;
    L_WORD retval = API_OK; /* to check api39_bm_ini return value */


    api_io_mode_con(p_api_dev, API_BM_MODE, API_OFF);
    api_io_bm_ini(p_api_dev);

    /* Clear BM ISR Flag */
    api_io_bm_ir_clr(p_api_dev);

    p_api_dev->bm_status[ p_api_dev->biu ] = API_HALT;
    p_api_dev->bm_status_dsp[ p_api_dev->biu ] = API_BM_STATUS_HALT;

    p_api_dev->bm_status_cnt[ p_api_dev->biu ].msg_cnt = 0L;
    p_api_dev->bm_status_cnt[ p_api_dev->biu ].err_cnt = 0L;

    /* Recording */
    p_api_dev->bm_rec[ p_api_dev->biu ].mode = API_OFF;
    p_api_dev->bm_rec[ p_api_dev->biu ].hfi_org = 0L;
    p_api_dev->bm_rec[ p_api_dev->biu ].hfi_cnt = 0L;
    p_api_dev->bm_rec[ p_api_dev->biu ].saddr = p_api_dev->glb_mem.biu[ p_api_dev->biu ].base.bm_buf;
    p_api_dev->bm_rec[ p_api_dev->biu ].size = 0L;

    /* Clear Dyntag Monitor RT-SA Lookup Table */
    api_io_bm_dyntagm_lut_free(p_api_dev,0/*all*/,0/*na*/);

    /* Clear Dyntag Monitor Setup Table */
    for(i=0; i<MAX_API_BM_DYNTAG_MON_ID; i++) {
        api_io_bm_dyntagm_entry_init(p_api_dev,(L_WORD)i/*tag_ix*/);
    }    

    /* Message Filter Recording */
    p_api_dev->bm_msg_flt_mode[ p_api_dev->biu ] = API_OFF;
    p_api_dev->bm_msg_flt_dsp[ p_api_dev->biu ].cnt = 0;
    for(i = 0; i < MAX_API_BM_MSG_FLT_CMD; i++)
    {
        p_api_dev->bm_msg_flt_dsp[ p_api_dev->biu ].cw[ i ] = 0x0000;
        p_api_dev->bm_msg_flt_dsp[ p_api_dev->biu ].pos1[ i ] = 0x00;
        p_api_dev->bm_msg_flt_dsp[ p_api_dev->biu ].pos2[ i ] = 0x00;
    }

    p_api_dev->bm_msg_flt_dsp[ p_api_dev->biu ].trigd = API_OFF; 
    p_api_dev->bm_msg_flt_dsp[ p_api_dev->biu ].ovfl = API_OFF;
    p_api_dev->bm_msg_flt_dsp[ p_api_dev->biu ].out_size = 0x0000;
    p_api_dev->bm_msg_flt_dsp[ p_api_dev->biu ].old_mbfp = p_api_dev->glb_mem.biu[ p_api_dev->biu ].base.bm_buf;
    p_api_dev->bm_msg_flt_dsp[ p_api_dev->biu ].cur_mbfp = p_api_dev->glb_mem.biu[ p_api_dev->biu ].base.bm_buf;
    p_api_dev->bm_msg_flt_dsp[ p_api_dev->biu ].cur_entry_ptr = p_api_dev->glb_mem.biu[ p_api_dev->biu ].base.bm_buf;

    p_api_dev->bm_msg_flt_dsp[ p_api_dev->biu ].cur_sw_errw = 0x0000;
    p_api_dev->bm_msg_flt_dsp[ p_api_dev->biu ].data_cnt = 0;
    for(i = 0; i < 32; i++)
        p_api_dev->bm_msg_flt_dsp[ p_api_dev->biu ].data[ i ] = 0x0000;

    p_api_dev->bm_msg_flt_dsp[ p_api_dev->biu ].bm_entry_cnt = 0L;

    for(i = 0; i < 64; i++)
        p_api_dev->bm_msg_flt_dsp[ p_api_dev->biu ].bm_entry[ i ] = 0L;

    p_api_dev->bm_msg_flt_dsp[ p_api_dev->biu ].cur_ttag_high = 0L;
    p_api_dev->bm_msg_flt_dsp[ p_api_dev->biu ].cur_ttag_low = 0L;
    p_api_dev->bm_msg_flt_dsp[ p_api_dev->biu ].cur_cw[0] = 0L;
    p_api_dev->bm_msg_flt_dsp[ p_api_dev->biu ].cur_cw[1] = 0L;
    p_api_dev->bm_msg_flt_dsp[ p_api_dev->biu ].cur_sw[0] = 0L;
    p_api_dev->bm_msg_flt_dsp[ p_api_dev->biu ].cur_sw[1] = 0L;


    if( ul_StreamIsHS( p_api_dev, p_api_dev->biu ) )
        retval = api39_bm_ini(p_api_dev);

    return retval;

} /* end: api_bm_ini */

/*****************************************************************************/
/*                                                                           */
/*   Module : API_BM                   Submodule : API_BM_INI_INTR           */
/*                                                                           */
/*   Author : Marco Motz               Project   : API1553-DS                */
/*                                                                           */
/*   Source : C                        Tools     : PC/AT; Norton Editor;     */
/*                                                 CYGNUS, GNU-C, As, and LD */
/*                                                 IDT-C 5.1 Toolkit         */
/*---------------------------------------------------------------------------*/
/*   Create : 01.05.98   Update : 01.05.98                                   */
/*---------------------------------------------------------------------------*/
/*    Descriptions                                                           */
/*    ------------                                                           */
/*    Inputs    : Interrupt Mode [imod]                                      */
/*                Strobe Mode [smod]                                         */
/*                                                                           */
/*    Outputs   : Instruction acknowledge type [ackfl]                       */
/*                                                                           */
/*    Description :                                                          */
/*    This function handles the 'API_BM_INI_INTR' instruction to initialize  */
/*    API1553-DS BM related Interrupts and Output Strobes.                   */
/*                                                                           */
/*****************************************************************************/

L_WORD api_bm_ini_intr_cmd( TY_API_DEV *p_api_dev, TY_MIL_COM_LS_BM_INI_INTR_INPUT * in, TY_MIL_COM_ACK * out )
{
    return api_bm_ini_intr( p_api_dev, (BYTE)in->imod, (BYTE)in->smod );
}


L_WORD api_bm_ini_intr(TY_API_DEV *p_api_dev, BYTE imod, BYTE smod)
{
    if( ( (imod == API_SCB_IMOD_DIS) || (imod == API_SCB_IMOD_BUF_FULL) || (imod == API_SCB_IMOD_TR_START) || (imod == API_SCB_IMOD_TR_STOP) ) && 
        ( (smod == API_SCB_SMOD_DIS) || (smod == API_SCB_SMOD_BUF_FULL) || (smod == API_SCB_SMOD_TR_START) || (smod == API_SCB_SMOD_TR_STOP) )  )
    {
        api_io_bm_intr_con(p_api_dev, imod, smod);
        return 0;
    }

    return API_NAK;

} /* end: api_bm_ini_intr */

/*****************************************************************************/
/*                                                                           */
/*   Module : API_BM                   Submodule : API_BM_INI_CAP            */
/*                                                                           */
/*   Author : Marco Motz               Project   : API1553-DS                */
/*                                                                           */
/*   Source : C                        Tools     : PC/AT; Norton Editor;     */
/*                                                 CYGNUS, GNU-C, As, and LD */
/*                                                 IDT-C 5.1 Toolkit         */
/*---------------------------------------------------------------------------*/
/*   Create : 01.05.98   Update : 01.05.99                                   */
/*---------------------------------------------------------------------------*/
/*    Descriptions                                                           */
/*    ------------                                                           */
/*    Inputs    : Capture Mode [cmod]                                        */
/*                Trace After Trigger counter [tat]                          */
/*                Message Capture counter [mcc]                              */
/*                Recording File size [fsize]                                */
/*                                                                           */
/*    Outputs   : Instruction acknowledge type [ackfl]                       */
/*                                                                           */
/*    Description :                                                          */
/*    This function handles the 'API_BM_INI_CAP' instruction to initialize   */
/*    the API1553-DS BM Capture mode.                                        */
/*                                                                           */
/*****************************************************************************/

L_WORD api_bm_ini_cap_cmd( TY_API_DEV *p_api_dev, TY_MIL_COM_LS_BM_INI_CAP_INPUT * in, TY_MIL_COM_ACK * out )
{
    return api_bm_ini_cap( p_api_dev, (BYTE)in->cmod, in->tat, (WORD)in->mcc, (WORD)in->fsize );
}


L_WORD api_bm_ini_cap(TY_API_DEV *p_api_dev, BYTE cmod, L_WORD tat, WORD mcc, WORD fsize)
{
    if( (cmod != API_BM_CMOD_ALL)  &&
        (cmod != API_BM_CMOD_ONLY) &&
        (cmod != API_BM_CMOD_REC)  &&
        (cmod != API_BM_CMOD_MSG_FLT_REC) )
        return API_ERR_INVALID_MODE;

    if (cmod == API_BM_CMOD_ONLY)
    {
        if (!mil_fw_is_cmd_supported(p_api_dev, MIL_FW_CMD_BM_CAPMODE_ONLY))
            return API_ERR_CMD_NOT_SUPPORTED_BY_FW;
    }

    api_io_bm_cap_con(p_api_dev, cmod, tat, mcc, fsize);

    return 0;

} /* end: api_bm_ini_cap */

/*****************************************************************************/
/*                                                                           */
/*   Module : API_BM                   Submodule : API_BM_INI_SWXM           */
/*                                                                           */
/*   Author : Marco Motz               Project   : API1553-DS                */
/*                                                                           */
/*   Source : C                        Tools     : PC/AT; Norton Editor;     */
/*                                                 CYGNUS, GNU-C, As, and LD */
/*                                                 IDT-C 5.1 Toolkit         */
/*---------------------------------------------------------------------------*/
/*   Create : 01.05.98   Update : 01.05.98                                   */
/*---------------------------------------------------------------------------*/
/*    Descriptions                                                           */
/*    ------------                                                           */
/*    Inputs    : Status Word Exception Mask [swxm]                          */
/*                                                                           */
/*    Outputs   : Instruction acknowledge type [ackfl]                       */
/*                                                                           */
/*    Description :                                                          */
/*    This function handles the 'API_BM_INI_SWXM' instruction to initialize  */
/*    the API1553-DS BM Status Word Exception Mask.                          */
/*                                                                           */
/*****************************************************************************/

L_WORD api_bm_ini_swxm( TY_API_DEV *p_api_dev, TY_MIL_COM_LS_BM_INI_SWXM_INPUT * in, TY_MIL_COM_ACK * out )
{
    api_io_bm_swxm_con( p_api_dev, (WORD)in->swxm );

    return 0;
}

/*****************************************************************************/
/*                                                                           */
/*   Module : API_BM                   Submodule : API_BM_MODE               */
/*                                                                           */
/*   Author : Marco Motz               Project   : API1553-DS                */
/*                                                                           */
/*   Source : C                        Tools     : PC/AT; Norton Editor;     */
/*                                                 CYGNUS, GNU-C, As, and LD */
/*                                                 IDT-C 5.1 Toolkit         */
/*---------------------------------------------------------------------------*/
/*   Create : 01.05.98   Update : 11.11.05                                   */
/*---------------------------------------------------------------------------*/
/*    Descriptions                                                           */
/*    ------------                                                           */
/*    Inputs    : BM Control (START/HALT) [con]                              */
/*                                                                           */
/*    Outputs   : Instruction acknowledge type [ackfl]                       */
/*                                                                           */
/*    Description :                                                          */
/*    This function handles the 'API_BM_START' and 'API_BM_HALT' instruction */
/*    to control the dynamic operations (START / STOP) of the API1553-DS BM. */
/*                                                                           */
/*****************************************************************************/

L_WORD api_bm_start(TY_API_DEV *p_api_dev, TY_MIL_COM * in, TY_MIL_COM_ACK * out )
{
    if (API_DEVICE_MODE_IS_SIMULATOR(p_api_dev->drv_mode[p_api_dev->biu]))
        return API_ERR_BM_NOT_AVAILABLE;

    if (ul_StreamIsHS(p_api_dev, p_api_dev->biu))
        return api_x_bm_mode( p_api_dev, API_BUSY );
    else
        return api_bm_mode( p_api_dev, API_BUSY );
}

L_WORD api_bm_halt(TY_API_DEV *p_api_dev, TY_MIL_COM * in, TY_MIL_COM_ACK * out )
{
    if( ul_StreamIsHS( p_api_dev, p_api_dev->biu ) )
        return api_x_bm_mode( p_api_dev, API_HALT );
    else
        return api_bm_mode( p_api_dev, API_HALT );
}


L_WORD api_bm_mode(TY_API_DEV *p_api_dev, BYTE con)
{
    L_WORD i, mbfp, addr;
    BYTE rt;
    L_WORD *ptr;
    AiUInt32 retVal = 0;

    if( (con >= API_BUSY) && (p_api_dev->bm_status[ p_api_dev->biu ] != API_BUSY) )
    {

        if( (con == API_BM_RUN_SETUP) || (con == API_BUSY) )
        {
            /* BM ON */
            api_io_bm_buf_ini(p_api_dev);

            /* Init Message/Error Counter */
            for(rt = 0; rt < MAX_API_RT; rt++)
                api_io_bm_rt_act_ini_cnt(p_api_dev, rt);

            /* Clear Dyntag Monitor Setup Table */
            for(i=0; i<MAX_API_BM_DYNTAG_MON_ID; i++) {
                api_io_bm_dyntagm_entry_init_cnt(p_api_dev,i/*tag_ix*/); /* 11.11.05 */
            }

            /* Init System Control Block registers */
            (*p_api_dev->cb_p[ p_api_dev->biu ]).msw = 0L;
            (*p_api_dev->cb_p[ p_api_dev->biu ]).bm_get = ul_ConvertTSWPtrToGRAMPtr(p_api_dev, p_api_dev->biu, p_api_dev->glb_mem.biu[ p_api_dev->biu ].base.bm_buf);
            (*p_api_dev->cb_p[ p_api_dev->biu ]).mbfp = ul_ConvertTSWPtrToGRAMPtr(p_api_dev, p_api_dev->biu, p_api_dev->glb_mem.biu[ p_api_dev->biu ].base.bm_buf);
            (*p_api_dev->cb_p[ p_api_dev->biu ]).mstp = ul_ConvertTSWPtrToGRAMPtr(p_api_dev, p_api_dev->biu, p_api_dev->glb_mem.biu[ p_api_dev->biu ].base.bm_buf);

            /* Recording Mode */
            if( (p_api_dev->bm_rec[ p_api_dev->biu ].mode == API_ON) || (p_api_dev->bm_msg_flt_mode[ p_api_dev->biu ] == API_ON) )
            {
                /* Enable Interrupt for Recording */
                api_io_bm_imod_set(p_api_dev, API_SCB_IMOD_BUF_FULL);
                p_api_dev->bm_rec[ p_api_dev->biu ].hfi_cnt = p_api_dev->bm_rec[ p_api_dev->biu ].hfi_org;
                p_api_dev->bm_rec[ p_api_dev->biu ].saddr = p_api_dev->glb_mem.biu[ p_api_dev->biu ].base.bm_buf;
                p_api_dev->bm_rec[ p_api_dev->biu ].size = 0L;
            }

            /* Clear Global BM Message and Error Counter */
            p_api_dev->bm_status_cnt[ p_api_dev->biu ].msg_cnt = 0L;
            p_api_dev->bm_status_cnt[ p_api_dev->biu ].err_cnt = 0L;
            p_api_dev->bm_status_cnt[ p_api_dev->biu ].bus_load_pri = 0L;
            p_api_dev->bm_status_cnt[ p_api_dev->biu ].bus_load_sec = 0L;
            p_api_dev->bm_status_cnt[ p_api_dev->biu ].bus_load_pri_avg = 0L;
            p_api_dev->bm_status_cnt[ p_api_dev->biu ].bus_load_sec_avg = 0L;
            api_io_bm_glb_cnt_ini(p_api_dev);
        }

        if( (con == API_BM_RUN_FAST) || (con == API_BUSY) )
        {
            api_io_mode_con(p_api_dev, API_BM_MODE, API_ON);
            p_api_dev->bm_status[ p_api_dev->biu ] = API_BUSY;
            p_api_dev->bm_status_dsp[ p_api_dev->biu ] = API_BM_STATUS_BUSY;
        }

        retVal = mil_tasks_busload_calc_start(p_api_dev, (AiUInt8)p_api_dev->biu);
        if(retVal)
        {
            return retVal;
        }
    }
    else
    {

        /* BM OFF */
        api_io_mode_con(p_api_dev, API_BM_MODE, API_OFF);
        p_api_dev->bm_status[ p_api_dev->biu ] = API_HALT;
        p_api_dev->bm_status_dsp[ p_api_dev->biu ] = API_BM_STATUS_HALT;

        /* Recording */
        if(p_api_dev->bm_rec[ p_api_dev->biu ].mode == API_ON)
        {

            /* Disable Interrupt */
            api_io_bm_imod_set(p_api_dev, API_SCB_IMOD_DIS);

            /* Read Monitor Status Word and verify if BM triggered */
            if(p_api_dev->bm_rec[ p_api_dev->biu ].hfi_org == p_api_dev->bm_rec[ p_api_dev->biu ].hfi_cnt)
            {
                /* HFI never occured */
                addr = ul_ConvertGRAMPtrToTSWPtr(p_api_dev, p_api_dev->biu, (*p_api_dev->cb_p[ p_api_dev->biu ]).mstp);

                /* Check if Command Word */
                ptr = (L_WORD*)API_GLB_MEM_ADDR_ABS(addr &0xFFFFFFFC);
                if( ( (BSWAP32(BSWAP32(*ptr) ) &0xF0000000L) == 0x80000000L) ||  /* CW on PRI */
                    ( (BSWAP32(BSWAP32(*ptr) ) &0xF0000000L) == 0xC0000000L) )
                {
                    /* CW on SEC */
                    ptr = (L_WORD*)API_GLB_MEM_ADDR_ABS( (addr - 8L) &0xFFFFFFFC);
                    if( (BSWAP32(*ptr) &0xF0000000L) == 0x30000000L)
                        /* Timetag High */
                        addr -= 8L;
                }
            }
            else
            {
                /* HFI already occured once */
                addr = p_api_dev->bm_rec[ p_api_dev->biu ].saddr;
                if( (addr + p_api_dev->glb_mem.biu[ p_api_dev->biu ].size.bm_buf / 2L) < (p_api_dev->glb_mem.biu[ p_api_dev->biu ].size.bm_buf + p_api_dev->glb_mem.biu[ p_api_dev->biu ].base.bm_buf) )
                    addr += p_api_dev->glb_mem.biu[ p_api_dev->biu ].size.bm_buf / 2L;
                else
                    addr -= p_api_dev->glb_mem.biu[ p_api_dev->biu ].size.bm_buf / 2L;
            }

            /* Set Start Address to copy from */
            p_api_dev->bm_rec[ p_api_dev->biu ].saddr = addr;
            mbfp = api_io_bm_read_mbfp(p_api_dev, p_api_dev->biu);

            /* Calculate remaining size in entries */
            if( (BSWAP32( (*p_api_dev->cb_p[ p_api_dev->biu ]).msw) &0x00010000L) == 0x00010000L)
            {
                if(mbfp >= addr)
                    p_api_dev->bm_rec[ p_api_dev->biu ].size = (mbfp - addr) / 4L;
                else
                    p_api_dev->bm_rec[ p_api_dev->biu ].size = ( ( (p_api_dev->glb_mem.biu[ p_api_dev->biu ].base.bm_buf + p_api_dev->glb_mem.biu[ p_api_dev->biu ].size.bm_buf) - addr) + (mbfp - p_api_dev->glb_mem.biu[ p_api_dev->biu ].base.bm_buf) ) / 4L;

                wbflush();
            }
            else
            {
                /* Size is only valid if BM triggered */
                p_api_dev->bm_rec[ p_api_dev->biu ].size = 0L;
                wbflush();
            }
        }
        else if(p_api_dev->bm_msg_flt_mode[ p_api_dev->biu ] == API_ON)
        {
            /* Message Filter */
            p_api_dev->bm_msg_flt_dsp[ p_api_dev->biu ].trigd = API_OFF; 
            p_api_dev->bm_msg_flt_dsp[ p_api_dev->biu ].ovfl = API_OFF;
            p_api_dev->bm_msg_flt_dsp[ p_api_dev->biu ].out_size = 0x0000;
            p_api_dev->bm_msg_flt_dsp[ p_api_dev->biu ].old_mbfp = p_api_dev->glb_mem.biu[ p_api_dev->biu ].base.bm_buf;
            p_api_dev->bm_msg_flt_dsp[ p_api_dev->biu ].cur_mbfp = p_api_dev->glb_mem.biu[ p_api_dev->biu ].base.bm_buf;
            p_api_dev->bm_msg_flt_dsp[ p_api_dev->biu ].cur_entry_ptr = p_api_dev->glb_mem.biu[ p_api_dev->biu ].base.bm_buf;
            p_api_dev->bm_msg_flt_dsp[ p_api_dev->biu ].cur_sw_errw = 0x0000;
            p_api_dev->bm_msg_flt_dsp[ p_api_dev->biu ].data_cnt = 0;
            p_api_dev->bm_msg_flt_dsp[ p_api_dev->biu ].bm_entry_cnt = 0L;
            p_api_dev->bm_msg_flt_dsp[ p_api_dev->biu ].cur_ttag_high = 0L;
            p_api_dev->bm_msg_flt_dsp[ p_api_dev->biu ].cur_ttag_low = 0L;
            p_api_dev->bm_msg_flt_dsp[ p_api_dev->biu ].cur_cw[0] = 0L;
            p_api_dev->bm_msg_flt_dsp[ p_api_dev->biu ].cur_cw[1] = 0L;
            p_api_dev->bm_msg_flt_dsp[ p_api_dev->biu ].cur_sw[0] = 0L;
            p_api_dev->bm_msg_flt_dsp[ p_api_dev->biu ].cur_sw[1] = 0L;
        }

        retVal = mil_tasks_busload_calc_stop(p_api_dev, (AiUInt8)p_api_dev->biu);
        if(retVal)
        {
            return retVal;
        }
    }

    return 0;

} /* end: api_bm_mode */

/*****************************************************************************/
/*                                                                           */
/*   Module : API_BM                   Submodule : API_BM_STATUS_READ        */
/*                                                                           */
/*   Author : Marco Motz               Project   : API1553-DS                */
/*                                                                           */
/*   Source : C                        Tools     : PC/AT; Norton Editor;     */
/*                                                 CYGNUS, GNU-C, As, and LD */
/*                                                 IDT-C 5.1 Toolkit         */
/*---------------------------------------------------------------------------*/
/*   Create : 01.05.98   Update : 01.11.98                                   */
/*---------------------------------------------------------------------------*/
/*    Descriptions                                                           */
/*    ------------                                                           */
/*    Inputs    : none                                                       */
/*                                                                           */
/*    Outputs   : Instruction acknowledge type [ackfl]                       */
/*                                                                           */
/*    Description :                                                          */
/*    This function handles the 'API_BM_STATUS_READ' instruction to read the */
/*    API1553-DS BM mode status and message/error counter information.       */
/*                                                                           */
/*****************************************************************************/

L_WORD api_bm_status_read(TY_API_DEV *p_api_dev, TY_MIL_COM * in, TY_MIL_COM_LS_BM_STATUS_OUTPUT * out )
{
    L_WORD msg_cnt, err_cnt;

    if(p_api_dev->bm_status[p_api_dev->biu] == API_BUSY)
    {
        UsbSynchronizeBMSystemControlBlock(p_api_dev, p_api_dev->biu, In);
    }

    /* BM Message Counter */
    msg_cnt = BSWAP32( (*p_api_dev->cb_p[ p_api_dev->biu ]).bm_mc);

    /* BM Error Counter */
    err_cnt = BSWAP32( (*p_api_dev->cb_p[ p_api_dev->biu ]).bm_ec);

    p_api_dev->bm_status_cnt[ p_api_dev->biu ].msg_cnt = msg_cnt;
    p_api_dev->bm_status_cnt[ p_api_dev->biu ].err_cnt = err_cnt;


    /* Status Info */
    out->men              = ((BSWAP32( (*p_api_dev->cb_p[ p_api_dev->biu ]).ssw) & API_SSW_MNE_BIT) >> 23);
    out->msw              = (BSWAP32( (*p_api_dev->cb_p[ p_api_dev->biu ]).msw) >> 16);
    out->msp              = (BSWAP32( (*p_api_dev->cb_p[ p_api_dev->biu ]).msw) );

    out->glb_msg_cnt = p_api_dev->bm_status_cnt[p_api_dev->biu].msg_cnt;
    out->glb_err_cnt = p_api_dev->bm_status_cnt[p_api_dev->biu].err_cnt;

    if (mil_fw_is_cmd_supported(p_api_dev, MIL_FW_CMD_BM_PRISEC_COUNTER))
    {
        out->glb_word_cnt_sec = BSWAP32((*p_api_dev->cb_p[p_api_dev->biu]).bm_wcnt_sec);
        out->glb_word_cnt_pri = BSWAP32((*p_api_dev->cb_p[p_api_dev->biu]).bm_wcnt_pri);

        out->glb_msg_cnt_sec = BSWAP32((*p_api_dev->cb_p[p_api_dev->biu]).bm_xfer_cnt_sec);
        out->glb_msg_cnt_pri = BSWAP32((*p_api_dev->cb_p[p_api_dev->biu]).bm_xfer_cnt_pri);
        out->glb_err_cnt_sec = BSWAP32((*p_api_dev->cb_p[p_api_dev->biu]).bm_err_cnt_sec);
        out->glb_err_cnt_pri = BSWAP32((*p_api_dev->cb_p[p_api_dev->biu]).bm_err_cnt_pri);

        out->bus_load_sec = p_api_dev->bm_status_cnt[p_api_dev->biu].bus_load_sec;
        out->bus_load_pri = p_api_dev->bm_status_cnt[p_api_dev->biu].bus_load_pri;
        out->bus_load_sec_avg = p_api_dev->bm_status_cnt[p_api_dev->biu].bus_load_sec_avg;
        out->bus_load_pri_avg = p_api_dev->bm_status_cnt[p_api_dev->biu].bus_load_pri_avg;
    }
    else
    {
        out->glb_word_cnt_sec = 0;
        out->glb_word_cnt_pri = 0;

        out->glb_msg_cnt_sec = 0;
        out->glb_msg_cnt_pri = 0;
        out->glb_err_cnt_sec = 0;
        out->glb_err_cnt_pri = 0;

        out->bus_load_sec = 0;
        out->bus_load_pri = 0;
        out->bus_load_sec_avg = 0;
        out->bus_load_pri_avg = 0;
    }

    return 0;

} /* end: api_bm_status_read */

/*****************************************************************************/
/*                                                                           */
/*   Module : API_BM                   Submodule : API_BM_ACT_READ           */
/*                                                                           */
/*   Author : Marco Motz               Project   : API1553-DS                */
/*                                                                           */
/*   Source : C                        Tools     : PC/AT; Norton Editor;     */
/*                                                 CYGNUS, GNU-C, As, and LD */
/*                                                 IDT-C 5.1 Toolkit         */
/*---------------------------------------------------------------------------*/
/*   Create : 01.05.98   Update : 01.11.98                                   */
/*---------------------------------------------------------------------------*/
/*    Descriptions                                                           */
/*    ------------                                                           */
/*    Inputs    : Activity List Entry number [entry_no]                      */
/*                                                                           */
/*    Outputs   : Instruction acknowledge type [ackfl]                       */
/*                                                                           */
/*    Description :                                                          */
/*    This function handles the 'API_BM_ACT_READ' instruction to read the    */
/*    current message/error counter on RT Subaddress level.                  */
/*                                                                           */
/*****************************************************************************/

L_WORD api_bm_act_read(TY_API_DEV *p_api_dev, TY_MIL_COM_LS_BM_ACT_READ_INPUT * in, TY_MIL_COM_LS_BM_ACT_READ_OUTPUT * out )
{
    short  i      = 0;
    short  j      = 0;
    short  repeat = API_ON;
    short  rt     = 0;
    short  sa     = 0;
    short  tr     = 0;
    L_WORD cc     = 0;
    L_WORD ec     = 0;
    AiPtrDiff actDescAddress;
    TY_API_BM_ACTIVITY *entry;

    WORD entry_no = (WORD)in->entry_no;


    if(p_api_dev->bm_status[p_api_dev->biu] == API_BUSY)
    {
        UsbSynchronizeMonitorActivityFilteringPage(p_api_dev, p_api_dev->biu, In);
    }

    /* Set Pointer to the start of the Activity List */
    actDescAddress = (AiPtrDiff)(GET_BM_ACTIVITY_DESC(p_api_dev, 0, 0, 0));

    out->fnd = API_OFF;
    out->rt = 0;
    out->tr = 0;
    out->sa = 0;
    out->cc = 0L; /* Message Counter */
    out->ec = 0L; /* Error Counter */
    out->et = 0L; /* Error Type */


    while( (repeat == API_ON) && (i < MAX_API_BM_RT_ACT_SIZE) )
    {
        entry = (TY_API_BM_ACTIVITY *)actDescAddress;

        cc = BSWAP32(entry->transfer_count);
        ec = BSWAP32(entry->error_count);
        if(p_api_dev->ulUseEnhancedBmActivityPage)
        {
            cc += BSWAP32(entry->transfer_count_2);
            ec += BSWAP32(entry->error_count_2);
        }

        if( (cc != 0L) || (ec != 0L) )
        {
            if(entry_no == j)
            {
                out->cc = cc;
                out->ec = ec;
                out->et = BSWAP32(entry->status);
                out->fnd = API_ON;
                rt = i / MAX_API_RT_SA_MC;
                sa = (short) (i - (rt *MAX_API_RT_SA_MC) );
                tr = sa / MAX_API_RT_MC_SA;
                sa -= (short) (tr *MAX_API_RT_MC_SA);
                out->rt = rt;
                out->sa = sa;
                out->tr = tr;
                repeat = API_OFF;
            }
            else
                j++;
        }
        i++;


        /* get next activity entry */
        if(p_api_dev->ulUseEnhancedBmActivityPage)
            actDescAddress += sizeof(struct ty_api_bm_activity_and_msg_filter_desc_enhanced);
        else
            actDescAddress += sizeof(struct ty_api_bm_activity_and_msg_filter_desc);

    }

    return 0;

} /* end: api_bm_act_read */

/*****************************************************************************/
/*    Descriptions                                                           */
/*    ------------                                                           */
/*    Inputs    : Monitor Buffer Trigger Pointer [ctp_addr]                  */
/*                Monitor Buffer Fill/End Pointer [etp_addr]                 */
/*                                                                           */
/*    Outputs   : Capture Start Pointer [addr]                               */
/*                                                                           */
/*    Description :                                                          */
/*    This function is used to return the Monitor Capture Start Address.     */
/*    for modes where no trace before trigger is used                        */
/*****************************************************************************/
static L_WORD api_bm_stack_start_of_message(TY_API_DEV *p_api_dev, short dest_biu,  L_WORD ctp_addr, L_WORD etp_addr )
{
    L_WORD addr, addr_old;
    L_WORD volatile *ptr;
    short repeat, seq;

    addr = ctp_addr;
    addr_old = addr;


    seq = 0;
    if(ctp_addr > etp_addr)
        seq = 1; /* wrap around indicator */

    repeat = API_ON;
    while(repeat == API_ON)
    {
        if (p_api_dev->bm_status[dest_biu] == API_BUSY)
        {
            UsbSynchronizeLWord(p_api_dev, addr, In);
        }

        /* Set pointer to Global Memory address */
        ptr = (L_WORD*)API_GLB_MEM_ADDR_ABS(addr);

        if( (BSWAP32(*ptr) &0xF0000000L) != 0L)
        {
            if( (BSWAP32(*ptr) &0xF0000000L) != 0x30000000L) /* Look for TTHigh as start of message */
            {
                /* Updated ? */
                addr_old = addr;
                if(seq == 1)
                {
                    /* CTP > ETP */
                    if( (addr - 4) > etp_addr)
                        addr -= 4;
                    else
                        break;
                }
                else
                {
                    /* CTP < ETP */
                    if ((addr - 4) < p_api_dev->glb_mem.biu[dest_biu].base.bm_buf)
                    {
                        /* wrap to last entry in buffer */
                        addr = p_api_dev->glb_mem.biu[dest_biu].base.bm_buf + p_api_dev->glb_mem.biu[dest_biu].size.bm_buf - 4;
                        if(addr == etp_addr)
                        {
                            addr = addr_old;
                            break;
                        }
                    }
                    else
                    {
                        if( (addr - 4) < ctp_addr)
                            addr -= 4;
                        else
                        {
                            if( (addr - 4) > etp_addr)
                                addr = etp_addr + 4;
                            else
                                break;
                        }
                    }
                }
            }
            else
            {
                /* addr points to the start */
                break;
            }
        }
        else
        {
            /* fall back to last updated entry or start addr */
            addr = addr_old;
            break;
        }
    } /* end while */

    /* Return entry start pointer */
    return (addr);

} /* end: api_bm_stack_start_of_message */

/*****************************************************************************/
/*                                                                           */
/*   Module : API_BM                   Submodule : API_BM_STACK_STP_GET      */
/*                                                                           */
/*   Author : Marco Motz               Project   : API1553-DS                */
/*                                                                           */
/*   Source : C                        Tools     : PC/AT; Norton Editor;     */
/*                                                 CYGNUS, GNU-C, As, and LD */
/*                                                 IDT-C 5.1 Toolkit         */
/*---------------------------------------------------------------------------*/
/*   Create : 01.05.98   Update : 01.05.98                                   */
/*---------------------------------------------------------------------------*/
/*    Descriptions                                                           */
/*    ------------                                                           */
/*    Inputs    : Monitor Buffer Trigger Pointer [ctp_addr]                  */
/*                Monitor Buffer Fill/End Pointer [etp_addr]                 */
/*                                                                           */
/*    Outputs   : Capture Start Pointer [addr]                               */
/*                                                                           */
/*    Description :                                                          */
/*    This function is used to return the Monitor Capture Start Address.     */
/*                                                                           */
/*****************************************************************************/

L_WORD api_bm_stack_stp_get(TY_API_DEV *p_api_dev, short dest_biu, L_WORD ctp_addr, L_WORD etp_addr)
{
  L_WORD addr, addr_old;
  L_WORD volatile *ptr;
  short repeat, seq;

  addr = ctp_addr;
  addr_old = addr;

  if(ctp_addr != etp_addr)
  {

    seq = 0;
    if(ctp_addr > etp_addr)
      seq = 1;

    repeat = API_ON;
    while(repeat == API_ON)
    {
        if(p_api_dev->bm_status[p_api_dev->biu] == API_BUSY)
        {
            UsbSynchronizeLWord(p_api_dev, addr, In);
        }

      /* Set pointer to Global Memory address */
      ptr = (L_WORD*)API_GLB_MEM_ADDR_ABS(addr);

      if( (BSWAP32(*ptr) &0xF0000000L) != 0L)
      {
         /* Updated ? */
        addr_old = addr;
        if(seq == 1)
        {
           /* CTP > ETP */
          if( (addr - 4) > etp_addr)
            addr -= 4;
          else
            repeat = API_OFF;
        }
        else
        {
           /* CTP < ETP */
          if( (addr - 4) < p_api_dev->glb_mem.biu[ p_api_dev->biu ].base.bm_buf)
          {
            /* wrap to last entry in buffer */
            addr = p_api_dev->glb_mem.biu[ p_api_dev->biu ].base.bm_buf + p_api_dev->glb_mem.biu[ p_api_dev->biu ].size.bm_buf - 4;
            /*PRINTF("stp_get: addr = %08X, mon size = %08X\n", addr, p_api_dev->glb_mem.biu[ p_api_dev->biu ].size.bm_buf, 0);*/
            if(addr == etp_addr)
            {
              addr = addr_old;
              repeat = API_OFF;
            }
          }
          else
          {
            if( (addr - 4) < ctp_addr)
              addr -= 4;
            else
            {
              if( (addr - 4) > etp_addr)
                addr = etp_addr + 4;
              else
                repeat = API_OFF;
            }
          }
        }
      }
      else
      {
        addr = addr_old;
        repeat = API_OFF;
      }
    } /* end while */
  }

  /* Return entry start pointer */
  return (addr);

} /* end: api_bm_stack_stp_get */

/*****************************************************************************/
/*                                                                           */
/*   Module : API_BM                   Submodule : API_BM_STACK_PTR_READ     */
/*                                                                           */
/*   Author : Marco Motz               Project   : API1553-DS                */
/*                                                                           */
/*   Source : C                        Tools     : PC/AT; Norton Editor;     */
/*                                                 CYGNUS, GNU-C, As, and LD */
/*                                                 IDT-C 5.1 Toolkit         */
/*---------------------------------------------------------------------------*/
/*   Create : 01.05.98   Update : 01.05.98                                   */
/*---------------------------------------------------------------------------*/
/*    Descriptions                                                           */
/*    ------------                                                           */
/*    Inputs    : none                                                       */
/*                                                                           */
/*    Outputs   : Instruction acknowledge type [ackfl]                       */
/*                                                                           */
/*    Description :                                                          */
/*    This function handles the 'API_BM_PTR_READ' instruction to read the    */
/*    actual Monitor Buffer Pointers.                                        */
/*                                                                           */
/*****************************************************************************/

L_WORD api_bm_stack_ptr_read_ir(TY_API_DEV *p_api_dev, short dest_biu, TY_MIL_COM_LS_BM_STACK_PTR_READ_OUTPUT * out)
{
    short  trg_start = 0;
    short  trg_stop  = 0;
    BYTE   st        = 0;
    L_WORD bm_mode   = 0;
    L_WORD bm_tat    = 0;


    if (p_api_dev->bm_status[dest_biu] == API_BUSY)
    {
        UsbSynchronizeLWord(p_api_dev,
            API_GLB_MEM_ADDR_REL(&p_api_dev->cb_p[dest_biu]->msw), In);
        UsbSynchronizeLWord(p_api_dev,
            API_GLB_MEM_ADDR_REL(&p_api_dev->cb_p[dest_biu]->mstp), In);
        UsbSynchronizeLWord(p_api_dev,
            API_GLB_MEM_ADDR_REL(&p_api_dev->cb_p[dest_biu]->tat), In);
    }

    /* Monitor Status Word */
    out->status = (BSWAP32((*p_api_dev->cb_p[dest_biu]).msw) >> 16);

    trg_start = API_OFF; /* Capture Start */
    if ((out->status &0x01) == 0x01)
        trg_start = API_ON;

    trg_stop = API_OFF; /* Capture Stop */
    if ((out->status &0x02) == 0x02)
        trg_stop = API_ON;

    st = 0;
    if (trg_start == API_ON)
    {
        st = 1;
        if (trg_stop == API_ON)
            st = 2;
    }

    /* Monitor state */
    out->status = st;

    /* Monitor Buffer Fill/End Pointer */
    out->etp = api_io_bm_read_mbfp(p_api_dev, dest_biu);

    /* Monitor Start Trigger Pointer */
    if (st > 0)
        out->ctp = ul_ConvertGRAMPtrToTSWPtr(p_api_dev, dest_biu, (*p_api_dev->cb_p[dest_biu]).mstp);
    else
        out->ctp = p_api_dev->glb_mem.biu[dest_biu].base.bm_buf;

    /* Monitor Start Pointer */
    if (st > 0)
    {
        bm_mode = BSWAP32((*p_api_dev->cb_p[dest_biu]).scw);
        bm_mode = (bm_mode & 0x00c00000) >> 22;
        bm_tat  = BSWAP32((*p_api_dev->cb_p[dest_biu]).tat);

        if ((bm_mode == 0) && (bm_tat == 0))
        {
            /* In recording mode we have no trace before trigger. So we search the start of the trigger message */
            out->stp = api_bm_stack_start_of_message(p_api_dev, dest_biu, out->ctp, out->etp);
        }
        else
        {
            /* Search the start of data by looking fo a not updated entry */
            out->stp = api_bm_stack_stp_get(p_api_dev, dest_biu, out->ctp, out->etp);
        }
    }
    else
    {
        out->stp = out->ctp;
    }

    return 0;

} /* end: api_bm_stack_ptr_read */

L_WORD api_bm_stack_ptr_read(TY_API_DEV *p_api_dev, TY_MIL_COM * in, TY_MIL_COM_LS_BM_STACK_PTR_READ_OUTPUT * out)
{
    return api_bm_stack_ptr_read_ir( p_api_dev, p_api_dev->biu, out );
} /* end: api_bm_stack_ptr_read */

/*****************************************************************************/
/*                                                                           */
/*   Module : API_BM                   Submodule : API_BM_STACK_ENTRY_READ   */
/*                                                                           */
/*   Author : Marco Motz               Project   : API1553-DS                */
/*                                                                           */
/*   Source : C                        Tools     : PC/AT; Norton Editor;     */
/*                                                 CYGNUS, GNU-C, As, and LD */
/*                                                 IDT-C 5.1 Toolkit         */
/*---------------------------------------------------------------------------*/
/*   Create : 01.05.98   Update : 01.05.98                                   */
/*---------------------------------------------------------------------------*/
/*    Descriptions                                                           */
/*    ------------                                                           */
/*    Inputs    : Buffer Pointer Read mode [mode]                            */
/*                Entry Offset sign [sgn]                                    */
/*                Number of Entries [offset]                                 */
/*                                                                           */
/*    Outputs   : Instruction acknowledge type [ackfl]                       */
/*                                                                           */
/*    Description :                                                          */
/*    This function handles the 'API_BM_ENTRY_READ' instruction to read the  */
/*    specified Monitor Entry.                                               */
/*                                                                           */
/*****************************************************************************/

L_WORD api_bm_stack_entry_read(TY_API_DEV *p_api_dev, TY_MIL_COM_LS_BM_STACK_ENTRY_READ_INPUT *in, TY_MIL_COM_LS_BM_STACK_ENTRY_READ_OUTPUT *out)
{
    L_WORD addr, laddr, ctp, etp;
    L_WORD volatile *ptr;
    BYTE mode     = (BYTE)in->mode;
    BYTE sgn      = (BYTE)in->sgn;
    L_WORD offset = in->offset;

    switch(mode)
    {
    case API_BM_STACK_STP:
        if( (offset < p_api_dev->glb_mem.biu[ p_api_dev->biu ].size.bm_buf / 4) && (sgn == 0) )
        {

            /* Read from Monitor Start Pointer */
            etp = api_io_bm_read_mbfp(p_api_dev, p_api_dev->biu);
            ctp = BSWAP32( (*p_api_dev->cb_p[ p_api_dev->biu ]).mstp);
            addr = api_bm_stack_stp_get(p_api_dev, p_api_dev->biu, ctp, etp);
            laddr = addr;
            laddr += (offset *4);
            if(laddr < (p_api_dev->glb_mem.biu[ p_api_dev->biu ].base.bm_buf + p_api_dev->glb_mem.biu[ p_api_dev->biu ].size.bm_buf) )
                addr = laddr;
            else
                addr = laddr - p_api_dev->glb_mem.biu[ p_api_dev->biu ].size.bm_buf;

            if(p_api_dev->bm_status[p_api_dev->biu] == API_BUSY)
            {
                UsbSynchronizeLWord(p_api_dev, (addr & 0xFFFFFFFC), In);
            }

            /* Set pointer to Global Memory and read entry */
            ptr = (L_WORD*)API_GLB_MEM_ADDR_ABS(addr &0xFFFFFFFC);
            out->bm_stack_entry = BSWAP32(*ptr);
        }
        else
            return API_ERR_ADDRESS_OUT_OF_RANGE;
        break;

    case API_BM_STACK_CTP:
        if( (offset < p_api_dev->glb_mem.biu[ p_api_dev->biu ].size.bm_buf / 4) && ( (sgn == 0) || (sgn == 1) ) )
        {

            /* Read from Monitor Start Trigger Pointer */
            addr = ul_ConvertGRAMPtrToTSWPtr(p_api_dev, p_api_dev->biu, (*p_api_dev->cb_p[ p_api_dev->biu ]).mstp);
            laddr = addr;
            if(sgn == 0)
            {
                /* positive offset */
                laddr += (offset *4);
                if(laddr < (p_api_dev->glb_mem.biu[ p_api_dev->biu ].base.bm_buf + p_api_dev->glb_mem.biu[ p_api_dev->biu ].size.bm_buf) )
                    addr = laddr;
                else
                    addr = laddr - p_api_dev->glb_mem.biu[ p_api_dev->biu ].size.bm_buf;
            }
            else
            {
                /* negative offset */
                laddr -= (offset *4);
                if(laddr < p_api_dev->glb_mem.biu[ p_api_dev->biu ].base.bm_buf)
                    addr = laddr + p_api_dev->glb_mem.biu[ p_api_dev->biu ].size.bm_buf;
                else
                    addr = laddr;
            }

            if(p_api_dev->bm_status[p_api_dev->biu] == API_BUSY)
            {
                UsbSynchronizeLWord(p_api_dev, (addr & 0xFFFFFFFC), In);
            }

            /* Set pointer to Global Memory and read entry */
            ptr = (L_WORD*)API_GLB_MEM_ADDR_ABS(addr &0xFFFFFFFC);
            out->bm_stack_entry = BSWAP32(*ptr);
        }
        else
            return API_ERR_ADDRESS_OUT_OF_RANGE;
        break;

    case API_BM_STACK_ETP:
        if( (offset < p_api_dev->glb_mem.biu[ p_api_dev->biu ].size.bm_buf / 4) && (sgn == 1) )
        {

            /* Rread from Monitor Buffer Fill/End Pointer */
            addr = api_io_bm_read_mbfp(p_api_dev, p_api_dev->biu);
            laddr = addr;
            laddr -= (offset *4);
            if(laddr < p_api_dev->glb_mem.biu[ p_api_dev->biu ].base.bm_buf)
                addr = laddr + p_api_dev->glb_mem.biu[ p_api_dev->biu ].size.bm_buf;
            else
                addr = laddr;

            if(p_api_dev->bm_status[p_api_dev->biu] == API_BUSY)
            {
                UsbSynchronizeLWord(p_api_dev, (addr & 0xFFFFFFFC), In);
            }

            /* Set pointer to Global Memory and read entry */
            ptr = (L_WORD*)API_GLB_MEM_ADDR_ABS(addr &0xFFFFFFFC);
            out->bm_stack_entry = BSWAP32(*ptr);
        }
        else
            return API_ERR_ADDRESS_OUT_OF_RANGE;
        break;

    case API_BM_STACK_ABS:
        addr = offset;

        if(p_api_dev->bm_status[p_api_dev->biu] == API_BUSY)
        {
            UsbSynchronizeLWord(p_api_dev, (addr & 0xFFFFFFFC), In);
        }

        /* Set pointer to Global Memory and read entry */
        ptr = (L_WORD*)API_GLB_MEM_ADDR_ABS(addr &0xFFFFFFFC);
        out->bm_stack_entry = BSWAP32(*ptr);

        break;

    default:
        return API_ERR_INVALID_MODE;
    }

    return 0;

} /* end: api_bm_stack_entry_read */

/*****************************************************************************/
/*                                                                           */
/*   Module : API_BM                   Submodule : API_BM_STACK_FIND_ENTRY   */
/*                                                                           */
/*   Author : Marco Motz               Project   : API1553-DS                */
/*                                                                           */
/*   Source : C                        Tools     : PC/AT; Norton Editor;     */
/*                                                 CYGNUS, GNU-C, As, and LD */
/*                                                 IDT-C 5.1 Toolkit         */
/*---------------------------------------------------------------------------*/
/*   Create : 01.05.98   Update : 01.05.98                                   */
/*---------------------------------------------------------------------------*/
/*    Descriptions                                                           */
/*    ------------                                                           */
/*    Inputs    : Search Specification [fspec]                               */
/*                Entry Word to find [fword]                                 */
/*                Entry Mask Word [fmask]                                    */
/*                                                                           */
/*    Outputs   : Search fail/success [status]                               */
/*                Entry Search Address [saddr]                               */
/*                                                                           */
/*    Description :                                                          */
/*    This function is used to search on the specified Bus Monitor Entriy.   */
/*                                                                           */
/*****************************************************************************/

static short api_bm_stack_find_entry(TY_API_DEV *p_api_dev, WORD fspec, WORD fword, WORD fmask, L_WORD *saddr)
{
  L_WORD entry, laddr, cur_addr, start_addr;
  L_WORD volatile *ptr;
  short status, repeat;
  WORD data, tag, temp;

  status = API_OK;

  cur_addr =  *saddr;
  start_addr =  *saddr;

  repeat = API_ON;
  while(repeat == API_ON)
  {
      if(p_api_dev->bm_status[p_api_dev->biu] == API_BUSY)
      {
          UsbSynchronizeLWord(p_api_dev, cur_addr, In);
      }

    /* Get Monitor Entry */
    ptr = (L_WORD*)API_GLB_MEM_ADDR_ABS(cur_addr);
    entry = BSWAP32(*ptr);
    tag = HI_WORD(entry);
    data = LO_WORD(entry);

    if( (tag &0xF000) != 0)
    {
       /* Updated ? */
      switch(fspec &0x0003)
      {
        case 0:
           /* Look for Bus Word Entry */
          if( (tag &0x8000) == 0x8000)
          {
            switch( (tag &0xF000) >> 12)
            {
            case 0x8:
               /* CW1 on PRI */
              temp = 0x2100;
              break;
            case 0x9:
               /* CW2 on PRI */
              temp = 0x0500;
              break;
            case 0xA:
               /* DW on PRI */
              temp = 0x1100;
              break;
            case 0xB:
               /* SW on PRI */
              temp = 0x0900;
              break;
            case 0xC:
               /* CW1 on SEC */
              temp = 0x2200;
              break;
            case 0xD:
               /* CW2 on SEC */
              temp = 0x0600;
              break;
            case 0xE:
               /* DW on SEC */
              temp = 0x1200;
              break;
            case 0xF:
               /* SW on SEC */
              temp = 0x0A00;
              break;
            default:
              temp = 0xFF00;
              break;
            }

            if( (fspec &0xFF00) == temp)
            {
              if(fword == (data &fmask) )
                repeat = API_OFF;
            }
          }
          break;
        case 1:
           /* Look for Error Word Entry */
          if( (tag &0xF000) == 0x1000)
          {
            if(fword == (data &fmask) )
              repeat = API_OFF;
          }
          break;
        case 2:
           /* Look for TimeTag Low Entry */
          break;
        case 3:
           /* Look for TimeTag High Entry */
          break;
        default:
          break;
      }

      if(repeat == API_ON)
      {
        laddr = cur_addr;

        if( (laddr + 4) < (p_api_dev->glb_mem.biu[ p_api_dev->biu ].base.bm_buf + p_api_dev->glb_mem.biu[ p_api_dev->biu ].size.bm_buf) )
         /* Last Entry */
          cur_addr += 4;
        else
          cur_addr = p_api_dev->glb_mem.biu[ p_api_dev->biu ].base.bm_buf;

        if(cur_addr == start_addr)
        {
           /* If wrapped around */
          status = API_ERR;
          repeat = API_OFF;
        }
      }
    }
    else
    {
      status = API_ERR;
      repeat = API_OFF;
    }
  }

  *saddr = cur_addr;

  return (status);

} /* end: api_bm_stack_find_entry */

/*****************************************************************************/
/*                                                                           */
/*   Module : API_BM                   Submodule : API_BM_STACK_ENTRY_FIND   */
/*                                                                           */
/*   Author : Marco Motz               Project   : API1553-DS                */
/*                                                                           */
/*   Source : C                        Tools     : PC/AT; Norton Editor;     */
/*                                                 CYGNUS, GNU-C, As, and LD */
/*                                                 IDT-C 5.1 Toolkit         */
/*---------------------------------------------------------------------------*/
/*   Create : 01.05.98   Update : 01.05.98                                   */
/*---------------------------------------------------------------------------*/
/*    Descriptions                                                           */
/*    ------------                                                           */
/*    Inputs    : Buffer Pointer Read mode [mode]                            */
/*                Entry Offset sign [sgn]                                    */
/*                Number of Entries [offset]                                 */
/*                Search Specification [spec]                                */
/*                Entry Word to find [etw]                                   */
/*                Entry MAsk Word [etm]                                      */
/*                                                                           */
/*    Outputs   : Instruction acknowledge type [ackfl]                       */
/*                                                                           */
/*    Description :                                                          */
/*    This function handles the 'API_BM_ENTRY_FIND' instruction to find the  */
/*    specified Monitor Entry.                                               */
/*                                                                           */
/*****************************************************************************/

L_WORD api_bm_stack_entry_find(TY_API_DEV *p_api_dev, TY_MIL_COM_LS_BM_STACK_ENTRY_FIND_INPUT * in, TY_MIL_COM_LS_BM_STACK_ENTRY_FIND_OUTPUT * out)
{
    L_WORD addr, laddr, ctp, etp;
    L_WORD volatile *ptr;

    BYTE mode     = (BYTE)in->mode;
    BYTE sgn      = (BYTE)in->sgn;
    L_WORD offset = in->offset;
    WORD spec     = (WORD)in->spec;
    WORD etw      = (WORD)in->etw;
    WORD etm      = (WORD)in->etm;

    switch(mode)
    {
    case API_BM_STACK_STP:
        if( (offset < p_api_dev->glb_mem.biu[ p_api_dev->biu ].size.bm_buf / 4) && (sgn == 0) )
        {
            if(p_api_dev->bm_status[p_api_dev->biu] == API_BUSY)
            {
                UsbSynchronizeLWord(p_api_dev, 
                    API_GLB_MEM_ADDR_REL(&p_api_dev->cb_p[p_api_dev->biu]->mstp), In);
            }

            /* Read from Monitor Start Pointer */
            etp = api_io_bm_read_mbfp(p_api_dev, p_api_dev->biu);
            ctp = ul_ConvertGRAMPtrToTSWPtr(p_api_dev, p_api_dev->biu, (*p_api_dev->cb_p[ p_api_dev->biu ]).mstp);
            addr = api_bm_stack_stp_get(p_api_dev, p_api_dev->biu, ctp, etp);
            laddr = addr;
            laddr += (offset *4);
            if(laddr < (p_api_dev->glb_mem.biu[ p_api_dev->biu ].base.bm_buf + p_api_dev->glb_mem.biu[ p_api_dev->biu ].size.bm_buf) )
                addr = laddr;
            else
                addr = laddr - p_api_dev->glb_mem.biu[ p_api_dev->biu ].size.bm_buf;

            /* Search on entry */
            if(api_bm_stack_find_entry(p_api_dev, spec, etw, etm, &addr) == API_OK)
            {
                out->fnd = 1;
                out->eptr = addr;

                /* Set pointer to Global Memory and read entry */
                ptr = (L_WORD*)API_GLB_MEM_ADDR_ABS(addr &0xFFFFFFFC);
                out->entry = BSWAP32(*ptr);
            }
        }
        else
            return API_ERR_ADDRESS_OUT_OF_RANGE;
        break;

    case API_BM_STACK_CTP:
        if( (offset < p_api_dev->glb_mem.biu[ p_api_dev->biu ].size.bm_buf / 4) && ( (sgn == 0) || (sgn == 1) ) )
        {

            if(p_api_dev->bm_status[p_api_dev->biu] == API_BUSY)
            {
                UsbSynchronizeLWord(p_api_dev, 
                    API_GLB_MEM_ADDR_REL(&p_api_dev->cb_p[p_api_dev->biu]->mstp), In);
            }

            /* Read from Monitor Start Trigger Pointer */
            addr = ul_ConvertGRAMPtrToTSWPtr(p_api_dev, p_api_dev->biu, (*p_api_dev->cb_p[ p_api_dev->biu ]).mstp);
            laddr = addr;
            if(sgn == 0)
            {
                /* positive offset */
                laddr += (offset *4);
                if(laddr < (p_api_dev->glb_mem.biu[ p_api_dev->biu ].base.bm_buf + p_api_dev->glb_mem.biu[ p_api_dev->biu ].size.bm_buf) )
                    addr = laddr;
                else
                    addr = laddr - p_api_dev->glb_mem.biu[ p_api_dev->biu ].size.bm_buf;
            }
            else
            {
                /* negative offset */
                laddr -= (offset *4);
                if(laddr < p_api_dev->glb_mem.biu[ p_api_dev->biu ].base.bm_buf)
                    addr = laddr + p_api_dev->glb_mem.biu[ p_api_dev->biu ].size.bm_buf;
                else
                    addr = laddr;
            }

            /* Search on entry */
            if(api_bm_stack_find_entry(p_api_dev, spec, etw, etm, &addr) == API_OK)
            {
                out->fnd = 1;
                out->eptr = addr;

                /* Set pointer to Global Memory and read entry */
                ptr = (L_WORD*)API_GLB_MEM_ADDR_ABS(addr &0xFFFFFFFC);
                out->entry = BSWAP32(*ptr);
            }
        }
        else
            return API_ERR_ADDRESS_OUT_OF_RANGE;
        break;

    case API_BM_STACK_ETP:
        if( (offset < p_api_dev->glb_mem.biu[ p_api_dev->biu ].size.bm_buf / 4) && (sgn == 1) )
        {

            /* Rread from Monitor Buffer Fill/End Pointer */
            addr = api_io_bm_read_mbfp(p_api_dev, p_api_dev->biu);
            laddr = addr;
            laddr -= (offset *4);
            if(laddr < p_api_dev->glb_mem.biu[ p_api_dev->biu ].base.bm_buf)
                addr = laddr + p_api_dev->glb_mem.biu[ p_api_dev->biu ].size.bm_buf;
            else
                addr = laddr;

            /* Search on entry */
            if(api_bm_stack_find_entry(p_api_dev, spec, etw, etm, &addr) == API_OK)
            {
                out->fnd = 1;
                out->eptr = addr;

                /* Set pointer to Global Memory and read entry */
                ptr = (L_WORD*)API_GLB_MEM_ADDR_ABS(addr &0xFFFFFFFC);
                out->entry = BSWAP32(*ptr);
            }
        }
        else
            return API_ERR_ADDRESS_OUT_OF_RANGE;
        break;

    default:
        return API_ERR_INVALID_MODE;
    }

    return 0;

} /* end: api_bm_stack_entry_find */

/*****************************************************************************/
/*                                                                           */
/*   Module : API_BM                   Submodule : API_BM_REC_STATUS_READ    */
/*                                                                           */
/*   Author : Marco Motz               Project   : API1553-DS                */
/*                                                                           */
/*   Source : C                        Tools     : PC/AT; Norton Editor;     */
/*                                                 CYGNUS, GNU-C, As, and LD */
/*                                                 IDT-C 5.1 Toolkit         */
/*---------------------------------------------------------------------------*/
/*   Create : 01.05.98   Update : 10.06.99                                   */
/*---------------------------------------------------------------------------*/
/*    Descriptions                                                           */
/*    ------------                                                           */
/*    Inputs    : none                                                       */
/*                                                                           */
/*    Outputs   : Instruction acknowledge type [ackfl]                       */
/*                                                                           */
/*    Description :                                                          */
/*    This function handles the 'API_BM_REC_STATUS_READ' instruction to read */
/*    the actual Recording status.                                           */
/*                                                                           */
/*****************************************************************************/

L_WORD api_bm_rec_status_read(TY_API_DEV *p_api_dev, TY_MIL_COM * in, TY_MIL_COM_LS_BM_REC_STATUS_OUTPUT * out )
{
    short repeat;
    L_WORD volatile *ptr;


    ptr = &p_api_dev->bm_rec[ p_api_dev->biu ].size;

  /* Confirm that no interruption happened */
  repeat = API_ON;
  while(repeat == API_ON)
  {
    out->hfi_cnt = p_api_dev->bm_rec[ p_api_dev->biu ].hfi_cnt;
    out->saddr = p_api_dev->bm_rec[ p_api_dev->biu ].saddr;
    out->size = p_api_dev->bm_rec[ p_api_dev->biu ].size;
    wbflush();

    /* wait */

    WAIT_CLOCKS(p_api_dev->clk_1us *5);

    if( (out->hfi_cnt == p_api_dev->bm_rec[ p_api_dev->biu ].hfi_cnt) && 
        (out->saddr == p_api_dev->bm_rec[ p_api_dev->biu ].saddr) && 
        (out->size == p_api_dev->bm_rec[ p_api_dev->biu ].size) )
    {
      if(out->size != 0)
        *ptr = 0;

      repeat = API_OFF;

    }
  }
  out->status  = p_api_dev->bm_status_dsp[ p_api_dev->biu ];


    return 0;

} /* end: api_bm_rec_status_read */

/*****************************************************************************/
/*                                                                           */
/*   Module : API_BM                   Submodule : API_BM_COPY_REC_DAT       */
/*                                                                           */
/*   Author : Marco Motz               Project   : API1553-DS                */
/*                                                                           */
/*   Source : C                        Tools     : PC/AT; Norton Editor;     */
/*                                                 CYGNUS, GNU-C, As, and LD */
/*                                                 IDT-C 5.1 Toolkit         */
/*---------------------------------------------------------------------------*/
/*   Create : 01.05.98   Update : 24.05.00                                   */
/*---------------------------------------------------------------------------*/
/*    Descriptions                                                           */
/*    ------------                                                           */
/*    Inputs    : Start Address to copy data from [saddr]                    */
/*                Amount of Words to copy [size]                             */
/*                                                                           */
/*    Outputs   : Instruction acknowledge type [ackfl]                       */
/*                                                                           */
/*    Description :                                                          */
/*    This function handles the 'API_BM_COPY_REC_DAT' instruction to copy    */
/*    the specified amount of Monitor data.                                  */
/*                                                                           */
/*****************************************************************************/

L_WORD api_bm_copy_rec_dat(TY_API_DEV *p_api_dev, TY_MIL_COM_LS_BM_COPY_REC_DAT_INPUT * in, TY_MIL_COM_ACK * out )
{
    L_WORD volatile *dest_ptr;
    L_WORD volatile *src_ptr;
    L_WORD i            = 0;
    L_WORD size1        = 0;
    L_WORD size2        = 0;
    L_WORD* dest_addr   = (L_WORD*) API_REC_SHARED_MEM_START_ADDR;
    L_WORD start_offset = in->start_offset;
    L_WORD size         = in->size;


    if(size <= p_api_dev->glb_mem.biu[ p_api_dev->biu ].size.bm_buf / 2L / 4L)
    {
        /* Half Buffer Size */

        /* Set Pointers */
        src_ptr = (L_WORD*)API_GLB_MEM_ADDR_ABS(start_offset); /* Global RAM */
        dest_ptr = (L_WORD*)dest_addr; /* Shared RAM */

        /* Calculate copy areas */
        if( (start_offset + size * 4L) > (p_api_dev->glb_mem.biu[ p_api_dev->biu ].size.bm_buf + p_api_dev->glb_mem.biu[ p_api_dev->biu ].base.bm_buf) )
        {
            size1 = ( (p_api_dev->glb_mem.biu[ p_api_dev->biu ].size.bm_buf + p_api_dev->glb_mem.biu[ p_api_dev->biu ].base.bm_buf) - start_offset) / 4L;
            size2 = size - size1;
        }
        else
        {
            size1 = size;
            size2 = 0L;
        }

        if(p_api_dev->bm_status[p_api_dev->biu] == API_BUSY)
        {
            UsbSynchronizeMonitorDataBuffer(p_api_dev, p_api_dev->biu, In);
        }

        /* Copy Data from Global to Shared RAM */
        for(i = 0; i < size1; i++)
        {
            *dest_ptr = *src_ptr;
            dest_ptr++;
            src_ptr++;
        }
        if(size2 != 0)
        {
           /* Wrap to BM Buffer Start Address */
           src_ptr = (L_WORD*)API_GLB_MEM_ADDR_ABS(p_api_dev->glb_mem.biu[ p_api_dev->biu ].base.bm_buf); /* Global RAM */
           for(i = 0; i < size2; i++)
           {
               *dest_ptr = *src_ptr;
               dest_ptr++;
               src_ptr++;
           }
        }
        (*p_api_dev->cb_p[p_api_dev->biu]).bm_get = API_GLB_MEM_ADDR_REL(src_ptr);

    }
    else
        return API_ERR_NO_SPACE_LEFT;

    return 0;

} /* end: api_bm_copy_rec_dat */

/*****************************************************************************/
/*                                                                           */
/*   Module : API_BM                   Submodule : API_BM_INI_MSG_FLT        */
/*                                                                           */
/*   Author : Marco Motz               Project   : API1553-DS                */
/*                                                                           */
/*   Source : C                        Tools     : PC/AT; Norton Editor;     */
/*                                                 CYGNUS, GNU-C, As, and LD */
/*                                                 IDT-C 5.1 Toolkit         */
/*---------------------------------------------------------------------------*/
/*   Create : 01.05.98   Update : 01.05.98                                   */
/*---------------------------------------------------------------------------*/
/*    Descriptions                                                           */
/*    ------------                                                           */
/*    Inputs    : Remote Terminal Address [rt]                               */
/*                Message Capture Enable Bits for Receive  RT SA [rcv_sa]    */
/*                Message Capture Enable Bits for Transmit RT SA [xmt_sa]    */
/*                Message Capture Enable Bits for Receive  SA MC [rcv_mc]    */
/*                Message Capture Enable Bits for Transmit SA MC [xmt_mc]    */
/*                                                                           */
/*    Outputs   : Instruction acknowledge type [ackfl]                       */
/*                                                                           */
/*    Description :                                                          */
/*    This function handles the 'API_BM_INI_MSG_FLT' instruction to init the */
/*    Message Capture Bits of the API1553-DS BM.                             */
/*                                                                           */
/*****************************************************************************/

L_WORD api_bm_ini_msg_flt(TY_API_DEV *p_api_dev, TY_MIL_COM_LS_BM_INI_MSG_FLT_INPUT * in, TY_MIL_COM_ACK * out)
{
    if(in->rt >= MAX_API_RT)
        return API_ERR_INVALID_RT;


    api_io_bm_ini_msg_flt(p_api_dev, (BYTE)in->rt, in->rcv_sa, in->xmt_sa, in->rcv_mc, in->xmt_mc);

    return 0;
}


/*****************************************************************************/
/*                                                                           */
/*   Module : API_BM                   Submodule : API_BM_INI_MSG_ILL        */
/*                                                                           */
/*   Author : Marco Motz               Project   : API1553-DS                */
/*                                                                           */
/*   Source : C                        Tools     : PC/AT; Norton Editor;     */
/*                                                 CYGNUS, GNU-C, As, and LD */
/*                                                 IDT-C 5.1 Toolkit         */
/*---------------------------------------------------------------------------*/
/*   Create : 01.05.98   Update : 01.05.98                                   */
/*---------------------------------------------------------------------------*/
/*    Descriptions                                                           */
/*    ------------                                                           */
/*    Inputs    : Remote Terminal Address [rt]                               */
/*                Message Illegalize Bits for Receive  RT SA [rcv_sa]        */
/*                Message Illegalize Bits for Transmit RT SA [xmt_sa]        */
/*                Message Illegalize Bits for Receive  SA MC [rcv_mc]        */
/*                Message Illegalize Bits for Transmit SA MC [xmt_mc]        */
/*                                                                           */
/*    Outputs   : Instruction acknowledge type [ackfl]                       */
/*                                                                           */
/*    Description :                                                          */
/*    This function handles the 'API_BM_INI_MSG_ILL' instruction to init the */
/*    Message Illegalize Bits of the API1553-DS BM.                          */
/*                                                                           */
/*****************************************************************************/

L_WORD api_bm_ini_msg_ill(TY_API_DEV *p_api_dev, TY_MIL_COM_LS_BM_INI_MSG_ILL_INPUT * in, TY_MIL_COM_ACK * out)
{
    if(in->rt >= MAX_API_RT)
        return API_ERR_INVALID_RT;


    api_io_bm_ini_msg_ill(p_api_dev, (BYTE)in->rt, in->rcv_sa, in->xmt_sa, in->rcv_mc, in->xmt_mc);

    return 0;
}

/*****************************************************************************/
/*                                                                           */
/*   Module : API_BM                   Submodule : API_BM_INI_FTW            */
/*                                                                           */
/*   Author : Marco Motz               Project   : API1553-DS                */
/*                                                                           */
/*   Source : C                        Tools     : PC/AT; Norton Editor;     */
/*                                                 CYGNUS, GNU-C, As, and LD */
/*                                                 IDT-C 5.1 Toolkit         */
/*---------------------------------------------------------------------------*/
/*   Create : 01.05.98   Update : 01.05.98                                   */
/*---------------------------------------------------------------------------*/
/*    Descriptions                                                           */
/*    ------------                                                           */
/*    Inputs    : Stop Trigger Mask pattern     [htm]                        */
/*                Stop Trigger Compare pattern  [htc]                        */
/*                Start Trigger Mask pattern    [stm]                        */
/*                Start Trigger Compare pattern [stc]                        */
/*                                                                           */
/*    Outputs   : Instruction acknowledge type [ackfl]                       */
/*                                                                           */
/*    Description :                                                          */
/*    This function handles the 'API_BM_INI_FTW' instruction to init the     */
/*    API1553-DS BM Function Trigger Word.                                   */
/*                                                                           */
/*****************************************************************************/

L_WORD api_bm_ini_ftw(TY_API_DEV *p_api_dev, TY_MIL_COM_LS_BM_INI_FTW_INPUT * in, TY_MIL_COM_ACK * out)
{
    api_io_bm_ini_ftw(p_api_dev, (BYTE)in->con, (BYTE)in->htm, (BYTE)in->htc, (BYTE)in->stm, (BYTE)in->stc);

    return 0;
}

/*****************************************************************************/
/*                                                                           */
/*   Module : API_BM                   Submodule : API_BM_INI_TIW            */
/*                                                                           */
/*   Author : Marco Motz               Project   : API1553-DS                */
/*                                                                           */
/*   Source : C                        Tools     : PC/AT; Norton Editor;     */
/*                                                 CYGNUS, GNU-C, As, and LD */
/*                                                 IDT-C 5.1 Toolkit         */
/*---------------------------------------------------------------------------*/
/*   Create : 01.05.98   Update : 01.05.98                                   */
/*---------------------------------------------------------------------------*/
/*    Descriptions                                                           */
/*    ------------                                                           */
/*    Inputs    : External Trigger TCB Index   [eti]                         */
/*                Any Error TCB Index          [aei]                         */
/*                Sequence Trigger 0 TCB Index [ti0]                         */
/*                Sequence Trigger 1 TCB Index [ti1]                         */
/*                                                                           */
/*    Outputs   : Instruction acknowledge type [ackfl]                       */
/*                                                                           */
/*    Description :                                                          */
/*    This function handles the 'API_BM_INI_TIW' instruction to init the     */
/*    API1553-DS BM Trigger Index Word.                                      */
/*                                                                           */
/*****************************************************************************/

L_WORD api_bm_ini_tiw(TY_API_DEV *p_api_dev, TY_MIL_COM_LS_BM_INI_TIW_INPUT * in, TY_MIL_COM_ACK * out)
{
    if (in->con == API_BM_WRITE_TI1)
    {
        if (!mil_fw_is_cmd_supported(p_api_dev, MIL_FW_CMD_BM_SEVERAL_TCBS))
            return API_ERR_CMD_NOT_SUPPORTED_BY_FW;
    }

    api_io_bm_ini_tiw(p_api_dev, (BYTE)in->con, (BYTE)in->eti, (BYTE)in->aei, (BYTE)in->ti0, (BYTE)in->ti1);

    return 0;
}

/*****************************************************************************/
/*                                                                           */
/*   Module : API_BM                   Submodule : API_BM_INI_TCI            */
/*                                                                           */
/*   Author : Marco Motz               Project   : API1553-DS                */
/*                                                                           */
/*   Source : C                        Tools     : PC/AT; Norton Editor;     */
/*                                                 CYGNUS, GNU-C, As, and LD */
/*                                                 IDT-C 5.1 Toolkit         */
/*---------------------------------------------------------------------------*/
/*   Create : 01.05.98   Update : 01.05.98                                   */
/*---------------------------------------------------------------------------*/
/*    Descriptions                                                           */
/*    ------------                                                           */
/*    Inputs    : Remote Terminal Address [rt]                               */
/*                RT Subaddress/Modecode [sa_mc]                             */
/*                Subaddress Type [sa_type]                                  */
/*                Time Tag Mode [tagm]                                       */
/*                Trigger Block Function [tfct]                              */
/*                Trigger Control Block Index [tid]                          */
/*                                                                           */
/*    Outputs   : Instruction acknowledge type [ackfl]                       */
/*                                                                           */
/*    Description :                                                          */
/*    This function handles the 'API_BM_INI_TCI' instruction to init the     */
/*    specified API1553-DS BM Trigger Control Index field.                   */
/*                                                                           */
/*****************************************************************************/

L_WORD api_bm_ini_tci(TY_API_DEV *p_api_dev, TY_MIL_COM_LS_BM_INI_TCI_INPUT * in, TY_MIL_COM_ACK * out)
{
    if(in->rt >= MAX_API_RT)
        return API_ERR_INVALID_RT; 

    if(in->sa_mc >= MAX_API_RT_MC_SA)
        return API_ERR_INVALID_RT_SA; 

    if( (in->sa_type != API_RT_SA_TYPE_RCV) && (in->sa_type != API_RT_SA_TYPE_XMT) && (in->sa_type != API_RT_MC_TYPE_RCV) && (in->sa_type != API_RT_MC_TYPE_XMT) )
        return API_ERR_INVALID_RT_SA_TYPE; 

    api_io_bm_rt_act_con(p_api_dev, (BYTE)in->rt, (BYTE)in->sa_mc, (BYTE)in->sa_type, (BYTE)in->tagm, (BYTE)in->tfct, (BYTE)in->tid);

    return 0;
}

/*****************************************************************************/
/*                                                                           */
/*   Module : API_BM                   Submodule : API_BM_INI_TCB            */
/*                                                                           */
/*   Author : Ralf Kuderer             Project   : API1553-DS                */
/*                                                                           */
/*   Source : C                        Tools     : PC/AT; Norton Editor;     */
/*                                                 CYGNUS, GNU-C, As, and LD */
/*                                                 IDT-C 5.1 Toolkit         */
/*---------------------------------------------------------------------------*/
/*   Create : 01.05.98   Update : 17.03.00                                   */
/*---------------------------------------------------------------------------*/
/*    Descriptions                                                           */
/*    ------------                                                           */
/*    Inputs    : Trigger Control Block Index [tid]                          */
/*                Trigger Control Block enable/disable [ten]                 */
/*                Trigger Type [tt]                                          */
/*                Strobe on Trigger [sot]                                    */
/*                Interrupt on Trigger [tri]                                 */
/*                Inversion of Limit Check [inv]                             */
/*                Reset Mask for Monitor Status Trigger Word [tres]          */
/*                Set Mask for Monitor Status Trigger Word [tset]            */
/*                Trigger Specification [tsp]                                */
/*                Next Trigger Control Block Index to use [next]             */
/*                Next Trigger Control Block Index to use on EOM [eom]       */
/*                Trigger Data Word [tdw]                                    */
/*                Trigger Mask Word [tmw]                                    */
/*                Trigger Upper Limit value [tuli]                           */
/*                Trigger Lower Limit value [tlli]                           */
/*                                                                           */
/*    Outputs   : Instruction acknowledge type [ackfl]                       */
/*                                                                           */
/*    Description :                                                          */
/*    This function handles the 'API_BM_INI_TCB' instruction to define a     */
/*    API1553-DS BM Trigger Control Block descriptor.                        */
/*                                                                           */
/*****************************************************************************/

L_WORD api_bm_ini_tcb(TY_API_DEV *p_api_dev, TY_MIL_COM_LS_BM_INI_TCB_INPUT * in, TY_MIL_COM_ACK * out)
{
    if( (in->ten != API_BM_TCB_TRG_DIS) && 
        (in->ten != API_BM_TCB_TRG_ENA) )
      return API_ERR_PARAM3_NOT_IN_RANGE;

    if( (in->tt != API_BM_TCB_TYPE_ERR)  && 
        (in->tt != API_BM_TCB_TYPE_EXT)  &&
        (in->tt != API_BM_TCB_TYPE_RCV)  &&
        (in->tt != API_BM_TCB_TYPE_DATA) && 
        (in->tt != API_BM_TCB_TYPE_HS) )
      return API_ERR_PARAM4_NOT_IN_RANGE;

    if (in->next != 0xFF)
    {
        if (!mil_fw_is_cmd_supported(p_api_dev, MIL_FW_CMD_BM_SEVERAL_TCBS))
            return API_ERR_CMD_NOT_SUPPORTED_BY_FW;
    }

    if (in->tt == API_BM_TCB_TYPE_DATA)
    {
        if (!mil_fw_is_cmd_supported(p_api_dev, MIL_FW_CMD_BM_TRIGGER_ON_DATA_VALUE))
            return API_ERR_CMD_NOT_SUPPORTED_BY_FW;
    }

    if(in->ten == API_BM_TCB_TRG_DIS)
      api_io_bm_tcb_desc_ini(p_api_dev, (BYTE)in->tid);
    else
      api_io_bm_tcb_desc_con(p_api_dev, (BYTE)in->tid, (BYTE)in->tt, (BYTE)in->sot, (BYTE)in->tri, (BYTE)in->inv, (BYTE)in->tres, (BYTE)in->tset, (BYTE)in->tsp, (BYTE)in->next, (BYTE)in->eom, (WORD)in->tdw, (WORD)in->tmw, (WORD)in->tuli, (WORD)in->tlli);

    return 0;

}

/*****************************************************************************/
/*                                                                           */
/*   Module : API_BM                   Submodule : API_BM_RT_SA_ACT_READ     */
/*                                                                           */
/*   Author : Marco Motz               Project   : API1553-DS                */
/*                                                                           */
/*   Source : C                        Tools     : PC/AT; Norton Editor;     */
/*                                                 CYGNUS, GNU-C, As, and LD */
/*                                                 IDT-C 5.1 Toolkit         */
/*---------------------------------------------------------------------------*/
/*   Create : 01.05.98   Update : 01.05.98                                   */
/*---------------------------------------------------------------------------*/
/*    Descriptions                                                           */
/*    ------------                                                           */
/*    Inputs    : Remote Terminal Address [rt]                               */
/*                RT Subaddress/Modecode [sa_mc]                             */
/*                Subaddress Type [sa_type]                                  */
/*                                                                           */
/*    Outputs   : Instruction acknowledge type [ackfl]                       */
/*                                                                           */
/*    Description :                                                          */
/*    This function handles the 'API_BM_RT_SA_ACT_READ' instruction to read  */
/*    the transfer/error counter locations from the BM Activity and Filter   */
/*    page for the specified RT Subaddress.                                  */
/*                                                                           */
/*****************************************************************************/

static BYTE api_bm_rt_sa_act_read_internal(TY_API_DEV *p_api_dev, BYTE rt, BYTE sa, BYTE sa_type, L_WORD *sa_ctrl_sum, L_WORD *sa_msg_sum, L_WORD *sa_err_sum )
{
    L_WORD rx_sa_ctrl, rx_sa_msg, rx_sa_err;
    L_WORD tx_sa_ctrl, tx_sa_msg, tx_sa_err;
    L_WORD rx_mc_ctrl, rx_mc_msg, rx_mc_err;
    L_WORD tx_mc_ctrl, tx_mc_msg, tx_mc_err;
    TY_API_BM_ACTIVITY *entry;

    *sa_ctrl_sum = 0L;
    *sa_msg_sum  = 0L;
    *sa_err_sum  = 0L;



    switch(sa_type)
    {
    case API_RT_SA_TYPE_RCV:
    case API_RT_SA_TYPE_XMT:
    case API_RT_MC_TYPE_RCV:
    case API_RT_MC_TYPE_XMT:
        entry = (TY_API_BM_ACTIVITY*) GET_BM_ACTIVITY_DESC(p_api_dev, rt, sa, sa_type);
        *sa_ctrl_sum = BSWAP16(entry->status);
        *sa_msg_sum  = BSWAP32(entry->transfer_count);
        *sa_err_sum  = BSWAP32(entry->error_count);
        if (p_api_dev->ulUseEnhancedBmActivityPage)   *sa_msg_sum += BSWAP32(entry->transfer_count_2);
        if (p_api_dev->ulUseEnhancedBmActivityPage)   *sa_err_sum += BSWAP32(entry->error_count_2);
        break;

    case API_RT_SA_TYPE_ALL:  /* a sum of all SA-types */
        entry = (TY_API_BM_ACTIVITY*) GET_BM_ACTIVITY_DESC(p_api_dev, rt, sa, API_RT_SA_TYPE_RCV);
        rx_sa_ctrl = BSWAP16(entry->status);
        rx_sa_msg  = BSWAP32(entry->transfer_count);
        rx_sa_err  = BSWAP32(entry->error_count);
        if (p_api_dev->ulUseEnhancedBmActivityPage)   rx_sa_msg += BSWAP32(entry->transfer_count_2);
        if (p_api_dev->ulUseEnhancedBmActivityPage)   rx_sa_err += BSWAP32(entry->error_count_2);

        entry = (TY_API_BM_ACTIVITY*) GET_BM_ACTIVITY_DESC(p_api_dev, rt, sa, API_RT_SA_TYPE_XMT);
        tx_sa_ctrl = BSWAP16(entry->status);
        tx_sa_msg  = BSWAP32(entry->transfer_count);
        tx_sa_err  = BSWAP32(entry->error_count);
        if (p_api_dev->ulUseEnhancedBmActivityPage)   tx_sa_msg += BSWAP32(entry->transfer_count_2);
        if (p_api_dev->ulUseEnhancedBmActivityPage)   tx_sa_err += BSWAP32(entry->error_count_2);

        entry = (TY_API_BM_ACTIVITY*) GET_BM_ACTIVITY_DESC(p_api_dev, rt, sa, API_RT_MC_TYPE_RCV);
        rx_mc_ctrl = BSWAP16(entry->status);
        rx_mc_msg  = BSWAP32(entry->transfer_count);
        rx_mc_err  = BSWAP32(entry->error_count);
        if (p_api_dev->ulUseEnhancedBmActivityPage)   rx_mc_msg += BSWAP32(entry->transfer_count_2);
        if (p_api_dev->ulUseEnhancedBmActivityPage)   rx_mc_err += BSWAP32(entry->error_count_2);

        entry = (TY_API_BM_ACTIVITY*) GET_BM_ACTIVITY_DESC(p_api_dev, rt, sa, API_RT_MC_TYPE_XMT);
        tx_mc_ctrl = BSWAP16(entry->status);
        tx_mc_msg  = BSWAP32(entry->transfer_count);
        tx_mc_err  = BSWAP32(entry->error_count);
        if (p_api_dev->ulUseEnhancedBmActivityPage)   tx_mc_msg += BSWAP32(entry->transfer_count_2);
        if (p_api_dev->ulUseEnhancedBmActivityPage)   tx_mc_err += BSWAP32(entry->error_count_2);

        *sa_ctrl_sum = rx_sa_ctrl | tx_sa_ctrl | rx_mc_ctrl | tx_mc_ctrl;
        *sa_msg_sum  = rx_sa_msg + tx_sa_msg + rx_mc_msg + tx_mc_msg;
        *sa_err_sum  = rx_sa_err + tx_sa_err + rx_mc_err + tx_mc_err;
        break;

    default:
        break;
    }

    return 0;

} /* end: api_bm_rt_sa_act_read_internal */



L_WORD api_bm_rt_sa_act_read(TY_API_DEV *p_api_dev, BYTE rt, BYTE sa, BYTE sa_type)
{
    if(p_api_dev->bm_status[p_api_dev->biu] == API_BUSY)
    {
        UsbSynchronizeMonitorActivityPageOfSA(p_api_dev, p_api_dev->biu, rt, sa, sa_type, In);
    }

    api_bm_rt_sa_act_read_internal( p_api_dev, rt, sa, sa_type, &p_api_dev->bm_rt_act_dsp.et, &p_api_dev->bm_rt_act_dsp.cc, &p_api_dev->bm_rt_act_dsp.ec );

    return 0;
} /* end: api_bm_rt_sa_act_read */


L_WORD api_bm_rt_sa_act_read_cmd(TY_API_DEV *p_api_dev, TY_MIL_COM_LS_BM_RT_SA_ACT_READ_INPUT * in, TY_MIL_COM_LS_BM_RT_SA_ACT_READ_OUTPUT * out )
{
    if(p_api_dev->bm_status[p_api_dev->biu] == API_BUSY)
    {
        UsbSynchronizeMonitorActivityPageOfSA(p_api_dev, p_api_dev->biu, (BYTE)in->rt, (BYTE)in->sa, (BYTE)in->sa_type, In);
    }

    api_bm_rt_sa_act_read_internal( p_api_dev, (BYTE)in->rt, (BYTE)in->sa, (BYTE)in->sa_type, &out->et, &out->cc, &out->ec );

    return 0;
}

/*****************************************************************************/
/*                                                                           */
/*   Module : API_BM                   Submodule : API_BM_RT_ACT_READ        */
/*                                                                           */
/*   Author : Marco Motz               Project   : API1553-DS                */
/*                                                                           */
/*   Source : C                        Tools     : PC/AT; Norton Editor;     */
/*                                                 CYGNUS, GNU-C, As, and LD */
/*                                                 IDT-C 5.1 Toolkit         */
/*---------------------------------------------------------------------------*/
/*   Create : 01.05.98   Update : 01.05.98                                   */
/*---------------------------------------------------------------------------*/
/*    Descriptions                                                           */
/*    ------------                                                           */
/*    Inputs    : Remote Terminal Address [rt]                               */
/*                                                                           */
/*    Outputs   : Instruction acknowledge type [ackfl]                       */
/*                                                                           */
/*    Description :                                                          */
/*    This function handles the 'API_BM_RT_ACT_READ' instruction to read     */
/*    the transfer/error counter locations from the BM Activity and Filter   */
/*    page for the specified RT.                                             */
/*                                                                           */
/*****************************************************************************/

L_WORD api_bm_rt_act_read(TY_API_DEV *p_api_dev, TY_MIL_COM_LS_BM_RT_ACT_READ_INPUT * in, TY_MIL_COM_LS_BM_RT_ACT_READ_OUTPUT * out )
{
    L_WORD retval = 0;
    L_WORD sa_ctrl_sum  = 0;
    L_WORD sa_msg_sum   = 0;
    L_WORD sa_err_sum   = 0;
    L_WORD rt_ctrl_sum  = 0;
    L_WORD rt_msg_sum   = 0;
    L_WORD rt_err_sum   = 0;

    BYTE rt = (BYTE)in->rt;
    short j;


    if(p_api_dev->bm_status[p_api_dev->biu] == API_BUSY)
    {
        UsbSynchronizeMonitorActivityPageOfRT(p_api_dev, p_api_dev->biu, rt, In);
    }

    for(j = 0; j < MAX_API_RT_MC_SA; j++)
    {
        /* initialize sums of the RT SA */
        sa_ctrl_sum = 0L;
        sa_msg_sum = 0L;
        sa_err_sum = 0L;

        retval = api_bm_rt_sa_act_read_internal( p_api_dev, rt, (BYTE)j /*sa*/, API_RT_SA_TYPE_ALL, &sa_ctrl_sum, &sa_msg_sum, &sa_err_sum );

        rt_ctrl_sum |= sa_ctrl_sum;
        rt_msg_sum += sa_msg_sum;
        rt_err_sum += sa_err_sum;
    }

    out->cc = rt_msg_sum; /* Message Counter */
    out->ec = rt_err_sum; /* Error Counter */
    out->et = rt_ctrl_sum; /* Error Type */


    return retval;
} /* end: api_bm_rt_act_read */


/*****************************************************************************/
/*                                                                           */
/*   Module : API_BM                   Submodule : API_BM_INI_MSG_FLT_REC    */
/*                                                                           */
/*   Author : Michael Stahl            Project   : API1553-DS                */
/*                                                                           */
/*   Source : C                        Tools     : PC/AT; Norton Editor;     */
/*                                                 CYGNUS, GNU-C, As, and LD */
/*                                                 IDT-C 5.1 Toolkit         */
/*---------------------------------------------------------------------------*/
/*   Create : 01.05.99   Update : 01.05.99                                   */
/*---------------------------------------------------------------------------*/
/*    Descriptions                                                           */
/*    ------------                                                           */
/*    Inputs    : Amount of Filter Command Words [cnt]                       */
/*                Filter Command Word (MS-Byte first) [cw]                   */
/*                First Data Word position in message 'cw' [pos1]            */
/*                Last Data Word position in message 'cw' [pos2]             */
/*                                                                           */
/*    Outputs   : Instruction acknowledge type [ackfl]                       */
/*                                                                           */
/*    Description :                                                          */
/*    This function handles the 'API_BM_INI_MSG_FLT_REC' instruction to init */
/*    the BM Message Filter Recording of the API1553-DS BM.                  */
/*                                                                           */
/*****************************************************************************/

L_WORD api_bm_ini_msg_flt_rec(TY_API_DEV *p_api_dev, TY_MIL_COM_LS_BM_INI_MSG_FLT_REC_INPUT * in, TY_MIL_COM_ACK * out)
{
    return api_io_bm_ini_msg_flt_rec(p_api_dev, (BYTE)in->cnt, &in->mrec[0]);

}

/*****************************************************************************/
/*                                                                           */
/*   Module : API_BM                   Submodule : API_BM_READ_MSG_FLT_REC   */
/*                                                                           */
/*   Author : Michael Stahl            Project   : API1553-DS                */
/*                                                                           */
/*   Source : C                        Tools     : PC/AT; Norton Editor;     */
/*                                                 CYGNUS, GNU-C, As, and LD */
/*                                                 IDT-C 5.1 Toolkit         */
/*---------------------------------------------------------------------------*/
/*   Create : 01.05.99   Update : 01.05.99                                   */
/*---------------------------------------------------------------------------*/
/*    Descriptions                                                           */
/*    ------------                                                           */
/*    Inputs    : Message Filter Recording Mode [mode]                       */
/*              : Message Filter Recording Format [con]                      */
/*              : Size of allocated buffer in Words [max_size]               */
/*              : Size of allocated messages [max_msg]                       */
/*                                                                           */
/*    Outputs   : Instruction acknowledge type [ackfl]                       */
/*                                                                           */
/*    Description :                                                          */
/*    This function handles the 'API_BM_READ_MSG_FLT_REC' instruction to     */
/*    read the BM Message Filter Recording data of the API1553-DS BM.        */
/*                                                                           */
/*****************************************************************************/

L_WORD api_bm_read_msg_flt_rec(TY_API_DEV *p_api_dev, TY_MIL_COM_LS_BM_MSG_FLT_REC_INPUT * in, TY_MIL_COM_LS_BM_MSG_FLT_REC_OUTPUT * out )
{
    api_io_bm_read_msg_flt_rec(p_api_dev, (BYTE)in->mode, (BYTE)in->con, in->max_size, in->max_msg);

    out->ovfl     = p_api_dev->bm_msg_flt_dsp[ p_api_dev->biu ].ovfl;
    out->out_size = p_api_dev->bm_msg_flt_dsp[ p_api_dev->biu ].out_size;

    /*printf("out.size in words: %d\n\r",p_api_dev->bm_msg_flt_dsp[p_api_dev->biu].out_size); */
    return 0;
}

/*****************************************************************************/
/*                                                                           */
/*   Module : API_BM                   Submodule : API_BM_DYNTAG_MON_DEF     */
/*                                                                           */
/*   Author : Marco Motz               Project   : API1553-DS                */
/*                                                                           */
/*   Source : C                        Tools     : PC/AT; Norton Editor;     */
/*                                                 CYGNUS, GNU-C, As, and LD */
/*                                                 IDT-C 5.1 Toolkit         */
/*---------------------------------------------------------------------------*/
/*   Create : 11.11.05   Update : 11.11.05                                   */
/*---------------------------------------------------------------------------*/
/*    Descriptions                                                           */
/*    ------------                                                           */
/*    Inputs    : Dyntag Monitor Identifier [id]                             */
/*                Dyntag Monitor control (enable/disable) [con]              */
/*                RT-Address [rt]                                            */
/*                RT-Subaddress [sa]                                         */
/*                RT-Subaddress-Type [sa_type]                               */
/*                Reserved [res]                                             */
/*                Dyntag Type [dyntag_type]                                  */
/*                Dyntag Word Position [dyntag_wpos]                         */
/*                Dyntag Bit Position [dyntag_bpos]                          */
/*                Dyntag Bit Position [dyntag_wpos]                          */
/*                Dyntag Step size [dyntag_step]                             */
/*                                                                           */
/*    Outputs   : Instruction acknowledge type [ackfl]                       */
/*                                                                           */
/*    Description :                                                          */
/*    This function handles the 'API_BM_DYNTAG_MON_DEF' instruction used to  */
/*    define a Dyntag Monitor ID.                                            */
/*                                                                           */
/*****************************************************************************/

L_WORD api_bm_dyntag_mon_def(TY_API_DEV *p_api_dev, TY_MIL_COM_LS_BM_DYNTAG_DEF_INPUT * in, TY_MIL_COM_ACK * out )
{
    L_WORD tag_ix,rt_tmp,sa_tmp,sa_type_tmp;
    BYTE id            = (BYTE)in->id;
    BYTE con           = (BYTE)in->con;
    BYTE rt            = (BYTE)in->rt;
    BYTE sa            = (BYTE)in->sa;
    BYTE sa_type       = (BYTE)in->sa_type;
    WORD dyntag_type   = (WORD)in->dyntag_type;
    WORD dyntag_wpos   = (WORD)in->dyntag_wpos;

#ifdef DEBUG_DYNTAGMON
  PRINTF2("api_bm_dyntag_mon_def id=%d con=%d",id,con);
  PRINTF3(" rt=%d sa=%d sa_type=%d\n",rt,sa,sa_type);
#endif  

    tag_ix = 0L;


    if((id <= 0)                        ||
       (id > MAX_API_BM_DYNTAG_MON_ID)  ||
       (rt >= MAX_API_RT)               ||
       (sa >= MAX_API_RT_MC_SA)         ||
       ((sa_type != API_RT_SA_TYPE_RCV) && (sa_type != API_RT_SA_TYPE_XMT)) )
    {
        return API_NAK;
    }



    tag_ix = (L_WORD)id - 1;

    switch(con)
    {
      case API_BM_DYNTAG_MON_DIS:
        rt_tmp = p_api_dev->bm_ls_dyntagm_set[ p_api_dev->biu ].dyntag[tag_ix].rt;
        sa_tmp = p_api_dev->bm_ls_dyntagm_set[ p_api_dev->biu ].dyntag[tag_ix].sa;
        sa_type_tmp = p_api_dev->bm_ls_dyntagm_set[ p_api_dev->biu ].dyntag[tag_ix].sa_type;
        api_io_bm_dyntagm_entry_init(p_api_dev,tag_ix); 
        api_io_bm_dyntagm_con(p_api_dev, tag_ix, rt_tmp, sa_tmp, sa_type_tmp);
        break;        

      case API_BM_DYNTAG_MON_ENA:
        if (p_api_dev->bm_ls_dyntagm_set[ p_api_dev->biu ].dyntag[tag_ix].con == API_BM_DYNTAG_MON_DIS)
        {
          p_api_dev->bm_ls_dyntagm_set[ p_api_dev->biu ].dyntag[tag_ix].con = API_BM_DYNTAG_MON_ENA;
          p_api_dev->bm_ls_dyntagm_set[ p_api_dev->biu ].dyntag[tag_ix].rt = rt;
          p_api_dev->bm_ls_dyntagm_set[ p_api_dev->biu ].dyntag[tag_ix].sa = sa;
          p_api_dev->bm_ls_dyntagm_set[ p_api_dev->biu ].dyntag[tag_ix].sa_type = sa_type;

          p_api_dev->bm_ls_dyntagm_set[ p_api_dev->biu ].dyntag[tag_ix].res = 0;
          p_api_dev->bm_ls_dyntagm_set[ p_api_dev->biu ].dyntag[tag_ix].dyntag_type = (BYTE)dyntag_type;
          p_api_dev->bm_ls_dyntagm_set[ p_api_dev->biu ].dyntag[tag_ix].dyntag_wpos = dyntag_wpos;
          /* p_api_dev->bm_ls_dyntagm_set[ p_api_dev->biu ].dyntag[tag_ix].dyntag_bpos = dyntag_bpos; */
          /* p_api_dev->bm_ls_dyntagm_set[ p_api_dev->biu ].dyntag[tag_ix].dyntag_blen = dyntag_blen; */
          /* p_api_dev->bm_ls_dyntagm_set[ p_api_dev->biu ].dyntag[tag_ix].dyntag_step = dyntag_step; */
        
          p_api_dev->bm_ls_dyntagm_set[ p_api_dev->biu ].dyntag[tag_ix].stale_cnt = 0L;
          p_api_dev->bm_ls_dyntagm_set[ p_api_dev->biu ].dyntag[tag_ix].bad_cnt = 0L;
          p_api_dev->bm_ls_dyntagm_set[ p_api_dev->biu ].dyntag[tag_ix].good_cnt = 0L;
          p_api_dev->bm_ls_dyntagm_set[ p_api_dev->biu ].dyntag[tag_ix].dw = 0L;
          p_api_dev->bm_ls_dyntagm_set[ p_api_dev->biu ].dyntag[tag_ix].state = 0L;
          p_api_dev->bm_ls_dyntagm_set[ p_api_dev->biu ].dyntag[tag_ix].stale_cnt_contig = 0L;
          p_api_dev->bm_ls_dyntagm_set[ p_api_dev->biu ].dyntag[tag_ix].good_cnt_contig = 0L;

          api_io_bm_dyntagm_con(p_api_dev, tag_ix, rt, sa, sa_type);
        }
        break;

      default:
        return API_ERR_INVALID_CON;
    }


  return 0;

} /* end: api_bm_dyntag_mon_def */

/*****************************************************************************/
/*                                                                           */
/*   Module : API_BM                   Submodule : API_BM_DYNTAG_MON_READ    */
/*                                                                           */
/*   Author : Marco Motz               Project   : API1553-DS                */
/*                                                                           */
/*   Source : C                        Tools     : PC/AT; Norton Editor;     */
/*                                                 CYGNUS, GNU-C, As, and LD */
/*                                                 IDT-C 5.1 Toolkit         */
/*---------------------------------------------------------------------------*/
/*   Create : 11.11.05   Update : 11.11.05                                   */
/*---------------------------------------------------------------------------*/
/*    Descriptions                                                           */
/*    ------------                                                           */
/*    Inputs    : Dyntag Monitor Identifier [id]                             */
/*                Reserved [res]                                             */
/*                                                                           */
/*    Outputs   : Instruction acknowledge type [ackfl]                       */
/*                                                                           */
/*    Description :                                                          */
/*    This function handles the 'API_BM_DYNTAG_MON_READ' instruction used    */
/*    to read status information from the specified Dyntag Monitor ID.       */
/*                                                                           */
/*****************************************************************************/

L_WORD api_bm_dyntag_mon_read(TY_API_DEV *p_api_dev, TY_MIL_COM_LS_BM_DYNTAG_MON_READ_INPUT * in, TY_MIL_COM_LS_BM_DYNTAG_MON_READ_OUTPUT * out )
{
    L_WORD tag_ix = in->id - 1;

#ifdef DEBUG_DYNTAGMONx
    PRINTF2("api_bm_dyntag_mon_read id=%d res=%d\n",id,res);
#endif

    if((in->id > 0) && (in->id <= MAX_API_BM_DYNTAG_MON_ID))
    {
        out->stale_cnt = p_api_dev->bm_ls_dyntagm_set[ p_api_dev->biu ].dyntag[tag_ix].stale_cnt;
        out->bad_cnt   = p_api_dev->bm_ls_dyntagm_set[ p_api_dev->biu ].dyntag[tag_ix].bad_cnt;
        out->good_cnt  = p_api_dev->bm_ls_dyntagm_set[ p_api_dev->biu ].dyntag[tag_ix].good_cnt;

        return API_OK;
    }
    else
    {
        return API_ERR_INVALID_ID;
    }
}




NAMESPACE1553_END

