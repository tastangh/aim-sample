/* SPDX-FileCopyrightText: 2001-2024 AIM GmbH <info@aim-online.com> */
/* SPDX-License-Identifier: MIT OR GPL-2.0-or-later */

#ifndef __myPROTOTYPES__
#define __myPROTOTYPES__


/* ------ used in main.c*/

#ifdef __monitor__
  #ifndef __boardupdate__
    int main(int argc, char* argv[]);
  #endif
  #ifdef __boardupdate__
   int main2(int argc, char *argv[]);
  #endif
#endif

#if defined(_AIM_SYSDRV) && defined(__LYNXOS)
#else
  #ifdef __cplusplus
    extern "C" {
  #endif
#endif

#ifdef __monitor__
   #define MON_ONLY_PRINTF(...) printf(__VA_ARGS__)
#else
   #define MON_ONLY_PRINTF(...) 
#endif

int AI_Printf(const char *,...);
int MonPrintBatchCondition(TY_MONITOR_DATA_STRUCT *GLOBAL_VAR, const char * format, ...);

/* implemented in aim_monitor.c */
void* Api_memcpy( void * dst, const void * src,size_t count );


AiUInt32 start_hardware(  TY_MONITOR_DATA_STRUCT *GLOBAL_VAR);

/* ------ used in mon_prog.c*/
AiUInt32 read_PCIFPGA_REG_PMC ( TY_MONITOR_DATA_STRUCT *GLOBAL_VAR);
AiUInt32 start_ATMEL          ( TY_MONITOR_DATA_STRUCT *GLOBAL_VAR);
AiUInt32 read_NOVRAM_VAL_PMC  ( TY_MONITOR_DATA_STRUCT *GLOBAL_VAR);
AiUInt32 global_RAM_SIZE_PMC  ( TY_MONITOR_DATA_STRUCT *GLOBAL_VAR);
AiUInt32 global_RAM_TEST_PMC  ( TY_MONITOR_DATA_STRUCT *GLOBAL_VAR);
AiUInt32 global_RAM_TEST_full ( TY_MONITOR_DATA_STRUCT *GLOBAL_VAR);
AiUInt32 boot_IOFPGA_PMC      ( TY_MONITOR_DATA_STRUCT *GLOBAL_VAR);
AiUInt32 boot_BIP1_PMC        ( TY_MONITOR_DATA_STRUCT *GLOBAL_VAR);
AiUInt32 boot_BIP2_PMC        ( TY_MONITOR_DATA_STRUCT *GLOBAL_VAR);
AiUInt32 boot_ASP_PMC         ( TY_MONITOR_DATA_STRUCT *GLOBAL_VAR);
AiUInt32 start_BIP1_PMC       ( TY_MONITOR_DATA_STRUCT *GLOBAL_VAR);
AiUInt32 start_BIP2_PMC       ( TY_MONITOR_DATA_STRUCT *GLOBAL_VAR);
AiUInt32 start_ASP_PMC        ( TY_MONITOR_DATA_STRUCT *GLOBAL_VAR);
AiUInt32 program_tcp_novram   ( TY_MONITOR_DATA_STRUCT *GLOBAL_VAR);
AiUInt32 program_tcp_flash      ( TY_MONITOR_DATA_STRUCT *GLOBAL_VAR);
AiUInt32 read_TCP_flash       ( TY_MONITOR_DATA_STRUCT *GLOBAL_VAR);
AiUInt32 read_tcp_novram      ( TY_MONITOR_DATA_STRUCT *GLOBAL_VAR);
AiUInt32 set_IRIG_InternTime  ( TY_MONITOR_DATA_STRUCT *GLOBAL_VAR, AiUInt32 *leap_year,
                                 AiUInt32 *DayHi,AiUInt32 *DayLo, AiUInt32 *Hour ,AiUInt32 *Minute, AiUInt32 *Second);

/* ------ used in mon_misc.h*/
/*void   kbdflush               (void);*/
void   print_menu             ( TY_MONITOR_DATA_STRUCT *GLOBAL_VAR);
void   print_load_menu        ( TY_MONITOR_DATA_STRUCT *GLOBAL_VAR);
void   print_test_menu        ( TY_MONITOR_DATA_STRUCT *GLOBAL_VAR);
void   print_novram_menu      ( TY_MONITOR_DATA_STRUCT *GLOBAL_VAR);
void   print_open_menu        ( TY_MONITOR_DATA_STRUCT *GLOBAL_VAR);
AiUInt32 wait_clocks          ( TY_MONITOR_DATA_STRUCT *GLOBAL_VAR, AiUInt32 u_sec);
AiUInt32 search_data_pattern  ( TY_MONITOR_DATA_STRUCT *GLOBAL_VAR);

/* ------ used in ini_stio.c */
#if defined _AIM_1553_SYSDRV_USB || defined _AIM_429_SYSDRV_USB
    #define TcpOut(arg1, arg2, arg3)
    #define TcpIn(arg1, arg2, arg3)
#else
    AiUInt32 TcpOut           ( TY_MONITOR_DATA_STRUCT *GLOBAL_VAR, AiUInt32 port, AiUInt32 TCPData);
    AiUInt32 TcpIn            ( TY_MONITOR_DATA_STRUCT *GLOBAL_VAR, AiUInt32 port, AiUInt32 *TCPData);
#endif

AiUInt32 PCI_Mem_read         ( TY_MONITOR_DATA_STRUCT *GLOBAL_VAR, AiUInt32 Mem_loc);
AiUInt32 PCI_Mem_write        ( TY_MONITOR_DATA_STRUCT *GLOBAL_VAR, AiUInt32 Mem_loc, AiUInt32 data);
AiUInt32 PCI_Mem_write_flex   ( TY_MONITOR_DATA_STRUCT *GLOBAL_VAR, AiUInt32 Mem_loc, AiUInt32 data, AiUInt8 word_size);
AiUInt32 PCI_Mem_read_flex    ( TY_MONITOR_DATA_STRUCT *GLOBAL_VAR, AiUInt32 Mem_loc,AiUInt8 word_size);
AiUInt32 PCI_IOReg_read       ( TY_MONITOR_DATA_STRUCT *GLOBAL_VAR, AiUInt32 IO_Reg);
AiUInt32 PCI_IOReg_write      ( TY_MONITOR_DATA_STRUCT *GLOBAL_VAR, AiUInt32 IO_reg, AiUInt32 data);
AiUInt32 PCI_ASPRam_read      ( TY_MONITOR_DATA_STRUCT *GLOBAL_VAR, AiUInt32 IO_Reg);
AiUInt32 PCI_ASPRam_write     ( TY_MONITOR_DATA_STRUCT *GLOBAL_VAR, AiUInt32 IO_reg, AiUInt32 data);
AiUInt32 NOVRam_chksum        ( TY_MONITOR_DATA_STRUCT *GLOBAL_VAR);
AiUInt32 NOVRam_write         ( TY_MONITOR_DATA_STRUCT *GLOBAL_VAR, AiUInt32 nvByteAddr, AiUInt32 nvdata);
AiUInt32 NOVRam_read          ( TY_MONITOR_DATA_STRUCT *GLOBAL_VAR, AiUInt32 nvByteAddr);
void   NOVRam_default         ( TY_MONITOR_DATA_STRUCT *GLOBAL_VAR);
AiUInt32 update_NOVRAM        ( TY_MONITOR_DATA_STRUCT *GLOBAL_VAR);
AiUInt32 show_NOVRAM_values   ( TY_MONITOR_DATA_STRUCT *GLOBAL_VAR);

/* ------ used in prog_misc.c */

AiUInt32 get_ILCA1_EXT        ( TY_MONITOR_DATA_STRUCT *GLOBAL_VAR);
AiUInt32 get_ILCA2_EXT        ( TY_MONITOR_DATA_STRUCT *GLOBAL_VAR);
AiUInt32 get_BIP1_EXT         ( TY_MONITOR_DATA_STRUCT *GLOBAL_VAR);
AiUInt32 get_BIP2_EXT         ( TY_MONITOR_DATA_STRUCT *GLOBAL_VAR);
AiUInt32 get_PLCA_EXT         ( TY_MONITOR_DATA_STRUCT *GLOBAL_VAR);
AiUInt32 get_TCP_EXT          ( TY_MONITOR_DATA_STRUCT *GLOBAL_VAR);
AiUInt32 get_ASP_EXT          ( TY_MONITOR_DATA_STRUCT *GLOBAL_VAR);

void     read_board_values    ( TY_MONITOR_DATA_STRUCT *GLOBAL_VAR);      /* v01.08 added */   


AiUInt32 init_novram          ( TY_MONITOR_DATA_STRUCT *GLOBAL_VAR);
AiUInt32 put_novram_setup     ( TY_MONITOR_DATA_STRUCT *GLOBAL_VAR);
AiUInt32 enter_hw             ( TY_MONITOR_DATA_STRUCT *GLOBAL_VAR);
AiUInt32 ini_bite_adr_test    ( TY_MONITOR_DATA_STRUCT *GLOBAL_VAR, AiUInt32 StartAddress,AiUInt32 EndAddress);
AiUInt32 ini_bite_pat_test    ( TY_MONITOR_DATA_STRUCT *GLOBAL_VAR, AiUInt32 StartAddress,AiUInt32 EndAddress);
AiUInt32 ini_bite_walk0_test  ( TY_MONITOR_DATA_STRUCT *GLOBAL_VAR, AiUInt32 StartAddress,AiUInt32 EndAddress);
AiUInt32 ini_bite_walk1_test  ( TY_MONITOR_DATA_STRUCT *GLOBAL_VAR, AiUInt32 StartAddress,AiUInt32 EndAddress);
int    chat_to_int            ( char single_char);
AiUInt8  read_spi             ( TY_MONITOR_DATA_STRUCT *GLOBAL_VAR);
void   write_spi              ( TY_MONITOR_DATA_STRUCT *GLOBAL_VAR, AiUInt8 DByte);
AiUInt16 read_spi_flash       ( TY_MONITOR_DATA_STRUCT *GLOBAL_VAR, AiUInt32 address);
AiUInt32 read_32bit_spi_flash_mirror ( TY_MONITOR_DATA_STRUCT *GLOBAL_VAR, AiUInt32 address);
AiUInt8  read_spi_novram      ( TY_MONITOR_DATA_STRUCT *GLOBAL_VAR, AiUInt32 address);
AiUInt32 write_spi_novram     ( TY_MONITOR_DATA_STRUCT *GLOBAL_VAR, AiUInt32 address, AiUInt8 data);
AiUInt32 write_spi_flash      ( TY_MONITOR_DATA_STRUCT *GLOBAL_VAR, AiUInt32 address, AiUInt16 Data);
AiUInt32 spi_prog_enable      ( TY_MONITOR_DATA_STRUCT *GLOBAL_VAR);
void   erase_tcp              ( TY_MONITOR_DATA_STRUCT *GLOBAL_VAR);
AiUInt32 make_S3rec_line      ( AiInt8 *line,AiUInt8 *memory,  AiUInt32 address, AiUInt32 size);
AiUInt32 print_s3recs         ( TY_MONITOR_DATA_STRUCT *GLOBAL_VAR, AiUInt32 type, AiUInt8 *memblock, AiUInt32 memsize);
AiUInt32 Load_Srec            ( TY_MONITOR_DATA_STRUCT *GLOBAL_VAR, AiUInt32 type, AiUInt8 *buffer);
void   ShowBuffer               ( TY_MONITOR_DATA_STRUCT *GLOBAL_VAR, AiUInt32 buf,AiUInt32 count, AiUInt32 type);

_API_EXTERN unsigned long ul_LoadCode( void* driverData,  char ac_FileName[],
                                       unsigned char *puc_StartAddr, unsigned long *pul_Size,
                                       unsigned long ul_BufSize );
/*
_API_EXTERN unsigned long ul_LoadCode( unsigned long ul_BoardNr,  char ac_FileName[],
                                       unsigned char *puc_StartAddr, unsigned long *pul_Size,
                                       unsigned long ul_BufSize );
*/
#ifdef _AIM_WINDOWS
  _API_EXTERN unsigned long ul_WriteMemToFile( unsigned char ac_FileName[], unsigned char *puc_Addr,
                                               unsigned long ul_Size);
#endif

void     IRIG_EXTERN          ( TY_MONITOR_DATA_STRUCT *GLOBAL_VAR);   /* V01.08, ML, 17/09/2004 */
void     IRIG_INTERN          ( TY_MONITOR_DATA_STRUCT *GLOBAL_VAR); /* V01.08, ML, 17/09/2004 */
AiUInt32 RD_IRIG_SWITCH       ( TY_MONITOR_DATA_STRUCT *GLOBAL_VAR); /* V01.08, ML, 17/09/2004 */


AiUInt32 update_NOVRAM_FLASH  ( TY_MONITOR_DATA_STRUCT *GLOBAL_VAR);      /*V01.14 added */
AiUInt32 get_flash_mirror     ( TY_MONITOR_DATA_STRUCT *GLOBAL_VAR, AiUInt8 *data);
AiUInt32 prog_TCP_flash       ( TY_MONITOR_DATA_STRUCT *GLOBAL_VAR, AiUInt32 block, AiUInt32 signature, AiUInt8 *data);
AiUInt32 prog_TCP_FL_bits     ( TY_MONITOR_DATA_STRUCT *GLOBAL_VAR, AiUInt32 signature);                  /*V01.14 added */
AiUInt32 verify_TCP_flash     ( TY_MONITOR_DATA_STRUCT *GLOBAL_VAR, AiUInt8 *data);

void read_board_values        ( TY_MONITOR_DATA_STRUCT *GLOBAL_VAR);      /* v01.08 added */
void read_board_values_flash  ( TY_MONITOR_DATA_STRUCT *GLOBAL_VAR);      /* MMA */

AiUInt32 read_32bit_SPI_NVR   ( TY_MONITOR_DATA_STRUCT *GLOBAL_VAR, AiUInt32 address); /* v01.08 added */
AiUInt32 write_32bit_SPI_NVR  ( TY_MONITOR_DATA_STRUCT *GLOBAL_VAR, 
                                AiUInt32 address,AiUInt32 data); /*ml 01.08.2005 board update*/

#if defined(__monitor__) || defined(_SMARTLIB)
   /* ------ used in MonSpecific */
   AiInt16 AmcReadMemData        ( AiHandle hDrv, AiUInt8 uc_MemType, AiUInt32 ul_Offset, AiUInt8 uc_Width, void* pv_Data, AiUInt32 ul_Size, unsigned long DEVICE_TYPE_RD);
   AiInt16 AmcWriteMemData       ( AiHandle hDrv, AiUInt8 uc_MemType, AiUInt32 ul_Offset, AiUInt8 uc_Width, void* pv_Data, AiUInt32 ul_Size, unsigned long DEVICE_TYPE_WR);

   AiInt16 AmcJtagExecute        ( AiHandle hDrv );
   AiInt16 AmcProgramTCPFlash    ( AiHandle hDrv );

   AiUInt32 Save_Code            ( TY_MONITOR_DATA_STRUCT *GLOBAL_VAR, AiUInt32 type, AiUInt8 *srec, AiUInt32 size);
   AiUInt32 Load_Code            ( TY_MONITOR_DATA_STRUCT *GLOBAL_VAR, char file_name[MON_MAX_PATH], AiInt8 *srec, AiUInt8 usetemppath);

   AiUInt32 dump_GLOBAL_RAM      ( TY_MONITOR_DATA_STRUCT *GLOBAL_VAR);
   AiUInt32 dump_LOCAL_ASPRam    ( TY_MONITOR_DATA_STRUCT *GLOBAL_VAR);
   AiUInt32 dump_IO_REG          ( TY_MONITOR_DATA_STRUCT *GLOBAL_VAR);

   AiUInt32 fill_GLOBAL_RAM      ( TY_MONITOR_DATA_STRUCT *GLOBAL_VAR);
   AiUInt32 fill_LOCAL_ASPRam    ( TY_MONITOR_DATA_STRUCT *GLOBAL_VAR);

   AiUInt32 open_PMC_IO_REG      ( TY_MONITOR_DATA_STRUCT *GLOBAL_VAR);
   AiUInt32 debug_XSCALE         ( TY_MONITOR_DATA_STRUCT *GLOBAL_VAR);

   AiUInt32 jtag_loop            ( TY_MONITOR_DATA_STRUCT *GLOBAL_VAR);

   AiUInt32 open_GLOBAL_RAM      ( TY_MONITOR_DATA_STRUCT *GLOBAL_VAR);
   AiUInt32 fill_GLOBAL_RAM_flex ( TY_MONITOR_DATA_STRUCT *GLOBAL_VAR);
   AiUInt32 open_LOCAL_ASPRam    ( TY_MONITOR_DATA_STRUCT *GLOBAL_VAR);

   AiUInt32 read_ATMEL_Version   ( TY_MONITOR_DATA_STRUCT *GLOBAL_VAR);
   AiUInt32 read_ATMEL_IRIG_Reg  ( TY_MONITOR_DATA_STRUCT *GLOBAL_VAR);   /*v01.04 added */

   void ADC_ch_dump              ( TY_MONITOR_DATA_STRUCT *GLOBAL_VAR);   /* v01.09 added*/
   void read_NOVRam_loop         ( TY_MONITOR_DATA_STRUCT *GLOBAL_VAR);   /* v01.0a added*/
   void read_novram_spi          ( TY_MONITOR_DATA_STRUCT *GLOBAL_VAR);   /* v01.13 added*/

   /* ------ used in jtag.c */
   AiUInt8 jtag_putInstr         ( TY_MONITOR_DATA_STRUCT *GLOBAL_VAR, AiUInt32 com);
   void jtag_setPort             ( TY_MONITOR_DATA_STRUCT *GLOBAL_VAR, AiUInt8 port, AiUInt8 val);
   void jtag_pulseClock          ( TY_MONITOR_DATA_STRUCT *GLOBAL_VAR);
   void jtag_clockOutBit         ( TY_MONITOR_DATA_STRUCT *GLOBAL_VAR, AiUInt8 port, AiUInt8 val);
   AiInt32 jtag_value            ( lenVal *x);
   AiInt32 jtag_EqualLenVal      ( lenVal *expected, lenVal *actual, lenVal *mask);
   AiInt8 jtag_RetBit            ( lenVal *lv, AiInt32 byte, AiInt32 bit);
   void jtag_SetBit              ( lenVal *lv, AiInt32 byte, AiInt32 bit, AiInt16 val);
   void jtag_addVal              ( lenVal *resVal, lenVal *val1, lenVal *val2);
   void jtag_readVal             ( TY_MONITOR_DATA_STRUCT *GLOBAL_VAR, lenVal *x, AiInt16 numBytes, AiUInt32 *file_counter);
   void jtag_clockOutLenVal      ( TY_MONITOR_DATA_STRUCT *GLOBAL_VAR, lenVal *lv,AiInt32 numBits,lenVal *tdoStore);
   void jtag_doSDRMasking        ( lenVal *dataVal, lenVal *nextData, lenVal *addressMask,lenVal *dataMask);
   void jtag_gotoIdle            ( TY_MONITOR_DATA_STRUCT *GLOBAL_VAR);
   void jtag_gotoRESET           ( TY_MONITOR_DATA_STRUCT *GLOBAL_VAR);
   AiInt16 jtag_loadSDR          ( TY_MONITOR_DATA_STRUCT *GLOBAL_VAR, AiInt32 numBits, lenVal *dataVal, lenVal *TDOExpected, lenVal *TDOMask);
   AiUInt32 jtag_load            ( TY_MONITOR_DATA_STRUCT *GLOBAL_VAR);
   AiUInt32 jtag_execute         ( TY_MONITOR_DATA_STRUCT *GLOBAL_VAR);

   AiUInt32 jtag_execute1        ( TY_MONITOR_DATA_STRUCT *GLOBAL_VAR);
#endif

#if defined(_AIM_SYSDRV) && defined(__LYNXOS)
#else
  #ifdef __cplusplus
    }
  #endif
#endif


#if defined(__monitor__)
/**********************************************
   used in usb_prog.c
**********************************************/
   void NET2282_init(TY_MONITOR_DATA_STRUCT *GLOBAL_VAR, NET2282_TY *net2282_var);
   void NET2282_read_spi_flash(TY_MONITOR_DATA_STRUCT *GLOBAL_VAR, AiUInt16 bytecount, AiUInt8 *srec, NET2282_TY *net2282_var);
   void NET2282_read_spi_novram(TY_MONITOR_DATA_STRUCT *GLOBAL_VAR, AiUInt8 *srec, NET2282_TY *net2282_var);
   void NET2282_prog_TCP_flash(TY_MONITOR_DATA_STRUCT *GLOBAL_VAR, AiUInt16 start, AiUInt16 size, AiUInt8 *srec, NET2282_TY *net2282_var);
   void NET2282_write_spi_novram(TY_MONITOR_DATA_STRUCT *GLOBAL_VAR, AiUInt16 offset, AiUInt8 *srec, NET2282_TY *net2282_var);
   void readNET2282struct(TY_MONITOR_DATA_STRUCT *GLOBAL_VAR, NET2282_TY *net2282_var);
   void writeNET2282struct(TY_MONITOR_DATA_STRUCT *GLOBAL_VAR,NET2282_TY *net2282_var);
   void NET2282_update_novram(TY_MONITOR_DATA_STRUCT *GLOBAL_VAR);
   void NET2282_setnovramvalue(AiUInt32 value, AiUInt16 address, char *novram);
   AiUInt32 NET2282read_NOVRAM_VAL_PMC( TY_MONITOR_DATA_STRUCT *GLOBAL_VAR );
   void NET2282_batch_update_TCP(TY_MONITOR_DATA_STRUCT *GLOBAL_VAR);
   void NET2282_init_novram(TY_MONITOR_DATA_STRUCT *GLOBAL_VAR, NET2282_TY *net2282_var);

#ifndef APU_PROG_INTERFACE_H
#define APU_PROG_INTERFACE_H


typedef enum _e_ApuInterface{
   AI_APU_1553,
   AI_APU_429,
   AI_APU_FDX,
   AI_APU_FDXTAP
} TY_E_APUINTERFACE;

_API_EXTERN unsigned int  _API_DLL_FUNC uiOpenDevice(TY_E_APUINTERFACE e_Type);

_API_EXTERN void _API_DLL_FUNC vReadConfigRegArea(unsigned int *puiBuffer, unsigned int ulDWords, unsigned int ulStartAddr);
_API_EXTERN void _API_DLL_FUNC vWriteConfigRegArea(unsigned int *puiBuffer, unsigned int ulDWords, unsigned int ulStartAddr);

_API_EXTERN void _API_DLL_FUNC vRead8051MemoryArea(char *pcBuffer, unsigned int ulByteSize, unsigned int ulStartAddr);
_API_EXTERN void _API_DLL_FUNC vWrite8051MemoryArea(char *pcBuffer, unsigned int ulByteSize, unsigned int ulStartAddr);

_API_EXTERN int _API_DLL_FUNC bReadEEProm(char *pcFileName);
_API_EXTERN int _API_DLL_FUNC bWriteEEProm(char *pcFileName);


#endif //  APU_PROG_INTERFACE_H

/**********************************************
   used in usb_jtag.c
**********************************************/
AiUInt32 usb_jtag_execute(TY_MONITOR_DATA_STRUCT *GLOBAL_VAR);
AiUInt32 usb_jtag_execute1( TY_MONITOR_DATA_STRUCT *GLOBAL_VAR );
AiUInt32 usb_jtag_load            (TY_MONITOR_DATA_STRUCT *GLOBAL_VAR);
AiUInt32 usb_write_NET2282_command(TY_MONITOR_DATA_STRUCT *GLOBAL_VAR, AiUInt8 command, NET2282_TY net2282data);
AiUInt32 usb_write_NET2282_data   (TY_MONITOR_DATA_STRUCT *GLOBAL_VAR, AiUInt8 *data, AiUInt32 size, AiUInt16 address);
AiUInt8  usb_read_NET2282_status(TY_MONITOR_DATA_STRUCT *GLOBAL_VAR,/*AiUInt8 command,*/ AiUInt32 timeout);
AiUInt32 usb_read_NET2282_data   (TY_MONITOR_DATA_STRUCT *GLOBAL_VAR, AiUInt8 *data, AiUInt32 size, AiUInt16 address);
void  usb_read_NET2282_struct(TY_MONITOR_DATA_STRUCT *GLOBAL_VAR);

void caluculate_diff_time(TY_MONITOR_DATA_STRUCT *GLOBAL_VAR);

void DumpCharBufferAsByte(unsigned char *pcBuffer, unsigned int ulSize, unsigned int ulStartAddr) ;

void program_NET228x_firmware(TY_MONITOR_DATA_STRUCT *GLOBAL_VAR);

#endif /*#if defined(__monitor__)*/

#endif
