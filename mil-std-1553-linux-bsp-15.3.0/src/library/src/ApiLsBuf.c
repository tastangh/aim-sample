/* SPDX-FileCopyrightText: 2001-2022 AIM GmbH <info@aim-online.com> */
/* SPDX-License-Identifier: MIT */

/*! \file ApiLsBuf.c
 *
 * LS buffer functions
 * 
 */

#include "AiOs.h"
#include "Ai_cdef.h"
#ifdef _API3910
#include "Ai3910i_def.h"
#include "Ai3910i_fnc.h"
#include "Ai1553_Internal_def.h"
#include "Ai3910_Internal_def.h"
#else
#include "Ai1553i_def.h"
#include "Ai1553i_fnc.h"
#include "Ai1553_Internal_def.h"
#endif
#include "BWTS_API.H"
#include "DEFS_V.H"
#include "Dll_Fnc.h"
#include "AiMapDllToTsw.h"


extern AiInt16 _ApiCmdSetSysDbgLevel(AiUInt32 bModule, AiUInt32 ul_DbgLevel);


//***************************************************************************
//***
//***            A P I 1 5 5 3 - D S    F U N C T I O N S
//***
//***************************************************************************



//***************************************************************************
//


//   Module : APC_API                  Submodule : ApiCmdBufWrite
//
//   Author : Michael Stahl            Project   : API1553-DS
//
//---------------------------------------------------------------------------
//   Create : 01.07.99   Update : 01.07.99
//---------------------------------------------------------------------------
//    Descriptions
//    ------------
//    Inputs    : Buffer Type (BC/RT) [bt]
//                Buffer Header ID [hid]
//                Buffer ID [bid]
//                Dataword position [d_pos]
//                Byte position [b_pos]
//                Bit Lenght in Byte [b_len]
//                Data word [data]
//
//    Outputs   : Instruction execution status [status],
//
//    Description :
//    This function applies the 'API_BUF_WRITE' instruction to the APC1553-DS
//    ASP Driver SW to write data byte/words to the Global RAM.
//
//***************************************************************************

AiInt16 ApiCmdBufWrite_(AiUInt32 bModule, AiUInt8 biu, AiUInt8 bt, AiUInt16 hid, AiUInt16 bid, AiUInt8 data_pos, 
		                AiUInt8 bit_pos, AiUInt8 bit_len, AiUInt16 data)
{
	AiInt16 uw_RetVal = API_OK;
	TY_MIL_COM_LS_BUF_WRITE_INPUT  xInput;
	TY_MIL_COM_ACK                 xOutput;

	MIL_COM_INIT( &xInput.cmd, API_STREAM(bModule), 0, MIL_COM_LS_BUF_WRITE, sizeof(xInput), sizeof(xOutput) );

	xInput.bt       = bt;
	xInput.hid      = hid;
	xInput.bid      = bid;
	xInput.word_pos = data_pos;
	xInput.bit_pos  = bit_pos;
	xInput.bit_len  = bit_len;
	xInput.data     = data;

	uw_RetVal = API_CMD_STRUCT_LS_BUF_WRITE( bModule, &xInput, &xOutput );

	return uw_RetVal;
}

AI_LIB_FUNC AiReturn AI_CALL_CONV ApiCmdBufWrite(AiUInt32 bModule, AiUInt8 biu, AiUInt8 bt, AiUInt16 hid,
                                                 AiUInt16 bid, AiUInt8 data_pos, AiUInt8 bit_pos, AiUInt8 bit_len,
                                                 AiUInt16 data)
{
    AiInt16 uw_RetVal = API_OK;
    
    if( (0 == (bModule & API_STREAM_MASK)) && ((biu < MIN_BIU) || (biu > MAX_BIU)) )
        uw_RetVal = API_ERR_WRONG_BIU;
    else if( (0 == bt) || (bt > MAX_MSG_BUF) )
        uw_RetVal = API_ERR_PARAM3_NOT_IN_RANGE;
    else if( hid > MAX_HEADER_ID  )
        uw_RetVal = API_ERR_PARAM4_NOT_IN_RANGE;
    else if( data_pos > MAX_DATA_WORDPOS )
        uw_RetVal = API_ERR_PARAM6_NOT_IN_RANGE;
    else if( bit_pos > MAX_DATA_BITPOS )
        uw_RetVal = API_ERR_PARAM7_NOT_IN_RANGE;
    else if( (bit_len < MIN_DATA_BITLEN) || (bit_len > MAX_DATA_BITLEN) )
        uw_RetVal = API_ERR_PARAM8_NOT_IN_RANGE;
    else
    {
        uw_RetVal = ApiCmdBufWrite_(bModule, biu, bt, hid, bid, data_pos, bit_pos, bit_len, data);
    }

    v_ExamineRetVal( "ApiCmdBufWrite", uw_RetVal );
    TRACE_BEGIN
    TRACE_FCTA("ApiCmdBufWrite", uw_RetVal); 
    TRACE_PARA(bModule);
    TRACE_PARA(biu);
    TRACE_PARA(bt);
    TRACE_PARA(hid);
    TRACE_PARA(bid);
    TRACE_PARA(data_pos);
    TRACE_PARA(bit_pos);
    TRACE_PARA(bit_len);
    TRACE_PARE(data);
    TRACE_FCTE;
    TRACE_END

    return uw_RetVal;
}



//***************************************************************************
//    Descriptions
//    Update the milbus words in the buffer queue
//    ------------
//    Inputs    : Buffer Type (BC/RT) [bt]
//                Buffer Header ID [hid]
//                Buffer ID [bid]
//                Buffer Offset in Words [offset]
//                Buffer Size in Words [len]
//                Buffer Data [data]
//
//    Outputs   : Instruction execution status [status]
//
//***************************************************************************

AiInt16 ApiCmdBufWriteBlock_(AiUInt32 bModule, AiUInt8 biu, AiUInt8 bt, AiUInt16 hid, AiUInt16 bid, AiUInt8 offset, AiUInt8 len, AiUInt16* data)
{
    int i = 0;
    AiInt16 uw_RetVal = API_OK;
    TY_MIL_COM_LS_BUF_WRITE_BLOCK_INPUT   xInput;
    TY_MIL_COM_ACK  xOutput;

    MIL_COM_INIT(&xInput.cmd, API_STREAM(bModule), 0, MIL_COM_LS_BUF_WRITE_BLOCK, sizeof(xInput), sizeof(xOutput));

    xInput.cmd.ulSwapControl = (len-offset) << 16;

    xInput.bt = bt;
    xInput.hid = hid;
    xInput.bid = bid;
    xInput.offset = offset;
    xInput.count = len;

    for (i = 0; i < len; i++)
        xInput.data[i] = data[i];

    uw_RetVal = API_CMD_STRUCT_LS_BUF_WRITE_BLOCK(bModule, &xInput, &xOutput);

    return uw_RetVal;
}

AI_LIB_FUNC AiReturn AI_CALL_CONV ApiCmdBufWriteBlock(AiUInt32 bModule, AiUInt8 biu, AiUInt8 bt, AiUInt16 hid,
    AiUInt16 bid, AiUInt8 offset, AiUInt8 len, AiUInt16* data)
{
    int i = 0;
    AiInt16 uw_RetVal = API_OK;

    if ((0 == (bModule & API_STREAM_MASK)) && ((biu < MIN_BIU) || (biu > MAX_BIU)))
        uw_RetVal = API_ERR_WRONG_BIU;
    else if ((0 == bt) || (bt > MAX_MSG_BUF))
        uw_RetVal = API_ERR_PARAM3_NOT_IN_RANGE;
    else if (hid > MAX_HEADER_ID)
        uw_RetVal = API_ERR_PARAM4_NOT_IN_RANGE;
    else if ((len < MIN_DATA_WORDS) || ((offset+len) > MAX_DATA_WORDS))
        uw_RetVal = API_ERR_PARAM6_NOT_IN_RANGE;
    else if (NULL == data)
        uw_RetVal = API_ERR_PARAM7_IS_NULL;
    else
    {
        uw_RetVal = ApiCmdBufWriteBlock_(bModule, biu, bt, hid, bid, offset, len, data);
    }

    v_ExamineRetVal("ApiCmdBufWriteBlock", uw_RetVal);

    TRACE_BEGIN
        TRACE("        AiUInt16 data[32];\n");
    for (i = 0; i < len; i++)
    {
        TRACE1("        data[%02d] = ", i); TRACE2("0x%x;\n", data[i]);
    }
    TRACE_FCTA("ApiCmdBufWriteBlock", uw_RetVal);
    TRACE_PARA(bModule);
    TRACE_PARA(biu);
    TRACE_PARA(bt);
    TRACE_PARA(hid);
    TRACE_PARA(bid);
    TRACE_PARA(offset);
    TRACE_PARA(len);
    TRACE_RPARA("data");
    TRACE_FCTE;
    TRACE_END

        return uw_RetVal;
}



//***************************************************************************
//    Descriptions
//    Update the current buffer queue index in host controlled mode
//    ------------
//    Inputs    : Buffer Type (BC/RT) [bt]
//                Buffer Header ID [hid]
//                Control [con]
//
//    Outputs   : Instruction execution status [status]
//
//***************************************************************************

AiInt16 ApiCmdBufHostControl_(AiUInt32 bModule, AiUInt8 biu, AiUInt8 bt, AiUInt16 hid, AiUInt32 con)
{
    int i = 0;
    AiInt16 uw_RetVal = API_OK;
    TY_MIL_COM_LS_BUF_HOST_CONTROL_INPUT xInput;
    TY_MIL_COM_ACK  xOutput;

    MIL_COM_INIT(&xInput.cmd, API_STREAM(bModule), 0, MIL_COM_LS_BUF_HOST_CONTROL, sizeof(xInput), sizeof(xOutput));

    xInput.bt = bt;
    xInput.hid = hid;
    xInput.con = con;

    uw_RetVal = API_CMD_STRUCT_LS_BUF_HOST_CONTROL(bModule, &xInput, &xOutput);

    return uw_RetVal;
}

AI_LIB_FUNC AiReturn AI_CALL_CONV ApiCmdBufHostControl(AiUInt32 bModule, AiUInt8 biu, AiUInt8 bt, AiUInt16 hid, AiUInt32 con)
{
    int i = 0;
    AiInt16 uw_RetVal = API_OK;

    if ((0 == (bModule & API_STREAM_MASK)) && ((biu < MIN_BIU) || (biu > MAX_BIU)))
        uw_RetVal = API_ERR_WRONG_BIU;
    else if ((0 == bt) || (bt > MAX_MSG_BUF))
        uw_RetVal = API_ERR_PARAM3_NOT_IN_RANGE;
    else if (hid > MAX_HEADER_ID)
        uw_RetVal = API_ERR_PARAM4_NOT_IN_RANGE;
    else
    {
        uw_RetVal = ApiCmdBufHostControl_(bModule, biu, bt, hid, con);
    }

    v_ExamineRetVal("ApiCmdBufHostControl", uw_RetVal);

    TRACE_BEGIN
    TRACE_FCTA("ApiCmdBufHostControl", uw_RetVal);
    TRACE_PARA(bModule);
    TRACE_PARA(biu);
    TRACE_PARA(bt);
    TRACE_PARA(hid);
    TRACE_PARA(con);
    TRACE_END

        return uw_RetVal;
}


//***************************************************************************
//
//   Module : APC_API                  Submodule : ApiCmdBufDef
//
//   Author : Marco Motz               Project   : API1553-DS
//
//---------------------------------------------------------------------------
//   Create : 01.06.98   Update : 01.06.98
//---------------------------------------------------------------------------
//    Descriptions
//    ------------
//    Inputs    : Buffer Type (BC/RT) [bt]
//                Buffer Header ID [hid]
//                Buffer ID [bid]
//                Buffer Size in Words [len]
//                Buffer Data [data]
//
//    Outputs   : Instruction execution status [status]
//                Returning Buffer ID [rid]
//                Returning Buffer Address [raddr]
//
//    Description :
//    This function applies the 'TG_API_BUF_DEF' instruction to the APC1553-DS
//    ASP Driver SW to write data words to the selected Data Buffer.
//
//***************************************************************************

AiInt16 ApiCmdBufDef_(AiUInt32 bModule, AiUInt8 biu, AiUInt8 bt, AiUInt16 hid, AiUInt16 bid, AiUInt8 len, AiUInt16 *data,
                      AiUInt16 *rid, AiUInt32 *raddr)
{
    int i = 0;
    AiInt16 uw_RetVal = API_OK;
    TY_MIL_COM_LS_BUF_DEF_INPUT   xInput;
    TY_MIL_COM_LS_BUF_DEF_OUTPUT  xOutput;

    MIL_COM_INIT( &xInput.cmd, API_STREAM(bModule), 0, MIL_COM_LS_BUF_DEF, sizeof(xInput), sizeof(xOutput) );

    xInput.cmd.ulSwapControl = len << 16;

    xInput.bt       = bt;
    xInput.hid      = hid;
    xInput.bid      = bid;
    xInput.count    = len;

    for( i=0; i<len; i++ )
    	xInput.data[i]   = data[i];

    uw_RetVal = API_CMD_STRUCT_LS_BUF_DEF( bModule, &xInput, &xOutput );


    if( rid   ) *rid   = xOutput.bid;
    if( raddr ) *raddr = xOutput.address;

    return uw_RetVal;
}

AI_LIB_FUNC AiReturn AI_CALL_CONV ApiCmdBufDef(AiUInt32 bModule, AiUInt8 biu, AiUInt8 bt, AiUInt16 hid,
                                               AiUInt16 bid, AiUInt8 len, AiUInt16 *data,
                                               AiUInt16 *rid, AiUInt32 *raddr)
{
    int i = 0;
    AiInt16 uw_RetVal = API_OK;

    if( (0 == (bModule & API_STREAM_MASK)) && ((biu < MIN_BIU) || (biu > MAX_BIU)) )
        uw_RetVal = API_ERR_WRONG_BIU;
    else if( (0 == bt) || (bt > MAX_MSG_BUF)  )
        uw_RetVal = API_ERR_PARAM3_NOT_IN_RANGE;
    else if( hid > MAX_HEADER_ID  )
        uw_RetVal = API_ERR_PARAM4_NOT_IN_RANGE;
    else if( (len < MIN_DATA_WORDS) || (len > MAX_DATA_WORDS) )
        uw_RetVal = API_ERR_PARAM6_NOT_IN_RANGE;
    else if( NULL == data )
        uw_RetVal = API_ERR_PARAM7_IS_NULL;
    else if( NULL == rid )
        uw_RetVal = API_ERR_PARAM8_IS_NULL;
    else if( NULL == raddr )
        uw_RetVal = API_ERR_PARAM9_IS_NULL;
    else
    {
        uw_RetVal = ApiCmdBufDef_(bModule, biu, bt, hid, bid, len, data, rid, raddr);
    }
    
    v_ExamineRetVal( "ApiCmdBufDef", uw_RetVal );
    
    TRACE_BEGIN
    TRACE("        AiUInt16 data[32];\n");
    TRACE("        AiUInt16 rid;\n");
    TRACE("        AiUInt32 raddr;\n");
    for(i=0; i<len; i++)
    {
    	TRACE1("        data[%02d] = ", i); TRACE2("0x%x;\n", data[i]);
    }
    TRACE_FCTA("ApiCmdBufDef", uw_RetVal); 
    TRACE_PARA(bModule);
    TRACE_PARA(biu);
    TRACE_PARA(bt);
    TRACE_PARA(hid);
    TRACE_PARA(bid);
    TRACE_PARA(len);
    TRACE_RPARA("data");
    TRACE_RPARA("&rid");
    TRACE_RPARE("&raddr");
    TRACE_FCTE;
    if( rid   ) TRACE1("    --> rid   = 0x%x\n", *rid);
    if( raddr ) TRACE1("    --> raddr = 0x%x\n", *raddr);
    TRACE_END

    return uw_RetVal;
}

//***************************************************************************
//
//   Module : APC_API                  Submodule : ApiCmdBufRead
//
//   Author : Marco Motz               Project   : API1553-DS
//
//---------------------------------------------------------------------------
//   Create : 01.06.98   Update : 01.06.98
//---------------------------------------------------------------------------
//    Descriptions
//    ------------
//    Inputs    : Buffer Type (BC/RT) [bt]
//                Buffer Header ID [hid]
//                Buffer ID [bid]
//                Buffer Size in Words [len]
//
//    Outputs   : Instruction execution status [status]
//                Buffer Data [data]
//                Returning Buffer ID [rid]
//                Returning Buffer Address [raddr]
//
//    Description :
//    This function applies the 'TG_API_BUF_READ' instruction to the APC1553-DS
//    ASP Driver SW to read data words from the selected Data Buffer.
//
//***************************************************************************

AiInt16 ApiCmdBufRead_(AiUInt32 bModule, AiUInt8 biu, AiUInt8 bt, AiUInt16 hid, AiUInt16 bid, AiUInt8 len, 
		               AiUInt16 *data, AiUInt16 *rid, AiUInt32 *raddr)
{
	int i = 0;
	AiInt16 uw_RetVal = API_OK;
	TY_MIL_COM_LS_BUF_READ_INPUT   xInput;
	TY_MIL_COM_LS_BUF_READ_OUTPUT  xOutput;

	MIL_COM_INIT( &xInput.cmd, API_STREAM(bModule), 0, MIL_COM_LS_BUF_READ, sizeof(xInput), sizeof(xOutput) );

	xInput.bt       = bt;
	xInput.hid      = hid;
	xInput.bid      = bid;
	xInput.count    = len;

	uw_RetVal = API_CMD_STRUCT_LS_BUF_READ( bModule, &xInput, &xOutput );


	if( rid   ) *rid   = xOutput.bid;
	if( raddr ) *raddr = xOutput.address;

	if( data )
	{
		for( i=0; i<len; i++ )
			data[i] = xOutput.data[i];
	}

	return uw_RetVal;
}

AI_LIB_FUNC AiReturn AI_CALL_CONV ApiCmdBufRead(AiUInt32 bModule, AiUInt8 biu, AiUInt8 bt, AiUInt16 hid,
                                                AiUInt16 bid, AiUInt8 len, AiUInt16 *data,
                                                AiUInt16 *rid, AiUInt32 *raddr)
{
    int i = 0;
    AiInt16 uw_RetVal = API_OK;

    if( (0 == (bModule & API_STREAM_MASK)) && ((biu < MIN_BIU) || (biu > MAX_BIU)) )
        uw_RetVal = API_ERR_WRONG_BIU;
    else if( (0 == bt) || (bt > MAX_MSG_BUF) )
        uw_RetVal = API_ERR_PARAM3_NOT_IN_RANGE;
    else if( hid > MAX_HEADER_ID  )
        uw_RetVal = API_ERR_PARAM4_NOT_IN_RANGE;
    else if( (len < MIN_DATA_WORDS) || (len > MAX_DATA_WORDS) )
        uw_RetVal = API_ERR_PARAM6_NOT_IN_RANGE;
    else if( NULL == data )
        uw_RetVal = API_ERR_PARAM7_IS_NULL;
    else if( NULL == rid )
        uw_RetVal = API_ERR_PARAM8_IS_NULL;
    else if( NULL == raddr )
        uw_RetVal = API_ERR_PARAM9_IS_NULL;
    else
    {
        uw_RetVal = ApiCmdBufRead_(bModule, biu, bt, hid, bid, len, data, rid, raddr);
    }
    
    v_ExamineRetVal( "ApiCmdBufRead", uw_RetVal );
    TRACE_BEGIN
    TRACE("        AiUInt16 data[32];\n");
    TRACE("        AiUInt16 rid;\n");
    TRACE("        AiUInt32 raddr;\n");
    TRACE_FCTA("ApiCmdBufRead", uw_RetVal); 
    TRACE_PARA(bModule);
    TRACE_PARA(biu);
    TRACE_PARA(bt);
    TRACE_PARA(hid);
    TRACE_PARA(bid);
    TRACE_PARA(len);
    TRACE_RPARA("data");
    TRACE_RPARA("&rid");
    TRACE_RPARE("&raddr");
    TRACE_FCTE;
    if( data )
    	for(i=0; i<len; i++)
    	{
    		TRACE1("    --> data[%02d] = ", i); TRACE2("0x%x\n", data[i]);
    	}
    if( rid   ) TRACE1("    --> rid   = 0x%x\n", *rid);
    if( raddr ) TRACE1("    --> raddr = 0x%x\n", *raddr);
    TRACE_END

    return uw_RetVal;
}

//***************************************************************************
//
//   Module : APC_API                  Submodule : ApiCmdBufBWTSMsgLoad
//
//   Author : Marco Motz               Project   : API1553-DS
//
//---------------------------------------------------------------------------
//   Create : 01.02.99   Update : 01.02.99
//---------------------------------------------------------------------------
//    Descriptions
//    ------------
//    Inputs    : Buffer ID [bid]
//                Transfer mode (0=cyclic/1=acyclic) [xfer_mode]
//                Spare [res]
//                Buffer Data [data]
//
//    Outputs   : Instruction execution status [status]
//                Returning status indication [st]
//
//    Description :
//    This function applies the 'TG_API_BUF_BWTS_LD_MSG' instruction to the
//    APC1553-DS ASP Driver SW to write message data to a buffer queue.
//
//***************************************************************************

AiInt16 ApiCmdBufBWTSMsgLoad_(AiUInt32 bModule, AiUInt8 biu, AiUInt16 bid, AiUInt8 xfer_mode, AiUInt8 res, AiUInt16 *data, AiUInt32 *st)
{
	AiInt16 in_lword_size = 0;
	AiUInt8 info[4];
	AiInt16 uw_RetVal = API_OK;

	info[0] = (AiUInt8)(bid >> 8);
	info[1] = (AiUInt8)(bid);
	info[2] = xfer_mode;
	info[3] = res;

	uw_RetVal = ApiIo(bModule, biu, TG_API_BUF_BWTS_LD_MSG, TG_ACK2,
			&info[0], 4, data, 32,
			b_p, &tg_os, w_p, &tg_os,
			st, &in_lword_size);

	return uw_RetVal;
}

AI_LIB_FUNC AiReturn AI_CALL_CONV ApiCmdBufBWTSMsgLoad(AiUInt32 bModule, AiUInt8 biu, AiUInt16 bid,
                                                       AiUInt8 xfer_mode, AiUInt8 res, AiUInt16 *data,
                                                       AiUInt32 *st)
{
    int i = 0;
    AiInt16 uw_RetVal = API_OK;

    if( (0 == (bModule & API_STREAM_MASK)) && ((biu < MIN_BIU) || (biu > MAX_BIU)) )
        uw_RetVal = API_ERR_WRONG_BIU;
    /*  else if( bid > MAX_BUFFER_ID )
    uw_RetVal = API_ERR_PARAM3_NOT_IN_RANGE;*/
    else if( NULL == data )
        uw_RetVal = API_ERR_PARAM6_IS_NULL;
    else if( NULL == st )
    	uw_RetVal = API_ERR_PARAM7_IS_NULL;
    else
    {
    	uw_RetVal = ApiCmdBufBWTSMsgLoad_(bModule, biu, bid, xfer_mode, res, data, st);
    }

    v_ExamineRetVal( "ApiCmdBufBWTSMsgLoad", uw_RetVal );

    TRACE_BEGIN
    TRACE("        AiUInt16 data[32];\n");
    TRACE("        AiUInt32 st;\n");
    for(i=0; i<32; i++)
    {
    	TRACE1("        data[%02d] = ", i); TRACE2("0x%x;\n", data[i]);
    }
    TRACE_FCTA("ApiCmdBufBWTSMsgLoad", uw_RetVal); 
    TRACE_PARA(bModule);
    TRACE_PARA(biu);
    TRACE_PARA(bid);
    TRACE_PARA(xfer_mode);
    TRACE_PARA(res);
    TRACE_RPARA("data");
    TRACE_RPARE("&st");
    TRACE_FCTE;
    TRACE1("    --> st = 0x%x\n", *st);
    TRACE_END

    return uw_RetVal;
}



//***************************************************************************
//
//   Module : APC_API                  Submodule : ApiCmdRamWriteDataset
//
//   Author : Marco Motz               Project   : API1553-DS
//
//---------------------------------------------------------------------------
//   Create : 01.06.98   Update : 01.06.98
//---------------------------------------------------------------------------
//    Descriptions
//    ------------
//    Inputs    : Dataset ID [dsid]
//                Data Words to write [data]
//
//    Outputs   : Instruction execution status [status]
//
//    Description :
//    This function applies the 'TG_API_RAM_DATASET_WR' instruction to the APC1553-DS
//    ASP Driver SW to write the specified data words to the selected Dataset Buffer.
//
//***************************************************************************

AiInt16 ApiCmdRamWriteDataset_(AiUInt32 bModule, AiUInt16 dsid, AiUInt16 *data)
{
    int i = 0;
    AiInt16 uw_RetVal = API_OK;
    TY_MIL_COM_LS_BUF_DATASET_WRITE_INPUT xInput;
    TY_MIL_COM_ACK                        xOutput;

    MIL_COM_INIT( &xInput.cmd, API_STREAM(bModule), 0, MIL_COM_LS_BUF_DATASET_WRITE, sizeof(xInput), sizeof(xOutput) );

    xInput.dsid   = dsid;
    for( i=0; i<32; i++ )
    	xInput.data[i] = data[i];

    uw_RetVal = API_CMD_STRUCT_LS_BUF_DATASET_WRITE( bModule, &xInput, &xOutput );

    return uw_RetVal;
}

AI_LIB_FUNC AiReturn AI_CALL_CONV ApiCmdRamWriteDataset(AiUInt32 bModule, AiUInt16 dsid, AiUInt16 *data)
{
    int i = 0;
    AiInt16 uw_RetVal = API_OK;

    if( dsid > MAX_DATASET_BUF_ID )
    	uw_RetVal = API_ERR_PARAM2_NOT_IN_RANGE;
    else if( NULL == data )
    	uw_RetVal = API_ERR_PARAM3_IS_NULL;
    else
    {
    	uw_RetVal = ApiCmdRamWriteDataset_(bModule, dsid, data);
    }

    v_ExamineRetVal( "ApiCmdRamWriteDataset", uw_RetVal );

    TRACE_BEGIN
    TRACE("        AiUInt32 data[32];\n");
    for(i=0; i<32; i++)
    {
    	TRACE1("        data[%02d] = ", i); TRACE2("0x%x;\n", data[i]);
    }
    TRACE_FCTA("ApiCmdRamWriteDataset", uw_RetVal); 
    TRACE_PARA(bModule);
    TRACE_PARA(dsid);
    TRACE_RPARE("data");
    TRACE_FCTE;
    TRACE_END

    return uw_RetVal;
}

//***************************************************************************
//
//   Module : APC_API                  Submodule : ApiCmdRamReadDataset
//
//   Author : Marco Motz               Project   : API1553-DS
//
//---------------------------------------------------------------------------
//   Create : 01.06.98   Update : 01.06.98
//---------------------------------------------------------------------------
//    Descriptions
//    ------------
//    Inputs    : Dataset ID [dsid]
//
//    Outputs   : Instruction execution status [status]
//                Data Words to read [data]
//
//    Description :
//    This function applies the 'TG_API_RAM_DATASET_RD' instruction to the APC1553-DS
//    ASP Driver SW to read data words from the selected Dataset Buffer.
//
//***************************************************************************

AiInt16 ApiCmdRamReadDataset_(AiUInt32 bModule, AiUInt16 dsid, AiUInt16 *data)
{
    int i = 0;
    AiInt16 uw_RetVal = API_OK;
    TY_MIL_COM_LS_BUF_DATASET_READ_INPUT   xInput;
    TY_MIL_COM_LS_BUF_DATASET_READ_OUTPUT  xOutput;

    MIL_COM_INIT( &xInput.cmd, API_STREAM(bModule), 0, MIL_COM_LS_BUF_DATASET_READ, sizeof(xInput), sizeof(xOutput) );

    xInput.dsid   = dsid;

    uw_RetVal = API_CMD_STRUCT_LS_BUF_DATASET_READ( bModule, &xInput, &xOutput );

    for( i=0; i<32; i++ )
    	data[i] = xOutput.data[i];

    return uw_RetVal;
}

AI_LIB_FUNC AiReturn AI_CALL_CONV ApiCmdRamReadDataset(AiUInt32 bModule, AiUInt16 dsid, AiUInt16 *data)
{
    int i = 0;
    AiInt16 uw_RetVal = API_OK;

    if( dsid > MAX_DATASET_BUF_ID )
    	uw_RetVal = API_ERR_PARAM2_NOT_IN_RANGE;
    else if( NULL == data )
    	uw_RetVal = API_ERR_PARAM3_IS_NULL;
    else
    {
    	uw_RetVal = ApiCmdRamReadDataset_(bModule, dsid, data);
    }

    v_ExamineRetVal( "ApiCmdRamReadDataset", uw_RetVal );
    TRACE_BEGIN
    TRACE("        AiUInt32 data[32];\n");
    TRACE_FCTA("ApiCmdRamReadDataset", uw_RetVal); 
    TRACE_PARA(bModule);
    TRACE_PARA(dsid);
    TRACE_RPARE("data");
    TRACE_FCTE;
    for(i=0; i<32; i++)
    {
    	TRACE1("    --> data[%02d] = ", i); TRACE2("0x%x\n", data[i]);
    }
    TRACE_END

    return uw_RetVal;
}

//***************************************************************************
//
//   Module : APC_API                  Submodule : ApiCmdBufC1760Con
//
//   Author : Marco Motz               Project   : API1553-DS
//
//---------------------------------------------------------------------------
//   Create : 13.10.99   Update : 13.10.99
//---------------------------------------------------------------------------
//    Descriptions
//    ------------
//    Inputs    : tbd
//
//    Outputs   : Instruction execution status [status]
//
//    Description :
//    This function applies the 'TG_API_C1760_CON' instruction to the API1553-DS
//    ASP Driver SW.
//
//***************************************************************************

 AiInt16 ApiCmdBufC1760Con_(AiUInt32 bModule, AiUInt8 biu, TY_API_C1760_CON *pc1760)
{
    AiInt16 i;
    AiInt16 uw_RetVal = API_OK;
    TY_MIL_COM_LS_BUF_C1760_CON_INPUT xInput;
    TY_MIL_COM_ACK                    xOutput;

    MIL_COM_INIT( &xInput.cmd, API_STREAM(bModule), 0, MIL_COM_LS_BUF_C1760_CON, sizeof(xInput), sizeof(xOutput) );

    xInput.cmd.ulSwapControl = 16;

    xInput.mode   = pc1760->mode;
    xInput.buf_id       = pc1760->buf_id;
    xInput.c01       = pc1760->c01;

    for( i=0;  i < MAX_API_C1760_C02; i++)
    	xInput.c02[i] = pc1760->c02[i];

    uw_RetVal = API_CMD_STRUCT_LS_BUF_C1760_CON( bModule, &xInput, &xOutput );


    return uw_RetVal;
}

AI_LIB_FUNC AiReturn AI_CALL_CONV ApiCmdBufC1760Con(AiUInt32 bModule, AiUInt8 biu, TY_API_C1760_CON *pc1760)
{
    AiInt16 i;
    AiInt16 uw_RetVal = API_OK;

    if( (0 == (bModule & API_STREAM_MASK)) && ((biu < MIN_BIU) || (biu > MAX_BIU)) )
        uw_RetVal = API_ERR_WRONG_BIU;
    else if( NULL == pc1760 )
        uw_RetVal = API_ERR_PARAM3_IS_NULL;
    else if( pc1760->mode > API_ENA )
        uw_RetVal = API_ERR_1760_MODE_NOT_IN_RANGE;
    else if( pc1760->c01 > MAX_DATA_WORDPOS )
        uw_RetVal = API_ERR_1760_C01_NOT_IN_RANGE;
    else
    {
        for( i=0; i<16; i++ )
        {
            if( API_OK == uw_RetVal )
            {
                if( (1 == pc1760->c02[i]) || (pc1760->c02[i] > MAX_DATA_WORDPOS) )
                    uw_RetVal = API_ERR_1760_C02_NOT_IN_RANGE;
            }
        }
    }

    if( API_OK == uw_RetVal )
    {
        uw_RetVal = ApiCmdBufC1760Con_(bModule, biu, pc1760);
    }

    v_ExamineRetVal( "ApiCmdBufC1760Con", uw_RetVal );
    TRACE_BEGIN
    TRACE("        TY_API_C1760_CON pc1760;\n");
    TRACE1("        pc1760.buf_id  = 0x%x;\n", pc1760->buf_id);
    TRACE1("        pc1760.c01     = 0x%x;\n", pc1760->c01);
    for(i=0; i<MAX_API_C1760_C02; i++)
    {
    	TRACE1("        pc1760.c02[%02d] = ", i); TRACE2("0x%x;\n", pc1760->c02[i]);
    }
    TRACE1("        pc1760.mode    = 0x%x;\n", pc1760->mode);
    TRACE_FCTA("ApiCmdBufC1760Con", uw_RetVal); 
    TRACE_PARA(bModule);
    TRACE_PARA(biu);
    TRACE_RPARE("&pc1760");
    TRACE_FCTE;
    TRACE_END

    return uw_RetVal;
}

//***************************************************************************
//
//   Module : APC_API                  Submodule : ApiBHModify
//
//   Author : Patrick Giesel           Project   : API1553-DS
//
//---------------------------------------------------------------------------
//   Create : 22.09.05   Update : 
//---------------------------------------------------------------------------
//    Descriptions
//    ------------
//    Inputs    : tbd
//
//    Outputs   : tbd
//
//    Description :
//    This function modifies the buffer header directly on-the-fly.
//
//***************************************************************************

AiInt16 ApiBHModify_(AiUInt32 bModule, TY_API_BH_MODIFY *px_BHInfo)
{
	AiUInt32 ul_Temp;
	AiInt16 uw_RetVal = API_OK;

	uw_RetVal = ApiReadMemData(bModule, API_MEMTYPE_GLOBAL, px_BHInfo->ul_BHAddr, 4, &ul_Temp );
	if( px_BHInfo->ul_BufferQueueMode != 0xFFFFFFFF )
	{
		ul_Temp &= 0xCFFFFFFF;
		ul_Temp |= ((px_BHInfo->ul_BufferQueueMode & 0x00000003) << 28);
	}

	if( px_BHInfo->ul_DataBufferStoreMode != 0xFFFFFFFF )
	{
		ul_Temp &= 0xF3FFFFFF;
		ul_Temp |= ((px_BHInfo->ul_DataBufferStoreMode & 0x00000003) << 26);
	}

	if( px_BHInfo->ul_BufferSizeMode != 0xFFFFFFFF )
	{
		ul_Temp &= 0xFF7FFFFF;
		ul_Temp |= ((px_BHInfo->ul_BufferSizeMode & 0x00000001) << 23);
	}

	if( px_BHInfo->ul_BufferSize != 0xFFFFFFFF )
	{
		ul_Temp &= 0xFF83FFFF;
		ul_Temp |= ((px_BHInfo->ul_BufferSize & 0x0000001F) << 18);
	}

	if( px_BHInfo->ul_BufferQueueSize != 0xFFFFFFFF )
	{
		ul_Temp &= 0xFFFC3FFF;
		ul_Temp |= ((px_BHInfo->ul_BufferQueueSize & 0x0000000F) << 14);
	}

	if( px_BHInfo->ul_StatusQueueSize != 0xFFFFFFFF )
	{
		ul_Temp &= 0xFFFFCFFF;
		ul_Temp |= ((px_BHInfo->ul_StatusQueueSize & 0x00000003) << 12);
	}

	if( px_BHInfo->ul_EventQueueSize != 0xFFFFFFFF )
	{
		ul_Temp &= 0xFFFFF3FF;
		ul_Temp |= ((px_BHInfo->ul_EventQueueSize & 0x00000003) << 10);
	}

	if( px_BHInfo->ul_CurrentBufferIndex != 0xFFFFFFFF )
	{
		ul_Temp &= 0xFFFFFF00;
		ul_Temp |= (px_BHInfo->ul_CurrentBufferIndex & 0x000000FF);
	}

	uw_RetVal = ApiWriteMemData(bModule, API_MEMTYPE_GLOBAL, px_BHInfo->ul_BHAddr, 4, &ul_Temp );

	return uw_RetVal;
}

AI_LIB_FUNC AiReturn AI_CALL_CONV ApiBHModify(AiUInt32 bModule, TY_API_BH_MODIFY *px_BHInfo)
{
    AiInt16 uw_RetVal = API_OK;

    if (NULL == px_BHInfo)
        uw_RetVal = API_ERR_PARAM2_IS_NULL;
    else if( (px_BHInfo->ul_DataBufferStoreMode > MAX_BSM) && (px_BHInfo->ul_DataBufferStoreMode != 0xFFFFFFFF) )
        uw_RetVal = API_ERR_PARAM4_NOT_IN_RANGE;
    else if( px_BHInfo->ul_BufferSizeMode != 0xFFFFFFFF )
        uw_RetVal = API_ERR_PARAM5_NOT_IN_RANGE;
    else if( (px_BHInfo->ul_BufferSize != 0xFFFFFFFF) && (px_BHInfo->ul_BufferSize < MIN_BH_BUF_SIZE) && (px_BHInfo->ul_BufferSize > MAX_BH_BUF_SIZE) )
        uw_RetVal = API_ERR_PARAM6_NOT_IN_RANGE;
    else if( (px_BHInfo->ul_BufferQueueMode > MAX_BQM) && (px_BHInfo->ul_BufferQueueMode != 0xFFFFFFFF) )
        uw_RetVal = API_ERR_PARAM7_NOT_IN_RANGE;
    else if( (px_BHInfo->ul_BufferQueueSize > MAX_QUEUE_SIZE) && (px_BHInfo->ul_BufferQueueSize != 0xFFFFFFFF) )
        uw_RetVal = API_ERR_PARAM8_NOT_IN_RANGE;
    else if( px_BHInfo->ul_StatusQueueSize != 0xFFFFFFFF ) /* not supported yet */
        uw_RetVal = API_ERR_PARAM9_NOT_IN_RANGE;
    else if( px_BHInfo->ul_EventQueueSize != 0xFFFFFFFF ) /* not supported yet */
        uw_RetVal = API_ERR_PARAM10_NOT_IN_RANGE;
    else if( (px_BHInfo->ul_CurrentBufferIndex > 0xFF) && (px_BHInfo->ul_CurrentBufferIndex != 0xFFFFFFFF) )
        uw_RetVal = API_ERR_PARAM11_NOT_IN_RANGE;

    if( API_OK == uw_RetVal )
    {
        uw_RetVal = ApiBHModify_(bModule, px_BHInfo);
    }

    v_ExamineRetVal( "ApiBHModify", uw_RetVal );
    TRACE_BEGIN
    TRACE("        TY_API_BH_MODIFY x_BHInfo;\n");
    TRACE1("        x_BHInfo.ul_BHAddr               = 0x%x;\n", px_BHInfo->ul_BHAddr);
    TRACE1("        x_BHInfo.ul_BufferQueueMode      = 0x%x;\n", px_BHInfo->ul_BufferQueueMode);
    TRACE1("        x_BHInfo.ul_BufferQueueSize      = 0x%x;\n", px_BHInfo->ul_BufferQueueSize);
    TRACE1("        x_BHInfo.ul_BufferSize           = 0x%x;\n", px_BHInfo->ul_BufferSize);
    TRACE1("        x_BHInfo.ul_BufferSizeMode       = 0x%x;\n", px_BHInfo->ul_BufferSizeMode);
    TRACE1("        x_BHInfo.ul_CurrentBufferIndex   = 0x%x;\n", px_BHInfo->ul_CurrentBufferIndex);
    TRACE1("        x_BHInfo.ul_DataBufferStoreMode  = 0x%x;\n", px_BHInfo->ul_DataBufferStoreMode);
    TRACE1("        x_BHInfo.ul_EventQueueSize       = 0x%x;\n", px_BHInfo->ul_EventQueueSize);
    TRACE1("        x_BHInfo.ul_StatusQueueSize      = 0x%x;\n", px_BHInfo->ul_StatusQueueSize);
    TRACE_FCTA("ApiBHModify", uw_RetVal); 
    TRACE_PARA(bModule);
    TRACE_RPARE("&x_BHInfo");
    TRACE_FCTE;
    TRACE_END

    return uw_RetVal;
}

