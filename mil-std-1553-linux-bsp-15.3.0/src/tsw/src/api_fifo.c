/* SPDX-FileCopyrightText: 2001-2022 AIM GmbH <info@aim-online.com> */
/* SPDX-License-Identifier: MIT OR GPL-2.0-or-later */

/*! \file api_ini.c
    System Control module of the 1553 ASP Driver SW. 
    This module comprises the handler routines for the System Control    
    instructions.
*/

#include "mil_tsw_includes.h"



NAMESPACE1553_START




TY_MIL_COM_TABLE_ENTRY axTargetSysFifoCommands[] = {
  {MIL_COM_SYS_FIFO_INI,      (TY_MIL_COM_FUNC_PTR)api_sys_ini_fifo,         "api_sys_ini_fifo",         sizeof(TY_MIL_COM_SYS_FIFO_INI_INPUT),       sizeof(TY_MIL_COM_ACK)                      },
  {MIL_COM_SYS_FIFO_WRITE,    (TY_MIL_COM_FUNC_PTR)api_buf_fifo_write,       "api_buf_fifo_write",       sizeof(TY_MIL_COM_SYS_FIFO_WRITE_INPUT),     sizeof(TY_MIL_COM_ACK)                      },
  {MIL_COM_SYS_FIFO_STATUS,   (TY_MIL_COM_FUNC_PTR)api_buf_fifo_status_read, "api_buf_fifo_status_read", sizeof(TY_MIL_COM_SYS_FIFO_STATUS_INPUT),    sizeof(TY_MIL_COM_SYS_FIFO_STATUS_OUTPUT)   },
  {MIL_COM_SYS_FIFO_RT_ASSIGN,(TY_MIL_COM_FUNC_PTR)api_rt_fifo_assign,       "api_rt_fifo_assign",       sizeof(TY_MIL_COM_SYS_FIFO_RT_ASSIGN_INPUT), sizeof(TY_MIL_COM_ACK)                      },
  {MIL_COM_SYS_FIFO_BC_ASSIGN,(TY_MIL_COM_FUNC_PTR)api_bc_fifo_assign,       "api_bc_fifo_assign",       sizeof(TY_MIL_COM_SYS_FIFO_BC_ASSIGN_INPUT), sizeof(TY_MIL_COM_ACK)                      }
};


void api_sys_fifo_command_table( TY_MIL_COM_TABLE_ENTRY ** table, AiUInt32 * size )
{
    *size  = sizeof(axTargetSysFifoCommands) / sizeof(TY_MIL_COM_TABLE_ENTRY);
    *table = axTargetSysFifoCommands;
}

/*****************************************************************************/
/*                                                                           */
/*   Module : API_SYS                  Submodule : API_SYS_INI_FIFO    */
/*                                                                           */
/*   Author : Michael Stahl            Project   : API1553-DS                */
/*                                                                           */
/*   Source : C                        Tools     : PC/AT; Norton Editor;     */
/*                                                 CYGNUS, GNU-C, As, and LD */
/*                                                 IDT-C 5.1 Toolkit         */
/*---------------------------------------------------------------------------*/
/*   Create : 22.09.99   Update : 22.09.99                                   */
/*---------------------------------------------------------------------------*/
/*    Descriptions                                                           */
/*    ------------                                                           */
/*    Inputs    : Amount of Fifo's in queue [f_nbr]                          */
/*                Amount of Buffer's in one Fifo [b_nbr]                      */
/*                                                                           */
/*    Outputs   : Instruction acknowledge type [ackfl]                       */
/*                                                                           */
/*    Description :                                                          */
/*    This function handles the 'API_INI_FIFO' instruction to allocate   */
/*    a buffer pool in the global RAM area.                                  */
/*                                                                           */
/*****************************************************************************/
L_WORD api_sys_ini_fifo( TY_API_DEV *p_api_dev, TY_MIL_COM_SYS_FIFO_INI_INPUT * in, TY_MIL_COM_ACK * out )
{

  if( (in->f_nbr > 0) && (in->f_nbr <= MAX_API_QUEUE_SIZE) && (in->b_nbr >= 2) && (in->b_nbr <= MAX_API_FIFO_SIZE) )
  {
    api_io_ini_fifo(p_api_dev, (BYTE)in->f_nbr, (WORD)in->b_nbr);
  }
  else
  {
    return API_ERR_PARAMETER_OUT_OF_RANGE;
  }

  return 0;

} /* end: api_sys_ini_fifo */


/*****************************************************************************/
/*                                                                           */
/*   Module : API_BUF                  Submodule : API_BUF_FIFO_WRITE        */
/*                                                                           */
/*   Author : Michael Stahl            Project   : API1553-DS                */
/*                                                                           */
/*   Source : C                        Tools     : PC/AT; Norton Editor;     */
/*                                                 CYGNUS, GNU-C, As, and LD */
/*                                                 IDT-C 5.1 Toolkit         */
/*---------------------------------------------------------------------------*/
/*   Create : 22.09.99   Update : 22.09.99                                   */
/*---------------------------------------------------------------------------*/
/*    Descriptions                                                           */
/*    ------------                                                           */
/*    Inputs:     Fifo Id [f_id]                                             */
/*                Size to copy in WORD's [size]                              */
/*                Dataset Words to write [data_p]                            */
/*                                                                           */
/*    Outputs   : Instruction acknowledge type [ackfl]                       */
/*                                                                           */
/*    Description :                                                          */
/*    This function handles the 'API_BUF_FIFO_WRITE' instruction to write    */
/*    the specified data to the fifo queueu memory area of the API1553-DS.   */
/*                                                                           */
/*****************************************************************************/
L_WORD api_buf_fifo_write( TY_API_DEV *p_api_dev, TY_MIL_COM_SYS_FIFO_WRITE_INPUT * in, TY_MIL_COM_ACK * out )
{
  L_WORD i;
  L_WORD ret_val = 0;
  L_WORD w_size, tmp_size;
  WORD   volatile *ptr;

  BYTE f_id = (BYTE)in->id;

  if( (f_id > 0) && (f_id <= p_api_dev->queue_setup[ p_api_dev->biu ].max_queue) )
  {

    tmp_size = ( (p_api_dev->queue_setup[ p_api_dev->biu ].max_size + 1) / 2) *2;

    w_size = (WORD) (tmp_size *(MAX_API_BUF_SIZE / 2) );

    if(in->count <= (w_size / 2) )
    {

      /* Set pointer to Fifo Memory address */
      if(p_api_dev->queue_setup[ p_api_dev->biu ].queue[ f_id - 1 ].flag == API_OFF)
      {
        ptr = (WORD*)API_FIFO_MEM_ADDR_ABS(p_api_dev->queue_setup[ p_api_dev->biu ].queue[ f_id - 1 ].saddr);
        p_api_dev->queue_setup[ p_api_dev->biu ].queue[ f_id - 1 ].flag = API_ON;
      }
      else
      {
        ptr = (WORD*)API_FIFO_MEM_ADDR_ABS(p_api_dev->queue_setup[ p_api_dev->biu ].queue[ f_id - 1 ].half_buf_addr);
        p_api_dev->queue_setup[ p_api_dev->biu ].queue[ f_id - 1 ].flag = API_OFF;
      }
      /* Write data to Dataset Memory */
      for(i = 0; i < in->count; i++)
      {
        ptr[i] = BSWAP16(in->data[i]);
      }
    }
    else
      ret_val = API_ERR_INVALID_SIZE;
  }
  else
    ret_val = API_ERR_INVALID_ID;

  return ret_val;

} /* end: api_buf_fifo_write */



/*****************************************************************************/
/*                                                                           */
/*   Module : API_BUF                  Submodule : API_BUF_FIFO_STATUS_READ  */
/*                                                                           */
/*   Author : Michael Stahl            Project   : API1553-DS                */
/*                                                                           */
/*   Source : C                        Tools     : PC/AT; Norton Editor;     */
/*                                                 CYGNUS, GNU-C, As, and LD */
/*                                                 IDT-C 5.1 Toolkit         */
/*---------------------------------------------------------------------------*/
/*   Create : 22.09.99   Update : 22.09.99                                   */
/*---------------------------------------------------------------------------*/
/*    Descriptions                                                           */
/*    ------------                                                           */
/*    Inputs:     Fifo Id [f_id]                                             */
/*                                                                           */
/*    Outputs   : Instruction acknowledge type [ackfl]                       */
/*                Buffer transmit status [b_status]                          */
/*                                                                           */
/*    Description :                                                          */
/*    This function handles the 'API_BUF_FIFO_MARK_READ' instruction to read */
/*    the actual buffer status of the specified queue.                       */
/*                                                                           */
/*****************************************************************************/
L_WORD api_buf_fifo_status_read( TY_API_DEV *p_api_dev, TY_MIL_COM_SYS_FIFO_STATUS_INPUT * in, TY_MIL_COM_SYS_FIFO_STATUS_OUTPUT * out )
{
  L_WORD ret_val = 0;
  BYTE   f_id    = (BYTE)in->id;

  if( (f_id > 0) && (f_id <= p_api_dev->queue_setup[ p_api_dev->biu ].max_queue) )
  {
    out->status = p_api_dev->queue_setup[ p_api_dev->biu ].queue[ f_id - 1 ].buf_cnt;

    if(out->status != 0)
    {
      p_api_dev->queue_setup[ p_api_dev->biu ].queue[ f_id - 1 ].buf_cnt = 0;
    }
  }
  else
  {
    ret_val = API_ERR_INVALID_ID;
  }

  return ret_val;

} /* end: api_buf_fifo_status_read */



/*****************************************************************************/
/*                                                                           */
/*   Module : API_RT                   Submodule : API_RT_FIFO_ASSIGN        */
/*                                                                           */
/*   Author : Michael Stahl            Project   : API1553-DS                */
/*                                                                           */
/*   Source : C                        Tools     : PC/AT; Norton Editor;     */
/*                                                 CYGNUS, GNU-C, As, and LD */
/*                                                 IDT-C 5.1 Toolkit         */
/*---------------------------------------------------------------------------*/
/*   Create : 22.09.99   Update : 22.09.99                                   */
/*---------------------------------------------------------------------------*/
/*    Descriptions                                                           */
/*    ------------                                                           */
/*    Inputs    : RT Dynamic Tagging control (enable,disable) [con],         */
/*                Fifo Index [f_id],                                         */
/*                RT [rt],                                                   */
/*                SA [sa],                                                   */
/*                                                                           */
/*    Outputs   : Instruction acknowledge type [ackfl]                       */
/*                                                                           */
/*    Description :                                                          */
/*    This function handles the 'API_RT_FIFO_ASSIGN' instruction to control  */
/*    the Dynamic Tagging within a RT Transfer.                              */
/*                                                                           */
/*****************************************************************************/
L_WORD api_rt_fifo_assign( TY_API_DEV *p_api_dev, TY_MIL_COM_SYS_FIFO_RT_ASSIGN_INPUT * in, TY_MIL_COM_ACK * out )
{
  WORD i;
  L_WORD ret_val = 0;

  BYTE con  = (BYTE)in->con;
  BYTE f_id = (BYTE)in->f_id;
  BYTE rt   = (BYTE)in->rt;
  BYTE sa   = (BYTE)in->sa;


  if( (rt < MAX_API_RT) && (sa < MAX_API_RT_MC_SA) && (f_id > 0) && (f_id <= p_api_dev->queue_setup[ p_api_dev->biu ].max_queue) && ( (con == API_RT_FIFO_ENA) || (con == API_RT_FIFO_DIS) ) )
  {

    if( (p_api_dev->queue_setup[ p_api_dev->biu ].max_queue != 0) && (p_api_dev->queue_setup[ p_api_dev->biu ].max_size != 0) )
    {
      /* Find tagging slot */
      if(con == API_RT_FIFO_ENA)
      {

        /* check if queue is free */
        if(p_api_dev->queue_setup[ p_api_dev->biu ].queue[ f_id - 1 ].mode == API_OFF)
        {

          for(i = 0; i < p_api_dev->queue_setup[ p_api_dev->biu ].max_queue; i++)
          {

            /* check if rt/sa is always in the queue */
            if( (p_api_dev->queue_setup[ p_api_dev->biu ].queue[ i ].rt == rt) && (p_api_dev->queue_setup[ p_api_dev->biu ].queue[ i ].sa == sa) )
            {

              /* delete old rt/sa */
              p_api_dev->queue_setup[ p_api_dev->biu ].queue[ i ].mode = API_OFF;
              p_api_dev->queue_setup[ p_api_dev->biu ].queue[ i ].xid = 0;
              p_api_dev->queue_setup[ p_api_dev->biu ].queue[ i ].rt = 0xFF;
              p_api_dev->queue_setup[ p_api_dev->biu ].queue[ i ].sa = 0xFF;
              p_api_dev->queue_setup[ p_api_dev->biu ].queue[ i ].cur_buf_addr = p_api_dev->queue_setup[ p_api_dev->biu ].queue[ i ].saddr;
              p_api_dev->queue_setup[ p_api_dev->biu ].queue[ i ].flag = API_OFF;
              p_api_dev->queue_setup[ p_api_dev->biu ].queue[ i ].buf_cnt = 0;
              api_io_rt_sa_fifo_con(p_api_dev, rt, sa, 0);
            }
          }
          /* insert new rt/sa */
          p_api_dev->queue_setup[ p_api_dev->biu ].queue[ f_id - 1 ].mode = API_ON;
          p_api_dev->queue_setup[ p_api_dev->biu ].queue[ f_id - 1 ].xid = 0;
          p_api_dev->queue_setup[ p_api_dev->biu ].queue[ f_id - 1 ].rt = rt;
          p_api_dev->queue_setup[ p_api_dev->biu ].queue[ f_id - 1 ].sa = sa;
          p_api_dev->queue_setup[ p_api_dev->biu ].queue[ f_id - 1 ].cur_buf_addr = p_api_dev->queue_setup[ p_api_dev->biu ].queue[ f_id - 1 ].saddr;
          p_api_dev->queue_setup[ p_api_dev->biu ].queue[ f_id - 1 ].flag = API_OFF;
          p_api_dev->queue_setup[ p_api_dev->biu ].queue[ f_id - 1 ].buf_cnt = 0;

          api_io_rt_sa_fifo_con(p_api_dev, rt, sa, f_id);
        }
        else
          ret_val = API_ERR;
      }
      else
      {
         /* Disable */
        if(p_api_dev->queue_setup[ p_api_dev->biu ].queue[ f_id - 1 ].mode == API_ON)
        {

          if( (p_api_dev->queue_setup[ p_api_dev->biu ].queue[ f_id - 1 ].rt == rt) && (p_api_dev->queue_setup[ p_api_dev->biu ].queue[ f_id - 1 ].sa == sa) )
          {

            /* Xfer slot */
            p_api_dev->queue_setup[ p_api_dev->biu ].queue[ f_id - 1 ].mode = API_OFF;
            p_api_dev->queue_setup[ p_api_dev->biu ].queue[ f_id - 1 ].xid = 0;
            p_api_dev->queue_setup[ p_api_dev->biu ].queue[ f_id - 1 ].rt = 0xFF;
            p_api_dev->queue_setup[ p_api_dev->biu ].queue[ f_id - 1 ].sa = 0xFF;
            p_api_dev->queue_setup[ p_api_dev->biu ].queue[ f_id - 1 ].cur_buf_addr = p_api_dev->queue_setup[ p_api_dev->biu ].queue[ f_id - 1 ].saddr;
            api_io_rt_sa_fifo_con(p_api_dev, rt, sa, 0);
          }
          else
            ret_val = API_ERR;
        }
      }
    }
    else
      ret_val = API_ERR;
  }
  else
    ret_val = API_ERR_INVALID_RT;

  return (ret_val);

} /* end: api_rt_fifo_assign */

/*****************************************************************************/
/*                                                                           */
/*   Module : API_BC                   Submodule : API_BC_FIFO_ASSIGN        */
/*                                                                           */
/*   Author : Michael Stahl            Project   : API1553-DS                */
/*                                                                           */
/*   Source : C                        Tools     : PC/AT; Norton Editor;     */
/*                                                 CYGNUS, GNU-C, As, and LD */
/*                                                 IDT-C 5.1 Toolkit         */
/*---------------------------------------------------------------------------*/
/*   Create : 22.09.99   Update : 22.09.99                                   */
/*---------------------------------------------------------------------------*/
/*    Descriptions                                                           */
/*    ------------                                                           */
/*    Inputs    : BC Dynamic Tagging control (enable,disable) [con],         */
/*                Fifo Index [f_id],                                         */
/*                BC Transfer Identifier [xid],                              */
/*                                                                           */
/*    Outputs   : Instruction acknowledge type [ackfl]                       */
/*                                                                           */
/*    Description :                                                          */
/*    This function handles the 'API_BC_FIFO_ASSIGN' instruction to control  */
/*    the Dynamic Tagging within a BC Transfer.                              */
/*                                                                           */
/*****************************************************************************/

L_WORD api_bc_fifo_assign( TY_API_DEV *p_api_dev, TY_MIL_COM_SYS_FIFO_BC_ASSIGN_INPUT * in, TY_MIL_COM_ACK * out )
{
    WORD i     = 0;
    BYTE con   = (BYTE)in->con;
    BYTE f_id  = (BYTE)in->f_id;
    WORD xid   = (WORD)in->xid;

    if( (xid > 0) && (xid < p_api_dev->glb_mem.biu[ p_api_dev->biu ].count.bc_xfer_desc) && 
        (f_id > 0) && (f_id <= p_api_dev->queue_setup[ p_api_dev->biu ].max_queue) && 
        ( (con == API_BC_FIFO_ENA) || (con == API_BC_FIFO_DIS) ) )
    {

        if( (p_api_dev->queue_setup[ p_api_dev->biu ].max_queue != 0) && 
            (p_api_dev->queue_setup[ p_api_dev->biu ].max_size != 0) )
        {
            /* Find tagging slot */
            if(con == API_BC_FIFO_ENA)
            {

                /* check if fifo is free */
                if(p_api_dev->queue_setup[ p_api_dev->biu ].queue[ f_id - 1 ].mode == API_OFF)
                {

                    for(i = 0; i < p_api_dev->queue_setup[ p_api_dev->biu ].max_queue; i++)
                    {

                        /* check if xid is always in the queue */
                        if(p_api_dev->queue_setup[ p_api_dev->biu ].queue[ i ].xid == xid)
                        {

                            /* delete old xid */
                            p_api_dev->queue_setup[ p_api_dev->biu ].queue[ i ].mode = API_OFF;
                            p_api_dev->queue_setup[ p_api_dev->biu ].queue[ i ].xid = 0;
                            p_api_dev->queue_setup[ p_api_dev->biu ].queue[ i ].rt = 0xFF;
                            p_api_dev->queue_setup[ p_api_dev->biu ].queue[ i ].sa = 0xFF;
                            p_api_dev->queue_setup[ p_api_dev->biu ].queue[ i ].cur_buf_addr = p_api_dev->queue_setup[ p_api_dev->biu ].queue[ i ].saddr;
                            p_api_dev->queue_setup[ p_api_dev->biu ].queue[ i ].flag = API_OFF;
                            p_api_dev->queue_setup[ p_api_dev->biu ].queue[ i ].buf_cnt = 0;

                            api_io_bc_xfer_fifo_con(p_api_dev, xid, 0);

                        }
                    }
                    /* insert new xid */
                    p_api_dev->queue_setup[ p_api_dev->biu ].queue[ f_id - 1 ].mode = API_ON;
                    p_api_dev->queue_setup[ p_api_dev->biu ].queue[ f_id - 1 ].xid = xid;
                    p_api_dev->queue_setup[ p_api_dev->biu ].queue[ f_id - 1 ].rt = 0xFF;
                    p_api_dev->queue_setup[ p_api_dev->biu ].queue[ f_id - 1 ].sa = 0xFF;
                    p_api_dev->queue_setup[ p_api_dev->biu ].queue[ f_id - 1 ].cur_buf_addr = p_api_dev->queue_setup[ p_api_dev->biu ].queue[ f_id - 1 ].saddr;
                    p_api_dev->queue_setup[ p_api_dev->biu ].queue[ f_id - 1 ].flag = API_OFF;
                    p_api_dev->queue_setup[ p_api_dev->biu ].queue[ f_id - 1 ].buf_cnt = 0;

                    api_io_bc_xfer_fifo_con(p_api_dev, xid, f_id);

                }
                else
                    return API_NAK;
            }
            else
            {
                /* Disable */
                if(p_api_dev->queue_setup[ p_api_dev->biu ].queue[ f_id - 1 ].mode == API_ON)
                {
                    if(p_api_dev->queue_setup[ p_api_dev->biu ].queue[ f_id - 1 ].xid == xid)
                    {

                        /* Xfer slot */
                        p_api_dev->queue_setup[ p_api_dev->biu ].queue[ f_id - 1 ].mode = API_OFF;
                        p_api_dev->queue_setup[ p_api_dev->biu ].queue[ f_id - 1 ].xid = 0;
                        p_api_dev->queue_setup[ p_api_dev->biu ].queue[ f_id - 1 ].rt = 0xFF;
                        p_api_dev->queue_setup[ p_api_dev->biu ].queue[ f_id - 1 ].sa = 0xFF;
                        p_api_dev->queue_setup[ p_api_dev->biu ].queue[ f_id - 1 ].cur_buf_addr = p_api_dev->queue_setup[ p_api_dev->biu ].queue[ f_id - 1 ].saddr;
                        p_api_dev->queue_setup[ p_api_dev->biu ].queue[ f_id - 1 ].flag = API_OFF;
                        p_api_dev->queue_setup[ p_api_dev->biu ].queue[ f_id - 1 ].buf_cnt = 0;
                        api_io_bc_xfer_fifo_con(p_api_dev, xid, 0);
                    }
                    else
                        return API_NAK;
                }
            }
        }
        else
            return API_NAK;
    }
    else
        return API_NAK;

    return 0;

} /* end: api_bc_fifo_assign */

NAMESPACE1553_END


/*** end of file ***/

