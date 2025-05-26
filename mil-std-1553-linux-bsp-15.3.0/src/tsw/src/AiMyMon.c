/* SPDX-FileCopyrightText: 2010-2022 AIM GmbH <info@aim-online.com> */
/* SPDX-License-Identifier: MIT OR GPL-2.0-or-later */

/*! \file AiMyMon.c
        This file implements the AyE micro monitor communicaton.
*/

#include "hw/AiMyMon.h"


/*

*/
AiInt16 MyMonCmdVersionCheck( TyMyMonContext * pContext, AiUInt32 * pulDeviceVersion )
{
    AiBoolean bRetVal = AiFalse;
    AiUInt32 ulValue;

    if( pulDeviceVersion ) *pulDeviceVersion = 0;

    if( !pContext )
        return MYMON_ERR_NO_CONTEXT;

    if( !pContext->pfReadGlobalExt )
        return MYMON_ERR_MEM_FUNCTION_MISSING;

    bRetVal = pContext->pfReadGlobalExt( pContext, MYMON_GREG_VERSION_COMM, &ulValue, 1 );

    if( !bRetVal ) 
        return MYMON_ERR_MEM_ACCESS_FAILED;


    if( pulDeviceVersion ) *pulDeviceVersion = ulValue;


    if( (ulValue & MYMON_COMMUNICATION_MASK_VER) != MYMON_COMMUNICATON_VERSION )
        /* error incompatible version */
        return MYMON_ERR_VERSION_INCOMPATIBLE;
    else if( (ulValue & MYMON_COMMUNICATION_MASK_REV) >= (MYMON_COMMUNICATON_REVISION + 1) )
        /* error old revision */
        return MYMON_ERR_OLD_REVISION;

    return MYMON_OK; /* compatible */
}


AiInt16 MyMonCmdExecute(  TyMyMonContext * pContext, 
                          AiUInt32 ulCommand,
                          AiUInt32 ulInputCount,  AiUInt32 ulInput[],
                          AiUInt32 ulOutputCount, AiUInt32 ulOutput[] )
{
    AiUInt32  i             = 0;
    AiBoolean bRetVal       = AiFalse;
    AiUInt32  ulValue       = 0;
    AiUInt32  ulCommandZero = 0;


    /* check if no other process is running */
    /* multiple processes are not supported */ 


    /* copy input data into registers */

    bRetVal = pContext->pfWriteIo( pContext, MYMON_REG_INPUT_0, ulInput, ulInputCount );

    if( !bRetVal ) return MYMON_ERR_MEM_ACCESS_FAILED;

    /* execute command */

    ulCommand = ulCommand << 24;

    bRetVal = pContext->pfWriteIo( pContext, MYMON_REG_CONTROL, &ulCommand, 1 );

    if( !bRetVal ) return MYMON_ERR_MEM_ACCESS_FAILED;

    pContext->pfWait( pContext, 100 );

    /* wait until command is finished */ 
    
    for( i=0; i<1000000; i++ )
    {
        bRetVal = pContext->pfReadIo( pContext, MYMON_REG_STATUS, &ulValue, 1 );

        if( !bRetVal ) return MYMON_ERR_MEM_ACCESS_FAILED;

        if(    ( (ulValue & MYMON_STATUS_MASK) == MYMON_STATUS_FREE )
            && ( (ulValue & MYMON_STATUS_CMD)  == ulCommand         ) )
            /* finished */
            break;

        pContext->pfWait( pContext, 100 );
    }

    if( i == 1000000 )
        /* timeout occured */
        return MYMON_ERR_TIMEOUT;

    /* copy return data */

    if( ulOutputCount > 0 )
       bRetVal = pContext->pfReadIo( pContext, MYMON_REG_OUTPUT_0, ulOutput, ulOutputCount );

    if( !bRetVal ) return MYMON_ERR_MEM_ACCESS_FAILED;


    /* -- execute command 0 to reset the status --- */
    bRetVal = pContext->pfWriteIo( pContext, MYMON_REG_CONTROL, &ulCommandZero, 1 );


    return MYMON_OK;
}









AiInt16 MyMonCmdReadMyMonVersion(      TyMyMonContext * pContext, 
                                       AiUInt32       * pulVersion )
{
    AiUInt32 ulValue = 0;
    AiInt16  wRetVal = 0;

    wRetVal = MyMonCmdExecute( pContext, MYMON_CMD_MYMON_VER, 0, 0, 1, &ulValue );

    if( pulVersion )
        *pulVersion = ulValue;

    return wRetVal;
}




AiInt16 MyMonCmdReadMicroBlazeVersion( TyMyMonContext * pContext, 
                                       AiUInt32       * pulVersion )
{
    AiUInt32 ulValue = 0;
    AiInt16  wRetVal = 0;

    wRetVal = MyMonCmdExecute( pContext, MYMON_CMD_MBLAZE_VER, 0, 0, 1, &ulValue );

    if( pulVersion )
        *pulVersion = ulValue;

    return wRetVal;
}


AiInt16 MyMonCmdReadDDRRAMBase(        TyMyMonContext * pContext, 
                                       AiUInt32       * pulResult, 
                                       AiUInt32       * pulBaseAddress,
                                       AiUInt32       * pulEndAddress )
{
    AiInt16  wRetVal    = 0;
    AiUInt32 ulValue[3] = {0,0,0};

    wRetVal = MyMonCmdExecute( pContext, MYMON_CMD_DDRRAM_BASE, 0, 0, 3, ulValue );

    if( pulResult )      *pulResult      = ulValue[0];
    if( pulBaseAddress ) *pulBaseAddress = ulValue[1];
    if( pulEndAddress )  *pulEndAddress  = ulValue[2];

    return wRetVal;

}



AiInt16 MyMonCmdReadBlockRAMBase(      TyMyMonContext * pContext, 
                                       AiUInt32       * pulResult, 
                                       AiUInt32       * pulBaseAddress,
                                       AiUInt32       * pulEndAddress )
{
    AiInt16  wRetVal    = 0;
    AiUInt32 ulValue[3] = {0,0,0};

    wRetVal = MyMonCmdExecute( pContext, MYMON_CMD_BLOCKRAM_BASE, 0, 0, 3, ulValue );

    if( pulResult )      *pulResult      = ulValue[0];
    if( pulBaseAddress ) *pulBaseAddress = ulValue[1];
    if( pulEndAddress )  *pulEndAddress  = ulValue[2];

    return wRetVal;

}


AiInt16 MyMonCmdReadPCIEReg(      TyMyMonContext * pContext, 
                                  AiUInt32       * pulResult, 
                                  AiUInt32       * pulBaseAddress,
                                  AiUInt32       * pulEndAddress )
{
    AiInt16  wRetVal    = 0;
    AiUInt32 ulValue[3] = {0,0,0};

    wRetVal = MyMonCmdExecute( pContext, MYMON_CMD_PCIE_REG, 0, 0, 3, ulValue );

    if( pulResult )      *pulResult      = ulValue[0];
    if( pulBaseAddress ) *pulBaseAddress = ulValue[1];
    if( pulEndAddress )  *pulEndAddress  = ulValue[2];

    return wRetVal;

}



AiInt16 MyMonCmdReadPeripheralReg(     TyMyMonContext * pContext, 
                                       AiUInt32       * pulResult, 
                                       AiUInt32       * pulBaseAddress,
                                       AiUInt32       * pulEndAddress )
{
    AiInt16  wRetVal    = 0;
    AiUInt32 ulValue[3] = {0,0,0};

    wRetVal = MyMonCmdExecute( pContext, MYMON_CMD_PERIPHERAL_REG, 0, 0, 3, ulValue );

    if( pulResult )      *pulResult      = ulValue[0];
    if( pulBaseAddress ) *pulBaseAddress = ulValue[1];
    if( pulEndAddress )  *pulEndAddress  = ulValue[2];

    return wRetVal;

}



AiInt16 MyMonCmdComponentReset( TyMyMonContext * pContext, 
                                AiUInt32         ulComponent,
                                AiUInt32       * pulStatus )
{
    AiUInt16 wRetVal;
    AiUInt32 ulValue;


    wRetVal = MyMonCmdExecute( pContext, MYMON_CMD_RESET_SET, 1, &ulComponent, 1, &ulValue );

    if( pulStatus )
        *pulStatus = ulValue;

    if(    ( wRetVal  == MYMON_OK   )
        && ( ulValue  != MYMON_PASS ) )
    {
        wRetVal = MYMON_ERR_STATUS;
    }

    return wRetVal;
}


AiInt16 MyMonCmdComponentLeaveReset( TyMyMonContext * pContext, AiUInt32 ulComponent, AiUInt32 * pulStatus )
{
    AiUInt16 wRetVal;
    AiUInt32 ulValue;

    wRetVal = MyMonCmdExecute( pContext, MYMON_CMD_RESET_LEAVE, 1, &ulComponent, 1, &ulValue );

    if( pulStatus )
        *pulStatus = ulValue;

    if(    ( wRetVal  == MYMON_OK   )
        && ( ulValue  != MYMON_PASS ) )
    {
        wRetVal = MYMON_ERR_STATUS;
    }

    return wRetVal;
}


AiInt16 MyMonCmdComponentTest( TyMyMonContext * pContext, AiUInt32 ulComponent, AiUInt32 * pulStatus )
{
    AiUInt16 wRetVal;
    AiUInt32 ulValue;

    wRetVal = MyMonCmdExecute( pContext, MYMON_CMD_TEST, 1, &ulComponent, 1, &ulValue );

    if( pulStatus )
        *pulStatus = ulValue;

    if(    ( wRetVal  == MYMON_OK   )
        && ( ulValue  != MYMON_PASS ) )
    {
        wRetVal = MYMON_ERR_STATUS;
    }

    return wRetVal;
}


AiInt16 MyMonCmdComponentStart( TyMyMonContext * pContext, 
                                AiUInt32         ulComponent,
                                AiUInt32       * pulStatus )
{
    AiUInt16 wRetVal;
    AiUInt32 ulValue;

    wRetVal = MyMonCmdExecute( pContext, MYMON_CMD_START, 1, &ulComponent, 1, &ulValue );

    if( pulStatus )
        *pulStatus = ulValue;

    if(    ( wRetVal  == MYMON_OK   )
        && ( ulValue  != MYMON_PASS ) )
    {
        wRetVal = MYMON_ERR_STATUS;
    }

    return wRetVal;
}


AiInt16 MyMonCmdDeviceStartAll( TyMyMonContext * pContext, AiUInt32 * pulStatus )
{
    AiUInt32 ulInput = 0;
    AiUInt32 ulValue = 0;
    AiInt16  wRetVal = 0;

    wRetVal = MyMonCmdExecute( pContext, MYMON_CMD_START_ALL, 0, &ulInput, 1, &ulValue );

    if( pulStatus )
        *pulStatus = ulValue;

    if(    ( wRetVal  == MYMON_OK   )
        && ( ulValue  != MYMON_PASS ) )
    {
        wRetVal = MYMON_ERR_STATUS;
    }

    return wRetVal;
}



AiInt16 MyMonCmdMemoryFill( TyMyMonContext * pContext,
                            AiUInt32         ulStartAddress,
                            AiUInt32         ulEndAddress,
                            AiUInt32         ulPattern,
                            AiUInt32       * pulStatus )
{
    AiUInt16 wRetVal    = 0;
    AiUInt32 ulOutput   = 0;
    AiUInt32 ulInput[3] = {0,0,0};

    ulInput[0] = ulStartAddress;
    ulInput[1] = ulEndAddress;
    ulInput[2] = ulPattern;

    wRetVal = MyMonCmdExecute( pContext, MYMON_CMD_MEM_FILL, 3, ulInput, 1, &ulOutput );

    if( pulStatus )
        *pulStatus = ulOutput;

    if(    ( wRetVal  == MYMON_OK   )
        && ( ulOutput != MYMON_PASS ) )
    {
        wRetVal = MYMON_ERR_STATUS;
    }


    return wRetVal;
}



AiInt16 MyMonCmdMemoryWrite(           TyMyMonContext * pContext,
                                       AiUInt32         ulAddress,
                                       AiUInt32         ulPattern,
                                       AiUInt32       * pulStatus )
{
    AiUInt16 wRetVal    = 0;
    AiUInt32 ulOutput   = 0;
    AiUInt32 ulInput[2] = {0,0};

    ulInput[0] = ulAddress;
    ulInput[1] = ulPattern;

    wRetVal = MyMonCmdExecute( pContext, MYMON_CMD_MEM_WRITE, 2, ulInput, 1, &ulOutput );

    if( pulStatus )
        *pulStatus = ulOutput;

    if(    ( wRetVal  == MYMON_OK   )
        && ( ulOutput != MYMON_PASS ) )
    {
        wRetVal = MYMON_ERR_STATUS;
    }

    return wRetVal;
}

AiInt16 MyMonCmdMemoryRead(            TyMyMonContext * pContext,
                                       AiUInt32         ulAddress,
                                       AiUInt32       * pulPattern )
{
    AiUInt16 wRetVal    = 0;
    AiUInt32 ulOutput   = 0;
    AiUInt32 ulInput    = 0;

    ulInput = ulAddress;

    wRetVal = MyMonCmdExecute( pContext, MYMON_CMD_MEM_READ, 1, &ulInput, 1, &ulOutput );

    if( pulPattern )
        *pulPattern = ulOutput;

    return wRetVal;
}


AiInt16 MyMonCmdNovramMirrorCreate(    TyMyMonContext * pContext,
                                       AiUInt32       * pulStatus )
{
    AiUInt16 wRetVal    = 0;
    AiUInt32 ulOutput   = 0;

    wRetVal = MyMonCmdExecute( pContext, MYMON_CMD_NOVRAM_CREATE, 0, 0, 1, &ulOutput );

    if( pulStatus )
        *pulStatus = ulOutput;

    if(    ( wRetVal  == MYMON_OK   )
        && ( ulOutput != MYMON_PASS ) )
    {
        wRetVal = MYMON_ERR_STATUS;
    }


    return wRetVal;
}

AiInt16 MyMonCmdNovramMirrorFlush(     TyMyMonContext * pContext,
                                       AiUInt32       * pulStatus )
{
    AiUInt16 wRetVal    = 0;
    AiUInt32 ulOutput   = 0;

    wRetVal = MyMonCmdExecute( pContext, MYMON_CMD_NOVRAM_FLUSH, 0, 0, 1, &ulOutput );

    if( pulStatus )
        *pulStatus = ulOutput;

    if(    ( wRetVal  == MYMON_OK   )
        && ( ulOutput != MYMON_PASS ) )
    {
        wRetVal = MYMON_ERR_STATUS;
    }

    return wRetVal;
}

AiInt16 MyMonCmdNovramMirrorWrite(     TyMyMonContext * pContext,
                                       AiUInt32         ulOffset,
                                       AiUInt32         ulValue,
                                       AiUInt32       * pulStatus )
{
    AiUInt16 wRetVal    = 0;
    AiUInt32 ulOutput   = 0;
    AiUInt32 ulInput[2] = {0,0};

    ulInput[0] = ulOffset;
    ulInput[1] = ulValue;

    wRetVal = MyMonCmdExecute( pContext, MYMON_CMD_NOVRAM_WRITE, 2, ulInput, 1, &ulOutput );

    if( pulStatus )
        *pulStatus = ulOutput;

    if(    ( wRetVal  == MYMON_OK   )
        && ( ulOutput != MYMON_PASS ) )
    {
        wRetVal = MYMON_ERR_STATUS;
    }

    return wRetVal;
}

AiInt16 MyMonCmdNovramMirrorRead(      TyMyMonContext * pContext,
                                       AiUInt32         ulOffset,
                                       AiUInt32       * pulValue )
{
    AiUInt16 wRetVal    = 0;
    AiUInt32 ulOutput   = 0;
    AiUInt32 ulInput    = 0;

    ulInput = ulOffset;

    wRetVal = MyMonCmdExecute( pContext, MYMON_CMD_NOVRAM_READ, 1, &ulInput, 1, &ulOutput );

    if( pulValue )
        *pulValue = ulOutput;

    return wRetVal;
}



AiInt16 MyMonCmdIrigSet( TyMyMonContext * pContext, 
                         AiUInt16         uwDay, 
                         AiUInt16         uwHour, 
                         AiUInt16         uwMinute, 
                         AiUInt16         uwSecond, 
                         AiBoolean        bLeapYear )
{
    AiUInt16 wRetVal;
    AiUInt32 ulInput[3];

    ulInput[0] = 0;     /* time                */ 
    ulInput[1] = 0;     /* IEEE1344 (reserved) */
    ulInput[2] = 0;     /* update + leap Year  */

    if( uwSecond >  59 ) return MYMON_ERR_RANGE_CHECK;
    if( uwMinute >  59 ) return MYMON_ERR_RANGE_CHECK;
    if( uwHour   >  23 ) return MYMON_ERR_RANGE_CHECK;
    if( uwDay    > 366 ) return MYMON_ERR_RANGE_CHECK;


    ulInput[0] |= uwSecond;
    ulInput[0] |= uwMinute <<  6;
    ulInput[0] |= uwHour   << 12;
    ulInput[0] |= uwDay    << 17;

    ulInput[2]  = 1ul<<31;

    if( bLeapYear )
        ulInput[2]  |= 1<<3;

    wRetVal = MyMonCmdExecute( pContext, MYMON_CMD_IRIG_SET, 3, ulInput, 0, 0 );

    return wRetVal;
}

AiInt16 MyMonCmdIrigGet(  TyMyMonContext * pContext, 
                          AiUInt16       * puwDay, 
                          AiUInt16       * puwHour, 
                          AiUInt16       * puwMinute, 
                          AiUInt16       * puwSecond, 
                          AiUInt16       * puwMicro, 
                          AiUInt32       * pulStatus )
{
    AiUInt16 wRetVal;
    AiUInt32 ulOutput[3] = {0,0,0};

    wRetVal = MyMonCmdExecute( pContext, MYMON_CMD_IRIG_GET, 0, 0, 3, ulOutput );

    if(puwSecond) *puwSecond = (ulOutput[0] >>  0) & 0x0003F;
    if(puwMinute) *puwMinute = (ulOutput[0] >>  6) & 0x0003F;
    if(puwHour)   *puwHour   = (ulOutput[0] >> 12) & 0x0001F;
    if(puwDay)    *puwDay    = (ulOutput[0] >> 17) & 0x001FF;
    if(puwMicro)  *puwMicro  = (ulOutput[1] >>  0) & 0xFFFFF;

    if(pulStatus) *pulStatus = ulOutput[2];

    return wRetVal;
}





AiInt16 MyMonCmdIrigSwitch(            TyMyMonContext * pContext,
                                       AiUInt32         ulSwitch,
                                       AiUInt32       * pulStatus,
                                       AiUInt32       * pulSwitch )
{
    AiInt16  wRetVal     = 0;
    AiUInt32 ulInput     = 0;
    AiUInt32 ulOutput[2] = {0,0};


    ulInput = ulSwitch;

    wRetVal = MyMonCmdExecute( pContext, MYMON_CMD_IRIG_SWITCH, 1, &ulInput, 2, ulOutput );

    if( pulStatus ) *pulStatus  = ulOutput[0];
    if( pulSwitch ) *pulSwitch  = ulOutput[1];

    if(    ( wRetVal     == MYMON_OK   )
        && ( ulOutput[0] != MYMON_PASS ) )
    {
        wRetVal = MYMON_ERR_STATUS;
    }


    return wRetVal;

}


AiInt16 MyMonCmdComponentUpdate(  TyMyMonContext * pContext, 
                                  AiUInt32         ulComponent, 
                                  AiUInt32         ulMemOffset, 
                                  AiUInt32         ulSize, 
                                 AiUInt32          ulMemOffsetDecoded, 
                                 AiUInt32          ulMaxSizeDecoded, 
                                  AiUInt32       * pulReturn,
                                  AiUInt32       * pulStatus  )
{
    AiUInt16 wRetVal;
    AiUInt32 ulInput[5];
    AiUInt32 ulOutput[2];

    ulInput[0] = ulComponent;
    ulInput[1] = ulMemOffset;
    ulInput[2] = ulSize;
    ulInput[3] = ulMemOffsetDecoded;
    ulInput[4] = ulMaxSizeDecoded;

    wRetVal = MyMonCmdExecute( pContext, MYMON_CMD_UPDATE_SET, 5, ulInput, 2, ulOutput);

    if( pulReturn )
        *pulReturn = ulOutput[0];

    if( pulStatus )
        *pulStatus = ulOutput[1];

    if(    ( wRetVal     == MYMON_OK   )
        && ( ulOutput[0] != MYMON_PASS ) )
    {
        wRetVal = MYMON_ERR_STATUS;
    }


    return wRetVal;
}



AiInt16 MyMonCmdCounterGet( TyMyMonContext * pContext, 
                            AiUInt8        * pucTemp1High,
                            AiUInt8        * pucTemp1Lowest,
                            AiUInt8        * pucTemp1Average,
                            AiUInt8        * pucTemp2High,
                            AiUInt8        * pucTemp2Lowest,
                            AiUInt8        * pucTemp2Average,
                            AiUInt32       * pulTimestampCount )
{
    AiUInt16 wRetVal;
    AiUInt32 ulOutput[3] = {0,0,0};

    wRetVal = MyMonCmdExecute( pContext, MYMON_CMD_COUNTER_GET, 0, 0, 3, ulOutput);

    if( pucTemp1High )      *pucTemp1High      = (ulOutput[0]       ) & 0x7F;
    if( pucTemp1Lowest )    *pucTemp1Lowest    = (ulOutput[0] >>  8 ) & 0x7F;
    if( pucTemp1Average )   *pucTemp1Average   = (ulOutput[0] >> 24 ) & 0x7F;
    if( pucTemp2High )      *pucTemp2High      = (ulOutput[1]       ) & 0x7F;
    if( pucTemp2Lowest )    *pucTemp2Lowest    = (ulOutput[1] >>  8 ) & 0x7F;
    if( pucTemp2Average )   *pucTemp2Average   = (ulOutput[1] >> 24 ) & 0x7F;
    if( pulTimestampCount ) *pulTimestampCount = ulOutput[2];

    return wRetVal;
}







