/* SPDX-FileCopyrightText: 2010-2024 AIM GmbH <info@aim-online.com> */
/* SPDX-License-Identifier: MIT */

/*
This sample shows how to implement the AyE milscope with external bus coupling.
This sample configures the AyE Milscope and sends a single transfer.
The Milscope data is received and stored into a CSV file.
The sample needs a bus with transformer termination and no external bus traffic.
*/

#include "1553_sample.h"

static AiReturn PrepareSimulation(AiUInt32 ulModHandle);
static AiReturn RunSimulation(AiUInt32 ulModHandle);
static AiReturn StopSimulation(AiUInt32 ulModHandle);

static AiReturn PrepareMilscope(AiUInt32 ulModHandle, TY_API_SCOPE_BUFFER ** pxScopeBufferList);
static AiReturn StartMilscope(AiUInt32 ulModHandle, TY_API_SCOPE_BUFFER ** pxScopeBufferList);
static AiReturn WaitForMilscopeFinished(AiUInt32 ulModHandle, AiUInt8 * pucFinish);
static AiReturn StopMilscope(AiUInt32 ulModHandle);
static AiReturn FinalizeMilscope(AiUInt32 ulModHandle, TY_API_SCOPE_BUFFER ** pxScopeBufferList);

static void ReadData(         AiUInt32 ulModHandle, TY_API_SCOPE_BUFFER* pBuffer );



#define SCOPE_BUFFER_COUNT 4
#define SCOPE_RETRIGGER    1


const char * szCsvFileNameAPE = "milscope_data_ape.csv";
char szCsvPathAPE[AI_OS_MAX_PATH];
FILE * fp = NULL;

/* ######################################################################################*/
/*                                                                                       */
/*                                     Sample entry point                                */
/*                                                                                       */
/* ######################################################################################*/
AiReturn LsMilscopeSampleAPE(AiUInt32 ulModHandle)
{
    AiUInt32 ulLoop = 0;
    AiUInt8  ucFinish = 0;
    AiReturn ret = 0;
    AiUInt32 ulMilscopeType = 0;
    AiUInt32 ulOutCount = 0;

    TY_API_SCOPE_BUFFER ** pxScopeBufferList = NULL;

    ret = ApiCmdSysGetBoardInfo(ulModHandle, TY_BOARD_INFO_MILSCOPE_TYPE, 1, &ulMilscopeType, &ulOutCount);

    AIM_ERROR_MESSAGE_RETURN(ret);

    if (ulMilscopeType != API_MILSCOPE_TYPE_AYE)
    {
        printf("No AyE Milscope found.\n");
        return 0;
    }

    pxScopeBufferList = AiOsMalloc(sizeof(TY_API_SCOPE_BUFFER*) * SCOPE_BUFFER_COUNT);
    memset(pxScopeBufferList, 0, sizeof(TY_API_SCOPE_BUFFER*) * SCOPE_BUFFER_COUNT);

    printf("Running LsMilscopeSampleAPE \n");

    /* Setup RTs and BC */
    ret = PrepareSimulation(ulModHandle);

    AIM_TRACE_RETURN(ret);

    ret = PrepareMilscope(ulModHandle, pxScopeBufferList);

    AIM_TRACE_RETURN(ret);


    AiOsGetTempPath(szCsvPathAPE, sizeof(szCsvPathAPE));
    
    if (strlen(szCsvPathAPE) > (AI_OS_MAX_PATH - strlen(szCsvPathAPE)))
    {
        printf("Error : Temp path too long\n");
        return 1;
    }

    strcpy(&szCsvPathAPE[strlen(szCsvPathAPE)], szCsvFileNameAPE);
    printf("Path to csv file:  %s\n", szCsvPathAPE);

    fp = fopen(szCsvPathAPE, "w");

    if (fp == NULL)
    {
        printf("Failed to open file %s\n", szCsvPathAPE);
        return API_ERR;
    }


    // In order to implement a retrigger mode the following for loop
    // can be changed. While in the loop the Milscope will 
    // continously capture data.
    for (ulLoop = 0; ulLoop < 1; ulLoop++)
    {
        ret = StartMilscope(ulModHandle, pxScopeBufferList);

        AIM_TRACE_BREAK(ret);

        /* Start the simulation  */
        ret = RunSimulation(ulModHandle);

        AIM_TRACE_BREAK(ret);

        /* Wait for MilScope recording to be finished */
        ret = WaitForMilscopeFinished(ulModHandle, &ucFinish);

        AIM_TRACE_BREAK(ret);

        /* No need to read the data */
        /* The library will call our buffer callback function instead */

        /* Stop MilScope */
        ret = StopMilscope(ulModHandle);

        AIM_TRACE_BREAK(ret);

        /* Stop the simulation */
        ret = StopSimulation(ulModHandle);

        AIM_TRACE_BREAK(ret);
    }

    fclose(fp);
    FinalizeMilscope(ulModHandle, pxScopeBufferList);
    AiOsFree(pxScopeBufferList);


    return 0;
}




AiReturn PrepareSimulation(AiUInt32 ulModHandle)
{
    AiUInt8 i           = 0;
    AiReturn ret     = 0;

    AiUInt16 uwHeaderId        = 0;
    AiUInt16 auwTransferIds[1] ={ 1 };

    AiUInt16 auwMessageData[32];
    memset( &auwMessageData,  0, sizeof(auwMessageData)  );

    printf("PrepareSimulation ... \n");

    /*********************************************/
    /*                                           */
    /* --      Simulate RT3 receive SA13     --- */
    /*                                           */
    /*********************************************/
    ret = MilUtilEnableRT(ulModHandle, 3 /*RT*/, 8.0);

    AIM_TRACE_RETURN(ret);

    ret = MilUtilEnableRTSA(ulModHandle, 3 /*RT*/, 13 /*SA*/, API_RT_TYPE_RECEIVE_SA, MilUtilNextRTHid());

    AIM_TRACE_RETURN(ret);

    /*********************************************/
    /*                                           */
    /* --  Initialize the Bus Controller     --- */
    /*                                           */
    /*********************************************/
    ret = ApiCmdBCIni( ulModHandle, 0, 0, 0, 0, 0 );
    AIM_ERROR_MESSAGE_RETURN(ret);


    printf("\n");
    printf(" %s\n %s\n %s\n %s\n %s\n\n", 
                         "Framing :        1 times ",
                         "------------------------ ",
                         "BC         -> RT3RcvSA13 ",
                         "------------------------ ",
                         "       total 1 transfers " );
    printf("\n");




    /*********************************************/
    /*                                           */
    /* --           BC -> RT3 SA13           --- */
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

    /* Data to send to the RT */
    for (i = 0; i < 32; i++)
        /* auwMessageData[] = { 0xBC01, 0xBC02, 0xBC03 ... 0xBC32 } */
        auwMessageData[ i ] = 0xBC00 + i + 1;

    ret = MilUtilSetData(ulModHandle, uwHeaderId, API_BUF_BC_MSG, 1 /*First buffer in queue*/, 32, auwMessageData, NULL);

    AIM_TRACE_RETURN(ret);

    /*********************************************/
    /*                     BC                    */
    /* --      Setup Major/Minor Framing  ---    */
    /*                                           */
    /*********************************************/
    ret = MilUtilBCCreateSingleFrameFraming(ulModHandle, auwTransferIds, 1);
    AIM_TRACE_RETURN(ret);

    printf("PrepareSimulation ... done\n");

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
    AIM_ERROR_MESSAGE_RETURN(ret);

    /*********************************************/
    /*                                           */
    /* --         Start Bus Controller       --- */
    /*                                           */
    /*********************************************/

    /* ApiCmdBCStart(   module, 0, mode,                     count, mframe time, 0, major_frame_out,  minor_frame_out ); */
    ret = ApiCmdBCStart( ulModHandle, 0, API_BC_START_IMMEDIATELY,     1, 10,          0, &ulMajorAddr,     aulMinorAddr );
    AIM_ERROR_MESSAGE_RETURN(ret);

    printf("running\n");
    return 0;
}

AiReturn StopSimulation(AiUInt32 ulModHandle)
{
    AiReturn ret = 0;

    printf("StopSimulation ... ");

    /* Stop Bus Controller */
    ret = ApiCmdBCHalt(    ulModHandle, 0 );
    AIM_ERROR_MESSAGE_RETURN(ret);

    /* Stop all RTs */
    ret = ApiCmdRTHalt(    ulModHandle, 0 );
    AIM_ERROR_MESSAGE_RETURN(ret);

    printf("done\n");
    return 0;
}


AiReturn PrepareMilscope(AiUInt32 ulModHandle, TY_API_SCOPE_BUFFER ** pxScopeBufferList)
{
    AiReturn ret = 0;


    TY_API_SCOPE_SETUP  xScopeSetup;
    TY_API_SCOPE_TRG_EX xTrigger;

    memset( &xScopeSetup, 0, sizeof(xScopeSetup) );
    memset( &xTrigger,    0, sizeof(xTrigger)    );


    /* Test if stopping is possible before setup without crash */
    ret = ApiCmdScopeReset(ulModHandle);

    AIM_ERROR_MESSAGE_RETURN( ret );

    /* 
    ret = ApiCmdScopeCalibrate(module_handle, 1, &xScopeCalibration);

    AIM_ERROR_MESSAGE_RETURN( ret );

    printf("Offset A 30V: %d\n", xScopeCalibration.lOffsetA);
    printf("Offset B 30V: %d\n", xScopeCalibration.lOffsetB);
    printf("Offset A 3V: %d\n", xScopeCalibration.lOffsetA100);
    printf("Offset B 3V: %d\n\n", xScopeCalibration.lOffsetB100);
    */



    xScopeSetup.ul_OperationMode = API_SCOPE_OPR_CHANNEL_A;
    xScopeSetup.ul_CouplingPri   = API_SCOPE_COUPLING_STUB;
    xScopeSetup.ul_CaptureMode   = API_SCOPE_START_SINGLE;
    xScopeSetup.ul_SamplingRate  = API_SCOPE_RATE_EVERY;

    ret = ApiCmdScopeSetup( ulModHandle, &xScopeSetup );
    AIM_ERROR_MESSAGE_RETURN( ret );

    AiOsSleep(1000);

    xTrigger.ul_TrgMode  = API_SCOPE_MODE_GREATER_THAN;
    xTrigger.ul_TrgSrc   = API_SCOPE_SRC_PRIMARY;
    xTrigger.ul_TrgValue = 600;
    xTrigger.ul_TrgFlags = 0;

    ret = ApiCmdScopeTriggerDefEx( ulModHandle, &xTrigger);

    AIM_ERROR_MESSAGE_RETURN( ret );


    ret = ApiCreateScopeBufferList( ulModHandle, SCOPE_BUFFER_COUNT, pxScopeBufferList, ReadData, NULL );

    AIM_ERROR_MESSAGE_RETURN( ret );


    return 0;
}

AiReturn StartMilscope(AiUInt32 ulModHandle, TY_API_SCOPE_BUFFER ** pxScopeBufferList)
{
    AiReturn ret = 0;
    AiUInt32 BufferCount = 0;
    TY_API_SCOPE_STATUS     Status;


    ret = ApiProvideScopeBuffers(ulModHandle, SCOPE_BUFFER_COUNT, pxScopeBufferList );

    AIM_ERROR_MESSAGE_RETURN( ret );

    ret = ApiCmdScopeStatus( ulModHandle, &Status, &BufferCount );

    AIM_ERROR_MESSAGE_RETURN( ret );

    printf("Prepared scope with %d buffers\n", BufferCount );

    ret = ApiCmdScopeStart(ulModHandle);

    AIM_ERROR_MESSAGE_RETURN( ret );

    return 0;
}

AiReturn WaitForMilscopeFinished(AiUInt32 ulModHandle, AiUInt8 * pucFinish)
{
    AiReturn ret     = 0;
    AiUInt32 BufferCount = 0;

    TY_API_SCOPE_STATUS     Status;
    TY_API_SCOPE_WAIT_STATE WaitResultFlags;


    memset( &Status,          0, sizeof(Status)          );
    memset( &WaitResultFlags, 0, sizeof(WaitResultFlags) );

    *pucFinish = FALSE;





    while( !*pucFinish )
    {
        WaitResultFlags = 0;

        ret = ApiWaitForScopeFinished( ulModHandle, 10000, &WaitResultFlags );

        AIM_ERROR_MESSAGE_RETURN( ret );

        if( WaitResultFlags & SCOPE_WAIT_OVERFLOW )
        {
            printf("Overflow in scope\n");

            ret = ApiCmdScopeStop(ulModHandle);
    
            AIM_ERROR_MESSAGE( ret );
            WaitForUser();
            return API_ERR;
        }
        else if(WaitResultFlags & SCOPE_WAIT_TIMEOUT)
        {
            ret = ApiCmdScopeStatus( ulModHandle, &Status, &BufferCount );

            AIM_ERROR_MESSAGE_RETURN( ret );

            if( Status != SCOPE_STATUS_WAIT_FOR_TRIGGER )
            {
                printf("Time out before all buffers were processed!\n");
                return API_ERR;
            }
            else
            {
                printf("Time out but continuing to wait for trigger with %d buffers\n", BufferCount );
            }
        }
        else
        {

            ret = ApiCmdScopeStatus( ulModHandle, &Status, &BufferCount );

            AIM_ERROR_MESSAGE_RETURN( ret );

            printf("Scope finished buffer count %d\n", BufferCount );

            /* Scope finished, do next run */
            *pucFinish = TRUE;
        }
    }

    return API_OK;
}



static void ReadData( AiUInt32 ulModHandle, TY_API_SCOPE_BUFFER* pBuffer )
{
    unsigned int  i        = 0;
    int           value    = 0;
    
    printf("ReadData id %d\n", pBuffer->ulID );

    if(pBuffer->ulFlags & SCOPE_BUFFER_FLAG_CANCELED)
    {
        return;
    }




    if(pBuffer->ulFlags & SCOPE_BUFFER_FLAG_TRIGGER)
    {
        printf( "Trigger buffer with data size %d and trigger ID %d complete\n", pBuffer->ulDataSize, pBuffer->ulTriggerId );
    }

    for(i = 0; i < NUM_SCOPE_SAMPLES(pBuffer->ulDataSize); i++)
    {
        value = SCOPE_SAMPLE_BY_SAMPLE_ID(pBuffer->pvBuffer,i);

        if(pBuffer->eBufferType == SCOPE_BUFFER_PRIMARY)
        {
            /* channel A data */
            fprintf(fp, "%d,\n", value);
        }
        else
        {
            /* channel B data */
        }
    }

}


AiReturn StopMilscope(AiUInt32 ulModHandle)
{
    AiReturn ret = 0;

    ret = ApiCmdScopeStop(ulModHandle);

    AIM_ERROR_MESSAGE_RETURN( ret );


    return ret;
}

AiReturn FinalizeMilscope(AiUInt32 ulModHandle, TY_API_SCOPE_BUFFER ** pxScopeBufferList)
{
    int i;

    for( i=0; i<SCOPE_BUFFER_COUNT; i++ )
        ApiFreeScopeBuffer( ulModHandle, pxScopeBufferList[i] );

    return 0;
}
