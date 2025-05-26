/* SPDX-FileCopyrightText: 2015-2024 AIM GmbH <info@aim-online.com> */
/* SPDX-License-Identifier: MIT */

/*
This file contains helper functions that are used in the samples.
*/

#include "mil_util.h"

static AiUInt32 g_bc_hid = 1;
static AiUInt32 g_rt_hid = 1;
static AiUInt32 g_bid[MAX_BIU] = {1,100,200,300};

/* Reset ids when the simulation is stopped and is reconfigured */
void MilUtilResetIds()
{
    g_bc_hid    = 1;
    g_rt_hid    = 1;
    g_bid[0]    = 1;
    g_bid[1]    = 101;
    g_bid[2]    = 201;
    g_bid[3]    = 301;
}

/* BC buffer header ids are uniqe for each stream */
AiUInt32 MilUtilNextBCHid()
{
    AiUInt32 id = g_bc_hid;
    g_bc_hid++;
    return id;
}

/* RT buffer header ids are uniqe for each stream */
AiUInt32 MilUtilNextRTHid()
{
    AiUInt32 id = g_rt_hid;
    g_rt_hid++;
    return id;
}

/* Buffer Ids are shared between RT,BC and between all streams on one module */
AiUInt32 MilUtilNextBid(AiUInt32 ulModHandle, AiUInt32 qSize)
{
    /* Note: No check is applied here if we reached the next section since the
    sample does uses only a few buffers.
    There might be more buffers available depending on the amount
    of memory equiped on your module. See ApiCmdSysGetMemPartition
    x_Sim[0].ul_BufCount for details. */
    AiUInt32 id = g_bid[GET_STREAM_ID(ulModHandle) - 1];
    g_bid[GET_STREAM_ID(ulModHandle) - 1] += 1<<qSize;
    return id;
}

AiUInt32 MilUtilIsBcActive(AiUInt32 ulModHandle)
{
    AiReturn ret;
    TY_API_BC_STATUS_DSP xBcStatus;

    memset(&xBcStatus, 0, sizeof(xBcStatus));

    ret = ApiCmdBCStatusRead(ulModHandle, 0, &xBcStatus);

    AIM_ERROR_MESSAGE(ret);

    if ((ret==API_OK) && (xBcStatus.status == API_BC_STATUS_BUSY))
        return 1;
    else
        return 0;
}

AiUInt32 MilUtilIsBcActiveCheckCount(AiUInt32 ulModHandle, AiUInt32 ulTransferCount)
{
    AiReturn ret;
    TY_API_BC_STATUS_DSP xBcStatus;

    memset(&xBcStatus, 0, sizeof(xBcStatus));

    ret = ApiCmdBCStatusRead(ulModHandle, 0, &xBcStatus);

    AIM_ERROR_MESSAGE(ret);

    if ((ret==API_OK) && ((xBcStatus.glb_msg_cnt + xBcStatus.glb_err_cnt) < ulTransferCount))
        // Return count of transfer left until BC done
        return ulTransferCount - (xBcStatus.glb_msg_cnt + xBcStatus.glb_err_cnt);
    else
        return 0;
}

AiReturn MilUtilConfigureHardware(AiUInt32 ulModHandle, AiUInt8 coupling)
{
    AiReturn ret = 0;

    /* Set the primary coupling */
    /*
        0 API_CAL_CPL_ISOLATED           Isolated, on-board termination
        1 API_CAL_CPL_TRANSFORM          Transformer coupled MILbus
        2 API_CAL_CPL_DIRECT             Direct coupled MILbus
        3 API_CAL_CPL_EXTERNAL           Externally coupled MILbus with on-board transformer-coupled MILbus termination
        4 API_CAL_CPL_WRAP_AROUND_LOOP   On-board, digital wrap-around loop between MILbus Encoder and Decoder
        without relying on the external MILbus.
        */
    ret = ApiCmdCalCplCon(ulModHandle, 0, API_CAL_BUS_PRIMARY, coupling);

    AIM_ERROR_MESSAGE_RETURN(ret);

    /* Set the secondary coupling */
    ret = ApiCmdCalCplCon(ulModHandle, 0, API_CAL_BUS_SECONDARY, coupling);

    AIM_ERROR_MESSAGE_RETURN(ret);

    return 0;
}



AiReturn MilUtilEnableMailboxRT(AiUInt32 ulModHandle, AiUInt8 ucRt)
{
    AiReturn ret = 0;

    /* ApiCmdRTIni( module, biu,  RT,  simulation, bus, response_time, status_word (RT<<11) ); */
    ret = ApiCmdRTIni(ulModHandle, 0, ucRt, API_RT_ENABLE_MONITORING, API_RT_RSP_BOTH_BUSSES, 8, ucRt << 11);

    AIM_ERROR_MESSAGE_RETURN(ret);

    /* Enable RT3 */
    ret = ApiCmdRTEnaDis(ulModHandle, 0, ucRt, API_ENA);

    AIM_ERROR_MESSAGE_RETURN(ret);

    return 0;
}



AiReturn MilUtilEnableRT(AiUInt32 ulModHandle, AiUInt8 ucRt, AiFloat rep_time)
{
    AiReturn ret = 0;

    /* ApiCmdRTIni( module, biu,  RT,  simulation, bus, response_time, status_word (RT<<11) ); */
    ret = ApiCmdRTIni(ulModHandle, 0, ucRt, API_RT_ENABLE_SIMULATION, API_RT_RSP_BOTH_BUSSES, rep_time, ucRt << 11);

    AIM_ERROR_MESSAGE_RETURN(ret);

    /* Enable RT3 */
    ret = ApiCmdRTEnaDis(ulModHandle, 0, ucRt, API_ENA);

    AIM_ERROR_MESSAGE_RETURN(ret);

    return 0;
}



AiReturn MilUtilEnableRTSABufferQueue(AiUInt32 ulModHandle,
    AiUInt8  ucRt,
    AiUInt8  ucSa,
    AiUInt8  ucType,
    AiUInt16 uwHeaderId,
    AiUInt8  ucQueueSize)
{
    int      i              = 0;
    int      bid_count      = 0;
    AiReturn ret        = 0;
    AiUInt16 uwOutIndex     = 0;
    AiUInt32 ulOutAddr      = 0;
    AiUInt16 uwBufferId     = MilUtilNextBid(ulModHandle,ucQueueSize);

    AiUInt16 auwMessageData[32];
    TY_API_RT_BH_INFO xBhInfo;

    memset(&xBhInfo, 0, sizeof(xBhInfo));

    memset(&auwMessageData, 0, sizeof(auwMessageData));

    bid_count = 1 << ucQueueSize;

    /* ApiCmdRTBHDef(   module, 0, HID,     BID, 0, 0, */
    ret = ApiCmdRTBHDef(ulModHandle, 0, uwHeaderId, uwBufferId, 0, 0,
        ucQueueSize,        /* buffer/status queue size in pow(2,n) */
        API_BQM_CYCLIC,          /* cyclic buffer queue  */
        API_BSM_RX_KEEP_CURRENT, /* keep same buffer on error */
        API_SQM_AS_QSIZE,        /* enable status queue */
        0, 0, &xBhInfo);

    AIM_ERROR_MESSAGE_RETURN(ret);

    for (i=1; i<=bid_count; i++)
    {
        auwMessageData[0] = (ucSa <<8 | i);

        /* ApiCmdBufDef(   module, 0, rt/bc,            HID, relative offset, Word count, data,           info_out,      addr_out ); */
        ret = ApiCmdBufDef(ulModHandle, 0, API_BUF_RT_MSG, uwHeaderId, i, 32, auwMessageData, &uwOutIndex, &ulOutAddr);
        AIM_ERROR_MESSAGE_RETURN(ret);
    }

    /* ApiCmdRTSACon(   module, 0, RT,   SA,     HID,  type,                    enabled,          0, 0, 0 ); */
    ret = ApiCmdRTSACon(ulModHandle, 0, ucRt, ucSa, uwHeaderId, ucType, API_RT_ENABLE_SA, 0, 0, 0);

    AIM_ERROR_MESSAGE_RETURN(ret);

    return 0;
}

AiReturn MilUtilEnableRTSA(AiUInt32 ulModHandle,
    AiUInt8 ucRt,
    AiUInt8 ucSa,
    AiUInt8 ucType,
    AiUInt16 uwHeaderId)
{
    return MilUtilEnableRTSABufferQueue(ulModHandle, ucRt, ucSa, ucType, uwHeaderId, API_QUEUE_SIZE_1);
}



AiReturn MilUtilBCCreateTransferWithGap(AiUInt32 ulModHandle,
    AiUInt16  uwTransferId,
    AiUInt8   ucType,
    AiUInt8   ucTransmitRt,
    AiUInt8   ucTransmitSa,
    AiUInt8   ucReceiveRt,
    AiUInt8   ucReceiveSa,
    AiUInt8   ucWordCount,
    AiUInt16  uwHeaderId,
    AiUInt8   ucQueueSize,
    AiUInt32  ulInterruptControl,
    AiUInt8   gap_mode,
    AiUInt16  gap)
{
    AiReturn  ret = 0;
    AiUInt32 ulOutAddr = 0;
    AiUInt16 uwBufferOffset = 0;
    AiUInt32 ulBufferCount = 0;
    AiUInt16 uwBufferId = MilUtilNextBid(ulModHandle, ucQueueSize);

    AiUInt16 auwMessageData[32];

    TY_API_BC_BH_INFO  xBhInfo;
    TY_API_BC_XFER     xXferDescriptor;

    memset(&xBhInfo, 0, sizeof(xBhInfo));
    memset(&xXferDescriptor, 0, sizeof(xXferDescriptor));
    memset(&auwMessageData, 0, sizeof(auwMessageData));


    ulBufferCount = 1 << ucQueueSize;

    /*********************************************/
    /*                                           */
    /* --            BC -> RT3RcvSA13        --- */
    /*                                           */
    /*********************************************/
    xXferDescriptor.xid = uwTransferId;
    xXferDescriptor.hid = uwHeaderId;
    xXferDescriptor.type = ucType;
    xXferDescriptor.chn = API_BC_XFER_BUS_PRIMARY;
    xXferDescriptor.xmt_rt = ucTransmitRt;
    xXferDescriptor.xmt_sa = ucTransmitSa;
    xXferDescriptor.rcv_rt = ucReceiveRt;
    xXferDescriptor.rcv_sa = ucReceiveSa;
    xXferDescriptor.wcnt = ucWordCount;
    xXferDescriptor.tic = ulInterruptControl;
    xXferDescriptor.gap_mode = gap_mode;
    xXferDescriptor.gap = gap;

    /* Install the transfer descriptor */
    ret = ApiCmdBCXferDef(ulModHandle, 0, &xXferDescriptor, &ulOutAddr);

    AIM_ERROR_MESSAGE_RETURN(ret);


    /* ApiCmdBCBHDef(   module, 0, HID,   BID,   0, 0, */
    ret = ApiCmdBCBHDef(ulModHandle, 0, uwHeaderId, uwBufferId, 0, 0,
        ucQueueSize,            /* buffer/status queue size in pow(2,n) */
        API_BQM_CYCLIC,         /* cyclic buffer queue */
        API_BSM_TX_KEEP_SAME,   /* keep same buffer on error */
        API_SQM_AS_QSIZE,       /* enable status queue */
        0, 0, &xBhInfo);

    AIM_ERROR_MESSAGE_RETURN(ret);

    /* Fill all buffers in queue with buffer id  */
    for (uwBufferOffset = 1; uwBufferOffset <= ulBufferCount; uwBufferOffset++)
    {
        ret = MilUtilSetData(ulModHandle, uwHeaderId, API_BUF_BC_MSG, uwBufferOffset, 32, auwMessageData, NULL);

        AIM_TRACE_RETURN(ret);
    }



    return ret;
}





AiReturn MilUtilBCCreateTransfer(AiUInt32 ulModHandle,
    AiUInt16  uwTransferId,
    AiUInt8   ucType,
    AiUInt8   ucTransmitRt,
    AiUInt8   ucTransmitSa,
    AiUInt8   ucReceiveRt,
    AiUInt8   ucReceiveSa,
    AiUInt8   ucWordCount,
    AiUInt16  uwHeaderId,
    AiUInt8   ucQueueSize,
    AiUInt32  ulInterruptControl)
{
    return MilUtilBCCreateTransferWithGap(ulModHandle,
        uwTransferId, ucType, ucTransmitRt, ucTransmitSa,
        ucReceiveRt, ucReceiveSa, ucWordCount, uwHeaderId,
        ucQueueSize, ulInterruptControl,
        0 /* gap_mode*/, 0 /*  gap*/);
    
}

AiReturn MilUtilBCCreateSingleFrameFraming(AiUInt32 ulModHandle, AiUInt16 aulTransferIds[], AiUInt32 ulTransferIdsCount)
{
    AiReturn ret;
    AiUInt32 i;
    TY_API_BC_FRAME      xMinorFrame;
    TY_API_BC_MFRAME_EX  xMajorFrame;

    memset(&xMinorFrame, 0, sizeof(xMinorFrame));
    memset(&xMajorFrame, 0, sizeof(xMajorFrame));

    /*********************************************/
    /*                                           */
    /* --         Prepare Minor Frame        --- */
    /*                                           */
    /*********************************************/
    for (i=0; i<ulTransferIdsCount; i++)
    {
        /*
        Each minor frame can contain up to 128 [cnt] instructions:
        For transfers the [xid] contains the Transfer id and [instr] is set to API_BC_INSTR_TRANSFER
        Other instructions possible are API_BC_INSTR_SKIP (Used to generate BC branch interrupts), API_BC_INSTR_WAIT and API_BC_INSTR_STROBE
        */
        xMinorFrame.xid[i]   = aulTransferIds[i]; /* Transfer Id */
        xMinorFrame.instr[i] = API_BC_INSTR_TRANSFER;
    }
    xMinorFrame.cnt = ulTransferIdsCount;

    /*********************************************/
    /*                                           */
    /* --         Install Minor Frame        --- */
    /*                                           */
    /*********************************************/
    /*
    The API can handle up to 64 uniqe Minor Frames.
    The minor frame is address by its id [id].
    */
    xMinorFrame.id = 1;
    ret = ApiCmdBCFrameDef(ulModHandle, 0, &xMinorFrame);
    AIM_ERROR_MESSAGE_RETURN(ret);

    /*********************************************/
    /*                                           */
    /* --         Install Major Frame        --- */
    /*                                           */
    /*********************************************/
    /*
        The API can handle only a single Major Frame.
        This major frame can contain up to 512 [cnt] Minor Frames idendified by their ids [fid].
        */
    xMajorFrame.cnt    = 1;
    xMajorFrame.fid[0] = 1;
    ret = ApiCmdBCMFrameDefEx(ulModHandle, 0, &xMajorFrame);
    AIM_ERROR_MESSAGE_RETURN(ret);


    return API_OK;
}



AiReturn MilUtilGetData(AiUInt32 ulModHandle,
    AiUInt16  uwHeaderId,
    AiUInt8   ucBufferType,
    AiUInt16  uwIndex,
    AiUInt8   ucWordCount,
    AiUInt16  auwMessageData[])
{
    AiReturn  ret        = 0;
    AiUInt16 uwOutIndex     = 0;
    AiUInt32 ulOutAddr      = 0;

    ret = ApiCmdBufRead(ulModHandle,
        0,
        ucBufferType,
        uwHeaderId,
        uwIndex,
        ucWordCount,
        auwMessageData,
        &uwOutIndex,
        &ulOutAddr);

    AIM_ERROR_MESSAGE_RETURN(ret);

    return 0;
}
/*
*   MilUtilSetData
*   initialisze all 32 words of the buffer to 0 and set
*   the data in the defined range
*   ulOutAddr: Corresponding Data Buffer Address
*/
AiReturn MilUtilSetData(AiUInt32  ulModHandle,
    AiUInt16  uwHeaderId,
    AiUInt8   ucBufferType,
    AiUInt16  uwIndex,
    AiUInt8   ucWordCount,
    AiUInt16  auwMessageData[],
    AiUInt32  *ulOutAddr)
{
    AiReturn  ret        = 0;
    AiUInt16 uwOutIndex     = 0;
    AiUInt32 ulLocalOutAddr = 0;

    ret = ApiCmdBufDef(ulModHandle, 0,
        ucBufferType,
        uwHeaderId,
        uwIndex, /* write to buffer at index 1..n */
        ucWordCount,
        auwMessageData,
        &uwOutIndex,
        &ulLocalOutAddr);

    AIM_ERROR_MESSAGE_RETURN(ret);

    if (ulOutAddr != NULL)
        *ulOutAddr = ulLocalOutAddr;

    return 0;
}




/*
returns 0 if the data is valid
retuns 1 if the data is not valid
*/
AiReturn MilUtilBMLsInterpretData(AiUInt32 ulModHandle, MIL_UTIL_BM_ITERATOR * pIterator, TY_API_QUEUE_BUF_EX * pInterpreted)
{
    AiUInt32 ulIndex             = 0;
    AiUInt8  ucBMEntryType       = 0;
    AiUInt32 ulBMEntryData       = 0;
    AiUInt32 ulBufferIndex       = 0;
    AiUInt32 ulCurrentEntry      = 0;
    AiReturn ret             = API_OK;

    if (pIterator->iterator_position >= pIterator->buffer_end)
        return API_ERR_QUEUE_EMPTY;

    if ((pIterator->iterator_position + 3) > pIterator->buffer_end)
    {
        fprintf(stderr, "Error: %s Not enough data\n", __FUNCTION__);
        return API_ERR;
    }

    memset(pInterpreted, 0, sizeof(TY_API_QUEUE_BUF_EX));

    /* The longest BM message is (RT2RT) 38 DW */
    /* TTHigh, TTLow, CW1, CW2, SW1, DW1..DW32, SW2 */
    for (ulIndex=0; ulIndex<38; ulIndex++)
    {
        /* Interpret BM entry */
        ulCurrentEntry = BSWAP32(*pIterator->iterator_position);
        ucBMEntryType = (AiUInt8)(ulCurrentEntry >> 28); /* Bits 28 .. 31 */
        ulBMEntryData = (AiUInt32)(ulCurrentEntry & 0x3ffffff); /* Bits  0 .. 26 */

        // -- Message detection logic ---
        if ((ulIndex == 0) && (ucBMEntryType != 0x3))
        {
            /* First entry is supposed to be a TTHigh */
            ret = API_ERR;
            fprintf(stderr, "Error: %s No time tag high\n", __FUNCTION__);
            break;
        }

        if ((ulIndex == 1) && (ucBMEntryType != 0x2))
        {
            /* Second entry is supposed to be a TTLow */
            ret = API_ERR;
            fprintf(stderr, "Error: %s No time tag low\n", __FUNCTION__);
            break;
        }

        if ((ulIndex > 0) && (ucBMEntryType == 0x3))
        {
            /* We found the TTHigh of the next message.
               stop parsing here.
               */
            break;
        }

        // -- Message data interpretation ---

        switch (ucBMEntryType)
        {
        case 0x1: /* Error Word           */
            if (ulBMEntryData & API_BM_ET_NRESP_MASK) pInterpreted->msg_trw |= API_BM_MSG_ERR_NO_RESP    << 12;
            else if (ulBMEntryData & API_BM_ET_MANCH_MASK)  pInterpreted->msg_trw |= API_BM_MSG_ERR_MANCH      << 12;
            else if (ulBMEntryData & API_BM_ET_HBIT_MASK)   pInterpreted->msg_trw |= API_BM_MSG_ERR_HBIT       << 12;
            else if (ulBMEntryData & API_BM_ET_LBIT_MASK)   pInterpreted->msg_trw |= API_BM_MSG_ERR_LBIT       << 12;
            else if (ulBMEntryData & API_BM_ET_PAR_MASK)    pInterpreted->msg_trw |= API_BM_MSG_ERR_PARITY     << 12;
            else if (ulBMEntryData & API_BM_ET_ISYNC_MASK)  pInterpreted->msg_trw |= API_BM_MSG_ERR_ISYNC      << 12;
            else if (ulBMEntryData & API_BM_ET_IWGAP_MASK)  pInterpreted->msg_trw |= API_BM_MSG_ERR_GAP        << 12;
            else if (ulBMEntryData & API_BM_ET_TX_MASK)     pInterpreted->msg_trw |= API_BM_MSG_ERR_BOTH_CHN   << 12;
            else if (ulBMEntryData & API_BM_ET_ILLEGL_MASK) pInterpreted->msg_trw |= API_BM_MSG_ERR_MODECODE   << 12;
            else if (ulBMEntryData & API_BM_ET_GAP_MASK)    pInterpreted->msg_trw |= API_BM_MSG_ERR_EARLY_RESP << 12;
            else if (ulBMEntryData & API_BM_ET_TADDR_MASK)  pInterpreted->msg_trw |= API_BM_MSG_ERR_BIT_SET    << 12;
            else if (ulBMEntryData & API_BM_ET_STAT_MASK)   pInterpreted->msg_trw |= API_BM_MSG_ERR_SW_EXCEPT  << 12;
            else if (ulBMEntryData & API_BM_ET_HCNT_MASK)   pInterpreted->msg_trw |= API_BM_MSG_ERR_HCNT       << 12;
            else if (ulBMEntryData & API_BM_ET_LCNT_MASK)   pInterpreted->msg_trw |= API_BM_MSG_ERR_LCNT       << 12;
            else if (ulBMEntryData & API_BM_ET_ALTER_MASK)  pInterpreted->msg_trw |= API_BM_MSG_ERR_ILLEGL     << 12;
            break;
        case 0x2: /* Time Tag Low         */
            pInterpreted->ttag_lo = ulBMEntryData;
            break;
        case 0x3: /* Time Tag High        */
            pInterpreted->ttag_hi = ulBMEntryData;
            break;
        case 0x8: /* Command Word   (Pri) */
            pInterpreted->rt_addr  = ((ulBMEntryData>>11) & 0x1F);
            pInterpreted->sa_type  = ((ulBMEntryData>>10) & 0x1);
            pInterpreted->sa_mc    = ((ulBMEntryData>>5) & 0x1F);
            pInterpreted->word_cnt = ((ulBMEntryData)& 0x1F);
            pInterpreted->rbf_trw = 1; /* Primary */
            break;
        case 0x9: /* Command Word 2 (Pri) */
            pInterpreted->rt_addr2 = ((ulBMEntryData>>11) & 0x1F);
            pInterpreted->sa_type2 = ((ulBMEntryData>>10) & 0x1);
            pInterpreted->sa_mc2   = ((ulBMEntryData>>5) & 0x1F);
            break;
        case 0xa: /* Data Word      (Pri) */
            pInterpreted->buffer[ulBufferIndex] = ulBMEntryData;
            ulBufferIndex++;
            break;
        case 0xb: /* Status Word    (Pri) */
            pInterpreted->msg_trw |= (1<<11);                 /* Bus Primary            */
            pInterpreted->msg_trw |= (ulBMEntryData & 0x3FF); /* Status Word (Bit 0..10)*/
            break;
        case 0xc: /* Command Word   (Sec) */
            pInterpreted->rt_addr  = ((ulBMEntryData>>11) & 0x1F);
            pInterpreted->sa_type  = ((ulBMEntryData>>10) & 0x1);
            pInterpreted->sa_mc    = ((ulBMEntryData>>5) & 0x1F);
            pInterpreted->word_cnt = ((ulBMEntryData)& 0x1F);
            pInterpreted->rbf_trw  = 0; /* Secondary */
            break;
        case 0xd: /* Command Word 2 (Sec) */
            pInterpreted->rt_addr2 = ((ulBMEntryData>>11) & 0x1F);
            pInterpreted->sa_type2 = ((ulBMEntryData>>10) & 0x1);
            pInterpreted->sa_mc2   = ((ulBMEntryData>>5) & 0x1F);
            break;
        case 0xe: /* Data Word      (Sec) */
            pInterpreted->buffer[ulBufferIndex] = ulBMEntryData;
            ulBufferIndex++;
            break;
        case 0xf: /* Status Word    (Sec) */
            pInterpreted->msg_trw &= ~(1<<11);                /* Bus Secondary          */
            pInterpreted->msg_trw |= (ulBMEntryData & 0x3FF); /* Status Word (Bit 0..10)*/
            break;
        case 0x0: /* Entry not updated    */
        default: /* Reserved 4..7        */
            /* Ignored */
            break;
        }

        pIterator->iterator_position++;

        if (pIterator->iterator_position >= pIterator->buffer_end)
            break;
    }

    // -- clean up ---- 

    return ret;
}


AiReturn MilUtilBMLsQueueBufShow(AiUInt32 ulModHandle, TY_API_QUEUE_BUF_EX * pInterpreted)
{
    int i = 0;

    if (pInterpreted->rt_addr2 == 0)
    {
        if (pInterpreted->sa_mc != 0)
        {
            if (pInterpreted->sa_type == API_RT_TYPE_RECEIVE_SA)
            {
                // BC2RT
                printf("BC to RT%d SA%d", pInterpreted->rt_addr, pInterpreted->sa_mc);
            }
            else
            {
                // RT2BC
                printf("RT%d SA%d to BC", pInterpreted->rt_addr, pInterpreted->sa_mc);
            }
        }
        else
        {
            // Mode Code
            printf("RT%d MC%d", pInterpreted->rt_addr, pInterpreted->word_cnt);
        }
    }
    else
    {
        // RT2RT
        printf("RT%d SA%d to RT%d SA%d", pInterpreted->rt_addr2, pInterpreted->sa_mc2,
            pInterpreted->rt_addr, pInterpreted->sa_mc);
    }

    if (pInterpreted->sa_mc != 0)
    {
        if (pInterpreted->word_cnt == 0)
            pInterpreted->word_cnt = 32;
        for (i=0; i<pInterpreted->word_cnt; i++)
        {
            if (i%8 == 0)
                printf("\n");
            printf("0x%04X, ", pInterpreted->buffer[i]);
        }
    }
    printf("\n");

    if (pInterpreted->msg_trw & 0xF000)
        printf("Error: %x\n", pInterpreted->msg_trw & 0xF000);

    printf("\n");
    return 0;
}

/* This function returns a 1 based logical BIU. Return values are 1 thgrough 8 */
AiUInt32 MilUtilBMGetLogicalBiuFromStream(AiUInt32 ulModHandle, AiUInt32 ulStream, AiUInt32 ulHsBiu)
{
    AiUInt32 streams;
    AiUInt32 protocol;
    AiUInt32 ulBoardInfoCount;

    ApiCmdSysGetBoardInfo(ulModHandle, TY_BOARD_INFO_CHANNEL_COUNT, 1, &streams, &ulBoardInfoCount);
    ApiCmdSysGetBoardInfo(ulModHandle, TY_BOARD_INFO_PROTOCOL, 1, &protocol, &ulBoardInfoCount);

    if (protocol == API_PROTOCOL_1553)
    {
        /* On 1553 boards stream is equal to logical BIU */
        return ulStream;
    }
    else if ((protocol == API_PROTOCOL_3910)
        || (protocol == API_PROTOCOL_EFEX))
    {
        /* We may have up to two HS streams with 1 LS BIU and 1 HS BIU each */
        if (ulStream == 1)
            return ulHsBiu ? 2 : 1;
        else
            return ulHsBiu ? 4 : 3;
    }
    else if ((protocol == API_PROTOCOL_1553_3910)
        || (protocol == API_PROTOCOL_1553_EFEX))
    {
        /* We may have 1, 2 or 4 LS Streams and one HS stream */
        /* chns=2,3,5*/

        if (ulStream < streams)
            /* LS stream return stream as logical BIU */
            return ulStream;

        return ulHsBiu ? ulStream + 1 : ulStream;
    }


    return ulStream;
}

static void MilUtilBMPrintCW(AiUInt16 cw)
{
    AiUInt32 rt = ((cw >> 11) & 0x1F);
    AiUInt32 tx = ((cw >> 10) & 0x1);
    AiUInt32 sa_mc = ((cw >> 5) & 0x1F);
    AiUInt32 wc = ((cw)& 0x1F);

    if (cw == 0)
    {
        printf("N/A");
    }
    else
    {
        // C <RT>_<T/R>_<SA>_<WC>
        printf("C%02d_%c_%02d_%02d", rt, tx == 0 ? 'R' : 'T', sa_mc, wc);
    }
}

static void MilUtilBMPrintSW(AiUInt16 sw)
{
    AiUInt32 rt = ((sw >> 11) & 0x1F);
    AiUInt32 tx = ((sw >> 10) & 0x1);
    AiUInt32 sa_mc = ((sw >> 5) & 0x1F);
    AiUInt32 wc = ((sw)& 0x1F);

    // C <RT>_<T/R>_<SA>_<WC>
    printf("S%02d", rt);
    if ((sw & 0x7FF) == 0) printf("_CS");
    if ((sw & 1 << 0) != 0) printf("_TF");
    if ((sw & 1 << 1) != 0) printf("_DBCA");
    if ((sw & 1 << 2) != 0) printf("_SSF");
    if ((sw & 1 << 3) != 0) printf("_BUSY");
    if ((sw & 1 << 4) != 0) printf("_BCR");
    if ((sw & 1 << 5) != 0) printf("_RES1");
    if ((sw & 1 << 6) != 0) printf("_RES2");
    if ((sw & 1 << 7) != 0) printf("_RES3");
    if ((sw & 1 << 8) != 0) printf("_SRQ");
    if ((sw & 1 << 9) != 0) printf("_INST");
    if ((sw & 1 << 10) != 0) printf("_ME");
}

void MilUtilBMPrintError(AiUInt16 err)
{
    if ((err & 1 << 15) != 0) printf("E");
    if ((err & 1 << 14) != 0) printf("_AlternateBus");
    if ((err & 1 << 13) != 0) printf("_WordCountLow");
    if ((err & 1 << 12) != 0) printf("_WordCountHigh");
    if ((err & 1 << 11) != 0) printf("_SW");
    if ((err & 1 << 10) != 0) printf("_Address");
    if ((err & 1 << 9) != 0) printf("_EarlyResp");
    if ((err & 1 << 8) != 0) printf("_IllegalCW");
    if ((err & 1 << 7) != 0) printf("_BothBusses");
    if ((err & 1 << 6) != 0) printf("_Gap");
    if ((err & 1 << 5) != 0) printf("_InvSync");
    if ((err & 1 << 4) != 0) printf("_Parity");
    if ((err & 1 << 3) != 0) printf("_BitCountLow");
    if ((err & 1 << 2) != 0) printf("_BitCountHigh");
    if ((err & 1 << 1) != 0) printf("_Mancheser");
    if ((err & 1 << 0) != 0) printf("_NoResp");
}


AiReturn MilUtilBMLsShowData(AiUInt32 ulModHandle, MIL_UTIL_BM_ITERATOR * pIterator)
{
    AiUInt32 ulIndex = 0;
    AiUInt8  ucBMEntryType = 0;
    AiUInt32 ulBMEntryData = 0;
    AiUInt32 ulBufferIndex = 0;
    AiUInt32 ulCurrentEntry = 0;
    AiReturn ret = API_OK;

    if (pIterator->iterator_position >= pIterator->buffer_end)
        return API_ERR_QUEUE_EMPTY;

    if ((pIterator->iterator_position + 3) > pIterator->buffer_end)
    {
        fprintf(stderr, "Error: %s Not enough data\n", __FUNCTION__);
        return API_ERR;
    }

    do
    {
        /* Interpret BM entry */
        ulCurrentEntry = BSWAP32(*pIterator->iterator_position);
        ucBMEntryType = (AiUInt8)(ulCurrentEntry >> 28); /* Bits 28 .. 31 */
        ulBMEntryData = (AiUInt32)(ulCurrentEntry & 0x3ffffff); /* Bits  0 .. 26 */

        // -- Message data interpretation ---

        switch (ucBMEntryType)
        {
        case 0x1: /* Error Word           */
            printf("ERROR:   0x%04X (", ulBMEntryData & 0xFFFF);
            MilUtilBMPrintError(ulBMEntryData);
            printf(")\n");
            break;
        case 0x2: /* Time Tag Low         */
            printf("TTLOW:   0x%08X (%02d.%06ds) \n", ulBMEntryData, (ulBMEntryData >> 20) & 0x3F, ulBMEntryData & 0xFFFFF);
            break;
        case 0x3: /* Time Tag High        */
            printf("---------------\n");
            printf("TTHIGH:  0x%08X (%03dd:%02dh:%02dm)\n", ulBMEntryData, (ulBMEntryData >> 11) & 0x1FF, (ulBMEntryData >> 6) & 0x1F, ulBMEntryData & 0x3F);
            break;
        case 0x8: /* Command Word   (Pri) */
            printf("CW1_PRI: 0x%4X (", ulBMEntryData & 0xFFFF);
            MilUtilBMPrintCW(ulBMEntryData);
            printf(")\n");
            break;
        case 0x9: /* Command Word 2 (Pri) */
            printf("CW2_PRI: 0x%4X (", ulBMEntryData & 0xFFFF);
            MilUtilBMPrintCW(ulBMEntryData);
            printf(")\n");
            break;
        case 0xa: /* Data Word      (Pri) */
            printf("DW_PRI:  0x%04X (%4.2f us)\n", ulBMEntryData & 0xFFFF, ((ulBMEntryData >> 16) & 0x1FF) * 0.25);
            break;
        case 0xb: /* Status Word    (Pri) */
            printf("SW_PRI:  0x%04X (", ulBMEntryData & 0xFFFF);
            MilUtilBMPrintSW(ulBMEntryData);
            printf(")\n");
            break;
        case 0xc: /* Command Word   (Sec) */
            printf("CW1_SEC: 0x%4X (", ulBMEntryData & 0xFFFF);
            MilUtilBMPrintCW(ulBMEntryData);
            printf(")\n");
            break;
        case 0xd: /* Command Word 2 (Sec) */
            printf("CW2_SEC: 0x%4X (", ulBMEntryData & 0xFFFF);
            MilUtilBMPrintCW(ulBMEntryData);
            printf(")\n");
            break;
        case 0xe: /* Data Word      (Sec) */
            printf("DW_SEC:  0x%04X (%4.2f us)\n", ulBMEntryData & 0xFFFF, ((ulBMEntryData >> 16) & 0x1FF) * 0.25);
            break;
        case 0xf: /* Status Word    (Sec) */
            printf("SW_SEC:  0x%04X (", ulBMEntryData & 0xFFFF);
            MilUtilBMPrintSW(ulBMEntryData);
            printf(")\n");
            break;
        case 0x0: /* Entry not updated    */
            printf("INVALID: 0x%08X\n", ulBMEntryData);
        default: /* Reserved 4..7        */
            /* Ignored */
            break;
        }

        pIterator->iterator_position++;

        if (pIterator->iterator_position >= pIterator->buffer_end)
            break;

    } while (ret == API_OK);

    // -- clean up ---- 

    return ret;
}

AiReturn MilUtilBMLsSaveInterpretedData(AiUInt32 ulModHandle, FILE* pFileBm, TY_API_QUEUE_BUF_EX* pInterpreted)
{

    fprintf(pFileBm, "----------------------------------\n");
    int i = 0;
    fprintf(pFileBm,"TT: %03dd:%02dh:%02dm %02d.%06ds ", (pInterpreted->ttag_hi >> 11) & 0x1FF, (pInterpreted->ttag_hi >> 6) & 0x1F, pInterpreted->ttag_hi & 0x3F,
                                                  (pInterpreted->ttag_lo >> 20) & 0x3F, pInterpreted->ttag_lo & 0xFFFFF);

    if (pInterpreted->rt_addr2 == 0)
    {
        if (pInterpreted->sa_mc != 0)
        {
            if (pInterpreted->sa_type == API_RT_TYPE_RECEIVE_SA)
            {
                // BC2RT
                fprintf(pFileBm, "BC to RT%d SA%d", pInterpreted->rt_addr, pInterpreted->sa_mc);
            }
            else
            {
                // RT2BC
                fprintf(pFileBm, "RT%d SA%d to BC", pInterpreted->rt_addr, pInterpreted->sa_mc);
            }
        }
        else
        {
            // Mode Code
            fprintf(pFileBm, "RT%d MC%d", pInterpreted->rt_addr, pInterpreted->word_cnt);
        }
    }
    else
    {
        // RT2RT
        fprintf(pFileBm, "RT%d SA%d to RT%d SA%d", pInterpreted->rt_addr2, pInterpreted->sa_mc2,
            pInterpreted->rt_addr, pInterpreted->sa_mc);
    }

    if (pInterpreted->sa_mc != 0)
    {
        if (pInterpreted->word_cnt == 0)
            pInterpreted->word_cnt = 32;
        for (i = 0; i < pInterpreted->word_cnt; i++)
        {
            if (i % 8 == 0)
            {
                fprintf(pFileBm, "\n");
            }
            fprintf(pFileBm, "0x%04X, ", pInterpreted->buffer[i]);
        }
    }
    fprintf(pFileBm, "\n");

    if (pInterpreted->msg_trw & 0xF000)
        fprintf(pFileBm, "Error: %x\n", pInterpreted->msg_trw & 0xF000);

    fprintf(pFileBm, "\n");
    return 0;

}