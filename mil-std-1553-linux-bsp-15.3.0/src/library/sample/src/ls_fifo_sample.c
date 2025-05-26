/* SPDX-FileCopyrightText: 2010-2022 AIM GmbH <info@aim-online.com> */
/* SPDX-License-Identifier: MIT */

/*
This sample shows how to implement a transmittion FIFO on a transmit buffer.
This sample creates a cyclic frame with one BC2RT transfer.
The transfer has a transmit FIFO assigned.The frame is sent 2000 times with RT enabled.
The sample polls the FIFO reload size and generates unique transfer data for each FIFO reload.
At the end of the framing the BC and RT status is printed.
The transfer count should be as expected and no errors are displayed.
This sample is executed with internal coupling.No external connection is required.
*/

#include "1553_sample.h"


static AiReturn PrepareSimulation(AiUInt32 ulModHandle);
static AiReturn RunSimulation(AiUInt32 ulModHandle);
static AiReturn InitializeFifo(AiUInt32 ulModHandle);
static AiReturn PutDataInFifo(AiUInt32 ulModHandle, AiUInt16 uwReloadSize);
static AiReturn StopSimulation(AiUInt32 ulModHandle);
static AiReturn PrintSimulationStatistic(AiUInt32 ulModHandle);


#define FIFO_BUFFER_COUNT 64

AiUInt32 uwGlobalFirstDataWord   = 0;
AiUInt32 uwGlobalLastDataWord    = 0xFFFF;

/* ######################################################################################*/
/*                                                                                       */
/*                                     Sample entry point                                */
/*                                                                                       */
/* ######################################################################################*/
AiReturn LsFifoSample(AiUInt32 ulModHandle)
{
    AiReturn ret      = 0;
    AiUInt16 uwRelaodSize = 0;

    printf("Running fifo sample\n");

    /* Setup RTs and BC */
    ret = PrepareSimulation(ulModHandle);
    AIM_TRACE_RETURN(ret);

    /* Setup the Fifo and write initialial data to it */
    ret = InitializeFifo(ulModHandle);
    AIM_TRACE_RETURN(ret);

    /* Start the simulation and the bus monitor */
    ret = RunSimulation(ulModHandle);
    AIM_TRACE_RETURN(ret);


    while( MilUtilIsBcActive(ulModHandle) )
    {
        ret = ApiCmdFifoReadStatus( ulModHandle, 0, 1 /*fifo*/, &uwRelaodSize );

        AIM_ERROR_MESSAGE_RETURN(ret);

        if( uwRelaodSize == 0xFFFF )
        {
            printf("Error: Reload underrun.\n");
            break;
        }

        if( uwRelaodSize != 0 )
        {
            /* One half buffer can be relaoded */
            ret = PutDataInFifo( ulModHandle, uwRelaodSize );

            AIM_TRACE_RETURN(ret);
        }
        else
        {
            /* Relax CPU */
            AiOsSleep(10);
        }
    }


    /* Stop the simulation and the bus monitor */
    StopSimulation( ulModHandle );

    PrintSimulationStatistic( ulModHandle );

    return 0;
}





AiReturn PrepareSimulation(AiUInt32 ulModHandle)
{
    AiReturn ret;
    AiUInt16    auwTransferIds[1] = {1};

    printf("PrepareRemoteTerminals ... ");

    /*********************************************/
    /*                 RT                        */
    /* --      Simulate RT3 receive SA13     --- */
    /*                                           */
    /*********************************************/
    ret = MilUtilEnableRT(ulModHandle, 3 /*RT*/, 8.0);

    AIM_TRACE_RETURN(ret);

    ret = MilUtilEnableRTSA(ulModHandle, 3 /*RT*/, 13 /*SA*/, API_RT_TYPE_RECEIVE_SA, MilUtilNextRTHid());

    AIM_TRACE_RETURN(ret);

    /*********************************************/
    /*                  BC                       */
    /* --  Initialize the Bus Controller     --- */
    /*                                           */
    /*********************************************/
    ret = ApiCmdBCIni(ulModHandle, 0, 0, 0, 0, 0);
    AIM_ERROR_MESSAGE_RETURN(ret);

    printf("\n");
    printf(" %s\n %s\n %s\n %s\n %s\n", 
                                             "Framing :     2000 times ",
                                             "------------------------ ",
                                             "BC         -> RT3RcvSA13 ",
                                             "------------------------ ",
                                             "    total 2000 transfers " );
    printf("\n");

    printf("PrepareBusController ... ");


    /*********************************************/
    /*                    BC                     */
    /* --            BC -> RT3RcvSA13        --- */
    /*                                           */
    /*********************************************/
    ret = MilUtilBCCreateTransfer(ulModHandle,
        auwTransferIds[0],   /* Transfer Id */
        API_BC_TYPE_BCRT,
        0,  /* Transmit RT */
        0,  /* Transmit SA */
        3,  /* Receive  RT */
        13, /* Receive  SA */
        0,  /* Word count 32 */
        MilUtilNextBCHid(),
        API_QUEUE_SIZE_1,
        API_BC_TIC_NO_INT);

    AIM_TRACE_RETURN(ret);

    /*********************************************/
    /*                     BC                    */
    /* --      Setup Major/Minor Framing  ---    */
    /*                                           */
    /*********************************************/
    ret = MilUtilBCCreateSingleFrameFraming(ulModHandle, auwTransferIds, 1);
    AIM_TRACE_RETURN(ret);


    printf("done\n");
    return 0;
}







AiReturn RunSimulation(AiUInt32 ulModHandle)
{
    AiReturn ret;
    AiUInt32 ulMajorAddr     = 0;
    AiUInt32 aulMinorAddr[64];

    memset( &aulMinorAddr, 0, sizeof(aulMinorAddr) );

    printf("\nRunSimulation ... ");

    /*********************************************/
    /*                                           */
    /* --       Start all enabled RTs        --- */
    /*                                           */
    /*********************************************/
    ret = ApiCmdRTStart( ulModHandle, 0 );
    AIM_ERROR_MESSAGE_RETURN(ret);
    
    /*********************************************/
    /*                                           */
    /* --         Start Bus Controller       --- */
    /*                                           */
    /*********************************************/

    /*    ApiCmdBCStart(   module,    0, mode,                     count, mframe time, 0, major_frame_out,  minor_frame_out ); */
    ret = ApiCmdBCStart( ulModHandle, 0, API_BC_START_IMMEDIATELY, 2000,  10,          0, &ulMajorAddr,     aulMinorAddr );
    AIM_ERROR_MESSAGE_RETURN(ret);


    printf("running\n");
    return 0;
}



AiReturn InitializeFifo(AiUInt32 ulModHandle)
{
    AiReturn ret = 0;


    /* Initialize Fifo Queue */
    ret = ApiCmdFifoIni( ulModHandle, 0, 1 /*fifo's*/, FIFO_BUFFER_COUNT /*buffer's*/ );

    AIM_ERROR_MESSAGE_RETURN(ret);

    ret = ApiCmdBCAssignFifo( ulModHandle, 0, API_ENA, 1 /*fifo*/, 1 /*transfer id*/ );

    AIM_ERROR_MESSAGE_RETURN(ret);

    /* The FIFO can only be reloaded in blocks of half buffers
       As soon as the FIFO transmitted half of its data the command
       ApiCmdFifoReadStatus will return a status containing the amount
       of AiUInt16 words that can be realoaded into the first half buffer
       This half buffer must then be realoaded as one block with the command
       ApiCmdFifoWrite.
     */


    /* Fill first half buffer of the fifo with initial data */
    ret = PutDataInFifo( ulModHandle, (FIFO_BUFFER_COUNT/2)*32 );

    AIM_TRACE_RETURN(ret);

    /* Fill second half buffer of the fifo with initial data */
    ret = PutDataInFifo( ulModHandle, (FIFO_BUFFER_COUNT/2)*32 );

    AIM_TRACE_RETURN(ret);


    return ret;
}

AiReturn PutDataInFifo(AiUInt32 ulModHandle, AiUInt16 uwReloadSize)
{
    int        i              = 0;
    AiReturn   ret        = 0;
    AiUInt16 * auwMessageData = NULL;

    auwMessageData = (AiUInt16 *)AiOsMalloc(uwReloadSize * sizeof(AiUInt16));

    memset( auwMessageData, 0, uwReloadSize * sizeof(AiUInt16) );

    for( i=0; i<(uwReloadSize/32); i++ )
    {
        /* Prepare data for the next FIFO half buffer */
        auwMessageData[i*32 + 0 ] = uwGlobalFirstDataWord++;
        auwMessageData[i*32 + 31] = uwGlobalLastDataWord--;
    }

    printf("0x%X>", uwReloadSize );

    /* write the next FIFO half buffer to the device */
    ret = ApiCmdFifoWrite( ulModHandle, 0, 1 /*fifo*/, uwReloadSize, auwMessageData );

    AiOsFree( auwMessageData );

    AIM_ERROR_MESSAGE_RETURN(ret);

    return ret;
}

AiReturn StopSimulation(AiUInt32 ulModHandle)
{
    printf("\nStopSimulation ... ");

    /* Stop Bus Controller */
    ApiCmdBCHalt(    ulModHandle, 0 );
    /* Stop all RTs */
    ApiCmdRTHalt(    ulModHandle, 0 );

    printf("done\n");
    return 0;
}



AiReturn PrintSimulationStatistic(AiUInt32 ulModHandle)
{
    TY_API_BC_STATUS_DSP xBcStatus;
    TY_API_RT_STATUS_DSP xRtStatus;

    memset( &xBcStatus, 0, sizeof(xBcStatus) );
    memset( &xRtStatus, 0, sizeof(xRtStatus) );

    printf("PrintSimulationStatistic ...\n");

    ApiCmdBCStatusRead( ulModHandle, 0, &xBcStatus );
    printf("BC  ( %d messages / %d errors )\n", xBcStatus.glb_msg_cnt, xBcStatus.glb_err_cnt );

    ApiCmdRTStatusRead( ulModHandle, 0, &xRtStatus );
    printf("RTs ( %d messages  / %d errors )\n", xRtStatus.glb_msg_cnt, xRtStatus.glb_err_cnt );

    printf("Please note that one RT to RT transfer counts as two messages in RTs and BM\n");
    printf("PrintSimulationStatistic ... done\n");
    return 0;
}

