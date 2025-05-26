/* SPDX-FileCopyrightText: 2021 AIM GmbH <info@aim-online.com> */
/* SPDX-License-Identifier: MIT  */

/*
Workfile:   AiArtix7Mon.c
*/


#include "hw/AiHwArtix7.h"
#include "hw/AiMyMon.h"
#include "Wrapper.h"
#include "AiOs.h"


AiReturn AiArtix7MonCmdExecute(AiUInt32 ul_ModuleHandle,
                          AiUInt32 ulCommand,
                          AiUInt32 ulInputCount,  AiUInt32 ulInput[],
                          AiUInt32 ulOutputCount, AiUInt32 ulOutput[] )
{
    AiUInt32  i             = 0;
    AiReturn  ret;
    AiUInt32  ulValue       = 0;
    AiUInt32  ulCommandZero = 0;


    /* check if no other process is running */
    /* multiple processes are not supported */ 


    /* copy input data into registers */

    ret = SlWrapWriteBlockMemData(ul_ModuleHandle, 3, ARTIX7_REG_TCP_INPUT_0*4, 4, ulInputCount, ulInput);

    if( ret ) return ARTIX7_MON_ERR_MEM_ACCESS_FAILED;

    /* execute command */

    ulCommand = ulCommand << 24;

    ret = SlWrapWriteBlockMemData(ul_ModuleHandle, 3, ARTIX7_REG_TCP_CONTROL*4, 4, 1, &ulCommand);

    if (ret) return ARTIX7_MON_ERR_MEM_ACCESS_FAILED;

    AiOsSleep( 100 );

    /* wait until command is finished */ 
    
    for( i=0; i<1000000; i++ )
    {
        ret = SlWrapReadBlockMemData(ul_ModuleHandle, 3, ARTIX7_REG_TCP_STATUS*4, 4, 1, &ulValue);

        if (ret) return ARTIX7_MON_ERR_MEM_ACCESS_FAILED;

        if (((ulValue & ARTIX7_MON_STATUS_MASK) == ARTIX7_MON_STATUS_FREE)
            && ((ulValue & ARTIX7_MON_STATUS_CMD)  == ulCommand))
            /* finished */
            break;

        AiOsSleep( 100 );
    }

    if( i == 1000000 )
        /* timeout occured */
        return ARTIX7_MON_ERR_TIMEOUT;

    /* copy return data */

    if( ulOutputCount > 0 )
        ret = SlWrapReadBlockMemData(ul_ModuleHandle, 3, ARTIX7_REG_TCP_OUTPUT_0*4, 4, ulOutputCount, ulOutput);
    
    if (ret) return ARTIX7_MON_ERR_MEM_ACCESS_FAILED;


    /* -- execute command 0 to reset the status --- */
    SlWrapWriteBlockMemData(ul_ModuleHandle, 3, ARTIX7_REG_TCP_CONTROL*4, 4, 1, &ulCommandZero);


    return ARTIX7_MON_OK;
}





AiReturn AiArtix7MonCmdFlashPartitionErase(AiUInt32 ul_ModuleHandle,
    AiUInt32         ulPartition,
    AiUInt32       * pulStatus)
{
    AiReturn ret = 0;
    AiUInt32 ulInput[1] = {0};
    AiUInt32 ulOutput[1] ={0};

    ulInput[0] = ulPartition;

    ret = AiArtix7MonCmdExecute(ul_ModuleHandle, ARTIX7_MON_CMD_FLASH_SECTOR_ERASE, 1, ulInput, 1, ulOutput);

    if   ((ret         == ARTIX7_MON_OK)
       && (ulOutput[0] != ARTIX7_MON_PASS))
    {
        ret = ARTIX7_MON_ERR_STATUS;
    }

    if (pulStatus)
        *pulStatus = ulOutput[0];

    return ret;
}

AiReturn AiArtix7MonCmdBitfileUpdateChunk(AiUInt32 ul_ModuleHandle,
    AiUInt32         ulPartition,
    AiUInt32         ulFlashOffset,
    AiUInt32         ulSramOffset,
    AiUInt32         ulSramSize,
    AiUInt32       * pulStatus)
{
    AiReturn ret;
    AiUInt32 ulInput[4]  = {0};
    AiUInt32 ulOutput[1] = {0};

    ulInput[0] = ulPartition;
    ulInput[1] = ulFlashOffset;
    ulInput[2] = ulSramOffset;
    ulInput[3] = ulSramSize;

    ret = AiArtix7MonCmdExecute(ul_ModuleHandle, ARTIX7_MON_CMD_FLASH_BLOCK_WRITE, 4, ulInput, 1, ulOutput);

    if ((ret     == ARTIX7_MON_OK)
        && (ulOutput[0] != ARTIX7_MON_PASS))
    {
        ret = ARTIX7_MON_ERR_STATUS;
    }

    if (pulStatus)
        *pulStatus = ulOutput[0];

    return ret;
}


AiReturn AiArtix7MonCmdFlashPartitionVerify(AiUInt32 ul_ModuleHandle,
    AiUInt32         ulPartition,
    AiUInt32         ulFileSize,
    AiUInt32         ulChecksum,
    AiUInt32       * pulStatus)
{
    AiReturn ret;
    AiUInt32 ulInput[3]  = {0};
    AiUInt32 ulOutput[1] = {0};

    ulInput[0] = ulPartition;
    ulInput[1] = ulFileSize;
    ulInput[2] = ulChecksum;

    ret = AiArtix7MonCmdExecute(ul_ModuleHandle, ARTIX7_MON_CMD_FLASH_SECTOR_VERIFY, 3, ulInput, 1, ulOutput);

    if ((ret     == ARTIX7_MON_OK)
        && (ulOutput[0] != ARTIX7_MON_PASS))
    {
        ret = ARTIX7_MON_ERR_STATUS;
    }

    if (pulStatus)
        *pulStatus = ulOutput[0];

    return ret;
}

