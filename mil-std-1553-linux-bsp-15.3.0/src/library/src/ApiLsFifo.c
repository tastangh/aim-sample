/* SPDX-FileCopyrightText: 2015-2022 AIM GmbH <info@aim-online.com> */
/* SPDX-License-Identifier: MIT */

/*! \file ApiLsFifo.c
 *
 * LS FIFO functions
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

#include "AiMapDllToTsw.h"

//***************************************************************************
//***
//***            A P I 1 5 5 3 - D S    F U N C T I O N S
//***
//***************************************************************************

//***************************************************************************
//***       F I F O     C O M M A N D S
//***************************************************************************

//***************************************************************************
//
//   Module : APC_API                  Submodule : ApiCmdFifoIni
//
//   Author : Michael Stahl            Project   : API1553-DS
//
//---------------------------------------------------------------------------
//   Create : 22.09.99   Update : 22.09.99
//---------------------------------------------------------------------------
//    Descriptions
//    ------------
//    Inputs    : Amount of Fifo's in the Queue  [fifo_nbr]
//                        Amount of Buffer's in one Fifo [buf_nbr]
//
//    Outputs   : Instruction execution status [status]
//
//    Description :
//    This function applies the 'API_INI_FIFO' instruction to the API1553-DS
//    ASP Driver SW to init the buffer pool in the shared RAM.
//
//***************************************************************************

AiInt16 ApiCmdFifoIni_(AiUInt32 bModule, AiUInt8 biu, AiUInt8 fifo_nbr, AiUInt16 buf_nbr)
{
    AiInt16 uw_RetVal = API_OK;
    TY_MIL_COM_SYS_FIFO_INI_INPUT   xInput;
    TY_MIL_COM_ACK                  xOutput;

    MIL_COM_INIT( &xInput.cmd, API_STREAM(bModule), 0, MIL_COM_SYS_FIFO_INI, sizeof(xInput), sizeof(xOutput) );

    xInput.f_nbr = fifo_nbr;
    xInput.b_nbr = buf_nbr;

    uw_RetVal = API_CMD_STRUCT_SYS_FIFO_INI( bModule, &xInput, &xOutput );
    
    return uw_RetVal;
}

AI_LIB_FUNC AiReturn AI_CALL_CONV ApiCmdFifoIni(AiUInt32 bModule, AiUInt8 biu, AiUInt8 fifo_nbr, AiUInt16 buf_nbr)
{
    AiInt16 uw_RetVal = API_OK;

    if( (0 == (bModule & API_STREAM_MASK)) && ((biu < MIN_BIU) || (biu > MAX_BIU)) )
        uw_RetVal = API_ERR_WRONG_BIU;
    else if( (fifo_nbr < MIN_FIFO_ID) || (fifo_nbr > MAX_FIFO_ID) )
        uw_RetVal = API_ERR_PARAM3_NOT_IN_RANGE;
    else if( (buf_nbr < MIN_BUF_NB_IN_FIFO) || (buf_nbr > MAX_BUF_NB_IN_FIFO) )
        uw_RetVal = API_ERR_PARAM4_NOT_IN_RANGE;
    else
    {
        uw_RetVal = ApiCmdFifoIni_(bModule, biu, fifo_nbr, buf_nbr);
    }
    
    v_ExamineRetVal( "ApiCmdFifoIni", uw_RetVal );
    
    TRACE_BEGIN
    TRACE_FCTA("ApiCmdFifoIni", uw_RetVal); 
    TRACE_PARA(bModule);
    TRACE_PARA(biu);
    TRACE_PARA(fifo_nbr);
    TRACE_PARE(buf_nbr);
    TRACE_FCTE;
    TRACE_END
    
    return uw_RetVal;
}

//***************************************************************************
//
//   Module : APC_API                  Submodule : ApiCmdBCAssignFifo
//

//   Author : Michael Stahl            Project   : API1553-DS
//
//---------------------------------------------------------------------------
//   Create : 22.09.99   Update : 22.09.99
//---------------------------------------------------------------------------
//    Descriptions
//    ------------
//    Inputs    : BC Fifo Tagging Control (Ena/Dis) [con],
//                Fifo Id                                       [f_id],
//                Transfer Id                               [xid]
//
//    Outputs   : Instruction execution status [status]
//
//    Description :
//    This function applies the 'TG_API_FIFO_BC_ASSIGN' instruction to the API1553-DS
//    ASP Driver SW to assign the specify fifo with the specified BC transfer.
//
//***************************************************************************

AiInt16 ApiCmdBCAssignFifo_(AiUInt32 bModule, AiUInt8 biu, AiUInt8 con, AiUInt8 f_id, AiUInt16 xid)
{
    TY_MIL_COM_SYS_FIFO_BC_ASSIGN_INPUT xInput;
    TY_MIL_COM_ACK                      xOutput;
    AiInt16 uw_RetVal = API_OK;

    MIL_COM_INIT( &xInput.cmd, API_STREAM(bModule), 0, MIL_COM_SYS_FIFO_BC_ASSIGN, sizeof(xInput), sizeof(xOutput) );

    xInput.con  = con;
    xInput.f_id = f_id;
    xInput.xid  = xid;

    uw_RetVal = API_CMD_STRUCT_SYS_FIFO_BC_ASSIGN( bModule, &xInput, &xOutput );

    return uw_RetVal;
}

AI_LIB_FUNC AiReturn AI_CALL_CONV ApiCmdBCAssignFifo(AiUInt32 bModule, AiUInt8 biu, AiUInt8 con, AiUInt8 f_id, AiUInt16 xid)
{
    AiInt16 uw_RetVal = API_OK;

    if( (0 == (bModule & API_STREAM_MASK)) && ((biu < MIN_BIU) || (biu > MAX_BIU)) )
        return API_ERR_WRONG_BIU;
    if( con > API_ENA )
        return API_ERR_PARAM3_NOT_IN_RANGE;
    if( (f_id < MIN_FIFO_ID) || (f_id > MAX_FIFO_ID) )
        return API_ERR_PARAM4_NOT_IN_RANGE;
    if( (xid < MIN_XFER_ID) || (xid > MAX_XFER_ID) )
        return API_ERR_PARAM5_NOT_IN_RANGE;

    uw_RetVal = ApiCmdBCAssignFifo_(bModule, biu, con, f_id, xid);

    v_ExamineRetVal( "ApiCmdBCAssignFifo", uw_RetVal );
    TRACE_BEGIN
    TRACE_FCTA("ApiCmdBCAssignFifo", uw_RetVal); 
    TRACE_PARA(bModule);
    TRACE_PARA(biu);
    TRACE_PARA(con);
    TRACE_PARA(f_id);
    TRACE_PARE(xid);
    TRACE_FCTE;
    TRACE_END

    return uw_RetVal;
}

//***************************************************************************
//
//   Module : APC_API                  Submodule : ApiCmdRTSAAssignFifo
//
//   Author : Michael Stahl            Project   : API1553-DS
//
//---------------------------------------------------------------------------
//   Create : 22.09.99   Update : 22.09.99
//---------------------------------------------------------------------------
//    Descriptions
//    ------------
//    Inputs    : RT Fifo Tagging Control (Ena/Dis) [con],
//                        Fifo Id [f_id],
//                RT Address [rt],
//                        Subaddress {sa]
//
//    Outputs   : Instruction execution status [status]
//
//    Description :
//    This function applies the 'API_FIFO_RTSA_ASSIGN' instruction to the API1553-DS
//    ASP Driver SW to assign the specify Fifo with specified RT/SA.
//
//***************************************************************************

AiInt16 ApiCmdRTSAAssignFifo_(AiUInt32 bModule, AiUInt8 biu, AiUInt8 con, AiUInt8 f_id, AiUInt8 rt, AiUInt8 sa)
{
    AiInt16 uw_RetVal = API_OK;
    TY_MIL_COM_SYS_FIFO_RT_ASSIGN_INPUT xInput;
    TY_MIL_COM_ACK                      xOutput;

    MIL_COM_INIT( &xInput.cmd, API_STREAM(bModule), 0, MIL_COM_SYS_FIFO_RT_ASSIGN, sizeof(xInput), sizeof(xOutput) );

    xInput.con  = con;
    xInput.f_id = f_id;
    xInput.rt   = rt;
    xInput.sa   = sa;

    uw_RetVal = API_CMD_STRUCT_SYS_FIFO_RT_ASSIGN( bModule, &xInput, &xOutput );

    return uw_RetVal;
}

AI_LIB_FUNC AiReturn AI_CALL_CONV ApiCmdRTSAAssignFifo(AiUInt32 bModule, AiUInt8 biu, AiUInt8 con,
                                                       AiUInt8 f_id, AiUInt8 rt, AiUInt8 sa)
{
    AiInt16 uw_RetVal = API_OK;

    if( (0 == (bModule & API_STREAM_MASK)) && ((biu < MIN_BIU) || (biu > MAX_BIU)) )
        uw_RetVal = API_ERR_WRONG_BIU;
    else if( con > API_ENA )
        uw_RetVal = API_ERR_PARAM3_NOT_IN_RANGE;
    else if( (f_id < MIN_FIFO_ID) || (f_id > MAX_FIFO_ID) )
        uw_RetVal = API_ERR_PARAM4_NOT_IN_RANGE;
    else if( rt > MAX_RT_ADDR )
        uw_RetVal = API_ERR_PARAM5_NOT_IN_RANGE;
    else if( (sa < 1) || (sa > MAX_RT_SA) )
        uw_RetVal = API_ERR_PARAM6_NOT_IN_RANGE;
    else
    {
        uw_RetVal = ApiCmdRTSAAssignFifo_(bModule, biu, con, f_id, rt, sa);
    }

    v_ExamineRetVal( "ApiCmdRTSAAssignFifo", uw_RetVal );

    TRACE_BEGIN
    TRACE_FCTA("ApiCmdRTSAAssignFifo", uw_RetVal); 
    TRACE_PARA(bModule);
    TRACE_PARA(biu);
    TRACE_PARA(con);
    TRACE_PARA(f_id);
    TRACE_PARA(rt);
    TRACE_PARE(sa);
    TRACE_FCTE;
    TRACE_END

    return uw_RetVal;
}

//***************************************************************************
//
//   Module : APC_API                  Submodule : ApiCmdFifoWrite
//
//   Author : Michael Stahl            Project   : API1553-DS
//
//---------------------------------------------------------------------------
//   Create : 22.09.99   Update : 22.09.99
//---------------------------------------------------------------------------
//    Descriptions
//    ------------
//    Inputs    : Fifo Id [f_id],
//                Size in Word's to write [size],
//                        Data to write [data]
//
//    Outputs   : Instruction execution status [status]
//
//    Description :
//    This function applies the 'API_FIFO_BUF_WRITE' instruction to the API1553-DS
//    ASP Driver SW to write reload the buffer's in the specified Fifo.
//
//***************************************************************************
AiInt16 ApiCmdFifoWrite_(AiUInt32 ulModHandle, AiUInt8 biu, AiUInt8 f_id, AiUInt16 size, AiUInt16 *data)
{
    AiUInt32 i = 0;
    AiInt16 wRetVal = API_OK;
    TY_MIL_COM_SYS_FIFO_WRITE_INPUT   * pxInput  = NULL;
    TY_MIL_COM_ACK                      xOutput;

    AiUInt32 ulInputSize  = sizeof(TY_MIL_COM_SYS_FIFO_WRITE_INPUT)  +  size * sizeof(AiUInt16);

    pxInput  = (TY_MIL_COM_SYS_FIFO_WRITE_INPUT*)AiOsMalloc( ulInputSize );

    MIL_COM_INIT( &pxInput->cmd, API_STREAM(ulModHandle), 0, MIL_COM_SYS_FIFO_WRITE, ulInputSize, sizeof(xOutput) );

    pxInput->id    = f_id;
    pxInput->count = size;
        
    for( i=0; i<pxInput->count; i++ )
       pxInput->data[i] = data[i];

    pxInput->cmd.ulSwapControl = pxInput->count<<16;

    wRetVal = API_CMD_STRUCT_SYS_FIFO_WRITE( ulModHandle, pxInput, &xOutput );
    
    AiOsFree(pxInput);

    return wRetVal;
}

AI_LIB_FUNC AiReturn AI_CALL_CONV ApiCmdFifoWrite(AiUInt32 ulModHandle, AiUInt8 biu, AiUInt8 f_id, AiUInt16 size, AiUInt16 *data)
{
    AiUInt32 i = 0;
    AiInt16 wRetVal = API_OK;

    if( (0 == (ulModHandle & API_STREAM_MASK)) && ((biu < MIN_BIU) || (biu > MAX_BIU)) )
        wRetVal = API_ERR_WRONG_BIU;
    else if( (f_id < MIN_FIFO_ID) || (f_id > MAX_FIFO_ID) )
        wRetVal = API_ERR_PARAM3_NOT_IN_RANGE;
    else if( 0 == size )
        wRetVal = API_ERR_PARAM4_NOT_IN_RANGE;
    else if( NULL == data )
        wRetVal = API_ERR_PARAM5_IS_NULL;
    else
    {
        wRetVal = ApiCmdFifoWrite_(ulModHandle, biu, f_id, size, data);
    }

    v_ExamineRetVal( "ApiCmdFifoWrite", wRetVal );

    TRACE_BEGIN
    TRACE1("        AiUInt16 data[%d];\n", size);
    for(i=0; i<size; i++)
    {
        TRACE1("          data[%d] = ", i); TRACE2("0x%x;\n", data[i]);
    }
    TRACE_FCTA("ApiCmdFifoWrite", wRetVal); 
    TRACE_PARA(ulModHandle);
    TRACE_PARA(biu);
    TRACE_PARA(f_id);
    TRACE_PARA(size);
    TRACE_RPARE("data");
    TRACE_FCTE;
    TRACE_END

    return wRetVal;
}

//***************************************************************************
//
//   Module : APC_API                  Submodule : ApiCmdFifoReadStatus
//
//   Author : Michael Stahl            Project   : API1553-DS
//
//---------------------------------------------------------------------------
//   Create : 22.09.99   Update : 22.09.99
//---------------------------------------------------------------------------
//    Descriptions
//    ------------
//    Inputs    : Fifo Id [f_id],
//
//    Outputs   : Instruction execution status [status]
//                Fifo status [b_status]
//
//    Description :
//    This function applies the 'API_FIFO_STATUS_READ' instruction to the API1553-DS
//    ASP Driver SW to read the status of the specified Fifo.
//
//***************************************************************************

AiInt16 ApiCmdFifoReadStatus_(AiUInt32 bModule, AiUInt8 biu, AiUInt8 f_id, AiUInt16 *b_status)
{
    AiInt16 uw_RetVal = API_OK;
    TY_MIL_COM_SYS_FIFO_STATUS_INPUT   xInput;
    TY_MIL_COM_SYS_FIFO_STATUS_OUTPUT  xOutput;

    MIL_COM_INIT( &xInput.cmd, API_STREAM(bModule), 0, MIL_COM_SYS_FIFO_STATUS, sizeof(xInput), sizeof(xOutput) );

    xInput.id = f_id;

    uw_RetVal = API_CMD_STRUCT_SYS_FIFO_STATUS( bModule, &xInput, &xOutput );

    *b_status = xOutput.status;
 
    return uw_RetVal;
}

AI_LIB_FUNC AiReturn AI_CALL_CONV ApiCmdFifoReadStatus(AiUInt32 bModule, AiUInt8 biu, AiUInt8 f_id, AiUInt16 *b_status)
{
    AiInt16 uw_RetVal = API_OK;

    if( (0 == (bModule & API_STREAM_MASK)) && ((biu < MIN_BIU) || (biu > MAX_BIU)) )
        uw_RetVal = API_ERR_WRONG_BIU;
    else if( (f_id < MIN_FIFO_ID) || (f_id > MAX_FIFO_ID) )
        uw_RetVal = API_ERR_PARAM3_NOT_IN_RANGE;
    else if( NULL == b_status )
        uw_RetVal = API_ERR_PARAM4_IS_NULL;
    else
    {
        uw_RetVal = ApiCmdFifoReadStatus_(bModule, biu, f_id, b_status);
    }
 
    v_ExamineRetVal( "ApiCmdFifoReadStatus", uw_RetVal );

    TRACE_BEGIN
    TRACE("        AiUInt16 status;\n");
    TRACE_FCTA("ApiCmdFifoReadStatus", uw_RetVal); 
    TRACE_PARA(bModule);
    TRACE_PARA(biu);
    TRACE_PARA(f_id);
    TRACE_RPARE("&status");
    TRACE_FCTE;
    TRACE1("    --> status = 0x%x\n", *b_status );
    TRACE_END

    return uw_RetVal;
}
