/* SPDX-FileCopyrightText: 2001-2022 AIM GmbH <info@aim-online.com> */
/* SPDX-License-Identifier: MIT OR GPL-2.0-or-later */

/*! \file api_ir.c
    Interrupt Handler module of the AYI1553 ASP Driver SW.
*/

#include "mil_tsw_includes.h"
#include "api_ir.h"
#include "api_int_loglist.h"
#include "mil_buffer.h"
#include "api_defs.h"


#ifdef _NUCLEUS
  #ifdef _AMC1553
    #define CPU_CLK_DEF		CORE_FREQ
  #else 
    extern NU_HISR hisr_DmaComplete;
  #endif 
#endif 





#define API_DATAQUEUE_HEADER(x) ((TY_API_DATA_QUEUE_HEADER *)API_SHARED_MEM_ADDR_ABS(p_api_dev->dataqueue_header_offsets[x]));

#define API_ISR_USEC_STEP 500L
#define API_ISR_RTI_SEC 2000L
#define API_ISR_USEC_TM(tm) ((CPU_CLK_DEF / 2) * tm)


/* Byte-Swap full 32-bit word */
#define ISWAP32(X)  ((((X) & 0xFFL)<<24) + (((X) & 0xFF00L)<<8) + (((X) & 0xFF0000L)>>8) + (((X) & 0xFF000000L)>>24))
/* Byte-Swap Words within the 32-bit word */
#define ISWAP16(X)  ((((X) & 0xFFL)<<8)  + (((X) & 0xFF00L)>>8) + (((X) & 0xFF0000L)<<8) + (((X) & 0xFF000000L)>>8))    


NAMESPACE1553_START

L_WORD set_lock(TY_API_DEV *p_api_dev, L_WORD volatile *ptr);
void release_lock(TY_API_DEV *p_api_dev, L_WORD volatile *ptr);

/*****************************************************************************/
/*                                                                           */
/*   Module : API_IR                   Submodule : API_IR_MODE_CON           */
/*                                                                           */
/*   Author : Marco Motz               Project   : API1553-DS                */
/*                                                                           */
/*   Source : C                        Tools     : PC/AT; Norton Editor;     */
/*                                                 CYGNUS, GNU-C, As, and LD */
/*                                                 IDT-C 5.1 Toolkit         */
/*---------------------------------------------------------------------------*/
/*   Create : 01.05.98   Update : 25.05.00                                   */
/*---------------------------------------------------------------------------*/
/*    Descriptions                                                           */
/*    ------------                                                           */
/*    Inputs    : API1553-DS mode (RT/BC/SFT/CAL) [mode],                    */
/*                API1553-DS mode control (Enable/Disable) [con]             */
/*                                                                           */
/*    Outputs   : none                                                       */
/*                                                                           */
/*    Description :                                                          */
/*    This function is used to enable or disable the specified API1553-DS    */
/*    operational mode within the ISR.                                       */
/*                                                                           */
/*****************************************************************************/

void api_ir_mode_con(TY_API_DEV *p_api_dev, short dest_biu, BYTE mode, BYTE con)
{
  L_WORD stat;

  stat = BSWAP32( (*p_api_dev->cb_p[ dest_biu ]).ssw);
  wbflush(); /* System Status Word */

  switch(mode)
  {
    case API_RT_MODE:
      break;

      /* 25.05.00 */
    case API_BC_MODE:
      if(con == API_ON)
      {

        /* BC Operation stopped before ? */
        if( (stat &API_SSW_BCE_BIT) == 0L)
        {
            /* Enable BC Operation */
            ExecuteFirmwareCommand(p_api_dev, dest_biu, API_SCB_COM_ENA_BC);
        }
      }
      else
      {

        /* BC Operation started before ? */
        if( (stat &API_SSW_BCE_BIT) == API_SSW_BCE_BIT)
        {
            /* Disable BC Operation */
            ExecuteFirmwareCommand(p_api_dev, dest_biu, API_SCB_COM_DIS_BC);
        }

        if(p_api_dev->bc_status[p_api_dev->biu] == API_BUSY)
        {
            UsbSynchronizeBC(p_api_dev, p_api_dev->biu, In);
        }
      }
      break;

    case API_BM_MODE:
      if(con == API_ON)
      {

        /* BM Operation stopped before ? */
        if( (stat &API_SSW_MNE_BIT) == 0L)
        {
            /* Enable BM Operation */
            ExecuteFirmwareCommand(p_api_dev, dest_biu, API_SCB_COM_ENA_BC);
        }
      }
      else
      {

        /* BM Operation started before ? */
        if( (stat &API_SSW_MNE_BIT) == API_SSW_MNE_BIT)
        {
            /* Disable BM Operation */
            ExecuteFirmwareCommand(p_api_dev, dest_biu, API_SCB_COM_DIS_BM);
        }
      }
      break;

      /* 01.08.99 */
    case API_REP_MODE:
      if(con == API_ON)
      {
        /* REP Operation stopped before ? */
        if( (stat &API_SSW_RPE_BIT) == 0L)
        {
            /* Enable REP Operation */
            ExecuteFirmwareCommand(p_api_dev, dest_biu, API_SCB_COM_ENA_REP);
        }
      }
      else
      {

        /* REP Operation started before ? */
        if( (stat &API_SSW_RPE_BIT) == API_SSW_RPE_BIT)
        {
            /* Disable BM Operation */
            ExecuteFirmwareCommand(p_api_dev, dest_biu, API_SCB_COM_DIS_REP);
        }
      }
      break;

    default:
      break;
  }

} /* end: api_ir_mode_con */

/*****************************************************************************/
/*                                                                           */
/*   Module : API_IR                   Submodule : API_TIMER0_ISR            */
/*                                                                           */
/*   Author : Michael Stahl            Project   : API1553-DS                */
/*                                                                           */
/*   Source : C                        Tools     : PC/AT; Norton Editor;     */
/*                                                 CYGNUS, GNU-C, As, and LD */
/*                                                 IDT-C 5.1 Toolkit         */
/*---------------------------------------------------------------------------*/
/*   Create : 13.10.99   Update : 13.10.99                                   */
/*---------------------------------------------------------------------------*/
/*    Descriptions                                                           */
/*    ------------                                                           */
/*    Inputs    : none                                                       */
/*                                                                           */
/*    Outputs   : none                                                       */
/*                                                                           */
/*    Description :                                                          */
/*    NOTE: DO NOT USE FLOATING POINT CALCULATION IN THIS ISR !!!            */
/*                                                                           */
/*****************************************************************************/
#ifndef _AMC1553

  #ifndef _NUCLEUS
void api_timer0_isr(TY_API_DEV *p_api_dev, L_WORD cpu_cause, L_WORD cpu_stat, EXFRAME_TY *exc)
{


  /* Timer 0 */
  if(p_api_dev->timer_intr[ 0 ].tena == API_ON)
  {

    if(++p_api_dev->timer_intr[ 0 ].isr_cnt >= p_api_dev->timer_intr[ 0 ].predev)
    {
      p_api_dev->timer_intr[ 0 ].tcnt++;
      p_api_dev->timer_intr[ 0 ].isr_cnt = 0L;
    }
  }
  /* Timer 1 */
  if(p_api_dev->timer_intr[ 1 ].tena == API_ON)
  {

    if(++p_api_dev->timer_intr[ 1 ].isr_cnt >= p_api_dev->timer_intr[ 1 ].predev)
    {
      p_api_dev->timer_intr[ 1 ].tcnt++;
      p_api_dev->timer_intr[ 1 ].isr_cnt = 0L;
    }
  }
  /* Timer 2 */
  if(p_api_dev->timer_intr[ 2 ].tena == API_ON)
  {

    if(++p_api_dev->timer_intr[ 2 ].isr_cnt >= p_api_dev->timer_intr[ 2 ].predev)
    {
      p_api_dev->timer_intr[ 2 ].tcnt++;
      p_api_dev->timer_intr[ 2 ].isr_cnt = 0L;
    }
  }
  /* Timer 3 */
  if(p_api_dev->timer_intr[ 3 ].tena == API_ON)
  {
    if(++p_api_dev->timer_intr[ 3 ].isr_cnt >= p_api_dev->timer_intr[ 3 ].predev)
    {
      p_api_dev->timer_intr[ 3 ].tcnt++;
      p_api_dev->timer_intr[ 3 ].isr_cnt = 0L;
    }
  }

} /* end: api_timer0_isr */
  #endif 
#endif 


/*****************************************************************************/
/*                                                                           */
/*   Module : API_IR                   Submodule : API_IR_SYSTAG             */
/*                                                                           */
/*   Author : Patrick Giesel           Project   : API1553-DS                */
/*                                                                           */
/*   Source : C                        Tools     : PC/AT; Norton Editor;     */
/*                                                 CYGNUS, GNU-C, As, and LD */
/*                                                 IDT-C 5.1 Toolkit         */
/*---------------------------------------------------------------------------*/
/*   Create : 09.11.05   Update : 15.05.07                                   */
/*---------------------------------------------------------------------------*/
/*    Descriptions                                                           */
/*    ------------                                                           */
/*    Inputs    :                                                            */
/*                                                                           */
/*    Outputs   : none                                                       */
/*                                                                           */
/*    Description :                                                          */
/*                                                                           */
/*****************************************************************************/

void api_ir_calculate_checksum( TY_API_DEV *p_api_dev, short dest_biu, WORD systag_id, L_WORD dbp )
{
WORD dw, bufw[32], min, max, step, wpos, chksum, k;
WORD volatile *src_p;
WORD volatile *dbp_p;
  
  if(p_api_dev->ls_systag_setup[ dest_biu ].dytag[ systag_id ].fct == API_SYSTAG_CHECKSUM)
  {
    if(p_api_dev->ls_systag_setup[ dest_biu ].dytag[ systag_id ].run_mode == API_SYSTAG_RUN_RESUME)
    {
      step = p_api_dev->ls_systag_setup[ dest_biu ].dytag[ systag_id ].step; /* Checksum calculation function (Plus or Xor) */
      wpos = p_api_dev->ls_systag_setup[ dest_biu ].dytag[ systag_id ].wpos; /* Destination */
      min  = p_api_dev->ls_systag_setup[ dest_biu ].dytag[ systag_id ].min;  /* Source (Start) */
      max  = p_api_dev->ls_systag_setup[ dest_biu ].dytag[ systag_id ].max;  /* Source (End) */

      if (max >= min)
      {
        chksum = 0;

        /* Destination Pointer */
        dbp_p = (WORD*)API_GLB_MEM_ADDR_ABS(dbp + 2 * wpos);

        /* Source Pointer */
        src_p = (WORD*)API_GLB_MEM_ADDR_ABS(dbp + 2 * min);

        /* Calculate Checksum */
        switch(step)
        {
          case 0:

            /* PLUS */
            do
            {
              /* Read physical Value */
              dw = BSWAP16(*src_p);

              /* Build Checksum */
              chksum += dw;

              /* next word */
              src_p++;
              min++;
            } while(max >= min);

            /* Init Checksum word */
            *dbp_p = BSWAP16(chksum);
            break;

          case 1:

            /* XOR */
            do
            {
              /* Read physical Value */
              dw = BSWAP16(*src_p);

              /* Build Checksum */
              chksum ^= dw;

              /* next word */
              src_p++;
              min++;
            } while(max >= min);

            /* Init Checksum word */
            *dbp_p = BSWAP16(chksum);
            break;

          case 2: 
            
            /* 1760 */
          
            /* Copy buffer data */
            for(k=0; (k<wpos) && (k<32); k++) {
               bufw[k] = BSWAP16(*src_p);
               /* next word */
               src_p++;
            }

            /* Generate C01 checksum */
            chksum = api_io_c1760_calc_c01_fmt(p_api_dev, (WORD*)bufw, (BYTE)wpos);

            /* Init Checksum word */
            *dbp_p = BSWAP16(chksum);
            break;  

          default:
            break;
        }
      }
    }
  }
  
} /* end: api_ir_calculate_checksum */

/*****************************************************************************/
/*                                                                           */
/*   Module : API_IR                   Submodule : API_IR_SYSTAG             */
/*                                                                           */
/*   Author : Marco Motz               Project   : API1553-DS                */
/*                                                                           */
/*   Source : C                        Tools     : PC/AT; Norton Editor;     */
/*                                                 CYGNUS, GNU-C, As, and LD */
/*                                                 IDT-C 5.1 Toolkit         */
/*---------------------------------------------------------------------------*/
/*   Create : 25.05.00   Update : 15.10.05                                   */
/*---------------------------------------------------------------------------*/
/*    Descriptions                                                           */
/*    ------------                                                           */
/*    Inputs    :                                                            */
/*                                                                           */
/*    Outputs   : none                                                       */
/*                                                                           */
/*    Description :                                                          */
/*                                                                           */
/*****************************************************************************/

#ifdef CMPL4SIM
void api_ir_systag(TY_API_DEV *p_api_dev, short dest_biu, BYTE bc_rt_mode, WORD xid_rtsa, L_WORD dbp)
{
  WORD dw, wval, min, max, step, mask, wpos, bpos, index, dytag_id, cnt,i,j,k;
  WORD dataset_id, current_id, dataset_cnt, dataset_mode;
  WORD states_cnt, states_mode, states_index;
  WORD volatile *src_p;
  WORD volatile *dbp_p;
  BYTE *ptr;
  short cmpl_found;
  short chksum_found;

  dw = 0;
  wval = 0;
  states_cnt = 0;
  states_mode = 0;
  states_index = 0;

  cmpl_found = 0;
  chksum_found = 0;

  dbp_p = (WORD*)API_GLB_MEM_ADDR_ABS_IR(0,dest_biu);
  src_p = (WORD*)API_DATASET_MEM_ADDR_ABS_IR(0);

  if (bc_rt_mode == API_BC_MODE)
  {
    /* BC Mode */
    dytag_id = p_api_dev->bc_setup[ dest_biu ].bc_systag[ xid_rtsa ].id;
    cnt = p_api_dev->bc_setup[ dest_biu ].bc_systag[ xid_rtsa ].cnt;
    ptr = p_api_dev->bc_setup[ dest_biu ].bc_systag[ xid_rtsa ].p;
  }   
  else
  {
    /* RT Mode */
    dytag_id = p_api_dev->rt_setup[ dest_biu ].rt_systag[ xid_rtsa >> 8 ].sa[ xid_rtsa & 0x00FF ].id;
    cnt = p_api_dev->rt_setup[ dest_biu ].rt_systag[ xid_rtsa >> 8 ].sa[ xid_rtsa & 0x00FF ].cnt;
    ptr = p_api_dev->rt_setup[ dest_biu ].rt_systag[ xid_rtsa >> 8 ].sa[ xid_rtsa & 0x00FF ].p;
  }

  UsbSynchronizeMemoryArea(p_api_dev, dbp, MAX_API_BUF_SIZE, In);

  if ((cnt != 0) && (ptr != NULL))
  {
    /***************************************/
    /* Single Word Functions               */
    /***************************************/

    for(k=0; k<cnt; k++) 
    {
      /* Get Systag Index from List */
      j = *(ptr+k)-1; 

      /* Abort on out of range index */
      if (j >= MAX_API_SYS_LS_SYSTAGS)
        break;

      if(p_api_dev->ls_systag_setup[ dest_biu ].dytag[ j ].run_mode == API_SYSTAG_RUN_RESUME)
      {
        min = p_api_dev->ls_systag_setup[ dest_biu ].dytag[ j ].min;
        max = p_api_dev->ls_systag_setup[ dest_biu ].dytag[ j ].max;
        step = p_api_dev->ls_systag_setup[ dest_biu ].dytag[ j ].step;
        mask = p_api_dev->ls_systag_setup[ dest_biu ].dytag[ j ].mask;
        wpos = p_api_dev->ls_systag_setup[ dest_biu ].dytag[ j ].wpos;
        bpos = p_api_dev->ls_systag_setup[ dest_biu ].dytag[ j ].bpos;
        index = p_api_dev->ls_systag_setup[ dest_biu ].dytag[ j ].index;

        /* Read physical Word */
        switch(p_api_dev->ls_systag_setup[ dest_biu ].dytag[ j ].fct)
        {
          case API_SYSTAG_TRIANG_INC:
          case API_SYSTAG_TRIANG_DEC:
          case API_SYSTAG_RAMP_INC:
          case API_SYSTAG_RAMP_DEC:
            dbp_p = (WORD*)API_GLB_MEM_ADDR_ABS_IR(dbp + 2 * wpos, dest_biu);
            dw = BSWAP16(*dbp_p); /* read value from buffer */
            wval = (dw & mask) >> bpos;
            dw = dw & ~mask;
            break;

          case API_SYSTAG_STATES:
            dataset_id = min;
            states_cnt = max;
            states_mode = step;
            states_index = index;

            /* Set pointer to Dataset Memory address */
            src_p = (WORD*)API_DATASET_MEM_ADDR_ABS_IR(dataset_id);

            /* Set pointer to Global Memory address */
            dbp_p = (WORD*)API_GLB_MEM_ADDR_ABS_IR(dbp + 2 * wpos, dest_biu);
            dw = BSWAP16(*dbp_p); /* read value from buffer */
            dw = dw & ~mask;
            break;

          case API_SYSTAG_COMP:
            cmpl_found = 1; continue;

          case API_SYSTAG_CHECKSUM:
            chksum_found = 1; continue;

          default:
            break;
        }

        /* Calculation */
        switch(p_api_dev->ls_systag_setup[ dest_biu ].dytag[ j ].fct)
        {
          case API_SYSTAG_RAMP_INC:
            if(wval < max)
            {
              if( ( (wval + step) > max) || ( (wval + step) < wval) )
                wval = max;
              else
                wval += step;
            }
            else
              wval = min;
            dw |= ( (wval << bpos) &mask);
            *dbp_p = BSWAP16(dw);
            break;

          case API_SYSTAG_RAMP_DEC:
            if(wval > min)
            {
              if( ( (wval - step) < min) || ( (wval - step) > wval) )
                wval = min;
              else
                wval -= step;
            }
            else
              wval = max;
            dw |= ( (wval << bpos) &mask);
            *dbp_p = BSWAP16(dw);
            break;

          case API_SYSTAG_TRIANG_INC:
            if(wval < max)
            {
              if( ( (wval + step) > max) || ( (wval + step) < wval) )
                wval = max;
              else
                wval += step;
            }
            else
            {
              /* Switch mode */
              wval -= step;
              p_api_dev->ls_systag_setup[ dest_biu ].dytag[ j ].fct = API_SYSTAG_TRIANG_DEC;
            }
            dw |= ( (wval << bpos) &mask);
            *dbp_p = BSWAP16(dw);
            break;

          case API_SYSTAG_TRIANG_DEC:
            if(wval > min)
            {
              if( ( (wval - step) < min) || ( (wval - step) > wval) )
                wval = min;
              else
                wval -= step;
            }
            else
            {
              /* Switch mode */
              wval += step;
              p_api_dev->ls_systag_setup[ dest_biu ].dytag[ j ].fct = API_SYSTAG_TRIANG_INC;
            }
            dw |= ( (wval << bpos) &mask);
            *dbp_p = BSWAP16(dw);
            break;

          case API_SYSTAG_STATES:
            /* Copy Word from Dataset to Global Memory Buffer */
            dw |= ( (BSWAP16(*(src_p + states_index)) << bpos) & mask); /* create initial value */
            *dbp_p = BSWAP16(dw);

            if( (states_mode == 0) || (states_mode == 1) )
            {
              if(states_index < (states_cnt - 1) )
              {
                /* Increment current index */
                p_api_dev->ls_systag_setup[ dest_biu ].dytag[ j ].index++;
              }
              else
              {
                if(states_mode == 0)
                {
                  /* Cyclic State mode */
                  /* wrap-around */
                  p_api_dev->ls_systag_setup[ dest_biu ].dytag[ j ].index = 0;
                }
              }
            }
            break;

          default:
            break;
        }
      }
    }

    /**************************/
    /* Complement Functions   */
    /**************************/
    if (cmpl_found) 
    {
      for(k=0; k<cnt; k++) 
      {
        /* Get Systag Index from List */
        j = *(ptr+k)-1; 

        if(p_api_dev->ls_systag_setup[ dest_biu ].dytag[ j ].fct == API_SYSTAG_COMP)
        {
          if(p_api_dev->ls_systag_setup[ dest_biu ].dytag[ j ].run_mode == API_SYSTAG_RUN_RESUME)
          {
            min  = p_api_dev->ls_systag_setup[ dest_biu ].dytag[ j ].min;  /* Source Wpos */
            step = p_api_dev->ls_systag_setup[ dest_biu ].dytag[ j ].step; /* Complement Number */
            wpos = p_api_dev->ls_systag_setup[ dest_biu ].dytag[ j ].wpos; /* Destination Wpos */

            /* Destination Pointer */
            dbp_p = (WORD*)API_GLB_MEM_ADDR_ABS_IR(dbp + 2 * wpos, dest_biu);

            /* Source Pointer */
            src_p = (WORD*)API_GLB_MEM_ADDR_ABS_IR(dbp + 2 * min, dest_biu);

            /* Read Source Word and calculate complement */
            dw = BSWAP16(*src_p);
            switch(step)
            {
              case 1:
                /* 1's complement */
                *dbp_p = BSWAP16(~dw);
                break;
              case 2:
                /* 2's complement */
                *dbp_p = BSWAP16(~dw + 1);
                break;
              default:
                break;
            }
          }
        }
      }
    } 

    /**************************/
    /* Checksum Functions     */
    /**************************/
    if (chksum_found) 
    {
      for(k=0; k<cnt; k++) 
      {
        /* Get Systag Index from List */
        j = *(ptr+k)-1; 
        api_ir_calculate_checksum( p_api_dev, dest_biu, j, dbp );
      }
    } 
  }
  else
  {
    /**************************/
    /* Dynamic Dataset        */
    /**************************/
    j = dytag_id-1;
    if( (p_api_dev->ls_systag_setup[ dest_biu ].dytag[ j ].bc_rt == bc_rt_mode) && 
      (p_api_dev->ls_systag_setup[ dest_biu ].dytag[ j ].xid_rtsa == xid_rtsa) && 
      (p_api_dev->ls_systag_setup[ dest_biu ].dytag[ j ].fct == API_SYSTAG_DATASET) )
    {
      if(p_api_dev->ls_systag_setup[ dest_biu ].dytag[ j ].run_mode == API_SYSTAG_RUN_RESUME)
      {
        dataset_id   = p_api_dev->ls_systag_setup[ dest_biu ].dytag[ j ].min;
        dataset_cnt  = p_api_dev->ls_systag_setup[ dest_biu ].dytag[ j ].max;
        dataset_mode = p_api_dev->ls_systag_setup[ dest_biu ].dytag[ j ].step;
        current_id   = p_api_dev->ls_systag_setup[ dest_biu ].dytag[ j ].wpos;

        /* Destination Pointer in Global Memory */
        dbp_p = (WORD*)API_GLB_MEM_ADDR_ABS_IR(dbp, dest_biu);

        /* Source Pointer in Shared Memory */
        src_p = (WORD*)API_DATASET_MEM_ADDR_ABS_IR(current_id);

        /* Copy Dataset to Buffer */
        for(i = 0; i < MAX_API_BUF_SIZE / 2; i++)
          *(dbp_p + i) = *(src_p + i);

        if( (dataset_mode == 0) || (dataset_mode == 1) )
        {
          if( (current_id - dataset_id) < (dataset_cnt - 1) )
          {
            /* Increment current Dataset ID */
            p_api_dev->ls_systag_setup[ dest_biu ].dytag[ j ].wpos++;
          }
          else
          {
            if(dataset_mode == 0)
            {
              /* Cyclic Dataset mode */
              /* Re-Init Dataset ID on wrap-around */
              p_api_dev->ls_systag_setup[ dest_biu ].dytag[ j ].wpos = p_api_dev->ls_systag_setup[ dest_biu ].dytag[ j ].min;
            }
          }
        }
      }
    }
  }

  UsbSynchronizeMemoryArea(p_api_dev, dbp, MAX_API_BUF_SIZE, Out);

} /* end: api_ir_systag */
#endif 

/*****************************************************************************/
/*                                                                           */
/*   Module : API_IR                   Submodule : API_IR_TRACK              */
/*                                                                           */
/*   Author : Magnus Moehr             Project   : API1553-DS                */
/*                                                                           */
/*   Source : C                        Tools     : PC/AT; Norton Editor;     */
/*                                                 CYGNUS, GNU-C, As, and LD */
/*                                                 IDT-C 5.1 Toolkit         */
/*---------------------------------------------------------------------------*/
/*   Create : 15.05.02   Update :                                            */
/*---------------------------------------------------------------------------*/
/*    Descriptions                                                           */
/*    ------------                                                           */
/*    Inputs    : Track Parameter Info [pTrack]                              */
/*                Pointer to data Buffer [dbp]                               */
/*                                                                           */
/*    Outputs   : none                                                       */
/*                                                                           */
/*    Description :                                                          */
/* This function sorts out the received data according the Track Parameter   */
/* Value and copies the data to the corresponding multiplex location         */
/*                                                                           */
/*****************************************************************************/
#ifdef CMPL4SIM
void api_ir_track(TY_API_DEV *p_api_dev, short biu, BYTE uTrackId, TY_API_TRACK_DEF *pTrack, TY_API_TRACK_DEF *pTrackList, L_WORD dbp, L_WORD sqp)
{
    WORD *pSource;
    L_WORD *pStatusQueue;
    WORD *pSourceMuxId;
    WORD *pDest;
    WORD uMask;
    WORD uMuxId;
    WORD uTrackSize = pTrack->uTrackSize;
    int i, j;
    BYTE uc_NoMoreMemory = FALSE;
    BYTE uc_Abort = FALSE;

    /* Pointer to data Buffer */
    pSource = (WORD*)API_GLB_MEM_ADDR_ABS_IR(dbp, biu);
    pStatusQueue = (L_WORD*)API_GLB_MEM_ADDR_ABS_IR(sqp, biu);
    pSource += pTrack->uStartPos;
    pSourceMuxId = pSource;

    /* move the source ptr to the data start of the defined track */
    pSource += pTrack->l_Offset;

    /* Track Parameter mask */
    uMask = (1 << pTrack->uBlen) - 1;

    UsbSynchronizeMemoryArea(p_api_dev, API_GLB_MEM_ADDR_REL(pStatusQueue),
                             p_api_dev->glb_mem.biu[biu].size.bc_sq_area / 
                             p_api_dev->glb_mem.biu[biu].count.bc_sq_area, In);
    UsbSynchronizeMemoryArea(p_api_dev, API_GLB_MEM_ADDR_REL(pSourceMuxId), 
                             pTrack->uTrackFileCount * (uTrackSize * 2), In);

    if(pTrack->l_Offset != 0)
    {
        UsbSynchronizeMemoryArea(p_api_dev, API_GLB_MEM_ADDR_REL(pSource),
                                 pTrack->uTrackFileCount * (uTrackSize * 2), In);
    }

    /* Copy is repeated for all */ 
    for(i = 0; i < pTrack->uTrackFileCount; i++)
    {
        /* Get the Track Parameter Value which represents the Multiplex location */
        uMuxId = ( (BSWAP16(*pSourceMuxId) ) >> pTrack->uBpos) & uMask;

        if( (uMuxId < pTrack->uTrackStates) || (0 == pTrack->uTrackStates) )
        {
            TY_API_TRACK_LIST_ELEMENT *px_TrackElement = NULL;
            /*printf("ir: %d, try to find mux track %d, px_TrackElement=%p\r\n", i, uMuxId, px_TrackElement);*/
            api_sys_track_find_list_element(p_api_dev, pTrack->px_TrackStateList, uMuxId, &px_TrackElement);
            /*printf("ir: %d, try to find mux track %d, px_TrackElement=%p, trackstates=%d\r\n", i, uMuxId, px_TrackElement, pTrack->uTrackStates);*/
            if( (NULL == px_TrackElement) && (0 == pTrack->uTrackStates))
            {
                if(1 == pTrack->bPreAllocated)
                    uc_NoMoreMemory = TRUE;
                else
                {
                    TY_API_TRACK_LIST_ELEMENT x_TrackElement;

                    x_TrackElement.puw_Dest = (WORD*)MALLOC(pTrack->uTrackSize * sizeof(WORD));
                    /*printf("ir: %d, mux track %d is not yet allocated, puw_Dest=%p\r\n", i, uMuxId, x_TrackElement.puw_Dest);*/

                    if( x_TrackElement.puw_Dest )
                    {
                        x_TrackElement.uc_Flags      = 0;
                        x_TrackElement.uw_TrackState = uMuxId;
                        /*printf("ir: %d, mux track %d will be added to list\r\n", i, uMuxId);*/
                        if( API_OK != api_sys_track_add_list_element(p_api_dev, pTrack, uMuxId, &x_TrackElement) )
                        {
                            /*printf("ir: %d, mux track %d could not be added to list\r\n", i, uMuxId);*/
                            uc_NoMoreMemory = TRUE;
                            FREE(x_TrackElement.puw_Dest);
                            x_TrackElement.puw_Dest = NULL;
                        }
                        else
                            api_sys_track_find_list_element(p_api_dev, pTrack->px_TrackStateList, uMuxId, &px_TrackElement);
                    }
                    else
                        uc_NoMoreMemory = TRUE;
                }
            }

            if( FALSE == uc_NoMoreMemory && px_TrackElement )
            {
                /* Copy data to the destination */
                pDest = px_TrackElement->puw_Dest;
                for(j = 0; j < uTrackSize; j++)
                {
                    pDest[ j ] = BSWAP16(pSource[ j ]);
                }

                if(ul_StreamIsHS(p_api_dev, biu))
                    pTrack->ul_LastTimeTag = *(pStatusQueue+5);
                else
                    pTrack->ul_LastTimeTag = *(pStatusQueue+3);

                /* mark received flag for this mux */
                px_TrackElement->uc_Flags |= 0x3; /* Set bit 0 and 1 */
            }
        }
        pSource      += uTrackSize;
        pSourceMuxId += uTrackSize;
    }

    if(pTrack->uRelCount != 0)
    {
        /* Search related Mux Track Buffer */
        TY_API_TRACK_DEF *pMuxTrack = pTrackList;
        TY_API_TRACK_LIST_ELEMENT *px_TrackElement = NULL;
        TY_API_TRACK_LIST_ELEMENT *px_MuxTrackElement = NULL;

        for(i = 0; i < API_MAX_MUX_BUFFER; i++, pMuxTrack++)
        {
            if(pMuxTrack->bEnable && (pMuxTrack->uType == API_TRACK_TYPE_MUX) )
            {
                /* Check for the current MUX track is related to the current received track */
                if(uTrackId == pMuxTrack->uXfer)
                {
                    WORD uMuxValOnTrack = pMuxTrack->uTrackFileCount;

                    api_sys_track_find_list_element(p_api_dev, pTrack->px_TrackStateList, uMuxValOnTrack, &px_TrackElement);

                    if( !px_TrackElement ) break;

                    /*printf("mux: valontrack=%d px_TrackElement->uc_Flags = %d\r\n",uMuxValOnTrack,px_TrackElement->uc_Flags); */
                    if(px_TrackElement->uc_Flags & 0x2)
                        /* Check mux state currently received */           
                    {
                        /* und nun der ganze zirkus noch einmal fr den track auf dem track
                         * nun kommen die daten alerdings aus dem schon gemultiplexten bereich
                         */
                        pSource = px_TrackElement->puw_Dest;
                        pSource += pMuxTrack->uStartPos;
                        pSourceMuxId = pSource;

                        /* move the source ptr to the data start of the defined track */
                        pSource += pMuxTrack->l_Offset;

                        uMask = (1 << pMuxTrack->uBlen) - 1;
                        uMuxId = ( (/*BSWAP16*/(*pSourceMuxId) ) >> pMuxTrack->uBpos) & uMask;
                        /*printf("mux id=%d, psource=%p, psourceorg=%p\r\n",uMuxId,pSource,px_TrackElement->puw_Dest);*/
                        if( (uMuxId < pMuxTrack->uTrackStates) || (0 == pMuxTrack->uTrackStates) )
                        {
                            api_sys_track_find_list_element(p_api_dev, pMuxTrack->px_TrackStateList, uMuxId, &px_MuxTrackElement);

                            if( (NULL == px_MuxTrackElement) && (0 == pMuxTrack->uTrackStates) )
                            {
                                if(1 == pMuxTrack->bPreAllocated)
                                    uc_NoMoreMemory = TRUE;
                                else
                                {
                                    TY_API_TRACK_LIST_ELEMENT x_MuxTrackElement;
                                    x_MuxTrackElement.puw_Dest = (WORD*)MALLOC(pMuxTrack->uTrackSize * sizeof(WORD));

                                    uc_NoMoreMemory = FALSE;
                                    if( x_MuxTrackElement.puw_Dest )
                                    {
                                        x_MuxTrackElement.uc_Flags      = 0;
                                        x_MuxTrackElement.uw_TrackState = uMuxId;
                                        if( API_OK != api_sys_track_add_list_element(p_api_dev, pMuxTrack, uMuxId, &x_MuxTrackElement) )
                                        {
                                            uc_NoMoreMemory = TRUE;
                                            FREE(x_MuxTrackElement.puw_Dest);
                                            x_MuxTrackElement.puw_Dest = NULL;
                                        }
                                        else
                                            api_sys_track_find_list_element(p_api_dev, pMuxTrack->px_TrackStateList, uMuxId, &px_MuxTrackElement);
                                    }
                                    else
                                        uc_NoMoreMemory = TRUE;
                                }
                            }

                            if( FALSE == uc_NoMoreMemory )
                            {
                                uTrackSize = pMuxTrack->uTrackSize;

                                pDest = px_MuxTrackElement->puw_Dest;
                                /*printf("ir: %d mux track %d data is copied, dest=%p, src=%p\r\n", i, uMuxId, pDest, pSource);*/
                                for(j = 0; j < uTrackSize; j++)
                                {
                                    pDest[ j ] = /*BSWAP16*/(pSource[ j ]);
                                    /*printf("dest[%d] = %04X, source[%d] = %04X\r\n", j, pDest[ j ], j, BSWAP16(pSource[ j ]));*/
                                }

                                if(ul_StreamIsHS(p_api_dev, biu))
                                    pMuxTrack->ul_LastTimeTag = *(pStatusQueue+5);
                                else
                                    pMuxTrack->ul_LastTimeTag = *(pStatusQueue+3);
                                /*printf("sqp=%08X last TT = %08X\r\n", sqp, pTrack->ul_LastTimeTag);*/

                                /* mark received flag for this mux */
                                px_MuxTrackElement->uc_Flags = 0x1;
                            }
                        }
                    }
                }
            }
        }

        /* clear the current received flags */
        px_TrackElement = pTrack->px_TrackStateList;
        while(!uc_Abort)
        {
            if(NULL != px_TrackElement)
            {
                px_TrackElement->uc_Flags &= (~0x2);
                px_TrackElement = (TY_API_TRACK_LIST_ELEMENT *)px_TrackElement->px_Next;
            }
            else
                uc_Abort = TRUE;
        }
    }
}

#endif 
/*****************************************************************************/
/*                                                                           */
/*   Module : API_IR                   Submodule : API_IR_BM_REC             */
/*                                                                           */
/*   Author : Ralf Kuderer             Project   : API1553-DS                */
/*                                                                           */
/*   Source : C                        Tools     : PC/AT; Norton Editor;     */
/*                                                 CYGNUS, GNU-C, As, and LD */
/*                                                 IDT-C 5.1 Toolkit         */
/*---------------------------------------------------------------------------*/
/*   Create : 01.05.98   Update : 11.12.00                                   */
/*---------------------------------------------------------------------------*/
/*    Descriptions                                                           */
/*    ------------                                                           */
/*    Inputs    : Half Buffer Full Address [addr]                            */
/*                                                                           */
/*    Outputs   : none                                                       */
/*                                                                           */
/*    Description :                                                          */
/*    This function is used to handle an Half Buffer Full Interrupt in       */
/*    Recording mode.                                                        */
/*                                                                           */
/*****************************************************************************/

#ifdef CMPL4MON
void api_ir_bm_rec( TY_API_DEV *p_api_dev, short dest_biu, L_WORD addr )
{
  L_WORD saddr;


  /* Calculate Half Buffer Size */
  p_api_dev->bm_rec[ dest_biu ].size = p_api_dev->glb_mem.biu[ dest_biu ].size.bm_buf / 2L;
  wbflush();

  /* Calculate Monitor Buffer Start Address */
  if( (addr - p_api_dev->bm_rec[ dest_biu ].size) >= p_api_dev->glb_mem.biu[ dest_biu ].base.bm_buf)
  {
    wbflush();
    saddr = addr - (p_api_dev->bm_rec[ dest_biu ].size - 4);
  }
  else
  {
    wbflush();
    saddr = addr + (p_api_dev->bm_rec[ dest_biu ].size + 4);
    if( (saddr - p_api_dev->glb_mem.biu[ dest_biu ].size.bm_buf) >= p_api_dev->glb_mem.biu[ dest_biu ].base.bm_buf)
      saddr -= p_api_dev->glb_mem.biu[ dest_biu ].size.bm_buf;
  }
  wbflush();
  /* Half Buffer Full Interrupt */
  p_api_dev->bm_rec[ dest_biu ].size /= 4L; /* in entries */
  wbflush();
  p_api_dev->bm_rec[ dest_biu ].saddr = saddr; /* block to record */
  wbflush();
  p_api_dev->bm_rec[ dest_biu ].hfi_cnt += 1; /* increment */ /*tj: 11.12.00 */
  wbflush();

  /* Stop the BM if zero */
  /*tj: 11.12.00 */
  /*        if (p_api_dev->bm_rec[dest_biu].hfi_cnt == 0L) {
   *           p_api_dev->bm_rec[dest_biu].mode = API_OFF;
   *           api_ir_mode_con(API_BM_MODE, API_OFF);
   *           p_api_dev->bm_status[dest_biu] = API_HALT;
   *           p_api_dev->bm_status_dsp[dest_biu] = API_HALT; 
   *        }
   *     }
   */   

} /* end: api_ir_bm_rec */
#endif 

/*****************************************************************************/
/*                                                                           */
/*   Module : API_IR                  Submodule : API_IR_BM_DYNTAGM_STATES   */
/*                                                                           */
/*   Author : Marco Motz              Project   : API1553-DS                 */
/*                                                                           */
/*   Source : C                       Tools     : PC/AT; Norton Editor;      */
/*                                                CYGNUS, GNU-C, As, and LD  */
/*                                                IDT-C 5.1 Toolkit          */
/*---------------------------------------------------------------------------*/
/*   Create : 11.11.05   Update : 11.11.05                                   */
/*---------------------------------------------------------------------------*/
/*    Descriptions                                                           */
/*    ------------                                                           */
/*    Inputs    : Monitor Buffer Fill Pointer [addr]                         */
/*                                                                           */
/*    Outputs   : none                                                       */
/*                                                                           */
/*    Description :                                                          */
/*    This function is used to handle Good/Bad States of the Dynamic Tag     */
/*    Monitoring function.                                                   */
/*                                                                           */
/*****************************************************************************/

#ifdef CMPL4MON
static void api_ir_bm_dyntagm_states(TY_API_DEV *p_api_dev, short dest_biu, L_WORD dw, L_WORD tag_ix)
{
L_WORD actual_dw, prev_dw;

    actual_dw = dw;

    switch(p_api_dev->bm_ls_dyntagm_set[ dest_biu ].dyntag[tag_ix].dyntag_type) {
      case API_LS_DYTAG_FCT_SAW_16:
           actual_dw &= 0x0000FFFFL;
           break;
      case API_LS_DYTAG_FCT_SAW_8_LOW:
           actual_dw &= 0x000000FFL;
           break;
      case API_LS_DYTAG_FCT_SAW_8_HIGH:
           actual_dw = actual_dw >> 8;
           actual_dw &= 0x000000FFL;
           break;
      default:
           break;     
    }
    
    prev_dw = p_api_dev->bm_ls_dyntagm_set[ dest_biu ].dyntag[tag_ix].dw;

    if ((prev_dw & 0x80000000L) == 0x80000000L) {
      prev_dw &= 0x7FFFFFFFL;
    
      switch (p_api_dev->bm_ls_dyntagm_set[ dest_biu ].dyntag[tag_ix].state) {
      
        case 0: /* GOOD */
             if (actual_dw != prev_dw) {
                /* Good */ 
                p_api_dev->bm_ls_dyntagm_set[ dest_biu ].dyntag[tag_ix].good_cnt++;
             }
             else {
                /* Stale (x1) */
                p_api_dev->bm_ls_dyntagm_set[ dest_biu ].dyntag[tag_ix].stale_cnt++;
                p_api_dev->bm_ls_dyntagm_set[ dest_biu ].dyntag[tag_ix].stale_cnt_contig++;
                p_api_dev->bm_ls_dyntagm_set[ dest_biu ].dyntag[tag_ix].state = 1; /* goto GOOD1 state */
             }   
             break;
             
        case 1: /* GOOD1 */
             if (actual_dw != prev_dw) {
                /* Good */ 
                p_api_dev->bm_ls_dyntagm_set[ dest_biu ].dyntag[tag_ix].good_cnt++;
                p_api_dev->bm_ls_dyntagm_set[ dest_biu ].dyntag[tag_ix].stale_cnt_contig = 0;
                p_api_dev->bm_ls_dyntagm_set[ dest_biu ].dyntag[tag_ix].state = 0; /* return to GOOD state */
             }
             else {
                /* Stale */
                p_api_dev->bm_ls_dyntagm_set[ dest_biu ].dyntag[tag_ix].stale_cnt++;
                p_api_dev->bm_ls_dyntagm_set[ dest_biu ].dyntag[tag_ix].stale_cnt_contig++;
                if (p_api_dev->bm_ls_dyntagm_set[ dest_biu ].dyntag[tag_ix].stale_cnt_contig >= 3) {
                   /* Bad */
                   p_api_dev->bm_ls_dyntagm_set[ dest_biu ].dyntag[tag_ix].bad_cnt++;
                   p_api_dev->bm_ls_dyntagm_set[ dest_biu ].dyntag[tag_ix].stale_cnt_contig = 0;
                   p_api_dev->bm_ls_dyntagm_set[ dest_biu ].dyntag[tag_ix].state = 2; /* goto BAD state */
                }
             }   
             break;
             
        case 2: /* BAD */
             if (actual_dw == prev_dw) {
                /* Stale and Bad */
                p_api_dev->bm_ls_dyntagm_set[ dest_biu ].dyntag[tag_ix].stale_cnt++;
                p_api_dev->bm_ls_dyntagm_set[ dest_biu ].dyntag[tag_ix].bad_cnt++;
             }
             else {
                /* Good (x1) */ 
                p_api_dev->bm_ls_dyntagm_set[ dest_biu ].dyntag[tag_ix].bad_cnt++;
                p_api_dev->bm_ls_dyntagm_set[ dest_biu ].dyntag[tag_ix].good_cnt_contig++;
                p_api_dev->bm_ls_dyntagm_set[ dest_biu ].dyntag[tag_ix].state = 3; /* goto BAD1 state */
             }
             break;
             
        case 3: /* BAD1 */
             if (actual_dw == prev_dw) {
                /* Stale */
                p_api_dev->bm_ls_dyntagm_set[ dest_biu ].dyntag[tag_ix].stale_cnt++;
                /* Bad */ 
                p_api_dev->bm_ls_dyntagm_set[ dest_biu ].dyntag[tag_ix].bad_cnt++;
                p_api_dev->bm_ls_dyntagm_set[ dest_biu ].dyntag[tag_ix].good_cnt_contig = 0;
                p_api_dev->bm_ls_dyntagm_set[ dest_biu ].dyntag[tag_ix].state = 2; /* return to BAD state */
             }
             else {
                p_api_dev->bm_ls_dyntagm_set[ dest_biu ].dyntag[tag_ix].good_cnt_contig++;
                if (p_api_dev->bm_ls_dyntagm_set[ dest_biu ].dyntag[tag_ix].good_cnt_contig >= 3) {
                   /* Good */
                   p_api_dev->bm_ls_dyntagm_set[ dest_biu ].dyntag[tag_ix].good_cnt++;
                   p_api_dev->bm_ls_dyntagm_set[ dest_biu ].dyntag[tag_ix].good_cnt_contig = 0;
                   p_api_dev->bm_ls_dyntagm_set[ dest_biu ].dyntag[tag_ix].state = 0; /* goto GOOD state */
                }
                else {
                   /* still Bad */ 
                   p_api_dev->bm_ls_dyntagm_set[ dest_biu ].dyntag[tag_ix].bad_cnt++;
                }
             }   
             break;
             
        default:
             break;
      }
    }              

    /* Store Dyntag value */
    p_api_dev->bm_ls_dyntagm_set[ dest_biu ].dyntag[tag_ix].dw = actual_dw | 0x80000000L;

} /* end: api_ir_bm_dyntagm_states */
#endif

/*****************************************************************************/
/*                                                                           */
/*   Module : API_IR                   Submodule : API_IR_BM_DYNTAGM         */
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
/*    Inputs    : Monitor Buffer Fill Pointer [addr]                         */
/*                                                                           */
/*    Outputs   : none                                                       */
/*                                                                           */
/*    Description :                                                          */
/*    This function is used to handle a Monitor Filter Descriptor Interrupt  */
/*    and is used to check the monitored message on incrementing Dyntags.    */
/*                                                                           */
/*****************************************************************************/

#ifdef CMPL4MON
#ifndef PREVIOUS_IMPLEMENTATION
void api_ir_bm_dyntagm(TY_API_DEV *p_api_dev, short dest_biu, L_WORD addr)
{
L_WORD i,j,index,flag,tmpl,mbfp,size,size1,size2,cnt,wcnt,wp,tag_ix;
WORD cw1,cw2,dataw[32],rt,sa,tr;
L_WORD volatile *src_p;
L_WORD buf[40];
L_WORD *p;

  size = 0L;
  
  rt = 0;
  sa = 0;
  tr = 0;
  
  cw1 = 0x0000;
  cw2 = 0x0000;                                                          
  
  for (i=0; i<32; i++) 
     dataw[i] = 0x0000;

  UsbSynchronizeLWord(p_api_dev, API_GLB_MEM_ADDR_REL(&p_api_dev->cb_p[dest_biu]->mbfp), In);
  UsbSynchronizeMonitorDataBuffer(p_api_dev, dest_biu, In);

  /* Set Start Pointer of message */
  src_p = (L_WORD*)API_GLB_MEM_ADDR_ABS_IR(addr, dest_biu);

  /* Get Monitor Fill Pointer */
  mbfp = ul_ConvertGRAMPtrToTSWPtr(p_api_dev, dest_biu, (*p_api_dev->cb_p[ dest_biu ]).mbfp);

#ifdef DEBUG_DYNTAGMON
  PRINTF3("addr=%08lx mbfp=%08lx %ld ", 
      addr,mbfp, 
      p_api_dev->bm_ls_dyntagm_lut[ dest_biu ].rt[0].sa[1].cnt);
  PRINTF3("%d %d %d\n",
      p_api_dev->bm_ls_dyntagm_set[ dest_biu ].dyntag[0].con,
      p_api_dev->bm_ls_dyntagm_set[ dest_biu ].dyntag[0].rt,
      p_api_dev->bm_ls_dyntagm_set[ dest_biu ].dyntag[0].sa);
#endif

  /* Copy Monitor data to temp buffer */
  if (mbfp > addr) {
     size =  (mbfp - addr) / 4L + 1;
     for (i=0; (i<=size) && (i<40); i++) {
         buf[i] = *src_p;
         src_p++;
     }
  }
  else {
     if (mbfp < addr) {
        /* Fill Pointer wrap-around occured */ 
        size1 =  (p_api_dev->glb_mem.biu[ dest_biu ].size.bm_buf +  p_api_dev->glb_mem.biu[ dest_biu ].base.bm_buf - addr) / 4L;
        for (i=0; (i<size1) && (i<40); i++) {
            buf[i] = *src_p;
            src_p++;
        }
        wbflush();
        /* Rest from BM start address */
        size2 =  (mbfp - p_api_dev->glb_mem.biu[ dest_biu ].base.bm_buf) / 4L;
        src_p = (L_WORD*)API_GLB_MEM_ADDR_ABS_IR(p_api_dev->glb_mem.biu[ dest_biu ].base.bm_buf, dest_biu);
        for (j=0; (j<=size2) && (i<40); j++,i++) {
            buf[i] = *src_p;
            src_p++;
        }
        
        size = size1 + size2 + 1;
        
#ifdef DEBUG_DYNTAGMON
        PRINTF3("mbfp=%08lx addr=%08lx size=%ld\n",mbfp,addr,size);
        PRINTF2("size1=%ld size2=%ld\n",size1,size2);
        for (i=0;i<40;i++) PRINTF1("%08lx\n",buf[i]);
#endif

     }
  }   
  
  wbflush();
      
#ifdef DEBUG_DYNTAGMON
  PRINTF1("size=%ld\n",size);
#endif
  
  /* Get Command and Data Words */
  for (i=0, cnt=0, flag=0, wcnt=0; (i < size) && (i < 40) && (flag == 0); i++) {
    tmpl = BSWAP32(buf[i]);
    switch (tmpl & 0xF0000000L) {
      case 0x80000000: /* 1.Command Word on PRI */
      case 0xC0000000: /* 1.Command Word on SEC */
           if (cw1 == 0x0000) {
               cw1 = (WORD)tmpl;
               wcnt = tmpl & 0x1F;
               if (wcnt == 0) wcnt = 32;
           }
           else {
              flag = 1; /* next command word detected */
           }   
           break;
      case 0x90000000: /* 2.Command Word on PRI */
      case 0xD0000000: /* 2.Command Word on SEC */
           if (cw2 == 0x0000) cw2 = (WORD)tmpl;
           break;
      case 0xA0000000: /* Data Word on PRI */
      case 0xE0000000: /* Data Word on SEC */
           if ((cnt < 32) && (cnt < wcnt)) {
              dataw[cnt] = (WORD)tmpl;
              cnt++;
           }
           break;
      default:
           break;
    }       
  }

  /* Check first Command Word */   
  rt = (cw1 >> 11) & 0x001F;
  tr = (cw1 >> 10) & 0x0001;
  sa = (cw1 >> 5)  & 0x001F;
  index = sa + (tr * 32);
  wcnt = cnt;    
     
#ifdef DEBUG_DYNTAGMON
  PRINTF2("cw1=%04x index=%ld\n",cw1, index);
#endif
     
  /* Get Dyntag Monitor ID count for specified RT-SA */   
  cnt = p_api_dev->bm_ls_dyntagm_lut[ dest_biu ].rt[rt].sa[index].cnt;
  p = p_api_dev->bm_ls_dyntagm_lut[ dest_biu ].rt[rt].sa[index].p;

#ifdef DEBUG_DYNTAGMON
  PRINTF2("cnt=%ld wcnt=%ld\n",cnt,wcnt);
#endif
     
  if ((cnt != 0) && (p != NULL) && (wcnt != 0)) {
    for (i=0;i<cnt; i++) {
      /* Check all Dyntag Monitor ID */
      tag_ix = p[i];
      if ((tag_ix != 0) && (tag_ix <= MAX_API_BM_DYNTAG_MON_ID)) {
        tag_ix--;
        
        wp = p_api_dev->bm_ls_dyntagm_set[ dest_biu ].dyntag[tag_ix].dyntag_wpos;
        if (wp < wcnt) {
           /* Check States */
           api_ir_bm_dyntagm_states(p_api_dev, dest_biu, (L_WORD) dataw[wp], tag_ix);
        }
      }
    }
  }

} /* end: api_ir_bm_dyntagm */

#else

void api_ir_bm_dyntagm(TY_API_DEV *p_api_dev, short dest_biu, L_WORD addr)
{
L_WORD i,j,index,flag,tmpl,dw,dw_max,mbfp,size,size1,size2,cnt,wcnt,tag_ix;
WORD cw1,cw2,dataw[32],rt,sa,tr;
L_WORD volatile *src_p;
L_WORD buf[40];
L_WORD *p;

  size = 0L;
  
  rt = 0;
  sa = 0;
  tr = 0;
  
  cw1 = 0x0000;
  cw2 = 0x0000;                                                          
  
  for (i=0; i<32; i++) 
     dataw[i] = 0x0000;

  /* Set Start Pointer of message */
  src_p = (L_WORD*)API_GLB_MEM_ADDR_ABS_IR(addr, dest_biu);

  /* Get Monitor Fill Pointer */
  mbfp = ul_ConvertGRAMPtrToTSWPtr(p_api_dev, dest_biu, (*p_api_dev->cb_p[ dest_biu ]).mbfp);

#ifdef DEBUG_DYNTAGMON
  PRINTF3("addr=%08x mbfp=%08x %d ", 
      addr,mbfp, 
      p_api_dev->bm_ls_dyntagm_lut[ dest_biu ].rt[0].sa[1].cnt);
  PRINTF3("%d %d %d\n",
      p_api_dev->bm_ls_dyntagm_set[ dest_biu ].dyntag[0].con,
      p_api_dev->bm_ls_dyntagm_set[ dest_biu ].dyntag[0].rt,
      p_api_dev->bm_ls_dyntagm_set[ dest_biu ].dyntag[0].sa);
#endif

  /* Copy Monitor data to temp buffer */
  if (mbfp > addr) {
     size =  (mbfp - addr) / 4L + 1;
     for (i=0; (i<=size) && (i<40); i++) {
         buf[i] = *src_p;
         src_p++;
     }
  }
  else {
     if (mbfp < addr) {
        /* Fill Pointer wrap-around occured */ 
        size1 =  (p_api_dev->glb_mem.biu[ dest_biu ].size.bm_buf +  p_api_dev->glb_mem.biu[ dest_biu ].base.bm_buf - addr) / 4L;
        for (i=0; (i<size1) && (i<40); i++) {
            buf[i] = *src_p;
            src_p++;
        }
        wbflush();
        /* Rest from BM start address */
        size2 =  (mbfp - p_api_dev->glb_mem.biu[ dest_biu ].base.bm_buf) / 4L;
        src_p = (L_WORD*)API_GLB_MEM_ADDR_ABS_IR(p_api_dev->glb_mem.biu[ dest_biu ].base.bm_buf, dest_biu);
        for (j=0; (j<=size2) && (i<40); j++,i++) {
            buf[i] = *src_p;
            src_p++;
        }
        
        size = size1 + size2 + 1;
        
#ifdef DEBUG_DYNTAGMON
        PRINTF3("mbfp=%08x addr=%08x size=%d\n",mbfp,addr,size);
        PRINTF2("size1=%ld size2=%ld\n",size1,size2);
        for (i=0;i<40;i++) PRINTF1("%08lx\n",buf[i]);
#endif

     }
  }   
  
  wbflush();
      
#ifdef DEBUG_DYNTAGMON
  PRINTF1("size=%ld\n",size);
#endif
  
  /* Get Command and Data Words */
  for (i=0, cnt=0, flag=0, wcnt=0; (i < size) && (i < 40) && (flag == 0); i++) {
    tmpl = BSWAP32(buf[i]);
    switch (tmpl & 0xF0000000L) {
      case 0x80000000: /* 1.Command Word on PRI */
      case 0xC0000000: /* 1.Command Word on SEC */
           if (cw1 == 0x0000) {
               cw1 = (WORD)tmpl;
               wcnt = tmpl & 0x1F;
               if (wcnt == 0) wcnt = 32;
           }
           else {
              flag = 1; /* next command word detected */
           }   
           break;
      case 0x90000000: /* 2.Command Word on PRI */
      case 0xD0000000: /* 2.Command Word on SEC */
           if (cw2 == 0x0000) cw2 = (WORD)tmpl;
           break;
      case 0xA0000000: /* Data Word on PRI */
      case 0xE0000000: /* Data Word on SEC */
           if ((cnt < 32) && (cnt < wcnt)) {
              dataw[cnt] = (WORD)tmpl;
              cnt++;
           }
           break;
      default:
           break;
    }       
  }

  /* Check first Command Word */   
  rt = (cw1 >> 11) & 0x001F;
  tr = (cw1 >> 10) & 0x0001;
  sa = (cw1 >> 5)  & 0x001F;
  index = sa + (tr * 32);
  wcnt = cnt;    
     
#ifdef DEBUG_DYNTAGMON
  PRINTF2("cw1=%04lx index=%ld\n",cw1, index);
#endif
     
  /* Get Dyntag Monitor ID count for specified RT-SA */   
  cnt = p_api_dev->bm_ls_dyntagm_lut[ dest_biu ].rt[rt].sa[index].cnt;
  p = p_api_dev->bm_ls_dyntagm_lut[ dest_biu ].rt[rt].sa[index].p;

#ifdef DEBUG_DYNTAGMON
  PRINTF2("cnt=%ld wcnt=%ld\n",cnt,wcnt);
#endif
     
  if ((cnt != 0) && (p != NULL) && (wcnt != 0)) {
    for (i=0;i<cnt; i++) {
      /* Check all Dyntag Monitor ID */
      tag_ix = p[i];
      if ((tag_ix != 0) && (tag_ix <= MAX_API_BM_DYNTAG_MON_ID)) {
        tag_ix--;
        
        tmpl = 0L;
        dw_max = 0L;
        if (p_api_dev->bm_ls_dyntagm_set[ dest_biu ].dyntag[tag_ix].dyntag_wpos < wcnt)
           tmpl = dataw[p_api_dev->bm_ls_dyntagm_set[ dest_biu ].dyntag[tag_ix].dyntag_wpos];
        
        switch(p_api_dev->bm_ls_dyntagm_set[ dest_biu ].dyntag[tag_ix].dyntag_type) {
          case API_LS_DYTAG_FCT_SAW_16:
               tmpl &= 0x0000FFFFL;
               dw_max = 65536;
               break;
          case API_LS_DYTAG_FCT_SAW_8_LOW:
               tmpl &= 0x000000FFL;
               dw_max = 256;
               break;
          case API_LS_DYTAG_FCT_SAW_8_HIGH:
               tmpl = tmpl >> 8;
               tmpl &= 0x000000FFL;
               dw_max = 256;
               break;
          default:
               break;     
        }
        
        dw = p_api_dev->bm_ls_dyntagm_set[ dest_biu ].dyntag[tag_ix].dw;

#ifdef DEBUG_DYNTAGMON
        PRINTF3("found id=%d type=%d wpos=%d ",tag_ix,
          p_api_dev->bm_ls_dyntagm_set[ dest_biu ].dyntag[tag_ix].dyntag_type,
          p_api_dev->bm_ls_dyntagm_set[ dest_biu ].dyntag[tag_ix].dyntag_wpos);
        PRINTF2("%08lx %08lx\n",tmpl,dw);
#endif

        if (dw & 0x80000000L) {
           dw &= 0x7FFFFFFFL;
            
           if (tmpl == dw) {
              /* Stale */
              p_api_dev->bm_ls_dyntagm_set[ dest_biu ].dyntag[tag_ix].stale_cnt++;
#ifdef DEBUG_DYNTAGMON
              PRINTF0("stale\n");
              PRINTF3(" %d %08x %08x",wcnt,tmpl,dw);
              PRINTF2(" %08x %08x\n",mbfp,addr);
#endif              
           } 
           else {
              if (tmpl == (dw+1)) {
                 /* Good */ 
                 p_api_dev->bm_ls_dyntagm_set[ dest_biu ].dyntag[tag_ix].good_cnt++;
                 /*printf("ok\n");*/
              }
              else if ((tmpl == 0) && ((dw+1) == dw_max)) {
                 /* Good */ 
                 p_api_dev->bm_ls_dyntagm_set[ dest_biu ].dyntag[tag_ix].good_cnt++;
#ifdef DEBUG_DYNTAGMON
                 PRINTF0("wrapped\n");
#endif                 
              }
              else {
                 /* Bad */
                 p_api_dev->bm_ls_dyntagm_set[ dest_biu ].dyntag[tag_ix].bad_cnt++;
#ifdef DEBUG_DYNTAGMON
                 PRINTF0("bad\n");
                 PRINTF3(" %d %08x %08x",wcnt,tmpl,dw);
                 PRINTF2(" %08x %08x\n",mbfp,addr);
#endif                 
              }
           }
        }              

        /* Store Dyntag value */
        p_api_dev->bm_ls_dyntagm_set[ dest_biu ].dyntag[tag_ix].dw = tmpl | 0x80000000L;
      }
    }
  }

} /* end: api_ir_bm_dyntagm */
#endif
#endif 

/*****************************************************************************/
/*                                                                           */
/*   Module : API_IR                   Submodule : API_IR_BCRT_QUEUE         */
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
/*    Inputs    : Queue Index [queue_i]                                      */
/*                BC Data Buffer Pointer [dbp]                               */
/*                                                                           */
/*    Outputs   : none                                                       */
/*                                                                           */
/*    Description :                                                          */
/*    This function is utilized to perform Fifo Tagging in the API1553-DS    */
/*    BC/RT Global Memory Data Buffer.                                       */
/*                                                                           */
/*****************************************************************************/

#ifdef CMPL4SIM
void api_ir_bcrt_queue( TY_API_DEV *p_api_dev, short dest_biu, WORD fifo_i, L_WORD dbp )
{
  L_WORD saddr, current_id, w_size;
  WORD id, j, buf_cnt;
  WORD volatile *dbp_p;
  WORD volatile *dqueue_p;

  id = fifo_i - 1;

  saddr = p_api_dev->queue_setup[ dest_biu ].queue[ id ].saddr;
  current_id = p_api_dev->queue_setup[ dest_biu ].queue[ id ].cur_buf_addr;

  buf_cnt = 0;
  /* Set pointer to Dataset Memory address */
  dbp_p = (WORD*)API_GLB_MEM_ADDR_ABS_IR(dbp, dest_biu);
  dqueue_p = (WORD*)API_FIFO_MEM_ADDR_ABS_IR(current_id);

  /* Copy Dataset to Buffer */
  for(j = 0; j < MAX_API_BUF_SIZE / 2; j++)
  {
    *(dbp_p + j) = *(dqueue_p + j);
  }

  current_id += MAX_API_BUF_SIZE;

  if(current_id > (saddr + ( (p_api_dev->queue_setup[ dest_biu ].max_size *MAX_API_BUF_SIZE) - 1) ) )
  {

    p_api_dev->queue_setup[ dest_biu ].queue[ id ].cur_buf_addr = saddr;

    w_size = ( (saddr + (p_api_dev->queue_setup[ dest_biu ].max_size *MAX_API_BUF_SIZE) - p_api_dev->queue_setup[ dest_biu ].queue[ id ].half_buf_addr) / 2);
    buf_cnt = (WORD)w_size;
  }
  else
  {

    if(current_id == p_api_dev->queue_setup[ dest_biu ].queue[ id ].half_buf_addr)
    {

      w_size = ( (p_api_dev->queue_setup[ dest_biu ].queue[ id ].half_buf_addr - saddr) / 2);
      buf_cnt = (WORD)w_size;
    }

    p_api_dev->queue_setup[ dest_biu ].queue[ id ].cur_buf_addr = current_id;

  }

  if( (p_api_dev->queue_setup[ dest_biu ].queue[ id ].buf_cnt != 0) && (buf_cnt != 0) )
  {
    p_api_dev->queue_setup[ dest_biu ].queue[ id ].buf_cnt = 0xFFFF;
  }
  else
  {
    if(buf_cnt != 0)
    {
      p_api_dev->queue_setup[ dest_biu ].queue[ id ].buf_cnt = buf_cnt;
    }
  }

  UsbSynchronizeMemoryArea(p_api_dev, dbp, MAX_API_BUF_SIZE, Out);

} /* end: api_ir_bcrt_queue */
#endif 
/*****************************************************************************/
/*                                                                           */
/*   Module : API_IR                   Submodule : API_IR_BC_SRVW            */
/*                                                                           */
/*   Author : Marco Motz               Project   : API1553-DS                */
/*                                                                           */
/*   Source : C                        Tools     : PC/AT; Norton Editor;     */
/*                                                 CYGNUS, GNU-C, As, and LD */
/*                                                 IDT-C 5.1 Toolkit         */
/*---------------------------------------------------------------------------*/
/*   Create : 11.12.00   Update : 11.12.00                                   */
/*---------------------------------------------------------------------------*/
/*    Descriptions                                                           */
/*    ------------                                                           */
/*    Inputs    : Remote Terminal Address [rt_addr]                          */
/*                                                                           */
/*    Outputs   : none                                                       */
/*                                                                           */
/*    Description :                                                          */
/*    This function is used to decode the received Vector Word mode code.    */
/*                                                                           */
/*****************************************************************************/

#ifdef CMPL4SIM
void api_ir_bc_srvw(TY_API_DEV *p_api_dev, short dest_biu, BYTE rt_addr, WORD vw, L_WORD xid)
{
  short tb_index, tb_index1, rt_index, sa_index, i, j;
  WORD xid_srvw;
  L_WORD instr;

  /* Read received Vector Word */

  /* Get BC Transaction ID from lookup table */
  tb_index1 = vw >> 12;
  tb_index = tb_index1 &0x7;
  rt_index = (vw >> 7) &0x001F;
  sa_index = vw &0x007F;
  xid_srvw = p_api_dev->bc_srvw_tab[ dest_biu ].t[ tb_index ].ta[ rt_index ].sa_mid[ sa_index ].xid &0x7FFF;

  /* Decode ID code */
  if( (xid_srvw != 0) && ( (tb_index1 &0x8) == 0x8) )
  {
    switch(tb_index)
    {
      case API_BC_SRVW_ID_SINGLE_RX:
      case API_BC_SRVW_ID_SINGLE_TX:

        /* XFER Opcode */
        instr = MAKE_INSTR(API_BC_INSTR_XFER, 
                (ul_ConvertTSWPtrToGRAMPtr(p_api_dev, dest_biu, p_api_dev->glb_mem.biu[ dest_biu ].base.bc_xfer_desc) +
                (xid_srvw *MAX_API_BC_XFER_DESC_SIZE)) );
        (*p_api_dev->cb_p[ dest_biu ]).rsv1[ 0 ] = BSWAP32(instr);

        /* RET Opcode */
        instr = MAKE_INSTR(API_BC_INSTR_RET, 0);
        (*p_api_dev->cb_p[ dest_biu ]).rsv1[ 1 ] = BSWAP32(instr);

        /* Start acyclic transmission */
        (*p_api_dev->cb_p[ dest_biu ]).aip = ul_ConvertTSWPtrToGRAMPtr(p_api_dev, dest_biu, p_api_dev->glb_mem.biu[ dest_biu ].cb_addr + 0x1B8); /* start xfer at rsv1[0]*/
        break;

      case API_BC_SRVW_ID_MULTIP_RX:
        p_api_dev->bc_srvw_tab[ dest_biu ].t[ API_BC_SRVW_ID_DELETE_RX ].ta[ rt_index ].sa_mid[ sa_index ].xid |= 0x8000;
        p_api_dev->bc_srvw_tab[ dest_biu ].t[ API_BC_SRVW_ID_MULTIP_RX ].ta[ rt_index ].sa_mid[ sa_index ].xid |= 0x8000;

        instr = MAKE_INSTR(API_BC_INSTR_XFER, 
                 (ul_ConvertTSWPtrToGRAMPtr(p_api_dev, dest_biu, p_api_dev->glb_mem.biu[ dest_biu ].base.bc_xfer_desc) +
                 (xid_srvw *MAX_API_BC_XFER_DESC_SIZE)) );
        for(i = 0, j = 0; i < p_api_dev->bc_srvw_subframe[ dest_biu ].cnt; i++)
        {
          if( (p_api_dev->bc_srvw_subframe[ dest_biu ].instr[ i ] &0xFFFFFFFEL) == instr)
            p_api_dev->bc_srvw_subframe[ dest_biu ].instr[ i ] |= 1L;
           /* enable */
          if( (p_api_dev->bc_srvw_subframe[ dest_biu ].instr[ i ] &1L) == 1L)
          {
            *(p_api_dev->bc_lip_instr_p[ dest_biu ] + j) = BSWAP32(p_api_dev->bc_srvw_subframe[ dest_biu ].instr[ i ] &0xFFFFFFFEL);
            j++;
          }
        }
        *(p_api_dev->bc_lip_instr_p[ dest_biu ] + j) = BSWAP32(MAKE_INSTR(API_BC_INSTR_RET, 0) );
        break;

      case API_BC_SRVW_ID_MULTIP_TX:
        p_api_dev->bc_srvw_tab[ dest_biu ].t[ API_BC_SRVW_ID_DELETE_TX ].ta[ rt_index ].sa_mid[ sa_index ].xid |= 0x8000;
        p_api_dev->bc_srvw_tab[ dest_biu ].t[ API_BC_SRVW_ID_MULTIP_TX ].ta[ rt_index ].sa_mid[ sa_index ].xid |= 0x8000;

        instr = MAKE_INSTR(API_BC_INSTR_XFER, 
                (ul_ConvertTSWPtrToGRAMPtr(p_api_dev, dest_biu, p_api_dev->glb_mem.biu[ dest_biu ].base.bc_xfer_desc) + 
                (xid_srvw *MAX_API_BC_XFER_DESC_SIZE)) );
        for(i = 0, j = 0; i < p_api_dev->bc_srvw_subframe[ dest_biu ].cnt; i++)
        {
          if( (p_api_dev->bc_srvw_subframe[ dest_biu ].instr[ i ] &0xFFFFFFFEL) == instr)
            p_api_dev->bc_srvw_subframe[ dest_biu ].instr[ i ] |= 1L;
           /* enable */
          if( (p_api_dev->bc_srvw_subframe[ dest_biu ].instr[ i ] &1L) == 1L)
          {
            *(p_api_dev->bc_lip_instr_p[ dest_biu ] + j) = BSWAP32(p_api_dev->bc_srvw_subframe[ dest_biu ].instr[ i ] &0xFFFFFFFEL);
            j++;
          }
        }
        *(p_api_dev->bc_lip_instr_p[ dest_biu ] + j) = BSWAP32(MAKE_INSTR(API_BC_INSTR_RET, 0) );
        break;

      case API_BC_SRVW_ID_DELETE_RX:
        if( (p_api_dev->bc_srvw_tab[ dest_biu ].t[ API_BC_SRVW_ID_DELETE_RX ].ta[ rt_index ].sa_mid[ sa_index ].xid &0x8000) == 0x8000)
        {
          p_api_dev->bc_srvw_tab[ dest_biu ].t[ API_BC_SRVW_ID_DELETE_RX ].ta[ rt_index ].sa_mid[ sa_index ].xid &= 0x7FFF;
          p_api_dev->bc_srvw_tab[ dest_biu ].t[ API_BC_SRVW_ID_MULTIP_RX ].ta[ rt_index ].sa_mid[ sa_index ].xid &= 0x7FFF;

          instr = MAKE_INSTR(API_BC_INSTR_XFER, 
                  (ul_ConvertTSWPtrToGRAMPtr(p_api_dev, dest_biu, p_api_dev->glb_mem.biu[ dest_biu ].base.bc_xfer_desc) +
                  (xid_srvw * MAX_API_BC_XFER_DESC_SIZE)) );
          for(i = 0, j = 0; i < p_api_dev->bc_srvw_subframe[ dest_biu ].cnt; i++)
          {
            if( (p_api_dev->bc_srvw_subframe[ dest_biu ].instr[ i ] &0xFFFFFFFEL) == instr)
              p_api_dev->bc_srvw_subframe[ dest_biu ].instr[ i ] &= 0xFFFFFFFEL;
             /* disable */
            if( (p_api_dev->bc_srvw_subframe[ dest_biu ].instr[ i ] &1L) == 1L)
            {
              *(p_api_dev->bc_lip_instr_p[ dest_biu ] + j) = BSWAP32(p_api_dev->bc_srvw_subframe[ dest_biu ].instr[ i ] &0xFFFFFFFEL);
              j++;
            }
          }
          *(p_api_dev->bc_lip_instr_p[ dest_biu ] + j) = BSWAP32(MAKE_INSTR(API_BC_INSTR_RET, 0) );
        }
        else
          xid_srvw = 0;
        break;

      case API_BC_SRVW_ID_DELETE_TX:
        if( (p_api_dev->bc_srvw_tab[ dest_biu ].t[ API_BC_SRVW_ID_DELETE_TX ].ta[ rt_index ].sa_mid[ sa_index ].xid &0x8000) == 0x8000)
        {
          p_api_dev->bc_srvw_tab[ dest_biu ].t[ API_BC_SRVW_ID_DELETE_TX ].ta[ rt_index ].sa_mid[ sa_index ].xid &= 0x7FFF;
          p_api_dev->bc_srvw_tab[ dest_biu ].t[ API_BC_SRVW_ID_MULTIP_TX ].ta[ rt_index ].sa_mid[ sa_index ].xid &= 0x7FFF;

          instr = MAKE_INSTR(API_BC_INSTR_XFER, 
                  (ul_ConvertTSWPtrToGRAMPtr(p_api_dev, dest_biu, p_api_dev->glb_mem.biu[ dest_biu ].base.bc_xfer_desc) + 
                  (xid_srvw *MAX_API_BC_XFER_DESC_SIZE)) );
          for(i = 0, j = 0; i < p_api_dev->bc_srvw_subframe[ dest_biu ].cnt; i++)
          {
            if( (p_api_dev->bc_srvw_subframe[ dest_biu ].instr[ i ] &0xFFFFFFFEL) == instr)
              p_api_dev->bc_srvw_subframe[ dest_biu ].instr[ i ] &= 0xFFFFFFFEL;
             /* disable */
            if( (p_api_dev->bc_srvw_subframe[ dest_biu ].instr[ i ] &1L) == 1L)
            {
              *(p_api_dev->bc_lip_instr_p[ dest_biu ] + j) = BSWAP32(p_api_dev->bc_srvw_subframe[ dest_biu ].instr[ i ] &0xFFFFFFFEL);
              j++;
            }
          }
          *(p_api_dev->bc_lip_instr_p[ dest_biu ] + j) = BSWAP32(MAKE_INSTR(API_BC_INSTR_RET, 0) );
        }
        else
          xid_srvw = 0;
        break;

      default:
        break;
    }
  }

  /* Prepare Status info */
  p_api_dev->bc_srvw_rt_dsp[ dest_biu ].rt[ rt_addr ].xid = xid_srvw;
  p_api_dev->bc_srvw_rt_dsp[ dest_biu ].rt[ rt_addr ].lvw = vw;
  p_api_dev->bc_srvw_rt_dsp[ dest_biu ].rt[ rt_addr ].src++;

  if( p_api_dev->bc_srvw_xid_dsp[ dest_biu ].px_Xid )
  {
    p_api_dev->bc_srvw_xid_dsp[ dest_biu ].px_Xid[ xid ].lvw = vw;
    p_api_dev->bc_srvw_xid_dsp[ dest_biu ].px_Xid[ xid ].src++;
  }

} /* end: api_ir_bc_srvw */
#endif 

/*****************************************************************************/
/*                                                                           */
/*   Module : API_IR                   Submodule : API_IR_BC                 */
/*                                                                           */
/*   Author : Marco Motz               Project   : API1553-DS                */
/*                                                                           */
/*   Source : C                        Tools     : PC/AT; Norton Editor;     */
/*                                                 CYGNUS, GNU-C, As, and LD */
/*                                                 IDT-C 5.1 Toolkit         */
/*---------------------------------------------------------------------------*/
/*   Create : 01.05.98   Update : 24.07.02                                   */
/*---------------------------------------------------------------------------*/
/*    Descriptions                                                           */
/*    ------------                                                           */
/*    Inputs    : Interrupt Status LogList Entry [w1]                        */
/*                Interrupt Status LogList Entry [w2]                        */
/*                                                                           */
/*    Outputs   : none                                                       */
/*                                                                           */
/*    Description :                                                          */
/*    This function is used to handle a API1553-DS Bus Controller Interrupt. */
/*                                                                           */
/*****************************************************************************/

#ifdef CMPL4SIM
void api_ir_bc(TY_API_DEV *p_api_dev, short dest_biu, L_WORD w1, L_WORD w2)
{
  L_WORD desc_addr, xid, index, dbp, sqp, xf_id, db_id, cindex, rt_addr;
  L_WORD  current_buffer_index = 0;
  L_WORD  sts = 0;
  TY_API_TRACK_DEF *pTrack;
  struct ty_api_bc_xfer_desc *px_XferDesc;
  struct ty_api_bc_bh_desc *px_BufHdr;
  struct ty_api_bc_sq_desc *px_StatusQueue;
  L_WORD pbi_rel_xfer_desc_area_start = 0;
 
  BYTE c1760_ix;
  #ifdef _CMPL4S1I1553               
  BYTE bc_param_comb_cnt, bc_action_cnt;
  #endif 
  WORD bc_dytag_i, bc_queue_i;
  WORD volatile *c1760_dbp_p;
  int i;
  
  xf_id = 0L;
  db_id = 0L;
  xid = 0;
  dbp = 0;
  sqp = 0;

  if(p_api_dev->bite_stream[ dest_biu ] == API_OFF)
  {
    if( (w1 &0x10000000) == 0x10000000)
    {
       /* UDF */
      if( (w1 &0x08000000) == 0x08000000)
      {
         /* BCH */
        /* BC Halt Interrupt */
      }
      if( (w1 &0x01000000) == 0x01000000)
      {
         /* IXI */
        /* BC Index Interrupt */

        index = w1 &0x000000FFL;

        /*** 03.10.02: added for Mate extensions, GetMsg functionality ***/
        /* TBD: hid is fixed to 1 at the moment -> get from BC Transfer Descriptor */
        desc_addr = w2 - p_api_dev->glb_mem.biu[ dest_biu ].base.bc_xfer_desc;
        xid = desc_addr / MAX_API_BC_XFER_DESC_SIZE;
        /* Get Buffer Address from Buffer Header */
        desc_addr = ul_ConvertGRAMPtrToTSWPtr(p_api_dev, dest_biu, (*p_api_dev->bc_xfer_desc_p[ dest_biu ])[ xid ].bhd_ptr & BSWAP32(0x03fffffful) );
        desc_addr -= p_api_dev->glb_mem.biu[ dest_biu ].base.bc_bh_area;
      }

      if( (w1 &0x00800000) == 0x00800000)
      {
         /* EVI */
        /* BC Event Interrupt */
      }
      /* 03.12.02 (Bugfix): Generate Interrupt to Host not working for AEI and UXI */
      if( ( (w1 &0x04000000) == 0x04000000) ||  /* UXI = BC Unexpected Response Interrupt */
      ( (w1 &0x02000000) == 0x02000000) ||  /* AEI = BC Any Error Interrupt */
      ( (w1 &0x00400000) == 0x00400000) )
      {
         /* PGI = BC Programmed Transfer Interrupt */

        if(w2 >= p_api_dev->glb_mem.biu[ dest_biu ].base.bc_xfer_desc)
        {

          /* Calculate Transfer ID from interrupting Xfer Descriptor Address */
          desc_addr = w2 - p_api_dev->glb_mem.biu[ dest_biu ].base.bc_xfer_desc;
          xid = desc_addr / MAX_API_BC_XFER_DESC_SIZE;
          if(xid < p_api_dev->glb_mem.biu[ dest_biu ].count.bc_xfer_desc)
          {
            current_buffer_index = w1 & 0xFF;
            /* Get Buffer Address from Buffer Header */
            desc_addr = ul_ConvertGRAMPtrToTSWPtr(p_api_dev, dest_biu, (*p_api_dev->bc_xfer_desc_p[ dest_biu ])[ xid ].bhd_ptr & BSWAP32(0x03fffffful) );
            desc_addr -= p_api_dev->glb_mem.biu[ dest_biu ].base.bc_bh_area;
            index = desc_addr / MAX_API_BC_BH_DESC_SIZE;
            dbp = ul_ConvertGRAMPtrToTSWPtr(p_api_dev, dest_biu, (*p_api_dev->bc_bh_desc_p[ dest_biu ])[ index ].bp_addr);

            xf_id = xid; /* Transfer ID */
            db_id = (dbp - p_api_dev->glb_mem.sim_buf_base_addr[ dest_biu ]) / MAX_API_BUF_SIZE;

            /* Get first status queue entry */

            sqp = ul_ConvertGRAMPtrToTSWPtr(p_api_dev, dest_biu, (*p_api_dev->bc_bh_desc_p[ dest_biu ])[ index ].sq_addr);

            /* Support status queue */
            UsbSynchronizeLWord(p_api_dev, API_GLB_MEM_ADDR_REL(&((*p_api_dev->bc_bh_desc_p[dest_biu])[index].bh_ctrl)), In);
            cindex = BSWAP32((*p_api_dev->bc_bh_desc_p[dest_biu])[index].bh_ctrl);
            sts = (cindex >> 12) &0x00000003L;

            if (sts == 2)
                sqp = sqp + (current_buffer_index*sizeof(struct ty_api_rt_sq_desc));

            /* Set lld according to data buffer queue index */
            db_id += current_buffer_index;

            /* 25.05.00: System Dynamic Data */
            bc_dytag_i = p_api_dev->bc_setup[ dest_biu ].bc_systag[ xid ].id;
            if(bc_dytag_i != 0)
            {
              api_ir_systag(p_api_dev, dest_biu, API_BC_MODE, (WORD)xid, dbp);

              /* 1760 Protocol Handling */
              if(api_io_c1760_mode_used(p_api_dev, (BYTE)dest_biu) == API_OK)
              {
                /* 1760 Buffer */
                if(api_io_c1760_buf_get(p_api_dev, (BYTE)dest_biu, (WORD)db_id, &c1760_ix) == API_OK)
                {
                  /* 1760 Checksum Handling */
                  c1760_dbp_p = (WORD*)API_GLB_MEM_ADDR_ABS_IR(dbp, dest_biu);
                  api_io_c1760_buf_set(p_api_dev, (BYTE)dest_biu, c1760_ix, c1760_dbp_p);
                }
              }
            }

            /* 22.09.99 */
            bc_queue_i = (WORD) (BSWAP32( (*p_api_dev->bc_xfer_desc_p[ dest_biu ])[ xid ].fifo_id) >> 16);
            if( (bc_queue_i != 0) && (p_api_dev->queue_setup[ dest_biu ].queue[ bc_queue_i - 1 ].xid != 0) )
            {
              api_ir_bcrt_queue( p_api_dev, dest_biu, bc_queue_i, dbp );
            }

  #ifdef _CMPL4S1I1553               
            /* 24.05.00 */
            bc_action_cnt = p_api_dev->bc_setup[ dest_biu ].bc_action.xid[ xid ];
            if(bc_action_cnt != 0)
            {
              /* 17.05.00 */
              s1i_ir_bc_profile(bc_action_cnt, xid, dbp);
            }
            /* 24.05.00 */
            bc_param_comb_cnt = api_param_comb_setup.comb_cnt;
            if(bc_param_comb_cnt != 0)
            {
              /* 17.05.00 */
              s1i_ir_param_comb(db_id, dbp);
            }
  #endif 
          }
        }

        /* 11.12.00 : Service Request and Vector Word handling */
        else if(p_api_dev->bc_srvw_con[ dest_biu ] == API_ON)
        {
          if(p_api_dev->bc_status[dest_biu] == API_BUSY)
          {
              UsbSynchronizeBCMiscBlock(p_api_dev, dest_biu, In);
          }

          /*printf("SRVW-INT recognized!\n\r");*/
          
          pbi_rel_xfer_desc_area_start = API_GLB_GLOBREL_TO_PBIREL_BIU( p_api_dev->glb_mem.biu[ dest_biu ].base.bc_xfer_desc, dest_biu);

          if(w2 == (p_api_dev->glb_mem.biu[ dest_biu ].cb_addr + 0x180) )
          {
            if( p_api_dev->cb_p[ dest_biu ]->xf_desc[0].xf_last >= pbi_rel_xfer_desc_area_start )
            {
                /* Get initial xid of transfer descriptor */
                desc_addr = (*p_api_dev->cb_p[ dest_biu ]).xf_desc[0].xf_last - pbi_rel_xfer_desc_area_start;
                xid = desc_addr / MAX_API_BC_XFER_DESC_SIZE;

                /* Get RT Address from last Status Word */
                rt_addr = (BSWAP32( (*p_api_dev->cb_p[ dest_biu ]).xf_bh[ 0 ].lcw_lsw) &0x0000F800L) >> 11;
                /*printf("SRVW handling for addr 0x180, rt_addr = %d\n\r", rt_addr);*/
                api_ir_bc_srvw(p_api_dev, dest_biu, (BYTE)rt_addr, (WORD) (BSWAP32( (*p_api_dev->cb_p[ dest_biu ]).xf_bh[ 0 ].data) ), xid );
            }
            else
            {
                /*Outch: xid will be invalid if we continue. xf_last must not be smaller than base.bc_xfer_desc */
                /* Synchronize BC Miscellaneuous Block again for debug purpose. */
                UsbSynchronizeBCMiscBlock(p_api_dev, dest_biu, In);
            }
          }
          else if(w2 == (p_api_dev->glb_mem.biu[ dest_biu ].cb_addr + 0x19C) )
          {
            /* Get initial xid of transfer descriptor */
            desc_addr = (*p_api_dev->cb_p[ dest_biu ]).xf_desc[1].xf_last - pbi_rel_xfer_desc_area_start;
            xid = desc_addr / MAX_API_BC_XFER_DESC_SIZE;

            /* Get RT Address from last Status Word */
            rt_addr = (BSWAP32( (*p_api_dev->cb_p[ dest_biu ]).xf_bh[ 1 ].lcw_lsw) &0x0000F800L) >> 11;
            /*printf("SRVW handling for addr 0x19C, rt_addr = %d\n\r", rt_addr);*/
            api_ir_bc_srvw(p_api_dev, dest_biu, (BYTE)rt_addr, (WORD) (BSWAP32( (*p_api_dev->cb_p[ dest_biu ]).xf_bh[ 1 ].data) ), xid );
          }
          else if(w2 == (p_api_dev->glb_mem.biu[ dest_biu ].cb_addr + 0x280) )
          {
            /* Get initial xid of transfer descriptor */
            desc_addr = (*p_api_dev->cb_p[ dest_biu ]).xf_desc_rtrt[0].xf_last - pbi_rel_xfer_desc_area_start;
            xid = desc_addr / MAX_API_BC_XFER_DESC_SIZE;

            /* Get RT Address from last Status Word */
            rt_addr = (BSWAP32( (*p_api_dev->cb_p[ dest_biu ]).xf_bh_rtrt[ 0 ].lcw_lsw) &0x0000F800L) >> 11;
            /*printf("SRVW handling for addr 0x280, rt_addr = %d\n\r", rt_addr);*/
            api_ir_bc_srvw(p_api_dev, dest_biu, (BYTE)rt_addr, (WORD) (BSWAP32( (*p_api_dev->cb_p[ dest_biu ]).xf_bh_rtrt[ 0 ].data) ), xid );
          }
          else if(w2 == (p_api_dev->glb_mem.biu[ dest_biu ].cb_addr + 0x29C) )
          {
            /* Get initial xid of transfer descriptor */
            desc_addr = (*p_api_dev->cb_p[ dest_biu ]).xf_desc_rtrt[1].xf_last - pbi_rel_xfer_desc_area_start;
            xid = desc_addr / MAX_API_BC_XFER_DESC_SIZE;

            /* Get RT Address from last Status Word */
            rt_addr = (BSWAP32( (*p_api_dev->cb_p[ dest_biu ]).xf_bh_rtrt[ 1 ].lcw_lsw) &0x0000F800L) >> 11;
            /*printf("SRVW handling for addr 0x29C, rt_addr = %d\n\r", rt_addr);*/
            api_ir_bc_srvw(p_api_dev, dest_biu, (BYTE)rt_addr, (WORD) (BSWAP32( (*p_api_dev->cb_p[ dest_biu ]).xf_bh_rtrt[ 1 ].data) ), xid );
          }
        }
      }

      if( (w1 &0x04000000) == 0x04000000 )   /* UXI = BC Unexpected Response Interrupt */
      {
        if(w2 >= p_api_dev->glb_mem.biu[ dest_biu ].base.bc_xfer_desc)
        {
          px_XferDesc = (struct ty_api_bc_xfer_desc *)API_GLB_MEM_ADDR_ABS_IR(w2, dest_biu);

          /* check if transfer is a MC0 */
          if( ((px_XferDesc->cw2_cw1 & 0x000003ff) == 0x000003E0) || ((px_XferDesc->cw2_cw1 & 0x000003ff) == 0x00000000) )
          {
            if( (p_api_dev->bc_dbc_stop_status[ dest_biu ] == API_BUSY) && (p_api_dev->bc_status[ dest_biu ] == API_BUSY) )
            {
              px_BufHdr = (struct ty_api_bc_bh_desc *)API_GLB_MEM_ADDR_ABS_IR(px_XferDesc->bhd_ptr & 0x03FFFFFF, dest_biu);
              px_StatusQueue = (struct ty_api_bc_sq_desc *)API_GLB_MEM_ADDR_ABS_IR(px_BufHdr->sq_addr & 0x03FFFFFF, dest_biu);

              UsbSynchronizeLWord(p_api_dev, API_GLB_MEM_ADDR_REL(&px_StatusQueue->lsw2_lsw1),
                                  In);

              px_StatusQueue->lsw2_lsw1 &= 0x0000FFFF;

              if( px_StatusQueue->lsw2_lsw1 & 0x00000002) /* if DBCA bit of RT status word is set */
              {
                /* Stop Bus Controller */
                api_ir_mode_con(p_api_dev, dest_biu, API_BC_MODE, API_OFF);
                p_api_dev->bc_status[ dest_biu ] = API_HALT;
              }
            }
          }
        }
      }

      /* Mux  Buffer */
      pTrack = &p_api_dev->ls_track[ dest_biu][ 0 ];
      for(i = 0; i < API_MAX_MUX_BUFFER; i++, pTrack++)
      {
        if(pTrack->bEnable && (pTrack->uType == API_TRACK_TYPE_BC) && (pTrack->uXfer == xid) )
        {
          api_ir_track(p_api_dev, dest_biu, (BYTE)i, pTrack, p_api_dev->ls_track[ dest_biu ], dbp, sqp);
        }
      }

      /* Call Application Specified Interrupt Handler */
      /* ... tbd ... */
      /* Interrupt event List */

      if(p_api_dev->bc_setup[ dest_biu ].xfer_interrupt[ xf_id ] == API_ON)
      {
        if( p_api_dev->ul_DbgLevel & DBG_INTERRUPT_CNT)
          p_api_dev->ul_CntBCInt++;
        api_write_int_loli_gen_int(p_api_dev, ((dest_biu > (BIU_2-1)) ? (dest_biu+3) : dest_biu), w1, w2, (p_api_dev->ul_DbgLevel & DBG_INTERRUPT_CNT) ? p_api_dev->ul_CntBCInt : xf_id, db_id);
      }
      else if( (w1 & 0x09800000) != 0 ) /* BC Halt, BC Index, BC Event */
      {
        if( p_api_dev->ul_DbgLevel & DBG_INTERRUPT_CNT)
          p_api_dev->ul_CntBCInt++;
        api_write_int_loli_gen_int(p_api_dev, ((dest_biu > (BIU_2-1)) ? (dest_biu+3) : dest_biu), w1, w2, (p_api_dev->ul_DbgLevel & DBG_INTERRUPT_CNT) ? p_api_dev->ul_CntBCInt : xid, db_id);
      }
      else if( p_api_dev->bc_srvw_con[ dest_biu ] == API_ON )
      {
        if( p_api_dev->ul_DbgLevel & DBG_INTERRUPT_CNT)
          p_api_dev->ul_CntBCInt++;
        api_write_int_loli_gen_int(p_api_dev, ((dest_biu > (BIU_2-1)) ? (dest_biu+3) : dest_biu), w1, w2, (p_api_dev->ul_DbgLevel & DBG_INTERRUPT_CNT) ? p_api_dev->ul_CntBCInt : xf_id, db_id);
      }
      if( 0 == (p_api_dev->ul_DbgLevel & DBG_INTERRUPT_CNT))
        p_api_dev->ul_CntBCInt = 0;
    }
  }

} /* end: api_ir_bc */
#endif 

/*****************************************************************************/
/*                                                                           */
/*   Module : API_IR                   Submodule : API_IR_BC_SKIP            */
/*                                                                           */
/*   Author : Marco Motz               Project   : API1553-DS                */
/*                                                                           */
/*   Source : C                        Tools     : PC/AT; Norton Editor;     */
/*                                                 CYGNUS, GNU-C, As, and LD */
/*                                                 IDT-C 5.1 Toolkit         */
/*---------------------------------------------------------------------------*/
/*   Create : 25.05.00   Update : 28.08.02                                   */
/*---------------------------------------------------------------------------*/
/*    Descriptions                                                           */
/*    ------------                                                           */
/*    Inputs    : Interrupt Status LogList Entry [w1]                        */
/*                Interrupt Status LogList Entry [w2]                        */
/*                                                                           */
/*    Outputs   : none                                                       */
/*                                                                           */
/*    Description :                                                          */
/*    This function is used to handle a API1553 BC Relative Branch (Skip)    */
/*    Interrupt.                                                             */
/*                                                                           */
/*****************************************************************************/

#ifdef CMPL4SIM
void api_ir_bc_skip(TY_API_DEV *p_api_dev, short dest_biu, L_WORD w1, L_WORD w2)
{

  if(p_api_dev->bite_stream[ dest_biu ] == API_OFF)
  {
    if( (w1 &0x10000000) == 0x10000000)
    {
       /* UDF */
      /*printf("BC Skip: %08x %08x sinst=%08x\n\r",w1,w2,p_api_dev->bc_setup[dest_biu].sinst);*/
      if(p_api_dev->bc_setup[ dest_biu ].sinst == w2)
      {
        /* BC Halt Interrupt */
        api_ir_mode_con(p_api_dev, dest_biu, API_BC_MODE, API_OFF);
        p_api_dev->bc_status_dsp[ dest_biu ].status = API_BC_STATUS_HALT;
        p_api_dev->bc_status_dsp[ dest_biu ].hxfer = 0x0000;
        p_api_dev->bc_status_cnt[ dest_biu ].msg_cnt = 0L;
        p_api_dev->bc_status_cnt[ dest_biu ].err_cnt = 0L;
        p_api_dev->bc_status_cnt[ dest_biu ].hip = 0L;
        p_api_dev->bc_status_cnt[ dest_biu ].mfc = 0L;
        p_api_dev->bc_status_cnt[ dest_biu ].res1 = 0L;
        p_api_dev->bc_status_cnt[ dest_biu ].res2 = 0L;
        p_api_dev->bc_trg_status[ dest_biu ] = API_HALT;
        p_api_dev->bc_dbc_start_status[ dest_biu ] = API_HALT;
        p_api_dev->bc_dbc_stop_status[ dest_biu ]  = API_HALT;
        p_api_dev->bc_dbc_stop_xfctrl_save[ dest_biu ] = 0;
        p_api_dev->bc_dbc_stop_xfswm_save[ dest_biu ] = 0;
        p_api_dev->bc_status[ dest_biu ] = API_HALT;
      }
      else if(p_api_dev->bc_setup[dest_biu].bc_triggered_inst == w2)
      {
          /* BC Start was externally triggered */

          if(p_api_dev->bc_status[dest_biu] == API_HALT 
             && p_api_dev->bc_trg_status[dest_biu] == API_BUSY)
          {
              /* set actual BC status to busy, so USB system driver knows BC data has to 
                 be synchronized with board */
              p_api_dev->bc_status[dest_biu] = API_BUSY;
          }
      }

      /* Call Application Specified Interrupt Handler */
      /* ... tbd ... */
      /* Interrupt event List */
      /* Interrupt Loglist structure changed */
      api_write_int_loli_gen_int(p_api_dev, ((dest_biu > (BIU_2-1)) ? (dest_biu+3) : dest_biu), w1, w2, 0, 0);
    }
  }

} /* end: api_ir_bc_skip */
#endif 

/*****************************************************************************/
/*                                                                           */
/*   Module : API_IR                   Submodule : API_IR_RT                 */
/*                                                                           */
/*   Author : Marco Motz               Project   : API1553-DS                */
/*                                                                           */
/*   Source : C                        Tools     : PC/AT; Norton Editor;     */
/*                                                 CYGNUS, GNU-C, As, and LD */
/*                                                 IDT-C 5.1 Toolkit         */
/*---------------------------------------------------------------------------*/
/*   Create : 01.05.98   Update : 22.08.03                                   */
/*---------------------------------------------------------------------------*/
/*    Descriptions                                                           */
/*    ------------                                                           */
/*    Inputs    : Interrupt Status LogList Entry [w1]                        */
/*                Interrupt Status LogList Entry [w2]                        */
/*                                                                           */
/*    Outputs   : none                                                       */
/*                                                                           */
/*    Description :                                                          */
/*    This function is used to handle API1553-DS Remote Terminal Interrupts. */
/*                                                                           */
/*****************************************************************************/

#ifdef CMPL4SIM
void api_ir_rt(TY_API_DEV *p_api_dev, short dest_biu, L_WORD w1, L_WORD w2)
{
  L_WORD sqp;
  L_WORD desc_addr, rt, sa, sa_ty = 0, offs, index, dbp, cw_id, db_id, sts, cindex, rt_hid;
  L_WORD current_buffer_index = 0;
  WORD rt_dytag_i, rt_queue_i;
  L_WORD tmpl;
  L_WORD volatile *src_p;
  #ifdef _CMPL4S1I1553
  WORD rt_sa_event_cnt, rt_sa_action_cnt;
  BYTE rt_param_comb_cnt;
  #endif 
  BYTE c1760_ix;
  WORD volatile *c1760_dbp_p;
  int i;
  struct ty_api_rt_sq_desc *px_StatusQueue;

  cw_id = 0L;
  db_id = 0L;
  rt = 0;
  sa = 0;
  index = 0;

  if(p_api_dev->bite_stream[ dest_biu ] == API_OFF)
  {

    if( (w1 &0x10000000) == 0x10000000)
    {
       /* UDF */

      /* Interrupt caused by */
      if( (w1 &0x01000000) == 0x01000000)
      {
         /* IXI */
        /* RT Index Interrupt */
        index = w1 &0x000000FFL;

        /*** 03.10.02: added for Mate extensions, GetMsg functionality ***/

        /* get Buffer Header ID */
        /* then get buffer header pointer from SA Descriptor */
        src_p = (L_WORD*)API_GLB_MEM_ADDR_ABS_IR(w2 + 4, dest_biu);
        tmpl = (*src_p) &0x03FFFFFF;

        /* calculate Buffer Header ID */
        rt_hid = tmpl - API_GLB_MEM_ADDR_REL(p_api_dev->rt_bh_desc_p[ dest_biu ]);
        rt_hid /= MAX_API_RT_BH_DESC_SIZE;

      }

      if( (w1 &0x00800000) == 0x00800000)
      {
         /* EVI */
        /* RT Event Interrupt */
      }

      if( ( (w1 &0x02000000) == 0x02000000) ||  /* AEI = RT Any Error Interrupt */
      ( (w1 &0x00400000) == 0x00400000) )
      {
         /* PGI = Programmed RT Subaddress Interrupt */

        /* Calculate RT and SA from interrupting RT-SA Descriptor Address */
        if(    ( w2 >=  p_api_dev->glb_mem.biu[ dest_biu ].base.rt_sa_desc)
            && ( w2 <  (p_api_dev->glb_mem.biu[ dest_biu ].base.rt_sa_desc + p_api_dev->glb_mem.biu[ dest_biu ].size.rt_sa_desc)) )
        {
          desc_addr = w2 - p_api_dev->glb_mem.biu[ dest_biu ].base.rt_sa_desc;
          current_buffer_index = w1 & 0xFF;
          rt = desc_addr / MAX_API_RT_SA_AREA_SIZE;
          offs = desc_addr - (rt *MAX_API_RT_SA_AREA_SIZE);
          offs /= MAX_API_RT_SA_DESC_SIZE;
          sa_ty = offs / MAX_API_RT_MC_SA;
          sa = offs - (sa_ty *MAX_API_RT_MC_SA);
          index = sa + (MAX_API_RT_MC_SA *sa_ty);

          /* Get Buffer Address from Buffer Header */
          desc_addr = ul_ConvertGRAMPtrToTSWPtr(p_api_dev, dest_biu, (*p_api_dev->rt_sa_desc_p[ dest_biu ])[ rt ].sa_mc[ index ].bhd_ptr & BSWAP32(0x03fffffful) );
          desc_addr -= p_api_dev->glb_mem.biu[ dest_biu ].base.rt_bh_area;
          index = desc_addr / MAX_API_RT_BH_DESC_SIZE;
          dbp = ul_ConvertGRAMPtrToTSWPtr(p_api_dev, dest_biu, (*p_api_dev->rt_bh_desc_p[ dest_biu ])[ index ].bp_addr);
          sqp = ul_ConvertGRAMPtrToTSWPtr(p_api_dev, dest_biu, (*p_api_dev->rt_bh_desc_p[ dest_biu ])[ index ].sq_addr);


          /* Get first queue entry */
          px_StatusQueue = (struct ty_api_rt_sq_desc *)API_GLB_MEM_ADDR_ABS_IR(sqp, dest_biu);

          /* Support status queue */
          UsbSynchronizeLWord(p_api_dev, API_GLB_MEM_ADDR_REL(&((*p_api_dev->rt_bh_desc_p[dest_biu])[index].bh_ctrl)), In);
          cindex = BSWAP32((*p_api_dev->rt_bh_desc_p[dest_biu])[index].bh_ctrl);
          sts = (cindex >> 12) &0x00000003L;

          if (sts == 2)
          {
              sqp = sqp + (current_buffer_index*sizeof(struct ty_api_rt_sq_desc));
              px_StatusQueue = &px_StatusQueue[current_buffer_index];
          }

          /* Return the last Command Word in LLC (includes Wordcount) */
          UsbSynchronizeLWord(p_api_dev, API_GLB_MEM_ADDR_REL(&px_StatusQueue->lcw_lsw), In);
          cw_id = BSWAP32(px_StatusQueue->lcw_lsw) >> 16;


          switch(sa_ty)
          {
            case API_RT_SA_TYPE_RCV:
            case API_RT_SA_TYPE_XMT:
              /* cw_id = ( (rt &0x1FL) << 11) | ( (sa &0x1FL) << 5) | ( (sa_ty &0x1L) << 10); */
              db_id = (dbp - p_api_dev->glb_mem.sim_buf_base_addr[ dest_biu ]) / MAX_API_BUF_SIZE;
              break;
            case API_RT_MC_TYPE_RCV:
            case API_RT_MC_TYPE_XMT:
              /* cw_id = ( (rt &0x1FL) << 11) | (sa &0x1FL) | ( (sa_ty &0x1L) << 10); */
              db_id = (dbp - p_api_dev->glb_mem.sim_buf_base_addr[ dest_biu ]) / MAX_API_BUF_SIZE;

              /* Handle Dynamic Bus Control Modecode 0 */
              if( (sa_ty == API_RT_MC_TYPE_XMT) && (sa == 0) )
              {
                if( (p_api_dev->bc_dbc_start_status[ dest_biu ] == API_BUSY) && (p_api_dev->bc_status[ dest_biu ] == API_HALT) )
                {
                  if( sqp != 0 )
                  {
                    px_StatusQueue = (struct ty_api_rt_sq_desc *)API_GLB_MEM_ADDR_ABS_IR(sqp, dest_biu);

                    if( px_StatusQueue->lcw_lsw & 0x00000002) /* if DBCA bit of RT status word is set */
                    {
                      /* Start Bus Controller */
                      api_ir_mode_con(p_api_dev, dest_biu, API_BC_MODE, API_ON);
                      p_api_dev->bc_status[ dest_biu ] = API_BUSY;
                    }
                  }
                }
              }
              break;
            default:
              break;
          }

          /* Set lld according to data buffer queue index */
          db_id += current_buffer_index;

  #ifdef _CMPL4S1I1553
          /* Event Detection */
          if( (rt < MAX_API_RT) && (sa < MAX_API_RT_MC_SA) && ( (sa_ty == API_RT_SA_TYPE_RCV) || (sa_ty == API_RT_SA_TYPE_XMT) ) )
          {

            rt_sa_event_cnt = p_api_dev->rt_setup[ dest_biu ].rt_event[ rt ].sa[ sa ];
            if(rt_sa_event_cnt != 0)
            {
              s1i_ir_rt_sa_event(rt_sa_event_cnt, rt, sa, sa_ty, dbp);
            }
          }
  #endif 

          if( (rt < MAX_API_RT) && (sa < MAX_API_RT_MC_SA) && (sa_ty == API_RT_SA_TYPE_XMT) )
          {

            /* System Dynamic Data */
            rt_dytag_i = p_api_dev->rt_setup[ dest_biu ].rt_systag[ rt ].sa[ sa ].id;
            if(rt_dytag_i != 0)
            {
              api_ir_systag(p_api_dev, dest_biu, API_RT_MODE, (WORD) ( (rt << 8) | sa), dbp);

              /* 1760 Protocol Handling */
              if(api_io_c1760_mode_used(p_api_dev, (BYTE)dest_biu) == API_OK)
              {
                /* 1760 Buffer */
                if(api_io_c1760_buf_get(p_api_dev, (BYTE)dest_biu, (WORD)db_id, &c1760_ix) == API_OK)
                {
                  /* 1760 Checksum Handling */
                  c1760_dbp_p = (WORD*)API_GLB_MEM_ADDR_ABS_IR(dbp, dest_biu);
                  api_io_c1760_buf_set(p_api_dev, (BYTE)dest_biu, c1760_ix, c1760_dbp_p);
                }
              }
            }

            rt_queue_i = p_api_dev->rt_setup[ dest_biu ].rt_queue[ rt ].sa[ sa ];
            if( (rt_queue_i != 0) && (p_api_dev->queue_setup[ dest_biu ].queue[ rt_queue_i - 1 ].rt != 0xFF) )
            {
              api_ir_bcrt_queue(p_api_dev, dest_biu, rt_queue_i, dbp);
            }
          }

          /* Mux  Buffer */
          if( (sa_ty == API_RT_SA_TYPE_RCV) || (sa_ty == API_RT_SA_TYPE_XMT) )
          {
            BYTE bTX = (sa_ty == API_RT_SA_TYPE_XMT);
            TY_API_TRACK_DEF *pTrack;

            pTrack = &p_api_dev->ls_track[ dest_biu][ 0 ];
            for(i = 0; i < API_MAX_MUX_BUFFER; i++, pTrack++ )
            {
              if(pTrack->bEnable && (pTrack->uType == API_TRACK_TYPE_RT) )
              {
                if( (pTrack->uRT == rt) && (pTrack->uSA == sa) && (pTrack->bTX == bTX) )
                {
                  api_ir_track(p_api_dev, dest_biu, (BYTE)i, pTrack, p_api_dev->ls_track[ dest_biu ], dbp, sqp);
                }
              }
            }
          }

  #ifdef _CMPL4S1I1553
          /* Action Management */
          if( (rt < MAX_API_RT) && (sa < MAX_API_RT_MC_SA) && ( (sa_ty == API_RT_MC_TYPE_XMT) || (sa_ty == API_RT_SA_TYPE_XMT) ) )
          {
            rt_sa_action_cnt = p_api_dev->rt_setup[ dest_biu ].rt_action[ rt ].sa[ sa ];
            if(rt_sa_action_cnt != 0)
            {
              s1i_ir_rt_sa_profile(rt_sa_action_cnt, rt, sa, sa_ty, dbp);
            }
          }

          rt_param_comb_cnt = api_param_comb_setup.comb_cnt;
          if( (rt_param_comb_cnt != 0) && ( (sa_ty == API_RT_MC_TYPE_XMT) || (sa_ty == API_RT_SA_TYPE_XMT) ) )
          {
            s1i_ir_param_comb(db_id, dbp);
          }
  #endif 
        }
      }

      /* Call Application Specified Interrupt Handler */
      /* ... tbd ... */
      /* Interrupt event List */
      /* Interrupt Loglist structure changed rh,28.12.99 */
      if(p_api_dev->rt_setup[ dest_biu ].rt_int[ rt ].sa_rx[ sa ] == API_ON)
      {
        if( ( (sa_ty == API_RT_MC_TYPE_RCV) || (sa_ty == API_RT_SA_TYPE_RCV) ) )
        {
          if( p_api_dev->ul_DbgLevel & DBG_INTERRUPT_CNT)
            p_api_dev->ul_CntRTInt++;
          api_write_int_loli_gen_int(p_api_dev, ((dest_biu > (BIU_2-1)) ? (dest_biu+3) : dest_biu), w1, w2, (p_api_dev->ul_DbgLevel & DBG_INTERRUPT_CNT) ? p_api_dev->ul_CntRTInt : cw_id, db_id);
        }
      }
      if(p_api_dev->rt_setup[ dest_biu ].rt_int[ rt ].sa_tx[ sa ] == API_ON)
      {
        if( ( (sa_ty == API_RT_MC_TYPE_XMT) || (sa_ty == API_RT_SA_TYPE_XMT) ) )
        {
          if( p_api_dev->ul_DbgLevel & DBG_INTERRUPT_CNT)
            p_api_dev->ul_CntRTInt++;
          api_write_int_loli_gen_int(p_api_dev, ((dest_biu > (BIU_2-1)) ? (dest_biu+3) : dest_biu), w1, w2, (p_api_dev->ul_DbgLevel & DBG_INTERRUPT_CNT) ? p_api_dev->ul_CntRTInt : cw_id, db_id);
        }
      }
      if( (w1 & 0x01800000) != 0) /* RT Index, Event IR */
      {
        if( p_api_dev->ul_DbgLevel & DBG_INTERRUPT_CNT)
          p_api_dev->ul_CntRTInt++;
        api_write_int_loli_gen_int(p_api_dev, ((dest_biu > (BIU_2-1)) ? (dest_biu+3) : dest_biu), w1, w2, (p_api_dev->ul_DbgLevel & DBG_INTERRUPT_CNT) ? p_api_dev->ul_CntRTInt : cw_id, db_id);
      }
      if( 0 == (p_api_dev->ul_DbgLevel & DBG_INTERRUPT_CNT))
        p_api_dev->ul_CntRTInt = 0;
    }
  }

} /* end: api_ir_rt */
#endif 

/*****************************************************************************/
/*                                                                           */
/*   Module : API_IR                   Submodule : API_IR_BM                 */
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
/*    Inputs    : Interrupt Status LogList Entry [w1]                        */
/*                Interrupt Status LogList Entry [w2]                        */
/*                                                                           */
/*    Outputs   : none                                                       */
/*                                                                           */
/*    Description :                                                          */
/*    This function is used to handle a API1553-DS Bus Monitor Interrupt.    */
/*                                                                           */
/*****************************************************************************/

AiUInt8 map_biu_to_dq_id[] = { API_DATA_QUEUE_ID_BM_REC_BIU1,
                                API_DATA_QUEUE_ID_BM_REC_BIU2,
                                API_DATA_QUEUE_ID_BM_REC_BIU3,
                                API_DATA_QUEUE_ID_BM_REC_BIU4,
                                API_DATA_QUEUE_ID_BM_REC_BIU5,
                                API_DATA_QUEUE_ID_BM_REC_BIU6,
                                API_DATA_QUEUE_ID_BM_REC_BIU7,
                                API_DATA_QUEUE_ID_BM_REC_BIU8 };


#ifdef CMPL4MON
void api_ir_bm(TY_API_DEV *p_api_dev, short dest_biu, L_WORD w1, L_WORD w2)
{
    TY_API_DATA_QUEUE_HEADER * pDataQueueHeader;

  /* Set BM Interrupt Flag */
  p_api_dev->bm_ir_status[ dest_biu ] = API_ON;

  if(p_api_dev->bite_stream[ dest_biu ] == API_OFF)
  {

    if( (w1 &0x10000000) == 0x10000000)
    {
       /* UDF */
      if( (w1 &0x04000000) == 0x04000000)
      {
         /* MTI */
        /* Monitor Trigger Interrupt */
      }
      if( (w1 &0x02000000) == 0x02000000)
      {
         /* MBF */
        /* Monitor Buffer Full Interrupt */
          pDataQueueHeader = API_DATAQUEUE_HEADER(map_biu_to_dq_id[dest_biu]);

          if (pDataQueueHeader->status & API_DATA_QUEUE_STATUS_START)
          {
              /* Process the corresponding recording data queue */
#ifndef _NUCLEUS
              mil_tasks_dataqueue_process(p_api_dev, dest_biu);
#endif
          }

        /* Recording Mode */
        if(p_api_dev->bm_rec[ dest_biu ].mode == API_ON)
        {
          api_ir_bm_rec(p_api_dev, dest_biu, w2);
        }
      }
      if( (w1 &0x01000000) == 0x01000000)
      {
         /* MSI */
        /* Monitor Start Interrupt */
      }
      if( (w1 &0x00800000) == 0x00800000)
      {
         /* MST */
        /* Monitor Stop Interrupt */
      }

      if( (w1 & 0x00400000) == 0x00400000)
      {
         /* MFI */
        /* Monitor Filter Descriptor Interrupt */

        /* Dyntag Monitor check */         
         api_ir_bm_dyntagm(p_api_dev, dest_biu, w2);
      }

      /* Call Application Specified Interrupt Handler */
      /* ... tbd ... */

      /* Interrupt event List */
      api_write_int_loli_gen_int(p_api_dev, ((dest_biu > (BIU_2-1)) ? (dest_biu+3) : dest_biu), w1, w2, 0, 0);
    }
  }

} /* end: api_ir_bm */
#endif 


/*****************************************************************************/
/*                                                                           */
/*   Module : API_IR                   Submodule : API_IR_REP                */
/*                                                                           */
/*   Author : Michael Stahl            Project   : API1553-DS                */
/*                                                                           */
/*   Source : C                        Tools     : PC/AT; Norton Editor;     */
/*                                                 CYGNUS, GNU-C, As, and LD */
/*                                                 IDT-C 5.1 Toolkit         */
/*---------------------------------------------------------------------------*/
/*   Create : 01.08.99   Update : 25.05.00                                   */
/*---------------------------------------------------------------------------*/
/*    Descriptions                                                           */
/*    ------------                                                           */
/*    Inputs    : Interrupt Status LogList Entry [w1]                        */
/*                Interrupt Status LogList Entry [w2]                        */
/*                                                                           */
/*    Outputs   : none                                                       */
/*                                                                           */
/*    Description :                                                          */
/*    This function is used to handle a API1553-DS Replay Interrupt.         */
/*                                                                           */
/*****************************************************************************/

#ifdef CMPL4REP
void api_ir_rep(TY_API_DEV *p_api_dev, short dest_biu, L_WORD w1, L_WORD w2)
{

  /* Evaluate REP Interrupt Flags */
  if(p_api_dev->bite_stream[ dest_biu ] == API_OFF)
  {
    if( (w1 &0x10000000) == 0x10000000)
    {
       /* UDF */
      if( (w1 &0x00100000) == 0x00100000)
      {
         /* RPI */
        /* 25.05.00 : Increment Replay Half Buffer Full Interrupt */
        p_api_dev->sim_rep[ dest_biu ].rpi_cnt++;
        wbflush();
      }
      if( (w1 &0x00200000) == 0x00200000)
      {
         /* RSO */
        /* 25.05.00 : Replay Stop Interrupt TBD */
        /*  p_api_dev->sim_rep[dest_biu].mode = API_OFF; */
        wbflush();
      }

      /* 13.10.99 */
      /* Interrupt event List */
      /* Interrupt Loglist structure changed rh,28.12.99 */
      api_write_int_loli_gen_int(p_api_dev, ((dest_biu > (BIU_2-1)) ? (dest_biu+3) : dest_biu), w1, w2, 0, 0);
    }
  }

} /* end: api_ir_rep */
#endif 


/*****************************************************************************/
/*                                                                           */
/*   Module : API_IR                   Submodule : API_IR                    */
/*                                                                           */
/*   Author : Marco Motz               Project   : API1553-DS                */
/*                                                                           */
/*   Source : C                        Tools     : PC/AT; Norton Editor;     */
/*                                                 CYGNUS, GNU-C, As, and LD */
/*                                                 IDT-C 5.1 Toolkit         */
/*---------------------------------------------------------------------------*/
/*   Create : 01.05.98   Update : 25.05.00                                   */
/*---------------------------------------------------------------------------*/
/*    Descriptions                                                           */
/*    ------------                                                           */
/*    Inputs    : none                                                       */
/*                                                                           */
/*    Outputs   : none                                                       */
/*                                                                           */
/*    Description :                                                          */
/*                                                                           */
/*****************************************************************************/

void api_ir( TY_API_DEV * p_api_dev, short dest_biu )
{
  L_WORD            w1          = 0;
  L_WORD            w2          = 0;
  L_WORD            islp        = 0;
  L_WORD            cur_islp    = 0;
  L_WORD            loop_max    = 0;
  L_WORD            source      = 0;
  L_WORD volatile * ptr         = NULL;

  /* Perform Loglist Scan for both BIU */
  

#if defined(_NUCLEUS)
  for(dest_biu = 0; dest_biu < p_api_dev->ul_NbOfStreams1+p_api_dev->ul_NbOfStreams2; dest_biu++)
  {
#endif 

#ifndef _AMC1553
    if( API_TYPE_3910 == p_api_dev->auc_StreamType[dest_biu] )
      continue;
#endif

    UsbSynchronizeLWord(p_api_dev, API_GLB_MEM_ADDR_REL(&p_api_dev->cb_p[dest_biu]->ilp), In);

    /* Read Current Loglist Pointer */
    cur_islp = ul_ConvertGRAMPtrToTSWPtr(p_api_dev, dest_biu, (*p_api_dev->cb_p[ dest_biu ]).ilp);

    /* Last loglist pointer to which was read */
    islp = p_api_dev->islp[dest_biu];

    if(cur_islp > islp)
    {
        /* Get all new loglist entries */
        UsbSynchronizeMemoryArea(p_api_dev, API_GLB_PBIREL_TO_GLOBREL(islp),
                                 cur_islp - islp, In);
    }
    else if(cur_islp < islp)
    {
        /* wrap around in log list. Get entries from last read to end of log list */
        UsbSynchronizeMemoryArea(p_api_dev, API_GLB_PBIREL_TO_GLOBREL(islp),
            (p_api_dev->glb_mem.biu[ dest_biu ].ir_log_addr + (8 *MAX_API_ISLP_SIZE)) - islp, In);
        
        if(cur_islp > p_api_dev->glb_mem.biu[ dest_biu ].ir_log_addr)
        {
            /* Get entries from start of log list to current log list pointer */
            UsbSynchronizeMemoryArea(p_api_dev, 
                                     API_GLB_PBIREL_TO_GLOBREL(p_api_dev->glb_mem.biu[ dest_biu ].ir_log_addr),
                                     cur_islp - p_api_dev->glb_mem.biu[ dest_biu ].ir_log_addr, 
                                     In);
        }                           
    }

    if( (cur_islp != 0L) && ( (cur_islp - (p_api_dev->glb_mem.biu[ dest_biu ].ir_log_addr)) < 0x400L) )
    {

      /* Loglist Scanner */
      for(islp = p_api_dev->islp[ dest_biu ], loop_max = MAX_API_ISLP_SIZE; 
          (islp != cur_islp) && (loop_max != 0);
          loop_max--)
      {

        /* Read Loglist Entry */ 
        ptr = (L_WORD*)API_GLB_MEM_ADDR_ABS_IR(islp, dest_biu);
        w1 = BSWAP32(*(ptr + 0) ); /* Loglist Entry Word 1 */
        w2 = ul_ConvertGRAMPtrToTSWPtr(p_api_dev, dest_biu, (*(ptr + 1) & BSWAP32(0x03FFFFFFFUL)) ); /* Loglist Entry Word 2 */

        /*printf("--> API_IR: Loglist Entry = %08X %08X at islp=%08X\n\r",w1,w2,islp);*/
           
        /* Get Interrupt reason */
        source = w1 & 0xE0000000L;
        source >>= 29;

        /* Process Interrupt event */
        switch(source)
        {
#ifdef CMPL4SIM
          case 0:
             /* RT Interrupt */
            api_ir_rt(p_api_dev, dest_biu, w1, w2);
            break;
          case 1:
             /* BC Interrupt */
            api_ir_bc(p_api_dev, dest_biu, w1, w2);
            break;
          case 3:
             /* REPLAY Interrupt*/
            api_ir_rep(p_api_dev, dest_biu, w1, w2);
            break;
          case 4:
             /* BC Relative Branch Interrupt */
            api_ir_bc_skip(p_api_dev, dest_biu, w1, w2);
            break;
#endif 

#ifdef CMPL4MON
          case 2:
             /* BM */
            api_ir_bm(p_api_dev, dest_biu, w1, w2);
            break;
#endif 
          default:
            break;
        }

        /* Clear Update Flag */
        *ptr &= BSWAP32(0xEFFFFFFFL);

        /* Increment and check if wrapped around */
        islp += 8;
        if(islp >= (p_api_dev->glb_mem.biu[ dest_biu ].ir_log_addr + (8 *MAX_API_ISLP_SIZE) ) )
          islp = p_api_dev->glb_mem.biu[ dest_biu ].ir_log_addr;
      }

      /* Store last Loglist Pointer */
      p_api_dev->islp[ dest_biu ] = islp;
    }
#if defined(_NUCLEUS)
  }
#endif 
} /* end: api_ir */

/*****************************************************************************/
/*                                                                           */
/*   Module : API_IR                   Submodule : API_IR_TRANSP             */
/*                                                                           */
/*   Author : Marco Motz               Project   : API1553-DS                */
/*                                                                           */
/*   Source : C                        Tools     : PC/AT; Norton Editor;     */
/*                                                 CYGNUS, GNU-C, As, and LD */
/*                                                 IDT-C 5.1 Toolkit         */
/*---------------------------------------------------------------------------*/
/*   Create : 18.11.02   Update : 18.11.02                                   */
/*---------------------------------------------------------------------------*/
/*    Descriptions                                                           */
/*    ------------                                                           */
/*    Inputs    : none                                                       */
/*                                                                           */
/*    Outputs   : none                                                       */
/*                                                                           */
/*    Description :                                                          */
/*                                                                           */
/*****************************************************************************/

#if defined(_NUCLEUS)
void api_ir_transp( TY_API_DEV *p_api_dev, short dest_biu )
{
  L_WORD w1, w2, islp, cur_islp, loop_max;
  L_WORD volatile *ptr;

  /* Perform Loglist Scan for both BIU */
  for(dest_biu = 0; dest_biu < MAX_ISR_BIU_IR; dest_biu++)
  {

    /* Read Actual Loglist Pointer */
    cur_islp = ul_ConvertGRAMPtrToTSWPtr(p_api_dev, dest_biu, (*p_api_dev->cb_p[ dest_biu ]).ilp);

    if( (cur_islp != 0L) && ( (cur_islp - p_api_dev->glb_mem.biu[ dest_biu ].ir_log_addr) < 0x400L) )
    {

      /* Loglist Scanner */
      for(islp = p_api_dev->islp[ dest_biu ], loop_max = MAX_API_ISLP_SIZE; (islp != cur_islp) && (loop_max != 0); loop_max--)
      {

        /* Read Loglist Entry */
        ptr = (L_WORD*)API_GLB_MEM_ADDR_ABS_IR(islp, dest_biu);
        w1 = *(ptr + 0); /* Loglist Entry Word 1 */
        w2 = *(ptr + 1); /* Loglist Entry Word 2 */
        api_write_int_loli_gen_int(p_api_dev, ((dest_biu > (BIU_2-1)) ? (dest_biu+3) : dest_biu), w1, w2, 0, 0);

        /* Clear Update Flag */
        *ptr &= 0xEFFFFFFFL;

        /* Increment and check if wrapped around */
        islp += 8;
        if(islp >= (p_api_dev->glb_mem.biu[ dest_biu ].ir_log_addr + (8 *MAX_API_ISLP_SIZE) ) )
          islp = p_api_dev->glb_mem.biu[ dest_biu ].ir_log_addr;
      }

      /* Store last Loglist Pointer */
      p_api_dev->islp[ dest_biu ] = islp;
    }
  }

} /* end: api_ir_transp */
#endif 

/*****************************************************************************/
/*                                                                           */
/*   Module : API_IR                   Submodule : API_ISR                   */
/*                                                                           */
/*   Author : Marco Motz               Project   : API1553-DS                */
/*                                                                           */
/*   Source : C                        Tools     : PC/AT; Norton Editor;     */
/*                                                 CYGNUS, GNU-C, As, and LD */
/*                                                 IDT-C 5.1 Toolkit         */
/*---------------------------------------------------------------------------*/
/*   Create : 01.05.98   Update : 01.02.99                                   */
/*---------------------------------------------------------------------------*/
/*    Descriptions                                                           */
/*    ------------                                                           */
/*    Inputs    : none                                                       */
/*                                                                           */
/*    Outputs   : none                                                       */
/*                                                                           */
/*    Description :                                                          */
/*                                                                           */
/*****************************************************************************/
#ifndef _NUCLEUS
  #ifndef _AMC1553

void api_isr(L_WORD cpu_cause, L_WORD cpu_stat, EXFRAME_TY *exc)
{

  /* 01.02.99: Disable BIU Interrupt */
  disable_int(~(BIU1_INT | BIU2_INT) );

  api_ir( px_api_dev, -1 ); /* call ISR with unknown BIU */

} /* end: api_isr */

  #endif /* !_AMC1553 */
#else /* _NUCLEUS */

void API_HISR_Interrupt(void)
{
  /*printf("hisr int\r\n");*/
  api_ir( px_api_dev, -1 ); /* call ISR  with unknown BIU  */
}

void API_ISR1_Interrupt(INT vector_number)
{
  STATUS status_int;
  /*printf("lisr1 int\r\n");*/
  status_int = NU_Activate_HISR(&px_api_dev->hisr);
  if(status_int != NU_SUCCESS)
    PRINTF1("\n\r LISR error %08X\n\r", status_int);
}

void API_ISR2_Interrupt(INT vector_number)
{
  NU_Activate_HISR(&px_api_dev->hisr);
}

void API_ISR3_Interrupt(INT vector_number)
{
  NU_Activate_HISR(&px_api_dev->hisr);
}

void API_ISR4_Interrupt(INT vector_number)
{
  NU_Activate_HISR(&px_api_dev->hisr);
}

void API_ISR5_Interrupt(INT vector_number)
{
  NU_Activate_HISR(&px_api_dev->hisr);
}


void API_ISR6_Interrupt(INT vector_number)
{
  NU_Activate_HISR(&px_api_dev->hisr);
}


void API_ISR7_Interrupt(INT vector_number)
{
  NU_Activate_HISR(&px_api_dev->hisr);
}


void API_ISR8_Interrupt(INT vector_number)
{
  NU_Activate_HISR(&px_api_dev->hisr);
}





  #ifndef _AMC1553




void API_MSCOPE_Interrupt(INT vector_number)
{
  TY_MS_CONTROL_REG	   ControlReg;
  TY_API_DATA_QUEUE_HEADER *pDataQueueHeader;

  if( px_api_dev->ul_DbgLevel & DBG_TRACE_MILSCOPE )
    PRINTF1("API_MSCOPE_Interrupt(%dd)\r\n", vector_number );

  /* clear interrupt flag */
  IntFlags.flags.milscope = 1;

  /* Stop MilScope */
  memset( &ControlReg, 0, sizeof(TY_MS_CONTROL_REG) );

  ControlReg.bits.reset     = 1;
  ControlReg.bits.sing_cont = MSCOPE_CAP_DISABLE;
  p_MS_ControlReg->all      = ControlReg.all;

  /* Stop DMA */
  p_MS_NextDesAdrReg->all               = 0;
  p_MS_DmaDescCntReg->bits.dma_desc_cnt = 0;  


  /* Check if the dataque is open and send data */

  if( pDataQueueHeader->status & API_DATA_QUEUE_STATUS_START )
  {
    NU_Activate_HISR(&px_api_dev->milscope_hisr); /* API_MSCOPE_HISR */
  }
}


#define SCOPE_INFO_BLOCKSIZE 0x100

#define VALUE_IN_BLOCK(  value, block ) ( (value) % (block) )


void API_MSCOPE_HISR()
{
  TY_API_DEV         * p_api_dev           = px_api_dev;

  L_WORD             * pul_BufAddrDest     = 0;
  L_WORD             * pul_BufAddrSrc      = 0;

  L_WORD               ul_VirtualEndAddr   = 0;
  L_WORD               ul_VirtualTrgAddr   = 0;
  L_WORD               ul_VirtualEndBlock  = 0;
  L_WORD               ul_VirtualTrgBlock  = 0;

  L_WORD               ul_BytesToCopy      = 0;
  L_WORD               ul_MaxBlocks        = 0;
  L_WORD               ul_ByteOffset       = 0;
  L_WORD               ul_CopySecondPart   = TRUE;
  L_WORD               ulDataQueueSize     = 0;
  L_WORD               ulLongWordCount     = 0;
  L_WORD               ul_BytesToSkip      = 0;


  TY_API_DATA_QUEUE_HEADER *pDataQueueHeader;

  if( p_api_dev->ul_DbgLevel & DBG_TRACE_MILSCOPE )
    PRINTF0("API_MSCOPE_HISR()\r\n" );

  if( p_api_dev->dataqueue_header_offsets[API_DATA_QUEUE_ID_MIL_SCOPE] == 0 )
  {
      PRINTF2("ERROR %s %d: API_MSCOPE_Interrupt queue not initialized\r\n", __FUNCTION__, __LINE__ );
      return;
  }

  pDataQueueHeader = API_DATAQUEUE_HEADER(API_DATA_QUEUE_ID_MIL_SCOPE);

  /* -- Prepare source and destination pointers --- */
  pul_BufAddrSrc   = (L_WORD *)(ac_dis1adr_glob & 0x0FFFFFFF);  /* 0x07704000 */
  pul_BufAddrDest  = (L_WORD *)(API_SHARED_MEM_ADDR_ABS(pDataQueueHeader->put));

  /* -- Read the "Display Descriptor" values from the LCA --- */
  ul_VirtualEndBlock = *p_MS_DDES_vebn & 0x00001FFF;
  ul_VirtualTrgBlock = *p_MS_DDES_vtbn & 0x00001FFF;
  ul_VirtualEndAddr  = *p_MS_DDES_vea  & 0x000003FF;
  ul_VirtualTrgAddr  = *p_MS_DDES_vta  & 0x000003FF;


  /* -- Analyze data, and print statistik --- */
  if( 0 != (p_api_dev->ul_InstrLevel[INSTR_MILSCOPE] & INSTR_MILSCOPE_PRINT) )
  {

      mscope_print_data( p_api_dev, 
                       ul_VirtualEndBlock,
                       ul_VirtualTrgBlock,
                       ul_VirtualEndAddr,
                       ul_VirtualTrgAddr,
                       pul_BufAddrSrc, 
                       pul_BufAddrDest,
                       ac_dissize_glob );

  }


  /* -- Backup data for post analysis --- */
  
  if( 0 != (p_api_dev->ul_InstrLevel[INSTR_MILSCOPE] & INSTR_MILSCOPE_BACKUP_DATA) )
  {
    mscope_backup_data( p_api_dev, 
                        ul_VirtualEndBlock,
                        ul_VirtualTrgBlock,
                        ul_VirtualEndAddr,
                        ul_VirtualTrgAddr,
                        pul_BufAddrSrc, 
                        pul_BufAddrDest,
                        ac_dissize_glob );


  }


  if( pDataQueueHeader->status & API_DATA_QUEUE_STATUS_CAP_SINGLE )
  {
    /* -- single shot mode -- */
    /* No DMA is used for this mode, so we have to rescue the data from the Burst RAM.      */
    /* The MilScope fills the Burst RAM which is 2*MSCOPE_BRAM_SIZE in size as ring buffer. */
    /* The interrupt occures when the trace after trigger time is exceeded.                 */

    if( p_api_dev->ul_DbgLevel & DBG_MILSCOPE)
      PRINTF0("MilScope interrupt for single shot\r\n");

    /* -- copy data from milscope to shared ram --- */
    /* maybe this step can be omitted but it is a good idea to rescue the data */
    /* in the burst ram as soon as possible, it might be overwritten           */
    memcpy( pul_BufAddrSrc, (void *)MSCOPE_BRAM, 2 * MSCOPE_BRAM_SIZE );

    /* -- limit input values --- */
    if( ul_VirtualEndAddr >= (MSCOPE_BRAM_SIZE/8) )
    {
      /* 
         The end address is within a 2 block boundary relative to the last 
         even block.


                BRAM0 3K    BRAM1 3K
         Block  0           1
         Block  2           3
         Block  4           5
         Block  6           7
         Block  8           9
                .....
         Block 30          31
                   |
                   +------ ul_VirtualEndAddr relative to "even" block index
      */

      ul_VirtualEndAddr  = ul_VirtualEndAddr - (MSCOPE_BRAM_SIZE/8);

      if( (ul_VirtualEndBlock % 2) == 0)
        /* grml, grrrrrrr */
        ul_VirtualEndBlock++;
    }

    /* fix out of range virtual block */
    ul_VirtualEndBlock = ul_VirtualEndBlock % 2;



    /* -- calculate offsets for the blocks to be copied ---                       */
    /* The data is in a circular buffer                                           */
    /* In single shot mode the circular buffer is 2*MSCOPE_BRAM_SIZE in size      */
    /* The hardware provides the ul_VirtualEndAddr pointer which should always    */
    /* point to the trace befor trigger position. So that the distance between    */
    /* ul_VirtualTrgBlock and ul_VirtualEndAddr is the trace befor trigger part.  */
    /* We start to copy the ring buffer at the ul_VirtualEndAddr pointer and copy */
    /* 2*MSCOPE_BRAM_SIZE. In case the ul_VirtualEndAddr points to the beginning  */
    /* of the burst ram we use only one memcpy.                                   */

    /* -- prepare data copy --- */
    ul_ByteOffset     = ( ul_VirtualEndBlock * MSCOPE_BRAM_SIZE ) + ( ul_VirtualEndAddr * 8 );
    ul_BytesToCopy    = (                  2 * MSCOPE_BRAM_SIZE ) -   ul_ByteOffset;
    ul_BytesToSkip    = 0;

    if( ul_BytesToCopy >= (2 * MSCOPE_BRAM_SIZE ) )
      ul_CopySecondPart = FALSE;


    /* -- if copy is disabled, overwrite descriptors to copy the raw data --- */
    if( 0 != (p_api_dev->ul_InstrLevel[INSTR_MILSCOPE] & INSTR_MILSCOPE_NO_COPY) )
    {
      ul_ByteOffset     = 0;
      ul_BytesToCopy    = 2 * MSCOPE_BRAM_SIZE;
      ul_CopySecondPart = FALSE;
      ul_BytesToSkip    = 0;
    }

    /* -- calculate number of long words for post processing --- */
    ulLongWordCount = (MSCOPE_BRAM_SIZE*2)/4;

    /* Return data :
       SCOPE_INFO_BLOCKSIZE * 4 bytes per word
       MSCOPE_BRAM_SIZE     * 2 burst rams data
    */
    ulDataQueueSize = SCOPE_INFO_BLOCKSIZE*4;


  }
  else
  {
    /* -- continouse mode -- */
    /*		
     The milscope DMA copied the data into the shared ram.
     We do now copy the data into the data queue. The valid data starts at the end address
     in the end trigger block. So we copy from the end position to the end of the memory first.
     If not all data is copied this means the end position is not in the first block of the
     memory. In this case we copy the fist part of the memory to the end position in the second step.
     Because the milscope DMA does always copy blocks of MSCOPE_BRAM_SIZE it might happen that the data in the memory
     between the end block and the end address is invalid. The invalid data my grow up to a maximum
     of one block (MSCOPE_BRAM_SIZE). For this reason we subtract MSCOPE_BRAM_SIZE from the first part.
    */

    if( p_api_dev->ul_DbgLevel & DBG_MILSCOPE)
      PRINTF0("MilScope interrupt for continues capturing\r\n");

    /* get max number of 3KB blocks reflecting the queue size */
    ul_MaxBlocks = pDataQueueHeader->data_size/MSCOPE_BRAM_SIZE;


    if( ul_VirtualEndAddr >= (MSCOPE_BRAM_SIZE/8) )
    {
      /* 
         The end address is within a 2 block boundary relative to the last 
         even block.


                BRAM0 3K    BRAM1 3K
         Block  0           1
         Block  2           3
         Block  4           5
         Block  6           7
         Block  8           9
                .....
         Block 30          31
                   |
                   +------ ul_VirtualEndAddr relative to "even" block index
      */

      ul_VirtualEndAddr  = ul_VirtualEndAddr - (MSCOPE_BRAM_SIZE/8);

      if( (ul_VirtualEndBlock % 2) == 0)
        /* grml, grrrrrrr */
        ul_VirtualEndBlock++;
    }

    /* fix out of range virtual end block */
    ul_VirtualEndBlock = ul_VirtualEndBlock % ul_MaxBlocks;


    /* -- prepare copy data --- */
    /* ul_VirtualEndBlock is in steps of 3 KBytes       */
    /* ul_VirtualEndAddr  is in steps of 8 Bytes        */
    ul_ByteOffset     = (ul_VirtualEndBlock * MSCOPE_BRAM_SIZE) + ( ul_VirtualEndAddr * 8 );
    ul_BytesToCopy    = (ul_MaxBlocks       * MSCOPE_BRAM_SIZE) -   ul_ByteOffset;
    ul_BytesToSkip    = MSCOPE_BRAM_SIZE;

    if(ul_BytesToCopy >= (pDataQueueHeader->data_size - MSCOPE_BRAM_SIZE))
    {
      ul_CopySecondPart = FALSE;
      ul_BytesToCopy -= MSCOPE_BRAM_SIZE;
    }

    /* -- if copy is disabled, overwrite descriptors to copy the raw data --- */
    if( 0 != (p_api_dev->ul_InstrLevel[INSTR_MILSCOPE] & INSTR_MILSCOPE_NO_COPY) )
    {
      ul_ByteOffset     = 0;
      ul_BytesToCopy    = pDataQueueHeader->data_size;
      ul_BytesToSkip    = 0;
      ul_CopySecondPart = FALSE;
    }


    /* -- calculate number of long words for post processing --- */
    ulLongWordCount = ((pDataQueueHeader->data_size)/4)-SCOPE_INFO_BLOCKSIZE;

    /* -- copy all available data to host --- */
    /* One buffer is subtracted because it may contain invalid data,
       see description above */
    ulDataQueueSize += SCOPE_INFO_BLOCKSIZE*4;
  }



  /* -- clear destination --- */
  /* FIXME: If memset is called here, PBA.pro regression test fails in TaT test => why ?
  memset( (void*)pul_BufAddrDest, 0, ulDataQueueSize );
  */

  /* -- copy data into the data queue --- */
  /* copy end of memory to the beginning */
  memcpy( (BYTE *)&pul_BufAddrDest[SCOPE_INFO_BLOCKSIZE], (BYTE *)&(pul_BufAddrSrc[ul_ByteOffset/4]), ul_BytesToCopy);

  ulDataQueueSize += ul_BytesToCopy;

  if( ul_CopySecondPart )
  {
    /* append rest */
    if( ul_ByteOffset > ul_BytesToSkip )
    {
      memcpy( (BYTE *)&pul_BufAddrDest[SCOPE_INFO_BLOCKSIZE+ul_BytesToCopy/4], (BYTE *)&pul_BufAddrSrc[0],  ul_ByteOffset-ul_BytesToSkip );

      ulDataQueueSize += (ul_ByteOffset-ul_BytesToSkip);
    }
  }


  if( p_api_dev->ul_DbgLevel & DBG_MILSCOPE )
  {
    PRINTF3("MilScope COPY1 : memcpy(%08X, %08X, %d);\r\n", (BYTE *)&pul_BufAddrDest[SCOPE_INFO_BLOCKSIZE], (BYTE *)&(pul_BufAddrSrc[ul_ByteOffset/4]), ul_BytesToCopy );

    if( ul_CopySecondPart )
      if( ul_ByteOffset > ul_BytesToSkip )
        PRINTF3("MilScope COPY2 : memcpy(%08X, %08X,  %d);\r\n", (BYTE *)&pul_BufAddrDest[SCOPE_INFO_BLOCKSIZE+ul_BytesToCopy/4], (BYTE *)&pul_BufAddrSrc[0], ul_ByteOffset-ul_BytesToSkip );
  }

  /* -- postprocessing --- */
  mscope_add_calibration( p_api_dev, &pul_BufAddrDest[SCOPE_INFO_BLOCKSIZE], ulLongWordCount );
  mscope_reorder_samples( p_api_dev, &pul_BufAddrDest[SCOPE_INFO_BLOCKSIZE], ulLongWordCount );
  mscope_reorder_fix(     p_api_dev, &pul_BufAddrDest[SCOPE_INFO_BLOCKSIZE], ulLongWordCount ); 





  /* -- Backup dataqueue for post analysis --- */
  
  if( 0 != (p_api_dev->ul_InstrLevel[INSTR_MILSCOPE] & INSTR_MILSCOPE_BACKUP_DQUEUE) )
  {
    mscope_backup_dqueue( p_api_dev, 
                          pul_BufAddrDest, 
                          ulDataQueueSize );

  }




  /* Milscope can be started again */
  p_api_dev->ul_MilScopeRunning = 0;


  /* -- Copy data to data queue done, send size to host --- */
  pDataQueueHeader->put = pDataQueueHeader->put + ulDataQueueSize;

}

  #endif

#endif /* _NUCLEUS */

/*****************************************************************************/
/*                                                                           */
/*   Module : API_IR                   Submodule : API_ISR_TRANSP            */
/*                                                                           */
/*   Author : Marco Motz               Project   : API1553-DS                */
/*                                                                           */
/*   Source : C                        Tools     : PC/AT; Norton Editor;     */
/*                                                 CYGNUS, GNU-C, As, and LD */
/*                                                 IDT-C 5.1 Toolkit         */
/*---------------------------------------------------------------------------*/
/*   Create : 18.11.02   Update : 18.11.02                                   */
/*---------------------------------------------------------------------------*/
/*    Descriptions                                                           */
/*    ------------                                                           */
/*    Inputs    : none                                                       */
/*                                                                           */
/*    Outputs   : none                                                       */
/*                                                                           */
/*    Description :                                                          */
/*                                                                           */
/*****************************************************************************/
#ifndef _AMC1553
  #ifndef _NUCLEUS

void api_isr_transp(L_WORD cpu_cause, L_WORD cpu_stat, EXFRAME_TY *exc)
{

  /* 01.02.99: Disable BIU Interrupt */
  disable_int(~(BIU1_INT | BIU2_INT) );

  api_ir_transp( px_api_dev, -1 ); /* call ISR with unknown BIU */

} /* end: api_isr_transp */
  #endif 
#endif 

/*****************************************************************************/
/*                                                                           */
/*   Module : API_IR                   Submodule : API_GEN_INT2HOST          */
/*                                                                           */
/*   Author : Rolf Heitzmann           Project   : API-/AVI-/AXI-1553        */
/*                                                                           */
/*   Source : C                        Tools     : PC/AT;                    */
/*                                                 CYGNUS, GNU-C, As, and LD */
/*                                                 IDT-C 5.1 Toolkit         */
/*---------------------------------------------------------------------------*/
/*   Create : 27.12.99   Update : 12.05.00 rh                                */
/*---------------------------------------------------------------------------*/
/*    Descriptions                                                           */
/*    ------------                                                           */
/*    Inputs    : none                                                       */
/*                                                                           */
/*    Outputs   : none                                                       */
/*                                                                           */
/*    Description :                                                          */
/*                                                                           */
/*****************************************************************************/
void api1553_gen_int2host(TY_API_DEV *p_api_dev, L_WORD index)
{
#if !defined(_NUCLEUS)
 #if !defined(_AMC1553)
  L_WORD regVal; /* Register Value to write */
 #endif  
#else
  BYTE   *p_temp;
  L_WORD *pl_temp;
  TY_HW_INFORMATION *pHwInfo;
#endif

   /* PRINTF1("generate int to host (DevType:%d)\r\n", p_api_dev->uc_DeviceType); */

  switch(p_api_dev->uc_DeviceType)
  {
#ifdef _AMC1553
  case TYPE_APU1553_1:
  case TYPE_APU1553_2:
    /* not supported */
    break;
  case TYPE_AEC1553_1:
  case TYPE_AEC1553_2:
  case TYPE_AXC1553_1:
  case TYPE_AXC1553_2:
  case TYPE_AXC1553_4:
  case TYPE_AXE1553_1:
  case TYPE_AXE1553_2:
  case TYPE_AXE1553_4:
  case TYPE_ASE1553_1:
  case TYPE_ASE1553_2:
  case TYPE_ASE1553_4:
  case TYPE_AMCX1553_1:
  case TYPE_AMCX1553_2:
  case TYPE_AMCX1553_4:
  case TYPE_AMCE1553_1:
  case TYPE_AMCE1553_2:
  case TYPE_AMCE1553_4:
  case TYPE_APE1553_1:
  case TYPE_APE1553_2:
  case TYPE_APE1553_4:
  case TYPE_ANET1553_1:
  case TYPE_ANET1553_2:
    break;
#endif /*_AMC1553*/

/* AYX boards */
#if defined(_NUCLEUS)
  case TYPE_APX1553_1:
  case TYPE_APX1553_2:
  case TYPE_APX1553_4:
  case TYPE_APX3910:
  case TYPE_APX3910Xp:
  case TYPE_ACX1553_1:
  case TYPE_ACX1553_2:
  case TYPE_ACX1553_2_TWO_PBIS:
  case TYPE_ACX1553_4:
  case TYPE_ACX1553_4_TWO_PBIS:
  case TYPE_ACX1553_4_DS_TWO_PBIS:   
  case TYPE_ACX1553_8:
  case TYPE_ACX3910:
  case TYPE_ACX3910Xp:
  case TYPE_ACX3910_2:
  case TYPE_ACX3910Xp_2:
  case TYPE_ACX_EFA_1_TWO_PBIS:      
  case TYPE_ACX_EFAXp_1_TWO_PBIS:    
  case TYPE_ACX_EFA_1_DS_TWO_PBIS:   
  case TYPE_ACX_EFAXp_1_DS_TWO_PBIS: 
  case TYPE_ACX_EFA_2_TWO_PBIS:      
  case TYPE_ACX_EFAXp_2_TWO_PBIS:   
  case TYPE_ACX_EFA_2_DS_TWO_PBIS:   
  case TYPE_ACX_EFAXp_2_DS_TWO_PBIS: 
  case TYPE_ACX_EFA_4_TWO_PBIS:      
  case TYPE_ACX_EFAXp_4_TWO_PBIS:   
    p_temp = (BYTE*)p_ASL_Lca_IoRegs;
    p_temp[ 0x170 ] = 1;
    break;
  case TYPE_AVX1553_1:
  case TYPE_AVX1553_2:
  case TYPE_AVX1553_2_TWO_PBIS:
  case TYPE_AVX1553_4:
  case TYPE_AVX1553_4_TWO_PBIS:
  case TYPE_AVX1553_4_DS_TWO_PBIS:   
  case TYPE_AVX1553_8:
  case TYPE_AVX3910:
  case TYPE_AVX3910Xp:
  case TYPE_AVX3910_2:
  case TYPE_AVX3910Xp_2:
  case TYPE_AVX_EFA_1_TWO_PBIS:      
  case TYPE_AVX_EFA_1_DS_TWO_PBIS:   
  case TYPE_AVX_EFAXp_1_TWO_PBIS:    
  case TYPE_AVX_EFAXp_1_DS_TWO_PBIS: 
  case TYPE_AVX_EFA_2_TWO_PBIS:      
  case TYPE_AVX_EFAXp_2_TWO_PBIS:   
  case TYPE_AVX_EFA_2_DS_TWO_PBIS:   
  case TYPE_AVX_EFAXp_2_DS_TWO_PBIS: 
  case TYPE_AVX_EFA_4_TWO_PBIS:      
  case TYPE_AVX_EFAXp_4_TWO_PBIS:   
    pl_temp = (L_WORD*)(0xFB000440); /* TSI148 VICR register */
    pHwInfo = (TY_HW_INFORMATION*) (p_api_dev->SharedRAMBase);

    /* write Interrupt Vector 1..7 to IRQL in VICR */
    *pl_temp |= _BSWAP32((pHwInfo->ul_Interrupt & 0x007) << 8); /* VICR register is BIG endian */
    break;
#endif
  default:
    PRINTF1("ERROR in api1553_gen_int2host: unknown Device Type %i\r\n", p_api_dev->uc_DeviceType);
  }
  
} /* end: api1553_gen_int2host */


/*****************************************************************************/
/*                                                                           */
/*   Module : API_IR                   Submodule : API_WRITE_INT_LOLI_GEN_INT*/
/*                                                                           */
/*   Author : Rolf Heitzmann           Project   : API-/AVI-/AXI-1553        */
/*                                                                           */
/*   Source : C                        Tools     : PC/AT;                    */
/*                                                 CYGNUS, GNU-C, As, and LD */
/*                                                 IDT-C 5.1 Toolkit         */
/*---------------------------------------------------------------------------*/
/*   Create : 27.12.99   Update : 27.12.99                                   */
/*---------------------------------------------------------------------------*/
/*    Descriptions                                                           */
/*    ------------                                                           */
/*    Inputs    : none                                                       */
/*                                                                           */
/*    Outputs   : none                                                       */
/*                                                                           */
/*    Description :                                                          */
/*                                                                           */
/*****************************************************************************/

void api_write_int_loli_gen_int(TY_API_DEV *p_api_dev, WORD uw_IntType, L_WORD ul_w1, L_WORD ul_w2, L_WORD ul_w3, L_WORD ul_w4)
{
  L_WORD cnt;

  /* Get the index to the destination loglist */                     
  cnt = BSWAP32(p_api_dev->intr_list_p->put_cnt);

  /* write the loglist entries */
  p_api_dev->intr_list_p->entry[ cnt ].lla = BSWAP32(ul_w1);
  p_api_dev->intr_list_p->entry[ cnt ].llb = BSWAP32(ul_w2);
  p_api_dev->intr_list_p->entry[ cnt ].llc.ul_All = BSWAP32( (0x01000000 << uw_IntType) | (ul_w3 &0x00FFFFFF) );
  p_api_dev->intr_list_p->entry[ cnt ].lld.ul_All = BSWAP32( ((uw_IntType & 0xFF) << 24) | (ul_w4 &0x0000FFFF) );

  /* increment the loglist index */
  API_INC_LOLI_CNT(cnt);
  p_api_dev->intr_list_p->put_cnt = BSWAP32(cnt);

  /* generate a HW interrupt */
  api1553_gen_int2host(p_api_dev, uw_IntType);

  return ;

} /* end: api_write_int_loli_gen_int */

/*****************************************************************************/
/*                                                                           */
/*   Module : API_IR                   Submodule : API_IR_DATA_QUEUE         */
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
/*    Inputs    : Queue Index [queue_i]                                      */
/*                Data Buffer Pointer [dbp]                                  */
/*                                                                           */
/*    Outputs   : none                                                       */
/*                                                                           */
/*    Description :                                                          */
/*                                                                           */
/*****************************************************************************/

#ifdef CMPL4SIM


static L_WORD is_hs_queue(TY_API_DEV *p_api_dev, TY_API_DATA_QUEUE_HEADER *pDataQueueHeader)
{
    switch(p_api_dev->uc_DeviceType)
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
        if (pDataQueueHeader->id == 1)
            return 1;
        break;

    case TYPE_ACX3910_2:
    case TYPE_ACX3910Xp_2:
    case TYPE_AVX3910_2:
    case TYPE_AVX3910Xp_2:
        if ((pDataQueueHeader->id == 1) || (pDataQueueHeader->id == 3))
            return 1;
        break;

    case TYPE_ACX_EFA_1_TWO_PBIS:      
    case TYPE_ACX_EFAXp_1_TWO_PBIS:    
    case TYPE_ACX_EFA_1_DS_TWO_PBIS:   
    case TYPE_ACX_EFAXp_1_DS_TWO_PBIS: 
    case TYPE_AVX_EFA_1_TWO_PBIS:      
    case TYPE_AVX_EFA_1_DS_TWO_PBIS:   
    case TYPE_AVX_EFAXp_1_TWO_PBIS:    
    case TYPE_AVX_EFAXp_1_DS_TWO_PBIS: 
        if (pDataQueueHeader->id == 2)
            return 1;
        break;

    case TYPE_ACX_EFA_2_TWO_PBIS:      
    case TYPE_ACX_EFAXp_2_TWO_PBIS:   
    case TYPE_ACX_EFA_2_DS_TWO_PBIS:   
    case TYPE_ACX_EFAXp_2_DS_TWO_PBIS: 
    case TYPE_AVX_EFA_2_TWO_PBIS:      
    case TYPE_AVX_EFAXp_2_TWO_PBIS:   
    case TYPE_AVX_EFA_2_DS_TWO_PBIS:   
    case TYPE_AVX_EFAXp_2_DS_TWO_PBIS: 
        if (pDataQueueHeader->id == 3)
            return 1;
        break;

    case TYPE_ACX_EFA_4_TWO_PBIS:      
    case TYPE_ACX_EFAXp_4_TWO_PBIS:   
    case TYPE_AVX_EFA_4_TWO_PBIS:      
    case TYPE_AVX_EFAXp_4_TWO_PBIS:   
        if (pDataQueueHeader->id == 16)
            return 1;
        break;

    default:
        break;
    }

    return 0;
}




#define BM_ENTRY_SIZE 4

static AiBoolean is_recording_possible(TY_API_DEV *p_api_dev, AiUInt8 id )
{
    L_WORD ul_MonitorStatusWord;
    L_WORD ul_MonMode, ul_MonTat;
    L_WORD ul_MonDataCapturingActive;
    L_WORD ul_MonDataFirstCapturingActive;
    L_WORD ul_MonBufferFull; /* only used for 3910, EFEX */
    L_WORD ul_IsHsQueue = 0;
    short dest_stream = 0;
    
    TY_API_DATA_QUEUE_HEADER *pDataQueueHeader;

    pDataQueueHeader = API_DATAQUEUE_HEADER(id);

    if (p_api_dev->bm_status[pDataQueueHeader->biu] == API_BUSY)
    {
        UsbSynchronizeBMSystemControlBlock(p_api_dev, pDataQueueHeader->biu, In);
    }

    if (pDataQueueHeader->biu < p_api_dev->ul_NbOfStreams1)
        dest_stream = 0;
    else
        dest_stream = 1;



    /* -- Check if Data Queue is assigned to HS or LS biu --- */

    ul_IsHsQueue = is_hs_queue(p_api_dev, pDataQueueHeader);



    /* -- Read monitor status flags --- */

    if (ul_IsHsQueue)
    {
        ul_MonitorStatusWord           = BSWAP32((*px_api39_cb_p[dest_stream]).ul_msw);
        ul_MonDataCapturingActive      = ul_MonitorStatusWord & 0x04000000;
        ul_MonDataFirstCapturingActive = ul_MonitorStatusWord & 0x02000000;
        ul_MonBufferFull               = ul_MonitorStatusWord & 0x01000000;
        ul_MonMode                     = BSWAP32((*px_api39_cb_p[dest_stream]).ul_scw);
        ul_MonMode                     = (ul_MonMode & 0x00400000) >> 22;
        ul_MonTat                      = BSWAP32((*px_api39_cb_p[dest_stream]).ul_mtat);
    }
    else
    {
        if (p_api_dev->bm_status[pDataQueueHeader->biu] == API_BUSY)
        {
            UsbSynchronizeLWord(p_api_dev,
                API_GLB_MEM_ADDR_REL(&p_api_dev->cb_p[pDataQueueHeader->biu]->msw), In);
            UsbSynchronizeLWord(p_api_dev,
                API_GLB_MEM_ADDR_REL(&p_api_dev->cb_p[pDataQueueHeader->biu]->tat), In);
        }

        ul_MonitorStatusWord           = BSWAP32((*p_api_dev->cb_p[pDataQueueHeader->biu]).msw);
        ul_MonDataCapturingActive      = ul_MonitorStatusWord & 0x00020000;
        ul_MonDataFirstCapturingActive = ul_MonitorStatusWord & 0x00010000;
        ul_MonBufferFull               = 0;
        ul_MonMode                     = BSWAP32((*p_api_dev->cb_p[pDataQueueHeader->biu]).scw);
        ul_MonMode                     = (ul_MonMode & 0x00c00000) >> 22;
        ul_MonTat                      = BSWAP32((*p_api_dev->cb_p[pDataQueueHeader->biu]).tat);
    }



    /* -- Check for trigger --- */
    /* if monitor did not trigger yet, do nothing and exit immediately */
    /* monitor did not trigger if MFDCS is not set */
    if (0 == ul_MonDataFirstCapturingActive)
        return AiFalse;


    /* -- Check for trace after triggger --- */

    if (ul_IsHsQueue)
    {
        /* if tat is not reached yet in standard capture mode (3910), BFF is not set. In this case do not record anything, exit immediately */
        if ((0 == ul_MonMode) && (ul_MonTat != 0) && (0 == ul_MonBufferFull))
            return AiFalse;
    }
    else
    {
        /* if tat is not reached yet in standard capture mode, MDCA is still set. In this case do not record anything, exit immediately */
        if ((0 == ul_MonMode) && (ul_MonTat != 0) && ul_MonDataCapturingActive)
            return AiFalse;
    }

    /* -- check for queue errors --- */
    if ((pDataQueueHeader->status &API_DATA_QUEUE_STATUS_ERROR) == API_DATA_QUEUE_STATUS_ERROR)
        return AiFalse;


    /* -- check if queue is started --- */
    if ((pDataQueueHeader->status &API_DATA_QUEUE_STATUS_START) != API_DATA_QUEUE_STATUS_START)
        return AiFalse;


    /* -- Only print overflow error once --- */
    if ((pDataQueueHeader->status & API_DATA_QUEUE_STATUS_OVERFLOW) == API_DATA_QUEUE_STATUS_OVERFLOW)
        return AiFalse;


    return AiTrue;
}


static L_WORD data_queue_get_bm_start_and_size(TY_API_DEV *p_api_dev, TY_API_DATA_QUEUE_HEADER *pDataQueueHeader, L_WORD * bm_start, L_WORD * bm_size)
{
    L_WORD ucHs = 0;

    ucHs = is_hs_queue(p_api_dev, pDataQueueHeader);

    /* -- get trigger and BM pointers --- */
    if (p_api_dev->bm_status[pDataQueueHeader->biu] == API_BUSY)
    {
        UsbSynchronizeBMSystemControlBlock(p_api_dev, pDataQueueHeader->biu, In);
    }


    if (ucHs)
    {
        *bm_start = BSWAP32( (*px_api39_cb_p[ pDataQueueHeader->hs_stream ]).ul_mbba);
        *bm_start = API_GLB_PBIREL_TO_GLOBREL_BIU(*bm_start, pDataQueueHeader->biu);

        *bm_size  = BSWAP32( (*px_api39_cb_p[ pDataQueueHeader->hs_stream ]).ul_scw);
        *bm_size  = ((*bm_size)>>10) & 0xFF;
        *bm_size  = (*bm_size+1) * 64 * 1024;
    }
    else
    {
        *bm_start = BSWAP32((*p_api_dev->cb_p[pDataQueueHeader->biu]).mbba);
        *bm_start = API_GLB_PBIREL_TO_GLOBREL_BIU(*bm_start, pDataQueueHeader->biu);

        *bm_size  = BSWAP32((*p_api_dev->cb_p[pDataQueueHeader->biu]).scw);
        *bm_size  = ((*bm_size)>>8) & 0xFF;
        *bm_size  = (*bm_size+1) * 64 * 1024;
    }

    return 0;
}

static L_WORD data_queue_get_bm_offsets(TY_API_DEV *p_api_dev, TY_API_DATA_QUEUE_HEADER *pDataQueueHeader, L_WORD bm_start, L_WORD bm_size, L_WORD * bm_put, L_WORD * bm_get)
{
    L_WORD ucHs = 0;
    L_WORD ret = 0;
    TY_MIL_COM_LS_BM_STACK_PTR_READ_OUTPUT ls_bm_ptr;

#ifdef _API3910
    TY_MIL_COM_HS_BM_PTR_READ_OUTPUT hs_bm_ptr;
#endif

    if (p_api_dev->bm_status[pDataQueueHeader->biu] == API_BUSY)
    {
        UsbSynchronizeBMSystemControlBlock(p_api_dev, pDataQueueHeader->biu, In);
    }

    ucHs = is_hs_queue(p_api_dev, pDataQueueHeader);

    *bm_put = 0;
    *bm_get = 0;
    /* -- get trigger and BM pointers --- */

    if (pDataQueueHeader->triggered == 0)
    {
        if (ucHs)
        {
#ifdef _API3910
            ret = api39_bm_stack_ptr_read_ir(p_api_dev, pDataQueueHeader->biu, pDataQueueHeader->hs_stream, &hs_bm_ptr);

            if (ret != 0)
                return ret;

            pDataQueueHeader->triggered = hs_bm_ptr.status;

            *bm_get   = hs_bm_ptr.stp; /* Start of trace before trigger data */
            *bm_put   = hs_bm_ptr.etp; /* End of BM data */
#endif            
        }
        else
        {
            ret = api_bm_stack_ptr_read_ir(p_api_dev, pDataQueueHeader->biu, &ls_bm_ptr);

            if (ret != 0)
                return ret;

            pDataQueueHeader->triggered = (BYTE)ls_bm_ptr.status;

            /* This is the normal behavior for future fw releases where etp points to the next free entry */
            *bm_get = ls_bm_ptr.stp;     /* Start of trigger, no trace before trigger handling */
            *bm_put = ls_bm_ptr.etp;     /* End of BM data                                     */
        }

        /* Store trace before trigger start of data */
        (*p_api_dev->cb_p[ pDataQueueHeader->biu ]).bm_get = *bm_get;
    }
    else
    {
        /* We know the BM triggered already so we avoid parsing the whole stack again with StackpRead */
        *bm_put = BSWAP32(p_api_dev->cb_p[pDataQueueHeader->biu]->mbfp);
        *bm_put = API_GLB_PBIREL_TO_GLOBREL_BIU(*bm_put, pDataQueueHeader->biu);

        *bm_get = (*p_api_dev->cb_p[ pDataQueueHeader->biu ]).bm_get;
    }

    return 0;
}

static L_WORD data_queue_update_put(TY_API_DEV *p_api_dev, L_WORD id, L_WORD data_queue_put)
{
    TY_API_DATA_QUEUE_HEADER *pDataQueueHeader;

    pDataQueueHeader = API_DATAQUEUE_HEADER(id);

    pDataQueueHeader->put = data_queue_put;

    return 0;
}

static L_WORD data_queue_get_put(TY_API_DEV *p_api_dev, L_WORD id)
{
    TY_API_DATA_QUEUE_HEADER *pDataQueueHeader;

    pDataQueueHeader = API_DATAQUEUE_HEADER(id);

    return pDataQueueHeader->put;
}

static L_WORD data_queue_copy(TY_API_DEV *p_api_dev, L_WORD id, L_WORD bm_copy_start, L_WORD bm_copy_size, L_WORD * data_queue_put)
{
    L_WORD data_queue_free_bytes;
    L_WORD data_queue_free_bytes_til_end;
    L_WORD data_queue_free1;
    L_WORD i;

    TY_API_DATA_QUEUE_HEADER *pDataQueueHeader;

    L_WORD volatile *bm_read_pointer;
    L_WORD volatile *data_queue_write_pointer;

    pDataQueueHeader = API_DATAQUEUE_HEADER(id);


    if (p_api_dev->bm_status[pDataQueueHeader->biu] == API_BUSY)
    {
        UsbSynchronizeMonitorDataBuffer(p_api_dev, pDataQueueHeader->biu, In);
    }
    
    /* -- Get shared RAM data queue limits--- */
    data_queue_free_bytes = mil_tsw_buffer_producer_free_bytes(BM_ENTRY_SIZE, pDataQueueHeader->data_size, *data_queue_put, pDataQueueHeader->get);
    data_queue_free_bytes_til_end = mil_tsw_buffer_bytes_to_end(pDataQueueHeader->data_size, pDataQueueHeader->data_start, *data_queue_put);

    if (bm_copy_size > data_queue_free_bytes)
    {
        bm_copy_size = data_queue_free_bytes;
    }

    /* make copy size multiple of entry size */
    bm_copy_size = (bm_copy_size/BM_ENTRY_SIZE)*BM_ENTRY_SIZE;



    /* -- Calculate which areas of the buffer can be updated --- */
    if (data_queue_free_bytes > data_queue_free_bytes_til_end)
    {
        data_queue_free1 = data_queue_free_bytes_til_end;
    }
    else
    {
        data_queue_free1 = data_queue_free_bytes;
    }

    if (bm_copy_size <= data_queue_free1 )
    {
        bm_read_pointer          = (L_WORD volatile *)API_GLB_PBIREL_TO_TSWPTR_BIU(bm_copy_start, pDataQueueHeader->biu);
        data_queue_write_pointer = (L_WORD volatile *)API_SHARED_MEM_ADDR_ABS(*data_queue_put);

        /* copy single */
        /* bm_copy_start --> bm_copy_start + bm_copy_size; */
        for (i=0; i<bm_copy_size/BM_ENTRY_SIZE; i++)
        {
            data_queue_write_pointer[i] = bm_read_pointer[i];
        }

        mil_tsw_buffer_increment_offset(data_queue_put, bm_copy_size, pDataQueueHeader->data_start, pDataQueueHeader->data_size);
        bm_read_pointer += bm_copy_size;
    }
    else
    {
        /* copy chunk1 */
        bm_read_pointer          = (L_WORD volatile *)API_GLB_PBIREL_TO_TSWPTR_BIU(bm_copy_start, pDataQueueHeader->biu);
        data_queue_write_pointer = (L_WORD volatile *)API_SHARED_MEM_ADDR_ABS(*data_queue_put);

        /* bm_copy_start --> bm_copy_start + data_queue_free1; */
        for (i=0; i<data_queue_free1/BM_ENTRY_SIZE; i++)
        {
            data_queue_write_pointer[i] = bm_read_pointer[i];
        }

        mil_tsw_buffer_increment_offset(data_queue_put, data_queue_free1, pDataQueueHeader->data_start, pDataQueueHeader->data_size);
        bm_read_pointer += data_queue_free1/BM_ENTRY_SIZE;

        /* copy chunk2 */
        data_queue_write_pointer = (L_WORD volatile *)API_SHARED_MEM_ADDR_ABS(*data_queue_put);

        /* bm_copy_start+data_queue_free1 --> bm_copy_start + bm_copy_size */
        for (i=0; i<(bm_copy_size - data_queue_free1)/BM_ENTRY_SIZE; i++)
        {
            data_queue_write_pointer[i] = bm_read_pointer[i];
        }

        mil_tsw_buffer_increment_offset(data_queue_put, (bm_copy_size - data_queue_free1), pDataQueueHeader->data_start, pDataQueueHeader->data_size);
        bm_read_pointer += (bm_copy_size - data_queue_free1)/BM_ENTRY_SIZE;
    }


    return bm_copy_size;

}





void api_ir_data_queue(TY_API_DEV *p_api_dev, AiUInt8 id)
{
    TY_API_DATA_QUEUE_HEADER *pDataQueueHeader;

    L_WORD bm_available_bytes;
    L_WORD bm_available_bytes_til_end;
    L_WORD bm_size, bm_put, bm_get;
    L_WORD bm_chunk_1;
    L_WORD bm_chunk_2;
    L_WORD data_queue_put;
    L_WORD size;
    L_WORD bm_start;
    L_WORD msw_overflow_bitpos;


    pDataQueueHeader = API_DATAQUEUE_HEADER(id);

    if( !is_recording_possible(p_api_dev, id) )
        /* Nothing to do */
        return;

    if (is_hs_queue(p_api_dev, pDataQueueHeader))
        msw_overflow_bitpos = 24;
    else
        msw_overflow_bitpos = 18;



    /* -- Get global RAM data availability --- */

    data_queue_get_bm_start_and_size(p_api_dev, pDataQueueHeader, &bm_start, &bm_size);

    data_queue_get_bm_offsets(p_api_dev, pDataQueueHeader, bm_start, bm_size, &bm_put, &bm_get);

    bm_available_bytes         = mil_tsw_buffer_consumer_available_bytes(bm_size, bm_put, bm_get);
    bm_available_bytes_til_end = mil_tsw_buffer_bytes_to_end(bm_size, bm_start, bm_get);

    if( (bm_available_bytes > 0) && (p_api_dev->ul_DbgLevel & DBG_DQUEUE_IR) )
    {
        PRINTF2("%d bm_start                      = 0x%08X\r\n", pDataQueueHeader->biu, bm_start);
        PRINTF2("%d bm_size                       = 0x%08X\r\n", pDataQueueHeader->biu, bm_size);
        PRINTF2("%d bm_put                        = 0x%08X\r\n", pDataQueueHeader->biu, bm_put);
        PRINTF2("%d bm_get                        = 0x%08X\r\n", pDataQueueHeader->biu, bm_get);
        PRINTF2("%d bm_available_bytes            = 0x%08X\r\n", pDataQueueHeader->biu, bm_available_bytes);
        PRINTF2("%d bm_available_bytes_til_end    = 0x%08X\r\n", pDataQueueHeader->biu, bm_available_bytes_til_end);
    }

    if (bm_available_bytes > bm_available_bytes_til_end)
    {
        bm_chunk_1 = bm_available_bytes_til_end;
        bm_chunk_2 = bm_available_bytes - bm_available_bytes_til_end;
    }
    else
    {
        bm_chunk_1 = bm_available_bytes;
        bm_chunk_2 = 0;
    }

    /* -- Copy chunks --- */
    data_queue_put = data_queue_get_put(p_api_dev, id);

    if (bm_chunk_1)
    {
        size = data_queue_copy(p_api_dev, id, bm_get, bm_chunk_1, &data_queue_put);

        mil_tsw_buffer_increment_offset(&bm_get, size, bm_start, bm_size);
    
        (*p_api_dev->cb_p[ pDataQueueHeader->biu ]).bm_get = bm_get;
         
        if (size != bm_chunk_1)
            goto OVERLFOW;

    }

    if (bm_chunk_2)
    {
        size = data_queue_copy(p_api_dev, id, bm_get, bm_chunk_2, &data_queue_put);

        mil_tsw_buffer_increment_offset(&bm_get, size, bm_start, bm_size);

        (*p_api_dev->cb_p[ pDataQueueHeader->biu ]).bm_get = bm_get;

        if (size != bm_chunk_2)
            goto OVERLFOW;

    }

    /* check FW overflow flag*/
    if (((*p_api_dev->cb_p[pDataQueueHeader->biu]).msw >> msw_overflow_bitpos) & 0x1)
        goto OVERLFOW;

    /* -- Signal data availability to host --- */
    /* Not put must update at after both chunks are copied 
       in order to guarantee complete messages.
    */
    data_queue_update_put( p_api_dev, id, data_queue_put );

    /* successfull termination */
    return;

OVERLFOW:
    /* allow to read all the data that was copied until the overlfow occured */
    data_queue_update_put(p_api_dev, id, data_queue_put);

    /* overflow handling */
    pDataQueueHeader->status |= API_DATA_QUEUE_STATUS_OVERFLOW;

    PRINTF0("\r\n");
    PRINTF0("ERROR: API_DATA_QUEUE_STATUS_OVERFLOW\r\n");
    PRINTF1("pDataQueueHeader->status     = 0x%08X\r\n", pDataQueueHeader->status);
    PRINTF1("pDataQueueHeader->put        = 0x%08X\r\n", pDataQueueHeader->put);
    PRINTF1("pDataQueueHeader->get        = 0x%08X\r\n", pDataQueueHeader->get);
    PRINTF1("pDataQueueHeader->data_start = 0x%08X\r\n", pDataQueueHeader->data_start);
    PRINTF1("pDataQueueHeader->data_size  = 0x%08X\r\n", pDataQueueHeader->data_size);
    PRINTF1("bm_put                       = 0x%08X\r\n", bm_put);
    PRINTF1("bm_get                       = 0x%08X\r\n", bm_get);
    PRINTF1("bm_size                      = 0x%08X\r\n", bm_size);
    PRINTF1("bm_chunk_1                   = 0x%08X\r\n", bm_chunk_1);
    PRINTF1("bm_chunk_2                   = 0x%08X\r\n", bm_chunk_2);

    return;
}

#endif 




/*****************************************************************************/
/*                                                                           */
/*   Module : API_IR        Submodule : api_ir_enque_aye_discretes_from_fpga */
/*                                                                           */
/*   Author : Marco Motz            Project   : 1553-BSP                     */
/*                                                                           */
/*   Source : C                     Tools     : PC/AT; Norton Editor;        */
/*---------------------------------------------------------------------------*/
/*   Create : 20.02.17   Update : 20.02.17                                   */
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

#define GENERIC_ACQ_ENTRY_COUNT 4

void api_ir_enque_aye_discretes_from_fpga(TY_API_DEV *p_api_dev, L_WORD id, L_WORD tt_hi, L_WORD tt_lo, L_WORD val)
{
    TY_API_DATA_QUEUE_HEADER *pDataQueueHeader;
    L_WORD dw[GENERIC_ACQ_ENTRY_COUNT];
    L_WORD size = 0;

    pDataQueueHeader = API_DATAQUEUE_HEADER(API_DATA_QUEUE_ID_GENERIC_ACQ);

    if ((pDataQueueHeader->status & API_DATA_QUEUE_STATUS_START) != API_DATA_QUEUE_STATUS_START)
        return;

    size = GENERIC_ACQ_ENTRY_COUNT * BM_ENTRY_SIZE;

    dw[0] = (id << 24) | (1 << 16) | size;
    dw[1] = (tt_hi << 6) | (tt_lo >> 26);
    dw[2] = tt_lo & 0x03FFFFFFL;
    dw[3] = val;

    api_ir_data_queue_generic_acq(p_api_dev, pDataQueueHeader, 1, size, dw);

} /* end: api_ir_enque_aye_discretes_from_fpga */

/*****************************************************************************/
/*                                                                           */
/*   Module : API_IR               Submodule : API_IR_DATA_QUEUE_GENERIC_ACQ */
/*                                                                           */
/*   Author : Marco Motz            Project   : 1553-BSP                     */
/*                                                                           */
/*   Source : C                     Tools     : PC/AT; Norton Editor;        */
/*---------------------------------------------------------------------------*/
/*   Create : 22.12.2008   Update : 20.02.17                                 */
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

void api_ir_data_queue_generic_acq(TY_API_DEV *p_api_dev, TY_API_DATA_QUEUE_HEADER *pDataQueueHeader,
    L_WORD id, L_WORD size, L_WORD *data)
{
    L_WORD i;
    L_WORD copy_size;
    L_WORD data_queue_free_bytes;
    L_WORD data_queue_put;
    L_WORD data_queue_get;
    L_WORD package[GENERIC_ACQ_ENTRY_COUNT]; /* HEADER, HIGH, LOW, DATA */
    L_WORD volatile *data_queue_write_pointer;

    pDataQueueHeader = API_DATAQUEUE_HEADER(API_DATA_QUEUE_ID_GENERIC_ACQ);

    if ((pDataQueueHeader->status & API_DATA_QUEUE_STATUS_ERROR) == 0)
    {
        if ((pDataQueueHeader->status & API_DATA_QUEUE_STATUS_START) == API_DATA_QUEUE_STATUS_START)
        {
            /* Get actual Write Pointer value */
            data_queue_put = (L_WORD)pDataQueueHeader->put;
            data_queue_get = (L_WORD)pDataQueueHeader->get;
            copy_size      = (GENERIC_ACQ_ENTRY_COUNT * BM_ENTRY_SIZE);

            /* -- Get shared RAM data queue limits--- */
            data_queue_free_bytes = mil_tsw_buffer_producer_free_bytes(BM_ENTRY_SIZE, pDataQueueHeader->data_size, data_queue_put, data_queue_get);

            if (data_queue_free_bytes < copy_size)
            {
                goto OVERLFOW_GENERIC_ACQ;
            }

            if (p_api_dev->ul_DeviceGroup == AI_DEVICE_AYE)
            {
                package[0] = *(data + 0);
                package[1] = *(data + 1);
                package[2] = *(data + 2);
                package[3] = *(data + 3);
            }
            else
            {
#ifdef _CMPL4DTS
                package[0] = (id << 28) | (ip_drv[id].ip_type << 24) | (ip_drv[id].ip_setup.din.model << 16) | (12 + 4 * size);
#else
                package[0] = (id << 24) | (1 << 16) | (12 + 4 * size);
#endif
                package[1] = p_api_dev->api_irg_time.tm_hi;
                package[2]  = p_api_dev->api_irg_time.tm_lo;
                package[3]  = *data;
            }

            for (i=0; i<GENERIC_ACQ_ENTRY_COUNT; i++)
            {
                data_queue_write_pointer = (L_WORD volatile *)API_SHARED_MEM_ADDR_ABS(data_queue_put);

                /* copy entry */
                *data_queue_write_pointer = package[i];

                mil_tsw_buffer_increment_offset(&data_queue_put, BM_ENTRY_SIZE, pDataQueueHeader->data_start, pDataQueueHeader->data_size);
            }

            pDataQueueHeader->put = data_queue_put;

#ifdef _CMPL4DEBUG_
            /* Debug */
            api_buf_ram_wr_lword(p_api_dev, 0x3f0, pDataQueueHeader->put);
            api_buf_ram_wr_lword(p_api_dev, 0x3f4, pDataQueueHeader->get);
            api_buf_ram_wr_lword(p_api_dev, 0x3f8, pDataQueueHeader->data_start);
            api_buf_ram_wr_lword(p_api_dev, 0x3fc, pDataQueueHeader->data_size);
            api_buf_ram_wr_lword(p_api_dev, 0x3d0, data_queue_free_bytes);
            api_buf_ram_wr_lword(p_api_dev, 0x3d4, pDataQueueHeader->status);
            api_buf_ram_wr_lword(p_api_dev, 0x3d8, (*p_api_dev->cb_p[ pDataQueueHeader->biu ]).bm_get);
            api_buf_ram_wr_lword(p_api_dev, 0x3dc, 0xcafecafe);
#endif
        }
    }


    /* successfull termination */
    return;

OVERLFOW_GENERIC_ACQ:
    /* overflow handling */
    pDataQueueHeader->status |= API_DATA_QUEUE_STATUS_OVERFLOW;

    PRINTF0("\r\n");
    PRINTF0("ERROR: API_DATA_QUEUE_STATUS_OVERFLOW\r\n");
    PRINTF1("pDataQueueHeader->status     = 0x%08X\r\n", pDataQueueHeader->status);
    PRINTF1("pDataQueueHeader->put        = 0x%08X\r\n", pDataQueueHeader->put);
    PRINTF1("pDataQueueHeader->get        = 0x%08X\r\n", pDataQueueHeader->get);
    PRINTF1("pDataQueueHeader->data_start = 0x%08X\r\n", pDataQueueHeader->data_start);
    PRINTF1("pDataQueueHeader->data_size  = 0x%08X\r\n", pDataQueueHeader->data_size);

    return;


} /* end: api_ir_data_queue_generic_acq */

#ifdef _CMPL4DTS

/*****************************************************************************/
/*                                                                           */
/*   Module : API_IR                   Submodule : API_IR_DISCR_REC          */
/*                                                                           */
/*   Author : Marco Motz               Project   : API-/AVI-/AXI-1553        */
/*                                                                           */
/*   Source : C                        Tools     : PC/AT;                    */
/*                                                 CYGNUS, GNU-C, As, and LD */
/*                                                 IDT-C 5.1 Toolkit         */
/*---------------------------------------------------------------------------*/
/*   Create : 22.10.10     Update : 22.10.10                                 */
/*---------------------------------------------------------------------------*/
/*    Descriptions                                                           */
/*    ------------                                                           */
/*    Inputs    : none                                                       */
/*                                                                           */
/*    Outputs   : none                                                       */
/*                                                                           */
/*    Description :                                                          */
/*                                                                           */
/*****************************************************************************/
void api_ir_testsignal(TY_API_DEV *p_api_dev,L_WORD id)
{
    L_WORD tmpl, lval, chn;

    chn = (ip_drv[id].stask_setup.ena >> 4) & 0x0FL;
 
    if (chn != 0) {
        tmpl = ip_drv[id].out_data[0];
        lval = (tmpl >> (chn-1)) & 0x01L;
        if (lval == 1L) {
            /* RESET Discrete */
            LWORD_RESET_BITS(tmpl,(chn-1),1);
        }   
        else {
            /* SET Discrete */
            LWORD_SET_BITS(tmpl,1,(chn-1),1);
        }
        ip_drv[id].out_data[0] = tmpl; 
    }   
}
/*****************************************************************************/
/*                                                                           */
/*   Module : API_IR                   Submodule : API_IR_DISCR_REC          */
/*                                                                           */
/*   Author : Marco Motz               Project   : API-/AVI-/AXI-1553        */
/*                                                                           */
/*   Source : C                        Tools     : PC/AT;                    */
/*                                                 CYGNUS, GNU-C, As, and LD */
/*                                                 IDT-C 5.1 Toolkit         */
/*---------------------------------------------------------------------------*/
/*   Create : 22.10.10     Update : 22.10.10                                 */
/*---------------------------------------------------------------------------*/
/*    Descriptions                                                           */
/*    ------------                                                           */
/*    Inputs    : none                                                       */
/*                                                                           */
/*    Outputs   : none                                                       */
/*                                                                           */
/*    Description :                                                          */
/*                                                                           */
/*****************************************************************************/

void api_ir_discr_rec(TY_API_DEV *p_api_dev,L_WORD id)
{
    L_WORD tmpl;
    TY_API_DATA_QUEUE_HEADER *pDataQueueHeader;

    pDataQueueHeader = API_DATAQUEUE_HEADER(API_DATA_QUEUE_ID_GENERIC_ACQ);

    if( (pDataQueueHeader->status & API_DATA_QUEUE_STATUS_START) != API_DATA_QUEUE_STATUS_START)
        return;

    /* Read Input data */
    ip_drv[id].p_din_read_func(id,0/*all*/,1/* read from value not from HW*/,0,&tmpl,0);

    if ((ip_drv[id].stask_setup.ena & 0x02L) == 0)
    {
        /* Check on state change */
        if (tmpl != ip_drv[id].stask_setup.res)
        {
            ip_drv[id].stask_setup.res = tmpl;
            api_ir_data_queue_generic_acq(p_api_dev,pDataQueueHeader,id/*Slot*/,1/*size*/,&tmpl);
        }
        else
        {
            /* record first entry after start */
            if (ip_drv[id].rec_status.entry_cnt == 0L)
            {
                ip_drv[id].rec_status.entry_cnt++;
                api_ir_data_queue_generic_acq(p_api_dev,pDataQueueHeader,id/*Slot*/,1/*size*/,&tmpl);
            }
        }
    }
    else
    {
        /* Record each sample */
        api_ir_data_queue_generic_acq(p_api_dev,pDataQueueHeader,id/*Slot*/,1/*size*/,&tmpl);
    }
}

/*****************************************************************************/
/*                                                                           */
/*   Module : API_IR                   Submodule : API_IR_ANA_REC            */
/*                                                                           */
/*   Author : Marco Motz               Project   : API-/AVI-/AXI-1553        */
/*                                                                           */
/*   Source : C                        Tools     : PC/AT;                    */
/*                                                 CYGNUS, GNU-C, As, and LD */
/*                                                 IDT-C 5.1 Toolkit         */
/*---------------------------------------------------------------------------*/
/*   Create : 22.10.10     Update : 22.10.10                                 */
/*---------------------------------------------------------------------------*/
/*    Descriptions                                                           */
/*    ------------                                                           */
/*    Inputs    : none                                                       */
/*                                                                           */
/*    Outputs   : none                                                       */
/*                                                                           */
/*    Description :                                                          */
/*                                                                           */
/*****************************************************************************/

void  api_ir_ana_rec(TY_API_DEV *p_api_dev,L_WORD id)
{
    L_WORD tmpl,i;
    TY_API_DATA_QUEUE_HEADER * pDataQueueHeader;

    pDataQueueHeader = API_DATAQUEUE_HEADER(API_DATA_QUEUE_ID_GENERIC_ACQ);
    tmpl = 0;

    if( (pDataQueueHeader->status & API_DATA_QUEUE_STATUS_START) != API_DATA_QUEUE_STATUS_START)
        return;

    if ((ip_drv[id].stask_setup.ena & 0x02L) == 0) {

        /* Check on state change 8 Inputs*/
        for (i = 0; i < 8; i++)
        {
            if(((INT16)(ip_drv[id].rec_data[i]+ip_drv[id].stask_setup.ustep) < (INT16)ip_drv[id].in_data[i]) ||
               ((INT16)(ip_drv[id].rec_data[i]-ip_drv[id].stask_setup.ustep) > (INT16)ip_drv[id].in_data[i]))
            {
                ip_drv[id].rec_data[i] = (WORD)ip_drv[id].in_data[i];
                tmpl = 1;
            }
        }
        /* Check on state change 4 Outputs*/
        for (i = 0; i < 4; i++)
        {
            if(ip_drv[id].rec_data[i+8] != (WORD)ip_drv[id].out_data[i])
            {
                ip_drv[id].rec_data[i+8] = (WORD)ip_drv[id].out_data[i];
                tmpl = 1;
            }
        }

        if (tmpl == 1)
        {
            api_ir_data_queue_generic_acq(p_api_dev,pDataQueueHeader,id/*Slot*/,(8+4)/2/*size*/,(L_WORD*)ip_drv[id].rec_data);
        }
        else
        {
            /* record first entry after start */
            if (ip_drv[id].rec_status.entry_cnt == 0L)
            {
                ip_drv[id].rec_status.entry_cnt++;

                /* Copy 8 Inputs*/
                for (i = 0; i < 8; i++)
                {
                    ip_drv[id].rec_data[i] = (WORD)ip_drv[id].in_data[i];
                }
                /* Copy 4 Outputs*/
                for (i = 0; i < 4; i++)
                {
                    ip_drv[id].rec_data[i+8] = (WORD)ip_drv[id].out_data[i];
                }
                api_ir_data_queue_generic_acq(p_api_dev,pDataQueueHeader,id/*Slot*/,(8+4)/2/*size*/,(L_WORD*)ip_drv[id].rec_data);
            }
        }
    }  
    else
    {
        /* Record each sample */
        /* Copy 8 Inputs*/
        for (i = 0; i < 8; i++)
        {
            ip_drv[id].rec_data[i] = ip_drv[id].in_data[i];
        }
        /* Copy 4 Outputs*/
        for (i = 0; i < 4; i++)
        {
            ip_drv[id].rec_data[i+8] = ip_drv[id].out_data[i];
        }
        api_ir_data_queue_generic_acq(p_api_dev,pDataQueueHeader,id/*Slot*/,(8+4)/2/*size*/,(L_WORD*)ip_drv[id].rec_data);
    }

}/* end: api_ir_ana_rec */


#endif

/*****************************************************************************/
/*                                                                           */
/*   Module : API_IR                   Submodule : API_RTI1_ISR              */
/*                                                                           */
/*   Author : Thomas Jahn              Project   : API-/AVI-/AXI-1553        */
/*                                                                           */
/*   Source : C                        Tools     : PC/AT;                    */
/*                                                 CYGNUS, GNU-C, As, and LD */
/*                                                 IDT-C 5.1 Toolkit         */
/*---------------------------------------------------------------------------*/
/*   Create : 17.12.2002   Update : 22.10.10                                 */
/*---------------------------------------------------------------------------*/
/*    Descriptions                                                           */
/*    ------------                                                           */
/*    Inputs    : none                                                       */
/*                                                                           */
/*    Outputs   : none                                                       */
/*                                                                           */
/*    Description :                                                          */
/*                                                                           */
/*****************************************************************************/
#ifndef _NUCLEUS
#ifndef _AMC1553
static void api_rti1(TY_API_DEV *p_api_dev, L_WORD ul_ApiIsrRtiSec, L_WORD ul_ApiIsrUSecStep)
{
#ifdef IRIG
static L_WORD toggle = 0;
#endif 
L_WORD time_lo, time_hi, time_usec, time_sec, time_min, time_hour, time_day;
L_WORD t_day, t_hour, t_min, t_sec, t_usec;
L_WORD sth, stl, sync_flag;
TY_HW_INFORMATION *lptr;

#if defined(_NUCLEUS)
L_WORD tmpl, id, chn;
#endif

#ifdef _CMPL4DTS
TY_API_DATA_QUEUE_HEADER *pDataQueueHeader;
#endif




#if defined(_NUCLEUS)    /* only for AyX */
  if( p_api_dev->ul_GenIo_TestSeq_Running != 0 )
      api_gen_io_test_seq_handle_event(p_api_dev, 500);
#endif


  #ifdef IRIG
  lptr = (TY_HW_INFORMATION*) (p_api_dev->SharedRAMBase);
  sync_flag = 0;

  if(++toggle >= ul_ApiIsrRtiSec)
  {
      toggle = 0;

      /* Check on NO COMMAND */
      mil_hw_irig_get_time(p_api_dev, &t_day, &t_hour, &t_min, &t_sec, &t_usec, &sth, &stl);
      sync_flag = 1;

      if( sync_flag == 1 )
      {
          p_api_dev->api_irg_time.tm_lo = stl;
          p_api_dev->api_irg_time.tm_hi = sth;
          lptr->ul_IrigTimeLo = p_api_dev->api_irg_time.tm_lo;
          lptr->ul_IrigTimeHi = p_api_dev->api_irg_time.tm_hi;
      }
  }

  /* If time was not read from hw => interpolate */
  if(sync_flag == 0)
  {
    /* Read Global Time */
    time_lo = p_api_dev->api_irg_time.tm_lo;
    time_hi = p_api_dev->api_irg_time.tm_hi;

    time_usec = time_lo  & 0x000FFFFFL;
    time_sec = (time_lo  & 0x03F00000L) >> 20;
    time_min = (time_lo  & 0xFC000000L) >> 26;
    time_hour = (time_hi & 0x000007C0L) >> 6;
    time_day = (time_hi  & 0x000FF800L) >> 11;

    time_usec += ul_ApiIsrUSecStep;
    if(time_usec >= 1000000L)
    {
      time_sec += (time_usec / 1000000L);
      time_usec = (time_usec % 1000000L);
    }
    if(time_sec >= 60L)
    {
      time_min += (time_sec / 60L);
      time_sec = (time_sec % 60L);
    }
    if(time_min >= 60L)
    {
      time_hour += (time_min / 60L);
      time_min = (time_min % 60L);
    }
    if(time_hour >= 24L)
    {
      time_day += (time_hour / 24L);
      time_hour = (time_hour % 24L);
    }

    time_lo = 0L;
    time_hi = 0L;
    time_lo = ( (time_min & 0x3FL) << 26)  | ( (time_sec & 0x3FL) << 20) | (time_usec & 0x000FFFFFL);
    time_hi = ( (time_day & 0x1FFL) << 11) | ( (time_hour & 0x1FL) << 6) | (time_min & 0x3FL);

    /* Write back */
    p_api_dev->api_irg_time.tm_lo = time_lo;
    p_api_dev->api_irg_time.tm_hi = time_hi;
    lptr->ul_IrigTimeLo = p_api_dev->api_irg_time.tm_lo;
    lptr->ul_IrigTimeHi = p_api_dev->api_irg_time.tm_hi;
  }

   /*****************************************/
   /*** SAMPLING TASK FOR I/O INTERFACES  ***/
   /*****************************************/
#ifdef _CMPL4DTS
  pDataQueueHeader = API_DATAQUEUE_HEADER(API_DATA_QUEUE_ID_MIL_SCOPE);

   for (id=0; id<MAX_IP_DRV; id++) {
	   /* APX Discretes */
       if (ip_drv[id].ip_type == API_APX_IP_TYPE) {
		   if ((ip_drv[id].used != 0) && (ip_drv[id].p_din_read_func != NULL)) {

		      /* INPUT */
		      if (ip_drv[id].stask_setup.isr_cnt >= ip_drv[id].stask_setup.rate) {
		         ip_drv[id].stask_setup.isr_cnt = 0L;
		         ip_drv[id].p_din_read_func(id,0/*allchn*/,0,0,&tmpl,0);
			  }
			  else if ((ip_drv[id].stask_setup.ena & 0x01L) == API_ON) {
		         ip_drv[id].p_din_read_func(id,0/*allchn*/,0,0,&tmpl,0);
			  }
			  ip_drv[id].stask_setup.isr_cnt++;

		      /* OUTPUT */  
		      if(ip_drv[id].out_data[0] != ip_drv[id].act_data[0]) {
		         api_io_dig_out_write_apx(id, ip_drv[id].out_data[0]);
				 ip_drv[id].act_data[0] = ip_drv[id].out_data[0];
			  }

		      /* Testsignal */  
	          if ((ip_drv[id].stask_setup.ena & 0x01L) == API_ON) {
		          if (ip_drv[id].stask_setup.isr_cnt >= ip_drv[id].stask_setup.rate) {
			         api_ir_testsignal(p_api_dev,id);
				  }
			  }
		   }
	   }
       /* MILScope active ??? */
       if( (pDataQueueHeader->status & API_DATA_QUEUE_STATUS_START) != API_DATA_QUEUE_STATUS_START )
	   {
		   /* Discretes */
	       if (ip_drv[id].ip_type == API_DIG_IP_TYPE) {
			   if ((ip_drv[id].used != 0) && (ip_drv[id].p_din_read_func != NULL)) {

			      /* INPUT */
			      if (ip_drv[id].stask_setup.isr_cnt >= ip_drv[id].stask_setup.rate) {
			         ip_drv[id].stask_setup.isr_cnt = 0L;
			         ip_drv[id].p_din_read_func(id,0/*allchn*/,0,0,&tmpl,0);
				  }
				  else if ((ip_drv[id].stask_setup.ena & 0x01L) == API_ON) {
			         ip_drv[id].p_din_read_func(id,0/*allchn*/,0,0,&tmpl,0);
				  }
				  ip_drv[id].stask_setup.isr_cnt++;

			      /* OUTPUT */  
			      if(ip_drv[id].out_data[0] != ip_drv[id].act_data[0]) {
					  if( ip_drv[id].ip_setup.din.model == 8 )
  						api_io_dig_out_write_ip470(id, ip_drv[id].out_data[0], 0);
					  else
 						api_io_dig_out_write_ip408(id, ip_drv[id].out_data[0]);

					  ip_drv[id].act_data[0] = ip_drv[id].out_data[0];
				  }
			      /* Testsignal */  
	              if ((ip_drv[id].stask_setup.ena & 0x01L) == API_ON) {
			         if (ip_drv[id].stask_setup.isr_cnt >= ip_drv[id].stask_setup.rate) {
				        api_ir_testsignal(p_api_dev,id);
					 }
				  }
			   }
		   } /* end Discretes */

		   /* Analog */
	       if (ip_drv[id].ip_type == API_ANA_IP_TYPE) {
			   if ((ip_drv[id].used != 0) && (ip_drv[id].p_din_read_func != NULL)) {

				  /* Analog Input */
			      switch(ip_drv[id].stask_setup.isr_cnt){
				    case 0:
					     /* Channel1*/
					     /* write to ctrl register */
						 chn = 1;
				         api_io_ana_out_write_ip850(id, 0, 0x90/* Automatic Setting Time + Differential*/ + (chn-1));
				  	     ip_drv[id].stask_setup.isr_cnt++;	  /* next channel*/
						 break;
					case 1:
				         /* read status register */
						 chn = 1;
				         tmpl = api_io_ana_in_read_ip850(id,0);
						 if ((tmpl & 0xC000) == 0){
				           ip_drv[id].in_data[chn-1] = api_io_ana_in_read_ip850(id,chn);
						 }
					     /* Channel2*/
					     /* write to ctrl register */
						 chn = 2;
				         api_io_ana_out_write_ip850(id, 0, 0x90/* Automatic Setting Time + Differential*/ + (chn-1));

						 ip_drv[id].stask_setup.isr_cnt++;	  /* next channel*/
					     break;
				    case 2:
				         /* read status register */
						 chn = 2;
				         tmpl = api_io_ana_in_read_ip850(id,0);
						 if ((tmpl & 0xC000) == 0){
				           ip_drv[id].in_data[chn-1] = api_io_ana_in_read_ip850(id,chn);
						 }
					     /* Channel3*/
					     /* write to ctrl register */
						 chn = 3;
				         api_io_ana_out_write_ip850(id, 0, 0x90/* Automatic Setting Time + Differential*/ + (chn-1));

						 ip_drv[id].stask_setup.isr_cnt++;	  /* next channel*/
					     break;
					case 3:
				         /* read status register */
						 chn = 3;
				         tmpl = api_io_ana_in_read_ip850(id,0);
						 if ((tmpl & 0xC000) == 0){
				           ip_drv[id].in_data[chn-1] = api_io_ana_in_read_ip850(id,chn);
						 }
					     /* Channel4*/
					     /* write to ctrl register */
						 chn = 4;
				         api_io_ana_out_write_ip850(id, 0, 0x90/* Automatic Setting Time + Differential*/ + (chn-1));

						 ip_drv[id].stask_setup.isr_cnt++;	  /* next channel*/
					     break;
					case 4:
				         /* read status register */
						 chn = 4;
				         tmpl = api_io_ana_in_read_ip850(id,0);
						 if ((tmpl & 0xC000) == 0){
				           ip_drv[id].in_data[chn-1] = api_io_ana_in_read_ip850(id,chn);
						 }
					     /* Channel5*/
					     /* write to ctrl register */
						 chn = 5;
				         api_io_ana_out_write_ip850(id, 0, 0x90/* Automatic Setting Time + Differential*/ + (chn-1));

						 ip_drv[id].stask_setup.isr_cnt++;	  /* next channel*/
					     break;
					case 5:
				         /* read status register */
						 chn = 5;
				         tmpl = api_io_ana_in_read_ip850(id,0);
						 if ((tmpl & 0xC000) == 0){
				           ip_drv[id].in_data[chn-1] = api_io_ana_in_read_ip850(id,chn);
						 }
					     /* Channel6*/
					     /* write to ctrl register */
						 chn = 6;
				         api_io_ana_out_write_ip850(id, 0, 0x90/* Automatic Setting Time + Differential*/ + (chn-1));

						 ip_drv[id].stask_setup.isr_cnt++;	  /* next channel*/
					     break;
					case 6:
				         /* read status register */
						 chn = 6;
				         tmpl = api_io_ana_in_read_ip850(id,0);
						 if ((tmpl & 0xC000) == 0){
				           ip_drv[id].in_data[chn-1] = api_io_ana_in_read_ip850(id,chn);
						 }
						 ip_drv[id].stask_setup.isr_cnt++;	  /* next channel*/
					     /* Channel7*/
					     /* write to ctrl register */
						 chn = 7;
				         api_io_ana_out_write_ip850(id, 0, 0x90/* Automatic Setting Time + Differential*/ + (chn-1));

						 ip_drv[id].stask_setup.isr_cnt++;	  /* next channel*/
					     break;
					case 7:
				         /* read status register */
						 chn = 7;
				         tmpl = api_io_ana_in_read_ip850(id,0);
						 if ((tmpl & 0xC000) == 0){
				           ip_drv[id].in_data[chn-1] = api_io_ana_in_read_ip850(id,chn);
						 }
						 ip_drv[id].stask_setup.isr_cnt++;	  /* next channel*/
					     /* Channel8*/
					     /* write to ctrl register */
						 chn = 8;
				         api_io_ana_out_write_ip850(id, 0, 0x90/* Automatic Setting Time + Differential*/ + (chn-1));

						 ip_drv[id].stask_setup.isr_cnt++;	  /* next channel*/
					     break;
					case 8:
				         /* read status register */
						 chn = 8;
				         tmpl = api_io_ana_in_read_ip850(id,0);
						 if ((tmpl & 0xC000) == 0){
				           ip_drv[id].in_data[chn-1] = api_io_ana_in_read_ip850(id,chn);
						 }
						 ip_drv[id].stask_setup.isr_cnt++;	  /* next channel*/
					default:
			             if (++ip_drv[id].stask_setup.isr_cnt >= ip_drv[id].stask_setup.rate) {
			                ip_drv[id].stask_setup.isr_cnt = 0L;
						 }
						 else{
						    ip_drv[id].stask_setup.isr_cnt++;	  /* next */
						 }
					     break;
					}/* end switch */ 
		            
					/* Analog Output */
				    chn = 1;
			        if(ip_drv[id].out_data[chn-1] != ip_drv[id].act_data[chn-1]){
					    tmpl = ip_drv[id].out_data[chn-1];
					    api_io_ana_out_write_ip850(id, chn, tmpl);
				        /* write to DAC Load Register */
			            api_io_ana_out_write_ip850(id, 0xFF, 0xFF);
			            ip_drv[id].act_data[chn-1] = ip_drv[id].out_data[chn-1];
					}
					chn = 2;
			        if(ip_drv[id].out_data[chn-1] != ip_drv[id].act_data[chn-1]){
					    tmpl = ip_drv[id].out_data[chn-1];
					    api_io_ana_out_write_ip850(id, chn, tmpl);
				        /* write to DAC Load Register */
			            api_io_ana_out_write_ip850(id, 0xFF, 0xFF);
			            ip_drv[id].act_data[chn-1] = ip_drv[id].out_data[chn-1];
					}
				    chn = 3;
			        if(ip_drv[id].out_data[chn-1] != ip_drv[id].act_data[chn-1]){
					    tmpl = ip_drv[id].out_data[chn-1];
					    api_io_ana_out_write_ip850(id, chn, tmpl);
				        /* write to DAC Load Register */
			            api_io_ana_out_write_ip850(id, 0xFF, 0xFF);
			            ip_drv[id].act_data[chn-1] = ip_drv[id].out_data[chn-1];
					}
					chn = 4;
			        if(ip_drv[id].out_data[chn-1] != ip_drv[id].act_data[chn-1]){
					    tmpl = ip_drv[id].out_data[chn-1];
					    api_io_ana_out_write_ip850(id, chn, tmpl);
				        /* write to DAC Load Register */
			            api_io_ana_out_write_ip850(id, 0xFF, 0xFF);
			            ip_drv[id].act_data[chn-1] = ip_drv[id].out_data[chn-1];
					}
			   }
		   } /* end: Analog */
	   }/* end: MILScope */
   } /* end: for */
   /******************************************/
   /***   SAMPLING TASK FOR I/O INTERFACES ***/
   /******************************************/
   for (id=0; id<MAX_IP_DRV; id++) {
	   /* perform Recording Sampling */
	   if ((ip_drv[id].used != 0) && (ip_drv[id].p_din_read_func != NULL)) {
	     if ((ip_drv[id].stask_setup.ena & 0x01L) == API_ON) {

	         /******************************************/
	         /*** RECORDING                          ***/
	         /******************************************/
	         if (ip_drv[id].rec_status.rec_con == API_ON) {
	            if ((ip_drv[id].ip_type == API_DIG_IP_TYPE)  ||
	                (ip_drv[id].ip_type == API_APX_IP_TYPE)) {
			       api_ir_discr_rec(p_api_dev,id);
				}
	            else if (ip_drv[id].ip_type == API_ANA_IP_TYPE) {
			       api_ir_ana_rec(p_api_dev,id);
				}
	         }   
	     }     
	   } 
    } /* end: for */
#endif /* _CMPL4DTS */
  
 #endif /* IRIG */
 
} /* end: api_rti1 */

#endif /* _NUCLEUS */
#endif /* _AMC1553 */

/*****************************************************************************/
/*                                                                           */
/*   Module : API_IR                   Submodule : API_RTI1_ISR              */
/*                                                                           */
/*   Author : Thomas Jahn              Project   : API-/AVI-/AXI-1553        */
/*                                                                           */
/*   Source : C                        Tools     : PC/AT;                    */
/*                                                 CYGNUS, GNU-C, As, and LD */
/*                                                 IDT-C 5.1 Toolkit         */
/*---------------------------------------------------------------------------*/
/*   Create : 17.12.2002   Update :                                          */
/*---------------------------------------------------------------------------*/
/*    Descriptions                                                           */
/*    ------------                                                           */
/*    Inputs    : none                                                       */
/*                                                                           */
/*    Outputs   : none                                                       */
/*                                                                           */
/*    Description :                                                          */
/*                                                                           */
/*****************************************************************************/

#ifndef _NUCLEUS
  #ifndef _AMC1553

void api_rti1_isr(L_WORD cpu_cause, L_WORD cpu_stat, EXFRAME_TY *exc)
{

  api_rti1(px_api_dev, API_ISR_RTI_SEC, API_ISR_USEC_STEP);

}

  #endif /* !_AMC1553 */
#else /* _NUCLEUS */

void API_RTI1_Interrupt(INT vector_number)
{

  api_rti1(px_api_dev, API_ISR_RTI_SEC, API_ISR_USEC_STEP );

}

#endif /* _NUCLEUS */

#if defined(_NUCLEUS)
/*****************************************************************************/
/*                                                                           */
/*   Module : API_IR                   Submodule : API_DMA0                  */
/*                                                                           */
/*   Author : Thomas Jahn              Project   : API-/AVI-/AXI-1553        */
/*                                                                           */
/*   Source : C                        Tools     : PC/AT;                    */
/*                                                 CYGNUS, GNU-C, As, and LD */
/*                                                 IDT-C 5.1 Toolkit         */
/*---------------------------------------------------------------------------*/
/*   Create : 19.02.2003   Update :                                          */
/*---------------------------------------------------------------------------*/
/*    Descriptions                                                           */
/*    ------------                                                           */
/*    Inputs    : none                                                       */
/*                                                                           */
/*    Outputs   : none                                                       */
/*                                                                           */
/*    Description :                                                          */
/*                                                                           */
/*****************************************************************************/

void api_dma0(TY_API_DEV *p_api_dev)
{

  #ifdef _INTERRUPT_TEST_
  printf("\n\rGtDMA0<");
  #endif 

  #ifndef _NUCLEUS
  api_dma_confirm_interrupt(p_api_dev, 0);
  #else
  NU_Activate_HISR(&hisr_DmaComplete);
  #endif 

}

  #ifndef _NUCLEUS
void api_dma0_isr(L_WORD cpu_cause, L_WORD cpu_stat, EXFRAME_TY *exc)
{
  api_dma0(px_api_dev);
}

  #else 
void API_DMA0_Interrupt(INT vector_number)
{
  api_dma0(px_api_dev);
}

  #endif 
#endif /* defined(_NUCLEUS) */


NAMESPACE1553_END

/* end of file */

