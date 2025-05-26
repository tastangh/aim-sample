/* SPDX-FileCopyrightText: 2001-2022 AIM GmbH <info@aim-online.com> */
/* SPDX-License-Identifier: MIT OR GPL-2.0-or-later */

/*! \file api_buf.c
1553 Buffer handling module of the ASP Driver SW.This module comprises
the handler routines for the 1553 Buffer instructions.
*/

#include "mil_tsw_includes.h"


NAMESPACE1553_START



TY_MIL_COM_TABLE_ENTRY axTargetLsBufCommands[] = {
  {MIL_COM_LS_BUF_DEF,          (TY_MIL_COM_FUNC_PTR)api_buf_def,           "api_buf_def",           sizeof(TY_MIL_COM_LS_BUF_DEF_INPUT),           sizeof(TY_MIL_COM_LS_BUF_DEF_OUTPUT)          },
  {MIL_COM_LS_BUF_READ,         (TY_MIL_COM_FUNC_PTR)api_buf_read,          "api_buf_read",          sizeof(TY_MIL_COM_LS_BUF_READ_INPUT),          sizeof(TY_MIL_COM_LS_BUF_READ_OUTPUT)         },
  {MIL_COM_LS_BUF_WRITE,        (TY_MIL_COM_FUNC_PTR)api_buf_write,         "api_buf_write",         sizeof(TY_MIL_COM_LS_BUF_WRITE_INPUT),         sizeof(TY_MIL_COM_ACK)                        },
  {MIL_COM_LS_BUF_C1760_CON,    (TY_MIL_COM_FUNC_PTR)api_buf_c1760_con,     "api_buf_c1760_con",     sizeof(TY_MIL_COM_LS_BUF_C1760_CON_INPUT),     sizeof(TY_MIL_COM_ACK)                        },
  {MIL_COM_LS_BUF_DATASET_WRITE,(TY_MIL_COM_FUNC_PTR)api_buf_dataset_write, "api_buf_dataset_write", sizeof(TY_MIL_COM_LS_BUF_DATASET_WRITE_INPUT), sizeof(TY_MIL_COM_ACK)                        },
  {MIL_COM_LS_BUF_DATASET_READ, (TY_MIL_COM_FUNC_PTR)api_buf_dataset_read,  "api_buf_dataset_read",  sizeof(TY_MIL_COM_LS_BUF_DATASET_READ_INPUT),  sizeof(TY_MIL_COM_LS_BUF_DATASET_READ_OUTPUT) },
  {MIL_COM_LS_BUF_WRITE_BLOCK,  (TY_MIL_COM_FUNC_PTR)api_buf_write_block,   "api_buf_write_block",   sizeof(TY_MIL_COM_LS_BUF_WRITE_BLOCK_INPUT),   sizeof(TY_MIL_COM_ACK)                        },
  {MIL_COM_LS_BUF_HOST_CONTROL, (TY_MIL_COM_FUNC_PTR)api_buf_host_control,  "api_buf_host_control",  sizeof(TY_MIL_COM_LS_BUF_HOST_CONTROL_INPUT),  sizeof(TY_MIL_COM_ACK)                        },
};



void api_ls_buf_command_table( TY_MIL_COM_TABLE_ENTRY ** table, AiUInt32 * size )
{
    *size = sizeof(axTargetLsBufCommands) / sizeof(TY_MIL_COM_TABLE_ENTRY);
    *table = axTargetLsBufCommands;
}



/*****************************************************************************/
/*                                                                           */
/*   Module : API_BUF                  Submodule : API_BUF_DEF               */
/*                                                                           */
/*   Author : Marco Motz               Project   : API1553-DS                */
/*                                                                           */
/*   Source : C                        Tools     : PC/AT; Norton Editor;     */
/*                                                 CYGNUS, GNU-C, As, and LD */
/*                                                 IDT-C 5.1 Toolkit         */
/*---------------------------------------------------------------------------*/
/*   Create : 01.05.98   Update : 10.06.03                                   */
/*---------------------------------------------------------------------------*/
/*    Descriptions                                                           */
/*    ------------                                                           */
/*    Inputs:     Buffer Type (BC/RT) [bt]                                   */
/*                Buffer Header ID [hid]                                     */
/*                Data Buffer ID [bid]                                       */
/*                Amount of Data Words to write [len]                        */
/*                Data Words to write [data_p]                               */
/*                                                                           */
/*    Outputs   : Instruction acknowledge type [ackfl]                       */
/*                                                                           */
/*    Description :                                                          */
/*    This function handles the 'API_BUF_DEF' instruction to write the       */
/*    specified amount of Data Words to the selected Data Buffer within      */
/*    the API1553-DS Global Memory.                                          */
/*                                                                           */
/*****************************************************************************/
L_WORD api_buf_def( TY_API_DEV *p_api_dev, TY_MIL_COM_LS_BUF_DEF_INPUT * in, TY_MIL_COM_LS_BUF_DEF_OUTPUT * out )
{
  L_WORD hid_max = 0;
  L_WORD addr    = 0;
  L_WORD tmpl    = 0;
  L_WORD ret_val = 0;

  WORD volatile *c1760_ptr = NULL;
  WORD volatile *ptr       = NULL;

  WORD buffer_id = 0;
  BYTE c1760_ix  = 0;
  WORD index     = 0;
  WORD i         = 0;

  if(in->bt == API_BUF_BC_TYPE)
    hid_max = p_api_dev->glb_mem.biu[ p_api_dev->biu ].count.bc_bh_area;
  else if(in->bt == API_BUF_RT_TYPE)
    hid_max = p_api_dev->glb_mem.biu[ p_api_dev->biu ].count.rt_bh_area;

  if( ( (in->bt == API_BUF_BC_TYPE) || (in->bt == API_BUF_RT_TYPE) ) && (in->hid < hid_max) && (in->bid < p_api_dev->glb_mem.sim_buf_count[ p_api_dev->biu ]) && (in->count >= 1) && (in->count <= 32) )
  {

    if(in->hid == 0)
    {
      /* Use absolute Buffer ID */
      buffer_id = (WORD)in->bid;
      addr = API_GLB_GLOBREL_TO_PBIREL(p_api_dev->glb_mem.sim_buf_base_addr[ p_api_dev->biu ]);
      addr += (buffer_id *MAX_API_BUF_SIZE);
      addr &= 0x03FFFFFCL;
      ptr = (WORD*) API_GLB_PBIREL_TO_TSWPTR(addr);
    }
    else
    {
      /* From Buffer Header */
      index = 0;
      if(in->bt == API_BUF_BC_TYPE)
      {
        /* BC */
        addr = BSWAP32( (*p_api_dev->bc_bh_desc_p[ p_api_dev->biu ])[ in->hid ].bp_addr);
        if(in->bid == 0)
          index = (WORD) ( BSWAP32((*p_api_dev->bc_bh_desc_p[ p_api_dev->biu ])[ in->hid ].bh_ctrl) & 0x000000FFL );
        else
          index = in->bid - 1;
      }
      else
      {
        /* RT */
        addr = BSWAP32( (*p_api_dev->rt_bh_desc_p[ p_api_dev->biu ])[ in->hid ].bp_addr);
        if(in->bid == 0)
          index = (WORD) ( BSWAP32((*p_api_dev->rt_bh_desc_p[ p_api_dev->biu ])[ in->hid ].bh_ctrl) & 0x000000FFL );
        else
          index = in->bid - 1;
      }

      addr += (index * MAX_API_BUF_SIZE);
      addr &= (0x03FFFFFCL);
      ptr = (WORD*) API_GLB_PBIREL_TO_TSWPTR(addr);

      tmpl = addr - API_GLB_GLOBREL_TO_PBIREL(p_api_dev->glb_mem.sim_buf_base_addr[ p_api_dev->biu ]);
      tmpl /= MAX_API_BUF_SIZE;
      buffer_id = (WORD) (tmpl);
    }

    /* Write data to buffer */
    c1760_ptr = ptr;
    for(i = 0; i < MAX_API_BUF_SIZE/2; i++)
    {
      if(i < in->count)
      {
        ptr[i] = BSWAP16(in->data[i]);
      }
      else
        ptr[i] = 0x0000;
    }

    /* 1760 Protocol Handling */
    if(api_io_c1760_mode_used(p_api_dev, (BYTE)p_api_dev->biu) == API_OK)
    {
      /* 1760 Buffer */
      if(api_io_c1760_buf_get(p_api_dev, (BYTE)p_api_dev->biu, buffer_id, &c1760_ix) == API_OK)
      {
        /* 1760 Checksum Handling */
        api_io_c1760_buf_set(p_api_dev, (BYTE)p_api_dev->biu, c1760_ix, c1760_ptr);
      }
    }

    if(p_api_dev->bc_status[p_api_dev->biu] == API_BUSY && in->bt == API_BUF_BC_TYPE)
    {
        UsbSynchronizeMemoryArea(p_api_dev, API_GLB_PBIREL_TO_GLOBREL(addr), MAX_API_BUF_SIZE, Out);
    }
    else if(p_api_dev->rt_status[p_api_dev->biu] == API_BUSY && in->bt == API_BUF_RT_TYPE)
    {
        UsbSynchronizeMemoryArea(p_api_dev, API_GLB_PBIREL_TO_GLOBREL(addr), MAX_API_BUF_SIZE, Out);
    }

    if( out )
    {
        out->bid     = buffer_id;
        out->address = API_GLB_PBIREL_TO_GLOBREL(addr);
    }
  }
  else
  {
    ret_val = API_ERR_PARAMETER_OUT_OF_RANGE;
  }

  return ret_val;

} /* end: api_buf_def */

/*****************************************************************************/
/*                                                                           */
/*   Module : API_BUF                  Submodule : API_BUF_READ              */
/*                                                                           */
/*   Author : Marco Motz               Project   : API1553-DS                */
/*                                                                           */
/*   Source : C                        Tools     : PC/AT; Norton Editor;     */
/*                                                 CYGNUS, GNU-C, As, and LD */
/*                                                 IDT-C 5.1 Toolkit         */
/*---------------------------------------------------------------------------*/
/*   Create : 01.05.98   Update : 10.06.03                                   */
/*---------------------------------------------------------------------------*/
/*    Descriptions                                                           */
/*    ------------                                                           */
/*    Inputs:     Buffer Type (BC/RT) [bt]                                   */
/*                Buffer Header ID [hid]                                     */
/*                Data Buffer ID [bid]                                       */
/*                Amount of Data Words to write [len]                        */
/*                                                                           */
/*    Outputs   : Instruction acknowledge type [ackfl]                       */
/*                Data Words to read [data_p]                                */
/*                                                                           */
/*    Description :                                                          */
/*    This function handles the 'API_BUF_DEF' instruction to write the       */
/*    specified amount of Data Words to the selected Data Buffer within      */
/*    the API1553-DS Global Memory.                                          */
/*                                                                           */
/*****************************************************************************/
L_WORD api_buf_read( TY_API_DEV *p_api_dev, TY_MIL_COM_LS_BUF_READ_INPUT * in, TY_MIL_COM_LS_BUF_READ_OUTPUT * out )
{
  L_WORD ret_val       = 0L;
  L_WORD hid_max       = 0L;
  L_WORD addr          = 0L;
  L_WORD tmpl          = 0L;
  WORD   volatile *ptr = 0;
  WORD   buffer_id     = 0;
  WORD   index         = 0;
  WORD   i;

  if(in->bt == API_BUF_BC_TYPE)
    hid_max = p_api_dev->glb_mem.biu[ p_api_dev->biu ].count.bc_bh_area;
  else if(in->bt == API_BUF_RT_TYPE)
    hid_max = p_api_dev->glb_mem.biu[ p_api_dev->biu ].count.rt_bh_area;

  if( ( (in->bt == API_BUF_BC_TYPE) || (in->bt == API_BUF_RT_TYPE) ) && (in->hid < hid_max) && ( (0xffff == in->bid) || (in->bid < p_api_dev->glb_mem.sim_buf_count[ p_api_dev->biu ]) ) && (in->count >= 1) && (in->count <= 32) )
  {

    if(in->hid == 0)
    {
      /* Use absolute Buffer ID */
      buffer_id = (WORD)in->bid;
      addr = p_api_dev->glb_mem.sim_buf_base_addr[ p_api_dev->biu ];
      addr += (buffer_id *MAX_API_BUF_SIZE);
      addr &= 0x03FFFFFCL;
      ptr = (WORD*) API_GLB_PBIREL_TO_TSWPTR(addr);
    }
    else
    {
      /* From Buffer Header */
      index = 0;
      if(in->bt == API_BUF_BC_TYPE)
      {
        /* BC */
        addr = BSWAP32( (*p_api_dev->bc_bh_desc_p[ p_api_dev->biu ])[ in->hid ].bp_addr);
        if(in->bid == 0)
        {
            if(p_api_dev->bc_status[p_api_dev->biu] == API_BUSY)
            {
                UsbSynchronizeLWord(p_api_dev, 
                    API_GLB_MEM_ADDR_REL(&(*p_api_dev->bc_bh_desc_p[ p_api_dev->biu ])[ in->hid ].bh_ctrl), 
                    In);
            }

            index = (WORD) (BSWAP32( (*p_api_dev->bc_bh_desc_p[ p_api_dev->biu ])[ in->hid ].bh_ctrl) &0x000000FFL);
        }
        else if(0xffff == in->bid)
        {
            if(p_api_dev->bc_status[p_api_dev->biu] == API_BUSY)
            {
                UsbSynchronizeLWord(p_api_dev, 
                    API_GLB_MEM_ADDR_REL(&(*p_api_dev->bc_bh_desc_p[ p_api_dev->biu ])[ in->hid ].bh_ctrl), 
                    In);
            }

            index = (WORD) (BSWAP32( (*p_api_dev->bc_bh_desc_p[ p_api_dev->biu ])[ in->hid ].bh_ctrl) &0x000000FFL);
            if(index > 0)
                index--;
            else
                index = (1 << ( (BSWAP32( (*p_api_dev->bc_bh_desc_p[ p_api_dev->biu ])[ in->hid ].bh_ctrl) &0x0003C000) >> 14) ) - 1;
        }
        else
          index = in->bid - 1;
      }
      else
      {
        /* RT */
        addr = BSWAP32( (*p_api_dev->rt_bh_desc_p[ p_api_dev->biu ])[ in->hid ].bp_addr);
        if(in->bid == 0)
        {
            if(p_api_dev->rt_status[p_api_dev->biu] == API_BUSY)
            {
                UsbSynchronizeLWord(p_api_dev, 
                    API_GLB_MEM_ADDR_REL(&(*p_api_dev->rt_bh_desc_p[ p_api_dev->biu ])[ in->hid ].bh_ctrl), 
                    In);
            }

            index = (WORD) (BSWAP32( (*p_api_dev->rt_bh_desc_p[ p_api_dev->biu ])[ in->hid ].bh_ctrl) &0x000000FFL);
        }
        else if(0xffff == in->bid)
        {
            if(p_api_dev->rt_status[p_api_dev->biu] == API_BUSY)
            {
                UsbSynchronizeLWord(p_api_dev, 
                    API_GLB_MEM_ADDR_REL(&(*p_api_dev->rt_bh_desc_p[ p_api_dev->biu ])[ in->hid ].bh_ctrl), 
                    In);
            }

            index = (WORD) (BSWAP32( (*p_api_dev->rt_bh_desc_p[ p_api_dev->biu ])[ in->hid ].bh_ctrl) &0x000000FFL);
            if(index > 0)
              index--;
            else
              index = (1 << ( (BSWAP32( (*p_api_dev->rt_bh_desc_p[ p_api_dev->biu ])[ in->hid ].bh_ctrl) &0x0003C000) >> 14) ) - 1;
        }
        else
          index = in->bid - 1;
      }

      addr += (index *MAX_API_BUF_SIZE);
      addr &= 0x03FFFFFCL;
      ptr = (WORD*) API_GLB_PBIREL_TO_TSWPTR(addr);

      tmpl = addr - p_api_dev->glb_mem.sim_buf_base_addr[ p_api_dev->biu ];
      tmpl /= MAX_API_BUF_SIZE;
      buffer_id = (WORD) (tmpl);
    }

    out->bid     = buffer_id;
    out->address = BSWAP32(API_GLB_PBIREL_TO_GLOBREL(addr));

    if(p_api_dev->rt_status[p_api_dev->biu] == API_BUSY && in->bt == API_BUF_RT_TYPE)
    {
        UsbSynchronizeMemoryArea(p_api_dev, API_GLB_MEM_ADDR_REL(addr), MAX_API_BUF_SIZE, In);
    }
    else if(p_api_dev->bc_status[p_api_dev->biu] == API_BUSY && in->bt == API_BUF_BC_TYPE)
    {
        UsbSynchronizeMemoryArea(p_api_dev, API_GLB_MEM_ADDR_REL(addr), MAX_API_BUF_SIZE, In);
    }

    /* Read data from buffer */
    for(i = 0; i < in->count; i++)
      out->data[i] = BSWAP16(ptr[i]);

    out->count = in->count;
    out->xAck.ulSwapControl = in->count << 16;
  }
  else
  {
    ret_val = API_ERR_PARAMETER_OUT_OF_RANGE;
  }

  return ret_val;

} /* end: api_buf_read */

/*****************************************************************************/
/*                                                                           */
/*   Module : API_BUF                  Submodule : API_BUF_RAM_WRITE         */
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
/*    Inputs:     Start Address of the Global Memory [addr]                  */
/*                Amount of Data Words to write [len]                        */
/*                Data Words [data_p]                                        */
/*                                                                           */
/*    Outputs   : Instruction acknowledge type [ackfl]                       */
/*                                                                           */
/*    Description :                                                          */
/*    This function handles the 'API_RAM_WRITE' instruction to write the     */
/*    specified amount of Data Words directly to the API1553-DS Global       */
/*    Memory beginning at the specified Start Address.                       */
/*                                                                           */
/*****************************************************************************/

BYTE api_buf_ram_write(TY_API_DEV *p_api_dev, L_WORD addr, BYTE len, BYTE *data_p)
{
  WORD volatile *ptr;
  WORD data;
  BYTE ackfl;
  short i;

  ackfl = API_ACK;

  addr &= 0x03FFFFFEL; /* even Byte address, divisible by 2 */

  /* Set pointer to Global Memory address */
  ptr = (WORD*)API_GLB_MEM_ADDR_ABS(addr);

  /* Write data to Global Memory */
  for(i = 0; i < len; i++)
  {
    data = MAKE_WORD(*(data_p + (2 *i) ), *(data_p + (2 *i) + 1) );
    *ptr = BSWAP16(data);
    ptr++;
  }

  UsbSynchronizeMemoryArea(p_api_dev, addr, len * sizeof(AiUInt16), Out);

  return (ackfl);

} /* end: api_buf_ram_write */

/*****************************************************************************/
/*                                                                           */
/*   Module : API_BUF                  Submodule : API_BUF_RAM_READ          */
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
/*    Inputs:     Start Address of the Global Memory [addr]                  */
/*                Amount of Data Words to read [len]                         */
/*                                                                           */
/*    Outputs   : Instruction acknowledge type [ackfl]                       */
/*                                                                           */
/*    Description :                                                          */
/*    This function handles the 'API_RAM_READ' instruction to read the       */
/*    specified amount of Data Words directly from the API1553-DS Global     */
/*    Memory beginning at the specified Start Address.                       */
/*                                                                           */
/*****************************************************************************/

BYTE api_buf_ram_read(TY_API_DEV *p_api_dev, L_WORD addr, BYTE len)
{
  WORD volatile *ptr;
  BYTE ackfl;
  short i;

  ackfl = API_ACK2;

  addr &= 0x03FFFFFEL; /* even Byte address, divisible by 2 */

  UsbSynchronizeActiveMemoryBlocks(p_api_dev, addr, len * sizeof(AiUInt16), In);

  /* Set pointer to Global Memory address */
  ptr = (WORD*)API_GLB_MEM_ADDR_ABS(addr);

  /* Read data from Global Memory */
  for(i = 0; i < len; i++)
  {
    p_api_dev->word_data_arr[ i ] = BSWAP16(*ptr);
    ptr++;
  }

  p_api_dev->b_length = 0;
  p_api_dev->w_length = len;
  p_api_dev->lw_length = 0;
  p_api_dev->ack_w_ptr = &(p_api_dev->word_data_arr[ 0 ]);

  return (ackfl);

} /* end: api_buf_ram_read */

/*****************************************************************************/
/*                                                                           */
/*   Module : API_BUF                  Submodule : API_BUF_RAM_WR_LWORD      */
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
/*    Inputs:     Start Address of the Global Memory [addr]                  */
/*                Data to write [lval]                                       */
/*                                                                           */
/*    Outputs   : Instruction acknowledge type [ackfl]                       */
/*                                                                           */
/*    Description :                                                          */
/*    This function handles the 'API_RAM_WR_LWORD' instruction to write the  */
/*    specified longword directly to the API1553-DS Global Memory.           */
/*                                                                           */
/*****************************************************************************/

BYTE api_buf_ram_wr_lword(TY_API_DEV *p_api_dev, L_WORD addr, L_WORD lval)
{
  BYTE ackfl;

  ackfl = API_ACK;

  api_io_write_lword(p_api_dev, addr, lval);

  return (ackfl);

} /* end: api_buf_ram_wr_lword */

/*****************************************************************************/
/*                                                                           */
/*   Module : API_BUF                  Submodule : API_BUF_RAM_WR_WORD       */
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
/*    Inputs:     Start Address of the Global Memory [addr]                  */
/*                Data to write [wval]                                       */
/*                                                                           */
/*    Outputs   : Instruction acknowledge type [ackfl]                       */
/*                                                                           */
/*    Description :                                                          */
/*    This function handles the 'API_RAM_WR_WORD' instruction to write the   */
/*    specified word directly to the API1553-DS Global Memory.              */
/*                                                                           */
/*****************************************************************************/

BYTE api_buf_ram_wr_word(TY_API_DEV *p_api_dev, L_WORD addr, WORD wval)
{
  BYTE ackfl;

  ackfl = API_ACK;

  api_io_write_word(p_api_dev, addr, wval);

  return (ackfl);

} /* end: api_buf_ram_wr_word */

/*****************************************************************************/
/*                                                                           */
/*   Module : API_BUF                  Submodule : API_BUF_RAM_WR_BYTE       */
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
/*    Inputs:     Start Address of the Global Memory [addr]                  */
/*                Data to write [bval]                                       */
/*                                                                           */
/*    Outputs   : Instruction acknowledge type [ackfl]                       */
/*                                                                           */
/*    Description :                                                          */
/*    This function handles the 'API_RAM_WR_BYTE' instruction to write the   */
/*    specified byte directly to the API1553-DS Global Memory.               */
/*                                                                           */
/*****************************************************************************/

BYTE api_buf_ram_wr_byte(TY_API_DEV *p_api_dev, L_WORD addr, BYTE bval)
{
  BYTE ackfl;

  ackfl = API_ACK;

  api_io_write_byte(p_api_dev, addr, bval);

  return (ackfl);

} /* end: api_buf_ram_wr_byte */

/*****************************************************************************/
/*                                                                           */
/*   Module : API_BUF                  Submodule : API_BUF_RAM_RD_LWORD      */
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
/*    Inputs:     Start Address of the Global Memory [addr]                  */
/*                                                                           */
/*    Outputs   : Instruction acknowledge type [ackfl]                       */
/*                Data read from memory [lval]                               */
/*                                                                           */
/*    Description :                                                          */
/*    This function handles the 'API_RAM_RD_LWORD' instruction to read the   */
/*    specified longword directly from the API1553-DS Global Memory.         */
/*                                                                           */
/*****************************************************************************/

BYTE api_buf_ram_rd_lword(TY_API_DEV *p_api_dev, L_WORD addr)
{
  BYTE ackfl;
  L_WORD lval;

  ackfl = API_ACK2;

  lval = api_io_read_lword(p_api_dev, addr);
  p_api_dev->lword_data_arr[ 0 ] = lval;

  p_api_dev->b_length = 0;
  p_api_dev->w_length = 0;
  p_api_dev->lw_length = 1;
  p_api_dev->ack_lw_ptr = &(p_api_dev->lword_data_arr[ 0 ]);

  return (ackfl);

} /* end: api_buf_ram_rd_lword */

/*****************************************************************************/
/*                                                                           */
/*   Module : API_BUF                  Submodule : API_BUF_RAM_RD_WORD       */
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
/*    Inputs:     Start Address of the Global Memory [addr]                  */
/*                                                                           */
/*    Outputs   : Instruction acknowledge type [ackfl]                       */
/*                Data read from memory [wval]                               */
/*                                                                           */
/*    Description :                                                          */
/*    This function handles the 'API_RAM_RD_WORD' instruction to read the    */
/*    specified word directly from the API1553-DS Global Memory.             */
/*                                                                           */
/*****************************************************************************/

BYTE api_buf_ram_rd_word(TY_API_DEV *p_api_dev, L_WORD addr)
{
  BYTE ackfl;
  WORD wval;

  ackfl = API_ACK2;

  wval = api_io_read_word(p_api_dev, addr);
  p_api_dev->word_data_arr[ 0 ] = wval;

  p_api_dev->b_length = 0;
  p_api_dev->w_length = 1;
  p_api_dev->lw_length = 0;
  p_api_dev->ack_w_ptr = &(p_api_dev->word_data_arr[ 0 ]);

  return (ackfl);

} /* end: api_buf_ram_rd_word */

/*****************************************************************************/
/*                                                                           */
/*   Module : API_BUF                  Submodule : API_BUF_RAM_RD_BYTE       */
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
/*    Inputs:     Start Address of the Global Memory [addr]                  */
/*                                                                           */
/*    Outputs   : Instruction acknowledge type [ackfl]                       */
/*                Data read from memory [bval]                               */
/*                                                                           */
/*    Description :                                                          */
/*    This function handles the 'API_RAM_RD_BYTE' instruction to read the    */
/*    specified byte directly from the API1553-DS Global Memory.             */
/*                                                                           */
/*****************************************************************************/

BYTE api_buf_ram_rd_byte(TY_API_DEV *p_api_dev, L_WORD addr)
{
  BYTE ackfl;
  BYTE bval;

  ackfl = API_ACK2;

  bval = api_io_read_byte(p_api_dev, addr);
  p_api_dev->byte_data_arr[ 0 ] = bval;

  p_api_dev->b_length = 1;
  p_api_dev->w_length = 0;
  p_api_dev->lw_length = 0;
  p_api_dev->ack_b_ptr = &(p_api_dev->byte_data_arr[ 0 ]);

  return (ackfl);

} /* end: api_buf_ram_rd_byte */

/*****************************************************************************/
/*                                                                           */
/*   Module : API_BUF                  Submodule : API_BUF_DATASET_WRITE     */
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
/*    Inputs:     Dataset Buffer ID [dsid]                                   */
/*                Dataset Words to write [data_p]                            */
/*                                                                           */
/*    Outputs   : Instruction acknowledge type [ackfl]                       */
/*                                                                           */
/*    Description :                                                          */
/*    This function handles the 'API_RAM_DATASET_WR' instruction to write    */
/*    the specified data to the dataset memory area of the API1553-DS.       */
/*                                                                           */
/*****************************************************************************/
L_WORD api_buf_dataset_write( TY_API_DEV *p_api_dev, TY_MIL_COM_LS_BUF_DATASET_WRITE_INPUT * in, TY_MIL_COM_ACK * out )
{
  WORD i;
  WORD volatile *ptr;

  /* Set pointer to Dataset Memory address */
  ptr = (WORD*)API_DATASET_MEM_ADDR_ABS(in->dsid);

  /* Write data to Dataset Memory */
  for(i = 0; i < MAX_API_BUF_SIZE / 2; i++)
  {
    ptr[i] = BSWAP16(in->data[i]);
  }

  return 0;

} /* end: api_buf_dataset_write */

/*****************************************************************************/
/*                                                                           */
/*   Module : API_BUF                  Submodule : API_BUF_DATASET_READ      */
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
/*    Inputs:     Dataset Buffer ID [dsid]                                   */
/*                                                                           */
/*    Outputs   : Instruction acknowledge type [ackfl]                       */
/*                Dataset Words to read [data_p]                             */
/*                                                                           */
/*    Description :                                                          */
/*    This function handles the 'API_RAM_DATASET_RD' instruction to read     */
/*    data from the dataset memory area of the API1553-DS.                   */
/*                                                                           */
/*****************************************************************************/
L_WORD api_buf_dataset_read( TY_API_DEV *p_api_dev, TY_MIL_COM_LS_BUF_DATASET_READ_INPUT  * in, TY_MIL_COM_LS_BUF_DATASET_READ_OUTPUT * out )
{
  WORD volatile *ptr;
  short i;

  /* Set pointer to Dataset Memory address */
  ptr = (WORD*)API_DATASET_MEM_ADDR_ABS(in->dsid);

  /* Read data from Dataset Memory */
  for(i = 0; i < MAX_API_BUF_SIZE / 2; i++)
  {
    out->data[i] = BSWAP16(ptr[i]);
  }

  out->xAck.ulSwapControl = 32 << 16;

  return 0;

} /* end: api_buf_dataset_read */

/*****************************************************************************/
/*                                                                           */
/*   Module : API_BUF                  Submodule : API_BUF_WRITE             */
/*                                                                           */
/*   Author : Marco Motz               Project   : API1553-DS                */
/*                                                                           */
/*   Source : C                        Tools     : PC/AT; Norton Editor;     */
/*                                                 CYGNUS, GNU-C, As, and LD */
/*                                                 IDT-C 5.1 Toolkit         */
/*---------------------------------------------------------------------------*/
/*   Create : 01.07.99   Update : 10.06.03                                   */
/*---------------------------------------------------------------------------*/
/*    Descriptions                                                           */
/*    ------------                                                           */
/*    Inputs:     Buffer Type (BC/RT) [bt]                                   */
/*                Buffer Header ID [hid]                                     */
/*                Data Buffer ID [bid]                                       */
/*                Data Word position in Buffer [d_pos]                       */
/*                Byte position in Data Word [b_pos]                         */
/*                Bit length to write [b_len]                                */
/*                Data Word to write [data]                                  */
/*                                                                           */
/*    Outputs   : Instruction acknowledge type [ackfl]                       */
/*                                                                           */
/*    Description :                                                          */
/*    This function handles the 'API_BUF_WRITE' instruction to write the     */
/*    specified Data Word to the selected Data Buffer within                 */
/*    the API1553-DS Global Memory.                                          */
/*                                                                           */
/*****************************************************************************/
L_WORD api_buf_write( TY_API_DEV *p_api_dev, TY_MIL_COM_LS_BUF_WRITE_INPUT * in, TY_MIL_COM_ACK * out )
{
  WORD buffer_id, index;
  L_WORD hid_max;
  WORD volatile *w_ptr;
  L_WORD addr, tmpl;
  L_WORD ret_val = 0;
  BYTE len, flag;
  WORD w_data, cur_data, prev_data;
  BYTE c1760_ix;
  WORD volatile *c1760_ptr;

  BYTE bt     = (BYTE)in->bt;
  WORD hid    = (WORD)in->hid;
  WORD bid    = (WORD)in->bid;
  BYTE d_pos  = (BYTE)in->word_pos;
  BYTE b_pos  = (BYTE)in->bit_pos;
  BYTE b_len  = (BYTE)in->bit_len;
  WORD data   = (WORD)in->data;

  addr = 0L;
  buffer_id = 0x0000;
  flag = API_ON;

  hid_max = 0;
  if(bt == API_BUF_BC_TYPE)
    hid_max = p_api_dev->glb_mem.biu[ p_api_dev->biu ].count.bc_bh_area;
  else if(bt == API_BUF_RT_TYPE)
    hid_max = p_api_dev->glb_mem.biu[ p_api_dev->biu ].count.rt_bh_area;

  if( ( (bt == API_BUF_BC_TYPE) || (bt == API_BUF_RT_TYPE) ) && (hid < hid_max) && (bid < p_api_dev->glb_mem.sim_buf_count[ p_api_dev->biu ]) && ( (d_pos <= 32) && (d_pos > 0) ) && ( (b_len > 0) && (b_len <= 16) ) )
  {

    if(hid == 0)
    {
      /* Use absolute Buffer ID */
      buffer_id = bid;
      addr = p_api_dev->glb_mem.sim_buf_base_addr[ p_api_dev->biu ];
      addr += (buffer_id *MAX_API_BUF_SIZE);
      addr &= 0x03FFFFFCL;
      w_ptr = (WORD*)API_GLB_MEM_ADDR_ABS(addr);
    }
    else
    {
      /* From Buffer Header */
      index = 0;
      if(bt == API_BUF_BC_TYPE)
      {
        /* BC */
        addr = BSWAP32( (*p_api_dev->bc_bh_desc_p[ p_api_dev->biu ])[ hid ].bp_addr);
        if(bid == 0)
        {
            if(p_api_dev->bc_status[p_api_dev->biu] == API_BUSY)
            {
                UsbSynchronizeLWord(p_api_dev, 
                    API_GLB_MEM_ADDR_REL(&(*p_api_dev->bc_bh_desc_p[ p_api_dev->biu ])[ hid ].bh_ctrl), 
                    In);
            }

            index = (WORD) (BSWAP32( (*p_api_dev->bc_bh_desc_p[ p_api_dev->biu ])[ hid ].bh_ctrl) &0x000000FFL);
        }
        else
          index = bid - 1;
      }
      else
      {
        /* RT */
        addr = BSWAP32((*p_api_dev->rt_bh_desc_p[ p_api_dev->biu ])[ hid ].bp_addr);
        if(bid == 0)
        {
            if(p_api_dev->rt_status[p_api_dev->biu] == API_BUSY)
            {
                UsbSynchronizeLWord(p_api_dev, 
                    API_GLB_MEM_ADDR_REL(&(*p_api_dev->rt_bh_desc_p[ p_api_dev->biu ])[ hid ].bh_ctrl), 
                    In);
            }

            index = (WORD) (BSWAP32( (*p_api_dev->rt_bh_desc_p[ p_api_dev->biu ])[ hid ].bh_ctrl) &0x000000FFL);
        }
        else
          index = bid - 1;
      }

      addr += (index *MAX_API_BUF_SIZE);
      addr &= 0x03FFFFFCL;
      w_ptr = (WORD*)API_GLB_MEM_ADDR_ABS(addr);

      tmpl = addr - p_api_dev->glb_mem.sim_buf_base_addr[ p_api_dev->biu ];
      tmpl /= MAX_API_BUF_SIZE;
      buffer_id = (WORD) (tmpl);
    }

    while(flag == API_ON)
    {
      prev_data = BSWAP16(*(w_ptr + (d_pos - 1) ) );
      w_data = prev_data;

      if( (b_pos + b_len) > 16)
      {
        len = (16-b_pos);
      }
      else
      {
        len = b_len;
      }
      WORD_RESET_BITS(w_data, b_pos, len);
      WORD_SET_BITS(w_data, data, b_pos, len);


      if( (b_pos + b_len) < 9)
      {
         /* Update only Low Byte */

        cur_data = BSWAP16(*(w_ptr + (d_pos - 1) ) );
        if(prev_data == cur_data)
        {
          /* Write data to Global Memory */
          *( (BYTE*) (w_ptr + (d_pos - 1) ) ) = (BYTE)w_data;
          flag = API_OFF;
        }
      }
      else
      {
        if(b_pos > 7)
        {
           /* Update only High Byte */

          cur_data = BSWAP16(*(w_ptr + (d_pos - 1) ) );
          if(prev_data == cur_data)
          {
            /* Write data to Global Memory */
            *( ( (BYTE*) (w_ptr + (d_pos - 1) ) ) + 1) = (BYTE) (w_data >> 8);
            flag = API_OFF;
          }
        }
        else
        {

          cur_data = BSWAP16(*(w_ptr + (d_pos - 1) ) ); /* Update 16Bit Data */
          if(prev_data == cur_data)
          {
            /* Write data to Global Memory */
            *(w_ptr + (d_pos - 1) ) = BSWAP16(w_data);
            flag = API_OFF;
          }
        }
      }
    }
    /* 13.10.99 */
    /* 1760 Protocol Handling */
    c1760_ptr = w_ptr;
    if(api_io_c1760_mode_used(p_api_dev, (BYTE)p_api_dev->biu) == API_OK)
    {
      /* 1760 Buffer */
      if(api_io_c1760_buf_get(p_api_dev, (BYTE)p_api_dev->biu, buffer_id, &c1760_ix) == API_OK)
      {
        /* 1760 Checksum Handling */
        api_io_c1760_buf_set(p_api_dev, (BYTE)p_api_dev->biu, c1760_ix, c1760_ptr);
      }
    }

    if(p_api_dev->bc_status[p_api_dev->biu] == API_BUSY && bt == API_BUF_BC_TYPE)
    {
        UsbSynchronizeMemoryArea(p_api_dev, API_GLB_MEM_ADDR_REL(w_ptr + (d_pos - 1)), 2, 
                                 Out);
    }
    else if(p_api_dev->rt_status[p_api_dev->biu] == API_BUSY && bt == API_BUF_RT_TYPE)
    {
        UsbSynchronizeMemoryArea(p_api_dev, API_GLB_MEM_ADDR_REL(w_ptr + (d_pos - 1)), 2, 
                                 Out);
    }
  }
  else
  {
    ret_val = API_ERR_PARAMETER_OUT_OF_RANGE;
  }

  return ret_val;

} /* end: api_buf_write */


/*****************************************************************************/
/*                                                                           */
/*    Descriptions                                                           */
/*    ------------                                                           */
/*    Inputs:     Buffer Type (BC/RT) [bt]                                   */
/*                Buffer Header ID [hid]                                     */
/*                Data Buffer ID [bid]                                       */
/*                Data Buffer Offset [offset]                                */
/*                Amount of Data Words to write [len]                        */
/*                Data Words to write [data_p]                               */
/*                                                                           */
/*    Outputs   : Instruction acknowledge type [ackfl]                       */
/*                                                                           */
/*    Description :                                                          */
/*    This function handles the 'API_BUF_WRITE_BLOCK' instruction            */
/*    to write the specified amount of Data Words to the selected            */
/*    Data Buffer within the Global Memory.                                  */
/*                                                                           */
/*****************************************************************************/
L_WORD api_buf_write_block(TY_API_DEV* p_api_dev, TY_MIL_COM_LS_BUF_WRITE_BLOCK_INPUT* in, TY_MIL_COM_ACK* out)
{
    L_WORD hid_max = 0;
    L_WORD addr = 0;
    L_WORD tmpl = 0;
    L_WORD ret_val = 0;

    WORD volatile* c1760_ptr = NULL;
    WORD volatile* ptr = NULL;

    WORD buffer_id = 0;
    BYTE c1760_ix = 0;
    WORD index = 0;
    WORD i = 0;

    if (in->bt == API_BUF_BC_TYPE)
        hid_max = p_api_dev->glb_mem.biu[p_api_dev->biu].count.bc_bh_area;
    else if (in->bt == API_BUF_RT_TYPE)
        hid_max = p_api_dev->glb_mem.biu[p_api_dev->biu].count.rt_bh_area;

    if (     ((in->bt == API_BUF_BC_TYPE) || (in->bt == API_BUF_RT_TYPE)) 
          && (in->hid < hid_max) 
          && ((in->bid < p_api_dev->glb_mem.sim_buf_count[p_api_dev->biu]) || (in->bid == 0xffff))
          && (in->count >= 1) 
          && ((in->offset + in->count) <= 32))
    {

        if (in->hid == 0)
        {
            /* Use absolute Buffer ID */
            buffer_id = (WORD)in->bid;
            addr = API_GLB_GLOBREL_TO_PBIREL(p_api_dev->glb_mem.sim_buf_base_addr[p_api_dev->biu]);
            addr += (buffer_id * MAX_API_BUF_SIZE);
            addr &= 0x03FFFFFCL;
            ptr = (WORD*)API_GLB_PBIREL_TO_TSWPTR(addr);
        }
        else
        {
            /* From Buffer Header */
            index = 0;
            if (in->bt == API_BUF_BC_TYPE)
            {
                /* BC */
                addr = BSWAP32((*p_api_dev->bc_bh_desc_p[p_api_dev->biu])[in->hid].bp_addr);
                if (in->bid == 0)
                {
                    /* Use current index */
                    index = (WORD)(BSWAP32((*p_api_dev->bc_bh_desc_p[p_api_dev->biu])[in->hid].bh_ctrl) & 0x000000FFL);
                }
                else if (0xffff == in->bid)
                {
                    /* Use next index */
                    index = (WORD)(BSWAP32((*p_api_dev->bc_bh_desc_p[p_api_dev->biu])[in->hid].bh_ctrl) & 0x000000FFL);
                    tmpl = (1 << ((BSWAP32((*p_api_dev->bc_bh_desc_p[p_api_dev->biu])[in->hid].bh_ctrl) & 0x0003C000) >> 14)) - 1;
                    if (index == tmpl)
                        index = 0;
                    else
                        index++;
                }
                else
                {
                    /* Use buffer queue index */
                    index = in->bid - 1;
                }
            }
            else
            {
                /* RT */
                addr = BSWAP32((*p_api_dev->rt_bh_desc_p[p_api_dev->biu])[in->hid].bp_addr);
                if (in->bid == 0)
                {
                    /* Use current inde x*/
                    index = (WORD)(BSWAP32((*p_api_dev->rt_bh_desc_p[p_api_dev->biu])[in->hid].bh_ctrl) & 0x000000FFL);
                }
                else if (0xffff == in->bid)
                {
                    /* Use next index */
                    index = (WORD)(BSWAP32((*p_api_dev->rt_bh_desc_p[p_api_dev->biu])[in->hid].bh_ctrl) & 0x000000FFL);
                    tmpl = (1 << ((BSWAP32((*p_api_dev->rt_bh_desc_p[p_api_dev->biu])[in->hid].bh_ctrl) & 0x0003C000) >> 14)) - 1;
                    if (index == tmpl)
                        index = 0;
                    else
                        index++;
                }
                else
                {
                    /* Use buffer queue index */
                    index = in->bid - 1;
                }
            }

            addr += (index * MAX_API_BUF_SIZE);
            addr &= (0x03FFFFFCL);
            ptr = (WORD*)API_GLB_PBIREL_TO_TSWPTR(addr);

            tmpl = addr - API_GLB_GLOBREL_TO_PBIREL(p_api_dev->glb_mem.sim_buf_base_addr[p_api_dev->biu]);
            tmpl /= MAX_API_BUF_SIZE;
            buffer_id = (WORD)(tmpl);
        }

        /* Write data to buffer */
        c1760_ptr = ptr;
        for (i = 0; i < in->count; i++)
        {
            ptr[i+in->offset] = BSWAP16(in->data[i]);
        }

        /* 1760 Protocol Handling */
        if (api_io_c1760_mode_used(p_api_dev, (BYTE)p_api_dev->biu) == API_OK)
        {
            /* 1760 Buffer */
            if (api_io_c1760_buf_get(p_api_dev, (BYTE)p_api_dev->biu, buffer_id, &c1760_ix) == API_OK)
            {
                /* 1760 Checksum Handling */
                api_io_c1760_buf_set(p_api_dev, (BYTE)p_api_dev->biu, c1760_ix, c1760_ptr);
            }
        }

        if (p_api_dev->bc_status[p_api_dev->biu] == API_BUSY && in->bt == API_BUF_BC_TYPE)
        {
            UsbSynchronizeMemoryArea(p_api_dev, API_GLB_PBIREL_TO_GLOBREL(addr), MAX_API_BUF_SIZE, Out);
        }
        else if (p_api_dev->rt_status[p_api_dev->biu] == API_BUSY && in->bt == API_BUF_RT_TYPE)
        {
            UsbSynchronizeMemoryArea(p_api_dev, API_GLB_PBIREL_TO_GLOBREL(addr), MAX_API_BUF_SIZE, Out);
        }
    }
    else
    {
        ret_val = API_ERR_PARAMETER_OUT_OF_RANGE;
    }

    return ret_val;

} /* end: api_buf_write_block */


/*****************************************************************************/
/*                                                                           */
/*    Descriptions                                                           */
/*    ------------                                                           */
/*    Inputs:     Buffer Type (BC/RT) [bt]                                   */
/*                Buffer Header ID [hid]                                     */
/*                Control [con]                                              */
/*                                                                           */
/*    Outputs   : Instruction acknowledge type [ackfl]                       */
/*                                                                           */
/*    Description :                                                          */
/*    This function handles the 'API_BUF_HOST_CONTROL' instruction           */
/*    to advance the current buffer index to the next buffer queue index.    */
/*                                                                           */
/*****************************************************************************/
L_WORD api_buf_host_control(TY_API_DEV* p_api_dev, TY_MIL_COM_LS_BUF_HOST_CONTROL_INPUT* in, TY_MIL_COM_ACK* out)
{
    L_WORD hid_max = 0;
    L_WORD ret_val = 0;

    L_WORD bh_offset = 0;

    if (in->bt == API_BUF_BC_TYPE)
        hid_max = p_api_dev->glb_mem.biu[p_api_dev->biu].count.bc_bh_area;
    else if (in->bt == API_BUF_RT_TYPE)
        hid_max = p_api_dev->glb_mem.biu[p_api_dev->biu].count.rt_bh_area;

    if (((in->bt == API_BUF_BC_TYPE) || (in->bt == API_BUF_RT_TYPE)) && (in->hid < hid_max))
    {
        /* From Buffer Header */
        if (in->bt == API_BUF_BC_TYPE)
        {
            /* BC */
            bh_offset = API_GLB_TSWPTR_TO_GLOBREL(p_api_dev->bc_bh_desc_p[p_api_dev->biu]);
        }
        else
        {
            /* RT */
            bh_offset = API_GLB_TSWPTR_TO_GLOBREL(p_api_dev->rt_bh_desc_p[p_api_dev->biu]);
        }

        bh_offset = bh_offset + in->hid * 0x10;

        /* store buffer header offset in system command input word */
        (*p_api_dev->cb_p[ p_api_dev->biu ]).cmd_inout = bh_offset;
        wbflush();
        UsbSynchronizeLWord(p_api_dev, API_GLB_MEM_ADDR_REL(&p_api_dev->cb_p[p_api_dev->biu]->cmd_inout), Out);
  
        if (ExecuteFirmwareCommand(p_api_dev, p_api_dev->biu, API_SCB_COM_HOST_CTRL_INCR) != 0)
            ret_val = API_ERR_TARGET_RESPONSE_TIMEOUT;
    }
    else
    {
        ret_val = API_ERR_PARAMETER_OUT_OF_RANGE;
    }

    return ret_val;

} /* end: api_buf_host_control */


/*****************************************************************************/
/*                                                                           */
/*   Module : API_BUF                  Submodule : API_BUF_C1760_CON         */
/*                                                                           */
/*   Author : Marco Motz               Project   : API1553-DS                */
/*                                                                           */
/*   Source : C                        Tools     : PC/AT; Norton Editor;     */
/*                                                 CYGNUS, GNU-C, As, and LD */
/*                                                 IDT-C 5.1 Toolkit         */
/*---------------------------------------------------------------------------*/
/*   Create : 13.10.99   Update : 13.10.99                                   */
/*---------------------------------------------------------------------------*/
/*    Descriptions                                                           */
/*    ------------                                                           */
/*    Inputs:     tbd                                                        */
/*                                                                           */
/*    Outputs   : Instruction acknowledge type [ackfl]                       */
/*                                                                           */
/*    Description :                                                          */
/*    This function handles the 'API_BUF_C1760_CON' instruction.             */
/*                                                                           */
/*****************************************************************************/
L_WORD api_buf_c1760_con( TY_API_DEV *p_api_dev, TY_MIL_COM_LS_BUF_C1760_CON_INPUT * in, TY_MIL_COM_ACK * out )
{
  return api_io_c1760_con(p_api_dev, (BYTE)p_api_dev->biu, (BYTE)in->mode, (WORD)in->buf_id, (BYTE)in->c01, in->c02);
}




/*****************************************************************************/
/*                                                                           */
/*   Module : API_BUF                  Submodule : API_BUF                   */
/*                                                                           */
/*   Author : Marco Motz               Project   : API1553-DS                */
/*                                                                           */
/*   Source : C                        Tools     : PC/AT; Norton Editor;     */
/*                                                 CYGNUS, GNU-C, As, and LD */
/*                                                 IDT-C 5.1 Toolkit         */
/*---------------------------------------------------------------------------*/
/*   Create : 01.05.98   Update : 13.10.99                                   */
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
/*    Entry function for the API1553-DS Buffer Handling related operations.  */
/*    This function is utilized to decode the Buffer instruction information */
/*    and to activate the appropriate handler routine. After execution of    */
/*    the instruction, acknowledge information is returned.                  */
/*                                                                           */
/*    Instructions:                                                          */
/*    - API_RAM_WRITE:            Direct Write Global Memory                 */
/*    - API_RAM_READ:             Direct Read Global Memory                  */
/*    - API_RAM_LWORD_WR:         Direct Write Longword                      */
/*    - API_RAM_LWORD_RD:         Direct Read Longword                       */
/*    - API_RAM_WORD_WR:          Direct Write Word                          */
/*    - API_RAM_WORD_RD:          Direct Read Word                           */
/*    - API_RAM_BYTE_WR:          Direct Write Byte                          */
/*    - API_RAM_BYTE_RD:          Direct Read Byte                           */
/*                                                                           */
/*****************************************************************************/

BYTE api_buf(TY_API_DEV *p_api_dev, BYTE cmd, WORD size, BYTE *info_p)
{
  BYTE ackfl, bsize, bval;
  WORD cmp_size, wval;
  L_WORD laddr, lval;

  ackfl = API_NAK;

  switch(cmd)
  {
    case API_RAM_WRITE:
      bsize = *(info_p + 4);
      laddr = MAKE_B2LWORD(*info_p, *(info_p + 1), *(info_p + 2), *(info_p + 3) );
      cmp_size = bsize;
      cmp_size *= 2;
      cmp_size += 9;
      if(size == cmp_size)
        ackfl = api_buf_ram_write(p_api_dev, laddr, bsize, info_p + 5);
      break;

    case API_RAM_READ:
      bsize = *(info_p + 4);
      laddr = MAKE_B2LWORD(*info_p, *(info_p + 1), *(info_p + 2), *(info_p + 3) );
      if(size == 9)
        ackfl = api_buf_ram_read(p_api_dev, laddr, bsize);
      break;

    case API_RAM_LWORD_WR:
      laddr = MAKE_B2LWORD(*(info_p + 0), *(info_p + 1), *(info_p + 2), *(info_p + 3) );
      lval = MAKE_B2LWORD(*(info_p + 4), *(info_p + 5), *(info_p + 6), *(info_p + 7) );
      if(size == 12)
        ackfl = api_buf_ram_wr_lword(p_api_dev, laddr, lval);
      break;

    case API_RAM_LWORD_RD:
      laddr = MAKE_B2LWORD(*(info_p + 0), *(info_p + 1), *(info_p + 2), *(info_p + 3) );
      if(size == 8)
        ackfl = api_buf_ram_rd_lword(p_api_dev, laddr);
      break;

    case API_RAM_WORD_WR:
      laddr = MAKE_B2LWORD(*(info_p + 0), *(info_p + 1), *(info_p + 2), *(info_p + 3) );
      wval = MAKE_WORD(*(info_p + 4), *(info_p + 5) );
      if(size == 10)
        ackfl = api_buf_ram_wr_word(p_api_dev, laddr, wval);
      break;

    case API_RAM_WORD_RD:
      laddr = MAKE_B2LWORD(*(info_p + 0), *(info_p + 1), *(info_p + 2), *(info_p + 3) );
      if(size == 8)
        ackfl = api_buf_ram_rd_word(p_api_dev, laddr);
      break;

    case API_RAM_BYTE_WR:
      laddr = MAKE_B2LWORD(*(info_p + 0), *(info_p + 1), *(info_p + 2), *(info_p + 3) );
      bval = *(info_p + 4);
      if(size == 9)
        ackfl = api_buf_ram_wr_byte(p_api_dev, laddr, bval);
      break;

    case API_RAM_BYTE_RD:
      laddr = MAKE_B2LWORD(*(info_p + 0), *(info_p + 1), *(info_p + 2), *(info_p + 3) );
      if(size == 8)
        ackfl = api_buf_ram_rd_byte(p_api_dev, laddr);
      break;

    default:
      break;
  }

  return (ackfl);

} /* end: api_buf */

NAMESPACE1553_END

