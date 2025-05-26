/* SPDX-FileCopyrightText: 2003-2022 AIM GmbH <info@aim-online.com> */
/* SPDX-License-Identifier: MIT OR GPL-2.0-or-later */

/*! \file api_dma.c
    These funcitons are to initialize and start DMA
*/

#include "mil_tsw_includes.h"
#include "api_dma.h"

/******************************************************************************
 *  Defines
 ******************************************************************************/

/******************************************************************************
 *  local Vars (Modul)	  
 ******************************************************************************/

/******************************************************************************
 *  extern Vars
 ******************************************************************************/

/******************************************************************************
 *  local prototypes
 ******************************************************************************/
extern BYTE api_dma_setup(TY_API_DEV *p_api_dev, BYTE uc_DmaNo, ty_api_dma_setup *px_GtDmaSetup);
extern BYTE api_dma_run(TY_API_DEV *p_api_dev, BYTE uc_DmaNo);

/*
##############################################################################
#  Start of Code
##############################################################################
 */

/* --------------------------------------------------------------------------- 
Global Functions
------------------------------------------------------------------------- */


/**********************************************************************
 * @Function: api_dma_SignalDmaCompleted
 * @Descr:    Signals the polling host that DMA transfer is complete. Called in
 *            interrupt handler's context.
 * @Author:   Markus Pietrek
 ***********************************************************************/

void api_dma_SignalDmaCompleted(TY_API_DEV *p_api_dev)
{
  L_WORD tmpl;

  /* DMA Communication Control Register */
  /* Clear Execution Bit */
  /* Target Communication Status Register */
  /* Set Completed Bit, clear Execution Bit */
  tmpl = 0L;
  tmpl |= API_DMA_COM_STATUS_CPL_BIT_SET;
  tmpl &= API_DMA_COM_STATUS_EXE_BIT_RES;
  p_api_dev->targ_com_reg_p->ul_DmaStatus = tmpl;
  wbflush();

  api_write_int_loli_gen_int(p_api_dev, GT_ICAUSE_DMA0_INDEX, 0, 0, 0, 0);
}

/****************************************************************************
Function    : api_dma_Host2TargetCom
-----------------------------------------------------------------------------
Create      : 26.02.2003   Update  : 
-----------------------------------------------------------------------------

Inputs      : -

Outputs     : -

Description : 

 *****************************************************************************/
void api_dma_Host2TargetCom(TY_API_DEV *p_api_dev)
{
  BYTE uc_retVal = 0;
  L_WORD tmpl, tm;
  static INT8 flag = 0;

  if(flag == 0)
  {
    flag = 1;

    /* Check DMA Control Register if Host ready */
    tmpl = p_api_dev->targ_com_reg_p->ul_DmaControl;
    wbflush();
    tmpl &= API_DMA_COM_CONTROL_EXE_BIT_SET;

    if(tmpl == API_DMA_COM_CONTROL_EXE_BIT_SET)
    {
      set_time_mark( (UINT32*) &tm);

      //printf("G");
      /* DMA Communication Status Register */
      /* Clear Completed Bit, set Execution Bit */
      tmpl = 0L;
      tmpl |= API_DMA_COM_STATUS_EXE_BIT_SET;
      tmpl &= API_DMA_COM_STATUS_CPL_BIT_RES;
      p_api_dev->targ_com_reg_p->ul_DmaStatus = tmpl;
      wbflush();

      tmpl = p_api_dev->targ_com_reg_p->ul_DmaStatus;
      wbflush();
      if( (tmpl &API_DMA_COM_STATUS_INI_BIT_SET) != API_DMA_COM_STATUS_INI_BIT_SET)
      {
        api_dma_init(p_api_dev);
        tmpl |= API_DMA_COM_STATUS_INI_BIT_SET;
        p_api_dev->targ_com_reg_p->ul_DmaStatus = tmpl;
        wbflush();
      }

      /* DMA Communication Control Register */
      /* Clear Execution Bit */
      tmpl = p_api_dev->targ_com_reg_p->ul_DmaControl;
      wbflush();
      tmpl &= API_DMA_COM_CONTROL_EXE_BIT_RES;
      p_api_dev->targ_com_reg_p->ul_DmaControl = tmpl;
      wbflush();

      uc_retVal = api_dma_run(p_api_dev, 0 /* DMANo*/);
      if(uc_retVal != 0)
      {
        PRINTF1("ERROR: api_dma_run %X\n\r", uc_retVal);
      }


      //     enable_int((BIU1_INT | BIU2_INT));
      p_api_dev->targ_com_reg_p->ul_Time = (time_since_mark( (UINT32*) &tm) / 75);
      wbflush();
      //printf("1.src_addr %X, dest_addr %X, size %X mem_type %d\n\r",src_addr,dest_addr,size,mem_type);
    }
    flag = 0;
  }

}
