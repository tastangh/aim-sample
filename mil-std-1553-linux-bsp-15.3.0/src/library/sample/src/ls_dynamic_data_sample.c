/* SPDX-FileCopyrightText: 2010-2022 AIM GmbH <info@aim-online.com> */
/* SPDX-License-Identifier: MIT */

/*
This sample shows how to implement FW based Dytags and TSW based Systags.
This sample creates a cyclic frame with two BC2RT transfers.
The first transfer has two dynamic data tags(ApiCmdBCDytagDef) enabled in order
to enable an automatic firmware counter on the first and the last data word.
The second transfer has a Systag(ApiCmdSystagDef) enabled
to send the data based on a data set.
The framing is started with the RT enabled and a total of 2000 transfers.
At the end of the framing the BC status is checked to ensure
the correct transfer count has been send and no errors occurred.
This test is executed with internal coupling.No external connection is required.
*/

#include "1553_sample.h"




static AiReturn PrepareRemoteTerminals(AiUInt32 ulModHandle);
static AiReturn PrepareBusController(AiUInt32 ulModHandle);
static AiReturn PrepareDytagOnDataBuffer(AiUInt32 ulModHandle, AiUInt16 uwHid);
static AiReturn PrepareSystagOnDataBuffer(AiUInt32 ulModHandle, AiUInt16 uwXid);
static AiReturn RunSimulation(AiUInt32 ulModHandle);
static AiReturn PollStatus(AiUInt32 ulModHandle);
static AiReturn StopSimulation(AiUInt32 ulModHandle);
static AiReturn PrintSimulationStatistic(AiUInt32 ulModHandle);






/* ######################################################################################*/
/*                                                                                       */
/*                                     Sample entry point                                */
/*                                                                                       */
/* ######################################################################################*/
AiReturn LsDynamicDataSample(AiUInt32 ulModHandle)
{
    AiReturn ret = 0;


    printf("Running LsDynamicDataSample \n");

    /* Setup RTs and BC */
    ret = PrepareRemoteTerminals( ulModHandle );

    AIM_TRACE_RETURN(ret);

    ret = PrepareBusController( ulModHandle );

    AIM_TRACE_RETURN(ret);

    /* Start the simulation and the bus monitor */
    ret = RunSimulation( ulModHandle );

    AIM_TRACE_RETURN(ret);

    /* Poll and print status as long as the BC is active */
    ret = PollStatus( ulModHandle );

    AIM_TRACE_RETURN(ret);

    /* Stop the simulation and the bus monitor */
    ret = StopSimulation( ulModHandle );

    AIM_TRACE_RETURN(ret);

    ret = PrintSimulationStatistic( ulModHandle );

    AIM_TRACE_RETURN(ret);

    return 0;
}


AiReturn PrepareRemoteTerminals(AiUInt32 ulModHandle)
{
    AiReturn ret;

    printf("PrepareRemoteTerminals ... ");

    /*********************************************/
    /*                                           */
    /* -- Simulate RT3 receive SA13 and SA15 --- */
    /*                                           */
    /*********************************************/

    /* -- RT3 --- */

    ret = MilUtilEnableRT(ulModHandle, 3 /*RT*/, 8.0);

    AIM_TRACE_RETURN(ret);

    /* -- SA13 --- */

    ret = MilUtilEnableRTSA(ulModHandle, 3 /*RT*/, 13 /*SA*/, API_RT_TYPE_RECEIVE_SA, MilUtilNextRTHid());

    AIM_TRACE_RETURN(ret);

    /* -- SA15 --- */

    ret = MilUtilEnableRTSA(ulModHandle, 3 /*RT*/, 15 /*SA*/, API_RT_TYPE_RECEIVE_SA, MilUtilNextRTHid());

    AIM_TRACE_RETURN(ret);

    printf("done\n");
    return 0;
}



AiReturn PrepareBusController(AiUInt32 ulModHandle)
{
    AiUInt16 uwHeaderId    = 0;
    AiUInt16 auwTransferIds[2] ={ 1, 2 };

    AiReturn ret    = 0;

    AiUInt16 auwMessageData[32];

    memset( &auwMessageData,  0, sizeof(auwMessageData)  );

    /* Initialize the Bus Controller */
    ret = ApiCmdBCIni( ulModHandle, 0, 0, 0, 0, 0 );

    AIM_ERROR_MESSAGE_RETURN( ret );

    printf("\n");
    printf(" %s\n %s\n %s\n %s\n %s\n %s\n", 
                                 "Framing :             1000 times ",
                                 "-------------------------------- ",
                                 "BC         -> RT3RcvSA13 (Dytag )",
                                 "BC         -> RT3RcvSA13 (Systag)",
                                 "-------------------------------- ",
                                 "            total 2000 transfers " );
    printf("\n");

    printf("PrepareBusController ... ");


    /*********************************************/
    /*                                           */
    /* --       BC -> RT3RcvSA13 (Dytag)     --- */
    /*                                           */
    /*********************************************/
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
        API_QUEUE_SIZE_1,
        API_BC_TIC_NO_INT);

    AIM_TRACE_RETURN(ret);

    /* Setup Dytag for this transfer */
    ret = PrepareDytagOnDataBuffer(ulModHandle, uwHeaderId);

    AIM_TRACE_RETURN(ret);


    /*********************************************/
    /*                                           */
    /* --      BC -> RT3RcvSA15 (Systag)     --- */
    /*                                           */
    /*********************************************/
    uwHeaderId   = MilUtilNextBCHid();

    ret = MilUtilBCCreateTransfer(ulModHandle,
        auwTransferIds[1],
        API_BC_TYPE_BCRT,
        0,  /* Transmit RT */
        0,  /* Transmit SA */
        3,  /* Receive  RT */
        15, /* Receive  SA */
        0,  /* Word count 32 */
        uwHeaderId,
        API_QUEUE_SIZE_1,
        API_BC_TIC_NO_INT);

    AIM_TRACE_RETURN(ret);

    /* Setup Systag for this transfer */
    ret = PrepareSystagOnDataBuffer(ulModHandle, auwTransferIds[1]);

    AIM_TRACE_RETURN(ret);

    /*********************************************/
    /*                     BC                    */
    /* --      Setup Major/Minor Framing  ---    */
    /*                                           */
    /*********************************************/
    ret = MilUtilBCCreateSingleFrameFraming(ulModHandle, auwTransferIds, 2);
    AIM_TRACE_RETURN(ret);


    printf("done\n");
    return 0;
}



AiReturn PrepareDytagOnDataBuffer(AiUInt32 ulModHandle, AiUInt16 uwHid)
{
    AiReturn        ret = 0;
    TY_API_BC_DYTAG axDytag[4];

    memset( axDytag, 0, sizeof(TY_API_BC_DYTAG) * 4 );

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
    axDytag[ 0 ].tag_fct = API_DYTAG_FCT_POS_TRIANGLE;


    /* Lower Limit */
    axDytag[ 0 ].min = 50;
    /* Upper Limit */
    axDytag[ 0 ].max = 100; 
    /* Step size */
    axDytag[ 0 ].step = 1;
    /* Maniputlate first data word  */
    axDytag[ 0 ].wpos = 0;


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
    axDytag[ 1 ].tag_fct = API_DYTAG_FCT_POS_RAMP;
    axDytag[ 1 ].min     = 40;
    axDytag[ 1 ].max     = 90;
    axDytag[ 1 ].step    = 1;
    /* Manipulate last data word */
    axDytag[ 1 ].wpos    = 31; 


    /* -- Dytag 2 and 3 are not used --- */
    /* The dytag array must alway have a size of 4 */
    axDytag[ 2 ].tag_fct = API_DYTAG_FCT_DISABLE;
    axDytag[ 3 ].tag_fct = API_DYTAG_FCT_DISABLE;


    /*********************************************/
    /*                                           */
    /* --       Install Dytag on board       --- */
    /*                                           */
    /*********************************************/
    ret = ApiCmdBCDytagDef( ulModHandle, 0, API_ENA, uwHid, API_DYTAG_STD_MODE, axDytag );

    AIM_ERROR_MESSAGE_RETURN( ret );

    return ret;
}



AiReturn PrepareSystagOnDataBuffer(AiUInt32 ulModHandle, AiUInt16 uwXid)
{
    int      i       = 0;
    AiReturn ret = 0;

    TY_API_SYSTAG xSystag;
    AiUInt16      auwData[32];

    memset( &xSystag, 0, sizeof(xSystag) );
    memset( auwData,  0, sizeof(auwData) );


    /*
       Systags are handled in the onboard software. Systags may be used 
       if Dytags are not sufficient. A maximum of 255 Systags can be handled.
       See description of ApiCmdSystagDef in the reference manual for a 
       description of the different Systag types.
    */

    /****************************************************/
    /*                                                  */
    /* -- Prepare 3 data buffers to send alternatly --- */
    /*                                                  */
    /****************************************************/

    /* Dataset Write (buffer 1) */
    for( i = 0; i < 32; i++ )
        auwData[ i ] = 0xDD01;

    ret = ApiCmdRamWriteDataset( ulModHandle, 0 , auwData );

    AIM_ERROR_MESSAGE_RETURN( ret );

    /* Dataset Write (buffer 2) */
    for( i = 0; i < 32; i++ )
        auwData[ i ] = 0xDD02;

    ret = ApiCmdRamWriteDataset( ulModHandle, 1 , auwData );

    AIM_ERROR_MESSAGE_RETURN( ret );

    /* Dataset Write (buffer 3) */
    for( i = 0; i < 32; i++ )
        auwData[ i ] = 0xDD03;

    ret = ApiCmdRamWriteDataset( ulModHandle, 2 , auwData );

    AIM_ERROR_MESSAGE_RETURN( ret );

    /*************************************************/
    /*                                               */
    /* -- Configure Systag 1 for transfer uwXid  --- */
    /*                                               */
    /*************************************************/

    /* Transfer ID */
    xSystag.xid_rtsa = uwXid;
    /* Systag type */
    xSystag.fct      = API_SYSTAG_FCT_DATASET;
    /* Dataset: buffer start ID */
    xSystag.min      = 0;
    /* Dataset: amount of buffers */
    xSystag.max      = 3;
    xSystag.step     = API_SYSTAG_STEP_CYCLIC;
    xSystag.wpos     = 0;
    xSystag.bpos     = 0;



    /*********************************************/
    /*                                           */
    /* --       Install Systag on board      --- */
    /*                                           */
    /*********************************************/
    ret = ApiCmdSystagDef( ulModHandle, 0, 1 /*id*/, API_ENA, API_BC_MODE, &xSystag );

    AIM_ERROR_MESSAGE_RETURN( ret );

    return ret;
}



AiReturn RunSimulation(AiUInt32 ulModHandle)
{
    AiReturn ret;
    AiUInt32 ulMajorAddr     = 0;
    AiUInt32 aulMinorAddr[64];

    memset( &aulMinorAddr, 0, sizeof(aulMinorAddr) );

    printf("RunSimulation ... ");

    /* Start all enabled RTs */
    ret = ApiCmdRTStart( ulModHandle, 0 );

    AIM_ERROR_MESSAGE_RETURN( ret );

    /* Start BC */
    /* ApiCmdBCStart(   module, 0, mode,                     count, mframe time, 0, major_frame_out,  minor_frame_out ); */
    ret = ApiCmdBCStart( ulModHandle, 0, API_BC_START_IMMEDIATELY, 1000,  10,          0, &ulMajorAddr,     aulMinorAddr );

    AIM_ERROR_MESSAGE_RETURN( ret );

    printf("running\n");
    return 0;
}

AiReturn PollStatus(AiUInt32 ulModHandle)
{
    AiUInt32 scale = 1;
    AiReturn ret;
    TY_API_BC_STATUS_DSP xBcStatus;

    memset( &xBcStatus, 0, sizeof(xBcStatus) );

    while( MilUtilIsBcActive(ulModHandle) == 0 ); /* wait for BC start */

    while( MilUtilIsBcActive(ulModHandle) != 0 )
    {
        /* Read status while BC is active */
        ret = ApiCmdBCStatusRead( ulModHandle, 0, &xBcStatus );
        AIM_ERROR_MESSAGE_RETURN( ret );
    
        if ((xBcStatus.glb_msg_cnt >= scale*100))
        {
            printf("BC message count %d\n", xBcStatus.glb_msg_cnt);
            scale++;
        }
        AiOsSleep(500);
    }

    /* Read status after BC stop */
    ret = ApiCmdBCStatusRead( ulModHandle, 0, &xBcStatus );

    AIM_ERROR_MESSAGE_RETURN( ret );

    printf("BC message count %d\n", xBcStatus.glb_msg_cnt);


    if(    (xBcStatus.glb_msg_cnt != 2000)
        || (xBcStatus.glb_err_cnt != 0   ) )
    {
        printf("Error : expected to receive 2000 transfers and 0 errors.\n");
        return 1;
    }


    return 0;
}


AiReturn StopSimulation(AiUInt32 ulModHandle)
{
    AiReturn ret;
    printf("StopSimulation ... ");

    /* Stop Bus Controller */
    ret = ApiCmdBCHalt( ulModHandle, 0 );

    AIM_ERROR_MESSAGE_RETURN( ret );

    /* Stop all RTs */
    ret = ApiCmdRTHalt( ulModHandle, 0 );

    AIM_ERROR_MESSAGE_RETURN( ret );

    printf("done\n");
    return 0;
}



AiReturn PrintSimulationStatistic(AiUInt32 ulModHandle)
{
    AiReturn ret;

    TY_API_BC_STATUS_DSP xBcStatus;

    memset( &xBcStatus, 0, sizeof(xBcStatus) );

    printf("PrintSimulationStatistic ...\n");

    ret = ApiCmdBCStatusRead( ulModHandle, 0, &xBcStatus );

    AIM_ERROR_MESSAGE_RETURN( ret );

    printf("BC  ( %d messages  / %d errors )\n", xBcStatus.glb_msg_cnt, xBcStatus.glb_err_cnt );

    printf("PrintSimulationStatistic ... done\n");
    return 0;
}

