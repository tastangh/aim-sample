/* SPDX-FileCopyrightText: 2002-2021 AIM GmbH <info@aim-online.com> */
/* SPDX-License-Identifier: MIT OR GPL-2.0-or-later */

#if defined (__monitor__)
  #ifndef _CRT_SECURE_NO_WARNINGS
    #define _CRT_SECURE_NO_WARNINGS /* warnings to use strcpy_s/scanf_s instead of  strcpy/scanf ML 27/12/2011 */
  #endif
  #include "MonSpecific.h"
#elif defined (_SMARTLIB)
  #include "stdio.h"
  #if defined _AIM_WINDOWS
    #include "Windows.h"
  #endif
#endif

#include "defs.h"
#include "type_def.h"
#include "prototypes.h"



/*****************************************************************************/
/*                                                                           */
/*   Module : mon_wait.c               Submodule : wait_clocks               */
/*   Author : Lamp Matthias            Project   : PMC-1553-MON              */
/*---------------------------------------------------------------------------*/
/*   Create : 09.05.2001    Update :   01.07.05   (FDXTAP timing)            */
/*---------------------------------------------------------------------------*/
/*    Descriptions                                                           */
/*    ------------                                                           */
/*    Inputs    :    u_sec                                                   */
/*                                                                           */
/*    Outputs   :    void                                                    */
/*                                                                           */
/*    Description :  waits a number of us (max 1s)                           */
/*                   by the counter (1MHz) in the PCI FPGA                   */
/*****************************************************************************/



AiUInt32 wait_clocks(TY_MONITOR_DATA_STRUCT *GLOBAL_VAR, AiUInt32 u_sec)
{
    AiUInt32 first_counter, second_counter;
    AiUInt32 x;

#if defined(_AIM_SYSDRV) && defined(__LYNXOS)  /* TAP yet not used with LynxOS */

#else
    if( GLOBAL_VAR->STATUS.argv2 == 5 ) /*FDX TAP case*/
    {
        if( u_sec < 500 )
            return (PASS);
        else
            AIM_WAIT(u_sec / 1000); /*vorlaeufig fuer Marco 29/06/05 ML*/
    }
    else

#endif

    {
        first_counter = PCI_IOReg_read(GLOBAL_VAR, PMC_COUNTER_Reg_Adr);
        x = 0;
        do
        {
            second_counter = PCI_IOReg_read(GLOBAL_VAR, PMC_COUNTER_Reg_Adr);

            if( second_counter == first_counter ) /*reads the same value, ERROR*/
                x++; /* lets say if you read 100 times the same vale, */
            else
                /*  there should be something wrong*/
                x = 0; /* otherwise OK*/

            if( (second_counter - first_counter) >= u_sec )
            {
                x = 100;
                break;
            }

        } while( x != 100 );
    }
    return (PASS);
}

