/* SPDX-FileCopyrightText: 2001-2024 AIM GmbH <info@aim-online.com> */
/* SPDX-License-Identifier: MIT OR GPL-2.0-or-later */

/*! \file api_sys.c
    System Control module of the 1553 ASP Driver SW. 
    This module comprises the handler routines for the System Control    
    instructions.
*/

#include "mil_tsw_includes.h"
#include "api_int_loglist.h"

#if MAX_API_SYS_LS_SYSTAGS != 255
# define LS_SYSTAG_ID_IN_RANGE(id) (((id)>0) && ((id) <= MAX_API_SYS_LS_SYSTAGS))
#else
/* gcc warns about (BYTE)id <= 255 is always true */
# define LS_SYSTAG_ID_IN_RANGE(id) (((id)>0))
#endif

/*#include "i39_defv.h"
 * include causes errors -> Workarround: */
extern BYTE uc_HsSubAddress;


NAMESPACE1553_START


/* to satisfy GNU */
L_WORD efex_switch( TY_API_DEV *p_api_dev, BYTE Board);






TY_MIL_COM_TABLE_ENTRY axTargetSysCommands[] = {
  {MIL_COM_SYS_OFFSET,          (TY_MIL_COM_FUNC_PTR)NULL,                      "void",                      sizeof(TY_MIL_COM),                             0                                              },
  {MIL_COM_SYS_PING,            (TY_MIL_COM_FUNC_PTR)api_sys_ping,              "api_sys_ping",              sizeof(TY_MIL_COM_SYS_PING_INPUT),              0                                              },
  {MIL_COM_SYS_DMA_SETUP_SGL,   (TY_MIL_COM_FUNC_PTR)api_dma_setup_sgl,         "api_dma_setup_sgl",         sizeof(TY_MIL_COM_SYS_DMA_SETUP_SGL_INPUT),     sizeof(TY_MIL_COM_ACK)                         },
  {MIL_COM_SYS_MALLOC,          (TY_MIL_COM_FUNC_PTR)api_sys_malloc,            "api_sys_malloc",            sizeof(TY_MIL_COM_SYS_MALLOC_INPUT),            sizeof(TY_MIL_COM_SYS_MALLOC_OUTPUT)           },
  {MIL_COM_SYS_FREE,            (TY_MIL_COM_FUNC_PTR)api_sys_free,              "api_sys_free",              sizeof(TY_MIL_COM_SYS_FREE_INPUT),              sizeof(TY_MIL_COM_ACK)                         },
  {MIL_COM_SYS_GET_BOARD_INFO,  (TY_MIL_COM_FUNC_PTR)api_sys_get_board_info,    "api_sys_get_board_info",    sizeof(TY_MIL_COM_SYS_GET_INFO_INPUT),          sizeof(TY_MIL_COM_SYS_GET_INFO_OUTPUT)         },
  {MIL_COM_SYS_GET_MEM_LOCATION,(TY_MIL_COM_FUNC_PTR)api_sys_get_mem_loc,       "api_sys_get_mem_loc",       sizeof(TY_MIL_COM_SYS_GET_MEM_LOCATION_INPUT),  sizeof(TY_MIL_COM_SYS_GET_MEM_LOCATION_OUTPUT) },
  {MIL_COM_SYS_MEM_PART_SET,    (TY_MIL_COM_FUNC_PTR)api_sys_set_mem_part_cmd,  "api_sys_set_mem_part_cmd",  sizeof(TY_MIL_COM_SYS_MEM_PART_SET_INPUT),      sizeof(TY_MIL_COM_SYS_MEM_PART_SET_OUTPUT)     },
  {MIL_COM_SYS_MEM_PART_GET,    (TY_MIL_COM_FUNC_PTR)api_sys_get_mem_part_cmd,  "api_sys_get_mem_part_cmd",  sizeof(TY_MIL_COM_SYS_MEM_PART_GET_INPUT),      sizeof(TY_MIL_COM_SYS_MEM_PART_GET_OUTPUT)     },
  {MIL_COM_SYS_SYSTAG_CON,      (TY_MIL_COM_FUNC_PTR)api_sys_ls_systag_con,     "api_sys_ls_systag_con",     sizeof(TY_MIL_COM_SYS_SYSTAG_CON_INPUT),        sizeof(TY_MIL_COM_ACK)                         },
  {MIL_COM_SYS_SYSTAG_DEF,      (TY_MIL_COM_FUNC_PTR)api_sys_ls_systag_def,     "api_sys_ls_systag_def",     sizeof(TY_MIL_COM_SYS_SYSTAG_DEF_INPUT),        sizeof(TY_MIL_COM_ACK)                         },
  {MIL_COM_SYS_MILBUS_PROT,     (TY_MIL_COM_FUNC_PTR)api_sys_def_milbus_prot,   "api_sys_def_milbus_prot",   sizeof(TY_MIL_COM_SYS_MILBUS_PROT_INPUT),       sizeof(TY_MIL_COM_ACK)                         },
  {MIL_COM_SYS_RESP_TOUT_DEF,   (TY_MIL_COM_FUNC_PTR)api_sys_def_resp_tout,     "api_sys_def_resp_tout",     sizeof(TY_MIL_COM_SYS_RESP_TOUT_DEF_INPUT),     sizeof(TY_MIL_COM_ACK)                         },
  {MIL_COM_SYS_PXI_CON,         (TY_MIL_COM_FUNC_PTR)api_sys_pxi_con,           "api_sys_pxi_con",           sizeof(TY_MIL_COM_SYS_PXI_CON_INPUT),           sizeof(TY_MIL_COM_ACK)                         },
  {MIL_COM_SYS_DISCRETES_READ,  (TY_MIL_COM_FUNC_PTR)api_sys_discretes_read,    "api_sys_discretes_read",    sizeof(TY_MIL_COM),                             sizeof(TY_MIL_COM_SYS_DISCRETES_READ_OUTPUT)   },
  {MIL_COM_SYS_DISCRETES_WRITE, (TY_MIL_COM_FUNC_PTR)api_sys_discretes_write,   "api_sys_discretes_write",   sizeof(TY_MIL_COM_SYS_DISCRETES_WRITE_INPUT),   sizeof(TY_MIL_COM_ACK)                         },
  {MIL_COM_SYS_DISCRETES_INIT,  (TY_MIL_COM_FUNC_PTR)api_sys_discretes_init,    "api_sys_discretes_init",    sizeof(TY_MIL_COM_SYS_DISCRETES_INIT_INPUT),    sizeof(TY_MIL_COM_ACK)                         },
  {MIL_COM_SYS_SYNC_COUNTER_SET,(TY_MIL_COM_FUNC_PTR)api_sys_sync_counter_set,  "api_sys_sync_counter_set",  sizeof(TY_MIL_COM_SYS_SYNC_COUNTER_SET_INPUT),  sizeof(TY_MIL_COM_ACK)                         },
  {MIL_COM_SYS_SYNC_COUNTER_GET,(TY_MIL_COM_FUNC_PTR)api_sys_sync_counter_get,  "api_sys_sync_counter_get",  sizeof(TY_MIL_COM),                             sizeof(TY_MIL_COM_SYS_SYNC_COUNTER_GET_OUTPUT) },
  {MIL_COM_SYS_IRIG_GET,        (TY_MIL_COM_FUNC_PTR)api_sys_irig_get,          "api_sys_irig_get",          sizeof(TY_MIL_COM),                             sizeof(TY_MIL_COM_SYS_IRIG_GET_OUTPUT)         },
  {MIL_COM_SYS_IRIG_STATUS_GET, (TY_MIL_COM_FUNC_PTR)api_sys_irig_status_get,   "api_sys_irig_status_get",   sizeof(TY_MIL_COM),                             sizeof(TY_MIL_COM_SYS_IRIG_STATUS_GET_OUTPUT)  },
  {MIL_COM_SYS_IRIG_SET,        (TY_MIL_COM_FUNC_PTR)api_sys_irig_set,          "api_sys_irig_set",          sizeof(TY_MIL_COM_SYS_IRIG_SET_INPUT),          sizeof(TY_MIL_COM_ACK)                         },
  {MIL_COM_SYS_IRIG_SOURCE_SET, (TY_MIL_COM_FUNC_PTR)api_sys_irig_source_set,   "api_sys_irig_source_set",   sizeof(TY_MIL_COM_SYS_IRIG_SOURCE_SET_INPUT),   sizeof(TY_MIL_COM_ACK)                         },
  {MIL_COM_SYS_RESET,           (TY_MIL_COM_FUNC_PTR)api_sys_reset_cmd,         "api_sys_reset_cmd",         sizeof(TY_MIL_COM_SYS_RESET_INPUT),             sizeof(TY_MIL_COM_SYS_RESET_OUTPUT)            },
  {MIL_COM_SYS_RESET_INFO,      (TY_MIL_COM_FUNC_PTR)api_sys_reset_info,        "api_sys_reset_info",        sizeof(TY_MIL_COM),                             sizeof(TY_MIL_COM_SYS_RESET_OUTPUT)            },
  {MIL_COM_SYS_DISCRETES_REC_INIT,     (TY_MIL_COM_FUNC_PTR)api_sys_discretes_rec_init,     "api_sys_discretes_rec_init",     sizeof(TY_MIL_COM_SYS_DISCRETES_REC_INIT_INPUT),     sizeof(TY_MIL_COM_ACK)                               },
  {MIL_COM_SYS_DISCRETES_CONFIG_READ,  (TY_MIL_COM_FUNC_PTR)api_sys_discretes_config_read,  "api_sys_discretes_config_read",  sizeof(TY_MIL_COM),                                  sizeof(TY_MIL_COM_SYS_DISCRETES_CONFIG_READ_OUTPUT)  },
  {MIL_COM_SYS_TRIGGER_EDGE_INPUT_SET, (TY_MIL_COM_FUNC_PTR)api_sys_trigger_edge_input_set, "api_sys_trigger_edge_input_set", sizeof(TY_MIL_COM_WITH_VALUE), sizeof(TY_MIL_COM_ACK)            },
  {MIL_COM_SYS_TRIGGER_EDGE_INPUT_GET, (TY_MIL_COM_FUNC_PTR)api_sys_trigger_edge_input_get, "api_sys_trigger_edge_input_get", sizeof(TY_MIL_COM),            sizeof(TY_MIL_COM_ACK_WITH_VALUE) },
  {MIL_COM_SYS_DISCRETES_INFO, (TY_MIL_COM_FUNC_PTR)api_sys_discretes_info_read, "api_sys_discretes_info_read", sizeof(TY_MIL_COM), sizeof(TY_MIL_COM_SYS_DISCRETES_INFO_READ_OUTPUT) },
  {MIL_COM_SYS_PXI_GEO_ADDR, (TY_MIL_COM_FUNC_PTR)api_sys_pxi_geo_addr, "api_sys_pxi_geo_addr", sizeof(TY_MIL_COM), sizeof(TY_MIL_COM_ACK_WITH_VALUE) },
  {MIL_COM_SYS_TRIGGER_DIGITAL_LOOP_CON, (TY_MIL_COM_FUNC_PTR)api_sys_trigger_digital_loop_con, "api_sys_trigger_digital_loop_con", sizeof(TY_MIL_COM_WITH_VALUE), sizeof(TY_MIL_COM_ACK) },
  {MIL_COM_SYS_TRIGGER_DIGITAL_LOOP_GET, (TY_MIL_COM_FUNC_PTR)api_sys_trigger_digital_loop_get, "api_sys_trigger_digital_loop_get", sizeof(TY_MIL_COM),            sizeof(TY_MIL_COM_ACK_WITH_VALUE) }
};



void api_sys_command_table( TY_MIL_COM_TABLE_ENTRY ** table, AiUInt32 * size )
{
    *size  = sizeof(axTargetSysCommands) / sizeof(TY_MIL_COM_TABLE_ENTRY);
    *table = axTargetSysCommands;
}



L_WORD api_sys_reset_info( TY_API_DEV *p_api_dev, TY_MIL_COM * in, TY_MIL_COM_SYS_RESET_OUTPUT * out )
{
    out->mbufs    = p_api_dev->sys_mem[ p_api_dev->biu ].mbufs;
    out->sbufs    = p_api_dev->sys_mem[ p_api_dev->biu ].sbufs;
    out->m39_bufs = api39_sys_mem[ p_api_dev->hs_stream ].uc_m39_bufs;
    out->s39_bufs = api39_sys_mem[ p_api_dev->hs_stream ].uc_s39_bufs;
	
    out->maddr    = p_api_dev->sys_mem[ p_api_dev->biu ].maddr;
    out->saddr    = p_api_dev->sys_mem[ p_api_dev->biu ].saddr;
    out->m39_addr = api39_sys_mem[ p_api_dev->hs_stream ].ul_m39_offset;
    out->s39_addr = api39_sys_mem[ p_api_dev->hs_stream ].ul_s39_addr;

    return 0;
}

/*****************************************************************************/
/*                                                                           */
/*   Module : API_SYS                  Submodule : API_SYS_RESET             */
/*                                                                           */
/*   Author : Marco Motz               Project   : API1553-DS                */
/*                                                                           */
/*   Source : C                        Tools     : PC/AT; Norton Editor;     */
/*                                                 CYGNUS, GNU-C, As, and LD */
/*                                                 IDT-C 5.1 Toolkit         */
/*---------------------------------------------------------------------------*/
/*   Create : 01.05.98   Update : 11.05.09                                   */
/*---------------------------------------------------------------------------*/
/*    Descriptions                                                           */
/*    ------------                                                           */
/*    Inputs    : Reset control [rc]                                         */
/*                                                                           */
/*    Outputs   : Instruction acknowledge type [ackfl]                       */
/*                                                                           */
/*    Description :                                                          */
/*    This function handles the 'API_RESET' instruction to initialize the    */
/*    API1553-DS hardware and software to an initial state.                  */
/*                                                                           */
/*****************************************************************************/
L_WORD api_sys_reset_cmd( TY_API_DEV *p_api_dev, TY_MIL_COM_SYS_RESET_INPUT * in, TY_MIL_COM_SYS_RESET_OUTPUT * out )
{
    L_WORD ret_val = 0;

    /* Set the single function globals back to default */
    if (API_DEVICE_MODE_IS_SINGLEFUNCTION(p_api_dev->drv_mode[p_api_dev->biu]))
    {
      p_api_dev->drv_exe[ p_api_dev->biu ]       = 0;
      p_api_dev->drv_rt_status[ p_api_dev->biu ] = 0;
    }


    ret_val = api_sys_reset( p_api_dev, (BYTE)in->mode );

    if( ret_val == 0 )
        if( ul_StreamIsHS( p_api_dev, p_api_dev->biu ) )
            ret_val = api39_sys_reset( p_api_dev, (BYTE)in->mode,  (BYTE)in->mbufs );

    if( ret_val == 0 )
    {
        out->mbufs    = p_api_dev->sys_mem[ p_api_dev->biu ].mbufs;
        out->sbufs    = p_api_dev->sys_mem[ p_api_dev->biu ].sbufs;
        out->m39_bufs = api39_sys_mem[ p_api_dev->hs_stream ].uc_m39_bufs; /* Used in Api3910CmdHsReset only */
        out->s39_bufs = api39_sys_mem[ p_api_dev->hs_stream ].uc_s39_bufs; /* Used in Api3910CmdHsReset only */

        out->maddr    = p_api_dev->sys_mem[ p_api_dev->biu ].maddr;
        out->saddr    = p_api_dev->sys_mem[ p_api_dev->biu ].saddr;
        out->m39_addr = api39_sys_mem[ p_api_dev->hs_stream ].ul_m39_offset;
        out->s39_addr = api39_sys_mem[ p_api_dev->hs_stream ].ul_s39_addr;
    }

    return ret_val;
}


BYTE api_sys_reset_legacy(TY_API_DEV *p_api_dev, BYTE rc)
{
    BYTE   ackfl   = API_NAK;
    L_WORD ret_val = 0;


    ret_val = api_sys_reset( p_api_dev, rc );

    if( ret_val == 0 )
    {
        ackfl = API_ACK2;
        p_api_dev->b_length = 4;
        p_api_dev->w_length = 0;
        p_api_dev->lw_length = 4;
        p_api_dev->ack_b_ptr = &(p_api_dev->sys_mem[ p_api_dev->biu ].mbufs);
        p_api_dev->ack_lw_ptr = &(p_api_dev->sys_mem[ p_api_dev->biu ].maddr);
    }
    else
    {
        ackfl = API_NAK;
        SET_NAK_INFO(ret_val);
    }

    return ackfl;
}


L_WORD api_sys_reset(TY_API_DEV *p_api_dev, BYTE rc)
{
L_WORD ret_val = 0;
BYTE rt;
short k, l;
L_WORD j;
L_WORD ul_Hid;

  if( (API_RESET_USE_COUNTER_MODE_2 & rc) != 0 )
  {
    api_io_set_special_op_mode(p_api_dev, 1 );
  }
  else
  {
    api_io_set_special_op_mode(p_api_dev, 0 );
  }



  if( ( (API_RESET_ONLY_IF_NOT_ALREADY_RESETTED == rc) && 
        (TRUE == p_api_dev->reset_can_be_done) ) || 
        (API_RESET_ONLY_IF_NOT_ALREADY_RESETTED != rc) )
  {
    /* Test if BIU is responding */
    if(api_io_test_biu(p_api_dev) != API_OK)
    {
      p_api_dev->biu_active[ p_api_dev->biu ] = API_ERR;
      PRINTF0("ERROR: Test BIU\n\r");
      ret_val = API_ERR_BIU_NOT_ACTIVE;
    }
    else
    {
      p_api_dev->biu_active[ p_api_dev->biu ] = API_OK;

      /* Interrupt Loglist */
      p_api_dev->islp[ p_api_dev->biu ] = p_api_dev->glb_mem.biu[ p_api_dev->biu ].ir_log_addr;

      /* Init API System Control Block */
      api_io_mode_con(p_api_dev, API_BIU_MODE, API_OFF);
      api_io_cb_ini(p_api_dev, INIT_ALL);
      api_io_mode_con(p_api_dev, API_BIU_MODE, API_ON);

      /* Init API Interrupt Status Log List */
      api_io_islp_ini(p_api_dev);

      /* Clear Simulator Buffer */
      if( (rc & API_RESET_WITHOUT_SIMBUF) == 0 )
      {
        api_io_sim_buf_ini(p_api_dev);
      }

#ifdef CMPL4SIM
      /*** Init BC ***/
      api_io_mode_con(p_api_dev, API_BC_MODE, API_OFF);
      api_io_bc_ini(p_api_dev, API_SCB_RTRY_DIS, API_OFF, 0, 0); 

      /* Clear BC ISR Flag */
      api_io_bc_ir_clr(p_api_dev);

      /* Init BC Globals */
      p_api_dev->bc_trg_status[ p_api_dev->biu ] = API_HALT;
      p_api_dev->bc_dbc_start_status[ p_api_dev->biu ] = API_HALT;
      p_api_dev->bc_dbc_stop_status[ p_api_dev->biu ]  = API_HALT;
      p_api_dev->bc_dbc_stop_xfctrl_save[ p_api_dev->biu ] = 0;
      p_api_dev->bc_dbc_stop_xfswm_save[ p_api_dev->biu ] = 0;
      p_api_dev->bc_status[ p_api_dev->biu ] = API_HALT;
      p_api_dev->bc_traffic_override_enabled[p_api_dev->biu] = API_OFF;

      for( ul_Hid=0; ul_Hid<(MAX_API_BC_HID * MAX_MEM_MUL); ul_Hid++ )
        p_api_dev->bc_dytag_status[p_api_dev->biu][ul_Hid] = API_OFF;

      p_api_dev->bc_status_dsp[ p_api_dev->biu ].status = API_BC_STATUS_HALT;
      p_api_dev->bc_status_dsp[ p_api_dev->biu ].hxfer = 0x0000;

      p_api_dev->bc_status_cnt[ p_api_dev->biu ].msg_cnt = 0L;
      p_api_dev->bc_status_cnt[ p_api_dev->biu ].err_cnt = 0L;
      p_api_dev->bc_status_cnt[ p_api_dev->biu ].hip = 0L; 
      p_api_dev->bc_status_cnt[ p_api_dev->biu ].mfc = 0L; 
      p_api_dev->bc_status_cnt[ p_api_dev->biu ].res1 = 0L;
      p_api_dev->bc_status_cnt[ p_api_dev->biu ].res2 = 0L;

      p_api_dev->bc_setup[ p_api_dev->biu ].smod = 0;
      p_api_dev->bc_setup[ p_api_dev->biu ].cnt = 0;
      p_api_dev->bc_setup[ p_api_dev->biu ].ftime = 0L;
      p_api_dev->bc_setup[ p_api_dev->biu ].sinst = 0L; 

      /* Minor Frame Setup */
      for(k = 0; k < MAX_API_BC_FRAME; k++)
      {
        p_api_dev->bc_setup[ p_api_dev->biu ].frame[ k ].cnt = 0x0000;
        for(j = 0; j < MAX_API_BC_XFER_IN_FRAME; j++)
          p_api_dev->bc_setup[ p_api_dev->biu ].frame[ k ].instr[ j ] = 0L;
      }

      /* Major Frame Setup */
      p_api_dev->bc_setup[ p_api_dev->biu ].mframe.cnt = 0x00;
      for(j = 0; j < MAX_API_BC_FRAME_IN_MFRAME; j++)
        p_api_dev->bc_setup[ p_api_dev->biu ].mframe.fr[ j ] = 0x00;

      /* Support Firmware BC Dynamic Data */
      for(j = 0; (unsigned)j < p_api_dev->glb_mem.biu[ p_api_dev->biu ].count.bc_eq_area; j++)
      {
        p_api_dev->bc_setup[ p_api_dev->biu ].ls_dytag[ j ].ena = API_OFF;
        p_api_dev->bc_setup[ p_api_dev->biu ].ls_dytag[ j ].mode = 0;
      }

      /* System Dynamic Data */
      api_io_bc_systag_free(p_api_dev, 0/*all*/, 0/*na*/); 
      
       /* BC Tracks */
      api_sys_track_init(p_api_dev, FALSE, TRUE, FALSE);

      /*** Init FIFO Queue ***/
      p_api_dev->queue_setup[ p_api_dev->biu ].max_queue = 0;
      p_api_dev->queue_setup[ p_api_dev->biu ].max_size = 0;

      for(j = 0; j < MAX_API_QUEUE_SIZE; j++)
      {
        p_api_dev->queue_setup[ p_api_dev->biu ].queue[ j ].mode = 0;
        p_api_dev->queue_setup[ p_api_dev->biu ].queue[ j ].saddr = 0L;
        p_api_dev->queue_setup[ p_api_dev->biu ].queue[ j ].cur_buf_addr = 0L;
        p_api_dev->queue_setup[ p_api_dev->biu ].queue[ j ].half_buf_addr = 0L;
        p_api_dev->queue_setup[ p_api_dev->biu ].queue[ j ].xid = 0;
        p_api_dev->queue_setup[ p_api_dev->biu ].queue[ j ].rt = 0;
        p_api_dev->queue_setup[ p_api_dev->biu ].queue[ j ].sa = 0;
        p_api_dev->queue_setup[ p_api_dev->biu ].queue[ j ].flag = 0;
        p_api_dev->queue_setup[ p_api_dev->biu ].queue[ j ].buf_cnt = 0;
      }

      /* Service Request and Vector Word handling */

      /* free memory and re-allocate it because it might be changed with set_mem_partition */
      if(NULL != p_api_dev->bc_srvw_xid_dsp[ p_api_dev->biu ].px_Xid)
      {
        FREE(p_api_dev->bc_srvw_xid_dsp[ p_api_dev->biu ].px_Xid);
        p_api_dev->bc_srvw_xid_dsp[ p_api_dev->biu ].px_Xid = NULL;
      }

      p_api_dev->bc_srvw_xid_dsp[ p_api_dev->biu ].px_Xid = (struct ty_api_bc_srvw_dsp*) MALLOC(sizeof(struct ty_api_bc_srvw_dsp) * p_api_dev->glb_mem.biu[ p_api_dev->biu ].count.bc_xfer_desc);
      if( NULL != p_api_dev->bc_srvw_xid_dsp[ p_api_dev->biu ].px_Xid )
      {
        for(j=0; j<p_api_dev->glb_mem.biu[ p_api_dev->biu ].count.bc_xfer_desc; j++)
        {
          p_api_dev->bc_srvw_xid_dsp[ p_api_dev->biu ].px_Xid[j].lvw = 0;
          p_api_dev->bc_srvw_xid_dsp[ p_api_dev->biu ].px_Xid[j].xid = 0;
          p_api_dev->bc_srvw_xid_dsp[ p_api_dev->biu ].px_Xid[j].src = 0;
        }
      }

      p_api_dev->bc_srvw_con[ p_api_dev->biu ] = API_OFF;
      for(j = 0; j < MAX_API_BC_SRVW_RT; j++)
      {
        p_api_dev->bc_srvw_rt_dsp[ p_api_dev->biu ].rt[ j ].xid = 0x0000;
        p_api_dev->bc_srvw_rt_dsp[ p_api_dev->biu ].rt[ j ].lvw = 0x0000;
        p_api_dev->bc_srvw_rt_dsp[ p_api_dev->biu ].rt[ j ].src = 0L;
      }

      for(j = 0; j < MAX_API_BC_SRVW_ID; j++)
      {
        for(k = 0; k < MAX_API_BC_SRVW_RT; k++)
        {
          for(l = 0; l < MAX_API_BC_SRVW_SAMID; l++)
          {
            p_api_dev->bc_srvw_tab[ p_api_dev->biu ].t[ j ].ta[ k ].sa_mid[ l ].xid = 0;
          }
        }
      }

      p_api_dev->bc_srvw_subframe[ p_api_dev->biu ].cnt = 0;
      for(k = 0; k < MAX_API_BC_XFER_IN_FRAME; k++)
        p_api_dev->bc_srvw_subframe[ p_api_dev->biu ].instr[ k ] = 0L;

      /*** Init RT ***/
      api_io_mode_con(p_api_dev, API_RT_MODE, API_OFF);
      api_io_rt_ini(p_api_dev);

      /* Clear RT ISR Flags */
      for(rt = 0; rt < MAX_API_RT; rt++)
        api_io_rt_ir_clr(p_api_dev, rt);

      /* Init RT Globals */
      p_api_dev->rt_status[ p_api_dev->biu ] = API_HALT;
      p_api_dev->single_rt_addr[p_api_dev->biu] = 0;

      /* RT Dynamic Tagging */
      for(rt = 0; rt < MAX_API_RT; rt++)
      {
        for(j = 0; j < MAX_API_RT_XMT_SA; j++)
        {
          p_api_dev->rt_setup[ p_api_dev->biu ].rt_dytag[ rt ].sa[ j ] = 0;
          p_api_dev->rt_setup[ p_api_dev->biu ].rt_queue[ rt ].sa[ j ] = 0; 
        }

        for(ul_Hid = 0; ul_Hid < (MAX_API_RT_HID * MAX_MEM_MUL); ul_Hid++)
          p_api_dev->rt_dytag_status[ p_api_dev->biu ][ ul_Hid ] = API_OFF; /* disable all bits for all 32 RTs */
      }

      /* Support Firmware RT Dynamic Data */
      for(j = 0; (unsigned)j < p_api_dev->glb_mem.biu[ p_api_dev->biu ].count.rt_eq_area; j++)
      {
        p_api_dev->rt_setup[ p_api_dev->biu ].ls_dytag[ j ].ena = API_OFF;
        p_api_dev->rt_setup[ p_api_dev->biu ].ls_dytag[ j ].mode = 0;
      }

      /* System Dynamic Data */
      api_io_rt_systag_free(p_api_dev, 0/*all*/, 0/*na*/, 0/*na*/); 

      /* RT Tracks */
      api_sys_track_init(p_api_dev, FALSE, FALSE, TRUE);

      /* 1760 Protocol Functions */
      if( (rc &API_RESET_1760) == API_RESET_1760)
        api_io_c1760_ini(p_api_dev, (BYTE)p_api_dev->biu, API_ON);
       /* Enable 1760 */
      else
        api_io_c1760_ini(p_api_dev, (BYTE)p_api_dev->biu, API_OFF);
       /* Disable 1760 */

      /* LS System Dynamic Data */
      api_io_ls_systag_dis(p_api_dev, API_LS_SYSTAG_RESET_ALL, 0, 0);
#endif 

      if( (rc & API_RESET_WITHOUT_MONITOR) == 0 )
      {

#ifdef CMPL4MON
        /*** Init BM ***/
        api_io_mode_con(p_api_dev, API_BM_MODE, API_OFF);
        api_io_bm_ini(p_api_dev);
        api_io_bm_buf_ini(p_api_dev);

        /* Clear BM ISR Flag */
        api_io_bm_ir_clr(p_api_dev);

        /* Init BM Globals */
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
        api_io_bm_dyntagm_lut_free(p_api_dev,0/*all*/,0/*na*/); /* 11.11.05 */

        /* Clear Dyntag Monitor Setup Table */
        for(j=0; j<MAX_API_BM_DYNTAG_MON_ID; j++) {
           api_io_bm_dyntagm_entry_init(p_api_dev,(L_WORD)j/*tag_ix*/); /* 11.11.05 */
        }    
#endif 
       
#ifdef CMPL4SIM

        /*** Init REP ***/
        api_io_mode_con(p_api_dev, API_REP_MODE, API_OFF);
        api_io_rep_buf_ini(p_api_dev);

        /* REP specific registers of System Control Block */
        api_io_cb_ini(p_api_dev, INIT_REP);

        /* Init REP Globals */
        p_api_dev->rep_status[ p_api_dev->biu ] = API_REP_STOP;

        p_api_dev->sim_rep[ p_api_dev->biu ].mode = API_OFF;
        p_api_dev->sim_rep[ p_api_dev->biu ].rpi_cnt = 0L;
        p_api_dev->sim_rep[ p_api_dev->biu ].saddr = p_api_dev->glb_mem.biu[ p_api_dev->biu ].base.rep_buf;
        p_api_dev->sim_rep[ p_api_dev->biu ].size = 0L;
        p_api_dev->sim_rep[ p_api_dev->biu ].entry_cnt = 0L;
        p_api_dev->sim_rep[ p_api_dev->biu ].flag = API_OFF;
#endif 
        /* Init API Coupling and Amplitude register */
        api_io_cal_ini(p_api_dev);

        /* Init API MILbus Protocol */
        api_io_milprot_ini(p_api_dev);
      }

      /* System Memory Info */
#ifdef CMPL4SIM
      p_api_dev->sys_mem[ p_api_dev->biu ].sbufs = (BYTE) (p_api_dev->glb_mem.sim_buf_size[ p_api_dev->biu ] / 0x10000L);
      p_api_dev->sys_mem[ p_api_dev->biu ].saddr = p_api_dev->glb_mem.sim_buf_base_addr[ p_api_dev->biu ];
#endif 
#ifdef CMPL4MON

      p_api_dev->sys_mem[ p_api_dev->biu ].mbufs = (BYTE) (p_api_dev->glb_mem.biu[ p_api_dev->biu ].size.bm_buf / 0x10000L);
      p_api_dev->sys_mem[ p_api_dev->biu ].maddr = p_api_dev->glb_mem.biu[ p_api_dev->biu ].base.bm_buf;


#endif 
      /* Selftest */
      p_api_dev->bite_stream[ p_api_dev->biu ] = API_OFF;

#if defined(_NUCLEUS)
      /* Interrupt Loglist Events */
      p_api_dev->intr_list_p->put_cnt = 0L;
      p_api_dev->intr_list_p->get_cnt = 0L;
      for(j = 0; j < MAX_API_IR_EVENTS; j++)
      {
        p_api_dev->intr_list_p->entry[ j ].lla = 0xFFFFFFFFL;
        p_api_dev->intr_list_p->entry[ j ].llb = 0xFFFFFFFFL;
        p_api_dev->intr_list_p->entry[ j ].llc.ul_All = 0xFFFFFFFFL;
        p_api_dev->intr_list_p->entry[ j ].lld.ul_All = 0xFFFFFFFFL;
      }

      /* API Timer Interrupt Events */
      for(j = 0; j < MAX_API_TIMER; j++)
      {
        p_api_dev->timer_intr[ j ].tcnt = 0L;
        p_api_dev->timer_intr[ j ].tena = 0L;
        p_api_dev->timer_intr[ j ].predev = 0xFFFFFFFFL;
        p_api_dev->timer_intr[ j ].isr_cnt = 0L;
      }
#endif
    }
  }else{
    ret_val = API_ERR_INVALID_DEVICE_STATE;
  }

  return ret_val;
} /* end: api_sys_reset */

/*****************************************************************************/
/*                                                                           */
/*   Module : API_SYS                  Submodule : API_SYS_DEF_RESP_TOUT     */
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
/*    Inputs    : Response Timeout in 0.25us steps [tout]                    */
/*                                                                           */
/*    Outputs   : Instruction acknowledge type [ackfl]                       */
/*                                                                           */
/*    Description :                                                          */
/*    This function handles the 'API_DEF_RESP_TOUT' instruction to           */
/*    initialize the API1553-DS Response Timeout value.                      */
/*                                                                           */
/*****************************************************************************/
L_WORD api_sys_def_resp_tout( TY_API_DEV *p_api_dev, TY_MIL_COM_SYS_RESP_TOUT_DEF_INPUT * in, TY_MIL_COM_ACK * out )
{
  api_io_resp_tout_def(p_api_dev, (BYTE)in->timeout);

  return 0;
} /* end: api_sys_def_resp_tout */



/*****************************************************************************/
/*                                                                           */
/*   Module : API_SYS                  Submodule : API_SYS_LOAD_SREC         */
/*                                                                           */
/*   Author : Marco Motz               Project   : API1553-DS                */
/*                                                                           */
/*   Source : C                        Tools     : PC/AT; Norton Editor;     */
/*                                                 CYGNUS, GNU-C, As, and LD */
/*                                                 IDT-C 5.1 Toolkit         */
/*---------------------------------------------------------------------------*/
/*   Create : 01.05.98   Update : 21.09.00                                   */
/*---------------------------------------------------------------------------*/
/*    Descriptions                                                           */
/*    ------------                                                           */
/*    Inputs    : Download Mode [mode]                                       */
/*                Amount of Characters [cnt]                                 */
/*                Offset to Start Address [offset]                           */
/*                Array of Characters [data_p]                               */
/*                                                                           */
/*    Outputs   : Instruction acknowledge type [ackfl]                       */
/*                                                                           */
/*    Description :                                                          */
/*    This function handles the 'API_LOAD_SREC' instruction to download      */
/*    Characters in S-Record format to the API1553-DS target.                */
/*                                                                           */
/*****************************************************************************/

BYTE api_sys_load_srec_legacy(TY_API_DEV *p_api_dev, BYTE mode, BYTE cnt, L_WORD offset, BYTE *data_p)
{
#ifndef _AMC1553     
  BYTE ibuf[ 256 ], *line_addr, *str_ptr, Hex[ 11 ] = "\0";
  BYTE chksum, st, Bytes[ 256 ];
  L_WORD i, rec_length;
  L_WORD SRecAddr   = 0;
  L_WORD SRecType   = 0;
  L_WORD SRecTypeNr = 0;
#endif 
  BYTE ackfl, status;



  static L_WORD fsize = 0L;
#ifndef _AMC1553     
  static L_WORD addrlo = 0xffffffff;
  static L_WORD addrhi = 0;
#endif /* _AMC1553 */
  status = 0; /* Ok */

  ackfl = API_ACK2;
#ifndef _AMC1553     
  chksum = 0x00;
  rec_length = 0L;
  line_addr = 0;

  for(i = 0; i < 256; i++)
  {
    ibuf[ i ] = 0x00;
    Bytes[ i ] = 0x00;
  }

  if(mode == 1)
  {

    /* Read a SREC Line */
    for(i = 0; (i < cnt) && (i < 256); i++)
      ibuf[ i ] = *(data_p + i);

    if(ibuf[ 0 ] == 'S')
    {
      if( (ibuf[ 1 ] == '0') || (ibuf[ 1 ] == '1') || (ibuf[ 1 ] == '2') || (ibuf[ 1 ] == '3') || 
          (ibuf[ 1 ] == '7') || (ibuf[ 1 ] == '8') || (ibuf[ 1 ] == '9') )
      {

        /* Get S-Record length */
        Hex[ 0 ] = '\0';
        strncat((char *)&Hex[ 0 ], (char *)&ibuf[ 2 ], 2);
        rec_length = strtoul((char *)&Hex[ 0 ], (char **)&str_ptr, 16);
        chksum = (BYTE)rec_length;

        /* Convert all hex nibbles to Bytes */
        for(i = 0; i < rec_length; i++)
        {
          Hex[ 0 ] = '\0';
          strncat((char *)&Hex[ 0 ], (char *)&ibuf[ (i *2) + 4 ], 2);
          Bytes[ i ] = strtoul((char *)&Hex[ 0 ], (char **)&str_ptr, 16);
          chksum += Bytes[ i ];
        }

        /* analyze if correct SREC is downloaded for this board */
        SRecAddr   = (UINT32)( (Bytes[ 0 ] << 24) + (Bytes[ 1 ] << 16) + (Bytes[ 2 ] << 8) + Bytes[ 3 ]); 
        SRecType   = (UINT32)( (Bytes[ 4 ] << 24) + (Bytes[ 5 ] << 16) + (Bytes[ 6 ] << 8) + Bytes[ 7 ]); 
        SRecTypeNr = (UINT32)((Bytes[ 8 ] << 8) + Bytes[ 9 ]); 
        if( (ibuf[ 1 ] == '0') || (ibuf[ 1 ] == '3') )
        {
          if( (0 == SRecAddr) && (11 == rec_length) )
          {
            /*                         Size SRecAddr SRecType Nr   ChkSum */
            /* SREC must look like: S3 0B   00000000 504C4341 0000 D4 --> PLCA */
            /* SREC must look like: S3 0B   00000000 494C4341 0000 DB --> ILCA */
            /* SREC must look like: S3 0B   00000000 41535020 0000 F0 --> ASP  */
            /* SREC must look like: S3 0B   00000000 42495020 0000 F9 --> BIP  */
            switch(SRecType)
            {
              case 0x504C4341: /* PLCA */
                  PRINTF1("\n\r\n\rDownloading PLCA%04X...", SRecTypeNr);
                  switch(p_api_dev->BoardConfig & 0x000000FF)
                  {
                    case 0x00000090: 
                        if( SRecTypeNr != 0x0040 )
                        {
                          PRINTF0("ERROR!! SREC is not designed for an APX platform!!!\r\n");
                          status = 2;
                        }
                        else
                        {
                          PRINTF0("OK\r\n");
                          p_api_dev->uc_SRecMarkFound = 1;
                        }
                        break;
                    case 0x00000043:
                    case 0x00000021: 
                    case 0x00000023: 
                        if( SRecTypeNr != 0x0041 )
                        {
                          PRINTF0("ERROR!! SREC is not designed for an ACX-6U (no PXI) / AVX platform!!!\r\n");
                          status = 2;
                        }
                        else
                        {
                          PRINTF0("OK\r\n");
                          p_api_dev->uc_SRecMarkFound = 1;
                        }
                        break;
                    case 0x00000041:
                        if( SRecTypeNr != 0x0042 )
                        {
                          PRINTF0("ERROR!! SREC is not designed for an ACX-3U (PXI) platform!!!\r\n");
                          status = 2;
                        }
                        else
                        {
                          PRINTF0("OK\r\n");
                          p_api_dev->uc_SRecMarkFound = 1;
                        }
                        break;
                    default:
                        break;
                  }
                  break;
              case 0x494C4341: /* ILCA */
                  PRINTF1("\n\r\n\rDownloading ILCA%04X...", SRecTypeNr);
                  if( (SRecTypeNr != 0x0018) && (SRecTypeNr != 0x001C) && (SRecTypeNr != 0x0030) &&
                      (SRecTypeNr != 0x0070) )
                  {
                    PRINTF0("ERROR!! SREC is not designed for an AyX1553/3910 board!!!\r\n");
                    status = 2;
                  }
                  else
                  {
                    PRINTF0("OK\r\n");
                    p_api_dev->uc_SRecMarkFound = 1;
                  }
                  break;
              case 0x41535020: /* ASP  */
                  PRINTF1("\n\r\n\rDownloading ASP_%04X...", SRecTypeNr);
                  if( (SRecTypeNr != 0x0014) )
                  {
                    PRINTF0("ERROR!! SREC is not designed for an AyX1553/3910 board!!!\r\n");
                    status = 2;
                  }
                  else
                  {
                    PRINTF0("OK\r\n");
                    p_api_dev->uc_SRecMarkFound = 1;
                  }
                  break;
              case 0x42495020: /* BIP  */
                  PRINTF1("\n\r\n\rDownloading BIP_%04X...", SRecTypeNr);
                  if( (SRecTypeNr != 0x0010) && (SRecTypeNr != 0x0014) && (SRecTypeNr != 0x0018) &&
                      (SRecTypeNr != 0x0030) && (SRecTypeNr != 0x0070) )
                  {
                    PRINTF0("ERROR!! SREC is not designed for an AyX1553/3910 board!!!\r\n");
                    status = 2;
                  }
                  else
                  {
                    PRINTF0("OK\r\n");
                    p_api_dev->uc_SRecMarkFound = 1;
                  }
                  break;
              case 0x4D4F4E20: /* MON  */
                  PRINTF1("\n\r\n\rDownloading MON_%04X...", SRecTypeNr);
                  if( (SRecTypeNr != 0x0001) )
                  {
                    PRINTF0("ERROR!! SREC is not designed for an AyX board!!!\r\n");
                    status = 2;
                  }
                  else
                  {
                    PRINTF0("OK\r\n");
                    p_api_dev->uc_SRecMarkFound = 1;
                  }
                  break;
              case 0x54435020: /* TCP  */
                  PRINTF1("\n\r\n\rDownloading TCP_%04X...", SRecTypeNr);
                  if( (SRecTypeNr != 0x0020) )
                  {
                    PRINTF0("ERROR!! SREC is not designed for an AyX board!!!\r\n");
                    status = 2;
                  }
                  else
                  {
                    PRINTF0("OK\r\n");
                    p_api_dev->uc_SRecMarkFound = 1;
                  }
                  break;
              default:
                  break;
            }

          }
        }

        if(chksum == 255)
        {
          /* Extract Address */
          st = 0;
          switch(ibuf[ 1 ])
          {
            case '3':
            case '7':
              /* 32-bit address */
              line_addr = (BYTE *) ( (Bytes[ 0 ] << 24) + (Bytes[ 1 ] << 16) + (Bytes[ 2 ] << 8) + Bytes[ 3 ]);
              st = 5;
              break;
            case '2':
            case '8':
              /* 24-bit address */
              line_addr = (BYTE *) ( (Bytes[ 0 ] << 16) + (Bytes[ 1 ] << 8) + Bytes[ 2 ]);
              st = 4;
              break;
            case '1':
            case '9':
              /* 24-bit address */
              line_addr = (BYTE *) ( (Bytes[ 0 ] << 8) + Bytes[ 1 ]);
              st = 3;
              break;
            default:
              break;
          }

          if( (ibuf[ 1 ] == '1') || (ibuf[ 1 ] == '2') || (ibuf[ 1 ] == '3') )
          {
            /* Store SREC to Destination if legal Address Range */
            i = (L_WORD)line_addr;

            if( (UINT32)line_addr < addrlo)
              addrlo = (UINT32)line_addr;
            if( (rec_length - st + (UINT32)line_addr) > addrhi)
              addrhi = rec_length - st + (UINT32)line_addr;

            if( (i + rec_length - st) <= 0xbffffffcL /*DLOAD_END_ADDR*/)
            {
              for(i = 0; i < (rec_length - st); i++)
              {
                *(line_addr + i + offset) = Bytes[ i + st - 1 ];

                p_api_dev->SrecCheckSum = p_api_dev->SrecCheckSum + (UINT32)Bytes[ i + st - 1 ];
              }
              fsize = addrhi - addrlo;
            }
            else
              status = 4;
             /* Download Address Error */
          }
        }
        else
        {
          status = 3;
          p_api_dev->uc_SRecMarkFound = 0;
        }
         /* Checksum Error */
      }
      else
        status = 2;
       /* SRec type */
    }
    else
      status = 1;
     /* SRec not found */
  }
  else if(mode == 0)
  {
    fsize = 0L;
    addrlo = 0xffffffff;
    addrhi = 0;
    p_api_dev->SrecCheckSum = 0;
  }
  else if(mode == 2)
  {
    p_api_dev->ApiRestartAddress = offset;
    p_api_dev->ApiRestartCyclicFlag = TRUE;
  }
  else if(mode == 3)
  {
    p_api_dev->ApiRestartAddress = offset;
    p_api_dev->ApiRestartCyclicFlag = FALSE;
  }
  else
  {
    ackfl = API_NAK;
    SET_NAK_INFO(API_ERR_INVALID_MODE);
  }

  if( status != 0 )
    p_api_dev->uc_SRecMarkFound = 0;

#endif 
  if(ackfl == API_ACK2)
  {
    p_api_dev->byte_data_arr[ 0 ] = status;
    p_api_dev->lword_data_arr[ 0 ] = fsize;

    p_api_dev->b_length = 1;
    p_api_dev->w_length = 0;
    p_api_dev->lw_length = 1;
    p_api_dev->ack_b_ptr = &(p_api_dev->byte_data_arr[ 0 ]);
    p_api_dev->ack_lw_ptr = &(p_api_dev->lword_data_arr[ 0 ]);
  }
  return (ackfl);

} /* end: api_sys_load_srec */

/*****************************************************************************/
/*                                                                           */
/*   Module : API_SYS                  Submodule : API_SYS_PROG_FLASH        */
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
/*    Inputs    : Source RAM Address [src_addr]                              */
/*                Flash Prom Sector [sect]                                   */
/*                Number of bytes to be programmed [size]                    */
/*                                                                           */
/*    Outputs   : Instruction acknowledge type [ackfl]                       */
/*                                                                           */
/*    Description :                                                          */
/*    This function handles the 'API_PROG_FLASH' instruction to program      */
/*    the API1553-DS Flash Proms in-circuit.                                 */
/*                                                                           */
/*****************************************************************************/

BYTE api_sys_prog_flash_legacy(TY_API_DEV *p_api_dev, L_WORD src_addr, L_WORD sect, L_WORD size)
{
  BYTE ackfl;
  L_WORD st = 0;

#ifndef _AMC1553

  /* if we are a PXI board, only do an update, if a correct SREC mark was found */
  if( (p_api_dev->BoardConfig & 0x000000FF) == 0x00000041 )
  {
    if( 0 == p_api_dev->uc_SRecMarkFound )
    {
      PRINTF0("Flash will not be programmed because there was no correct SREC mark identified for this board\r\n");
      st = 2;
    }
  }
  p_api_dev->uc_SRecMarkFound = 0; /* reset flag */

  /* Function Protos from module INI_PROG */

  if( 0 == st )
  {
    #ifndef _NUCLEUS    

    st = program_flash(src_addr, sect, size);
    if(st != 0L)
      st = 2L;

    #else 

    L_WORD address;

    /* sector 0xff is a special case */
    switch( sect ){
    case 0xfe:
      st = TCP_Spi_Flash_Prog( src_addr, size );
      PRINTF2("\r\nProgramming TCP from 0x%x. Returns %i\r\n", src_addr, st);
      break;
    case 0xff:
      st = ASL_PROMProg( src_addr );
      PRINTF2("\r\nProgramming ASL from 0x%x. Returns %i\r\n", src_addr, st);
      break;

	default:
      if( 0 == sect ) /* in case of sector 0, unprotect the sector */
      {
        address = FLASH_BP;

        MEM16(address) = (UINT16)0x6060;
        MEM16(address) = (UINT16)0xD0D0; 

        /* Wait until sector is unprotected (take appr. 1.4sec) */
        wait_clocks(500000 * clk_1us);
        wait_clocks(500000 * clk_1us);
        wait_clocks(500000 * clk_1us);
      }
      st = Flash_Block_Program_16(src_addr, sect, size);
      if(st != 0L)
        st = 2L;
      if( 0 == sect ) /* in case of sector 0, protect the sector */
      {
        address = FLASH_BP + (FLASH_SEC_S * 0);  /* Get first Block-Address */
        MEM16(address) = (UINT16)0x6060;
        MEM16(address) = (UINT16)0x0101;  
        wait_clocks(500000 * clk_1us);

        address = FLASH_BP + (FLASH_SEC_S * 1);  /* Get second Block-Address */
        MEM16(address) = (UINT16)0x6060;
        MEM16(address) = (UINT16)0x0101;  
        wait_clocks(500000 * clk_1us);
      }
	}

    #endif /* _NUCLEUS */
  }

#endif 

  p_api_dev->byte_data_arr[ 0 ] = (BYTE)st;

  ackfl = API_ACK2;

  p_api_dev->b_length = 1;
  p_api_dev->w_length = 0;
  p_api_dev->lw_length = 0;
  p_api_dev->ack_b_ptr = &(p_api_dev->byte_data_arr[ 0 ]);

  return (ackfl);
} /* end: api_sys_prog_flash */









/*****************************************************************************/
/*                                                                           */
/*   Module : API_SYS                  Submodule : API_SYS_DEF_MILBUS_PROT   */
/*                                                                           */
/*   Author : Michael Stahl            Project   : API1553-DS                */
/*                                                                           */
/*   Source : C                        Tools     : PC/AT; Norton Editor;     */
/*                                                 CYGNUS, GNU-C, As, and LD */
/*                                                 IDT-C 5.1 Toolkit         */
/*---------------------------------------------------------------------------*/
/*   Create : 01.02.99   Update : 01.02.99                                   */
/*---------------------------------------------------------------------------*/
/*    Descriptions                                                           */
/*    ------------                                                           */
/*    Inputs    : Milbus protocol type [mil_prot]                            */
/*                RT set mode [mode]                                         */
/*                RT [rt]                                                    */
/*                                                                           */
/*    Outputs   : Instruction acknowledge type [ackfl]                       */
/*                                                                           */
/*    Description :                                                          */
/*    This function handles the 'API_DEF_MILBUS_PROT' instruction to set the */
/*    Milbus Protocol register.                                              */
/*                                                                           */
/*****************************************************************************/

L_WORD api_sys_def_milbus_prot( TY_API_DEV *p_api_dev, TY_MIL_COM_SYS_MILBUS_PROT_INPUT * in, TY_MIL_COM_ACK * out )
{
    if ((BYTE)in->prot == API_MILBUS_PROT_A)
    {
        if (!mil_fw_is_cmd_supported(p_api_dev, MIL_FW_CMD_PROT_1553A))
            return API_ERR_CMD_NOT_SUPPORTED_BY_FW;

        if (API_DEVICE_MODE_IS_EMBEDDED(p_api_dev->drv_mode[p_api_dev->biu]))
            return API_ERR_CMD_NOT_SUPPORTED_BY_FW;
    }

    api_io_milbus_prot_def(p_api_dev, (BYTE)in->prot, (BYTE)in->mode, (BYTE)in->rt);

    return 0;
} /* end: api_sys_def_milbus_prot */


/*****************************************************************************/
/*                                                                           */
/*   Module : API_SYS                  Submodule : API_SYS_LS_SYSTAG_DEF     */
/*                                                                           */
/*   Author : Marco Motz               Project   : API1553-DS                */
/*                                                                           */
/*   Source : C                        Tools     : PC/AT; Norton Editor;     */
/*                                                 CYGNUS, GNU-C, As, and LD */
/*                                                 IDT-C 5.1 Toolkit         */
/*---------------------------------------------------------------------------*/
/*   Create : 25.05.00   Update : 15.05.07                                   */
/*---------------------------------------------------------------------------*/
/*    Descriptions                                                           */
/*    ------------                                                           */
/*    Inputs    : System Dynamic Data Identifier [id]                        */
/*                System Dynamic Data control (enable/disable) [con]         */
/*                System Dynamic Data mode (BC/RT) [bcrt_mode]               */
/*                BC Transfer ID / RT-Subaddress [xid_rtsa]                  */
/*                System Dyanmic Data function [fct]                         */
/*                Lower Limit of System Dynamic Data word [min]              */
/*                Upper Limit of System Dynamic Data word [max]              */
/*                Step size of System Dynamic Data word [step]               */
/*                Word position of System Dynamic Data word [wpos]           */
/*                Bit position and length System Dynamic Data [bpos_nbit]    */
/*                                                                           */
/*    Outputs   : Instruction acknowledge type [ackfl]                       */
/*                                                                           */
/*    Description :                                                          */
/*    This function handles the 'API_LS_SYSTAG_DEF' instruction used to      */
/*    define System Dynamic Data words.                                      */
/*                                                                           */
/*****************************************************************************/
L_WORD api_sys_ls_systag_def( TY_API_DEV *p_api_dev, TY_MIL_COM_SYS_SYSTAG_DEF_INPUT * in, TY_MIL_COM_ACK * out )
{
  BYTE tag_ix; 
  WORD mask, bpos, nbit;
  L_WORD ret_val = 0;

  BYTE id        = (BYTE )in->id;
  BYTE con       = (BYTE )in->con;
  BYTE bcrt_mode = (BYTE )in->mode;
  WORD xid_rtsa  = (WORD )in->xid_rtsa;
  WORD fct       = (WORD )in->fct;
  WORD min       = (WORD )in->min;
  WORD max       = (WORD )in->max;
  WORD step      = (WORD )in->step;
  WORD wpos      = (WORD )in->wpos;
  WORD bpos_nbit = (WORD )in->bpos_nbit;

  if( LS_SYSTAG_ID_IN_RANGE( id ) )
  {
    tag_ix = id - 1;
    switch(con & 0x01)
    {
      case API_SYSTAG_DIS:
        api_io_ls_systag_entry_init(p_api_dev, tag_ix);
        if(bcrt_mode == API_BC_MODE)
          api_io_bc_systag_con(p_api_dev, tag_ix, (WORD) (xid_rtsa & MAX_API_BC_XFER_MASK) );
        else if(bcrt_mode == API_RT_MODE)
          api_io_rt_sa_systag_con(p_api_dev, tag_ix, (BYTE) ( (xid_rtsa >> 8) & 0x1F), (BYTE) (xid_rtsa & 0x1F) );
        else
        {
          ret_val = API_ERR_INVALID_MODE;
        }
        break;

      case API_SYSTAG_ENA:
        if(p_api_dev->ls_systag_setup[ p_api_dev->biu ].dytag[ tag_ix ].mode == API_SYSTAG_DIS)
        {
          p_api_dev->ls_systag_setup[ p_api_dev->biu ].dytag[ tag_ix ].bc_rt = bcrt_mode;
          p_api_dev->ls_systag_setup[ p_api_dev->biu ].dytag[ tag_ix ].xid_rtsa = xid_rtsa;
          p_api_dev->ls_systag_setup[ p_api_dev->biu ].dytag[ tag_ix ].fct = fct;

          /* Set init flag */
          if( (con & 0x02) == 0x02)
            p_api_dev->ls_systag_setup[ p_api_dev->biu ].dytag[ tag_ix ].init = API_ON;

          switch(fct)
          {
          case API_SYSTAG_RAMP_INC:
          case API_SYSTAG_RAMP_DEC:
          case API_SYSTAG_TRIANG_INC:
          case API_SYSTAG_TRIANG_DEC:
            bpos = ( (bpos_nbit >> 8) & 0x000F);
            nbit = bpos_nbit & 0x00FF;
            if(nbit == 0)
              nbit = 16;
            mask = 0xFFFF;
            if(nbit < 16)
              mask = ~(0xFFFF << nbit);

            p_api_dev->ls_systag_setup[ p_api_dev->biu ].dytag[ tag_ix ].min = min & mask;
            p_api_dev->ls_systag_setup[ p_api_dev->biu ].dytag[ tag_ix ].max = max & mask;
            p_api_dev->ls_systag_setup[ p_api_dev->biu ].dytag[ tag_ix ].step = step & mask;
            p_api_dev->ls_systag_setup[ p_api_dev->biu ].dytag[ tag_ix ].wpos = wpos;
            p_api_dev->ls_systag_setup[ p_api_dev->biu ].dytag[ tag_ix ].bpos = bpos;
            p_api_dev->ls_systag_setup[ p_api_dev->biu ].dytag[ tag_ix ].nbit = nbit;
            p_api_dev->ls_systag_setup[ p_api_dev->biu ].dytag[ tag_ix ].mask = mask << bpos;
            break;

          case API_SYSTAG_DATASET:
            p_api_dev->ls_systag_setup[ p_api_dev->biu ].dytag[ tag_ix ].min = min;
            p_api_dev->ls_systag_setup[ p_api_dev->biu ].dytag[ tag_ix ].max = max;
            p_api_dev->ls_systag_setup[ p_api_dev->biu ].dytag[ tag_ix ].step = step & 0x01;
            p_api_dev->ls_systag_setup[ p_api_dev->biu ].dytag[ tag_ix ].wpos = 0;
            p_api_dev->ls_systag_setup[ p_api_dev->biu ].dytag[ tag_ix ].bpos = 0;
            p_api_dev->ls_systag_setup[ p_api_dev->biu ].dytag[ tag_ix ].nbit = 0;
            p_api_dev->ls_systag_setup[ p_api_dev->biu ].dytag[ tag_ix ].mask = 0;
            if(max == 0)
              p_api_dev->ls_systag_setup[ p_api_dev->biu ].dytag[ tag_ix ].max = 1;
            break;

          case API_SYSTAG_STATES:
            bpos = ( (bpos_nbit >> 8) & 0x000F);
            nbit = bpos_nbit & 0x00FF;
            if(nbit == 0)
              nbit = 16;
            mask = 0xFFFF;
            if(nbit < 16)
              mask = ~(0xFFFF << nbit);

            p_api_dev->ls_systag_setup[ p_api_dev->biu ].dytag[ tag_ix ].min = min;
            p_api_dev->ls_systag_setup[ p_api_dev->biu ].dytag[ tag_ix ].max = max;
            p_api_dev->ls_systag_setup[ p_api_dev->biu ].dytag[ tag_ix ].step = step & 0x01;
            p_api_dev->ls_systag_setup[ p_api_dev->biu ].dytag[ tag_ix ].wpos = wpos;
            p_api_dev->ls_systag_setup[ p_api_dev->biu ].dytag[ tag_ix ].bpos = 0;
            p_api_dev->ls_systag_setup[ p_api_dev->biu ].dytag[ tag_ix ].index = 0;
            p_api_dev->ls_systag_setup[ p_api_dev->biu ].dytag[ tag_ix ].nbit = 0;
            p_api_dev->ls_systag_setup[ p_api_dev->biu ].dytag[ tag_ix ].mask = mask << bpos;
            if(max == 0)
              p_api_dev->ls_systag_setup[ p_api_dev->biu ].dytag[ tag_ix ].max = 1;
            break;

          case API_SYSTAG_COMP:
            p_api_dev->ls_systag_setup[ p_api_dev->biu ].dytag[ tag_ix ].min = min & 0x1F;
            p_api_dev->ls_systag_setup[ p_api_dev->biu ].dytag[ tag_ix ].max = 0;
            p_api_dev->ls_systag_setup[ p_api_dev->biu ].dytag[ tag_ix ].step = step & 0x03;
            p_api_dev->ls_systag_setup[ p_api_dev->biu ].dytag[ tag_ix ].wpos = wpos;
            p_api_dev->ls_systag_setup[ p_api_dev->biu ].dytag[ tag_ix ].bpos = 0;
            p_api_dev->ls_systag_setup[ p_api_dev->biu ].dytag[ tag_ix ].nbit = 0;
            p_api_dev->ls_systag_setup[ p_api_dev->biu ].dytag[ tag_ix ].mask = 0;
            break;

          case API_SYSTAG_CHECKSUM: 
            p_api_dev->ls_systag_setup[ p_api_dev->biu ].dytag[ tag_ix ].step = step & 0x03; 
            if (step == 2 /* Checksum mode = 1760 */) {
              p_api_dev->ls_systag_setup[ p_api_dev->biu ].dytag[ tag_ix ].min = 0;
              p_api_dev->ls_systag_setup[ p_api_dev->biu ].dytag[ tag_ix ].max = wpos;
            }
            else { /* Checksum mode = Plus (0) and Xor (1) */
              p_api_dev->ls_systag_setup[ p_api_dev->biu ].dytag[ tag_ix ].min = min & 0x1F;
              p_api_dev->ls_systag_setup[ p_api_dev->biu ].dytag[ tag_ix ].max = max & 0x1F;
            }
            p_api_dev->ls_systag_setup[ p_api_dev->biu ].dytag[ tag_ix ].wpos = wpos;
            p_api_dev->ls_systag_setup[ p_api_dev->biu ].dytag[ tag_ix ].bpos = 0;
            p_api_dev->ls_systag_setup[ p_api_dev->biu ].dytag[ tag_ix ].nbit = 0;
            p_api_dev->ls_systag_setup[ p_api_dev->biu ].dytag[ tag_ix ].mask = 0;
            break;

          default:
            break;
          }

          p_api_dev->ls_systag_setup[ p_api_dev->biu ].dytag[ tag_ix ].mode = API_SYSTAG_ENA;
          p_api_dev->ls_systag_setup[ p_api_dev->biu ].dytag[ tag_ix ].run_mode = API_SYSTAG_RUN_RESUME;

          if(bcrt_mode == API_BC_MODE)
            api_io_bc_systag_con(p_api_dev, tag_ix, (WORD) (xid_rtsa & MAX_API_BC_XFER_MASK));
          else if(bcrt_mode == API_RT_MODE)
            api_io_rt_sa_systag_con(p_api_dev, tag_ix, (BYTE) ( (xid_rtsa >> 8) & 0x1F), (BYTE) (xid_rtsa & 0x1F) );
          else
          {
            ret_val = API_ERR_INVALID_MODE;
          }
        }
        break;

      default:
        ret_val = API_ERR_INVALID_MODE;
        break;
    }

  }
  else
  {
    ret_val = API_ERR_PARAMETER_OUT_OF_RANGE;
  }

  return ret_val;

} /* end: api_sys_ls_systag_def */

/*****************************************************************************/
/*                                                                           */
/*   Module : API_SYS                  Submodule : API_SYS_LS_SYSTAG_CON     */
/*                                                                           */
/*   Author : Marco Motz               Project   : API1553-DS                */
/*                                                                           */
/*   Source : C                        Tools     : PC/AT; Norton Editor;     */
/*                                                 CYGNUS, GNU-C, As, and LD */
/*                                                 IDT-C 5.1 Toolkit         */
/*---------------------------------------------------------------------------*/
/*   Create : 25.05.00   Update : 25.05.00                                   */
/*---------------------------------------------------------------------------*/
/*    Descriptions                                                           */
/*    ------------                                                           */
/*    Inputs    : System Dynamic Data Identifier [id]                        */
/*                System Dynamic Data control (suspend/resume) [con]         */
/*                                                                           */
/*    Outputs   : Instruction acknowledge type [ackfl]                       */
/*                                                                           */
/*    Description :                                                          */
/*    This function handles the 'API_LS_SYSTAG_CON' instruction used to      */
/*    define System Dynamic Data words.                                      */
/*                                                                           */
/*****************************************************************************/
L_WORD api_sys_ls_systag_con( TY_API_DEV *p_api_dev, TY_MIL_COM_SYS_SYSTAG_CON_INPUT   * in, TY_MIL_COM_ACK * out )
{
  BYTE tag_ix;
  L_WORD xid_rtsa;
  L_WORD bc_rt;
  L_WORD index;
  L_WORD desc_addr;
  L_WORD rt, sa;
  L_WORD dbp = 0;
  L_WORD ret_val = 0;
  BYTE id    = (BYTE)in->id;
  BYTE con   = (BYTE)in->con;


  if( LS_SYSTAG_ID_IN_RANGE( id ) )
  {
    tag_ix = id - 1;
    switch(con)
    {
      case API_SYSTAG_RUN_DISABLED:
      case API_SYSTAG_RUN_SUSPEND:
        p_api_dev->ls_systag_setup[ p_api_dev->biu ].dytag[ tag_ix ].run_mode = API_SYSTAG_RUN_SUSPEND;
        break;

      case API_SYSTAG_RUN_ENABLED:
      case API_SYSTAG_RUN_RESUME:
        p_api_dev->ls_systag_setup[ p_api_dev->biu ].dytag[ tag_ix ].run_mode = API_SYSTAG_RUN_RESUME;
        break;

      case API_SYSTAG_RECALC_CHECKSUM:
        xid_rtsa = p_api_dev->ls_systag_setup[ p_api_dev->biu ].dytag[ tag_ix ].xid_rtsa;
        bc_rt    = p_api_dev->ls_systag_setup[ p_api_dev->biu ].dytag[ tag_ix ].bc_rt;

        if( API_BC_MODE == bc_rt )
        {
          if(xid_rtsa < p_api_dev->glb_mem.biu[ p_api_dev->biu ].count.bc_xfer_desc)
          {
            /* Get Buffer Address from Buffer Header */
            desc_addr = ul_ConvertGRAMPtrToTSWPtr(p_api_dev, p_api_dev->biu, (*p_api_dev->bc_xfer_desc_p[ p_api_dev->biu ])[ xid_rtsa ].bhd_ptr & BSWAP32(0x03fffffful) );
            desc_addr -= p_api_dev->glb_mem.biu[ p_api_dev->biu ].base.bc_bh_area;
            index = desc_addr / MAX_API_BC_BH_DESC_SIZE;
            dbp = BSWAP32( (*p_api_dev->bc_bh_desc_p[ p_api_dev->biu ])[ index ].bp_addr & BSWAP32(0x03fffffful) );
          }
        }
        else
        {
          rt = (xid_rtsa & 0x0000FF00) >> 8;
          sa = xid_rtsa & 0x0000001F;
          index = sa + (MAX_API_RT_MC_SA * API_RT_SA_TYPE_XMT);

          /* Get Buffer Address from Buffer Header */
          desc_addr = ul_ConvertGRAMPtrToTSWPtr(p_api_dev, p_api_dev->biu, (*p_api_dev->rt_sa_desc_p[ p_api_dev->biu ])[ rt ].sa_mc[ index ].bhd_ptr & 0x03ffffff);
          desc_addr -= p_api_dev->glb_mem.biu[ p_api_dev->biu ].base.rt_bh_area;
          index = desc_addr / MAX_API_RT_BH_DESC_SIZE;
          dbp = BSWAP32( (*p_api_dev->rt_bh_desc_p[ p_api_dev->biu ])[ index ].bp_addr & 0x03ffffff);
        }
        api_ir_calculate_checksum( p_api_dev, (short)(p_api_dev->biu), tag_ix, dbp );
        break;
      default:
        ret_val = API_ERR_INVALID_MODE;
        break;
    }
  }
  else
  {
    ret_val = API_ERR_PARAMETER_OUT_OF_RANGE;
  }

  return ret_val;

} /* end: api_sys_ls_systag_con */


/*****************************************************************************/
/*                                                                           */
/*   Module : API_SYS                  Submodule : q */
/*                                                                           */
/*   Author : Marco Motz               Project   : API1553-DS                */
/*                                                                           */
/*   Source : C                        Tools     : PC/AT; Norton Editor;     */
/*                                                 CYGNUS, GNU-C, As, and LD */
/*                                                 IDT-C 5.1 Toolkit         */
/*---------------------------------------------------------------------------*/
/*   Create : 28.08.02   Update : 28.08.02                                   */
/*---------------------------------------------------------------------------*/
/*    Descriptions                                                           */
/*    ------------                                                           */
/*    Inputs    :                                                            */
/*                                                                           */
/*    Outputs   :                                                            */
/*                                                                           */
/*    Description :                                                          */
/*                                                                           */
/*****************************************************************************/
L_WORD api_sys_get_mem_part_cmd( TY_API_DEV *p_api_dev, TY_MIL_COM_SYS_MEM_PART_GET_INPUT * in, TY_MIL_COM_SYS_MEM_PART_GET_OUTPUT * out )
{
    L_WORD ret_val = 0;

    /* -- set ls part --- */
    ret_val = api_sys_get_mem_partition(p_api_dev, in->mode, &out->mem_info );

    if( ret_val != 0 )
        return ret_val;

    /* -- check if HS stream is on first PBI --- */
    p_api_dev->biu       = BIU_2-1;
    p_api_dev->hs_stream = 0;

    if( ul_StreamIsHS( p_api_dev, p_api_dev->biu ) )
        ret_val = api39_sys_get_mem_partition(p_api_dev, in->mode, &out->mem_info);

    if( ret_val != 0 )
        return ret_val;

    /* -- check if HS stream is on second PBI --- */
    p_api_dev->biu       = p_api_dev->ul_NbOfStreams1 + BIU_2-1;
    p_api_dev->hs_stream = 1;
    
    if( ul_StreamIsHS( p_api_dev, p_api_dev->biu ) )
        ret_val = api39_sys_get_mem_partition(p_api_dev, in->mode, &out->mem_info);

    return ret_val;
}


L_WORD api_sys_get_mem_partition( TY_API_DEV *p_api_dev, L_WORD mode, TY_API_GET_MEM_INFO * mem_info )
{
  L_WORD ul_ActualStreamNb;
  BYTE   b_FirstBiuOfHSStream;
  BYTE   biu;

  for( biu=0; biu<BIU_8; biu++ )
  {

    /* -- address values -- */
    mem_info->ax_BiuAddr[biu].ul_Cb          = p_api_dev->glb_mem.biu[ biu ].cb_addr;
    mem_info->ax_BiuAddr[biu].ul_IrLog       = p_api_dev->glb_mem.biu[ biu ].ir_log_addr;
    mem_info->ax_BiuAddr[biu].ul_RtDesc      = p_api_dev->glb_mem.biu[ biu ].rt_desc_addr;
    mem_info->ax_BiuAddr[biu].ul_BmTcb       = p_api_dev->glb_mem.biu[ biu ].bm_tcb_addr;
    mem_info->ax_BiuAddr[biu].ul_BmAct       = p_api_dev->glb_mem.biu[ biu ].bm_act_addr;
    mem_info->ax_BiuAddr[biu].ul_RtSaDesc    = p_api_dev->glb_mem.biu[ biu ].base.rt_sa_desc;
    mem_info->ax_BiuAddr[biu].ul_RtBhArea    = p_api_dev->glb_mem.biu[ biu ].base.rt_bh_area;
    mem_info->ax_BiuAddr[biu].ul_RtSqArea    = p_api_dev->glb_mem.biu[ biu ].base.rt_sq_area;
    mem_info->ax_BiuAddr[biu].ul_RtEqArea    = p_api_dev->glb_mem.biu[ biu ].base.rt_eq_area;
    mem_info->ax_BiuAddr[biu].ul_BcBhArea    = p_api_dev->glb_mem.biu[ biu ].base.bc_bh_area;
    mem_info->ax_BiuAddr[biu].ul_BcSqArea    = p_api_dev->glb_mem.biu[ biu ].base.bc_sq_area;
    mem_info->ax_BiuAddr[biu].ul_BcEqArea    = p_api_dev->glb_mem.biu[ biu ].base.bc_eq_area;
    mem_info->ax_BiuAddr[biu].ul_BcXferDesc  = p_api_dev->glb_mem.biu[ biu ].base.bc_xfer_desc;
    mem_info->ax_BiuAddr[biu].ul_BcHipInstr  = p_api_dev->glb_mem.biu[ biu ].base.bc_hip_instr;
    mem_info->ax_BiuAddr[biu].ul_BcLipInstr  = p_api_dev->glb_mem.biu[ biu ].base.bc_lip_instr;
    mem_info->ax_BiuAddr[biu].ul_BcAcycInstr = p_api_dev->glb_mem.biu[ biu ].base.bc_acyc_instr;
    mem_info->ax_BiuAddr[biu].ul_RepBuf      = p_api_dev->glb_mem.biu[ biu ].base.rep_buf;
    mem_info->ax_BiuAddr[biu].ul_BmBuf       = p_api_dev->glb_mem.biu[ biu ].base.bm_buf;


    /* -- size values -- */
    mem_info->ax_BiuSize[biu].ul_BcHipInstr  = p_api_dev->glb_mem.biu[ biu ].size.bc_hip_instr;
    mem_info->ax_BiuSize[biu].ul_BcLipInstr  = p_api_dev->glb_mem.biu[ biu ].size.bc_lip_instr;
    mem_info->ax_BiuSize[biu].ul_BcAcycInstr = p_api_dev->glb_mem.biu[ biu ].size.bc_acyc_instr;
    mem_info->ax_BiuSize[biu].ul_RepBuf      = p_api_dev->glb_mem.biu[ biu ].size.rep_buf;
    mem_info->ax_BiuSize[biu].ul_BmBuf       = p_api_dev->glb_mem.biu[ biu ].size.bm_buf;

    /* -- count values -- */
    mem_info->ax_BiuCnt[biu].ul_RtBhArea   = p_api_dev->glb_mem.biu[ biu ].count.rt_bh_area;
    mem_info->ax_BiuCnt[biu].ul_RtSqArea   = p_api_dev->glb_mem.biu[ biu ].count.rt_sq_area;
    mem_info->ax_BiuCnt[biu].ul_RtEqArea   = p_api_dev->glb_mem.biu[ biu ].count.rt_eq_area;
    mem_info->ax_BiuCnt[biu].ul_BcBhArea   = p_api_dev->glb_mem.biu[ biu ].count.bc_bh_area;
    mem_info->ax_BiuCnt[biu].ul_BcSqArea   = p_api_dev->glb_mem.biu[ biu ].count.bc_sq_area;
    mem_info->ax_BiuCnt[biu].ul_BcEqArea   = p_api_dev->glb_mem.biu[ biu ].count.bc_eq_area;
    mem_info->ax_BiuCnt[biu].ul_BcXferDesc = p_api_dev->glb_mem.biu[ biu ].count.bc_xfer_desc;
    
  }

  /* -- Simulator buffer values for memory bar 1 --- */
  mem_info->x_Sim[0].ul_BufBaseAddr   = p_api_dev->glb_mem.sim_buf_base_addr[ BIU_1 - 1 ];
  mem_info->x_Sim[0].ul_BufSize       = p_api_dev->glb_mem.sim_buf_size[      BIU_1 - 1 ];
  mem_info->x_Sim[0].ul_BufCount      = p_api_dev->glb_mem.sim_buf_count[     BIU_1 - 1 ];
  mem_info->x_Sim[0].ul_HsBufBaseAddr = 0; /* HS -> see i39_sys.c */
  mem_info->x_Sim[0].ul_HsBufSize     = 0; /* HS -> see i39_sys.c */
  mem_info->x_Sim[0].ul_HsRes         = 0; /* HS -> see i39_sys.c */

  /* -- Simulator buffer values for memory bar 2 --- */
  mem_info->x_Sim[1].ul_BufBaseAddr   = p_api_dev->glb_mem.sim_buf_base_addr[ p_api_dev->ul_NbOfStreams1 + BIU_1 - 1 ];
  mem_info->x_Sim[1].ul_BufSize       = p_api_dev->glb_mem.sim_buf_size[      p_api_dev->ul_NbOfStreams1 + BIU_1 - 1 ];
  mem_info->x_Sim[1].ul_BufCount      = p_api_dev->glb_mem.sim_buf_count[     p_api_dev->ul_NbOfStreams1 + BIU_1 - 1 ];
  mem_info->x_Sim[1].ul_HsBufBaseAddr = 0; /* HS -> see i39_sys.c */
  mem_info->x_Sim[1].ul_HsBufSize     = 0; /* HS -> see i39_sys.c */
  mem_info->x_Sim[1].ul_HsRes         = 0; /* HS -> see i39_sys.c */

  /* -- Memory bar sizes bar 1 / 2 --- */
  mem_info->aul_GlobalMemSize[0]       =  p_api_dev->GlobalRAMSize  / 1024 / 1024;
  mem_info->aul_GlobalMemSize[1]       =  p_api_dev->GlobalRAMSize2 / 1024 / 1024;


  ul_ActualStreamNb = 1;
  b_FirstBiuOfHSStream = 1;
  for( biu=BIU_1-1; biu<BIU_8; biu++ )
  {
    if(biu<(p_api_dev->ul_NbOfStreams1+p_api_dev->ul_NbOfStreams2))
    {
      if(ul_StreamIs3910(p_api_dev,biu))
      {
        if( b_FirstBiuOfHSStream )
        {
          b_FirstBiuOfHSStream = 0;
          mem_info->x_BiuInfo[biu].ul_Protocol = 1;                 /* 1553 protocol*/
          mem_info->x_BiuInfo[biu].ul_StreamNb = ul_ActualStreamNb; /* stream number */
        }
        else
        {
          b_FirstBiuOfHSStream = 1;
          mem_info->x_BiuInfo[biu].ul_Protocol = 2;                 /* 3910 protocol*/
          mem_info->x_BiuInfo[biu].ul_StreamNb = ul_ActualStreamNb; /* stream number */
          ul_ActualStreamNb++;
        }
      }
      else if(ul_StreamIsEFEX(p_api_dev,biu))
      {
        if( b_FirstBiuOfHSStream )
        {
          b_FirstBiuOfHSStream = 0;
          mem_info->x_BiuInfo[biu].ul_Protocol = 1;                 /* 1553 protocol*/
          mem_info->x_BiuInfo[biu].ul_StreamNb = ul_ActualStreamNb; /* stream number */
        }
        else
        {
          b_FirstBiuOfHSStream = 1;
          mem_info->x_BiuInfo[biu].ul_Protocol = 3;                 /* EFEX protocol*/
          mem_info->x_BiuInfo[biu].ul_StreamNb = ul_ActualStreamNb; /* stream number */
          ul_ActualStreamNb++;
        }
      }
      else
      {
        mem_info->x_BiuInfo[biu].ul_Protocol = 1;                   /* 1553 protocol*/
        mem_info->x_BiuInfo[biu].ul_StreamNb = ul_ActualStreamNb;   /* stream number */
        ul_ActualStreamNb++;
      }

      if(biu < p_api_dev->ul_NbOfStreams1)
        mem_info->x_BiuInfo[biu].ul_MemoryBank = 0; /* memory bank 1 */
      else
        mem_info->x_BiuInfo[biu].ul_MemoryBank = 1; /* memory bank 2 */
    }
    else
    {
        /* Biu not used */
        mem_info->x_BiuInfo[biu].ul_Protocol   = 0;
        mem_info->x_BiuInfo[biu].ul_StreamNb   = 0;
        mem_info->x_BiuInfo[biu].ul_MemoryBank = 0;
    }
  }

  if(mode == 1)
  {
    enter_drv_dump_mem_layout(p_api_dev);
  }

  return 0;

} /* end: api_sys_get_mem_partition */

/*****************************************************************************/
/*                                                                           */
/*   Module : API_SYS                  Submodule : q */
/*                                                                           */
/*   Author : Marco Motz               Project   : API1553-DS                */
/*                                                                           */
/*   Source : C                        Tools     : PC/AT; Norton Editor;     */
/*                                                 CYGNUS, GNU-C, As, and LD */
/*                                                 IDT-C 5.1 Toolkit         */
/*---------------------------------------------------------------------------*/
/*   Create : 28.08.02   Update : 28.08.02                                   */
/*---------------------------------------------------------------------------*/
/*    Descriptions                                                           */
/*    ------------                                                           */
/*    Inputs    :                                                            */
/*                                                                           */
/*    Outputs   :                                                            */
/*                                                                           */
/*    Description :                                                          */
/*                                                                           */
/*****************************************************************************/

BYTE api_sys_get_mem_partition_legacy(TY_API_DEV *p_api_dev, BYTE mode)
{
  BYTE ackfl;
  L_WORD ul_Index = 0;
  short ul_BiuIndex;
  L_WORD ul_ActualStreamNb;
  BYTE b_FirstBiuOfHSStream;

  for( ul_BiuIndex=0; ul_BiuIndex<BIU_2; ul_BiuIndex++ )
  {
    p_api_dev->lword_data_arr[ ul_Index++ ] = p_api_dev->glb_mem.biu[ ul_BiuIndex ].cb_addr;
    p_api_dev->lword_data_arr[ ul_Index++ ] = p_api_dev->glb_mem.biu[ ul_BiuIndex ].ir_log_addr;
    p_api_dev->lword_data_arr[ ul_Index++ ] = p_api_dev->glb_mem.biu[ ul_BiuIndex ].rt_desc_addr;
    p_api_dev->lword_data_arr[ ul_Index++ ] = p_api_dev->glb_mem.biu[ ul_BiuIndex ].bm_tcb_addr;
    p_api_dev->lword_data_arr[ ul_Index++ ] = p_api_dev->glb_mem.biu[ ul_BiuIndex ].bm_act_addr;
    p_api_dev->lword_data_arr[ ul_Index++ ] = p_api_dev->glb_mem.biu[ ul_BiuIndex ].base.rt_sa_desc;
    p_api_dev->lword_data_arr[ ul_Index++ ] = p_api_dev->glb_mem.biu[ ul_BiuIndex ].base.rt_bh_area;
    p_api_dev->lword_data_arr[ ul_Index++ ] = p_api_dev->glb_mem.biu[ ul_BiuIndex ].base.rt_sq_area;
    p_api_dev->lword_data_arr[ ul_Index++ ] = p_api_dev->glb_mem.biu[ ul_BiuIndex ].base.rt_eq_area;
    p_api_dev->lword_data_arr[ ul_Index++ ] = p_api_dev->glb_mem.biu[ ul_BiuIndex ].base.bc_bh_area;
    p_api_dev->lword_data_arr[ ul_Index++ ] = p_api_dev->glb_mem.biu[ ul_BiuIndex ].base.bc_sq_area;
    p_api_dev->lword_data_arr[ ul_Index++ ] = p_api_dev->glb_mem.biu[ ul_BiuIndex ].base.bc_eq_area;
    p_api_dev->lword_data_arr[ ul_Index++ ] = p_api_dev->glb_mem.biu[ ul_BiuIndex ].base.bc_xfer_desc;
    p_api_dev->lword_data_arr[ ul_Index++ ] = p_api_dev->glb_mem.biu[ ul_BiuIndex ].base.bc_hip_instr;
    p_api_dev->lword_data_arr[ ul_Index++ ] = p_api_dev->glb_mem.biu[ ul_BiuIndex ].base.bc_lip_instr;
    p_api_dev->lword_data_arr[ ul_Index++ ] = p_api_dev->glb_mem.biu[ ul_BiuIndex ].base.bc_acyc_instr;
    p_api_dev->lword_data_arr[ ul_Index++ ] = p_api_dev->glb_mem.biu[ ul_BiuIndex ].base.rep_buf;
    p_api_dev->lword_data_arr[ ul_Index++ ] = p_api_dev->glb_mem.biu[ ul_BiuIndex ].base.bm_buf;
  }

  /* here ul_Index = 36 */
  p_api_dev->lword_data_arr[ ul_Index++ ] = p_api_dev->glb_mem.sim_buf_base_addr[ BIU_1 - 1 ];
  p_api_dev->lword_data_arr[ ul_Index++ ] = p_api_dev->glb_mem.sim_buf_size[ BIU_1 - 1 ];
  p_api_dev->lword_data_arr[ ul_Index++ ] = p_api_dev->glb_mem.sim_buf_count[ BIU_1 - 1 ];
  p_api_dev->lword_data_arr[ ul_Index++ ] = 0; /* reserved for HS information -> see i39_sys.c */
  p_api_dev->lword_data_arr[ ul_Index++ ] = 0; /* reserved for HS information -> see i39_sys.c */
  p_api_dev->lword_data_arr[ ul_Index++ ] = 0; /* reserved for HS information -> see i39_sys.c */

  /* here ul_Index = 42 */
  for( ul_BiuIndex=0; ul_BiuIndex<BIU_2; ul_BiuIndex++ )
  {
    p_api_dev->lword_data_arr[ ul_Index++ ] = p_api_dev->glb_mem.biu[ ul_BiuIndex ].size.bc_hip_instr;
    p_api_dev->lword_data_arr[ ul_Index++ ] = p_api_dev->glb_mem.biu[ ul_BiuIndex ].size.bc_lip_instr;
    p_api_dev->lword_data_arr[ ul_Index++ ] = p_api_dev->glb_mem.biu[ ul_BiuIndex ].size.bc_acyc_instr;
    p_api_dev->lword_data_arr[ ul_Index++ ] = p_api_dev->glb_mem.biu[ ul_BiuIndex ].size.rep_buf;
    p_api_dev->lword_data_arr[ ul_Index++ ] = p_api_dev->glb_mem.biu[ ul_BiuIndex ].size.bm_buf;
  }

  /* here ul_Index = 52 */
  for( ul_BiuIndex=0; ul_BiuIndex<BIU_2; ul_BiuIndex++ )
  {
    p_api_dev->lword_data_arr[ ul_Index++ ] = p_api_dev->glb_mem.biu[ ul_BiuIndex ].count.rt_bh_area;
    p_api_dev->lword_data_arr[ ul_Index++ ] = p_api_dev->glb_mem.biu[ ul_BiuIndex ].count.rt_sq_area;
    p_api_dev->lword_data_arr[ ul_Index++ ] = p_api_dev->glb_mem.biu[ ul_BiuIndex ].count.rt_eq_area;
    p_api_dev->lword_data_arr[ ul_Index++ ] = p_api_dev->glb_mem.biu[ ul_BiuIndex ].count.bc_bh_area;
    p_api_dev->lword_data_arr[ ul_Index++ ] = p_api_dev->glb_mem.biu[ ul_BiuIndex ].count.bc_sq_area;
    p_api_dev->lword_data_arr[ ul_Index++ ] = p_api_dev->glb_mem.biu[ ul_BiuIndex ].count.bc_eq_area;
    p_api_dev->lword_data_arr[ ul_Index++ ] = p_api_dev->glb_mem.biu[ ul_BiuIndex ].count.bc_xfer_desc;
  }

  /* here ul_Index = 66 */
  p_api_dev->lword_data_arr[ ul_Index++ ] = p_api_dev->GlobalRAMSize / 1024 / 1024;
  p_api_dev->lword_data_arr[ ul_Index++ ] = p_api_dev->GlobalRAMSize2 / 1024 / 1024;

  /* here ul_Index = 68 */
  for( ul_BiuIndex=(BIU_3-1); ul_BiuIndex<BIU_4; ul_BiuIndex++ )
  {
    p_api_dev->lword_data_arr[ ul_Index++ ] = p_api_dev->glb_mem.biu[ ul_BiuIndex ].cb_addr;
    p_api_dev->lword_data_arr[ ul_Index++ ] = p_api_dev->glb_mem.biu[ ul_BiuIndex ].ir_log_addr;
    p_api_dev->lword_data_arr[ ul_Index++ ] = p_api_dev->glb_mem.biu[ ul_BiuIndex ].rt_desc_addr;
    p_api_dev->lword_data_arr[ ul_Index++ ] = p_api_dev->glb_mem.biu[ ul_BiuIndex ].bm_tcb_addr;
    p_api_dev->lword_data_arr[ ul_Index++ ] = p_api_dev->glb_mem.biu[ ul_BiuIndex ].bm_act_addr;
    p_api_dev->lword_data_arr[ ul_Index++ ] = p_api_dev->glb_mem.biu[ ul_BiuIndex ].base.rt_sa_desc;
    p_api_dev->lword_data_arr[ ul_Index++ ] = p_api_dev->glb_mem.biu[ ul_BiuIndex ].base.rt_bh_area;
    p_api_dev->lword_data_arr[ ul_Index++ ] = p_api_dev->glb_mem.biu[ ul_BiuIndex ].base.rt_sq_area;
    p_api_dev->lword_data_arr[ ul_Index++ ] = p_api_dev->glb_mem.biu[ ul_BiuIndex ].base.rt_eq_area;
    p_api_dev->lword_data_arr[ ul_Index++ ] = p_api_dev->glb_mem.biu[ ul_BiuIndex ].base.bc_bh_area;
    p_api_dev->lword_data_arr[ ul_Index++ ] = p_api_dev->glb_mem.biu[ ul_BiuIndex ].base.bc_sq_area;
    p_api_dev->lword_data_arr[ ul_Index++ ] = p_api_dev->glb_mem.biu[ ul_BiuIndex ].base.bc_eq_area;
    p_api_dev->lword_data_arr[ ul_Index++ ] = p_api_dev->glb_mem.biu[ ul_BiuIndex ].base.bc_xfer_desc;
    p_api_dev->lword_data_arr[ ul_Index++ ] = p_api_dev->glb_mem.biu[ ul_BiuIndex ].base.bc_hip_instr;
    p_api_dev->lword_data_arr[ ul_Index++ ] = p_api_dev->glb_mem.biu[ ul_BiuIndex ].base.bc_lip_instr;
    p_api_dev->lword_data_arr[ ul_Index++ ] = p_api_dev->glb_mem.biu[ ul_BiuIndex ].base.bc_acyc_instr;
    p_api_dev->lword_data_arr[ ul_Index++ ] = p_api_dev->glb_mem.biu[ ul_BiuIndex ].base.rep_buf;
    p_api_dev->lword_data_arr[ ul_Index++ ] = p_api_dev->glb_mem.biu[ ul_BiuIndex ].base.bm_buf;
  }

  /* here ul_Index = 104 */
  for( ul_BiuIndex=(BIU_3-1); ul_BiuIndex<BIU_4; ul_BiuIndex++ )
  {
    p_api_dev->lword_data_arr[ ul_Index++ ] = p_api_dev->glb_mem.biu[ ul_BiuIndex ].size.bc_hip_instr;
    p_api_dev->lword_data_arr[ ul_Index++ ] = p_api_dev->glb_mem.biu[ ul_BiuIndex ].size.bc_lip_instr;
    p_api_dev->lword_data_arr[ ul_Index++ ] = p_api_dev->glb_mem.biu[ ul_BiuIndex ].size.bc_acyc_instr;
    p_api_dev->lword_data_arr[ ul_Index++ ] = p_api_dev->glb_mem.biu[ ul_BiuIndex ].size.rep_buf;
    p_api_dev->lword_data_arr[ ul_Index++ ] = p_api_dev->glb_mem.biu[ ul_BiuIndex ].size.bm_buf;
  }

  /* here ul_Index = 114 */
  for( ul_BiuIndex=(BIU_3-1); ul_BiuIndex<BIU_4; ul_BiuIndex++ )
  {
    p_api_dev->lword_data_arr[ ul_Index++ ] = p_api_dev->glb_mem.biu[ ul_BiuIndex ].count.rt_bh_area;
    p_api_dev->lword_data_arr[ ul_Index++ ] = p_api_dev->glb_mem.biu[ ul_BiuIndex ].count.rt_sq_area;
    p_api_dev->lword_data_arr[ ul_Index++ ] = p_api_dev->glb_mem.biu[ ul_BiuIndex ].count.rt_eq_area;
    p_api_dev->lword_data_arr[ ul_Index++ ] = p_api_dev->glb_mem.biu[ ul_BiuIndex ].count.bc_bh_area;
    p_api_dev->lword_data_arr[ ul_Index++ ] = p_api_dev->glb_mem.biu[ ul_BiuIndex ].count.bc_sq_area;
    p_api_dev->lword_data_arr[ ul_Index++ ] = p_api_dev->glb_mem.biu[ ul_BiuIndex ].count.bc_eq_area;
    p_api_dev->lword_data_arr[ ul_Index++ ] = p_api_dev->glb_mem.biu[ ul_BiuIndex ].count.bc_xfer_desc;
  }

  /* here ul_Index = 128 */
  for( ul_BiuIndex=(BIU_5-1); ul_BiuIndex<BIU_8; ul_BiuIndex++ )
  {
    p_api_dev->lword_data_arr[ ul_Index++ ] = p_api_dev->glb_mem.biu[ ul_BiuIndex ].cb_addr;
    p_api_dev->lword_data_arr[ ul_Index++ ] = p_api_dev->glb_mem.biu[ ul_BiuIndex ].ir_log_addr;
    p_api_dev->lword_data_arr[ ul_Index++ ] = p_api_dev->glb_mem.biu[ ul_BiuIndex ].rt_desc_addr;
    p_api_dev->lword_data_arr[ ul_Index++ ] = p_api_dev->glb_mem.biu[ ul_BiuIndex ].bm_tcb_addr;
    p_api_dev->lword_data_arr[ ul_Index++ ] = p_api_dev->glb_mem.biu[ ul_BiuIndex ].bm_act_addr;
    p_api_dev->lword_data_arr[ ul_Index++ ] = p_api_dev->glb_mem.biu[ ul_BiuIndex ].base.rt_sa_desc;
    p_api_dev->lword_data_arr[ ul_Index++ ] = p_api_dev->glb_mem.biu[ ul_BiuIndex ].base.rt_bh_area;
    p_api_dev->lword_data_arr[ ul_Index++ ] = p_api_dev->glb_mem.biu[ ul_BiuIndex ].base.rt_sq_area;
    p_api_dev->lword_data_arr[ ul_Index++ ] = p_api_dev->glb_mem.biu[ ul_BiuIndex ].base.rt_eq_area;
    p_api_dev->lword_data_arr[ ul_Index++ ] = p_api_dev->glb_mem.biu[ ul_BiuIndex ].base.bc_bh_area;
    p_api_dev->lword_data_arr[ ul_Index++ ] = p_api_dev->glb_mem.biu[ ul_BiuIndex ].base.bc_sq_area;
    p_api_dev->lword_data_arr[ ul_Index++ ] = p_api_dev->glb_mem.biu[ ul_BiuIndex ].base.bc_eq_area;
    p_api_dev->lword_data_arr[ ul_Index++ ] = p_api_dev->glb_mem.biu[ ul_BiuIndex ].base.bc_xfer_desc;
    p_api_dev->lword_data_arr[ ul_Index++ ] = p_api_dev->glb_mem.biu[ ul_BiuIndex ].base.bc_hip_instr;
    p_api_dev->lword_data_arr[ ul_Index++ ] = p_api_dev->glb_mem.biu[ ul_BiuIndex ].base.bc_lip_instr;
    p_api_dev->lword_data_arr[ ul_Index++ ] = p_api_dev->glb_mem.biu[ ul_BiuIndex ].base.bc_acyc_instr;
    p_api_dev->lword_data_arr[ ul_Index++ ] = p_api_dev->glb_mem.biu[ ul_BiuIndex ].base.rep_buf;
    p_api_dev->lword_data_arr[ ul_Index++ ] = p_api_dev->glb_mem.biu[ ul_BiuIndex ].base.bm_buf;

    p_api_dev->lword_data_arr[ ul_Index++ ] = p_api_dev->glb_mem.biu[ ul_BiuIndex ].size.bc_hip_instr;
    p_api_dev->lword_data_arr[ ul_Index++ ] = p_api_dev->glb_mem.biu[ ul_BiuIndex ].size.bc_lip_instr;
    p_api_dev->lword_data_arr[ ul_Index++ ] = p_api_dev->glb_mem.biu[ ul_BiuIndex ].size.bc_acyc_instr;
    p_api_dev->lword_data_arr[ ul_Index++ ] = p_api_dev->glb_mem.biu[ ul_BiuIndex ].size.rep_buf;
    p_api_dev->lword_data_arr[ ul_Index++ ] = p_api_dev->glb_mem.biu[ ul_BiuIndex ].size.bm_buf;

    p_api_dev->lword_data_arr[ ul_Index++ ] = p_api_dev->glb_mem.biu[ ul_BiuIndex ].count.rt_bh_area;
    p_api_dev->lword_data_arr[ ul_Index++ ] = p_api_dev->glb_mem.biu[ ul_BiuIndex ].count.rt_sq_area;
    p_api_dev->lword_data_arr[ ul_Index++ ] = p_api_dev->glb_mem.biu[ ul_BiuIndex ].count.rt_eq_area;
    p_api_dev->lword_data_arr[ ul_Index++ ] = p_api_dev->glb_mem.biu[ ul_BiuIndex ].count.bc_bh_area;
    p_api_dev->lword_data_arr[ ul_Index++ ] = p_api_dev->glb_mem.biu[ ul_BiuIndex ].count.bc_sq_area;
    p_api_dev->lword_data_arr[ ul_Index++ ] = p_api_dev->glb_mem.biu[ ul_BiuIndex ].count.bc_eq_area;
    p_api_dev->lword_data_arr[ ul_Index++ ] = p_api_dev->glb_mem.biu[ ul_BiuIndex ].count.bc_xfer_desc;
  }

  /* here ul_Index = 248 */
  p_api_dev->lword_data_arr[ ul_Index++ ] = p_api_dev->glb_mem.sim_buf_base_addr[ p_api_dev->ul_NbOfStreams1 + BIU_1 - 1 ];
  p_api_dev->lword_data_arr[ ul_Index++ ] = p_api_dev->glb_mem.sim_buf_size[ p_api_dev->ul_NbOfStreams1 + BIU_1 - 1 ];
  p_api_dev->lword_data_arr[ ul_Index++ ] = p_api_dev->glb_mem.sim_buf_count[ p_api_dev->ul_NbOfStreams1 + BIU_1 - 1 ];
  p_api_dev->lword_data_arr[ ul_Index++ ] = 0; /* only HS */
  p_api_dev->lword_data_arr[ ul_Index++ ] = 0; /* only HS */
  p_api_dev->lword_data_arr[ ul_Index++ ] = 0;

  /* here ul_Index = 254 */
  ul_ActualStreamNb = 1;
  b_FirstBiuOfHSStream = 1;
  for( ul_BiuIndex=BIU_1; ul_BiuIndex<=BIU_8; ul_BiuIndex++ )
  {
    if(ul_BiuIndex<=(p_api_dev->ul_NbOfStreams1+p_api_dev->ul_NbOfStreams2))
    {
      if(ul_StreamIs3910(p_api_dev,ul_BiuIndex - 1))
      {
        if( b_FirstBiuOfHSStream )
        {
          b_FirstBiuOfHSStream = 0;
          p_api_dev->lword_data_arr[ ul_Index++ ] = 1; /* 1553 protocol*/
          p_api_dev->lword_data_arr[ ul_Index++ ] = ul_ActualStreamNb; /* stream number */
        }
        else
        {
          b_FirstBiuOfHSStream = 1;
          p_api_dev->lword_data_arr[ ul_Index++ ] = 2; /* 3910 protocol*/
          p_api_dev->lword_data_arr[ ul_Index++ ] = ul_ActualStreamNb; /* stream number */
          ul_ActualStreamNb++;
        }
      }
      else if(ul_StreamIsEFEX(p_api_dev,ul_BiuIndex - 1))
      {
        if( b_FirstBiuOfHSStream )
        {
          b_FirstBiuOfHSStream = 0;
          p_api_dev->lword_data_arr[ ul_Index++ ] = 1; /* 1553 protocol*/
          p_api_dev->lword_data_arr[ ul_Index++ ] = ul_ActualStreamNb; /* stream number */
        }
        else
        {
          b_FirstBiuOfHSStream = 1;
          p_api_dev->lword_data_arr[ ul_Index++ ] = 3; /* EFEX protocol*/
          p_api_dev->lword_data_arr[ ul_Index++ ] = ul_ActualStreamNb; /* stream number */
          ul_ActualStreamNb++;
        }
      }
      else
      {
        p_api_dev->lword_data_arr[ ul_Index++ ] = 1; /* 1553 protocol*/
        p_api_dev->lword_data_arr[ ul_Index++ ] = ul_ActualStreamNb; /* stream number */
        ul_ActualStreamNb++;
      }

      if(ul_BiuIndex <= p_api_dev->ul_NbOfStreams1)
        p_api_dev->lword_data_arr[ ul_Index++ ] = 0; /* memory bank */
      else
        p_api_dev->lword_data_arr[ ul_Index++ ] = 1; /* memory bank */
    }
    else
    {
      p_api_dev->lword_data_arr[ ul_Index++ ] = 0;
      p_api_dev->lword_data_arr[ ul_Index++ ] = 0;
      p_api_dev->lword_data_arr[ ul_Index++ ] = 0;
    }
  }

  /* here ul_Index = 278 */

  if(mode == 1)
  {
    enter_drv_dump_mem_layout(p_api_dev);
  }

  ackfl = API_ACK2;

  p_api_dev->b_length = 0;
  p_api_dev->w_length = 0;
  p_api_dev->lw_length = 278;
  p_api_dev->ack_lw_ptr = &(p_api_dev->lword_data_arr[ 0 ]);

  return (ackfl);

} /* end: api_sys_get_mem_partition */
/*****************************************************************************/
/*                                                                           */
/*   Module : API_SYS                  Submodule : API_SYS_SET_MEM_PARTITION */
/*                                                                           */
/*   Author : Marco Motz               Project   : API1553-DS                */
/*                                                                           */
/*   Source : C                        Tools     : PC/AT; Norton Editor;     */
/*                                                 CYGNUS, GNU-C, As, and LD */
/*                                                 IDT-C 5.1 Toolkit         */
/*---------------------------------------------------------------------------*/
/*   Create : 02.06.2003 Update : 25.10.2006                                 */
/*---------------------------------------------------------------------------*/
/*    Descriptions                                                           */
/*    ------------                                                           */
/*    Inputs    :                                                            */
/*                                                                           */
/*    Outputs   :                                                            */
/*                                                                           */
/*    Description :                                                          */
/*                                                                           */
/*****************************************************************************/
L_WORD api_sys_set_mem_part_cmd( TY_API_DEV *p_api_dev, TY_MIL_COM_SYS_MEM_PART_SET_INPUT * in, TY_MIL_COM_SYS_MEM_PART_SET_OUTPUT * out )
{
    short i;
    TY_MEM_DEFINITION ax_MemDef[MAX_BIU];

    for( i=0; i<MAX_BIU ; i++ )
    {
        ax_MemDef[i].rt_bh_area    = in->mem_info.ax_BiuCnt[i].ul_RtBhArea;
        ax_MemDef[i].rt_sq_area    = in->mem_info.ax_BiuCnt[i].ul_RtSqArea;
        ax_MemDef[i].rt_eq_area    = in->mem_info.ax_BiuCnt[i].ul_RtEqArea;
        ax_MemDef[i].bc_bh_area    = in->mem_info.ax_BiuCnt[i].ul_BcBhArea;
        ax_MemDef[i].bc_sq_area    = in->mem_info.ax_BiuCnt[i].ul_BcSqArea;
        ax_MemDef[i].bc_eq_area    = in->mem_info.ax_BiuCnt[i].ul_BcEqArea;
        ax_MemDef[i].bc_xfer_desc  = in->mem_info.ax_BiuCnt[i].ul_BcXferDesc;

        ax_MemDef[i].bc_hip_instr  = in->mem_info.ax_BiuSize[i].ul_BcHipInstr  / 4; /* Get part size in bytes / set in entries */
        ax_MemDef[i].bc_lip_instr  = in->mem_info.ax_BiuSize[i].ul_BcLipInstr  / 4; /* Get part size in bytes / set in entries */
        ax_MemDef[i].bc_acyc_instr = in->mem_info.ax_BiuSize[i].ul_BcAcycInstr / 4; /* Get part size in bytes / set in entries */
        ax_MemDef[i].rep_buf       = in->mem_info.ax_BiuSize[i].ul_RepBuf;
        ax_MemDef[i].bm_buf        = in->mem_info.ax_BiuSize[i].ul_BmBuf;
        
        if(i < p_api_dev->ul_NbOfStreams1)
            ax_MemDef[i].sim_buf       = in->mem_info.aul_SimBufSize[0];
        else
            ax_MemDef[i].sim_buf       = in->mem_info.aul_SimBufSize[1];
    }

    /* if at least first PBI is 1553 initialize LS Memory */
    if( (0x00003000 != (p_api_dev->BoardType & 0x0000F000)) &&
        (0x00007000 != (p_api_dev->BoardType & 0x0000F000)) )
        api_sys_set_mem_partition(p_api_dev, in->mode, ax_MemDef, &out->status, out->mem_used );

    /* if at least one PBI is 3910/EFEX initialize HS Memory */
    if( (0x00003000 == (p_api_dev->BoardType & 0x0000F000)) ||
        (0x00007000 == (p_api_dev->BoardType & 0x0000F000)) ||
        (0x30000000 == (p_api_dev->BoardType & 0xF0000000)) ||
        (0x70000000 == (p_api_dev->BoardType & 0xF0000000)) )
    {

        /* Modify values to ensure compatibility between get and set mem partition */
        for( i=0; i<MAX_BIU; i++ )
        {
            ax_MemDef[i].bc_bh_area   *= 2;
            ax_MemDef[i].bc_sq_area   *= 2;
            ax_MemDef[i].bc_eq_area   *= 2;


            if( p_api_dev->auc_StreamType[i] == API_TYPE_1553 )
              /* must not be doubled on HS BIU */
              ax_MemDef[i].bc_xfer_desc *= 2;
        }

        api39_sys_set_mem_partition(p_api_dev, in->mode, ax_MemDef, &out->status, out->mem_used);
    }

    return 0;
}


L_WORD api_sys_set_mem_partition( TY_API_DEV *p_api_dev, L_WORD mode, TY_MEM_DEFINITION ax_MemDef[MAX_BIU], L_WORD * ret_status, L_WORD ret_mem_used[2] )
{
  L_WORD st;
  L_WORD param_chk[MAX_BIU];
  L_WORD ul_Index;
  L_WORD status;
  L_WORD mem_used[2];

    //BYTE mode, TY_MEM_DEFINITION ax_MemDef[MAX_BIU]

  PRINTF0("api_sys_set_mem_partition \n\r");
  PRINTF0("              BIU1       BIU2       BIU3       BIU4\n\r");
  PRINTF4("rt_bh_area    [%08d] [%08d] [%08d] [%08d]\n\r", ax_MemDef[0].rt_bh_area, ax_MemDef[1].rt_bh_area, ax_MemDef[2].rt_bh_area, ax_MemDef[3].rt_bh_area);
  PRINTF4("rt_sq_area    [%08d] [%08d] [%08d] [%08d]\n\r", ax_MemDef[0].rt_sq_area, ax_MemDef[1].rt_sq_area, ax_MemDef[2].rt_sq_area, ax_MemDef[3].rt_sq_area);
  PRINTF4("rt_eq_area    [%08d] [%08d] [%08d] [%08d]\n\r", ax_MemDef[0].rt_eq_area, ax_MemDef[1].rt_eq_area, ax_MemDef[2].rt_eq_area, ax_MemDef[3].rt_eq_area);
  PRINTF4("bc_bh_area    [%08d] [%08d] [%08d] [%08d]\n\r", ax_MemDef[0].bc_bh_area, ax_MemDef[1].bc_bh_area, ax_MemDef[2].bc_bh_area, ax_MemDef[3].bc_bh_area);
  PRINTF4("bc_sq_area    [%08d] [%08d] [%08d] [%08d]\n\r", ax_MemDef[0].bc_sq_area, ax_MemDef[1].bc_sq_area, ax_MemDef[2].bc_sq_area, ax_MemDef[3].bc_sq_area);
  PRINTF4("bc_eq_area    [%08d] [%08d] [%08d] [%08d]\n\r", ax_MemDef[0].bc_eq_area, ax_MemDef[1].bc_eq_area, ax_MemDef[2].bc_eq_area, ax_MemDef[3].bc_eq_area);
  PRINTF4("bc_xfer_desc  [%08d] [%08d] [%08d] [%08d]\n\r", ax_MemDef[0].bc_xfer_desc, ax_MemDef[1].bc_xfer_desc, ax_MemDef[2].bc_xfer_desc, ax_MemDef[3].bc_xfer_desc);
  PRINTF4("bc_hip_instr  [%08d] [%08d] [%08d] [%08d]\n\r", ax_MemDef[0].bc_hip_instr, ax_MemDef[1].bc_hip_instr, ax_MemDef[2].bc_hip_instr, ax_MemDef[3].bc_hip_instr);
  PRINTF4("bc_lip_instr  [%08d] [%08d] [%08d] [%08d]\n\r", ax_MemDef[0].bc_lip_instr, ax_MemDef[1].bc_lip_instr, ax_MemDef[2].bc_lip_instr, ax_MemDef[3].bc_lip_instr);
  PRINTF4("bc_acyc_instr [%08d] [%08d] [%08d] [%08d]\n\r", ax_MemDef[0].bc_acyc_instr, ax_MemDef[1].bc_acyc_instr, ax_MemDef[2].bc_acyc_instr, ax_MemDef[3].bc_acyc_instr);
  PRINTF4("rep_buf       [%08X] [%08X] [%08X] [%08X]\n\r", ax_MemDef[0].rep_buf, ax_MemDef[1].rep_buf, ax_MemDef[2].rep_buf, ax_MemDef[3].rep_buf);
  PRINTF4("bm_buf        [%08X] [%08X] [%08X] [%08X]\n\r", ax_MemDef[0].bm_buf, ax_MemDef[1].bm_buf, ax_MemDef[2].bm_buf, ax_MemDef[3].bm_buf);
  PRINTF4("sim_buf       [%08X] [%08X] [%08X] [%08X]\n\r\n\r", ax_MemDef[0].sim_buf, ax_MemDef[1].sim_buf, ax_MemDef[2].sim_buf, ax_MemDef[3].sim_buf);

  PRINTF0("              BIU5       BIU6       BIU7       BIU8\n\r");
  PRINTF4("rt_bh_area    [%08d] [%08d] [%08d] [%08d]\n\r", ax_MemDef[4].rt_bh_area, ax_MemDef[5].rt_bh_area, ax_MemDef[6].rt_bh_area, ax_MemDef[7].rt_bh_area);
  PRINTF4("rt_sq_area    [%08d] [%08d] [%08d] [%08d]\n\r", ax_MemDef[4].rt_sq_area, ax_MemDef[5].rt_sq_area, ax_MemDef[6].rt_sq_area, ax_MemDef[7].rt_sq_area);
  PRINTF4("rt_eq_area    [%08d] [%08d] [%08d] [%08d]\n\r", ax_MemDef[4].rt_eq_area, ax_MemDef[5].rt_eq_area, ax_MemDef[6].rt_eq_area, ax_MemDef[7].rt_eq_area);
  PRINTF4("bc_bh_area    [%08d] [%08d] [%08d] [%08d]\n\r", ax_MemDef[4].bc_bh_area, ax_MemDef[5].bc_bh_area, ax_MemDef[6].bc_bh_area, ax_MemDef[7].bc_bh_area);
  PRINTF4("bc_sq_area    [%08d] [%08d] [%08d] [%08d]\n\r", ax_MemDef[4].bc_sq_area, ax_MemDef[5].bc_sq_area, ax_MemDef[6].bc_sq_area, ax_MemDef[7].bc_sq_area);
  PRINTF4("bc_eq_area    [%08d] [%08d] [%08d] [%08d]\n\r", ax_MemDef[4].bc_eq_area, ax_MemDef[5].bc_eq_area, ax_MemDef[6].bc_eq_area, ax_MemDef[7].bc_eq_area);
  PRINTF4("bc_xfer_desc  [%08d] [%08d] [%08d] [%08d]\n\r", ax_MemDef[4].bc_xfer_desc, ax_MemDef[5].bc_xfer_desc, ax_MemDef[6].bc_xfer_desc, ax_MemDef[7].bc_xfer_desc);
  PRINTF4("bc_hip_instr  [%08d] [%08d] [%08d] [%08d]\n\r", ax_MemDef[4].bc_hip_instr, ax_MemDef[5].bc_hip_instr, ax_MemDef[6].bc_hip_instr, ax_MemDef[7].bc_hip_instr);
  PRINTF4("bc_lip_instr  [%08d] [%08d] [%08d] [%08d]\n\r", ax_MemDef[4].bc_lip_instr, ax_MemDef[5].bc_lip_instr, ax_MemDef[6].bc_lip_instr, ax_MemDef[7].bc_lip_instr);
  PRINTF4("bc_acyc_instr [%08d] [%08d] [%08d] [%08d]\n\r", ax_MemDef[4].bc_acyc_instr, ax_MemDef[5].bc_acyc_instr, ax_MemDef[6].bc_acyc_instr, ax_MemDef[7].bc_acyc_instr);
  PRINTF4("rep_buf       [%08X] [%08X] [%08X] [%08X]\n\r", ax_MemDef[4].rep_buf, ax_MemDef[5].rep_buf, ax_MemDef[6].rep_buf, ax_MemDef[7].rep_buf);
  PRINTF4("bm_buf        [%08X] [%08X] [%08X] [%08X]\n\r", ax_MemDef[4].bm_buf, ax_MemDef[5].bm_buf, ax_MemDef[6].bm_buf, ax_MemDef[7].bm_buf);
  PRINTF4("sim_buf       [%08X] [%08X] [%08X] [%08X]\n\r\n\r", ax_MemDef[4].sim_buf, ax_MemDef[5].sim_buf, ax_MemDef[6].sim_buf, ax_MemDef[7].sim_buf);

  status = API_SYS_SET_MEM_FAILED;
  mem_used[0] = 0L;
  mem_used[1] = 0L;

  if (mode == 2)
  {
      if (!mil_fw_is_cmd_supported(p_api_dev, MIL_FW_CMD_BM_ENH_ACTIVITY_PAGE))
      {
          if (ret_status)
              *ret_status = API_ERR_CMD_NOT_SUPPORTED_BY_FW;

          if (ret_mem_used)
          {
              ret_mem_used[0] = mem_used[0];
              ret_mem_used[1] = mem_used[1];
          }
          return 0;
      }
  }

  for( ul_Index=0; ul_Index<MAX_BIU; ul_Index++ )
  {
    param_chk[ul_Index] = (ax_MemDef[ul_Index].rt_bh_area   <= (MAX_MEM_MUL * MAX_API_RT_HID) )  && 
            (ax_MemDef[ul_Index].rt_sq_area   <= (MAX_MEM_MUL * MAX_API_DEFAULT_BUF_ID) )   && 
            (ax_MemDef[ul_Index].rt_eq_area   <= (MAX_MEM_MUL * MAX_API_RT_EQ) )   && 
            (ax_MemDef[ul_Index].bc_bh_area   <= (MAX_MEM_MUL * MAX_API_BC_HID) )  && 
            (ax_MemDef[ul_Index].bc_sq_area   <= (MAX_MEM_MUL * MAX_API_DEFAULT_BUF_ID) )   && 
            (ax_MemDef[ul_Index].bc_eq_area   <= (MAX_MEM_MUL * MAX_API_BC_EQ) )   && 
            (ax_MemDef[ul_Index].bc_xfer_desc <= (MAX_MEM_MUL * MAX_API_BC_XFER) ) && 
            ( (ax_MemDef[ul_Index].bm_buf % 0x10000) == 0) && ( (ax_MemDef[ul_Index].sim_buf % 0x10000) == 0);
  }

  if (param_chk[0] && param_chk[1] && param_chk[2] && param_chk[3] &&
      param_chk[4] && param_chk[5] && param_chk[6] && param_chk[7])
  {
    /* Check if requested size would fit into the Global Memory */
    st = enter_drv_check_mem_layout(p_api_dev, ax_MemDef, mem_used);

    if(st == API_OK)
    {
      /* initialize memory mapping of memory bank 0, if 1553 */
      if( (0x00003000 != (p_api_dev->BoardType & 0x0000F000)) &&
          (0x00007000 != (p_api_dev->BoardType & 0x0000F000)) )
        enter_drv_set_mem_layout(p_api_dev, mode, 0, ax_MemDef);

      /* initialize memory mapping of memory bank 1, if 1553  */
      if( (0x30000000 != (p_api_dev->BoardType & 0xF0000000)) &&
          (0x70000000 != (p_api_dev->BoardType & 0xF0000000)) &&
          (0x00FF0000 != (p_api_dev->BoardType & 0x00FF0000)) )
        enter_drv_set_mem_layout(p_api_dev, mode, 1, ax_MemDef);

      /* Instantiate the pointers */
      api_ini(p_api_dev, 2); /* API1553 */

      status = API_SYS_SET_MEM_OK;
    }
    else
    {
      /* requested layout does not fit -> use default */
      status = API_SYS_SET_MEM_FAILED;
    }
  }
  else {
    /* input parameter out of range */
    status = API_SYS_SET_MEM_PARAM_OUT_OF_RANGE;
  }

  if(mode == 1)
  {
    enter_drv_dump_mem_layout(p_api_dev);
  }

  if( ret_status ) 
  {
      *ret_status = status;
  }
  if( ret_mem_used ) 
  {
      ret_mem_used[0] = mem_used[0];
      ret_mem_used[1] = mem_used[1];
  }

  return 0;

} /* end: api_sys_set_mem_partition */






/*! \brief Read discrete input values and add the value to the host to target acknowlege */
/*!

    \param  p_api_dev    TY_API_DEV*      The global variable container.

    \return BYTE The return value is API_ACK2
*/

L_WORD api_sys_discretes_read(TY_API_DEV *p_api_dev, TY_MIL_COM * in, TY_MIL_COM_SYS_DISCRETES_READ_OUTPUT  * out)
{
  return mil_hw_discretes_read( p_api_dev, &out->value );
}

L_WORD api_sys_discretes_config_read(TY_API_DEV *p_api_dev, TY_MIL_COM * in, TY_MIL_COM_SYS_DISCRETES_CONFIG_READ_OUTPUT  * out)
{
	out->value = p_api_dev->discretes.setup;
	return API_OK;
}

L_WORD api_sys_discretes_info_read(TY_API_DEV *p_api_dev, TY_MIL_COM * in, TY_MIL_COM_SYS_DISCRETES_INFO_READ_OUTPUT  * out)
{
    L_WORD discreteChannels = 0;
    L_WORD canIn = 0;
    L_WORD canOut = 0;
    L_WORD ret_val = 0;

    ret_val = mil_hw_discretes_info_read(p_api_dev, &discreteChannels, &canIn, &canOut);
    out->discreteChannels = discreteChannels;
    out->canIn = canIn;
    out->canOut = canOut;

    return ret_val;
}


L_WORD api_sys_trigger_edge_input_set(TY_API_DEV *p_api_dev, TY_MIL_COM_WITH_VALUE * in, TY_MIL_COM_ACK  * out)
{
    return mil_hw_trigger_edge_input_set(p_api_dev, p_api_dev->biu, in->value);
}


L_WORD api_sys_trigger_edge_input_get(TY_API_DEV *p_api_dev, TY_MIL_COM * in, TY_MIL_COM_ACK_WITH_VALUE  * out)
{
    return mil_hw_trigger_edge_input_get(p_api_dev, p_api_dev->biu, &out->value);
}

L_WORD api_sys_trigger_digital_loop_con(TY_API_DEV *p_api_dev, TY_MIL_COM_WITH_VALUE * in, TY_MIL_COM_ACK  * out)
{
    return mil_hw_trigger_digital_loop_con(p_api_dev, in->value);
}

L_WORD api_sys_trigger_digital_loop_get(TY_API_DEV *p_api_dev, TY_MIL_COM * in, TY_MIL_COM_ACK_WITH_VALUE  * out)
{
    return mil_hw_trigger_digital_loop_get(p_api_dev, &out->value);
}



L_WORD api_sys_discretes_write(TY_API_DEV *p_api_dev, TY_MIL_COM_SYS_DISCRETES_WRITE_INPUT * in, TY_MIL_COM_ACK * out)
{
  return mil_hw_discretes_write( p_api_dev, in->mask, in->value );
}


L_WORD api_sys_discretes_init(TY_API_DEV *p_api_dev, TY_MIL_COM_SYS_DISCRETES_INIT_INPUT * in, TY_MIL_COM_ACK * out)
{
	p_api_dev->discretes.setup = in->setup;
	return mil_hw_discretes_init( p_api_dev, in->setup );
}

L_WORD api_sys_discretes_rec_init(TY_API_DEV *p_api_dev, TY_MIL_COM_SYS_DISCRETES_REC_INIT_INPUT * in, TY_MIL_COM_ACK * out)
{
	return mil_hw_discretes_fifo(p_api_dev, in->setup);
}


/*****************************************************************************/
/*                                                                           */
/*   Module : API_SYS                  Submodule : API_SYNC_COUNTER_SET      */
/*                                                                           */
/*   Author : Patrick Giesel           Project   : API1553-DS                */
/*                                                                           */
/*   Source : C                        Tools     : PC/AT; Norton Editor;     */
/*                                                 CYGNUS, GNU-C, As, and LD */
/*                                                 IDT-C 5.1 Toolkit         */
/*---------------------------------------------------------------------------*/
/*   Create : 13.02.07   Update : -                                          */
/*---------------------------------------------------------------------------*/
/*    Descriptions                                                           */
/*    ------------                                                           */
/*    Inputs    : Sync Counter Value [ul_SyncCntVal]                         */
/*                                                                           */
/*    Outputs   : Instruction acknowledge type [ackfl]                       */
/*                                                                           */
/*    Description :                                                          */
/*    This function handles the 'API_BC_SYNC_CNT_SET' instruction to set     */
/*    the MC17 synchronization counter.                                      */
/*                                                                           */
/*****************************************************************************/

L_WORD api_sys_sync_counter_set(TY_API_DEV *p_api_dev, TY_MIL_COM_SYS_SYNC_COUNTER_SET_INPUT * in, TY_MIL_COM_ACK * out )
{
  L_WORD ret_val = 0;

  if (!mil_fw_is_cmd_supported(p_api_dev, MIL_FW_CMD_RT_ACTIVATE_SYNC_MC))
      /* According to BS get and set make no sense on Multichannel  */
      return API_ERR_CMD_NOT_SUPPORTED_BY_FW;

  /* store sync counter value in system command input word */
  (*p_api_dev->cb_p[ p_api_dev->biu ]).cmd_inout = BSWAP32(in->value);
  wbflush();
  UsbSynchronizeLWord(p_api_dev, API_GLB_MEM_ADDR_REL(&p_api_dev->cb_p[p_api_dev->biu]->cmd_inout), Out);
  
  if (ExecuteFirmwareCommand(p_api_dev, p_api_dev->biu, API_SCB_COM_SET_SYNC) != 0)
      ret_val = API_ERR_TARGET_RESPONSE_TIMEOUT;

  return ret_val;
}

/*****************************************************************************/
/*                                                                           */
/*   Module : API_SYS                  Submodule : API_SYNC_COUNTER_GET      */
/*                                                                           */
/*   Author : Patrick Giesel           Project   : API1553-DS                */
/*                                                                           */
/*   Source : C                        Tools     : PC/AT; Norton Editor;     */
/*                                                 CYGNUS, GNU-C, As, and LD */
/*                                                 IDT-C 5.1 Toolkit         */
/*---------------------------------------------------------------------------*/
/*   Create : 13.02.07   Update : -                                          */
/*---------------------------------------------------------------------------*/
/*    Descriptions                                                           */
/*    ------------                                                           */
/*    Inputs    : Sync Counter Value [ul_SyncCntVal]                         */
/*                                                                           */
/*    Outputs   : Instruction acknowledge type [ackfl]                       */
/*                                                                           */
/*    Description :                                                          */
/*    This function handles the 'API_BC_SYNC_CNT_SET' instruction to set     */
/*    the MC17 synchronization counter.                                      */
/*                                                                           */
/*****************************************************************************/
L_WORD api_sys_sync_counter_get( TY_API_DEV *p_api_dev, TY_MIL_COM * in, TY_MIL_COM_SYS_SYNC_COUNTER_GET_OUTPUT * out )
{
  L_WORD ret_val = 0;

  if (!mil_fw_is_cmd_supported(p_api_dev, MIL_FW_CMD_RT_ACTIVATE_SYNC_MC))
      /* According to BS get and set make no sense on Multichannel  */
      return API_ERR_CMD_NOT_SUPPORTED_BY_FW;

  /* clear sync counter value in system command input word */
  (*p_api_dev->cb_p[ p_api_dev->biu ]).cmd_inout = 0;
  wbflush();
  UsbSynchronizeLWord(p_api_dev, API_GLB_MEM_ADDR_REL(&p_api_dev->cb_p[p_api_dev->biu]->cmd_inout), Out);

  if (ExecuteFirmwareCommand(p_api_dev, p_api_dev->biu, API_SCB_COM_GET_SYNC) != 0)
      ret_val = API_ERR_TARGET_RESPONSE_TIMEOUT;

  UsbSynchronizeLWord(p_api_dev, API_GLB_MEM_ADDR_REL(&p_api_dev->cb_p[p_api_dev->biu]->cmd_inout), In);
  UsbSynchronizeLWord(p_api_dev, API_GLB_MEM_ADDR_REL(&p_api_dev->cb_p[p_api_dev->biu]->syscnt_init), In);
  UsbSynchronizeLWord(p_api_dev, API_GLB_MEM_ADDR_REL(&p_api_dev->cb_p[p_api_dev->biu]->syscnt_ttl), In);
  UsbSynchronizeLWord(p_api_dev, API_GLB_MEM_ADDR_REL(&p_api_dev->cb_p[p_api_dev->biu]->syscnt_tth), In);

  out->value = BSWAP32( (*p_api_dev->cb_p[ p_api_dev->biu ]).cmd_inout);
  out->init  = BSWAP32( (*p_api_dev->cb_p[ p_api_dev->biu ]).syscnt_init);
  out->ttl   = BSWAP32( (*p_api_dev->cb_p[ p_api_dev->biu ]).syscnt_ttl);
  out->tth   = BSWAP32( (*p_api_dev->cb_p[ p_api_dev->biu ]).syscnt_tth);

  return ret_val;
}

/*****************************************************************************/
/*                                                                           */
/*   Module : API_SYS                  Submodule : API_SYS_PXI_CON           */
/*                                                                           */
/*   Author : Patrick Giesel           Project   : API1553-DS                */
/*                                                                           */
/*   Source : C                        Tools     : PC/AT; Norton Editor;     */
/*                                                 CYGNUS, GNU-C, As, and LD */
/*                                                 IDT-C 5.1 Toolkit         */
/*---------------------------------------------------------------------------*/
/*   Create : 13.02.07   Update : -                                          */
/*---------------------------------------------------------------------------*/
/*    Descriptions                                                           */
/*    ------------                                                           */
/*    Inputs    : Mode [ul_Mode]                                             */
/*                Trigger Source [ul_TrgSource]                              */
/*                Trigger Destination [ul_TrgDest]                           */
/*                TT Clear Flag  [ul_TTClear]                                */
/*                                                                           */
/*    Outputs   : none                                                       */
/*                                                                           */
/*    Description :                                                          */
/*    This function handles the 'API_SYS_PXI_CON' instruction to set         */
/*    the PXI Trigger and TT Source.                                         */
/*                                                                           */
/*****************************************************************************/
L_WORD api_sys_pxi_con( TY_API_DEV *p_api_dev, TY_MIL_COM_SYS_PXI_CON_INPUT * in, TY_MIL_COM_ACK * out )
{
    AiUInt32 retval     = 0;

    switch( in->mode )
    {
    case API_PXI_CLR_TRG:
        retval = mil_hw_pxi_trigger_in_disable( p_api_dev);
        if(retval != 0) break;
        retval = mil_hw_pxi_trigger_out_disable(p_api_dev);
        break;
    case API_PXI_SET_TTSRC_EXTERNAL:
        retval = mil_hw_pxi_timetag_src_control(p_api_dev, 0, in->tt_clear);
        break;
    case API_PXI_SET_TTSRC_INTERNAL:
        retval = mil_hw_pxi_timetag_src_control(p_api_dev, 1, in->tt_clear);
        break;
    case API_PXI_SET_TRG:
        if( in->source < API_TRG_BC_CHN1 ) /* Trigger Source is a PXI Trigger Line */
        {
            if( in->dest > API_TRG_PXI7 ) /* Trigger Destination MUST be a board trigger */
            {
                retval = mil_hw_pxi_trigger_in_select(p_api_dev, (BYTE)in->dest, in->source);
            }
            else
            {
                retval = API_ERR_PARAMETER_OUT_OF_RANGE;
            }
        }
        else /* Trigger Source is a Board Trigger Line */
        {
            if( in->dest <= API_TRG_PXI7 ) /* Trigger Dest MUST be a PXI Trigger Line */
            {
                retval = mil_hw_pxi_trigger_out_select(p_api_dev, (AiUInt8)in->source, (AiUInt8)in->dest);
            }
            else
            {
                retval = API_ERR_PARAMETER_OUT_OF_RANGE;
            }
        }
        break;
    default:
        retval = API_ERR_INVALID_MODE;
        break;
    }


    return retval;
}

L_WORD api_sys_pxi_geo_addr(TY_API_DEV *p_api_dev, TY_MIL_COM * in, TY_MIL_COM_ACK_WITH_VALUE * out)
{
    L_WORD retval = 0;

    retval = mil_hw_pxi_geo_addr(p_api_dev, &out->value);

    return retval;
}




L_WORD api_sys_irig_get( TY_API_DEV *p_api_dev, TY_MIL_COM * in, TY_MIL_COM_SYS_IRIG_GET_OUTPUT * out )
{
    L_WORD ret_val = 0;
    L_WORD day, hour, min, sec, usec, sth, stl;

    ret_val = mil_hw_irig_get_time( p_api_dev, &day, &hour, &min, &sec, &usec, &sth, &stl );

    out->day   = day;
    out->hour  = hour;
    out->min   = min;
    out->sec   = sec;
    out->usec  = usec;

    return ret_val;
}




L_WORD api_sys_irig_status_get( TY_API_DEV *p_api_dev, TY_MIL_COM * in, TY_MIL_COM_SYS_IRIG_STATUS_GET_OUTPUT * out )
{
    L_WORD ret_val   = 0;
    L_WORD irig_sync = 0;
    L_WORD irig_src  = 0;

    ret_val = mil_hw_irig_get_status( p_api_dev, &irig_sync, &irig_src );

    out->sync   = irig_sync;
    out->src    = irig_src;

    return ret_val;
}





L_WORD api_sys_irig_set( TY_API_DEV *p_api_dev, TY_MIL_COM_SYS_IRIG_SET_INPUT * in, TY_MIL_COM_ACK * out )
{
    L_WORD sec = in->sec & 0xFF; /* Bits 0 ..  7 sec */
    L_WORD us  = in->sec >> 8;   /* Bits 8 .. 31 us  */
    return mil_hw_irig_set(p_api_dev, in->day, in->hour, in->min, sec, us);
}



L_WORD api_sys_irig_source_set( TY_API_DEV *p_api_dev, TY_MIL_COM_SYS_IRIG_SOURCE_SET_INPUT * in, TY_MIL_COM_ACK * out )
{
    return mil_hw_irig_set_external(p_api_dev, (BYTE)in->source );
}






/*****************************************************************************/
/*                                                                           */
/*   Module : API_SYS                  Submodule : API_SYS                   */
/*                                                                           */
/*   Author : Marco Motz               Project   : API1553-DS                */
/*                                                                           */
/*   Source : C                        Tools     : PC/AT; Norton Editor;     */
/*                                                 CYGNUS, GNU-C, As, and LD */
/*                                                 IDT-C 5.1 Toolkit         */
/*---------------------------------------------------------------------------*/
/*   Create : 01.05.98   Update : 11.05.09                                   */
/*---------------------------------------------------------------------------*/
/*    Descriptions                                                           */
/*    ------------                                                           */
/*    Inputs    : Instruction identifier [cmd],                              */
/*                Instruction size [size],                                   */
/*                Instruction parameter list [info_p]                        */
/*                                                                           */
/*    Outputs   : Instruction acknowledge type [ackfl]                       */
/*                                                                           */
/*    Description :                                                          */
/*    Entry function for the API1553-DS System related operations.           */
/*    This function is utilized to decode the instruction information and    */
/*    to activate the appropriate handler routine. After execution of the    */
/*    instruction acknowledge information is returned. Instructions:         */
/*                                                                           */
/*    - API_SYS_RESET:         Reset Command                                 */
/*    - API_READ_SW_VERS:      Read Software Version Command                 */
/*    - API_LOAD_SREC:         Load S-Record Command                         */
/*    - API_PROG_FLASH:        Program Flash Prom Command                    */
/*    - API_SYS_COUNTER_SET:                                                 */
/*    - API_SYS_COUNTER_GET:                                                 */
/*                                                                           */
/*****************************************************************************/

BYTE api_sys(TY_API_DEV *p_api_dev, L_WORD cmd, WORD size, BYTE *info_p)
{
  BYTE ackfl, cnt;

  ackfl = API_NAK;

  switch(cmd)
  {
    case API_RESET:
      size -= 3; /* CMD size incremented from 1 to 4  */

      if(size == 5)
        ackfl = api_sys_reset_legacy(p_api_dev,  *info_p);
      else
        PRINTF2("ERROR: API_RESET size exp: %x det: %x \n\r", 5, size);

      p_api_dev->reset_can_be_done = FALSE;
      break;

    case API_READ_SW_VERS:
      size -= 3; /* CMD size incremented from 1 to 4  */

      if(size == 4)
        ackfl = api_sys_read_sw_vers_legacy(p_api_dev);
      break;

    case API_LOAD_SREC:
      size -= 3; /* CMD size incremented from 1 to 4  */

      cnt = *(info_p + 1);
      if(size == (10+cnt) )
        ackfl = api_sys_load_srec_legacy(p_api_dev,  *info_p, cnt, MAKE_B2LWORD(*(info_p + 2), *(info_p + 3), *(info_p + 4), *(info_p + 5) ), info_p + 6);
      break;

    case API_PROG_FLASH:
      size -= 3; /* CMD size incremented from 1 to 4  */

      if(size == 16)
        ackfl = api_sys_prog_flash_legacy(p_api_dev, MAKE_B2LWORD(*(info_p + 0), *(info_p + 1), *(info_p + 2), *(info_p + 3) ), MAKE_B2LWORD(*(info_p + 4), *(info_p + 5), *(info_p + 6), *(info_p + 7) ), MAKE_B2LWORD(*(info_p + 8), *(info_p + 9), *(info_p + 10), *(info_p + 11) ) );
      break;

    case API_SYS_GET_MEM_PARTITION:
      size -= 3; /* CMD size incremented from 1 to 4  */

      if(size == 5)
        ackfl = api_sys_get_mem_partition_legacy(p_api_dev,  *info_p);
      break;
    case API_SYS_UTIL:
      size -= 3; /* CMD size incremented from 1 to 4  */

      ackfl = api_sys_util_legacy(p_api_dev,  *info_p, (info_p + 1) );
      break;

    default:
      ackfl = api_sys_dts( p_api_dev, cmd, size, info_p);
      break;
  }

  return (ackfl);

} /* end: api_sys */









/* -- Functions for new host to target communication --- */



/**********************************************************************
 * @Function: api_sys_ping
 * @Descr:    This function is used to check the new host to target
 *            communication mechanism.
 *
 * @Author:   Martin Haag
 ***********************************************************************/
L_WORD api_sys_ping( TY_API_DEV *p_api_dev, TY_MIL_COM_SYS_PING_INPUT * pxInput, TY_MIL_COM_SYS_PING_OUTPUT * pxOutput )
{
  L_WORD ulIndex   = 0;
  L_WORD ulAckSize = 0;

  /* -- calculate required output buffer size --- */

  ulAckSize = sizeof(TY_MIL_COM_ACK) + (pxInput->ulDataCount + 1) * sizeof(L_WORD);

  /* -- we have a dynamic ack size so we have to check it here --- */

  if( ulAckSize > pxInput->xCommand.ulExpectedAckSize )
  {
    return API_ERR_WRONG_ACK_SIZE;
  }

  /* -- get all input items and return them in reversed order --- */

  pxOutput->ulDataCount = pxInput->ulDataCount;

  for( ulIndex = 0; ulIndex < pxInput->ulDataCount; ulIndex++ )
  {
    pxOutput->aulData[ pxOutput->ulDataCount - ulIndex - 1 ] = pxInput->aulData[ulIndex];
  }


  /* -- set ack size --- */
  pxOutput->xAck.ulSize = ulAckSize;

  return 0;
}


/**********************************************************************
 * @Function: api_sys_malloc
 * @Descr:    This function can be used to allocate a block of memory
 *            that can then be used by the host. This command is 
 *            used to allocate memory for the SRE download during the
 *            AEC update.
 *
 * @Input: ulMemory => Memory type (local/shared/global)
 *         ulSize   => Size of block in bytes
 *         ulTag    => Name of caller for debug purpose ( e.g. 'SRE1' )
 *
 * @Output: ulMemIdentity => Uniqe identifier of the allocated memory block
 *
 * @Author:   Martin Haag
 ***********************************************************************/
L_WORD api_sys_malloc( TY_API_DEV *p_api_dev, TY_MIL_COM_SYS_MALLOC_INPUT * cmd_p, TY_MIL_COM_SYS_MALLOC_OUTPUT * ack_p )
{

  /* -- set ack size --- */
  ack_p->xAck.ulSize = sizeof(TY_MIL_COM_SYS_MALLOC_OUTPUT);

  /* 1.) check if memory location is free             */
  /* 2.) allocate memory                              */
  /* 3.) store input struct in global memory manager  */
  /* 4.) return memory identifier                     */

  /* 0 = global; 1 = shared; 2 = local; */

  if( cmd_p->ulMemory != 0 )
      return API_ERR_MEMORY_INVALID;

  /* This command is currently only used for the AEC and Artix7 update */
  
  if (p_api_dev->ul_DeviceGroup == AI_DEVICE_AYE )
  {
      /* If global memory is requested we do always return offset 16MB */
      /* The TSW does only use 16MB but the memory bar has 32MB        */

      if (cmd_p->ulSize > 0x01000000)
          return API_ERR_NO_SPACE_LEFT;

      ack_p->ulOffset = 0x01000000;
  }
  else if(   (p_api_dev->ul_DeviceGroup == AI_DEVICE_ARTIX7)
          || (p_api_dev->ul_DeviceGroup == AI_DEVICE_ARTIXUS))
  {
      /* If global memory is requested we do always return offset BIU0 BM area 
         with a size of 512KB. Make sure the BIU0 is disabled during the update. 
       */
      if (cmd_p->ulSize > 0x00080000)
          return API_ERR_NO_SPACE_LEFT;

      if( p_api_dev->bm_status[0] == API_BUSY )
          return API_ERR_INVALID_DEVICE_STATE;

      ack_p->ulOffset = ul_ConvertTSWPtrToGRAMPtr(p_api_dev, 0, p_api_dev->glb_mem.biu[0].base.bm_buf);
  }
  else
  {
      return API_ERR_CMD_NOT_FOUND;
  }



  return 0;
}


/**********************************************************************
 * @Function: api_sys_free
 * @Descr:    This function is used to free a block of memory which was
 *            allocated with the api_sys_malloc function.
 *
 * @Input: ulMemIdentity => Uniqe identifier of the allocated memory block
 *         ulTag         => Name of caller for debug purpose ( e.g. 'SRE1' )
 *
 * @Author:   Martin Haag
 ***********************************************************************/
L_WORD api_sys_free( TY_API_DEV *p_api_dev, TY_MIL_COM_SYS_FREE_INPUT * cmd_p, TY_MIL_COM_ACK * ack_p )
{
    if (p_api_dev->ul_DeviceGroup == AI_DEVICE_AYE)
    {
        /* nothing to do */
        return 0;
    }
    else if ( (p_api_dev->ul_DeviceGroup == AI_DEVICE_ARTIX7) 
           || (p_api_dev->ul_DeviceGroup == AI_DEVICE_ARTIXUS) )
    {
        /* nothing to do */
        return 0;
    }
    else
    {
        return API_ERR_CMD_NOT_FOUND;
    }

    return 0;
}



static AiBoolean isDbtePbi(TY_API_DEV *p_api_dev, short ls_biu)
{
    /* If DBTE PBI (0x1F)*/

    if ((((*p_api_dev->cb_p[ls_biu]).cid & 0x00ff0000) >> 16) == 0x1F)
        return AiTrue;
    else
        return AiFalse;
}

/**********************************************************************
 * @Function: api_sys_get_board_info
 * @Descr:    This function is used to get information about the board, that
 *            is already known to the target software. With ulDataStart and
 *            ulDataCount you can read some or all data available. Also we
 *            avoid incompatibities if additional data is added to this command.
 *
 * @Input: ulStream    => Id of the stream (this may affect e.g. ulGlobalRamStartOffset)
 *         ulDataStart => Index of the first requested AiUInt32
 *         ulDataCount => number of AiUInt32 requested
 *
 * @Output: ulDataCount => ammount of AiUInt32 that is really returned
 *          aulData     => the board information
 *
 * @Author:   Jochen Pfaadt
 ***********************************************************************/
L_WORD api_sys_get_board_info( TY_API_DEV *p_api_dev, TY_MIL_COM_SYS_GET_INFO_INPUT * cmd_p, TY_MIL_COM_SYS_GET_INFO_OUTPUT * ack_p )
{
  L_WORD i = 0;
  short  ulCurrentBiu            = 0;
  L_WORD ulGlobalRamStartOffset  = 0;
  L_WORD ulCanChangeAmplitude    = 0, ulCanChangeAmplitudePacked = 0;
  L_WORD ulCanChangeAmplitudeHighRes = 0;
  L_WORD ulDiscreteCount         = 0;
  L_WORD ulDiscreteConfig        = 0;
  L_WORD ulIsMultiChannel        = 0;
  L_WORD ulIsHsRedundant         = 0;
  L_WORD ulBoardInfo[TY_BOARD_INFO_MAX];
  L_WORD physical_biu;
  L_WORD protocol;

  TY_IR_CAPABILITIES             xIRCapabilities;
  
  xIRCapabilities.ul_All         = 0;

  mil_hw_convert_stream_to_LogicalBiu( p_api_dev, cmd_p->xCommand.ulStream - 1, 0, &ulCurrentBiu, NULL );

  /* collect info that is not easily available */
  mil_hw_can_change_amplitude(p_api_dev, ulCurrentBiu, &ulCanChangeAmplitude, &ulCanChangeAmplitudePacked);
  mil_hw_can_change_amplitude_high_res(p_api_dev, ulCurrentBiu, &ulCanChangeAmplitudeHighRes);
  /*  mil_hw_get_irig_capabilities    ( p_api_dev, &xIrigCapabilities );                   */
/*  mil_hw_get_coupling_capabilities( p_api_dev, ulCurrentBiu, &xCouplingCapabilities ); */
  mil_hw_get_GlobalRamStartOffset ( p_api_dev, ulCurrentBiu, &ulGlobalRamStartOffset);
  mil_hw_get_discretes            ( p_api_dev, &ulDiscreteCount, &ulDiscreteConfig);
  mil_hw_get_ir_capabilities      ( p_api_dev, ulCurrentBiu, &xIRCapabilities);
  mil_hw_get_is_hs_redundant      ( p_api_dev, ulCurrentBiu, &ulIsHsRedundant);

  ulIsMultiChannel = ul_StreamIsMultiChannel(p_api_dev, ulCurrentBiu);

  physical_biu = api_sys_convert_biu_logical_to_physical( p_api_dev, p_api_dev->biu );

  if (ul_StreamIs3910(p_api_dev, p_api_dev->biu ))
      protocol = 2; /* 3910 protocol */
  else if (ul_StreamIsEFEX(p_api_dev, p_api_dev->biu))
      protocol = 3; /* EFEX protocol */
  else
      protocol = 1; /* 1553 */


  /* now put all information into the return array */
  ulBoardInfo[TY_BOARD_INFO_DEVICE_TYPE]                = p_api_dev->uc_DeviceType;
  ulBoardInfo[TY_BOARD_INFO_CHANNEL_COUNT]              = uc_CalcNumberOfStreams(p_api_dev);
  ulBoardInfo[TY_BOARD_INFO_BIU_COUNT]                  = uc_CalcNumberOfPhysicalBius(p_api_dev);
  ulBoardInfo[TY_BOARD_INFO_BOARD_TYPE]                 = p_api_dev->ini_info.board_type;
  ulBoardInfo[TY_BOARD_INFO_BOARD_CONFIG]               = p_api_dev->ini_info.board_config;
  ulBoardInfo[TY_BOARD_INFO_SERIAL]                     = p_api_dev->BoardSerial;
  ulBoardInfo[TY_BOARD_INFO_PARTNO]                     = p_api_dev->BoardPartNo;
  ulBoardInfo[TY_BOARD_INFO_SIZE_GLOBAL]                = p_api_dev->GlobalRAMSize;
  ulBoardInfo[TY_BOARD_INFO_SIZE_SHARED]                = p_api_dev->SharedRAMSize;
  ulBoardInfo[TY_BOARD_INFO_OFFS_GLOBAL]                = ulGlobalRamStartOffset;
  ulBoardInfo[TY_BOARD_INFO_CHANGE_AMPL]                = ulCanChangeAmplitude;
  ulBoardInfo[TY_BOARD_INFO_COUPLING]                   = p_api_dev->x_CouplingCapabilities.ul_All;
  ulBoardInfo[TY_BOARD_INFO_IRIG]                       = p_api_dev->x_IrigCapabilities.ul_All;
  ulBoardInfo[TY_BOARD_INFO_DISCRETE_CNT]               = ulDiscreteCount;
  ulBoardInfo[TY_BOARD_INFO_DISCRETE_CONFIG]            = ulDiscreteConfig;
  ulBoardInfo[TY_BOARD_INFO_IR]                         = xIRCapabilities.ul_All;
  ulBoardInfo[TY_BOARD_INFO_IS_MULTI_CHANNEL]           = ulIsMultiChannel;
  ulBoardInfo[TY_BOARD_INFO_IS_HS_REDUNDANT]            = ulIsHsRedundant;
  ulBoardInfo[TY_BOARD_INFO_CAN_HIGH_RES_ZERO_CROSSING] = mil_hw_is_high_res_zero_crossing_supported(p_api_dev);
  ulBoardInfo[TY_BOARD_INFO_HAS_ELECTRICAL_INTERFACE]   = hasElectricalInterface(p_api_dev, p_api_dev->biu);
  ulBoardInfo[TY_BOARD_INFO_MILSCOPE_TYPE]              = mil_hw_scope_type_get(p_api_dev);
  ulBoardInfo[TY_BOARD_INFO_IS_DBTE]                    = isDbtePbi(p_api_dev, p_api_dev->biu);
  ulBoardInfo[TY_BOARD_INFO_HAS_EXTERNAL_IRIG]          = mil_hw_irig_has_external_input(p_api_dev);
  ulBoardInfo[TY_BOARD_INFO_HAS_EXTERNAL_TRIGGER]       = mil_hw_trigger_has_external_input(p_api_dev);
  ulBoardInfo[TY_BOARD_INFO_PROTOCOL]                   = protocol;
  ulBoardInfo[TY_BOARD_INFO_APPLICATION_TYPE]           = (physical_biu<2)? p_api_dev->ini_info.bt[physical_biu] : p_api_dev->ini_info.bt2[physical_biu-2];
  ulBoardInfo[TY_BOARD_INFO_CHANGE_AMPL_HIGH_RES]       = ulCanChangeAmplitudeHighRes;
  ulBoardInfo[TY_BOARD_INFO_CHANGE_AMPL_PACKED]         = ulCanChangeAmplitudePacked;
  ulBoardInfo[TY_BOARD_INFO_CAN_OVERRIDE_TRAFFIC]       = mil_fw_is_cmd_supported(p_api_dev, MIL_FW_CMD_BC_TRAFFIC_OVERRIDE) ? 1 : 0;
  ulBoardInfo[TY_BOARD_INFO_HAS_FAST_GAP]               = mil_fw_is_cmd_supported(p_api_dev, MIL_FW_CMD_BC_FAST_GAPMODE) ? 1 : 0;
  ulBoardInfo[TY_BOARD_INFO_HAS_LOW_PRIORITY_LIST]      = mil_fw_is_cmd_supported(p_api_dev, MIL_FW_CMD_BC_LOW_PRIORITY_LIST) ? 1 : 0;
  ulBoardInfo[TY_BOARD_INFO_HAS_BC_INSTR_RETURN_ON_XFER_ERROR] = mil_fw_is_cmd_supported(p_api_dev, MIL_FW_CMD_BC_INSTR_RETURN_ON_XFER_ERROR) ? 1 : 0;
  ulBoardInfo[TY_BOARD_INFO_USB_AIM_POWER_CLASS] = mil_hw_usb_aim_power_class(p_api_dev);
  ulBoardInfo[TY_BOARD_INFO_HAS_DIGITAL_TRIGGER_LOOP] = mil_hw_trigger_has_digital_loop(p_api_dev);

  
  

  /* provide board information, as requested with ulDataStart and ulDataCount*/
  for(i=0; i<cmd_p->ulDataCount; i++)
  {
    if ((i+cmd_p->ulDataStart)>=TY_BOARD_INFO_MAX) break;

    ack_p->aulData[i] = ulBoardInfo[i + cmd_p->ulDataStart];
  }

  ack_p->ulDataCount = i;

  /* -- set ack size --- */
  if (ack_p->ulDataCount==0)
      ack_p->xAck.ulSize = sizeof(TY_MIL_COM_SYS_GET_INFO_OUTPUT);
  else
      ack_p->xAck.ulSize = sizeof(TY_MIL_COM_SYS_GET_INFO_OUTPUT) + (ack_p->ulDataCount - 1) * sizeof(L_WORD);


  return 0;
}


L_WORD api_sys_get_mem_loc( TY_API_DEV *p_api_dev, TY_MIL_COM_SYS_GET_MEM_LOCATION_INPUT * cmd_p, TY_MIL_COM_SYS_GET_MEM_LOCATION_OUTPUT * ack_p )
{
  short ulBiu;
  mil_hw_convert_stream_to_LogicalBiu( p_api_dev, cmd_p->xCommand.ulStream - 1, 0, &ulBiu, NULL );

  /* -- set ack size --- */
  ack_p->xAck.ulSize = sizeof(TY_MIL_COM_SYS_GET_MEM_LOCATION_OUTPUT);




  switch( cmd_p->ulMode )
  {
  case TY_GETMEM_RT_DESCR:
    ack_p->ulMemType = 0;
    ack_p->ulOffset  = API_GLB_MEM_ADDR_REL(&(*p_api_dev->rt_desc_p[ulBiu])[cmd_p->ulId].rt_ctrl);
    break;
  default:
    ack_p->ulMemType = 0;
    ack_p->ulOffset  = 0;
    return API_ERR_INVALID_MODE;
  }



  return 0;
}


/*
 * convert physical biu to logical biu
 * In case of multichannel this funciton will return the higher level logical biu
 * */
BYTE api_sys_convert_biu_physical_to_logical( TY_API_DEV * p_api_dev, short physical_biu )
{
    int i;
    BYTE logical_biu = 0;

    for( i=0; i<MAX_PHY_BIU; i++ )
    {
      switch( (p_api_dev->BoardType >> (i*8) ) & 0x000000F0L)
      {
        case 0x00000010:
        case 0x00000030:
        case 0x00000070:
            logical_biu += 1;
          break;
        case 0x000000A0:
            logical_biu += 2;
          break;
        default:
          break;
      }

      if( i == physical_biu )
          break;
    }


    return logical_biu - 1;
}


short api_sys_convert_biu_logical_to_physical( TY_API_DEV * p_api_dev, short logical_biu )
{
    L_WORD bt;
    short i;
    short current_logical_biu = 0;

    for(i=0; i<MAX_PHY_BIU; i++ )
    {
        bt = ( (p_api_dev->BoardType >> (i*8) ) & 0x000000F0);

        if     ( bt == 0x10) current_logical_biu += 1;
        else if( bt == 0x30) current_logical_biu += 1;
        else if( bt == 0x70) current_logical_biu += 1;
        else if( bt == 0xA0) current_logical_biu += 2;
        else
          /* We should never get here. 
             However in case of an invalid NOVRAM we want to avoid having out of range indices later on. */
          break;

        if (current_logical_biu > logical_biu)
            break;
    }

    return i;
}

NAMESPACE1553_END


/*** end of file ***/

