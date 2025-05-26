/* SPDX-FileCopyrightText: 2010-2022 AIM GmbH <info@aim-online.com> */
/* SPDX-License-Identifier: MIT */

/*
This sample shows how to handle user event callbacks to reload a trasmit data buffer.
This sample creates a cyclic frame with one BC2RT transfer.
The transfer has a buffer queue with 2 buffers enabled on the transmitter side.
The transfer has the user interrupt callback enabled.
The frame is sent 1000 times with RT and BM enabled.
Each time the transfer is send the firmware emits a user interrupt.
The sample interrupt callback handler checks the interrupt source and updates the transmit data in the inactive buffer.
This shows how to ensure transmit buffer data consistency.
The BM is used to continuously read the transmitted data and check the first and last data word of each transfer.
At the end of the framing the BC, RT status and the received interrupt count is printed.
The transfer count and interrupt count should be as expected and no errors are displayed.
This test is executed with internal coupling.No external connection is required.
*/

#include "1553_sample.h"

static AiReturn PrepareRemoteTerminals(AiUInt32 ulModHandle);
static AiReturn PrepareBusController(AiUInt32 ulModHandle);
static AiReturn PrepareMonitoring(AiUInt32 ulModHandle);
static AiReturn RunSimulation(AiUInt32 ulModHandle);
static AiReturn StopSimulation(AiUInt32 ulModHandle);
static AiReturn Monitor(AiUInt32 ulModHandle);
static AiReturn CheckTransfer(AiUInt32 ulModHandle, AiUInt32 ulMessageNr, const TY_API_QUEUE_BUF * pxMessage);
static AiReturn PrintSimulationStatistic(AiUInt32 ulModHandle);
static void  AI_CALL_CONV BcInterrupt(AiUInt32 ulModHandle, AiUInt8 ucBiu, AiUInt8 ucType, TY_API_INTR_LOGLIST_ENTRY * pxInfo);

/* Interrupt related */
static AiUInt16 uwGlobalDataCounter = 1;
static AiUInt8  ucBufferQueueSelect = 1;
static AiUInt32 ulIntCounter        = 0;

static AiUInt32 ulBufAddr[2] = {0,0};

/* ######################################################################################*/
/*                                                                                       */
/*                                     Sample entry point                                */
/*                                                                                       */
/* ######################################################################################*/
AiReturn LsInterruptSample(AiUInt32 ulModHandle)
{
    AiReturn ret;
    printf("Running LsInterruptSample \n");

    /* -- Install interrupt handler  --- */ 
    ret = ApiInstIntHandler(ulModHandle, API_INT_LS, API_INT_BC, BcInterrupt);
    AIM_ERROR_MESSAGE_RETURN(ret);

    ret = PrepareMonitoring( ulModHandle );
    AIM_TRACE_RETURN(ret);

    /* Setup RTs and BC */
    ret = PrepareRemoteTerminals( ulModHandle );
    AIM_TRACE_RETURN(ret);

    ret = PrepareBusController(ulModHandle);
    AIM_TRACE_RETURN(ret);

    /* Start the simulation and the bus monitor */
    ret = RunSimulation( ulModHandle );
    AIM_TRACE_RETURN(ret);

    /* Read Bus Monitor data as long as the BC is active */
    ret = Monitor( ulModHandle );
    AIM_TRACE_RETURN(ret);

    /* Stop the simulation and the bus monitor */
    ret = StopSimulation(ulModHandle);
    AIM_TRACE_RETURN(ret);

    ret = PrintSimulationStatistic(ulModHandle);
    AIM_TRACE_RETURN(ret);

    /* -- Delete interrupt handler  --- */
    ret = ApiDelIntHandler( ulModHandle, API_INT_LS, API_INT_BC );
    AIM_ERROR_MESSAGE_RETURN(ret);

    return 0;
}



AiReturn PrepareRemoteTerminals(AiUInt32 ulModHandle)
{
    AiReturn ret;

    printf("PrepareRemoteTerminals ... ");

    /* -- Simulate RT3 receive SA13 --- */
    ret = MilUtilEnableRT(ulModHandle, 3 /*RT*/, 8.0);

    AIM_TRACE_RETURN(ret);

    ret = MilUtilEnableRTSA(ulModHandle, 3 /*RT*/, 13 /*SA*/, API_RT_TYPE_RECEIVE_SA, MilUtilNextRTHid());

    AIM_TRACE_RETURN(ret);

    printf("done\n");
    return 0;
}



AiReturn PrepareBusController(AiUInt32 ulModHandle)
{
    /* Framing : 
       ------------------------
       BC         -> RT3RcvSA13
       RT6XmtSA16 -> BC
       RT6XmtSA16 -> RT3RcvSA13
       BC         -> RT6RcvMC17
    */
    AiUInt16 i          = 0;
    AiReturn ret;
    AiUInt16 auwTransferIds[1] ={ 1 };
    AiUInt16 uwHeaderId   = 0;

    AiUInt16 auwMessageData[32];

    memset( &auwMessageData,  0, sizeof(auwMessageData)  );

    /* Initialize the Bus Controller */
    ret = ApiCmdBCIni( ulModHandle, 0, 0, 0, 0, 0 );

    AIM_ERROR_MESSAGE_RETURN(ret);


    /* Framing */
    printf("\n");
    printf(" %s\n %s\n %s\n %s\n %s\n ", 
                                             "Framing :     1000 times ",
                                             "------------------------ ",
                                             "BC         -> RT3RcvSA13 ",
                                             "------------------------ ",
                                             "    total 1000 transfers " );
    printf("\n");

    printf("PrepareBusController ... ");

    /* -- BC -> RT3RcvSA13 -- */
    uwHeaderId   = MilUtilNextBCHid();


    ret = MilUtilBCCreateTransfer(ulModHandle,
        auwTransferIds[0],
        API_BC_TYPE_BCRT,
        0,  /* Transmit RT */
        0,  /* Transmit SA */
        3,  /* Receive  RT */
        13, /* Receive  SA */
        0,  /* Word count 32 */
        uwHeaderId,
        API_QUEUE_SIZE_2,
        API_BC_TIC_INT_ON_XFER_END );


    AIM_TRACE_RETURN(ret);


    /* Initial data to send in buffer one */
    for (i = 0; i < 32; i++)
        auwMessageData[ i ] = uwGlobalDataCounter++;

    ret = MilUtilSetData(ulModHandle, uwHeaderId, API_BUF_BC_MSG, 1 /* First buffer in queue */, 32, auwMessageData, &ulBufAddr[0]);

    AIM_TRACE_RETURN(ret);

    /* Initial data to send in buffer two */
    for (i = 0; i < 32; i++)
        auwMessageData[ i ] = uwGlobalDataCounter++;

    ret = MilUtilSetData(ulModHandle, uwHeaderId, API_BUF_BC_MSG, 2 /* Second buffer in queue */, 32, auwMessageData, &ulBufAddr[1]);
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


AiReturn PrepareMonitoring(AiUInt32 ulModHandle)
{
    AiReturn ret;
    printf("PrepareMonitoring ... ");

    ret = ApiCmdQueueIni(   ulModHandle, 0 );
    AIM_ERROR_MESSAGE_RETURN(ret);

    ret = ApiCmdBMIntrMode(ulModHandle, 0, 0, 0, 0);
    AIM_ERROR_MESSAGE_RETURN(ret);

    printf("done\n");
    return 0;
}


AiReturn RunSimulation(AiUInt32 ulModHandle)
{
    AiReturn ret;
    AiUInt32 ulMajorAddr     = 0;
    AiUInt32 aulMinorAddr[64];

    memset( &aulMinorAddr, 0, sizeof(aulMinorAddr) );

    printf("RunSimulation ... ");

    /* Start Bus Monitor */
    ret = ApiCmdQueueStart( ulModHandle, 0 );
    AIM_ERROR_MESSAGE_RETURN(ret);
    /* Start all enabled RTs */
    ret = ApiCmdRTStart( ulModHandle, 0 );
    AIM_ERROR_MESSAGE_RETURN(ret);
    /* Start BC */
    /* ApiCmdBCStart(   module, 0, mode,                     count, mframe time, 0, major_frame_out,  minor_frame_out ); */
    ret = ApiCmdBCStart( ulModHandle, 0, API_BC_START_IMMEDIATELY, 1000,  60,          0, &ulMajorAddr,     aulMinorAddr );
    AIM_ERROR_MESSAGE_RETURN(ret);

    printf("running\n");
    return 0;
}

AiReturn StopSimulation(AiUInt32 ulModHandle)
{
    printf("StopSimulation ... ");

    /* Stop Bus Controller */
    ApiCmdBCHalt(    ulModHandle, 0 );
    /* Stop all RTs */
    ApiCmdRTHalt(    ulModHandle, 0 );
    /* Stop Bus Monitor */
    ApiCmdQueueHalt( ulModHandle, 0 );

    printf("done\n");
    return 0;
}

AiReturn Monitor(AiUInt32 ulModHandle)
{
    AiReturn         ret          = 0;
    AiUInt32         ulBmMessageCount = 0;
    TY_API_QUEUE_BUF xQueueMsg;

    memset( &xQueueMsg, 0, sizeof(xQueueMsg) );

    printf("Monitor ... \n");

    /* Wait for BC start */
    while( MilUtilIsBcActive( ulModHandle ) == 0 );

    AiOsSleep(500);
    /* Read BM data as long as BC is active */
    while( 1 )
    {
        ret = ApiCmdQueueRead( ulModHandle, 0, &xQueueMsg );

        if(    ( ret != API_OK ) 
            && ( ret != API_ERR_QUEUE_EMPTY ) )
        {
            /* Error while reading data */
            AIM_ERROR_MESSAGE_RETURN(ret);
        }

        if (ret == API_ERR_QUEUE_EMPTY)
        {
            if (MilUtilIsBcActive(ulModHandle) == 1) {
                AiOsSleep(500);
                continue;
            } else
                break;
        }

        if( ret == API_OK )
        {
            /* Received data */
            ret = CheckTransfer( ulModHandle, ulBmMessageCount, &xQueueMsg );

            if( ret != 0 )
                break;

            ulBmMessageCount++;
        }

        if( (ulBmMessageCount % 100) == 0 )
            printf("Bus Monitor received %d transfers (%d ir)\n", ulBmMessageCount, ulIntCounter );

        AiOsSleep(10);
    }

    AiOsSleep(500);

    printf("Bus Monitor received %d transfers (%d ir)\n", ulBmMessageCount, ulIntCounter );


    printf("\n");

    if( ulIntCounter != 1000 )
    {
        printf("Expected to receive %d interrupts got %d\n", 1000, ulIntCounter );
        return 1;
    }

    if( ulBmMessageCount == 1000 )
    {
        printf("Monitor ... done\n");
    }
    else
    {
        printf("Expected to receive %d transfers got %d\n", 1000, ulBmMessageCount );
        printf("Monitor ... failed\n");
    }

    return 0;
}


AiReturn CheckTransfer(AiUInt32 ulModHandle, AiUInt32 ulTransferNo, const TY_API_QUEUE_BUF * pxMessage)
{
    /* Framing : 
       ------------------------
       BC         -> RT3RcvSA13
    */
    if( pxMessage->buffer[0] != ((ulTransferNo  * 32) + 1) )
    {
        printf("\n");
        printf("Expected first data word 0x%04X to be 0x%04X at transfer %d\n",
                                               pxMessage->buffer[0],
                                               ((ulTransferNo  * 32) + 1),
                                               ulTransferNo );
        return 1;
    }

    if( pxMessage->buffer[31] != ((ulTransferNo + 1)  * 32) )
    {
        printf("\n");
        printf("Expected last data word 0x%04X to be 0x%04X at transfer %d\n",
                                               pxMessage->buffer[31],
                                               ((ulTransferNo + 1)  * 32),
                                               ulTransferNo );
        return 1;
    }

    return 0;
}


AiReturn PrintSimulationStatistic(AiUInt32 ulModHandle)
{
    TY_API_BC_STATUS_DSP xBcStatus;

    memset( &xBcStatus, 0, sizeof(xBcStatus) );

    printf("PrintSimulationStatistic ...\n");

    ApiCmdBCStatusRead( ulModHandle, 0, &xBcStatus );
    printf("BC  ( %d messages / %d errors )\n", xBcStatus.glb_msg_cnt, xBcStatus.glb_err_cnt );

    printf("PrintSimulationStatistic ... done\n");
    return 0;
}


static void  AI_CALL_CONV BcInterrupt(AiUInt32 ulModHandle, AiUInt8 ucBiu, AiUInt8 ucType, TY_API_INTR_LOGLIST_ENTRY * pxInfo)
{
    AiUInt8  i           = 0;

    AiUInt16 auwData[32];

    /* ApiWriteBlockMemData */
    AiUInt32 ul_BytesWritten;

    /* handle only BC interrupts with PGI and UDF set */
    if ((pxInfo->ul_Lla & 0xffff0000) !=  0x30400000)
        return;

    ulIntCounter++;

    /*
    When the first interrupt occures we update the first buffer because the BC 
    is currently sending buffer two. When the second interrupt occures the BC is sending
    buffer one again and we update buffer two. 
    */
    ucBufferQueueSelect = (ucBufferQueueSelect == 1) ? 0 : 1;

    /* Update data */
    for ( i = 0; i < 32; i++)
        auwData[i] = uwGlobalDataCounter++;

    
    /* copy frame data into the onboard buffer */
    /* For VME environments, don't call any functions that use the host to target communication
     * The name of functions that use the host to target communication start with "ApiCmd"
     */
    ApiWriteBlockMemData( ulModHandle, API_MEMTYPE_GLOBAL, ulBufAddr[ucBufferQueueSelect], 2, auwData, 32, &ul_BytesWritten );

}



 
