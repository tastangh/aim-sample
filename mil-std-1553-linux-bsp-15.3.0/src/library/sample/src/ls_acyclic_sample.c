/* SPDX-FileCopyrightText: 2010-2022 AIM GmbH <info@aim-online.com> */
/* SPDX-License-Identifier: MIT */

/*
This sample shows how to to send acyclic frames.
This sample creates an acyclic frame with different transfer types(BC2RT, RT2RT and RT2BC).
The BC is started in idle mode with the RT enabled and the acyclic frame
is send 300 times producing a total of 900 tranfers.
After each frame the received status and data is checked.
This test is executed with internal coupling.No external connection is required.
*/

#include "1553_sample.h"



static AiReturn PrepareRemoteTerminals(AiUInt32 ulModHandle);
static AiReturn PrepareBusController(AiUInt32 ulModHandle);
static AiReturn PrepareAcyclicTransfers(AiUInt32 ulModHandle);
static AiReturn RunSimulation(AiUInt32 ulModHandle);
static AiReturn StopSimulation(AiUInt32 ulModHandle);
static AiReturn SetTransmitData(AiUInt32 ulModHandle, AiUInt16 uwBufferValue, AiUInt8 ucBufferType, AiUInt16 uwTransmitBufferHeader);
static AiReturn CheckReceiveData(AiUInt32 ulModHandle, AiUInt16 uwBufferValue, AiUInt8 ucBufferType, AiUInt16 uwTransmitBufferHeader);
static AiReturn SendAcyclicDataBlocking(AiUInt32 ulModHandle, AiUInt16 uwTransferId);
static AiReturn PrintSimulationStatistic(AiUInt32 ulModHandle);

#define BC_TRANSFER_COUNT 900

/* BC Header ids are shared by all transfers on a stream */
static AiUInt16 transfer1_buffer_header_id;
static AiUInt16 transfer2_buffer_header_id;

/* RT Header ids are shared by all RTs on a stream */
static AiUInt16 rt3_sa13_buffer_header_id;
static AiUInt16 rt4_sa14_buffer_header_id;

#define BC2RT_TRANSFER_ID          1
#define RT2BC_TRANSFER_ID          2
#define RT2RT_TRANSFER_ID          3

/* ######################################################################################*/
/*                                                                                       */
/*                                     Sample entry point                                */
/*                                                                                       */
/* ######################################################################################*/
AiReturn LsAcyclicSample(AiUInt32 ulModHandle)
{
    AiReturn ret = 0;
    AiUInt16 uwBufferValue = 0;

    printf("Running LsAcyclicSample \n");

    ret = PrepareRemoteTerminals( ulModHandle );

    AIM_TRACE_RETURN( ret );

    ret = PrepareBusController( ulModHandle );

    AIM_TRACE_RETURN( ret );

    ret = PrepareAcyclicTransfers( ulModHandle );

    AIM_TRACE_RETURN( ret );

    ret = RunSimulation( ulModHandle );

    AIM_TRACE_RETURN( ret );


    printf("\n");
    printf(" %s\n %s\n %s\n %s\n %s\n %s\n %s\n",
        "Framing :      300 times ",
        "------------------------ ",
        "BC         -> RT3RcvSA13 ",
        "RT4XmtSA14 -> BC         ",
        "RT4XmtSA14 -> RT3RcvSA13 ",
        "------------------------ ",
        "    total  900 transfers ");
    printf("\n");

    printf("\n");

    printf("Sending acyclic transfers\n");

    uwBufferValue=1;
    do
    {
        /* -- Send and verify BC to RT --- */

        ret = SetTransmitData(ulModHandle, uwBufferValue, API_BUF_BC_MSG, transfer1_buffer_header_id);

        AIM_TRACE_RETURN(ret);

        ret = SendAcyclicDataBlocking(ulModHandle, BC2RT_TRANSFER_ID );

        AIM_TRACE_RETURN(ret);

        ret = CheckReceiveData(ulModHandle, uwBufferValue, API_BUF_RT_MSG, rt3_sa13_buffer_header_id);

        AIM_TRACE_RETURN(ret);

        uwBufferValue++;

        /* -- Send and verify RT to BC --- */

        ret = SetTransmitData(ulModHandle, uwBufferValue, API_BUF_RT_MSG, rt4_sa14_buffer_header_id);

        AIM_TRACE_RETURN(ret);

        ret = SendAcyclicDataBlocking(ulModHandle, RT2BC_TRANSFER_ID);

        AIM_TRACE_RETURN(ret);

        ret = CheckReceiveData(ulModHandle, uwBufferValue, API_BUF_BC_MSG, transfer2_buffer_header_id);

        AIM_TRACE_RETURN(ret);

        uwBufferValue++;

        /* -- Send and verify RT to RT --- */

        ret = SetTransmitData(ulModHandle, uwBufferValue, API_BUF_RT_MSG, rt4_sa14_buffer_header_id);

        AIM_TRACE_RETURN(ret);

        ret = SendAcyclicDataBlocking(ulModHandle, RT2RT_TRANSFER_ID);

        AIM_TRACE_RETURN(ret);

        ret = CheckReceiveData(ulModHandle, uwBufferValue, API_BUF_RT_MSG, rt3_sa13_buffer_header_id);

        AIM_TRACE_RETURN(ret);

        if ((uwBufferValue % 100) == 0)
            printf("Transmitted %d acyclic transfers.\n", uwBufferValue);

        uwBufferValue++;


    } while (uwBufferValue<=BC_TRANSFER_COUNT);

    printf("\n");

    ret = StopSimulation( ulModHandle );

    AIM_TRACE_RETURN( ret );

    PrintSimulationStatistic(ulModHandle);

    return 0;
}









AiReturn PrepareRemoteTerminals(AiUInt32 ulModHandle)
{
    AiUInt16 uwHeaderId = 0;
    AiReturn ret    = 0;

    printf("PrepareRemoteTerminals ... ");

    /* -- Simulate RT3 receive SA13 --- */

    rt3_sa13_buffer_header_id = MilUtilNextRTHid();

    ret = MilUtilEnableRT( ulModHandle, 3 /*RT*/, 8.0 );

    AIM_TRACE_RETURN( ret );

    ret = MilUtilEnableRTSA(ulModHandle, 3 /*RT*/, 13 /*SA*/, API_RT_TYPE_RECEIVE_SA, rt3_sa13_buffer_header_id);

    AIM_TRACE_RETURN( ret );


    /* -- Simulate RT4 transmit SA14 --- */

    rt4_sa14_buffer_header_id = MilUtilNextRTHid();

    ret = MilUtilEnableRT(ulModHandle, 4 /*RT*/, 8.0);

    AIM_TRACE_RETURN(ret);

    ret = MilUtilEnableRTSA(ulModHandle, 4 /*RT*/, 14 /*SA*/, API_RT_TYPE_TRANSMIT_SA, rt4_sa14_buffer_header_id);

    AIM_TRACE_RETURN(ret);



    printf("done\n");
    return 0;
}



AiReturn PrepareBusController(AiUInt32 ulModHandle)
{
    AiReturn ret;
    TY_API_BC_FRAME    xMinorFrame;
    TY_API_BC_MFRAME   xMajorFrame;

    memset( &xMinorFrame, 0, sizeof(xMinorFrame) );
    memset( &xMajorFrame, 0, sizeof(xMajorFrame) );

    /* Initialize the Bus Controller */
    ret = ApiCmdBCIni( ulModHandle, 0, 0, 0, 0, 0 );

    AIM_ERROR_MESSAGE_RETURN( ret );

    printf("PrepareBusController ... ");

    /* In this sample we do not send any cyclic transfers. Instead
       we include only a wait instruction in the minor major framing.
       This method can be used to send only acyclic frames.
    */
    xMinorFrame.cnt        = 1;
    xMinorFrame.xid[   0 ] = 10; /* wait time */
    xMinorFrame.instr[ 0 ] = API_BC_INSTR_WAIT;

    /* -- Install Minor Frame --- */
    xMinorFrame.id = 1;
    ret = ApiCmdBCFrameDef( ulModHandle, 0, &xMinorFrame );

    AIM_ERROR_MESSAGE_RETURN( ret );

    /* -- Install Major Frame --- */
    xMajorFrame.cnt    = 1;
    xMajorFrame.fid[0] = 1;
    ret = ApiCmdBCMFrameDef( ulModHandle, 0, &xMajorFrame );

    AIM_ERROR_MESSAGE_RETURN( ret );

    printf("done\n");
    return 0;
}









AiReturn PrepareAcyclicTransfers(AiUInt32 ulModHandle)
{
    AiReturn ret     = 0;

    printf("PrepareAcyclicTransfer ... ");

    /* -- BC -> RT3RcvSA13 -- */

    /* creating a acyclic transfer is equal to creating a cyclic transfer */
    transfer1_buffer_header_id = MilUtilNextBCHid();

    ret = MilUtilBCCreateTransfer( ulModHandle, 
                                       BC2RT_TRANSFER_ID,
                                       API_BC_TYPE_BCRT, 
                                       0,  /* Transmit RT */ 
                                       0,  /* Transmit SA */
                                       3,  /* Receive  RT */
                                       13, /* Receive  SA */
                                       0,  /* Word count 32 */
                                       transfer1_buffer_header_id,
                                       API_QUEUE_SIZE_1,
                                       API_BC_TIC_NO_INT);


    AIM_TRACE_RETURN( ret );


    /* -- RT4RcvSA14 -> BC -- */

    transfer2_buffer_header_id = MilUtilNextBCHid();

    ret = MilUtilBCCreateTransfer(ulModHandle,
        RT2BC_TRANSFER_ID,
        API_BC_TYPE_RTBC,
        4,   /* Transmit RT */
        14,  /* Transmit SA */
        0,   /* Receive  RT */
        0,   /* Receive  SA */
        0,   /* Word count 32 */
        transfer2_buffer_header_id,
        API_QUEUE_SIZE_1,
        API_BC_TIC_NO_INT);


    AIM_TRACE_RETURN(ret);


    /* -- RT4RcvSA14 -> RT3RcvSA13 -- */

    ret = MilUtilBCCreateTransfer(ulModHandle,
        RT2RT_TRANSFER_ID,
        API_BC_TYPE_RTRT,
        4,   /* Transmit RT */
        14,  /* Transmit SA */
        3,   /* Receive  RT */
        13,  /* Receive  SA */
        0,   /* Word count 32 */
        MilUtilNextBCHid(),
        API_QUEUE_SIZE_1,
        API_BC_TIC_NO_INT);


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

    printf("RunSimulation ... ");

    /* Start all enabled RTs */
    ret = ApiCmdRTStart( ulModHandle, 0 );

    AIM_ERROR_MESSAGE_RETURN( ret );

    /* Start BC */
    /* Start BC with count 0 means run forever */
    /* ApiCmdBCStart(   module, 0, mode,                     count, mframe time, 0, major_frame_out,  minor_frame_out ); */
    ret = ApiCmdBCStart( ulModHandle, 0, API_BC_START_IMMEDIATELY,     0, 10,          0, &ulMajorAddr,     aulMinorAddr );

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

    printf("done\n");
    return 0;
}




AiReturn SetTransmitData(AiUInt32 ulModHandle, AiUInt16 uwBufferValue, AiUInt8 ucBufferType, AiUInt16 uwTransmitBufferHeader)
{
    AiInt16 i;
    AiReturn ret = 0;
    AiUInt16 auwMessageData[32];

    /* Data to send */
    for (i = 0; i < 32; i++)
        auwMessageData[i] = uwBufferValue;

    ret = MilUtilSetData(ulModHandle, uwTransmitBufferHeader, ucBufferType, 1 /*First buffer in queue*/, 32, auwMessageData, NULL);

    AIM_TRACE_RETURN(ret);

    return ret;
}

AiReturn CheckReceiveData(AiUInt32 ulModHandle, AiUInt16 uwBufferValue, AiUInt8 ucBufferType, AiUInt16 uwTransmitBufferHeader)
{
    AiInt16 i;
    AiReturn ret = 0;
    AiUInt16 auwMessageData[32];

    ret = MilUtilGetData(ulModHandle, uwTransmitBufferHeader, ucBufferType, 0 /* Read current buffer */, 32, auwMessageData);

    AIM_TRACE_RETURN(ret);

    /* Check data */
    for (i = 0; i < 32; i++)
    {
        if( auwMessageData[i] != uwBufferValue )
        {
            ret = API_ERR;
            printf("CheckReceiveData: Invalid at index=%d, %d != %d\n", i, auwMessageData[i], uwBufferValue );
            break;
        }
    }


    return ret;
}


AiReturn SendAcyclicDataBlocking(AiUInt32 ulModHandle, AiUInt16 uwTransferId )
{
    AiReturn ret = 0;
    TY_API_BC_XFER_DSP   xfer_status;
    TY_API_BC_ACYC       xAcyclicFrame;

    memset(&xfer_status,   0, sizeof(TY_API_BC_XFER_DSP));
    memset(&xAcyclicFrame, 0, sizeof(TY_API_BC_ACYC));

    /* assign the transfer to the acyclic frame */
    xAcyclicFrame.cnt        = 1;
    xAcyclicFrame.xid[0] = uwTransferId;
    xAcyclicFrame.instr[0] = API_BC_INSTR_TRANSFER;

    /* -- Install Acyclic Frame --- */
    ret = ApiCmdBCAcycPrep(ulModHandle, 0, &xAcyclicFrame);

    AIM_ERROR_MESSAGE_RETURN(ret);

    /* Clear transfer buffer status and set BRW/BUF_STAT to API_BUF_NOT_USED. 
       Please note that BRW/BUF_STAT are part of the status queue assigned to the transfer buffer header. 
       Please make sure that the Buffer Header id used for this transfer is not used for any other transfer.
       Please make also sure that only one status queue entry is used by specifying either "qsize=API_QUEUE_SIZE_1" or "sqm=API_SQM_ONE_ENTRY_ONLY" when calling ApiCmdBCBHDef.
    */
    ret = ApiCmdBCXferRead(ulModHandle, 0, uwTransferId, 0x7 /*clear status on read*/, &xfer_status);
    AIM_ERROR_MESSAGE_RETURN(ret);

    /* ApiCmdBCAcycSend will hand over the acyclic frame pointer to the firmware. 
       This function is non blocking. The transfer will be sent by the firmware at an appropriate time.
       It is important to check the status of the last transfer in the acyclic frame to detect if the acyclic frame has been sent completely.
       To do so we use the ApiCmdBCXferRead, BRW/BUF_STAT value. See below.
    */
    /* ApiCmdBCAcycSend(             module, biu, mode,                         timetag_high, timetag_low */
    ret = ApiCmdBCAcycSend(ulModHandle, 0, API_BC_ACYC_SEND_IMMEDIATELY, 0, 0);
    AIM_ERROR_MESSAGE_RETURN(ret);

    do
    {
        /* uwTransferId must be the last Transfer in the acyclic frame and must not be part of the cyclic framing*/
        ret = ApiCmdBCXferRead(ulModHandle, 0, uwTransferId, 0x7 /*clear status on read*/, &xfer_status);
        AIM_ERROR_MESSAGE_RETURN(ret);

        /* Receive  buffers (RT to BC / RT to RT) will have BRW/BUF_STAT set to API_BUF_FULL  when the transfer is finished */
        /* Transmit buffers (BC to RT)            will have BRW/BUF_STAT set to API_BUF_EMPTY when the transfer is finished */
    } while (((xfer_status.brw >> 12) & 0x3) == API_BUF_NOT_USED);

    return ret;
}


AiReturn PrintSimulationStatistic(AiUInt32 ulModHandle)
{
    AiReturn ret;
    TY_API_BC_STATUS_DSP xBcStatus;
    TY_API_RT_STATUS_DSP xRtStatus;
    TY_API_BM_STATUS_DSP xBmStatus;

    memset(&xBcStatus, 0, sizeof(xBcStatus));
    memset(&xRtStatus, 0, sizeof(xRtStatus));
    memset(&xBmStatus, 0, sizeof(xBmStatus));

    printf("PrintSimulationStatistic ...\n");

    ret = ApiCmdBCStatusRead(ulModHandle, 0, &xBcStatus);

    AIM_ERROR_MESSAGE_RETURN(ret);

    if ((xBcStatus.glb_msg_cnt != BC_TRANSFER_COUNT) || (xBcStatus.glb_err_cnt != 0))
    {
        printf("Expected to receive %d transfers and 0 errors got (%d/%d)\n", BC_TRANSFER_COUNT, xBcStatus.glb_msg_cnt, xBcStatus.glb_err_cnt);
    }
    else
    {
        printf("BC  ( %d messages / %d errors )\n", xBcStatus.glb_msg_cnt, xBcStatus.glb_err_cnt);
    }

    ret = ApiCmdRTStatusRead(ulModHandle, 0, &xRtStatus);

    AIM_ERROR_MESSAGE_RETURN(ret);

    printf("RTs ( %d messages  / %d errors )\n", xRtStatus.glb_msg_cnt, xRtStatus.glb_err_cnt);

    printf("Please note that one RT to RT transfer counts as two messages in RTs and BM\n");
    printf("PrintSimulationStatistic ... done\n");
    return 0;
}
