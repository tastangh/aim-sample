/* SPDX-FileCopyrightText: 2002-2021 AIM GmbH <info@aim-online.com> */
/* SPDX-License-Identifier: MIT OR GPL-2.0-or-later */

#ifndef __INI_DEFS__
#define __INI_DEFS__

#if defined(__VXWORKS__)
  #include "taskLib.h"
#endif

#define MON_MAX_PATH 256

/* PCI FPGA REGISTER ADDRESSES*/
#define    PMC_RESET_Reg_Adr       0x00
#define    PMC_FPGA_Reg_Adr        0x20
#define    PMC_JTAG_Reg_Adr        0x30
#define    PMC_JTAG_KEY_Reg_Adr    0x31
#define    PMC_TCP_DATA_Reg_Adr    0x40
#define    PMC_TCP_PROG_Reg_Adr    0x41
#define    PMC_TTHIGH_Reg_Adr      0x50
#define    PMC_TTLOW_Reg_Adr       0x51
#define    PMC_COUNTER_Reg_Adr     0x52 /*0x148*/
#define    PMC_IRIG_SAMPLE_Reg_Adr 0x53 /*V02.12 added*/
#define    PMC_IRIGSTATUS_Adr      0x54 /*V02.20 added*/
#define    PMC_IRQ_EVENT_Reg_Adr   0x60
#define    PMC_IRQ_VEC_Reg_Adr     0x61
#define    PMC_IRQ_MASK_Reg_Adr    0x62
#define    PMC_VERSION_Reg_Adr     0x80
#define    PMC_MAILBOX_Reg_Adr     0x10
#define    PMC_MBOX_cnt_Reg_Adr    0x11
#define    PMC_DEVICEID_Reg_Adr    0x01

/* NOVRAM REGISTER ADDRESSES*/
#define    NVR_MAGIC_Adr           0x000
#define    NVR_BOARD_SERIAL_Adr    0x00c
#define    NVR_BOARD_CONFIG_Adr    0x028
#define    NVR_BOARD_TYPE_Adr      0x02c
#define    NVR_NVRAM_ENABLE_Adr    0x030
#define    NVR_BOARD_BUILD_Adr     0x03c
#define    NVR_NVRAM_CHANGE_Adr    0x040
#define    NVR_BOARD_PARTNO_Adr    0x048
#define    NVR_SYSCLK_Adr          0x04c
#define    NVR_CPUCLK_Adr          0x050
#define    NVR_SW_KEY_Adr          0x0b0
#define    NVR_BOOT_PROG_Adr       0x0b8
#define    NVR_GRAM_Size_Adr       0x0bc 
#define    NVR_LRAM_Size_Adr       0x0c0
#define    NVR_PLCAEX_A            0x054    /* Type of PCI LCA srec. file */
#define    NVR_TCPEX_A             0x058    /* Type of TCP srec. file */
#define    NVR_ILCA1EX_A           0x060    /* Type of IO-LCA1 srec. file */
#define    NVR_ILCA2EX_A           0x064    /* Type of IO-LCA2 srec. file */
#define    NVR_ILCA3EX_A           0x068    /* Type of IO-LCA3 srec. file */
#define    NVR_ILCA4EX_A           0x06c    /* Type of IO-LCA4 srec. file */
#define    NVR_BIP1EX_A            0x070    /* Type of BIP1 srec. file */
#define    NVR_BIP2EX_A            0x074    /* Type of BIP2 srec. file */
#define    NVR_BIP3EX_A            0x078    /* Type of BIP3 srec. file */
#define    NVR_BIP4EX_A            0x07c    /* Type of BIP4 srec. file */
#define    NVR_ASPEX_A             0x080    /* Type of ASP srec. file */
#define    NVR_IRIG_OFFSET_Adr     0x0c4    /* V02.12, IRIG Offset Register address */
#define    NVR_BOARD_ST_Adr        0xc8      /*V02.12, 21.03.07*/
#define    NVR_HW_VAR_Adr          0xcc      /*V02.12, 21.03.07*/
#define    NVR_PART_EXT_Adr        0x5c      /*V02.13, 15.10.07*/
#define    NVR_PCI_HDR_Adr         0x98      /*V02.13, 15.10.07*/
#define    NVR_MAGIC2_Adr          0x1f8
#define    NVR_CHECKSUM_Adr        0x1fc

#ifndef TRUE
   #define    TRUE    1
#endif

#ifndef PASS
   #define    PASS    1
#endif

#ifndef FALSE
   #define    FALSE    0
#endif

#ifndef FAIL
   #define    FAIL    0
#endif

#ifndef ZERO 
  #define   ZERO    0
#endif

#define  DISABEL  2

/* some ASCII definitions*/
#define BEL             0x07
#define BS              0x08
#define TAB             0x09
#define LF              0x0a
#define UP              0x0b
#define CLS             0x0c
#define CR              0x0d
#define XON             0x11
#define XOFF            0x13
#define CTRL_X          0x18
#define ESC             0x1b
#ifndef SPACE
  #define SPACE           0x20
#endif
#define DEL             0x7f

/* misc definitions */
/*#define MON_VERSION     0x0100    */
/*#define MON_VERSION     0x0102     Monitor Version V01.01 */
                           /* ASP functionality added */
                           /* changes in Register definition and start up, */
                           /* depending on device ID */
                           
                           /* v01.02 change JTAG timing */
/*#define MON_VERSION     0x0103     Monitor Version V01.03 test write spi flash to 10000 */
                           /* TCP and PROM only */
/*#define MON_VERSION     0x0104     Reset LOCAL Memory Map of ASP */
                           /* aim_mil.sys or aim_fdx.sys or aim_civ.sys driver */
                           /* SCR1089, GLOBAL and LOCAL RAM values NOVRAM, IRIG status register */
                           /* SPI update for AVR MEGA8535 */
/*#define MON_VERSION     0x0105  PC card functionality, start hardware*/
/*#define MON_VERSION     0x0106  PC card functionality, start hardware */
/*#define MON_VERSION     0x0107  APC429, set global memory area to 14MB */
                           /* function added http: global_RAM_TEST_full */
/*#define MON_VERSION     0x0108  IRIG intern Extern switch (SCR) */
                           /* STATUS FLAG register */
                           /* printf during update (SCR) */
/*#define MON_VERSION     0x0109  SCR 1456 */
                           /* For the APM429 the BIU needs the channel 
                              configuration. The monitor should write the 
                              Board Type from the NOVRAM to the Global RAM.
                              The position in the Global RAM is for the first 
                              BIU 3C and for the second BIU 103C and 1003C. 
                              The information should be written for all 
                              families(AFDX, 1553, 3910, ARINC etc). */
/*#define MON_VERSION     0x0110*/    /* Adaptation of UI to non-Windows environment 
                              usage e.g. LINUX */
/*#define MON_VERSION     0x0111     Adaptation of UI to non-Windows environment 
                              usage e.g. LynxOS */

/*#define MON_VERSION     0x0112*/  /* NOVRam Test read loop */
                           /* new start parameter 0 General AIM driver */
                           /* bug in AmcReadMemData, SCR1517 */
/*#define MON_VERSION     0x0113*/  /* NOVRam (CPU speed) value into GRAM before */
                                    /* BIU start */
                           /* 10/02/05 start parameter to read NOVRAM values, 
                                       and print them to the screen */

/*#define MON_VERSION     0x0114*/  /* To solve the problem with the NOVRAM, a copy of */         
                                    /* NOVRAM is saved in the TCP Flash (upper part)   */
                           /* Adr. 0x1e00...0x2000                            */

/*#define MON_VERSION     0x0115*/  /* corrected compiler errors for system driver */         
/*#define MON_VERSION     0x0116*/  /* deleate auto creation of flash mirror */         
/*#define MON_VERSION     0x0117*/  /* 17.06.2005*/
                           /* SCR1788, support for fdXTap added */
                           /* SCR1789, PROM.sre coding added for different cards prom-12.sre ...*/
                           /* SCR1790, slow down dump memory */         

/*#define MON_VERSION     0x0118*/      /* 19.12.2005*/
                           /* SCR1921, bug fix voltage measurement*/
                           /* SCR    , change SCR 1789 Functions*/
                           /*          PROM decoding not depending on the part numbers*/

/*#define MON_VERSION     0x0119*/  /* 11.01.2006*/
                           /* BUG FIX, */
/* **************************************************************** */
/*#define MON_VERSION     0x0200*/  /* 23.02.2006*/
                           /* SCR2079, new FILENAME coding for IO-FPGA, BIU... */
                           /* the NOVRAM values Platform Type and BIU Type is added*/
                           /* Meeting 23.02.06 with Bertie & Patrik */
                           /* new NOVRAM values for BIU usw. */
/*#define MON_VERSION     0x0201*/  /* 08.03.2006 PG*/
                                    /* SCR2105, adapt to VME-PC */
/*#define MON_VERSION     0x0203*/  /* 03/09.03.2006 ML*/
                           /* add defoult falues for IO-FPGA 1 and 2 boot,*/
                           /* otherwise no NOVRAM programming is possible*/
                           /* modify IO-routine of update NOVRAM values*/
/*#define MON_VERSION     0x0204*/  /* 18/07/06 Error in boot IO-FPGA function, */
                                    /* even by one channel cards, the IO-FPGA2 is booted*/
                           /* SCR/CDR 2269*/
/*#define MON_VERSION     0x0210*/  /* SCR / CDR2281, 20.07.06-04.09.06, Monitor modification:*/
                                    /* New DevID 4252 for AMC1553-4 (>V03.00)*/
                                    /* HW integration necessary*/
                           /* 2 new IRQs (BIU1C2, BIU2C4), IRQ event and mask reg.*/
/*#define MON_VERSION     0x0211*/  /* SCR / CDR2367, 24.10.06, Monitor modification:*/
/*#define MON_VERSION     0x0212*/  /* SCR / CDR, 21.01.07, IRIG OFFSET Function*/
                           /* NVR_IRIG_OFFSET, PMC_IRIG_SAMPLE_Reg added,*/
                           /* IO functions for this registers (ML)*/
                           /* 21.03.07 new NOVRAM Words BoardSubType, HardwareVariant*/
/*#define MON_VERSION       0x0213     15.10.07*/
                                    /* SCR2687 (23.04.2007) APM/AMC-Monitor: search SRE files for update in*/
                                    /*                      execution directory instead of "c:\aim\pmc" */               
                                    /* SCR2807 (28.06.2007) AMC/APM Support new NOVRAM fields "PCI Header"*/
                                    /*                      and Part Number Extension*/
/*#define MON_VERSION       0x0214     12.02.09 */
                                    /* SCR3485: Check the first SREC line in download file to be sure that */
                                    /*          it is downloaded for the correct board type */
                                    /* --> re-formatted source code */
/*#define MON_VERSION       0x0215      11.03.09 */
                                    /* SCR3614: PCI-LCA fails on many PC (mainly fast PCs) */
/*#define MON_VERSION       0x0216     15.10.09, ML  19.10.09 Bug in the Version No.*/
                                    /* increase global_memory to 16MByte, APU429_USB function added */
/*#define MON_VERSION       0x0217     10.03.10, ML  APU1553 functionality / driver acess */
/*#define MON_VERSION         0x0218   10.05.10, ML */
                                    /* - SMART Update files added / compared */
                                    /* - NOVRAM view (show NOVRAM values) production view */
                                    /* - APU1553 2MB is shown error */

/*#define MON_VERSION         0x0219   13.07.10, rh */
                                    /* - SMART Update files added / compared */
                                    /* - NOVRAM view (show NOVRAM values) production view */
                                    /* - APU1553 2MB is shown error */
                                    /* - ML 13.07.10, release, DeviceIoControl(..) added for the update case u (update prom and tcp) and b (PROM, TCO NOVRAM)*/
/*#define MON_VERSION         0x0220  */
                                                  /* 01.12.10, ML */
                                                  /* - filename byte array expendet to 256Bytes */
                                                  /* - enumerate for new driver (MH)            */
                                                  /* - PMC IRIG Status Register                 */
/*#define MON_VERSION         0x0221  */
                                                  /* 23.05.11, ML */
                                                  /* - test Functions "China CPU"              */
                                                  /* - enumerate for new driver (MH) (AEC429)  */
/*#define MON_VERSION         0x0222  */
                                                  /* 23.05.11, ML                      */
                                                  /* - ML Test, Jochen Problem                 */
                                                  /* - 12.12.2011 JTAG update  addd / included */
                                                  /*              NET228x USB based            */
                                                  /*   migration to Visual C++ 2008            */
                                                  /* -  ApuProgInterface.lib (21.10.2011) needed         */
                                                  /*    ApuProgInterface.dll (21.10.2011) for USB update */
/*#define MON_VERSION         0x0223 */
                                                  /* 30.10.12, ML                      */
                                                  /* 422 support                    */
                                                  /* use of the temp path           */
                                                  /* bug in wait clocks calculation */
                                                  /* usb update setting of cksel0 was false (this is als o false in program TCP, see CDR HW_BUG05240) */
                                                  /* but ovverwritten in update NOVRAM / NOVRAM mirrow, this should be corrected in the next version */
/*#define MON_VERSION           0x0224  */  
                                                  /* 0x224 29.08.2013, ML MMA */
                                                  /* ML, set lock Bits LB2 and LB1 to 10                                       */ 
                                                  /* LB2, LB1 = 1 0 = Future Programming of the Flsh and EEPROM is disabled in */
                                                  /* Paralell and Serial Programming mode,. The Fuse Bits are locked           */
                                                  /* in both Serial and Paralell programming mode                              */
                                                  /* MMA, NOVRAM restore in start_hardware CDR_05431                           */
                                                  /* ML, NOVRAM Restore, take care of the Lock Bits                            */
/* #define MON_VERSION          0x0225 */
                                                  /* 0x225 02.09.2013 also for AT 8535    */
                                                  /* changes in the NOVRAM resore process */
/*#define MON_VERSION           0x0226 */ 
                                                  /* 0x226 11.09.2013 USB based devices (APU) update of the TCP   */
                                                  /* see CDR 5446, 5447 */
                                                  /* failure in function "NET2282_batch_update_TCP()", "<= mem_length" instat of "< mem_length"   */ 
/*#define MON_VERSION           0x0227 */ 	
												  /* 0x0227 15.05.2014 */
/*#define MON_VERSION           0x0228 */ 
												  /* 0x0228 04.05.2015, version V02.27 is incombatible to 32Bit Operating Systems, Compiler Settings changed, see VID */
												  /*                     AIPM added */
//#define MON_VERSION           0x0229			  /* 0x0229 24.09.2015, ML RS485 added */ 
#define MON_VERSION           0x0230			  /* 0x0230 24.06.2016 Testversion for MMA, change LOCK Bits for NETchip */ 


/* platform type          */
#define PMC_32      0x50
#define PMC_32ASP   0x52
#define PMC_64      0x54
#define PMC_64ASP   0x56
#define API_V2      0x60
#define PC_CARD     0x70
#define USB_BASED   0xa0
    
#define MIL1553     0x10    /*BIU TYPE = MIL-STD 1553 */
#define ARINC429    0x20    /*BIU TYPE = ARINC429     */
#define AFDX_BOARD  0x60    /*BIU TYPE = AFDX         */

/*  ********************************************************
   prom file name definition:
   PROMxxyz.sre
      | ||-- 1  : protocol type BIU 2 = MIL-STD 1553
      | |    2  : protocol type BIU 2 = ARINC429    
      | |    6  : protocol type BIU 2 = AFDX
      | |    0  : protocol type BIU 2 = not present
      | |
      | |--- 1  : protocol type BIU 1 = MIL-STD 1553
      |      2  : protocol type BIU 1 = ARINC429
      |      6  : protocol type BIU 1 = AFDX
      |      0  : protocol type BIU 1 = not present
      |
      |----  50 : platform type = PMC 32Bit 
             52 : platform type = PMC 32Bit with ASP
             54 : platform type = PMC 64Bit
             56 : platform type = PMC 64Bit with ASP
             60 : platform type = API V2
             70 : platform type = PC-Card
             a0 : platform type = USB Based
   
   e.g.: prom6060.sre = API FDX V2
         prom5011.sre = PMC 32Bit 2/4 channel 1553 (AMC1553-2)

    ********************************************************/

/* device IDs */
#define dveID_PMC32        0x0050    /* 32bit PMC */
#define dveID_PMC32_ASP    0x0052    /* 32bit PMC, with ASP */
#define dveID_PMC64        0x0054    /* 64bit PMC */
#define dveID_PMC64_ASP    0x4857    /* 64bit PMC, with ASP */
#define dveID_PCCard_1553 0x4a57    /* PC card def ID (v01.05)*/
#define dveID_PMC32_2nd    0x4252     /* 32bit PMC 2nd Generation >V03.00*/

#define CLK_1US            1
#define NVR_MAGIC_def     0x4c616d70    /* NOVRam Magic, used as good indicator*/

/* JTAG*/
#define JTAG_Key          0x4c5c4c59

/* von API mon uebernommen*/

/* EEPROM register bit definitions */
#define  TC_E_WRBSY_B      (1 << 1)      /* EEPROM write running */
#define  TC_E_RDBSY_B      (1 << 0)      /* EEPROM read busy */
#define  TC_E_WRENA_B      (1 << 0)      /* EEPROM Write enable 27.07,98 BUG Bit 1 H.Z. */

/* IRIG register bit definitions */
#define TC_I_DETECT_B      (1 << 0)      /* valid IRIG detected */
#define TC_I_ELATCH_B      (1 << 0)      /* Latch IRIG Encoder Data */
#define TC_I_DLATCH_B      (2 << 0)      /* Latch IRIG Decoder Data */
#define TC_I_LEAP_B        (7 << 0)      /* Set IRIG Decoder Leap Year */

/* IRIG Register definition */
#define TC_ESTAT           0x5      /* EEPROM status register     */
#define TC_ECTRL           0x6         /* EEPROM control register     */
#define TC_ADDRHI          0x7         /* EEPROM address high       */
#define TC_ADDRLO          0x8         /* EEPROM address low       */
#define TC_EWRDAT          0x9         /* EEPROM write data       */
#define TC_ERDDAT          0xa         /* EEPROM read data       */

#define TC_ISTAT           0xb         /* IRIG status register     */
#define TC_ICTRL           0xc         /* IRIG control register     */
#define TC_IEDAYHI         0xd         /* IRIG time encode day high  */
#define TC_IEDAYLO         0xe         /* IRIG time encode day low   */
#define TC_IEHOUR          0xf         /* IRIG time encode hour      */
#define TC_IEMIN           0x10       /* IRIG time encode minute    */
#define TC_IESEC           0x11       /* IRIG time encode seconds   */

#define TC_IDDAYHI         0x12       /* IRIG time decode day high  */
#define TC_IDDAYLO         0x13       /* IRIG time decode day low   */
#define TC_IDHOUR          0x14       /* IRIG time decode hour      */
#define TC_IDMIN           0x15       /* IRIG time decode minute    */
#define TC_IDSEC           0x16       /* IRIG time decode seconds   */

#define TC_MSECHI          0x17       /* IRIG time decode ms       */
#define TC_MSECLO          0x18       /* IRIG time decode ms       */
#define TC_SRAMAHI         0x19       /* SRAM Address high         */
#define TC_SRAMALO         0x1a       /* SRAM Address low          */
#define TC_SRAMDAT         0x1b       /* SRAM Data                 */
#define TC_WRIRIGEXINx     0x1d       /* Write to Register IRIG extern / intern_not */
#define TC_RDIRIGEXINx     0x1e       /* Read Register IRIG  extern / intern_not*/

#define TC_VERSION         0x1f       /* TCP Firmware Version     */

/*-----IRIG intern / extern*/
#define IRIG_intern        0xff
#define IRIG_extern        0x00

/* TCP - InCuircuit Programing definitions */
#define SPI_RD_FLASH_LOW        0x20
#define SPI_RD_FLASH_HIGH       0x28

#define SPI_WR_FLASH_LOW        0x40
#define SPI_WR_FLASH_HIGH       0x48

#define SPI_RD_NOVRAM           0xa0
#define SPI_WR_NOVRAM           0xc0

#define SPI_ERASE_BYTE0         0xac
#define SPI_ERASE_BYTE1         0x80

#define SPI_PROGRAM_BYTE0       0xac
#define SPI_PROGRAM_BYTE1       0x53

#define SPI_RD_DEVCODE          0x30

#define SPI_RD_VENDORCODE       0x00
#define SPI_RD_PARTFAMILY       0x01
#define SPI_RD_PARTNUMBER       0x02

#define SPI_RD_LOCKFUSE_BITS    0x58

#define SPI_AVR8535             0x9303 /* AVR 90S8535   */
#define SPI_AVRMEGA8535         0x9308 /* AVR MEGA 8535 */

#define SPI_MEGA8535_RD_LOCK_BITS        0x58
#define SPI_MEGA8535_RD_FUSE_BITS_B1     0x50
#define SPI_MEGA8535_RD_FUSE_BITS_B2     0x00
#define SPI_MEGA8535_RD_FUSE_BITS_H_B1   0x58
#define SPI_MEGA8535_RD_FUSE_BITS_H_B2   0x08

#define SPI_MEGA8535_WR_LOCK_BITS_B1     0xac
#define SPI_MEGA8535_WR_LOCK_BITS_B2     0xe0
#define SPI_MEGA8535_WR_FUSE_BITS_B1     0xac
#define SPI_MEGA8535_WR_FUSE_BITS_B2     0xa0
#define SPI_MEGA8535_WR_FUSE_BITS_H_B1   0xac
#define SPI_MEGA8535_WR_FUSE_BITS_H_B2   0xa8

#define SPI_MEGA8535_WR_MEM_PAGE         0x4c  /* write program memory page */
#define SPI_MEGA8535_WR_PROG_MEM_HIGHB   0x48  /* load program memory high byte */
#define SPI_MEGA8535_WR_PROG_MEM_LOWB    0x40 /* load program memory low byte */

#define MAX_DATA_SIZE 0x10000

/****************************************************************/
/* NET2282 defines                                              */
/****************************************************************/
/* define Address */
#define NET2282_SYSAD_v1  0x1c00    /* NET228x SYS Pointer Version V01.x    (SPI)         */
#define NET2282_SYSAD_v2  0x2800    /* NET228x SYS Pointer Version V02.x  (JTAG included) */


/* define commands */
#define NET2282_IDLE       0x00
#define NET2282_WR_JTAG_B1 0x01
#define NET2282_WR_JTAG_B2 0x02
#define NET2282_RD_JTAG    0x03
#define NET2282_WR_SPI_B1  0x11
#define NET2282_WR_SPI_B2  0x12
#define NET2282_RD_SPI     0x13
#define NET2282_WR_NOVRAM  0x14
#define NET2282_RD_NOVRAM  0x15
#define NET2282_INIT       0x20
#define NET2282_SPI_ENABLE 0x21
#define NET2282_DEBUG1     0x70
#define NET2282_DEBUG2     0x71
#define NET2282_DEBUG3     0x72
#define NET2282_DEBUG4     0x73




/* PMC Driver */
#ifdef __monitor__
  #ifdef _AIM_LINUX
    /* ioctl's for aim_fdx. */
    #define AIM_FDX_IOCTL_BASE      0xbb
    #define AIM_FDX_GET_DEVICES     _IOR(AIM_FDX_IOCTL_BASE,   0, AiUInt32)
  #elif _AIM_VME
    /* ioctl's for aim_fdx. */
    #define IOCTL_READ              _IOWR('k', 2, AIMMIL_IOC_TYPE *)
    #define IOCTL_WRITE             _IOWR('k', 3, AIMMIL_IOC_TYPE *)
    #define IOCTL_INIT               _IOWR('k', 5, char *)
    #define IOCTL_FDXIO_READM       _IOWR('k', 33, IOC_FDXIO_MEMIO *)
    #define IOCTL_FDXIO_WRITEM      _IOWR('k', 34, IOC_FDXIO_MEMIO *)
    #define AIM_FDX_IOCTL_BASE      0xbb
    #define AIM_FDX_GET_DEVICES     _IOWR('k', 5, char *)
    #define AIM_FDX_MEM_ACCESS_PREP _IOW(AIM_FDX_IOCTL_BASE,   3, Memprep)
  #else
#ifndef CTL_CODE
   #define CTL_CODE(DeviceType,Function,Method,Access)(((DeviceType)<<16)|((Access)<<14)|((Function)<<2)|(Method))
#endif      /* visual C 2008, also defined in winioctl.h */
   #define METHOD_BUFFERED             0
   #define METHOD_IN_DIRECT            1
   #define METHOD_OUT_DIRECT           2
   #define METHOD_NETHER               3

   #define FILE_DEVICE_API_FDX         0x8100
   #define FILE_DEVICE_API_MIL         0x8101
   #define FILE_DEVICE_API_PAN         0x8102
   #define FILE_DEVICE_API_CIV         0x8103
   #define FILE_DEVICE_API_422         0x8105
   #define FILE_DEVICE_API_AIPM        0x8106		// steht in AimIoCtl.h (PVCS)
   #define FILE_DEVICE_API_RS485       0x8107		// steht in AimIoCtl.h (PVCS)

   #define FILE_ANY_ACCESS             0
#ifndef FILE_READ_ACCESS
   #define FILE_READ_ACCESS           (0x0001)
#endif    /* visual C 2008, also defined in winioctl.h */
   #define FILE_WITE_ACCESS            (0x0002)
   #define IOCTL_READ_MEM_DATA_FDX     CTL_CODE(FILE_DEVICE_API_FDX,812,METHOD_BUFFERED,FILE_ANY_ACCESS)
   #define IOCTL_WRITE_MEM_DATA_FDX    CTL_CODE(FILE_DEVICE_API_FDX,813,METHOD_BUFFERED,FILE_ANY_ACCESS)
   #define IOCTL_READ_MEM_DATA_MIL     CTL_CODE(FILE_DEVICE_API_MIL,812,METHOD_BUFFERED,FILE_ANY_ACCESS)
   #define IOCTL_WRITE_MEM_DATA_MIL    CTL_CODE(FILE_DEVICE_API_MIL,813,METHOD_BUFFERED,FILE_ANY_ACCESS) 
   /* never used #define IOCTL_JTAG_EXECUTE_MIL      CTL_CODE(0x8101,816,METHOD_BUFFERED,FILE_ANY_ACCESS) */
   #define IOCTL_READ_MEM_DATA_PSI     CTL_CODE(FILE_DEVICE_API_PAN,812,METHOD_BUFFERED,FILE_ANY_ACCESS)
   #define IOCTL_WRITE_MEM_DATA_PSI    CTL_CODE(FILE_DEVICE_API_PAN,813,METHOD_BUFFERED,FILE_ANY_ACCESS)
   #define IOCTL_READ_MEM_DATA_CIV     CTL_CODE(FILE_DEVICE_API_CIV,812,METHOD_BUFFERED,FILE_ANY_ACCESS)
   #define IOCTL_WRITE_MEM_DATA_CIV    CTL_CODE(FILE_DEVICE_API_CIV,813,METHOD_BUFFERED,FILE_ANY_ACCESS)
   #define IOCTL_READ_MEM_DATA_FDXTAP  CTL_CODE(FILE_DEVICE_API_FDX,812,METHOD_BUFFERED,FILE_ANY_ACCESS)
   #define IOCTL_WRITE_MEM_DATA_FDXTAP CTL_CODE(FILE_DEVICE_API_FDX,813,METHOD_BUFFERED,FILE_ANY_ACCESS)
   #define IOCTL_READ_MEM_DATA_RS422   CTL_CODE(FILE_DEVICE_API_422,812,METHOD_BUFFERED,FILE_ANY_ACCESS)
   #define IOCTL_WRITE_MEM_DATA_RS422  CTL_CODE(FILE_DEVICE_API_422,813,METHOD_BUFFERED,FILE_ANY_ACCESS) 

   #define IOCTL_READ_MEM_DATA_AIPM   CTL_CODE(FILE_DEVICE_API_AIPM,812,METHOD_BUFFERED,FILE_ANY_ACCESS)
   #define IOCTL_WRITE_MEM_DATA_AIPM  CTL_CODE(FILE_DEVICE_API_AIPM,813,METHOD_BUFFERED,FILE_ANY_ACCESS)

   #define IOCTL_READ_MEM_DATA_RS485   CTL_CODE(FILE_DEVICE_API_RS485,812,METHOD_BUFFERED,FILE_ANY_ACCESS)
   #define IOCTL_WRITE_MEM_DATA_RS485  CTL_CODE(FILE_DEVICE_API_RS485,813,METHOD_BUFFERED,FILE_ANY_ACCESS)




   #define IOCTL_DISABLE_INT_FDX       CTL_CODE(FILE_DEVICE_API_FDX,824,METHOD_BUFFERED,FILE_ANY_ACCESS)
   #define IOCTL_ENABLE_INT_FDX        CTL_CODE(FILE_DEVICE_API_FDX,825,METHOD_BUFFERED,FILE_ANY_ACCESS)
   #define IOCTL_DISABLE_INT_MIL       CTL_CODE(FILE_DEVICE_API_MIL,824,METHOD_BUFFERED,FILE_ANY_ACCESS)
   #define IOCTL_ENABLE_INT_MIL        CTL_CODE(FILE_DEVICE_API_MIL,825,METHOD_BUFFERED,FILE_ANY_ACCESS)
   #define IOCTL_DISABLE_INT_CIV       CTL_CODE(FILE_DEVICE_API_CIV,824,METHOD_BUFFERED,FILE_ANY_ACCESS)
   #define IOCTL_ENABLE_INT_CIV        CTL_CODE(FILE_DEVICE_API_CIV,825,METHOD_BUFFERED,FILE_ANY_ACCESS)
   #define IOCTL_DISABLE_INT_RS422     CTL_CODE(FILE_DEVICE_API_422,824,METHOD_BUFFERED,FILE_ANY_ACCESS)
   #define IOCTL_ENABLE_INT_RS422      CTL_CODE(FILE_DEVICE_API_422,825,METHOD_BUFFERED,FILE_ANY_ACCESS)

   #define IOCTL_DISABLE_INT_AIPM     CTL_CODE(FILE_DEVICE_API_AIPM,824,METHOD_BUFFERED,FILE_ANY_ACCESS)
   #define IOCTL_ENABLE_INT_AIPM      CTL_CODE(FILE_DEVICE_API_AIPM,825,METHOD_BUFFERED,FILE_ANY_ACCESS)

   #define IOCTL_DISABLE_INT_RS485     CTL_CODE(FILE_DEVICE_API_RS485,824,METHOD_BUFFERED,FILE_ANY_ACCESS)
   #define IOCTL_ENABLE_INT_RS485      CTL_CODE(FILE_DEVICE_API_RS485,825,METHOD_BUFFERED,FILE_ANY_ACCESS)



  #endif
#endif /* monitor */

/*********************************************/
/* CPLD - InCuircuit Programing definitions (JTAG) */

#define  JTAG_TCK          0x0
#define  JTAG_TMS          0x1
#define  JTAG_TDI          0x2

#define TC_IO_TDO_B       (1<<7)
#define TC_IO_TDI_B       (1<<6)
#define TC_IO_TCK_B       (1<<5)
#define TC_IO_TMS_B       (1<<4)
#define TC_IO_TOGGLE_B    (1<<0)

#define  JTAG_INIT_BY0      0x35  /* Enable TCP for JTAG load */
#define  JTAG_INIT_BY1      0xca  /* Command TCP JTAG load */
#define JTAG_TCPRST_RTRY  10    /* Execute 10 times reset to restart the TCP */

#define JTAG_C_CLC        0x3    /* TCP JTAG Command: Clear Instruction */
#define JTAG_C_CLO        0x2    /* TCP JTAG Command: Load lower 2 bit of Instruction */
#define JTAG_C_CHI        0x1    /* TCP JTAG Command: Load upper 2 bit of Instruction */
#define JTAG_C_CEX        0x0    /* TCP JTAG Command: Execute Instruction */

#define JTAG_I_NOP        0x00  /* TCP JTAG Instr.: NOP */
#define JTAG_I_TDO        0x01  /* TCP JTAG Instr.: Read TDO  */
#define JTAG_I_TDI_0      0x02  /* TCP JTAG Instr.: Set TDI 0 (even instr.) */
#define JTAG_I_TDI_1      0x03  /* TCP JTAG Instr.: Set TDI 1 (odd instr.)  */
#define JTAG_I_TMS_0      0x04  /* TCP JTAG Instr.: Set TMS 0 (even instr.) */
#define JTAG_I_TMS_1      0x05  /* TCP JTAG Instr.: Set TMS 1 (odd instr.)  */
#define JTAG_I_TCK_0      0x06  /* TCP JTAG Instr.: Set TCK 0 (even instr.) */
#define JTAG_I_TCK_1      0x07  /* TCP JTAG Instr.: Set TCK 1 (odd instr.)  */
#define JTAG_I_PTCK        0x09  /* TCP JTAG Instr.: Pulse TCK 0->1->0 */

#define JTAG_TC_BP        (TC_BP)

#define JTAG_WTCOM        (clk_1us*12) /* Max. TCP Command     execution time (8) */
#define JTAG_WTINSTR      (clk_1us*15) /* Max. TCP Instruction execution time (10) */

/* encodings of xsvf instructions */
#define XCOMPLETE         0 
#define XTDOMASK          1 
#define XSIR              2 
#define XSDR              3 
#define XRUNTEST          4        
#define XREPEAT           7
#define XSDRSIZE          8
#define XSDRTDO           9
#define XSETSDRMASKS      10
#define XSDRINC           11
#define XSDRB             0xc
#define XSDRC             0xd
#define XSDRE             0xe
#define XSDRTDOB          0xf
#define XSDRTDOC          0x10
#define XSDRTDOE          0x11
#define XSTATE            0x12
#define XENDIR            0x13  
#define XENDDR            0x14

/* return number of bytes necessary for "num" bits */
#define JTAG_BYTES(num) (((num%8)==0) ? (num/8) : (num/8+1))

/*********************************************/
/* The following defines define the debug levels */
#ifdef _DEBUG
  #define MON_DBG_INIT              0x00000002
  #define MON_DBG_ERROR             0x40000000
  #define  MON_DBG_MINIMAL          0x80000000
  #define MON_DBG_ALL               0xFFFFFFFF

  #define FKT_MON                   "       Monitor-SW"

  #if defined(_SMARTLIB)
    #define MonPrint(LEVEL,FKT,STRING)
  #elif defined(_AIM_VME)
    #if defined (_AIM_FDX)
      #define LOGFILE "c:\wdm.log"

      #define MonPrint(LEVEL,FKT,STRING) \
              do \
              { \
                unsigned long _level = (LEVEL); \
                if( (ul_FdxDbgLevel & DBG_MINIMAL) == 0 ) \
                  _level &= ~DBG_MINIMAL; \
                if( (ul_FdxDbgLevel & _level) == _level ) \
                { \
                  printf( "%s (%s): ", DLL_NAME, FKT );\
                  printf STRING;\
                } \
              } while (0)
    #elif defined (_AIM_1553)

      #define MonPrint(LEVEL,FKT,STRING) \
              do \
              { \
                unsigned long _level = (LEVEL); \
                if( (ul_DbgLevel & DBG_MINIMAL) == 0 ) \
                  _level &= ~DBG_MINIMAL; \
                if( (ul_DbgLevel & _level) == _level ) \
                { \
                  printf( "%s (%s): ", AI1553_DLL_NAME, FKT );\
                  printf STRING;\
                } \
              } while (0)
    #elif defined _AMC429

      #define MonPrint(LEVEL,FKT,STRING) \
              do \
              { \
                unsigned long _level = (LEVEL); \
                if( (Ai429ul_DbgLevel & DBG_MINIMAL) == 0 ) \
                  _level &= ~DBG_MINIMAL; \
                if( (Ai429ul_DbgLevel & _level) == _level ) \
                { \
                  printf( "%s (%s): ", AI429_DLL_NAME, FKT );\
                  printf STRING;\
                } \
              } while (0)
    #endif
  #elif defined (_AIM_WINDOWS)
    #define MonPrint(LEVEL,FKT,STRING) \
            do \
            { \
              unsigned long _level = (LEVEL); \
              if( (ApiDebugLevel & DBG_MINIMAL) == 0 ) \
                _level &= ~DBG_MINIMAL; \
              if( (ApiDebugLevel & _level) == _level ) \
              { \
                DbgPrint( "%s (%s): ", DRV_NAME, FKT );\
                DbgPrint STRING;\
              } \
            } while (0)
  #elif defined(_AIM_VXI)
    #if defined (_AIM_FDX)
      #define LOGFILE "c:\\lvrt.log"

      #define MonPrint(LEVEL,FKT,STRING) \
              do \
              { \
                unsigned long _level = (LEVEL); \
                if( (ul_FdxDbgLevel & DBG_MINIMAL) == 0 ) \
                  _level &= ~DBG_MINIMAL; \
                if( (ul_FdxDbgLevel & _level) == _level ) \
                { \
                   FILE* p_logfile = NULL; \
                   p_logfile = fopen(LOGFILE,"a"); \
                   if(p_logfile != NULL) \
                   { \
                      fprintf( p_logfile, "%s (%s): ", DLL_NAME, FKT);\
                      fclose(p_logfile);\
                   } \
                  printf( "%s (%s): ", DLL_NAME, FKT );\
                  printf STRING;\
                } \
              } while (0)
    #elif defined (_AIM_1553) 
      #define LOGFILE " "

      #define MonPrint(LEVEL,FKT,STRING) \
              do \
              { \
                unsigned long _level = (LEVEL); \
                if( (Aiul_DbgLevel & DBG_MINIMAL) == 0 ) \
                  _level &= ~DBG_MINIMAL; \
                if( (Aiul_DbgLevel & _level) == _level ) \
                { \
                  printf( "%s (%s): ", AI1553_DLL_NAME, FKT );\
                  printf STRING;\
                } \
              } while (0)
    #elif defined (_AMC429)
      #define LOGFILE " "

      #define MonPrint(LEVEL,FKT,STRING) \
              do \
              { \
                unsigned long _level = (LEVEL); \
                if( (Aiul_DbgLevel & DBG_MINIMAL) == 0 ) \
                  _level &= ~DBG_MINIMAL; \
                if( (Aiul_DbgLevel & _level) == _level ) \
                { \
                  printf( "%s (%s): ", AI429_DLL_NAME, FKT );\
                  printf STRING;\
                } \
              } while (0)
    #endif
  #endif

  #if defined(_SMARTLIB)
    #define MonPrintN(LEVEL,STRING)
  #elif defined (_AIM_VME)
    #if defined (_AIM_FDX)
      #define MonPrintN(LEVEL,STRING) \
              do \
                 { \
                    unsigned long _level = (LEVEL); \
                    if( (ul_FdxDbgLevel & DBG_MINIMAL) == 0 ) \
                       _level &= ~DBG_MINIMAL; \
                    if( (ul_FdxDbgLevel & _level) == _level ) \
                    { \
                  printf STRING;\
                    } \
              } while (0)
    #elif defined (_AIM_1553)
      #define MonPrintN(LEVEL,STRING) \
              do \
                 { \
                unsigned long _level = (LEVEL); \
                  if( (ul_DbgLevel & DBG_MINIMAL) == 0 ) \
                       _level &= ~DBG_MINIMAL; \
                    if( (ul_DbgLevel & _level) == _level ) \
                    { \
                  printf STRING;\
                    } \
              } while (0)
    #elif defined (_AIM_429)
      #define MonPrintN(LEVEL,STRING) \
              do \
              { \
            unsigned long _level = (LEVEL); \
            if( (Ai429ul_DbgLevel & DBG_MINIMAL) == 0 ) \
               _level &= ~DBG_MINIMAL; \
            if( (Ai429ul_DbgLevel & _level) == _level ) \
            { \
               printf STRING;\
            } \
      } while (0)
    #endif
  #elif defined (_AIM_WINDOWS)
    #define MonPrintN(LEVEL,STRING) \
               do \
               { \
                  unsigned long _level = (LEVEL); \
                  if( (ApiDebugLevel & DBG_MINIMAL) == 0 ) \
                     _level &= ~DBG_MINIMAL; \
                  if( (ApiDebugLevel & _level) == _level ) \
                  { \
                 DbgPrint STRING;\
                  } \
            } while (0)
  #elif defined (_AIM_VXI)
    #if defined (_AIM_FDX)
      #define MonPrintN(LEVEL,STRING) DEBUGOUTN(LEVEL,STRING) \
              do \
                 { \
                    unsigned long _level = (LEVEL); \
                    if( (ul_FdxDbgLevel & DBG_MINIMAL) == 0 ) \
                       _level &= ~DBG_MINIMAL; \
                    if( (ul_FdxDbgLevel & _level) == _level ) \
                    { \
                       FILE* p_logfile = NULL; \
                       p_logfile = fopen(LOGFILE,"a"); \
                       if(p_logfile != NULL) \
                       { \
                          fclose(p_logfile); \
                       } \
                  printf STRING;\
                    } \
              } while (0)
    #elif defined (_AIM_1553) || defined (_AMC429) /*_AMC429 used for LabView RT */
      #define MonPrintN(LEVEL,STRING) \
              do \
                 { \
                    unsigned long _level = (LEVEL); \
                    if( (Aiul_DbgLevel & DBG_MINIMAL) == 0 ) \
                       _level &= ~DBG_MINIMAL; \
                    if( (Aiul_DbgLevel & _level) == _level ) \
                    { \
                  printf STRING;\
                    } \
              } while (0)
    #endif
  #endif

  #if defined(_SMARTLIB)
    #define MonPrintf(FILENAME,LEVEL,FKT,STRING)
  #elif defined (_AIM_VME)
    #if defined (_AIM_FDX)
      #define MonPrintf(FILENAME,LEVEL,FKT,STRING) \
              do \
                 { \
                    unsigned long _level = (LEVEL); \
                    if( (ul_FdxDbgLevel & DBG_MINIMAL) == 0 ) \
                       _level &= ~DBG_MINIMAL; \
                    if( (ul_FdxDbgLevel & _level) == _level ) \
                    { \
                       printf( "%s (%s): ", DLL_NAME, FKT );\
                   printf STRING;\
                    } \
              } while (0)
    #elif defined (_AIM_1553)
      #define MonPrintf(FILENAME,LEVEL,FKT,STRING) \
              do \
                 { \
                    unsigned long _level = (LEVEL); \
                    if( (ul_DbgLevel & DBG_MINIMAL) == 0 ) \
                       _level &= ~DBG_MINIMAL; \
                    if( (ul_DbgLevel & _level) == _level ) \
                    { \
                       printf( "%s (%s): ", AI1553_DLL_NAME, FKT );\
                       printf STRING;\
                    } \
                 } while (0)
    #elif defined( _AMC429)
      #define MonPrintf(FILENAME,LEVEL,FKT,STRING) \
              do \
                 { \
                    unsigned long _level = (LEVEL); \
                    if( (Ai429ul_DbgLevel & DBG_MINIMAL) == 0 ) \
                       _level &= ~DBG_MINIMAL; \
                    if( (Ai429ul_DbgLevel & _level) == _level ) \
                    { \
                       printf( "%s (%s): ", AI429_DLL_NAME, FKT );\
                       printf STRING;\
                    } \
              } while (0)
    #endif
  #elif defined (_AIM_WINDOWS)
    #define MonPrintf(FILENAME,LEVEL,FKT,STRING) \
           do \
           { \
               /* \
               AiHandle h_File;\
               AiUInt32 ul_BytesWrite;\
               unsigned long _level = (LEVEL); \
               if( (ApiDebugLevel & DBG_MINIMAL) == 0 ) \
                 _level &= ~DBG_MINIMAL; \
               if( (ApiDebugLevel & _level) == _level ) \
               { \
                 unsigned char *fbuf;\
                 WCHAR *ac_WFileName;\
                 fbuf         = (unsigned char *) ExAllocatePool(NonPagedPool, 100);\
                 ac_WFileName = (WCHAR *) ExAllocatePool(NonPagedPool, 200);\
                 sprintf(fbuf, "%s%d", FILENAME, GLOBAL_VAR->uc_ModuleNr);\
                 mbstowcs(ac_WFileName,fbuf,100);\
                 OpenFile( ac_WFileName, FALSE, TRUE, &h_File );\
                 WriteFile( h_File, STRING, sizeof(STRING), &ul_BytesWrite );\
                 CloseFile(h_File);\
                 */ \
                 DbgPrint( "%s (%s): ", DRV_NAME, FKT );\
                 DbgPrint STRING;\
               /* \
                 ExFreePool(fbuf); \
                 ExFreePool(ac_WFileName); \
               }  */ \
            } while (0)
    #define MonPrintfVar(FILENAME,LEVEL,FKT,STRING) \
            do \
            { \
                /* \
              AiHandle h_File;\
              AiUInt32 ul_BytesWrite;\
              unsigned long _level = (LEVEL); \
              if( (ApiDebugLevel & DBG_MINIMAL) == 0 ) \
                  _level &= ~DBG_MINIMAL; \
              if( (ApiDebugLevel & _level) == _level ) \
              { \
                unsigned char *fbuf;\
                WCHAR *ac_WFileName;\
                fbuf         = (unsigned char *) ExAllocatePool(NonPagedPool, 100);\
                ac_WFileName = (WCHAR *) ExAllocatePool(NonPagedPool, 200);\
                sprintf(fbuf, "%s%d", FILENAME, GLOBAL_VAR->uc_ModuleNr);\
                mbstowcs(ac_WFileName,fbuf,100);\
                OpenFile( ac_WFileName, FALSE, TRUE, &h_File );\
                WriteFile( h_File, STRING, sizeof(STRING), &ul_BytesWrite );\
                CloseFile(h_File);\
                ExFreePool(fbuf);\
                ExFreePool(ac_WFileName);\
              } \
                */ \
            } while (0)
  #elif defined (_AIM_VXI)
    #if defined (_AIM_FDX)
      #define MonPrintf(FILENAME,LEVEL,FKT,STRING) DEBUGOUT(LEVEL,FKT,STRING) /*\
              do \
                 { \
                    unsigned long _level = (LEVEL); \
                    if( (ul_FdxDbgLevel & DBG_MINIMAL) == 0 ) \
                       _level &= ~DBG_MINIMAL; \
                    if( (ul_FdxDbgLevel & _level) == _level ) \
                    { \
                       FILE* p_logfile = NULL; \
                       p_logfile = fopen(LOGFILE,"a"); \
                       if(p_logfile != NULL) \
                       { \
                          fprintf( p_logfile, "%s (%s): (_level:%08lx ul_FdxDbgLevel:%08lx)", DLL_NAME, FKT, _level, ul_FdxDbgLevel); \
                          fclose(p_logfile); \
                       } \
                       printf( "%s (%s): ", DLL_NAME, FKT );\
                       printf STRING;\
                    } \
              } while (0) */
    #elif defined (_AIM_1553)
      #define MonPrintf(FILENAME,LEVEL,FKT,STRING) \
              do \
                 { \
                    unsigned long _level = (LEVEL); \
                    if( (Aiul_DbgLevel & DBG_MINIMAL) == 0 ) \
                       _level &= ~DBG_MINIMAL; \
                    if( (Aiul_DbgLevel & _level) == _level ) \
                    { \
                       printf( "%s (%s): ", AI1553_DLL_NAME, FKT );\
                   printf STRING;\
                    } \
              } while (0)
    #elif defined (_AMC429)
      #define MonPrintf(FILENAME,LEVEL,FKT,STRING) \
              do \
                 { \
                    unsigned long _level = (LEVEL); \
                    if( (Aiul_DbgLevel & DBG_MINIMAL) == 0 ) \
                       _level &= ~DBG_MINIMAL; \
                    if( (Aiul_DbgLevel & _level) == _level ) \
                    { \
                       printf( "%s (%s): ", AI429_DLL_NAME, FKT );\
                   printf STRING;\
                    } \
              } while (0)
    #endif
  #elif defined (_AIM_LINUX)
    #define LOGFILE ""
    #include <linux/kernel.h>
    #define MonPrint(LEVEL,FKT,STRING) do { \
            printk( KERN_INFO "aim: %s:%s", FKT, __FUNCTION__ );    \
              printk STRING; \
            printk( "\n" ); \
            } while( 0 )
    #define MonPrintN(LEVEL,STRING) do { \
            printk( KERN_INFO "aim: %s", __FUNCTION__ );         \
              printk STRING; \
            printk( "\n" ); \
            } while( 0 )
    #define MonPrintf(FILENAME,LEVEL,FKT,STRING) do {   \
            printk( KERN_INFO "aim: %s:%s", FKT, __FUNCTION__ );     \
              printk STRING; \
            printk( "\n" ); \
            } while( 0 )
  #elif defined (_AIM_QNX)
    #include "global.h"
    #define LOGFILE ""
    #define MonPrint(LEVEL,FKT,STRING) do { if( ( ( guiLogMask & ( LOG_MASK_MONITOR | LOG_MASK_MODE_WORK ) )  == ( LOG_MASK_MONITOR | LOG_MASK_MODE_WORK) ) ) { printf STRING; } } while( 0 )
    #define MonPrintN(LEVEL,STRING) MonPrint( LEVEL, "", STRING )
    #define MonPrintf(FILENAME,LEVEL,FKT,STRING) MonPrint( LEVEL, "", STRING )
  #endif
#else
  #ifdef _AIM_QNX
    #ifdef _AIM_SYSDRV
      #include "global.h"
      #define MonPrint(LEVEL,FKT,STRING) do { if( ( ( guiLogMask & ( LOG_MASK_MONITOR | LOG_MASK_MODE_WORK ) )  == ( LOG_MASK_MONITOR | LOG_MASK_MODE_WORK) ) ) { printf STRING; } } while( 0 )
      #define MonPrintN(LEVEL,STRING) MonPrint( LEVEL, "", STRING )
      #define MonPrintf(FILENAME,LEVEL,FKT,STRING) MonPrint( LEVEL, "", STRING )
    #else
      #define MonPrint(LEVEL,FKT,STRING) do { printf STRING; } while( 0 )
      #define MonPrintN(LEVEL,STRING) MonPrint( LEVEL, "", STRING )
      #define MonPrintf(FILENAME,LEVEL,FKT,STRING) MonPrint( LEVEL, "", STRING )
    #endif /* _AIM_SYSDRV */
  #else
    #define MonPrint(LEVEL,FKT,STRING)
   #define MonPrintN(LEVEL,STRING)
    #define MonPrintf(FILENAME,LEVEL,FKT,STRING)
    #define MonPrintfVar(FILENAME,LEVEL,FKT,STRING)
  #endif /* _AIM_QNX */
#endif  

#ifdef __LYNX_OS__
  #define AIM_WAIT(x) sleep(x/1000)
#endif
#ifdef __VXWORKS__
  #include <drv/timer/timerDev.h>
  #define AIM_WAIT(x) taskDelay(sysClkRateGet()* x/1000)
#endif
#if defined(__INTEGRITY__)
  #define AIM_WAIT(x) sleep(x/1000)
#endif

#if defined(_AIM_VMEWINDOWS)
  #define AIM_WAIT(x) Sleep(x)
#endif
#if defined(_SMARTLIB) && !defined _AIM_LINUX
  #define AIM_WAIT(x) Sleep(x)
#endif
#if defined(_AIM_WINDOWS) && defined(_AIM_SYSDRV)
  #define AIM_WAIT(x)
#endif
#ifdef _AIM_LINUX
  #ifdef _AIM_SYSDRV
    #include <linux/version.h>

    #if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,0)
      #define LINUX_2_4
    #endif

    #if defined(CONFIG_MODVERSIONS) && defined(LINUX_2_4) && !defined(MODVERSIONS)
      #define MODVERSIONS
    #endif

    #ifdef MODVERSIONS
      #include <linux/modversions.h>
    #endif

    #include <linux/string.h>
    #include <linux/kernel.h>    /* sprintf */
    #include <linux/delay.h>
    #define AIM_WAIT(x) udelay((x)*1000)
    #define API_OK 0
  #else
    #include <unistd.h>
    #define AIM_WAIT(x) usleep((x)*1000)
  #endif
#endif

#ifdef _AIM_QNX
  #include <stdio.h>             /* printf */
  #include <unistd.h>            /* usleep */
  #define AIM_WAIT(x) usleep((x)*1000)
  #define API_OK     0
#endif

#ifdef _AIM_VXI
  #define AIM_WAIT(x) Sleep(x)
#endif

#ifdef __monitor__
  #ifndef AIM_WAIT /* may be done by application */
     #define AIM_WAIT(x)
  #endif
#endif

#if defined (_AIM_VME)
   #define SPRINTF(STRING) sprintf STRING;
#elif defined (_AIM_VXI)
   #define SPRINTF(STRING) sprintf STRING;
#elif defined(_AIM_WINDOWS) && defined(_AIM_SYSDRV)
  #define SPRINTF(STRING) sprintf STRING;
#elif defined(_SMARTLIB)
  #define SPRINTF(STRING) sprintf STRING;
#elif defined(_AIM_LINUX)
   #define SPRINTF(STRING) sprintf STRING
#elif defined(_AIM_QNX)
   #define SPRINTF(STRING) sprintf STRING
#elif defined(__monitor__)
   #define SPRINTF(STRING) sprintf STRING;
#endif

#endif /*if not defined __INI_DEFS__*/


