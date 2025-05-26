/* SPDX-FileCopyrightText: 2010-2022 AIM GmbH <info@aim-online.com> */
/* SPDX-License-Identifier: MIT */

/*
This sample shows how to to send different transfer types and record them with the BM.
This sample creates a framing with different transfer types(BC2RT, RT2BC, RT2RT, MC)
including mode codes and runs the framing with BC, RT and BM active.
The BC, RT and BM status as well as the transmitted data words are checked for errors.
This sample is executed with internal coupling.No external connection is required.
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


/* ######################################################################################*/
/*                                                                                       */
/*                                     Sample entry point                                */
/*                                                                                       */
/* ######################################################################################*/
AiReturn LsBcRtBmSample(AiUInt32 ulModHandle)
{
    AiReturn ret = 0;

    printf("Running LsBcRtBmSample \n");

    /* Setup RTs and BC */
    ret = PrepareMonitoring(      ulModHandle );

    AIM_TRACE_RETURN( ret );

    ret = PrepareRemoteTerminals( ulModHandle );

    AIM_TRACE_RETURN( ret );

    ret = PrepareBusController(   ulModHandle );

    AIM_TRACE_RETURN( ret );

    /* Start the simulation and the bus monitor */
    ret = RunSimulation( ulModHandle );

    AIM_TRACE_RETURN( ret );

    /* Read Bus Monitor data as long as the BC is active */
    ret = Monitor( ulModHandle );

    AIM_TRACE_RETURN( ret );

    /* Stop the simulation and the bus monitor */
    ret = StopSimulation( ulModHandle );

    AIM_TRACE_RETURN( ret );

    ret = PrintSimulationStatistic( ulModHandle );

    AIM_TRACE_RETURN( ret );

    return 0;
}



AiReturn PrepareRemoteTerminals(AiUInt32 ulModHandle)
{
    AiUInt8 i           = 0;

    AiUInt16 uwHeaderId = 0;
    AiUInt16 auwMessageData[32];

    AiReturn ret    = 0;


    printf("PrepareRemoteTerminals ... ");

    /*********************************************/
    /*                                           */
    /* --      Simulate RT3 receive SA13     --- */
    /*                                           */
    /*********************************************/
    uwHeaderId = MilUtilNextRTHid();

    ret = MilUtilEnableRT( ulModHandle, 3 /*RT*/, 8.0 );

    AIM_TRACE_RETURN( ret );

    ret = MilUtilEnableRTSA(ulModHandle, 3 /*RT*/, 13 /*SA*/, API_RT_TYPE_RECEIVE_SA, uwHeaderId);

    AIM_TRACE_RETURN( ret );


    /*********************************************/
    /*                                           */
    /* --       Simulate RT6 transmit SA16   --- */
    /*                                           */
    /*********************************************/
    uwHeaderId = MilUtilNextRTHid();

    ret = MilUtilEnableRT( ulModHandle, 6 /*RT*/, 8.0 );

    AIM_TRACE_RETURN( ret );

    ret = MilUtilEnableRTSA(ulModHandle, 6 /*RT*/, 16 /*SA*/, API_RT_TYPE_TRANSMIT_SA, uwHeaderId);

    AIM_TRACE_RETURN( ret );
    
    /* Data to send from the RT6 TxSA16 */
    for( i=0; i<32; i++ )
        auwMessageData[i] = 0xCE00 + i + 1;

    ret = MilUtilSetData(ulModHandle, uwHeaderId, API_BUF_RT_MSG, 1 /*First buffer in queue*/, 32, auwMessageData, NULL);

    AIM_TRACE_RETURN( ret );

    /*********************************************/
    /*                                           */
    /* --  Simulate RT6 receive modecode 2   --- */
    /*                                           */
    /*********************************************/
    uwHeaderId = MilUtilNextRTHid();

    /* RT6 is already configured above */

    ret = MilUtilEnableRTSA(ulModHandle, 6 /*RT*/, 2 /*SA*/, API_RT_TYPE_TRANSMIT_MODECODE, uwHeaderId);

    AIM_TRACE_RETURN( ret );

    printf("done\n");
    return 0;
}



AiReturn PrepareBusController(AiUInt32 ulModHandle)
{
    AiUInt16 i          = 0;
    AiReturn ret    = 0;

    AiUInt16 auwTransferIds[4] = { 1, 2, 3, 4 };
    AiUInt16 uwHeaderId    = 0;
    AiUInt16 auwMessageData[32];

    /*********************************************/
    /*                                           */
    /* --  Initialize the Bus Controller     --- */
    /*                                           */
    /*********************************************/
    ret = ApiCmdBCIni( ulModHandle, 0, 0, 0, 0, 0 );

    AIM_ERROR_MESSAGE_RETURN( ret );

    printf("\n");
    printf(" %s\n %s\n %s\n %s\n %s\n %s\n %s\n %s\n", 
                                             "Framing :     1000 times ",
                                             "------------------------ ",
                                             "BC         -> RT3RcvSA13 ",
                                             "RT6XmtSA16 -> BC         ",
                                             "RT6XmtSA16 -> RT3RcvSA13 ",
                                             "RT6XmtMC02 -> BC         ",
                                             "------------------------ ",
                                             "    total 4000 transfers " );
    printf("\n");

    printf("PrepareBusController ... ");


    /*********************************************/
    /*                                           */
    /* --            BC -> RT3RcvSA13        --- */
    /*                                           */
    /*********************************************/
    uwHeaderId   = MilUtilNextBCHid();


    ret = MilUtilBCCreateTransfer( ulModHandle, 
                                       auwTransferIds[0],
                                       API_BC_TYPE_BCRT, 
                                       0,  /* Transmit RT */ 
                                       0,  /* Transmit SA */
                                       3,  /* Receive  RT */
                                       13, /* Receive  SA */
                                       0,  /* Word count 32 */
                                       uwHeaderId, 
                                       API_QUEUE_SIZE_1,
                                       API_BC_TIC_NO_INT);


    AIM_TRACE_RETURN( ret );


    /* Data to send to the RT */
    for (i = 0; i < 32; i++)
        auwMessageData[ i ] = 0xBC00 + i + 1;

    ret = MilUtilSetData(ulModHandle, uwHeaderId, API_BUF_BC_MSG, 1 /*First buffer in queue*/, 32, auwMessageData, NULL);

    AIM_TRACE_RETURN( ret );

    /*********************************************/
    /*                                           */
    /* --          RT6XmtSA16 -> BC          --- */
    /*                                           */
    /*********************************************/
    uwHeaderId   = MilUtilNextBCHid();

    ret = MilUtilBCCreateTransfer( ulModHandle, 
                                       auwTransferIds[1],
                                       API_BC_TYPE_RTBC, 
                                       6,  /* Transmit RT */ 
                                       16, /* Transmit SA */
                                       0,  /* Receive  RT */
                                       0,  /* Receive  SA */
                                       0,  /* Word count 32 */
                                       uwHeaderId, 
                                       API_QUEUE_SIZE_1,
                                       API_BC_TIC_NO_INT);


    AIM_TRACE_RETURN( ret );

    /*********************************************/
    /*                                           */
    /* --     RT6XmtSA16 -> RT3RcvSA13       --- */
    /*                                           */
    /*********************************************/
    uwHeaderId   = MilUtilNextBCHid();

    ret = MilUtilBCCreateTransfer( ulModHandle, 
                                       auwTransferIds[2],
                                       API_BC_TYPE_RTRT, 
                                       6,   /* Transmit RT */ 
                                       16,  /* Transmit SA */
                                       3,   /* Receive  RT */
                                       13,  /* Receive  SA */
                                       0,   /* Word count 32 */
                                       uwHeaderId, 
                                       API_QUEUE_SIZE_1,
                                       API_BC_TIC_NO_INT);

    AIM_TRACE_RETURN( ret );

    /*********************************************/
    /*                                           */
    /* --           RT6XmtMC02 -> BC         --- */
    /*                                           */
    /*********************************************/
    uwHeaderId   = MilUtilNextBCHid();

    ret = MilUtilBCCreateTransfer( ulModHandle, 
                                       auwTransferIds[3],
                                       API_BC_TYPE_RTBC, 
                                       6,   /* Transmit RT */ 
                                       0,   /* Transmit SA */
                                       0,   /* Receive  RT */
                                       0,   /* Receive  SA */
                                       2,   /* Mode Code   */
                                       uwHeaderId, 
                                       API_QUEUE_SIZE_1,
                                       API_BC_TIC_NO_INT);

    AIM_TRACE_RETURN( ret );

    /*********************************************/
    /*                     BC                    */
    /* --      Setup Major/Minor Framing  ---    */
    /*                                           */
    /*********************************************/
    ret = MilUtilBCCreateSingleFrameFraming(ulModHandle, auwTransferIds, 4);
    AIM_TRACE_RETURN(ret);

    printf("done\n");
    return 0;
}


AiReturn PrepareMonitoring(AiUInt32 ulModHandle)
{
    AiReturn ret;
    printf("PrepareMonitoring ... ");

    ret = ApiCmdQueueIni(   ulModHandle, 0 );

    AIM_ERROR_MESSAGE_RETURN( ret );

    ret = ApiCmdBMIntrMode( ulModHandle, 0, 0, 0, 0 );

    AIM_ERROR_MESSAGE_RETURN( ret );

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

    /*********************************************/
    /*                                           */
    /* --          Start Bus Monitor         --- */
    /*                                           */
    /*********************************************/
    ret = ApiCmdQueueStart( ulModHandle, 0 );

    AIM_ERROR_MESSAGE_RETURN( ret );

    /*********************************************/
    /*                                           */
    /* --       Start all enabled RTs        --- */
    /*                                           */
    /*********************************************/
    ret = ApiCmdRTStart( ulModHandle, 0 );

    AIM_ERROR_MESSAGE_RETURN( ret );

    /*********************************************/
    /*                                           */
    /* --         Start Bus Controller       --- */
    /*                                           */
    /*********************************************/

    /*  BC start modes :
        1  API_BC_START_IMMEDIATELY            Start immediately
        2  API_BC_START_EXT_TRIGGER            Start by external BC trigger input
        3  API_BC_START_RT_MODECODE            Start by RT Modecode Dynamic Bus Control
        4  API_BC_START_EXTERNAL_PULSE         Start on external pulse (BC trigger input)
        5  API_BC_START_SETUP                  Prepare for API_BC_START_FAST
        6  API_BC_START_FAST                   Start fast defined with API_BC_START_SETUP.
        7  API_BC_START_INSTR_TABLE            Start with Instruction Table
        8  API_BC_START_CONTINUE_ON_BC_HALT    Start after BC halt (not possible on AMC1553-4)
    */

    /* ApiCmdBCStart(   module, 0, mode,                     count, mframe time, 0, major_frame_out,  minor_frame_out ); */
    ret = ApiCmdBCStart( ulModHandle, 0, API_BC_START_IMMEDIATELY, 1000,  10,          0, &ulMajorAddr,     aulMinorAddr );

    AIM_ERROR_MESSAGE_RETURN( ret );

    printf("running\n");
    return 0;
}

AiReturn StopSimulation(AiUInt32 ulModHandle)
{
    AiReturn ret;
    printf("StopSimulation ... ");

    /* Stop Bus Controller */
    ret = ApiCmdBCHalt(    ulModHandle, 0 );

    AIM_ERROR_MESSAGE_RETURN( ret );

    /* Stop all RTs */
    ret = ApiCmdRTHalt(    ulModHandle, 0 );

    AIM_ERROR_MESSAGE_RETURN( ret );

    /* Stop Bus Monitor */
    ret = ApiCmdQueueHalt( ulModHandle, 0 );

    AIM_ERROR_MESSAGE_RETURN( ret );

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

    /*********************************************/
    /*                                           */
    /* --           Wait for BC start        --- */
    /*                                           */
    /*********************************************/
    while( MilUtilIsBcActive( ulModHandle ) == 0 );

    /************************************************/
    /*                                              */
    /* --  Read BM data as long as BC is active --- */
    /*                                              */ 
    /************************************************/
    while( 1 )
    {
        memset( &xQueueMsg, 0, sizeof(xQueueMsg) );

        ret = ApiCmdQueueRead( ulModHandle, 0, &xQueueMsg );

        if( ret == API_ERR_QUEUE_EMPTY )
        {
            /* No data available */

            if( MilUtilIsBcActive( ulModHandle ) == 1 ) 
            {
                /* The BC is active so we may get data the next time */
                AiOsSleep(200);
                continue;
            }
            else
            {
                break;
            }
        }
        else if( ret == API_OK )
        {
            /* Received data */
            ret = CheckTransfer( ulModHandle, ulBmMessageCount, &xQueueMsg );

            if( ret != 0 )
                break;

            ulBmMessageCount++;
        }
        else
        {
            AIM_ERROR_MESSAGE_RETURN( ret );
        }

        if ((ulBmMessageCount % 500) == 0)
            printf("Bus Monitor received %d transfers\n", ulBmMessageCount);

    }

    printf("\n");

    if( ulBmMessageCount == 4000 )
    {
        printf("Monitor ... done\n");
    }
    else
    {
        printf("Expected to receive %d transfers got %d\n", 4000, ulBmMessageCount );
        printf("Monitor ... failed\n");
    }

    return 0;
}


AiReturn CheckTransfer(AiUInt32 ulModHandle, AiUInt32 ulTransferNo, const TY_API_QUEUE_BUF * pxMessage)
{
    /* Framing : 
       ------------------------
       BC         -> RT3RcvSA13
       RT6XmtSA16 -> BC
       RT6XmtSA16 -> RT3RcvSA13
       RT6XmtMC02 -> BC
    */

    AiUInt8  aucExpectedRts[]           = { 3,      6,      3,      6 };
    AiUInt8  aucExpectedRts2[]          = { 0,      0,      6,      0 };
    AiUInt16 auwExpectedFirstDataWord[] = { 0xBC01, 0xCE01, 0xCE01, 0 };

    if( aucExpectedRts[ ulTransferNo % 4 ] != pxMessage->rt_addr )
    {
        printf("\n");
        printf("Expected RT %d got %d at transfer %d\n",
                                               aucExpectedRts[ ulTransferNo % 4 ],
                                               pxMessage->rt_addr,
                                               ulTransferNo );
        return 1;
    }

    if( aucExpectedRts2[ ulTransferNo % 4 ] != pxMessage->rt_addr2 )
    {
        printf("\n");
        printf("Expected RT (2) %d got %d at transfer %d\n",
                                               aucExpectedRts2[ ulTransferNo % 4 ],
                                               pxMessage->rt_addr2,
                                               ulTransferNo );
        return 1;
    }

    if( pxMessage->word_cnt > 1 ) /* Skip MC2 because it has no data */
    {
        if( auwExpectedFirstDataWord[ ulTransferNo % 4 ] != pxMessage->buffer[0] )
        {
            printf("\n");
            printf("Expected first data word 0x%04X got 0x%04X at transfer %d\n",
                                                   auwExpectedFirstDataWord[ ulTransferNo % 4 ],
                                                   pxMessage->buffer[0],
                                                   ulTransferNo );
            return 1;
        }
    }
    return 0;
}


AiReturn PrintSimulationStatistic(AiUInt32 ulModHandle)
{
    AiReturn ret;
    TY_API_BC_STATUS_DSP xBcStatus;
    TY_API_RT_STATUS_DSP xRtStatus;
    TY_API_BM_STATUS_DSP xBmStatus;

    memset( &xBcStatus, 0, sizeof(xBcStatus) );
    memset( &xRtStatus, 0, sizeof(xRtStatus) );
    memset( &xBmStatus, 0, sizeof(xBmStatus) );

    printf("PrintSimulationStatistic ...\n");

    ret = ApiCmdBCStatusRead( ulModHandle, 0, &xBcStatus );

    AIM_ERROR_MESSAGE_RETURN( ret );

    printf("BC  ( %d messages / %d errors )\n", xBcStatus.glb_msg_cnt, xBcStatus.glb_err_cnt );

    ret = ApiCmdRTStatusRead( ulModHandle, 0, &xRtStatus );

    AIM_ERROR_MESSAGE_RETURN( ret );

    printf("RTs ( %d messages  / %d errors )\n", xRtStatus.glb_msg_cnt, xRtStatus.glb_err_cnt );

    ret = ApiCmdBMStatusRead( ulModHandle, 0, &xBmStatus );

    AIM_ERROR_MESSAGE_RETURN( ret );

    printf("BM  ( %d messages  / %d errors )\n", xBmStatus.glb_msg_cnt, xBmStatus.glb_err_cnt );

    printf("Please note that one RT to RT transfer counts as two messages in RTs and BM\n");
    printf("PrintSimulationStatistic ... done\n");
    return 0;
}

