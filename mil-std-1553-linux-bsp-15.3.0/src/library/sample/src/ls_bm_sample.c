/* SPDX-FileCopyrightText: 2010-2024 AIM GmbH <info@aim-online.com> */
/* SPDX-License-Identifier: MIT */

/*
This sample shows how to monitor a 1553 bus with external bus coupling.
This sample monitors and displays all traffic on the external bus.
The sample needs a bus with transformer termination and external bus traffic.
*/

#include "1553_sample.h"

static AiReturn Recording(AiUInt32 ulModHandle, AiUInt32 ulStream, AiUInt8 ucUseAns);
static AiReturn StartRecording(AiUInt32 ulModHandle, AiUInt8 ucQueueId, AiUInt8 ucUseAns);
static AiReturn StopRecording(AiUInt32 ulModHandle, AiUInt8 ucQueueId);
static AiReturn CloseRecording(AiUInt32 ulModHandle, AiUInt8 ucQueueId);
static AiReturn IsQueueDataAvailable(AiUInt32 ulModHandle, AiUInt8 ucQueueId, AiUInt32 * pulDataAvailable);
static AiReturn ReadData(AiUInt32 ulModHandle, AiUInt8 ucQueueId, FILE * pFile, AiUInt32 ulDataAvailable);


/* Global constants */
static char szGlobalFileName[AI_OS_MAX_PATH];


#define GLOBAL_RECORDING_BUFFER_SIZE (128*1024) /* Read  blocks of 128 KB */

static AiUInt32 ulInterpreted;

/* ######################################################################################*/
/*                                                                                       */
/*                                     Sample entry point                                */
/*                                                                                       */
/* ######################################################################################*/
AiReturn LsBmSample(AiUInt32 ulModHandle, AiUInt32 ulStream, AiUInt8 ucUseAns)
{
    AiReturn ret = 0;
    char                szDummy[32];
    AiUInt32            ulValue = 0;
    TY_API_RESET_INFO    xApiResetInfo;
    
    memset( &xApiResetInfo, 0, sizeof(xApiResetInfo) );

    printf("Running LsBmSample\n");

    /* Interpreted mode or raw mode */
    /* Check for options                  */
    printf("Choose the mode\n");
    printf("0 : Interpreted mode: interprets an message and return the data in a structure\n");
    printf("1 : Raw mode: display every BM Entry on the screen\n");

    printf("choice [0] ? ");
    AiOsGetString(szDummy, sizeof(szDummy), "0");
    sscanf(szDummy, "%d", &ulValue);

    switch (ulValue)
    {
        case 0:  ulInterpreted = 1; break;
        case 1:  ulInterpreted = 0; break;
        default: ulInterpreted = 1; break;
    }

    /* Create tmp file name with path */

    AiOsGetTempPath( szGlobalFileName, AI_OS_MAX_PATH );
    
    if( strlen(szGlobalFileName) > (AI_OS_MAX_PATH - strlen("ls_bm_sample.prc") ) )
    {
        printf("Error : Temp path too long\n" );
        return 1;
    }

    strcpy( &szGlobalFileName[strlen(szGlobalFileName)], "ls_bm_sample.prc" );

    /* Recording sample */
   ret = Recording( ulModHandle, ulStream, ucUseAns );

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
AiUInt8 ucStreamToDataQueueIdMax = 8;
AiUInt8 aucStreamToDataQueueId[] = { 0 /* invalid stream number */, 
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
    AiUInt32 timeout_counter      = 0;

    TY_API_DATA_QUEUE_READ xDataQueue;



    memset( &xDataQueue,         0, sizeof(xDataQueue)         );

    ulBiu = MilUtilBMGetLogicalBiuFromStream(ulModHandle, ulStream, 0);

    if (ulBiu > ucStreamToDataQueueIdMax)
    {
        printf("Invalid biu %d\n", ulBiu);
        return 1;
    }

    /* convert stream to data queue id */
    ucQueueId = aucStreamToDataQueueId[ulBiu];


    /* Start the bus monitor */
    ret = StartRecording( ulModHandle, ucQueueId, ucUseAns );

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

    /******************************************************/
    /*                                                    */
    /* --  Read BM data as long a Key has been pressed--- */
    /*                                                    */
    /******************************************************/
    printf("\nPress any key to stop recording\n");

    while( !AiOsKbhit() )
    {

        ret = IsQueueDataAvailable( ulModHandle, ucQueueId, &ulDataAvailable );

        AIM_TRACE_RETURN( ret );

        if (timeout_counter < 10)
        {
            if (ulDataAvailable < GLOBAL_RECORDING_BUFFER_SIZE)
            {
                /* We want to read blocks of 128 KB. */
                /* no enough data available       */
                /* wait some time and check again */
                AiOsSleep(50);
                timeout_counter++;
                continue;
            }
            else
            {
                timeout_counter = 0;
            }
        }

        if (ulDataAvailable > 0 )
        {
            /* -- read available data --- */
            ret = ReadData(ulModHandle, ucQueueId, pWriteFile, ulDataAvailable);

            AIM_TRACE_RETURN(ret);
        }

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
        ret = IsQueueDataAvailable( ulModHandle, ucQueueId, &ulDataAvailable);

        AIM_TRACE_RETURN( ret );

        if( ulDataAvailable == 0 )
        {
            /* The simulation and the monitor are stopped */
            /* If the queue is empty we should be done    */
            break;
        }

        ret = ReadData( ulModHandle, ucQueueId, pWriteFile, ulDataAvailable);

        AIM_TRACE_RETURN( ret );

    }
    while(1);



    /************************************************/
    /*                                              */
    /* --               Clean up                --- */
    /*                                              */
    /************************************************/
    CloseRecording( ulModHandle, ucQueueId );

    fclose(pWriteFile);

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


AiReturn ReadData(AiUInt32 ulModHandle, AiUInt8 ucQueueId, FILE * pFile, AiUInt32 ulDataAvailable)
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

    if (ulInterpreted)
    {
        do
        {
            ret = MilUtilBMLsInterpretData(ulModHandle, &xBmIterator, &xBmInterpretedMessage);

            if (ret == API_OK)
                MilUtilBMLsQueueBufShow(ulModHandle, &xBmInterpretedMessage);
        } while (ret == API_OK);

        if (ret != API_ERR_QUEUE_EMPTY)
            AIM_ERROR_MESSAGE(ret);
    }
    else
    {
        ret = MilUtilBMLsShowData(ulModHandle, &xBmIterator);
        if (ret != API_ERR_QUEUE_EMPTY)
            AIM_ERROR_MESSAGE(ret);
    }

    AiOsFree( aucRecordingBuffer );

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






