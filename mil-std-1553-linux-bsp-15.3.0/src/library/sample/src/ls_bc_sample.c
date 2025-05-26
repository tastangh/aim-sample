/* SPDX-FileCopyrightText: 2023-2023 AIM GmbH <info@aim-online.com> */
/* SPDX-License-Identifier: MIT */

/*
This sample shows how to send transfers with external bus coupling.
This sample creates a framing with two different transfers(BC2RT, RT2BC).
The transfers are send to RT19 wrap around SA30.
The sample needs a bus with transformer termination and an external RT19.
This sample can be used in combination with the ls_ptp_vtp_sample
which implements a RT19 with external coupling.
*/

#include "1553_sample.h"


#define RT               19
#define SHARED_BUFFER_SA 30



static AiReturn PrepareBusController(AiUInt32 ulModHandle);
static AiReturn RunSimulation(AiUInt32 ulModHandle);
static AiReturn StopSimulation(AiUInt32 ulModHandle);
static AiReturn CheckTransfer(AiUInt32 ulModHandle, AiUInt32 ulMessageNr, const TY_API_QUEUE_BUF * pxMessage);
static AiReturn PrintSimulationStatistic(AiUInt32 ulModHandle);


/* ######################################################################################*/
/*                                                                                       */
/*                                     Sample entry point                                */
/*                                                                                       */
/* ######################################################################################*/
AiReturn LsBcSample(AiUInt32 ulModHandle)
{
    AiReturn ret = 0;
    AiUInt32 ulMajorAddr     = 0;
    AiUInt32 aulMinorAddr[64];
    TY_API_BC_STATUS_DSP xBcStatus;
    memset(&xBcStatus, 0, sizeof(xBcStatus));

    printf("Running LsBcSample \n");
    printf("%s\n%s\n%s\n%s\n",
           "Note: This sample implements only the BC part. ",
           "      The coupling is set to transformer.",
           "      To get successful transfers the LsPtpVtpSample, or any other RT19", 
           "      wrap around SA30 implementation, needs to be started on the connected stream.\n");

    /* Setup RTs and BC */

    ret = PrepareBusController(   ulModHandle );

    AIM_TRACE_RETURN( ret );

    /* Start BC */
    ret = ApiCmdBCStart(ulModHandle, 0, API_BC_START_IMMEDIATELY, 1000, 10, 0, &ulMajorAddr, aulMinorAddr);

    AIM_ERROR_MESSAGE_RETURN(ret);

    /* Print status while BC active */
    do
    {
        AiOsSleep(1000);

        ret = ApiCmdBCStatusRead(ulModHandle, 0, &xBcStatus);

        AIM_ERROR_MESSAGE_RETURN(ret);

        printf("BC  ( %d messages / %d errors )\n", xBcStatus.glb_msg_cnt, xBcStatus.glb_err_cnt);
    } while (MilUtilIsBcActive(ulModHandle) == 1);


    /* Stop Bus Controller */
    ret = ApiCmdBCHalt(ulModHandle, 0);

    AIM_ERROR_MESSAGE_RETURN(ret);

    return 0;
}




AiReturn PrepareBusController(AiUInt32 ulModHandle)
{
    AiUInt16 i      = 0;
    AiReturn ret    = 0;

    AiUInt16 auwTransferIds[2] = { 1, 2 };
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
    printf(" %s\n %s\n %s\n %s\n %s\n %s\n", 
                                             "Framing :      1000 times  ",
                                             "-------------------------- ",
                                             "BC          -> RT19RcvSA30 ",
                                             "RT19XmtSA30 -> BC          ",
                                             "-------------------------- ",
                                             "     total 2000 transfers " );
    printf("\n");

    printf("PrepareBusController ... ");


    /*********************************************/
    /*                                           */
    /* --            BC -> RT9RcvSA30        --- */
    /*                                           */
    /*********************************************/
    uwHeaderId   = MilUtilNextBCHid();


    ret = MilUtilBCCreateTransfer( ulModHandle, 
                                       auwTransferIds[0],
                                       API_BC_TYPE_BCRT, 
                                       0,   /* Transmit RT */ 
                                       0,   /* Transmit SA */
                                       RT,                /* Receive  RT */
                                       SHARED_BUFFER_SA,  /* Receive  SA */
                                       0,   /* Word count 32 */
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
    /* --         RT19XmtSA30 -> BC          --- */
    /*                                           */
    /*********************************************/
    uwHeaderId   = MilUtilNextBCHid();

    ret = MilUtilBCCreateTransfer( ulModHandle, 
                                       auwTransferIds[1],
                                       API_BC_TYPE_RTBC, 
                                       RT,               /* Transmit RT */ 
                                       SHARED_BUFFER_SA, /* Transmit SA */
                                       0,  /* Receive  RT */
                                       0,  /* Receive  SA */
                                       0,  /* Word count 32 */
                                       uwHeaderId, 
                                       API_QUEUE_SIZE_1,
                                       API_BC_TIC_NO_INT);


    AIM_TRACE_RETURN( ret );

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


