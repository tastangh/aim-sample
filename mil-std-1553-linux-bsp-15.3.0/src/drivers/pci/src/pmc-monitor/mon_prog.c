/* SPDX-FileCopyrightText: 2001-2021 AIM GmbH <info@aim-online.com> */
/* SPDX-License-Identifier: MIT OR GPL-2.0-or-later */


/*****************************************************************************/
/*                                                                           */
/*   Module : MON_PROG.C               Submodule : -                         */
/*                                                                           */
/*   Author : Lamp Matthias            Project   : PMC_MON                   */
/*                                                                           */
/*   Source : C                        Tools     : PC/AT; Codewright V6.0c;  */
/*                                                 Microsoft Visual C++ 6.0  */
/*---------------------------------------------------------------------------*/
/*   Create : 04.10.2001   Update :   24.10.2003                             */
/*---------------------------------------------------------------------------*/
/*    Descriptions                                                           */
/*    ------------                                                           */
/*    Inputs      : none                                                     */
/*                                                                           */
/*    Outputs     : none                                                     */
/*                                                                           */
/*    Description : programms whitch are called from the monitor             */
/*                  main programms                                           */
/*                                                                           */
/*****************************************************************************/

#if defined (__monitor__)
  #ifndef _CRT_SECURE_NO_WARNINGS
    #define _CRT_SECURE_NO_WARNINGS /* warnings to use strcpy_s/scanf_s instead of  strcpy/scanf ML 27/12/2011 */
  #endif
  #include "MonSpecific.h"
  #include "defs.h"
  #include "type_def.h"
  #include "prototypes.h"
#elif defined (_SMARTLIB)
  #include "stdio.h"
  #include "string.h"
  #include "MonSpecific.h"
  #include "defs.h"
  #include "type_def.h"
  #include "prototypes.h"
  #include "Ai_cdef.h"
  #include "Wrapper.h"  /* needed for v_SLLogFile */
  #include "SmartLib.h" /* needed for v_SLLogFile */
#else
  #ifdef _AIM_VMEWINDOWS
    #include <windows.h>
  #endif
  #include "defs.h"
  #include "type_def.h"
  #include "prototypes.h"
  #if defined (_AIM_VME)
    #include <string.h> /* for declaration of memset */
    #if defined (_AIM_FDX)
      #ifdef _AIM_VMEWINDOWS
        _API_EXTERN AiUInt32 ul_FdxDbgLevel;
        #include <stdio.h>
      #endif
      #include <string.h>
      #include <stdlib.h>
      #include "../AiFdx_intern.h"
      #if defined(__LYNXOS)     /* LynxOS driver    */
        #include "../generic.h"
      #endif
    #elif defined (_AIM_1553)
      #include <stdio.h>
      #include <stdlib.h>
      #include "../Ai1553i_def.h"
      #include "../DEFS_V.H"
    #elif defined (_AMC429)
      #include <stdio.h>
      #include <stdlib.h>
      #include "Api429.h"
      #include "A429_Defs_v.h"
    #endif
  #elif defined (_AIM_WINDOWS)   /* system driver*/
    #include <stddcls.h>
    #include <stdlib.h>
    #include ".\..\..\..\include\api_defs.h"
    #include ".\..\..\..\include\api_defv.h"
    #include ".\..\..\..\include\fileio.h"
    #include ".\..\..\..\include\perf.h"
  #elif defined _AIM_VXI
    #if defined (_AIM_1553)
      #include "ai1553axi.h"
      #include <stdio.h> 
      #include "defs.h"
        #include "ai1553_version.h"
      extern AiUInt32 Aiul_DbgLevel;
    #elif defined (_AMC429) /*Path for LabView RT for AMC429 */
        #include <stdio.h>
      #include <stdlib.h>
      #include "Api429.h"
      #include "A429_Defs_v.h"
        _API_EXTERN AiUInt32 Aiul_DbgLevel;
    #else
      #include <stdio.h>
      #include <stdlib.h>
      #include ".\..\AiFdx_intern.h"
    #endif
  #endif
#endif



#define ASP_BOOT_TEST

/* set by ASP when running. Can't be shared in defs.h with AMC1553T target
 * software as defs.h can't be included due to ini_defs.h, so locally defined,
 * also defined in amc1553t:ini_xscale.c :-( */
#define MBOX_ASP_STARTED_MAGIC    0xAABBCCDD

/*****************************************************************************/
/*   Module : mon_prog.c               submodule : READ_PCIFPGA_REG_PMC      */
/*   Author : Matthias Lamp            Project   : PMC-1553                  */
/*---------------------------------------------------------------------------*/
/*   Create : 24/10/2001   Update :                                          */
/*---------------------------------------------------------------------------*/
/*    Descriptions                                                           */
/*    ------------                                                           */
/*    Inputs      :   -                                                      */
/*                                                                           */
/*    Outputs     :   AiUInt32   PASS /FAIL                                  */
/*                                                                           */
/*    Description :   reads the PCI FPGA IO register                         */
/*                    and writes start values to them                        */
/*                                                                           */
/*****************************************************************************/
AiUInt32 read_PCIFPGA_REG_PMC(TY_MONITOR_DATA_STRUCT *GLOBAL_VAR)
{
    AiUInt32 temp;

#if defined(_AIM_SYSDRV) && defined(_AIM_WINDOWS)
    LARGE_INTEGER x_Start;
    x_Start.QuadPart = read_Xtimestamp();
#endif

    GLOBAL_VAR->BOOT_CONTROL_Reg.Reg.PCI_REG = 0; /* clear status bit*/
    GLOBAL_VAR->PMC_FPGA_Reg.all             = PCI_IOReg_read(GLOBAL_VAR, PMC_FPGA_Reg_Adr);
    GLOBAL_VAR->PMC_COUNTER_Reg              = PCI_IOReg_read(GLOBAL_VAR, PMC_COUNTER_Reg_Adr);
    GLOBAL_VAR->PMC_IRQ_EVENT_Reg.all        = PCI_IOReg_read(GLOBAL_VAR, PMC_IRQ_EVENT_Reg_Adr);
    GLOBAL_VAR->PMC_IRQ_VEC_Reg              = PCI_IOReg_read(GLOBAL_VAR, PMC_IRQ_VEC_Reg_Adr);
    GLOBAL_VAR->PMC_JTAG_Reg.all             = PCI_IOReg_read(GLOBAL_VAR, PMC_JTAG_Reg_Adr);
    GLOBAL_VAR->PMC_RESET_Reg.all            = PCI_IOReg_read(GLOBAL_VAR, PMC_RESET_Reg_Adr);
    GLOBAL_VAR->PMC_TCP_DATA_Reg.all         = PCI_IOReg_read(GLOBAL_VAR, PMC_TCP_DATA_Reg_Adr);
    GLOBAL_VAR->PMC_TCP_PROG_Reg.all         = PCI_IOReg_read(GLOBAL_VAR, PMC_TCP_PROG_Reg_Adr);
    GLOBAL_VAR->PMC_TTHIGH_Reg.all           = PCI_IOReg_read(GLOBAL_VAR, PMC_TTHIGH_Reg_Adr);
    GLOBAL_VAR->PMC_TTLOW_Reg.all            = PCI_IOReg_read(GLOBAL_VAR, PMC_TTLOW_Reg_Adr);
    GLOBAL_VAR->PMC_VERSION_Reg.all          = PCI_IOReg_read(GLOBAL_VAR, PMC_VERSION_Reg_Adr);

    temp = PCI_IOReg_read(GLOBAL_VAR, PMC_MAILBOX_Reg_Adr);
    GLOBAL_VAR->PMC_MAILBOX_Reg.all = temp & 0xffffffff; /* mask 32 BITs*/

    temp = PCI_IOReg_read(GLOBAL_VAR, PMC_MBOX_cnt_Reg_Adr);
    GLOBAL_VAR->PMC_MBOX_cnt_Reg.all = temp & 0xffffffff; /* mask 32 BITs*/
    GLOBAL_VAR->PMC_DEVICEID_Reg.all = PCI_IOReg_read(GLOBAL_VAR, PMC_DEVICEID_Reg_Adr);

    MON_ONLY_PRINTF("\nPMC_FPGA_Reg        %08x",GLOBAL_VAR->PMC_FPGA_Reg.all);
    MON_ONLY_PRINTF("\nPMC_COUNTER_Reg     %08x",GLOBAL_VAR->PMC_COUNTER_Reg);
    MON_ONLY_PRINTF("\nPMC_IRQ_EVENT_Reg   %08x",GLOBAL_VAR->PMC_IRQ_EVENT_Reg.all);
    MON_ONLY_PRINTF("\nPMC_IRQ_MASK_Reg    write only");
    MON_ONLY_PRINTF("\nPMC_IRQ_VEC_Reg     %08x",GLOBAL_VAR->PMC_IRQ_VEC_Reg);
    MON_ONLY_PRINTF("\nPMC_JTAG_Reg        %08x",GLOBAL_VAR->PMC_JTAG_Reg.all);
    MON_ONLY_PRINTF("\nPMC_JTAG_KEY_Reg    write only");
    MON_ONLY_PRINTF("\nPMC_RESET_Reg       %08x",GLOBAL_VAR->PMC_RESET_Reg.all);
    MON_ONLY_PRINTF("\nPMC_TCP_DATA_Reg    %08x",GLOBAL_VAR->PMC_TCP_DATA_Reg.all);
    MON_ONLY_PRINTF("\nPMC_TCP_PROG_Reg    %08x",GLOBAL_VAR->PMC_TCP_PROG_Reg.all);
    MON_ONLY_PRINTF("\nPMC_TTHIGH_Reg      %08x",GLOBAL_VAR->PMC_TTHIGH_Reg.all);
    MON_ONLY_PRINTF("\nPMC_TTLOW_Reg       %08x",GLOBAL_VAR->PMC_TTLOW_Reg.all);
    MON_ONLY_PRINTF("\nPMC_IRIG_SAMPLE_Reg write only   (stored value: %08x",GLOBAL_VAR->PMC_IRIG_SAMPLE_Reg.all);
    MON_ONLY_PRINTF("\nPMC_VERSION_Reg     %08x",GLOBAL_VAR->PMC_VERSION_Reg.all);
    MON_ONLY_PRINTF("\nPMC_MAILBOX_Reg     %08x",GLOBAL_VAR->PMC_MAILBOX_Reg.all);
    MON_ONLY_PRINTF("\nPMC_MBOX_cnt_Reg    %08x",GLOBAL_VAR->PMC_MBOX_cnt_Reg.all);
    MON_ONLY_PRINTF("\nPMC_DEVICEID_Reg    %08x",GLOBAL_VAR->PMC_DEVICEID_Reg.all);

#if defined(_AIM_SYSDRV) && defined(_AIM_WINDOWS)
    MonPrint ( DBG_INIT, FKT_MON, ("TIME: read_PCIFPGA_REG_PMC      %I64u\n", (read_Xtimestamp() - x_Start.QuadPart)));
#endif

    if( (GLOBAL_VAR->PMC_VERSION_Reg.Reg.revision == 0) && (GLOBAL_VAR->PMC_VERSION_Reg.Reg.version == 0) )
        return (FAIL);
    else
    {
        GLOBAL_VAR->BOOT_CONTROL_Reg.Reg.PCI_REG = 1; /* PCI Register found*/
        return (PASS);
    }
}

/*****************************************************************************/
/*   Module : mon_prog.c               submodule : start_ATMEL               */
/*   Author : Matthias Lamp            Project   : PMC-1553                  */
/*---------------------------------------------------------------------------*/
/*   Create : 10/10/2001   Update :                                          */
/*---------------------------------------------------------------------------*/
/*    Descriptions                                                           */
/*    ------------                                                           */
/*    Inputs      :   -                                                      */
/*                                                                           */
/*    Outputs     :   AiUInt32   PASS /FAIL                                  */
/*                                                                           */
/*    Description :   starts the ATMEL, writes start values (IRIG intern     */
/*                    enable IRIG control (FPGA)                             */
/*                                                                           */
/*****************************************************************************/
AiUInt32 start_ATMEL(TY_MONITOR_DATA_STRUCT *GLOBAL_VAR)
{

    AiUInt32 atmel_ready = 0;
    AiUInt32 temp, x;

    /* dummy out, to clear ATMEL READY*/
    /* disable IRIG Control*/
    GLOBAL_VAR->BOOT_CONTROL_Reg.Reg.ATMEL_OK = 0; /* clear status bit*/
    GLOBAL_VAR->PMC_RESET_Reg.Reg.ATMEL_RESx = 0;
    GLOBAL_VAR->PMC_RESET_Reg.Reg.IRIG_RESx = 0; /* IRIG control enabled (in function with TCP)*/
    PCI_IOReg_write(GLOBAL_VAR, PMC_RESET_Reg_Adr, GLOBAL_VAR->PMC_RESET_Reg.all); /* TCP in reaset*/
    wait_clocks(GLOBAL_VAR, 20000); /* wait 20 ms*/

    GLOBAL_VAR->PMC_RESET_Reg.Reg.ATMEL_RESx = 1;
    PCI_IOReg_write(GLOBAL_VAR, PMC_RESET_Reg_Adr, GLOBAL_VAR->PMC_RESET_Reg.all); /* TCP not longer in reset*/
    wait_clocks(GLOBAL_VAR, 20000); /* wait 20 ms*/

    /* wait for ATMEL ready, if not return FAIL*/
    x = 0;
    do /* wait for ATMEL_RDY*/
    {
        GLOBAL_VAR->PMC_TCP_DATA_Reg.all = PCI_IOReg_read(GLOBAL_VAR,
        PMC_TCP_DATA_Reg_Adr);
        atmel_ready = GLOBAL_VAR->PMC_TCP_DATA_Reg.Reg.ATMEL_RDY;
        if( atmel_ready == 1 )
            break;
        wait_clocks(GLOBAL_VAR, 300); /* wait 1 ms*/
        x++;
    } while( x < 3000 ); /* avoid hanging up TCP (wait for ATMEL_RDY 300ms 3000*100us)*/

    if( atmel_ready != 1 )
    {
        AI_Printf("\n!!! ERROR !!! start ATMEL, wait for ready");
        return (FAIL); /*ATMEL does not response ready*/
    }

    /*ATMEL response ready, set IRIG to intern*/
    TcpOut(GLOBAL_VAR, TC_WRIRIGEXINx, IRIG_intern);
    wait_clocks(GLOBAL_VAR, 1000); /* wait 1 ms*/

    /* IRIG control enable*/
    GLOBAL_VAR->PMC_RESET_Reg.Reg.IRIG_RESx = 1; /* IRIG control enabled (in function with TCP)*/
    PCI_IOReg_write(GLOBAL_VAR, PMC_RESET_Reg_Adr, GLOBAL_VAR->PMC_RESET_Reg.all); /* TCP not longer in reset*/
    TcpIn(GLOBAL_VAR, TC_VERSION, &temp);
    GLOBAL_VAR->ATMEL_VER.Reg.VERSION = temp;

    if( GLOBAL_VAR->ATMEL_VER.Reg.VERSION == 0 )
    {
        AI_Printf("\n !!! ERROR !!! TCP Version: %x", temp);
        return (FAIL);
    }

    AI_Printf("\nTCP Version: %x", GLOBAL_VAR->ATMEL_VER.Reg.VERSION);

    GLOBAL_VAR->BOOT_CONTROL_Reg.Reg.ATMEL_OK = 1; /* ATMEL started*/
    return (PASS);
}

/*****************************************************************************/
/*   Module : mon_prog.c               submodule : read_NOVRAM_VAL_PMC       */
/*   Author : Matthias Lamp            Project   : PMC-1553                  */
/*---------------------------------------------------------------------------*/
/*   Create : 08/10/2001   Update :  08.04.2005 v01.14                       */
/*---------------------------------------------------------------------------*/
/*    Descriptions                                                           */
/*    ------------                                                           */
/*    Inputs    : -                                                          */
/*                                                                           */
/*    Outputs   : AiUInt 32  PASS /FAIL                                      */
/*                                                                           */
/*    Description : reads the NOVRAm from the TCP (ATMEL) and writes the     */
/*                  values to the GLOBAL_VAR                                 */
/*                  checks the integrity of the NOVRAM (checksum, magic)     */
/*                  v01.14 Functionality of FLASH NOVRAM mirror              */
/*                                                                           */
/*****************************************************************************/
AiUInt32 read_NOVRAM_VAL_PMC(TY_MONITOR_DATA_STRUCT *GLOBAL_VAR)
{
    AiUInt32 flag_flash = PASS; /*V01.14*/
    AiUInt32 x, temp;
    AiUInt8 *data;

    data = (GLOBAL_VAR->GLOBAL_TEMP_MEM);
    memset(data, 0, 512); /* set buffer (allocated memory) to "0" */

    GLOBAL_VAR->BOOT_CONTROL_Reg.Reg.NOVRam_Reg = 0; /* clear status bit*/

    flag_flash = get_flash_mirror(GLOBAL_VAR, data); /* indicates valid NOVRAM mirrow in TCP FLASH*/

    GLOBAL_VAR->NVR_MAGIC = NOVRam_read(GLOBAL_VAR, NVR_MAGIC_Adr); /*read magic 1*/
    GLOBAL_VAR->NVR_MAGIC2 = NOVRam_read(GLOBAL_VAR, NVR_MAGIC2_Adr); /*read magic 2*/

    if( (GLOBAL_VAR->NVR_MAGIC == NVR_MAGIC_def) ^ (GLOBAL_VAR->NVR_MAGIC2 == NVR_MAGIC_def) )
    { /*one invalid NOVRAM MAGIC (1 xor 2)*/
        AI_Printf("\nNOVRAM MAGIC 1 or 2 is invalid, restore both");
        GLOBAL_VAR->NVR_MAGIC = NVR_MAGIC_def; /*restore location 1*/
        NOVRam_write(GLOBAL_VAR, NVR_MAGIC_Adr, GLOBAL_VAR->NVR_MAGIC);

        GLOBAL_VAR->NVR_MAGIC2 = NVR_MAGIC_def; /*restore location 2*/
        NOVRam_write(GLOBAL_VAR, NVR_MAGIC2_Adr, GLOBAL_VAR->NVR_MAGIC2);
    }
    else if( (GLOBAL_VAR->NVR_MAGIC != NVR_MAGIC_def) && (GLOBAL_VAR->NVR_MAGIC2 != NVR_MAGIC_def) )
    { /*both Locations are faulty, check mirror*/
        AI_Printf("\ninvalid NOVRAM Magic 1 and 2");

        if( flag_flash == PASS )
        { /*valid FLASH MIRROR*/
            AI_Printf("\nvalid NOVRAM FLASH mirror found, restore NOVRAM");
            /* Restore NOVRAM values witzh the values from the FLASH mirror */
            AI_Printf("\nRestore NOVRAM with the FLASH mirror values");
            for( x = 0; x < 512; x += 4 )
            {
                temp = (*(data + (x) + 3) << 24) | (*(data + (x) + 2) << 16) | (*(data + (x) + 1) << 8) | *(data + (x));
                if( NOVRam_write(GLOBAL_VAR, x, temp) == FAIL )
                {
                    AI_Printf("\nWriting NOVRAM FLASH mirror to NOVRAM FAIL");
                    AI_Printf("\nData: %08x at address: %03d", temp, x);
                    return (FAIL);
                }
            }

            GLOBAL_VAR->NVR_MAGIC = NOVRam_read(GLOBAL_VAR, NVR_MAGIC_Adr); /*read magic 1*/
            GLOBAL_VAR->NVR_MAGIC2 = NOVRam_read(GLOBAL_VAR, NVR_MAGIC2_Adr); /*read magic 2*/

            if( (GLOBAL_VAR->NVR_MAGIC != NVR_MAGIC_def) || (GLOBAL_VAR->NVR_MAGIC2 != NVR_MAGIC_def) )
            { /*error restoring flash*/
                AI_Printf("\nError restoring NOVRAM, NOVRAM MAGIC 1 or 2 FALSE");
                AI_Printf("\nNVR MAGIC 1 is: %08x NVR MAGIC 2 is: %08x should: %08x", GLOBAL_VAR->NVR_MAGIC, GLOBAL_VAR->NVR_MAGIC2,
                NVR_MAGIC_def);
                return (FAIL);
            }
            AI_Printf("\nNOVRAM restored from FLASH mirror");
        }
        else
        { /*no valid NOVRAM, no valid FLASH mirror, =>new board!*/
            AI_Printf("\ninit_NOVRAM, no valid NOVRAM FLASH mirror"); /*hier wird magic geschrieben*/
            if( init_novram(GLOBAL_VAR) == FAIL ) /*program start values to NOVRam*/
            {
                AI_Printf("\n init_NOVRAM error");
                return (FAIL);
            }
        }
    }

    AI_Printf("\n read NOVRAM values");
    /* read now the novram values*/
    if( enter_hw(GLOBAL_VAR) == FAIL )
    {
        if( flag_flash == PASS )
        { /*valid FLASH MIRROR*/
            AI_Printf("\nvalid NOVRAM FLASH mirror found, restore NOVRAM");
            /* Restore NOVRAM values witzh the values from the FLASH mirror */
            AI_Printf("\nRestore NOVRAM with the FLASH mirror values");
            for( x = 0; x < 512; x += 4 )
            {
                temp = (*(data + (x) + 3) << 24) | (*(data + (x) + 2) << 16) | (*(data + (x) + 1) << 8) | *(data + (x));
                if( NOVRam_write(GLOBAL_VAR, x, temp) == FAIL )
                {
                    AI_Printf("\nWriting NOVRAM FLASH mirror to NOVRAM FAIL");
                    AI_Printf("\nData: %08x at address: %03d", temp, x);
                    return (FAIL);
                }
            }

            GLOBAL_VAR->NVR_MAGIC = NOVRam_read(GLOBAL_VAR, NVR_MAGIC_Adr); /*read magic 1*/
            GLOBAL_VAR->NVR_MAGIC2 = NOVRam_read(GLOBAL_VAR, NVR_MAGIC2_Adr); /*read magic 2*/

            if( (GLOBAL_VAR->NVR_MAGIC != NVR_MAGIC_def) || (GLOBAL_VAR->NVR_MAGIC2 != NVR_MAGIC_def) )
            { /*error restoring flash*/
                AI_Printf("\nError restoring NOVRAM, NOVRAM MAGIC 1 or 2 FALSE");
                AI_Printf("\nNVR MAGIC 1 is: %08x NVR MAGIC 2 is: %08x should: %08x", GLOBAL_VAR->NVR_MAGIC, GLOBAL_VAR->NVR_MAGIC2,
                NVR_MAGIC_def);
                return (FAIL);
            }

            AI_Printf("\nNOVRAM restored from FLASH mirror");

            if( enter_hw(GLOBAL_VAR) == FAIL )
            {
                AI_Printf("\nENTER HW fail2nd time, after NOVRAM restore");
                return (FAIL);
            }
        }
        else
        {
            AI_Printf("\nInvalid NOVRAM, no NOVRAM FLASH mirror");
            return (FAIL);
        }
    }

    /*first NOVRAM run, new board*/
    GLOBAL_VAR->NVR_BOARD_SERIAL.all = NOVRam_read(GLOBAL_VAR,
    NVR_BOARD_SERIAL_Adr);
    if( (GLOBAL_VAR->NVR_BOARD_SERIAL.Reg.SERIAL > 9999) || (GLOBAL_VAR->NVR_BOARD_SERIAL.Reg.SERIAL == 0) )
    {
        AI_Printf("\nNo valid Serial Number found, set to 9999");
        GLOBAL_VAR->NVR_BOARD_SERIAL.Reg.SERIAL = 9999;
        NOVRam_write(GLOBAL_VAR, NVR_BOARD_SERIAL_Adr, GLOBAL_VAR->NVR_BOARD_SERIAL.all);

        /*new checksum*/
        GLOBAL_VAR->NVR_CHECKSUM = NOVRam_chksum(GLOBAL_VAR);
        if( NOVRam_write(GLOBAL_VAR, NVR_CHECKSUM_Adr, GLOBAL_VAR->NVR_CHECKSUM) == FAIL )
            return (FAIL);

        /*and update FLASH*/
        if( update_NOVRAM_FLASH(GLOBAL_VAR) == FAIL )
        {
            AI_Printf("\nERROR creating NOVRAM FLASH mirror");
            return (FAIL);
        }
    }

    GLOBAL_VAR->BOOT_CONTROL_Reg.Reg.NOVRam_Reg = 1; /* NOVRAM values OK*/
    return (PASS);
}

/*****************************************************************************/
/*   Module : INI_MISC        Submodule : GetRamSize                         */
/*   Author : Lamp Matthias   Project   : PMC-1553 MON                       */
/*---------------------------------------------------------------------------*/
/*   Create : 10/10/2001      Update    : 24.07.2002                         */
/*---------------------------------------------------------------------------*/
/*    Inputs    : void                                                       */
/*    Outputs   : AiUInt32 RAM Size in Bytes                                 */
/*                                                                           */
/*    Description : look for the RAM size in Bytes                           */
/*                    ASP RAM, SSRAM, SDRAM added                                 */
/*****************************************************************************/
AiUInt32 global_RAM_SIZE_PMC(TY_MONITOR_DATA_STRUCT *GLOBAL_VAR)
{
    AiUInt32 size = 0, save, not_save, i, tmp, xy;
#if defined(_AIM_SYSDRV) && defined(_AIM_WINDOWS)
    LARGE_INTEGER x_Start;
    x_Start.QuadPart = read_Xtimestamp();
#endif

    if( (GLOBAL_VAR->PMC_DEVICEID_Reg.Reg.device_id == dveID_PMC32_ASP) || (GLOBAL_VAR->PMC_DEVICEID_Reg.Reg.device_id == dveID_PMC64_ASP) )
    {
        AI_Printf("\nwith ASP\n");

        /* aktivate SDRAM controller, write on to the register (usually done by the driver)*/
        GLOBAL_VAR->PMC_RESET_Reg.Reg.SDRAM_start = 1;
        PCI_IOReg_write(GLOBAL_VAR, PMC_RESET_Reg_Adr, GLOBAL_VAR->PMC_RESET_Reg.all);

        /* 1: GLOBAL RAM*/
        if( GLOBAL_VAR->RAM_Size_Reg.Reg.GRam_Size == 0 ) /*first time, works only once*/
        {
            save = PCI_Mem_read(GLOBAL_VAR, 0); /*reads value of address 0 (start address)*/
            not_save = ~save;
            i = 1;
            size = 0;

            /* this loop checks the RAM boundaries in 512kByte Steps*/
            while( (size == 0) && (i < 64) )
            {
                tmp = PCI_Mem_read(GLOBAL_VAR, (i * 128 * 1024)); /*save value from next test*/
                PCI_Mem_write(GLOBAL_VAR, (i * 128 * 1024), not_save); /*store neg value*/

                xy = PCI_Mem_read(GLOBAL_VAR, (i * 128 * 1024)); /*dummy read*/

                if( (PCI_Mem_read(GLOBAL_VAR, 0) != save) || (PCI_Mem_read(GLOBAL_VAR, (i * 128 * 1024)) != not_save) )
                    size = (i * 512 * 1024); /* get size value of the RAM*/

                PCI_Mem_write(GLOBAL_VAR, (i * 128 * 1024), tmp); /* write value back     */
                i++; /* next 512k*/
                if( i == 63 )
                {
                    size = (i * 512 * 1024);
                    AI_Printf("\nmaximum RAM");
                    break;
                }
            }

            PCI_Mem_write(GLOBAL_VAR, 0, save); /* write initial value back*/

            AI_Printf("\nGLOBAL RAM found, RAM SIZE=%8d [B]", size);
            AI_Printf("\nGLOBAL RAM found, RAM SIZE=%8d [kB]", (size / 1024));
            AI_Printf("\nGLOBAL RAM found, RAM SIZE=%8d [MB]", (size / 1024 / 1024));
            GLOBAL_VAR->RAM_Size_Reg.Reg.GRam_Size = (size / 1024 / 1024);
        }
        else
        {
            AI_Printf("GLOBAL RAM size already checked\n");
        }

        /* 2: SDRAM LOCAL (ASP)*/
        if( GLOBAL_VAR->RAM_Size_Reg.Reg.Local_ASPRam == 0 ) /*first time, works only once*/
        {
            save = PCI_ASPRam_read(GLOBAL_VAR, 0); /*reads value of address 0 (start address)*/
            not_save = ~save;
            i = 1;
            size = 0;

            /* this loop checks the RAM boundaries in 512kByte Steps*/
            while( (size == 0) && (i < 64) )
            {
                tmp = PCI_ASPRam_read(GLOBAL_VAR, (i * 128 * 1024)); /*save value from next test*/
                PCI_ASPRam_write(GLOBAL_VAR, (i * 128 * 1024), not_save); /*store neg value*/

                xy = PCI_ASPRam_read(GLOBAL_VAR, (i * 128 * 1024)); /*dummy read*/

                if( (PCI_ASPRam_read(GLOBAL_VAR, 0) != save) || (PCI_ASPRam_read(GLOBAL_VAR, (i * 128 * 1024)) != not_save) )
                    size = (i * 512 * 1024); /* get size value of the RAM*/

                PCI_ASPRam_write(GLOBAL_VAR, (i * 128 * 1024), tmp); /* write value back     */
                i++; /* next 512k*/
                if( i == 63 )
                {
                    size = (i * 512 * 1023);

                    AI_Printf("\nmaximum RAM");
                    break;
                }
            }

            PCI_ASPRam_write(GLOBAL_VAR, 0, save); /* write initial value back*/

            AI_Printf("\nLOCAL RAM found, RAM SIZE= %8d [B]", size);
            AI_Printf("\nLOCAL RAM found, RAM SIZE= %8d [kB]", (size / 1024));
            AI_Printf("\nLOCAL RAM found, RAM SIZE= %8d [MB]", (size / 1024 / 1024));
            GLOBAL_VAR->RAM_Size_Reg.Reg.Local_ASPRam = (size / 1024 / 1024);
        }
        else
            AI_Printf("\nLOCAL RAM size already checked\n");

        AI_Printf("\nGLOBAL RAM Size: %3dMB, ", GLOBAL_VAR->RAM_Size_Reg.Reg.GRam_Size);
        AI_Printf("\nLocal ASP RAM Size: %3dMB", GLOBAL_VAR->RAM_Size_Reg.Reg.Local_ASPRam);
    } /*end with ASP*/
    else
    {
        AI_Printf("\nwithout ASP\n");

#ifndef __monitor__
        if( 0 == GLOBAL_VAR->RAM_Size_Reg.Reg.GRam_Size ) /*first time, works only once*/
        {
#endif
            GLOBAL_VAR->RAM_Size_Reg.Reg.GRam_Size = 0; /*clear status*/

            save = PCI_Mem_read(GLOBAL_VAR, 0); /*reads value of address 0 (start address)*/
            not_save = ~save;
            i = 1;
            size = 0;

            /* this loop checks the RAM boundaries in 512kByte Steps*/
            while( (size == 0) && (i < 64) )
            {
                tmp = PCI_Mem_read(GLOBAL_VAR, (i * 128 * 1024)); /*save value from next test*/
                PCI_Mem_write(GLOBAL_VAR, (i * 128 * 1024), not_save); /*store neg value*/

                xy = PCI_Mem_read(GLOBAL_VAR, (i * 128 * 1024)); /*dummy read*/

                if( (PCI_Mem_read(GLOBAL_VAR, 0) != save) || (PCI_Mem_read(GLOBAL_VAR, (i * 128 * 1024)) != not_save) )
                    size = (i * 512 * 1024); /* get size value of the RAM*/

                PCI_Mem_write(GLOBAL_VAR, (i * 128 * 1024), tmp); /* write value back     */
                i++; /* next 512k*/
                if( i == 63 )
                {
                    size = (i * 512 * 1024);
                    AI_Printf("\nmaximum RAM");
                    break;
                }
            }

            PCI_Mem_write(GLOBAL_VAR, 0, save); /* write initial value back*/
            GLOBAL_VAR->RAM_Size_Reg.Reg.GRam_Size = (size / 1024 / 1024);
#ifndef __monitor__
        }
#endif

        AI_Printf("\nGLOBAL RAM found, RAM SIZE=%8d [B]", size);
        AI_Printf("\nGLOBAL RAM found, RAM SIZE=%8d [kB]", (size / 1024));
        AI_Printf("\nGLOBAL RAM found, RAM SIZE=%8d [MB]", (size / 1024 / 1024));
    }
#if defined(_AIM_SYSDRV) && defined(_AIM_WINDOWS)
    MonPrint ( DBG_INIT, FKT_MON, ("TIME: global_RAM_SIZE_PMC      %I64u\n", (read_Xtimestamp() - x_Start.QuadPart)));
#endif

    return (PASS);
}
/*****************************************************************************/
/*                                                                           */
/*   Module : mon_prog.c               Submodule : global_RAM_TEST_PMC       */
/*   Author : Matthias Lamp            Project   : API-ASP-MON               */
/*---------------------------------------------------------------------------*/
/*   Create : 10/10/2001    Update :                                         */
/*---------------------------------------------------------------------------*/
/*    Descriptions                                                           */
/*    ------------                                                           */
/*    Inputs    :                                                            */
/*                                                                           */
/*    Outputs   : -                                                          */
/*                                                                           */
/*    Description : Executes the selftests.                                  */
/*                                                                           */
/*****************************************************************************/
AiUInt32 global_RAM_TEST_PMC(TY_MONITOR_DATA_STRUCT *GLOBAL_VAR)
{
    AiUInt32 EAddress;
    AiUInt32 temp = PASS;
#if defined(_AIM_SYSDRV) && defined(_AIM_WINDOWS)
    LARGE_INTEGER x_Start;
    x_Start.QuadPart = read_Xtimestamp();
#endif

    EAddress = GLOBAL_VAR->RAM_Size_Reg.Reg.GRam_Size;
    EAddress = EAddress * 1024 - 4; /* MByte in KByte*/

    /*test the global ram*/
    if( ((ini_bite_pat_test(GLOBAL_VAR, 0, 60/*EAddress*/)) == FAIL) || ((ini_bite_adr_test(GLOBAL_VAR, 0, EAddress)) == FAIL)
                || ((ini_bite_walk0_test(GLOBAL_VAR, 0, 60/*EAddress*/)) == FAIL)
                || ((ini_bite_walk1_test(GLOBAL_VAR, 0, 60/*EAddress*/)) == FAIL) )
    {
        temp = FAIL;
    }
    AI_Printf("\nup to now, only single PCI Bus access is implemented needed time >1 houre?");

#if defined(_AIM_SYSDRV) && defined(_AIM_WINDOWS)
    MonPrint ( DBG_INIT, FKT_MON, ("TIME: global_RAM_TEST_PMC      %I64u\n", (read_Xtimestamp() - x_Start.QuadPart)));
#endif

    return (temp);
}

/*****************************************************************************/
/*                                                                           */
/*   Module : mon_prog.c               Submodule : global_RAM_TEST_full      */
/*   Author : Matthias Lamp            Project   : API-ASP-MON               */
/*---------------------------------------------------------------------------*/
/*   Create : 01/07/2004    Update :                                         */
/*---------------------------------------------------------------------------*/
/*    Descriptions                                                           */
/*    ------------                                                           */
/*    Inputs    :                                                            */
/*                                                                           */
/*    Outputs   : -                                                          */
/*                                                                           */
/*    Description : Executes the selftests.                                  */
/*                                                                           */
/*****************************************************************************/
AiUInt32 global_RAM_TEST_full(TY_MONITOR_DATA_STRUCT *GLOBAL_VAR)
{
    AiUInt32 i;
    AiUInt32 EAddress;
    AiUInt32 temp = PASS;

    EAddress = GLOBAL_VAR->RAM_Size_Reg.Reg.GRam_Size;
    EAddress = EAddress * 1024 * 1024 / 4 - 4; /*     MByte in Byte*/

    /*test the global ram*/
    for( i = 0; i < 3; i++ )
    {
        AI_Printf("\nIteration: %d", i);
        if( ((ini_bite_pat_test(GLOBAL_VAR, 0, EAddress)) == FAIL) || ((ini_bite_adr_test(GLOBAL_VAR, 0, EAddress)) == FAIL)
                    || ((ini_bite_walk0_test(GLOBAL_VAR, 0, EAddress)) == FAIL)
                    || ((ini_bite_walk1_test(GLOBAL_VAR, 0, EAddress)) == FAIL) )
        {
            AI_Printf("\nRAM TEST fail, iteration %x", i);
            temp = FAIL;
        }
        AI_Printf("\nup to now, only single PCI Bus access is implemented needed time >1 houre?");
    }

    return (temp);
}

/*****************************************************************************/
/*                                                                           */
/*   Module : mon_prog.c               Submodule : boot_IOFPGA               */
/*   Author : Lamp Matthias            Project   : PMC-1553-MON              */
/*---------------------------------------------------------------------------*/
/*   Create : 09.05.2001   Update :                                          */
/*---------------------------------------------------------------------------*/
/*    Descriptions                                                           */
/*    ------------                                                           */
/*    Inputs    : void                                                        */
/*                                                                           */
/*    Outputs   : PASS/FAIL                                                  */
/*                                                                           */
/*    Description : load srec for IO FPGA 1 and 2 and boots the FPGAs        */
/*                                                                           */
/*****************************************************************************/
AiUInt32 boot_IOFPGA_PMC(TY_MONITOR_DATA_STRUCT *GLOBAL_VAR)
{
    AiUInt32 x, y, tmp, mem_length;
#if defined(_AIM_SYSDRV) && defined(_AIM_WINDOWS)
    LARGE_INTEGER x_Start;
    LARGE_INTEGER x_Temp;
    x_Start.QuadPart = read_Xtimestamp();
#endif

    AI_Printf("\nBOOT IO-FPGA 1 and 2");

    GLOBAL_VAR->BOOT_CONTROL_Reg.Reg.FPGA = 0; /* clear status bit*/
    GLOBAL_VAR->PMC_IRQ_EVENT_Reg.all = 0;
    PCI_IOReg_write(GLOBAL_VAR, PMC_IRQ_EVENT_Reg_Adr, GLOBAL_VAR->PMC_IRQ_EVENT_Reg.all);

    /* reset BIU1- & BIU2-Prozessor & IO-FPGA 1 & 2*/
    GLOBAL_VAR->PMC_RESET_Reg.Reg.BIP1_RESx = 0; /* BIP1 in Reset*/
    GLOBAL_VAR->PMC_RESET_Reg.Reg.BIP2_RESx = 0; /* BIP2 in Reset*/
    GLOBAL_VAR->PMC_RESET_Reg.Reg.FPGA1_RESx = 0; /* FPGA1 in Reset*/
    GLOBAL_VAR->PMC_RESET_Reg.Reg.FPGA2_RESx = 0; /* FPGA2 in Reset*/
    PCI_IOReg_write(GLOBAL_VAR, PMC_RESET_Reg_Adr, GLOBAL_VAR->PMC_RESET_Reg.all); /* resets the BIPs & FPGAs       */

    GLOBAL_VAR->PMC_FPGA_Reg.Reg.BOOT_PROGx = 0; /* clearing Configuration memory*/
    PCI_IOReg_write(GLOBAL_VAR, PMC_FPGA_Reg_Adr, GLOBAL_VAR->PMC_FPGA_Reg.all);

    GLOBAL_VAR->PMC_FPGA_Reg.Reg.BOOT_INITx = 1; /* just to be sure that I red the 0*/
    GLOBAL_VAR->PMC_FPGA_Reg.Reg.BOOT_DONE = 1; /* just to be sure that I red the 0*/

    tmp = 0;

    /* 1:*/
    while( GLOBAL_VAR->PMC_FPGA_Reg.Reg.BOOT_INITx == 1 ) /* wait for INITx=0*/
    {
        wait_clocks(GLOBAL_VAR, 100); /*100us*/
        GLOBAL_VAR->PMC_FPGA_Reg.all = PCI_IOReg_read(GLOBAL_VAR,
        PMC_FPGA_Reg_Adr);

        tmp++;
        if( tmp > 500 ) /* no given timing constant lets say 50ms*/
        {
            AI_Printf("\n ERROR IO-FPGA's does not response INITx = 0 (clear config. mem.) ");
            return (FAIL);
        }
    }

    /* 1.1    DONE must also be ZERO*/
    tmp = 0;
    while( GLOBAL_VAR->PMC_FPGA_Reg.Reg.BOOT_DONE == 1 ) /* wait for DONE=0*/
    {
        wait_clocks(GLOBAL_VAR, 100); /*100us*/
        GLOBAL_VAR->PMC_FPGA_Reg.all = PCI_IOReg_read(GLOBAL_VAR,
        PMC_FPGA_Reg_Adr);

        tmp++;
        if( tmp > 500 ) /* no given timing constant lets say 50ms*/
        {
            AI_Printf("\n ERROR IO-FPGA's does not response DONE = 0 (clear config. mem.) ");
            return (FAIL);
        }
    }

    /* 2:                put PROGRAM pin high*/
    wait_clocks(GLOBAL_VAR, 100); /*100us*/
    GLOBAL_VAR->PMC_FPGA_Reg.Reg.BOOT_PROGx = 1; /* FPGA is clearing memory, wait for INITx=1*/
    PCI_IOReg_write(GLOBAL_VAR, PMC_FPGA_Reg_Adr, GLOBAL_VAR->PMC_FPGA_Reg.all);

    /* 3:*/
    tmp = 0;
    while( GLOBAL_VAR->PMC_FPGA_Reg.Reg.BOOT_INITx == 0 ) /* wait for INITx=1 (mem. is cleared)*/
    {
        wait_clocks(GLOBAL_VAR, 100); /*100us*/
        GLOBAL_VAR->PMC_FPGA_Reg.all = PCI_IOReg_read(GLOBAL_VAR,
        PMC_FPGA_Reg_Adr);

        tmp++;
        if( tmp > 500 ) /* no given timing constant lets say 50ms*/
        {
            AI_Printf("\n ERROR IO-FPGA's does not response INITx = 1 (clearing config.mem finish) ");
            return (FAIL);
        }
    }

    AI_Printf("\nIO-FPGA 1 and 2 configuration memory is cleared");
    AI_Printf("\nLoading configuration data from hard disk");

    mem_length = Load_Srec(GLOBAL_VAR, 1, GLOBAL_VAR->GLOBAL_TEMP_MEM);
    if( mem_length == FAIL ) /* 1 = SRECORD Type: IO-FPGA 1*/
    {
        AI_Printf("\nERROR by Loading IO-FPGA 1 S-Record\n");
        return (FAIL);
    }

    /* boot IO-FPGA 1*/
    /* initialization*/
    GLOBAL_VAR->PMC_FPGA_Reg.Reg.BOOT_CS0x = 0; /* CS1x   = 0*/
    GLOBAL_VAR->PMC_FPGA_Reg.Reg.BOOT_CS1x = 1; /* CS2x   = 1*/
    PCI_IOReg_write(GLOBAL_VAR, PMC_FPGA_Reg_Adr, GLOBAL_VAR->PMC_FPGA_Reg.all);

#if defined(_AIM_SYSDRV) && defined(_AIM_WINDOWS)
    x_Temp.QuadPart = read_Xtimestamp();
#endif

    for( x = 0; x <= mem_length; x++ )
    {
        GLOBAL_VAR->PMC_FPGA_Reg.Reg.BOOT_DATA = *(GLOBAL_VAR->GLOBAL_TEMP_MEM + x);
        GLOBAL_VAR->PMC_FPGA_Reg.Reg.BOOT_CLK = 1; /* Clock to 1*/
        /* to 0 by the FPGA*/
        PCI_IOReg_write(GLOBAL_VAR, PMC_FPGA_Reg_Adr, GLOBAL_VAR->PMC_FPGA_Reg.all);

        if( (x % 100) == 0 )
        {
            MON_ONLY_PRINTF("*");
            if( (x % 1000) == 0 )
                MON_ONLY_PRINTF("\r           %08x\r",x);
        }
        if( x >= 512 * 1024 ) /*overrun*/
        {
            AI_Printf("\nERROR end of memory block \\0 not found (IO-FPGA 1)");
            break; /*end not found*/
        }
    }
#if defined(_AIM_SYSDRV) && defined(_AIM_WINDOWS)
    MonPrint ( DBG_INIT, FKT_MON, ("TIME: boot_IOFPGA_PMC (while)     %I64u\n", (read_Xtimestamp() - x_Temp.QuadPart)));
#endif

    /* boot IO-FPGA 2*/
    if( (GLOBAL_VAR->PMC_DEVICEID_Reg.Reg.device_id != dveID_PMC32_ASP) && (GLOBAL_VAR->PMC_DEVICEID_Reg.Reg.device_id != dveID_PMC64_ASP)
                && (GLOBAL_VAR->PMC_DEVICEID_Reg.Reg.device_id != dveID_PMC32_2nd) && /*V02.10 new 4ch. 1553 Board only 1 FPGA*/
                (GLOBAL_VAR->PMC_DEVICEID_Reg.Reg.dev_type_BIP2 != 0xff) ) /* V02.04 added, no BIU2 present */
    { /* if ASP, no BIU2, therefore no IO-FPGA 2*/
        /*      &IO_FPGA_MEM_2 address pointer to loaded s-record*/
        mem_length = Load_Srec(GLOBAL_VAR, 2, GLOBAL_VAR->GLOBAL_TEMP_MEM);
        if( mem_length == FAIL ) /* 2 = SRECORD Type: IO-FPGA 2*/
        {

            AI_Printf("\nERROR by Loading IO-FPGA 2 S-Record\n");
            return (FAIL);
        }

        /* initialization*/
        GLOBAL_VAR->PMC_FPGA_Reg.Reg.BOOT_CS0x = 1; /* CS1x   = 1*/
        GLOBAL_VAR->PMC_FPGA_Reg.Reg.BOOT_CS1x = 0; /* CS2x   = 0*/
        PCI_IOReg_write(GLOBAL_VAR, PMC_FPGA_Reg_Adr, GLOBAL_VAR->PMC_FPGA_Reg.all);

#if defined(_AIM_SYSDRV) && defined(_AIM_WINDOWS)
        x_Temp.QuadPart = read_Xtimestamp();
#endif

        x = 0;
        for( x = 0; x <= mem_length; x++ )
        {
            GLOBAL_VAR->PMC_FPGA_Reg.Reg.BOOT_DATA = *(GLOBAL_VAR->GLOBAL_TEMP_MEM + x);
            GLOBAL_VAR->PMC_FPGA_Reg.Reg.BOOT_CLK = 1; /* Clock to 1*/
            /* to 0 by the FPGA*/
            PCI_IOReg_write(GLOBAL_VAR, PMC_FPGA_Reg_Adr, GLOBAL_VAR->PMC_FPGA_Reg.all);

            if( (x % 100) == 0 )
            {
                MON_ONLY_PRINTF("*");
                if( x % 1000 == 0 )
                    MON_ONLY_PRINTF("\r           %08x\r",x);
            }

            if( x >= 512 * 1024 ) /*overrun*/
            {
                AI_Printf("\nERROR end of memory block \\0 not found (IO-FPGA 2)");
                break; /*end not found*/
            }
        }
#if defined(_AIM_SYSDRV) && defined(_AIM_WINDOWS)
        MonPrint ( DBG_INIT, FKT_MON, ("TIME: boot_IOFPGA_PMC (while2)     %I64u\n", (read_Xtimestamp() - x_Temp.QuadPart)));
#endif
    } /*end if ASP*/

    /* finish boot sequenze*/
    /* look if /INIT=low, if so error by calculating CRC  ERROR*/
    GLOBAL_VAR->PMC_FPGA_Reg.Reg.BOOT_CS0x = 1; /* CS1x   = 1*/
    GLOBAL_VAR->PMC_FPGA_Reg.Reg.BOOT_CS1x = 1; /* CS2x   = 1*/
    PCI_IOReg_write(GLOBAL_VAR, PMC_FPGA_Reg_Adr, GLOBAL_VAR->PMC_FPGA_Reg.all); /* write*/

    for( y = 0; y < 0x20; y++ ) /* 32 CLK pulses*/
    {
        GLOBAL_VAR->PMC_FPGA_Reg.Reg.BOOT_DATA = 0xaf; /* dummy Data*/
        GLOBAL_VAR->PMC_FPGA_Reg.Reg.BOOT_CLK = 1; /* Clock to 1*/
        /* to 0 by the FPGA*/
        PCI_IOReg_write(GLOBAL_VAR, PMC_FPGA_Reg_Adr, GLOBAL_VAR->PMC_FPGA_Reg.all);
    }

    /* the END routine*/

    /*    wait for DONE goes high (a given time)*/
    /*  boot ERROR*/
    /*  if DONE stays low, check if INIT = low, ifso CRC ERROR*/
    /* else success*/
    tmp = PASS;
    x = 0;
    y = 0;
    while( GLOBAL_VAR->PMC_FPGA_Reg.Reg.BOOT_DONE == 0 ) /*as long as DONE=0*/
    {
        wait_clocks(GLOBAL_VAR, 100); /* wait 100 us*/
        GLOBAL_VAR->PMC_FPGA_Reg.all = PCI_IOReg_read(GLOBAL_VAR,
        PMC_FPGA_Reg_Adr);

        x++;
        if( x >= 100 ) /*2nd break down condition*/
        {
            tmp = FAIL;
            AI_Printf("\nERROR by booting IO-FPGA's (DONE=0) \n");

            while( GLOBAL_VAR->PMC_FPGA_Reg.Reg.BOOT_INITx == 0 )
            {
                wait_clocks(GLOBAL_VAR, 100); /* wait 100 us*/
                GLOBAL_VAR->PMC_FPGA_Reg.all = PCI_IOReg_read(GLOBAL_VAR,
                PMC_FPGA_Reg_Adr);

                y++;
                if( y >= 100 ) /*2nd break down condition*/
                {
                    AI_Printf("\nCRC ERROR by booting IO-FPGA's (INITx=0)\n");
                    break;
                }
            }
            break;
        }
    }

    /* leave reset FPGA 1 & 2*/
    GLOBAL_VAR->PMC_RESET_Reg.Reg.FPGA1_RESx = 1; /* FPGA 1 out of Reset*/
    GLOBAL_VAR->PMC_RESET_Reg.Reg.FPGA2_RESx = 1; /* FPGA 2 out of Reset*/
    PCI_IOReg_write(GLOBAL_VAR, PMC_RESET_Reg_Adr, GLOBAL_VAR->PMC_RESET_Reg.all); /* resets the BIP       */

    if( tmp == PASS )
    {
        GLOBAL_VAR->BOOT_CONTROL_Reg.Reg.FPGA = 1; /* status bit FPGA booted*/
        AI_Printf("\nBOOT IO-FPGA 1 and 2 passed");
#if defined(_AIM_SYSDRV) && defined(_AIM_WINDOWS)
        MonPrint ( DBG_INIT, FKT_MON, ("TIME: boot_IOFPGA_PMC      %I64u\n", (read_Xtimestamp() - x_Start.QuadPart)));
#endif
    }

    return (tmp);
}

/*****************************************************************************/
/*                                                                           */
/*   Module : mon_prog.c               Submodule : boot_BIP1_PMC             */
/*   Author : Lamp Matthias            Project   : PMC-1553-MON              */
/*---------------------------------------------------------------------------*/
/*   Create : 16.10.2001   Update :                                          */
/*---------------------------------------------------------------------------*/
/*    Descriptions                                                           */
/*    ------------                                                           */
/*    Inputs    : AiUInt32  void                                                */
/*                                                                           */
/*    Outputs   : PASS/FAIL                                                  */
/*                                                                           */
/*    Description : copy BIU 1 Prozessor boot code into GLOBAL RAM,          */
/*                  and starts the processor                                 */
/*                                                                           */
/*****************************************************************************/
AiUInt32 boot_BIP1_PMC(TY_MONITOR_DATA_STRUCT *GLOBAL_VAR)
{
    AiUInt32 temp, x, y, mem_length;

    AiUInt8 *BIP1_MEM;
#if defined(_AIM_SYSDRV) && defined(_AIM_WINDOWS)
    LARGE_INTEGER x_Start;
    x_Start.QuadPart = read_Xtimestamp();
#endif

    BIP1_MEM = GLOBAL_VAR->GLOBAL_TEMP_MEM;

    /******************************************************************************************/

    GLOBAL_VAR->BOOT_CONTROL_Reg.Reg.BIP_1_boot = 0; /* clear status bit*/

    /* reset TRSTx*/
    GLOBAL_VAR->PMC_JTAG_KEY_Reg = JTAG_Key; /*enable write access to JTAG Port*/
    PCI_IOReg_write(GLOBAL_VAR, PMC_JTAG_KEY_Reg_Adr, GLOBAL_VAR->PMC_JTAG_KEY_Reg);

    GLOBAL_VAR->PMC_JTAG_Reg.Reg.JTAG_OE = 1;
    GLOBAL_VAR->PMC_JTAG_Reg.Reg.TCK = 0;
    GLOBAL_VAR->PMC_JTAG_Reg.Reg.TDO = 0;
    GLOBAL_VAR->PMC_JTAG_Reg.Reg.TMS = 1;
    GLOBAL_VAR->PMC_JTAG_Reg.Reg.TRSTx = 0;
    PCI_IOReg_write(GLOBAL_VAR, PMC_JTAG_Reg_Adr, GLOBAL_VAR->PMC_JTAG_Reg.all);
    wait_clocks(GLOBAL_VAR, 2000);

    /* 1: Initialization, IRQ things, processor in RESET*/
    if( (GLOBAL_VAR->PMC_DEVICEID_Reg.Reg.device_id != dveID_PMC32_ASP) && /* device ID undefined, old AMC1553 FPGA design,*/
    (GLOBAL_VAR->PMC_DEVICEID_Reg.Reg.device_id != dveID_PMC64_ASP) && /* => bit Order in PMC_IRQ_EVENT_Reg is    */
    (GLOBAL_VAR->PMC_DEVICEID_Reg.Reg.device_id != dveID_PMC32_2nd) && /* different.*/
    (GLOBAL_VAR->PMC_DEVICEID_Reg.Reg.device_id != dveID_PMC32) && /* v01.05 PC-Card added*/
    (GLOBAL_VAR->PMC_DEVICEID_Reg.Reg.device_id != dveID_PCCard_1553) )
        GLOBAL_VAR->PMC_IRQ_EVENT_Reg.Reg.host_BIU1_o = 0; /* clear, to be sure*/
    else
        GLOBAL_VAR->PMC_IRQ_EVENT_Reg.Reg.host_BIU1 = 0; /* clear, to be sure*/

    GLOBAL_VAR->PMC_IRQ_EVENT_Reg.Reg.BIU1_host = 1; /* write one to clear Interrupt*/
    GLOBAL_VAR->PMC_IRQ_EVENT_Reg.Reg.enable = 0; /* no PCI Interrupt, only register*/
    PCI_IOReg_write(GLOBAL_VAR, PMC_IRQ_EVENT_Reg_Adr, GLOBAL_VAR->PMC_IRQ_EVENT_Reg.all);

    GLOBAL_VAR->PMC_IRQ_MASK_Reg.Reg.mask_0 = 0; /* enable BIU 1 to host Interrupt*/
    PCI_IOReg_write(GLOBAL_VAR, PMC_IRQ_MASK_Reg_Adr, GLOBAL_VAR->PMC_IRQ_MASK_Reg.all);

    GLOBAL_VAR->PMC_RESET_Reg.Reg.BIP1_RESx = 0; /* BIP 1 in RESET*/
    PCI_IOReg_write(GLOBAL_VAR, PMC_RESET_Reg_Adr, GLOBAL_VAR->PMC_RESET_Reg.all);

    /* 2: Load S-Rec*/
    mem_length = Load_Srec(GLOBAL_VAR, 3, BIP1_MEM);

#ifdef _AIM_WINDOWS
    //  ul_WriteMemToFile( "biu_sre0.mem", BIP1_MEM, mem_length);
#endif

    if( mem_length == FAIL ) /* 3 = SRECORD Type BIP 1*/
    { /* BIP1_MEM memory pointer to decoded s-rec*/
        AI_Printf("\nERROR by Loading BIP 1 S-Record\n");
        GLOBAL_VAR->PMC_JTAG_KEY_Reg = JTAG_Key; /*enable write access to JTAG Port*/
        PCI_IOReg_write(GLOBAL_VAR, PMC_JTAG_KEY_Reg_Adr, GLOBAL_VAR->PMC_JTAG_KEY_Reg);

        GLOBAL_VAR->PMC_JTAG_Reg.Reg.JTAG_OE = 0; /*disable JTAG port*/
        PCI_IOReg_write(GLOBAL_VAR, PMC_JTAG_Reg_Adr, GLOBAL_VAR->PMC_JTAG_Reg.all);

        return (FAIL);
    }

    /* 3: copy S-Record into GLOBAL RAM (AiUInt8 to AiUInt32)*/

    AI_Printf("\n");

    y = 0;
    for( x = 0; x <= mem_length; x = x + 4 )
    {
        temp = 0;
        temp = *(BIP1_MEM + x);
        temp = temp | (*(BIP1_MEM + x + 1) << 8);
        temp = temp | (*(BIP1_MEM + x + 2) << 16);
        temp = temp | (*(BIP1_MEM + x + 3) << 24);

        PCI_Mem_write(GLOBAL_VAR, y, temp);
        y++;
        if( x % 100 == 0 )
        {
            MON_ONLY_PRINTF("*");
            if( x % 1000 == 0 )
                MON_ONLY_PRINTF("\r           %08x\r",x);
        }

        if( x > 256 * 1024 )
        {
            AI_Printf("ERROR mem_length");
            break;
        }
    }
    AI_Printf("\r           %08x\r", x);

    /* 4: processor out of RESET*/
    GLOBAL_VAR->PMC_RESET_Reg.Reg.BIP1_RESx = 1; /* BIP 1 out of RESET*/
    PCI_IOReg_write(GLOBAL_VAR, PMC_RESET_Reg_Adr, GLOBAL_VAR->PMC_RESET_Reg.all);

    /* TRSTx out of reset*/
    GLOBAL_VAR->PMC_JTAG_KEY_Reg = JTAG_Key; /*enable write access to JTAG Port*/
    PCI_IOReg_write(GLOBAL_VAR, PMC_JTAG_KEY_Reg_Adr, GLOBAL_VAR->PMC_JTAG_KEY_Reg);

    GLOBAL_VAR->PMC_JTAG_Reg.Reg.JTAG_OE = 1;
    GLOBAL_VAR->PMC_JTAG_Reg.Reg.TCK = 0;
    GLOBAL_VAR->PMC_JTAG_Reg.Reg.TDO = 0;
    GLOBAL_VAR->PMC_JTAG_Reg.Reg.TMS = 0;
    GLOBAL_VAR->PMC_JTAG_Reg.Reg.TRSTx = 1;
    PCI_IOReg_write(GLOBAL_VAR, PMC_JTAG_Reg_Adr, GLOBAL_VAR->PMC_JTAG_Reg.all);

    /* 5: wait for BIU 1 IRQ*/
    AI_Printf("\nWait for BIP 1 boot IRQ ...\n");

    x = 0;
    GLOBAL_VAR->PMC_IRQ_EVENT_Reg.Reg.BIU1_host = 0;
    /* pull register to wait for IRQ*/
    while( GLOBAL_VAR->PMC_IRQ_EVENT_Reg.Reg.BIU1_host == 0 ) /* BIU 1 sends Interrupt (boot OK)*/
    {
        wait_clocks(GLOBAL_VAR, 1000); /* wait 2 msec*/

        AI_Printf("\rBIU_1 to host IRQ: %x", GLOBAL_VAR->PMC_IRQ_EVENT_Reg.Reg.BIU1_host);

        GLOBAL_VAR->PMC_IRQ_EVENT_Reg.all = PCI_IOReg_read(GLOBAL_VAR,
        PMC_IRQ_EVENT_Reg_Adr);

        if( x >= 50000 ) /* time-out*/
        {
            AI_Printf("\n!!!ERROR!!! BIU_1 does not response boot IRQ");
            AI_Printf("\n%d", x);
            GLOBAL_VAR->PMC_IRQ_EVENT_Reg.Reg.enable = 0; /* no PCI Interrupt, only register*/
            PCI_IOReg_write(GLOBAL_VAR, PMC_IRQ_EVENT_Reg_Adr, GLOBAL_VAR->PMC_IRQ_EVENT_Reg.all);

            GLOBAL_VAR->PMC_IRQ_MASK_Reg.Reg.mask_0 = 1; /* disable BIU 1 to host Interrupt*/
            PCI_IOReg_write(GLOBAL_VAR, PMC_IRQ_MASK_Reg_Adr, GLOBAL_VAR->PMC_IRQ_MASK_Reg.all);

            AI_Printf("\nerror no boot IRQ ready");

            GLOBAL_VAR->PMC_JTAG_KEY_Reg = JTAG_Key; /*enable write access to JTAG Port*/
            PCI_IOReg_write(GLOBAL_VAR, PMC_JTAG_KEY_Reg_Adr, GLOBAL_VAR->PMC_JTAG_KEY_Reg);

            GLOBAL_VAR->PMC_JTAG_Reg.Reg.JTAG_OE = 0; /*disable JTAG port*/
            PCI_IOReg_write(GLOBAL_VAR, PMC_JTAG_Reg_Adr, GLOBAL_VAR->PMC_JTAG_Reg.all);

            return (FAIL);
        }
        x++;
    }

    /* 6: boot IRQ OK*/

    AI_Printf("\nBIU 1 successfully loaded");

    /* reset interrupt controll*/
    GLOBAL_VAR->PMC_IRQ_EVENT_Reg.Reg.BIU1_host = 1; /* write one to clear Interrupt*/
    GLOBAL_VAR->PMC_IRQ_EVENT_Reg.Reg.enable = 0; /* no PCI Interrupt, only register*/
    PCI_IOReg_write(GLOBAL_VAR, PMC_IRQ_EVENT_Reg_Adr, GLOBAL_VAR->PMC_IRQ_EVENT_Reg.all);

    GLOBAL_VAR->PMC_IRQ_MASK_Reg.Reg.mask_0 = 1; /* disable BIU 1 to host Interrupt*/
    PCI_IOReg_write(GLOBAL_VAR, PMC_IRQ_MASK_Reg_Adr, GLOBAL_VAR->PMC_IRQ_MASK_Reg.all);

    GLOBAL_VAR->BOOT_CONTROL_Reg.Reg.BIP_1_boot = 1; /* BIP 1 boot OK*/

    GLOBAL_VAR->PMC_JTAG_KEY_Reg = JTAG_Key; /*enable write access to JTAG Port*/
    PCI_IOReg_write(GLOBAL_VAR, PMC_JTAG_KEY_Reg_Adr, GLOBAL_VAR->PMC_JTAG_KEY_Reg);

    GLOBAL_VAR->PMC_JTAG_Reg.Reg.JTAG_OE = 0; /*disable JTAG port*/
    PCI_IOReg_write(GLOBAL_VAR, PMC_JTAG_Reg_Adr, GLOBAL_VAR->PMC_JTAG_Reg.all);
#if defined(_AIM_SYSDRV) && defined(_AIM_WINDOWS)
    MonPrint ( DBG_INIT, FKT_MON, ("TIME: boot_BIP1_PMC      %I64u\n", (read_Xtimestamp() - x_Start.QuadPart)));
#endif

    return (PASS);
}

/*****************************************************************************/
/*                                                                           */
/*   Module : mon_prog.c               Submodule : boot_BIP2_PMC             */
/*   Author : Lamp Matthias            Project   : PMC-1553-MON              */
/*---------------------------------------------------------------------------*/
/*   Create : 16.10.2001   Update :                                          */
/*---------------------------------------------------------------------------*/
/*    Descriptions                                                           */
/*    ------------                                                           */
/*    Inputs    : AiUInt32  void                                                */
/*                                                                           */
/*    Outputs   : PASS/FAIL                                                  */
/*                                                                           */
/*    Description : copy BIU 2 Prozessor boot code into GLOBAL RAM,          */
/*                  and starts the processor                                 */
/*                                                                           */
/*****************************************************************************/
AiUInt32 boot_BIP2_PMC(TY_MONITOR_DATA_STRUCT *GLOBAL_VAR)
{
    AiUInt32 temp, x, y, mem_length;
    AiUInt8 *BIP2_MEM;

#if defined(_AIM_SYSDRV) && defined(_AIM_WINDOWS)
    LARGE_INTEGER x_Start;
    x_Start.QuadPart = read_Xtimestamp();
#endif

    BIP2_MEM = GLOBAL_VAR->GLOBAL_TEMP_MEM;

    /******************************************************************************************/

    GLOBAL_VAR->BOOT_CONTROL_Reg.Reg.BIP_2_boot = 0; /* clear status bit*/

    /* reset TRSTx*/
    GLOBAL_VAR->PMC_JTAG_KEY_Reg = JTAG_Key; /*enable write access to JTAG Port*/
    PCI_IOReg_write(GLOBAL_VAR, PMC_JTAG_KEY_Reg_Adr, GLOBAL_VAR->PMC_JTAG_KEY_Reg);

    GLOBAL_VAR->PMC_JTAG_Reg.Reg.JTAG_OE = 1;
    GLOBAL_VAR->PMC_JTAG_Reg.Reg.TCK = 0;
    GLOBAL_VAR->PMC_JTAG_Reg.Reg.TDO = 0;
    GLOBAL_VAR->PMC_JTAG_Reg.Reg.TMS = 1;
    GLOBAL_VAR->PMC_JTAG_Reg.Reg.TRSTx = 0;
    PCI_IOReg_write(GLOBAL_VAR, PMC_JTAG_Reg_Adr, GLOBAL_VAR->PMC_JTAG_Reg.all);
    wait_clocks(GLOBAL_VAR, 2000);

    /* 1: Initialization, IRQ things, processor in RESET*/
    GLOBAL_VAR->PMC_IRQ_EVENT_Reg.Reg.BIU2_host = 1; /* write one to clear Interrupt*/
    if( (GLOBAL_VAR->PMC_DEVICEID_Reg.Reg.device_id != dveID_PMC32_ASP) && /* device ID undefined, old AMC1553 FPGA design,*/
    (GLOBAL_VAR->PMC_DEVICEID_Reg.Reg.device_id != dveID_PMC64_ASP) && /* => bit Order in PMC_IRQ_EVENT_Reg is    */
    (GLOBAL_VAR->PMC_DEVICEID_Reg.Reg.device_id != dveID_PMC32) && /* different.*/
    (GLOBAL_VAR->PMC_DEVICEID_Reg.Reg.device_id != dveID_PMC32_2nd) && /* v01.05 PC-Card added*/
    (GLOBAL_VAR->PMC_DEVICEID_Reg.Reg.device_id != dveID_PCCard_1553) )
        GLOBAL_VAR->PMC_IRQ_EVENT_Reg.Reg.host_BIU2_o = 0; /* clear, to be sure*/
    else
        GLOBAL_VAR->PMC_IRQ_EVENT_Reg.Reg.host_BIU2 = 0; /* clear, to be sure*/

    GLOBAL_VAR->PMC_IRQ_EVENT_Reg.Reg.enable = 0; /* no PCI Interrupt, only register*/
    PCI_IOReg_write(GLOBAL_VAR, PMC_IRQ_EVENT_Reg_Adr, GLOBAL_VAR->PMC_IRQ_EVENT_Reg.all);

    GLOBAL_VAR->PMC_IRQ_MASK_Reg.Reg.mask_1 = 0; /* enable BIU 2 to host Interrupt*/
    PCI_IOReg_write(GLOBAL_VAR, PMC_IRQ_MASK_Reg_Adr, GLOBAL_VAR->PMC_IRQ_MASK_Reg.all);

    GLOBAL_VAR->PMC_RESET_Reg.Reg.BIP2_RESx = 0; /* BIP 2 in RESET*/
    PCI_IOReg_write(GLOBAL_VAR, PMC_RESET_Reg_Adr, GLOBAL_VAR->PMC_RESET_Reg.all);

    /* 2: Load S-Rec*/
    mem_length = Load_Srec(GLOBAL_VAR, 4, BIP2_MEM);
    if( mem_length == FAIL ) /* 4 = SRECORD Type BIP 2*/
    { /* BIP1_MEM memory pointer to decoded s-rec*/
        AI_Printf("\nERROR by Loading BIP 2 S-Record\n");
        GLOBAL_VAR->PMC_JTAG_KEY_Reg = JTAG_Key; /*enable write access to JTAG Port*/
        PCI_IOReg_write(GLOBAL_VAR, PMC_JTAG_KEY_Reg_Adr, GLOBAL_VAR->PMC_JTAG_KEY_Reg);

        GLOBAL_VAR->PMC_JTAG_Reg.Reg.JTAG_OE = 0; /*disable JTAG port*/
        PCI_IOReg_write(GLOBAL_VAR, PMC_JTAG_Reg_Adr, GLOBAL_VAR->PMC_JTAG_Reg.all);

        return (FAIL);
    }

    /* 3: copy S-Record into GLOBAL RAM (AiUInt8 to AiUInt32)*/
    AI_Printf("\n");
    y = 0;
    for( x = 0; x <= mem_length; x = x + 4 )
    {
        temp = 0;
        temp = *(BIP2_MEM + x);
        temp = temp | (*(BIP2_MEM + x + 1) << 8);
        temp = temp | (*(BIP2_MEM + x + 2) << 16);
        temp = temp | (*(BIP2_MEM + x + 3) << 24);

        PCI_Mem_write(GLOBAL_VAR, y, temp);
        y++;
        if( x % 100 == 0 )
        {
            MON_ONLY_PRINTF("*");
            if( x % 1000 == 0 )
                MON_ONLY_PRINTF("\r           %08x\r",x);
        }
        if( x > 256 * 1024 )
        {
            AI_Printf("ERROR mem_length");
            break;
        }
    }
    AI_Printf("\r           %08x\r", x);

    /* 4: processor out of RESET*/
    GLOBAL_VAR->PMC_RESET_Reg.Reg.BIP2_RESx = 1; /* BIP 2 out of RESET*/
    PCI_IOReg_write(GLOBAL_VAR, PMC_RESET_Reg_Adr, GLOBAL_VAR->PMC_RESET_Reg.all);

    /* TRSTx out of reset*/
    GLOBAL_VAR->PMC_JTAG_Reg.Reg.TRSTx = 1;
    PCI_IOReg_write(GLOBAL_VAR, PMC_JTAG_Reg_Adr, GLOBAL_VAR->PMC_JTAG_Reg.all);

    /* 5: wait for BIU 2 IRQ*/
    AI_Printf("\nWait for BIP 2 boot IRQ ...\n");

    x = 0;
    GLOBAL_VAR->PMC_IRQ_EVENT_Reg.Reg.BIU2_host = 0;

    /* pull register to wait for IRQ*/
    while( GLOBAL_VAR->PMC_IRQ_EVENT_Reg.Reg.BIU2_host == 0 ) /* BIU 2 sends Interrupt (boot OK)*/
    {
        wait_clocks(GLOBAL_VAR, 1000);

        AI_Printf("\rBIU_2 to host IRQ: %x", GLOBAL_VAR->PMC_IRQ_EVENT_Reg.Reg.BIU2_host);

        GLOBAL_VAR->PMC_IRQ_EVENT_Reg.all = PCI_IOReg_read(GLOBAL_VAR,
        PMC_IRQ_EVENT_Reg_Adr);

        if( x >= 50000 ) /* time-out*/
        {
            AI_Printf("\n!!!ERROR!!! BIU_2 does not response boot IRQ");
            AI_Printf("\n%d", x);
            GLOBAL_VAR->PMC_IRQ_EVENT_Reg.Reg.enable = 0; /* no PCI Interrupt, only register*/
            PCI_IOReg_write(GLOBAL_VAR, PMC_IRQ_EVENT_Reg_Adr, GLOBAL_VAR->PMC_IRQ_EVENT_Reg.all);

            GLOBAL_VAR->PMC_IRQ_MASK_Reg.Reg.mask_1 = 1; /* disable BIU 2 to host Interrupt*/
            PCI_IOReg_write(GLOBAL_VAR, PMC_IRQ_MASK_Reg_Adr, GLOBAL_VAR->PMC_IRQ_MASK_Reg.all);
            AI_Printf("\nerror no boot IRQ ready");
            GLOBAL_VAR->PMC_JTAG_KEY_Reg = JTAG_Key; /*enable write access to JTAG Port*/
            PCI_IOReg_write(GLOBAL_VAR, PMC_JTAG_KEY_Reg_Adr, GLOBAL_VAR->PMC_JTAG_KEY_Reg);

            GLOBAL_VAR->PMC_JTAG_Reg.Reg.JTAG_OE = 0; /*disable JTAG port*/
            PCI_IOReg_write(GLOBAL_VAR, PMC_JTAG_Reg_Adr, GLOBAL_VAR->PMC_JTAG_Reg.all);

            return (FAIL);
        }
        x++;
    }

    /* 6: boot IRQ OK*/

    AI_Printf("\nBIU 2 successfully loaded");

    /* reset interrupt controll*/
    GLOBAL_VAR->PMC_IRQ_EVENT_Reg.Reg.BIU2_host = 1; /* write one to clear Interrupt*/
    GLOBAL_VAR->PMC_IRQ_EVENT_Reg.Reg.enable = 0; /* no PCI Interrupt, only register*/
    PCI_IOReg_write(GLOBAL_VAR, PMC_IRQ_EVENT_Reg_Adr, GLOBAL_VAR->PMC_IRQ_EVENT_Reg.all);

    GLOBAL_VAR->PMC_IRQ_MASK_Reg.Reg.mask_1 = 1; /* disable BIU 1 to host Interrupt*/
    PCI_IOReg_write(GLOBAL_VAR, PMC_IRQ_MASK_Reg_Adr, GLOBAL_VAR->PMC_IRQ_MASK_Reg.all);

    GLOBAL_VAR->BOOT_CONTROL_Reg.Reg.BIP_2_boot = 1; /* BIP 1 boot OK*/

    GLOBAL_VAR->PMC_JTAG_KEY_Reg = JTAG_Key; /*enable write access to JTAG Port*/
    PCI_IOReg_write(GLOBAL_VAR, PMC_JTAG_KEY_Reg_Adr, GLOBAL_VAR->PMC_JTAG_KEY_Reg);

    GLOBAL_VAR->PMC_JTAG_Reg.Reg.JTAG_OE = 0; /*disable JTAG port*/
    PCI_IOReg_write(GLOBAL_VAR, PMC_JTAG_Reg_Adr, GLOBAL_VAR->PMC_JTAG_Reg.all);
#if defined(_AIM_SYSDRV) && defined(_AIM_WINDOWS)
    MonPrint ( DBG_INIT, FKT_MON, ("TIME: boot_BIP2_PMC      %I64u\n", (read_Xtimestamp() - x_Start.QuadPart)));
#endif

    return (PASS);
}

/*****************************************************************************/
/*                                                                           */
/*   Module : mon_prog.c               Submodule : boot_ASP_PMC              */
/*   Author : Lamp Matthias            Project   : PMC-1553-MON              */
/*---------------------------------------------------------------------------*/
/*   Create : 16.07.2002   Update :  04.08.2003 (v01.04)                     */
/*            added in v01.01                                                */
/*---------------------------------------------------------------------------*/
/*    Descriptions                                                           */
/*    ------------                                                           */
/*    Inputs    : AiUInt32  void                                                */
/*                                                                           */
/*    Outputs   : PASS/FAIL                                                  */
/*                                                                           */
/*    Description : copy ASP Prozessor boot code into local ASP RAM,         */
/*                  and starts the processor                                 */
/*                    only on cards with ASP                                     */
/*                                                                           */
/*****************************************************************************/
AiUInt32 boot_ASP_PMC(TY_MONITOR_DATA_STRUCT *GLOBAL_VAR)
{
    AiUInt32 temp, x, y, mem_length;
    AiUInt8 *ASP_MEM;
#ifndef __monitor__
    AiUInt32 val;
#endif
#if defined(_AIM_SYSDRV) && defined(_AIM_WINDOWS)
    LARGE_INTEGER x_Start;
    x_Start.QuadPart = read_Xtimestamp();
#endif

    ASP_MEM = GLOBAL_VAR->GLOBAL_TEMP_MEM;

    /******************************************************************************************/

    GLOBAL_VAR->BOOT_CONTROL_Reg.Reg.ASP_boot = 0; /* clear status bit*/

    /* v01.04 reset memory map*/
    GLOBAL_VAR->PMC_JTAG_KEY_Reg = JTAG_Key;
    PCI_IOReg_write(GLOBAL_VAR, PMC_JTAG_KEY_Reg_Adr, GLOBAL_VAR->PMC_JTAG_KEY_Reg);

    PCI_IOReg_write(GLOBAL_VAR, 0x02, 0x0); /* schreibzugriff auf ByteAdr 0x08, JTAG key gesichert*/

    /* reset TRSTx*/
    GLOBAL_VAR->PMC_JTAG_KEY_Reg = JTAG_Key; /*enable write access to JTAG Port*/
    PCI_IOReg_write(GLOBAL_VAR, PMC_JTAG_KEY_Reg_Adr, GLOBAL_VAR->PMC_JTAG_KEY_Reg);

    GLOBAL_VAR->PMC_JTAG_Reg.Reg.JTAG_OE = 1;
    GLOBAL_VAR->PMC_JTAG_Reg.Reg.TCK = 0;
    GLOBAL_VAR->PMC_JTAG_Reg.Reg.TDO = 0;
    GLOBAL_VAR->PMC_JTAG_Reg.Reg.TMS = 1;
    GLOBAL_VAR->PMC_JTAG_Reg.Reg.TRSTx = 0;
    PCI_IOReg_write(GLOBAL_VAR, PMC_JTAG_Reg_Adr, GLOBAL_VAR->PMC_JTAG_Reg.all);
    wait_clocks(GLOBAL_VAR, 2000);

    /* 1: Initialization, IRQ things, processor in RESET*/
    GLOBAL_VAR->PMC_IRQ_EVENT_Reg.Reg.ASP_host = 1; /* write one to clear Interrupt*/
    GLOBAL_VAR->PMC_IRQ_EVENT_Reg.Reg.host_ASP = 0; /* clear, to be sure*/
    GLOBAL_VAR->PMC_IRQ_EVENT_Reg.Reg.enable = 0; /* no PCI Interrupt, only register*/
    PCI_IOReg_write(GLOBAL_VAR, PMC_IRQ_EVENT_Reg_Adr, GLOBAL_VAR->PMC_IRQ_EVENT_Reg.all);

    GLOBAL_VAR->PMC_IRQ_MASK_Reg.Reg.mask_6 = 0; /* enable ASP to host Interrupt*/
    PCI_IOReg_write(GLOBAL_VAR, PMC_IRQ_MASK_Reg_Adr, GLOBAL_VAR->PMC_IRQ_MASK_Reg.all);

    GLOBAL_VAR->PMC_RESET_Reg.Reg.ASP_RESx = 0; /* ASP in RESET*/
    PCI_IOReg_write(GLOBAL_VAR, PMC_RESET_Reg_Adr, GLOBAL_VAR->PMC_RESET_Reg.all);

    GLOBAL_VAR->PMC_MAILBOX_Reg.all = 0; /* preset it in case mailbox is never
     * written by target to indicate
     * failure */

#ifdef ASP_BOOT_TEST
    /* Empty mailbox in case some unprocessed stuff is present that has written
     * before ASP has been reset. ASP will write this register to indicate it
     * has set the shared memory regions. */

    GLOBAL_VAR->PMC_MBOX_cnt_Reg.all = PCI_IOReg_read(GLOBAL_VAR,
    PMC_MBOX_cnt_Reg_Adr);
    if( GLOBAL_VAR->PMC_MBOX_cnt_Reg.Reg.sentByASP )
        /* clear flag */
        temp = PCI_IOReg_read(GLOBAL_VAR, PMC_MAILBOX_Reg_Adr);
#endif  /* ASP_BOOT_TEST */

    /* 2: Load S-Rec*/
    mem_length = Load_Srec(GLOBAL_VAR, 9, ASP_MEM);

#ifdef _AIM_WINDOWS
    //  ul_WriteMemToFile( "asp_sre0.mem", ASP_MEM, mem_length);
#endif

    if( mem_length == FAIL ) /* 9 = SRECORD Type ASP*/
    { /* ASP_MEM memory pointer to decoded s-rec*/

        AI_Printf("\nERROR by Loading ASP S-Record\n");

        GLOBAL_VAR->PMC_JTAG_KEY_Reg = JTAG_Key; /*enable write access to JTAG Port*/
        PCI_IOReg_write(GLOBAL_VAR, PMC_JTAG_KEY_Reg_Adr, GLOBAL_VAR->PMC_JTAG_KEY_Reg);

        GLOBAL_VAR->PMC_JTAG_Reg.Reg.JTAG_OE = 0; /*disable JTAG port*/
        PCI_IOReg_write(GLOBAL_VAR, PMC_JTAG_Reg_Adr, GLOBAL_VAR->PMC_JTAG_Reg.all);

        return (FAIL);
    }

    /* 3: copy S-Record into GLOBAL RAM (AiUInt8 to AiUInt32)*/

    AI_Printf("\n");

    y = 0;
    for( x = 0; x <= mem_length; x = x + 4 )
    {
        temp = 0;
        temp = *(ASP_MEM + x);
        temp = temp | (*(ASP_MEM + x + 1) << 8);
        temp = temp | (*(ASP_MEM + x + 2) << 16);
        temp = temp | (*(ASP_MEM + x + 3) << 24);

        PCI_ASPRam_write(GLOBAL_VAR, y, temp);
        y++;
        if( x % 100 == 0 )
        {
            MON_ONLY_PRINTF("*");
            if( x % 1000 == 0 )
                MON_ONLY_PRINTF("\r           %08x\r",x);
        }
    }
    AI_Printf("\r           %08x\r", x);

    /*      PCI_ASPRam_write(GLOBAL_VAR, (0x03001030/4),0x0);    */
    GLOBAL_VAR->PMC_JTAG_KEY_Reg = JTAG_Key; /*enable write access to JTAG Port*/
    PCI_IOReg_write(GLOBAL_VAR, PMC_JTAG_KEY_Reg_Adr, GLOBAL_VAR->PMC_JTAG_KEY_Reg);
    GLOBAL_VAR->PMC_JTAG_Reg.Reg.JTAG_OE = 0; /*disable JTAG port*/
    PCI_IOReg_write(GLOBAL_VAR, PMC_JTAG_Reg_Adr, GLOBAL_VAR->PMC_JTAG_Reg.all);

    /* 4: processor out of RESET*/
    GLOBAL_VAR->PMC_RESET_Reg.Reg.ASP_RESx = 1; /* ASP out of RESET*/
    PCI_IOReg_write(GLOBAL_VAR, PMC_RESET_Reg_Adr, GLOBAL_VAR->PMC_RESET_Reg.all);

    /* TRSTx out of reset*/
    GLOBAL_VAR->PMC_JTAG_KEY_Reg = JTAG_Key; /*enable write access to JTAG Port*/
    PCI_IOReg_write(GLOBAL_VAR, PMC_JTAG_KEY_Reg_Adr, GLOBAL_VAR->PMC_JTAG_KEY_Reg);

    GLOBAL_VAR->PMC_JTAG_Reg.Reg.TRSTx = 1;
    PCI_IOReg_write(GLOBAL_VAR, PMC_JTAG_Reg_Adr, GLOBAL_VAR->PMC_JTAG_Reg.all);

    /* 5: boot IRQ OK*/
#ifdef __monitor__
    AI_Printf("\nASP successfully loaded (no IRQ controll !!!)");
#else
    val = 0;
    x = 0;

    x = 50000;

    /* poll that the mailbox register is written by ASP to indicate shared mem
     * is up */
    do
    {
        GLOBAL_VAR->PMC_MBOX_cnt_Reg.all = PCI_IOReg_read(GLOBAL_VAR,
        PMC_MBOX_cnt_Reg_Adr);
        if( GLOBAL_VAR->PMC_MBOX_cnt_Reg.Reg.sentByASP )
        {
            GLOBAL_VAR->PMC_MAILBOX_Reg.all = PCI_IOReg_read(GLOBAL_VAR,
            PMC_MAILBOX_Reg_Adr);
            if( GLOBAL_VAR->PMC_MAILBOX_Reg.all == MBOX_ASP_STARTED_MAGIC )
                break;
        }

        wait_clocks(GLOBAL_VAR, 100); /* 100us */
    } while( --x );

    if( !x )
    {
#ifdef _SMARTLIB
#elif defined(_AIM_WINDOWS)
        unsigned char buf[70];

        sprintf(buf, "\n!!!ERROR!!! ASP is not up, timed out (%08X)\n", GLOBAL_VAR->PMC_MAILBOX_Reg.all);
        MonPrintfVar("\\SystemRoot\\System32\\Drivers\\asp_err.log", DBG_ERROR, FKT_MON, (buf ));
#endif

        ShowBuffer(GLOBAL_VAR, (0x00000000 / 4)/*SAddress*/, 32, 2); /* = type memory ASP*/
        ShowBuffer(GLOBAL_VAR, (0x00001000 / 4)/*SAddress*/, 32, 2); /* = type memory ASP*/
        ShowBuffer(GLOBAL_VAR, (0x00000000 / 4)/*SAddress*/, 32, 2); /* = type memory ASP Local (begin of code)*/

        return (FAIL);
    }

    ShowBuffer(GLOBAL_VAR, (0x00000000 / 4)/*SAddress*/, 32, 2); /* = type memory ASP*/
    ShowBuffer(GLOBAL_VAR, (0x00001000 / 4)/*SAddress*/, 32, 2); /* = type memory ASP*/
#endif

    /*reset interrupt controll*/
    GLOBAL_VAR->PMC_IRQ_EVENT_Reg.Reg.ASP_host = 1; /*write one to clear Interrupt*/
    GLOBAL_VAR->PMC_IRQ_EVENT_Reg.Reg.enable = 0; /*no PCI Interrupt, only register*/
    PCI_IOReg_write(GLOBAL_VAR, PMC_IRQ_EVENT_Reg_Adr, GLOBAL_VAR->PMC_IRQ_EVENT_Reg.all);

    GLOBAL_VAR->PMC_IRQ_MASK_Reg.Reg.mask_6 = 1; /*disable BIU 1 to host Interrupt*/
    PCI_IOReg_write(GLOBAL_VAR, PMC_IRQ_MASK_Reg_Adr, GLOBAL_VAR->PMC_IRQ_MASK_Reg.all);

    GLOBAL_VAR->BOOT_CONTROL_Reg.Reg.ASP_boot = 1; /*BIP 1 boot OK*/

    GLOBAL_VAR->PMC_JTAG_KEY_Reg = JTAG_Key; /*nable write access to JTAG Port*/
    PCI_IOReg_write(GLOBAL_VAR, PMC_JTAG_KEY_Reg_Adr, GLOBAL_VAR->PMC_JTAG_KEY_Reg);

    GLOBAL_VAR->PMC_JTAG_Reg.Reg.JTAG_OE = 0; /*isable JTAG port*/
    PCI_IOReg_write(GLOBAL_VAR, PMC_JTAG_Reg_Adr, GLOBAL_VAR->PMC_JTAG_Reg.all);
#if defined(_AIM_SYSDRV) && defined(_AIM_WINDOWS)
    MonPrint ( DBG_INIT, FKT_MON, ("TIME: boot_ASP_PMC      %I64u\n", (read_Xtimestamp() - x_Start.QuadPart)));
#endif

    return (PASS);
}

/*****************************************************************************/
/*                                                                           */
/*   Module : mon_prog.c               Submodule : start_BIP1_PMC            */
/*   Author : Lamp Matthias            Project   : PMC-1553-MON              */
/*---------------------------------------------------------------------------*/
/*   Create : 13/11/2001   Update :    29.09.04 NVR Board type to global ram */
/*---------------------------------------------------------------------------*/
/*    Descriptions                                                           */
/*    ------------                                                           */
/*    Inputs    : void                                                       */
/*                                                                           */
/*    Outputs   : AiUInt32:  PASS or FAIL                                      */
/*                                                                           */
/*    Description :  starts the BIP 1                                        */
/*                                                                           */
/*****************************************************************************/
AiUInt32 start_BIP1_PMC(TY_MONITOR_DATA_STRUCT *GLOBAL_VAR)
{
    /*OK, BIP 1 has successfully booted, now start it*/
    AiUInt32 x;
#if defined(_AIM_SYSDRV) && defined(_AIM_WINDOWS)
    LARGE_INTEGER x_Start;
    LARGE_INTEGER x_Temp;
    x_Start.QuadPart = read_Xtimestamp();
#endif

    /* v01.09 write NOVRAM word BORD TYPE to memlocation 0x3C*/
    PCI_Mem_write(GLOBAL_VAR, 0x3c >> 2, GLOBAL_VAR->NVR_BOARD_TYPE.all);

    /*v01.14 sollte eigentlich schon bei v01.13 drinnen gewesen sein*/
    PCI_Mem_write(GLOBAL_VAR, 0, GLOBAL_VAR->NVR_CPUCLK.Reg.CPU1_CLK);

    GLOBAL_VAR->PMC_IRQ_EVENT_Reg.all = PCI_IOReg_read(GLOBAL_VAR,
    PMC_IRQ_EVENT_Reg_Adr);
    AI_Printf("\nBIU1_host _: %x", GLOBAL_VAR->PMC_IRQ_EVENT_Reg.Reg.BIU1_host);

    GLOBAL_VAR->BOOT_CONTROL_Reg.Reg.BIP_1_start = 0; /*clear status bit*/

    /*Init IRQ things*/
    GLOBAL_VAR->PMC_IRQ_EVENT_Reg.Reg.BIU1_host = 1; /*write one to clear Interrupt*/
    GLOBAL_VAR->PMC_IRQ_EVENT_Reg.Reg.enable = 0; /*no PCI Interrupt, only register*/
    PCI_IOReg_write(GLOBAL_VAR, PMC_IRQ_EVENT_Reg_Adr, GLOBAL_VAR->PMC_IRQ_EVENT_Reg.all);

    GLOBAL_VAR->PMC_IRQ_MASK_Reg.Reg.mask_0 = 0; /*enable BIU 1 to host Interrupt*/
    PCI_IOReg_write(GLOBAL_VAR, PMC_IRQ_MASK_Reg_Adr, GLOBAL_VAR->PMC_IRQ_MASK_Reg.all);

    /*1: Interrupt processor*/
    AI_Printf("\nInterrupt BIP_1");

    if( (GLOBAL_VAR->PMC_DEVICEID_Reg.Reg.device_id != dveID_PMC32_ASP) && /*device ID undefined, old AMC1553 FPGA design,*/
    (GLOBAL_VAR->PMC_DEVICEID_Reg.Reg.device_id != dveID_PMC64_ASP) && /*=> bit Order in PMC_IRQ_EVENT_Reg is    */
    (GLOBAL_VAR->PMC_DEVICEID_Reg.Reg.device_id != dveID_PMC32) && /*different.*/
    (GLOBAL_VAR->PMC_DEVICEID_Reg.Reg.device_id != dveID_PMC32_2nd) && /* v01.05 PC-Card added*/
    (GLOBAL_VAR->PMC_DEVICEID_Reg.Reg.device_id != dveID_PCCard_1553) )
        GLOBAL_VAR->PMC_IRQ_EVENT_Reg.Reg.host_BIU1_o = 1; /*interrupt BIU 1*/
    else
        GLOBAL_VAR->PMC_IRQ_EVENT_Reg.Reg.host_BIU1 = 1; /*interrupt BIU 1*/

    PCI_IOReg_write(GLOBAL_VAR, PMC_IRQ_EVENT_Reg_Adr, GLOBAL_VAR->PMC_IRQ_EVENT_Reg.all);

    /*2: wait for Interrupt*/
    MON_ONLY_PRINTF("\nWait for BIP_1 INIT IRQ ...");

    x = 0;

#if defined(_AIM_SYSDRV) && defined(_AIM_WINDOWS)
    x_Temp.QuadPart = read_Xtimestamp();
#endif

    GLOBAL_VAR->PMC_IRQ_EVENT_Reg.Reg.BIU1_host = 0;
    while( GLOBAL_VAR->PMC_IRQ_EVENT_Reg.Reg.BIU1_host == 0 ) /*BIU sends Interrupt*/
    {
        wait_clocks(GLOBAL_VAR, 1000); /*wait 100 usec*/
        GLOBAL_VAR->PMC_IRQ_EVENT_Reg.all = PCI_IOReg_read(GLOBAL_VAR,
        PMC_IRQ_EVENT_Reg_Adr);

        MON_ONLY_PRINTF("\rBIU_1 to host IRQ: %x",GLOBAL_VAR->PMC_IRQ_EVENT_Reg.Reg.BIU1_host);

        if( x >= 50000 ) /*ime-out*/
        {
            AI_Printf("\n!!!ERROR!!! BIU_1 does not response INIT OK IRQ");
            AI_Printf("\n%d", x);
            GLOBAL_VAR->PMC_IRQ_EVENT_Reg.Reg.enable = 0; /*no PCI Interrupt, only register*/
            PCI_IOReg_write(GLOBAL_VAR, PMC_IRQ_EVENT_Reg_Adr, GLOBAL_VAR->PMC_IRQ_EVENT_Reg.all);

            GLOBAL_VAR->PMC_IRQ_MASK_Reg.Reg.mask_0 = 1; /*disable BIU 1 to host Interrupt*/
            PCI_IOReg_write(GLOBAL_VAR, PMC_IRQ_MASK_Reg_Adr, GLOBAL_VAR->PMC_IRQ_MASK_Reg.all);

            if( (GLOBAL_VAR->PMC_DEVICEID_Reg.Reg.device_id != dveID_PMC32_ASP) && /* device ID undefined, old AMC1553 FPGA design,*/
            (GLOBAL_VAR->PMC_DEVICEID_Reg.Reg.device_id != dveID_PMC64_ASP) && /*=> bit Order in PMC_IRQ_EVENT_Reg is    */
            (GLOBAL_VAR->PMC_DEVICEID_Reg.Reg.device_id != dveID_PMC32) && /*different.*/
            (GLOBAL_VAR->PMC_DEVICEID_Reg.Reg.device_id != dveID_PMC32_2nd) && /* v01.05 PC-Card added*/
            (GLOBAL_VAR->PMC_DEVICEID_Reg.Reg.device_id != dveID_PCCard_1553) )
                GLOBAL_VAR->PMC_IRQ_EVENT_Reg.Reg.host_BIU1_o = 0; /*clear IRQ*/
            else
                GLOBAL_VAR->PMC_IRQ_EVENT_Reg.Reg.host_BIU1 = 0; /*clear IRQ*/

            return (FAIL);
        }
        x++;
    }
#if defined(_AIM_SYSDRV) && defined(_AIM_WINDOWS)
    MonPrint ( DBG_INIT, FKT_MON, ("TIME: start_BIP1_PMC (while)     %I64u (x=%d)\n", (read_Xtimestamp() - x_Temp.QuadPart), x));
#endif

    MON_ONLY_PRINTF("\n BIP_1 IRQ INIT OK, X_count=%x",x);

    GLOBAL_VAR->PMC_IRQ_EVENT_Reg.Reg.BIU1_host = 1; /*write one to clear Interrupt*/
    GLOBAL_VAR->PMC_IRQ_EVENT_Reg.Reg.enable = 0; /*no PCI Interrupt, only register*/
    PCI_IOReg_write(GLOBAL_VAR, PMC_IRQ_EVENT_Reg_Adr, GLOBAL_VAR->PMC_IRQ_EVENT_Reg.all);

    GLOBAL_VAR->PMC_IRQ_MASK_Reg.Reg.mask_0 = 1; /*disable BIU 1 to host Interrupt*/

    PCI_IOReg_write(GLOBAL_VAR, PMC_IRQ_MASK_Reg_Adr, GLOBAL_VAR->PMC_IRQ_MASK_Reg.all);

    if( (GLOBAL_VAR->PMC_DEVICEID_Reg.Reg.device_id != dveID_PMC32_ASP) && /*device ID undefined, old AMC1553 FPGA design,*/
    (GLOBAL_VAR->PMC_DEVICEID_Reg.Reg.device_id != dveID_PMC64_ASP) && /*=> bit Order in PMC_IRQ_EVENT_Reg is    */
    (GLOBAL_VAR->PMC_DEVICEID_Reg.Reg.device_id != dveID_PMC32) && /*different.*/
    (GLOBAL_VAR->PMC_DEVICEID_Reg.Reg.device_id != dveID_PMC32_2nd) && /* v01.05 PC-Card added*/
    (GLOBAL_VAR->PMC_DEVICEID_Reg.Reg.device_id != dveID_PCCard_1553) )

        GLOBAL_VAR->PMC_IRQ_EVENT_Reg.Reg.host_BIU1_o = 0; /*clear IRQ*/
    else
        GLOBAL_VAR->PMC_IRQ_EVENT_Reg.Reg.host_BIU1 = 0; /*clear IRQ*/

    GLOBAL_VAR->BOOT_CONTROL_Reg.Reg.BIP_1_start = 1; /*BIP 1 start OK*/

#if defined(_AIM_SYSDRV) && defined(_AIM_WINDOWS)
    MonPrint ( DBG_INIT, FKT_MON, ("TIME: start_BIP1_PMC      %I64u\n", (read_Xtimestamp() - x_Start.QuadPart)));
#endif

    return (PASS);
}

/*****************************************************************************/
/*                                                                           */
/*   Module : mon_prog.c               Submodule : start_BIP2_PMC            */
/*   Author : Lamp Matthias            Project   : PMC-1553-MON              */
/*---------------------------------------------------------------------------*/
/*   Create : 13/11/2001   Update :    29.09.04 NVR Board type to global ram */
/*---------------------------------------------------------------------------*/
/*    Descriptions                                                           */
/*    ------------                                                           */
/*    Inputs    : void                                                       */
/*                                                                           */
/*    Outputs   : AiUInt32:  PASS or FAIL                                    */
/*                                                                           */
/*    Description :  starts the BIP 2                                         */
/*                                                                           */
/*****************************************************************************/
AiUInt32 start_BIP2_PMC(TY_MONITOR_DATA_STRUCT *GLOBAL_VAR)
{
    /*OK, BIP 2 has successfully booted, not start it*/
    AiUInt32 x;
#if defined(_AIM_SYSDRV) && defined(_AIM_WINDOWS)
    LARGE_INTEGER x_Start;
    x_Start.QuadPart = read_Xtimestamp();
#endif

    /* v01.09 write NOVRAM word BORD TYPE to memlocation 0x103C, 0x1003c*/
    PCI_Mem_write(GLOBAL_VAR, 0x103c >> 2, GLOBAL_VAR->NVR_BOARD_TYPE.all);
    PCI_Mem_write(GLOBAL_VAR, 0x1003c >> 2, GLOBAL_VAR->NVR_BOARD_TYPE.all);

    /*v01.14 sollte eigentlich schon bei v01.13 drinnen gewesen sein*/
    PCI_Mem_write(GLOBAL_VAR, 0x1000 >> 2, GLOBAL_VAR->NVR_CPUCLK.Reg.CPU2_CLK);
    PCI_Mem_write(GLOBAL_VAR, 0x10000 >> 2, GLOBAL_VAR->NVR_CPUCLK.Reg.CPU2_CLK);

    GLOBAL_VAR->BOOT_CONTROL_Reg.Reg.BIP_2_start = 1; /* BIP 2 start OK*/

    /* Init IRQ things*/
    GLOBAL_VAR->PMC_IRQ_EVENT_Reg.Reg.BIU2_host = 1; /* write one to clear Interrupt*/
    GLOBAL_VAR->PMC_IRQ_EVENT_Reg.Reg.enable = 0; /* no PCI Interrupt, only register*/
    PCI_IOReg_write(GLOBAL_VAR, PMC_IRQ_EVENT_Reg_Adr, GLOBAL_VAR->PMC_IRQ_EVENT_Reg.all);

    GLOBAL_VAR->PMC_IRQ_MASK_Reg.Reg.mask_1 = 0; /* enable BIU 2 to host Interrupt*/
    PCI_IOReg_write(GLOBAL_VAR, PMC_IRQ_MASK_Reg_Adr, GLOBAL_VAR->PMC_IRQ_MASK_Reg.all);

    /* 1: Interrupt processor*/
    MON_ONLY_PRINTF("\nInterrupt BIP_2");

    if( (GLOBAL_VAR->PMC_DEVICEID_Reg.Reg.device_id != dveID_PMC32_ASP) && /* device ID undefined, old AMC1553 FPGA design,*/
    (GLOBAL_VAR->PMC_DEVICEID_Reg.Reg.device_id != dveID_PMC64_ASP) && /* => bit Order in PMC_IRQ_EVENT_Reg is    */
    (GLOBAL_VAR->PMC_DEVICEID_Reg.Reg.device_id != dveID_PMC32) && /* different.*/
    (GLOBAL_VAR->PMC_DEVICEID_Reg.Reg.device_id != dveID_PMC32_2nd) && /* v01.05 PC-Card added  */
    (GLOBAL_VAR->PMC_DEVICEID_Reg.Reg.device_id != dveID_PCCard_1553) )

        GLOBAL_VAR->PMC_IRQ_EVENT_Reg.Reg.host_BIU2_o = 1; /* interrupt BIU2*/
    else
        GLOBAL_VAR->PMC_IRQ_EVENT_Reg.Reg.host_BIU2 = 1; /* interrupt BIU 2*/

    PCI_IOReg_write(GLOBAL_VAR, PMC_IRQ_EVENT_Reg_Adr, GLOBAL_VAR->PMC_IRQ_EVENT_Reg.all);

    /* 2: wait for Interrupt*/
    MON_ONLY_PRINTF("\nWait for BIP_2 INIT IRQ ...");

    x = 0;
    GLOBAL_VAR->PMC_IRQ_EVENT_Reg.Reg.BIU2_host = 0;

    while( GLOBAL_VAR->PMC_IRQ_EVENT_Reg.Reg.BIU2_host == 0 ) /* BIU sends Interrupt*/
    {
        wait_clocks(GLOBAL_VAR, 1000); /* wait 20 usec*/
        GLOBAL_VAR->PMC_IRQ_EVENT_Reg.all = PCI_IOReg_read(GLOBAL_VAR,
        PMC_IRQ_EVENT_Reg_Adr);

        MON_ONLY_PRINTF("\rBIU_2 to host IRQ: %x",GLOBAL_VAR->PMC_IRQ_EVENT_Reg.Reg.BIU2_host);

        if( x >= 50000 ) /*time-out  (5 sec)*/
        {
            AI_Printf("\n!!!ERROR!!! BIU_2 does not response INIT OK IRQ");
            AI_Printf("\n%d", x);
            GLOBAL_VAR->PMC_IRQ_EVENT_Reg.Reg.enable = 0; /* no PCI Interrupt, only register*/
            PCI_IOReg_write(GLOBAL_VAR, PMC_IRQ_EVENT_Reg_Adr, GLOBAL_VAR->PMC_IRQ_EVENT_Reg.all);

            GLOBAL_VAR->PMC_IRQ_MASK_Reg.Reg.mask_1 = 1; /* disable BIU 2 to host Interrupt*/
            PCI_IOReg_write(GLOBAL_VAR, PMC_IRQ_MASK_Reg_Adr, GLOBAL_VAR->PMC_IRQ_MASK_Reg.all);

            if( (GLOBAL_VAR->PMC_DEVICEID_Reg.Reg.device_id != dveID_PMC32_ASP) && /* device ID undefined, old AMC1553 FPGA design,*/
            (GLOBAL_VAR->PMC_DEVICEID_Reg.Reg.device_id != dveID_PMC64_ASP) && /* => bit Order in PMC_IRQ_EVENT_Reg is    */
            (GLOBAL_VAR->PMC_DEVICEID_Reg.Reg.device_id != dveID_PMC32) && /* different.*/
            (GLOBAL_VAR->PMC_DEVICEID_Reg.Reg.device_id != dveID_PMC32_2nd) && /* v01.05 PC-Card added  */
            (GLOBAL_VAR->PMC_DEVICEID_Reg.Reg.device_id != dveID_PCCard_1553) )
                GLOBAL_VAR->PMC_IRQ_EVENT_Reg.Reg.host_BIU2_o = 0; /* clear IRQ*/
            else
                GLOBAL_VAR->PMC_IRQ_EVENT_Reg.Reg.host_BIU2 = 0; /* clear IRQ*/
            return (FAIL);
        }
        x++;
    }MON_ONLY_PRINTF("\n BIP_2 IRQ INIT OK");

    GLOBAL_VAR->PMC_IRQ_EVENT_Reg.Reg.BIU1_host = 1; /* write one to clear Interrupt*/
    GLOBAL_VAR->PMC_IRQ_EVENT_Reg.Reg.enable = 0; /* no PCI Interrupt, only register*/
    PCI_IOReg_write(GLOBAL_VAR, PMC_IRQ_EVENT_Reg_Adr, GLOBAL_VAR->PMC_IRQ_EVENT_Reg.all);

    GLOBAL_VAR->PMC_IRQ_MASK_Reg.Reg.mask_1 = 1; /* disable BIU 2 to host Interrupt*/
    PCI_IOReg_write(GLOBAL_VAR, PMC_IRQ_MASK_Reg_Adr, GLOBAL_VAR->PMC_IRQ_MASK_Reg.all);

    if( (GLOBAL_VAR->PMC_DEVICEID_Reg.Reg.device_id != dveID_PMC32_ASP) && /* device ID undefined, old AMC1553 FPGA design,*/
    (GLOBAL_VAR->PMC_DEVICEID_Reg.Reg.device_id != dveID_PMC64_ASP) && /* => bit Order in PMC_IRQ_EVENT_Reg is    */
    (GLOBAL_VAR->PMC_DEVICEID_Reg.Reg.device_id != dveID_PMC32) && /* different.*/
    (GLOBAL_VAR->PMC_DEVICEID_Reg.Reg.device_id != dveID_PMC32_2nd) && (GLOBAL_VAR->PMC_DEVICEID_Reg.Reg.device_id != dveID_PCCard_1553) ) /*der dveID_PCCard_1553 Zweig war da nicht*/
        GLOBAL_VAR->PMC_IRQ_EVENT_Reg.Reg.host_BIU2_o = 0; /* clear IRQ*/
    else
        GLOBAL_VAR->PMC_IRQ_EVENT_Reg.Reg.host_BIU2 = 0; /* clear IRQ*/

    GLOBAL_VAR->BOOT_CONTROL_Reg.Reg.BIP_2_start = 1; /* BIP 2 start OK*/
#if defined(_AIM_SYSDRV) && defined(_AIM_WINDOWS)
    MonPrint ( DBG_INIT, FKT_MON, ("TIME: start_BIP2_PMC      %I64u\n", (read_Xtimestamp() - x_Start.QuadPart)));
#endif

    return (PASS);
}

/*****************************************************************************/
/*                                                                           */
/*   Module : mon_prog.c               Submodule : program_tcp_flash         */
/*   Author : Lamp Matthias            Project   : PMC-1553-MON              */
/*---------------------------------------------------------------------------*/
/*   Create : 17/10/2001   Update :   28.10.03                               */
/*---------------------------------------------------------------------------*/
/*    Descriptions                                                           */
/*    ------------                                                           */
/*    Inputs    : void                                                       */
/*                                                                           */
/*    Outputs   : AiUInt32:  PASS or FAIL                                    */
/*                                                                           */
/*    Description :  Programmes TCP Flash (new TCP Firmware)                 */
/*                   device type AVR Mega8535 added (v01.04)                 */
/*                                                                           */
/*****************************************************************************/
#if !defined(_AIM_SYSDRV) && !defined(_AIM_VME)
AiUInt32 program_tcp_flash(TY_MONITOR_DATA_STRUCT *GLOBAL_VAR)
{
    AiUInt32 x, y, z, flag, rtry, flag1;
    AiUInt32 mem_length, data, tmp, signature = 0;
    AiUInt16 Data; /*    16Bit Data*/
    AiUInt8 *TCP_FLASH_MEM;

#if !defined(_SMARTLIB) && defined(_AIM_WINDOWS)
    NET2282_TY *net2282_var;
    net2282_var = (NET2282_TY*) malloc(0x80);

    if( GLOBAL_VAR->STATUS.argv2 > '5' )
    {
        NET2282_init(GLOBAL_VAR, net2282_var);
    }
#endif
    TCP_FLASH_MEM = GLOBAL_VAR->GLOBAL_TEMP_MEM;

    AI_Printf("\nProgramming of TCP FLASH Memory");
    AI_Printf("\n");

    /*************************************************/
    /* enable SPI communication,*/
    /* read device ID, ...*/
    signature = spi_prog_enable(GLOBAL_VAR);
    if( signature == FAIL )
    {
#if !defined(_SMARTLIB) && defined(_AIM_WINDOWS)
        free(net2282_var);
#endif
        return (FAIL);
    }
    /*************************************************/

    /* read LOCK and FUSE BITS*/
    if( (signature & 0xffff) == SPI_AVRMEGA8535 ) /*AVR MEGA8535 v01.04*/
    {
        /* during update, disable outputs */
        if( GLOBAL_VAR->STATUS_FLAGS.Reg.BATCH_MODE == 0 ) /* normal monitor mode */
        {
            AI_Printf("\nAVR MEGA 8535");
        }
        else
            AI_Printf("AVR MEGA 8535\n", 1, 1);

        write_spi(GLOBAL_VAR, SPI_MEGA8535_RD_LOCK_BITS); /* read lock bits*/
        write_spi(GLOBAL_VAR, 0x00); /* 0*/
        write_spi(GLOBAL_VAR, 0x00); /* x*/
        tmp = read_spi(GLOBAL_VAR); /* lock bits*/

        wait_clocks(GLOBAL_VAR, 5000);

        /* during update, disable outputs */
        if( GLOBAL_VAR->STATUS_FLAGS.Reg.BATCH_MODE == 0 ) /* normal monitor mode */
        {
            MON_ONLY_PRINTF("\nLock Bits (0=programmed, 1=unprogrammed)");MON_ONLY_PRINTF("\nBLB12:    %x, BLB11:    %x, BLB02:    %x, BLB01:    %x,\nLB2:      %x, LB1:      %x",
                        (tmp & 0x20)>>5,(tmp & 0x10)>>4,(tmp & 0x8)>>3,(tmp & 0x4)>>2,(tmp & 0x2)>>1,(tmp & 0x1) );
        }

        write_spi(GLOBAL_VAR, SPI_MEGA8535_RD_FUSE_BITS_B1); /* read fuse bits*/
        write_spi(GLOBAL_VAR, SPI_MEGA8535_RD_FUSE_BITS_B2);
        write_spi(GLOBAL_VAR, 0x00); /* x*/
        tmp = read_spi(GLOBAL_VAR); /* fuse bits*/

        wait_clocks(GLOBAL_VAR, 5000);

        /* during update, disable outputs */
        if( GLOBAL_VAR->STATUS_FLAGS.Reg.BATCH_MODE == 0 ) /* normal monitor mode */
        {
            MON_ONLY_PRINTF("\nFUSE Bits (0=programmed, 1=unprogrammed)");MON_ONLY_PRINTF("\nBODLEVEL: %x, BODEN:    %x, SUT1:     %x, SUT0:     %x,\nCKSEL3:   %x, CKSEL2:   %x, CKSEL1:   %x, CKSEL0:   %x",
                        (tmp & 0x20)>>7,(tmp & 0x20)>>6,(tmp & 0x20)>>5,(tmp & 0x10)>>4,(tmp & 0x8)>>3,(tmp & 0x4)>>2,(tmp & 0x2)>>1,(tmp & 0x1) );
        }

        write_spi(GLOBAL_VAR, SPI_MEGA8535_RD_FUSE_BITS_H_B1); /* read fuse high bits*/
        write_spi(GLOBAL_VAR, SPI_MEGA8535_RD_FUSE_BITS_H_B2);
        write_spi(GLOBAL_VAR, 0x00); /* x*/
        tmp = read_spi(GLOBAL_VAR); /* fuse high bits*/

        wait_clocks(GLOBAL_VAR, 5000);
        /* during update, disable outputs */
        if( GLOBAL_VAR->STATUS_FLAGS.Reg.BATCH_MODE == 0 ) /* normal monitor mode */
        {
            MON_ONLY_PRINTF("\nFUSE HIGH Bits (0=programmed, 1=unprogrammed)");
            MON_ONLY_PRINTF("\nS8535C:   %x, WDTON:    %x, SPIEN:    %x, CKOPT:    %x,\nEESAVE:   %x, BOOTSZ1:  %x, BOOTSZ0:  %x, BOOTRST:  %x", \
                        (tmp & 0x20)>>7,(tmp & 0x20)>>6,(tmp & 0x20)>>5,(tmp & 0x10)>>4,(tmp & 0x8)>>3,(tmp & 0x4)>>2,(tmp & 0x2)>>1,(tmp & 0x1) );
        }
    } /*if ((signature & 0xffff) == SPI_AVRMEGA8535) */
    else /*AVR 90S8535*/
    {
        /* during update, disable outputs */
        if( GLOBAL_VAR->STATUS_FLAGS.Reg.BATCH_MODE == 0 ) /* normal monitor mode */
        {
            AI_Printf("\nAVR 90S8535");
        }

        write_spi(GLOBAL_VAR, SPI_RD_LOCKFUSE_BITS); /* read lock / fuse bits*/
        write_spi(GLOBAL_VAR, 0x00); /* x*/
        write_spi(GLOBAL_VAR, 0x00); /* x*/

        tmp = read_spi(GLOBAL_VAR); /* Part Number*/
        wait_clocks(GLOBAL_VAR, 5000);

        /* during update, disable outputs */
        if( GLOBAL_VAR->STATUS_FLAGS.Reg.BATCH_MODE == 0 ) /* normal monitor mode */
        {
            MON_ONLY_PRINTF("\nLock Bit 1: %x  Lock Bit 2: %x  Fuse Bit SPIEN: %x   Fuse Bit FSTRT: %x  ", \
                        (tmp & 0x80)>>7,(tmp & 0x40)>>6,(tmp & 0x20)>>5,(tmp & 0x1));
        }
    }

    /**************************************************/
    /* Save NovRAM Contents,*/
    /* in variables*/
    /* during update, disable outputs */
    if( GLOBAL_VAR->STATUS_FLAGS.Reg.BATCH_MODE == 0 ) /* normal monitor mode */
        AI_Printf("\nSaving NOVRam contents");
    tmp = read_tcp_novram(GLOBAL_VAR);                /* MMA NET2282 */
    if( tmp == FAIL )
    {
        AI_Printf("\n!!! ERROR !!! save NOVRAM values failed");
#if !defined(_SMARTLIB) && defined(_AIM_WINDOWS)
        free(net2282_var);
#endif
        return (FAIL);
    }
    /* during update, disable outputs */
    if( GLOBAL_VAR->STATUS_FLAGS.Reg.BATCH_MODE == 0 ) /* normal monitor mode */
    {
        AI_Printf("\nNOVRam values read");
    }

    /* open S-Record, and convert to hex (*memory)    */

#if !defined(_SMARTLIB) /* in case of SmartLib this is already done in SmrtLib.c */
    mem_length = Load_Srec(GLOBAL_VAR, 6, TCP_FLASH_MEM);
    if( mem_length == FAIL ) /* 6 = SRECORD Type: TCP FLASH*/
    {
        AI_Printf("\nERROR by Loading TCP FLASH S-Record\n");
#if !defined(_SMARTLIB) && defined(_AIM_WINDOWS)
        free(net2282_var);
#endif
        return (FAIL);
    }
#else
    mem_length = GLOBAL_VAR->ul_MemLengthTcpUpdate;
#endif

    /* Load NOVRAM Contents to have mirror in FLASH*/
    mem_length = Load_Srec(GLOBAL_VAR,0x17,TCP_FLASH_MEM +0x01e00);
    if(mem_length == FAIL)
    {
        MON_ONLY_PRINTF("ERROR loaded file ");
#if !defined(_SMARTLIB) && defined(_AIM_WINDOWS)
        free(net2282_var);
#endif
        return(FAIL);
    }
    else
    {
        mem_length += 0x01e00;
    }

    /* program flash    */
    flag = FAIL;
    flag1 = PASS;
    rtry = 0;

    AI_Printf("Shutting down TCP Programming may take several minutes");

    /* Write and Verify, retry upto 5 times, if result is invald board is not operational */
    while( (rtry < 5) && (flag == FAIL) )
    {
        if( rtry != 0 )
        {
            AI_Printf("\nProgramming Attempt failed! Starting Retry %x of 4.", rtry);
        }

        /* erase the FLASH and NOVRAM*/
        /* during update, disable outputs */
        if( GLOBAL_VAR->STATUS_FLAGS.Reg.BATCH_MODE == 0 ) /* normal monitor mode */
        {
            AI_Printf("\nErase all data on TCP.");
        }
        erase_tcp(GLOBAL_VAR);

        AI_Printf("\nProgramming Flash.\n");

        /* now use the loaded srec*/
        if( (signature & 0xffff) == SPI_AVRMEGA8535 ) /*AVR MEGA8535 v01.04*/
        {
            if( GLOBAL_VAR->STATUS.argv2 < '5' )
            {
                z = 0;
                for( y = 0; y < 128; y++ ) /* flash is organized as 128 pages*/
                {
                    if( y % 8 == 0 )
                    {
                        AI_Printf("\r               program byte no: %04x of %04x       \r", z, mem_length);
                    }
                    else
                    {
                        AI_Printf(".");
                    }
                    for( x = 0; x < 32; x++ ) /* 32 words per page*/
                    {
                        if( z > mem_length )
                            break;
                        else
                            tmp = *(TCP_FLASH_MEM + z);

                        z++;
                        write_spi(GLOBAL_VAR, SPI_MEGA8535_WR_PROG_MEM_LOWB);
                        write_spi(GLOBAL_VAR, 0x00);
                        write_spi(GLOBAL_VAR, (AiUInt8) x);
                        write_spi(GLOBAL_VAR, (AiUInt8) tmp);

                        wait_clocks(GLOBAL_VAR, (1 << rtry) * 100);

                        if( z > mem_length )
                            break;
                        else
                            tmp = *(TCP_FLASH_MEM + z);

                        z++;
                        write_spi(GLOBAL_VAR, SPI_MEGA8535_WR_PROG_MEM_HIGHB);
                        write_spi(GLOBAL_VAR, 0x00);
                        write_spi(GLOBAL_VAR, (AiUInt8) x);
                        write_spi(GLOBAL_VAR, (AiUInt8) tmp);

                        wait_clocks(GLOBAL_VAR, (1 << rtry) * 100);
                    } /*for x*/

                    /* and now program the page*/
                    write_spi(GLOBAL_VAR, SPI_MEGA8535_WR_MEM_PAGE);
                    write_spi(GLOBAL_VAR, (AiUInt8) ((y & 0x78) >> 3)); /* bit 6,5,4,3 */
                    write_spi(GLOBAL_VAR, (AiUInt8) ((y & 0x07) << 5)); /* bit 2,1,0*/
                    write_spi(GLOBAL_VAR, 0x00);

                    wait_clocks(GLOBAL_VAR, (1 << rtry) * 5000);

                    if( z > mem_length )
                        break;
                } /* for y*/
            } /* if(GLOBAL_VAR->STATUS.argv2 < '5') */
            else /* USB case */
            {
#if !defined(_SMARTLIB) && defined(_AIM_WINDOWS)
                NET2282_prog_TCP_flash(GLOBAL_VAR, 0, 0x2000, TCP_FLASH_MEM, net2282_var);
#else
                AI_Printf("ERROR UDP Update not supported with Smartlib! \n");
#endif
            }

            MON_ONLY_PRINTF("\n");
            z = 0;
            flag = PASS;
            flag1 = PASS;
            if( GLOBAL_VAR->STATUS.argv2 < '5' ) /*/////////////////////////////////////////////////// da stimmt was nicht */
            {
                for( x = 0; x <= (mem_length / 2); x++ )
                {
                    data = (*(TCP_FLASH_MEM + z + 1) << 8);
                    data = (data | (*(TCP_FLASH_MEM + z)));
                    z = z + 2;
                    tmp = read_spi_flash(GLOBAL_VAR, x);
                    if( tmp != data )
                    {
                        AI_Printf("\nverify error, adr.:%04x flash:%04x srec:%04x", x, tmp, data);
                        flag = FAIL;
                        flag1 = FAIL;
                        break;
                    }
                    if( x % (8 * 32) == 0 )
                    {
                        AI_Printf("\r                verify byte no: %04x of %04x       \r", x * 2, mem_length);
                    }
                    else if( x % 32 == 0 )
                    {
                        AI_Printf(".");
                    }
                } /*for x*/
            }

            if( flag1 == PASS )
            {
                AI_Printf("\nverify passed");
                flag = PASS;
            }
        } /*if signature*/
        else
        {
            x = 0;
            AI_Printf("mem_length = %08X\n", mem_length);
            for( y = 0; y < mem_length; y++ )
            {
                if( y % 8 == 0 )
                    AI_Printf("\r               byte no: %x of %x       \r", y, mem_length);

                Data = (*(TCP_FLASH_MEM + x + 1) << 8);
                Data = (Data | (*(TCP_FLASH_MEM + x)));

                /* MSB---data---LSB*/
                /* one flash location*/
                flag1 = write_spi_flash(GLOBAL_VAR, x >> 1, Data);

                if( flag1 == FAIL )
                    flag = FAIL;

                if( flag1 == PASS )
                {
                    AI_Printf(".");
                }
                else
                {
                    AI_Printf("e\n");
                }

                if( x >= 8 * 1024 + 1 ) /*OVERRUN '\0' not found     */
                {
                    AI_Printf("ERROR did not found '\\0' in module program_tcp_flash");
                    flag1 = FAIL;
                    break;
                }
                x = x + 2;
            }/*for()*/
        } /*else signatureo*/

        flag = flag1;
        rtry++;
    } /* while*/

    if( flag == PASS )
    {
        /* during update, disable outputs */
        if( GLOBAL_VAR->STATUS_FLAGS.Reg.BATCH_MODE == 0 ) /* normal monitor mode */
        {
            AI_Printf("\nProgramming Done");
        }
    }
    else
    {
        AI_Printf("\nProgramming Failed");
#if !defined(_SMARTLIB) && defined(_AIM_WINDOWS)
        free(net2282_var);
#endif
        return (FAIL);
    }

    /* Restore NovRAM Contents */
    /* during update, disable outputs */
    if( GLOBAL_VAR->STATUS_FLAGS.Reg.BATCH_MODE == 0 ) /* normal monitor mode */
    {
        AI_Printf("\n\nRestoring NOVRam contents");
    }

    if( program_tcp_novram(GLOBAL_VAR) == FAIL )
    {
        AI_Printf("\n!!! ERROR !!! unable to restore NOVRAM");
#if !defined(_SMARTLIB) && defined(_AIM_WINDOWS)
        free(net2282_var);
#endif
        return (FAIL);
    }

    /* ML add write fuse bits, v02.24 */

    prog_TCP_FL_bits(GLOBAL_VAR, signature);
    /* ml end */

    AI_Printf("\nTCP-Flash Programming finished, start TCP ...");
    /* start ATMEl, and wait for ready */
    tmp = start_ATMEL(GLOBAL_VAR);
    if( tmp == PASS )
    {
        AI_Printf("\nTCP successfully started");
    }
    else
    {
        AI_Printf("\nERROR, TCP does not response ready");
    }

    /* SPI PROG disable */
    GLOBAL_VAR->PMC_TCP_PROG_Reg.all = 0;
    GLOBAL_VAR->PMC_TCP_PROG_Reg.Reg.SPI_OE = 0;
    GLOBAL_VAR->PMC_TCP_PROG_Reg.Reg.SPI_MOSI = 0;
    GLOBAL_VAR->PMC_TCP_PROG_Reg.Reg.SPI_CLK = 0;
    PCI_IOReg_write(GLOBAL_VAR, PMC_TCP_PROG_Reg_Adr, GLOBAL_VAR->PMC_TCP_PROG_Reg.all);
    wait_clocks(GLOBAL_VAR, 10000);

    /*doofe stelle, da eigentlich das meiste schon gemacht*/

/*  dofe stelle eliminiert in dem vor dem TCP programmieren schon das NOVRAM in den Speicher geladen wird.
 *  --> Dann ist Mirror schon vorhanden
    if( GLOBAL_VAR->STATUS.argv2 < '5' )
    {  nur wenn kein USB-Board, ansonsten schon gemacht!!!
        create FLASH MIRROR

        AI_Printf("\nNo NOVRAM FLASH mirror exists, create it ... ");

        if( update_NOVRAM_FLASH(GLOBAL_VAR) == FAIL )
        {
            AI_Printf("\nERROR creating NOVRAM FLASH mirror");
#if !defined(_SMARTLIB) && defined(_AIM_WINDOWS)
            free(net2282_var);
#endif
            return (FAIL);
        }
    }
*/

#if !defined(_SMARTLIB) && defined(_AIM_WINDOWS)
    free(net2282_var);
#endif
    return (tmp);
}
#endif

/*****************************************************************************/
/*                                                                           */
/*   Module : mon_prog.c               Submodule : start_ASP_PMC             */
/*   Author : Lamp Matthias            Project   : PMC-1553-MON              */
/*---------------------------------------------------------------------------*/
/*   Create : 24/07/2002   Update :   .  .                                   */
/*---------------------------------------------------------------------------*/
/*    Descriptions                                                           */
/*    ------------                                                           */
/*    Inputs    : void                                                       */
/*                                                                           */
/*    Outputs   : AiUInt32:  PASS or FAIL                                    */
/*                                                                           */
/*    Description :  starts the ASP (when available)                         */
/*                     added in v01.01                                         */
/*                                                                           */
/*****************************************************************************/
AiUInt32 start_ASP_PMC(TY_MONITOR_DATA_STRUCT *GLOBAL_VAR)
{
    MON_ONLY_PRINTF("\n ASP start without any handshake");

    return (PASS);
}

/*****************************************************************************/
/*                                                                           */
/*   Module : mon_prog.c               Submodule : read_tcp_novram           */
/*   Author : Lamp Matthias            Project   : PMC-1553-MON              */
/*---------------------------------------------------------------------------*/
/*   Create : 17.04.2001   Update :   .  .                                   */
/*---------------------------------------------------------------------------*/
/*    Descriptions                                                           */
/*    ------------                                                           */
/*    Inputs    : -                                                          */
/*                                                                           */
/*    Outputs   : AiUInt32 PASS / FAIL                                         */
/*                                                                           */
/*    Description :  Reads TCP NOVRAM data, and writes it to SREC            */
/*                                                                           */
/*****************************************************************************/
AiUInt32 read_tcp_novram(TY_MONITOR_DATA_STRUCT *GLOBAL_VAR)
{
    AiUInt32 i, size = 512;
    AiUInt8 *memory;

#ifndef _SMARTLIB
    memory = GLOBAL_VAR->GLOBAL_TEMP_MEM;
#else
    memory = GLOBAL_VAR->GLOBAL_TEMP_MEM + GLOBAL_VAR->GLOBAL_TEMP_MEM_SIZE/2;
#endif

    /* during update, disable outputs */
    if( GLOBAL_VAR->STATUS_FLAGS.Reg.BATCH_MODE == 0 ) /* normal monitor mode */
        MON_ONLY_PRINTF("\nReading TCP-NOVRam");
    else
        AI_Printf("\nReading TCP-NOVRam");

    for( i = 0; i < size; i++ )
    {
        *(memory + i) = read_spi_novram(GLOBAL_VAR, i); /* write NOVRAM data to *memory*/
        if( i % 32 == 0 )
        {
            AI_Printf("*"); /* a dot each 32th readed NOVRAM word */
        }
    }

    /* during update, disable outputs */
    if( GLOBAL_VAR->STATUS_FLAGS.Reg.BATCH_MODE == 0 ) /* normal monitor mode */
        AI_Printf("\nWrite NOVRAM values to S3-Record");

    i = 0;

#ifdef __monitor__
    i = print_s3recs(GLOBAL_VAR,7, memory,size); /* 7= tcp novram*/
#else
    i = PASS;
#endif

    if( i == PASS )
        return (PASS);
    else
        return (FAIL);
}

#ifdef __monitor__
/*****************************************************************************/
/*                                                                           */
/*   Module : ini_prog.c               Submodule : program_tcp_novram        */
/*   Author : Lamp Matthias            Project   : PMC-1553-MON              */
/*---------------------------------------------------------------------------*/
/*   Create : 14/05/2001   Update :   .  .                                   */
/*---------------------------------------------------------------------------*/
/*    Descriptions                                                           */
/*    ------------                                                           */
/*    Inputs    : GLOBAL_VAR, NOVRAM values found at mem+offset              */
/*                                                                           */
/*    Outputs   : AiUInt32:  PASS or FAIL                                      */
/*                                                                           */
/*    Description :  Programmes TCP NOVRam                                   */
/*                                                                           */
/*****************************************************************************/
AiUInt32 program_tcp_novram( TY_MONITOR_DATA_STRUCT *GLOBAL_VAR)
{
    AiUInt32 i, flag, rtry, flag1;
    AiUInt8 *memory;

    NET2282_TY *net2282_var;
    net2282_var = (NET2282_TY*)malloc(0x80);

    if(GLOBAL_VAR->STATUS.argv2 >= '5')
    {
        NET2282_init(GLOBAL_VAR, net2282_var);
    }

#ifndef _SMARTLIB
    memory = GLOBAL_VAR->GLOBAL_TEMP_MEM;
#else
    memory = GLOBAL_VAR->GLOBAL_TEMP_MEM + GLOBAL_VAR->GLOBAL_TEMP_MEM_SIZE/2;
#endif

#ifdef __monitor__
    /* during update, disable outputs */
    if (GLOBAL_VAR->STATUS_FLAGS.Reg.BATCH_MODE==0) /* normal monitor mode */
    AI_Printf("\nProgramming of TCP None Volatile Memory (NOVRam)");
#elif defined _SMARTLIB
    v_SLLogFile("Programming of TCP None Volatile Memory (NOVRam)\n", 1, 1);
#endif

#ifndef _SMARTLIB
    /* open S-Record, and convert to hex (*memory)*/
    if(Load_Srec(GLOBAL_VAR,0x17,memory)==FAIL)
    {
        MON_ONLY_PRINTF("ERROR loaded file ");
        free(net2282_var);
        return(FAIL);
    }
#endif

    flag = FAIL;
    flag1 = PASS;
    rtry = 0;

    /* Write and Verify, retry upto 5 times, if result is invald board is not operational */
    while ( (rtry < 5) && (flag==FAIL) )
    {
        if (rtry!=0)
        {
            AI_Printf("\nProgramming Attempt failed! Starting Retry.");
        }

        AI_Printf("\nProgramming NOVRam\n");
        if(GLOBAL_VAR->STATUS.argv2 < '5')
        {        // MMA NET2282
            for (i=0; i < 512;i++ )
            {
                flag1 = write_spi_novram(GLOBAL_VAR, i, *(memory+i));
                if (flag1 == FAIL)
                flag = FAIL;
                if(i%17==0)
                AI_Printf("\r                 NOVRAM words: %x     \r",(i/4));

                if (flag1 == PASS)
                {
                    AI_Printf(".");
                }
                else
                {
                    AI_Printf("e");
                }
            }
        }
        else
        {
            NET2282_write_spi_novram(GLOBAL_VAR, 0x0, memory, net2282_var); // write NOVRAM values into NOVRAM
            NET2282_prog_TCP_flash(GLOBAL_VAR, 0x1e00, 0x200, memory, net2282_var);// write NOVRAM values as mirror into FLASH
            flag1=PASS;
        }

        flag = flag1;
        rtry++;
    }

#ifdef __monitor__
    /* during update, disable outputs */
    if (GLOBAL_VAR->STATUS_FLAGS.Reg.BATCH_MODE==0) /* normal monitor mode */
    {
        if (flag==PASS)
        {
            AI_Printf("\n\nProgramming Done, Restarting TCP ");
        }
        else
        {
            AI_Printf("\n\nProgramming Failed, Restarting TCP ");
        }
    }
#elif defined _SMARTLIB
    if (flag == PASS)
    v_SLLogFile("\nProgramming Done, Restarting TCP\n", 1, 1);
    else
    v_SLLogFile("\nProgramming Failed, Restarting TCP\n", 1, 1);
#endif
    free(net2282_var);

    return(flag);
}
#endif

/*****************************************************************************/
/*                                                                           */
/*   Module : ini_prog.c               Submodule : set IRIG intern time      */
/*   Author : Lamp Matthias            Project   : PMC-1553-MON              */
/*---------------------------------------------------------------------------*/
/*   Create : 07/03/2002   Update :   .  .                                   */
/*---------------------------------------------------------------------------*/
/*    Descriptions                                                           */
/*    ------------                                                           */
/*    Inputs    : GLOBAL Struct,                                             */
/*                  AiUInt32 *leap_year  (indicates if leap year)              */
/*                AiUInt32 *DayHi,                                             */
/*                  AiUInt32 *DayLo,                                             */
/*                  AiUInt32 *Hour,                                             */
/*                  AiUInt32 *Minute,                                          */
/*                  AiUInt32 *Second                                             */
/*                                                                           */
/*    Outputs   : AiUInt32:  PASS or FAIL                                    */
/*                                                                           */
/*    Description : sets the IRIG intern (ENCODER) Time to new values        */
/*                                                                           */
/*****************************************************************************/
AiUInt32 set_IRIG_InternTime(TY_MONITOR_DATA_STRUCT *GLOBAL_VAR,
            AiUInt32 *leap_year,
            AiUInt32 *DayHi,
            AiUInt32 *DayLo,
            AiUInt32 *Hour,
            AiUInt32 *Minute,
            AiUInt32 *Second)
{
    AiUInt32 temp;

    TcpOut(GLOBAL_VAR, TC_IEDAYHI, *DayHi); /* IRIG time encode day high  */
    TcpOut(GLOBAL_VAR, TC_IEDAYLO, *DayLo); /* IRIG time encode day low   */
    TcpOut(GLOBAL_VAR, TC_IEHOUR, *Hour); /* IRIG time encode hour      */
    TcpOut(GLOBAL_VAR, TC_IEMIN, *Minute); /* IRIG time encode minute    */
    TcpOut(GLOBAL_VAR, TC_IESEC, *Second); /* IRIG time encode seconds   */

    /* set bit for leap year (or not)*/
    /* set bit for register update*/

    /* in the SysFlags(TCP firmware), or IRIG control register (Monitor)*/

    if( *leap_year == 1 )
        temp = TC_I_LEAP_B;
    else
        temp = ~TC_I_LEAP_B;

    temp = temp | TC_I_ELATCH_B;

    TcpOut(GLOBAL_VAR, TC_ICTRL, temp);

    return (PASS);
}
/*****************************************************************************/
/*                                                                           */
/*   Module : MonSpecific.c            Submodule : update_NOVRAM_FLASH       */
/*   Author : Lamp Matthias            Project   : PMC-MON                   */
/*---------------------------------------------------------------------------*/
/*   Create : 06/04/2005  (v01.14)     Update :    /  /                      */
/*---------------------------------------------------------------------------*/
/*    Descriptions                                                           */
/*    ------------                                                           */
/*    Inputs    : GLOBAL_VAR    Global struct                                 */
/*                                                                             */
/*    Outputs   :                                                            */
/*                                                                           */
/*    Description :   Copy a "Mirror" of the NOVRAM values into the          */
/*                    FLASH of the TCP.                                      */
/*                    Therfore it is neccesarry to erase the FLASH before    */
/*                    the FLASH can be changed. (Firmware!!!)                */
/*                                                                           */
/*****************************************************************************/
AiUInt32 update_NOVRAM_FLASH(TY_MONITOR_DATA_STRUCT *GLOBAL_VAR)
{
    /*
     1. read back TCP firmware (FLASH) into HOST memory
     2. read back TCP-NOVRAM
     3. chip erase (TCP, FLASH and EEPROM)
     4. Program TCP flash (from HOST memory)
     5. PROGRAM TCP EEPROM
     6. Copy NOVRAM mirror into TCP FLASH
     7. LOCK and FUSE BITS in the TCP
     8. Verify FLASH (FIRMWARE and NOVRAM)
     9. Restart TCP
     */
    AiUInt8 *srec;
    AiUInt32 signature;/*temp,*/
    AiUInt32 i, size = (8 * 1024);
    AiUInt16 dummymem;

    /*allocate memory*/
    srec = (GLOBAL_VAR->GLOBAL_TEMP_MEM);
    memset(srec, 0, size); /* set buffer (allocated memory) to "0" */

    /*1. read back TCP firmware (FLASH)*/

    MON_ONLY_PRINTF("\nReading TCP-Flash");

    wait_clocks(GLOBAL_VAR, 10000); /* wait 10ms */

    /* Enable SPI port Control */
    GLOBAL_VAR->PMC_TCP_PROG_Reg.all = 0;
    GLOBAL_VAR->PMC_TCP_PROG_Reg.Reg.SPI_OE = 1;
    PCI_IOReg_write(GLOBAL_VAR, PMC_TCP_PROG_Reg_Adr, GLOBAL_VAR->PMC_TCP_PROG_Reg.all);

    wait_clocks(GLOBAL_VAR, 100); /* 100us */
    AI_Printf("\nEnable Programming Mode on TCP.");

    /*************************************************/
    /* enable SPI communication,*/
    /* read device ID, ...*/
    signature = spi_prog_enable(GLOBAL_VAR);
    if( signature == FAIL )
        return (FAIL);
    /*************************************************/
    AI_Printf("\nReading TCP firmware from FLASH");
    for( i = 0; i < 0x1e00; i += 2 )
    {
        if( i % 0xff == 0 )
            AI_Printf(".");

        dummymem = read_spi_flash(GLOBAL_VAR, i >> 1);
        *(srec + i) = (AiUInt8) (dummymem & 0x00ff);
        *(srec + i + 1) = (AiUInt8) ((dummymem & 0xff00) >> 8);
    }

    /*2. read back TCP-NOVRAM */
    AI_Printf("\nReading NOVRAM-values from NOVRAM");
    for( i = 0x1e00; i < 0x2000; i++ )
        *(srec + i) = read_spi_novram(GLOBAL_VAR, i);

    /*3. chip erase (TCP, FLASH and EEPROM) */
    AI_Printf("\nERASE TCP");
    AI_Printf("\nNOVRAM and FLASH = 0xff");
    erase_tcp(GLOBAL_VAR);

    /* 4. Program TCP flash (from HOST memory) */
    MON_ONLY_PRINTF("\nProgram TCP firmware into FLASH");
    prog_TCP_flash(GLOBAL_VAR, 0, signature, srec); /* 0=program FIRMWARE into TCP-FLASH */

    /* 5. PROGRAM TCP EEPROM  (values from GLOBAL_VAR, & INIT process) */
    AI_Printf("\nProgram NOVRAM-values into NOVRAM\n");
    for( i = 0; i < 512; i++ )
    {
        if( write_spi_novram(GLOBAL_VAR, i, *(srec + 0x1e00 + i)) == FAIL )
        {
            MON_ONLY_PRINTF("e");
        }
        else
        {
            MON_ONLY_PRINTF(".");
        }
        if( i % 10 == 0 )
            AI_Printf("\r           NOVRAM words: %x     \r", (i / 4));
    }

    /* 6. Copy NOVRAM mirror into TCP FLASH */
    AI_Printf("\nProgram TCP NOVRAM mirror into FLASH");
    prog_TCP_flash(GLOBAL_VAR, 1, signature, (srec + 0x1e00)); /* 1 = write NOVRAM mirror into TCP-FLASH*/

    /* 7. LOCK and FUSE BITS in the TCP */
    if( prog_TCP_FL_bits(GLOBAL_VAR, signature) == FAIL ) /*Program the TCP Lock and Fuse Bits*/
    {
        AI_Printf("\nError Programming TCP Lock and Fuse Bits");
        return (FAIL);
    }

    /* 8. Verify FLASH (FIRMWARE and NOVRAM) */
    if( verify_TCP_flash(GLOBAL_VAR, (srec)) == FAIL )
    {
        AI_Printf("\nError Verify TCP FLASH");
        return (FAIL);
    }

    /* 9. restart TCP */
    /* SPI PROG disable */
    GLOBAL_VAR->PMC_TCP_PROG_Reg.all = 0;
    GLOBAL_VAR->PMC_TCP_PROG_Reg.Reg.SPI_OE = 0;
    GLOBAL_VAR->PMC_TCP_PROG_Reg.Reg.SPI_MOSI = 0;
    GLOBAL_VAR->PMC_TCP_PROG_Reg.Reg.SPI_CLK = 0;
    PCI_IOReg_write(GLOBAL_VAR, PMC_TCP_PROG_Reg_Adr, GLOBAL_VAR->PMC_TCP_PROG_Reg.all);
    wait_clocks(GLOBAL_VAR, 10000);
    AI_Printf("\nTCP update / programming finish, restart ...");
    if( (start_ATMEL(GLOBAL_VAR)) == PASS )
    {
        MON_ONLY_PRINTF("\nTCP successfully started");
    }
    else
    {
        AI_Printf("\nERROR, TCP does not response ready");
        return (FAIL);
    }

    return (PASS);
}

/*****************************************************************************/
/*                                                                           */
/*   Module : MonSpecific.c            Submodule : prog_TCP_flash            */
/*   Author : Lamp Matthias            Project   : PMC-MON                   */
/*---------------------------------------------------------------------------*/
/*   Create : 06/04/2005  (v01.14)     Update :    /  /                      */
/*---------------------------------------------------------------------------*/
/*    Descriptions                                                           */
/*    ------------                                                           */
/*    Inputs    : GLOBAL_VAR  Global struct                                     */
/*                AiUInt32    block      0 = Firmware                        */
/*                                         1 = NOVRAM mirror                   */
/*                AiUInt32    signature  ATMEL signature (8535 or mega8535)  */
/*                xxx         data       Data pointer                        */
/*                                                                             */
/*    Outputs   : AiUInt32    return value PASS / FAIL                       */
/*                                                                           */
/*    Description :   A part of the TCP flash (Block 0=Firmware, 1=NOVRAM)   */
/*                    is filled with data values                             */
/*                    TCP firmware, or TCP NOVRAM mirror                     */
/*                                                                           */
/*****************************************************************************/
AiUInt32 prog_TCP_flash(TY_MONITOR_DATA_STRUCT *GLOBAL_VAR, AiUInt32 block, AiUInt32 signature, AiUInt8 *data)
{
    AiUInt32 s_page, /*start page*/
    e_page, /*end page  */
    z = 0, x, y;
    AiUInt16 tmp;

    if( block == 0 ) /*prog. FIRMWARE*/
    {
        s_page = 0;
        e_page = 120;
    }
    else if( block == 1 ) /*prog. NOVRAM mirror*/
    {
        s_page = 120;
        e_page = 128;
    }
    else
    {
        MON_ONLY_PRINTF("\nError undefinded Block");
        return (FAIL);
    }

    if( (signature & 0xffff) == SPI_AVRMEGA8535 ) /* AVR MEGA8535  */
    {
        MON_ONLY_PRINTF("\nAVR MEGA 8535\n\n");
        for( y = s_page; y < e_page; y++ ) /* flash is organized as 128 pages*/
        {
            if( y % 8 == 0 )
            {
                AI_Printf("\r               program byte no: 0x%04x of 0x%04x \r", z, e_page * 2 * 32);
            }
            else
            {
                MON_ONLY_PRINTF(".");
            }
            for( x = 0; x < 32; x++ ) /* 32 words per page*/
            {
                if( z > 0x1e00 ) /* 120*32*2 max. Firmware size*/
                    break;
                else
                    tmp = *(data + z);

                z++;
                write_spi(GLOBAL_VAR, SPI_MEGA8535_WR_PROG_MEM_LOWB);
                write_spi(GLOBAL_VAR, 0x00);
                write_spi(GLOBAL_VAR, (AiUInt8) x);
                write_spi(GLOBAL_VAR, (AiUInt8) tmp);

                wait_clocks(GLOBAL_VAR, 4500); /* min. wait time */

                if( z > 0x1e00 ) /*max size*/
                    break;
                else
                    tmp = *(data + z);

                z++;
                write_spi(GLOBAL_VAR, SPI_MEGA8535_WR_PROG_MEM_HIGHB);
                write_spi(GLOBAL_VAR, 0x00);
                write_spi(GLOBAL_VAR, (AiUInt8) x);
                write_spi(GLOBAL_VAR, (AiUInt8) tmp);

                wait_clocks(GLOBAL_VAR, 4500); /* min. wait time */
            } /*for x*/

            /* and now program the page*/
            write_spi(GLOBAL_VAR, SPI_MEGA8535_WR_MEM_PAGE);
            write_spi(GLOBAL_VAR, (AiUInt8) ((y & 0x78) >> 3)); /* bit 6,5,4,3 */
            write_spi(GLOBAL_VAR, (AiUInt8) ((y & 0x07) << 5)); /* bit 2,1,0*/
            write_spi(GLOBAL_VAR, 0x00);

            wait_clocks(GLOBAL_VAR, 4500); /* min. wait time */

            if( z > 0x1e00 ) /*max size*/
                break;
        } /*for y*/
    }
    else if( (signature & 0xffff) == SPI_AVR8535 ) /* AVR 90S8535 */
    {
        MON_ONLY_PRINTF("\nAVR 90S8535\n\n");

        s_page *= 32; /* page no (0, 120)  * words (32) */
        e_page *= 32; /* page no (120, 128)* words (32) */

        x = 0;

        for( y = s_page; y < e_page; y++ )
        {
            if( y % 8 == 0 )
            {
                MON_ONLY_PRINTF("\r                   byte no: %04d of %04d (dec) \r",x,e_page*2);
            }

            tmp = (*(data + x + 1) << 8);
            tmp = (tmp | (*(data + x)));
            /* MSB---data---LSB*/

            /* one flash location*/
            if( write_spi_flash(GLOBAL_VAR, y, tmp) == FAIL )
            {
                MON_ONLY_PRINTF("\nError Programming Flash");
                return (FAIL);
            }
            else
            {
                MON_ONLY_PRINTF(".");
            }

            x = x + 2;
        }/*for()*/
    }/*if ((signature & 0xffff) == SPI_AVRMEGA8535)    AVR MEGA8535 v01.04*/
    else /* Error Case */
    {
        MON_ONLY_PRINTF("\nUndefinded ATMEL signature");
        return (FAIL);
    }
    return (PASS);
}

/*****************************************************************************/
/*                                                                           */
/*   Module : MonSpecific.c            Submodule : prog_TCP_FL_bits          */
/*   Author : Lamp Matthias            Project   : PMC-MON                   */
/*---------------------------------------------------------------------------*/
/*   Create : 07/04/2005  (v01.14)     Update :    02/09/2013 V02.24         */
/*---------------------------------------------------------------------------*/
/*    Descriptions                                                           */
/*    ------------                                                           */
/*    Inputs    : GLOBAL_VAR  Global struct                                     */
/*                AiUInt32    signature  ATMEL signature (8035 or mega8035)  */
/*                                                                             */
/*    Outputs   : AiUInt32    return value PASS / FAIL                       */
/*                                                                           */
/*    Description :   The Lock and Fuse Bits of the TCP are programmed       */
/*                    different for 8535 and MEGA8535                        */
/*                                                                           */
/*****************************************************************************/
AiUInt32 prog_TCP_FL_bits(TY_MONITOR_DATA_STRUCT *GLOBAL_VAR, AiUInt32 signature)
{
    AiUInt32 tmp;

    /*****************************************************************/
    /*-------- program LOCK and FUSE bits (first fuse than lock bits)*/
    if( (signature & 0xffff) == SPI_AVRMEGA8535 ) /* AVR MEGA8535 */
    {
        MON_ONLY_PRINTF("\nprogram FUSE bits, FUSE high bits and LOCK bits AVR MEGA8535");
        /* program fuse bits*/
        /* fuse bit format: BODLEVEL BODEN SUT1 SUT0 CKSEL3 CKSEL2 CKSEL1 CKSEL0*/
        /* BODLEVEL                       =  1    => brown out detector trigger level 0=4V, 1=2,7V*/
        /* BODEN                          =  0    => brown out detection enabled*/
        /* SUT1, SUT0                     =  00   => start up time is minimal, BOD must be enabled*/
        /* CKSEL3, CKSEL2, CKSEL1, CKSEL0 =  0000 => external clock*/
        write_spi(GLOBAL_VAR, SPI_MEGA8535_WR_FUSE_BITS_B1);
        write_spi(GLOBAL_VAR, SPI_MEGA8535_WR_FUSE_BITS_B2);
        write_spi(GLOBAL_VAR, 0x0);
        write_spi(GLOBAL_VAR, 0x80);

        wait_clocks(GLOBAL_VAR, 5000);

        /* program fuse high bits*/
        /* fuse high bit format: S8535C WDTON SPIEN CKOPT EESAVE BOOTSZ1 BOOTSZ0 BOOTRST*/
        /* S8535C           = 0   => select AT90S8535 compatibility mode*/
        /* WDTON            = 1   => disable*/
        /* SPIEN            = 0   => SPI enable (not accessible via SPI)*/
        /* CKOPT            = 1   => disable*/
        /* EESAVE           = 1   => EEPROM not preserved*/
        /* BOOTSZ1, BOOTSZ0 = 00  => max size (default)*/
        /* BOOTRST          = 1   => Resetvector = 0x0000 (Application Reset)*/
        write_spi(GLOBAL_VAR, SPI_MEGA8535_WR_FUSE_BITS_H_B1);
        write_spi(GLOBAL_VAR, SPI_MEGA8535_WR_FUSE_BITS_H_B2);
        write_spi(GLOBAL_VAR, 0x0);
        write_spi(GLOBAL_VAR, 0x59);

        wait_clocks(GLOBAL_VAR, 5000);

        /* program lock bits*/
        /* lock bit format: 1 1 BLB12 BLB11 BLB02 BLB01 LB2 LB1 (0 to program) */
        /* LB2, LB1     =11 (unprogrammed) => no memory lock features enabled*/
        /* BLB02, BLB01 =11 (unprogrammed) => no restrictions for SPM or LPM accessing the Aplication section*/
        /* BLB12, BLB11 =11 (unprogrammed) => no restrictions for SPM or LPM accessing the boot loader section*/
        write_spi(GLOBAL_VAR, SPI_MEGA8535_WR_LOCK_BITS_B1);
        write_spi(GLOBAL_VAR, SPI_MEGA8535_WR_LOCK_BITS_B2);
        write_spi(GLOBAL_VAR, 0x0);
        write_spi(GLOBAL_VAR, 0xFF); /* set lock bits*/ /* ML 28.08.2013 protect memory, mon v02.3027.06.2016 */
                                                        /* LB2, LB1 = 1 0 = Future Programming of the Flsh and EEPROM is disabled in */
                                                        /* Paralell and Serial Programming mode,. The Fuse Bits are locked */
                                                        /* in both Serial and Paralell programming mode */
                                                        /* BLB12, BLB11 = 1 0; SPM (store prog. memory) is not allowed to write aplication code */
                                                        /* BLB02, BLB01 = 1 0; SPM is not allowed to write to the boot loader section */
        wait_clocks(GLOBAL_VAR, 5000);

        /*****************************************************************/
        /*-------- verify LOCK and FUSE bits (first fuse than lock bits)*/
        /* verify fuse bits            */
        write_spi(GLOBAL_VAR, SPI_MEGA8535_RD_LOCK_BITS); /* read lock bits*/
        write_spi(GLOBAL_VAR, 0x00); /* 0*/
        write_spi(GLOBAL_VAR, 0x00); /* x*/
        tmp = read_spi(GLOBAL_VAR); /* lock bits*/

        wait_clocks(GLOBAL_VAR, 5000);

        if ((tmp & 0x3f) != 0x3f) /*verify lock bits (lower 6 bits)., v02.30*/
        {
            MON_ONLY_PRINTF("\nverify LOCK BITS fail");
            return (FAIL);
        }
        else
        {
            if( GLOBAL_VAR->STATUS_FLAGS.Reg.BATCH_MODE == 0 ) /* normal monitor mode */
            {
                MON_ONLY_PRINTF("\nLock Bits (0=programmed, 1=unprogrammed)"); 
                MON_ONLY_PRINTF("\nBLB12:    %x, BLB11:    %x, BLB02:    %x, BLB01:    %x,\nLB2:      %x, LB1:      %x",
                            (tmp & 0x20)>>5,(tmp & 0x10)>>4,(tmp & 0x8)>>3,(tmp & 0x4)>>2,(tmp & 0x2)>>1,(tmp & 0x1) );
            }
        }

        write_spi(GLOBAL_VAR, SPI_MEGA8535_RD_FUSE_BITS_B1); /* read fuse bits*/
        write_spi(GLOBAL_VAR, SPI_MEGA8535_RD_FUSE_BITS_B2);
        write_spi(GLOBAL_VAR, 0x00); /* x*/
        tmp = read_spi(GLOBAL_VAR); /* fuse bits*/

        wait_clocks(GLOBAL_VAR, 5000);

        if( tmp != 0x80 ) /*verify fuse low bits 0x0*/
        {
            MON_ONLY_PRINTF("\nverify FUSE BITS fail");
            return (FAIL);
        }
        else
        {
            if( GLOBAL_VAR->STATUS_FLAGS.Reg.BATCH_MODE == 0 ) /* normal monitor mode */
            {
                MON_ONLY_PRINTF("\nFUSE Bits (0=programmed, 1=unprogrammed)");
                MON_ONLY_PRINTF("\nBODLEVEL: %x, BODEN:    %x, SUT1:     %x, SUT0:     %x,\nCKSEL3:   %x, CKSEL2:   %x, CKSEL1:   %x, CKSEL0:   %x",
                            (tmp & 0x20)>>7,(tmp & 0x20)>>6,(tmp & 0x20)>>5,(tmp & 0x10)>>4,(tmp & 0x8)>>3,(tmp & 0x4)>>2,(tmp & 0x2)>>1,(tmp & 0x1) );
            }
        }

        write_spi(GLOBAL_VAR, SPI_MEGA8535_RD_FUSE_BITS_H_B1); /* read fuse high bits*/
        write_spi(GLOBAL_VAR, SPI_MEGA8535_RD_FUSE_BITS_H_B2);
        write_spi(GLOBAL_VAR, 0x00); /* x*/
        tmp = read_spi(GLOBAL_VAR); /* fuse high bits*/

        wait_clocks(GLOBAL_VAR, 5000);
        if( tmp != 0x59 ) /*verify fuse high bits*/
        {

            MON_ONLY_PRINTF("\nverify FUSE HIGH BITS fail");
            return (FAIL);
        }
        else
        {
            if( GLOBAL_VAR->STATUS_FLAGS.Reg.BATCH_MODE == 0 ) /* normal monitor mode */
            {
                MON_ONLY_PRINTF("\nFUSE HIGH Bits (0=programmed, 1=unprogrammed)");
                MON_ONLY_PRINTF("\nS8535C:   %x, WDTON:    %x, SPIEN:    %x, CKOPT:    %x,\nEESAVE:   %x, BOOTSZ1:  %x, BOOTSZ0:  %x, BOOTRST:  %x",
                            (tmp & 0x20)>>7,(tmp & 0x20)>>6,(tmp & 0x20)>>5,(tmp & 0x10)>>4,(tmp & 0x8)>>3,(tmp & 0x4)>>2,(tmp & 0x2)>>1,(tmp & 0x1) );
            }
        }
    } /*if signature*/
    else if( (signature & 0xffff) == SPI_AVR8535 ) /* *************************************** AVR 90S8535 */
    {
        /*  MON_ONLY_PRINTF("\nNo FUSE and LOCK Bits have to be programmend for AVR 90S8535"); */
        MON_ONLY_PRINTF("\nprogram FUSE bits and LOCK bits for AVR 90S8535");

        /* program fuse bit*/
        /* FSTRT FUSE, = 1 => When the FSTRT Fuse is programmed (?0?),        */
        /*                    the short start-up time is selected.            */
        /*                      Default value is unprogrammed (?1?).            */
        /* SPIEN FUSE, = 1 => NOT ACCESSIBLE                                  */
        /*                    When the SPIEN Fuse is programmed (?0?),        */
        /*                    Serial Program and Data Downloading is enabled. */
        /*                    Default value is programmed (?0?).              */
        /*                    The SPIEN Fuse is not accessible in             */
        /*                    Serial Programming Mode.                        */
        /* Write FSTRT Fuse: 1010.1100 1011.111F xxxx.xxxx xxxx.xxxx   F=FSTRT Fuse 0=prog. */
        /*                    ac Bf 00 00                                                   */
        write_spi(GLOBAL_VAR, 0xac);
        write_spi(GLOBAL_VAR, 0xbf);
        write_spi(GLOBAL_VAR, 0x00);
        write_spi(GLOBAL_VAR, 0x00);

        wait_clocks(GLOBAL_VAR, 5000);

        /* program lock bits*/
        /* Memory Lock Bits*/
        /* Mode LB1 LB2 Protection Type                                                */
        /*  1    1   1  No memory lock features enabled.                               */
        /*  2    0   1  Further programming of the Flash and EEPROM is disabled.(1)    */
        /*  3    0   0  Same as mode 2 and verify is also disabled.                    */
        /*  Note: 1. In Parallel Mode, further programming of the Fuse bits is also    */
        /*           disabled. Program the Fuse bits before programming the Lock bits. */
        /* LB1, LB2, = 01 => Mode 2                                                    */
        /* Write Lock Bits: 1010.1100 1111.1BA1 xxxx.xxxx xxxx.xxxx   B=Lock Bit 2 = 1 */
        /*                                                            A=Lock Bit 1 = 0 */
        /*                   ac fd 00 00*/
        write_spi(GLOBAL_VAR, 0xac);
        write_spi(GLOBAL_VAR, 0xfd);
        write_spi(GLOBAL_VAR, 0x00);
        write_spi(GLOBAL_VAR, 0x00);

        wait_clocks(GLOBAL_VAR, 5000);

        /*****************************************************************/
        /*-------- verify LOCK and FUSE bits                            */
        write_spi(GLOBAL_VAR, SPI_RD_LOCKFUSE_BITS); /* read LOCK and FUSEbits     */
        write_spi(GLOBAL_VAR, 0x00); /* x                          */
        write_spi(GLOBAL_VAR, 0x00); /* x                          */
        tmp = read_spi(GLOBAL_VAR); /* LOCK  and FUSE bits        */
        /* ABSx.xxxF   A = LOCK Bit 1 = 0 */
        /* 010x.xxx1   B = LOCK Bit 2 = 1 */
        /*             S = SPIEN Fuse = 0 */
        /*             F = FSTRT Fuse = 1 */
        wait_clocks(GLOBAL_VAR, 5000);

        if( (tmp & 0xe1) != 0x41 ) /* verify lock and fuse bits      */
        {
            MON_ONLY_PRINTF("\nverify LOCK and FUSE BITS fail");
            return (FAIL);
        }
        else
        {
            if( GLOBAL_VAR->STATUS_FLAGS.Reg.BATCH_MODE == 0 ) /* normal monitor mode */
            {
                AI_Printf("\nLock and FUSE Bits (0=programmed, 1=unprogrammed)");
                AI_Printf("\nLB1: %x, LB2: %x, FSTRT: %x, SPIEN: %x", (tmp & 0x80) >> 7, (tmp & 0x40) >> 6, (tmp & 0x20) >> 5, (tmp & 0x1));
            }
        }
    }
    else /* *************************************** undefined Signature */
    {
        MON_ONLY_PRINTF("\n *** ERROR ***, Undefinded ATMEL signature");
        return (FAIL);
    }
    return (PASS);
}

/*****************************************************************************/
/*                                                                           */
/*   Module : MonSpecific.c            Submodule : verify_TCP_flash          */
/*   Author : Lamp Matthias            Project   : PMC-MON                   */
/*---------------------------------------------------------------------------*/
/*   Create : 07/04/2005  (v01.14)     Update :    /  /                      */
/*---------------------------------------------------------------------------*/
/*    Descriptions                                                           */
/*    ------------                                                           */
/*    Inputs    : GLOBAL_VAR  Global struct                                     */
/*                AiUInt32    signature  ATMEL signature (8035 or mega8035)  */
/*                                                                             */
/*    Outputs   : AiUInt32    return value PASS / FAIL                       */
/*                                                                           */
/*    Description :   Verify the FLASH of the TCP with the programmed        */
/*                    FIRMWARE and NOVRAM data                               */
/*                                                                           */
/*****************************************************************************/
AiUInt32 verify_TCP_flash(TY_MONITOR_DATA_STRUCT *GLOBAL_VAR, AiUInt8 *data)
{
    AiUInt16 is_data, rd_data;
    AiUInt32 x;

    for( x = 0; x < 4096; x++ )
    {
        is_data = (*(data + (x * 2) + 1) << 8);
        is_data = (is_data | (*(data + (x * 2))));

        rd_data = read_spi_flash(GLOBAL_VAR, x);

        if( rd_data != is_data )
        {
            MON_ONLY_PRINTF("\nverify error, adr.:0x%04x flash:0x%04x srec:0x%04x",x,rd_data,is_data);
            return (FAIL);
        }
        if( x % (8 * 32) == 0 )
        {
            MON_ONLY_PRINTF("\r                verify byte no: 0x%04x of 0x2000       \r",x*2);
        }
        else if( x % 32 == 0 )
        {
            MON_ONLY_PRINTF(".");
        }
    }MON_ONLY_PRINTF("\nverify passed");
    return (PASS);
}

/*****************************************************************************/
/*                                                                           */
/*   Module : MonSpecific.c            Submodule : get_flash_mirror          */
/*   Author : Lamp Matthias            Project   : PMC-MON                   */
/*---------------------------------------------------------------------------*/
/*   Create : 07/04/2005  (v01.14)     Update :    /  /                      */
/*---------------------------------------------------------------------------*/
/*    Descriptions                                                           */
/*    ------------                                                           */
/*    Inputs    : GLOBAL_VAR  Global struct                                     */
/*                AiUInt32    data  data pointer to the read data            */
/*                                                                             */
/*    Outputs   : AiUInt32    return value PASS / FAIL                       */
/*                                                                           */
/*    Description :   Look if the FLASH mirror has valid data, and reads it  */
/*                                                                           */
/*****************************************************************************/
AiUInt32 get_flash_mirror(TY_MONITOR_DATA_STRUCT *GLOBAL_VAR, AiUInt8 *data)
{
    AiUInt32 x, y = 0, m1_tmp = 0, m2_tmp = 0, chk_tmp1 = 0, chk_tmp2 = 0;
    AiUInt16 rd_data;

    /*************************************************/
    /* enable SPI communication (TCP disabled)       */
    spi_prog_enable(GLOBAL_VAR);

    for( x = 0xf00; x <= 0xfff; x++ )
    {
        rd_data = read_spi_flash(GLOBAL_VAR, x);
        *(data + y) = rd_data & 0x00ff;
        *(data + y + 1) = (rd_data & 0xff00) >> 8;
        y += 2;
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

    m1_tmp = ((*(data + 3)) << 24) | ((*(data + 2)) << 16) | ((*(data + 1)) << 8) | (*(data)); /*Magic 1*/
    m2_tmp = ((*(data + 0x1fb)) << 24) | ((*(data + 0x1fa)) << 16) | ((*(data + 0x1f9)) << 8) | (*(data + 0x1f8)); /*Magic 1*/
    chk_tmp2 = ((*(data + 0x1ff)) << 24) | ((*(data + 0x1fe)) << 16) | ((*(data + 0x1fd)) << 8) | (*(data + 0x1fc)); /*chksum */

    /*calculate checksum*/
    for( x = 0; x < 508; x += 4 )
    {
        chk_tmp1 += (*(data + x + 3) << 24) | (*(data + x + 2) << 16) | (*(data + x + 1) << 8) | *(data + x); /*calculate chksum*/
    }

    if( chk_tmp1 != chk_tmp2 )
    {
        MON_ONLY_PRINTF("\nERROR NOVRAM FLASH mirror, calculated Checksum != FLASH Checksum");MON_ONLY_PRINTF("\nCalculated Checksum: %08x FLASH Checksum: %08x",chk_tmp1,chk_tmp2);
        return (FAIL);
    }
    if( (m1_tmp != NVR_MAGIC_def) || (m2_tmp != NVR_MAGIC_def) )
    {
        MON_ONLY_PRINTF("\nERROR NOVRAM FLASH mirror, NOVRAM MAGIC 1 or 2 FALSE");MON_ONLY_PRINTF("\nNVR MAGIC 1 is: %08x NVR MAGIC 2 is: %08x should: %08x",m1_tmp,m2_tmp,NVR_MAGIC_def);
        return (FAIL);
    }MON_ONLY_PRINTF("\nFound valid NOVRAM mirror in TCP FLASH");

    return (PASS);
}

/*****************************************************************************/
/*                                                                           */
/*   Module : ini_prog.c               Submodule : read_TCP_flash            */
/*   Author : Lamp Matthias            Project   : PMC-1553-MON              */
/*---------------------------------------------------------------------------*/
/*   Create : 17.10.2001   Update :   .  .                                   */
/*---------------------------------------------------------------------------*/
/*    Descriptions                                                           */
/*    ------------                                                           */
/*    Inputs    :                                                            */
/*                                                                           */
/*    Outputs   : AiUInt32 PASS / FAIL                                         */
/*                                                                           */
/*    Description :  Programmes TCP Flash                                    */
/*                                                                           */
/*****************************************************************************/
#ifdef __monitor__
AiUInt32 read_TCP_flash(TY_MONITOR_DATA_STRUCT *GLOBAL_VAR)
{
    AiUInt32 i,size=(8*1024);
    AiUInt16 dummymem;
    AiUInt8 *memory;

    memory = (AiUInt8 *) malloc(size);
    if(memory == ZERO)
    {
        AI_Printf( "\nInsufficient memory available\n" );
        return(FAIL);
    }

    MON_ONLY_PRINTF("\nReading TCP-Flash");
    wait_clocks(GLOBAL_VAR, 10000); /*wait 10ms*/

    /* Enable SPI port Control */
    /*    SPI_CLK_LOW*/
    GLOBAL_VAR->PMC_TCP_PROG_Reg.all = 0;
    GLOBAL_VAR->PMC_TCP_PROG_Reg.Reg.SPI_OE = 1;
    PCI_IOReg_write(GLOBAL_VAR, PMC_TCP_PROG_Reg_Adr,GLOBAL_VAR->PMC_TCP_PROG_Reg.all);

    wait_clocks(GLOBAL_VAR, 100); /*100us*/
    MON_ONLY_PRINTF("\nEnable Programming Mode on TCP.");

    spi_prog_enable(GLOBAL_VAR);
    MON_ONLY_PRINTF("\nReading TCP Flash");

    for (i=0; i<size;i+=2 )
    {
        if((i % 0xff)==0)
        AI_Printf(".");

        dummymem = read_spi_flash(GLOBAL_VAR, i>>1);
        *(memory+i) = (AiUInt8) ( dummymem & 0x00ff ); /* eventuell kein cast notwendig*/
        *(memory+i+1) = (AiUInt8) (( dummymem & 0xff00 ) >> 8 ); /* reihenfolge memory+i, memory+i+1*/
        /* kontrollieren*/
    }

    /* Release Reset from TCP */
    MON_ONLY_PRINTF("\nRead Done, enabling TCP.");
    wait_clocks(GLOBAL_VAR, 100); /*100us*/

    GLOBAL_VAR->PMC_RESET_Reg.Reg.ATMEL_RESx = 1;
    PCI_IOReg_write(GLOBAL_VAR, PMC_RESET_Reg_Adr,GLOBAL_VAR->PMC_RESET_Reg.all);
    wait_clocks(GLOBAL_VAR, 50000); /*50ms*/

    /* eventuell wait for ATMEL_RDY*/
    MON_ONLY_PRINTF("\nWrite FLASH to S3-Record");

    i = 0;
    i = print_s3recs(GLOBAL_VAR,6, memory,size); /* 6=tcp flash back*/

    /* SPI PROG disable */
    GLOBAL_VAR->PMC_TCP_PROG_Reg.all = 0;
    GLOBAL_VAR->PMC_TCP_PROG_Reg.Reg.SPI_OE = 0;
    GLOBAL_VAR->PMC_TCP_PROG_Reg.Reg.SPI_MOSI = 0;
    GLOBAL_VAR->PMC_TCP_PROG_Reg.Reg.SPI_CLK = 0;
    PCI_IOReg_write(GLOBAL_VAR, PMC_TCP_PROG_Reg_Adr,GLOBAL_VAR->PMC_TCP_PROG_Reg.all);
    wait_clocks(GLOBAL_VAR, 10000);

    free(memory);
    if(i==PASS)
    return(PASS);
    else
    return(FAIL);
}
#endif

#ifdef __monitor__

/*****************************************************************************/
/*                                                                           */
/*   Module : ini_prog.c               Submodule : fill_GLOBAL_RAM           */
/*   Author : Lamp Matthias            Project   : PMC-1553-MON              */
/*---------------------------------------------------------------------------*/
/*   Create : 06/10/2001   Update :   .  .                                   */
/*---------------------------------------------------------------------------*/
/*    Descriptions                                                           */
/*    ------------                                                           */
/*    Inputs    : GLOBAL Struct                                              */
/*                                                                           */
/*    Outputs   : AiUInt32:  PASS or FAIL                                      */
/*                                                                           */
/*    Description :  Dumpts a part of the memory (GLOBAL RAM)                */
/*                                                                           */
/*****************************************************************************/
AiUInt32 fill_GLOBAL_RAM( TY_MONITOR_DATA_STRUCT *GLOBAL_VAR )
{
    AiUInt32 i;
    AiUInt32 SAddress, EAddress,data;

    kbdflush();
    kbdflush();
    kbdflush();

    AI_Printf("\nEnter Start addr. End addr. and Data (hex) : ");
    SCANF("%lx %lx %lx",&SAddress,&EAddress,&data);

    if(EAddress> (GLOBAL_VAR->RAM_Size_Reg.Reg.GRam_Size*1024*1024))
    {
        AI_Printf("\nEND addr, out of the memory range (%08x)",(GLOBAL_VAR->RAM_Size_Reg.Reg.GRam_Size*1024*1024));
        return(FAIL);
    }

    for (i=(SAddress/4);i<(EAddress/4);i++) /*Achtung AiUInt32 Zugriffe sind 4 Bytes ueberall !!!*/
    PCI_Mem_write(GLOBAL_VAR,i,data);

    return(PASS);
}

/*****************************************************************************/
/*                                                                           */
/*   Module :                          Submodule : fill_GLOBAL_RAM_flex      */
/*   Author : Lamp Matthias            Project   : PMC-1553-MON              */
/*---------------------------------------------------------------------------*/
/*   Create : 12/12/2002   Update :   13/03/2003                             */
/*---------------------------------------------------------------------------*/
/*    Descriptions                                                           */
/*    ------------                                                           */
/*    Inputs    : GLOBAL Struct                                              */
/*                                                                           */
/*    Outputs   : AiAiUInt32  :  PASS or FAIL                                */
/*                                                                           */
/*    Description :  open access to GLOBAL RAM (Byte, Word Long or LongLong) */
/*                                                                           */
/*****************************************************************************/
AiUInt32 fill_GLOBAL_RAM_flex( TY_MONITOR_DATA_STRUCT *GLOBAL_VAR )
{
    AiUInt32 i,j;
    AiUInt32 SAddress, EAddress,data,loop_cnt;
    AiUInt32 choice,temp;
    AiUInt8 word_size,block_size;

    AI_Printf("\nEnter access mode (1=BYTE, 2=Word, 3=UINT32 (LONG), 4=64Bit) : ");

    choice = TOUPPER(GETCH());
    PUTCHAR(choice);
    kbdflush();

    AI_Printf("\nEnter Start addr. End addr. Data (hex) and loop cnt.: ");
    SCANF("%lx %lx %lx %lx",&SAddress,&EAddress,&data,&loop_cnt);

    if(EAddress> (GLOBAL_VAR->RAM_Size_Reg.Reg.GRam_Size*1024*1024))
    {
        AI_Printf("\nEND addr, out of the memory range (%08x)",(GLOBAL_VAR->RAM_Size_Reg.Reg.GRam_Size*1024*1024));
        return(FAIL);
    }

    switch (choice)
    {
        case '1' :
        word_size=1;
        data=data & 0xff;
        AI_Printf("\nAccess mode = BYTE\n");
        AI_Printf("\nStart addr.: %x End addr.: %x Data: %x;",SAddress,EAddress,data);
        break;
        case '2' :
        word_size=2;
        data=data & 0xffff;
        AI_Printf("\nAccess mode = WORD\n");
        AI_Printf("\nStart addr.: %x End addr.: %x Data: %x;",SAddress,EAddress,data);
        break;
        case '3' :
        word_size=4;
        data=data & 0xffffffff;
        AI_Printf("\nAccess mode = UINT32 (LONG)\n");
        AI_Printf("\nStart addr.: %x End addr.: %x Data: %x;",SAddress,EAddress,data);
        break;
        case '4' :
        word_size=8;
        AI_Printf("\nAccess mode = 64 Bit \n");
        AI_Printf("\nStart addr.: %x End addr.: %x Data: %x;",SAddress,EAddress,data);
        AI_Printf("\nEnter Blocksize (<125)");
        SCANF("%x",&block_size);
        if (block_size>=125)
        block_size=124;

        /* fill memory (block_size*8Byte)*/
        temp = block_size*word_size;
        for (j=0;j<=temp;j=j+4)
        {
            *(GLOBAL_VAR->GLOBAL_TEMP_MEM+j) = (AiUInt8) ((data & 0xff000000) >> 24);
            *(GLOBAL_VAR->GLOBAL_TEMP_MEM+j+1)= (AiUInt8) ((data & 0x00ff0000) >> 16);
            *(GLOBAL_VAR->GLOBAL_TEMP_MEM+j+2)= (AiUInt8) ((data & 0x0000ff00) >> 8);
            *(GLOBAL_VAR->GLOBAL_TEMP_MEM+j+3)= (AiUInt8) (data & 0x000000ff);
        }

        /* memory access*/
        AI_Printf("\n");
        for (j=1;j<=loop_cnt;j++)
        {
            AI_Printf("\rloop: %x of %x",j,loop_cnt);

            for(i=SAddress;i<EAddress;i=i+temp)
            {
                temp=block_size*word_size;
                AmcWriteMemData(GLOBAL_VAR->h_Drv, 0, i, word_size, &GLOBAL_VAR->GLOBAL_TEMP_MEM, block_size,GLOBAL_VAR->GLOBAL_DEVICE_TYPE_WR);
            }
        }
        return(PASS);
        break;
        default:
        return(FAIL);
    }

    AI_Printf("\n");
    for (j=1;j<=loop_cnt;j++)
    {
        AI_Printf("\rloop: %x of %x",j,loop_cnt);
        for (i=SAddress;i<EAddress;i=i+word_size)
        {
            PCI_Mem_write_flex(GLOBAL_VAR,i,data,word_size);
            temp=PCI_Mem_read_flex(GLOBAL_VAR,i,word_size);
            if (temp!=data)
            AI_Printf("data read: %x adr %x:\n",temp,j);
        }
    }
    return(PASS);
}

/*****************************************************************************/
/*                                                                           */
/*   Module : ini_prog.c               Submodule : fill_LOCAL_ASPRam         */
/*   Author : Lamp Matthias            Project   : PMC-1553-MON              */
/*---------------------------------------------------------------------------*/
/*   Create : 29/07/2002   Update :   .  .                                   */
/*---------------------------------------------------------------------------*/
/*    Descriptions                                                           */
/*    ------------                                                           */
/*    Inputs    : GLOBAL Struct                                              */
/*                                                                           */
/*    Outputs   : AiUInt32:  PASS or FAIL                                    */
/*                                                                           */
/*    Description :  fills a part of the memory (LOCAL RAM)                  */
/*                                                                           */
/*****************************************************************************/
AiUInt32 fill_LOCAL_ASPRam( TY_MONITOR_DATA_STRUCT *GLOBAL_VAR )
{
    AiUInt32 i;
    AiUInt32 SAddress, EAddress,data;

    kbdflush();
    kbdflush();
    kbdflush();

    AI_Printf("\nEnter Start addr. End addr. and Data (hex) : ");
    SCANF("%lx %lx %lx",&SAddress,&EAddress,&data);

    if(EAddress> (GLOBAL_VAR->RAM_Size_Reg.Reg.Local_ASPRam*1024*1024))
    {
        AI_Printf("\nEND addr, out of the memory range (%08x)",(GLOBAL_VAR->RAM_Size_Reg.Reg.Local_ASPRam*1024*1024));
        return(FAIL);
    }

    for (i=(SAddress/4);i<(EAddress/4);i++) /*Achtung AiUInt32 Zugriffe sind 4 Bytes ueberall !!!*/
    PCI_ASPRam_write(GLOBAL_VAR,i,data);

    return(PASS);
}

#endif

