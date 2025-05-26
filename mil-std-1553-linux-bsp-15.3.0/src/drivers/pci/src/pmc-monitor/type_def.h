/* SPDX-FileCopyrightText: 2002-2021 AIM GmbH <info@aim-online.com> */
/* SPDX-License-Identifier: MIT OR GPL-2.0-or-later */

#ifndef __INI_TYPE_DEF__
#define __INI_TYPE_DEF__

#ifdef _AIM_SYSDRV
  #ifdef _AIM_WINDOWS
    #include "Ai_cdef.h"
  #endif
  #ifdef _AIM_LINUX
    #include "./Ai_cdef.h"
  #endif
  #ifdef _AIM_QNX
    #include "Ai_cdef.h"
  #endif
  #ifdef __LYNX_OS__
    #include "./Ai_cdef.h"
  #endif
#elif defined __monitor__
  #include "Ai_cdef.h"
#elif defined _SMARTLIB
  #include "Ai_cdef.h"
#else
  #include "../Ai_cdef.h"
#endif


#include "defs.h"

/*******************************************************/
/* Type Definitions for compatibility with other CPU-S */
/*******************************************************/

/* ------ from JTAG Program ------ */
/* the lenVal structure is a byte oriented type used to store an */
/* arbitrary length binary value. As an example, the hex value   */
/* 0x0e3d is represented as a lenVal with len=2 (since 2 bytes   */
/* and val[0]=0e and val[1]=3d.  val[2-MAX_LEN] are undefined    */

/* maximum length (in bytes) of value to read in        */
/* this needs to be at least 4, and longer than the     */
/* length of the longest SDR instruction.  If there is, */
/* only 1 device in the chain, MAX_LEN must be at least */
/* ceil(27/8) == 4.  For 6 devices in a chain, MAX_LEN  */
/* must be 5, for 14 devices MAX_LEN must be 6, for 20  */
/* devices MAX_LEN must be 7, etc..                     */
/* You can safely set MAX_LEN to a smaller number if you*/
/* know how many devices will be in your chain.         */

typedef struct var_len_byte
{
  AiInt16 len;           /* number of chars in this value */
#ifdef __monitor__
  AiUInt8 val[0x2200];   /* bytes of data 6+1 , was 600*/
#elif defined _SMARTLIB
  AiUInt8 val[0x2200];   /* bytes of data 6+1 , was 600*/
#else
  AiUInt8 val[600];      /* bytes of data 6+1 , was 600*/
#endif
} lenVal;

/*******************************************************/
/* Struct Definition, for communication                */
/*******************************************************/
#ifdef __PPC_MVME__
  #ifndef HOST_ENDIAN_BIG
        #define HOST_ENDIAN_BIG
    #endif
#endif

typedef struct ty_monitor_data_struct/* TY_MONITOR_DATA_STRUCT GLOBAL_VAR_Struct*/
{
    unsigned long GLOBAL_DEVICE_TYPE_RD; /* Device Type ARINC, 1553, PANAVIA or FDX */
    unsigned long GLOBAL_DEVICE_TYPE_WR; /* Device Type ARINC, 1553, PANAVIA or FDX */
    unsigned long GLOBAL_DEVICE_TYPE_INTDI; /* Device Type ARINC, 1553, PANAVIA or FDX */
    unsigned long GLOBAL_DEVICE_TYPE_INTEN; /* Device Type ARINC, 1553, PANAVIA or FDX */
    unsigned long GLOBAL_DEVICE_CNT; /* LINUX Drivers */

    AiUInt32 *GLOBAL_RAM_BAdr; /* Base Adr Pointer to Global Ram of AMC*/
    AiUInt32 *PCI_FPGA_Reg_BAdr; /* Base Adr Pointer to PCI FPGA Register*/
    AiUInt32 *ASP_RAM_BAdr; /* Base Adr Pointer to ASP RAM*/
    AiUInt8 uc_ModuleNr;
#ifdef _AIM_VXI
    AiSession INSTRUMENT_HANDLE;
#endif

#ifndef _AIM_LINUX
    void * h_Drv; /* Driver Handle*/
#else
#ifndef __monitor__
    void * h_Drv; /* Driver Handle*/
#else
    int h_Drv; /* Driver Handle*/
#endif
#endif
    AiUInt8 *GLOBAL_TEMP_MEM; /* pointer to global tempeorarry memory*/
    AiUInt32 GLOBAL_TEMP_MEM_SIZE; /* size in BYTE [2MB]*/

    AiUInt32 DoNotBootPMCAsp;

    /*    PMC_RESET_Struct      PMC_RESET_Reg*/
    union pmc_reset_reg
    {
        AiUInt32 all;
        struct
        {
#ifdef HOST_ENDIAN_BIG
            AiUInt32 IRIG_RESx      : 1; /* IRIG control in RESET (0)*/
            AiUInt32                : 15; /* reserved*/
            AiUInt32 SDRAM_start    : 1; /* write one to start SDRAM (GLOBAL and LOCAL) only once v01.01 */
            AiUInt32                : 7; /* reserved*/
            AiUInt32 ASP_RESx       : 1; /* ASP Reset    (v01.01)*/
            AiUInt32                : 1; /* reserved*/
            AiUInt32 IRIG_INTx      : 1; /* IRIG xINTERN / EXTERN*/
            AiUInt32 ATMEL_RESx     : 1; /* Resets the ATMEl (TCP)*/
            AiUInt32 FPGA2_RESx     : 1; /* IO_FPGA 2 Reset*/
            AiUInt32 FPGA1_RESx     : 1; /* IO-FPGA 1 Reset*/
            AiUInt32 BIP2_RESx      : 1; /* BIU2 Prozessor Reset*/
            AiUInt32 BIP1_RESx      : 1; /* BIU1 Prozessor Reset*/
            AiUInt32                : 0; /* alignment     */
#else
            AiUInt32 BIP1_RESx      :1; /* BIU1 Prozessor Reset*/
            AiUInt32 BIP2_RESx      :1; /* BIU2 Prozessor Reset*/
            AiUInt32 FPGA1_RESx     :1; /* IO-FPGA 1 Reset*/
            AiUInt32 FPGA2_RESx     :1; /* IO_FPGA 2 Reset*/
            AiUInt32 ATMEL_RESx     :1; /* Resets the ATMEl (TCP)*/
            AiUInt32 IRIG_INTx      :1; /* IRIG xINTERN / EXTERN*/
            AiUInt32                :1; /* reserved*/
            AiUInt32 ASP_RESx       :1; /* ASP Reset    (v01.01)*/
            AiUInt32                :7; /* reserved*/
            AiUInt32 SDRAM_start    :1; /* write one to start SDRAM (GLOBAL and LOCAL) only once v01.01 */
            AiUInt32                :15; /* reserved*/
            AiUInt32 IRIG_RESx      :1; /* IRIG control in RESET (0)*/
            AiUInt32                :0; /* alignment     */
#endif
        } Reg;
    } PMC_RESET_Reg;

    /*    PMC_FPGA_Struct          PMC_FPGA_Reg          ;    */
    union
    {
        AiUInt32 all;
        struct
        {
#ifdef HOST_ENDIAN_BIG
            AiUInt32            : 17; /* reserved */
            AiUInt32 BOOT_DONE  : 1;
            AiUInt32 BOOT_INITx : 1;
            AiUInt32 BOOT_PROGx : 1;
            AiUInt32            : 1; /* reserved*/
            AiUInt32 BOOT_CLK   : 1; /* clock*/
            AiUInt32 BOOT_CS1x  : 1; /* CS FPGA 2*/
            AiUInt32 BOOT_CS0x  : 1; /* CS FPGA 1*/
            AiUInt32 BOOT_DATA  : 8; /* 8 bit Boot Data*/
            AiUInt32            : 0; /* alignment     */
#else
            AiUInt32 BOOT_DATA  :8; /* 8 bit Boot Data*/
            AiUInt32 BOOT_CS0x  :1; /* CS FPGA 1*/
            AiUInt32 BOOT_CS1x  :1; /* CS FPGA 2*/
            AiUInt32 BOOT_CLK   :1; /* clock*/
            AiUInt32            :1; /* reserved*/
            AiUInt32 BOOT_PROGx :1;
            AiUInt32 BOOT_INITx :1;
            AiUInt32 BOOT_DONE  :1;
            AiUInt32            :17; /* reserved */
            AiUInt32            :0; /* alignment     */
#endif
        } Reg;
    } PMC_FPGA_Reg;
    /* Register to boot the FPGA's    */

    /*    PMC_JTAG_Struct          PMC_JTAG_Reg          ;*/
    union
    {
        AiUInt32 all;
        struct
        {
#ifdef HOST_ENDIAN_BIG
            AiUInt32            : 24; /* reserved*/
            AiUInt32 JTAG_OE    : 1; /* JTAG Programming Interface o/p enable*/
            AiUInt32            : 2; /* reserved*/
            AiUInt32 TRSTx      : 1; /* Test Reset JTAG Signal */
            AiUInt32 TMS        : 1; /* Test Mode Select*/
            AiUInt32 TCK        : 1; /* Test Clock*/
            AiUInt32 TDI        : 1; /* Test Data In  (to first device,  op)*/
            AiUInt32 TDO        : 1; /* Test Data Out (from last device, ip)*/
            AiUInt32            : 0; /* alignment     */
#else
            AiUInt32 TDO        :1; /* Test Data Out (from last device, ip)*/
            AiUInt32 TDI        :1; /* Test Data In  (to first device,  op)*/
            AiUInt32 TCK        :1; /* Test Clock*/
            AiUInt32 TMS        :1; /* Test Mode Select*/
            AiUInt32 TRSTx      :1; /* Test Reset JTAG Signal */
            AiUInt32            :2; /* reserved*/
            AiUInt32 JTAG_OE    :1; /* JTAG Programming Interface o/p enable*/
            AiUInt32            :24; /* reserved*/
            AiUInt32            :0; /* alignment     */
#endif
        } Reg;
    } PMC_JTAG_Reg;
    /* JTAG Register, implementation of the IEEE 1149.1a TAP*/

    AiUInt32 PMC_JTAG_KEY_Reg; /* JTAG KEY to enable 1 JTAG write access*/

    /* PMC_TCP_DATA_Struct      PMC_TCP_DATA_Reg      ;*/
    union
    {
        AiUInt32 all;
        struct
        {
#ifdef HOST_ENDIAN_BIG
            AiUInt32            : 13; /* reserved*/
            AiUInt32 ATMEL_RDxWR: 1; /* Atmel write access (read not)*/
            AiUInt32 ATMEL_CSx  : 1; /* Intterupt Atmel*/
            AiUInt32 ATMEL_RDY  : 1; /* status bit Atmel ready*/
            AiUInt32            : 3; /* reserved*/
            AiUInt32 COMMAND    : 5; /* 5 Bit Command*/
            AiUInt32 DATA       : 8; /* 8 Bit Data*/
            AiUInt32            : 0; /* alignment*/
#else
            AiUInt32 DATA       :8; /* 8 Bit Data*/
            AiUInt32 COMMAND    :5; /* 5 Bit Command*/
            AiUInt32            :3; /* reserved*/
            AiUInt32 ATMEL_RDY  :1; /* status bit Atmel ready*/
            AiUInt32 ATMEL_CSx  :1; /* Intterupt Atmel*/
            AiUInt32 ATMEL_RDxWR:1; /* Atmel write access (read not)*/
            AiUInt32            :13; /* reserved*/
            AiUInt32            :0; /* alignment*/
#endif
        } Reg;
    } PMC_TCP_DATA_Reg;
    /* This register is used to communicated with the TCP*/

    /*PMC_TCP_PROG_Struct      PMC_TCP_PROG_Reg      ;*/
    union
    {
        AiUInt32 all;
        struct
        {
#ifdef HOST_ENDIAN_BIG
            AiUInt32            : 24; /* reserved*/
            AiUInt32 SPI_OE     : 1; /* Serial Programming Output Enable    */
            AiUInt32 SPI_CLK    : 1; /* Serial Clock (CLK)*/
            AiUInt32 SPI_MISO   : 1; /* Master In, Slave Out */
            AiUInt32 SPI_MOSI   : 1; /* Master Out, Slave In*/
            AiUInt32            : 4; /* reserved*/
            AiUInt32            : 0; /* alignment */
#else
            AiUInt32            : 4; /* reserved*/
            AiUInt32 SPI_MOSI   : 1; /* Master Out, Slave In*/
            AiUInt32 SPI_MISO   : 1; /* Master In, Slave Out */
            AiUInt32 SPI_CLK    : 1; /* Serial Clock (CLK)*/
            AiUInt32 SPI_OE     : 1; /* Serial Programming Output Enable    */
            AiUInt32            : 24; /* reserved*/
            AiUInt32            : 0; /* alignment */
#endif
        } Reg;
    } PMC_TCP_PROG_Reg;
    /* This register is used to program the TCP over the SPI interface*/

    /*PMC_TTHIGH_Struct     PMC_TTHIGH_Reg      ; */
    union
    {
        AiUInt32 all;
        struct
        {
#ifdef HOST_ENDIAN_BIG
            AiUInt32            : 6; /* reserved*/
            AiUInt32 day        : 9; /* day of year*/
            AiUInt32 hour       : 5; /* hour of day*/
            AiUInt32 minutes    : 6; /* minutes of hour*/
            AiUInt32 sec        : 6; /* seconds of minute*/
            AiUInt32            : 0; /* alignment     */
#else
            AiUInt32 sec        :6; /* seconds of minute*/
            AiUInt32 minutes    :6; /* minutes of hour*/
            AiUInt32 hour       :5; /* hour of day*/
            AiUInt32 day        :9; /* day of year*/
            AiUInt32            :6; /* reserved*/
            AiUInt32            :0; /* alignment     */
#endif
        } Reg;
    } PMC_TTHIGH_Reg;

    /*PMC_TTLOW_Struct      PMC_TTLOW_Reg          ;*/
    union
    {
        AiUInt32 all;
        struct
        {
#ifdef HOST_ENDIAN_BIG
            AiUInt32 minutes    : 6; /* minutes of hour*/
            AiUInt32 sec        : 6; /* seconds of minute*/
            AiUInt32 usec       : 20; /* us of seconds*/
            AiUInt32            : 0; /* alignment     */
#else
            AiUInt32 usec       :20; /* us of seconds*/
            AiUInt32 sec        :6; /* seconds of minute*/
            AiUInt32 minutes    :6; /* minutes of hour*/
            AiUInt32            :0; /* alignment     */
#endif
        } Reg;
    } PMC_TTLOW_Reg;
    /* The Time Tag Low register contains the lower part of the IRIG Time Tag*/

    /*IRIG Status register*/
    union
    {
        AiUInt32 all;
        struct
        {
#ifdef HOST_ENDIAN_BIG
            AiUInt32    : 0; /* alignment      */
            AiUInt32    : 26; /* reserved       */
            AiUInt32 DS : 1; /* Decoder Status */
            AiUInt32 IS : 1; /* IRIG Status    */
            AiUInt32 S  : 1; /* Sinus          */
            AiUInt32 RP : 1; /* Ref Pos        */
            AiUInt32 RM : 1; /* Ref Mark       */
            AiUInt32 IL : 1; /* IRIG Locked    */
#else
            AiUInt32 IL : 1; /* IRIG Locked    */
            AiUInt32 RM : 1; /* Ref Mark       */
            AiUInt32 RP : 1; /* Ref Pos        */
            AiUInt32 S  : 1; /* Sinus          */
            AiUInt32 IS : 1; /* IRIG Status    */
            AiUInt32 DECSTAT : 1; /* Decoder Status */
            AiUInt32    : 26; /* reserved       */
            AiUInt32    : 0; /* alignment      */
#endif
        } Reg;
    } PMC_IRIGSTATUS_Reg;

    AiUInt32 PMC_COUNTER_Reg;

    /*PMC_IRQ_EVENT_Struct  PMC_IRQ_EVENT_Reg   ;*/
    union
    {
        AiUInt32 all;
        struct
        {
            /* this register is overdesigned.*/
            /* to be compatible with amc1553 and amc-fdx, some IRQ are on two possitions*/
            /* depending on the device ID, */

#ifdef HOST_ENDIAN_BIG
            AiUInt32 enable         : 1; /* PCI Interrupt enable*/
            AiUInt32                : 5; /* reserved*/
            AiUInt32 BIU2C4_host    : 1; /* BIU2 to host IRQ ch.4 */
            AiUInt32 BIU1C2_host    : 1; /* BIU1 to HOSt IRQ ch.2 */
            AiUInt32                : 5; /* reserved*/
            AiUInt32 host_ASP       : 1; /* host to ASP IRQ    (v01.01)*/
            AiUInt32 host_BIU2      : 1; /* host to BIU2*/
            AiUInt32 host_BIU1      : 1; /* host to BIU1 Interrupt*/
            AiUInt32                : 9; /* reserved V02.10 added for 4ch 1553 FPGA*/
            AiUInt32 ASP_host       : 1; /* ASP to host IRQ     (v01.01)*/
            AiUInt32                : 1; /* reserved*/
            AiUInt32 host_BIU2_o    : 1; /* host to BIU2 (old, only AMC1553)*/
            AiUInt32 host_BIU1_o    : 1; /* host to BIU1 Interrupt (old, only AMC1553)*/
            AiUInt32 RT_IRQ         : 1; /* Real Time Interrupt*/
            AiUInt32 BIU2_host      : 1; /* BIU2 to host*/
            AiUInt32 BIU1_host      : 1; /* BIU1 to host Interrupt*/
            AiUInt32                : 0; /* alignment*/
#else
            AiUInt32 BIU1_host      : 1; /* BIU1 to host Interrupt*/
            AiUInt32 BIU2_host      : 1; /* BIU2 to host*/
            AiUInt32 RT_IRQ         : 1; /* Real Time Interrupt*/
            AiUInt32 host_BIU1_o    : 1; /* host to BIU1 Interrupt (old, only AMC1553)*/
            AiUInt32 host_BIU2_o    : 1; /* host to BIU2 (old, only AMC1553)*/
            AiUInt32                : 1; /* reserved*/
            AiUInt32 ASP_host       : 1; /* ASP to host IRQ     (v01.01)*/
            AiUInt32                : 9; /* reserved V02.10 added for 4ch 1553 FPGA*/
            AiUInt32 host_BIU1      : 1; /* host to BIU1 Interrupt*/
            AiUInt32 host_BIU2      : 1; /* host to BIU2*/
            AiUInt32 host_ASP       : 1; /* host to ASP IRQ    (v01.01)*/
            AiUInt32                : 5; /* reserved*/
            AiUInt32 BIU1C2_host    : 1; /* BIU1 to host IRQ ch.2 */
            AiUInt32 BIU2C4_host    : 1; /* BIU2 to HOSt IRQ ch.4 */
            AiUInt32                : 5; /* reserved*/
            AiUInt32 enable         : 1; /* PCI Interrupt enable*/
            AiUInt32                : 0; /* alignment*/
#endif
        } Reg;
    } PMC_IRQ_EVENT_Reg;
    /* Interrupt Event Register*/

    AiUInt32 PMC_IRQ_VEC_Reg;

    /*PMC_IRQ_MASK_Struct      PMC_IRQ_MASK_Reg    ;*/
    union
    {
        AiUInt32 all;
        struct
        {
#ifdef HOST_ENDIAN_BIG
            AiUInt32            : 6; /* reserved*/
            AiUInt32 mask_25    : 1; /* mask bit 9 of IRQ event Reg    V02.10, 04.09.06*/
            AiUInt32 mask_24    : 1; /* mask bit 8 of IRQ event Reg    V02.10*/
            AiUInt32            : 17; /* reserved*/
            AiUInt32 mask_6     : 1; /* mask bit 6 of IRQ event Reg    (v01.01)        */
            AiUInt32            : 3; /* reserved*/
            AiUInt32 mask_2     : 1; /* mask bit 2 of IRQ event Reg*/
            AiUInt32 mask_1     : 1; /* mask bit 1 of IRQ event Reg*/
            AiUInt32 mask_0     : 1; /* mask bit 0 of IRQ event Reg*/
            AiUInt32            : 0; /* alignment*/
#else
            AiUInt32 mask_0     : 1; /* mask bit 0 of IRQ event Reg*/
            AiUInt32 mask_1     : 1; /* mask bit 1 of IRQ event Reg*/
            AiUInt32 mask_2     : 1; /* mask bit 2 of IRQ event Reg*/
            AiUInt32            : 3; /* reserved*/
            AiUInt32 mask_6     : 1; /* mask bit 6 of IRQ event Reg    (v01.01)        */
            AiUInt32            : 17; /* reserved*/
            AiUInt32 mask_24    : 1; /* mask bit 8 of IRQ event Reg    V02.10, 04.09.06*/
            AiUInt32 mask_25    : 1; /* mask bit 9 of IRQ event Reg    V02.10*/
            AiUInt32            : 6; /* reserved*/
            AiUInt32            : 0; /* alignment*/
#endif
        } Reg;
    } PMC_IRQ_MASK_Reg;
    /* Mask the IRQ Event Register*/

    /* PMC_VERSION_Struct      PMC_VERSION_Reg     ;*/
    union
    {
        AiUInt32 all;
        struct
        {
#ifdef HOST_ENDIAN_BIG
            AiUInt32            : 20;/* reserved*/
            AiUInt32 version    : 8; /* PCI FPGA version*/
            AiUInt32 revision   : 4; /* PCI FPGA revision*/
            AiUInt32            : 0; /* alignment*/
#else
            AiUInt32 revision   : 4; /* PCI FPGA revision*/
            AiUInt32 version    : 8; /* PCI FPGA version*/
            AiUInt32            : 20;/* reserved*/
            AiUInt32            : 0; /* alignment*/
#endif
        } Reg;
    } PMC_VERSION_Reg;
    /*--------------------------------------------------------- v02.12*/
    /*ML, IRIG Offset function                                        */
    union
    {
        AiUInt32 all;
        struct
        {
#ifdef HOST_ENDIAN_BIG
            AiUInt32        : 23; /* reserved*/
            AiUInt32 sample : 9; /* sample point*/
            AiUInt32        : 0; /* alignment*/
#else
            AiUInt32 sample : 9; /* sample point*/
            AiUInt32        : 23; /* reserved*/
            AiUInt32        : 0; /* alignment*/
#endif
        } Reg;
    } PMC_IRIG_SAMPLE_Reg;

    /*--------------------------------------------------------- v01.01*/
    union
    {
        AiUInt32 all;
        struct
        {
#ifdef HOST_ENDIAN_BIG
            AiUInt32 : 20; /* reserved*/
            AiUInt32 : 8; /* */
            AiUInt32 : 4; /* */
            AiUInt32 : 0; /* alignment*/
#else
            AiUInt32 : 4; /* */
            AiUInt32 : 8; /* */
            AiUInt32 : 20; /* reserved*/
            AiUInt32 : 0; /* alignment*/
#endif
        } Reg;
    } PMC_MAILBOX_Reg;

    /*--------------------------------------------------------- v01.01    */
    union
    {
        AiUInt32 all;
        struct
        {
#ifdef HOST_ENDIAN_BIG
            AiUInt32            : 15; /* reserved */
            AiUInt32 sentByHost : 1;
            AiUInt32            : 15; /* reserved */
            AiUInt32 sentByASP  : 1;
            AiUInt32            : 0; /* alignment     */
#else
            AiUInt32 sentByASP  : 1;
            AiUInt32            : 15; /* reserved */
            AiUInt32 sentByHost : 1;
            AiUInt32            : 15; /* reserved */
            AiUInt32            : 0; /* alignment     */
#endif
        } Reg;
    } PMC_MBOX_cnt_Reg;

    /*--------------------------------------------------------- v01.01    */
    union
    {
        AiUInt32 all;
        struct
        {
#ifdef HOST_ENDIAN_BIG
            AiUInt32 device_id      : 16; /* device ID (copy of PCI header) */
            AiUInt32 dev_type_BIP2  : 8; /* device type BIU 2 (copy of pci header)*/
            AiUInt32 dev_type_BIP1  : 8; /* device type BIU 1 (copy of pci header)*/
            AiUInt32                : 0; /* alignment*/
#else
            AiUInt32 dev_type_BIP1  : 8; /* device type BIU 1 (copy of pci header)*/
            AiUInt32 dev_type_BIP2  : 8; /* device type BIU 2 (copy of pci header)*/
            AiUInt32 device_id      : 16; /* device ID (copy of PCI header) */
            AiUInt32                : 0; /* alignment*/
#endif
        } Reg;
    } PMC_DEVICEID_Reg;

    /*----------- NOVRAM values*/
    AiUInt32 NVR_MAGIC; /* used as a INIT indicator*/

    union
    {
        AiUInt32 all;
        struct
        {
#ifdef HOST_ENDIAN_BIG
            AiUInt32        : 16; /* reserved*/
            AiUInt32 SERIAL : 16; /* Serial No 0...9998*/
            AiUInt32        : 0;  /* alignment*/
#else
            AiUInt32 SERIAL : 16; /* Serial No 0...9998*/
            AiUInt32        : 16; /* reserved*/
            AiUInt32        : 0;  /* alignment*/
#endif
        } Reg;
    } NVR_BOARD_SERIAL; /* board serial No    */

    union
    {
        AiUInt32 all;
        struct
        {
#ifdef HOST_ENDIAN_BIG
            AiUInt32 MAINB_HWVer    : 16; /* main board HW version*/
            AiUInt32 MAINB_HWRev    : 8; /* main board HW revision*/
            AiUInt32 PLATFORM       : 8; /* platform type*/
            AiUInt32                : 0; /* alignment*/
#else
            AiUInt32 PLATFORM       : 8; /* platform type*/
            AiUInt32 MAINB_HWRev    : 8; /* main board HW revision*/
            AiUInt32 MAINB_HWVer    : 16; /* main board HW version*/
            AiUInt32                : 0; /* alignment*/
#endif
        } Reg;
    } NVR_BOARD_CONFIG; /* board configuration*/
    union
    {
        AiUInt32 all;
        struct
        {
#ifdef HOST_ENDIAN_BIG
            AiUInt32            : 16;/* reserved*/
            AiUInt32 BIU2_type  : 8; /* BIU 2 interface type (0xff=not present)*/
            AiUInt32 BIU1_type  : 8; /* BIU 1 interface type (0xff=not present)*/
            AiUInt32            : 0; /* alignment*/
#else
            AiUInt32 BIU1_type  : 8; /* BIU 1 interface type (0xff=not present)*/
            AiUInt32 BIU2_type  : 8; /* BIU 2 interface type (0xff=not present)*/
            AiUInt32            : 16;/* reserved*/
            AiUInt32             :0; /* alignment*/
#endif
        } Reg;
    } NVR_BOARD_TYPE; /* board type*/

    AiUInt32 NVR_NVRAM_ENABLE; /* enable Code for priveleged operations                    */

    union
    {
        AiUInt32 all;
        struct
        {
#ifdef HOST_ENDIAN_BIG
            AiUInt32 DAY    : 8; /* Board build day   DD*/
            AiUInt32 MONTH  : 8; /* Board build month MM*/
            AiUInt32 YEAR   : 16;/* Board build year  YYYY*/
            AiUInt32        : 0; /* alignment*/
#else
            AiUInt32 YEAR   : 16;/* Board build year  YYYY*/
            AiUInt32 MONTH  : 8; /* Board build month MM*/
            AiUInt32 DAY    : 8; /* Board build day   DD*/
            AiUInt32        : 0; /* alignment*/
#endif
        } Reg;
    } NVR_BOARD_BUILD; /* board build date                   */

    AiUInt32 NVR_NVRAM_CHANGE; /* date of last NOVRAM modification                      */

    union
    {
        AiUInt32 all;
        struct
        {
#ifdef HOST_ENDIAN_BIG
            AiUInt32            : 12; /* reserved*/
            AiUInt32 AIM_PARTNO : 20; /* AIM Product Part Number*/
            AiUInt32            : 0;  /* alignment*/
#else
            AiUInt32 AIM_PARTNO : 20; /* AIM Product Part Number*/
            AiUInt32            : 12; /* reserved*/
            AiUInt32            : 0;  /* alignment*/
#endif
        } Reg;
    } NVR_BOARD_PARTNO; /* board part No               */

    AiUInt32 NVR_SW_KEY; /* software license key            */

    /*NCR_BOOT_EN_Struct      NVR_BOOT_EN_Reg     ;  *//* disable / enable boot functions (FPGA,TCP...)                   */
    union
    {
        AiUInt32 all;
        struct
        {
#ifdef HOST_ENDIAN_BIG
            AiUInt32 lock                   : 1; /* lockk bit, enable / disable changeable of register values */
            AiUInt32                        : 1; /* reserved*/
            AiUInt32                        : 3; /* reserved*/
            AiUInt32 ATMEL_NVR_write_MON    : 1; /* NOVRAM write enable (via MONITOR)*/
            AiUInt32 ATMEL_NVR_write_SPI    : 1; /* NOVRAM write enable (via SPI)*/
            AiUInt32                        : 1; /* reserved*/
            AiUInt32                        : 2; /* not used / reserved*/
            AiUInt32 ATMEL_FLASH_write      : 1; /* write enable bit*/
            AiUInt32 ATMEL_FLASH_read       : 1; /* read enable bit*/
            AiUInt32                        : 2; /* not used / reserved*/
            AiUInt32 PROM_write             : 1; /* write enable bit*/
            AiUInt32 PROM_read              : 1; /* read enable bit*/
            AiUInt32                        : 4; /* reserved*/
            AiUInt32                        : 4; /* reserved*/
            AiUInt32                        : 4; /* reserved*/
            AiUInt32                        : 1; /* reserved*/
            AiUInt32 IO_FPGA_enable         : 1; /* boot enable bit*/
            AiUInt32                        : 2; /* not used*/
            AiUInt32                        : 0; /* alignment    */
#else
            AiUInt32                        : 2; /* not used*/
            AiUInt32 IO_FPGA_enable         : 1; /* boot enable bit*/
            AiUInt32                        : 1; /* reserved*/
            AiUInt32                        : 4; /* reserved*/
            AiUInt32                        : 4; /* reserved*/
            AiUInt32                        : 4; /* reserved*/
            AiUInt32 PROM_read              : 1; /* read enable bit*/
            AiUInt32 PROM_write             : 1; /* write enable bit*/
            AiUInt32                        : 2; /* not used / reserved*/
            AiUInt32 ATMEL_FLASH_read       : 1; /* read enable bit*/
            AiUInt32 ATMEL_FLASH_write      : 1; /* write enable bit*/
            AiUInt32                        : 2; /* not used / reserved*/
            AiUInt32                        : 1; /* reserved*/
            AiUInt32 ATMEL_NVR_write_SPI    : 1; /* NOVRAM write enable (via SPI)*/
            AiUInt32 ATMEL_NVR_write_MON    : 1; /* NOVRAM write enable (via MONITOR)*/
            AiUInt32                        : 3; /* reserved*/
            AiUInt32                        : 1; /* reserved*/
            AiUInt32 lock                   : 1; /* lockk bit, enable / disable changeable of register values */
            AiUInt32                        : 0; /* alignment     */
#endif
        } Reg;
    } NVR_PROG_EN_Reg; /*PMC_TCP_DATAReg;*/

    /* new 11/02/03*/
    union
    {
        AiUInt32 all;
        struct
        {
#ifdef HOST_ENDIAN_BIG
            AiUInt32            : 24; /* reserved*/
            AiUInt32 GRAM_Size  : 8;  /* Global Ram Size*/
            AiUInt32            : 0;  /* alignment*/
#else
            AiUInt32 GRAM_Size  : 8;  /* Global Ram Size*/
            AiUInt32            : 24; /* reserved*/
            AiUInt32            : 0;  /* alignment*/
#endif
        } Reg;
    } NVR_GRAM_SIZE; /* Global Ram Size*/

    union
    {
        AiUInt32 all;
        struct
        {
#ifdef HOST_ENDIAN_BIG
            AiUInt32            : 24; /* reserved*/
            AiUInt32 LRAM_Size  : 8;  /* Local Ram Size*/
            AiUInt32            : 0;  /* alignment*/
#else
            AiUInt32 LRAM_Size  :8;  /* Local Ram Size*/
            AiUInt32            :24; /* reserved*/
            AiUInt32            :0;  /* alignment*/
#endif
        } Reg;
    } NVR_LRAM_SIZE; /* Local Ram Size*/

    AiUInt32 NVR_MAGIC2; /* 2nd save location for NVR_MAGIC*/
    AiUInt32 NVR_CHECKSUM; /* checksum of NOV-RAM                  */

    union
    {
        AiUInt32 all;
        struct
        {
#ifdef HOST_ENDIAN_BIG
            AiUInt32            : 16;
            AiUInt32 SYS_CLK    : 16; /* System Clock [MHz]*/
            AiUInt32            : 0;  /* alignment*/
#else
            AiUInt32 SYS_CLK    : 16; /* System Clock [MHz]*/
            AiUInt32            : 16;
            AiUInt32            : 0;  /* alignment*/
#endif
        } Reg;
    } NVR_SYSCLK;

    union
    {
        AiUInt32 all;
        struct
        {
#ifdef HOST_ENDIAN_BIG
            AiUInt32 CPU2_CLK   : 16; /* BIP 2 / ASP clk [MHz]*/
            AiUInt32 CPU1_CLK   : 16; /* BIP 1 clk [MHz]*/
            AiUInt32            : 0;  /* alignment*/
#else
            AiUInt32 CPU1_CLK   : 16; /* BIP 1 clk [MHz]*/
            AiUInt32 CPU2_CLK   : 16; /* BIP 2 / ASP clk [MHz]*/
            AiUInt32            : 0;  /* alignment*/
#endif
        } Reg;
    } NVR_CPUCLK;

    /* V02.00, 23.02.06 */
    /* NOVRAM values added */
    /* new 11/02/03*/
    /* --------------------- */
    union
    {
        AiUInt32 all;
        struct
        {
#ifdef HOST_ENDIAN_BIG
            AiUInt32        : 16; /* reserved  */
            AiUInt32 PLCA   : 16; /* PLCA Type */
            AiUInt32        : 0;  /* alignment */
#else
            AiUInt32 PLCA   : 16; /* PLCA Type */
            AiUInt32        : 16; /* reserved  */
            AiUInt32        : 0;  /* alignment */
#endif
        } Reg;
    } NVR_PLCAEX; /* PLCA Type */

    /* --------------------- */
    union
    {
        AiUInt32 all;
        struct
        {
#ifdef HOST_ENDIAN_BIG
            AiUInt32        : 16; /* reserved */
            AiUInt32 TCP    : 16; /* TCP Type */
            AiUInt32        : 0;  /* alignment*/
#else
            AiUInt32 TCP    : 16; /* TCP Type */
            AiUInt32        : 16; /* reserved */
            AiUInt32        : 0;  /* alignment*/
#endif
        } Reg;
    } NVR_TCPEX; /* TCP Type */

    /* --------------------- */
    union
    {
        AiUInt32 all;
        struct
        {
#ifdef HOST_ENDIAN_BIG
            AiUInt32        : 16; /* reserved    */
            AiUInt32 ILCA1  : 16; /* ILCA-1 Type */
            AiUInt32        : 0;  /* alignment   */
#else
            AiUInt32 ILCA1  : 16; /* ILCA-1 Type */
            AiUInt32        : 16; /* reserved    */
            AiUInt32        : 0;  /* alignment   */
#endif
        } Reg;
    } NVR_ILCA1EX; /* IO LCA 1 Type */

    /* --------------------- */
    union
    {
        AiUInt32 all;
        struct
        {
#ifdef HOST_ENDIAN_BIG
            AiUInt32        : 16; /* reserved    */
            AiUInt32 ILCA2  : 16; /* ILCA-2 Type */
            AiUInt32        : 0;  /* alignment   */
#else
            AiUInt32 ILCA2  : 16; /* ILCA-2 Type */
            AiUInt32        : 16; /* reserved    */
            AiUInt32        : 0;  /* alignment   */
#endif
        } Reg;
    } NVR_ILCA2EX; /* IO LCA 2 Type */

    /* --------------------- */
    union
    {
        AiUInt32 all;
        struct
        {
#ifdef HOST_ENDIAN_BIG
            AiUInt32        : 16; /* reserved    */
            AiUInt32 ILCA3  : 16; /* ILCA-3 Type */
            AiUInt32        : 0;  /* alignment   */
#else
            AiUInt32 ILCA3  : 16; /* ILCA-3 Type */
            AiUInt32        : 16; /* reserved    */
            AiUInt32        : 0;  /* alignment   */
#endif
        } Reg;
    } NVR_ILCA3EX; /* IO LCA 3 Type */

    /* --------------------- */
    union
    {
        AiUInt32 all;
        struct
        {
#ifdef HOST_ENDIAN_BIG
            AiUInt32        : 16; /* reserved    */
            AiUInt32 ILCA4  : 16; /* ILCA-4 Type */
            AiUInt32        : 0;  /* alignment   */
#else
            AiUInt32 ILCA4  : 16; /* ILCA-4 Type */
            AiUInt32        : 16; /* reserved    */
            AiUInt32        : 0;  /* alignment   */
#endif
        } Reg;
    } NVR_ILCA4EX; /* IO LCA 4 Type */

    /* --------------------- */
    union
    {
        AiUInt32 all;
        struct
        {
#ifdef HOST_ENDIAN_BIG
            AiUInt32        : 16; /* reserved    */
            AiUInt32 BIP1   : 16; /* BIP-1 Type */
            AiUInt32        : 0;  /* alignment   */
#else
            AiUInt32 BIP1   : 16; /* BIP-1 Type */
            AiUInt32        : 16; /* reserved    */
            AiUInt32        : 0;  /* alignment   */
#endif
        } Reg;
    } NVR_BIP1EX; /* BIP 1 Type */

    /* --------------------- */
    union
    {
        AiUInt32 all;
        struct
        {
#ifdef HOST_ENDIAN_BIG
            AiUInt32        : 16; /* reserved    */
            AiUInt32 BIP2   : 16; /* BIP-2 Type */
            AiUInt32        : 0;  /* alignment   */
#else
            AiUInt32 BIP2   : 16; /* BIP-2 Type */
            AiUInt32        : 16; /* reserved    */
            AiUInt32        : 0;  /* alignment   */
#endif
        } Reg;
    } NVR_BIP2EX; /* BIP 2 Type */

    /* --------------------- */
    union
    {
        AiUInt32 all;
        struct
        {
#ifdef HOST_ENDIAN_BIG
            AiUInt32        : 16; /* reserved    */
            AiUInt32 BIP3   : 16; /* BIP-3 Type */
            AiUInt32        : 0;  /* alignment   */
#else
            AiUInt32 BIP3   : 16; /* BIP-3 Type */
            AiUInt32        : 16; /* reserved    */
            AiUInt32        : 0;  /* alignment   */
#endif
        } Reg;
    } NVR_BIP3EX; /* BIP 3 Type */

    /* --------------------- */
    union
    {
        AiUInt32 all;
        struct
        {
#ifdef HOST_ENDIAN_BIG
            AiUInt32        : 16; /* reserved    */
            AiUInt32 BIP4   : 16; /* BIP-4 Type */
            AiUInt32        : 0;  /* alignment   */
#else
            AiUInt32 BIP4   : 16; /* BIP-4 Type */
            AiUInt32        : 16; /* reserved    */
            AiUInt32        : 0;  /* alignment   */
#endif
        } Reg;
    } NVR_BIP4EX; /* BIP 4 Type */

    /* --------------------- */
    union
    {
        AiUInt32 all;
        struct
        {
#ifdef HOST_ENDIAN_BIG
            AiUInt32        : 16; /* reserved    */
            AiUInt32 ASP    : 16; /* ASP Type */
            AiUInt32        : 0;  /* alignment   */
#else
            AiUInt32 ASP    : 16; /* ASP Type */
            AiUInt32        : 16; /* reserved    */
            AiUInt32        : 0;  /* alignment   */
#endif
        } Reg;
    } NVR_ASPEX; /* ASP Type */

    /*--------------------------------------------------------- v02.12*/
    /*ML, IRIG Offset function                                        */
    union
    {
        AiUInt32 all;
        struct
        {
#ifdef HOST_ENDIAN_BIG
            AiUInt32        : 24;/* reserved */
            AiUInt32 sign   : 1; /* sign bit */
            AiUInt32 offset : 7; /* offset   */
            AiUInt32        : 0; /* alignment*/
#else
            AiUInt32 offset : 7; /* offset   */
            AiUInt32 sign   : 1; /* sign bit */
            AiUInt32        : 24;/* reserved */
            AiUInt32        : 0; /* alignment*/
#endif
        } Reg;
    } NVR_IRIG_OFFSET;

    /*ML, Board Sub Type*/
    union
    {
        AiUInt32 all;
        struct
        {
#ifdef HOST_ENDIAN_BIG
            AiUInt32 biu4 : 8; /* BIU4 Sub-Type */
            AiUInt32 biu3 : 8; /* BIU3 Sub-Type */
            AiUInt32 biu2 : 8; /* BIU2 Sub-Type */
            AiUInt32 biu1 : 8; /* BIU1 Sub-Type */
            AiUInt32      : 0; /* alignment*/
#else
            AiUInt32 biu1 :8; /* BIU1 Sub-Type */
            AiUInt32 biu2 :8; /* BIU2 Sub-Type */
            AiUInt32 biu3 :8; /* BIU3 Sub-Type */
            AiUInt32 biu4 :8; /* BIU4 Sub-Type */
            AiUInt32      :0; /* alignment*/
#endif
        } Reg;
    } NVR_BOARD_ST; /*V02.12, 21.03.07*/

    /*ML, Hardware Variant Word*/
    union
    {
        AiUInt32 all;
        struct
        {
#ifdef HOST_ENDIAN_BIG
            AiUInt32        : 24; /* reserved */
            AiUInt32 v1     : 1; /* Time Synchronisation Valid */
            AiUInt32        : 3; /* reserved */
            AiUInt32 mod    : 1; /* Time Synchronisation Mode */
            AiUInt32 en     : 1; /* Time Synchronisation Encoder Circuitry */
            AiUInt32 de     : 1; /* Time Synchronisation Decoder Circuitry  */
            AiUInt32 is     : 1; /* Physical IRIG- Intern/Extern Switch available */
            AiUInt32        : 0; /* alignment*/
#else
            AiUInt32 is     : 1; /* Physical IRIG- Intern/Extern Switch available */
            AiUInt32 de     : 1; /* Time Synchronisation Decoder Circuitry*/
            AiUInt32 en     : 1; /* Time Synchronisation Encoder Circuitry */
            AiUInt32 mod    : 1; /* Time Synchronisation Mode */
            AiUInt32        : 3; /* reserved   */
            AiUInt32 v1     : 1; /* Time Synchronisation Valid */
            AiUInt32        : 24; /* reserved */
            AiUInt32        : 0; /* alignment*/
#endif
        } Reg;
    } NVR_HW_VAR; /*V02.12, 21.03.07*/

    /*ML, NVR Board Part Number Extension Word Definition*/
    AiUInt32 NVR_PART_EXT; /* V02.13 15.10.07*/

    /*ML, NVR PCI Header Word Definition*/
    union
    {
        AiUInt32 all;
        struct
        {
#ifdef HOST_ENDIAN_BIG
            AiUInt32        : 16; /* reserved */
            AiUInt32 fixed  : 2; /* fixed value b"11" */
            AiUInt32        : 1; /* reserved */
            AiUInt32 tr     : 1; /* tranceiver type */
            AiUInt32 prot   : 4; /* protocol */
            AiUInt32 biu34  : 4; /* BIU 3/4 modes */
            AiUInt32 biu12  : 4; /* BIu 1/2 modes */
            AiUInt32        : 0; /* alignment*/
#else
            AiUInt32 biu12  : 4; /* BIu 1/2 modes */
            AiUInt32 biu34  : 4; /* BIU 3/4 modes */
            AiUInt32 prot   : 4; /* protocol */
            AiUInt32 tr     : 1; /* tranceiver type */
            AiUInt32        : 1; /* reserved */
            AiUInt32 fixed  : 2; /* fixed value 11 */
            AiUInt32        : 16; /* reserved */
            AiUInt32        : 0; /* alignment*/
#endif
        } Reg;
    } NVR_PCI_HDR; /*V02.13, 15.10.07*/

    /*----------- MISC values*/
    union /* used for the boot status*/
    {
        AiUInt32 all;
        struct
        {
#ifdef HOST_ENDIAN_BIG
            AiUInt32 PMC_BOOT_OK    : 1; /* PMC successfully booted*/
            AiUInt32 ASP_start      : 1; /* ASP start OK*/
            AiUInt32 ASP_boot       : 1; /* ASP boot OK*/
            AiUInt32                : 1;
            AiUInt32 BIP_2_start    : 1; /* BIU 2 Processor start OK*/
            AiUInt32 BIP_2_boot     : 1; /* BIU 2 Processor boot OK*/
            AiUInt32                : 1;
            AiUInt32 BIP_1_start    : 1; /* BIU 1 Processor start OK*/
            AiUInt32 BIP_1_boot     : 1; /* BIU 1 Processor boot OK*/
            AiUInt32                : 2;
            AiUInt32 FPGA           : 1; /* IO-FPGA 1 and 2 boot OK*/
            AiUInt32                : 1;
            AiUInt32 BOOT_ASP       : 1; /* ASP peresent    */
            AiUInt32 BOOT_BIU2      : 1; /* BIU2 present*/
            AiUInt32                : 2;
            AiUInt32 MEM_Test       : 1; /* RAM Test pased*/
            AiUInt32                : 9;
            AiUInt32 NOVRam_Reg     : 1; /* NOVRam Register valid*/
            AiUInt32                : 1;
            AiUInt32 ATMEL_OK       : 1; /* ATMEL valid (first RDY)*/
            AiUInt32                : 1;
            AiUInt32 PCI_REG        : 1; /* PCI Regisster valid*/
            AiUInt32                : 0; /* alignment     */
#else
            AiUInt32 PCI_REG        : 1; /* PCI Regisster valid*/
            AiUInt32                : 1;
            AiUInt32 ATMEL_OK       : 1; /* ATMEL valid (first RDY)*/
            AiUInt32                : 1;
            AiUInt32 NOVRam_Reg     : 1; /* NOVRam Register valid*/
            AiUInt32                : 9;
            AiUInt32 MEM_Test       : 1; /* RAM Test pased*/
            AiUInt32                : 2;
            AiUInt32 BOOT_BIU2      : 1; /* BIU2 present*/
            AiUInt32 BOOT_ASP       : 1; /* ASP peresent    */
            AiUInt32                : 1;
            AiUInt32 FPGA           : 1; /* IO-FPGA 1 and 2 boot OK*/
            AiUInt32                : 2;
            AiUInt32 BIP_1_boot     : 1; /* BIU 1 Processor boot OK*/
            AiUInt32 BIP_1_start    : 1; /* BIU 1 Processor start OK*/
            AiUInt32                : 1;
            AiUInt32 BIP_2_boot     : 1; /* BIU 2 Processor boot OK*/
            AiUInt32 BIP_2_start    : 1; /* BIU 2 Processor start OK*/
            AiUInt32                : 1;
            AiUInt32 ASP_boot       : 1; /* ASP boot OK*/
            AiUInt32 ASP_start      : 1; /* ASP start OK*/
            AiUInt32 PMC_BOOT_OK    : 1; /* PMC successfully booted*/
            AiUInt32                : 0; /* alignment     */
#endif
        } Reg;
    } BOOT_CONTROL_Reg;

    union /* v01.01, RAM Sizes*/
    {
        AiUInt32 all;
        struct
        {
#ifdef HOST_ENDIAN_BIG
            AiUInt32 Local_ASPRam   : 8; /* Local ASP RAM (SDRAM) (only by Modules with ASP)*/
            AiUInt32                : 8;
            AiUInt32 GRam_Size      : 8; /* Global Ram Size (SDRAM+SSRAM) (by Modules without ASP RAM only SSRAM)*/
            AiUInt32                : 8;
            AiUInt32                : 0; /* alignment     */
#else
            AiUInt32                : 8;
            AiUInt32 GRam_Size      : 8; /* Global Ram Size (SDRAM+SSRAM) (by Modules without ASP RAM only SSRAM)*/
            AiUInt32                : 8;
            AiUInt32 Local_ASPRam   : 8; /* Local ASP RAM (SDRAM) (only by Modules with ASP)*/
            AiUInt32                : 0; /* alignment     */
#endif
        } Reg;
    } RAM_Size_Reg;

    union
    {
        AiUInt32 all;
        struct
        {
#ifdef HOST_ENDIAN_BIG
            AiUInt32            : 24;/* reserved*/
            AiUInt32 VERSION    : 8; /* AIM Product Part Number*/
            AiUInt32            : 0; /* alignment*/
#else
            AiUInt32 VERSION    : 8; /* AIM Product Part Number*/
            AiUInt32            : 24;/* reserved*/
            AiUInt32            : 0; /* alignment*/
#endif
        } Reg;
    } ATMEL_VER; /* board part No    */

    AiUInt32 clk_1us;

    /* jtag */

    lenVal TDOMask; /* last TDOMask received */
    lenVal maxRepeat; /* max times tdo can fail before ISP considered failed */
    lenVal runTestTimes; /* value of last XRUNTEST instruction */

    /* v01.08, 22/09/2004 */
    union
    {
        AiUInt32 all;
        struct
        {
#ifdef HOST_ENDIAN_BIG
            AiUInt32            : 31; /* reserved*/
            AiUInt32 BATCH_MODE : 1;  /* Batch Mode*/
            AiUInt32            : 0;  /* alignment*/
#else
            AiUInt32 BATCH_MODE : 1;  /* Batch Mode*/
            AiUInt32            : 31; /* reserved */
            AiUInt32            : 0;  /* alignment*/
#endif
        } Reg;
    } STATUS_FLAGS;
    /* v01.08 end */
    /*ML 29/07/2005 Aenderung*/

    struct
    {
        AiUInt32 argv1;
        AiUInt32 argv2;
        AiUInt32 argv3;
        AiUInt32 argc;
		AiUInt32 NetChipVersion;
    } STATUS;

    /*v02.13, 16.10.07            */
    /*optional path for srec files*/
    AiInt8 path[MON_MAX_PATH];
    AiInt8 temp_path[MON_MAX_PATH];

#ifdef _SMARTLIB
    AiUInt32 ul_ApiModuleHandle;
    AiUInt32 ul_MemLengthTcpUpdate;
    AiUInt32 ul_NovRamBackupMemSize;
#endif

    /* ML, V02.22 JTAG update NET228x */
    AiUInt32 net228x_sys;

} TY_MONITOR_DATA_STRUCT;

/*******************************************************/
/* other structs                                       */
/*******************************************************/

#ifdef __monitor__
/* ------ from JTAG Program ------ */
/* the lenVal structure is a byte oriented type used to store an */
/* arbitrary length binary value. As an example, the hex value   */
/* 0x0e3d is represented as a lenVal with len=2 (since 2 bytes   */
/* and val[0]=0e and val[1]=3d.  val[2-MAX_LEN] are undefined    */

/* maximum length (in bytes) of value to read in        */
/* this needs to be at least 4, and longer than the     */
/* length of the longest SDR instruction.  If there is, */
/* only 1 device in the chain, MAX_LEN must be at least */
/* ceil(27/8) == 4.  For 6 devices in a chain, MAX_LEN  */
/* must be 5, for 14 devices MAX_LEN must be 6, for 20  */
/* devices MAX_LEN must be 7, etc..                     */
/* You can safely set MAX_LEN to a smaller number if you*/
/* know how many devices will be in your chain.         */

/*typedef struct var_len_byte*/
/*{*/
/*    AiInt16     len;       *//* number of chars in this value */
/*    AiUInt8      val[1000]; *//* bytes of data *//*6+1*/
/*} lenVal;*/
/* put into global struct*/

extern AiUInt32 DumpUpdate;

#endif

/* ****************************************************
 * NET2282 Command Struct                           *
 * Read with NET2282_init() to get all necessary    *
 * variables for correct communication              *
 * with USB devices (APU, AFDX-TAP)                 *
 **************************************************** */

typedef struct
{
    AiUInt8 status;             /* 0          8051 */
    AiUInt8 command_count;      /* 1          8051 */
    AiUInt8 livecounter;        /* 2          8051 */
    AiUInt8 failure;            /* 3          8051 */
    AiUInt8 debug1;             /* 4          8051 */
    AiUInt8 debug2;             /* 5          8051 */
    AiUInt8 debug3;             /* 6          8051 */
    AiUInt8 debug4;             /* 7          8051 */
    AiUInt16 byte_completed1;   /* 8-9        8051 */
    AiUInt16 byte_completed2;   /* 10-11      8051 */
    AiUInt16 pointer1;          /* 12-13      8051 */
    AiUInt16 size1;             /* 14-15      8051 */
    AiUInt16 pointer2;          /* 16-17      8051 */
    AiUInt16 size2;             /* 18-19      8051 */
    AiUInt32 bar0;              /* 20-23      8051 */
    AiUInt32 bar1;              /* 24-27      8051 */
    AiUInt32 idsel;             /* 28-31      8051 */
    AiUInt32 netchip_id;        /* 32-35      8051 */
    AiUInt8 reserved1[25];      /* 36-60      8051 */
    AiUInt8 version;            /* 61         8051 */
    AiUInt8 revision;           /* 62         8051 */
    AiUInt8 build;              /* 63         8051 */
    AiUInt16 bytecount1;        /* 64-65        PC */
    AiUInt16 bytecount2;        /* 66-67        PC */
    AiUInt16 startpage1;        /* 68-69        PC */
    AiUInt16 endpage1;          /* 70-71        PC */
    AiUInt16 startpage2;        /* 72-73        PC */
    AiUInt16 endpage2;          /* 74-75        PC */
    AiUInt8 reserved2[4];       /* 76-79        PC */
    AiUInt8 command;            /* 80           PC */
    AiUInt8 reserved3[3];       /* 81-83        PC */
    AiUInt32 XRUNTEST_value;    /* stored value of XRUNTEST   84-87 */
    AiUInt32 XSDRSIZE_value;    /* stored value of XSDRSIZE   88-91 */
    AiUInt32 magic;             /*                            92-95 */
    AiUInt8 XREPEAT_value;      /*                            96    */
    AiUInt8 reserved4[31];      /*                            97-127 */
} NET2282_TY;

typedef struct
{
    AiUInt16 bytecount1;        /* 64-65        PC */
    AiUInt16 bytecount2;        /* 66-67        PC */
    AiUInt16 startpage1;        /* 68-69        PC */
    AiUInt16 endpage1;          /* 70-71        PC */
    AiUInt16 startpage2;        /* 72-73        PC */
    AiUInt16 endpage2;          /* 74-75        PC */
    AiUInt8 reserved2[4];       /* 76-79        PC */
    AiUInt8 command;            /* 80           PC */
    AiUInt8 reserved3[3];       /* 81-83        PC */
    AiUInt32 XRUNTEST_value;    /* stored value of XRUNTEST   84-87 */
    AiUInt32 XSDRSIZE_value;    /* stored value of XSDRSIZE   88-91 */
    AiUInt32 magic;             /*                            92-95 */
    AiUInt8 XREPEAT_value;      /*                            96    */
    AiUInt8 reserved4[31];      /*                            97-127 */
} NET2282_COM_TY;

#endif /*if not defined __INI_TYPE_DEF__*/

/*

 von der Software benoetigte Variablen, und Ihre Position im S>TRUCT:

 selftest_result    = GLOBAL_VAR->GLOBAL_VAR->BOOT_CONTROL_Reg.PMC_BOOT_OK  (1bit)
 biu1_present        = GLOBAL_VAR->BOOT_CONTROL_Reg.BIP_1_start                (1bit)
 biu2_present        = GLOBAL_VAR->BOOT_CONTROL_Reg.BIP_2_start                (1bit)
 clk_1us            = GLOBAL_VAR->clk_1us
 BoardSerial        = GLOBAL_VAR->NVR_BOARD_SERIAL.SERIAL                    (16bit)
 BoardConfig        = GLOBAL_VAR->NVR_BOARD_CONFIG.PLATFORM                    (8bit)
 NVR_BOARD_CONFIG.MAINB_HWRev                (8bit)
 NVR_BOARD_CONFIG.MAINB_HWVer                (16bit)
 BoardType            = GLOBAL_VAR->NVR_BOARD_TYPE.BIU1_type                    (8bit)
 NVR_BOARD_TYPE.BIU2_type                    (8bit)
 BoardPartNo        = GLOBAL_VAR->NVR_BOARD_PARTNO.AIM_PARTNO                (16bit)
 CPUClk            = GLOBAL_VAR->NVR_CPUCLK.CPU1_CLK                        (16bit)
 ( GLOBAL_VAR->NVR_CPUCLK.CPU1_CLK fuer BIP2 or the ASP) (16bit)
 SYSClk            = GLOBAL_VAR->NVR_SYSCLK.SYS_CLK                        (16bit)

 */

