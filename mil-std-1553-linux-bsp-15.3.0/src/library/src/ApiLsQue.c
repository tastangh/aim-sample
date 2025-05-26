/* SPDX-FileCopyrightText: 2001-2022 AIM GmbH <info@aim-online.com> */
/* SPDX-License-Identifier: MIT */

/*! \file ApiLsQue.c
 *
 * LS queue functions
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
#include "DEFS_V.H"
#include "Dll_Fnc.h"


//***************************************************************************
//***
//***            A P I 1 5 5 3 - D S    F U N C T I O N S
//***
//***************************************************************************

//***************************************************************************
//***       Q U E U E I N G     C O M M A N D S
//***************************************************************************
//***************************************************************************
//
//   Module : APC_API                  Submodule : ApiCmdQueueIni
//
//   Author : Michael Stahl            Project   : API1553-DS
//
//---------------------------------------------------------------------------
//   Create : 10.06.99   Update : 22.06.99
//---------------------------------------------------------------------------
//    Descriptions
//    ------------
//    Inputs    : none
//
//    Outputs   : Instruction execution status [status]
//
//    Description : This function setup the BM in Message Filter Recording mode
//
//***************************************************************************

AiInt16 ApiCmdQueueIni_(AiUInt32 bModule, AiUInt8 biu)
{
	TY_API_BM_CAP_SETUP cap_setup;
	AiInt16 uw_RetVal = API_OK;

	uw_RetVal = ApiCmdBMIni_(bModule, biu);
	if( API_OK == uw_RetVal )
	{
		// Bus Monitor Commands
		cap_setup.cap_mode = 3; // Message Filter Recording mode */
		cap_setup.cap_tat = 0;
		cap_setup.cap_mcc = 0;
		cap_setup.cap_fsize = 0; // 0MB
		uw_RetVal = ApiCmdBMCapMode_(bModule, biu, &cap_setup);
	}

	return uw_RetVal;
}

AI_LIB_FUNC AiReturn AI_CALL_CONV ApiCmdQueueIni(AiUInt32 bModule, AiUInt8 biu)
{
    AiInt16 uw_RetVal = API_OK;

    if( (0 == (bModule & API_STREAM_MASK)) && ((biu < MIN_BIU) || (biu > MAX_BIU)) )
        uw_RetVal = API_ERR_WRONG_BIU;
    else
    {
        uw_RetVal = ApiCmdQueueIni_(bModule, biu);
    }

    v_ExamineRetVal( "ApiCmdQueueIni", uw_RetVal );

    TRACE_BEGIN
    TRACE_FCTA("ApiCmdQueueIni", uw_RetVal); 
    TRACE_PARA(bModule);
    TRACE_PARE(biu);
    TRACE_FCTE;
    TRACE_END

    return uw_RetVal;
}

//***************************************************************************
//
//   Module : APC_API                  Submodule : ApiCmdQueueFlush
//
//   Author : Michael Stahl            Project   : API1553-DS
//
//---------------------------------------------------------------------------
//   Create : 10.06.99   Update : 22.06.99
//---------------------------------------------------------------------------
//    Descriptions
//    ------------
//    Inputs    : none
//
//    Outputs   : Instruction execution status [status]
//
//    Description : This function flushs the BM buffer area
//
//***************************************************************************

AiInt16 ApiCmdQueueFlush_(AiUInt32 bModule, AiUInt8 biu)
{
    AiInt16 uw_RetVal = API_OK;

    uw_RetVal = ApiCmdQueueIni_(bModule, biu);
    if( API_OK == uw_RetVal )
    {
    	uw_RetVal = ApiCmdQueueStart_(bModule, biu);
    }

    return uw_RetVal;
}

AI_LIB_FUNC AiReturn AI_CALL_CONV ApiCmdQueueFlush(AiUInt32 bModule, AiUInt8 biu)
{
	AiInt16 uw_RetVal = API_OK;

	if( (0 == (bModule & API_STREAM_MASK)) && ((biu < MIN_BIU) || (biu > MAX_BIU)) )
		uw_RetVal = API_ERR_WRONG_BIU;
	else
	{
		uw_RetVal = ApiCmdQueueFlush_(bModule, biu);
	}

	v_ExamineRetVal( "ApiCmdQueueFlush", uw_RetVal );

	TRACE_BEGIN
	TRACE_FCTA("ApiCmdQueueFlush", uw_RetVal); 
	TRACE_PARA(bModule);
	TRACE_PARE(biu);
	TRACE_FCTE;
	TRACE_END

	return uw_RetVal;
}

//***************************************************************************
//
//   Module : APC_API                  Submodule : ApiCmdQueueStart
//
//   Author : Michael Stahl            Project   : API1553-DS
//
//---------------------------------------------------------------------------
//   Create : 10.06.99   Update : 10.06.99
//---------------------------------------------------------------------------
//    Descriptions
//    ------------
//    Inputs    : none
//
//    Outputs   : Instruction execution status [status]
//
//    Description : This function starts the queueing of messages
//
//***************************************************************************

AiInt16 ApiCmdQueueStart_(AiUInt32 bModule, AiUInt8 biu)
{
    return ApiCmdBMStart_(bModule, biu);
}

AI_LIB_FUNC AiReturn AI_CALL_CONV ApiCmdQueueStart(AiUInt32 bModule, AiUInt8 biu)
{
	AiInt16 uw_RetVal = API_OK;

	if( (0 == (bModule & API_STREAM_MASK)) && ((biu < MIN_BIU) || (biu > MAX_BIU)) )
		uw_RetVal = API_ERR_WRONG_BIU;
	else
	{
		uw_RetVal = ApiCmdQueueStart_(bModule, biu);
	}
	v_ExamineRetVal( "ApiCmdQueueStart", uw_RetVal );
	TRACE_BEGIN
	TRACE_FCTA("ApiCmdQueueStart", uw_RetVal); 
	TRACE_PARA(bModule);
	TRACE_PARE(biu);
	TRACE_FCTE;
	TRACE_END

	return uw_RetVal;
}

//***************************************************************************
//
//   Module : APC_API                  Submodule : ApiCmdQueueHalt
//
//   Author : Michael Stahl            Project   : API1553-DS
//
//---------------------------------------------------------------------------
//   Create : 10.06.99   Update : 10.06.99
//---------------------------------------------------------------------------
//    Descriptions
//    ------------
//    Inputs    : none
//
//    Outputs   : Instruction execution status [status]
//
//    Description : This function stops the queueing of messages
//
//***************************************************************************

AiInt16 ApiCmdQueueHalt_(AiUInt32 bModule, AiUInt8 biu)
{
	return ApiCmdBMHalt_(bModule, biu);
}

AI_LIB_FUNC AiReturn AI_CALL_CONV ApiCmdQueueHalt(AiUInt32 bModule, AiUInt8 biu)
{
    AiInt16 uw_RetVal = API_OK;

    if( (0 == (bModule & API_STREAM_MASK)) && ((biu < MIN_BIU) || (biu > MAX_BIU)) )
        uw_RetVal = API_ERR_WRONG_BIU;
    else
    {
        uw_RetVal = ApiCmdQueueHalt_(bModule, biu);
    }
    v_ExamineRetVal( "ApiCmdQueueHalt", uw_RetVal );
    TRACE_BEGIN
        TRACE_FCTA("ApiCmdQueueHalt", uw_RetVal); 
    TRACE_PARA(bModule);
    TRACE_PARE(biu);
    TRACE_FCTE;
    TRACE_END
        return(uw_RetVal);
}

//***************************************************************************
//
//   Module : APC_API                  Submodule : ApiCmdQueueStart
//
//   Author : Michael Stahl            Project   : API1553-DS
//
//---------------------------------------------------------------------------
//   Create : 10.06.99   Update : 10.06.99
//---------------------------------------------------------------------------
//    Descriptions
//    ------------
//    Inputs    : none
//
//    Outputs   : Instruction execution status [status]
//
//    Description : This function starts the queueing of messages
//
//***************************************************************************

AiInt16 ApiCmdQueueRead_(AiUInt32 bModule, AiUInt8 biu, TY_API_QUEUE_BUF *qmesg)
{
    AiUInt16 rec_buf[50];
    AiUInt8 ovfl;
    AiUInt32 lWordsRead;
    AiUInt8 rt[2];
    AiUInt8 tr[2];
    AiUInt8 sa_mc[2];
    AiUInt8 w_cnt;
    AiUInt8 sa_type[2];
    AiUInt8 data_cnt;
    AiUInt8 bus_flag;
    AiUInt16 msg_stat;
    AiUInt16 statusword[2];
    AiUInt32 ttag;
    AiUInt32 lval;
    AiUInt8 i;
    AiInt16 uw_RetVal = API_OK;

        uw_RetVal = ApiCmdBMReadMsgFltRec(bModule, biu, 1/*mode*/, 4 /*con*/, 50 /*max_size*/, 1/*max_msg*/,
            rec_buf, &ovfl, &lWordsRead);
        if( API_OK == uw_RetVal )
        {
            lval = rec_buf[6];
            rt[0] = (AiUInt8)((lval & 0xF800) >> 11);
            tr[0] = (AiUInt8)((lval & 0x0400) >> 10);
            sa_mc[0] = (AiUInt8)((lval & 0x03E0) >> 5);
            w_cnt = (AiUInt8)(lval & 0x01F);

            if(tr[0] == 0)
            { // receive subaddress/modecode
                if(sa_mc[0] == 0 || sa_mc[0] == 31)
                {    // Command is Mode Code Command
                    sa_mc[0] = w_cnt; // Mode Code value
                    sa_type[0] = 2;    // receive modecode
                }
                else sa_type[0] = 0; // receive subaddress
            }
            else 
            { // transmitt subaddress/modecode
                if(sa_mc[0] == 0 || sa_mc[0] == 31)
                {    // Command is Mode Code Command
                    sa_mc[0] = w_cnt; // Mode Code value
                    sa_type[0] = 3;    // transmitt modecode
                }
                else sa_type[0] = 1; // transmitt subaddress
            }

            lval = rec_buf[7];
            rt[1] = (AiUInt8)((lval & 0xF800) >> 11);
            tr[1] = (AiUInt8)((lval & 0x0400) >> 10);
            sa_mc[1] = (AiUInt8)((lval & 0x03E0) >> 5);
            w_cnt = (AiUInt8)(lval & 0x01F);

            if(tr[1] == 0)
            { // receive subaddress/modecode
                if(sa_mc[1] == 0 || sa_mc[1] == 31)
                {    // Command is Mode Code Command
                    sa_mc[1] = w_cnt; // Mode Code value
                    sa_type[1] = 2;    // receive modecode
                }
                else sa_type[1] = 0; // receive subaddress
            }
            else 
            { // transmitt subaddress/modecode
                if(sa_mc[1] == 0 || sa_mc[1] == 31)
                {    // Command is Mode Code Command
                    sa_mc[1] = w_cnt; // Mode Code value
                    sa_type[1] = 3;    // transmitt modecode
                }
                else sa_type[1] = 1; // transmitt subaddress
            }

            data_cnt = (AiUInt8) rec_buf[1];
            bus_flag = ((rec_buf[0] & 0x0800) >> 11);
            msg_stat = rec_buf[0];

            ttag = rec_buf[2];    // Time Tag High (Low Word)
            ttag &= 0x0000003F;
            ttag = (ttag << 26);
            lval = rec_buf[5];    // Time Tag Low (High Word)
            lval = (lval << 16);
            lval |= rec_buf[4];  // Time Tag Low (Low Word)
            ttag |= lval;

            statusword[0] = rec_buf[8];
            statusword[1] = rec_buf[9];

            qmesg->rt_addr = rt[0];
            qmesg->sa_mc = sa_mc[0];
            qmesg->sa_type = sa_type[0];
            qmesg->rt_addr2 = rt[1];
            qmesg->sa_mc2 = sa_mc[1];
            qmesg->sa_type2 = sa_type[1];
            qmesg->word_cnt = data_cnt;
            qmesg->rbf_trw = bus_flag;
            qmesg->msg_trw = msg_stat;
            qmesg->status_word1 = statusword[0];
            qmesg->status_word2 = statusword[1];
            for(i=0; i<32; i++)
            {
                qmesg->buffer[i] = rec_buf[10+i];
            }
            qmesg->ttag = ttag;

            if(ovfl == 0 && lWordsRead == 0)
            {
                uw_RetVal = API_ERR_QUEUE_EMPTY; // Queue empty
            }

            if(ovfl == 1 && lWordsRead == 0)
            {
                uw_RetVal = API_ERR_QUEUE_OVERFLOW; // Queue overflow
            }
        }


        return uw_RetVal;
}

AI_LIB_FUNC AiReturn AI_CALL_CONV ApiCmdQueueRead(AiUInt32 bModule, AiUInt8 biu, TY_API_QUEUE_BUF *qmesg)
{
    AiUInt8 i;
    AiInt16 uw_RetVal = API_OK;

    if( (0 == (bModule & API_STREAM_MASK)) && ((biu < MIN_BIU) || (biu > MAX_BIU)) )
        uw_RetVal = API_ERR_WRONG_BIU;
    else if( NULL == qmesg )
        uw_RetVal = API_ERR_PARAM3_IS_NULL;
    else
    {
        uw_RetVal = ApiCmdQueueRead_(bModule, biu, qmesg);
    }

    /* Queue empty is no error, so don't print any error message */
    if(uw_RetVal != API_ERR_QUEUE_EMPTY)
    {
        v_ExamineRetVal( "ApiCmdQueueRead", uw_RetVal );
    }

    TRACE_BEGIN
    TRACE("        TY_API_QUEUE_BUF qmesg;\n");
    TRACE_FCTA("ApiCmdQueueRead", uw_RetVal); 
    TRACE_PARA(bModule);
    TRACE_PARA(biu);
    TRACE_RPARE("&qmesg");
    TRACE_FCTE;
    for(i=0; i<32; i++)
    {
    	TRACE1("    --> qmesg.buffer[%02d]   = ", i); TRACE2("0x%x\n", qmesg->buffer[i]);
    }
    TRACE1("    --> qmesg.msg_trw      = 0x%x\n", qmesg->msg_trw);
    TRACE1("    --> qmesg.rbf_trw      = 0x%x\n", qmesg->rbf_trw);
    TRACE1("    --> qmesg.rt_addr      = 0x%x\n", qmesg->rt_addr);
    TRACE1("    --> qmesg.rt_addr2     = 0x%x\n", qmesg->rt_addr2);
    TRACE1("    --> qmesg.sa_mc        = 0x%x\n", qmesg->sa_mc);
    TRACE1("    --> qmesg.sa_mc2       = 0x%x\n", qmesg->sa_mc2);
    TRACE1("    --> qmesg.sa_type      = 0x%x\n", qmesg->sa_type);
    TRACE1("    --> qmesg.sa_type2     = 0x%x\n", qmesg->sa_type2);
    TRACE1("    --> qmesg.status_word1 = 0x%x\n", qmesg->status_word1);
    TRACE1("    --> qmesg.status_word2 = 0x%x\n", qmesg->status_word2);
    TRACE1("    --> qmesg.ttag         = 0x%x\n", qmesg->ttag);
    TRACE1("    --> qmesg.word_cnt     = 0x%x\n", qmesg->word_cnt);
    TRACE_END

    return uw_RetVal;
}
