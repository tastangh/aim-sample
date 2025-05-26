/* SPDX-FileCopyrightText: 2024-2024 AIM GmbH <info@aim-online.com> */
/* SPDX-License-Identifier: MIT */

/*
This sample shows how to setup a Trigger Control Block to trigger on a Command Word and Data Word.
    
The BC sends messages with 32 Data words and on the first Data Word a dynamic word incrementing the value.
RT1 Receive SA 1 data word 1: 
             0 .. 0xF Bus Monitor has not triggered
             0x10     Bus Monitor triggered and will monitor the following messages

To setup the Trigger Control Block for "Trigger on an error" or "Trigger on an external event" and further 
descriptions please have a look to the MIL-STD-1553 Programmers Guide chapter "BM Trigger Definition"
*/

#include "1553_sample.h"

static AiReturn PrepareRemoteTerminals(AiUInt32 ulModHandle);
static AiReturn PrepareBusController(AiUInt32 ulModHandle);
static AiReturn PrepareMonitoring(AiUInt32 ulModHandle);
static AiBoolean HasBmTriggered(AiUInt32 ulModHandle);
static AiReturn RunSimulation(AiUInt32 ulModHandle);
static AiReturn StopSimulation(AiUInt32 ulModHandle);
static AiReturn Monitor(AiUInt32 ulModHandle);
static AiReturn CheckTransfer(AiUInt32 ulModHandle, AiUInt32 ulMessageNr, const TY_API_QUEUE_BUF * pxMessage);
static AiReturn PrintSimulationStatistic(AiUInt32 ulModHandle);
static AiReturn PrepareDytagOnDataBuffer(AiUInt32 ulModHandle, AiUInt16 uwHid);


/* ######################################################################################*/
/*                                                                                       */
/*                                     Sample entry point                                */
/*                                                                                       */
/* ######################################################################################*/
AiReturn LsBcRtBmTcbSample(AiUInt32 ulModHandle)
{
    AiReturn ret = 0;

    printf("Running LsBcRtBmTcbSample \n");

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
    /* --      Simulate RT1 receive SA1     --- */
    /*                                           */
    /*********************************************/
    uwHeaderId = MilUtilNextRTHid();

    ret = MilUtilEnableRT( ulModHandle, 1 /*RT*/, 8.0);

    AIM_TRACE_RETURN( ret );

    ret = MilUtilEnableRTSA(ulModHandle, 1 /*RT*/, 1 /*SA*/, API_RT_TYPE_RECEIVE_SA, uwHeaderId);

    AIM_TRACE_RETURN( ret );


    /*********************************************/
    /*                                           */
    /* --       Simulate RT3 receive SA13   --- */
    /*                                           */
    /*********************************************/
    uwHeaderId = MilUtilNextRTHid();

    ret = MilUtilEnableRT(ulModHandle, 3 /*RT*/, 8.0);

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
                                             "BC         -> RT1RcvSA1  ",
                                             "RT6XmtSA16 -> BC         ",
                                             "RT6XmtSA16 -> RT3RcvSA13 ",
                                             "RT6XmtMC02 -> BC         ",
                                             "------------------------ ",
                                             "    total 4000 transfers " );
    printf("\n");

    printf("PrepareBusController ... ");


    /*********************************************/
    /*                                           */
    /* --            BC -> RT1RcvSA1         --- */
    /*                                           */
    /*********************************************/
    uwHeaderId   = MilUtilNextBCHid();


    ret = MilUtilBCCreateTransfer( ulModHandle, 
                                       auwTransferIds[0],
                                       API_BC_TYPE_BCRT, 
                                       0,  /* Transmit RT */ 
                                       0,  /* Transmit SA */
                                       1,  /* Receive  RT */
                                       1,  /* Receive  SA */
                                       0,  /* Word count 32 */
                                       uwHeaderId, 
                                       API_QUEUE_SIZE_1,
                                       API_BC_TIC_NO_INT);


    AIM_TRACE_RETURN( ret );

    /* Setup Dytag for this transfer */
    ret = PrepareDytagOnDataBuffer(ulModHandle, uwHeaderId);

    AIM_TRACE_RETURN(ret);



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

AiReturn PrepareDytagOnDataBuffer(AiUInt32 ulModHandle, AiUInt16 uwHid)
{
    AiReturn        ret = 0;
    TY_API_BC_DYTAG axDytag[4];

    memset(axDytag, 0, sizeof(TY_API_BC_DYTAG) * 4);

    /*
       Dytags are handled in the firmware. A maximum of 4 Dytags can be
       assigned to each data buffer. This means only 4 words of each buffer
       may be automatically manipulated by the firmware. If more than 4 words
       of a buffer need to be manipulated Systags may be used.

       See description of ApiCmdBCDytagDef in the reference manual for
       more details about Dytags.
    */

    /*********************************************/
    /*                                           */
    /* --  Configure Dytag 1 in data word 1  --- */
    /*                                           */
    /*********************************************/
    /*
        This Dytag will create the following progression in buffer word 1 :
        The start value is set to 0 in the function PrepareBusController()

        0, 1, 2, 3, ... 100, 99, 98, 97, ... 51, 50, 51, 52 ... 99, 100, 99, 98 ... 51, 50, 51, ...
        start value  ... max              ...    min        ...     max         ...     min
    */

    /* Dytag data generation function */
    /*
        0   API_DYTAG_FCT_DISABLE              Disable
        1   API_DYTAG_FCT_POS_RAMP             Positive Ramp Function
        2   API_DYTAG_FCT_NEG_RAMP             Negative Ramp Function
        3   API_DYTAG_FCT_POS_TRIANGLE         Positive Triangle Function
        4   API_DYTAG_FCT_NEG_TRIANGLE         Negative Triangle Function
        5   API_DYTAG_FCT_XMT_WORD             Transmit Data Word from specified Data Buffer ID
    */
    axDytag[0].tag_fct = API_DYTAG_FCT_POS_TRIANGLE;


    /* Lower Limit */
    axDytag[0].min = 50;
    /* Upper Limit */
    axDytag[0].max = 100;
    /* Step size */
    axDytag[0].step = 1;
    /* Maniputlate first data word  */
    axDytag[0].wpos = 0;


    /*********************************************/
    /*                                           */
    /* --  Configure Dytag 2 in data word 32 --- */
    /*                                           */
    /*********************************************/
    /*
        This Dytag will create the following progression in buffer word 32 :
        The start value is set to 0 in the function PrepareBusController()

        0, 1, 2, 3,  ... 90,  40, 41, 42, ... 90,   40, 41, 42 ... 90, 40, ...
        start value  ... max, min         ... max, min         ... max, min
    */
    axDytag[1].tag_fct = API_DYTAG_FCT_POS_RAMP;
    axDytag[1].min = 40;
    axDytag[1].max = 90;
    axDytag[1].step = 1;
    /* Manipulate last data word */
    axDytag[1].wpos = 31;


    /* -- Dytag 2 and 3 are not used --- */
    /* The dytag array must alway have a size of 4 */
    axDytag[2].tag_fct = API_DYTAG_FCT_DISABLE;
    axDytag[3].tag_fct = API_DYTAG_FCT_DISABLE;


    /*********************************************/
    /*                                           */
    /* --       Install Dytag on board       --- */
    /*                                           */
    /*********************************************/
    ret = ApiCmdBCDytagDef(ulModHandle, 0, API_ENA, uwHid, API_DYTAG_STD_MODE, axDytag);

    AIM_ERROR_MESSAGE_RETURN(ret);

    return ret;
}


AiReturn PrepareMonitoring(AiUInt32 ulModHandle)
{
    AiReturn ret;
    uint8_t tcbEnabled = 1;
    TY_API_BM_CAP_SETUP       xCaptureMode;
    TY_API_BM_TCB             api_bm_tcb;

    memset(&xCaptureMode, 0, sizeof(xCaptureMode));
    memset(&api_bm_tcb, 0, sizeof(api_bm_tcb));

    ret = ApiCmdBMIni(ulModHandle, 0);

    AIM_ERROR_MESSAGE_RETURN(ret);

    printf("PrepareMonitoring ... ");

    /* Setup Bus Monitor for Low speed Recording */
    ret = ApiCmdQueueIni(ulModHandle, 0);
    AIM_ERROR_MESSAGE_RETURN(ret);

    xCaptureMode.cap_mode = API_BM_CAPMODE_RECORDING;
    ret = ApiCmdBMCapMode(ulModHandle, 0, &xCaptureMode);

    AIM_ERROR_MESSAGE_RETURN(ret);
    /*
     Setup Dynamic Trigger − Received Word − Command word received on primary or
     secondary bus for RT1 SA1 with a 32−word data count
     init TCB 5 for Trigger on Command from RT1 Receive SA1 with Data word count = 32
     */
    api_bm_tcb.tt = API_BM_TRG_RECEIVED_WORD; /* Trigger Type */
    api_bm_tcb.sot = API_DIS;                 /* External Trigger */
    api_bm_tcb.tri = API_DIS;                 /* Interrupt on Trigger */
    api_bm_tcb.inv = API_DIS;                 /* Inversion of Limit Check */
    api_bm_tcb.tres = 0x00;                   /* Monitor Status Trigger pattern Reset Bits */
    api_bm_tcb.tset = 0x01;                   /* Monitor Status Trigger pattern Set Bits */
    api_bm_tcb.tsp = 0x31;                    /* Trigger spec bits set for Pri or Sec Bus & CW */
    api_bm_tcb.next = 0x06;                   /* next TCB */
    api_bm_tcb.eom = 0xFF;                    /* next TCB End of Message control */
    api_bm_tcb.tdw = 0x0820;                  /* Cmd = RT1RSA1 with word count = 32 */
    api_bm_tcb.tmw = 0xFFF0;                  /* Trigger mask word − all bits applicable */
    api_bm_tcb.tuli = 0x0000;                 /* Trigger upper limit */
    api_bm_tcb.tlli = 0x0000;                 /* Trigger lower limit */
    ApiCmdBMTCBIni(ulModHandle, 0, 5 /* tid */, API_ENA, &api_bm_tcb);

    /*
       Setup Dynamic Trigger − Data Value − first Data word equal to 0x0010.
       init TCB 6 for Trigger on Data Value 0x0010
       details description see MIL-STD-1553 Programmers Guide "Trigger on Data Value Condition"
    */

    api_bm_tcb.tt   = API_BM_TRG_DATA_VALUE;  /* API_BM_TRG_ERR_CONDITION or API_BM_TRG_EXTERNAL_EVENT or API_BM_TRG_RECEIVED_WORD or API_BM_TRG_DATA_VALUE */
    api_bm_tcb.sot  = API_DIS;  /* external trigger */
    api_bm_tcb.tri  = API_DIS;  /* assert Trigger Interrupt if trigger is true */
    api_bm_tcb.inv  = API_DIS;  /* Invert result of Limit Check */
    api_bm_tcb.tres = 0x00;     /* Trigger reset for ApiCmdBMFTWIni */
    api_bm_tcb.tset = 0x0E;     /* Trigger set   for ApiCmdBMFTWIni */
    api_bm_tcb.tsp  = 0x1;      /* Check the first Data word for value in .tuli / .tlli */
    api_bm_tcb.next = 0x5;      /* next TCB */
    api_bm_tcb.eom  = 0x5;      /* next TCB */
    api_bm_tcb.tdw  = 0x0000;
    api_bm_tcb.tmw  = 0xFFFF;   /* Data Word compare mask */
    api_bm_tcb.tuli = 0x0010;   /* Upper Linit */
    api_bm_tcb.tlli = 0x0010;   /* Lower Linit */

    ret = ApiCmdBMTCBIni(ulModHandle, 0, 6 /* TCB number */, API_ENA, &api_bm_tcb);

    AIM_ERROR_MESSAGE_RETURN(ret);

    // Set the Function Trigger Word with Mask and compare Values
    ret = ApiCmdBMFTWIni(ulModHandle, 0, API_BM_WRITE_ALL, 0x0 /* reserved */, 0x0 /* reserved */, 0xF /* Start mask */, 0xF /* Start compare */);

    AIM_ERROR_MESSAGE_RETURN(ret);

    /* Set the Trigger Index Word*/
    ret = ApiCmdBMTIWIni(ulModHandle, 0,
                                API_BM_WRITE_ALL, 
                                0,  /*External static trigger disabled*/
                                0,  /*Any error static trigger disabled*/
                                5,  /*Enable first sequence trigger with TCB No 1*/
                                0); /*Enable second sequence trigger with TCB No 2*/

    AIM_ERROR_MESSAGE_RETURN(ret);

    HasBmTriggered(ulModHandle);

    printf("done\n");
    return 0;
}

AiBoolean HasBmTriggered(AiUInt32 ulModHandle)
{
    TY_API_BM_STATUS_DSP status;
    AiReturn wRetVal;

    memset(&status, 0, sizeof(status));


    wRetVal = ApiCmdBMStatusRead(ulModHandle, 0, &status);
    AIM_ERROR_MESSAGE_RETURN(wRetVal);

    printf("BM status msw=0x%x men=0x%x: ", status.msw, status.men);

    if (wRetVal == API_OK)
    {
        if (status.msw & 0x1)
        {
            printf("TRIGGERED ");
            if (status.msw & 0x2)
                printf("Capture active");
            else
                printf("Capture stopped");
            printf("\n");
            return AiTrue;
        }
        else
        {
            printf("NOT TRIGGERED\n");
        }
    }

    return AiFalse;
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
    ret = ApiCmdBCStart( ulModHandle, 0, API_BC_START_IMMEDIATELY, 100,  10,          0, &ulMajorAddr,     aulMinorAddr );

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
    AiBoolean HasTriggered = AiFalse;


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
    HasBmTriggered(ulModHandle);
    while( 1 )
    {
        if (HasTriggered == AiFalse)
        {
            HasTriggered = HasBmTriggered(ulModHandle);
        }

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
            // printf("%04x %04x %04x .... %04x \n", xQueueMsg.buffer[0], xQueueMsg.buffer[1], xQueueMsg.buffer[2], xQueueMsg.buffer[31]);

            ulBmMessageCount++;
        }
        else
        {
            AIM_ERROR_MESSAGE_RETURN( ret );
        }
        /*  Trigger with message 0x10, Stop Trigger with message 0x30 
            0x30 - 0x10 * 4 Messages per frame = 
        */
        if ((ulBmMessageCount % 500) == 0)
            printf("Bus Monitor received %d transfers\n", ulBmMessageCount);

    }

    printf("\n");

    if( ulBmMessageCount == 336 )
    {
        printf("Monitor ... done\n");
    }
    else
    {
        printf("Expected to receive %d transfers got %d\n", 336, ulBmMessageCount );
        printf("Monitor ... failed\n");
    }
    HasBmTriggered(ulModHandle);

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

    printf("BM  ( %d messages  / %d errors / %.2f%% avg bus load )\n", xBmStatus.glb_msg_cnt, xBmStatus.glb_err_cnt, xBmStatus.bus_load_pri_avg * 0.01 );

    printf("Please note that one RT to RT transfer counts as two messages in RTs and BM\n");
    printf("PrintSimulationStatistic ... done\n");
    return 0;
}

