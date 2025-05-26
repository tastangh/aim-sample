/* SPDX-FileCopyrightText: 2010-2022 AIM GmbH <info@aim-online.com> */
/* SPDX-License-Identifier: MIT */

/*
This sample shows how to implement a BC instruction list.
This sample creates an instruction list with two frames.
The first frame contains a Strobe instruction and two BC2RT transfers.
The second frame contains only two BC2RT transfers.
The BC is started in instruction list mode with the RT enabled and
the instruction list is send with a total of 4000 transfers.
The pass criteria is that all 4000 transfer are send without an error.
This sample is executed with internal coupling.No external connection is required.
*/


#include "1553_sample.h"

static AiReturn PrepareRemoteTerminals(AiUInt32 ulModHandle);
static AiReturn PrepareBusController(AiUInt32 ulModHandle);
static AiReturn RunSimulation(AiUInt32 ulModHandle);
static AiReturn WaitForEndOfSimulation(AiUInt32 ulModHandle);
static AiReturn StopSimulation(AiUInt32 ulModHandle);
static AiReturn PrintSimulationStatistic(AiUInt32 ulModHandle);


/* ######################################################################################*/
/*                                                                                       */
/*                                     Sample entry point                                */
/*                                                                                       */
/* ######################################################################################*/
AiReturn LsInstructionListSample(AiUInt32 ulModHandle)
{
    AiReturn ret;

    printf("Running LsInstructionListSample \n");

    ret = PrepareRemoteTerminals( ulModHandle );
    AIM_TRACE_RETURN(ret);

    ret = PrepareBusController( ulModHandle );
    AIM_TRACE_RETURN(ret);

    ret = RunSimulation( ulModHandle );
    AIM_TRACE_RETURN(ret);

    ret = WaitForEndOfSimulation( ulModHandle );
    AIM_TRACE_RETURN(ret);

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
    ret = MilUtilEnableRT(ulModHandle, 3 /*RT*/, 8.0 );

    AIM_TRACE_RETURN(ret);

    ret = MilUtilEnableRTSA(ulModHandle, 3 /*RT*/, 13 /*SA*/, API_RT_TYPE_RECEIVE_SA, MilUtilNextRTHid());

    AIM_TRACE_RETURN(ret);

    printf("done\n");
    return 0;
}


AiReturn PrepareBusController(AiUInt32 ulModHandle)
{
    AiUInt32 i          = 0;
    AiUInt16 uwHeaderId      = 0;

    AiUInt32 ulInstrNr  = 0;
    AiUInt32 ulMaxInstructions = 0;
    AiUInt32 ulErrorPos = 0;
    AiUInt8  ucStatus   = 0;
    AiReturn ret        = 0;

    AiUInt16 auwMessageData[32];
    AiUInt32 *aulFirmwareInstructions;

    TY_API_BC_FW_INSTR *axInstructionTable;
    TY_API_GET_MEM_INFO xMemInfo;


    memset(&auwMessageData, 0, sizeof(auwMessageData));
    memset(&xMemInfo, 0, sizeof(xMemInfo));

    /* Get maximum count of intructions for instruction list */

    ret = ApiCmdSysGetMemPartition(ulModHandle, 0, &xMemInfo);

    AIM_ERROR_MESSAGE_RETURN(ret);

    ulMaxInstructions = xMemInfo.ax_BiuSize->ul_BcHipInstr / 4;

    printf("Instruction list size %d with maximum %d instructions.\n", xMemInfo.ax_BiuSize->ul_BcHipInstr, ulMaxInstructions);

    aulFirmwareInstructions = (AiUInt32*)AiOsMalloc(sizeof(AiUInt32) * ulMaxInstructions);
    axInstructionTable = (TY_API_BC_FW_INSTR*)AiOsMalloc(sizeof(TY_API_BC_FW_INSTR) * ulMaxInstructions);


    do /*Try*/
    {

        /*********************************************/
        /*                                           */
        /* --  Initialize the Bus Controller     --- */
        /*                                           */
        /*********************************************/
        ret = ApiCmdBCIni( ulModHandle, 0, 0, 0, 0, 0 );

        AIM_ERROR_MESSAGE_BREAK(ret);


        printf("\n");
        printf(" %s\n %s\n %s\n %s\n %s\n %s\n %s\n %s\n %s\n %s\n %s\n", 
                                                 "Framing :         1000 times ",
                                                 "---------------------------- ",
                                                 "FRAME1:",
                                                 "    Strobe                   ",
                                                 "    BC         -> RT3RcvSA13 ",
                                                 "    BC         -> RT3RcvSA13 ",
                                                 "FRAME2:",
                                                 "    BC         -> RT3RcvSA13 ",
                                                 "    BC         -> RT3RcvSA13 ",
                                                 "---------------------------- ",
                                                 "        total 4000 transfers " );
        printf("\n");

        printf("PrepareBusController ... ");


        /*********************************************/
        /*                                           */
        /* --            BC -> RT3RcvSA13        --- */
        /*                                           */
        /*********************************************/
        uwHeaderId = MilUtilNextBCHid();

        ret = MilUtilBCCreateTransfer(ulModHandle,
            1 /*Transfer Id*/,
            API_BC_TYPE_BCRT,
            0,  /* Transmit RT */
            0,  /* Transmit SA */
            3,  /* Receive  RT */
            13, /* Receive  SA */
            0,  /* Word count 32 */
            uwHeaderId,
            API_QUEUE_SIZE_1,
            API_BC_TIC_NO_INT);

        AIM_TRACE_BREAK(ret);

        /* Data to send to the RT */
        for (i = 0; i < 32; i++)
            /* auwMessageData[] = { 0xBC01, 0xBC02, 0xBC03 ... 0xBC32 } */
            auwMessageData[ i ] = (AiUInt16)(0xBC00 + i + 1);

        ret = MilUtilSetData(ulModHandle, uwHeaderId, API_BUF_BC_MSG, 1 /*First buffer in queue*/, 32, auwMessageData, NULL);

        AIM_TRACE_BREAK(ret);

        /*********************************************/
        /*                                           */
        /* --            Instruction Table       --- */
        /*                                           */
        /*********************************************/

        /* Get BC resources */
        ret = ApiCmdBCInstrTblIni( ulModHandle, 0 );

        AIM_ERROR_MESSAGE_BREAK(ret);

        /* Clear BC Instruction Table structure */
        ret = ApiCmdBCInstrTblGen( ulModHandle, 0, 0 /*clear*/, ulMaxInstructions, 0L /*dest_cnt*/, 0L /*dest_offset*/, axInstructionTable, aulFirmwareInstructions, &ulErrorPos, &ucStatus );

        AIM_ERROR_MESSAGE_BREAK(ret);

    
        ulInstrNr = 0;
        axInstructionTable[ulInstrNr].op    = API_BC_FWI_JUMP;    /* GOTO Start of Major Frame */
        axInstructionTable[ulInstrNr].par1  = 0x1111;             /* Major frame start label   */

        /* FRAME 1 */
        ulInstrNr++;
        axInstructionTable[ulInstrNr].label = 0x0001;          /* Start label of Frame 1    */
        axInstructionTable[ulInstrNr].op    = API_BC_FWI_STRB; /* Send strobe output       */
        ulInstrNr++;
        axInstructionTable[ulInstrNr].op    = API_BC_FWI_XFER; /* Send transfer             */
        axInstructionTable[ulInstrNr].par1  = 1;               /* transfer id               */
        ulInstrNr++;
        axInstructionTable[ulInstrNr].op    = API_BC_FWI_XFER; /* Send transfer             */
        axInstructionTable[ulInstrNr].par1  = 1;               /* transfer id               */
        ulInstrNr++;
        axInstructionTable[ulInstrNr].op    = API_BC_FWI_WMFT; /* Wait for minor frame time */
        ulInstrNr++;
        axInstructionTable[ulInstrNr].op    = API_BC_FWI_RET;  /* Return from minor frame 1 */

        /* FRAME 2 */
        ulInstrNr++;
        axInstructionTable[ulInstrNr].label = 0x0002;          /* Start label of Frame 2    */
        axInstructionTable[ulInstrNr].op    = API_BC_FWI_XFER; /* Send transfer             */
        axInstructionTable[ulInstrNr].par1  = 1;               /* transfer id               */
        ulInstrNr++;
        axInstructionTable[ulInstrNr].op    = API_BC_FWI_XFER; /* Send transfer             */
        axInstructionTable[ulInstrNr].par1  = 1;               /* transfer id               */
        ulInstrNr++;
        axInstructionTable[ulInstrNr].op    = API_BC_FWI_WMFT; /* Wait for minor frame time */
        ulInstrNr++;
        axInstructionTable[ulInstrNr].op    = API_BC_FWI_RET;  /* Return from minor frame 2 */


        /* Start of Major Frame / Call minor frames */
        ulInstrNr++;
        axInstructionTable[ulInstrNr].label = 0x1111;          /* Start of Major Frame Label*/
        axInstructionTable[ulInstrNr].op    = API_BC_FWI_CALL; /* Send frame */
        axInstructionTable[ulInstrNr].par1  = 0x0001;          /* frame 1 label             */
        ulInstrNr++;
        axInstructionTable[ulInstrNr].op    = API_BC_FWI_CALL; /* Send frame */
        axInstructionTable[ulInstrNr].par1  = 0x0002;          /* frame 2 label             */
        ulInstrNr++;
        axInstructionTable[ulInstrNr].op    = API_BC_FWI_DJZ;  /* Decrement major frame count and jump if zero */
        axInstructionTable[ulInstrNr].par1  = 0xFFFF;          /* Destination label                            */
        ulInstrNr++;
        axInstructionTable[ulInstrNr].op    = API_BC_FWI_JUMP;  /* Jump to start label                          */
        axInstructionTable[ulInstrNr].par1  = 0x1111;          /* Destination label                            */

        /* End of framing */
        ulInstrNr++;
        axInstructionTable[ulInstrNr].label = 0xFFFF;          /* End label   */
        axInstructionTable[ulInstrNr].op    = API_BC_FWI_HALT; /* Do nothing  */




        /* -- Convert and install instruction table --- */
        /* mode 3 => convert and write table */

        ret = ApiCmdBCInstrTblGen( ulModHandle, 0, 3, ulMaxInstructions, ulInstrNr, 0, 
                                                      axInstructionTable, 
                                                      aulFirmwareInstructions, 
                                                      &ulErrorPos, &ucStatus );

        AIM_ERROR_MESSAGE_BREAK(ret);

    }while(0); /*Catch*/

    AiOsFree(aulFirmwareInstructions);
    AiOsFree(axInstructionTable);

    printf("done\n");
    return ret;
}



AiReturn RunSimulation(AiUInt32 ulModHandle)
{
    AiUInt32 ulMajorAddr     = 0;
    AiUInt32 aulMinorAddr[64];
    AiReturn ret;

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

    /*  BC start modes :
        7  API_BC_START_INSTR_TABLE            Start with Instruction Table
    */

    /* ApiCmdBCStart(   module, 0, mode,                     count, mframe time, 0, major_frame_out,  minor_frame_out ); */
    ret = ApiCmdBCStart( ulModHandle, 0, API_BC_START_INSTR_TABLE, 1000,  10,          0, &ulMajorAddr,     aulMinorAddr );
    AIM_ERROR_MESSAGE_RETURN(ret);

    printf("running\n");
    return 0;
}


AiReturn WaitForEndOfSimulation(AiUInt32 ulModHandle)
{
    AiUInt32 transfers_left = 0;

    /* The ApiCmdBCStatusRead status bit will always be busy in case of an instruction list. 
       This is required to be able to send acyclic frames even if the instruction
       list has reached its end. To check if we reached the end of the simulation
       we check the number of transfers sent. */

    while( 1 )
    {
        transfers_left = MilUtilIsBcActiveCheckCount( ulModHandle, 1000 * 4 );

        printf("BC transfers to send %d\n", transfers_left );

        if( transfers_left == 0 )
            break;

        AiOsSleep(1000);
    }

    printf("\n");

    return 0;
}


AiReturn StopSimulation(AiUInt32 ulModHandle)
{
    printf("StopSimulation ... ");

    /* Stop Bus Controller */
    ApiCmdBCHalt(    ulModHandle, 0 );
    /* Stop all RTs */
    ApiCmdRTHalt(    ulModHandle, 0 );

    printf("done\n");
    return 0;
}



AiReturn PrintSimulationStatistic(AiUInt32 ulModHandle)
{
    AiReturn ret = 0;
    TY_API_BC_STATUS_DSP xBcStatus;
    TY_API_RT_STATUS_DSP xRtStatus;

    memset( &xBcStatus, 0, sizeof(xBcStatus) );
    memset( &xRtStatus, 0, sizeof(xRtStatus) );

    printf("PrintSimulationStatistic ...\n");

    ret = ApiCmdBCStatusRead( ulModHandle, 0, &xBcStatus );
    AIM_ERROR_MESSAGE_RETURN(ret);

    printf("BC  ( %d messages / %d errors )\n", xBcStatus.glb_msg_cnt, xBcStatus.glb_err_cnt );

    ret = ApiCmdRTStatusRead( ulModHandle, 0, &xRtStatus );
    AIM_ERROR_MESSAGE_RETURN(ret);

    printf("RTs ( %d messages  / %d errors )\n", xRtStatus.glb_msg_cnt, xRtStatus.glb_err_cnt);

    printf("Please note that one RT to RT transfer counts as two messages in RTs\n");
    printf("PrintSimulationStatistic ... done\n");
    return 0;
}



