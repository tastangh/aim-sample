/* SPDX-FileCopyrightText: 2008-2022 AIM GmbH <info@aim-online.com> */
/* SPDX-License-Identifier: MIT OR GPL-2.0-or-later */

/*! \file api_dts.c
    DTS system specific functions
*/

#include "mil_tsw_includes.h"



TY_MIL_COM_TABLE_ENTRY axTargetCustomDtsCommands[] = {
  {MIL_COM_CUSTOM_DTS_GEN_IO_TEST_SEQ,     (TY_MIL_COM_FUNC_PTR)api_gen_io_test_seq,       "api_gen_io_test_seq",       sizeof(TY_MIL_COM_CUSTOM_DTS_GEN_IO_TEST_SEQ_INPUT),       sizeof(TY_MIL_COM_CUSTOM_DTS_GEN_IO_TEST_SEQ_OUTPUT)      },
};


void api_custom_dts_command_table( TY_MIL_COM_TABLE_ENTRY ** table, AiUInt32 * size )
{
    *size  = sizeof(axTargetCustomDtsCommands) / sizeof(TY_MIL_COM_TABLE_ENTRY);
    *table = axTargetCustomDtsCommands;
}





#if !defined(_CMPL4DTS)
L_WORD api_gen_io_test_seq(TY_API_DEV *p_api_dev, TY_MIL_COM_CUSTOM_DTS_GEN_IO_TEST_SEQ_INPUT * pxInput, TY_MIL_COM_CUSTOM_DTS_GEN_IO_TEST_SEQ_OUTPUT * pxOutput ) { return API_ERR_PLATTFORM_NOT_SUPPORTED; }
#endif




#if defined(_CMPL4DTS)


/* Used for 1760 DTS System */
 extern BYTE api_dma_chn1_setup_start( TY_API_DEV *p_api_dev, BYTE uc_DmaDir, ty_api_dma_setup *px_DmaSetup );





BYTE api_io_gen_io_read(TY_API_DEV *p_api_dev, L_WORD addr,L_WORD res)
{
volatile L_WORD *ptr;
ty_api_dma_setup x_DmaSetup;
L_WORD lval,sh_addr,tmpl;

     lval = 0L;
     if (addr != 0L) {
		    sh_addr = (L_WORD)API_DATASET_MEM_ADDR_ABS(0/*dsid*/);
	        /* clear */
	        ptr = (L_WORD *)(sh_addr);
	        *ptr = 0L;

	        /* Setup & DMA Read */
	        x_DmaSetup.ul_Size        = 4;
	        x_DmaSetup.ul_Source      = sh_addr & 0x0FFFFFFFL;
	        x_DmaSetup.ul_Destination = addr;
	        wbflush();
	        x_DmaSetup.v_CallBack = NULL;
	        x_DmaSetup.uc_Type = API_DMA_DIRECTION_READ_SHARED;
	        api_dma_chn1_setup_start(px_api_dev, 1/*Read*/, &x_DmaSetup);
    
            /* Read value from Shared Memory */
            tmpl = *ptr;  
        
            lval  =  (tmpl >> 8) & 0x00FF00FFL;      
            lval |= ((tmpl << 8) & 0xFF00FF00L);      

	 }
     return(lval);

}

             
/*****************************************************************************/
/*                                                                           */
/*   Module : API_IO                   Submodule : API_IO_DIG_OUT_WRITE_IP408*/
/*                                                                           */
/*   Author : Marco Motz               Project   : API1553-DS                */
/*                                                                           */
/*   Source : C                        Tools     : PC/AT; Norton Editor;     */
/*                                                 CYGNUS, GNU-C, As, and LD */
/*                                                 IDT-C 5.1 Toolkit         */
/*---------------------------------------------------------------------------*/
/*   Create : 05.07.08   Update : 05.07.08                                   */
/*---------------------------------------------------------------------------*/
/*    Descriptions                                                           */
/*    ------------                                                           */
/*    Inputs    : none                                                       */
/*                                                                           */
/*    Outputs   :                                                            */
/*                                                                           */
/*    Description :                                                          */
/*                                                                           */
/*****************************************************************************/

void api_io_dig_out_write_ip408(L_WORD id, L_WORD val)
{
volatile L_WORD *ptr;
ty_api_dma_setup x_DmaSetup;
L_WORD tmpl;

     tmpl = 0L;
     
     if (ip_drv[id].ip_setup.dout.ip_addr != 0L) {
     
        tmpl  =  (val << 8) & 0xFF00FF00L;      
        tmpl |= ((val >> 8) & 0x00FF00FFL);      

        /* Write value to Shared Memory */
        ptr = (L_WORD *)(ip_drv[id].ip_setup.dout.sh_addr);
        *ptr = tmpl;        

        /* Setup & DMA Write */
        x_DmaSetup.ul_Size        = 4;
        x_DmaSetup.ul_Source      = ip_drv[id].ip_setup.dout.sh_addr & 0x0FFFFFFFL;
        x_DmaSetup.ul_Destination = ip_drv[id].ip_setup.dout.ip_addr;
        wbflush();

#ifdef DEBUG_IP
        PRINTF3("OUTPUT-WRITE-408: ul_Source=%08X ul_Destination=%08X Value=%08X\r\n", x_DmaSetup.ul_Source,x_DmaSetup.ul_Destination,val);
#endif

        x_DmaSetup.v_CallBack = NULL;
        x_DmaSetup.uc_Type = API_DMA_DIRECTION_READ_SHARED; 
        api_dma_chn1_setup_start(px_api_dev, 0/*Write*/, &x_DmaSetup);
     }

} /* end: api_io_dig_out_write_ip408 */    

/*****************************************************************************/
/*                                                                           */
/*   Module : API_IO                   Submodule : API_IO_DIG_OUT_READ_IP408 */
/*                                                                           */
/*   Author : Marco Motz               Project   : API1553-DS                */
/*                                                                           */
/*   Source : C                        Tools     : PC/AT; Norton Editor;     */
/*                                                 CYGNUS, GNU-C, As, and LD */
/*                                                 IDT-C 5.1 Toolkit         */
/*---------------------------------------------------------------------------*/
/*   Create : 05.07.08   Update : 05.07.08                                   */
/*---------------------------------------------------------------------------*/
/*    Descriptions                                                           */
/*    ------------                                                           */
/*    Inputs    : none                                                       */
/*                                                                           */
/*    Outputs   :                                                            */
/*                                                                           */
/*    Description :                                                          */
/*                                                                           */
/*****************************************************************************/

L_WORD api_io_dig_out_read_ip408(L_WORD id)
{
volatile L_WORD *ptr;
ty_api_dma_setup x_DmaSetup;
L_WORD lval,tmpl;

     lval = 0L;
     tmpl = 0L;
     
     if (ip_drv[id].ip_setup.dout.ip_addr != 0L) {
        
        /* clear */
        ptr = (L_WORD *)(ip_drv[id].ip_setup.dout.sh_addr + 4L);
        *ptr = 0L;

        /* Setup & DMA Read */
        x_DmaSetup.ul_Size        = 4;
        x_DmaSetup.ul_Source      =(ip_drv[id].ip_setup.dout.sh_addr + 4L) & 0x0FFFFFFFL;
        x_DmaSetup.ul_Destination = ip_drv[id].ip_setup.dout.ip_addr;
        wbflush();
        x_DmaSetup.v_CallBack = NULL;
        x_DmaSetup.uc_Type = API_DMA_DIRECTION_READ_SHARED; 
        api_dma_chn1_setup_start(px_api_dev, 1/*Read*/, &x_DmaSetup);

        /* Read value from Shared Memory */
        tmpl = *ptr;  
        
        lval  =  (tmpl >> 8) & 0x00FF00FFL;      
        lval |= ((tmpl << 8) & 0xFF00FF00L);      

#ifdef DEBUG_IP
        PRINTF3("OUTPUT-READ-408: ul_Source=%08X ul_Destination=%08X Value=%08X\r\n", x_DmaSetup.ul_Source,x_DmaSetup.ul_Destination,lval);
#endif
     }   

     return(lval);
    
} /* end: api_io_dig_out_read_ip408 */    

/*****************************************************************************/
/*                                                                           */
/*   Module : API_IO                   Submodule : API_IO_DIG_IN_READ_IP408  */
/*                                                                           */
/*   Author : Marco Motz               Project   : API1553-DS                */
/*                                                                           */
/*   Source : C                        Tools     : PC/AT; Norton Editor;     */
/*                                                 CYGNUS, GNU-C, As, and LD */
/*                                                 IDT-C 5.1 Toolkit         */
/*---------------------------------------------------------------------------*/
/*   Create : 05.07.08   Update : 05.07.08                                   */
/*---------------------------------------------------------------------------*/
/*    Descriptions                                                           */
/*    ------------                                                           */
/*    Inputs    : none                                                       */
/*                                                                           */
/*    Outputs   :                                                            */
/*                                                                           */
/*    Description :                                                          */
/*                                                                           */
/*****************************************************************************/

L_WORD api_io_dig_in_read_ip408(L_WORD id)
{
volatile L_WORD *ptr;
ty_api_dma_setup x_DmaSetup;
L_WORD lval,tmpl;

     lval = 0L;
     tmpl = 0L;
     
     if (ip_drv[id].ip_setup.din.ip_addr != 0L) {

        /* clear */
        ptr = (L_WORD *)(ip_drv[id].ip_setup.din.sh_addr);
        *ptr = 0L;

        /* Setup & DMA Read */
        x_DmaSetup.ul_Size        = 4;
        x_DmaSetup.ul_Source      = ip_drv[id].ip_setup.din.sh_addr & 0x0FFFFFFFL;
        x_DmaSetup.ul_Destination = ip_drv[id].ip_setup.din.ip_addr;
        wbflush();
        x_DmaSetup.v_CallBack = NULL;
        x_DmaSetup.uc_Type = API_DMA_DIRECTION_READ_SHARED; 
        api_dma_chn1_setup_start(px_api_dev, 1/*Read*/, &x_DmaSetup);

        /* Read value from Shared Memory */
        tmpl = *ptr;  
        
        lval  =  (tmpl >> 8) & 0x00FF00FFL;      
        lval |= ((tmpl << 8) & 0xFF00FF00L);      

#ifdef DEBUG_IP
        PRINTF3("INPUT-READ-408: ul_Source=%08X ul_Destination=%08X Value=%08X\r\n", x_DmaSetup.ul_Source,x_DmaSetup.ul_Destination,lval);
#endif
     }

     return(lval);
    
} /* end: api_io_dig_in_read_ip408 */    

/*****************************************************************************/
/*                                                                           */
/*   Module : API_IO                   Submodule : API_IO_DIG_OUT_WRITE_IP470*/
/*                                                                           */
/*   Author : Marco Motz               Project   : API1553-DS                */
/*                                                                           */
/*   Source : C                        Tools     : PC/AT; Norton Editor;     */
/*                                                 CYGNUS, GNU-C, As, and LD */
/*                                                 IDT-C 5.1 Toolkit         */
/*---------------------------------------------------------------------------*/
/*   Create : 05.07.08   Update : 05.07.08                                   */
/*---------------------------------------------------------------------------*/
/*    Descriptions                                                           */
/*    ------------                                                           */
/*    Inputs    : none                                                       */
/*                                                                           */
/*    Outputs   :                                                            */
/*                                                                           */
/*    Description :                                                          */
/*                                                                           */
/*****************************************************************************/

void api_io_dig_out_write_ip470(L_WORD id, L_WORD val, L_WORD offs)
{
L_WORD tmpl;

     tmpl = 0L;
     
     if (ip_drv[id].ip_setup.dout.ip_addr != 0L) {
     
        tmpl  =  (val <<  8) & 0x0000FF00L;  /* Port-4 */     
        tmpl |= ((val << 16) & 0xFF000000L); /* Port-5 */      

		mil_hw_pci_writelw( ip_drv[id].ip_setup.dout.ip_addr + offs, tmpl );

#ifdef DEBUG_IP
        PRINTF2("OUTPUT-WRITE-470: ul_Destination=%08X Value=%08X\r\n", ip_drv[id].ip_setup.dout.ip_addr + offs, val );
#endif
     }

} /* end: api_io_dig_out_write_ip470 */    

/*****************************************************************************/
/*                                                                           */
/*   Module : API_IO                   Submodule : API_IO_DIG_OUT_READ_IP470 */
/*                                                                           */
/*   Author : Marco Motz               Project   : API1553-DS                */
/*                                                                           */
/*   Source : C                        Tools     : PC/AT; Norton Editor;     */
/*                                                 CYGNUS, GNU-C, As, and LD */
/*                                                 IDT-C 5.1 Toolkit         */
/*---------------------------------------------------------------------------*/
/*   Create : 05.07.08   Update : 05.07.08                                   */
/*---------------------------------------------------------------------------*/
/*    Descriptions                                                           */
/*    ------------                                                           */
/*    Inputs    : none                                                       */
/*                                                                           */
/*    Outputs   :                                                            */
/*                                                                           */
/*    Description :                                                          */
/*                                                                           */
/*****************************************************************************/

L_WORD api_io_dig_out_read_ip470(L_WORD id)
{
L_WORD lval,tmpl;

     lval = 0L;
     tmpl = 0L;
     
     if (ip_drv[id].ip_setup.dout.ip_addr != 0L) {

		tmpl = mil_hw_pci_readlw( ip_drv[id].ip_setup.dout.ip_addr     );
		 
        lval  =  (tmpl >>  8) & 0x000000FFL;   /* Port-4 */   
        lval |= ((tmpl >> 16) & 0x0000FF00L);  /* Port-5 */    

#ifdef DEBUG_IP
        PRINTF2("OUTPUT-READ-470: ul_Source=%08X Value=%08X\r\n", ip_drv[id].ip_setup.dout.ip_addr,lval);
#endif
     }   

     return(lval);
    
} /* end: api_io_dig_out_read_ip470 */    

/*****************************************************************************/
/*                                                                           */
/*   Module : API_IO                   Submodule : API_IO_DIG_OUT_WRITE_IP470*/
/*                                                                           */
/*   Author : Marco Motz               Project   : API1553-DS                */
/*                                                                           */
/*   Source : C                        Tools     : PC/AT; Norton Editor;     */
/*                                                 CYGNUS, GNU-C, As, and LD */
/*                                                 IDT-C 5.1 Toolkit         */
/*---------------------------------------------------------------------------*/
/*   Create : 05.07.08   Update : 05.07.08                                   */
/*---------------------------------------------------------------------------*/
/*    Descriptions                                                           */
/*    ------------                                                           */
/*    Inputs    : none                                                       */
/*                                                                           */
/*    Outputs   :                                                            */
/*                                                                           */
/*    Description :                                                          */
/*                                                                           */
/*****************************************************************************/

void api_io_dig_out_write_ip470_0123(L_WORD id, L_WORD val)
{
volatile L_WORD *ptr;
ty_api_dma_setup x_DmaSetup;
L_WORD tmpl1,tmpl2;

     tmpl1 = 0L;
     tmpl2 = 0L;
     
     if (ip_drv[id].ip_setup.dout.ip_addr != 0L) {
     
        tmpl1  =  (val <<  8) & 0x0000FF00L;  /* Port-0 */     
        tmpl1 |= ((val << 16) & 0xFF000000L); /* Port-1 */      
        tmpl2  =  (val >>  8) & 0x0000FF00L;  /* Port-2 */     
        tmpl2 |=  (val        & 0xFF000000L); /* Port-3 */      

        /* Write value to Shared Memory */
        ptr = (L_WORD *)(ip_drv[id].ip_setup.dout.sh_addr);
        *(ptr+0) = tmpl1;
        *(ptr+1) = tmpl2;

        /* Setup & DMA Write */
        x_DmaSetup.ul_Size        = 8;
        x_DmaSetup.ul_Source      = ip_drv[id].ip_setup.dout.sh_addr & 0x0FFFFFFFL;
        x_DmaSetup.ul_Destination = ip_drv[id].ip_setup.dout.ip_addr;
        wbflush();

#ifdef DEBUG_IP
        PRINTF3("OUTPUT-WRITE-470: ul_Source=%08X ul_Destination=%08X Value=%08X\r\n", x_DmaSetup.ul_Source,x_DmaSetup.ul_Destination,val);
#endif

        x_DmaSetup.v_CallBack = NULL;
        x_DmaSetup.uc_Type = API_DMA_DIRECTION_READ_SHARED; 
        api_dma_chn1_setup_start(px_api_dev, 0/*Write*/, &x_DmaSetup);
     }

} /* end: api_io_dig_out_write_ip470_0123 */    

/*****************************************************************************/
/*                                                                           */
/*   Module : API_IO                   Submodule : API_IO_DIG_IN_READ_IP470  */
/*                                                                           */
/*   Author : Marco Motz               Project   : API1553-DS                */
/*                                                                           */
/*   Source : C                        Tools     : PC/AT; Norton Editor;     */
/*                                                 CYGNUS, GNU-C, As, and LD */
/*                                                 IDT-C 5.1 Toolkit         */
/*---------------------------------------------------------------------------*/
/*   Create : 05.07.08   Update : 05.07.08                                   */
/*---------------------------------------------------------------------------*/
/*    Descriptions                                                           */
/*    ------------                                                           */
/*    Inputs    : none                                                       */
/*                                                                           */
/*    Outputs   :                                                            */
/*                                                                           */
/*    Description :                                                          */
/*                                                                           */
/*****************************************************************************/

L_WORD api_io_dig_out_read_ip470_0123(L_WORD id)
{
volatile L_WORD *ptr;
ty_api_dma_setup x_DmaSetup;
L_WORD lval,tmpl1,tmpl2;

     lval = 0L;
     tmpl1 = 0L;
     tmpl2 = 0L;
     
     if (ip_drv[id].ip_setup.dout.ip_addr != 0L) {
        
        /* clear */
        ptr = (L_WORD *)(ip_drv[id].ip_setup.dout.sh_addr + 8L);
        *(ptr+0) = 0L;
        *(ptr+1) = 0L;

        /* Setup & DMA Read */
        x_DmaSetup.ul_Size        = 8;
        x_DmaSetup.ul_Source      =(ip_drv[id].ip_setup.dout.sh_addr + 8L) & 0x0FFFFFFFL;
        x_DmaSetup.ul_Destination = ip_drv[id].ip_setup.dout.ip_addr;
        wbflush();
        x_DmaSetup.v_CallBack = NULL;
        x_DmaSetup.uc_Type = API_DMA_DIRECTION_READ_SHARED; 
        api_dma_chn1_setup_start(px_api_dev, 1/*Read*/, &x_DmaSetup);

        /* Read value from Shared Memory */
        tmpl1 = *(ptr+0); 
        tmpl2 = *(ptr+1);
        
        lval  =  (tmpl1 >>  8) & 0x000000FFL;   /* Port-0 */
        lval |= ((tmpl1 >> 16) & 0x0000FF00L);  /* Port-1 */
        lval |= ((tmpl2 <<  8) & 0x00FF0000L);  /* Port-2 */
        lval |=  (tmpl2        & 0xFF000000L);  /* Port-3 */

#ifdef DEBUG_IP
        PRINTF3("OUTPUT-READ-470: ul_Source=%08X ul_Destination=%08X Value=%08X\r\n", x_DmaSetup.ul_Source,x_DmaSetup.ul_Destination,lval);
#endif
     }   

     return(lval);
    
} /* end: api_io_dig_out_read_ip470_0123 */    

/*****************************************************************************/
/*                                                                           */
/*   Module : API_IO                   Submodule : API_IO_DIG_IN_READ_IP470  */
/*                                                                           */
/*   Author : Marco Motz               Project   : API1553-DS                */
/*                                                                           */
/*   Source : C                        Tools     : PC/AT; Norton Editor;     */
/*                                                 CYGNUS, GNU-C, As, and LD */
/*                                                 IDT-C 5.1 Toolkit         */
/*---------------------------------------------------------------------------*/
/*   Create : 05.07.08   Update : 05.07.08                                   */
/*---------------------------------------------------------------------------*/
/*    Descriptions                                                           */
/*    ------------                                                           */
/*    Inputs    : none                                                       */
/*                                                                           */
/*    Outputs   :                                                            */
/*                                                                           */
/*    Description :                                                          */
/*                                                                           */
/*****************************************************************************/

L_WORD api_io_dig_in_read_ip470(L_WORD id)
{
	L_WORD lval,tmpl1,tmpl2;

     lval  = 0L;
     tmpl1 = 0L;
     tmpl2 = 0L;
     
     if (ip_drv[id].ip_setup.din.ip_addr != 0L) {

		tmpl1 = mil_hw_pci_readlw( ip_drv[id].ip_setup.din.ip_addr     );
		tmpl2 = mil_hw_pci_readlw( ip_drv[id].ip_setup.din.ip_addr + 4 );

        lval  =  (tmpl1 >>  8) & 0x000000FFL;   /* Port-0 */
        lval |= ((tmpl1 >> 16) & 0x0000FF00L);  /* Port-1 */
        lval |= ((tmpl2 <<  8) & 0x00FF0000L);  /* Port-2 */
        lval |=  (tmpl2        & 0xFF000000L);  /* Port-3 */

#ifdef DEBUG_IP
//        PRINTF2("INPUT-READ-470: ul_Source=%08X Value=%08X\r\n", ip_drv[id].ip_setup.din.ip_addr, lval);
#endif
     }

     return(lval);
    
} /* end: api_io_dig_in_read_ip470 */    

/*****************************************************************************/
/*                                                                           */
/*   Module : API_IO                   Submodule : API_IO_ANA_OUT_WRITE_IP850*/
/*                                                                           */
/*   Author : Marco Motz               Project   : API1553-DS                */
/*            Thomas Jahn                                                    */
/*   Source : C                        Tools     : PC/AT; Norton Editor;     */
/*                                                 CYGNUS, GNU-C, As, and LD */
/*                                                 IDT-C 5.1 Toolkit         */
/*---------------------------------------------------------------------------*/
/*   Create : 20.10.10   Update : 20.10.10                                   */
/*---------------------------------------------------------------------------*/
/*    Descriptions                                                           */
/*    ------------                                                           */
/*    Inputs    : none                                                       */
/*                                                                           */
/*    Outputs   :                                                            */
/*                                                                           */
/*    Description :                                                          */
/*                                                                           */
/*****************************************************************************/

void api_io_ana_out_write_ip850(L_WORD id,L_WORD chn, L_WORD val)
{
volatile L_WORD *ptr;
ty_api_dma_setup x_DmaSetup;
L_WORD tmpl;

     tmpl = 0L;
     
     if (ip_drv[id].ip_setup.dout.ip_addr != 0L) {
     
        tmpl  =  (val << 8) & 0xFF00FF00L;      
        tmpl |= ((val >> 8) & 0x00FF00FFL);      

        /* Write value to Shared Memory */
        ptr = (L_WORD *)(ip_drv[id].ip_setup.dout.sh_addr);
        *ptr = tmpl;        
        x_DmaSetup.ul_Source      = (ip_drv[id].ip_setup.dout.sh_addr) & 0x0FFFFFFFL;

        /* Setup & DMA Write */
        x_DmaSetup.ul_Size        = 2;
        
        switch (chn){
		   case 0:    /* Ctrl */ 
              x_DmaSetup.ul_Destination = ip_drv[id].ip_setup.dout.ip_addr;
		      break;
		   case 0xFF: /* DAC Load Register */
              x_DmaSetup.ul_Destination = ip_drv[id].ip_setup.dout.ip_addr + 0xE;
			  break;
		   default:	  /* DAC Data Register */
              x_DmaSetup.ul_Destination = ip_drv[id].ip_setup.dout.ip_addr + (6-2) + chn * 2;
			  break;
		};
        wbflush();

#ifdef DEBUG_IP
        PRINTF3("OUTPUT-WRITE-850: ul_Source=%08X ul_Destination=%08X Value=%08X\r\n", x_DmaSetup.ul_Source,x_DmaSetup.ul_Destination,val);
#endif

        x_DmaSetup.v_CallBack = NULL;
        x_DmaSetup.uc_Type = API_DMA_DIRECTION_READ_SHARED; 
        api_dma_chn1_setup_start(px_api_dev, 0/*Write*/, &x_DmaSetup);
     }

} /* end: api_io_ana_out_write_ip850 */    

/*****************************************************************************/
/*                                                                           */
/*   Module : API_IO                   Submodule : API_IO_ANA_OUT_READ_IP850 */
/*                                                                           */
/*   Author : Marco Motz               Project   : API1553-DS                */
/*            Thomas Jahn                                                    */
/*   Source : C                        Tools     : PC/AT; Norton Editor;     */
/*                                                 CYGNUS, GNU-C, As, and LD */
/*                                                 IDT-C 5.1 Toolkit         */
/*---------------------------------------------------------------------------*/
/*   Create : 20.10.10   Update : 20.10.10                                   */
/*---------------------------------------------------------------------------*/
/*    Descriptions                                                           */
/*    ------------                                                           */
/*    Inputs    : none                                                       */
/*                                                                           */
/*    Outputs   :                                                            */
/*                                                                           */
/*    Description :                                                          */
/*                                                                           */
/*****************************************************************************/

L_WORD api_io_ana_out_read_ip850(L_WORD id)
{
volatile L_WORD *ptr;
ty_api_dma_setup x_DmaSetup;
L_WORD lval,tmpl;

     lval = 0L;
     tmpl = 0L;
     
     if (ip_drv[id].ip_setup.dout.ip_addr != 0L) {
        
        /* clear */
        ptr = (L_WORD *)(ip_drv[id].ip_setup.dout.sh_addr + 4L);
        *ptr = 0L;

        /* Setup & DMA Read */
        x_DmaSetup.ul_Size        = 4;
        x_DmaSetup.ul_Source      =(ip_drv[id].ip_setup.dout.sh_addr + 4L) & 0x0FFFFFFFL;
        x_DmaSetup.ul_Destination = ip_drv[id].ip_setup.dout.ip_addr;
        wbflush();
        x_DmaSetup.v_CallBack = NULL;
        x_DmaSetup.uc_Type = API_DMA_DIRECTION_READ_SHARED; 
        api_dma_chn1_setup_start(px_api_dev, 1/*Read*/, &x_DmaSetup);

        /* Read value from Shared Memory */
        tmpl = *ptr;  
        
        lval  =  (tmpl >> 8) & 0x00FF00FFL;      
        lval |= ((tmpl << 8) & 0xFF00FF00L);      

#ifdef DEBUG_IP
        PRINTF3("OUTPUT-READ-850: ul_Source=%08X ul_Destination=%08X Value=%08X\r\n", x_DmaSetup.ul_Source,x_DmaSetup.ul_Destination,lval);
#endif
     }   

     return(lval);
    
} /* end: api_io_ana_out_read_ip850 */    

/*****************************************************************************/
/*                                                                           */
/*   Module : API_IO                   Submodule : API_IO_ANA_IN_READ_IP850  */
/*                                                                           */
/*   Author : Marco Motz               Project   : API1553-DS                */
/*            Thomas Jahn                                                    */
/*   Source : C                        Tools     : PC/AT; Norton Editor;     */
/*                                                 CYGNUS, GNU-C, As, and LD */
/*                                                 IDT-C 5.1 Toolkit         */
/*---------------------------------------------------------------------------*/
/*   Create : 20.10.10   Update : 20.10.10                                   */
/*---------------------------------------------------------------------------*/
/*    Descriptions                                                           */
/*    ------------                                                           */
/*    Inputs    : none                                                       */
/*                                                                           */
/*    Outputs   :                                                            */
/*                                                                           */
/*    Description :                                                          */
/*                                                                           */
/*****************************************************************************/

L_WORD api_io_ana_in_read_ip850(L_WORD id,L_WORD chn)
{
volatile L_WORD *ptr;
ty_api_dma_setup x_DmaSetup;
L_WORD lval,tmpl;

     lval = 0L;
     tmpl = 0L;
     if (ip_drv[id].ip_setup.din.ip_addr != 0L) {

        /* clear */
        ptr = (L_WORD *)(ip_drv[id].ip_setup.din.sh_addr);
        *ptr = 0L;

        /* Setup & DMA Read */
        x_DmaSetup.ul_Size        = 4;
	    x_DmaSetup.ul_Source      = (ip_drv[id].ip_setup.din.sh_addr) & 0x0FFFFFFFL;

        if(chn == 0) /* Status */
            x_DmaSetup.ul_Destination = ip_drv[id].ip_setup.din.ip_addr;
		else
            x_DmaSetup.ul_Destination = ip_drv[id].ip_setup.din.ip_addr + 4;
        wbflush();
        x_DmaSetup.v_CallBack = NULL;
        x_DmaSetup.uc_Type = API_DMA_DIRECTION_READ_SHARED; 
        api_dma_chn1_setup_start(px_api_dev, 1/*Read*/, &x_DmaSetup);

        /* Read value from Shared Memory */
        tmpl = *ptr;  
        
        lval  =  (tmpl >> 8) & 0x00FF00FFL;      
        lval |= ((tmpl << 8) & 0xFF00FF00L);      

#ifdef DEBUG_IP
        PRINTF3("INPUT-READ-850: ul_Source=%08X ul_Destination=%08X Value=%08X\r\n", x_DmaSetup.ul_Source,x_DmaSetup.ul_Destination,lval);
#endif
     }

     return(lval);
    
} /* end: api_io_ana_in_read_ip850 */    
/*****************************************************************************/
/*                                                                           */
/*   Module : API_IO                   Submodule : API_IO_DIG_OUT_WRITE_APX  */
/*                                                                           */
/*   Author : Marco Motz               Project   : API1553-DS                */
/*                                                                           */
/*   Source : C                        Tools     : PC/AT; Norton Editor;     */
/*                                                 CYGNUS, GNU-C, As, and LD */
/*                                                 IDT-C 5.1 Toolkit         */
/*---------------------------------------------------------------------------*/
/*   Create : 05.07.08   Update : 05.07.08                                   */
/*---------------------------------------------------------------------------*/
/*    Descriptions                                                           */
/*    ------------                                                           */
/*    Inputs    : none                                                       */
/*                                                                           */
/*    Outputs   :                                                            */
/*                                                                           */
/*    Description :                                                          */
/*                                                                           */
/*****************************************************************************/

void api_io_dig_out_write_apx(L_WORD id, L_WORD val)
{
L_WORD tmpl;

     tmpl = 0L;
     
#if defined(_NUCLEUS)
     tmpl = BSWAP32(p_ASL_Lca_IoRegs->ul_GPIOCtrlStatReg);
     tmpl &= 0xFF00FFFFL;
     tmpl |= ((val & 0xFFL) << 16); 
     p_ASL_Lca_IoRegs->ul_GPIOCtrlStatReg = BSWAP32(tmpl);
#endif

} /* end: api_io_dig_out_write_apx */    

/*****************************************************************************/
/*                                                                           */
/*   Module : API_IO                   Submodule : API_IO_DIG_OUT_READ_APX   */
/*                                                                           */
/*   Author : Marco Motz               Project   : API1553-DS                */
/*                                                                           */
/*   Source : C                        Tools     : PC/AT; Norton Editor;     */
/*                                                 CYGNUS, GNU-C, As, and LD */
/*                                                 IDT-C 5.1 Toolkit         */
/*---------------------------------------------------------------------------*/
/*   Create : 05.07.08   Update : 05.07.08                                   */
/*---------------------------------------------------------------------------*/
/*    Descriptions                                                           */
/*    ------------                                                           */
/*    Inputs    : none                                                       */
/*                                                                           */
/*    Outputs   :                                                            */
/*                                                                           */
/*    Description :                                                          */
/*                                                                           */
/*****************************************************************************/

L_WORD api_io_dig_out_read_apx(L_WORD id)
{
L_WORD lval,tmpl;
L_WORD ul_Conf;
L_WORD ul_Stat;

     lval = 0L;
     tmpl = 0L;
     
#if defined(_NUCLEUS)
     ul_Conf = BSWAP32(p_ASL_Lca_IoRegs->ul_GPIOConfReg) & 0xFFL;
     ul_Stat = BSWAP32(p_ASL_Lca_IoRegs->ul_GPIOCtrlStatReg);
     tmpl = (ul_Stat >> 16) & 0xFFL;
     tmpl &= ul_Conf; /* only outputs */
#endif

     lval = tmpl;

     return(lval);
    
} /* end: api_io_dig_out_read_apx */    

/*****************************************************************************/
/*                                                                           */
/*   Module : API_IO                   Submodule : API_IO_DIG_IN_READ_APX    */
/*                                                                           */
/*   Author : Marco Motz               Project   : API1553-DS                */
/*                                                                           */
/*   Source : C                        Tools     : PC/AT; Norton Editor;     */
/*                                                 CYGNUS, GNU-C, As, and LD */
/*                                                 IDT-C 5.1 Toolkit         */
/*---------------------------------------------------------------------------*/
/*   Create : 05.07.08   Update : 05.07.08                                   */
/*---------------------------------------------------------------------------*/
/*    Descriptions                                                           */
/*    ------------                                                           */
/*    Inputs    : none                                                       */
/*                                                                           */
/*    Outputs   :                                                            */
/*                                                                           */
/*    Description :                                                          */
/*                                                                           */
/*****************************************************************************/

L_WORD api_io_dig_in_read_apx(L_WORD id)
{
L_WORD lval,tmpl;
L_WORD ul_Conf;
L_WORD ul_Stat;
L_WORD ul_StatOut;
L_WORD ul_StatIn;

     lval = 0L;
     tmpl = 0L;

#if defined(_NUCLEUS)
     ul_Conf = BSWAP32(p_ASL_Lca_IoRegs->ul_GPIOConfReg) & 0xFFL;
     ul_Stat = BSWAP32(p_ASL_Lca_IoRegs->ul_GPIOCtrlStatReg);
     ul_StatOut  = (ul_Stat >> 16) & 0xFFL; /* get output value */
     ul_StatIn   =  ul_Stat & 0xFFL;        /* get input value */
     ul_StatOut &=  ul_Conf;          /* read output status */
     ul_StatIn  &= (ul_Conf ^ 0xFFL); /* read input status */
     tmpl = ul_StatOut | ul_StatIn; /* merge output and input together */
#endif
     
     lval = tmpl;
     
     return(lval);
    
} /* end: api_io_dig_in_read_apx */    

/*****************************************************************************/
/*                                                                           */
/*   Module : API_SYS                  Submodule : API_SYS_DRV_HW_INIT_IP408 */
/*                                                                           */
/*   Author : Marco Motz               Project   : API1553-DS                */
/*                                                                           */
/*   Source : C                        Tools     : PC/AT; Norton Editor;     */
/*                                                 CYGNUS, GNU-C, As, and LD */
/*                                                 IDT-C 5.1 Toolkit         */
/*---------------------------------------------------------------------------*/
/*   Create : 22.12.08   Update : 22.12.08                                   */
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

int api_sys_drv_hw_init_ip408(L_WORD id, L_WORD init_val1, L_WORD init_val2, L_WORD init_val3, L_WORD *out1, L_WORD *out2)
{

#ifdef DEBUG_IP
     PRINTF3("HW-INIT: val1=%08X val2=%08X val3=%08X\r\n",init_val1,init_val2,init_val3);
#endif

     api_io_dig_out_write_ip408(id, init_val1);
     
     return(0);

} /* end: api_sys_drv_hw_init_ip408 */

/*****************************************************************************/
/*                                                                           */
/*   Module : API_SYS                Submodule : API_SYS_DRV_OUT_WRITE_IP408 */
/*                                                                           */
/*   Author : Marco Motz               Project   : API1553-DS                */
/*                                                                           */
/*   Source : C                        Tools     : PC/AT; Norton Editor;     */
/*                                                 CYGNUS, GNU-C, As, and LD */
/*                                                 IDT-C 5.1 Toolkit         */
/*---------------------------------------------------------------------------*/
/*   Create : 22.12.08   Update : 22.12.08                                   */
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

int api_sys_drv_out_write_ip408(L_WORD id, L_WORD chn, L_WORD res, L_WORD val, L_WORD *out1, L_WORD *out2)
{
L_WORD tmpl,bval,rt_val;

#ifdef DEBUG_IP
     PRINTF3("OUT-CHN-WR: chn=%d res=%ld val=%08X\r\n",chn,res,val);
#endif

     if (chn <= (MAX_IP_DOUT_CHN+1)) {

        switch (chn) {
          case 0:
            /* write all discrete out channels at once */
            tmpl = val;
            if ((res & 0x01L) == 0x01L) tmpl = ~val; /* invert */

            ip_drv[id].out_data[0] = tmpl;

            break; 
               
          case 33:
            /* write all RT address discretes at once */
            rt_val = val & 0x1FL;
            tmpl = ip_drv[id].out_data[0];
            LWORD_RESET_BITS(tmpl,res,5);
            LWORD_SET_BITS(tmpl,rt_val,res,5);
            ip_drv[id].out_data[0] = tmpl;
            break;     
  
          default:
            /* write single discrete out channel */
            tmpl = ip_drv[id].out_data[0];
            if (res == 0L) {
               /*** Set Value ***/
               bval = val & 0x01L;
               if (bval == 0L) 
                  LWORD_RESET_BITS(tmpl,(chn-1),1);
               else 
                  LWORD_SET_BITS(tmpl,1,(chn-1),1);
                  ip_drv[id].out_data[0] = tmpl;
            }
            else {
                
                  /*** Toggle Value (for msec) ***/
                  bval = (tmpl >> (chn-1)) & 0x01L;
           
                  /* SET Discrete */
                  if (bval == 1L) 
                     LWORD_RESET_BITS(tmpl,(chn-1),1);
                  else 
                     LWORD_SET_BITS(tmpl,1,(chn-1),1);
                  api_io_dig_out_write_ip408(id, tmpl);
            
                  wait_clocks(px_api_dev->clk_1us * /*1000*/ 985 * res);

                  /* RESET Discrete */
                  if (bval == 1L) 
                     LWORD_SET_BITS(tmpl,1,(chn-1),1);
                  else 
                     LWORD_RESET_BITS(tmpl,(chn-1),1);
                  api_io_dig_out_write_ip408(id, tmpl);
            }
            break;  
        }   
     }
     
     return(0);

} /* end: api_sys_drv_out_write_ip408 */

/*****************************************************************************/
/*                                                                           */
/*   Module : API_SYS                 Submodule : API_SYS_DRV_OUT_READ_IP408 */
/*                                                                           */
/*   Author : Marco Motz               Project   : API1553-DS                */
/*                                                                           */
/*   Source : C                        Tools     : PC/AT; Norton Editor;     */
/*                                                 CYGNUS, GNU-C, As, and LD */
/*                                                 IDT-C 5.1 Toolkit         */
/*---------------------------------------------------------------------------*/
/*   Create : 22.12.08   Update : 22.12.08                                   */
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

int api_sys_drv_out_read_ip408(L_WORD id, L_WORD chn, L_WORD in2, L_WORD in3, L_WORD *val, L_WORD *out2)
{
L_WORD tmpl;

#ifdef DEBUG_IP
     PRINTF3("OUT-CHN-RD: chn=%d in2=%ld in3=%ld\r\n",chn,in2,in3);
#endif

     tmpl = 0L; 

     if (chn <= MAX_IP_DIN_CHN) {
        if(in2 == 0){
           ip_drv[id].act_data[0] = api_io_dig_out_read_ip408(id);
		   tmpl = ip_drv[id].act_data[0];
		}
		else {
		   tmpl = ip_drv[id].out_data[0];
		}
        if (chn != 0) {
           tmpl >>= (chn - 1);
           tmpl &= 0x01L;
        }
     }
     
     *val = tmpl;
     
     return(0);
       
} /* end: api_sys_drv_out_read_ip408 */

/*****************************************************************************/
/*                                                                           */
/*   Module : API_SYS                  Submodule : API_SYS_DRV_IN_READ_IP408 */
/*                                                                           */
/*   Author : Marco Motz               Project   : API1553-DS                */
/*                                                                           */
/*   Source : C                        Tools     : PC/AT; Norton Editor;     */
/*                                                 CYGNUS, GNU-C, As, and LD */
/*                                                 IDT-C 5.1 Toolkit         */
/*---------------------------------------------------------------------------*/
/*   Create : 22.12.08   Update : 22.12.08                                   */
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

int api_sys_drv_in_read_ip408(L_WORD id, L_WORD chn, L_WORD in2, L_WORD in3, L_WORD *val, L_WORD *out2)
{
L_WORD tmpl;

#ifdef DEBUG_IP
     PRINTF3("IN-CHN-RD ip408: chn=%d in2=%ld in3=%ld\r\n",chn,in2,in3);
#endif

     tmpl = 0L; 

     if (chn <= MAX_IP_DIN_CHN)
     {
         /* if in2 is set to 0, then read directly from the hardware */
	     if(in2 == 0)
         {
	        ip_drv[id].in_data[0] = api_io_dig_in_read_ip408(id);
		 }


         tmpl = ip_drv[id].in_data[0];
         if (chn != 0)
         {
             tmpl >>= (chn - 1);
             tmpl &= 0x01L;
         }
     }

     *val = tmpl;

     return(0);
       
} /* end: api_sys_drv_in_read_ip408 */

/*****************************************************************************/
/*                                                                           */
/*   Module : API_SYS                  Submodule : API_SYS_DRV_HW_INIT_IP470 */
/*                                                                           */
/*   Author : Marco Motz               Project   : API1553-DS                */
/*                                                                           */
/*   Source : C                        Tools     : PC/AT; Norton Editor;     */
/*                                                 CYGNUS, GNU-C, As, and LD */
/*                                                 IDT-C 5.1 Toolkit         */
/*---------------------------------------------------------------------------*/
/*   Create : 22.12.08   Update : 22.12.08                                   */
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

int api_sys_drv_hw_init_ip470(L_WORD id, L_WORD init_val1, L_WORD init_val2, L_WORD init_val3, L_WORD *out1, L_WORD *out2)
{

#ifdef DEBUG_IP
     PRINTF3("HW-INIT: val1=%08X val2=%08X val3=%08X\r\n",init_val1,init_val2,init_val3);
#endif

     /* Set Port0..3=INPUTs, Port4&5=OUTPUTs */
     api_io_dig_out_write_ip470(id,0x00000F00L,4L);
     api_io_dig_out_write_ip470(id,init_val1,0L);
     
     return(0);

} /* end: api_sys_drv_hw_init_ip470 */

/*****************************************************************************/
/*                                                                           */
/*   Module : API_SYS                Submodule : API_SYS_DRV_OUT_WRITE_IP470 */
/*                                                                           */
/*   Author : Marco Motz               Project   : API1553-DS                */
/*                                                                           */
/*   Source : C                        Tools     : PC/AT; Norton Editor;     */
/*                                                 CYGNUS, GNU-C, As, and LD */
/*                                                 IDT-C 5.1 Toolkit         */
/*---------------------------------------------------------------------------*/
/*   Create : 22.12.08   Update : 22.12.08                                   */
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

int api_sys_drv_out_write_ip470(L_WORD id, L_WORD chn, L_WORD res, L_WORD val, L_WORD *out1, L_WORD *out2)
{
L_WORD tmpl,bval,rt_val;

#ifdef DEBUG_IP
     PRINTF3("OUT-CHN-WR: chn=%d res=%ld val=%08X\r\n",chn,res,val);
#endif

     if (chn <= (MAX_IP_DOUT_CHN+1)) {

        switch (chn) {
          case 0:
            /* write all discrete out channels at once */
            tmpl = val;
            if ((res & 0x01L) == 0x01L) tmpl = ~val; /* invert */
            api_io_dig_out_write_ip470(id,tmpl,0L);
            break; 
               
          case 33:
            /* write all RT address discretes at once */
            rt_val = val & 0x1FL;
            tmpl = api_io_dig_out_read_ip470(id);
            LWORD_RESET_BITS(tmpl,res,5);
            LWORD_SET_BITS(tmpl,rt_val,res,5);
            api_io_dig_out_write_ip470(id,tmpl,0L);
            break;     
  
          default:
            /* write single discrete out channel */
            tmpl = api_io_dig_out_read_ip470(id);
            if (res == 0L) {
               /*** Set Value ***/
               bval = val & 0x01L;
               if (bval == 0L) 
                  LWORD_RESET_BITS(tmpl,(chn-1),1);
               else 
                  LWORD_SET_BITS(tmpl,1,(chn-1),1);
               api_io_dig_out_write_ip470(id,tmpl,0L);
            }
            else {
               /*** Toggle Value (for msec) ***/
               bval = (tmpl >> (chn-1)) & 0x01L;
           
               /* SET Discrete */
               if (bval == 1L) 
                  LWORD_RESET_BITS(tmpl,(chn-1),1);
               else 
                  LWORD_SET_BITS(tmpl,1,(chn-1),1);
               api_io_dig_out_write_ip470(id,tmpl,0L);
            
               wait_clocks(px_api_dev->clk_1us * 1000 * res);

               /* RESET Discrete */
               if (bval == 1L) 
                  LWORD_SET_BITS(tmpl,1,(chn-1),1);
               else 
                  LWORD_RESET_BITS(tmpl,(chn-1),1);
               api_io_dig_out_write_ip470(id,tmpl,0L);
            }
            break;  
        }   
     }
     
     return(0);

} /* end: api_sys_drv_out_write_ip470 */

/*****************************************************************************/
/*                                                                           */
/*   Module : API_SYS                 Submodule : API_SYS_DRV_OUT_READ_IP470 */
/*                                                                           */
/*   Author : Marco Motz               Project   : API1553-DS                */
/*                                                                           */
/*   Source : C                        Tools     : PC/AT; Norton Editor;     */
/*                                                 CYGNUS, GNU-C, As, and LD */
/*                                                 IDT-C 5.1 Toolkit         */
/*---------------------------------------------------------------------------*/
/*   Create : 22.12.08   Update : 22.12.08                                   */
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

int api_sys_drv_out_read_ip470(L_WORD id, L_WORD chn, L_WORD in2, L_WORD in3, L_WORD *val, L_WORD *out2)
{
L_WORD tmpl;

#ifdef DEBUG_IP
     PRINTF3("OUT-CHN-RD: chn=%d in2=%ld in3=%ld\r\n",chn,in2,in3);
#endif

     tmpl = 0L; 

     if (chn <= MAX_IP_DIN_CHN) {
        tmpl = api_io_dig_out_read_ip470(id);
        if (chn != 0) {
           tmpl >>= (chn - 1);
           tmpl &= 0x01L;
        }
     }
     
     *val = tmpl;
     
     return(0);
       
} /* end: api_sys_drv_out_read_ip470 */

/*****************************************************************************/
/*                                                                           */
/*   Module : API_SYS                  Submodule : API_SYS_DRV_IN_READ_IP470 */
/*                                                                           */
/*   Author : Marco Motz               Project   : API1553-DS                */
/*                                                                           */
/*   Source : C                        Tools     : PC/AT; Norton Editor;     */
/*                                                 CYGNUS, GNU-C, As, and LD */
/*                                                 IDT-C 5.1 Toolkit         */
/*---------------------------------------------------------------------------*/
/*   Create : 22.12.08   Update : 22.12.08                                   */
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

int api_sys_drv_in_read_ip470(L_WORD id, L_WORD chn, L_WORD in2, L_WORD in3, L_WORD *val, L_WORD *out2)
{
L_WORD tmpl;

#ifdef DEBUG_IP
     PRINTF3("IN-CHN-RD ip470: chn=%d in2=%ld in3=%ld\r\n",chn,in2,in3);
#endif

     tmpl = 0L; 

     if (chn <= MAX_IP_DIN_CHN) {
        tmpl = api_io_dig_in_read_ip470(id);
        if (chn != 0) {
           tmpl >>= (chn - 1);
           tmpl &= 0x01L;
        }
     }
     
     *val = tmpl;
     
     return(0);
       
} /* end: api_sys_drv_in_read_ip470 */
/*****************************************************************************/
/*                                                                           */
/*   Module : API_SYS                  Submodule : API_SYS_DRV_HW_INIT_IP850 */
/*                                                                           */
/*   Author : Marco Motz               Project   : API1553-DS                */
/*            Thomas Jahn                                                    */
/*   Source : C                        Tools     : PC/AT; Norton Editor;     */
/*                                                 CYGNUS, GNU-C, As, and LD */
/*                                                 IDT-C 5.1 Toolkit         */
/*---------------------------------------------------------------------------*/
/*   Create : 18.10.10   Update : 18.10.10                                   */
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

int api_sys_drv_hw_init_ip850(L_WORD id, L_WORD init_val1, L_WORD init_val2, L_WORD init_val3, L_WORD *out1, L_WORD *out2)
{

#ifdef DEBUG_IP
     PRINTF3("HW-INIT: val1=%08X val2=%08X val3=%08X\r\n",init_val1,init_val2,init_val3);
#endif

     api_io_ana_out_write_ip850(id, 0 /*chn*/, init_val1);

#ifdef DEBUG_IP
     printf("ip_850_id=%ld\r\n",id);
#endif

     return(0);

} /* end: api_sys_drv_hw_init_ip850 */

/*****************************************************************************/
/*                                                                           */
/*   Module : API_SYS                Submodule : API_SYS_DRV_OUT_WRITE_IP850 */
/*                                                                           */
/*   Author : Marco Motz               Project   : API1553-DS                */
/*            Thomas Jahn                                                    */
/*   Source : C                        Tools     : PC/AT; Norton Editor;     */
/*                                                 CYGNUS, GNU-C, As, and LD */
/*                                                 IDT-C 5.1 Toolkit         */
/*---------------------------------------------------------------------------*/
/*   Create : 18.10.10   Update : 18.10.10                                   */
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

int api_sys_drv_out_write_ip850(L_WORD id, L_WORD chn, L_WORD res, L_WORD val, L_WORD *out1, L_WORD *out2)
{

#ifdef DEBUG_IP
     PRINTF3("OUT-CHN-WR: chn=%d res=%ld val=%08X\r\n",chn,res,val);
#endif

	 ip_drv[id].out_data[chn-1] = val; 

     return(0);

} /* end: api_sys_drv_out_write_ip850 */

/*****************************************************************************/
/*                                                                           */
/*   Module : API_SYS                 Submodule : API_SYS_DRV_OUT_READ_IP850 */
/*                                                                           */
/*   Author : Marco Motz               Project   : API1553-DS                */
/*                                                                           */
/*   Source : C                        Tools     : PC/AT; Norton Editor;     */
/*                                                 CYGNUS, GNU-C, As, and LD */
/*                                                 IDT-C 5.1 Toolkit         */
/*---------------------------------------------------------------------------*/
/*   Create : 22.12.08   Update : 22.12.08                                   */
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

int api_sys_drv_out_read_ip850(L_WORD id, L_WORD chn, L_WORD in2, L_WORD in3, L_WORD *val, L_WORD *out2)
{

     *val = ip_drv[id].out_data[chn-1];
     
     return(0);
       
} /* end: api_sys_drv_out_read_ip850 */

/*****************************************************************************/
/*                                                                           */
/*   Module : API_SYS                  Submodule : API_SYS_DRV_IN_READ_IP850 */
/*                                                                           */
/*   Author : Marco Motz               Project   : API1553-DS                */
/*            Thomas Jahn                                                    */
/*   Source : C                        Tools     : PC/AT; Norton Editor;     */
/*                                                 CYGNUS, GNU-C, As, and LD */
/*                                                 IDT-C 5.1 Toolkit         */
/*---------------------------------------------------------------------------*/
/*   Create : 18.10.10   Update : 18.10.10                                   */
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

int api_sys_drv_in_read_ip850(L_WORD id, L_WORD chn, L_WORD in2, L_WORD in3, L_WORD *val, L_WORD *out2)
{
L_WORD tmpl,cnt;

#ifdef DEBUG_IP
     PRINTF3("IN-CHN-RD ip850: chn=%d in2=%ld in3=%ld\r\n",chn,in2,in3);
#endif

     tmpl = 0L; 
     cnt = 0L; 

	 if(in2 == 0){
		 if (chn == 0){
	        tmpl = api_io_ana_in_read_ip850(id,chn);
		 }else if (chn <= MAX_IP_AIN_CHN) {
		    /* write to ctrl register */
	        api_io_ana_out_write_ip850(id, 0, 0x90/* Automatic Setting Time + Differential*/ + (chn-1));

	        /* read status register */
	        tmpl = api_io_ana_in_read_ip850(id,0);
			while ((tmpl & 0xC000) != 0){
	           tmpl = api_io_ana_in_read_ip850(id,0);
			   cnt++;
			   if (cnt > 20) break;		   /* timeout*/
			}
	        ip_drv[id].in_data[chn-1] = api_io_ana_in_read_ip850(id,chn);
	     }
     }

     *val = ip_drv[id].in_data[chn-1];
     
     return(0);
       
} /* end: api_sys_drv_in_read_ip850 */

/*****************************************************************************/
/*                                                                           */
/*   Module : API_SYS                  Submodule : API_SYS_DRV_HW_INIT_APX   */
/*                                                                           */
/*   Author : Marco Motz               Project   : API1553-DS                */
/*                                                                           */
/*   Source : C                        Tools     : PC/AT; Norton Editor;     */
/*                                                 CYGNUS, GNU-C, As, and LD */
/*                                                 IDT-C 5.1 Toolkit         */
/*---------------------------------------------------------------------------*/
/*   Create : 22.12.08   Update : 22.12.08                                   */
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

int api_sys_drv_hw_init_apx(L_WORD id, L_WORD init_val1, L_WORD init_val2, L_WORD init_val3, L_WORD *out1, L_WORD *out2)
{
L_WORD tmpl;

#ifdef DEBUG_IP
     PRINTF3("HW-INIT: val1=%08X val2=%08X val3=%08X\r\n",init_val1,init_val2,init_val3);
#endif

     #if defined(_NUCLEUS)
      tmpl = BSWAP32(p_ASL_Lca_IoRegs->ul_GPIOConfReg);
      tmpl &= 0xFFFFFF00L;
      tmpl |= (init_val1 & 0xFFL);
      p_ASL_Lca_IoRegs->ul_GPIOConfReg = BSWAP32(tmpl);
     #endif 

     api_io_dig_out_write_apx(id, 0L);
     
     return(0);

} /* end: api_sys_drv_hw_init_apx */

/*****************************************************************************/
/*                                                                           */
/*   Module : API_SYS                  Submodule : API_SYS_DRV_OUT_WRITE_APX */
/*                                                                           */
/*   Author : Marco Motz               Project   : API1553-DS                */
/*                                                                           */
/*   Source : C                        Tools     : PC/AT; Norton Editor;     */
/*                                                 CYGNUS, GNU-C, As, and LD */
/*                                                 IDT-C 5.1 Toolkit         */
/*---------------------------------------------------------------------------*/
/*   Create : 22.12.08   Update : 22.12.08                                   */
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

int api_sys_drv_out_write_apx(L_WORD id, L_WORD chn, L_WORD res, L_WORD val, L_WORD *out1, L_WORD *out2)
{
L_WORD tmpl,bval;

#ifdef DEBUG_IP
     PRINTF3("OUT-CHN-WR: chn=%d res=%ld val=%08X\r\n",chn,res,val);
#endif

     if (chn <= MAX_APX_DOUT_CHN) {

        switch (chn) {
          case 0:
            /* write all discrete out channels at once */
            tmpl = val;
            if ((res & 0x01L) == 0x01L) tmpl = ~val; /* invert */
            api_io_dig_out_write_apx(id, tmpl);
            break; 
  
          default:
            /* write single discrete out channel */
            tmpl = api_io_dig_out_read_apx(id);
            if (res == 0L) {
               /*** Set Value ***/
               bval = val & 0x01L;
               if (bval == 0L) 
                  LWORD_RESET_BITS(tmpl,(chn-1),1);
               else 
                  LWORD_SET_BITS(tmpl,1,(chn-1),1);
               api_io_dig_out_write_apx(id, tmpl);
            }
            else {
               /*** Toggle Value (for msec) ***/
               bval = (tmpl >> (chn-1)) & 0x01L;
           
               /* SET Discrete */
               if (bval == 1L) 
                  LWORD_RESET_BITS(tmpl,(chn-1),1);
               else 
                  LWORD_SET_BITS(tmpl,1,(chn-1),1);
               api_io_dig_out_write_apx(id, tmpl);
            
               wait_clocks(px_api_dev->clk_1us * 1000 * res);

               /* RESET Discrete */
               if (bval == 1L) 
                  LWORD_SET_BITS(tmpl,1,(chn-1),1);
               else 
                  LWORD_RESET_BITS(tmpl,(chn-1),1);
               api_io_dig_out_write_apx(id, tmpl);
            }
            break;  
        }   
     }
     
     return(0);

} /* end: api_sys_drv_out_write_apx */

/*****************************************************************************/
/*                                                                           */
/*   Module : API_SYS                  Submodule : API_SYS_DRV_OUT_READ_APX  */
/*                                                                           */
/*   Author : Marco Motz               Project   : API1553-DS                */
/*                                                                           */
/*   Source : C                        Tools     : PC/AT; Norton Editor;     */
/*                                                 CYGNUS, GNU-C, As, and LD */
/*                                                 IDT-C 5.1 Toolkit         */
/*---------------------------------------------------------------------------*/
/*   Create : 22.12.08   Update : 22.12.08                                   */
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

int api_sys_drv_out_read_apx(L_WORD id, L_WORD chn, L_WORD in2, L_WORD in3, L_WORD *val, L_WORD *out2)
{
L_WORD tmpl;

#ifdef DEBUG_IP
     PRINTF3("OUT-CHN-RD: chn=%d in2=%ld in3=%ld\r\n",chn,in2,in3);
#endif

     tmpl = 0L; 

     if (chn <= MAX_APX_DIN_CHN) {
        tmpl = api_io_dig_out_read_apx(id);
        if (chn != 0) {
           tmpl >>= (chn - 1);
           tmpl &= 0x01L;
        }
     }
     
     *val = tmpl;
     
     return(0);
       
} /* end: api_sys_drv_out_read_apx */

/*****************************************************************************/
/*                                                                           */
/*   Module : API_SYS                  Submodule : API_SYS_DRV_IN_READ_APX   */
/*                                                                           */
/*   Author : Marco Motz               Project   : API1553-DS                */
/*                                                                           */
/*   Source : C                        Tools     : PC/AT; Norton Editor;     */
/*                                                 CYGNUS, GNU-C, As, and LD */
/*                                                 IDT-C 5.1 Toolkit         */
/*---------------------------------------------------------------------------*/
/*   Create : 22.12.08   Update : 22.12.08                                   */
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

int api_sys_drv_in_read_apx(L_WORD id, L_WORD chn, L_WORD in2, L_WORD in3, L_WORD *val, L_WORD *out2)
{
L_WORD tmpl;

#ifdef DEBUG_IP
     PRINTF3("IN-CHN-RD apx: chn=%d in2=%ld in3=%ld\r\n",chn,in2,in3);
#endif

     tmpl = 0L; 

     if (chn <= MAX_APX_DIN_CHN) {
        tmpl = api_io_dig_in_read_apx(id);
        if (chn != 0) {
           tmpl >>= (chn - 1);
           tmpl &= 0x01L;
        }
     }
     
     *val = tmpl;
     
     return(0);
       
} /* end: api_sys_drv_in_read_apx */

             
/*****************************************************************************/
/*                                                                           */
/*   Module : API_SYS                  Submodule : API_SYS_DRV_GLB_INIT      */
/*                                                                           */
/*   Author : Marco Motz               Project   : API1553-DS                */
/*                                                                           */
/*   Source : C                        Tools     : PC/AT; Norton Editor;     */
/*                                                 CYGNUS, GNU-C, As, and LD */
/*                                                 IDT-C 5.1 Toolkit         */
/*---------------------------------------------------------------------------*/
/*   Create : 22.12.08   Update : 22.12.08                                   */
/*---------------------------------------------------------------------------*/
/*    Descriptions                                                           */
/*    ------------                                                           */
/*    Inputs    : none                                                       */
/*                                                                           */
/*    Outputs   : none                                                       */
/*                                                                           */
/*    Description :                                                          */
/*    This function iss used to init Global Driver Variables & Structs.      */
/*                                                                           */
/*****************************************************************************/

void api_sys_drv_glb_init(void)
{
int j;

#ifdef DEBUG_IP
     PRINTF3("api_sys_drv_glb_init\r\n", 0, 0, 0);
#endif

     for (j=0; j<MAX_IP_DRV; j++) {
   
        ip_drv[j].used = API_OFF;
        ip_drv[j].ip_type = 0;
        ip_drv[j].p_dhw_init_func = 0;
        ip_drv[j].p_dout_write_func = 0;
        ip_drv[j].p_dout_read_func = 0;
        ip_drv[j].p_din_read_func = 0;
      
        /* IP Access Setup */
        ip_drv[j].ip_setup.din.model   = 0L;
        ip_drv[j].ip_setup.din.ip_addr = 0L;
        ip_drv[j].ip_setup.din.sh_addr = 0L;
        ip_drv[j].ip_setup.din.data    = 0L;

        ip_drv[j].ip_setup.dout.model   = 0L;
        ip_drv[j].ip_setup.dout.ip_addr = 0L;
        ip_drv[j].ip_setup.dout.sh_addr = 0L;
        ip_drv[j].ip_setup.dout.data    = 0L;
       
        /* IP Sampling Task Setup */
        ip_drv[j].stask_setup.ena     = API_OFF;
        ip_drv[j].stask_setup.isr_cnt = 0L;
        ip_drv[j].stask_setup.rate    = 0L;
        ip_drv[j].stask_setup.res     = 0L;
        ip_drv[j].stask_setup.ustep   = 0L;
        
        /* IP Discretes Recording Status */
        ip_drv[j].rec_status.rec_con    = API_OFF;         
        ip_drv[j].rec_status.rec_status = API_HALT;        
        ip_drv[j].rec_status.status     = 0L;              
        ip_drv[j].rec_status.wr_addr    = 0L;              
        ip_drv[j].rec_status.rd_addr    = 0L;              
        ip_drv[j].rec_status.entry_cnt  = 0L;              
     }
       
} /* end: api_sys_drv_glb_init */

/*****************************************************************************/
/*                                                                           */
/*   Module : API_SYS                  Submodule : API_SYS_DRV_REC_CON       */
/*                                                                           */
/*   Author : Marco Motz               Project   : API1553-DS                */
/*                                                                           */
/*   Source : C                        Tools     : PC/AT; Norton Editor;     */
/*                                                 CYGNUS, GNU-C, As, and LD */
/*                                                 IDT-C 5.1 Toolkit         */
/*---------------------------------------------------------------------------*/
/*   Create : 22.12.08   Update : 22.12.08                                   */
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

void api_sys_drv_rec_con(L_WORD con)
{
int j;
     
#ifdef DEBUG_IP
     PRINTF3("api_sys_drv_rec_con: con=%d\r\n",con,0,0);
#endif
    
     for (j=0; j<MAX_IP_DRV; j++) {
        if ((ip_drv[j].used != 0) && (ip_drv[j].p_din_read_func != NULL)) {
           if (con == API_ON) {
              /* Enable Recording */
              ip_drv[j].rec_status.status     = 1L;
              ip_drv[j].rec_status.wr_addr    = 0L;
              ip_drv[j].rec_status.rd_addr    = 0L;
              ip_drv[j].rec_status.entry_cnt  = 0L;
              ip_drv[j].rec_status.rec_con    = API_ON;
              ip_drv[j].rec_status.rec_status = API_BUSY;
           }   
           else {
              /* Disable Recording */
              ip_drv[j].rec_status.rec_con    = API_OFF;
              ip_drv[j].rec_status.rec_status = API_HALT;
              ip_drv[j].rec_status.status = 0L;
           }   
        }
     }
     
} /* end: api_sys_drv_rec_con */

/*****************************************************************************/
/*                                                                           */
/*   Module : API_SYS                  Submodule : API_SYS_IP_READ_ADDR      */
/*                                                                           */
/*   Author : Thomas Jahn              Project   : API1553-DS                */
/*                                                                           */
/*   Source : C                        Tools     : PC/AT; Norton Editor;     */
/*                                                 CYGNUS, GNU-C, As, and LD */
/*                                                 IDT-C 5.1 Toolkit         */
/*---------------------------------------------------------------------------*/
/*   Create : 20.07.10   Update :                                            */
/*---------------------------------------------------------------------------*/
/*    Descriptions                                                           */
/*    ------------                                                           */
/*    Inputs    :                                                            */
/*                                                                           */
/*    Outputs   :                                                            */
/*                                                                           */
/*    Description :reads value from external addr (pci-bus-master)           */
/*                                                                           */
/*****************************************************************************/

BYTE api_sys_gen_io_read(TY_API_DEV *p_api_dev, L_WORD addr,L_WORD res)
{
volatile L_WORD *ptr;
ty_api_dma_setup x_DmaSetup;
L_WORD lval,sh_addr,tmpl;
L_WORD found, id;

     lval = 0L;
	 found = 0;
     if (addr != 0L) {
        for (id=0; id<MAX_IP_DRV; id++) {
           if ((ip_drv[id].used != 0) && (ip_drv[id].p_din_read_func != NULL)) {
			  if(addr == ip_drv[id].ip_setup.dout.ip_addr){
			     found = 1;
                 ip_drv[id].p_dout_read_func(id, 0, 1/* read from value*/, 0, &tmpl, 0);
                 lval  =  (tmpl >> 8) & 0x00FF00FFL;      
                 lval |= ((tmpl << 8) & 0xFF00FF00L);      
			  }
		   }
		}

		if(found != 1)
        {
		    sh_addr = (L_WORD)API_DATASET_MEM_ADDR_ABS(0/*dsid*/);
	        /* clear */
	        ptr = (L_WORD *)(sh_addr);
	        *ptr = 0L;

	        /* Setup & DMA Read */
	        x_DmaSetup.ul_Size        = 4;
	        x_DmaSetup.ul_Source      = sh_addr & 0x0FFFFFFFL;
	        x_DmaSetup.ul_Destination = addr;
	        wbflush();
	        x_DmaSetup.v_CallBack = NULL;
	        x_DmaSetup.uc_Type = API_DMA_DIRECTION_READ_SHARED;
	        api_dma_chn1_setup_start(px_api_dev, 1/*Read*/, &x_DmaSetup);

	        /* Read value from Shared Memory */
	        lval = *ptr;  

#ifdef DEBUG_IP
        PRINTF3("INPUT-GEN-IO: ul_Source=%08X ul_Destination=%08X Value=%08X\r\n", x_DmaSetup.ul_Source,x_DmaSetup.ul_Destination,lval);
#endif
		}
     }
     p_api_dev->lword_data_arr[0] = lval;
     p_api_dev->b_length   = 0;
     p_api_dev->w_length   = 0;
     p_api_dev->lw_length  = 1;  
     p_api_dev->ack_lw_ptr = &(p_api_dev->lword_data_arr[0]);

     return(API_ACK2);
}


/*****************************************************************************/
/*    Descriptions                                                           */
/*    ------------                                                           */
/*    Inputs    :                                                            */
/*                                                                           */
/*    Outputs   :                                                            */
/*                                                                           */
/*    Description :                                                          */
/*                                                                           */
/*****************************************************************************/

BYTE api_gen_io_test_seq_DiscrOutSet(TY_API_DEV *p_api_dev, L_WORD ip, L_WORD val)
{
    BYTE ip_type    = 0;
    BYTE slot       = 0;

    if( ip == 0 )
    {
        return 1;
    }

     ip_type = ip & 0x0F;
     slot = ip >> 4; 

     if ((ip_type != 0) && (slot < MAX_IP_DRV)) {
       if ((ip_drv[slot].used != 0) && (ip_drv[slot].p_dout_write_func != NULL)) {
            ip_drv[slot].p_dout_write_func(slot, 0, 0, val, 0, 0);
       }
     } 

    return 0;
}



BYTE api_gen_io_test_seq_DiscrOutRead(TY_API_DEV *p_api_dev, L_WORD ip, L_WORD *val)
{
    BYTE ip_type    = 0;
    BYTE slot       = 0;

    if( ip == 0 )
    {
        return 1;
    }

    ip_type = ip & 0x0F;
    slot = ip >> 4; 

    if ((ip_type != 0) && (slot < MAX_IP_DRV)) {
        if ((ip_drv[slot].used != 0) && (ip_drv[slot].p_dout_read_func != NULL)) {
            ip_drv[slot].p_dout_read_func(slot, 0, 1, 0, val, 0);
        }
    }

    return 0;
}



BYTE api_gen_io_test_seq_DiscrInRead(TY_API_DEV *p_api_dev, L_WORD ip, L_WORD *val)
{
    BYTE ip_type    = 0;
    BYTE slot       = 0;

    if( ip == 0 )
    {
        //printf("ip == 0\r\n");
        return 1;
    }
    
    ip_type = ip & 0x0F;
    slot = ip >> 4; 

    if ((ip_type != 0) && (slot < MAX_IP_DRV)) {
        if ((ip_drv[slot].used != 0) && (ip_drv[slot].p_din_read_func != NULL)) {
            ip_drv[slot].p_din_read_func(slot, 0, 1, 0, val, 0);
        }
    }

    return 0;
}

BYTE api_gen_io_test_seq_DiscrRead(TY_API_DEV *p_api_dev, L_WORD cond, L_WORD *val)
{
    L_WORD ip      = 0;
    L_WORD inOrOut = 0;

    ip      = cond & 0xFF;
    inOrOut = (cond >>24) & 0xFF;

    if( inOrOut == 0 )
        return api_gen_io_test_seq_DiscrInRead( p_api_dev, ip, val );
    else
        return api_gen_io_test_seq_DiscrOutRead( p_api_dev, ip, val );

}


/*****************************************************************************/
/*    Descriptions                                                           */
/*    ------------                                                           */
/*    Inputs    :                                                            */
/*                                                                           */
/*    Outputs   :                                                            */
/*                                                                           */
/*    Description :                                                          */
/*                                                                           */
/*****************************************************************************/

BYTE api_gen_io_test_seq_check_start_cond(TY_API_DEV *p_api_dev, L_WORD oldDiscr, L_WORD currentDiscr, L_WORD cond /* start conditions */ )
{
    L_WORD discreteChannel = 0;
    L_WORD discreteMask    = 0;

    discreteChannel = (cond >>8) & 0xFF;
    discreteMask = 1<<(discreteChannel);

    switch( (cond >> 16) & 0xFF )
    {
    case 0:
        /* changed from 1 -> 0 */
        if( ((oldDiscr & discreteMask)     == discreteMask) &&
            ((currentDiscr & discreteMask) == 0 )  )
        {
            if( (p_api_dev->ul_DbgLevel & DBG_GENIO) )
                PRINTF2("Trigger 1-->0: 0x%08x --> 0x%08X\r\n", (oldDiscr & discreteMask), (currentDiscr & discreteMask) );
            return 1;
        }
        break;
    case 1:
        /* changed from 0 -> 1 */
        if( ((oldDiscr & discreteMask)     == 0) &&
            ((currentDiscr & discreteMask) == discreteMask )  )
        {
            if( (p_api_dev->ul_DbgLevel & DBG_GENIO) )
                PRINTF2("Trigger 0-->1: 0x%08x --> 0x%08X\r\n", (oldDiscr & discreteMask), (currentDiscr & discreteMask) );
            return 1;
        }
        break;
    case 2:
        /* changed */
        if( (oldDiscr & discreteMask) != (currentDiscr & discreteMask) )
        {
            if( (p_api_dev->ul_DbgLevel & DBG_GENIO) )
                PRINTF2("Trigger toggle: 0x%08x --> 0x%08X\r\n", (oldDiscr & discreteMask), (currentDiscr & discreteMask) );
            return 1;
        }
        break;
    default: 
        /* 3 .. 255 reserved */
        PRINTF0("api_gen_io_test_seq_check_start_cond default\r\n");
        return 1;

    }

    return 0;
}

/*****************************************************************************/
/*    Descriptions                                                           */
/*    ------------                                                           */
/*    Inputs    :                                                            */
/*                                                                           */
/*    Outputs   :                                                            */
/*                                                                           */
/*    Description :                                                          */
/*                                                                           */
/*****************************************************************************/

void api_gen_io_test_seq_handle_sequence(TY_API_DEV *p_api_dev, L_WORD SeqNo /*Sequence Number*/ )
{

    L_WORD                      index   = p_api_dev->aul_CurrentSequenceEvent[SeqNo];
    TY_API_GENIO_TEST_SEQ_ARRAY TestSeq = p_api_dev->xGenIoSeqTbl.ax_Seq[SeqNo];
    L_WORD                      tmp     = 0;
    L_WORD                      ip      = 0; /* GenIo slot and board type */



    if( p_api_dev->auc_SequenceActive[SeqNo] == 0 )
    {
        /* check if we have to wait to start */
        if( TestSeq.mode == 1 )
        {
            ip = TestSeq.cond & 0xFF;

            api_gen_io_test_seq_DiscrRead( p_api_dev, TestSeq.cond, &tmp );

            /* check if TestSeq.cond conditions are met */
            if( api_gen_io_test_seq_check_start_cond(p_api_dev, p_api_dev->ul_LastDiscrStat[SeqNo], tmp, TestSeq.cond) )
            {
                p_api_dev->auc_SequenceActive[SeqNo] = 1;
                p_api_dev->ul_GenIoTimer[SeqNo]      = 0;
            }

            p_api_dev->ul_LastDiscrStat[SeqNo] = tmp;
        }
    }



    if(p_api_dev->auc_SequenceActive[SeqNo] == 1)
    {
        if( p_api_dev->ul_GenIoTimer[SeqNo] >= TestSeq.tm_ms[index]*1000 )
        {
            if( TestSeq.genio_type_op[index] == 0 )
            {
                /* skip this command */
                //printf("skip%d.", SeqNo);
            }
            else
            {
                if( (p_api_dev->ul_DbgLevel & DBG_GENIO) )
                    PRINTF6("E%x %x: %d.0x08%x.0x%x   [%d]\r\n", SeqNo, index, TestSeq.tm_ms[index], TestSeq.chn_val[index], TestSeq.genio_type_op[index], p_api_dev->ul_GenIoTimer[SeqNo] );



                /****************************************/
                /* do something                         */
                /****************************************/

                if( TestSeq.genio_type_op[index] & 0x80000000 )  /* Bit 31 */
                {
                    /* send acyclic */
                    if( (p_api_dev->ul_DbgLevel & DBG_GENIO) )
                        PRINTF0("send acyclic (only on BIU0)\r\n");

                    /* currently works only for BIU_0. */
                    (*p_api_dev->cb_p[0]).aip = ul_ConvertTSWPtrToGRAMPtr( p_api_dev, 0, TestSeq.chn_val[index]);
                }
                else
                {
                    /* change discretes */
                    ip = TestSeq.genio_type_op[index] & 0xFF;

                    switch( (TestSeq.genio_type_op[index] & 0x00000F00) >> 8 )  /* Bit 11..8 */
                    {
                    case 0:
                        if( (p_api_dev->ul_DbgLevel & DBG_GENIO) )
                            PRINTF0("toggling\r\n");

                        /* toggle discrete output */
                        api_gen_io_test_seq_DiscrOutRead( p_api_dev, ip, &tmp );
                        tmp ^= TestSeq.chn_val[index];
                        api_gen_io_test_seq_DiscrOutSet(p_api_dev, ip, tmp);
                        break;

                    case 1:
                        if( (p_api_dev->ul_DbgLevel & DBG_GENIO) )
                            PRINTF0("setting to 0\r\n");

                        /* set masked discrete output to 0 */
                        api_gen_io_test_seq_DiscrOutRead( p_api_dev, ip, &tmp );
                        tmp &= ~TestSeq.chn_val[index];
                        api_gen_io_test_seq_DiscrOutSet(p_api_dev, ip, tmp);
                        break;

                    case 2:
                        if( (p_api_dev->ul_DbgLevel & DBG_GENIO) )
                            PRINTF0("setting to 1\r\n");

                        /* set masked discrete output to 1 */
                        api_gen_io_test_seq_DiscrOutRead( p_api_dev, ip, &tmp );
                        tmp |= TestSeq.chn_val[index];
                        api_gen_io_test_seq_DiscrOutSet(p_api_dev, ip, tmp);
                        break;

                    default: /* 3..15 are reserved */
                        if( (p_api_dev->ul_DbgLevel & DBG_GENIO) )
                            PRINTF0("skipping\r\n");
                        break;
                    }

                }
            }












            if( (index+1 >= MAX_GENIO_TEST_SEQ_ARRAY_SIZE)         ||
                (TestSeq.tm_ms[index + 1] > 10000)                 ||
                (TestSeq.tm_ms[index + 1] < TestSeq.tm_ms[index])  )
            {
                p_api_dev->auc_SequenceActive[SeqNo] = 2;
                
                if( (p_api_dev->ul_DbgLevel & DBG_GENIO) )
                    PRINTF1("Seq:%d off\r\n", SeqNo);
            }


            p_api_dev->aul_CurrentSequenceEvent[SeqNo] ++;

        }
    }

}

void api_gen_io_test_seq_handle_event(TY_API_DEV *p_api_dev, L_WORD timeIncr /* time increment since last call*/)
{
    p_api_dev->ul_GenIoTimer[0] += timeIncr;
    p_api_dev->ul_GenIoTimer[1] += timeIncr;
    p_api_dev->ul_GenIoTimer[2] += timeIncr;
    p_api_dev->ul_GenIoTimer[3] += timeIncr;


    api_gen_io_test_seq_handle_sequence( p_api_dev, 0 );
    api_gen_io_test_seq_handle_sequence( p_api_dev, 1 );
    api_gen_io_test_seq_handle_sequence( p_api_dev, 2 );
    api_gen_io_test_seq_handle_sequence( p_api_dev, 3 );
}



/*****************************************************************************/
/*    Descriptions                                                           */
/*    ------------                                                           */
/*    Inputs    :                                                            */
/*                                                                           */
/*    Outputs   :                                                            */
/*                                                                           */
/*    Description :                                                          */
/*                                                                           */
/*****************************************************************************/


void api_gen_io_test_seq_start(TY_API_DEV *p_api_dev)
{
    api_gen_io_test_seq_DiscrRead(  p_api_dev, p_api_dev->xGenIoSeqTbl.ax_Seq[0].cond, &p_api_dev->ul_LastDiscrStat[0] );
    api_gen_io_test_seq_DiscrRead(  p_api_dev, p_api_dev->xGenIoSeqTbl.ax_Seq[1].cond, &p_api_dev->ul_LastDiscrStat[1] );
    api_gen_io_test_seq_DiscrRead(  p_api_dev, p_api_dev->xGenIoSeqTbl.ax_Seq[2].cond, &p_api_dev->ul_LastDiscrStat[2] );
    api_gen_io_test_seq_DiscrRead(  p_api_dev, p_api_dev->xGenIoSeqTbl.ax_Seq[3].cond, &p_api_dev->ul_LastDiscrStat[3] );



    if( p_api_dev->xGenIoSeqTbl.ax_Seq[0].mode == 0 )  p_api_dev->auc_SequenceActive[0] = 1;
    if( p_api_dev->xGenIoSeqTbl.ax_Seq[1].mode == 0 )  p_api_dev->auc_SequenceActive[1] = 1;
    if( p_api_dev->xGenIoSeqTbl.ax_Seq[2].mode == 0 )  p_api_dev->auc_SequenceActive[2] = 1;
    if( p_api_dev->xGenIoSeqTbl.ax_Seq[3].mode == 0 )  p_api_dev->auc_SequenceActive[3] = 1;

    p_api_dev->ul_GenIoTimer[0] = 0;
    p_api_dev->ul_GenIoTimer[1] = 0;
    p_api_dev->ul_GenIoTimer[2] = 0;
    p_api_dev->ul_GenIoTimer[3] = 0;

    
    p_api_dev->ul_GenIo_TestSeq_Running = 1;
}

void api_gen_io_test_seq_stop(TY_API_DEV *p_api_dev)
{
    p_api_dev->ul_GenIo_TestSeq_Running = 0;


    p_api_dev->auc_SequenceActive[0] = 0;
    p_api_dev->auc_SequenceActive[1] = 0;
    p_api_dev->auc_SequenceActive[2] = 0;
    p_api_dev->auc_SequenceActive[3] = 0;


    p_api_dev->aul_CurrentSequenceEvent[0] = 0;
    p_api_dev->aul_CurrentSequenceEvent[1] = 0;
    p_api_dev->aul_CurrentSequenceEvent[2] = 0;
    p_api_dev->aul_CurrentSequenceEvent[3] = 0;
}


void api_gen_io_test_seq_setup(TY_API_DEV *p_api_dev, TY_API_GENIO_TEST_SEQ_TBL xSeqTbl )
{
    api_gen_io_test_seq_stop( p_api_dev );

    memcpy( &p_api_dev->xGenIoSeqTbl, &xSeqTbl, sizeof(TY_API_GENIO_TEST_SEQ_TBL) );


    if( (p_api_dev->ul_DbgLevel & DBG_GENIO) )
    {
        PRINTF2("Seq0 mode: %X, cond: 0x%08X\r\n", xSeqTbl.ax_Seq[0].mode, xSeqTbl.ax_Seq[0].cond );
        PRINTF2("Seq1 mode: %X, cond: 0x%08X\r\n", xSeqTbl.ax_Seq[1].mode, xSeqTbl.ax_Seq[1].cond );
        PRINTF2("Seq2 mode: %X, cond: 0x%08X\r\n", xSeqTbl.ax_Seq[2].mode, xSeqTbl.ax_Seq[2].cond );
        PRINTF2("Seq3 mode: %X, cond: 0x%08X\r\n", xSeqTbl.ax_Seq[3].mode, xSeqTbl.ax_Seq[3].cond );
    }

}


/*****************************************************************************/
/*    Descriptions                                                           */
/*    ------------                                                           */
/*    Inputs    :                                                            */
/*                                                                           */
/*    Outputs   :                                                            */
/*                                                                           */
/*    Description :                                                          */
/*                                                                           */
/*****************************************************************************/

L_WORD api_gen_io_test_seq(TY_API_DEV *p_api_dev, TY_MIL_COM_CUSTOM_DTS_GEN_IO_TEST_SEQ_INPUT * pxInput, TY_MIL_COM_CUSTOM_DTS_GEN_IO_TEST_SEQ_OUTPUT * pxOutput )
{
    /* -- set ack size --- */
    pxOutput->xAck.ulSize = sizeof(TY_MIL_COM_CUSTOM_DTS_GEN_IO_TEST_SEQ_OUTPUT);



    switch( pxInput->ul_Con )
    {
    case 0:  /* setup everything */
        api_gen_io_test_seq_setup( p_api_dev, pxInput->xSeqTbl );
        break;

    case 1:  /* start operation */
        api_gen_io_test_seq_start(p_api_dev);
        break;

    case 2:  /* stop operation */
        api_gen_io_test_seq_stop(p_api_dev);
        break;

    case 3:  /* return status */
        break;

    default:
        return API_ERR_INVALID_MODE;
    }



    pxOutput->aucSeqStatus[0] = p_api_dev->auc_SequenceActive[0];
    pxOutput->aucSeqStatus[1] = p_api_dev->auc_SequenceActive[1];
    pxOutput->aucSeqStatus[2] = p_api_dev->auc_SequenceActive[2];
    pxOutput->aucSeqStatus[3] = p_api_dev->auc_SequenceActive[3];

    return 0;
}




/*****************************************************************************/
/*                                                                           */
/*   Module : API_SYS                  Submodule : API_SYS_IP_ADDR_INIT      */
/*                                                                           */
/*   Author : Marco Motz               Project   : API1553-DS                */
/*                                                                           */
/*   Source : C                        Tools     : PC/AT; Norton Editor;     */
/*                                                 CYGNUS, GNU-C, As, and LD */
/*                                                 IDT-C 5.1 Toolkit         */
/*---------------------------------------------------------------------------*/
/*   Create : 22.12.08   Update : 22.12.08                                   */
/*---------------------------------------------------------------------------*/
/*    Descriptions                                                           */
/*    ------------                                                           */
/*    Inputs    : Industry Pack type [ip]                                    */
/*                Input/Output type [type]                                   */
/*                Channel select [chn]                                       */
/*                Spare value [res]                                          */
/*                Access address [addr]                                      */
/*                                                                           */
/*    Outputs   : Instruction acknowledge type [ackfl]                       */
/*                                                                           */
/*    Description :                                                          */
/*    This function handles the 'API_IP_MODULE_ADDR_INIT' instruction to     */
/*    initialise the specified address value for access to the selected IP.  */
/*                                                                           */
/*****************************************************************************/

BYTE api_sys_ip_addr_init(TY_API_DEV *p_api_dev, BYTE ip, BYTE type, BYTE chn, BYTE res, L_WORD addr)
{
BYTE ackfl;
BYTE ip_type, slot;

     ackfl = API_ACK;

#ifdef DEBUG_IP
     PRINTF3("api_sys_ip_addr_init: ip=%d res=%08x\r\n", ip, res, 0);
     PRINTF3("    Type=%d Chn=%d Addr=%08X\r\n", type, chn, addr);
#endif

     ip_type = ip & 0x0F;
     slot = ip >> 4; 

     if (ip_type == API_INIT_IP_TYPE) {
        /* init entire structure */
        api_sys_drv_glb_init();
     } 
     else if (slot < MAX_IP_DRV) {
       switch (ip_type) {

         case API_DIG_IP_TYPE:
              switch (type) {
                case 1: /* INPUT type */
                  ip_drv[slot].ip_setup.din.model   = chn;
                  ip_drv[slot].ip_setup.din.ip_addr = addr;
                  ip_drv[slot].ip_setup.din.sh_addr = (L_WORD)API_DATASET_MEM_ADDR_ABS(0/*dsid*/);
                  if (chn/*model*/ == 3)
                     ip_drv[slot].p_din_read_func = api_sys_drv_in_read_ip408;
                  if (chn/*model*/ == 8)
                     ip_drv[slot].p_din_read_func = api_sys_drv_in_read_ip470;
                  break;
                  
                case 2: /* OUTPUT type */
                  ip_drv[slot].ip_setup.dout.model   = chn;
                  ip_drv[slot].ip_setup.dout.ip_addr = addr;
                  ip_drv[slot].ip_setup.dout.sh_addr = (L_WORD)API_DATASET_MEM_ADDR_ABS(0/*dsid*/)+0x10L;
                  if (chn/*model*/ == 3) {
                     ip_drv[slot].p_dout_write_func = api_sys_drv_out_write_ip408;
                     ip_drv[slot].p_dout_read_func  = api_sys_drv_out_read_ip408;
                  }
                  if (chn/*model*/ == 8) {
                     ip_drv[slot].p_dout_write_func = api_sys_drv_out_write_ip470;
                     ip_drv[slot].p_dout_read_func  = api_sys_drv_out_read_ip470;
                  }
                  break;
           
                case 3: /* HW basic Init */
                  if (chn/*model*/ == 3)
                     ip_drv[slot].p_dhw_init_func = api_sys_drv_hw_init_ip408;
                  if (chn/*model*/ == 8) 
                     ip_drv[slot].p_dhw_init_func = api_sys_drv_hw_init_ip470;
                  if (ip_drv[slot].p_dhw_init_func != NULL) {
                      ip_drv[slot].p_dhw_init_func(slot,addr/*init-value*/,0,0,0,0);
                      ip_drv[slot].used = API_ON;
                      ip_drv[slot].ip_type = ip_type;
                  }
                  break;
           
                default:
                  ackfl = API_NAK;
                  break;
              }
              break;
              
         case API_ANA_IP_TYPE:
              switch (type) {
                case 1: /* INPUT type */
                  ip_drv[slot].ip_setup.din.model   = chn;
                  ip_drv[slot].ip_setup.din.ip_addr = addr;
                  ip_drv[slot].ip_setup.din.sh_addr = (L_WORD)API_DATASET_MEM_ADDR_ABS(0/*dsid*/);
                  ip_drv[slot].p_din_read_func = api_sys_drv_in_read_ip850;
                  break;
                  
                case 2: /* OUTPUT type */
                  ip_drv[slot].ip_setup.dout.model   = chn;
                  ip_drv[slot].ip_setup.dout.ip_addr = addr;
                  ip_drv[slot].ip_setup.dout.sh_addr = (L_WORD)API_DATASET_MEM_ADDR_ABS(0/*dsid*/) /*+0x10L*/;
                  ip_drv[slot].p_dout_write_func = api_sys_drv_out_write_ip850;
                  ip_drv[slot].p_dout_read_func  = api_sys_drv_out_read_ip850;
                  break;
           
                case 3: /* HW basic Init */
                  ip_drv[slot].p_dhw_init_func = api_sys_drv_hw_init_ip850;
                  if (ip_drv[slot].p_dhw_init_func != NULL) {
                      ip_drv[slot].p_dhw_init_func(slot,addr/*init-value*/,0,0,0,0);
                      ip_drv[slot].used = API_ON;
                      ip_drv[slot].ip_type = ip_type;
                  }
                  break;
           
                default:
                  ackfl = API_NAK;
                  break;
              }
              break;
              
         case API_APX_IP_TYPE:
              switch (type) {
                case 1: /* INPUT type */
                  ip_drv[slot].ip_setup.din.model = chn;
                  if (chn/*model*/ == 1)
                     ip_drv[slot].p_din_read_func = api_sys_drv_in_read_apx;
                  break;
                  
                case 2: /* OUTPUT type */
                  ip_drv[slot].ip_setup.dout.model = chn;
                  if (chn/*model*/ == 1) {
                     ip_drv[slot].p_dout_write_func = api_sys_drv_out_write_apx;
                     ip_drv[slot].p_dout_read_func  = api_sys_drv_out_read_apx;
                  }
                  break;
           
                case 3: /* HW basic Init */
                  if (chn/*model*/ == 1)
                     ip_drv[slot].p_dhw_init_func = api_sys_drv_hw_init_apx;
                  if (ip_drv[slot].p_dhw_init_func != NULL) {
                      ip_drv[slot].p_dhw_init_func(slot,addr/*init-value*/,0,0,0,0);
                      ip_drv[slot].used = API_ON;
                      ip_drv[slot].ip_type = ip_type;
                  }
                  break;
           
                default:
                  ackfl = API_NAK;
                  break;
              }
              break;
              
         default:
              break;
       }     
     }
     else
        ackfl = API_NAK;

     return(ackfl);

} /* end: api_sys_ip_addr_init */
/*****************************************************************************/
/*                                                                           */
/*   Module : API_SYS                  Submodule : API_SYS_IP_OUT_CHN_WRITE  */
/*                                                                           */
/*   Author : Marco Motz               Project   : API1553-DS                */
/*                                                                           */
/*   Source : C                        Tools     : PC/AT; Norton Editor;     */
/*                                                 CYGNUS, GNU-C, As, and LD */
/*                                                 IDT-C 5.1 Toolkit         */
/*---------------------------------------------------------------------------*/
/*   Create : 22.12.08   Update : 22.12.08                                   */
/*---------------------------------------------------------------------------*/
/*    Descriptions                                                           */
/*    ------------                                                           */
/*    Inputs    : Industry Pack type [ip]                                    */
/*                Channel select [chn]                                       */
/*                Spare value [res]                                          */
/*                Output value [val]                                         */
/*                                                                           */
/*    Outputs   : Instruction acknowledge type [ackfl]                       */
/*                                                                           */
/*    Description :                                                          */
/*    This function handles the 'API_IP_OUTPUT_CHN_WRITE' instruction to     */
/*    write the specified value to the selected IP output channel.           */
/*                                                                           */
/*****************************************************************************/

BYTE api_sys_ip_out_chn_write(TY_API_DEV *p_api_dev, BYTE ip, BYTE chn, L_WORD res, L_WORD val)
{
BYTE ackfl;
BYTE ip_type, slot;

#ifdef DEBUG_IP
     PRINTF3("api_sys_ip_out_chn_write: ip=%d\r\n", ip, 0, 0);
     PRINTF3("OUT-WRITE: Chn=%d Res=%d Val=%08X\r\n", chn, res, val);
#endif

     ackfl = API_ACK;

     ip_type = ip & 0x0F;
     slot = ip >> 4; 

     if ((ip_type != 0) && (slot < MAX_IP_DRV)) {
       if ((ip_drv[slot].used != 0) && (ip_drv[slot].p_dout_write_func != NULL)) {
            ip_drv[slot].p_dout_write_func(slot, chn, res, val, 0, 0);
       }
     } 
     else
       ackfl = API_NAK;

     return(ackfl);
     
} /* end: api_sys_ip_out_chn_write */

/*****************************************************************************/
/*                                                                           */
/*   Module : API_SYS                  Submodule : API_SYS_IP_OUT_CHN_READ   */
/*                                                                           */
/*   Author : Marco Motz               Project   : API1553-DS                */
/*                                                                           */
/*   Source : C                        Tools     : PC/AT; Norton Editor;     */
/*                                                 CYGNUS, GNU-C, As, and LD */
/*                                                 IDT-C 5.1 Toolkit         */
/*---------------------------------------------------------------------------*/
/*   Create : 22.12.08   Update : 22.12.08                                   */
/*---------------------------------------------------------------------------*/
/*    Descriptions                                                           */
/*    ------------                                                           */
/*    Inputs    : Industry Pack type [ip]                                    */
/*                Channel select [chn]                                       */
/*                Spare value [res]                                          */
/*                                                                           */
/*    Outputs   : Instruction acknowledge type [ackfl]                       */
/*                Input value [val]                                          */
/*                                                                           */
/*    Description :                                                          */
/*    This function handles the 'API_IP_OUT_CHN_READ' instruction to read-   */
/*    back the selected IP output channel.                                   */
/*                                                                           */
/*****************************************************************************/

BYTE api_sys_ip_out_chn_read(TY_API_DEV *p_api_dev,BYTE ip, BYTE chn, L_WORD res)
{
BYTE ackfl;
BYTE ip_type, slot;
L_WORD tmpl;

#ifdef DEBUG_IP
     PRINTF3("api_sys_ip_out_chn_read: ip=%d\r\n", ip, 0, 0);
     PRINTF3("OUT-READ: Chn=%d Res=%d\r\n", chn, res, 0);
#endif

     tmpl = 0L;
     ackfl = API_ACK2;

     ip_type = ip & 0x0F;
     slot = ip >> 4; 

     if ((ip_type != 0) && (slot < MAX_IP_DRV)) {
       if ((ip_drv[slot].used != 0) && (ip_drv[slot].p_dout_read_func != NULL)) {
            ip_drv[slot].p_dout_read_func(slot, chn, 1/* read from value*/, 0, &tmpl, 0);
       }
     } 
      else
       ackfl = API_NAK;

     p_api_dev->lword_data_arr[0] = tmpl;
     p_api_dev->b_length   = 0;
     p_api_dev->w_length   = 0;
     p_api_dev->lw_length  = 1;  
     p_api_dev->ack_lw_ptr = &(p_api_dev->lword_data_arr[0]);
     
     return(ackfl);
     
} /* end: api_sys_ip_out_chn_read */

/*****************************************************************************/
/*                                                                           */
/*   Module : API_SYS                  Submodule : API_SYS_IP_IN_CHN_READ    */
/*                                                                           */
/*   Author : Marco Motz               Project   : API1553-DS                */
/*                                                                           */
/*   Source : C                        Tools     : PC/AT; Norton Editor;     */
/*                                                 CYGNUS, GNU-C, As, and LD */
/*                                                 IDT-C 5.1 Toolkit         */
/*---------------------------------------------------------------------------*/
/*   Create : 22.12.08   Update : 22.12.08                                   */
/*---------------------------------------------------------------------------*/
/*    Descriptions                                                           */
/*    ------------                                                           */
/*    Inputs    : Industry Pack type [ip]                                    */
/*                Channel select [chn]                                       */
/*                Spare value [res]                                          */
/*                                                                           */
/*    Outputs   : Instruction acknowledge type [ackfl]                       */
/*                Input value [val]                                          */
/*                                                                           */
/*    Description :                                                          */
/*    This function handles the 'API_IP_IN_CHN_READ' instruction to read     */
/*    the selected IP input channel.                                         */
/*                                                                           */
/*****************************************************************************/

BYTE api_sys_ip_in_chn_read(TY_API_DEV *p_api_dev,BYTE ip, BYTE chn, L_WORD res)
{
BYTE ackfl;
BYTE ip_type, slot;
L_WORD tmpl;

#ifdef DEBUG_IP
     PRINTF3("api_sys_ip_in_chn_read: ip=%d\r\n", ip, 0, 0);
     PRINTF3("IN-READ: Chn=%d Res=%d\r\n", chn, res, 0);
#endif

     tmpl = 0L;
     ackfl = API_ACK2;

     ip_type = ip & 0x0F;
     slot = ip >> 4; 

     if ((ip_type != 0) && (slot < MAX_IP_DRV)) {
       if ((ip_drv[slot].used != 0) && (ip_drv[slot].p_din_read_func != NULL)) {
            ip_drv[slot].p_din_read_func(slot, chn, 1/* read from value not from HW*/, 0, &tmpl, 0);
       }
     }
     else
       ackfl = API_NAK;
      
     p_api_dev->lword_data_arr[0] = tmpl;
     p_api_dev->b_length   = 0;
     p_api_dev->w_length   = 0;
     p_api_dev->lw_length  = 1;  
     p_api_dev->ack_lw_ptr = &(p_api_dev->lword_data_arr[0]);

     return(ackfl);
     
} /* end: api_sys_ip_in_chn_read */

/*****************************************************************************/
/*                                                                           */
/*   Module : API_SYS                  Submodule : API_SYS_IP_STASK_CTRL     */
/*                                                                           */
/*   Author : Marco Motz               Project   : API1553-DS                */
/*                                                                           */
/*   Source : C                        Tools     : PC/AT; Norton Editor;     */
/*                                                 CYGNUS, GNU-C, As, and LD */
/*                                                 IDT-C 5.1 Toolkit         */
/*---------------------------------------------------------------------------*/
/*   Create : 22.12.08   Update : 18.02.09                                   */
/*---------------------------------------------------------------------------*/
/*    Descriptions                                                           */
/*    ------------                                                           */
/*    Inputs    : Sampling Task for IP [ip]                                  */
/*                Enable/Disable timer [con]                                 */
/*                Sampling rate [rate]                                       */
/*                                                                           */
/*    Outputs   : Instruction acknowledge type [ackfl]                       */
/*                                                                           */
/*    Description :                                                          */
/*    This function handles the 'API_IP_STASK_CTRL' instruction to control   */
/*    the sampling operating for the specified timer number.                 */
/*                                                                           */
/*****************************************************************************/

BYTE api_sys_ip_stask_ctrl(TY_API_DEV *p_api_dev, BYTE ip, BYTE con, 
                           L_WORD rate, L_WORD res1, L_WORD ustep)
{
BYTE ackfl;
BYTE ip_type, slot;

#ifdef DEBUG_IP
     printf("api_sys_ip_stask_ctrl: ip=%d con=%d rate=%d ustep=%d\r\n",ip,con,rate,ustep);
#endif

     ackfl = API_ACK;

     ip_type = ip & 0x0F;
     slot = ip >> 4; 

     if ((ip_type != 0) && (slot < MAX_IP_DRV)) {
        if (ip_drv[slot].used != 0) {
           if (con == API_OFF) {
              /* Disable */
#ifdef DEBUG_IP
              printf("Disable Sampling %d\r\n",ip);
#endif              
              ip_drv[slot].stask_setup.ena     = API_OFF;
              ip_drv[slot].stask_setup.rate    = 0;
              ip_drv[slot].stask_setup.ustep   = 0;
              ip_drv[slot].stask_setup.isr_cnt = 0L;
           }   
           else if (con == API_ON) {
#ifdef DEBUG_IP
              printf("Enable Sampling %d \r\n",ip);
#endif
              ip_drv[slot].rec_status.entry_cnt = 0L; /* record first entry after start */
              ip_drv[slot].stask_setup.isr_cnt  = 0L;
              ip_drv[slot].stask_setup.rate     = rate;
              ip_drv[slot].stask_setup.ustep    = ustep;
              ip_drv[slot].stask_setup.ena      = API_ON;
           }
           else if (con == 3/*API_ON*/) {
              if (ip_drv[slot].stask_setup.ena & 0x01) {
                 if (res1 != 0) {
                    ip_drv[slot].stask_setup.ena |= ((res1 << 4) & 0xF0);
                 }   
                 else {
                    ip_drv[slot].stask_setup.ena &= 0x0F;
                 }
              }   
           }
        }
     }
     else
       ackfl = API_NAK;

     return(ackfl);    

} /* end: api_sys_ip_stask_ctrl */

#endif  /* _CMPL4DTS */




BYTE api_sys_dts(TY_API_DEV *p_api_dev, L_WORD cmd, WORD size, BYTE *info_p)
{
  BYTE ackfl;

  ackfl = API_NAK;

  switch(cmd)
  {

#if defined(_CMPL4DTS)
    /* cases from api_sys.c */
    case API_IP_MODULE_ADDR_INIT:
      if (size == 12)
         ackfl = api_sys_ip_addr_init(p_api_dev,
                                      *(info_p+0), *(info_p+1), 
                                      *(info_p+2), *(info_p+3), 
                         MAKE_B2LWORD(*(info_p+4), *(info_p+5),
                                      *(info_p+6), *(info_p+7)));
      break;

    case API_IP_OUTPUT_CHN_WRITE:
      if (size == 14)
         ackfl = api_sys_ip_out_chn_write(p_api_dev,
                                      *(info_p+0), *(info_p+1), 
                         MAKE_B2LWORD(*(info_p+2), *(info_p+3),
                                      *(info_p+4), *(info_p+5)),
                         MAKE_B2LWORD(*(info_p+6), *(info_p+7),
                                      *(info_p+8), *(info_p+9)));
      break;

    case API_IP_OUTPUT_CHN_READ:
      if (size == 10)
         ackfl = api_sys_ip_out_chn_read(p_api_dev,
                                      *(info_p+0), *(info_p+1), 
                         MAKE_B2LWORD(*(info_p+2), *(info_p+3),
                                      *(info_p+4), *(info_p+5)));
      break;

    case API_IP_INPUT_CHN_READ:
      if (size == 10)
         ackfl = api_sys_ip_in_chn_read(p_api_dev,
                                      *(info_p+0), *(info_p+1),  
                         MAKE_B2LWORD(*(info_p+2), *(info_p+3),
                                      *(info_p+4), *(info_p+5)));
      break;

    case API_IP_STASK_CTRL:
      if (size == 18)
         ackfl = api_sys_ip_stask_ctrl(p_api_dev,
                                      *(info_p+0), *(info_p+1), 
                         MAKE_B2LWORD(*(info_p+2), *(info_p+3),
                                      *(info_p+4), *(info_p+5)),
                         MAKE_B2LWORD(*(info_p+6), *(info_p+7),
                                      *(info_p+8), *(info_p+9)),
                         MAKE_B2LWORD(*(info_p+10),*(info_p+11),
                                      *(info_p+12),*(info_p+13)));
      break;





    /* cases from i_x_sys.c */

    case API_GEN_IO_ADDR_INIT:
      if (size == 15)
         ackfl = api_sys_ip_addr_init(p_api_dev,
                                      *(info_p+0), *(info_p+1), 
                                      *(info_p+2), *(info_p+3), 
                         MAKE_B2LWORD(*(info_p+4), *(info_p+5),
                                      *(info_p+6), *(info_p+7)));
      break;

    case API_GEN_IO_OUT_CHN_WRITE:
      if (size == 17)
         ackfl = api_sys_ip_out_chn_write(p_api_dev,
                                      *(info_p+0), *(info_p+1), 
                         MAKE_B2LWORD(*(info_p+2), *(info_p+3),
                                      *(info_p+4), *(info_p+5)),
                         MAKE_B2LWORD(*(info_p+6), *(info_p+7),
                                      *(info_p+8), *(info_p+9)));
      break;

    case API_GEN_IO_IN_CHN_READ:
      if (size == 13)
         ackfl = api_sys_ip_in_chn_read(p_api_dev,
                                      *(info_p+0), *(info_p+1),  
                         MAKE_B2LWORD(*(info_p+2), *(info_p+3),
                                      *(info_p+4), *(info_p+5)));
      break;

    case API_GEN_IO_STASK_CTRL:
      if (size == 21)
         ackfl = api_sys_ip_stask_ctrl(p_api_dev,
                                      *(info_p+0), *(info_p+1), 
                         MAKE_B2LWORD(*(info_p+2), *(info_p+3),
                                      *(info_p+4), *(info_p+5)),
                         MAKE_B2LWORD(*(info_p+6), *(info_p+7),
                                      *(info_p+8), *(info_p+9)),
                         MAKE_B2LWORD(*(info_p+10),*(info_p+11),
                                      *(info_p+12),*(info_p+13)));
      break;

      case API_GEN_IO_READ:
      if (size == 15)
         ackfl = api_sys_gen_io_read(p_api_dev,
                         MAKE_B2LWORD(*(info_p+0), *(info_p+1), 
                                      *(info_p+2), *(info_p+3)), 
                         MAKE_B2LWORD(*(info_p+4), *(info_p+5),
                                      *(info_p+6), *(info_p+7)));
      break;

    case API_GEN_IO_OUT_CHN_READ:
      if (size == 13)
         ackfl = api_sys_ip_out_chn_read(p_api_dev,
                                      *(info_p+0), *(info_p+1),  
                         MAKE_B2LWORD(*(info_p+2), *(info_p+3),
                                      *(info_p+4), *(info_p+5)));
      break;
      case API_GEN_IO_GET_BOARDS:
	  break;

#endif

    default:
	  PRINTF2("no command found cmd [%X] size [%d]",cmd, size);
      break;
  }

  return (ackfl);

} /* end: api_sys_dts */
