/* SPDX-FileCopyrightText: 2001-2024 AIM GmbH <info@aim-online.com> */
/* SPDX-License-Identifier: MIT OR GPL-2.0-or-later */

#if defined (__monitor__)
  #ifndef _CRT_SECURE_NO_WARNINGS
    #define _CRT_SECURE_NO_WARNINGS // warnings to use strcpy_s/scanf_s instead of  strcpy/scanf ML 27/12/2011
  #endif
  #include "MonSpecific.h"
  #include "defs.h"
  #include "type_def.h"
  #include "prototypes.h"
#elif defined (_SMARTLIB)
  #include "stdio.h"
  #include "stdarg.h"
  #include "string.h"
  #include "MonSpecific.h"
  #include "defs.h"
  #include "type_def.h"
  #include "prototypes.h"
  #include "Wrapper.h"  /* needed for v_SLLogFile */
  #include "SmartLib.h" /* needed for v_SLLogFile */
#else
  #ifdef _AIM_VMEWINDOWS
    #include <windows.h>
    #define API_OK 0
  #endif 
  #include "defs.h"
  #include "type_def.h"
  #include "prototypes.h"
  #if defined (_AIM_VME)
    #if defined (_AIM_FDX)
      #ifdef _AIM_VMEWINDOWS
        _API_EXTERN AiUInt32 ul_FdxDbgLevel;
      #endif 
      #include <stdio.h>
      #include <stdlib.h>
      #include <string.h>
      #include "../AiFdx_intern.h"
      #define API_OK 0
      #if defined(_AIM_SYSDRV)     /* SYSDRV for LYNXOS */
        #include "../generic.h"
        #include "../aim.h"
      #endif 
    #elif defined (_AIM_1553)
      #include <stdlib.h>
      #include <stdio.h>
      #include <string.h>
      #include "Ai1553i_def.h"
      #include "DEFS_V.H"
    #elif defined (_AMC429)
      #include <stdlib.h>
      #include <stdio.h>
      #include <string.h>
      #include "Api429.h"
      #include "A429_defs_v.h"
    #endif 
  #elif defined (_AIM_WINDOWS)   /* system driver*/
    #include <stddcls.h>
    #include <stdlib.h>
    #include <string.h>

    #include <stdio.h>
    #include <stdarg.h>
  
    #include ".\..\..\..\include\api_defs.h" 
    #include ".\..\..\..\include\api_defv.h"
    #include ".\..\..\..\include\fileio.h"
    #include ".\..\..\..\include\perf.h"
  #elif defined (_AIM_LINUX)   /* added for LINUX system driver*/
    #include <linux/string.h>
    #define API_OK 0
  #elif defined (_AIM_VXI)
    #if defined (_AIM_1553)
      #include "ai1553axi.h"
      #include <stdio.h> 
      #include "defs.h"
      #include "ai1553_version.h"
      extern AiUInt32 Aiul_DbgLevel;
    #elif defined (_AMC429) /*Path for LabView RT for AMC429*/
      #include <stdio.h>
      #include <stdlib.h>
      #include "Api429.h"
      #include "A429_defs_v.h"
      _API_EXTERN AiUInt32 Aiul_DbgLevel;
    #else 
      #include <stdio.h>
      #include <stdlib.h>
      #include ".\..\AiFdx_intern.h"
      #define API_OK 0
      AiUInt32 ul_FdxDbgLevel;
    #endif 
  #endif 
#endif


extern void ShowBuffer(TY_MONITOR_DATA_STRUCT *GLOBAL_VAR, AiUInt32 buf, AiUInt32 count, AiUInt32 type);

#define MAXLINE 960

#if defined( _AIM_SYSDRV) && !defined(_AIM_LINUX)
   #include <AimPciProDefines.h>
   #include "prog_misc.tmh"
#endif

/*****************************************************************************/
/*                                                                           */
/*   Module : prog_misc.c              Submodule : AI_priintf                */
/*   Author : Rolf Heitzmann           Project   : PMC-1553-MON              */
/*---------------------------------------------------------------------------*/
/*   Create : 04.09.2013   Update :                                          */
/*---------------------------------------------------------------------------*/
/*    Descriptions                                                           */
/*    ------------                                                           */
/*    Inputs    :                                                            */
/*                                                                           */
/*    Outputs   :                                                            */
/*                                                                           */
/*    Description : Printf Output for different operating systems,           */
/*                  reduces the use of #ifdef ...                            */
/*                                                                           */
/*****************************************************************************/
int AI_Printf(const char * format, ...)
{
    char outbuf[MAXLINE];
    int i;

    va_list args;
    va_start (args, format);
    i = vsprintf(outbuf, format, args);
    va_end (args);

#ifdef __monitor__
    printf("%s", outbuf);
#elif defined _SMARTLIB
    v_SLLogFile(outbuf, 1, 1);
#elif defined DBG
    MonPrintf ( LOGFILE, DBG_ERROR, FKT_MON, (outbuf));
#elif defined(_AIM_SYSDRV) && defined(_AIM_WINDOWS)
    DoTraceMessage(AIMPCI_TRACE_MONITOR, "%s", outbuf);
#elif defined(_AIM_SYSDRV) && defined(_AIM_LINUX)
    printk("%s", outbuf);
#endif
    return i;
}

int MonPrintBatchCondition(TY_MONITOR_DATA_STRUCT *GLOBAL_VAR, const char * format, ...)
{
#ifdef __monitor__
    char outbuf[MAXLINE];
    int i = 0;

    if( GLOBAL_VAR->STATUS_FLAGS.Reg.BATCH_MODE == 0 )
    {
        va_list args;
        va_start (args, format);
        i = vsprintf(outbuf, format, args);
        va_end (args);

        printf("%s", outbuf);
    }
    return i;
#else 
    return 0;
#endif
}

/*****************************************************************************/
/*                                                                           */
/*   Module : prog_misc.c              Submodule : init_novram               */
/*   Author : Lamp Matthias            Project   : PMC-1553-MON              */
/*---------------------------------------------------------------------------*/
/*   Create : 22.03.2001   Update :                                          */
/*---------------------------------------------------------------------------*/
/*    Descriptions                                                           */
/*    ------------                                                           */
/*    Inputs    : -                                                          */
/*                                                                           */
/*    Outputs   : PASS/FAIL                                                  */
/*                                                                           */
/*    Description : Initializes the NOVRam (First Board Start).              */
/*                                                                           */
/*****************************************************************************/
AiUInt32 init_novram(TY_MONITOR_DATA_STRUCT *GLOBAL_VAR)
{
    AiUInt32 i;

    /* Probe NOVRam before using it */
    for( i = 0; i < 128; i++ )
    /*128 max NOVRAM words (as AiUInt32)*/
    {
        if( NOVRam_write(GLOBAL_VAR, i * 4, 0) == FAIL )
            return (FAIL);
    }
    if( NOVRam_chksum(GLOBAL_VAR) != 0 )
        return (FAIL);

    MON_ONLY_PRINTF("\npass chksum 0");

    for( i = 0; i < 128; i++ )
    {
        if( NOVRam_write(GLOBAL_VAR, i * 4, 0xffffffff) == FAIL )
            return (FAIL);
    }
    if( NOVRam_chksum(GLOBAL_VAR) != 0xffffff81 )
        return (FAIL);

    /* All unused locations are zero */
    MON_ONLY_PRINTF("\npass chksum 0xffffff81");

    for( i = 0; i < 128; i++ )
    {
        if( NOVRam_write(GLOBAL_VAR, i * 4, 0) == FAIL )
            return (FAIL);
    }

    if( NOVRam_chksum(GLOBAL_VAR) != 0 )
        return (FAIL);

    MON_ONLY_PRINTF("\npass chksum 0");
    /* write start values*/

    GLOBAL_VAR->NVR_BOARD_BUILD.all = 0x26022002; /*first build date*/
    GLOBAL_VAR->NVR_BOARD_SERIAL.Reg.SERIAL = 9999; /*board serieal = 9999*/
    /* enable all read and write functions, lock enable bit*/

    GLOBAL_VAR->NVR_PROG_EN_Reg.all = 0xffffffff;

    if( put_novram_setup(GLOBAL_VAR) == PASS )
    {
        MON_ONLY_PRINTF("\npass put_novram_setup");
        /*    if (NOVRam_write(GLOBAL_VAR, GLOBAL_VAR->NVR_MAGIC2, NVR_MAGIC_def)==PASS)*//* NOVRAM Magic 2*/

        /*   hier ueber die register gehen  */
        GLOBAL_VAR->NVR_MAGIC = NVR_MAGIC_def;
        GLOBAL_VAR->NVR_MAGIC2 = NVR_MAGIC_def;

        if( (NOVRam_write(GLOBAL_VAR, NVR_MAGIC_Adr, GLOBAL_VAR->NVR_MAGIC) == PASS) && /* NOVRAM Magic 1*/
        (NOVRam_write(GLOBAL_VAR, NVR_MAGIC2_Adr, GLOBAL_VAR->NVR_MAGIC2) == PASS) )
        /* NOVRAM Magic 2*/
        {
            GLOBAL_VAR->NVR_CHECKSUM = NOVRam_chksum(GLOBAL_VAR);
            if( NOVRam_write(GLOBAL_VAR, NVR_CHECKSUM_Adr, GLOBAL_VAR->NVR_CHECKSUM) == PASS )
                return (PASS);

            MON_ONLY_PRINTF("\nERROR writing checksum");
            return (FAIL);
        }

        MON_ONLY_PRINTF("\nERROR writing NOVRAM MAGIC");
        return (FAIL);
    }
    return (FAIL);
}

/*****************************************************************************/
/*                                                                           */
/*   Module : prog_misc.c              Submodule : put_novram_setup          */
/*   Author : Matthias Lamp            Project   : API-ASP-MON               */
/*---------------------------------------------------------------------------*/
/*   Create : 10/10/2001   Update :     11/02/03                             */
/*---------------------------------------------------------------------------*/
/*    Descriptions                                                           */
/*    ------------                                                           */
/*    Inputs    : -                                                          */
/*                                                                           */
/*    Outputs   : PASS/FAIL                                                  */
/*                                                                           */
/*    Description : Writes all parameters to NOV_RAM.                        */
/*                                                                           */
/*****************************************************************************/
AiUInt32 put_novram_setup(TY_MONITOR_DATA_STRUCT *GLOBAL_VAR)
{
    if( (NOVRam_write(GLOBAL_VAR, NVR_MAGIC_Adr, GLOBAL_VAR->NVR_MAGIC) == PASS)
                && (NOVRam_write(GLOBAL_VAR, NVR_BOARD_SERIAL_Adr, GLOBAL_VAR->NVR_BOARD_SERIAL.all) == PASS)
                && (NOVRam_write(GLOBAL_VAR, NVR_BOARD_CONFIG_Adr, GLOBAL_VAR->NVR_BOARD_CONFIG.all) == PASS)
                && (NOVRam_write(GLOBAL_VAR, NVR_BOARD_TYPE_Adr, GLOBAL_VAR->NVR_BOARD_TYPE.all) == PASS)
                && (NOVRam_write(GLOBAL_VAR, NVR_NVRAM_ENABLE_Adr, GLOBAL_VAR->NVR_NVRAM_ENABLE) == PASS)
                && (NOVRam_write(GLOBAL_VAR, NVR_BOARD_BUILD_Adr, GLOBAL_VAR->NVR_BOARD_BUILD.all) == PASS)
                && (NOVRam_write(GLOBAL_VAR, NVR_NVRAM_CHANGE_Adr, GLOBAL_VAR->NVR_NVRAM_CHANGE) == PASS)
                && (NOVRam_write(GLOBAL_VAR, NVR_BOARD_PARTNO_Adr, GLOBAL_VAR->NVR_BOARD_PARTNO.all) == PASS)
                && (NOVRam_write(GLOBAL_VAR, NVR_SYSCLK_Adr, GLOBAL_VAR->NVR_SYSCLK.all) == PASS)
                && (NOVRam_write(GLOBAL_VAR, NVR_CPUCLK_Adr, GLOBAL_VAR->NVR_CPUCLK.all) == PASS)
                && (NOVRam_write(GLOBAL_VAR, NVR_SW_KEY_Adr, GLOBAL_VAR->NVR_SW_KEY) == PASS)
                && (NOVRam_write(GLOBAL_VAR, NVR_BOOT_PROG_Adr, GLOBAL_VAR->NVR_PROG_EN_Reg.all) == PASS)
                && (NOVRam_write(GLOBAL_VAR, NVR_GRAM_Size_Adr, GLOBAL_VAR->NVR_GRAM_SIZE.all) == PASS)
                && (NOVRam_write(GLOBAL_VAR, NVR_LRAM_Size_Adr, GLOBAL_VAR->NVR_LRAM_SIZE.all) == PASS)
                && (NOVRam_write(GLOBAL_VAR, NVR_MAGIC2_Adr, GLOBAL_VAR->NVR_MAGIC2) == PASS)
                && (NOVRam_write(GLOBAL_VAR, NVR_CHECKSUM_Adr, NOVRam_chksum(GLOBAL_VAR)) == PASS) )
        return (PASS);
    else
        return (FAIL);
}

/*****************************************************************************/
/*                                                                           */
/*   Module : ini_prog.c               Submodule : get_novram_setup          */
/*   Author : Hans Zogalla             Project   : API-ASP-MON               */
/*---------------------------------------------------------------------------*/
/*   Create : 10/10/2001   Update :   11/02/03, 22.02.07  16.10.07           */
/*---------------------------------------------------------------------------*/
/*    Descriptions                                                           */
/*    ------------                                                           */
/*    Inputs    : -                                                          */
/*                                                                           */
/*    Outputs   : PASS/FAIL                                                  */
/*                                                                           */
/*    Description : Reads all used parameters from NOV_RAM, and writes it    */
/*                  to the GLOBAL STRUCT, add two new NOVRAM values          */
/*                    22.02.07 new Novram value IRIG_OFFSET                  */
/*                  16.10.07 2 new NVR values NVR_PART_EXT, NVR_PCI_HDR      */
/*                                                                           */
/*****************************************************************************/
static AiUInt32 get_novram_setup(TY_MONITOR_DATA_STRUCT *GLOBAL_VAR)
{
    if( GLOBAL_VAR->NVR_BOARD_SERIAL.Reg.SERIAL != 9999 )
        GLOBAL_VAR->NVR_BOARD_SERIAL.all = NOVRam_read(GLOBAL_VAR,
        NVR_BOARD_SERIAL_Adr);

    GLOBAL_VAR->NVR_MAGIC = NOVRam_read(GLOBAL_VAR, NVR_MAGIC_Adr);
    GLOBAL_VAR->NVR_BOARD_CONFIG.all = NOVRam_read(GLOBAL_VAR,
    NVR_BOARD_CONFIG_Adr);
    GLOBAL_VAR->NVR_BOARD_TYPE.all = NOVRam_read(GLOBAL_VAR,
    NVR_BOARD_TYPE_Adr);
    GLOBAL_VAR->NVR_NVRAM_ENABLE = NOVRam_read(GLOBAL_VAR,
    NVR_NVRAM_ENABLE_Adr);
    GLOBAL_VAR->NVR_BOARD_BUILD.all = NOVRam_read(GLOBAL_VAR,
    NVR_BOARD_BUILD_Adr);
    GLOBAL_VAR->NVR_NVRAM_CHANGE = NOVRam_read(GLOBAL_VAR,
    NVR_NVRAM_CHANGE_Adr);
    GLOBAL_VAR->NVR_BOARD_PARTNO.all = NOVRam_read(GLOBAL_VAR,
    NVR_BOARD_PARTNO_Adr);
    GLOBAL_VAR->NVR_SYSCLK.all = NOVRam_read(GLOBAL_VAR, NVR_SYSCLK_Adr);
    GLOBAL_VAR->NVR_CPUCLK.all = NOVRam_read(GLOBAL_VAR, NVR_CPUCLK_Adr);
    GLOBAL_VAR->NVR_SW_KEY = NOVRam_read(GLOBAL_VAR, NVR_SW_KEY_Adr);
    GLOBAL_VAR->NVR_PROG_EN_Reg.all = NOVRam_read(GLOBAL_VAR,
    NVR_BOOT_PROG_Adr);

    GLOBAL_VAR->NVR_GRAM_SIZE.all = NOVRam_read(GLOBAL_VAR, NVR_GRAM_Size_Adr);
    GLOBAL_VAR->NVR_LRAM_SIZE.all = NOVRam_read(GLOBAL_VAR, NVR_LRAM_Size_Adr);
    /*V02.00 23.02.06*/
    GLOBAL_VAR->NVR_PLCAEX.all = NOVRam_read(GLOBAL_VAR, NVR_PLCAEX_A);
    GLOBAL_VAR->NVR_TCPEX.all = NOVRam_read(GLOBAL_VAR, NVR_TCPEX_A);
    GLOBAL_VAR->NVR_ILCA1EX.all = NOVRam_read(GLOBAL_VAR, NVR_ILCA1EX_A);
    GLOBAL_VAR->NVR_ILCA2EX.all = NOVRam_read(GLOBAL_VAR, NVR_ILCA2EX_A);
    GLOBAL_VAR->NVR_ILCA3EX.all = NOVRam_read(GLOBAL_VAR, NVR_ILCA3EX_A);
    GLOBAL_VAR->NVR_ILCA4EX.all = NOVRam_read(GLOBAL_VAR, NVR_ILCA4EX_A);
    GLOBAL_VAR->NVR_BIP1EX.all = NOVRam_read(GLOBAL_VAR, NVR_BIP1EX_A);
    GLOBAL_VAR->NVR_BIP2EX.all = NOVRam_read(GLOBAL_VAR, NVR_BIP2EX_A);
    GLOBAL_VAR->NVR_BIP3EX.all = NOVRam_read(GLOBAL_VAR, NVR_BIP3EX_A);
    GLOBAL_VAR->NVR_BIP4EX.all = NOVRam_read(GLOBAL_VAR, NVR_BIP4EX_A);
    GLOBAL_VAR->NVR_ASPEX.all = NOVRam_read(GLOBAL_VAR, NVR_ASPEX_A);
    /*V02.12*/
    GLOBAL_VAR->NVR_IRIG_OFFSET.all = NOVRam_read(GLOBAL_VAR,
    NVR_IRIG_OFFSET_Adr);

    GLOBAL_VAR->NVR_BOARD_ST.all = NOVRam_read(GLOBAL_VAR, NVR_BOARD_ST_Adr);
    GLOBAL_VAR->NVR_HW_VAR.all = NOVRam_read(GLOBAL_VAR, NVR_HW_VAR_Adr);
    /*V02.13 16.10.07*/
    GLOBAL_VAR->NVR_PART_EXT = NOVRam_read(GLOBAL_VAR, NVR_PART_EXT_Adr);
    GLOBAL_VAR->NVR_PCI_HDR.all = NOVRam_read(GLOBAL_VAR, NVR_PCI_HDR_Adr);
    /*end*/
    GLOBAL_VAR->NVR_MAGIC2 = NOVRam_read(GLOBAL_VAR, NVR_MAGIC2_Adr);
    GLOBAL_VAR->NVR_CHECKSUM = NOVRam_read(GLOBAL_VAR, NVR_CHECKSUM_Adr);

    if( (GLOBAL_VAR->PMC_DEVICEID_Reg.Reg.device_id == dveID_PMC32_ASP) || (GLOBAL_VAR->PMC_DEVICEID_Reg.Reg.device_id == dveID_PMC64_ASP) )
    {

        MON_ONLY_PRINTF("\nAMC Module with ASP"); MON_ONLY_PRINTF("\nget GRAM and LRAM size from NOVRAM");

        MON_ONLY_PRINTF("\n\nRAM size from 'get RAM size'"); MON_ONLY_PRINTF("\nGLOBAL RAM Size: %3dMB, ",
                    GLOBAL_VAR->RAM_Size_Reg.Reg.GRam_Size); MON_ONLY_PRINTF("\nLocal ASP RAM Size: %3dMB",
                    GLOBAL_VAR->RAM_Size_Reg.Reg.Local_ASPRam);

        MON_ONLY_PRINTF("\n\nRAM size from TCP NOVRAM"); MON_ONLY_PRINTF("\nGLOBAL RAM Size: %3dMB, ",
                    GLOBAL_VAR->NVR_GRAM_SIZE.Reg.GRAM_Size); MON_ONLY_PRINTF("\nLocal ASP RAM Size: %3dMB",
                    GLOBAL_VAR->NVR_LRAM_SIZE.Reg.LRAM_Size);

        GLOBAL_VAR->RAM_Size_Reg.Reg.Local_ASPRam = GLOBAL_VAR->NVR_LRAM_SIZE.Reg.LRAM_Size;
        GLOBAL_VAR->RAM_Size_Reg.Reg.GRam_Size = GLOBAL_VAR->NVR_GRAM_SIZE.Reg.GRAM_Size;
    }
    return (PASS);
}

/*****************************************************************************/
/*                                                                           */
/*   Module : prog_misc.c              Submodule : enter_hw                  */
/*   Author : Lamp Matthias            Project   : API-ASP-MON               */
/*---------------------------------------------------------------------------*/
/*   Create : 10/10/2001  Update :                                           */
/*---------------------------------------------------------------------------*/
/*    Descriptions                                                           */
/*    ------------                                                           */
/*    Inputs    : -                                                          */
/*                                                                           */
/*    Outputs   : PASS / FAIL : Depending on Novram read                     */
/*                                                                           */
/*    Description : Reads the Setup from NovRAM or initializes with defaults */
/*                                                                           */
/*****************************************************************************/
AiUInt32 enter_hw(TY_MONITOR_DATA_STRUCT *GLOBAL_VAR)
{

    AiUInt32 ActChkSum; /*,ExitMon tmp,*/

    AI_Printf("\nenter_hw");

    NOVRam_default(GLOBAL_VAR);

    AI_Printf("\npass NOVRam_default");

    /* check if NOVRam values can be used */
    ActChkSum = NOVRam_chksum(GLOBAL_VAR);

    AI_Printf("\nActChkSum: %08x\n ", ActChkSum);

    GLOBAL_VAR->NVR_CHECKSUM = NOVRam_read(GLOBAL_VAR, NVR_CHECKSUM_Adr);

    AI_Printf("\nGLOBAL_VAR->NVR_CHECKSUM: %08x", GLOBAL_VAR->NVR_CHECKSUM);

    if( ActChkSum != GLOBAL_VAR->NVR_CHECKSUM )
    {
        AI_Printf("\nActChkSum: %08x GLOBAL_VAR->NVR_CHECKSUM: %08x", ActChkSum, GLOBAL_VAR->NVR_CHECKSUM);
        AI_Printf("\nWrong NOVRam checksum !!! ERROR !!!");

        return (FAIL);
    }
    return (get_novram_setup(GLOBAL_VAR));
}

/*****************************************************************************/
/*   Module : prog_misc   Submodule : ini_bite_adr_test                      */
/*   Create : 10/10/2001  Update :                                           */
/*---------------------------------------------------------------------------*/
/*    Inputs    : RAM Startaddress, RAM Endaddress                           */
/*    Outputs   : PASS,FAIL                                                  */
/*                                                                           */
/*    Description : checks the addressing capabilty of the RAM               */
/*****************************************************************************/
AiUInt32 ini_bite_adr_test(TY_MONITOR_DATA_STRUCT *GLOBAL_VAR, AiUInt32 StartAddress, AiUInt32 EndAddress)
{
    AiUInt32 i, j;

    j = (EndAddress - StartAddress); /* >> 2; */

    AI_Printf("\nini_bite_adrt_test address");

    for( i = 0; i < j; i++ )
    {
        if( i % 0x10000 == 0 )
            AI_Printf("\rWrite Data to Adr.:%08x of %08x", i, j);
        PCI_Mem_write(GLOBAL_VAR, StartAddress + i, i);
    }

    for( i = 0; i < j; i++ )
    {
        if( PCI_Mem_read(GLOBAL_VAR, (StartAddress + i) != i) )
        {
            AI_Printf("\n!!!ERROR!!! -ini_bite_adrt_test address=%08x", i);
            return (FAIL);
        }
        if( i % 0x10000 == 0 )
            AI_Printf("\rRead Data from Adr.:%08x of %08x", i, j);
    }
    return (PASS);
}

/*****************************************************************************/
/*   Module : prog_misc  Submodule : ini_pat_adr_test                        */
/*   Create : 10/10/2001 Update :                                            */
/*---------------------------------------------------------------------------*/
/*    Inputs    : RAM Startaddress, RAM Endaddress                           */
/*    Outputs   : PASS,FAIL                                                  */
/*                                                                           */
/*    Description : checks the RAM with some data pattern                    */
/*****************************************************************************/
AiUInt32 ini_bite_pat_test(TY_MONITOR_DATA_STRUCT *GLOBAL_VAR, AiUInt32 StartAddress, AiUInt32 EndAddress)
{
    AiUInt32 i, j;

    j = (EndAddress - StartAddress); /* >> 2;        AiUInt32 contra BYTE*/

    AI_Printf("\nini_bite_pat_test data = 0x5555.5555\n");
    for( i = 0; i < j; i++ )
    {
        if( i % 0x10000 == 0 )
            AI_Printf("\rWrite Data to Adr.:%08x of %08x", i, j);
        PCI_Mem_write(GLOBAL_VAR, (StartAddress + i), 0x55555555);
    }

    AI_Printf("\n");
    for( i = 0; i < j; i++ )
    {
        if( PCI_Mem_read(GLOBAL_VAR, (StartAddress + i)) != 0x55555555 )
        {
            AI_Printf("\n!!!ERROR!!! -ini_bite_pat_test address=%08x by 0x5555.5555", i);
            return (FAIL);
        }
        if( i % 0x10000 == 0 )
            AI_Printf("\rRead Data from Adr.:%08x of %08x", i, j);
    }

    AI_Printf("\nini_bite_pat_test data = 0xaaaa.aaaa\n");
    for( i = 0; i < j; i++ )
    {
        PCI_Mem_write(GLOBAL_VAR, (StartAddress + i), 0xaaaaaaaa);
        if( i % 0x10000 == 0 )
            AI_Printf("\rWrite Data to Adr.:%08x of %08x", i, j);
    }

    AI_Printf("\n");
    for( i = 0; i < j; i++ )
    {
        if( PCI_Mem_read(GLOBAL_VAR, (StartAddress + i)) != 0xaaaaaaaa )
        {
            AI_Printf("\n!!!ERROR!!! -ini_bite_pat_test address=%08x by 0xaaaaaaaa", i);
            return (FAIL);
        }
        if( i % 0x10000 == 0 )
            AI_Printf("\rRead Data from Adr.:%08x of %08x", i, j);
    }

    AI_Printf("\nini_bite_pat_test data = 0xffff.ffff\n");
    for( i = 0; i < j; i++ )
    {
        PCI_Mem_write(GLOBAL_VAR, (StartAddress + i), 0xffffffff);
        if( i % 0x10000 == 0 )
            AI_Printf("\rWrite Data to Adr.:%08x of %08x", i, j);
    }

    AI_Printf("\n");
    for( i = 0; i < j; i++ )
    {
        if( PCI_Mem_read(GLOBAL_VAR, (StartAddress + i)) != 0xffffffff )
        {
            AI_Printf("\n!!!ERROR!!! -ini_bite_pat_test address=%08x by 0xffffffff", i);
            return (FAIL);
        }
        if( i % 0x10000 == 0 )
            AI_Printf("\rRead Data from Adr.:%08x of %08x", i, j);
    }

    AI_Printf("\nini_bite_pat_test data = 0x0000.0000\n");
    for( i = 0; i < j; i++ )
    {
        PCI_Mem_write(GLOBAL_VAR, (StartAddress + i), 0x0);
        if( i % 0x10000 == 0 )
            AI_Printf("\rWrite Data to Adr.:%08x of %08x", i, j);
    }

    AI_Printf("\n");
    for( i = 0; i < j; i++ )
    {
        if( PCI_Mem_read(GLOBAL_VAR, (StartAddress + i)) != 0x0 )
        {
            AI_Printf("\n!!!ERROR!!! -ini_bite_pat_test address=%08x, by 0x00000000", i);
            return (FAIL);
        }
        if( i % 0x10000 == 0 )
            AI_Printf("\rRead Data from Adr.:%08x of %08x", i, j);
    }
    return (PASS);
}

/*****************************************************************************/
/*   Module : prog_misc    Submodule : ini_pat_walk0_test                    */
/*   Create : 10/10/2001   Update :                                          */
/*---------------------------------------------------------------------------*/
/*    Inputs    : RAM Startaddress, RAM Endaddress                           */
/*    Outputs   : PASS,FAIL                                                  */
/*                                                                           */
/*    Description : checks the RAM with a walking zero                       */
/*****************************************************************************/
AiUInt32 ini_bite_walk0_test(TY_MONITOR_DATA_STRUCT *GLOBAL_VAR, AiUInt32 StartAddress, AiUInt32 EndAddress)
{
    AiUInt32 i, j, k, tpat;

    j = (EndAddress - StartAddress); /* >> 2; */
    tpat = 0x00000001;

	if (EndAddress <= 0x100)										// op-during power up
	{
		AI_Printf("\n\rini_pat_walk0_test: ");
	}
	else
	{
		AI_Printf("\n");
	}
	
	for( k = 0; k < 32; k++ )
    {
		if (EndAddress>0x100)										// remove op-during power up
		{
			AI_Printf("\rini_pat_walk0_test: %08x             \n", (~tpat));
		}
		else
		{
			AI_Printf(".");
		}


        for( i = 0; i < j; i++ )
        {
			if((i % 0x10000 == 0) && (EndAddress>0x100))			// remove op-during power up
                AI_Printf("\rWrite Data to Adr.:%08x of %08x", i, j);

            PCI_Mem_write(GLOBAL_VAR, (StartAddress + i), ~tpat);
        }

        for( i = 0; i < j; i++ )
        {
			if ((i % 0x10000 == 0) && (EndAddress>0x100))			// remove op-during power up
                AI_Printf("\rRead Data from Adr.:%08x of %08x", i, j);

            if( PCI_Mem_read(GLOBAL_VAR, (StartAddress + i)) != ~tpat )
            {
                AI_Printf("\n!!!ERROR!!! -ini_pat_walk0_test address=%08x, by %08x", i, ~tpat);
                return (FAIL);
            }
        }
        tpat = tpat << 1;
    }
    return (PASS);
}

/*****************************************************************************/
/*   Module : prog_misc   Submodule : ini_pat_walk1_test                     */
/*   Create : 10/10/2001  Update :                                           */
/*---------------------------------------------------------------------------*/
/*    Inputs    : RAM Startaddress, RAM Endaddress                           */
/*    Outputs   : PASS,FAIL                                                  */
/*                                                                           */
/*    Description : checks the RAM with a walking one                        */
/*****************************************************************************/
AiUInt32 ini_bite_walk1_test(TY_MONITOR_DATA_STRUCT *GLOBAL_VAR, AiUInt32 StartAddress, AiUInt32 EndAddress)
{
    AiUInt32 i, j, k, tpat;

    j = (EndAddress - StartAddress); /* >> 2; */
    tpat = 0x00000001;

    
    
	if (EndAddress <= 0x100)										// op-during power up
	{
		AI_Printf("\n\rini_pat_walk1_test: ");
	}
	else
	{
		AI_Printf("\n");
	}

	for( k = 0; k < 32; k++ )
    {
		if (EndAddress>0x100)										// remove op-during power up
		{
			AI_Printf("\rini_pat_walk1_test: %08x             \n", (tpat));
		}
		else
		{
			AI_Printf(".");
		}

		for( i = 0; i < j; i++ )
        {
			if ((i % 0x10000 == 0) && (EndAddress>0x100))			// remove op-during power up
                AI_Printf("\rWrite Data to Adr.:%08x of %08x", i, j);
            PCI_Mem_write(GLOBAL_VAR, (StartAddress + i), tpat);
        }

        for( i = 0; i < j; i++ )
        {
			if ((i % 0x10000 == 0) && (EndAddress>0x100))			// remove op-during power up
			{
				AI_Printf("\rRead Data from Adr.:%08x of %08x", i, j);
			}
            
			if( PCI_Mem_read(GLOBAL_VAR, (StartAddress + i)) != tpat )
            {
                AI_Printf("\n!!!ERROR!!! ini_bite_pat_test address=%08x, by %08x", i, tpat);
                return (FAIL);
            }
        }
        tpat = tpat << 1;
    }
    return (PASS);
}

/*****************************************************************************/
/*   Module : prog_misc   Submodule : ul_getName                             */
/*                                                                           */
/*   Description : build a File Name from given input parameters             */
/*****************************************************************************/
static AiUInt32 ul_getName(char* filename, AiUInt32 ul_NvrExtPtr, AiUInt32 ul_NvrReadVal, char* szName, char* szOldName, AiUInt32 bDefaultName)
{
    AiUInt32 ul_Status;
    AiUInt32 nvr_ext;

    if( 0 == (nvr_ext = ul_NvrExtPtr) )
        /* old definition */
        nvr_ext = ul_NvrReadVal;

    if( nvr_ext == 0xffff )
    {
        AI_Printf("\n%s not present, NVR_%sEX=%04x", szOldName, szOldName, nvr_ext);
        AI_Printf("\nor new board, with NON PROGRAMMED NOVRAM");
        if( (bDefaultName) )
        {
            AI_Printf("\nuse STANDART %s file: '%s'", szOldName, filename);
            sprintf(filename, "%s.SRE", szOldName);
            ul_Status = TRUE;
        }
        else
        {
            ul_Status = FAIL; /* not present */
        }
    }
    else
    {
        sprintf(filename, "%s%04x.sre", szName, nvr_ext);
        ul_Status = TRUE;
    }
    return ul_Status;
}

/*****************************************************************************/
/*                                                                           */
/*   Module : prog_misc.c              Submodule : ul_BuildLoadFilename      */
/*   Author : Rolf Heitzmann           Project   : API-ASP-MON               */
/*---------------------------------------------------------------------------*/
/*   Create :  4/12/2013  Update :                                           */
/*---------------------------------------------------------------------------*/
/*    Descriptions                                                           */
/*    ------------                                                           */
/*    Inputs    : See Parameterlist                                          */
/*                                                                           */
/*    Outputs   : See Parameterlist                                          */
/*                                                                           */
/*    Description : Builds file name of loadable file                        */
/*                                                                           */
/*****************************************************************************/
static AiUInt32 ul_BuildLoadFilename(TY_MONITOR_DATA_STRUCT *GLOBAL_VAR,
            AiUInt32 type,
            char *filename,
            AiUInt32 bDefaultName,
            AiUInt8 *puc_TempPath)
{
    AiUInt32 ul_Status;
/*   AiUInt32 nvr_ext; */

    /*V02.00, create valid filename*/
    switch( type )
    /* S-Record type */
    {
        case 0x0:
            /* not used */
            ul_Status = FAIL;
            break;

        case 0x1:
            /* IO-FPGA 1 */
            ul_Status = ul_getName(filename, GLOBAL_VAR->NVR_ILCA1EX.Reg.ILCA1, get_ILCA1_EXT(GLOBAL_VAR), "ILCA", "IO-FPGA1",
                        bDefaultName);
            break;

        case 0x2:
            /* IO-FPGA 2 */
            ul_Status = ul_getName(filename, GLOBAL_VAR->NVR_ILCA2EX.Reg.ILCA2, get_ILCA2_EXT(GLOBAL_VAR), "ILCA", "IO-FPGA2",
                        bDefaultName);
            break;

        case 0x3:
            /* BIP-1 */
            ul_Status = ul_getName(filename, GLOBAL_VAR->NVR_BIP1EX.Reg.BIP1, get_BIP1_EXT(GLOBAL_VAR), "BIP_", "BIP-1", bDefaultName);
            break;

        case 0x4:
            /* BIP-2 */
            ul_Status = ul_getName(filename, GLOBAL_VAR->NVR_BIP2EX.Reg.BIP2, get_BIP2_EXT(GLOBAL_VAR), "BIP_", "BIP-2", bDefaultName);
            break;

        case 0x5:
            /* PROM (PCI-FPGA) */
            ul_Status = ul_getName(filename, GLOBAL_VAR->NVR_PLCAEX.Reg.PLCA, get_PLCA_EXT(GLOBAL_VAR), "PLCA", "PROM", bDefaultName);
            break;

        case 0x6:
            /* TCP Flash */
            ul_Status = ul_getName(filename, GLOBAL_VAR->NVR_TCPEX.Reg.TCP, get_TCP_EXT(GLOBAL_VAR), "TCP_", "TCP", bDefaultName);
            break;

        case 0x7:
            /* TCP NOVRam */
            strcpy((char*) filename, "TCP_NRAM.sre");
            ul_Status = TRUE;
            break;

        case 0x17:
            /* TCP_NOVRam */
            strcpy((char*) filename, "TCP_NOVRAM_back.sre");
            *puc_TempPath = TRUE;
            ul_Status = TRUE;
            break;
        case 0x8:
            /* GLOBAL RAM (only used to write back S-Rec) */
            strcpy((char*) filename, "GLOB_RAM.sre");
            ul_Status = TRUE;
            break;

        case 0x9:
            /* ASP */
            ul_Status = ul_getName(filename, GLOBAL_VAR->NVR_ASPEX.Reg.ASP, get_ASP_EXT(GLOBAL_VAR), "ASP_", "ASP", bDefaultName);
            break;

        case 0xa:
            /* PROM1.sre (PCI-FPGA) */
            strcpy((char*) filename, "prom1.sre");
            ul_Status = TRUE;
            break;

        default:
            /* reserved */
            ul_Status = FAIL;
            break;
    } /*switch */
    return ul_Status;

}

/*****************************************************************************/
/*                                                                           */
/*   Module : prog_misc.c              Submodule : ul_DecodeSrecordLine      */
/*   Author : Rolf Heitzmann           Project   : API-ASP-MON               */
/*---------------------------------------------------------------------------*/
/*   Create :  4/12/2013  Update :                                           */
/*---------------------------------------------------------------------------*/
/*    Descriptions                                                           */
/*    ------------                                                           */
/*    Inputs    : See Parameterlist                                          */
/*                                                                           */
/*    Outputs   : See Parameterlist                                          */
/*                                                                           */
/*    Description : Decodes one S-Record Line to binary                      */
/*                                                                           */
/*****************************************************************************/
static AiUInt32 ul_DecodeSrecordLine(AiUInt8 *pSrec,        /* Pointer to a Buffer which contaions the original S-Record data */
            AiUInt8 *buffer,                      /* Pointer to a Buffer where decoded data shall be written        */
            AiUInt8 *pNibble,                     /* Pointer to a Buffer which contaions already decoded data of line.*/
            AiUInt32 length,                      /* Length of Line*/
            AiUInt32 *pul_mem_count,              /* Cointer of Memory In/Out*/
            AiUInt32 *pul_mem_offset,             /* Offset of Memory In/Out*/
            AiUInt32 *pul_first_line)             /* Line for first decoded date line*/
{
    AiUInt32 mem_adr;
    AiUInt32 i;

    /* switch on S-Record type */
    switch( pSrec[1] )
    /*S-Record type*/
    {
        case '0':
            /*S0 Version information, not implemented*/
            break;
        case '1':
            /*S1 2byte address field, memory loadable data*/
            mem_adr = (((AiUInt32) pNibble[0]) << 8) | ((AiUInt32) pNibble[1]);
            if( *pul_first_line == 0 )
            /* if the first line is read calculate the offset*/
            {
                *pul_mem_offset = mem_adr;
                *pul_first_line = 1;
            }
            else if( mem_adr != (*pul_mem_count + *pul_mem_offset) )
            /*es gibt die Moeglichkeit von Luecken / Spruengen*/
            {
                if( mem_adr < (*pul_mem_count + *pul_mem_offset) )
                /*eigenlich der einzige Fall*/
                {
                    AI_Printf("\nAdr.Decoding ERROR @ %08x : %08x\n", (*pul_mem_count + *pul_mem_offset), mem_adr);
                    return (FAIL);
                    break;
                }
                for( i = 0; (*pul_mem_count + *pul_mem_offset) < mem_adr; i++ )
                /*fill unused memblocks*/
                {
                    *(buffer + *pul_mem_count) = 0x1;
                    (*pul_mem_count)++;
                }
            }
            for( i = 2; i < (length - 1); i++ )
            {
                *(buffer + *pul_mem_count) = pNibble[i];
                (*pul_mem_count)++;

            }
            break;
        case '2':
            /*S2 3byte address field, memory loadable data*/
            mem_adr = (AiUInt32) ((((AiUInt32) (pNibble[0])) << 16) | (((AiUInt32) (pNibble[1])) << 8) | (AiUInt32) pNibble[2]);

            if( *pul_first_line == 0 )
            /* if the first line is read calculate the offset*/
            {
                *pul_mem_offset = mem_adr;
                *pul_first_line = 1;
            }
            else if( mem_adr != (AiUInt32) (*pul_mem_count + *pul_mem_offset) )
            /*es gibt die Moeglichkeit von Luecken / Spruengen*/
            {
                if( mem_adr < (AiUInt32) (*pul_mem_count + *pul_mem_offset) )
                /*eigenlich der einzige Fall*/
                {
                    AI_Printf("\nAdr.Decoding ERROR @ %08x : %08x\n", (*pul_mem_count + *pul_mem_offset), mem_adr);
                    return (FAIL);
                    break;
                }
                for( i = 0; (*pul_mem_count + *pul_mem_offset) < mem_adr; i++ )
                /*fill unused memblocks*/
                {
                    *(buffer + *pul_mem_count) = 0x1;
                    (*pul_mem_count)++;
                }
            }
            /*-----------------                */
            for( i = 3; i < (length - 1); i++ )
            {
                *(buffer + *pul_mem_count) = pNibble[i];
                (*pul_mem_count)++;
            }
            break;
            /*-----------------*/
        case '3':
            /*S3 4byte address field, memory loadable data*/
            mem_adr = (((AiUInt32) (pNibble[0])) << 24) | (((AiUInt32) (pNibble[1])) << 16) | (((AiUInt32) (pNibble[2])) << 8)
                        | ((AiUInt32) (pNibble[3]));
            if( (0 == mem_adr) && (0xB == length) )
            {
                AI_Printf("[3] S3 Mark found\n");
            }
            else
            { /* only copy memory, if no S3 mark was detected (length=11 and address=0) */
                if( *pul_first_line == 0 )
                /* if the first line is read calculate the offset*/
                {
                    *pul_mem_offset = mem_adr;
                    *pul_first_line = 1;
                }
                else if( mem_adr != (*pul_mem_count + *pul_mem_offset) )
                /*es gibt die Moeglichkeit von Luecken / Spruengen*/
                {
                    if( mem_adr < (*pul_mem_count + *pul_mem_offset) )
                    /*eigenlich der einzige Fall*/
                    {
                        AI_Printf("\nAdr.Decoding ERROR @ %08x : %08x\n", (*pul_mem_count + *pul_mem_offset), mem_adr);
                        return (FAIL);
                        break;
                    }
                    for( i = 0; (*pul_mem_count + *pul_mem_offset) < mem_adr; i++ )
                    /*fill unused memblocks*/
                    {
                        *(buffer + *pul_mem_count) = 0x1;
                        (*pul_mem_count)++;
                    }
                }

                for( i = 4; i < (length - 1); i++ )
                {
                    *(buffer + *pul_mem_count) = pNibble[i];
                    (*pul_mem_count)++;
                }
            }
            break;
        case '4':
            /*S4 does not exists (by definition)*/
            AI_Printf("\nS-Record decoding errorr. \n");
            return (FAIL);
            break;
        case '5':
            /*S5 counting information, not implemented*/
            break;
        case '6':
            /*S6 does not exists (by definition)*/
            /* return (FAIL) */
            break;
        case '7':
            /*S7 address field = start execution address (4byte), no data field*/
            mem_adr = ((AiUInt32) (pNibble[0]) << 24) | ((AiUInt32) (pNibble[1]) << 16) | ((AiUInt32) (pNibble[2]) << 8)
                        | ((AiUInt32) (pNibble[3]));
            break;
        case '8':
            /*S8 address field = start execution address (3byte), no data field*/
            mem_adr = ((AiUInt32) (pNibble[0]) << 16) | ((AiUInt32) (pNibble[1]) << 8) | ((AiUInt32) (pNibble[2]));
            break;
        case '9':
            /*S9 address field = start execution address (2byte), no data field*/
            mem_adr = ((AiUInt32) (pNibble[0]) << 8) | ((AiUInt32) (pNibble[1]));
            break;
        default:
            AI_Printf("\nS-Record decoding error");
            /* return (FAIL) */
            break;
    } /*switch    */

    return (TRUE);
}

/*****************************************************************************/
/*                                                                           */
/*   Module : prog_misc.c              Submodule : ul_DecodeSrecordBlock     */
/*   Author : Rolf Heitzmann           Project   : API-ASP-MON               */
/*---------------------------------------------------------------------------*/
/*   Create :  4/12/2013  Update :                                           */
/*---------------------------------------------------------------------------*/
/*    Descriptions                                                           */
/*    ------------                                                           */
/*    Inputs    : AiUInt8* puc_SrecBlock Pointer to a Memory which contains  */
/*                                       Data in S-Record Format             */
/*                AiUInt8  *buffer       Pointer to a Memory Area whrere the */
/*                                       data shall be decoded.              */
/*                                                                           */
/*    Outputs   : returns size of decoded memory                             */
/*                                                                           */
/*    Description : load serecord, and decode it                             */
/*                                                                           */
/*****************************************************************************/
static AiUInt32 ul_DecodeSrecordBlock(AiUInt8* puc_SrecBlock, AiUInt8* puc_BinData)
{

    AiUInt32 i, j, mem_count = 0, mem_offset = 0, length, chk_sum, first_line = 0, count = 0;
    AiUInt8 *pSrec;
    AiUInt8 nibble[0x104]; /*maximum row length */

    /* decode S-rec file*/

    i = j = 0;
    MON_ONLY_PRINTF("\n");

    while( *(puc_SrecBlock + count) != '\0' )
    /*(ch!=EOF)*/
    {
        j++;
        /*search next start of Line, parse over Line End */
        while( *(puc_SrecBlock + count) != 'S' )
        {
            count++;
            if( *(puc_SrecBlock + count) == '\0' )
                break;

        }
        /*Start of Line found*/
        pSrec = (puc_SrecBlock + count);

        /* Auswerten der S-Record Zeile, Typ, ggf. Daten Speichern */

#ifdef __monitor__
        PRINTFR("."); /* a dot each readed S-Record line */
        if( j % 12 == 0 )
        PRINTFR("\r               \r");
#endif 

        /* the general format of an S-Record */
        /*  2      2       2..4      ?             2           */
        /* +------+-------+---/ /---+-----/ /-----+----------+ */
        /* | type | count | address |     data    | checksum | */
        /* +------+-------+---/ /---+----/ /------+----------+ */

        if( pSrec[0] == 0x53 )
        /*big 'S'*/
        {
            length = 0; /* count (pairwaise)*/
            length = (chat_to_int((char) pSrec[2]) << 4) | chat_to_int((char) pSrec[3]); /*bitwise or*/
            count += (length * 2) + 4;
            chk_sum = length;

            for( i = 4; i <= (3 + (length * 2)); i += 2 )
            {
                if( (((char) pSrec[i]) == '\n') || (((char) pSrec[i + 1]) == '\n') )
                {
                    AI_Printf("\n ERROR (length)\n");
                    break;
                }
                nibble[(i - 4) / 2] = (chat_to_int((char) pSrec[i]) << 4) | chat_to_int((char) pSrec[i + 1]);
                chk_sum += nibble[(i - 4) / 2];
            }

            if( (chk_sum & 0xff) != 255 )
            /*mask lower 8 bits*/
            {
                AI_Printf("\n ERROR (chk_sum) %x\n", (chk_sum & 0xff));
                break;
            }

            ul_DecodeSrecordLine(pSrec, puc_BinData, nibble, length, &mem_count, &mem_offset, &first_line);

        } /* if S*/
    } /*  while EOF*/

    return mem_count;
}

/*****************************************************************************/
/*                                                                           */
/*   Module : prog_misc.c              Submodule : Load_Srec                 */
/*   Author : Lamp Matthias            Project   : API-ASP-MON               */
/*---------------------------------------------------------------------------*/
/*   Create : 10/10/2001  Update :     01.12.2010 V02.20 filename array      */
/*                                     to 256 bytes                          */
/*---------------------------------------------------------------------------*/
/*    Descriptions                                                           */
/*    ------------                                                           */
/*    Inputs    : AiUInt32 type     (S-rec type FPGA, BIU...)                */
/*                AiUInt8  *buffer  (place of decoded S-rec)                 */
/*                                                                           */
/*    Outputs   : PASS / FAIL                                                */
/*                                                                           */
/*    Description : load serecord, and decode it                             */
/*                                                                           */
/*****************************************************************************/
/*static      AiUInt32 ibuf[ MON_MAX_PATH ];*/
/* static char filename[ MON_MAX_PATH ];*/
AiUInt32 Load_Srec(TY_MONITOR_DATA_STRUCT *GLOBAL_VAR, AiUInt32 type, AiUInt8 *buffer)
{
    char filename[MON_MAX_PATH];
#ifndef __monitor__
    unsigned long ul_Size;
    AiUInt32 ret;
    AiUInt32 bDefaultName = FALSE;
#else
    AiUInt32 bDefaultName = TRUE;
#endif 

#if defined(__VXWORKS__) && defined(__monitor__)
    unsigned long ul_Size;
    AiUInt32 ret;
#endif 

    AiUInt8 *srec; /* Buffer where data from file is written (copy of S-Record file) */
    /* AiUInt32 nvr_ext;*/

    AiUInt32 mem_count = 0;
    AiUInt8 temppath = FALSE;

    /* allocate memory */
    srec = (GLOBAL_VAR->GLOBAL_TEMP_MEM + (GLOBAL_VAR->GLOBAL_TEMP_MEM_SIZE / 2));
    memset(srec, 0, (GLOBAL_VAR->GLOBAL_TEMP_MEM_SIZE / 2));

    ul_BuildLoadFilename(GLOBAL_VAR, type, filename, bDefaultName, &temppath);

#if defined(__monitor__)
    /* V02.00, 24.02.06*/
#ifdef __STORE_IN_MEMORY__
    if ((ret = ul_LoadCode((unsigned long)GLOBAL_VAR->uc_ModuleNr, filename /* type of srec (FPGA1, FPGA2 ...)*/
                                        , srec /* adress of srec*/
                                        , &ul_Size, GLOBAL_VAR->GLOBAL_TEMP_MEM_SIZE / 2)) /* Device ID, BIU1 and 2 Type*/
                != 0)
    /* file open driver    */

#else

    if( Load_Code(GLOBAL_VAR, filename, srec, temppath) == FAIL )
    /*file open monitor*/
#endif
#elif defined(_SMARTLIB)
    ret = 1;
    if( ret != 0 )
#else
    if( (ret = ul_LoadCode(GLOBAL_VAR->h_Drv, filename /* type of srec (FPGA1, FPGA2 ...)*/
    , srec /* adress of srec*/
    , &ul_Size, GLOBAL_VAR->GLOBAL_TEMP_MEM_SIZE / 2)) /* Device ID, BIU1 and 2 Type*/
    != API_OK )
    /* file open driver    */
#endif 
    {
#ifdef __VXWORKS__
        printf("\r\n!!! ERROR !!! getting SRE. Name:%s Size:%ld BufSize:%d\r\n", filename, ul_Size, GLOBAL_VAR->GLOBAL_TEMP_MEM_SIZE / 2);
#else 
        AI_Printf("\n!!! ERROR !!! error by opening file");
#endif 
        return (FAIL);

    }

    mem_count = ul_DecodeSrecordBlock(srec, buffer);

    MonPrintBatchCondition(GLOBAL_VAR, "\nmemory_count = %08x bytes of memory\n", (mem_count - 1));

    return (mem_count - 1);
}

/*****************************************************************************/
/*                                                                           */
/*   Module : ini_prog.c               Submodule : chat_to_int               */
/*   Author : Lamp Matthias            Project   : API-ASP-MON               */
/*---------------------------------------------------------------------------*/
/*   Create : 14/05/2001   Update :  -                                       */
/*---------------------------------------------------------------------------*/
/*    Descriptions                                                           */
/*    ------------                                                           */
/*    Inputs    : char:     : a single character                             */
/*                                                                           */
/*    Outputs   : int       : the intager of the character                   */
/*                                                                           */
/*    Description : converts the character to the coresponding intager       */
/*                  char 0 -> 0x0 ... char a -> int 0xa ...                  */
/*                                                                           */
/*****************************************************************************/
int chat_to_int(char single_char)
{
    switch( single_char )
    {
        case '0':
            return (0x0);
        case '1':
            return (0x1);
        case '2':
            return (0x2);
        case '3':
            return (0x3);
        case '4':
            return (0x4);
        case '5':
            return (0x5);
        case '6':
            return (0x6);
        case '7':
            return (0x7);
        case '8':
            return (0x8);
        case '9':
            return (0x9);
        case 'a':
        case 'A':
            return (0xa);
        case 'b':
        case 'B':
            return (0xb);
        case 'c':
        case 'C':
            return (0xc);
        case 'd':
        case 'D':
            return (0xd);
        case 'e':
        case 'E':
            return (0xe);
        case 'f':
        case 'F':
            return (0xf);
        default:
            return (ZERO);
    }
}

/*****************************************************************************/
/*                                                                           */
/*   Module : prog_misc.c              Submodule : write_spi                 */
/*   Author : Lamp Matthias            Project   : PMC-1553-MON              */
/*---------------------------------------------------------------------------*/
/*   Create : 17.10.2001   Update :   .  .                                   */
/*---------------------------------------------------------------------------*/
/*    Descriptions                                                           */
/*    ------------                                                           */
/*    Inputs    : AiUInt8:  Byte to be send to SPI port                      */
/*                                                                           */
/*    Outputs   : -                                                          */
/*                                                                           */
/*    Description :  Shifts the byte to the SPI port                         */
/*                                                                           */
/*****************************************************************************/
void write_spi(TY_MONITOR_DATA_STRUCT *GLOBAL_VAR, AiUInt8 DByte)
{
    AiInt32 i;

    for( i = 7; i >= 0; i-- )
    {
        if( (DByte & (1 << i)) != 0 )
        {
            /*             SPI_OUT_1*/
            GLOBAL_VAR->PMC_TCP_PROG_Reg.all = 0;
            GLOBAL_VAR->PMC_TCP_PROG_Reg.Reg.SPI_OE = 1;
            GLOBAL_VAR->PMC_TCP_PROG_Reg.Reg.SPI_MOSI = 1;
            PCI_IOReg_write(GLOBAL_VAR, PMC_TCP_PROG_Reg_Adr, GLOBAL_VAR->PMC_TCP_PROG_Reg.all);

            wait_clocks(GLOBAL_VAR, 50); /*10us*/

            /*            SPI_OUT_1_CLK*/
            GLOBAL_VAR->PMC_TCP_PROG_Reg.all = 0;
            GLOBAL_VAR->PMC_TCP_PROG_Reg.Reg.SPI_OE = 1;
            GLOBAL_VAR->PMC_TCP_PROG_Reg.Reg.SPI_MOSI = 1;
            GLOBAL_VAR->PMC_TCP_PROG_Reg.Reg.SPI_CLK = 1;
            PCI_IOReg_write(GLOBAL_VAR, PMC_TCP_PROG_Reg_Adr, GLOBAL_VAR->PMC_TCP_PROG_Reg.all);

        }
        else
        {
            /*            SPI_OUT_0*/
            GLOBAL_VAR->PMC_TCP_PROG_Reg.all = 0;
            GLOBAL_VAR->PMC_TCP_PROG_Reg.Reg.SPI_OE = 1;
            PCI_IOReg_write(GLOBAL_VAR, PMC_TCP_PROG_Reg_Adr, GLOBAL_VAR->PMC_TCP_PROG_Reg.all);

            wait_clocks(GLOBAL_VAR, 50); /*10us*/

            /*            SPI_OUT_0_CLK*/
            GLOBAL_VAR->PMC_TCP_PROG_Reg.all = 0;
            GLOBAL_VAR->PMC_TCP_PROG_Reg.Reg.SPI_OE = 1;
            GLOBAL_VAR->PMC_TCP_PROG_Reg.Reg.SPI_CLK = 1;
            PCI_IOReg_write(GLOBAL_VAR, PMC_TCP_PROG_Reg_Adr, GLOBAL_VAR->PMC_TCP_PROG_Reg.all);

        }
        wait_clocks(GLOBAL_VAR, 50); /*10us*/

        /*        SPI_CLK_LOW*/
        GLOBAL_VAR->PMC_TCP_PROG_Reg.all = 0;
        GLOBAL_VAR->PMC_TCP_PROG_Reg.Reg.SPI_OE = 1;
        PCI_IOReg_write(GLOBAL_VAR, PMC_TCP_PROG_Reg_Adr, GLOBAL_VAR->PMC_TCP_PROG_Reg.all);
    } /*for*/
    return;
}

/*****************************************************************************/
/*                                                                           */
/*   Module : prog_misc.c              Submodule : read_spi                  */
/*   Author : Lamp Matthias            Project   : PMC-1553-MON              */
/*---------------------------------------------------------------------------*/
/*   Create : 17.10.2001   Update :   .  .                                   */
/*---------------------------------------------------------------------------*/
/*    Descriptions                                                           */
/*    ------------                                                           */
/*    Inputs    : -                                                          */
/*                                                                           */
/*    Outputs   : AiUInt8:  Byte received from  SPI port                     */
/*                                                                           */
/*    Description :  Shifts the byte from the SPI port                       */
/*                                                                           */
/*****************************************************************************/
AiUInt8 read_spi(TY_MONITOR_DATA_STRUCT *GLOBAL_VAR)
{
    AiInt32 i;
    AiUInt8 DByte;
    AiUInt32 temp;
    DByte = 0;

    wait_clocks(GLOBAL_VAR, 10); /*10us*/

    for( i = 7; i >= 0; i-- )
    {
        wait_clocks(GLOBAL_VAR, 10); /*10us*/

        /*        SPI_CLK_HIGH*/
        GLOBAL_VAR->PMC_TCP_PROG_Reg.all = 0;
        GLOBAL_VAR->PMC_TCP_PROG_Reg.Reg.SPI_OE = 1;
        GLOBAL_VAR->PMC_TCP_PROG_Reg.Reg.SPI_CLK = 1;
        PCI_IOReg_write(GLOBAL_VAR, PMC_TCP_PROG_Reg_Adr, GLOBAL_VAR->PMC_TCP_PROG_Reg.all);

        wait_clocks(GLOBAL_VAR, 10); /*10us*/

        temp = PCI_IOReg_read(GLOBAL_VAR, PMC_TCP_PROG_Reg_Adr);
        GLOBAL_VAR->PMC_TCP_PROG_Reg.Reg.SPI_MISO = (temp >> 5) & 1; /* mask bit        */

        if( GLOBAL_VAR->PMC_TCP_PROG_Reg.Reg.SPI_MISO != 0 )
            DByte = DByte | (1 << i);

        /*        SPI_CLK_LOW*/
        GLOBAL_VAR->PMC_TCP_PROG_Reg.all = 0;
        GLOBAL_VAR->PMC_TCP_PROG_Reg.Reg.SPI_OE = 1;
        PCI_IOReg_write(GLOBAL_VAR, PMC_TCP_PROG_Reg_Adr, GLOBAL_VAR->PMC_TCP_PROG_Reg.all);

        wait_clocks(GLOBAL_VAR, 10); /*10us*/
    }
    wait_clocks(GLOBAL_VAR, 10); /*10us*/
    return (DByte);
}

/*****************************************************************************/
/*                                                                           */
/*   Module : prog_misc.c              Submodule : read_spi_flash            */
/*   Author : Hans Zogalla             Project   : API-ASP-MON               */
/*---------------------------------------------------------------------------*/
/*   Create : 08.06.99   Update :  -                                         */
/*---------------------------------------------------------------------------*/
/*    Descriptions                                                           */
/*    ------------                                                           */
/*    Inputs    : AiUInt32 Address of TCP flash address                      */
/*                                                                           */
/*    Outputs   : AiUInt16:  word from TCP Flash                             */
/*                                                                           */
/*    Description :  Reads a location from TCP Flash                         */
/*                                                                           */
/*****************************************************************************/
AiUInt16 read_spi_flash(TY_MONITOR_DATA_STRUCT *GLOBAL_VAR, AiUInt32 address)
{
    AiUInt16 Data;

    Data = 0;

    write_spi(GLOBAL_VAR, SPI_RD_FLASH_LOW);
    write_spi(GLOBAL_VAR, (AiUInt8) ((address >> 8) & 0xff));
    write_spi(GLOBAL_VAR, (AiUInt8) (address & 0xff));

    Data = read_spi(GLOBAL_VAR) & 0xff;
    write_spi(GLOBAL_VAR, SPI_RD_FLASH_HIGH);
    write_spi(GLOBAL_VAR, (AiUInt8) ((address >> 8) & 0xff));
    write_spi(GLOBAL_VAR, (AiUInt8) (address & 0xff));

    Data = Data + ((read_spi(GLOBAL_VAR) & 0xff) << 8);
    return (Data);
}

/*****************************************************************************/
/*                                                                           */
/*   Module : prog_misc.c              Submodule : read_32bit_spi_flash      */
/*   Author : Hans Zogalla             Project   : API-ASP-MON               */
/*---------------------------------------------------------------------------*/
/*   Create : 08.06.99   Update :  -                                         */
/*---------------------------------------------------------------------------*/
/*    Descriptions                                                           */
/*    ------------                                                           */
/*    Inputs    : AiUInt32 Address of TCP flash address                      */
/*                                                                           */
/*    Outputs   : AiUInt16:  word from TCP Flash                             */
/*                                                                           */
/*    Description :  Reads a location from TCP Flash                         */
/*                                                                           */
/*****************************************************************************/
AiUInt32 read_32bit_spi_flash_mirror(TY_MONITOR_DATA_STRUCT *GLOBAL_VAR, AiUInt32 address)
{
    AiUInt32 Data;
    AiUInt32 Local_address;

    Local_address = (address >> 1) + 0xf00;
    Data = 0;

    write_spi(GLOBAL_VAR, SPI_RD_FLASH_LOW);
    write_spi(GLOBAL_VAR, (AiUInt8) ((Local_address >> 8) & 0xff));
    write_spi(GLOBAL_VAR, (AiUInt8) (Local_address & 0xff));

    Data = read_spi(GLOBAL_VAR) & 0xff;

    write_spi(GLOBAL_VAR, SPI_RD_FLASH_HIGH);
    write_spi(GLOBAL_VAR, (AiUInt8) ((Local_address >> 8) & 0xff));
    write_spi(GLOBAL_VAR, (AiUInt8) (Local_address & 0xff));

    Data = Data + ((read_spi(GLOBAL_VAR) & 0xff) << 8);

    write_spi(GLOBAL_VAR, SPI_RD_FLASH_LOW);
    write_spi(GLOBAL_VAR, (AiUInt8) (((Local_address + 1) >> 8) & 0xff));
    write_spi(GLOBAL_VAR, (AiUInt8) ((Local_address + 1) & 0xff));

    Data = Data + ((read_spi(GLOBAL_VAR) & 0xff) << 16);

    write_spi(GLOBAL_VAR, SPI_RD_FLASH_HIGH);
    write_spi(GLOBAL_VAR, (AiUInt8) (((Local_address + 1) >> 8) & 0xff));
    write_spi(GLOBAL_VAR, (AiUInt8) ((Local_address + 1) & 0xff));

    Data = Data + ((read_spi(GLOBAL_VAR) & 0xff) << 24);

    return (Data);
}

/*****************************************************************************/
/*                                                                           */
/*   Module : ini_prog.c               Submodule : read_spi_novram           */
/*   Author : Hans Zogalla             Project   : API-ASP-MON               */
/*---------------------------------------------------------------------------*/
/*   Create : 08.06.99   Update :  -                                         */
/*---------------------------------------------------------------------------*/
/*    Descriptions                                                           */
/*    ------------                                                           */
/*    Inputs    : AiUInt32 Address of TCP flash address                      */
/*                                                                           */
/*    Outputs   : AiUInt8:  word from TCP Flash                              */
/*                                                                           */
/*    Description :  Reads a location from TCP Flash                         */
/*                                                                           */
/*****************************************************************************/
AiUInt8 read_spi_novram(TY_MONITOR_DATA_STRUCT *GLOBAL_VAR, AiUInt32 address)
{
    AiUInt8 Data = 0;

    write_spi(GLOBAL_VAR, SPI_RD_NOVRAM);
    write_spi(GLOBAL_VAR, (AiUInt8) ((address >> 8) & 0x03));
    write_spi(GLOBAL_VAR, (AiUInt8) (address & 0xff));
    Data = read_spi(GLOBAL_VAR) & 0xff;

    return (Data);
}

/*****************************************************************************/
/*                                                                           */
/*   Module : prog_misc.c              Submodule : write_spi_novram          */
/*   Author : Hans Zogalla             Project   : API-ASP-MON               */
/*---------------------------------------------------------------------------*/
/*   Create : 08.06.99   Update :  -                                         */
/*---------------------------------------------------------------------------*/
/*    Descriptions                                                           */
/*    ------------                                                           */
/*    Inputs    : AiUInt32 Address of TCP flash address                      */
/*                                                                           */
/*    Outputs   : AiUInt16:  word from TCP Flash                             */
/*                                                                           */
/*    Description :  Reads a location from TCP Flash                         */
/*                                                                           */
/*****************************************************************************/
AiUInt32 write_spi_novram(TY_MONITOR_DATA_STRUCT *GLOBAL_VAR, AiUInt32 address, AiUInt8 data)
{
    AiUInt32 flag, i;

    flag = FAIL;
    i = 0;

    /* Write and Verify, try upto 4 times with 5/10/20/40 ms waits */
    while( (i < 4) && (flag == FAIL) )
    {
        write_spi(GLOBAL_VAR, SPI_WR_NOVRAM);
        write_spi(GLOBAL_VAR, (AiUInt8) ((address >> 8) & 0x03));
        write_spi(GLOBAL_VAR, (AiUInt8) (address & 0xff));
        write_spi(GLOBAL_VAR, (AiUInt8) (data & 0xff));

        wait_clocks(GLOBAL_VAR, (1 << i) * 5000);
        /* Verify write values */
        if( read_spi_novram(GLOBAL_VAR, address) == data )
            flag = PASS;
        i++;
    }
    return (flag);
}

/*****************************************************************************/
/*                                                                           */
/*   Module : prog_misc.c              Submodule : write_spi_flash           */
/*   Author : Hans Zogalla             Project   : API-ASP-MON               */
/*---------------------------------------------------------------------------*/
/*   Create : 08.06.99   Update :  -                                         */
/*---------------------------------------------------------------------------*/
/*    Descriptions                                                           */
/*    ------------                                                           */
/*    Inputs    : AiUInt32 Address of TCP flash address                      */
/*                                                                           */
/*    Outputs   : AiUInt32:  PASS or FAIL                                    */
/*                                                                           */
/*    Description :  Writes a TCP Flash location                             */
/*                                                                           */
/*****************************************************************************/
AiUInt32 write_spi_flash(TY_MONITOR_DATA_STRUCT *GLOBAL_VAR, AiUInt32 address, AiUInt16 Data)
{
    AiUInt32 flag, i;

    flag = FAIL;
    i = 0;

    /* Write and Verify, try upto 4 times with 5/10/20/40 ms waits */
    while( (i < 6) && (flag == FAIL) )
    /* was(i<4)*/
    {
        write_spi(GLOBAL_VAR, SPI_WR_FLASH_LOW);
        write_spi(GLOBAL_VAR, (AiUInt8) ((address >> 8) & 0xff));
        write_spi(GLOBAL_VAR, (AiUInt8) (address & 0xff));
        write_spi(GLOBAL_VAR, (AiUInt8) (Data & 0xff));

        wait_clocks(GLOBAL_VAR, (1 << i) * 10000); /* was 5000,  ML 04/07/03*/

        write_spi(GLOBAL_VAR, SPI_WR_FLASH_HIGH);
        write_spi(GLOBAL_VAR, (AiUInt8) ((address >> 8) & 0xff));
        write_spi(GLOBAL_VAR, (AiUInt8) (address & 0xff));
        write_spi(GLOBAL_VAR, (AiUInt8) ((Data >> 8) & 0xff));

        wait_clocks(GLOBAL_VAR, (1 << i) * 10000); /* was 5000   ML 04/07/03 from 1000 back to 5000*/

        /* Verify write values */
        if( read_spi_flash(GLOBAL_VAR, address) == Data )
            flag = PASS;
        i++;
    }
    AI_Printf("%x", i);
    return (flag);
}

/*****************************************************************************/
/*                                                                           */
/*   Module : prog_misc.c              Submodule : spi_prog_enable           */
/*   Author : Lamp Matthias            Project   : PMC-1553-MON              */
/*---------------------------------------------------------------------------*/
/*   Create : 03.01.2002   Update :   .  .                                   */
/*---------------------------------------------------------------------------*/
/*    Descriptions                                                           */
/*    ------------                                                           */
/*    Inputs    : -                                                          */
/*                                                                           */
/*    Outputs   :    PASS / FAIL                                             */
/*                                                                           */
/*    Description :  Enables the TCP Programming Port                        */
/*                                                                           */
/*****************************************************************************/
AiUInt32 spi_prog_enable(TY_MONITOR_DATA_STRUCT *GLOBAL_VAR)
{
    AiUInt32 Data, x;

    /* ATMEL inactive (reset),
     SCK to "0",
     ATMEL active,
     ATMEL inactive (reset)
     */

    /* ATMEL inactive*/
    GLOBAL_VAR->PMC_RESET_Reg.Reg.ATMEL_RESx = 0;
    PCI_IOReg_write(GLOBAL_VAR, PMC_RESET_Reg_Adr, GLOBAL_VAR->PMC_RESET_Reg.all);
    wait_clocks(GLOBAL_VAR, 20000);

    /* just to be sure, set SCK to "0"*/
    GLOBAL_VAR->PMC_TCP_PROG_Reg.all = 0;
    GLOBAL_VAR->PMC_TCP_PROG_Reg.Reg.SPI_OE = 1;
    GLOBAL_VAR->PMC_TCP_PROG_Reg.Reg.SPI_MOSI = 0;
    GLOBAL_VAR->PMC_TCP_PROG_Reg.Reg.SPI_CLK = 0;
    PCI_IOReg_write(GLOBAL_VAR, PMC_TCP_PROG_Reg_Adr, GLOBAL_VAR->PMC_TCP_PROG_Reg.all);
    wait_clocks(GLOBAL_VAR, 20000);

    /* ATMEL active*/
    GLOBAL_VAR->PMC_RESET_Reg.Reg.ATMEL_RESx = 1;
    PCI_IOReg_write(GLOBAL_VAR, PMC_RESET_Reg_Adr, GLOBAL_VAR->PMC_RESET_Reg.all);
    wait_clocks(GLOBAL_VAR, 20000);

    /* ATMEL inactive*/
    GLOBAL_VAR->PMC_RESET_Reg.Reg.ATMEL_RESx = 0;
    PCI_IOReg_write(GLOBAL_VAR, PMC_RESET_Reg_Adr, GLOBAL_VAR->PMC_RESET_Reg.all);
    wait_clocks(GLOBAL_VAR, 20000);

    /************************************************************************************************/
    /* Programming Enable
     Read Vendor Code
     Read Part Family / Flash Size
     Part Number
     */

    /* Programming Enable*/
    x = 0;
    Data = 0;
    do
    {
        write_spi(GLOBAL_VAR, SPI_PROGRAM_BYTE0); /* 0xac*/
        write_spi(GLOBAL_VAR, SPI_PROGRAM_BYTE1); /* 0x53*/
        Data = read_spi(GLOBAL_VAR); /* should 0x53*/
        write_spi(GLOBAL_VAR, 0x0); /* x*/

        wait_clocks(GLOBAL_VAR, 25000); /*wait at least 20ms*/
        x++;
    } while( (Data != 0x53) && (x < 32) );

    if( x == 32 )
    {
        AI_Printf("\n!!! ERROR !!! TCP does not response, no SPI access");
        AI_Printf("\n              Data should 0x53, is %x", Data);

        return (FAIL);
    }

    /* Read Vendor Code*/
    Data = 0;

    write_spi(GLOBAL_VAR, SPI_RD_DEVCODE); /* read signature bits*/
    write_spi(GLOBAL_VAR, 0x00); /* x*/
    write_spi(GLOBAL_VAR, SPI_RD_VENDORCODE); /* 0x00*/
    Data = read_spi(GLOBAL_VAR); /* Vendor Code (0x1e=ATMEL)*/
    wait_clocks(GLOBAL_VAR, 5000);

    if( Data == 0 )
    {
        AI_Printf("/n!!! ERROR !!! TCP is locked");
        return (FAIL);
    }
    MonPrintBatchCondition(GLOBAL_VAR, "\nVendorCode: %x (1e=ATMEL), ", Data);

    /* read Part Family, Flash Size*/
    /*Data = 0;*/

    write_spi(GLOBAL_VAR, SPI_RD_DEVCODE); /* read signature bits*/
    write_spi(GLOBAL_VAR, 0x00); /* x*/
    write_spi(GLOBAL_VAR, SPI_RD_PARTFAMILY); /* 0x01*/
    Data = (Data << 8) | read_spi(GLOBAL_VAR); /* Part Family, and Flash Size (9=AVR, 3=2^3kB Flash)*/
    wait_clocks(GLOBAL_VAR, 5000);

    MonPrintBatchCondition(GLOBAL_VAR, "PartFamily: %x (9=AVR), FlashSize: 2^%xkB, ", ((Data & 0xf0) >> 4), (Data & 0xf));

    /* read part Number*/
    write_spi(GLOBAL_VAR, SPI_RD_DEVCODE); /* read signature bits */
    write_spi(GLOBAL_VAR, 0x00); /* x */
    write_spi(GLOBAL_VAR, SPI_RD_PARTNUMBER); /* 0x02 */
    Data = (Data << 8) | read_spi(GLOBAL_VAR); /* Part Number */
    wait_clocks(GLOBAL_VAR, 5000);

    MonPrintBatchCondition(GLOBAL_VAR, "PartNo: %x", (Data & 0xff));

    return (Data);
}

/*****************************************************************************/
/*                                                                           */
/*   Module : ini_prog.c               Submodule : erase_tcp                 */
/*   Author : Hans Zogalla             Project   : API-ASP-MON               */
/*---------------------------------------------------------------------------*/
/*   Create : 08.06.99   Update :  -                                         */
/*---------------------------------------------------------------------------*/
/*    Descriptions                                                           */
/*    ------------                                                           */
/*    Inputs    : -                                                          */
/*                                                                           */
/*    Outputs   : -                                                          */
/*                                                                           */
/*    Description :  Erases all data in the TCP                              */
/*                                                                           */
/*****************************************************************************/
void erase_tcp(TY_MONITOR_DATA_STRUCT *GLOBAL_VAR)
{
    write_spi(GLOBAL_VAR, SPI_ERASE_BYTE0);
    write_spi(GLOBAL_VAR, SPI_ERASE_BYTE1);
    write_spi(GLOBAL_VAR, 0);
    write_spi(GLOBAL_VAR, 0);
    wait_clocks(GLOBAL_VAR, 10000); /*10ms*/
}

#ifdef __monitor__
/**********************************************************************************/
/*                                                                                */
/*   Module : prog_misc.c              Submodule : make_s3rec_line                */
/*   Author : Lamp Matthias            Project   : PMC-1553 Mon                   */
/*--------------------------------------------------------------------------------*/
/*   Create : 18.10.2001   Update :                                               */
/*--------------------------------------------------------------------------------*/
/*    Descriptions                                                                */
/*    ------------                                                                */
/*    Inputs    : AiInt8* line     : String pointer for generated string          */
/*                                   (S-Record)                                   */
/*                AiUInt8* memory  : Pointer to memory area for srec              */
/*                  AiUInt32 address : Startaddress of Memory block for s-rec line*/
/*                AiUInt32 size    : Number of bytes in this line                 */
/*                                                                                */
/*    Outputs   : void                                                            */
/*                                                                                */
/*    Description :  Generates a Motorola Srecord type 3 line if size > 0         */
/*                   or a S7 (Endrecord) if size = 0                              */
/*                                                                                */
/**********************************************************************************/
AiUInt32 make_S3rec_line(AiInt8 *line, AiUInt8 *memory, AiUInt32 address,
            AiUInt32 size)
{
    AiUInt32 i;
    AiUInt8 chksum, byte;

    if( size > 0 )
    sprintf(line, "S3%02X", size + 5);
    else
    sprintf(line, "S7%02X", size + 5);

    sprintf(line + 4, "%08X", address);

    chksum = (AiUInt8) (size + 5 + ((address >> 24) & 0xff)
                + ((address >> 16) & 0xff) + ((address >> 8) & 0xff)
                + (address & 0xff));
    for( i = 0; i < size; i++ )
    {
        byte = *(memory + address + i);
        chksum += byte;
        sprintf(line + (i * 2) + 12, "%02X", byte);
    }
    sprintf(line + (i * 2) + 12, "%02X", (AiUInt8) (0xff - chksum));
    sprintf(line + (i * 2) + 14, "\n");

    return ((i * 2) + 14 + 1);
}

/*****************************************************************************/
/*                                                                           */
/*   Module : prog_misc.c              Submodule : print_s3recs              */
/*   Author : Matthias Lamp            Project   : PMC-1553 MON              */
/*---------------------------------------------------------------------------*/
/*   Create : 18/10/2001   Update :                                          */
/*---------------------------------------------------------------------------*/
/*    Descriptions                                                           */
/*    ------------                                                           */
/*    Inputs    : AiInt8*   memblock : Pointer to memory block, for S-Record */
/*                AiUInt32  memsize  : filesize of memory block              */
/*                                     (in bytes, AiUInt8)                   */
/*                        type     : type of s-rec (BIU, TCP...)             */
/*                                                                           */
/*    Outputs   : AiUInt32  PASS / FIAL                                      */
/*                                                                           */
/*    Description : Asks for S-Record file location and converts memory      */
/*                  block to S3 record (+S7 end record)                      */
/*                                                                           */
/*****************************************************************************/
/* placed static here to avoid stack overflow */
static AiInt8 line[80];
AiUInt32 print_s3recs(TY_MONITOR_DATA_STRUCT *GLOBAL_VAR, AiUInt32 type,
            AiUInt8 *memblock, AiUInt32 memsize)
{
    AiUInt32 caddr, eaddr, cnt, x, temp, mem_adr = 0, mem_size = 0;
    AiUInt8 *memory; /*coded srec*/

    temp = GLOBAL_VAR->GLOBAL_TEMP_MEM_SIZE / 2;
    memory = (GLOBAL_VAR->GLOBAL_TEMP_MEM + temp);

    caddr = 0;
    eaddr = memsize;

    MON_ONLY_PRINTF("\n");
    x = 0;
    while( caddr < eaddr )
    {
        if( (eaddr - caddr) >= 21 )
        cnt = 21;
        else
        cnt = eaddr - caddr;

        mem_size = make_S3rec_line(line, memblock, caddr, cnt);
        caddr += cnt;

        sprintf((char*) (memory + mem_adr), "%s", line);

        mem_adr += mem_size;

        MonPrintBatchCondition(GLOBAL_VAR, "."); /* a dot each line*/
        x++;
    }
    /* Make End Record (S7) */
    mem_size = make_S3rec_line(line, memblock, 0, 0);

    sprintf((char*) (memory + mem_adr), "%s", line);
    mem_adr += mem_size;

    if( Save_Code(GLOBAL_VAR, type, memory, mem_adr) != PASS )
    {
        MON_ONLY_PRINTF("\n!!! ERROR !!! in saving S-record");
        return (FAIL);
    }

    MonPrintBatchCondition(GLOBAL_VAR, "\nS-Record (S3) successfully created");

    return (PASS);
}

#endif 

/*****************************************************************************/
/*                                                                           */
/*   Module : ini_misc.c               Submodule : ShowBuffer                */
/*   Author : Hans Zogalla             Project   : API-ASP-MON               */
/*---------------------------------------------------------------------------*/
/*   Create : 27.02.98   Update : 05.09.00 hz 01.03                          */
/*                                05/11/2001 changed for PMC                 */
/*                                29/07/2002 ASP read added (v01.01, ML)     */
/*---------------------------------------------------------------------------*/
/*    Descriptions                                                           */
/*    ------------                                                           */
/*    Inputs    : buf:   Memory Pointer                                      */
/*                count: Nr. of Word to be DUMPED to screen                  */
/*                  type:  0=GlobalRam, 1=IO-Register                          */
/*                                                                           */
/*    Outputs   : -                                                          */
/*                                                                           */
/*    Description :  Displays Count number of words on the console.          */
/*                   Implements the DUMP Memory Function. Display Format is: */
/*                                                                           */
/*  aaaaaaaa: dddddddd ddddddddd ddddddddd dddddddd  * TextTextTextText *    */
/*                                                                           */
/*  00000cnt: ... if count=64! (special case for NOVRam Read) !!             */
/*  v01.03 if count>0x10000 addr= (i*4) + (count>>16) &0xfffc, use 0x10000   */
/*  to enable function with address 0                                        */
/*                                                                           */
/*    NOVRAM DUMP not yet implemented ML                                     */
/*****************************************************************************/
/* placed static here to avoid stack overflow */
static AiInt8 AsciiStr[20];
void ShowBuffer(TY_MONITOR_DATA_STRUCT *GLOBAL_VAR, AiUInt32 buf, AiUInt32 count, AiUInt32 type)
{
    AiUInt32 i, j, k = 0, val;
    AiUInt32 cnt; /* ,tmp*/
    AiUInt32 ibuf_p;
    AiInt8 ch;
    char buffer[100];
    char buffer2[100];
    cnt = count & 0xffff;
    AsciiStr[0] = ZERO;

    if( (type == 0) || /* read PMC memory    */
    (type == 1) || /* read IO Register   */
    (type == 2) )
    {
        /* read ASP local ram */
        ibuf_p = buf & 0xfffffffc;
    }
    else
    {
        /* no valid type    */
        return;
    }

    for( i = 0; i < cnt; i++ )
    {
        if( (i % 4) == 0 )
        {
            if( k > 0 )
            {
                AsciiStr[k] = ZERO;
                sprintf(buffer2, "%s  * %s *\r\n", buffer, AsciiStr);
                AI_Printf("%s", buffer2);
                k = 0;
            }
            sprintf(buffer, "%08x: ", (ibuf_p * 4));
        }
        if( type == 0 )
            val = PCI_Mem_read(GLOBAL_VAR, ibuf_p); /* read PMC memory  */
        else if( type == 1 )
            val = PCI_IOReg_read(GLOBAL_VAR, ibuf_p); /* read IO Register */
        else if( type == 2 )
            val = PCI_ASPRam_read(GLOBAL_VAR, ibuf_p); /* read ASP memory  */

        for( j = 0; j < 4; j++ )
        {
            ch = (AiInt8) (val >> (24 - (j * 8)));
            if( (ch < ' ') || (ch > '~') )
                ch = '.';
            AsciiStr[k] = ch;
            k++;
        }
        sprintf(buffer2, "%s%08x ", buffer, val);
        sprintf(buffer, "%s", buffer2);

        ibuf_p++;
    }
    AsciiStr[k] = ZERO;

    sprintf(buffer2, "%s  * %s *\r\n", buffer, AsciiStr);
    AI_Printf("%s", buffer2);
}

/*****************************************************************************/
/*                                                                           */
/*   Module : main.c                   Submodule : start_hardware            */
/*   Author : Matthias Lamp            Project   : PMC-1553 MON              */
/*---------------------------------------------------------------------------*/
/*   Create : 29/10/2001   Update : 28.10.2002 (ASP added)                   */
/*                                  29.08.2013 ML, MMA NOVRAM restore        */
/*---------------------------------------------------------------------------*/
/*    Descriptions                                                           */
/*    ------------                                                           */
/*    Inputs    :   TY_MONITOR_DATA_STRUCT GLOBAL_VAR                        */
/*                        I/O routines boot place for global variables       */
/*    Outputs   : AiUInt32  PASS / FIAL                                      */
/*                                                                           */
/*    Description : a number of functions to start the PMC module            */
/*                                                                           */
/*****************************************************************************/
AiUInt32 start_hardware(TY_MONITOR_DATA_STRUCT *GLOBAL_VAR)
{
    AiUInt32 temp;
    AiUInt8 *data, *memory;
    AiUInt32 size = (8 * 1024);
    AiUInt32 i;

    data = (GLOBAL_VAR->GLOBAL_TEMP_MEM);
    memset(data, 0, 512); /* set buffer (allocated memory) to "0" */

    memory = (GLOBAL_VAR->GLOBAL_TEMP_MEM + 512);
    memset(memory, 0, size); /* set buffer (allocated memory) to "0" */

    /***********************************************************/
    /*    16.08.2013 MMA    */
    /*    Check NOVRAM Values before they are used    */

    temp = get_flash_mirror(GLOBAL_VAR, data); /* indicates valid NOVRAM mirrow in TCP FLASH*/
    for (i=0; i<0x200; i+=0x10)
    {
       AI_Printf("\n0x%08x: %02x%02x%02x%02x %02x%02x%02x%02x %02x%02x%02x%02x %02x%02x%02x%02x",i , data[i+ 3], data[i+ 2], data[i+ 1], data[i+ 0] \
                                                                                                   , data[i+ 7], data[i+ 6], data[i+ 5], data[i+ 4] \
                                                                                                   , data[i+11], data[i+10], data[i+ 9], data[i+ 8] \
                                                                                                   , data[i+15], data[i+14], data[i+13], data[i+12] );
    }
	
    if( temp == PASS )
    {
        AI_Printf("\nvalid FLASH Mirror Exists");
        read_board_values_flash(GLOBAL_VAR); /* v01.08, reads via SPI serial number, config */
    }
    else
    {
        AI_Printf("\nNO valid FLASH Mirror Exists, must be created manually!!!");
        AI_Printf("\nread values from NOVRam");
        read_board_values(GLOBAL_VAR); /* v01.08, reads via SPI serial number, config */
    }

    /***********************************************************/
     /*    16.08.2013 MMA    */
    /*    Check NOVRAM Values before they are used    */
    /*    END Modification    */
    /***********************************************************/

    GLOBAL_VAR->PMC_RESET_Reg.all = 0;
    PCI_IOReg_write(GLOBAL_VAR, PMC_RESET_Reg_Adr, GLOBAL_VAR->PMC_RESET_Reg.all); /*all devices in reset*/

    GLOBAL_VAR->PMC_JTAG_KEY_Reg = JTAG_Key; /*enable write access to JTAG Port*/
    PCI_IOReg_write(GLOBAL_VAR, PMC_JTAG_KEY_Reg_Adr, GLOBAL_VAR->PMC_JTAG_KEY_Reg);

    GLOBAL_VAR->PMC_JTAG_Reg.Reg.JTAG_OE = 0; /*disable JTAG port*/
    PCI_IOReg_write(GLOBAL_VAR, PMC_JTAG_Reg_Adr, GLOBAL_VAR->PMC_JTAG_Reg.all);

    GLOBAL_VAR->NVR_PROG_EN_Reg.all = 0;
    GLOBAL_VAR->BOOT_CONTROL_Reg.all = 0;

    GLOBAL_VAR->clk_1us = CLK_1US;

    wait_clocks(GLOBAL_VAR, 1000);

    temp = read_PCIFPGA_REG_PMC(GLOBAL_VAR); /* PCI_FPGA register*/

    MON_ONLY_PRINTF("PMC_FPGA_Reg      %08x\n", GLOBAL_VAR->PMC_FPGA_Reg.all); MON_ONLY_PRINTF("PMC_COUNTER_Reg   %08x\n", GLOBAL_VAR->PMC_COUNTER_Reg); MON_ONLY_PRINTF("PMC_IRQ_EVENT_Reg %08x\n",
                GLOBAL_VAR->PMC_IRQ_EVENT_Reg.all); MON_ONLY_PRINTF("PMC_IRQ_MASK_Reg  write only\n"); MON_ONLY_PRINTF("PMC_IRQ_VEC_Reg   %08x\n", GLOBAL_VAR->PMC_IRQ_VEC_Reg); MON_ONLY_PRINTF("PMC_JTAG_Reg      %08x\n", GLOBAL_VAR->PMC_JTAG_Reg.all); MON_ONLY_PRINTF("PMC_JTAG_KEY_Reg  write only\n"); MON_ONLY_PRINTF("PMC_RESET_Reg     write only\n"); MON_ONLY_PRINTF("PMC_TCP_DATA_Reg  %08x\n",
                GLOBAL_VAR->PMC_TCP_DATA_Reg.all); MON_ONLY_PRINTF("PMC_TCP_PROG_Reg  %08x\n",
                GLOBAL_VAR->PMC_TCP_PROG_Reg.all); MON_ONLY_PRINTF("PMC_TTHIGH_Reg    %08x\n", GLOBAL_VAR->PMC_TTHIGH_Reg.all); MON_ONLY_PRINTF("PMC_TTLOW_Reg     %08x\n", GLOBAL_VAR->PMC_TTLOW_Reg.all); MON_ONLY_PRINTF("PMC_VERSION_Reg   %08x\n",
                GLOBAL_VAR->PMC_VERSION_Reg.all); MON_ONLY_PRINTF("PMC_MAILBOX_Reg   %08x\n",
                GLOBAL_VAR->PMC_MAILBOX_Reg.all); MON_ONLY_PRINTF("PMC_MBOX_cnt_Reg  %08x\n",
                GLOBAL_VAR->PMC_MBOX_cnt_Reg.all); MON_ONLY_PRINTF("PMC_DEVICEID_Reg  %08x\n",
                GLOBAL_VAR->PMC_DEVICEID_Reg.all);

    if( temp == FAIL )
    {
        GLOBAL_VAR->BOOT_CONTROL_Reg.Reg.PCI_REG = 0; /* already 0*/
        AI_Printf("\n !!! ERROR !!! PCI-FPGA IO-Register not found");
        return (FAIL);
    }
    else
    {
        AI_Printf("\nPCI-FPGA IO-Register found");
    }
    IRIG_INTERN(GLOBAL_VAR); /* IRIG EXTERN, v01.08, ML, 17/09/04 */

    /* now RESET all divices*/
    GLOBAL_VAR->PMC_RESET_Reg.Reg.IRIG_INTx = 0; /* IRIG coupled to INTERN*/
    GLOBAL_VAR->PMC_RESET_Reg.Reg.ATMEL_RESx = 0; /* ATMEL in RESET*/
    GLOBAL_VAR->PMC_RESET_Reg.Reg.BIP1_RESx = 0; /* BIP 1 in RESET*/
    GLOBAL_VAR->PMC_RESET_Reg.Reg.BIP2_RESx = 0; /* BIP 2 in RESET*/
    GLOBAL_VAR->PMC_RESET_Reg.Reg.FPGA2_RESx = 0; /* FPGA 2 in RESET*/
    GLOBAL_VAR->PMC_RESET_Reg.Reg.IRIG_RESx = 0; /* IRIG disabled*/
    GLOBAL_VAR->PMC_RESET_Reg.Reg.ASP_RESx = 0; /* ASP in RESET*/
    GLOBAL_VAR->PMC_RESET_Reg.Reg.SDRAM_start = 0; /* disable SDRAM controller*/
    PCI_IOReg_write(GLOBAL_VAR, PMC_RESET_Reg_Adr, GLOBAL_VAR->PMC_RESET_Reg.all);

    /* v01.01*/
    /* depending on DEVICE ID boot ASP*/
    /*    if(GLOBAL_VAR->PMC_DEVICEID_Reg.device_id==devID_ASP)*/
    if( (GLOBAL_VAR->PMC_DEVICEID_Reg.Reg.device_id == dveID_PMC32_ASP) || (GLOBAL_VAR->PMC_DEVICEID_Reg.Reg.device_id == dveID_PMC64_ASP) )
        GLOBAL_VAR->BOOT_CONTROL_Reg.Reg.BOOT_ASP = 1; /* boot ASP if set*/

    /* v01.01 change the boot order, BIU_DONE pin*/
    /* v01.05 PC card has no IO-FPGA to boot, only leaf Reset*/
    if( GLOBAL_VAR->PMC_DEVICEID_Reg.Reg.device_id == dveID_PCCard_1553 )
    {
        GLOBAL_VAR->PMC_RESET_Reg.Reg.FPGA1_RESx = 1; /* FPGA 1 out of Reset*/
        GLOBAL_VAR->PMC_RESET_Reg.Reg.FPGA2_RESx = 1; /* FPGA 2 out of Reset*/
        PCI_IOReg_write(GLOBAL_VAR, PMC_RESET_Reg_Adr, GLOBAL_VAR->PMC_RESET_Reg.all); /* resets the BIP    */

        GLOBAL_VAR->BOOT_CONTROL_Reg.Reg.FPGA = 1; /* status bit FPGA booted*/
        AI_Printf("\n IO-FPGA 1 and 2 started");
    }
    else
    /* no PC-Card*/
    {
        temp = boot_IOFPGA_PMC(GLOBAL_VAR); /* boot IO-FPGAs  if mem test OK*/
        if( temp == FAIL )
        {
            GLOBAL_VAR->BOOT_CONTROL_Reg.Reg.FPGA = 0; /* already 0*/
            AI_Printf("\n !!! ERROR !!! IO-FPGA boot failed");
            return (FAIL);
        }
        else
        {
            AI_Printf("\nIO-FPGA boot passed");
        }
    } /* else, no PC-Card*/

    AI_Printf("start ATMEL...\n");

    temp = start_ATMEL(GLOBAL_VAR); /* start ATMEL if PCI Reg OK*/

    AI_Printf("End ATMEL...\n");
    if( temp == FAIL )
    {
        GLOBAL_VAR->BOOT_CONTROL_Reg.Reg.ATMEL_OK = 0; /* already 0*/
        AI_Printf("\n !!! ERROR !!! TCP (ATMEL) not found");
        return (FAIL);
    }
    else
    {
        AI_Printf("\nTCP (ATMEL) found");
    }

    temp = read_NOVRAM_VAL_PMC(GLOBAL_VAR); /* read NOVRAM if ATMEL OK*/
    if( temp == FAIL )
    {
        GLOBAL_VAR->BOOT_CONTROL_Reg.Reg.NOVRam_Reg = 0; /* already 0*/
        AI_Printf("\n !!! ERROR !!! invalid NOVRAM");
        return (FAIL);
    }
    else
    {
        AI_Printf("\nNOVRAM OK");
    }

    /* V02.12, ML, 23.02.07*/
    AI_Printf("\nProgram PMC_IRIG_SAMPLE Register with NOVRAM value");

    if( GLOBAL_VAR->NVR_IRIG_OFFSET.Reg.sign == 0 )
        /*sign bit=0 => poseitiv*/
        GLOBAL_VAR->PMC_IRIG_SAMPLE_Reg.Reg.sample = 250 + GLOBAL_VAR->NVR_IRIG_OFFSET.Reg.offset;
    else
        /*sign bit=1 => negativ*/
        GLOBAL_VAR->PMC_IRIG_SAMPLE_Reg.Reg.sample = 250 - GLOBAL_VAR->NVR_IRIG_OFFSET.Reg.offset;

    PCI_IOReg_write(GLOBAL_VAR, PMC_IRIG_SAMPLE_Reg_Adr, GLOBAL_VAR->PMC_IRIG_SAMPLE_Reg.all);
    /*end*/

    /* if BIU 2 =ff (not present, or BIU 2=00 reserved / unset)*/
    if( (GLOBAL_VAR->NVR_BOARD_TYPE.Reg.BIU2_type != 0xff) && (GLOBAL_VAR->NVR_BOARD_TYPE.Reg.BIU2_type != 0x00) )
    {
        MON_ONLY_PRINTF("\nBIU2 present");
        GLOBAL_VAR->BOOT_CONTROL_Reg.Reg.BOOT_BIU2 = 1; /* boot BIU 2 if set*/
    }
    temp = global_RAM_SIZE_PMC(GLOBAL_VAR); /* check (size) Global Ram*/
    if( temp == FAIL )
    {
        GLOBAL_VAR->RAM_Size_Reg.Reg.GRam_Size = 0; /* already 0*/
        AI_Printf("\n !!! ERROR !!! no GLOBAL RAM found");
        return (FAIL);
    }
    else
    {
        MON_ONLY_PRINTF("Found GLOBAL RAM\n");
        /*        if(GLOBAL_VAR->PMC_DEVICEID_Reg.device_id==devID_ASP)*/
        if( (GLOBAL_VAR->PMC_DEVICEID_Reg.Reg.device_id == dveID_PMC32_ASP)
                    || (GLOBAL_VAR->PMC_DEVICEID_Reg.Reg.device_id == dveID_PMC64_ASP) )
        {
            /*ASP*/
            AI_Printf("\n%d MB GLOBAL RAM RAM found", GLOBAL_VAR->RAM_Size_Reg.Reg.GRam_Size);
            AI_Printf("\n%d MB Local ASP RAM found", GLOBAL_VAR->RAM_Size_Reg.Reg.Local_ASPRam);
        }
        else
        /*no ASP*/
        {
            AI_Printf("\n%x MB GLOBAL RAM found", GLOBAL_VAR->RAM_Size_Reg.Reg.GRam_Size);
        }
    }
    temp = global_RAM_TEST_PMC(GLOBAL_VAR); /* Test memory if check OK*/
    if( temp == FAIL )
    {
        /*        GLOBAL_VAR->BOOT_CONTROL_Reg.MEM_Test =0;  already 0*/
        AI_Printf("\n !!! ERROR !!! GLOBAL RAM test failed");
        /*        return(FAIL);*/
    }
    else
    {
        /*        GLOBAL_VAR->BOOT_CONTROL_Reg.MEM_Test=1;  PCI Register found*/
        AI_Printf("\nGLOBAL RAM test passed");
    }

    temp = boot_BIP1_PMC(GLOBAL_VAR); /* boot BIP 1*/
    if( temp == FAIL )
    {
        GLOBAL_VAR->BOOT_CONTROL_Reg.Reg.BIP_1_boot = 0; /* already 0*/
        AI_Printf("\n !!! ERROR !!! BIP 1 boot failed");
        return (FAIL);
    }
    else
    {
        AI_Printf("\nBIP 1 boot passed");
    }
    if( (GLOBAL_VAR->BOOT_CONTROL_Reg.Reg.BOOT_BIU2 == 1) && (GLOBAL_VAR->PMC_DEVICEID_Reg.Reg.dev_type_BIP2 != 0xff) )
    /*BIU 2 present*/
    {
        temp = boot_BIP2_PMC(GLOBAL_VAR); /* boot BIP 2 if FPGA 2 */
        if( temp == FAIL )
        {
            GLOBAL_VAR->BOOT_CONTROL_Reg.Reg.BIP_2_boot = 0; /* already 0*/
            AI_Printf("\n !!! ERROR !!! BIP 2 boot failed");
            return (FAIL);
        }
        else
        {
            AI_Printf("\nBIP 2 boot passed");
        }
    }

    temp = start_BIP1_PMC(GLOBAL_VAR); /* start BIP 1 if booted */
    if( temp == FAIL )
    {
        GLOBAL_VAR->BOOT_CONTROL_Reg.Reg.BIP_1_start = 0; /* already 0*/
        AI_Printf("\n !!! ERROR !!! BIP 1 start failed");
        return (FAIL);
    }
    else
    {
        AI_Printf("\nBIP 1 start passed");
    }
    /*    ShowBuffer(GLOBAL_VAR, 0, 32,0);        0 = type memory */

    if( (GLOBAL_VAR->BOOT_CONTROL_Reg.Reg.BOOT_BIU2 == 1) && (GLOBAL_VAR->PMC_DEVICEID_Reg.Reg.dev_type_BIP2 != 0xff) )
    /*BIU 2 booted*/
    {
        temp = start_BIP2_PMC(GLOBAL_VAR); /* start BIP 2 if booted */
        if( temp == FAIL )
        {
            GLOBAL_VAR->BOOT_CONTROL_Reg.Reg.BIP_2_start = 0; /* already 0*/
            AI_Printf("\n !!! ERROR !!! BIP 2 start failed");
            return (FAIL);
        }
        else
        {
            GLOBAL_VAR->BOOT_CONTROL_Reg.Reg.BIP_2_start = 1; /* BIP 2  start OK*/
            AI_Printf("\nBIP 2 start passed");
        }
        /*        ShowBuffer(GLOBAL_VAR, 0x4000, 32,0);        0 = type memory */
    }
    /* v01.01 ASP*/
    AI_Printf("\nBOOT_ASP:%d", GLOBAL_VAR->BOOT_CONTROL_Reg.Reg.BOOT_ASP);
    AI_Printf("\nGLOBAL_VAR->DoNotBootPMCAsp %ld", GLOBAL_VAR->DoNotBootPMCAsp);

    if( GLOBAL_VAR->BOOT_CONTROL_Reg.Reg.BOOT_ASP == 1 )
    /* boot ASP if set*/
    {
        if( 0 != GLOBAL_VAR->DoNotBootPMCAsp )
            temp = PASS;
        else
            temp = boot_ASP_PMC(GLOBAL_VAR);
        /* boot ASP*/
        if( temp == FAIL )
        {
            GLOBAL_VAR->BOOT_CONTROL_Reg.Reg.ASP_boot = 0; /* already 0*/
            AI_Printf("\n !!! ERROR !!! ASP boot failed");
            return (FAIL);
        }
        else
        {
            AI_Printf("\nASP boot passed");
        }
        /*        ShowBuffer(GLOBAL_VAR, 0x3ff0000, 32,2);        2 = type memory */
    }

#ifdef monitor
    for (i = 0; i < 1000; i++)
    wait_clocks(GLOBAL_VAR, 100);
#endif 
    /* v01.01 ASP*/
    if( GLOBAL_VAR->BOOT_CONTROL_Reg.Reg.BOOT_ASP == 1 )
    /*ASP booted*/
    {
        temp = start_ASP_PMC(GLOBAL_VAR); /* start ASP if booted */
        if( temp == FAIL )
        {
            GLOBAL_VAR->BOOT_CONTROL_Reg.Reg.ASP_start = 0; /* already 0*/
            AI_Printf("\n !!! ERROR !!! ASP start failed");
            return (FAIL);
        }
        else
        {
            AI_Printf("\nASP start passed");
        }
    }

#ifdef monitor
    for (i = 0; i < 10; i++)
    wait_clocks(GLOBAL_VAR, 100);
#endif 

    IRIG_EXTERN(GLOBAL_VAR); /* IRIG intern, v01.08, ML, 17/09/04 */

    GLOBAL_VAR->BOOT_CONTROL_Reg.Reg.PMC_BOOT_OK = 1;

#ifdef monitor
    PUTCHAR(BEL);
#endif

    AI_Printf("\nPMC Module successful booted and started");

    return (PASS);
}

/*****************************************************************************/
/*                                                                           */
/*   Module : main.c                   Submodule : IRIG_EXTERN               */
/*   Author : Matthias Lamp            Project   : PMC-1553 MON              */
/*---------------------------------------------------------------------------*/
/*   Create : 17/09/2004   Update :   .  .                                   */
/*---------------------------------------------------------------------------*/
/*    Descriptions                                                           */
/*    ------------                                                           */
/*    Inputs    :   TY_MONITOR_DATA_STRUCT GLOBAL_VAR                        */
/*                                                                           */
/*    Outputs   :   none                                                     */
/*                                                                           */
/*    Description : switch IRIG decoder to EXTERN                            */
/*                                                                           */
/*****************************************************************************/
void IRIG_EXTERN(TY_MONITOR_DATA_STRUCT *GLOBAL_VAR)
{
    GLOBAL_VAR->PMC_RESET_Reg.Reg.IRIG_INTx = 0; /* IRIG coupled to EXTERN */
    PCI_IOReg_write(GLOBAL_VAR, PMC_RESET_Reg_Adr, GLOBAL_VAR->PMC_RESET_Reg.all);

    AI_Printf("\nIRIG decoder switched to EXTERN");

    return;
}

/*****************************************************************************/
/*                                                                           */
/*   Module : main.c                   Submodule : IRIG_INTERN               */
/*   Author : Matthias Lamp            Project   : PMC-1553 MON              */
/*---------------------------------------------------------------------------*/
/*   Create : 17/09/2004   Update :   .  .                                   */
/*---------------------------------------------------------------------------*/
/*    Descriptions                                                           */
/*    ------------                                                           */
/*    Inputs    :   TY_MONITOR_DATA_STRUCT GLOBAL_VAR                        */
/*                                                                           */
/*    Outputs   :   none                                                     */
/*                                                                           */
/*    Description : switch IRIG decoder to INTERN                            */
/*                                                                           */
/*****************************************************************************/
void IRIG_INTERN(TY_MONITOR_DATA_STRUCT *GLOBAL_VAR)
{
    GLOBAL_VAR->PMC_RESET_Reg.Reg.IRIG_INTx = 1; /* IRIG coupled to INTERN */
    PCI_IOReg_write(GLOBAL_VAR, PMC_RESET_Reg_Adr, GLOBAL_VAR->PMC_RESET_Reg.all);

    AI_Printf("\nIRIG decoder switched to INTERN");
    return;
}

/*****************************************************************************/
/*                                                                           */
/*   Module : main.c                   Submodule : RD_IRIG_SWITCH            */
/*   Author : Matthias Lamp            Project   : PMC-1553 MON              */
/*---------------------------------------------------------------------------*/
/*   Create : 17/09/2004   Update :   .  .                                   */
/*---------------------------------------------------------------------------*/
/*    Descriptions                                                           */
/*    ------------                                                           */
/*    Inputs    :   TY_MONITOR_DATA_STRUCT GLOBAL_VAR                        */
/*                                                                           */
/*    Outputs   :   0 = IRIG INTERN                                          */
/*                  1 = IRIG EXTERN                                          */
/*                                                                           */
/*    Description : read the status of the IRIG switch                       */
/*                                                                           */
/*****************************************************************************/
AiUInt32 RD_IRIG_SWITCH(TY_MONITOR_DATA_STRUCT *GLOBAL_VAR)
{
    union pmc_reset_reg resetreg;

    // TY_MONITOR_DATA_STRUCT dummy_GLOB_VAR;
    resetreg.all = PCI_IOReg_read(GLOBAL_VAR,PMC_RESET_Reg_Adr);

    if( resetreg.Reg.IRIG_INTx == 1 )
    {
        AI_Printf("\nIRIG decoder coupled to INTERN");
    }
    else
    {
        AI_Printf("\nIRIG decoder coupled to EXTERN");
    }

    return (resetreg.Reg.IRIG_INTx);
}

/*****************************************************************************/
/*                                                                           */
/*   Module : prog_misc.c              Submodule : get_ILCA1_EXT             */
/*   Author : Matthias Lamp            Project   : PMC-1553 MON              */
/*---------------------------------------------------------------------------*/
/*   Create : 24.02.06, V02.00         Update :   .  .                       */
/*---------------------------------------------------------------------------*/
/*    Descriptions                                                           */
/*    ------------                                                           */
/*    Inputs    :   TY_MONITOR_DATA_STRUCT GLOBAL_VAR                        */
/*                                                                           */
/*    Outputs   :   return(FileExtension)                                    */
/*                                                                           */
/*    Description : define a valid file extension for LCA1 file TYPE         */
/*                                                                           */
/*****************************************************************************/
AiUInt32 get_ILCA1_EXT(TY_MONITOR_DATA_STRUCT *GLOBAL_VAR)
{
    AiUInt32 biu_1, platform;

    biu_1 = GLOBAL_VAR->NVR_BOARD_TYPE.Reg.BIU1_type;
    platform = GLOBAL_VAR->NVR_BOARD_CONFIG.Reg.PLATFORM;

    MON_ONLY_PRINTF("\nNo valid NVR_ILCA1EX value found, calculate parameter");

    if( (biu_1 & 0xf0) == 0x10 )
        return (0x0010);
    /*AMC1553 IO-LCA, default*/
    else if( (biu_1 & 0xf0) == 0x20 )
        return (0x0020);
    /*AMC429-16 IO-LCA, default     (ML <  V02.00)*/
    else if( (biu_1 & 0xf0) == 0x60 )
        return (0x0060);
    /*AMC-FDX/ API-FDX-2 V2 IO-LCA, TAB default (XC2S300E)*/
    else
        return (0xffff);
}

/*****************************************************************************/
/*                                                                           */
/*   Module : prog_misc.c              Submodule : get_ILCA2_EXT             */
/*   Author : Matthias Lamp            Project   : PMC-1553 MON              */
/*---------------------------------------------------------------------------*/
/*   Create : 24.02.06, V02.00         Update :   .  .                       */
/*---------------------------------------------------------------------------*/
/*    Descriptions                                                           */
/*    ------------                                                           */
/*    Inputs    :   TY_MONITOR_DATA_STRUCT GLOBAL_VAR                        */
/*                                                                           */
/*    Outputs   :   return(FileExtension)                                    */
/*                                                                           */
/*    Description : define a valid file extension for LCA2 file TYPE         */
/*                                                                           */
/*****************************************************************************/
AiUInt32 get_ILCA2_EXT(TY_MONITOR_DATA_STRUCT *GLOBAL_VAR)
{
    AiUInt32 biu_2, platform;

    biu_2 = GLOBAL_VAR->NVR_BOARD_TYPE.Reg.BIU2_type;
    platform = GLOBAL_VAR->NVR_BOARD_CONFIG.Reg.PLATFORM;

    MON_ONLY_PRINTF("\nNo valid NVR_ILCA2EX value found, calculate parameter");

    if( (biu_2 & 0xf0) == 0x10 )
        return (0x0010);
    /*AMC1553 IO-LCA, default*/
    else if( (biu_2 & 0xf0) == 0x20 )
        return (0x0020);
    /*AMC429-16 IO-LCA, default     (ML <  V02.00)*/
    else if( (biu_2 & 0xf0) == 0x60 )
        return (0x0060);
    /*AMC-FDX/ API-FDX-2 V2 IO-LCA, TAB default (XC2S300E)*/
    else
        return (0xffff);
}

/*****************************************************************************/
/*                                                                           */
/*   Module : prog_misc.c              Submodule : get_BIP1_EXT              */
/*   Author : Matthias Lamp            Project   : PMC-1553 MON              */
/*---------------------------------------------------------------------------*/
/*   Create : 24.02.06, V02.00         Update :   .  .                       */
/*---------------------------------------------------------------------------*/
/*    Descriptions                                                           */
/*    ------------                                                           */
/*    Inputs    :   TY_MONITOR_DATA_STRUCT GLOBAL_VAR                         */
/*                                                                             */
/*    Outputs   :   return(FileExtension)                                    */
/*                                                                           */
/*    Description : define a valid file extension for BIP1 file TYPE         */
/*                                                                           */
/*****************************************************************************/
AiUInt32 get_BIP1_EXT(TY_MONITOR_DATA_STRUCT *GLOBAL_VAR)
{
    AiUInt32 biu_1, platform;

    biu_1 = GLOBAL_VAR->NVR_BOARD_TYPE.Reg.BIU1_type;
    platform = GLOBAL_VAR->NVR_BOARD_CONFIG.Reg.PLATFORM;

    MON_ONLY_PRINTF("\nNo valid NVR_BIP1EX value found, calculate parameter");

    if( (biu_1 & 0xf0) == 0x10 )
        return (0x0010);
    /*FW1553, default*/
    else if( (biu_1 & 0xf0) == 0x20 )
        return (0x0020);
    /*FW429, default (ML <  V02.00)*/
    else if( (biu_1 & 0xf0) == 0x60 )
        return (0x0060);
    /*FW-AFDX default*/
    else
        return (0xffff);
}

/*****************************************************************************/
/*                                                                           */
/*   Module : prog_misc.c              Submodule : get_BIP2_EXT              */
/*   Author : Matthias Lamp            Project   : PMC-1553 MON              */
/*---------------------------------------------------------------------------*/
/*   Create : 24.02.06, V02.00         Update :   .  .                       */
/*---------------------------------------------------------------------------*/
/*    Descriptions                                                           */
/*    ------------                                                           */
/*    Inputs    :   TY_MONITOR_DATA_STRUCT GLOBAL_VAR                        */
/*                                                                           */
/*    Outputs   :   return(FileExtension)                                    */
/*                                                                           */
/*    Description : define a valid file extension for BIP2 file TYPE         */
/*                                                                           */
/*****************************************************************************/
AiUInt32 get_BIP2_EXT(TY_MONITOR_DATA_STRUCT *GLOBAL_VAR)
{
    AiUInt32 biu_2, platform;

    biu_2 = GLOBAL_VAR->NVR_BOARD_TYPE.Reg.BIU2_type;
    platform = GLOBAL_VAR->NVR_BOARD_CONFIG.Reg.PLATFORM;

#ifdef __monitor__
    MON_ONLY_PRINTF("\nNo valid NVR_BIP2EX value found, calculate parameter");
#endif 

    if( (biu_2 & 0xf0) == 0x10 )
        return (0x0010); /*FW1553, default*/
    else if( (biu_2 & 0xf0) == 0x20 )
        return (0x0020); /*FW429, default (ML <  V02.00)*/
    else if( (biu_2 & 0xf0) == 0x60 )
        return (0x0060); /*FW-AFDX default*/
    else
        return (0xffff);
}

/*****************************************************************************/
/*                                                                           */
/*   Module : prog_misc.c              Submodule : get_PLCA_EXT              */
/*   Author : Matthias Lamp            Project   : PMC-1553 MON              */
/*---------------------------------------------------------------------------*/
/*   Create : 24.02.06, V02.00         Update :   .  .                       */
/*---------------------------------------------------------------------------*/
/*    Descriptions                                                           */
/*    ------------                                                           */
/*    Inputs    :   TY_MONITOR_DATA_STRUCT GLOBAL_VAR                        */
/*                                                                           */
/*    Outputs   :   return(FileExtension)                                    */
/*                                                                           */
/*    Description : define a valid file extension for PLCA file TYPE         */
/*                                                                           */
/*****************************************************************************/
AiUInt32 get_PLCA_EXT(TY_MONITOR_DATA_STRUCT *GLOBAL_VAR)
{
    AiUInt32 biu_1, biu_2, platform;

    biu_1 = GLOBAL_VAR->NVR_BOARD_TYPE.Reg.BIU1_type;
    biu_2 = GLOBAL_VAR->NVR_BOARD_TYPE.Reg.BIU2_type;
    platform = GLOBAL_VAR->NVR_BOARD_CONFIG.Reg.PLATFORM;

    MON_ONLY_PRINTF("\nNo valid NVR_PLCAEX value found, calculate parameter");

    if( platform == 0x50 )
    {
        if( biu_2 != 0xff )
            /* 32bit PCI-LCA, without ASP, both BIU's */
            return (0x0001);
        else
            /* 32bit PCI-LCA, without ASP, only one BIU */
            return (0x0002);
    }
    else if( platform == 0xa0 )
        /* 32bit PCI-LCA, special type for USB devices, 2 BIU's*/
        return (0x0008);
    else if( platform == 0x52 )
        /* 32bit PCI-LCA, with ASP */
        return (0x0010);
    else if( platform == 0x54 )
        /* 64bit PCI-LCA, without ASP */
        return (0x0020);
    else if( (platform == 0x56) || /* AMC / API-V2 */
    (platform == 0x60) )
        /* 64bit PCI-LCA, with ASP */
        return (0x0030);
    else if( platform == 0x70 )
    /* PC-Card */
    {
        if( (biu_1 & 0xf0) == 0x10 )
            return (0x1001);
        /* APM1553 PC-Card Interface-LCA */
        else if( (biu_1 & 0xf0) == 0x20 )
            /* APM429 PC-Card Interface-LCA */
            return (0x2001);
        else if( (biu_1 & 0xf0) == 0x60 )
            /* APM-FDX PC-Card Interface-LCA */
            return (0x6001);
        else
            /* unkonwn PC-Card */
            return (0xffff);
    }
    else
        /* Device not present */
        return (0xffff);
}

/*****************************************************************************/
/*                                                                           */
/*   Module : prog_misc.c              Submodule : get_TCP_EXT               */
/*   Author : Matthias Lamp            Project   : PMC-1553 MON              */
/*---------------------------------------------------------------------------*/
/*   Create : 24.02.06, V02.00         Update :   .  .                       */
/*---------------------------------------------------------------------------*/
/*    Descriptions                                                           */
/*    ------------                                                           */
/*    Inputs    :   TY_MONITOR_DATA_STRUCT GLOBAL_VAR                        */
/*                                                                           */
/*    Outputs   :   return(FileExtension)                                    */
/*                                                                           */
/*    Description : define a valid file extension for TCP file TYPE          */
/*                                                                           */
/*****************************************************************************/
AiUInt32 get_TCP_EXT(TY_MONITOR_DATA_STRUCT *GLOBAL_VAR)
{
    MON_ONLY_PRINTF("\nNo valid NVR_TCPEX value found, calculate parameter");
    /*Note: take care that there is the possibility to programm a new card,
     a valid return value /=0xffff*/
    return (0x0001); /*only one type exists*/
}

/*****************************************************************************/
/*                                                                           */
/*   Module : prog_misc.c              Submodule : get_ASP_EXT               */
/*   Author : Matthias Lamp            Project   : PMC-1553 MON              */
/*---------------------------------------------------------------------------*/
/*   Create : 24.02.06, V02.00         Update :   .  .                       */
/*---------------------------------------------------------------------------*/
/*    Descriptions                                                           */
/*    ------------                                                           */
/*    Inputs    :   TY_MONITOR_DATA_STRUCT GLOBAL_VAR                        */
/*                                                                           */
/*    Outputs   :   return(FileExtension)                                    */
/*                                                                           */
/*    Description : define a valid file extension for ASP file TYPE          */
/*                                                                           */
/*****************************************************************************/
AiUInt32 get_ASP_EXT(TY_MONITOR_DATA_STRUCT *GLOBAL_VAR)
{
    AiUInt32 biu_1, platform;

    biu_1 = GLOBAL_VAR->NVR_BOARD_TYPE.Reg.BIU1_type;
    platform = GLOBAL_VAR->NVR_BOARD_CONFIG.Reg.PLATFORM;

    MON_ONLY_PRINTF("\nNo valid NVR_ASPEX value found, calculate parameter");

    if( (platform == 0x52) || /*64bit/32bit PMC / API-V2*/
        (platform == 0x56) || (platform == 0x60) )
    {
        if( (biu_1 & 0xf0) == 0x10 )
            /* Target-SW 1553, default */
            return (0x0010);
        else if( (biu_1 & 0xf0) == 0x20 )
            /* Target-SW 429, default  */
            return (0x0020);
        else if( (biu_1 & 0xf0) == 0x60 )
            /* Target-SW AFDX, default */
            return (0x0060);
    }

    return (0xffff);
}

/*****************************************************************************
 

 ****************************************************************************/
static void print_board_values(TY_MONITOR_DATA_STRUCT *GLOBAL_VAR, AiUInt32 NVR_SERIAL, AiUInt32 NVR_BOARD_CFG, AiUInt32 NVR_BOARD_TYPE1)
{
    AiUInt32 temp;

    AI_Printf("\nSerial Number = %04d", (NVR_SERIAL & 0xffff));
    /*v01.17*/
    AI_Printf("\nAIM Board Part No. = %05x", GLOBAL_VAR->NVR_BOARD_PARTNO.Reg.AIM_PARTNO);

    temp = (NVR_BOARD_CFG & 0xff); /* Board Type */
    switch( temp )
    {
        case 0x50:
            /* 32bit PMC */
            AI_Printf("\nBoard Type is: 32bit PMC");
            break;
        case 0x52:
            /* 32bit PMC with ASP */
            AI_Printf("\nBoard Type is: 32bit PMC with ASP");
            break;
        case 0x54:
            /* 64bit PMC */
            AI_Printf("\nBoard Type is: 64bit PMC");
            break;
        case 0x56:
            /* 64bit PMC with ASP */
            AI_Printf("\nBoard Type is: 64bit PMC with ASP");
            break;
        case 0x60:
            /* 64bit API-1 with ASP, based on XSCALE */
            AI_Printf("\nBoard Type is: 64bit API-1 with ASP, based on XSCALE");
            break;
        case 0x70:
            /* PC-Card */
            AI_Printf("\nBoard Type is: PC-Card");
            break;
        case 0x80:
            /* PC104 */
            AI_Printf("\nBoard Type is: PC104");
            break;
        default:
            AI_Printf("\nBoard Type is: unkonwn");
            break;
    }

    temp = (NVR_BOARD_TYPE1 & 0xff); /* BIU 1 Type */
    switch( temp )
    {
        case 0x00:
            AI_Printf("\nBIU 1 Interface is: reserved (unset)");
            break;
        case 0x10:
            AI_Printf("\nBIU 1 Interface is: MIL-Std-1553");
            break;
        case 0x12:
            AI_Printf("\nBIU 1 Interface is: MIL-Std-1553 Simulator only");
            break;
        case 0x14:
            AI_Printf("\nBIU 1 Interface is: MIL-Std-1553 Single function only");
            break;
        case 0x20:
            AI_Printf("\nBIU 1 Interface is: ARINC 429-16");
            break;
        case 0x21:
            AI_Printf("\nBIU 1 Interface is: ARINC 429-4");
            break;
        case 0x22:
            AI_Printf("\nBIU 1 Interface is: ARINC 429-8");
            break;
        case 0x24:
            AI_Printf("\nBIU 1 Interface is: ARINC 429-16");
            break;
        case 0x30:
            AI_Printf("\nBIU 1 Interface is: STANAG3910");
            break;
        case 0x40:
            AI_Printf("\nBIU 1 Interface is: PANAVIA");
            break;
        case 0x50:
            AI_Printf("\nBIU 1 Interface is: ARINC 629");
            break;

        case 0x60:
            AI_Printf("\nBIU 1 Interface is: AFDX");
            break;
        case 0x62:
            AI_Printf("\nBIU 1 Interface is: AFDX, Simulator only");
            break;
        case 0x64:
            AI_Printf("\nBIU 1 Interface is: AFDX, Generic mode only");
            break;
        case 0x68:
            AI_Printf("\nBIU 1 Interface is: AFDX (Special definition for API-FDX-2 V2)");
            break;
        case 0x6a:
            AI_Printf("\nBIU 1 Interface is: AFDX, Simulator only (Special definition for API-FDX-2 V2)");
            break;
        case 0x6c:
            AI_Printf("\nBIU 1 Interface is: AFDX, Generic mode only (Special definition for API-FDX-2 V2)");
            break;

        case 0x61:
            AI_Printf("\nBIU 1 Interface is: AFDX (Boeing)");
            break;
        case 0x63:
            AI_Printf("\nBIU 1 Interface is: AFDX, Simulator only (Boeing)");
            break;
        case 0x65:
            AI_Printf("\nBIU 1 Interface is: AFDX, Generic mode only (Boeing)");
            break;
        case 0x69:
            AI_Printf("\nBIU 1 Interface is: AFDX (Special definition for API-FDX-2 V2, (Boeing))");
            break;
        case 0x6b:
            AI_Printf("\nBIU 1 Interface is: AFDX, Simulator only (Special definition for API-FDX-2 V2, (Boeing))");
            break;
        case 0x6d:
            AI_Printf("\nBIU 1 Interface is: AFDX, Generic mode only (Special definition for API-FDX-2 V2, (Boeing))");
            break;

        case 0x70:
            AI_Printf("\nBIU 1 Interface is: EFEX");
            break;
        case 0xfe:
            AI_Printf("\nBIU 1 Interface is: GENERIC");
            break;
        case 0xff:
            AI_Printf("\nBIU 1 Interface is: Not present");
            break;
        default:
            AI_Printf("\nBIU 1 Interface is: RESERVED");
            break;
    }

    temp = ((NVR_BOARD_TYPE1 >> 8) & 0xff); /* BIU 2 Type */
    switch( temp )
    {
        case 0x00:
            AI_Printf("\nBIU 2 Interface is: reserved (unset)");
            break;
        case 0x10:
            AI_Printf("\nBIU 2 Interface is: MIL-Std-1553");
            break;
        case 0x12:
            AI_Printf("\nBIU 2 Interface is: MIL-Std-1553 Simulator only");
            break;
        case 0x14:
            AI_Printf("\nBIU 2 Interface is: MIL-Std-1553 Single function only");
            break;
        case 0x20:
            AI_Printf("\nBIU 2 Interface is: ARINC 429-16");
            break;
        case 0x21:
            AI_Printf("\nBIU 2 Interface is: ARINC 429-4");
            break;
        case 0x22:
            AI_Printf("\nBIU 2 Interface is: ARINC 429-8");
            break;
        case 0x24:
            AI_Printf("\nBIU 2 Interface is: ARINC 429-16");
            break;
        case 0x30:
            AI_Printf("\nBIU 2 Interface is: STANAG3910");
            break;
        case 0x40:
            AI_Printf("\nBIU 2 Interface is: PANAVIA");
            break;
        case 0x50:
            AI_Printf("\nBIU 2 Interface is: ARINC 629");
            break;
        case 0x60:
            AI_Printf("\nBIU 1 Interface is: AFDX");
            break;
        case 0x62:
            AI_Printf("\nBIU 1 Interface is: AFDX, Simulator only");
            break;
        case 0x64:
            AI_Printf("\nBIU 1 Interface is: AFDX, Generic mode only");
            break;
        case 0x68:
            AI_Printf("\nBIU 1 Interface is: AFDX (Special definition for API-FDX-2 V2)");
            break;
        case 0x6a:
            AI_Printf("\nBIU 1 Interface is: AFDX, Simulator only (Special definition for API-FDX-2 V2)");
            break;
        case 0x6c:
            AI_Printf("\nBIU 1 Interface is: AFDX, Generic mode only (Special definition for API-FDX-2 V2)");
            break;

        case 0x61:
            AI_Printf("\nBIU 1 Interface is: AFDX (Boeing)");
            break;
        case 0x63:
            AI_Printf("\nBIU 1 Interface is: AFDX, Simulator only (Boeing)");
            break;
        case 0x65:
            AI_Printf("\nBIU 1 Interface is: AFDX, Generic mode only (Boeing)");
            break;
        case 0x69:
            AI_Printf("\nBIU 1 Interface is: AFDX (Special definition for API-FDX-2 V2, (Boeing))");
            break;
        case 0x6b:
            AI_Printf("\nBIU 1 Interface is: AFDX, Simulator only (Special definition for API-FDX-2 V2, (Boeing))");
            break;
        case 0x6d:
            AI_Printf("\nBIU 1 Interface is: AFDX, Generic mode only (Special definition for API-FDX-2 V2, (Boeing))");
            break;

        case 0x70:
            AI_Printf("\nBIU 2 Interface is: EFEX");
            break;
        case 0xfe:
            AI_Printf("\nBIU 2 Interface is: GENERIC");
            break;
        case 0xff:
            AI_Printf("\nBIU 2 Interface is: Not present");
            break;
        default:
            AI_Printf("\nBIU 2 Interface is: RESERVED");
            break;
    }
    return;
}

/*****************************************************************************/
/*                                                                           */
/*   Module : MonSpecific.c            Submodule : read_board_values         */
/*   Author : Lamp Matthias            Project   : PMC-MON                   */
/*---------------------------------------------------------------------------*/
/*   Create : 23/09/2004  (v01.08)     Update :    21/06/05  (v01.17)        */
/*                                                 read AIM Part No          */
/*---------------------------------------------------------------------------*/
/*    Descriptions                                                           */
/*    ------------                                                           */
/*    Inputs    : GLOBAL_VAR    Global struct                                */
/*                                                                           */
/*    Outputs   :                                                            */
/*                                                                           */
/*    Description :   reads the serial number and other board specific       */
/*                    parameters via the SPI Interface from the NOVRAM       */
/*                    shows the results (used for update TCP and PROM)       */
/*                                                                           */
/*****************************************************************************/
void read_board_values(TY_MONITOR_DATA_STRUCT *GLOBAL_VAR)
{

    AiUInt32 NVR_MAGIC_1, NVR_MAGIC_1_Adr = 0x000, NVR_MAGIC_2, NVR_MAGIC_2_Adr = 0x1f8, NVR_SERIAL, NVR_SERIAL_Adr = 0x00c, NVR_BOARD_CFG,
                NVR_BOARD_CFG_Adr = 0x028, NVR_BOARD_TYPE1, NVR_BOARD_TYPE1_Adr = 0x02c;
    AiUInt32 temp;

    /*************************************************/
    /* enable SPI communication,*/
    /* read device ID, ...*/
    temp = spi_prog_enable(GLOBAL_VAR);
    if( temp == FAIL )
    {
        MON_ONLY_PRINTF("\nerror SPI open");
        return;
    }
    /*************************************************/

    NVR_MAGIC_1 = read_32bit_SPI_NVR(GLOBAL_VAR, NVR_MAGIC_1_Adr);
    NVR_MAGIC_2 = read_32bit_SPI_NVR(GLOBAL_VAR, NVR_MAGIC_2_Adr);
    NVR_SERIAL = read_32bit_SPI_NVR(GLOBAL_VAR, NVR_SERIAL_Adr);
    NVR_BOARD_CFG = read_32bit_SPI_NVR(GLOBAL_VAR, NVR_BOARD_CFG_Adr);
    NVR_BOARD_TYPE1 = read_32bit_SPI_NVR(GLOBAL_VAR, NVR_BOARD_TYPE1_Adr);

    GLOBAL_VAR->NVR_BOARD_PARTNO.all /* V01.17 */
    = read_32bit_SPI_NVR(GLOBAL_VAR, NVR_BOARD_PARTNO_Adr);
    /*V01.19, 11/01/2006*/
    GLOBAL_VAR->NVR_BOARD_TYPE.all = NVR_BOARD_TYPE1;
    GLOBAL_VAR->NVR_BOARD_CONFIG.all = NVR_BOARD_CFG;
    GLOBAL_VAR->NVR_BOARD_SERIAL.all = NVR_SERIAL;

    /*V02.00 23.02.06*/
    GLOBAL_VAR->NVR_PLCAEX.all  = read_32bit_SPI_NVR(GLOBAL_VAR, NVR_PLCAEX_A);
    GLOBAL_VAR->NVR_TCPEX.all   = read_32bit_SPI_NVR(GLOBAL_VAR, NVR_TCPEX_A);
    GLOBAL_VAR->NVR_ILCA1EX.all = read_32bit_SPI_NVR(GLOBAL_VAR, NVR_ILCA1EX_A);
    GLOBAL_VAR->NVR_ILCA2EX.all = read_32bit_SPI_NVR(GLOBAL_VAR, NVR_ILCA2EX_A);
    GLOBAL_VAR->NVR_ILCA3EX.all = read_32bit_SPI_NVR(GLOBAL_VAR, NVR_ILCA3EX_A);
    GLOBAL_VAR->NVR_ILCA4EX.all = read_32bit_SPI_NVR(GLOBAL_VAR, NVR_ILCA4EX_A);
    GLOBAL_VAR->NVR_BIP1EX.all  = read_32bit_SPI_NVR(GLOBAL_VAR, NVR_BIP1EX_A);
    GLOBAL_VAR->NVR_BIP2EX.all  = read_32bit_SPI_NVR(GLOBAL_VAR, NVR_BIP2EX_A);
    GLOBAL_VAR->NVR_BIP3EX.all  = read_32bit_SPI_NVR(GLOBAL_VAR, NVR_BIP3EX_A);
    GLOBAL_VAR->NVR_BIP4EX.all  = read_32bit_SPI_NVR(GLOBAL_VAR, NVR_BIP4EX_A);
    GLOBAL_VAR->NVR_ASPEX.all   = read_32bit_SPI_NVR(GLOBAL_VAR, NVR_ASPEX_A);

    /*V02.12 23.03.07*/
    GLOBAL_VAR->NVR_BOARD_ST.all = read_32bit_SPI_NVR(GLOBAL_VAR, NVR_BOARD_ST_Adr);
    GLOBAL_VAR->NVR_HW_VAR.all = read_32bit_SPI_NVR(GLOBAL_VAR, NVR_HW_VAR_Adr);

    /*V02.13 16.10.07*/
    GLOBAL_VAR->NVR_PART_EXT = read_32bit_SPI_NVR(GLOBAL_VAR, NVR_PART_EXT_Adr);
    GLOBAL_VAR->NVR_PCI_HDR.all = read_32bit_SPI_NVR(GLOBAL_VAR,
    NVR_PCI_HDR_Adr);

    if( (NVR_MAGIC_1 != NVR_MAGIC_def) && (NVR_MAGIC_2 != NVR_MAGIC_def) )
    {
        MON_ONLY_PRINTF("\nError NVR Magic "); MON_ONLY_PRINTF("\nNVR_MAGIC_1 is: %x should: %x", NVR_MAGIC_1,
                    NVR_MAGIC_def); MON_ONLY_PRINTF("\nNVR_MAGIC_2 is: %x should: %x", NVR_MAGIC_2,
                    NVR_MAGIC_def);
        return; /* no valid NOVRAM Magic */
    }

    print_board_values(GLOBAL_VAR, NVR_SERIAL, NVR_BOARD_CFG, NVR_BOARD_TYPE1);

    /* ATMEL inactive*/
    GLOBAL_VAR->PMC_RESET_Reg.Reg.ATMEL_RESx = 0;
    PCI_IOReg_write(GLOBAL_VAR, PMC_RESET_Reg_Adr, GLOBAL_VAR->PMC_RESET_Reg.all);
    wait_clocks(GLOBAL_VAR, 20000);

    /* ATMEL active*/
    GLOBAL_VAR->PMC_RESET_Reg.Reg.ATMEL_RESx = 1;
    PCI_IOReg_write(GLOBAL_VAR, PMC_RESET_Reg_Adr, GLOBAL_VAR->PMC_RESET_Reg.all);
    wait_clocks(GLOBAL_VAR, 20000);

    /* SPI PROG disable */
    GLOBAL_VAR->PMC_TCP_PROG_Reg.all = 0;
    GLOBAL_VAR->PMC_TCP_PROG_Reg.Reg.SPI_OE = 0;
    GLOBAL_VAR->PMC_TCP_PROG_Reg.Reg.SPI_MOSI = 0;
    GLOBAL_VAR->PMC_TCP_PROG_Reg.Reg.SPI_CLK = 0;
    PCI_IOReg_write(GLOBAL_VAR, PMC_TCP_PROG_Reg_Adr, GLOBAL_VAR->PMC_TCP_PROG_Reg.all);
    wait_clocks(GLOBAL_VAR, 10000);
    /* **************** */

    return;
}

/*****************************************************************************/
/*                                                                           */
/*   Module : MonSpecific.c            Submodule : read_board_values_flash   */
/*   Author : Lamp Matthias            Project   : PMC-MON                   */
/*---------------------------------------------------------------------------*/
/*   Create : 23/09/2004  (v01.08)     Update :    21/06/05  (v01.17)        */
/*                                                 read AIM Part No          */
/*---------------------------------------------------------------------------*/
/*    Descriptions                                                           */
/*    ------------                                                           */
/*    Inputs    : GLOBAL_VAR    Global struct                                */
/*                                                                           */
/*    Outputs   :                                                            */
/*                                                                           */
/*    Description :   reads the serial number and other board specific       */
/*                    parameters via the SPI Interface from the NOVRAM       */
/*                    shows the results (used for update TCP and PROM)       */
/*                                                                           */
/*****************************************************************************/
void read_board_values_flash(TY_MONITOR_DATA_STRUCT *GLOBAL_VAR)
{

    AiUInt32 NVR_MAGIC_1, NVR_MAGIC_1_Adr = 0x000, NVR_MAGIC_2, NVR_MAGIC_2_Adr = 0x1f8, NVR_SERIAL, NVR_SERIAL_Adr = 0x00c, NVR_BOARD_CFG,
                NVR_BOARD_CFG_Adr = 0x028, NVR_BOARD_TYPE1, NVR_BOARD_TYPE1_Adr = 0x02c;
    AiUInt32 temp;

    /*************************************************/
    /* enable SPI communication,*/
    /* read device ID, ...*/
    temp = spi_prog_enable(GLOBAL_VAR);
    if( temp == FAIL )
    {
        MON_ONLY_PRINTF("\nerror SPI open");
        return;
    }
    /*************************************************/

    NVR_MAGIC_1     = read_32bit_spi_flash_mirror(GLOBAL_VAR, NVR_MAGIC_1_Adr);
    NVR_MAGIC_2     = read_32bit_spi_flash_mirror(GLOBAL_VAR, NVR_MAGIC_2_Adr);
    NVR_SERIAL      = read_32bit_spi_flash_mirror(GLOBAL_VAR, NVR_SERIAL_Adr);
    NVR_BOARD_CFG   = read_32bit_spi_flash_mirror(GLOBAL_VAR, NVR_BOARD_CFG_Adr);
    NVR_BOARD_TYPE1 = read_32bit_spi_flash_mirror(GLOBAL_VAR, NVR_BOARD_TYPE1_Adr);

    GLOBAL_VAR->NVR_BOARD_PARTNO.all /* V01.17 */
    = read_32bit_spi_flash_mirror(GLOBAL_VAR, NVR_BOARD_PARTNO_Adr);
    /*V01.19, 11/01/2006*/
    GLOBAL_VAR->NVR_BOARD_TYPE.all = NVR_BOARD_TYPE1;
    GLOBAL_VAR->NVR_BOARD_CONFIG.all = NVR_BOARD_CFG;
    GLOBAL_VAR->NVR_BOARD_SERIAL.all = NVR_SERIAL;

    /*V02.00 23.02.06*/
    GLOBAL_VAR->NVR_PLCAEX.all  = read_32bit_spi_flash_mirror(GLOBAL_VAR, NVR_PLCAEX_A);
    GLOBAL_VAR->NVR_TCPEX.all   = read_32bit_spi_flash_mirror(GLOBAL_VAR, NVR_TCPEX_A);
    GLOBAL_VAR->NVR_ILCA1EX.all = read_32bit_spi_flash_mirror(GLOBAL_VAR, NVR_ILCA1EX_A);
    GLOBAL_VAR->NVR_ILCA2EX.all = read_32bit_spi_flash_mirror(GLOBAL_VAR, NVR_ILCA2EX_A);
    GLOBAL_VAR->NVR_ILCA3EX.all = read_32bit_spi_flash_mirror(GLOBAL_VAR, NVR_ILCA3EX_A);
    GLOBAL_VAR->NVR_ILCA4EX.all = read_32bit_spi_flash_mirror(GLOBAL_VAR, NVR_ILCA4EX_A);
    GLOBAL_VAR->NVR_BIP1EX.all  = read_32bit_spi_flash_mirror(GLOBAL_VAR, NVR_BIP1EX_A);
    GLOBAL_VAR->NVR_BIP2EX.all  = read_32bit_spi_flash_mirror(GLOBAL_VAR, NVR_BIP2EX_A);
    GLOBAL_VAR->NVR_BIP3EX.all  = read_32bit_spi_flash_mirror(GLOBAL_VAR, NVR_BIP3EX_A);
    GLOBAL_VAR->NVR_BIP4EX.all  = read_32bit_spi_flash_mirror(GLOBAL_VAR, NVR_BIP4EX_A);
    GLOBAL_VAR->NVR_ASPEX.all   = read_32bit_spi_flash_mirror(GLOBAL_VAR, NVR_ASPEX_A);

    /*V02.12 23.03.07*/
    GLOBAL_VAR->NVR_BOARD_ST.all = read_32bit_spi_flash_mirror(GLOBAL_VAR, NVR_BOARD_ST_Adr);
    GLOBAL_VAR->NVR_HW_VAR.all = read_32bit_spi_flash_mirror(GLOBAL_VAR, NVR_HW_VAR_Adr);

    /*V02.13 16.10.07*/
    GLOBAL_VAR->NVR_PART_EXT = read_32bit_spi_flash_mirror(GLOBAL_VAR, NVR_PART_EXT_Adr);
    GLOBAL_VAR->NVR_PCI_HDR.all = read_32bit_spi_flash_mirror(GLOBAL_VAR, NVR_PCI_HDR_Adr);

    if( (NVR_MAGIC_1 != NVR_MAGIC_def) && (NVR_MAGIC_2 != NVR_MAGIC_def) )
    {
        MON_ONLY_PRINTF("\nError NVR Magic ");MON_ONLY_PRINTF("\nNVR_MAGIC_1 is: %x should: %x", NVR_MAGIC_1,
                    NVR_MAGIC_def);MON_ONLY_PRINTF("\nNVR_MAGIC_2 is: %x should: %x", NVR_MAGIC_2,
                    NVR_MAGIC_def);
        return; /* no valid NOVRAM Magic */
    }

    /* ATMEL inactive*/
    GLOBAL_VAR->PMC_RESET_Reg.Reg.ATMEL_RESx = 0;
    PCI_IOReg_write(GLOBAL_VAR, PMC_RESET_Reg_Adr, GLOBAL_VAR->PMC_RESET_Reg.all);
    wait_clocks(GLOBAL_VAR, 20000);

    /* ATMEL active*/
    GLOBAL_VAR->PMC_RESET_Reg.Reg.ATMEL_RESx = 1;
    PCI_IOReg_write(GLOBAL_VAR, PMC_RESET_Reg_Adr, GLOBAL_VAR->PMC_RESET_Reg.all);
    wait_clocks(GLOBAL_VAR, 20000);

    /* SPI PROG disable */
    GLOBAL_VAR->PMC_TCP_PROG_Reg.all = 0;
    GLOBAL_VAR->PMC_TCP_PROG_Reg.Reg.SPI_OE = 0;
    GLOBAL_VAR->PMC_TCP_PROG_Reg.Reg.SPI_MOSI = 0;
    GLOBAL_VAR->PMC_TCP_PROG_Reg.Reg.SPI_CLK = 0;
    PCI_IOReg_write(GLOBAL_VAR, PMC_TCP_PROG_Reg_Adr, GLOBAL_VAR->PMC_TCP_PROG_Reg.all);
    wait_clocks(GLOBAL_VAR, 10000);
    /* **************** */

    return;
}

/*****************************************************************************/
/*                                                                           */
/*   Module : MonSpecific.c            Submodule : read_32bit_SPI_NVR        */
/*   Author : Lamp Matthias            Project   : PMC-MON                   */
/*---------------------------------------------------------------------------*/
/*   Create : 23/09/2004  (v01.08)     Update :    /  /                      */
/*---------------------------------------------------------------------------*/
/*    Descriptions                                                           */
/*    ------------                                                           */
/*    Inputs    :     GLOBAL_VAR Global struct, UInt32 Addr.                 */
/*                                                                           */
/*    Outputs   :     32Bit Data                                             */
/*                                                                           */
/*    Description :   reads a 32Bit Value from the TCP NovRam via SPI        */
/*                                                                           */
/*****************************************************************************/
AiUInt32 read_32bit_SPI_NVR(TY_MONITOR_DATA_STRUCT *GLOBAL_VAR, AiUInt32 address)
{
    AiUInt32 NVR_data = 0;

    NVR_data = read_spi_novram(GLOBAL_VAR, (address + 3));
    NVR_data = (NVR_data << 8) | read_spi_novram(GLOBAL_VAR, (address + 2));
    NVR_data = (NVR_data << 8) | read_spi_novram(GLOBAL_VAR, (address + 1));
    NVR_data = (NVR_data << 8) | read_spi_novram(GLOBAL_VAR, address);

    return (NVR_data);
}

/*****************************************************************************/
/*                                                                           */
/*   Module : MonSpecific.c            Submodule : write_32bit_SPI_NVR       */
/*   Author : Lamp Matthias            Project   : PMC-MON                   */
/*---------------------------------------------------------------------------*/
/*   Create : 01.08.2005               Update :    /  /                      */
/*---------------------------------------------------------------------------*/
/*    Descriptions                                                           */
/*    ------------                                                           */
/*    Inputs    :     GLOBAL_VAR Global struct, UInt32 Addr. UInt32 data     */
/*                                                                           */
/*    Outputs   :                                                            */
/*                                                                           */
/*    Description :   writes a 32Bit Value to the TCP NovRam via SPI         */
/*                                                                           */
/*****************************************************************************/
AiUInt32 write_32bit_SPI_NVR(TY_MONITOR_DATA_STRUCT *GLOBAL_VAR, AiUInt32 address, AiUInt32 data)
{
    write_spi_novram(GLOBAL_VAR, (address + 3), (AiUInt8) ((data >> 24) & 0xff));
    write_spi_novram(GLOBAL_VAR, (address + 2), (AiUInt8) ((data >> 16) & 0xff));
    write_spi_novram(GLOBAL_VAR, (address + 1), (AiUInt8) ((data >> 8) & 0xff));
    write_spi_novram(GLOBAL_VAR, (address), (AiUInt8) ((data) & 0xff));

    return (PASS);
}
