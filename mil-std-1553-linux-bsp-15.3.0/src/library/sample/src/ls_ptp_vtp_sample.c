/* SPDX-FileCopyrightText: 2010-2022 AIM GmbH <info@aim-online.com> */
/* SPDX-License-Identifier: MIT */

/*
This sample shows how to implement a RT with external bus coupling.
This sample creates a RT19 with various SA and MC enabled.
The sample needs a bus with transformer termination and an external BC.
This sample can be used in combination with the ls_bc_sample
which implements a BC / RT19 transfers on SA30.
*/


#include "1553_sample.h"

#include "ls_ptp_vtp_sample.h"



#define SHARED_BUFFER_SA 30
#define BROADCAST_RT     31

#define FW_CMD_ENABLE_RT  0x03
#define FW_CMD_DISABLE_RT 0x0B

AiUInt8 ucTFBit = 0;
AiUInt8 ucSSFBit = 0;
AiUInt8 ucBUSYBit = 0;
AiUInt8 ucSRQBit = 0;
AiUInt8 ucFailSaftCondition = 0;
AiUInt8 ucRtParityErr = 0;

static AiUInt32 rt_addr     = 19;
static AiUInt32 rt_sa_count = 25;
static AiUInt32 rt_sa[][3]  =
                                {
                                   /* SA/MC, TYPE, Word Count */
                                    { 3, API_RT_TYPE_RECEIVE_SA, 1 },
                                    { SHARED_BUFFER_SA, API_RT_TYPE_RECEIVE_SA, 0 }, /*Buffer and Status shared between TX and RX by default*/
                                    { 5, API_RT_TYPE_RECEIVE_SA, 1 },
                                    { 26, API_RT_TYPE_RECEIVE_SA, 1 }, /*HSSA enabled by default*/

                                    { 1, API_RT_TYPE_TRANSMIT_SA, 1 },
                                    { 3, API_RT_TYPE_TRANSMIT_SA, 6 },
                                    { 20, API_RT_TYPE_TRANSMIT_SA, 1 },
                                    { 5, API_RT_TYPE_TRANSMIT_SA, 1 },
                                    { 22, API_RT_TYPE_TRANSMIT_SA, 1 },
                                    { SHARED_BUFFER_SA, API_RT_TYPE_TRANSMIT_SA, 0 },
                                    { 21, API_RT_TYPE_TRANSMIT_SA, 1 },
                                    { 26, API_RT_TYPE_TRANSMIT_SA, 0 },

                                    { 17, API_RT_TYPE_RECEIVE_MODECODE, 0 }, /*Synchronize With Data */

                                    { 0, API_RT_TYPE_TRANSMIT_MODECODE, 0 }, /*Dynamic Bus Control*/
                                    { 1, API_RT_TYPE_TRANSMIT_MODECODE, 0 }, /*Synchronize*/
                                    { 2, API_RT_TYPE_TRANSMIT_MODECODE, 0 }, /*Transmit Status Word*/
                                    { 3, API_RT_TYPE_TRANSMIT_MODECODE, 0 }, /*Initiate Selftest*/
                                    { 4, API_RT_TYPE_TRANSMIT_MODECODE, 0 }, /*Transmitter Shutdown*/
                                    { 5, API_RT_TYPE_TRANSMIT_MODECODE, 0 }, /*Overwrite Transmitter Shutdown*/
                                    { 6, API_RT_TYPE_TRANSMIT_MODECODE, 0 }, /*Inhibit Terminal Flag*/
                                    { 7, API_RT_TYPE_TRANSMIT_MODECODE, 0 }, /*Overwrite Inhibit Terminal Flag*/
                                    { 8, API_RT_TYPE_TRANSMIT_MODECODE, 0 }, /*Reset RT*/
                                    { 16, API_RT_TYPE_TRANSMIT_MODECODE, 0 }, /*Transmit Vector Word*/
                                    { 18, API_RT_TYPE_TRANSMIT_MODECODE, 0 }, /*Transmit Last Command*/
                                    { 19, API_RT_TYPE_TRANSMIT_MODECODE, 0 } /*Transmit BIT Word*/
                                };

static AiUInt32 rt31_sa_count = 11;
static AiUInt32 rt31_sa[][3] =
                                {
                                    /* SA/MC, TYPE, Word Count */
                                    { 17, API_RT_TYPE_RECEIVE_SA, 8 },
                                    { 19, API_RT_TYPE_RECEIVE_SA, 1 },
                                    { 26, API_RT_TYPE_RECEIVE_SA, 1 }, /*HSSA enabled by default*/

                                    { 17, API_RT_TYPE_RECEIVE_MODECODE, 0 }, /*Synchronize With Data */

                                    { 1, API_RT_TYPE_TRANSMIT_MODECODE, 0 }, /*Synchronize*/
                                    { 3, API_RT_TYPE_TRANSMIT_MODECODE, 0 }, /*Initiate Selftest*/
                                    { 4, API_RT_TYPE_TRANSMIT_MODECODE, 0 }, /*Transmitter Shutdown*/
                                    { 5, API_RT_TYPE_TRANSMIT_MODECODE, 0 }, /*Overwrite Transmitter Shutdown*/
                                    { 6, API_RT_TYPE_TRANSMIT_MODECODE, 0 }, /*Inhibit Terminal Flag*/
                                    { 7, API_RT_TYPE_TRANSMIT_MODECODE, 0 }, /*Overwrite Inhibit Terminal Flag*/
                                    { 8, API_RT_TYPE_TRANSMIT_MODECODE, 0 },  /*Reset RT*/
};

static const char * sa_type_name[] ={ "RECEIVE", "TRANSMIT", "RECEIVE_MODECODE", "TRANSMIT_MODECODE" };


/* ######################################################################################*/
/*                                                                                       */
/*                                     Sample entry point                                */
/*                                                                                       */
/* ######################################################################################*/
AiReturn LsPtpVtpSample(AiUInt32 ulModHandle)
{
    AiUInt32 loop = 1;
    AiReturn ret = 0;
    AiChar c;

    printf("Running LsPtpVtpSample\n");
    ret = LsPtpVtpSample_PrepareAndStartRTOperation(ulModHandle, rt_addr);
    AIM_TRACE_RETURN(ret);

    printf("\n");
    printf("RT simulation running\n");
    LsPtpVtpSample_Help();

    while (loop)
    {
        c = AiOsGetChar();
        fflush(stdin);

        switch (c)
        {
        case 'q':
            loop = 0;
            break;
        case 'p':
            LsPtpVtpSample_DTP(ulModHandle);
            break;
        case 'a':
            LsPtpVtpSample_DTPServer(ulModHandle);
            break;
        case 'd':
            LsPtpVtpSample_DataDump(ulModHandle);
            break;
        case 's':
            LsPtpVtpSample_Status(ulModHandle);
            break;
        case '\n':
            break;
        case 'h':
        default:
            LsPtpVtpSample_Help();
            break;
        }
    }

    LsPtpVtpSample_StopOperation(ulModHandle);
    return ret;
}

AiReturn LsPtpVtpSample_Automatic(AiUInt32 ulModHandle)
{
    AiReturn ret = 0;

    printf("Running LsPtpVtpSample (Jenkins)\n");
    ret = LsPtpVtpSample_PrepareAndStartRTOperation(ulModHandle, rt_addr);
    AIM_TRACE_RETURN(ret);
    printf("RT simulation running\n");
    LsPtpVtpSample_DTPServer(ulModHandle);
    return ret;
}

AiInt16 LsPtpVtpSample_PrepareAndStartRTOperation(AiUInt32 ulModHandle, AiUInt8 ucNewRtAddr)
{
    AiReturn ret = 0;

    ret = LsPtpVtpSample_SetupRT(ulModHandle, ucNewRtAddr);
    AIM_TRACE_RETURN(ret);

    ret = LsPtpVtpSample_SetupRT(ulModHandle, BROADCAST_RT);
    AIM_TRACE_RETURN(ret);

    ret = LsPtpVtpSample_StartOperation(ulModHandle);
    AIM_TRACE_RETURN(ret);

    return(ret);
}

AiReturn LsPtpVtpSample_SetupRT(AiUInt32 ulModHandle, AiUInt32 ulRtAddr)
{
    AiUInt32 i;
    AiReturn ret = API_OK;
    AiUInt32 offset = 0;
    AiUInt16 uwHeaderId;
    AiUInt16 uwSharedHeaderId;
    AiUInt8 isEmbedded;

    isEmbedded = CheckIfDeviceIsEmbedded(ulModHandle);

    printf("\n");

    if (isEmbedded && (ulRtAddr != BROADCAST_RT))
    {
        printf("Enable RT%d (dual redundancy enabled, SA word count check %s)\n", ulRtAddr, isEmbedded ? "enabled" : "disabled" );
        ret = ApiCmdRTIni(ulModHandle, 0, ulRtAddr, API_RT_ENABLE_DUAL_REDUNDANT, API_RT_RSP_BOTH_BUSSES, 8, ulRtAddr << 11);
        AIM_ERROR_MESSAGE_RETURN(ret);
    }
    else
    {
        printf("Enable RT%d (dual redundancy disabled, SA word count check %s)\n", ulRtAddr, isEmbedded ? "enabled" : "disabled");
        ret = ApiCmdRTIni(ulModHandle, 0, ulRtAddr, API_RT_ENABLE_SIMULATION, API_RT_RSP_BOTH_BUSSES, 8, ulRtAddr << 11);
        AIM_ERROR_MESSAGE_RETURN(ret);
    }

    uwSharedHeaderId = MilUtilNextRTHid();

    /* -- Setup RT --- */
    if (ulRtAddr != BROADCAST_RT)
    {
        for (i = 0; i < rt_sa_count; i++)
        {
            if (rt_sa[i][0] == SHARED_BUFFER_SA)
            {
                uwHeaderId = uwSharedHeaderId;
            }
            else
            {
                uwHeaderId = MilUtilNextRTHid();
            }

            /* Enable SA/MC */
            printf("Enable RT%d SA%3d  TYPE=%-20s WC=%3d\n", ulRtAddr /*rt*/, rt_sa[i][0] /*SA/MC*/, sa_type_name[rt_sa[i][1]] /*TYPE*/, rt_sa[i][2] /*wc*/);

            ret = MilUtilEnableRTSA(ulModHandle, ulRtAddr, rt_sa[i][0] /*SA/MC*/, rt_sa[i][1] /*TYPE*/, uwHeaderId);
            AIM_TRACE_RETURN(ret);

            if ((rt_sa[i][1] == API_RT_TYPE_RECEIVE_SA) || (rt_sa[i][1] == API_RT_TYPE_TRANSMIT_SA))
            {
                if (isEmbedded)
                {
                    ret = ApiCmdRTSADWCSet(ulModHandle, 0, ulRtAddr, rt_sa[i][0] /*SA/MC*/, rt_sa[i][1] /*TYPE*/, rt_sa[i][2] /* WORDCNT */);
                    AIM_TRACE_RETURN(ret);
                }
            }

        }
    }
    else
    {
        uwHeaderId = MilUtilNextRTHid();
        /* -- Setup RT31 --- */
        for (i = 0; i < rt31_sa_count; i++)
        {
            /* Enable SA/MC */
            printf("Enable RT%d SA%3d  TYPE=%-20s WC=%3d\n", 31 /*rt*/, rt31_sa[i][0] /*SA/MC*/, sa_type_name[rt31_sa[i][1]] /*TYPE*/, rt31_sa[i][2] /*wc*/);

            ret = MilUtilEnableRTSA(ulModHandle, BROADCAST_RT, rt31_sa[i][0] /*SA/MC*/, rt31_sa[i][1] /*TYPE*/, uwHeaderId);
            AIM_TRACE_RETURN(ret);

            if ((rt31_sa[i][1] == API_RT_TYPE_RECEIVE_SA) || (rt31_sa[i][1] == API_RT_TYPE_TRANSMIT_SA))
            {
                if(isEmbedded)
                {
                    ret = ApiCmdRTSADWCSet(ulModHandle, 0, BROADCAST_RT, rt31_sa[i][0] /*SA/MC*/, rt31_sa[i][1] /*TYPE*/, rt31_sa[i][2] /* WORDCNT */);
                    AIM_ERROR_MESSAGE_RETURN(ret);
                }
            }
        }
    }
    
    return ret;
}

AiReturn LsPtpVtpSample_DisableRT(AiUInt32 ulModHandle, AiUInt32 ulRtAddr)
{
    AiReturn ret = API_OK;

    ret = ApiCmdRTIni(ulModHandle, 0, ulRtAddr, API_RT_DISABLE_OPERATION, API_RT_RSP_BOTH_BUSSES, 8, ulRtAddr << 11);
    AIM_ERROR_MESSAGE_RETURN(ret);

    return ret;
}

AiReturn LsPtpVtpSample_StartOperation(AiUInt32 ulModHandle)
{
    AiReturn ret = API_OK;

    /* Start all RTs */
    ret = ApiCmdRTStart(ulModHandle, 0);
    AIM_ERROR_MESSAGE_RETURN(ret);

    return ret;
}

AiReturn LsPtpVtpSample_StopOperation(AiUInt32 ulModHandle)
{
    AiReturn ret = API_OK;

    /* Stop all RTs */
    ret = ApiCmdRTHalt(ulModHandle, 0);
    AIM_ERROR_MESSAGE_RETURN(ret);

    return ret;
}

void LsPtpVtpSample_Dump(AiUInt32 count, AiUInt16 * data)
{
    AiUInt32 i;

    for( i = 0; i < 32; i++ )
    {
        if( (i % 8) == 0 )
            printf("\n%04X: ", i);
        printf("%04X ", data[i]);
    }
}

void LsPtpVtpSample_DTPHelp()
{
    char szOn[10] = "ON    ";
    char szOff[10] = "OFF   ";

    printf("==============================================\n");
    printf("Enabled RT = %d\n", rt_addr);
    printf("==============================================\n");
    printf("TF    SSF   BUSY  SRQ   FST   RT-ParityError\n");
    printf("%s", ucTFBit ? szOn : szOff);
    printf("%s", ucSSFBit ? szOn : szOff);
    printf("%s", ucBUSYBit ? szOn : szOff);
    printf("%s", ucSRQBit ? szOn : szOff);
    printf("%s", ucFailSaftCondition ? szOn : szOff);
    printf("%s", ucRtParityErr ? szOn : szOff);
    printf("\n==============================================\n");
    printf("q: quit\n");
    printf("1: Set TF on\n");
    printf("2: Set TF off\n");
    printf("3: Set SSF on\n");
    printf("4: Set SSF off\n");
    printf("5: Set BUSY on\n");
    printf("6: Set BUSY off\n");
    printf("7: Set SRQ on\n");
    printf("8: Set SRQ off\n");
    printf("9: Set Transmit Vector Word to 0x5555\n");
    printf("a: Set Transmit BIT Word to 0x5555\n");
    printf("b: Enable Fail Safe Timer Condition\n");
    printf("c: Disable Fail Safe Timer Condition\n");
    printf("d: Switch RT address\n");
    printf("e: Set Error on the RT address lines on\n");
}

AiReturn LsPtpVtpSample_SetNXW(AiUInt32 ulModHandle, AiUInt32 ulRtAddr, AiUInt32 ulBitMask, AiBoolean bSet)
{
    AiReturn ret = API_OK;
    AiUInt32 ulNXW = 0;
    TY_API_RT_MSG_DSP msg_dsp;

    ret = ApiCmdRTMsgRead(ulModHandle, 0, ulRtAddr, &msg_dsp);
    AIM_ERROR_MESSAGE_RETURN(ret);
    ulNXW = msg_dsp.nxw;

    switch (bSet)
    {
    case 0:  /* reset */
        ulNXW = ulNXW & (~ulBitMask);
        break;
    case 1:  /* set */
        ulNXW = ulNXW | ulBitMask;
        break;
    default:
        printf("ERROR: set not in range\n");
        break;
    }
    /* redefine the Next RT Status Word */
    ret = ApiCmdRTNXW(ulModHandle, 0, ulRtAddr, ulNXW);
    AIM_ERROR_MESSAGE_RETURN(ret);

    return ret;
}


void LsPtpVtpSample_DTP(AiUInt32 ulModHandle)
{
    AiUInt32 loop = 1;
    AiReturn ret = 0;
    AiChar c;
    AiUInt16 data[32];
    AiUInt32 ulBufHeaderIndex;
    AiUInt32 ulBufHeaderAddr;
    AiUInt16 uwOutIndex;
    AiUInt32 ulOutAddr;
    AiUInt32 ulNewRtAddr;
    char     szDummy[32];

    printf("Ready for Diagnostic Test Procedure run:\n");
    LsPtpVtpSample_DTPHelp();

    while (loop)
    {
        LsPtpVtpSample_DTPHelp();
        c = AiOsGetChar();
        fflush(stdin);
        printf("key = %c\n",c);
        switch (c)
        {
        case 'q':
            loop = 0;
            LsPtpVtpSample_Help();
            break;
        case '1':
            LsPtpVtpSample_SetNXW(ulModHandle, rt_addr, 0x1, TRUE);
            ucTFBit = 1;
            break;
        case '2':
            LsPtpVtpSample_SetNXW(ulModHandle, rt_addr, 0x1, FALSE);
            ucTFBit = 0;
            break;
        case '3':
            LsPtpVtpSample_SetNXW(ulModHandle, rt_addr, 0x4, TRUE);
            ucSSFBit = 1;
            break;
        case '4':
            LsPtpVtpSample_SetNXW(ulModHandle, rt_addr, 0x4, FALSE);
            ucSSFBit = 0;
            break;
        case '5':
            LsPtpVtpSample_SetNXW(ulModHandle, rt_addr, 0x8, TRUE);
            ucBUSYBit = 1;
            break;
        case '6':
            LsPtpVtpSample_SetNXW(ulModHandle, rt_addr, 0x8, FALSE);
            ucBUSYBit = 0;
            break;
        case '7':
            LsPtpVtpSample_SetNXW(ulModHandle, rt_addr, 0x100, TRUE);
            ucSRQBit = 1;
            break;
        case '8':
            LsPtpVtpSample_SetNXW(ulModHandle, rt_addr, 0x100, FALSE);
            ucSRQBit = 0;
            break;
        case '9':
            ret = ApiCmdRTGetSABufferHeaderInfo(ulModHandle, 0, rt_addr, API_RT_TYPE_TRANSMIT_MODECODE, 16 /*SA/MC*/, &ulBufHeaderIndex, &ulBufHeaderAddr);
            AIM_ERROR_MESSAGE(ret);

            data[0] = 0x5555;
            ret = ApiCmdBufDef(ulModHandle, 0, API_BUF_RT_MSG, ulBufHeaderIndex, API_BUF_WRITE_TO_CURRENT, 1, data, &uwOutIndex, &ulOutAddr);
            AIM_ERROR_MESSAGE(ret);
            break;
        case 'a':
            ret = ApiCmdRTGetSABufferHeaderInfo(ulModHandle, 0, rt_addr, API_RT_TYPE_TRANSMIT_MODECODE, 19 /*SA/MC*/, &ulBufHeaderIndex, &ulBufHeaderAddr);
            AIM_ERROR_MESSAGE(ret);

            data[0] = 0x5555;
            ret = ApiCmdBufDef(ulModHandle, 0, API_BUF_RT_MSG, ulBufHeaderIndex, API_BUF_WRITE_TO_CURRENT, 1, data, &uwOutIndex, &ulOutAddr);
            AIM_ERROR_MESSAGE(ret);
            break;
        case 'b':
            ret = ApiCmdCalSigCon(ulModHandle, 0, API_CAL_BUS_PRIMARY, API_ENA);
            AIM_ERROR_MESSAGE(ret);
            ret = ApiCmdCalSigCon(ulModHandle, 0, API_CAL_BUS_SECONDARY, API_ENA);
            AIM_ERROR_MESSAGE(ret);
            ucFailSaftCondition = 1;
            break;
        case 'c':
            ret = ApiCmdCalSigCon(ulModHandle, 0, API_CAL_BUS_PRIMARY, API_DIS);
            AIM_ERROR_MESSAGE(ret);
            ret = ApiCmdCalSigCon(ulModHandle, 0, API_CAL_BUS_SECONDARY, API_DIS);
            AIM_ERROR_MESSAGE(ret);
            ucFailSaftCondition = 0;
            break;
        case 'd':
            LsPtpVtpSample_DisableRT(ulModHandle, rt_addr);
            MilUtilResetIds();
            printf("RT address to switch to: ");
            AiOsGetString(szDummy, sizeof(szDummy), "0");
            sscanf( szDummy, "%d", &ulNewRtAddr);
            LsPtpVtpSample_PrepareAndStartRTOperation(ulModHandle, ulNewRtAddr);
            rt_addr = ulNewRtAddr;
            break;
        case 'e':
            /* disable RT on the fly to stimulate an address line error*/
            ret = ApiCmdRTEnaDis(ulModHandle, 0, rt_addr, API_DIS);
            AIM_ERROR_MESSAGE(ret);
            break;
        case '\n':
            break;
        case 'h':
        default:
            LsPtpVtpSample_DTPHelp();
            break;
        }
    }
}


AiReturn LsPtpVtpSample_DtpCommand(AiUInt32 ulModHandle, struct dtp_command cmd)
{
    AiReturn ret = 0;
    AiUInt16 data;
    AiUInt32 ulBufHeaderIndex;
    AiUInt32 ulBufHeaderAddr;
    AiUInt16 uwOutIndex;
    AiUInt32 ulOutAddr;
    AiUInt32 ulNewRtAddr;

    switch (cmd.command)
    {
    case DTP_COMMAND_SET_NXW:
        printf("DTP_COMMAND_SET_NXW %x\n", cmd.payload);
        LsPtpVtpSample_SetNXW(ulModHandle, rt_addr, cmd.payload, TRUE);
        break;
    case DTP_COMMAND_CLEAR_NXW:
        printf("DTP_COMMAND_CLEAR_NXW %x\n", cmd.payload);
        LsPtpVtpSample_SetNXW(ulModHandle, rt_addr, cmd.payload, FALSE);
        break;
    case DTP_COMMAND_SET_MC16_DATA:
        printf("DTP_COMMAND_SET_MC16_DATA %x\n", cmd.payload);
        data = (AiUInt16)cmd.payload;
        ret = ApiCmdRTGetSABufferHeaderInfo(ulModHandle, 0, rt_addr, API_RT_TYPE_TRANSMIT_MODECODE, 16 /*SA/MC*/, &ulBufHeaderIndex, &ulBufHeaderAddr);
        AIM_ERROR_MESSAGE(ret);
        ret = ApiCmdBufDef(ulModHandle, 0, API_BUF_RT_MSG, ulBufHeaderIndex, API_BUF_WRITE_TO_CURRENT, 1, &data, &uwOutIndex, &ulOutAddr);
        AIM_ERROR_MESSAGE(ret);
        break;
    case DTP_COMMAND_SET_MC19_DATA:
        data = (AiUInt16)cmd.payload;
        printf("DTP_COMMAND_SET_MC19_DATA %x\n", cmd.payload);
        ret = ApiCmdRTGetSABufferHeaderInfo(ulModHandle, 0, rt_addr, API_RT_TYPE_TRANSMIT_MODECODE, 19 /*SA/MC*/, &ulBufHeaderIndex, &ulBufHeaderAddr);
        AIM_ERROR_MESSAGE(ret);
        ret = ApiCmdBufDef(ulModHandle, 0, API_BUF_RT_MSG, ulBufHeaderIndex, API_BUF_WRITE_TO_CURRENT, 1, &data, &uwOutIndex, &ulOutAddr);
        AIM_ERROR_MESSAGE(ret);

        break;
    case DTP_COMMAND_INIT:
        printf("DTP_COMMAND_INIT\n");
        break;
    case DTP_COMMAND_SET_RT_ADDR:
        printf("DTP_COMMAND_SET_RT_ADDR %d\n", cmd.payload);
        ulNewRtAddr = (AiUInt16)cmd.payload;
        LsPtpVtpSample_DisableRT(ulModHandle, rt_addr);

        MilUtilResetIds();

        LsPtpVtpSample_PrepareAndStartRTOperation(ulModHandle, ulNewRtAddr);
        rt_addr = ulNewRtAddr;
        break;
    case DTP_COMMAND_DISABLE_RT:
        /* disable RT on the fly to stimulate an address line error*/
        ret = ApiCmdRTEnaDis(ulModHandle, 0, rt_addr, API_DIS);
        AIM_ERROR_MESSAGE(ret);
        break;
    case DTP_COMMAND_EXIT:
        printf("DTP_COMMAND_EXIT\n");
        LsPtpVtpSample_DTPServerCleanup(ulModHandle);
        LsPtpVtpSample_StopOperation(ulModHandle);
        ApiClose(ulModHandle);
        exit(0);
        break;
    default:
        printf("Unknown command %x %x\n", cmd.command, cmd.payload);
        break;
    }

    return ret;

}




AiReturn LsPtpVtpSample_DataDump(AiUInt32 ulModHandle)
{
    AiReturn ret = API_OK;
    AiUInt32 i;
    AiUInt16 data[32];
    AiUInt32 ulBufHeaderIndex;
    AiUInt32 ulBufHeaderAddr;
    AiUInt16 uwId;
    AiUInt32 ulAddr;

    /* Dump Data of the receive SAs on RT */
    for( i = 0; i < rt_sa_count; i++ )
    {
        if (rt_sa[i][1] == API_RT_TYPE_RECEIVE_SA)
        {
            ret = ApiCmdRTGetSABufferHeaderInfo(ulModHandle, 0, rt_addr, API_RT_TYPE_RECEIVE_SA, rt_sa[i][0] /*SA/MC*/, &ulBufHeaderIndex, &ulBufHeaderAddr);
            AIM_ERROR_MESSAGE_RETURN(ret);

            ret = ApiCmdBufRead(ulModHandle, 0, API_BUF_RT_MSG, ulBufHeaderIndex, API_BUF_READ_FROM_LAST, 32, data, &uwId, &ulAddr);
            AIM_ERROR_MESSAGE_RETURN(ret);

            printf("\n");
            printf("RT RCV SA%d", rt_sa[i][0]);
            LsPtpVtpSample_Dump(32, data);
        }
    }
    printf("\n");

    return ret;
}

AiReturn LsPtpVtpSample_Status(AiUInt32 ulModHandle)
{
    AiReturn ret = API_OK;
    TY_API_RT_STATUS_DSP xRtStatus;
    memset(&xRtStatus, 0, sizeof(xRtStatus));

    ret = ApiCmdRTStatusRead(ulModHandle, 0, &xRtStatus);

    AIM_ERROR_MESSAGE_RETURN(ret);

    printf("RTs ( %d messages  / %d errors )\n", xRtStatus.glb_msg_cnt, xRtStatus.glb_err_cnt);

    return ret;
}

void LsPtpVtpSample_Help()
{
    printf("q: quit\n");
    printf("p: RT1553 PTP/VTP Mode\n");
    printf("a: RT1553 PTP/VTP Mode (automatic)\n");
    printf("s: status\n");
    printf("d: dump data\n");
    printf("h: help\n");
}

