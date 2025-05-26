/* SPDX-FileCopyrightText: 2001-2022 AIM GmbH <info@aim-online.com> */
/* SPDX-License-Identifier: MIT OR GPL-2.0-or-later */

/*! \file api_ini.c
    Global variables of the 1553 ASP Driver Software. 
*/

#include "mil_tsw_includes.h"

/* global data structure of the API device */
#if defined(_NUCLEUS)
TY_API_DEV *px_api_dev;
#endif 


/***********************************************/
/*  Structs for ASP Realtime Driver extension  */
/***********************************************/
#ifdef _CMPL4DTS

   TY_API_ASP_DRV ip_drv[MAX_IP_DRV] =  {
     /********************/
     /*** Slot 0       ***/  
     /********************/
        {
          0L,                     /*  used */
          0,                      /*  ip_type           */
          0,                      /*  p_dhw_init_func   */
          0,                      /*  p_dout_write_func */
          0,                      /*  p_dout_read_func  */
          0,                      /*  p_din_read_func   */
          {{ 0L,0L,0L,0L },       /*  ip_setup.din: model,ip_addr,sh_addr,data */
           { 0L,0L,0L,0L }},      /*  ip_setup.dout: model,ip_addr,sh_addr,data */
           { 0L,0L,0L,0L,0L },    /*  stask_setup: ena,isr_cnt,rate,res,ustep */
           { 0L,0L,0L,0L,0L,0L }, /*  rec_status: rec_con,rec_status,status,wr_addr,rd_addr,entry_cnt */
           { 0L,0L,0L,0L,0L,0L,0L,0L,0L,0L,0L,0L,0L,0L,0L,0L },  /*  in_data[16] */
           { 0L,0L,0L,0L,0L,0L,0L,0L,0L,0L,0L,0L,0L,0L,0L,0L },  /*  act_data[16] */
           { 0L,0L,0L,0L,0L,0L,0L,0L,0L,0L,0L,0L,0L,0L,0L,0L },  /*  out_data[16] */
           { 0, 0, 0, 0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0  }   /*  rec_data[16] */
        },                                                           

     /********************/
     /*** Slot 1       ***/  
     /********************/
        {
          0L,                     /*  used */
          0,                      /*  ip_type           */
          0,                      /*  p_dhw_init_func   */
          0,                      /*  p_dout_write_func */
          0,                      /*  p_dout_read_func  */
          0,                      /*  p_din_read_func   */
          {{ 0L,0L,0L,0L },       /*  ip_setup.din: model,ip_addr,sh_addr,data */
           { 0L,0L,0L,0L }},      /*  ip_setup.dout: model,ip_addr,sh_addr,data */
           { 0L,0L,0L,0L,0L },    /*  stask_setup: ena,isr_cnt,rate,res,ustep */
           { 0L,0L,0L,0L,0L,0L }, /*  rec_status: rec_con,rec_status,status,wr_addr,rd_addr,entry_cnt */
           { 0L,0L,0L,0L,0L,0L,0L,0L,0L,0L,0L,0L,0L,0L,0L,0L },  /*  in_data[16] */
           { 0L,0L,0L,0L,0L,0L,0L,0L,0L,0L,0L,0L,0L,0L,0L,0L },  /*  act_data[16] */
           { 0L,0L,0L,0L,0L,0L,0L,0L,0L,0L,0L,0L,0L,0L,0L,0L },  /*  out_data[16] */
           { 0, 0, 0, 0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0  }   /*  rec_data[16] */
        },
           
     /********************/
     /*** Slot 2       ***/  
     /********************/
        {
          0L,                     /*  used */
          0,                      /*  ip_type           */
          0,                      /*  p_dhw_init_func   */
          0,                      /*  p_dout_write_func */
          0,                      /*  p_dout_read_func  */
          0,                      /*  p_din_read_func   */
          {{ 0L,0L,0L,0L },       /*  ip_setup.din: model,ip_addr,sh_addr,data */
           { 0L,0L,0L,0L }},      /*  ip_setup.dout: model,ip_addr,sh_addr,data */
           { 0L,0L,0L,0L,0L },    /*  stask_setup: ena,isr_cnt,rate,res,ustep */
           { 0L,0L,0L,0L,0L,0L }, /*  rec_status: rec_con,rec_status,status,wr_addr,rd_addr,entry_cnt */
           { 0L,0L,0L,0L,0L,0L,0L,0L,0L,0L,0L,0L,0L,0L,0L,0L },  /*  in_data[16] */
           { 0L,0L,0L,0L,0L,0L,0L,0L,0L,0L,0L,0L,0L,0L,0L,0L },  /*  act_data[16] */
           { 0L,0L,0L,0L,0L,0L,0L,0L,0L,0L,0L,0L,0L,0L,0L,0L },  /*  out_data[16] */
           { 0, 0, 0, 0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0  }   /*  rec_data[16] */
        },
           
     /********************/
     /*** Slot 3       ***/  
     /********************/
        {
          0L,                     /*  used */
          0,                      /*  ip_type           */
          0,                      /*  p_dhw_init_func   */
          0,                      /*  p_dout_write_func */
          0,                      /*  p_dout_read_func  */
          0,                      /*  p_din_read_func   */
          {{ 0L,0L,0L,0L },       /*  ip_setup.din: model,ip_addr,sh_addr,data */
           { 0L,0L,0L,0L }},      /*  ip_setup.dout: model,ip_addr,sh_addr,data */
           { 0L,0L,0L,0L,0L },    /*  stask_setup: ena,isr_cnt,rate,res,ustep */
           { 0L,0L,0L,0L,0L,0L }, /*  rec_status: rec_con,rec_status,status,wr_addr,rd_addr,entry_cnt */
           { 0L,0L,0L,0L,0L,0L,0L,0L,0L,0L,0L,0L,0L,0L,0L,0L },  /*  in_data[16] */
           { 0L,0L,0L,0L,0L,0L,0L,0L,0L,0L,0L,0L,0L,0L,0L,0L },  /*  act_data[16] */
           { 0L,0L,0L,0L,0L,0L,0L,0L,0L,0L,0L,0L,0L,0L,0L,0L },  /*  out_data[16] */
           { 0, 0, 0, 0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0  }   /*  rec_data[16] */
        }
   };
#endif

 /* end: api_glob */
