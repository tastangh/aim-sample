/* SPDX-FileCopyrightText: 2010-2024 AIM GmbH <info@aim-online.com> */
/* SPDX-License-Identifier: MIT */

/*
This sample shows how to record 1553 bus monitor data and how replay the data afterwards.
This sample / test consists of two parts.The recording part and the replay part.

The recording part :
The sample creates a cyclic framing with a BC2RT transfer.
The transfer has a dynamic data word generation in data word position 1 enabled.
The transfer is send 5000 times with RT and BM enabled.
The test reads the BM with a threshold of a minimum of 128KB per BM read.
The BM data is parsed to count the number of transfers in the data.
Then the data is stored in a temporary file.
At the end of the framing the test checks the recording file size and the transfer count obtained from the data.

The replay part :
This part is only executed if there was no error in the recording part.
The test reads the temporary file generated in the recording step and replays it with the RT enabled.
At the end of the replay the RT status is checked if the correct amount of transfers has been received.
This test is executed with internal coupling.No external connection is required.
*/


#include "1553_sample.h"


static AiReturn PrepareSimulation(AiUInt32 ulModHandle);
static AiReturn RunSimulation(AiUInt32 ulModHandle);
static AiReturn StopSimulation(AiUInt32 ulModHandle);

static AiReturn Recording(AiUInt32 ulModHandle, AiUInt32 ulStream, AiUInt8 ucUseAns);
static AiReturn StartRecording(AiUInt32 ulModHandle, AiUInt8 ucQueueId, AiUInt8 ucUseAns);
static AiReturn StopRecording(AiUInt32 ulModHandle, AiUInt8 ucQueueId);
static AiReturn CloseRecording(AiUInt32 ulModHandle, AiUInt8 ucQueueId);
static AiReturn CheckRecording(AiUInt32 ulModHandle, AiUInt32 ulTotalMessageCount);
static AiReturn IsQueueDataAvailable(AiUInt32 ulModHandle, AiUInt8 ucQueueId, AiUInt32 * pulDataAvailable);
static AiReturn ReadData(AiUInt32 ulModHandle, AiUInt8 ucQueueId, FILE * pFile, AiUInt32 ulDataAvailable, AiUInt32 * pulMessageCount);





static AiReturn Replay(AiUInt32 ulModHandle);
static AiReturn StartRtMonitoring(AiUInt32 ulModHandle);
static AiReturn Replaying(AiUInt32 ulModHandle);
static AiReturn StopRtSimulation(AiUInt32 ulModHandle);
static AiReturn CheckReceivedData(AiUInt32 ulModHandle);


/* Global constants */
static char szGlobalFileName[AI_OS_MAX_PATH];


#define GLOBAL_RECORDING_BUFFER_SIZE (128*1024) /* Read  blocks of 128 KB */
#define GLOBAL_REPLAY_BUFFER_SIZE    (0x10000)


/* ######################################################################################*/
/*                                                                                       */
/*                                     Sample entry point                                */
/*                                                                                       */
/* ######################################################################################*/
AiReturn LsRecordingReplaySample(AiUInt32 ulModHandle, AiUInt32 ulStream, AiUInt8 ucUseAns)
{
    AiReturn ret = 0;

    TY_API_RESET_INFO    xApiResetInfo;
    
    memset( &xApiResetInfo, 0, sizeof(xApiResetInfo) );

    printf("Running LsRecordingReplaySample\n");



    /* Create tmp file name with path */

    AiOsGetTempPath( szGlobalFileName, AI_OS_MAX_PATH );
    
    if( strlen(szGlobalFileName) > (AI_OS_MAX_PATH - strlen("ls_recording_replay_sample.prc") ) )
    {
        printf("Error : Temp path too long\n" );
        return 1;
    }

    strcpy( &szGlobalFileName[strlen(szGlobalFileName)], "ls_recording_replay_sample.prc" );

    /* Recording sample */
   ret = Recording( ulModHandle, ulStream, ucUseAns );

   AIM_TRACE_RETURN(ret);


    /* Replay sample */
    ret = Replay(ulModHandle);

    AIM_TRACE_RETURN(ret);


    return 0;

}






/**********************************************************************
 *                                                                    *
 *                                                                    *
 *                            Recording                               *
 *                                                                    *
 *                                                                    *
 **********************************************************************/
AiUInt8 ucStream2DataQueueIdMax = 8;
AiUInt8 aucStream2DataQueueId[] = { 0 /* invalid stream number */, 
                                    API_DATA_QUEUE_ID_BM_REC_BIU1,
                                    API_DATA_QUEUE_ID_BM_REC_BIU2,
                                    API_DATA_QUEUE_ID_BM_REC_BIU3,
                                    API_DATA_QUEUE_ID_BM_REC_BIU4,
                                    API_DATA_QUEUE_ID_BM_REC_BIU5,
                                    API_DATA_QUEUE_ID_BM_REC_BIU6,
                                    API_DATA_QUEUE_ID_BM_REC_BIU7,
                                    API_DATA_QUEUE_ID_BM_REC_BIU8 };

AiReturn Recording(AiUInt32 ulModHandle, AiUInt32 ulStream, AiUInt8 ucUseAns)
{
    AiReturn  ret             = 0;
    AiUInt8   ucQueueId           = 0;
    AiUInt32  ulDataAvailable     = 0;
    AiUInt32  ulMessageCount      = 0;
    AiUInt32  ulTotalMessageCount = 0;
    AiUInt32  ulBiu               = 0;
    FILE    * pWriteFile          = NULL;

    TY_API_DATA_QUEUE_READ xDataQueue;



    memset( &xDataQueue,         0, sizeof(xDataQueue)         );

    ulBiu = MilUtilBMGetLogicalBiuFromStream(ulModHandle, ulStream, 0);

    if (ulBiu > ucStream2DataQueueIdMax)
    {
        printf("Invalid biu %d\n", ulBiu);
        return 1;
    }

    /* convert stream to data queue id */
    ucQueueId = aucStream2DataQueueId[ulBiu];


    /* Setup RTs and BC */
    ret = PrepareSimulation( ulModHandle );

    AIM_TRACE_RETURN( ret );

    /* Start the simulation and the bus monitor */
    ret = StartRecording( ulModHandle, ucQueueId, ucUseAns );

    AIM_TRACE_RETURN( ret );

    ret = RunSimulation( ulModHandle );

    AIM_TRACE_RETURN( ret );

    printf("Recording :\n");


    /************************************************/
    /*                                              */
    /* --    Open output file in tmp directory  --- */
    /*                                              */
    /************************************************/

    pWriteFile = fopen( szGlobalFileName, "wb" );


    if( pWriteFile == NULL )
    {
        printf("Error : could not open file %s\n", szGlobalFileName );
        return 1;
    }
    else
    {
        printf("Recording file %s\n", szGlobalFileName );
    }

    /************************************************/
    /*                                              */
    /* --  Read BM data as long as BC is active --- */
    /*                                              */
    /************************************************/
    while( MilUtilIsBcActive( ulModHandle ) )
    {

        ret = IsQueueDataAvailable( ulModHandle, ucQueueId, &ulDataAvailable );

        AIM_TRACE_RETURN( ret );

        if( ulDataAvailable < GLOBAL_RECORDING_BUFFER_SIZE )
        {
            /* We want to read blocks of 128 KB. */
            /* no enough data available       */
            /* wait some time and check again */
            AiOsSleep(50);
            continue;
        }

        /* -- read available data --- */
        ret = ReadData( ulModHandle, ucQueueId, pWriteFile, ulDataAvailable, &ulMessageCount );

        AIM_TRACE_RETURN( ret );

        ulTotalMessageCount += ulMessageCount;
    }


    /************************************************/
    /*                                              */
    /* --  Stopp the BM and the recording queue --- */
    /*                                              */
    /************************************************/
    ret = StopRecording( ulModHandle, ucQueueId );

    AIM_TRACE_RETURN( ret );

    /************************************************/
    /*                                              */
    /* --  Read leftovers from recording queue  --- */
    /*                                              */
    /************************************************/

    AiOsSleep(300);

    do
    {
        ret = IsQueueDataAvailable( ulModHandle, ucQueueId, &ulDataAvailable );

        AIM_TRACE_RETURN( ret );

        if( ulDataAvailable == 0 )
        {
            /* The simulation and the monitor are stopped */
            /* If the queue is empty we should be done    */
            break;
        }

        ret = ReadData( ulModHandle, ucQueueId, pWriteFile, ulDataAvailable, &ulMessageCount );

        AIM_TRACE_RETURN( ret );

        ulTotalMessageCount += ulMessageCount;
    }
    while(1);



    /************************************************/
    /*                                              */
    /* --               Clean up                --- */
    /*                                              */
    /************************************************/
    StopSimulation( ulModHandle );
    CloseRecording( ulModHandle, ucQueueId );

    fclose(pWriteFile);

    /* Check if the file recorded is ok */
    ret = CheckRecording( ulModHandle, ulTotalMessageCount );
    AIM_TRACE_RETURN(ret);

    return ret;
}




AiReturn IsQueueDataAvailable(AiUInt32 ulModHandle, AiUInt8 ucQueueId, AiUInt32 * pulDataAvailable)
{
    AiReturn                 ret     = 0;
    TY_API_DATA_QUEUE_READ   xDataQueue;
    TY_API_DATA_QUEUE_STATUS xDataQueuStatus;

    memset(&xDataQueue,      0, sizeof(xDataQueue));
    memset(&xDataQueuStatus, 0, sizeof(xDataQueuStatus));

    /* read 0 bytes to check if data is available */
    xDataQueue.id             = ucQueueId;
    xDataQueue.bytes_to_read  = 0;
    xDataQueue.buffer         = NULL;

    ret = ApiCmdDataQueueRead(ulModHandle, &xDataQueue, &xDataQueuStatus);

    AIM_ERROR_MESSAGE_RETURN( ret );

    if( pulDataAvailable ) 
        *pulDataAvailable = xDataQueuStatus.bytes_in_queue;

    return 0;
}


AiReturn ReadData(AiUInt32 ulModHandle, AiUInt8 ucQueueId, FILE * pFile, AiUInt32 ulDataAvailable, AiUInt32 * pulMessageCount )
{
    AiReturn ret = 0;
    AiUInt8 *aucRecordingBuffer;
    TY_API_DATA_QUEUE_READ xDataQueue;
    TY_API_DATA_QUEUE_STATUS xDataQueuStatus;
    MIL_UTIL_BM_ITERATOR   xBmIterator;
    TY_API_QUEUE_BUF_EX    xBmInterpretedMessage;

    aucRecordingBuffer = (AiUInt8 *)AiOsMalloc(ulDataAvailable);
    if(aucRecordingBuffer==0)
    {
        printf("unable to allocate aucRecordingBuffer\n");
        return 1;
    }

    memset( aucRecordingBuffer, 0, ulDataAvailable );
    memset(&xDataQueue, 0, sizeof(xDataQueue));
    memset(&xDataQueuStatus, 0, sizeof(xDataQueuStatus));

    xDataQueue.id     = ucQueueId;
    xDataQueue.buffer = aucRecordingBuffer;

    xDataQueue.bytes_to_read = ulDataAvailable;


    ret = ApiCmdDataQueueRead(ulModHandle, &xDataQueue, &xDataQueuStatus);

    AIM_ERROR_MESSAGE_RETURN( ret );

    if ((xDataQueuStatus.status & 0xFFFF))
    {
        printf("Error : ApiCmdDataQueueRead failed with status 0x%X\n", xDataQueuStatus.status);
        return 1;
    }

    /* Save buffer to file */
    fwrite(aucRecordingBuffer, 1, xDataQueuStatus.bytes_transfered, pFile);

    MIL_UTIL_BM_INIT_ITERATOR(&xBmIterator, aucRecordingBuffer, xDataQueuStatus.bytes_transfered);

    *pulMessageCount = 0;
    do
    {
        ret = MilUtilBMLsInterpretData(ulModHandle, &xBmIterator, &xBmInterpretedMessage);

        if(ret == API_OK)
            (*pulMessageCount)++;
    }while(ret == API_OK);

    if(ret != API_ERR_QUEUE_EMPTY)
        AIM_ERROR_MESSAGE(ret);

    printf("Monitor received %.2f KB with %d transfers\n",
        (float)xDataQueuStatus.bytes_transfered / 1024,
        (*pulMessageCount));


    AiOsFree( aucRecordingBuffer );

    return 0;
}




AiReturn PrepareSimulation(AiUInt32 ulModHandle)
{
    AiUInt16  i         = 0;
    AiReturn ret    = 0;
    AiUInt16    auwTransferIds[1] ={ 1 };

    AiUInt16 uwHeaderId = 0;
    AiUInt16 auwMessageData[32];

    TY_API_BC_DYTAG    axDytag[4];

    memset( &auwMessageData,  0, sizeof(auwMessageData)  );
    memset( axDytag,          0, sizeof(TY_API_BC_DYTAG) * 4 );


    printf("PrepareSimulation ... ");

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
    /* --  Initialize the Bus Controller     --- */
    /*                                           */
    /*********************************************/
    ret = ApiCmdBCIni( ulModHandle, 0, 0, 0, 0, 0 );

    AIM_ERROR_MESSAGE_RETURN( ret );

    printf("\n");
    printf(" %s\n %s\n %s\n %s\n %s\n", 
                    "Framing :     5000 times ",
                    "------------------------ ",
                    "BC         -> RT3RcvSA13 ",
                    "------------------------ ",
                    "    total 5000 transfers " );
    printf("\n");

    /*********************************************/
    /*                                           */
    /* --            BC -> RT3RcvSA13        --- */
    /*                                           */
    /*********************************************/
    uwHeaderId = MilUtilNextBCHid();

    ret = MilUtilBCCreateTransfer( ulModHandle, 
                                       auwTransferIds[0],  /* Transfer Id*/
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

    /**********************************************/
    /*                                            */
    /* -- Enable Dytag for dataword 1 in xfer --- */
    /*                                            */
    /**********************************************/
    axDytag[ 0 ].tag_fct = API_DYTAG_FCT_POS_RAMP;

    /* Lower Limit */
    axDytag[ 0 ].min = 0;
    /* Upper Limit */
    axDytag[ 0 ].max = 0xFFFF; 
    /* Step size */
    axDytag[ 0 ].step = 1;
    /* Maniputlate first data word  */
    axDytag[ 0 ].wpos = 0;

    ret = ApiCmdBCDytagDef( ulModHandle, 0, API_ENA, uwHeaderId, API_DYTAG_STD_MODE, axDytag );

    AIM_ERROR_MESSAGE_RETURN( ret );

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
    AiReturn ret = 0;

    AiUInt32 ulMajorAddr     = 0;
    AiUInt32 aulMinorAddr[64];

    memset( &aulMinorAddr, 0, sizeof(aulMinorAddr) );

    printf("RunSimulation ... ");

    /* -- Start all enabled RTs --- */
    ret = ApiCmdRTStart( ulModHandle, 0 );

    AIM_ERROR_MESSAGE_RETURN( ret );

    /* -- Start Bus Controller --- */

    /* ApiCmdBCStart(   module, 0, mode,                     count, mframe time, 0, major_frame_out,  minor_frame_out ); */
    ret = ApiCmdBCStart( ulModHandle, 0, API_BC_START_IMMEDIATELY, 5000,  10,          0, &ulMajorAddr,     aulMinorAddr );

    AIM_ERROR_MESSAGE_RETURN( ret );

    printf("running\n");
    return 0;
}


AiReturn StopSimulation(AiUInt32 ulModHandle)
{
    AiReturn ret = 0;
    
    printf("StopSimulation ... ");

    /* Stop Bus Controller */
    ApiCmdBCHalt(    ulModHandle, 0 );
    /* Stop all RTs */
    ApiCmdRTHalt(    ulModHandle, 0 );
    /* Stop Bus Monitor */
    ApiCmdBMHalt(ulModHandle, 0);

    printf("done\n");
    return 0;
}








AiReturn StartRecording(AiUInt32 ulModHandle, AiUInt8 ucQueueId, AiUInt8 ucUseAns)
{
    AiUInt32  size = 0;
    AiReturn  ret = 0;

    TY_API_BM_CAP_SETUP       xCaptureMode;

    memset( &xCaptureMode,        0, sizeof(xCaptureMode)     );

    printf("StartRecording ... ");

    /* Setup Bus Monitor for Low speed Recording */
    ret = ApiCmdBMIni( ulModHandle, 0 );

    AIM_ERROR_MESSAGE_RETURN( ret );

    xCaptureMode.cap_mode = API_BM_CAPMODE_RECORDING;

    ret = ApiCmdBMCapMode( ulModHandle, 0, &xCaptureMode );

    AIM_ERROR_MESSAGE_RETURN( ret );

    /* Start Bus Monitor */
    ret = ApiCmdBMStart( ulModHandle, 0 );

    AIM_ERROR_MESSAGE_RETURN( ret );
    
    /* -- open data queue --- */

    ret = ApiCmdDataQueueOpen(ulModHandle, ucQueueId, &size);

    AIM_ERROR_MESSAGE_RETURN( ret );

    printf("queue_size(0x%x) ... ", size );

    /* -- start data queue --- */
    ret = ApiCmdDataQueueControl(ulModHandle, ucQueueId, API_DATA_QUEUE_CTRL_MODE_START);

    AIM_ERROR_MESSAGE_RETURN( ret );

    printf("done\n");
    return 0;
}





AiReturn StopRecording(AiUInt32 ulModHandle, AiUInt8 ucQueueId)
{
    AiReturn                  ret  = 0;

    printf("StopRecording ... ");

    /* boards without an ASP might require some additional time */
    AiOsSleep(100);

    /* The BM should already be stopped. */
    /* This stop is only required in case of an error during the recording process. */
    ret = ApiCmdBMHalt( ulModHandle, 0 );

    AIM_ERROR_MESSAGE_RETURN( ret );

    ret = ApiCmdDataQueueControl(ulModHandle, ucQueueId, API_DATA_QUEUE_CTRL_MODE_STOP);

    AIM_ERROR_MESSAGE_RETURN( ret );

    printf("done\n");
    return ret;
}

AiReturn CloseRecording(AiUInt32 ulModHandle, AiUInt8 ucQueueId)
{
    AiReturn                  ret  = 0;

    printf("CloseRecording ... ");

    ret = ApiCmdDataQueueClose(ulModHandle, ucQueueId );

    AIM_ERROR_MESSAGE_RETURN( ret );

    printf("done\n");
    return ret;
}




AiReturn CheckRecording(AiUInt32 ulModHandle, AiUInt32 ulTotalMessageCount )
{
    AiReturn ret        = 0;
    AiUInt32 ulFileSize     = 0;

	if (ulTotalMessageCount != 5000 )
    {
        printf("Error : Expected to receive 5000 transfers got %d\n", ulTotalMessageCount );
        return 1;
    }
    else
    {
        printf("Received %d transfers\n", ulTotalMessageCount);
    }

    /* 5000 transfers with 36 entries out of 4 bytes */
    AiUInt32 ulExpectedSize = 5000 * (36 * 4);

    /* We do only check the filesize as a minimal condition */
    ret = AiOsGetFileSize( szGlobalFileName, &ulFileSize );

    if( ret )
    {
        printf("Error : could not get file size of %s\n", szGlobalFileName );
        return 1;
    }

    if( ulFileSize != ulExpectedSize )
    {
        printf("Error : Expected recording file size %d is %d\n", ulExpectedSize, ulFileSize );
        return 2;
    }

    return 0;
}










/**********************************************************************
 *                                                                    *
 *                                                                    *
 *                                  Replay                            *
 *                                                                    *
 *                                                                    *
 **********************************************************************/

AiReturn Replay(AiUInt32 ulModHandle)
{
    AiReturn ret = 0;

    ret = StartRtMonitoring(ulModHandle);

    AIM_TRACE_RETURN( ret );

    ret = Replaying( ulModHandle );

    AIM_TRACE_RETURN( ret );

    ret = StopRtSimulation( ulModHandle );

    AIM_TRACE_RETURN( ret );

    ret = CheckReceivedData( ulModHandle );

    AIM_TRACE_RETURN( ret );

    return 0;
}



AiReturn StartRtMonitoring(AiUInt32 ulModHandle)
{
    AiReturn ret    = 0;
    AiUInt8  ucRt       = 0;
    AiUInt8  ucSa       = 0;
    AiUInt16 uwHeaderId = 0;
    AiUInt16 uwBufferId = 0;

    printf("StartRtMonitoring ... ");

    /* By default all RTs are replayed by the Replay engine.
       If you need to responde with a real RT you may disable 
       the RT with ApiCmdReplayRT after the ApiCmdReplayIni call.

       The code below enables the RT for monitoring only. 
       The RT setup here will not responde on the bus.
       This is optional and is only done to obtain the statistics later.    
     */


    /*********************************************/
    /*                                           */
    /* --      Mailbox RT3 receive SA13      --- */
    /*                                           */
    /*********************************************/
    ret = MilUtilEnableMailboxRT(ulModHandle, 3 /*RT*/);

    AIM_TRACE_RETURN( ret );

    ret = MilUtilEnableRTSA(ulModHandle, 3 /*RT*/, 13 /*SA*/, API_RT_TYPE_RECEIVE_SA, MilUtilNextRTHid());

    AIM_TRACE_RETURN( ret );

    /* -- Start all enabled RTs --- */
    ret = ApiCmdRTStart( ulModHandle, 0 );

    AIM_ERROR_MESSAGE_RETURN( ret );

    /* -- Start Bus Montior to get transer statistics --- */
    ret = ApiCmdBMStart( ulModHandle, 0 );

    AIM_ERROR_MESSAGE_RETURN( ret );

    printf("done\n");
    return 0;
}




static AiUInt32 WriteReplayBufferFromFile( AiUInt32 ulModHandle, FILE * pFile )
{
    AiReturn            ret         = 0;
    AiUInt32            ulBytes         = 0;
    AiUInt32          * aucReplayBuffer = NULL;

    TY_API_REP_STATUS   xReplayStatus;

    memset( &xReplayStatus, 0, sizeof(xReplayStatus) );


    aucReplayBuffer = (AiUInt32*)AiOsMalloc(GLOBAL_REPLAY_BUFFER_SIZE);


    ret = ApiCmdReplayStatus( ulModHandle, 0, &xReplayStatus );

    AIM_ERROR_MESSAGE_RETURN( ret );


    ulBytes = (AiUInt32)fread( aucReplayBuffer, 1, GLOBAL_REPLAY_BUFFER_SIZE, pFile );

    if( ulBytes > 0 )
    {
        /* Got data from file */
        xReplayStatus.size = ulBytes;

        ret = ApiWriteRepData( ulModHandle, 0, &xReplayStatus, aucReplayBuffer, &ulBytes );

        AIM_ERROR_MESSAGE_RETURN( ret );

    }

    AiOsFree(aucReplayBuffer);

    return ulBytes;
}

AiReturn Replaying(AiUInt32 ulModHandle)
{
    AiReturn ret        = 0;
    AiUInt32 ulFileSize     = 0;
    AiUInt32 ulBytesWritten = 0;
    AiUInt32 ulLastRpiCount = 0;
    AiUInt32 ulRpiDelta     = 0;
    FILE   * pFile          = 0;

    TY_API_REP_STATUS xReplayStatus;

    memset( &xReplayStatus,  0, sizeof(&xReplayStatus)  );

    printf("Replaying ... \n");


    /************************************************/
    /*                                              */
    /* --          Get input file size          --- */
    /*                                              */
    /************************************************/

    ret = AiOsGetFileSize( szGlobalFileName, &ulFileSize );

    if( ret )
    {
        printf("Error : could not get file size of %s\n", szGlobalFileName );
        return 1;
    }

    /************************************************/
    /*                                              */
    /* --           Open input file             --- */
    /*                                              */
    /************************************************/
    pFile = fopen( szGlobalFileName, "rb");

    if( pFile == NULL )
    {
        printf("Error : could not open file %s\n", szGlobalFileName );
        return 2;
    }


    /************************************************/
    /*                                              */
    /* --      Initialize replay                --- */
    /*                                              */
    /************************************************/


    /* Before the replay start command can be called we need to initialize both
       half replay buffers with data. This is required because the replay will 
       be started at once after the command is issued. 
    */

    ret = ApiCmdReplayIni( ulModHandle, 0, 1, 0, 0, 0, 0, 0, 1, 0L, 0L, 1, ulFileSize );

    AIM_ERROR_MESSAGE_RETURN( ret );


    /* Load first half replay buffer */
    ulBytesWritten = WriteReplayBufferFromFile( ulModHandle, pFile );

    if( ulBytesWritten == 0 )
    {
        printf("Error : Not enough data in file %s for first half buffer.\n", szGlobalFileName );
        return 3;
    }

    /* Load second half replay buffer */
    ulBytesWritten = WriteReplayBufferFromFile( ulModHandle, pFile );

    if( ulBytesWritten == 0 )
    {
        printf("Error : Not enough data in file %s for second half buffer.\n", szGlobalFileName );
        return 4;
    }



    /* Both replay buffers are now filled with data. With the following command we start the
       onboard replay. Every time on half buffer is replayed the onboard software emits a 
       halfbuffer full interrupt (HFI). We use the status read command to poll the HFI count.
       If the HFI count is one higher than it was the last time then we now that exactly
       one half buffer was replayed. So we reload one half buffer. If the new HFI count
       increased by two since last time then our reload mechanism was to slow and we have a 
       buffer overrun. This means that one of the halfbuffers was replayed twice. */

    ret = ApiCmdReplayStart( ulModHandle, 0 );

    if(ret != API_OK)
    {
        fclose(pFile);	
        AIM_ERROR_MESSAGE_RETURN( ret );
    }

    /************************************************/
    /*                                              */
    /* --          Write replay data            --- */
    /*                                              */
    /************************************************/
    while( 1 )
    {
        /* Write replay data */
        ret = ApiCmdReplayStatus( ulModHandle, 0, &xReplayStatus );

        if (ret != API_OK)
        {
            AIM_ERROR_MESSAGE(ret);
            break;
        }


        if( xReplayStatus.entry_cnt == 0 )
        {
            printf("Replaying ...  %7d entries\n", xReplayStatus.entry_cnt);
            break;
        }

        /* check for replay overflow */
        ulRpiDelta = xReplayStatus.rpi_cnt - ulLastRpiCount;

        if( ulRpiDelta > 1 )
        {
            printf("Error : Replay overflow\n");
            break;
        }

        if( ulRpiDelta == 1 )
        {
            printf("Replaying ...  %7d entries\n", xReplayStatus.entry_cnt);
            /* one buffer was completely replayed
               reload data from file */
            WriteReplayBufferFromFile( ulModHandle, pFile );
            ulLastRpiCount++;
        }

        AiOsSleep(500);
    }


    /************************************************/
    /*                                              */
    /* --          Stop replay data             --- */
    /*                                              */
    /************************************************/
    ret = ApiCmdReplayStop( ulModHandle, 0 );

    AIM_ERROR_MESSAGE( ret );

    /************************************************/
    /*                                              */
    /* --           Close input file            --- */
    /*                                              */
    /************************************************/
    fclose(pFile);

    return ret;
}




AiReturn StopRtSimulation(AiUInt32 ulModHandle)
{
    AiReturn ret = 0;

    printf("StopRtSimulation ... ");

    /* Stop all RTs */
    ApiCmdRTHalt( ulModHandle, 0 );
    /* Stop Bus Monitor */
    ApiCmdBMHalt(ulModHandle, 0);

    printf("done\n");

    return 0;
}



AiReturn CheckReceivedData(AiUInt32 ulModHandle)
{
    AiReturn             ret  = 0;
    TY_API_RT_STATUS_DSP xRtStatus;

    memset( &xRtStatus, 0, sizeof(xRtStatus) );

    ret = ApiCmdRTStatusRead( ulModHandle, 0, &xRtStatus );

    AIM_ERROR_MESSAGE_RETURN( ret );


    if(    (xRtStatus.glb_msg_cnt != 5000)
        || (xRtStatus.glb_err_cnt != 0   ) )
    {
        printf("Error : Expected RT to receive %5d transfer got %5d\n", 5000, xRtStatus.glb_msg_cnt );
        printf("Error : Expected RT to receive %5d errors   got %5d\n",    0, xRtStatus.glb_err_cnt );

        return 1;
    }

    return 0;
}
