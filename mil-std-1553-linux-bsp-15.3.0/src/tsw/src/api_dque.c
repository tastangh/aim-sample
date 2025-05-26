/* SPDX-FileCopyrightText: 2008-2022 AIM GmbH <info@aim-online.com> */
/* SPDX-License-Identifier: MIT OR GPL-2.0-or-later */

/*! \file api_dqueue.c
    Data Queue module of the 1553 ASP Driver SW.
*/

#include "mil_tsw_includes.h"






NAMESPACE1553_START

#define API_DATAQUEUE_HEADER(x) ((TY_API_DATA_QUEUE_HEADER *)API_SHARED_MEM_ADDR_ABS(p_api_dev->dataqueue_header_offsets[x]));


TY_MIL_COM_TABLE_ENTRY axTargetLsDataQueueCommands[] = {
   /* LS Dataqueue Functions */
  {MIL_COM_LS_BM_DATAQUEUE_OPEN,    (TY_MIL_COM_FUNC_PTR)api_data_queue_open,     "api_data_queue_open",    sizeof(TY_MIL_COM_LS_BM_DATAQUEUE_OPEN_INPUT),    sizeof(TY_MIL_COM_LS_BM_DATAQUEUE_OPEN_OUTPUT)  },
  {MIL_COM_LS_BM_DATAQUEUE_CONTROL, (TY_MIL_COM_FUNC_PTR)api_data_queue_control,  "api_data_queue_control", sizeof(TY_MIL_COM_LS_BM_DATAQUEUE_CONTROL_INPUT), sizeof(TY_MIL_COM_ACK)  },
  {MIL_COM_LS_BM_DATAQUEUE_CLOSE,   (TY_MIL_COM_FUNC_PTR)api_data_queue_close,    "api_data_queue_close",   sizeof(TY_MIL_COM_LS_BM_DATAQUEUE_CLOSE_INPUT),   sizeof(TY_MIL_COM_ACK)  },

};


void api_ls_bm_dataqueue_command_table( TY_MIL_COM_TABLE_ENTRY ** table, AiUInt32 * size )
{
    *size = sizeof(axTargetLsDataQueueCommands) / sizeof(TY_MIL_COM_TABLE_ENTRY);
    *table = axTargetLsDataQueueCommands;
}





static BYTE uc_ApiDataQueueIsHs(TY_API_DEV *p_api_dev, L_WORD uc_Id)
{
  BYTE uc_DataQueueIsHs = 0;

  switch(p_api_dev->uc_DeviceType )
  {
    case TYPE_APX3910:
    case TYPE_APX3910Xp:
    case TYPE_ACX3910:
    case TYPE_ACX3910Xp:
    case TYPE_AVX3910:
    case TYPE_AVX3910Xp:
    case TYPE_ANET3910:
    case TYPE_ANET3910Xp:
    case TYPE_APXX3910:
    case TYPE_APXX3910Xp:
    case TYPE_APEX3910:
    case TYPE_APEX3910Xp:
    case TYPE_ACE3910:
    case TYPE_ACE3910Xp:
    case TYPE_AXC3910:
    case TYPE_AXC3910Xp:
      if( API_DATA_QUEUE_ID_BM_REC_BIU2 == uc_Id )
        uc_DataQueueIsHs = 1;
      break;

    case TYPE_ACX3910_2:
    case TYPE_ACX3910Xp_2:
    case TYPE_AVX3910_2:
    case TYPE_AVX3910Xp_2:
      if( API_DATA_QUEUE_ID_BM_REC_BIU2 == uc_Id )
        uc_DataQueueIsHs = 1;
      else if( API_DATA_QUEUE_ID_BM_REC_BIU4 == uc_Id )
        uc_DataQueueIsHs = 1;
      break;

    case TYPE_ACX_EFA_1_TWO_PBIS:      
    case TYPE_ACX_EFAXp_1_TWO_PBIS:    
    case TYPE_ACX_EFA_1_DS_TWO_PBIS:   
    case TYPE_ACX_EFAXp_1_DS_TWO_PBIS: 
    case TYPE_AVX_EFA_1_TWO_PBIS:      
    case TYPE_AVX_EFAXp_1_TWO_PBIS:    
    case TYPE_AVX_EFA_1_DS_TWO_PBIS:   
    case TYPE_AVX_EFAXp_1_DS_TWO_PBIS: 
      if( API_DATA_QUEUE_ID_BM_REC_BIU3 == uc_Id )
        uc_DataQueueIsHs = 1;
      break;

    case TYPE_ACX_EFA_2_TWO_PBIS:      
    case TYPE_ACX_EFAXp_2_TWO_PBIS:   
    case TYPE_ACX_EFA_2_DS_TWO_PBIS:   
    case TYPE_ACX_EFAXp_2_DS_TWO_PBIS: 
    case TYPE_AVX_EFA_2_TWO_PBIS:      
    case TYPE_AVX_EFAXp_2_TWO_PBIS:   
    case TYPE_AVX_EFA_2_DS_TWO_PBIS:   
    case TYPE_AVX_EFAXp_2_DS_TWO_PBIS: 
      if( API_DATA_QUEUE_ID_BM_REC_BIU4 == uc_Id )
        uc_DataQueueIsHs = 1;
      break;

    case TYPE_ACX_EFA_4_TWO_PBIS:      
    case TYPE_ACX_EFAXp_4_TWO_PBIS:   
    case TYPE_AVX_EFA_4_TWO_PBIS:      
    case TYPE_AVX_EFAXp_4_TWO_PBIS:   
      if( API_DATA_QUEUE_ID_BM_REC_BIU6 == uc_Id )
        uc_DataQueueIsHs = 1;
      break;
  }

  return uc_DataQueueIsHs;
}

L_WORD ul_ApiDataQueueMemAddr(TY_API_DEV *p_api_dev, L_WORD id, short biu)
{
  L_WORD data_queue_offset;

  data_queue_offset = API_DATA_QUEUE_START;

  if(   (id == API_DATA_QUEUE_ID_MIL_SCOPE)
     || (id == API_DATA_QUEUE_ID_GENERIC_ACQ) )
  {
      /* We support only milscope or generic acq but not both */
      /* This queue is located at the end of the 11MB Data Queue Area
       * and has a maximum size of 1MB
       * */
      data_queue_offset += (10 * API_DATA_QUEUE_SIZE_LS);
      return data_queue_offset;
  }

  switch(p_api_dev->uc_DeviceType )
  {
    case TYPE_ACX3910_2:
    case TYPE_ACX3910Xp_2:
    case TYPE_AVX3910_2:
    case TYPE_AVX3910Xp_2:
        /* We have one HS stream in between so we need to adjust the calculation */
        if( biu < 2 )
            data_queue_offset += (API_DATA_QUEUE_SIZE_LS * biu);
        else
            data_queue_offset += ((API_DATA_QUEUE_SIZE_LS * (biu-2)) + (API_DATA_QUEUE_SIZE_LS+ API_DATA_QUEUE_SIZE_HS) ) ;
      break;


    default:
        /* The HS stream, if present, is always the last one */
        data_queue_offset += (API_DATA_QUEUE_SIZE_LS * biu);
        break;
  }

  return data_queue_offset;
}






static AiUInt8 api_data_queue_open_get_biu(L_WORD ucId)
{
    AiUInt8 ucBiu = 0;

    /* BM Recording */
    if      (ucId == API_DATA_QUEUE_ID_BM_REC_BIU1)    ucBiu = 0;
    else if (ucId == API_DATA_QUEUE_ID_BM_REC_BIU2)    ucBiu = 1;
    else if (ucId == API_DATA_QUEUE_ID_BM_REC_BIU3)    ucBiu = 2;
    else if (ucId == API_DATA_QUEUE_ID_BM_REC_BIU4)    ucBiu = 3;
    else if (ucId == API_DATA_QUEUE_ID_BM_REC_BIU5)    ucBiu = 4;
    else if (ucId == API_DATA_QUEUE_ID_BM_REC_BIU6)    ucBiu = 5;
    else if (ucId == API_DATA_QUEUE_ID_BM_REC_BIU7)    ucBiu = 6;
    else if (ucId == API_DATA_QUEUE_ID_BM_REC_BIU8)    ucBiu = 7;
    /* Milscope */
    else if (ucId == API_DATA_QUEUE_ID_MIL_SCOPE)      ucBiu = 0;
    else if (ucId == API_DATA_QUEUE_ID_GENERIC_ACQ)    ucBiu = 0;

    return ucBiu;
}



/*****************************************************************************/
/*                                                                           */
/*   Module : API_DQUE                 Submodule : API_DATA_QUEUE_OPEN       */
/*                                                                           */
/*   Author : Thomas Jahn              Project   : API1553-DS                */
/*                                                                           */
/*   Source : C                        Tools     : PC/AT; Norton Editor;     */
/*                                                 CYGNUS, GNU-C, As, and LD */
/*                                                 IDT-C 5.1 Toolkit         */
/*---------------------------------------------------------------------------*/
/*   Create : 17.12.2002  Update : 11.05.09                                  */
/*---------------------------------------------------------------------------*/
/*    Descriptions                                                           */
/*    ------------                                                           */
/*    Inputs    : Queue Id [uc_Id]                                           */
/*                Target Size [uw_ASPQueueSize]                              */
/*    Outputs   : Instruction acknowledge type [ackfl]                       */
/*                                                                           */
/*    Description :                                                          */
/*    This function handles the 'API_DATA_QUEUE_OPEN' instruction to create  */
/*    a Queue.                                                               */
/*                                                                           */
/*****************************************************************************/
L_WORD api_data_queue_open(TY_API_DEV *p_api_dev, TY_MIL_COM_LS_BM_DATAQUEUE_OPEN_INPUT * in, TY_MIL_COM_LS_BM_DATAQUEUE_OPEN_OUTPUT * out)
{
    TY_API_DATA_QUEUE_HEADER * pDataQueueHeader;
    short biu;

    if (in->id > API_MAX_DATA_QUEUE_ID)
        return API_ERR_DQUEUE_ID_NOT_IN_RANGE;

    if (uc_ApiDataQueueIsHs(p_api_dev, in->id))        out->dataqueue_size = API_DATA_QUEUE_DATA_SIZE_HS;
    else if (in->id == API_DATA_QUEUE_ID_MIL_SCOPE)    out->dataqueue_size = API_DATA_QUEUE_DATA_SIZE_AYX_MS;
    else if (in->id == API_DATA_QUEUE_ID_GENERIC_ACQ)  out->dataqueue_size = API_DATA_QUEUE_DATA_SIZE_LS;
    else                                               out->dataqueue_size = API_DATA_QUEUE_DATA_SIZE_LS;


    biu = api_data_queue_open_get_biu(in->id);

    /* Set offset to Queue Control structure in SHARED */
    p_api_dev->dataqueue_header_offsets[in->id] = ul_ApiDataQueueMemAddr(p_api_dev, (BYTE)in->id, biu);

    pDataQueueHeader = API_DATAQUEUE_HEADER(in->id);

    memset( (void*)pDataQueueHeader, 0, sizeof(TY_API_DATA_QUEUE_HEADER));

    pDataQueueHeader->status     = 0L;
    pDataQueueHeader->data_start = p_api_dev->dataqueue_header_offsets[in->id] + API_DATA_QUEUE_DATA_OFFSET;
    pDataQueueHeader->data_size  = out->dataqueue_size;
    pDataQueueHeader->id         = (BYTE)in->id;
    pDataQueueHeader->put        = pDataQueueHeader->data_start; /* put==get => queue is empty */
    pDataQueueHeader->get        = pDataQueueHeader->data_start; 
    pDataQueueHeader->biu        = (BYTE)biu;
    pDataQueueHeader->hs_stream  = GET_HS_STREAM(pDataQueueHeader->biu);

    wbflush();

    out->dataqueue_offset = p_api_dev->dataqueue_header_offsets[in->id];

    if( p_api_dev->ul_DbgLevel & DBG_DQUEUE )
    {
        PRINTF1("pDataQueueHeader->status     = 0x%08X\r\n", pDataQueueHeader->status);
        PRINTF1("pDataQueueHeader->data_start = 0x%08X\r\n", pDataQueueHeader->data_start);
        PRINTF1("pDataQueueHeader->data_size  = 0x%08X\r\n", pDataQueueHeader->data_size);
        PRINTF1("pDataQueueHeader->id         = 0x%08X\r\n", pDataQueueHeader->id);
        PRINTF1("pDataQueueHeader->put        = 0x%08X\r\n", pDataQueueHeader->put);
        PRINTF1("pDataQueueHeader->get        = 0x%08X\r\n", pDataQueueHeader->get);
        PRINTF1("pDataQueueHeader->biu        = 0x%08X\r\n", pDataQueueHeader->biu);
        PRINTF1("pDataQueueHeader->hs_stream  = 0x%08X\r\n", pDataQueueHeader->hs_stream);
    }


#if defined(_NUCLEUS)
    if(API_DATA_QUEUE_ID_MIL_SCOPE == in->id)
        mscope_init( p_api_dev );
#endif

    return 0;
} /* end: api_data_queue_open */

/*****************************************************************************/
/*                                                                           */
/*   Module : API_DQUE                 Submodule : API_DATA_QUEUE_CONTROL    */
/*                                                                           */
/*   Author : Thomas Jahn              Project   : API1553-DS                */
/*                                                                           */
/*   Source : C                        Tools     : PC/AT; Norton Editor;     */
/*                                                 CYGNUS, GNU-C, As, and LD */
/*                                                 IDT-C 5.1 Toolkit         */
/*---------------------------------------------------------------------------*/
/*   Create : 17.12.2002   Update :                                          */
/*---------------------------------------------------------------------------*/
/*    Descriptions                                                           */
/*    ------------                                                           */
/*    Inputs    : Queue ID [uc_Id]                                           */
/*                Mode [uc_Mode]                                             */
/*                                                                           */
/*    Outputs   : Instruction acknowledge type [ackfl]                       */
/*                                                                           */
/*    Description :                                                          */
/*    This function handles the 'API_DATA_QUEUE_CONTROL' instruction         */
/*                                                                           */
/*****************************************************************************/

L_WORD api_data_queue_control(TY_API_DEV *p_api_dev, TY_MIL_COM_LS_BM_DATAQUEUE_CONTROL_INPUT * in, TY_MIL_COM_ACK * out)
{
#if defined(_NUCLEUS)
    L_WORD ret;
    L_WORD volatile *ptr;
#endif    
    L_WORD tmpl;
    TY_API_DATA_QUEUE_HEADER * pDataQueueHeader;

    if( in->id > API_MAX_DATA_QUEUE_ID )
        /* ID out of range */
        return API_ERR_INVALID_ID;

    if( p_api_dev->dataqueue_header_offsets[in->id] == 0 )
    {
        if( in->mode == API_DATA_QUEUE_CTRL_START )
        {
            /* DQ not open and can not be started */
            return API_ERR_BUFFER_NOT_ALLOCATED;
        }
        else
        {
            /* DQ not open, ignore command */
            return API_OK;
        }
    }

    pDataQueueHeader = API_DATAQUEUE_HEADER(in->id);


    switch (in->mode)
    {
    case API_DATA_QUEUE_CTRL_START:
        /* Get Data Buffer Pointer in Global Memory */

        pDataQueueHeader->triggered = 0;
        pDataQueueHeader->first_read = 0;


        if (ul_StreamIsHS(p_api_dev, pDataQueueHeader->biu) && ((pDataQueueHeader->id == 1) || (pDataQueueHeader->id == 3)))
        {
            tmpl = BSWAP32((*px_api39_cb_p[p_api_dev->hs_stream]).ul_scw);
            LWORD_RESET_BITS(tmpl, 10, 8);
            LWORD_SET_BITS(tmpl, (api39_sys_mem[p_api_dev->hs_stream].uc_m39_bufs - 1), 10, 8);
            (*px_api39_cb_p[p_api_dev->hs_stream]).ul_scw = BSWAP32(tmpl);

            wbflush();
        }

        pDataQueueHeader->put = pDataQueueHeader->data_start; /* put==get => queue is empty */;
        pDataQueueHeader->get = pDataQueueHeader->data_start;


        tmpl = pDataQueueHeader->status;
        tmpl &= (API_DATA_QUEUE_STATUS_CAP_SINGLE | API_DATA_QUEUE_STATUS_CAP_CONT); /* keep milscope bits and reset all other bits */
        tmpl |= API_DATA_QUEUE_STATUS_START; /* set enable bit and reset all other bits */
        pDataQueueHeader->status = tmpl;
        wbflush();


#if defined(_NUCLEUS) /* MilScope functions are only available for APX cards */
        if (API_DATA_QUEUE_ID_MIL_SCOPE == pDataQueueHeader->id)
        {
            ret = mscope_start( p_api_dev, MSCOPE_START );
            if( ret != 0 )
            {
                return ret;
            }

            *(ptr + 0) = pDataQueueHeader->status;
            wbflush();
        }
#ifdef _CMPL4DTS
        if( API_DATA_QUEUE_ID_GENERIC_ACQ == pDataQueueHeader->id)
        {
            api_sys_drv_rec_con(API_ON);
            *(ptr + 0) = pDataQueueHeader->status;
            wbflush();
        }
#endif
#endif

        /* start timer that will retrieve data from firmware buffer
         * into data queue.
         */
        tmpl = mil_tasks_dataqueue_timer_start(p_api_dev);
        if (tmpl)
        {
            return API_ERR;
        }
        break;

    case API_DATA_QUEUE_CTRL_STOP:
        /* Transfer remaining data in monitor buffer to queue before actually stopping it*/
        tmpl = mil_tasks_dataqueue_process(p_api_dev, pDataQueueHeader->id);
        if (tmpl)
        {
            return API_ERR;
        }

#if defined(_NUCLEUS) /* MilScope functions are only available for APX cards */
        if (API_DATA_QUEUE_ID_MIL_SCOPE == pDataQueueHeader->id) {
            mscope_start(p_api_dev, MSCOPE_STOP );
        }  
#ifdef _CMPL4DTS
        if( API_DATA_QUEUE_ID_GENERIC_ACQ == pDataQueueHeader->id)
        {
            api_sys_drv_rec_con(API_OFF);
        }
#endif
#endif
#if !defined(_AMC1553) /* API, APX */
        if (API_DATA_QUEUE_ID_MIL_SCOPE != pDataQueueHeader->id)
        {
            api_ir_data_queue(p_api_dev, pDataQueueHeader->id);
            api_dma_Host2TargetCom(p_api_dev);
        }
#endif
        pDataQueueHeader->status &= 0x7FFFFFFF;
        wbflush();

        /* stop timer that will retrieve data from firmware buffer
         * into data queue.
         */
        tmpl = mil_tasks_dataqueue_timer_stop(p_api_dev);
        if (tmpl)
        {
            return API_ERR;
        }
        break;

    case API_DATA_QUEUE_CTRL_FLUSH:
        tmpl = mil_tasks_dataqueue_process(p_api_dev, pDataQueueHeader->id);
        if (tmpl)
        {
            return API_ERR;
        }

        pDataQueueHeader->status &= ~(API_DATA_QUEUE_STATUS_CAP_SINGLE | API_DATA_QUEUE_STATUS_CAP_CONT);
        wbflush();
        break;

    case API_DATA_QUEUE_CTRL_SUSPEND:
        pDataQueueHeader->status &= 0x7FFFFFFF; /* reset Start bit */
        pDataQueueHeader->status |= 0x40000000; /* set suspend bit */
        wbflush();
        break;


    default:
        return API_ERR_INVALID_MODE;
        }

    return 0;

    } /* end: api_data_queue_control */

/*****************************************************************************/
/*                                                                           */
/*   Module : API_DQUE                 Submodule : API_DATA_QUEUE_CLOSE      */
/*                                                                           */
/*   Author : Thomas Jahn              Project   : API1553-DS                */
/*                                                                           */
/*   Source : C                        Tools     : PC/AT; Norton Editor;     */
/*                                                 CYGNUS, GNU-C, As, and LD */
/*                                                 IDT-C 5.1 Toolkit         */
/*---------------------------------------------------------------------------*/
/*   Create : 17.12.2002   Update : 11.05.09                                 */
/*---------------------------------------------------------------------------*/
/*    Descriptions                                                           */
/*    ------------                                                           */
/*    Inputs    : Queue ID [uc_Id]                                           */
/*                                                                           */
/*    Outputs   : Instruction acknowledge type [ackfl]                       */
/*                                                                           */
/*    Description :                                                          */
/*    This function handles the 'API_DATA_QUEUE_CLOSE' instruction           */
/*                                                                           */
/*****************************************************************************/
L_WORD api_data_queue_close(TY_API_DEV *p_api_dev, TY_MIL_COM_LS_BM_DATAQUEUE_CLOSE_INPUT * in, TY_MIL_COM_ACK * out )
{
    TY_API_DATA_QUEUE_HEADER * pDataQueueHeader;

    if( in->id > API_MAX_DATA_QUEUE_ID )
        /* ID out of range */
        return API_ERR_INVALID_ID;

    if( p_api_dev->dataqueue_header_offsets[in->id] == 0 )
        /* DQ not open */
        return API_OK;

    pDataQueueHeader = API_DATAQUEUE_HEADER(in->id);

    pDataQueueHeader->status = 0;

    return 0;
} /* end: api_data_queue_close */


NAMESPACE1553_END

