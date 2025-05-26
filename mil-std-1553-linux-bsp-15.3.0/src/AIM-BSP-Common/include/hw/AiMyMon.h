/* SPDX-FileCopyrightText: 2015-2021 AIM GmbH <info@aim-online.com> */
/* SPDX-License-Identifier: MIT OR GPL-2.0-or-later */

/*
Description: This module is used to access the on-board monitor (MyMon) on 
             AEC boards. 
*/

#ifndef __AI_MYMON_H__
#define __AI_MYMON_H__

#include "Ai_cdef.h"

#define MYMON_COMMUNICATON_VERSION   (1<<24)
#define MYMON_COMMUNICATON_REVISION  (0<<16)

#define MYMON_COMMUNICATION_MASK_VER  0xFF000000
#define MYMON_COMMUNICATION_MASK_REV  0x00FF0000



/* Defines used in the MyMon IO RAM located in the 
   FPGA. The MyMon registers start at offset 0x2000
*/

#define MYMON_REG_CONTROL    0x2000
#define MYMON_REG_INPUT_0    0x2004
#define MYMON_REG_INPUT_1    0x2008
#define MYMON_REG_INPUT_2    0x200C
#define MYMON_REG_INPUT_3    0x2010
#define MYMON_REG_INPUT_4    0x2014
#define MYMON_REG_INPUT_5    0x2018
#define MYMON_REG_INPUT_6    0x201C
#define MYMON_REG_STATUS     0x2020
#define MYMON_REG_OUTPUT_0   0x2024
#define MYMON_REG_OUTPUT_1   0x2028
#define MYMON_REG_OUTPUT_2   0x202C
#define MYMON_REG_OUTPUT_3   0x2030
#define MYMON_REG_OUTPUT_4   0x2034
#define MYMON_REG_OUTPUT_5   0x2038
#define MYMON_REG_BOOTSTATUS 0x203C


#define MYMON_STATUS_FREE    0x00400000
#define MYMON_STATUS_WORKING 0x00800000
#define MYMON_STATUS_MASK    0x00C00000
#define MYMON_STATUS_CMD     0xFF000000

#define MYMON_PASS              0x01

#define MYMON_CMD_MYMON_VER       0x01
#define MYMON_CMD_MBLAZE_VER      0x02
#define MYMON_CMD_DDRRAM_BASE     0x08
#define MYMON_CMD_BLOCKRAM_BASE   0x09
#define MYMON_CMD_PCIE_REG        0x0A
#define MYMON_CMD_PERIPHERAL_REG  0x0B
#define MYMON_CMD_RESET_SET       0x10
#define MYMON_CMD_RESET_LEAVE     0x11
#define MYMON_CMD_TEST            0x12
#define MYMON_CMD_START           0x13
#define MYMON_CMD_START_ALL       0x18
#define MYMON_CMD_MEM_FILL        0x20
#define MYMON_CMD_MEM_WRITE       0x21
#define MYMON_CMD_MEM_READ        0x22
#define MYMON_CMD_NOVRAM_CREATE   0x30
#define MYMON_CMD_NOVRAM_FLUSH    0x31
#define MYMON_CMD_NOVRAM_WRITE    0x32
#define MYMON_CMD_NOVRAM_READ     0x33
#define MYMON_CMD_IRIG_SET        0x40
#define MYMON_CMD_IRIG_GET        0x48
#define MYMON_CMD_IRIG_SWITCH     0x4C
#define MYMON_CMD_UPDATE_SET      0x50
#define MYMON_CMD_COUNTER_GET     0x58


#define MYMON_BOOT_READY_NOVRAM 0xCAFE0001 /* OP0 */
#define MYMON_BOOT_READY_BOOT   0xCAFE0002 /* OP1 */
#define MYMON_BOOT_READY_IRQ    0xCAFE0003 /* OP2 */

#define MYMON_COMPONENT_BIU1        1
#define MYMON_COMPONENT_BIU2        2
#define MYMON_COMPONENT_BIU3        3
#define MYMON_COMPONENT_BIU4        4
#define MYMON_COMPONENT_ASP         5
#define MYMON_COMPONENT_FPGA        6
#define MYMON_COMPONENT_MB          7
#define MYMON_COMPONENT_I2C_EEPROM  8





/* Defines used in the global extension RAM */

#define MYMON_GREG_VERSION_BIU1     0x40
#define MYMON_GREG_VERSION_BIU2     0x44
#define MYMON_GREG_VERSION_BIU3     0x48
#define MYMON_GREG_VERSION_BIU4     0x4C
#define MYMON_GREG_VERSION_ASP      0x50
#define MYMON_GREG_VERSION_FPGA     0x54
#define MYMON_GREG_VERSION_SOC      0x58
#define MYMON_GREG_VERSION_SPECIAL  0x5C
#define MYMON_GREG_VERSION_BIU1_IO  0x60
#define MYMON_GREG_VERSION_BIU2_IO  0x64
#define MYMON_GREG_VERSION_BIU3_IO  0x68
#define MYMON_GREG_VERSION_BIU4_IO  0x6C
#define MYMON_GREG_VERSION_PCIE     0x70
#define MYMON_GREG_VERSION_SP1      0x74
#define MYMON_GREG_VERSION_SP2      0x78
#define MYMON_GREG_VERSION_SP3      0x7C
#define MYMON_GREG_VERSION_MBLAZE   0x80
#define MYMON_GREG_VERSION_MYMON    0x84
#define MYMON_GREG_VERSION_COMM     0x88


/*! \def MYMON_GREG_ASP_STARTUP_IP
 * Byte offset of register used for ASP maintenance boot. IP address of the system is set here.
 */
#define MYMON_GREG_ASP_STARTUP_IP       0x100


/*! \def MYMON_GREG_ASP_STARTUP_NETMASK
 * Byte offset of register used for ASP maintenance boot. Subnetmask of the system is set here.
 */
#define MYMON_GREG_ASP_STARTUP_NETMASK  0x104



#define MYMON_GREG_NOVRAM_BASE_OFFSET   0xF400
#define MYMON_GREG_NOVRAM_MAGIC         0xF400
#define MYMON_GREG_NOVRAM_SERIAL        0xF404
#define MYMON_GREG_NOVRAM_BOARDCFG      0xF408
#define MYMON_GREG_NOVRAM_HWVARIANT     0xF40C
#define MYMON_GREG_NOVRAM_BOARDTYPE     0xF410
#define MYMON_GREG_NOVRAM_BOARDSUBTYP   0xF414
#define MYMON_GREG_NOVRAM_BOARDAPP_TYP  0xF418
#define MYMON_GREG_NOVRAM_PARTNO        0xF41C
#define MYMON_GREG_NOVRAM_PARTNO_EXT    0xF420
#define MYMON_GREG_NOVRAM_NOVENACODE    0xF424
#define MYMON_GREG_NOVRAM_FIRSTDATE     0xF428
#define MYMON_GREG_NOVRAM_ACTDATE       0xF42C
#define MYMON_GREG_NOVRAM_DUMPMODE      0xF430
#define MYMON_GREG_NOVRAM_SOC           0xF434
#define MYMON_GREG_NOVRAM_PHY1RAM_SIZE  0xF440
#define MYMON_GREG_NOVRAM_PHY2RAM_SIZE  0xF444
#define MYMON_GREG_NOVRAM_GRAM_SIZE     0xF450
#define MYMON_GREG_NOVRAM_MLCAEX        0xF470
#define MYMON_GREG_NOVRAM_TCPMONEX      0xF474
#define MYMON_GREG_NOVRAM_ASPEX         0xF478
#define MYMON_GREG_NOVRAM_RES0EX        0xF47C
#define MYMON_GREG_NOVRAM_BIP1EX        0xF480
#define MYMON_GREG_NOVRAM_BIP2EX        0xF484
#define MYMON_GREG_NOVRAM_XMC1EX        0xF488
#define MYMON_GREG_NOVRAM_RES2EX        0xF48C
#define MYMON_GREG_NOVRAM_IRIG_OFFSET   0xF4F4
#define MYMON_GREG_NOVRAM_CHECK_SUM     0xF4FC



#define MYMON_OK                         0x0000
#define MYMON_ERR_NO_CONTEXT             0x0001
#define MYMON_ERR_VERSION_INCOMPATIBLE   0x0002
#define MYMON_ERR_OLD_REVISION           0x0003
#define MYMON_ERR_MEM_FUNCTION_MISSING   0x0004
#define MYMON_ERR_MEM_ACCESS_FAILED      0x0005
#define MYMON_ERR_TIMEOUT                0x0006
#define MYMON_ERR_STATUS                 0x0007
#define MYMON_ERR_RANGE_CHECK            0x0008
#define MYMON_ERR_BUSY                   0x0009




struct TyMyMonUserData;
struct TyMyMonContext;

typedef AiBoolean (*TY_MYMON_MEM_FUNCTION) ( struct TyMyMonContext * pContext, AiUInt32 ulByteOffset, AiUInt32 ulData[], AiUInt32 ulLWCount );
typedef AiBoolean (*TY_MYMON_WAIT_FUNCTION)( struct TyMyMonContext * pContext, AiUInt32 ulMicroSeconds );

typedef struct TyMyMonContext 
{
    /* defined in user header */
    struct TyMyMonUserData * pxUserData;

    TY_MYMON_MEM_FUNCTION  pfWriteIo;
    TY_MYMON_MEM_FUNCTION  pfWriteGlobalExt;
    TY_MYMON_MEM_FUNCTION  pfReadIo;
    TY_MYMON_MEM_FUNCTION  pfReadGlobalExt;
    TY_MYMON_WAIT_FUNCTION pfWait;

} TyMyMonContext;




/* Additional functions */
#ifdef __cplusplus
extern "C"
{
#endif

AiInt16 MyMonCmdVersionCheck(    TyMyMonContext * pContext, AiUInt32 * pulDeviceVersion );

AiInt16 MyMonCmdExecute(         TyMyMonContext * pContext, 
                                 AiUInt32 ulCommand,
                                 AiUInt32 ulInputCount,  AiUInt32 ulInput[],
                                 AiUInt32 ulOutputCount, AiUInt32 ulOutput[] );

AiInt16 MyMonCmdExecuteAsync(TyMyMonContext * pContext,
                                AiUInt32 ulCommand,
                                AiUInt32 ulInputCount, AiUInt32 ulInput[]);

AiInt16 MyMonCmdExecuteAsyncGetStatus(TyMyMonContext * pContext,
                                AiUInt32 ulCommand,
                                AiUInt32 ulOutputCount, AiUInt32 ulOutput[]);



/* MyMON access functions */

AiInt16 MyMonCmdReadMyMonVersion(      TyMyMonContext * pContext, 
                                       AiUInt32       * pulVersion );

AiInt16 MyMonCmdReadMicroBlazeVersion( TyMyMonContext * pContext, 
                                       AiUInt32       * pulVersion );

AiInt16 MyMonCmdReadDDRRAMBase(        TyMyMonContext * pContext, 
                                       AiUInt32       * pulResult, 
                                       AiUInt32       * pulBaseAddress,
                                       AiUInt32       * pulEndAddress );

AiInt16 MyMonCmdReadBlockRAMBase(      TyMyMonContext * pContext, 
                                       AiUInt32       * pulResult, 
                                       AiUInt32       * pulBaseAddress,
                                       AiUInt32       * pulEndAddress );

AiInt16 MyMonCmdReadPCIEReg(          TyMyMonContext * pContext, 
                                       AiUInt32       * pulResult, 
                                       AiUInt32       * pulBaseAddress,
                                       AiUInt32       * pulEndAddress );

AiInt16 MyMonCmdReadPeripheralReg(     TyMyMonContext * pContext, 
                                       AiUInt32       * pulResult, 
                                       AiUInt32       * pulBaseAddress,
                                       AiUInt32       * pulEndAddress );

AiInt16 MyMonCmdComponentReset(        TyMyMonContext * pContext, 
                                       AiUInt32         ulComponent,
                                       AiUInt32       * pulStatus );

AiInt16 MyMonCmdComponentLeaveReset(   TyMyMonContext * pContext, 
                                       AiUInt32         ulComponent,
                                       AiUInt32       * pulStatus );

AiInt16 MyMonCmdComponentTest(         TyMyMonContext * pContext, 
                                       AiUInt32         ulComponent,
                                       AiUInt32       * pulStatus );

AiInt16 MyMonCmdComponentStart(        TyMyMonContext * pContext, 
                                       AiUInt32         ulComponent,
                                       AiUInt32       * pulStatus );

AiInt16 MyMonCmdDeviceStartAll(        TyMyMonContext * pContext,
                                       AiUInt32       * pulStatus );

AiInt16 MyMonCmdMemoryFill(            TyMyMonContext * pContext,
                                       AiUInt32         ulStartAddress,
                                       AiUInt32         ulEndAddress,
                                       AiUInt32         ulPattern,
                                       AiUInt32       * pulStatus );

AiInt16 MyMonCmdMemoryWrite(           TyMyMonContext * pContext,
                                       AiUInt32         ulAddress,
                                       AiUInt32         ulPattern,
                                       AiUInt32       * pulStatus );

AiInt16 MyMonCmdMemoryRead(            TyMyMonContext * pContext,
                                       AiUInt32         ulAddress,
                                       AiUInt32       * pulPattern );

AiInt16 MyMonCmdNovramMirrorCreate(    TyMyMonContext * pContext,
                                       AiUInt32       * pulStatus );

AiInt16 MyMonCmdNovramMirrorFlush(     TyMyMonContext * pContext,
                                       AiUInt32       * pulStatus );

AiInt16 MyMonCmdNovramMirrorWrite(     TyMyMonContext * pContext,
                                       AiUInt32         ulOffset,
                                       AiUInt32         pulValue,
                                       AiUInt32       * pulStatus );

AiInt16 MyMonCmdNovramMirrorRead(      TyMyMonContext * pContext,
                                       AiUInt32         ulOffset,
                                       AiUInt32       * pulValue );

AiInt16 MyMonCmdIrigSet(               TyMyMonContext * pContext, 
                                       AiUInt16         uwDay, 
                                       AiUInt16         uwHour, 
                                       AiUInt16         uwMinute, 
                                       AiUInt16         uwSecond, 
                                       AiBoolean        bLeapYear );

AiInt16 MyMonCmdIrigGet(               TyMyMonContext * pContext, 
                                       AiUInt16       * puwDay, 
                                       AiUInt16       * puwHour, 
                                       AiUInt16       * puwMinute, 
                                       AiUInt16       * puwSecond, 
                                       AiUInt16       * puwMicro, 
                                       AiUInt32       * pulStatus );

AiInt16 MyMonCmdIrigSwitch(            TyMyMonContext * pContext,
                                       AiUInt32         ulSwitch,
                                       AiUInt32       * pulStatus,
                                       AiUInt32       * pulSwitch );

AiInt16 MyMonCmdComponentUpdate(       TyMyMonContext * pContext, 
                                       AiUInt32         ulComponent, 
                                       AiUInt32         ulMemOffset, 
                                       AiUInt32         ulSize, 
                                       AiUInt32         ulMemOffsetDecoded, 
                                       AiUInt32         ulMaxSizeDecoded, 
                                       AiUInt32       * pulReturn,
                                       AiUInt32       * pulStatus );

AiInt16 MyMonCmdComponentUpdateAsync(TyMyMonContext * pContext,
                                       AiUInt32         ulComponent,
                                       AiUInt32         ulMemOffset,
                                       AiUInt32         ulSize,
                                       AiUInt32         ulMemOffsetDecoded,
                                       AiUInt32         ulMaxSizeDecoded);

AiInt16 MyMonCmdComponentUpdateAsyncCheck(TyMyMonContext * pContext,
                                          AiUInt32       * pulReturn,
                                          AiUInt32       * pulStatus);

AiInt16 MyMonCmdCounterGet(            TyMyMonContext * pContext, 
                                       AiUInt8        * pucTemp1High,
                                       AiUInt8        * pucTemp1Lowest,
                                       AiUInt8        * pucTemp1Average,
                                       AiUInt8        * pucTemp2High,
                                       AiUInt8        * pucTemp2Lowest,
                                       AiUInt8        * pucTemp2Average,
                                       AiUInt32       * pulTimestampCount );

#ifdef __cplusplus
}
#endif



#endif /* __AI_MYMON_H__ */







