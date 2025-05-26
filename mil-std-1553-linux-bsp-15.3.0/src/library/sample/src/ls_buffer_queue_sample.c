/* SPDX-FileCopyrightText: 2010-2022 AIM GmbH <info@aim-online.com> */
/* SPDX-License-Identifier: MIT */

/*
This sample shows how to handle receive buffer queues.
This sample creates a cyclic frame with one BC2RT transfer.
The transfer has a buffer queue with 16 buffers enabled on both the transmitter and receiver side.
Each data word in the transmit buffer queue is initialized with a unique data word.
The BC framing is then send with a total of 2000 transfers and the RT enabled.
During the framing the RT data is continuously read and the receive data buffer
consistency is ensured through the use of the buffer queue.
For each received transfer on the RT the first three and the last data words
are checked to match the expected data for the transfer.
At the end of the framing the BC and RT status is printed.
This status should show the expected amount of transfers and no errors.
This sample is executed with internal coupling.No external connection is required.
*/

#include "1553_sample.h"


static AiReturn PrepareRemoteTerminals(AiUInt32 ulModHandle);
static AiReturn PrepareBusController(AiUInt32 ulModHandle);
static AiReturn RunSimulation(AiUInt32 ulModHandle);
static AiReturn PollRtReceiveBufferQueue(AiUInt32 ulModHandle);
static AiReturn StopSimulation(AiUInt32 ulModHandle);
static AiReturn PrintSimulationStatistic(AiUInt32 ulModHandle);


static AiReturn CheckData(AiUInt32 ulTimeTag, AiUInt16 * pData, AiUInt32 ulStatus);

static AiUInt32 uwGlobalLastTimeTag   = 0;
static AiUInt32 uwGlobalTransferCount = 0;
static AiUInt16 rt3_sa13_buffer_header_id;

/* ######################################################################################*/
/*                                                                                       */
/*                                     Sample entry point                                */
/*                                                                                       */
/* ######################################################################################*/
AiReturn LsBufferQueueSample(AiUInt32 ulModHandle)
{
    AiReturn ret;

    printf("Running buffer/event queue sample\n");

    /* Setup RTs and BC */
    ret = PrepareRemoteTerminals( ulModHandle );
    
    AIM_TRACE_RETURN(ret);

    ret = PrepareBusController(   ulModHandle );

    AIM_TRACE_RETURN(ret);

    /* Start the simulation and the bus monitor */
    ret = RunSimulation( ulModHandle );

    AIM_TRACE_RETURN(ret);

    /* Read data from RT */
    ret = PollRtReceiveBufferQueue(ulModHandle);

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
    AiReturn ret        = 0;

    printf("PrepareRemoteTerminals ... ");

    /*********************************************/
    /*                                           */
    /* --      Simulate RT3 receive SA13     --- */
    /*                                           */
    /*********************************************/
    rt3_sa13_buffer_header_id = MilUtilNextRTHid();
    
    ret = MilUtilEnableRT(ulModHandle, 3 /*RT*/, 8.0);

    AIM_TRACE_RETURN(ret);

    ret = MilUtilEnableRTSABufferQueue(ulModHandle, 3 /*RT*/, 13 /*SA*/, API_RT_TYPE_RECEIVE_SA, rt3_sa13_buffer_header_id, API_QUEUE_SIZE_16);

    AIM_TRACE_RETURN(ret);

    printf("done\n");
    return 0;
}







AiReturn PrepareBusController(AiUInt32 ulModHandle)
{
    AiUInt16 i          = 0;
    AiUInt16 uwHeaderId      = 0;

    AiUInt16 auwTransferIds[1] ={ 1 };
    AiReturn  ret        = 0;
    AiUInt16 uwBufferOffset = 0;

    AiUInt16 auwMessageData[32];

    memset( &auwMessageData,  0, sizeof(auwMessageData)  );


    /*********************************************/
    /*                                           */
    /* --  Initialize the Bus Controller     --- */
    /*                                           */
    /*********************************************/
    ret = ApiCmdBCIni( ulModHandle, 0, 0, 0, 0, 0 );

    AIM_ERROR_MESSAGE_RETURN( ret );

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
    /*                                           */
    /* --            BC -> RT3RcvSA13        --- */
    /*                                           */
    /*********************************************/
    uwHeaderId = MilUtilNextBCHid();

    ret = MilUtilBCCreateTransfer(ulModHandle,
        auwTransferIds[0],  /* Transfer id */
        API_BC_TYPE_BCRT,
        0,  /* Transmit RT */
        0,  /* Transmit SA */
        3,  /* Receive  RT */
        13, /* Receive  SA */
        0,  /* Word count 32 */
        uwHeaderId,
        API_QUEUE_SIZE_16,
        API_BC_TIC_NO_INT);

    AIM_TRACE_RETURN(ret);

    /* Initialize all 16 buffers in the queue */
    for( uwBufferOffset = 1; uwBufferOffset <= 16; uwBufferOffset++ )
    {
        for( i = 0; i < 32; i++ )
            /* buffer  1 : auwMessageData[] = { 0x0001, 0x0002, 0x0003 ... 0x0032 } */
            /* buffer  2 : auwMessageData[] = { 0x1001, 0x1002, 0x1003 ... 0x1032 } */
            /* buffer  3 : auwMessageData[] = { 0x2001, 0x2002, 0x2003 ... 0x2032 } */
            /* ...                                                                  */
            /* buffer 16 : auwMessageData[] = { 0xF001, 0xF002, 0xF003 ... 0xF032 } */
            auwMessageData[ i ] = (0x1000 * (uwBufferOffset - 1)) + i + 1;

        ret = MilUtilSetData(ulModHandle, uwHeaderId, API_BUF_BC_MSG, uwBufferOffset, 32, auwMessageData, NULL);

        AIM_TRACE_RETURN(ret);
    }



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
    AiReturn ret         = 0;
    AiUInt32 ulMajorAddr     = 0;
    AiUInt32 aulMinorAddr[64];

    memset( &aulMinorAddr, 0, sizeof(aulMinorAddr) );

    printf("RunSimulation ... ");

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

    /* ApiCmdBCStart(   module, 0, mode,                     count, mframe time, 0, major_frame_out,  minor_frame_out ); */
    ret = ApiCmdBCStart( ulModHandle, 0, API_BC_START_IMMEDIATELY, 2000,  10,          0, &ulMajorAddr,     aulMinorAddr );

    AIM_ERROR_MESSAGE_RETURN( ret );

    printf("running\n");
    return 0;
}

AiReturn PollRtReceiveBufferQueue(AiUInt32 ulModHandle)
{
    AiUInt8  ucContinue             = 1;
    AiReturn ret                = 0;
    AiUInt32 ulLastFirmwareIndex    = 0;
    AiUInt32 ulCurrentFirmwareIndex = 0;
    AiUInt32 ulCurrentIndex         = 0;

    AiUInt16 auwData[32];

    TY_API_RT_SA_MSG_READ_IN xIdentifier;
    TY_API_RT_SA_STATUS_EX   xStatus;


    memset( &auwData[0],  0, sizeof(auwData)     );
    memset( &xIdentifier, 0, sizeof(xIdentifier) );
    memset( &xStatus,     0, sizeof(xStatus)     );

    printf("Poll data and status buffer queue ... \n");


    xIdentifier.ul_RtAddr = 3;
    xIdentifier.ul_SA     = 13;
    xIdentifier.ul_SaType = API_RT_TYPE_RECEIVE_SA;

    while( ucContinue )
    {
        ucContinue = MilUtilIsBcActive(ulModHandle);

        ret = ApiCmdRTSAMsgReadEx( ulModHandle, &xIdentifier, &xStatus );

        if( ret != API_OK )
        {
            ucContinue = 0;
            AIM_ERROR_MESSAGE( ret );
            break;
        }
   
        ulCurrentFirmwareIndex = xStatus.x_XferInfo.ul_ActBufferId;

        if( ulLastFirmwareIndex != ulCurrentFirmwareIndex )
        {
            /* Read from last buffer index to current buffer index */
            /* The current buffer index is not valid and will be read next time */
            ulCurrentIndex = ulLastFirmwareIndex;

#ifdef VERBOSE
            printf("\n");
            printf("    Queue  | DW1  | DW2  | DW3  | ... | DW32 | Timetag   | Status   |\n" );
            printf("---------------------------------------------------------------------\n" );
#endif


            while( ulCurrentIndex != ulCurrentFirmwareIndex )
            {
                ret = MilUtilGetData(ulModHandle,
                                        rt3_sa13_buffer_header_id,
                                        API_BUF_RT_MSG,
                                        ulCurrentIndex + 1,
                                        32,
                                        auwData );

                if( ret != API_OK )
                {
                    AIM_ERROR_MESSAGE( ret );
                    ucContinue = 0;
                    break;
                }

#ifdef VERBOSE
                printf("Buffer %2d  | %04X | %04X | %04X | ... | %04X | %08X  | %08X |\n",
                                                          ulCurrentIndex, 
                                                          auwData[0], auwData[1], auwData[2], auwData[31],
                                                          xStatus.x_XferSQueue[ulCurrentIndex].ul_SqTimeTag,
                                                          xStatus.x_XferSQueue[ulCurrentIndex].ul_SqStatusWords );
#else
                if ((uwGlobalTransferCount%100) == 0)
                    printf("Processed %d buffers\n", uwGlobalTransferCount);
#endif

                ret = CheckData( 
                              xStatus.x_XferSQueue[ulCurrentIndex].ul_SqTimeTag,
                              &auwData[0],
                              xStatus.x_XferSQueue[ulCurrentIndex].ul_SqStatusWords 
                          );

                if( ret != API_OK )
                {
                    AIM_ERROR_MESSAGE( ret );
                    ucContinue = 0;
                    break;
                }


                ucContinue = 1;
                /* ulCurrentIndex = 0, 1, 2, 3, ... 15 */
                ulCurrentIndex = (ulCurrentIndex+ 1) % 16;
            }

            ulLastFirmwareIndex = ulCurrentFirmwareIndex;
        }

        AiOsSleep(50);
    }

    printf("Processed %d buffers\n", uwGlobalTransferCount);

    return ret;
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
    TY_API_RT_STATUS_DSP xRtStatus;

    memset( &xBcStatus, 0, sizeof(xBcStatus) );
    memset( &xRtStatus, 0, sizeof(xRtStatus) );

    printf("PrintSimulationStatistic ...\n");

    ret = ApiCmdBCStatusRead( ulModHandle, 0, &xBcStatus );

    AIM_ERROR_MESSAGE_RETURN( ret );

    printf("BC  ( %d messages / %d errors )\n", xBcStatus.glb_msg_cnt, xBcStatus.glb_err_cnt );

    ret = ApiCmdRTStatusRead( ulModHandle, 0, &xRtStatus );

    AIM_ERROR_MESSAGE_RETURN( ret );

    printf("RTs ( %d messages  / %d errors )\n", xRtStatus.glb_msg_cnt, xRtStatus.glb_err_cnt );

    printf("Please note that one RT to RT transfer counts as two messages in RTs and BM\n");
    printf("PrintSimulationStatistic ... done\n");
    return 0;
}





AiReturn CheckData(AiUInt32 ulTimeTag, AiUInt16 * pData, AiUInt32 ulStatus)
{
    AiUInt8 ucQueue  = 0;

    AiUInt16 uwExpectedWord_00 = 0x0001;
    AiUInt16 uwExpectedWord_01 = 0x0002;
    AiUInt16 uwExpectedWord_02 = 0x0003;
    AiUInt16 uwExpectedWord_31 = 0x0020;

    /* -- Check timetag --- */

    if( ulTimeTag <= uwGlobalLastTimeTag )
    {
        printf("Error : Timetag not greater than last timetag for transfer %d\n", uwGlobalTransferCount );
        return 1;
    }

    uwGlobalLastTimeTag = ulTimeTag;
    

    /* -- Check data buffer --- */
    ucQueue = (uwGlobalTransferCount % 16);

    uwExpectedWord_00 = (ucQueue * 0x1000) + 0x0001;
    uwExpectedWord_01 = (ucQueue * 0x1000) + 0x0002;
    uwExpectedWord_02 = (ucQueue * 0x1000) + 0x0003;
    uwExpectedWord_31 = (ucQueue * 0x1000) + 0x0020;

    if(    (pData[ 0] != uwExpectedWord_00)
        || (pData[ 1] != uwExpectedWord_01)
        || (pData[ 2] != uwExpectedWord_02)
        || (pData[31] != uwExpectedWord_31) )
    {
        printf("Error : Invalid data for transfer %d\n", uwGlobalTransferCount );
        return 2;
    }



    uwGlobalTransferCount++;
    return 0;
}
