/* SPDX-FileCopyrightText: 2002-2024 AIM GmbH <info@aim-online.com> */
/* SPDX-License-Identifier: MIT OR GPL-2.0-or-later */

/* #define __monitor__ */

/*#define _SMARTLIB */



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
  #include "MonSpecific.h"
  #include "defs.h"
  #include "type_def.h"
  #include "prototypes.h"
  #include "Wrapper.h"
  #include "SmartLib.h"
#else
  #ifdef _AIM_VMEWINDOWS
    #include <windows.h>
    #define API_OK 0
  #endif
  #ifdef _AIM_VXI
    #include "Monitor/defs.h"
    #include "Monitor/type_def.h"
    #include "Monitor/prototypes.h"
  #else
    #include "defs.h"
    #include "type_def.h"
    #include "prototypes.h"
  #endif

  #if defined (_AIM_VME)
    #if defined (_AIM_FDX)
      #ifdef _AIM_VMEWINDOWS
        _API_EXTERN AiUInt32 ul_FdxDbgLevel;
        #include <stdio.h>
      #endif
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
  #elif defined _AIM_VXI
    #if defined (_AIM_1553)
        #include "ai1553axi.h"
      #include <stdio.h> 
      #include "Monitor/defs.h"
        #include "ai1553_version.h"
      #include <vpptype.h>
      #include <visa.h>
      #include <visa_io.h>
        _API_EXTERN AiUInt32 Aiul_DbgLevel;
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
        #include <visa.h>
    #endif
  #endif
#endif


AiUInt8 auc_Data[MAX_DATA_SIZE];

/*****************************************************************************/
/*                                                                           */
/*   Module : ini_stio.c               Submodule : TcpOut                    */
/*   Author : Matthias Lamp            Project   : API-ASP-MON               */
/*---------------------------------------------------------------------------*/
/*   Create : 09.10.2001   Update :   .  .                                   */
/*---------------------------------------------------------------------------*/
/*    Descriptions                                                           */
/*    ------------                                                           */
/*    Inputs    : port     TCP register address                              */
/*              : TCPData  data value to write to TCP                        */
/*                                                                           */
/*    Outputs   : PASS / FAIL                                                */
/*                                                                           */
/*    Description : Writes data to the TCP and avoids that accesses to the   */
/*                  TCP are faster than recommended for TCP.                 */
/*                                                                           */
/*****************************************************************************/
AiUInt32 TcpOut(TY_MONITOR_DATA_STRUCT *GLOBAL_VAR, AiUInt32 port,
        AiUInt32 TCPData)
{
    AiUInt32 x;

    /* 1: ATMEL Idle? (ATMEL_Rdy=1)*/
    x = 0;
    do /*wait for ATMEL_RDY*/
    {
        wait_clocks(GLOBAL_VAR, 100); /*100us*/
        GLOBAL_VAR->PMC_TCP_DATA_Reg.all = PCI_IOReg_read(GLOBAL_VAR,
                PMC_TCP_DATA_Reg_Adr);

        if( GLOBAL_VAR->PMC_TCP_DATA_Reg.Reg.ATMEL_RDY == 1 )
            break; /*ATMEL_RDY = 1*/
        x++;
    } while( x < 3000 ); /*avoid hanging up TCP (wait for ATMEL_RDY 300ms)*/

    if( GLOBAL_VAR->PMC_TCP_DATA_Reg.Reg.ATMEL_RDY == 0 )
    {
        AI_Printf("\n1 ERROR TCP not ready\n");
        return (FAIL);
    }

    /* 2: send command (register) and 8-bit data to write*/
    GLOBAL_VAR->PMC_TCP_DATA_Reg.Reg.DATA = TCPData; /* data to write*/
    GLOBAL_VAR->PMC_TCP_DATA_Reg.Reg.COMMAND = port; /* commando*/
    GLOBAL_VAR->PMC_TCP_DATA_Reg.Reg.ATMEL_RDxWR = 1; /* write access*/

    PCI_IOReg_write(GLOBAL_VAR, PMC_TCP_DATA_Reg_Adr,
            GLOBAL_VAR->PMC_TCP_DATA_Reg.all);
    wait_clocks(GLOBAL_VAR, 100); /*100us*/

    /* 3: ATMEL Idle? (ATMEL_Rdy=1) optional*/
    x = 0;
    do /*wait for ATMEL_RDY*/
    {
        wait_clocks(GLOBAL_VAR, 100); /*100us*/

        GLOBAL_VAR->PMC_TCP_DATA_Reg.all = PCI_IOReg_read(GLOBAL_VAR,
                PMC_TCP_DATA_Reg_Adr);
        if( GLOBAL_VAR->PMC_TCP_DATA_Reg.Reg.ATMEL_RDY == 1 )
            break; /*ATMEL_RDY = 1*/
        x++;
    } while( x < 3000 ); /*avoid hanging up TCP (wait for ATMEL_RDY 300ms)*/

    if( GLOBAL_VAR->PMC_TCP_DATA_Reg.Reg.ATMEL_RDY == 0 )
    {
        AI_Printf("\n3 ERROR TCP not ready (write to)\n");
        return (FAIL);
    }
    return (PASS);
}

/*****************************************************************************/
/*                                                                           */
/*   Module : ini_stio.c               Submodule : TCPIn                     */
/*   Author : Lamp Matthias            Project   : PMC-1553-MON              */
/*---------------------------------------------------------------------------*/
/*   Create : 09.10.2001   Update :    .  .                                  */
/*---------------------------------------------------------------------------*/
/*    Descriptions                                                           */
/*    ------------                                                           */
/*    Inputs    : port      TCP register address                             */
/*                TCPData   pointer                                          */
/*                                                                           */
/*    Outputs   : PASS / FAIL                                                */
/*                                                                           */
/*    Description : Reads data from the TCP and avoids that accesses to the  */
/*                  TCP are faster than recommended for TCP.                 */
/*                                                                           */
/*****************************************************************************/
AiUInt32 TcpIn(TY_MONITOR_DATA_STRUCT *GLOBAL_VAR, AiUInt32 port,
        AiUInt32 *TCPData)
{
    AiUInt32 x;

    /* 1: ATMEL Idle? (ATMEL_Rdy=1)*/
    x = 0;
    do /* wait for ATMEL_RDY*/
    {
        wait_clocks(GLOBAL_VAR, 100); /*100us*/

        GLOBAL_VAR->PMC_TCP_DATA_Reg.all = PCI_IOReg_read(GLOBAL_VAR,
                PMC_TCP_DATA_Reg_Adr);
        if( GLOBAL_VAR->PMC_TCP_DATA_Reg.Reg.ATMEL_RDY == 1 )
            break; /*ATMEL_RDY = 1*/
        x++;
    } while( x < 3000 ); /* avoid hanging up TCP (wait for ATMEL_RDY 300ms)*/

    if( GLOBAL_VAR->PMC_TCP_DATA_Reg.Reg.ATMEL_RDY == 0 )
    {
        AI_Printf("\n2 ERROR TCP not ready\n");
        return (FAIL);
    }

    /* 2: send read command to PMC_TCP_Reg*/
    GLOBAL_VAR->PMC_TCP_DATA_Reg.Reg.COMMAND = port;/* commando*/
    GLOBAL_VAR->PMC_TCP_DATA_Reg.Reg.ATMEL_RDxWR = 0; /* read access*/

    PCI_IOReg_write(GLOBAL_VAR, PMC_TCP_DATA_Reg_Adr,
            GLOBAL_VAR->PMC_TCP_DATA_Reg.all);

    /* 3: wait for ATMEL ready and read valid data */
    x = 0;
    do /*wait for ATMEL_RDY*/
    {
        wait_clocks(GLOBAL_VAR, 100); /*100us*/

        GLOBAL_VAR->PMC_TCP_DATA_Reg.all = PCI_IOReg_read(GLOBAL_VAR,
                PMC_TCP_DATA_Reg_Adr);
        if( GLOBAL_VAR->PMC_TCP_DATA_Reg.Reg.ATMEL_RDY == 1 )
            break; /*ATMEL_RDY = 1*/
        x++;
    } while( x < 3000 ); /*avoid hanging up TCP (wait for ATMEL_RDY 300ms)*/

    if( GLOBAL_VAR->PMC_TCP_DATA_Reg.Reg.ATMEL_RDY == 0 )
    {
        AI_Printf("\n3 ERROR TCP not ready\n");
        return (FAIL);
    }

    *TCPData = GLOBAL_VAR->PMC_TCP_DATA_Reg.Reg.DATA;
    return (PASS);
}

/*****************************************************************************/
/*                                                                           */
/*   Module : ini_stio.c               Submodule : PCI_Mem_read              */
/*   Author : Lamp Matthias            Project   : PMC-1553-MON              */
/*---------------------------------------------------------------------------*/
/*   Create : 09.10.2001   Update :    .  .                                  */
/*---------------------------------------------------------------------------*/
/*    Descriptions                                                           */
/*    ------------                                                           */
/*    Inputs      : AiUInt32 Mem_loc                                         */
/*                                                                           */
/*    Outputs     : AiUInt32 data                                            */
/*                                                                           */
/*    Description : reads the data from Global Ram, addressed with           */
/*                  GLOBAL_RAM_BAdr+offset (Mem_loc)                         */
/*                                                                           */
/*****************************************************************************/
AiUInt32 PCI_Mem_read(TY_MONITOR_DATA_STRUCT *GLOBAL_VAR, AiUInt32 Mem_loc)
{
    AiUInt32 data;

#ifdef __monitor__
    AmcReadMemData(GLOBAL_VAR->h_Drv, 0, (Mem_loc << 2), 4, auc_Data, 1,
            GLOBAL_VAR->GLOBAL_DEVICE_TYPE_RD); /* 0 = Globa*/
    data = (auc_Data[3] << 24) | (auc_Data[2] << 16) | (auc_Data[1]) << 8
    | (auc_Data[0]);
#endif

#ifndef __monitor__
#ifdef _AIM_VXI
    viMoveIn32(GLOBAL_VAR->INSTRUMENT_HANDLE,(ViUInt16) *(GLOBAL_VAR->GLOBAL_RAM_BAdr),Mem_loc<<2,1,(ViAUInt32) &data);
#elif defined(_SMARTLIB)
    ul_AiReadMemLWord(GLOBAL_VAR->ul_ApiModuleHandle, 0, (Mem_loc<<2), (AiUInt32 *)&data ); /* 3=PCI IO-Register (new)*/
#else
    Api_memcpy(auc_Data,
            ((AiUInt8 *) (GLOBAL_VAR->GLOBAL_RAM_BAdr)) + (Mem_loc << 2), 4); /* GLOBAL RAM*/
    data = (auc_Data[3] << 24) | (auc_Data[2] << 16) | (auc_Data[1]) << 8
            | (auc_Data[0]);
#endif
#endif    

    return (data);
}
/*****************************************************************************/
/*                                                                           */
/*   Module : ini_stio.c               Submodule : PCI_Mem_write             */
/*   Author : Lamp Matthias            Project   : PMC-1553-MON              */
/*---------------------------------------------------------------------------*/
/*   Create : 09.10.2001   Update :    .  .                                  */
/*---------------------------------------------------------------------------*/
/*    Descriptions                                                           */
/*    ------------                                                           */
/*    Inputs      : AiUInt32 Mem_loc, data                                   */
/*                                                                           */
/*    Outputs     : PASS, FAIL                                               */
/*                                                                           */
/*    Description : writes data to RAM (PMC card), addressed with            */
/*                  GLOBAL_RAM_BAdr+offset                                   */
/*                                                                           */
/*****************************************************************************/

AiUInt32 PCI_Mem_write(TY_MONITOR_DATA_STRUCT *GLOBAL_VAR, AiUInt32 Mem_loc,
        AiUInt32 data)
{
    auc_Data[0] = (AiUInt8) ((data & 0x000000ff));
    auc_Data[1] = (AiUInt8) ((data & 0x0000ff00) >> 8);
    auc_Data[2] = (AiUInt8) ((data & 0x00ff0000) >> 16);
    auc_Data[3] = (AiUInt8) ((data & 0xff000000) >> 24);

#ifdef __monitor__
    AmcWriteMemData(GLOBAL_VAR->h_Drv, 0, (Mem_loc << 2), 4, &auc_Data, 1,
            GLOBAL_VAR->GLOBAL_DEVICE_TYPE_WR); /* 0=GLOBAL RAM*/
#endif

#ifndef __monitor__
#ifdef _AIM_VXI
    viMoveOut32(GLOBAL_VAR->INSTRUMENT_HANDLE,(ViUInt16) *(GLOBAL_VAR->GLOBAL_RAM_BAdr),Mem_loc<<2,1,(ViAUInt32) auc_Data);
#elif defined(_SMARTLIB)
    ul_AiWriteMemLWord(GLOBAL_VAR->ul_ApiModuleHandle, 0, (Mem_loc<<2), data ); /* 3=PCI IO-Register (new)*/
#else
    Api_memcpy((AiUInt8 *) (GLOBAL_VAR->GLOBAL_RAM_BAdr) + (Mem_loc << 2),
            auc_Data, 4); /* GLOBAL RAM*/
#endif
#endif
    return (PASS);
}

/*****************************************************************************/
/*                                                                           */
/*   Module : ini_stio.c               Submodule : PCI_IOReg_read            */
/*   Author : Lamp Matthias            Project   : PMC-1553-MON              */
/*---------------------------------------------------------------------------*/
/*   Create : 27.03.2001   Update :    .  .                                  */
/*---------------------------------------------------------------------------*/
/*    Descriptions                                                           */
/*    ------------                                                           */
/*    Inputs      : AiUInt32  IO_Reg                                         */
/*                                                                           */
/*    Outputs     : AiUInt32    data                                         */
/*                                                                           */
/*    Description : reads the data from IO-Register, addressed with          */
/*                  GLOBAL_VAR.PCI_FPGA_Reg_BAdr+offset                      */
/*                                                                           */
/*****************************************************************************/
AiUInt32 PCI_IOReg_read(TY_MONITOR_DATA_STRUCT *GLOBAL_VAR, AiUInt32 IO_Reg)
{
    AiUInt32 data;
#ifdef __monitor__
    AmcReadMemData(GLOBAL_VAR->h_Drv, 3, (IO_Reg << 2), 4, auc_Data, 1,
            GLOBAL_VAR->GLOBAL_DEVICE_TYPE_RD); /* 3=PCI IO-Register (new)*/
#endif

#ifndef __monitor__
#ifdef _AIM_VXI
    viMoveIn32(GLOBAL_VAR->INSTRUMENT_HANDLE,(AiUInt16) *(GLOBAL_VAR->PCI_FPGA_Reg_BAdr),IO_Reg<<2,1,(ViAUInt32) auc_Data);
#elif defined(_SMARTLIB)
    /* AmcReadMemData(GLOBAL_VAR->h_Drv, 3, (IO_Reg<<2), 4, auc_Data, 1,GLOBAL_VAR->GLOBAL_DEVICE_TYPE_RD); */   /* 3=PCI IO-Register (new)*/
    ul_AiReadBlockMemData( GLOBAL_VAR->ul_ApiModuleHandle, 3, (IO_Reg<<2), 1, 4, auc_Data ); /* 3=PCI IO-Register (new)*/
#else
    Api_memcpy(auc_Data,
            (AiUInt8 *) (GLOBAL_VAR->PCI_FPGA_Reg_BAdr) + (IO_Reg << 2), 4); /* PCI IO-Register*/
#endif
#endif

    data = (auc_Data[3] << 24) | (auc_Data[2] << 16) | (auc_Data[1]) << 8
            | (auc_Data[0]);

    return (data);
}

/*****************************************************************************/
/*                                                                           */
/*   Module : ini_stio.c               Submodule : PCI_Mem_write_flex        */
/*   Author : Lamp Matthias            Project   : PMC-1553-MON              */
/*---------------------------------------------------------------------------*/
/*   Create : 09.10.2001   Update :    .  .                                  */
/*---------------------------------------------------------------------------*/
/*    Descriptions                                                           */
/*    ------------                                                           */
/*    Inputs      : AiUInt32 Mem_loc, data,word_size                         */
/*                                                                           */
/*    Outputs     : PASS, FAIL                                               */
/*                                                                           */
/*    Description : writes data to RAM (PMC card), addressed with            */
/*                  GLOBAL_RAM_BAdr+offset, Byte, Word or Long               */
/*                                                                           */
/*****************************************************************************/
AiUInt32 PCI_Mem_write_flex(TY_MONITOR_DATA_STRUCT *GLOBAL_VAR,
        AiUInt32 Mem_loc, AiUInt32 data, AiUInt8 word_size)
{
    auc_Data[0] = (AiUInt8) ((data & 0x000000ff));
    auc_Data[1] = (AiUInt8) ((data & 0x0000ff00) >> 8);
    auc_Data[2] = (AiUInt8) ((data & 0x00ff0000) >> 16);
    auc_Data[3] = (AiUInt8) ((data & 0xff000000) >> 24);

#ifdef __monitor__
    AmcWriteMemData(GLOBAL_VAR->h_Drv, 0, Mem_loc, word_size, &auc_Data, 1,
            GLOBAL_VAR->GLOBAL_DEVICE_TYPE_WR); /* 0=GLOBAL RAM*/
#endif

    return (PASS);
}

/*****************************************************************************/
/*                                                                           */
/*   Module : ini_stio.c               Submodule : PCI_Mem_read_flex         */
/*   Author : Lamp Matthias            Project   : PMC-1553-MON              */
/*---------------------------------------------------------------------------*/
/*   Create : 13.12.2002   Update :    .  .                                  */
/*---------------------------------------------------------------------------*/
/*    Descriptions                                                           */
/*    ------------                                                           */
/*    Inputs      : AiUInt32 Mem_loc                                         */
/*                                                                           */
/*    Outputs     : AiUInt32 data                                            */
/*                                                                           */
/*    Description : reads the data from Global Ram, addressed with           */
/*                  GLOBAL_RAM_BAdr+offset (Mem_loc) BYTE, WORD or LONG      */
/*                                                                           */
/*****************************************************************************/
AiUInt32 PCI_Mem_read_flex(TY_MONITOR_DATA_STRUCT *GLOBAL_VAR, AiUInt32 Mem_loc,
        AiUInt8 word_size)
{
    AiUInt32 data;

#ifdef __monitor__
    AmcReadMemData(GLOBAL_VAR->h_Drv, 0, Mem_loc, word_size, auc_Data, 1,
            GLOBAL_VAR->GLOBAL_DEVICE_TYPE_RD); /* 0 = Global Ram*/
#endif

    data = (auc_Data[3] << 24) | (auc_Data[2] << 16) | (auc_Data[1]) << 8
            | (auc_Data[0]);
    return (data);
}

/*****************************************************************************/
/*                                                                           */
/*   Module : ini_stio.c               Submodule : PCI_IOReg_write           */
/*   Author : Lamp Matthias            Project   : PMC-1553-MON              */
/*---------------------------------------------------------------------------*/
/*   Create : 09.10.2001   Update :    .  .                                  */
/*---------------------------------------------------------------------------*/
/*    Descriptions                                                           */
/*    ------------                                                           */
/*    Inputs      : AiUInt32 IO_Reg, data                                    */
/*                                                                           */
/*    Outputs     : PASS, FAIL                                               */
/*                                                                           */
/*    Description : writes data to IO-Register, addressed with               */
/*                  PMC_IOr_vAddr+offset                                     */
/*                                                                           */
/*****************************************************************************/
AiUInt32 PCI_IOReg_write(TY_MONITOR_DATA_STRUCT *GLOBAL_VAR, AiUInt32 IO_Reg,
        AiUInt32 data)
{
    auc_Data[0] = (AiUInt8) ((data & 0x000000ff));
    auc_Data[1] = (AiUInt8) ((data & 0x0000ff00) >> 8);
    auc_Data[2] = (AiUInt8) ((data & 0x00ff0000) >> 16);
    auc_Data[3] = (AiUInt8) ((data & 0xff000000) >> 24);
#ifdef __monitor__
    AmcWriteMemData(GLOBAL_VAR->h_Drv, 3, (IO_Reg << 2), 4, &auc_Data, 1,
            GLOBAL_VAR->GLOBAL_DEVICE_TYPE_WR); /* 3=PCI IO-Register (new)*/
#endif

#ifndef __monitor__
#ifdef _AIM_VXI
    viMoveOut32(GLOBAL_VAR->INSTRUMENT_HANDLE,(AiUInt16) *(GLOBAL_VAR->PCI_FPGA_Reg_BAdr),IO_Reg<<2,1,(ViAUInt32) auc_Data);
#elif defined(_SMARTLIB)
//  AmcWriteMemData(GLOBAL_VAR->h_Drv, 3, (IO_Reg<<2), 4, &auc_Data, 1,GLOBAL_VAR->GLOBAL_DEVICE_TYPE_WR );        /* 3=PCI IO-Register (new)*/
    ul_AiWriteBlockMemData( GLOBAL_VAR->ul_ApiModuleHandle, 3, (IO_Reg<<2), 1, 4, auc_Data ); /* 3=PCI IO-Register (new)*/
#else
    Api_memcpy((AiUInt8 *) (GLOBAL_VAR->PCI_FPGA_Reg_BAdr) + (IO_Reg << 2),
            auc_Data, 4); /* PCI IO-Register*/
#endif
#endif
    return (PASS);
}

/*****************************************************************************/
/*                                                                           */
/*   Module : ini_stio.c               Submodule : PCI_ASPRam_read           */
/*   Author : Lamp Matthias            Project   : PMC-1553-MON              */
/*---------------------------------------------------------------------------*/
/*   Create : 17.07.2002   Update :    .  .                                  */
/*---------------------------------------------------------------------------*/
/*    Descriptions                                                           */
/*    ------------                                                           */
/*    Inputs      : AiUInt32 Mem_loc                                         */
/*                                                                           */
/*    Outputs     : AiUInt32 data                                            */
/*                                                                           */
/*    Description : reads the data from ASP local Ram, addressed with        */
/*                  GLOBAL_RAM_BAdr+offset (Mem_loc)                         */
/*                  (only on cards with ASP)                                 */
/*                                                                           */
/*****************************************************************************/
AiUInt32 PCI_ASPRam_read(TY_MONITOR_DATA_STRUCT *GLOBAL_VAR, AiUInt32 Mem_loc)
{
    AiUInt32 data;

#ifdef __monitor__
    AmcReadMemData(GLOBAL_VAR->h_Drv, 2, (Mem_loc << 2), 4, auc_Data, 1,
            GLOBAL_VAR->GLOBAL_DEVICE_TYPE_RD); /* 2=local RAM (new)*/
    data = (auc_Data[3] << 24) | (auc_Data[2] << 16) | (auc_Data[1]) << 8
    | (auc_Data[0]);
#endif

#ifndef __monitor__
#ifdef _AIM_VXI
    viMoveIn32(GLOBAL_VAR->INSTRUMENT_HANDLE,(AiUInt16) *(GLOBAL_VAR->ASP_RAM_BAdr),Mem_loc<<2,1,(ViAUInt32) auc_Data);
    data=(auc_Data[3]<<24) | (auc_Data[2]<<16) | (auc_Data[1])<<8 | (auc_Data[0]);
#elif defined(_SMARTLIB)
    ul_AiReadMemLWord(GLOBAL_VAR->ul_ApiModuleHandle, 2, (Mem_loc<<2), (AiUInt32 *)&data ); /* 3=PCI IO-Register (new)*/
#else
    Api_memcpy(auc_Data,
            ((AiUInt8 *) (GLOBAL_VAR->ASP_RAM_BAdr)) + (Mem_loc << 2), 4); /* GLOBAL RAM*/
    data = (auc_Data[3] << 24) | (auc_Data[2] << 16) | (auc_Data[1]) << 8
            | (auc_Data[0]);
#endif
#endif    

    return (data);
}

/*****************************************************************************/
/*                                                                           */
/*   Module : ini_stio.c               Submodule : PCI_ASPRam_write          */
/*   Author : Lamp Matthias            Project   : PMC-1553-MON              */
/*---------------------------------------------------------------------------*/
/*   Create : 17.07.2002   Update :    .  .                                  */
/*---------------------------------------------------------------------------*/
/*    Descriptions                                                           */
/*    ------------                                                           */
/*    Inputs      : AiUInt32 Mem_loc, data                                   */
/*                                                                           */
/*    Outputs     : PASS, FAIL                                               */
/*                                                                           */
/*    Description : writes data to ASP local RAM (PMC card), addressed with  */
/*                  GLOBAL_RAM_BAdr+offset                                   */
/*                  (only on cards with ASP)                                 */
/*                                                                           */
/*****************************************************************************/
AiUInt32 PCI_ASPRam_write(TY_MONITOR_DATA_STRUCT *GLOBAL_VAR, AiUInt32 Mem_loc,
        AiUInt32 data)
{
    auc_Data[0] = (AiUInt8) ((data & 0x000000ff));
    auc_Data[1] = (AiUInt8) ((data & 0x0000ff00) >> 8);
    auc_Data[2] = (AiUInt8) ((data & 0x00ff0000) >> 16);
    auc_Data[3] = (AiUInt8) ((data & 0xff000000) >> 24);

#ifdef __monitor__
    AmcWriteMemData(GLOBAL_VAR->h_Drv, 2, (Mem_loc << 2), 4, &auc_Data, 1,
            GLOBAL_VAR->GLOBAL_DEVICE_TYPE_WR); /*2=local RAM (new)*/
#endif

#ifndef __monitor__
#ifdef _AIM_VXI
    viMoveOut32(GLOBAL_VAR->INSTRUMENT_HANDLE,(AiUInt16) *(GLOBAL_VAR->ASP_RAM_BAdr),Mem_loc<<2,1,(ViAUInt32) &data);
#elif defined(_SMARTLIB)
    ul_AiWriteMemLWord(GLOBAL_VAR->ul_ApiModuleHandle, 2, (Mem_loc<<2), data ); /* 3=PCI IO-Register (new)*/
#else
    Api_memcpy((AiUInt8 *) (GLOBAL_VAR->ASP_RAM_BAdr) + (Mem_loc << 2),
            auc_Data, 4); /* GLOBAL RAM*/
#endif
#endif

    return (PASS);
}

/*****************************************************************************/
/*                                                                           */
/*   Module : ini_prog.c               Submodule : NOVRam_read               */
/*   Author : Matthias Lamp            Project   : PMC-1553-MON              */
/*---------------------------------------------------------------------------*/
/*   Create : 09.10.2001   Update :    .  .                                  */
/*---------------------------------------------------------------------------*/
/*    Descriptions                                                           */
/*    ------------                                                           */
/*    Inputs      : AiUInt32 nvaddr NOV RAM address                          */
/*                                                                           */
/*    Outputs     : AiUInt32 Data from NOV RAM                               */
/*                                                                           */
/*    Description :  Reads a AiUInt32 from the NOV RAM                       */
/*                                                                           */
/*                                                                           */
/*****************************************************************************/
AiUInt32 NOVRam_read(TY_MONITOR_DATA_STRUCT *GLOBAL_VAR, AiUInt32 nvByteAddr)
{
    AiUInt32 i, nvdata, tmpdat;
    nvdata = 0;
    for( i = 0; i < 4; i++ )
    {
        TcpOut(GLOBAL_VAR, TC_ADDRLO, nvByteAddr & 0xFF);
        TcpOut(GLOBAL_VAR, TC_ADDRHI, (nvByteAddr >> 8) & 0x1);

        TcpIn(GLOBAL_VAR, TC_ERDDAT, &tmpdat);
        nvdata = (nvdata >> 8) | ((tmpdat & 0x000000ff) << 24);

        nvByteAddr++;
    }
    return (nvdata);
}

/*****************************************************************************/
/*                                                                           */
/*   Module : ini_prog.c               Submodule : NOVRam_write              */
/*   Author : Hans Zogalla             Project   : PMC-1553-MON              */
/*---------------------------------------------------------------------------*/
/*   Create : 09.10.2001   Update :    .  .                                  */
/*---------------------------------------------------------------------------*/
/*    Descriptions                                                           */
/*    ------------                                                           */
/*    Inputs      : AiUInt32 nvaddr NOV RAM address                          */
/*                : AiUInt32 nvdata Data value to be written to NOV RAM      */
/*                                                                           */
/*    Outputs     : PASS / FAIL                                              */
/*                                                                           */
/*    Description :  Writes a AiUInt32  into the NOV RAM                     */
/*                                                                           */
/*                                                                           */
/*****************************************************************************/
AiUInt32 NOVRam_write(TY_MONITOR_DATA_STRUCT *GLOBAL_VAR, AiUInt32 nvByteAddr,
        AiUInt32 nvdata)
{
    AiUInt32 i, j, addr, data, jtagtmp;

    addr = nvByteAddr;
    data = nvdata;

    TcpOut(GLOBAL_VAR, TC_ECTRL, TC_E_WRENA_B); /* EEProm write enable */

    for( i = 0; i < 4; i++ )
    {
        jtagtmp = 0xffffffff;
        j = 0;

        while( (jtagtmp & TC_E_WRBSY_B) != 0 )
        {
            TcpIn(GLOBAL_VAR, TC_ESTAT, &jtagtmp);
            wait_clocks(GLOBAL_VAR, 100);
            j++;
            if( j >= 250 )
            {
                MON_ONLY_PRINTF("\nERROR !!! NOVRAM write *** 1 ***");
                MON_ONLY_PRINTF("   @ adr. %02x i= %02x\n", nvByteAddr, i);
                return (FAIL);
            }
        }

        TcpOut(GLOBAL_VAR, TC_ADDRLO, nvByteAddr & 0xff);
        TcpOut(GLOBAL_VAR, TC_ADDRHI, (nvByteAddr >> 8) & 0x1);
        TcpOut(GLOBAL_VAR, TC_EWRDAT, nvdata);

        nvdata = nvdata >> 8;
        nvByteAddr++;
    } /*end for*/

    jtagtmp = 0;
    j = 0;

    do
    {
        TcpIn(GLOBAL_VAR, TC_ESTAT, &jtagtmp);
        wait_clocks(GLOBAL_VAR, 100);
        j++;
        if( j >= 250 )
        {
            MON_ONLY_PRINTF("\nERROR !!! NOVRAM write *** 2 ***");
            MON_ONLY_PRINTF("   @ adr. %02x\n", nvByteAddr);
            return (FAIL);
        }
    } while( (jtagtmp & TC_E_WRBSY_B) != 0 ); /*ML*/

    /* EEProm write disable */
    TcpOut(GLOBAL_VAR, TC_ECTRL, 0);

    /* Verify Data */
    jtagtmp = NOVRam_read(GLOBAL_VAR, addr);
    if( jtagtmp == data )
        return (PASS);
    else
    {
        MON_ONLY_PRINTF(
                "\nERROR !!! NOVRAM Adr. %08x data is: %08x data should: %08x ",
                addr, jtagtmp, data);
        MON_ONLY_PRINTF("\n@ adr. %02x", nvByteAddr);
        return (FAIL);
    }
}

/*****************************************************************************/
/*                                                                           */
/*   Module : ini_stio.c               Submodule : NOVRam_chksum             */
/*   Author : Hans Zogalla             Project   : API-ASP-MON               */
/*---------------------------------------------------------------------------*/
/*   Create : 27.02.98   Update :                                            */
/*---------------------------------------------------------------------------*/
/*    Descriptions                                                           */
/*    ------------                                                           */
/*    Inputs    : -                                                          */
/*                                                                           */
/*    Outputs   : .                                                          */
/*                                                                           */
/*    Description :  Calculates the checksum from NVRam loc 0..126           */
/*                                                                           */
/*                                                                           */
/*****************************************************************************/
AiUInt32 NOVRam_chksum(TY_MONITOR_DATA_STRUCT *GLOBAL_VAR)
{
    AiUInt32 i, sum;
    sum = 0;
    for( i = 0; i < 128 - 1; i++ )
    {
        sum += NOVRam_read(GLOBAL_VAR, i * 4); /*128=max NOVRAM AiUInt32 locations*/
    }
    return (sum); /*but not the last location, save pos.*/
}

/*****************************************************************************/
/*                                                                           */
/*   Module : ini_prog.c               Submodule : NOVRam_default            */
/*   Author : Matthias Lamp            Project   : API-ASP-MON               */
/*---------------------------------------------------------------------------*/
/*    Descriptions                                                           */
/*    ------------                                                           */
/*    Inputs    : -                                                          */
/*                                                                           */
/*    Outputs   : .                                                          */
/*                                                                           */
/*    Description :  Sets all NOV RAM controls into default state            */
/*                                                                           */
/*                                                                           */
/*****************************************************************************/
void NOVRam_default(TY_MONITOR_DATA_STRUCT *GLOBAL_VAR)
{
    TcpOut(GLOBAL_VAR, TC_ECTRL, 0); /* EEProm write disable */
}

