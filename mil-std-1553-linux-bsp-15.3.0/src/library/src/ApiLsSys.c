/* SPDX-FileCopyrightText: 2015-2024 AIM GmbH <info@aim-online.com> */
/* SPDX-License-Identifier: MIT */

/*! \file ApiLsSys.h
 *
 * LS system functions
 * 
 */

#include "AiOs.h"
#include "Ai_cdef.h"
#include "Ai_def.h"
#include "Ai_mutex.h"

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

#include "ApiLsScopeInternal.h"
#include "hw/ApiHw.h"

extern AiInt16 _ApiCmdSetSysDbgLevel(AiUInt32 bModule, AiUInt32 ul_DbgLevel); // 10.01.2000 pg

static char buf[2000];

//***************************************************************************
//***
//***            A P I 1 5 5 3 - D S    F U N C T I O N S
//***
//***************************************************************************

//***************************************************************************
//***               S Y S T E M    C O M M A N D S
//***************************************************************************











AI_LIB_FUNC AiReturn AI_CALL_CONV ApiCmdIni(AiUInt32 bModule, AiUInt8 mode, TY_API_INI_INFO *pini)
{
    int i = 0;

    AiInt16  uw_RetVal = API_OK;
    TY_DEVICE_INFO * pDevice = _ApiGetDeviceInfoPtrByModule(bModule);

    if (pDevice == NULL)
        return API_ERR_NO_MODULE_EXTENSION;


    ai_mutex_lock(pDevice->_hInitMutex);

    uw_RetVal = ApiCmdIniLegacy_(bModule, mode, pini);

    ai_mutex_release(pDevice->_hInitMutex);

    TRACE_BEGIN
        TRACE("        TY_API_INI_INFO ini;\n");
    TRACE_FCTA("ApiCmdIni", uw_RetVal); 
    TRACE_PARA(bModule);
    TRACE_PARA(mode);
    TRACE_RPARE("&ini");
    TRACE_FCTE;
    TRACE1("    --> ini.asp_bite_id      = 0x%x;\n", pini->asp_bite_id);
    TRACE1("    --> ini.asp_mon_id       = 0x%x;\n", pini->asp_mon_id);
    TRACE1("    --> ini.board_config     = 0x%x;\n", pini->board_config);
    TRACE1("    --> ini.bt[0]            = 0x%x;\n", pini->bt[0]);
    TRACE1("    --> ini.bt[1]            = 0x%x;\n", pini->bt[1]);
    TRACE1("    --> ini.bt[2]            = 0x%x;\n", pini->bt[2]);
    TRACE1("    --> ini.bt[3]            = 0x%x;\n", pini->bt[3]);
    TRACE1("    --> ini.chns             = 0x%x;\n", pini->chns);
    TRACE1("    --> ini.emod             = 0x%x;\n", pini->emod);
    TRACE1("    --> ini.flash_ram_addr   = 0x%x;\n", pini->flash_ram_addr);
    TRACE1("    --> ini.flash_ram_size   = 0x%x;\n", pini->flash_ram_size);
    TRACE1("    --> ini.glb_mem_addr     = 0x%x;\n", pini->glb_mem_addr);
    TRACE1("    --> ini.glb_mem_size     = 0x%x;\n", pini->glb_mem_size);
    TRACE1("    --> ini.irg              = 0x%x;\n", pini->irg);
    TRACE1("    --> ini.loc_dram_addr    = 0x%x;\n", pini->loc_dram_addr);
    TRACE1("    --> ini.loc_dram_size    = 0x%x;\n", pini->loc_dram_size);
    TRACE1("    --> ini.padd1            = 0x%x;\n", pini->padd1);
    TRACE1("    --> ini.padd2            = 0x%x;\n", pini->padd2);
    TRACE1("    --> ini.pbi_id_biu1      = 0x%x;\n", pini->pbi_id_biu1);
    TRACE1("    --> ini.pbi_id_biu2      = 0x%x;\n", pini->pbi_id_biu2);
    for(i=0; i<16; i++)
    {
        TRACE1("    --> ini.pci[%02d]          = ", i); TRACE2("0x%x\n", pini->pci[i]);
    }
    TRACE1("    --> ini.prot             = 0x%x;\n", pini->prot);
    TRACE1("    --> ini.res1             = 0x%x;\n", pini->res1);
    TRACE1("    --> ini.shared_dram_addr = 0x%x;\n", pini->shared_dram_addr);
    TRACE1("    --> ini.shared_dram_size = 0x%x;\n", pini->shared_dram_size);
    TRACE_END

        return( uw_RetVal );
}

/* All information prepared in _ApiPrepareBoardInfo__getItTheOldStyle is now also 
   available with ApiCmdSysGetBoardInfo. We return very basic information here 
   to be able to update boards with very old TSW versions. */
AiInt16 _ApiPrepareBoardInfo__getItTheOldStyle( AiUInt32 bModule )
{
    TY_DEVICE_INFO * pDevice = _ApiGetDeviceInfoPtrByModule( bModule );
    AiUInt32 ul_BoardType;
    AiUInt32 ul_BoardConfig;
    AiUInt32 ul_PartNo;
    AiUInt32 ul_SerialNo;
    AiUInt32 ul_Temp;
    AiUInt32 ul_SRAMOffset;
    AiUInt32 i;

    if( pDevice == NULL )
        return API_ERR_NO_MODULE_EXTENSION;

    _ApiReadMemData(bModule, API_MEMTYPE_SHARED, 0, 4, &ul_Temp);
    if( 0x53484152 == ul_Temp )
        ul_SRAMOffset = 0;
    else
        ul_SRAMOffset = 0x4000;

    _ApiReadMemData(bModule, API_MEMTYPE_SHARED, ul_SRAMOffset+32, 4, &ul_BoardType);
    _ApiReadMemData(bModule, API_MEMTYPE_SHARED, ul_SRAMOffset+36, 4, &ul_BoardConfig);
    _ApiReadMemData(bModule, API_MEMTYPE_SHARED, ul_SRAMOffset+40, 4, &ul_PartNo);
    _ApiReadMemData(bModule, API_MEMTYPE_SHARED, ul_SRAMOffset+44, 4, &ul_SerialNo);

    for (i=0; i<MAX_BIU; i++)
        pDevice->x_BoardInfo.ul_GlobalRamStartOffset[i] = 0;
    pDevice->x_BoardInfo.ul_DeviceType = 0;
    pDevice->x_BoardInfo.ul_NumberOfChannels = 1;
    pDevice->x_BoardInfo.ul_NumberOfBiu = 1;
    pDevice->x_BoardInfo.ul_NovRamBoardType   = ul_BoardType;
    pDevice->x_BoardInfo.ul_NovRamBoardConfig = ul_BoardConfig;
    pDevice->x_BoardInfo.ul_SerialNumber      = ul_SerialNo;
    pDevice->x_BoardInfo.ul_PartNumber        = ul_PartNo;
    pDevice->x_BoardInfo.ul_GlobalRamSize     = pDevice->x_IniPar.glb_mem_size;
    pDevice->x_BoardInfo.ul_SharedRamSize     = pDevice->x_IniPar.shared_dram_size;

    return 0;
}

void _ApiPrepareBoardInfo( AiUInt32 bModule )
{
    AiUInt32  aul_Info[TY_BOARD_INFO_MAX];
    AiInt16   wReturn;
    TY_DEVICE_INFO * pDevice = _ApiGetDeviceInfoPtrByModule( bModule );


    pDevice->x_BoardInfo.ul_GlobalRamStartOffset[0] = 0;
    pDevice->x_BoardInfo.ul_GlobalRamStartOffset[1] = 0;
    pDevice->x_BoardInfo.ul_GlobalRamStartOffset[2] = 0;
    pDevice->x_BoardInfo.ul_GlobalRamStartOffset[3] = 0;


    if( IsTswVersionGreaterOrEqual( pDevice, 20, 30 ) )
    {
        /* if the ApiCmdSysGetBoardInfo command is supported by the target software, use it */
        /* ApiCmdSysGetBoardInfo is stream based the device info here however is global. Use stream 1 info for now */
        bModule = (bModule & ~API_STREAM_MASK) | (1<<API_STREAM_POS);
        wReturn = ApiCmdSysGetBoardInfo( bModule, 0, TY_BOARD_INFO_MAX, aul_Info, NULL );

        if( wReturn == API_OK )
        {
            /*
            * We overwrite the values from _ApiPrepareBoardInfo__getItTheOldStyle with the
            * values from the TSW. This is because the TSW might have additional information
            * like the packed coupling bit or NOVRAM modifications.
            * */
            pDevice->x_BoardInfo.ul_DeviceType              = aul_Info[TY_BOARD_INFO_DEVICE_TYPE];
            pDevice->x_BoardInfo.ul_NumberOfChannels        = aul_Info[TY_BOARD_INFO_CHANNEL_COUNT];
            pDevice->x_BoardInfo.ul_NumberOfBiu             = aul_Info[TY_BOARD_INFO_BIU_COUNT];
            pDevice->x_BoardInfo.ul_NovRamBoardType         = aul_Info[TY_BOARD_INFO_BOARD_TYPE];
            pDevice->x_BoardInfo.ul_NovRamBoardConfig       = aul_Info[TY_BOARD_INFO_BOARD_CONFIG];
            pDevice->x_BoardInfo.ul_SerialNumber            = aul_Info[TY_BOARD_INFO_SERIAL];
            pDevice->x_BoardInfo.ul_PartNumber              = aul_Info[TY_BOARD_INFO_PARTNO];
            pDevice->x_BoardInfo.ul_GlobalRamSize           = aul_Info[TY_BOARD_INFO_SIZE_GLOBAL];
            pDevice->x_BoardInfo.ul_SharedRamSize           = aul_Info[TY_BOARD_INFO_SIZE_SHARED];
            pDevice->x_BoardInfo.x_BoardCapabilities.ul_CanChangeAmplitude         = aul_Info[TY_BOARD_INFO_CHANGE_AMPL];
            pDevice->x_BoardInfo.x_BoardCapabilities.x_CouplingCapabilities.ul_All = aul_Info[TY_BOARD_INFO_COUPLING];
            pDevice->x_BoardInfo.x_BoardCapabilities.x_IrigCapabilities.ul_All     = aul_Info[TY_BOARD_INFO_IRIG];
        }

    }
    else
    {
        /* if the target software is too old, get the data by other means */
        /* We read the board info the old way because it is difficult to convert
        * the ul_GlobaRamStartOffset stream based to physical BIU based
        * */
        _ApiPrepareBoardInfo__getItTheOldStyle(bModule);
    }


    sprintf(buf, "x_BoardInfo[%d].ul_DeviceType         = %d\n", bModule, pDevice->x_BoardInfo.ul_DeviceType );
    DEBUGOUT( DBG_INFO, FKT_APICMDINI, buf );
    sprintf(buf, "x_BoardInfo[%d].ul_NumberOfBiu        = %d\n", bModule, pDevice->x_BoardInfo.ul_NumberOfBiu );
    DEBUGOUT( DBG_INFO, FKT_APICMDINI, buf );
    sprintf(buf, "x_BoardInfo[%d].ul_NumberOfChannels   = %d\n", bModule, pDevice->x_BoardInfo.ul_NumberOfChannels );
    DEBUGOUT( DBG_INFO, FKT_APICMDINI, buf );
    sprintf(buf, "x_BoardInfo[%d].ul_CanChangeAmplitude = %d\n", bModule, pDevice->x_BoardInfo.x_BoardCapabilities.ul_CanChangeAmplitude );
    DEBUGOUT( DBG_INFO, FKT_APICMDINI, buf );
    sprintf(buf, "x_BoardInfo[%d].b_IrigSwitch          = %d\n", bModule, pDevice->x_BoardInfo.x_BoardCapabilities.x_IrigCapabilities.b.b_IrigSwitch );
    DEBUGOUT( DBG_INFO, FKT_APICMDINI, buf );
    sprintf(buf, "x_BoardInfo[%d].b_FreeWheeling        = %d\n", bModule, pDevice->x_BoardInfo.x_BoardCapabilities.x_IrigCapabilities.b.b_FreeWheeling );
    DEBUGOUT( DBG_INFO, FKT_APICMDINI, buf );
    sprintf(buf, "x_BoardInfo[%d].b_Sinusoidal          = %d\n", bModule, pDevice->x_BoardInfo.x_BoardCapabilities.x_IrigCapabilities.b.b_Sinusoidal );
    DEBUGOUT( DBG_INFO, FKT_APICMDINI, buf );
    sprintf(buf, "x_BoardInfo[%d].b_Isolated            = %d\n", bModule, pDevice->x_BoardInfo.x_BoardCapabilities.x_CouplingCapabilities.b.b_Isolated );
    DEBUGOUT( DBG_INFO, FKT_APICMDINI, buf );
    sprintf(buf, "x_BoardInfo[%d].b_Direct              = %d\n", bModule, pDevice->x_BoardInfo.x_BoardCapabilities.x_CouplingCapabilities.b.b_Direct );
    DEBUGOUT( DBG_INFO, FKT_APICMDINI, buf );
    sprintf(buf, "x_BoardInfo[%d].b_Transformer         = %d\n", bModule, pDevice->x_BoardInfo.x_BoardCapabilities.x_CouplingCapabilities.b.b_Transformer );
    DEBUGOUT( DBG_INFO, FKT_APICMDINI, buf );
    sprintf(buf, "x_BoardInfo[%d].b_Network             = %d\n", bModule, pDevice->x_BoardInfo.x_BoardCapabilities.x_CouplingCapabilities.b.b_Network );
    DEBUGOUT( DBG_INFO, FKT_APICMDINI, buf );
    sprintf(buf, "x_BoardInfo[%d].ul_NovRamBoardType    = %08X\n", bModule, pDevice->x_BoardInfo.ul_NovRamBoardType );
    DEBUGOUT( DBG_INFO, FKT_APICMDINI, buf );
    sprintf(buf, "x_BoardInfo[%d].ul_NovRamBoardConfig  = %08X\n", bModule, pDevice->x_BoardInfo.ul_NovRamBoardConfig );
    DEBUGOUT( DBG_INFO, FKT_APICMDINI, buf );
    sprintf(buf, "x_BoardInfo[%d].ul_SerialNumber       = %d\n", bModule, pDevice->x_BoardInfo.ul_SerialNumber );
    DEBUGOUT( DBG_INFO, FKT_APICMDINI, buf );
    sprintf(buf, "x_BoardInfo[%d].ul_PartNumber         = %08X\n", bModule, pDevice->x_BoardInfo.ul_PartNumber );
    DEBUGOUT( DBG_INFO, FKT_APICMDINI, buf );
    sprintf(buf, "x_BoardInfo[%d].ul_GlobalRamSize      = %08X\n", bModule, pDevice->x_BoardInfo.ul_GlobalRamSize );
    DEBUGOUT( DBG_INFO, FKT_APICMDINI, buf );
    sprintf(buf, "x_BoardInfo[%d].ul_SharedRamSize      = %08X\n", bModule, pDevice->x_BoardInfo.ul_SharedRamSize );
    DEBUGOUT( DBG_INFO, FKT_APICMDINI, buf );
    sprintf(buf, "x_BoardInfo[%d].ul_GlobalRamStartOffset[0] = %08X\n", bModule, pDevice->x_BoardInfo.ul_GlobalRamStartOffset[0] );
    DEBUGOUT( DBG_INFO, FKT_APICMDINI, buf );
    sprintf(buf, "x_BoardInfo[%d].ul_GlobalRamStartOffset[1] = %08X\n", bModule, pDevice->x_BoardInfo.ul_GlobalRamStartOffset[1] );
    DEBUGOUT( DBG_INFO, FKT_APICMDINI, buf );
    sprintf(buf, "x_BoardInfo[%d].ul_GlobalRamStartOffset[2] = %08X\n", bModule, pDevice->x_BoardInfo.ul_GlobalRamStartOffset[2] );
    DEBUGOUT( DBG_INFO, FKT_APICMDINI, buf );
    sprintf(buf, "x_BoardInfo[%d].ul_GlobalRamStartOffset[3] = %08X\n", bModule, pDevice->x_BoardInfo.ul_GlobalRamStartOffset[3] );
    DEBUGOUT( DBG_INFO, FKT_APICMDINI, buf );
}


AiInt16 ApiCmdIniLegacy_(AiUInt32 bModule, AiUInt8 mode, TY_API_INI_INFO *pini)
{
    AiInt16  in_byte_size;
    AiInt16  in_word_size;
    AiInt16  in_lword_size;
    AiUInt8  info;
    AiInt16  uw_RetVal = API_OK;
    AiUInt8  auc_Info[8];
    AiUInt16 auw_Info[4];
    AiUInt32 aul_Info[30];
    AiUInt32 i;
    TY_DEVICE_INFO * pDevice = _ApiGetDeviceInfoPtrByModule( bModule );

    if( pDevice == NULL )
        return API_ERR_NO_MODULE_EXTENSION;

    if( mode > MAX_INIT_MODE )
        uw_RetVal = API_ERR_PARAM2_NOT_IN_RANGE;
    else if( NULL == pini )
        uw_RetVal = API_ERR_PARAM3_IS_NULL;
    else
    {

        in_byte_size = 0;
        in_word_size = 0;
        in_lword_size = 0;


        info = mode;

        uw_RetVal = ApiIo(bModule & ~API_STREAM_MASK, 0, TG_API_INI, TG_ACK2,
            &info, 1, w_p, 0,
            auc_Info, &in_byte_size,
            auw_Info, &in_word_size,
            aul_Info, &in_lword_size);
        pini->bt[0] = auc_Info[0];
        pini->bt[1] = auc_Info[1];
        pini->chns  = auc_Info[2];
        pini->prot  = auc_Info[3];
        pini->emod  = auc_Info[4];
        pini->irg   = auc_Info[5];
        pini->res1  = auc_Info[6];
        pini->padd1 = 0;
        pini->padd2 = 0;

        pini->pbi_id_biu1 = auw_Info[0];
        pini->asp_mon_id  = auw_Info[1];
        pini->asp_bite_id = auw_Info[2];
        pini->pbi_id_biu2 = auw_Info[3];

        pini->board_config     = aul_Info[0];
        pini->glb_mem_size     = aul_Info[1];
        pini->glb_mem_addr     = aul_Info[2];
        pini->loc_dram_size    = aul_Info[3];
        pini->loc_dram_addr    = aul_Info[4];
        pini->shared_dram_size = aul_Info[5];
        pini->shared_dram_addr = aul_Info[6];
        pini->flash_ram_size   = aul_Info[7];
        pini->flash_ram_addr   = aul_Info[8];
        for( i=0; i<16; i++ )
            pini->pci[i] = aul_Info[9+i];

        pini->bt[2] = (AiUInt8)aul_Info[25];
        pini->bt[3] = (AiUInt8)aul_Info[26];
        pini->board_type       = aul_Info[27];
        pini->board_sub_type   = aul_Info[28];
        pini->hardware_variant = aul_Info[29];


        /* collect the data later needed for ApiGetBoardInfo */
        _ApiPrepareBoardInfo( bModule);


        /* If we are local and the mode is not read flush interrupts */
        if( GET_SERVER_ID(bModule) == 0 )
            if( mode != 1 )
                _ApiOsFlushInterrupts(bModule);


        v_ExamineRetVal( "ApiCmdIniLegacy_", uw_RetVal );
    }

    return(uw_RetVal);
} // end: ApiCmdIniLegacy_

AiInt16 ApiCmdIniStruct_(AiUInt32 bModule, AiUInt8 mode, TY_API_INI_INFO *pini)
{
    AiUInt32 i;
    AiInt16  uw_RetVal = API_OK;
    TY_MIL_COM_SYS_INI_INPUT    xInput;
    TY_MIL_COM_SYS_INI_OUTPUT   xOutput;

    TY_DEVICE_INFO * pDevice = _ApiGetDeviceInfoPtrByModule( bModule );

    if( pDevice == NULL )
        return API_ERR_NO_MODULE_EXTENSION;

    if( mode > MAX_INIT_MODE )
        uw_RetVal = API_ERR_PARAM2_NOT_IN_RANGE;
    else if( NULL == pini )
        uw_RetVal = API_ERR_PARAM3_IS_NULL;
    else
    {
        MIL_COM_INIT( &xInput.cmd, API_STREAM(bModule), 0, MIL_COM_SYS_INI, sizeof(xInput), sizeof(xOutput) );

        xInput.mode = mode;

        uw_RetVal = API_CMD_STRUCT_SYS_INI( bModule, &xInput, &xOutput );

        pini->bt[0] = (AiUInt8)xOutput.bt[0];
        pini->bt[1] = (AiUInt8)xOutput.bt[1];
        pini->bt[2] = (AiUInt8)xOutput.bt[2];
        pini->bt[3] = (AiUInt8)xOutput.bt[3];
        pini->chns  = (AiUInt8)xOutput.streams;
        pini->prot  = (AiUInt8)xOutput.prot;

        pini->pbi_id_biu1 = (AiUInt16)xOutput.pbi_id_biu1;
        pini->asp_mon_id  = (AiUInt16)xOutput.asp_mon_id;
        pini->asp_bite_id = (AiUInt16)xOutput.asp_bite_id;
        pini->pbi_id_biu2 = (AiUInt16)xOutput.pbi_id_biu2;

        pini->glb_mem_size     = xOutput.glb_mem_size;
        pini->loc_dram_size    = xOutput.loc_dram_size;
        pini->shared_dram_size = xOutput.shared_dram_size;

        pini->board_config     = xOutput.board_config;
        pini->board_type       = xOutput.board_type;
        pini->board_sub_type   = xOutput.board_sub_type;
        pini->hardware_variant = xOutput.hardware_variant;



        /* Obsolete values should be removed with an incompatible BSP */
        pini->emod  = 0;
        pini->irg   = 0;
        pini->res1  = 0;
        pini->padd1 = 0;
        pini->padd2 = 0;
        pini->glb_mem_addr     = 0;
        pini->loc_dram_addr    = 0;
        pini->shared_dram_addr = 0;
        pini->flash_ram_size   = 0;
        pini->flash_ram_addr   = 0;

        for( i=0; i<16; i++ )
            pini->pci[i] = 0;




        /* collect the data later needed for ApiGetBoardInfo */
        _ApiPrepareBoardInfo( bModule);


        /* If we are local and the mode is not read flush interrupts */
        if( GET_SERVER_ID(bModule) == 0 )
            if( mode != 1 )
                _ApiOsFlushInterrupts(bModule);

        v_ExamineRetVal( "ApiCmdIniStruct_", uw_RetVal );
    }

    return(uw_RetVal);
} // end: ApiCmdIniStruct_


//***************************************************************************
//
//   Module : APC_API                  Submodule : ApiCmdReset
//
//   Author : Marco Motz               Project   : API1553-DS
//
//---------------------------------------------------------------------------
//   Create : 01.06.97   Update : 12.04.00
//---------------------------------------------------------------------------
//    Descriptions
//    ------------
//    Inputs    : Reset control [rc]
//
//    Outputs   : Instruction execution status [status]
//                Reset Info structure [pres]
//
//    Description :
//    This function applies the 'API_RESET' instruction to the API1553-DS
//    ASP Driver SW to init the API1553-DS board.
//
//***************************************************************************

AI_LIB_FUNC AiReturn AI_CALL_CONV ApiCmdResetLegacy_(AiUInt32 bModule, AiUInt8 biu, AiUInt8 rc, TY_API_RESET_INFO *pres)
{
    AiInt16 in_byte_size = 0;
    AiUInt8 info[2];
    AiUInt8 uc_MBufSize = 0;
    AiUInt8   auc_Info[30];
    AiInt16 uw_RetVal = API_OK;

    info[0] = rc;
    info[1] = uc_MBufSize;

    uw_RetVal = ApiIo(bModule, biu, TG_API_RESET, TG_ACK2,
        &info[0], 2, w_p, 0,
        auc_Info, &in_byte_size,
        w_p, &tg_os,
        lw_p, &tg_os);

    // changed ack info
    pres->mbufs     = auc_Info[0];
    pres->sbufs     = auc_Info[1];
    pres->mon_addr  = (auc_Info[4] << 24);
    pres->mon_addr += (auc_Info[5] << 16);
    pres->mon_addr += (auc_Info[6] << 8);
    pres->mon_addr += (auc_Info[7]);
    pres->sim_addr  = (auc_Info[8] << 24);
    pres->sim_addr += (auc_Info[9] << 16);
    pres->sim_addr += (auc_Info[10] << 8);
    pres->sim_addr += (auc_Info[11]);

    return uw_RetVal;

} // end: ApiCmdResetLegacy_

AI_LIB_FUNC AiReturn AI_CALL_CONV ApiCmdReset_(AiUInt32 bModule, AiUInt8 biu, AiUInt8 rc, TY_API_RESET_INFO *pres)
{
    AiInt16 uw_RetVal = API_OK;
    TY_MIL_COM_SYS_RESET_INPUT   xInput;
    TY_MIL_COM_SYS_RESET_OUTPUT  xOutput;

    MIL_COM_INIT( &xInput.cmd, API_STREAM(bModule), 0, MIL_COM_SYS_RESET, sizeof(xInput), sizeof(xOutput) );

    xInput.mode  = rc;
    xInput.mbufs = 0; /* Only used for HS reset */

    uw_RetVal = API_CMD_STRUCT_SYS_RESET( bModule, &xInput, &xOutput );

    if( uw_RetVal == 0 )
    {
        pres->mbufs    = xOutput.mbufs;
        pres->sbufs    = xOutput.sbufs;
        pres->mon_addr = xOutput.maddr;
        pres->sim_addr = xOutput.saddr;
    }

    return(uw_RetVal);

} // end: ApiCmdReset_


AI_LIB_FUNC AiReturn AI_CALL_CONV ApiCmdReset(AiUInt32 bModule, AiUInt8 biu, AiUInt8 rc, TY_API_RESET_INFO *pres)
{
    AiInt16 wRetVal = API_OK;
    TY_DEVICE_INFO * pDevice = _ApiGetDeviceInfoPtrByModule(bModule);
    if (pDevice == NULL)
        return API_ERR_NO_MODULE_EXTENSION;

    if( (0 == (bModule & API_STREAM_MASK)) && ((biu < MIN_BIU) || (biu > MAX_BIU)) )
        wRetVal = API_ERR_WRONG_BIU;
    else if( NULL == pres )
        wRetVal = API_ERR_PARAM4_IS_NULL;
    else
    {
        if (IsTswVersionGreaterOrEqual( pDevice, 21, 0 ) )
            wRetVal = ApiCmdReset_(bModule, biu, rc, pres);
        else
            wRetVal = ApiCmdResetLegacy_(bModule, biu, rc, pres);
    }

    v_ExamineRetVal( "ApiCmdReset", wRetVal );
    TRACE_BEGIN
    TRACE("        TY_API_RESET_INFO res;\n");
    TRACE_FCTA("ApiCmdReset", wRetVal); 
    TRACE_PARA(bModule);
    TRACE_PARA(biu);
    TRACE_PARA(rc);
    TRACE_RPARE("&res");
    TRACE_FCTE;
    TRACE1("    --> res.mbufs    = 0x%x;\n", pres->mbufs);
    TRACE1("    --> res.mon_addr = 0x%x;\n", pres->mon_addr);
    TRACE1("    --> res.padding1 = 0x%x;\n", pres->padding1);
    TRACE1("    --> res.sbufs    = 0x%x;\n", pres->sbufs);
    TRACE1("    --> res.sim_addr = 0x%x;\n", pres->sim_addr);
    TRACE_END

    return wRetVal;
} // end: ApiCmdReset

//***************************************************************************

//
//   Module : APC_API                  Submodule : ApiCmdBite
//
//   Author : Marco Motz               Project   : API1553-DS
//
//---------------------------------------------------------------------------
//   Create : 01.06.97   Update : 01.06.97
//---------------------------------------------------------------------------
//    Descriptions
//    ------------
//    Inputs    : Selftest control [sc]
//
//    Outputs   : Instruction execution status [status]
//                Initialize Info structure [pini]
//
//    Description :
//    This function applies the 'API_INI' instruction to the API1553-DS
//    ASP Driver SW to init the API1553-DS board.
//
//***************************************************************************
AiInt16 ApiCmdBite_(AiUInt32 bModule, AiUInt8 biu, AiUInt8 sc, AiUInt8 bite_status[])
{
    AiInt16 uw_RetVal = API_OK;
    TY_MIL_COM_SYS_BITE_INPUT    xInput;
    TY_MIL_COM_SYS_BITE_OUTPUT   xOutput;

    MIL_COM_INIT( &xInput.cmd, API_STREAM(bModule), 0, MIL_COM_SYS_BITE, sizeof(xInput), sizeof(xOutput) );

    xInput.mode  = sc;

    uw_RetVal = API_CMD_STRUCT_SYS_BITE( bModule, &xInput, &xOutput );

    if( uw_RetVal == 0 )
    {
        bite_status[0] = (AiUInt8)((xOutput.result >> 8) & 0xFF);
        bite_status[1] = (AiUInt8)((xOutput.result     ) & 0xFF);
    }

    return uw_RetVal;
}

AI_LIB_FUNC AiReturn AI_CALL_CONV ApiCmdBite(AiUInt32 bModule, AiUInt8 biu, AiUInt8 sc, AiUInt8 bite_status[])
{
    AiInt16 uw_RetVal = API_OK;

    if( (0 == (bModule & API_STREAM_MASK)) && ((biu < MIN_BIU) || (biu > MAX_BIU)) )
        uw_RetVal = API_ERR_WRONG_BIU;
    else if( (sc & ~API_BITE_DISABLE_RESET) > MAX_BITE_CONTROL )
        uw_RetVal = API_ERR_PARAM3_NOT_IN_RANGE;
    else
    {
        uw_RetVal = ApiCmdBite_(bModule, biu, sc, bite_status);
    }
    
    v_ExamineRetVal( "ApiCmdBite", uw_RetVal );
    
    TRACE_BEGIN
    TRACE("        AiUInt8 bite_status[2];\n");
    TRACE_FCTA("ApiCmdBite", uw_RetVal); 
    TRACE_PARA(bModule);
    TRACE_PARA(biu);
    TRACE_PARA(sc);
    TRACE_RPARE("bite_status");
    TRACE_FCTE;
    TRACE1("    --> bite_status[0] = 0x%x;\n", bite_status[0]);
    TRACE1("    --> bite_status[1] = 0x%x;\n", bite_status[1]);
    TRACE_END
    
    return uw_RetVal;
} // end: ApiCmdBite

//***************************************************************************
//
//   Module : APC_API                  Submodule : ApiCmdDefRespTout
//
//   Author : Marco Motz               Project   : API1553-DS
//
//---------------------------------------------------------------------------
//   Create : 01.06.97   Update : 01.06.97
//---------------------------------------------------------------------------
//    Descriptions
//    ------------
//    Inputs    : Response Timeout value [resp_tout]
//
//    Outputs   : Instruction execution status [status]
//
//    Description :
//    This function applies the 'API_DEF_RESP_TOUT' instruction to the API1553-DS
//    ASP Driver SW to init the API1553-DS Response Timeout.
//
//***************************************************************************

AiInt16 ApiCmdDefRespTout_(AiUInt32 bModule, AiUInt8 biu, AiFloat resp_tout)
{
    AiInt16 uw_RetVal = API_OK;
    TY_MIL_COM_SYS_RESP_TOUT_DEF_INPUT xInput;
    TY_MIL_COM_ACK                     xOutput;

    MIL_COM_INIT( &xInput.cmd, API_STREAM(bModule), 0, MIL_COM_SYS_RESP_TOUT_DEF, sizeof(xInput), sizeof(xOutput) );

    xInput.timeout = (AiUInt32)(resp_tout * 4);

    uw_RetVal = API_CMD_STRUCT_SYS_RESP_TOUT_DEF( bModule, &xInput, &xOutput );
    
    return uw_RetVal;
}

AI_LIB_FUNC AiReturn AI_CALL_CONV ApiCmdDefRespTout(AiUInt32 bModule, AiUInt8 biu, AiFloat resp_tout)
{
    AiInt16 uw_RetVal = API_OK;

    if( (0 == (bModule & API_STREAM_MASK)) && ((biu < MIN_BIU) || (biu > MAX_BIU)) )
        uw_RetVal = API_ERR_WRONG_BIU;
    else if( resp_tout > MAX_RESP_TIME )
        uw_RetVal = API_ERR_PARAM3_NOT_IN_RANGE;
    else
    {
        uw_RetVal = ApiCmdDefRespTout_(bModule, biu, resp_tout);
    }
    
    v_ExamineRetVal( "ApiCmdDefRespTout", uw_RetVal );
    
    TRACE_BEGIN
    TRACE_FCTA("ApiCmdDefRespTout", uw_RetVal); 
    TRACE_PARA(bModule);
    TRACE_PARA(biu);
    TRACE_FPARE(resp_tout);
    TRACE_FCTE;
    TRACE_END
    
    return uw_RetVal;
} // end: ApiCmdDefRespTout


//***************************************************************************
//
//   Module : APC_API                  Submodule : ApiCmdLoadSRec
//
//   Author : Marco Motz               Project   : API1553-DS
//
//---------------------------------------------------------------------------
//   Create : 01.06.97   Update : 01.06.97
//---------------------------------------------------------------------------
//    Descriptions
//    ------------
//    Inputs    : Download mode [mode]
//                Amount of characters to download [cnt]
//                Offset to add to S-Record [offset]
//                Pointer to S-Record string [srec]
//
//    Outputs   : Instruction execution status [status]
//                Download status info [st]
//                Amount of downloaded characters [fsize]
//
//    Description :
//    This function applies the 'API_LOAD_SREC' instruction to the API1553-DS
//    ASP Driver SW to download S-Records to the API1553-DS target via PCI-bus.
//

//***************************************************************************

AiInt16 ApiCmdLoadSRec_(AiUInt32 bModule, AiUInt8 mode, AiUInt8 cnt, AiUInt32 offset,
                                                 AiUInt8 *srec, AiUInt8 *st, AiUInt32 *fsize)
{
    AiInt16 i;
    AiInt16 in_byte_size = 0;
    AiInt16 in_lword_size = 0;
    BYTE bbuf[256+6];
    AiInt16 uw_RetVal = API_OK;

    bbuf[0] = mode;
    bbuf[1] = cnt;
    bbuf[2] = (BYTE)(offset >> 24);
    bbuf[3] = (BYTE)(offset >> 16);
    bbuf[4] = (BYTE)(offset >> 8);
    bbuf[5] = (BYTE)(offset);
    for (i=0; i<cnt; i++)
        bbuf[6+i] = *(srec+i);

    uw_RetVal = ApiIo(bModule & ~API_STREAM_MASK, 0, TG_API_LOAD_SREC, TG_ACK2,
        bbuf, (AiInt16)(cnt+6), w_p, 0,
        st, &in_byte_size,
        w_p, &tg_os,
        fsize, &in_lword_size);
    
    return uw_RetVal;
}

AI_LIB_FUNC AiReturn AI_CALL_CONV ApiCmdLoadSRec(AiUInt32 bModule, AiUInt8 mode, AiUInt8 cnt, AiUInt32 offset,
                                                 AiUInt8 *srec, AiUInt8 *st, AiUInt32 *fsize)
{
    AiInt16 i;
    AiInt16 uw_RetVal = API_OK;

    if( mode > MAX_SREC_MODE )
        uw_RetVal = API_ERR_PARAM2_NOT_IN_RANGE;
    else if( (mode != API_SREC_DOWNLOAD) && (cnt != 0) )
        uw_RetVal = API_ERR_PARAM3_NOT_IN_RANGE;
    else if( (API_SREC_INIT == mode) && (offset != 0) )
        uw_RetVal = API_ERR_PARAM4_NOT_IN_RANGE;
    else if( (cnt > 0) && (NULL == srec) )
        uw_RetVal = API_ERR_PARAM5_IS_NULL;
    else if( NULL == st )
        uw_RetVal = API_ERR_PARAM6_IS_NULL;
    else if( NULL == fsize )
        uw_RetVal = API_ERR_PARAM7_IS_NULL;
    else
    {
        uw_RetVal = ApiCmdLoadSRec_(bModule, mode, cnt, offset, srec, st, fsize);
    }

    v_ExamineRetVal( "ApiCmdLoadSRec", uw_RetVal );

    TRACE_BEGIN
    TRACE("        AiUInt8  srec[80];\n");
    TRACE("        AiUInt8  st;\n");
    TRACE("        AiUInt32 fsize;\n");
    for(i=0; i<80; i++)
    {
        TRACE1("        srec[%02d] = ", i); TRACE2("%02x;\n", srec[i]);
    }
    TRACE_FCTA("ApiCmdLoadSRec", uw_RetVal); 
    TRACE_PARA(bModule);
    TRACE_PARA(mode);
    TRACE_PARA(cnt);
    TRACE_PARA(offset);
    TRACE_RPARA("srec");
    TRACE_RPARA("&st");
    TRACE_RPARE("&fsize");
    TRACE_FCTE;
    TRACE1("    --> st    = 0x%x\n", *st);
    TRACE1("    --> fsize = 0x%x\n", *fsize);
    TRACE_END
    
    return uw_RetVal;
}

//***************************************************************************
//
//   Module : APC_API                  Submodule : ApiCmdProgFlash
//
//   Author : Marco Motz               Project   : API1553-DS
//
//---------------------------------------------------------------------------
//   Create : 01.06.97   Update : 01.06.97
//---------------------------------------------------------------------------
//    Descriptions
//    ------------
//    Inputs    : Source Address [src_addr]
//                Sector number [setcor]
//                Program size [size]
//
//    Outputs   : Instruction execution status [status]
//                Programming status info [st]
//
//    Description :
//    This function applies the 'API_PROG_FLASH' instruction to the API1553-DS
//    ASP Driver SW to program the selected sector of the API1553-DS flash memory.
//
//***************************************************************************

AiInt16 ApiCmdProgFlash_(AiUInt32 bModule, AiUInt32 src_addr, AiUInt32 sector, AiUInt32 size, AiUInt8 *st)
{
    AiInt16 in_byte_size = 0;
    WORD wbuf[6];
    AiInt16 uw_RetVal = API_OK;

    wbuf[0] = (WORD)(src_addr >> 16);
    wbuf[1] = (WORD)(src_addr);
    wbuf[2] = (WORD)(sector >> 16);
    wbuf[3] = (WORD)(sector);
    wbuf[4] = (WORD)(size >> 16);
    wbuf[5] = (WORD)(size);

    uw_RetVal = ApiIo(bModule & ~API_STREAM_MASK, 0, TG_API_PROG_FLASH, TG_ACK2,
            b_p, 0, &wbuf[0], 6,
            st, &in_byte_size,
            w_p, &tg_os, lw_p, &tg_os);
    
    return uw_RetVal;
}

AI_LIB_FUNC AiReturn AI_CALL_CONV ApiCmdProgFlash(AiUInt32 bModule, AiUInt32 src_addr, AiUInt32 sector, AiUInt32 size, AiUInt8 *st)
{
    AiInt16 uw_RetVal = API_OK;

    if( NULL == st )
        uw_RetVal = API_ERR_PARAM5_IS_NULL;
    else
    {
        uw_RetVal = ApiCmdProgFlash_(bModule, src_addr, sector, size, st);
    }
    
    v_ExamineRetVal( "ApiCmdProgFlash", uw_RetVal );
    
    TRACE_BEGIN
    TRACE("        AiUInt8  st;\n");
    TRACE_FCTA("ApiCmdProgFlash", uw_RetVal); 
    TRACE_PARA(bModule);
    TRACE_PARA(src_addr);
    TRACE_PARA(sector);
    TRACE_PARA(size);
    TRACE_RPARE("&st");
    TRACE_FCTE;
    TRACE1("    --> st    = 0x%x\n", *st);
    TRACE_END
    
    return uw_RetVal;
}


//***************************************************************************
//
//   Module : APC_API                  Submodule : ApiCmdExecSys
//
//   Author : Marco Motz               Project   : API1553-DS
//
//---------------------------------------------------------------------------
//   Create : 01.06.97   Update : 01.06.97
//---------------------------------------------------------------------------
//    Descriptions
//    ------------
//    Inputs    : System Command mode [mode]
//                System Command control [con]
//
//    Outputs   : Instruction execution status [status]
//
//    Description :
//    This function applies the 'API_EXEC_SYS' instruction to the API1553-DS
//    ASP Driver SW to execute a system related command on the API1553 target.
//
//***************************************************************************

AiInt16 ApiCmdExecSys_(AiUInt32 bModule, AiUInt8 mode, AiUInt8 con)
{
    AiInt16 uw_RetVal = API_OK;
    AiUInt32 ul_Biu = 0;
    TY_DEVICE_INFO * pDevice = _ApiGetDeviceInfoPtrByModule( bModule );
    TY_MIL_COM_SYS_EXEC_INPUT   xInput;
    TY_MIL_COM_ACK              xOutput;

    if( pDevice == NULL )
        return API_ERR_NO_MODULE_EXTENSION;

    MIL_COM_INIT( &xInput.cmd, API_STREAM(bModule), 0, MIL_COM_SYS_EXEC, sizeof(xInput), sizeof(xOutput) );

    xInput.mode = mode;

    if( API_MODE_DDL == mode )
    {
        uw_RetVal = uw_ApiGetBiuFromStream( bModule, 0, &ul_Biu);
        if( ul_Biu != 0 )
            ul_Biu--;
        xInput.con = (AiUInt8)ul_Biu*2 + con;
    }
    else
        xInput.con = con;

    uw_RetVal = API_CMD_STRUCT_SYS_EXEC( bModule, &xInput, &xOutput );

    if( API_OK == uw_RetVal )
    {
        if( 8 == mode ) /* set HS subaddress */
            pDevice->uc_HsSA = con;
    }
    
    return uw_RetVal;
}

AI_LIB_FUNC AiReturn AI_CALL_CONV ApiCmdExecSys(AiUInt32 bModule, AiUInt8 mode, AiUInt8 con)
{
    AiInt16 uw_RetVal = API_OK;

    if( mode > MAX_EXECSYS_MODE )
        uw_RetVal = API_ERR_PARAM2_NOT_IN_RANGE;
    else if( (API_MODE_TIMETAG_SOURCE == mode) && (con > API_IRIG_EXTERN) )
        uw_RetVal = API_ERR_PARAM3_NOT_IN_RANGE;
    else if( (API_MODE_DSUB_CONNECT == mode) && (con > API_DSUB_TRG_IN) )
        uw_RetVal = API_ERR_PARAM3_NOT_IN_RANGE;
    else if( (API_MODE_HS_ADDR == mode) && (con > MAX_RT_SA) )
        uw_RetVal = API_ERR_PARAM3_NOT_IN_RANGE;
    else if( (API_MODE_DDL == mode) && (con > API_ENA) )
        uw_RetVal = API_ERR_PARAM3_NOT_IN_RANGE;
    else
    {
        uw_RetVal = ApiCmdExecSys_(bModule, mode, con);
    }
    
    v_ExamineRetVal( "ApiCmdExecSys", uw_RetVal );
    
    TRACE_BEGIN
    TRACE_FCTA("ApiCmdExecSys", uw_RetVal); 
    TRACE_PARA(bModule);
    TRACE_PARA(mode);
    TRACE_PARA(con);
    TRACE_FCTE;
    TRACE_END
    
    return uw_RetVal;
}

//***************************************************************************
//
//   Module : APC_API                  Submodule : ApiCmdSetIrigTime
//
//   Author : J. Schuler               Project   : APC_APPL
//
//---------------------------------------------------------------------------
//   Create : 01.06.97   Update : 01.06.97
//---------------------------------------------------------------------------
//    Descriptions
//    ------------
//    Inputs    :
//
//    Outputs   :
//
//    Description :
//
//***************************************************************************

AiReturn ApiCmdSetIrigTime_(AiUInt32 bModule, TY_API_IRIG_TIME *pirig_time)
{
    AiReturn uw_RetVal = API_OK;
    TY_MIL_COM_SYS_IRIG_SET_INPUT         xTimeInput;
    TY_MIL_COM_ACK                        xOutput;

    TY_DEVICE_INFO * pDevice = _ApiGetDeviceInfoPtrByModule( bModule );
    if( pDevice == NULL )
        return API_ERR_NO_MODULE_EXTENSION;

    MIL_COM_INIT( &xTimeInput.cmd, API_STREAM(bModule), 0, MIL_COM_SYS_IRIG_SET, sizeof(xTimeInput), sizeof(xOutput) );

    xTimeInput.day   = pirig_time->day;
    xTimeInput.hour  = pirig_time->hour;
    xTimeInput.min   = pirig_time->minute;

    if( pDevice->x_BoardInfo.x_BoardCapabilities.x_IrigCapabilities.b.b_InstantIrigSetOnInternalMode )
        xTimeInput.sec = pirig_time->second | (pirig_time->microsecond << 8);
    else
        xTimeInput.sec = pirig_time->second;

    uw_RetVal = API_CMD_STRUCT_SYS_IRIG_SET( bModule, &xTimeInput, &xOutput );
        
    return uw_RetVal;
}


AI_LIB_FUNC AiReturn AI_CALL_CONV ApiCmdSetIrigTime(AiUInt32 bModule, TY_API_IRIG_TIME *time)
{
    AiReturn uw_RetVal = API_OK;

    if (NULL == time)
        uw_RetVal = API_ERR_PARAM2_IS_NULL;
    else if (time->day == 0)
        uw_RetVal = API_ERR_IRIG_DAY_NOT_IN_RANGE;
    else if (time->hour > 23)
        uw_RetVal = API_ERR_IRIG_HOUR_NOT_IN_RANGE;
    else if (time->minute > 59)
        uw_RetVal = API_ERR_IRIG_MIN_NOT_IN_RANGE;
    else if (time->second > 59)
        uw_RetVal = API_ERR_IRIG_SEC_NOT_IN_RANGE;
    else
    {
        uw_RetVal = ApiCmdSetIrigTime_(bModule, time);
    }
    
    v_ExamineRetVal( "ApiCmdSetIrigTime", uw_RetVal );
    
    TRACE_BEGIN
    TRACE("        TY_API_IRIG_TIME irig_time;\n");
    TRACE1("        irig_time.day    = 0x%x;\n", time->day);
    TRACE1("        irig_time.hour   = 0x%x;\n", time->hour);
    TRACE1("        irig_time.minute = 0x%x;\n", time->minute);
    TRACE1("        irig_time.second = 0x%x;\n", time->second);
    TRACE_FCTA("ApiCmdSetIrigTime", uw_RetVal); 
    TRACE_PARA(bModule);
    TRACE_RPARE("&irig_time");
    TRACE_FCTE;
    TRACE_END
    
    return uw_RetVal;
}




AiReturn ApiCmdSetIrigStatus_(AiUInt32 bModule, TY_API_IRIG_SOURCE new_source)
{
    AiReturn uw_RetVal = API_OK;
    TY_MIL_COM_SYS_IRIG_SOURCE_SET_INPUT  xSourceInput;
    TY_MIL_COM_ACK                        xOutput;

    TY_DEVICE_INFO * pDevice = _ApiGetDeviceInfoPtrByModule(bModule);
    if (pDevice == NULL)
        return API_ERR_NO_MODULE_EXTENSION;

    MIL_COM_INIT(&xSourceInput.cmd, API_STREAM(bModule), 0, MIL_COM_SYS_IRIG_SOURCE_SET, sizeof(xSourceInput), sizeof(xOutput));

    xSourceInput.source = new_source;

    uw_RetVal = API_CMD_STRUCT_SYS_IRIG_SOURCE_SET(bModule, &xSourceInput, &xOutput);

    return uw_RetVal;
}


AI_LIB_FUNC AiReturn AI_CALL_CONV ApiCmdSetIrigStatus(AiUInt32 bModule, TY_API_IRIG_SOURCE new_source)
{
    AiReturn uw_RetVal = API_OK;

    uw_RetVal = ApiCmdSetIrigStatus_(bModule, new_source);

    v_ExamineRetVal("ApiCmdSetIrigStatus", uw_RetVal);

    TRACE_BEGIN
    TRACE1("new_source = %d;\n", new_source);
    TRACE_FCTA("ApiCmdSetIrigStatus", uw_RetVal);
    TRACE_PARA(bModule);
    TRACE_RPARE("new_source");
    TRACE_FCTE;
    TRACE_END

    return uw_RetVal;
}












//***************************************************************************
//
//   Module : APC_API                  Submodule : ApiCmdGetIrigTime
//
//   Author : J. Schuler               Project   : APC_APPL
//
//---------------------------------------------------------------------------
//   Create : 01.06.97   Update : 01.06.97

//---------------------------------------------------------------------------
//    Descriptions
//    ------------
//    Inputs    :
//
//    Outputs   :
//
//    Description :
//
//***************************************************************************

AiReturn ApiCmdGetIrigTime_(AiUInt32 bModule, TY_API_IRIG_TIME *time)
{
    AiReturn uw_RetVal = API_OK;
    TY_MIL_COM                              xInput;
    TY_MIL_COM_SYS_IRIG_GET_OUTPUT          xTimeOutput;

    MIL_COM_INIT( &xInput, API_STREAM(bModule), 0, MIL_COM_SYS_IRIG_GET, sizeof(xInput), sizeof(xTimeOutput) );

    uw_RetVal = API_CMD_STRUCT_SYS_IRIG_GET( bModule, &xInput, &xTimeOutput );

    time->day          = xTimeOutput.day;
    time->hour         = xTimeOutput.hour;
    time->minute       = xTimeOutput.min;
    time->second       = xTimeOutput.sec;
    time->microsecond  = xTimeOutput.usec;

    return uw_RetVal;
}

AI_LIB_FUNC AiReturn AI_CALL_CONV ApiCmdGetIrigTime(AiUInt32 bModule, TY_API_IRIG_TIME *time)
{
    AiInt16 uw_RetVal = API_OK;

    if (NULL == time)
        uw_RetVal = API_ERR_PARAM2_IS_NULL;
    else
    {
        uw_RetVal = ApiCmdGetIrigTime_(bModule, time);
    }

    v_ExamineRetVal( "ApiCmdGetIrigTime", uw_RetVal );

    if( (Aiul_DbgLevel & DBG_TRACE_PBAPRO) == 0)
    {
        TRACE_BEGIN
        TRACE("        TY_API_IRIG_TIME irig_time;\n");
        TRACE_FCTA("ApiCmdGetIrigTime", uw_RetVal); 
        TRACE_PARA(bModule);
        TRACE_RPARE("&irig_time");
        TRACE_FCTE;
        TRACE1("    --> irig_time.day          = 0x%x\n", time->day);
        TRACE1("    --> irig_time.hour         = 0x%x\n", time->hour);
        TRACE1("    --> irig_time.minute       = 0x%x\n", time->minute);
        TRACE1("    --> irig_time.second       = 0x%x\n", time->second);
        TRACE1("    --> irig_time.microsecond  = 0x%x\n", time->microsecond);
        TRACE_END
    }

    return uw_RetVal;
}
















AiReturn ApiCmdGetIrigStatus_(AiUInt32 bModule, TY_API_IRIG_SOURCE *source, AiBoolean * in_sync)
{
    AiInt16 uw_RetVal = API_OK;
    TY_MIL_COM                              xInput;
    TY_MIL_COM_SYS_IRIG_STATUS_GET_OUTPUT   xStatusOutput;

    MIL_COM_INIT(&xInput, API_STREAM(bModule), 0, MIL_COM_SYS_IRIG_STATUS_GET, sizeof(xInput), sizeof(xStatusOutput));

    uw_RetVal = API_CMD_STRUCT_SYS_IRIG_STATUS_GET(bModule, &xInput, &xStatusOutput);

    *source  = xStatusOutput.src;
    *in_sync = xStatusOutput.sync;

    return uw_RetVal;
}

AI_LIB_FUNC AiReturn AI_CALL_CONV ApiCmdGetIrigStatus(AiUInt32 bModule, TY_API_IRIG_SOURCE *source, AiBoolean * in_sync)
{
    AiInt16 uw_RetVal = API_OK;

    if (NULL == source)
        uw_RetVal = API_ERR_PARAM2_IS_NULL;
    else if (NULL == in_sync)
        uw_RetVal = API_ERR_PARAM3_IS_NULL;
    else
    {
        uw_RetVal = ApiCmdGetIrigStatus_(bModule, source, in_sync);
    }

    v_ExamineRetVal("ApiCmdGetIrigStatus", uw_RetVal);

    if ((Aiul_DbgLevel & DBG_TRACE_PBAPRO) == 0)
    {
        TRACE_BEGIN
        TRACE_FCTA("ApiCmdGetIrigStatus", uw_RetVal);
        TRACE_PARA(bModule);
        TRACE_FCTE;
        TRACE1("    *source          = 0x%x\n", *source );
        TRACE1("    *in_sync         = 0x%x\n", *in_sync);
        TRACE_END
    }

    return uw_RetVal;
}









//***************************************************************************
//
//   Module : APC_API                  Submodule : ApiCmdMilbusProtocol
//
//   Author : Michael Stahl            Project   : API1553-DS
//
//---------------------------------------------------------------------------
//   Create : 01.02.99   Update : 01.02.99
//---------------------------------------------------------------------------
//    Descriptions
//    ------------
//    Inputs    : MILbus Protocol [mil_prot]
//                RT set mode    [mode]
//                RT             [rt]
//
//    Outputs   : Instruction execution status [status]
//
//    Description :
//    This function applies the 'API_DEF_MILBUS_PROT' instruction to the
//    API1553-DS ASP Driver SW to set the MILbus protocol type.
//
//***************************************************************************

AiInt16 ApiCmdDefMilbusProtocol_(AiUInt32 bModule, AiUInt8 biu, AiUInt8 mil_prot, AiUInt8 mode, AiUInt8 rt)
{
    AiInt16 uw_RetVal = API_OK;
    TY_MIL_COM_SYS_MILBUS_PROT_INPUT xInput;
    TY_MIL_COM_ACK                   xOutput;

    MIL_COM_INIT( &xInput.cmd, API_STREAM(bModule), 0, MIL_COM_SYS_MILBUS_PROT, sizeof(xInput), sizeof(xOutput) );

    xInput.prot = mil_prot;
    xInput.mode = mode;
    xInput.rt   = rt;

    uw_RetVal = API_CMD_STRUCT_SYS_MILBUS_PROT( bModule, &xInput, &xOutput );

    return uw_RetVal;
}

AI_LIB_FUNC AiReturn AI_CALL_CONV ApiCmdDefMilbusProtocol(AiUInt32 bModule, AiUInt8 biu, AiUInt8 mil_prot, AiUInt8 mode, AiUInt8 rt)
{
    AiInt16 uw_RetVal = API_OK;

    if( (0 == (bModule & API_STREAM_MASK)) && ((biu < MIN_BIU) || (biu > MAX_BIU)) )
        uw_RetVal = API_ERR_WRONG_BIU;
    else if( mil_prot > MAX_PROTOCOL )
        uw_RetVal = API_ERR_PARAM3_NOT_IN_RANGE;
    else if( mode > MAX_PROTOCOL_MODE )
        uw_RetVal = API_ERR_PARAM4_NOT_IN_RANGE;
    else if( rt > MAX_RT_ADDR )
        uw_RetVal = API_ERR_PARAM5_NOT_IN_RANGE;
    else
    {
        uw_RetVal = ApiCmdDefMilbusProtocol_(bModule, biu, mil_prot, mode, rt);
    }

    v_ExamineRetVal( "ApiCmdDefMilbusProtocol", uw_RetVal );

    TRACE_BEGIN
    TRACE_FCTA("ApiCmdDefMilbusProtocol", uw_RetVal); 
    TRACE_PARA(bModule);
    TRACE_PARA(biu);
    TRACE_PARA(mil_prot);
    TRACE_PARA(mode);
    TRACE_PARE(rt);
    TRACE_FCTE;
    TRACE_END

    return uw_RetVal;
}


//***************************************************************************
//
//   Module : APC_API                  Submodule : ApiCmdSetDbgLevel
//
//   Author : Patrick Giesel           Project   : API3910
//
//---------------------------------------------------------------------------
//   Create : 10.01.2000   Update : 10.01.2000
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
AI_LIB_FUNC AiReturn AI_CALL_CONV ApiSetSysDbgLevel(AiUInt32 bModule, AiUInt32 ul_DbgLevel )
{
    AiInt16 uw_RetVal = API_OK;

    uw_RetVal = _ApiCmdSetSysDbgLevel( bModule, ul_DbgLevel );

    v_ExamineRetVal( "ApiSetSysDbgLevel", uw_RetVal );
    TRACE_BEGIN
        TRACE_FCTA("ApiSetSysDbgLevel", uw_RetVal); 
    TRACE_PARA(bModule);
    TRACE_PARE(ul_DbgLevel);
    TRACE_FCTE;
    TRACE_END
        return(uw_RetVal);
} // end: ApiCmdSetSysDbgLevel







//***************************************************************************
//
//   Module : ApiLsRep                 Submodule : ApiWriteRepData
//
//   Author : P. Giesel                Project   : APC_APPL
//
//---------------------------------------------------------------------------
//   Create : 01.08.99   Update : 31.07.2000
//---------------------------------------------------------------------------
//    Descriptions
//    ------------
//    Inputs    : none
//
//    Outputs   : none
//
//    Description :
//    This function writes data to the API board
//
//***************************************************************************
AiInt16 _ApiWriteRepData(AiUInt32 bModule, AiUInt8 biu, 
                           TY_API_REP_STATUS *api_rep_stat,
                           void *lpBuf,
                           AiUInt32 *lBytesWritten)
{
    AiChar buf[128];

    AiUInt32 loc_bytes_written = 0L;
    TY_API_REP_STATUS tx_RepStatus;
    AiInt16 uw_RetVal = API_OK;
    AiUInt32 ul_CalculatedBiu;
    AiUInt32 ulBlockSize;
    TY_DEVICE_INFO * pDevice = _ApiGetDeviceInfoPtrByModule( bModule );
    AiUInt32 ul_Offset;
    AiUInt32 ul_ReplayBufferSize;
    AiUInt32 ul_BiuOffset;

    if( pDevice == NULL )
        return API_ERR_NO_MODULE_EXTENSION;

    if( (0 == (bModule & API_STREAM_MASK)) && ((biu < MIN_BIU) || (biu > MAX_BIU)) )
        uw_RetVal = API_ERR_WRONG_BIU;
    else if( NULL == api_rep_stat )
        uw_RetVal = API_ERR_PARAM3_IS_NULL;
    else if( NULL == lpBuf )
        uw_RetVal = API_ERR_PARAM4_IS_NULL;
    else if( NULL == lBytesWritten )
        uw_RetVal = API_ERR_PARAM5_IS_NULL;
    else
    {
        uw_ApiGetBiuFromStream( bModule, biu, &ul_CalculatedBiu);

        uw_RetVal = ApiCmdReplayStatus( bModule, biu, &tx_RepStatus );
        if( API_OK == uw_RetVal )
        {
            api_rep_stat->status = tx_RepStatus.status;
            api_rep_stat->rpi_cnt= tx_RepStatus.rpi_cnt;

            if( api_rep_stat->size > 0 )
            {
                ul_BiuOffset = ApiHwBiuCbOffset(&pDevice->x_BoardInfo, pDevice->x_IniPar.prot, ul_CalculatedBiu );

                /* write replay data directly to global RAM */
                if( pDevice->auc_ReplayFlag[ ul_CalculatedBiu-1 ] != 0 )
                {
                    uw_RetVal = _ApiReadMemData(bModule, API_MEMTYPE_GLOBAL, ul_BiuOffset+0xc0, 4, &ul_ReplayBufferSize );

                    ul_Offset = api_rep_stat->saddr + ((ul_ReplayBufferSize & 0x000003FF)+1)*64*1024 / 2;
                    pDevice->auc_ReplayFlag[ ul_CalculatedBiu-1 ] = 0;
                }
                else
                {
                    ul_Offset = api_rep_stat->saddr;
                    pDevice->auc_ReplayFlag[ ul_CalculatedBiu-1 ] = 1;
                }

                sprintf( buf, "[%d] @0x%08X <= 0x%08X bytes [status=0x%08X, RPI=%d]\n", ul_CalculatedBiu-1, ul_Offset, api_rep_stat->size, tx_RepStatus.status, tx_RepStatus.rpi_cnt );
                DEBUGOUT( DBG_WRITEREP, __FUNCTION__, buf );

                if ( GET_SERVER_ID( bModule))
                {
                    /* Workaround for BUG00483: ANET1553 recording underrun with new socket based ANS */
                    *lBytesWritten = 0;
                    ulBlockSize = 0x4000;
                    while( api_rep_stat->size > 0 )
                    {
                        if( api_rep_stat->size < 0x4000 )
                            ulBlockSize = api_rep_stat->size;

                        _ApiWriteBlockMemData(bModule, API_MEMTYPE_GLOBAL, ul_Offset, 1, lpBuf, ulBlockSize, &loc_bytes_written );
                        lpBuf               = (AiUInt8*)lpBuf + ulBlockSize;
                        ul_Offset          += ulBlockSize;
                        *lBytesWritten     += loc_bytes_written;
                        api_rep_stat->size -= ulBlockSize;
                    }
                    /* Workaround end BUG00483*/
                }
                else
                {
                    _ApiWriteBlockMemData(bModule, API_MEMTYPE_GLOBAL, ul_Offset, 1, lpBuf, api_rep_stat->size, &loc_bytes_written );
                    *lBytesWritten = loc_bytes_written;
                }
            }
        }

    }
    v_ExamineRetVal( "ApiWriteRepData", uw_RetVal );

    return(uw_RetVal);
} // end: ApiWriteRepData

AiInt16 _API_DLL_FUNC ApiWriteRepDataF(AiUInt32 bModule, AiUInt8 biu, 
                                       TY_API_REP_STATUS *api_rep_stat,
                                       void *lpBuf,
                                       AiUInt32 *lBytesWritten)
{
    AiInt16 uw_RetVal;

    uw_RetVal = _ApiWriteRepData(bModule, biu, api_rep_stat, lpBuf, lBytesWritten);
    TRACE_BEGIN
        TRACE("        TY_API_REP_STATUS api_rep_stat;\n");
    TRACE("        AiUInt32          lpBuf[<VALUE>];\n");
    TRACE("        AiUInt32          lBytesWritten;\n");
    TRACE1("        api_rep_stat.entry_cnt = 0x%x;\n", api_rep_stat->entry_cnt);
    TRACE1("        api_rep_stat.padding1  = 0x%x;\n", api_rep_stat->padding1);
    TRACE1("        api_rep_stat.padding2  = 0x%x;\n", api_rep_stat->padding2);
    TRACE1("        api_rep_stat.rpi_cnt   = 0x%x;\n", api_rep_stat->rpi_cnt);
    TRACE1("        api_rep_stat.saddr     = 0x%x;\n", api_rep_stat->saddr);
    TRACE1("        api_rep_stat.size      = 0x%x;\n", api_rep_stat->size);
    TRACE1("        api_rep_stat.status    = 0x%x;\n", api_rep_stat->status);
    TRACE_FCTA("ApiWriteRepDataF", uw_RetVal); 
    TRACE_PARA(bModule);
    TRACE_PARA(biu);
    TRACE_RPARA("&api_rep_stat");
    TRACE_RPARA("lpBuf");
    TRACE_RPARE("&lBytesWritten");
    TRACE_FCTE;
    TRACE1("    --> api_rep_stat.entry_cnt = 0x%x\n", api_rep_stat->entry_cnt);
    TRACE1("    --> api_rep_stat.padding1  = 0x%x\n", api_rep_stat->padding1);
    TRACE1("    --> api_rep_stat.padding2  = 0x%x\n", api_rep_stat->padding2);
    TRACE1("    --> api_rep_stat.rpi_cnt   = 0x%x\n", api_rep_stat->rpi_cnt);
    TRACE1("    --> api_rep_stat.saddr     = 0x%x\n", api_rep_stat->saddr);
    TRACE1("    --> api_rep_stat.size      = 0x%x\n", api_rep_stat->size);
    TRACE1("    --> api_rep_stat.status    = 0x%x\n", api_rep_stat->status);
    TRACE1("    --> lBytesWritten          = 0x%x\n", *lBytesWritten);
    TRACE_END

        return(uw_RetVal);
}

AI_LIB_FUNC AiReturn AI_CALL_CONV ApiWriteRepData(AiUInt32 bModule, AiUInt8 biu,
                                                  TY_API_REP_STATUS *api_rep_stat,
                                                  void *lpBuf,
                                                  AiUInt32 *lBytesWritten)
{
    AiInt16 uw_RetVal;

    uw_RetVal = _ApiWriteRepData(bModule, biu, api_rep_stat, lpBuf, lBytesWritten);
    TRACE_BEGIN
        TRACE("        TY_API_REP_STATUS api_rep_stat;\n");
    TRACE("        AiUInt32          lpBuf[<VALUE>];\n");
    TRACE("        AiUInt32          lBytesWritten;\n");
    TRACE1("        api_rep_stat.entry_cnt = 0x%x;\n", api_rep_stat->entry_cnt);
    TRACE1("        api_rep_stat.padding1  = 0x%x;\n", api_rep_stat->padding1);
    TRACE1("        api_rep_stat.padding2  = 0x%x;\n", api_rep_stat->padding2);
    TRACE1("        api_rep_stat.rpi_cnt   = 0x%x;\n", api_rep_stat->rpi_cnt);
    TRACE1("        api_rep_stat.saddr     = 0x%x;\n", api_rep_stat->saddr);
    TRACE1("        api_rep_stat.size      = 0x%x;\n", api_rep_stat->size);
    TRACE1("        api_rep_stat.status    = 0x%x;\n", api_rep_stat->status);
    TRACE_FCTA("ApiWriteRepData", uw_RetVal); 
    TRACE_PARA(bModule);
    TRACE_PARA(biu);
    TRACE_RPARA("&api_rep_stat");
    TRACE_RPARA("lpBuf");
    TRACE_RPARE("&lBytesWritten");
    TRACE_FCTE;
    TRACE1("    --> api_rep_stat.entry_cnt = 0x%x\n", api_rep_stat->entry_cnt);
    TRACE1("    --> api_rep_stat.padding1  = 0x%x\n", api_rep_stat->padding1);
    TRACE1("    --> api_rep_stat.padding2  = 0x%x\n", api_rep_stat->padding2);
    TRACE1("    --> api_rep_stat.rpi_cnt   = 0x%x\n", api_rep_stat->rpi_cnt);
    TRACE1("    --> api_rep_stat.saddr     = 0x%x\n", api_rep_stat->saddr);
    TRACE1("    --> api_rep_stat.size      = 0x%x\n", api_rep_stat->size);
    TRACE1("    --> api_rep_stat.status    = 0x%x\n", api_rep_stat->status);
    TRACE1("    --> lBytesWritten          = 0x%x\n", *lBytesWritten);
    TRACE_END

        return(uw_RetVal);
}


//***************************************************************************
//
//   Module : ApiLsBM                  Submodule : ApiReadRecData
//
//   Author : Rolf Heitzmann           Project   : APC_APPL
//
//---------------------------------------------------------------------------
//   Create : 01.02.99   Update : 31.03.00 
//---------------------------------------------------------------------------
//    Descriptions
//    ------------
//    Inputs    : none
//
//    Outputs   : none
//
//    Description :
//    This function reads data from the APC board
//
//***************************************************************************
AI_LIB_FUNC AiReturn AI_CALL_CONV ApiReadRecData(AiUInt32 bModule, AiUInt8 biu,
                                                 TY_API_BM_REC *api_rec_stat,
                                                 void *lpBuf,
                                                 AiUInt32 *lBytesRead)
{
    AiUInt32 loc_bytes_read;
    TY_API_BM_REC rec_stat;
    AiInt16 uw_RetVal = API_OK;
    TY_DEVICE_INFO * pDevice = _ApiGetDeviceInfoPtrByModule( bModule );

    if( pDevice == NULL )
        return API_ERR_NO_MODULE_EXTENSION;

    if( (0 == (bModule & API_STREAM_MASK)) && ((biu < MIN_BIU) || (biu > MAX_BIU)) )
        uw_RetVal = API_ERR_WRONG_BIU;
    else if( NULL == api_rec_stat )
        uw_RetVal = API_ERR_PARAM3_IS_NULL;
    else if( NULL == lpBuf )
        uw_RetVal = API_ERR_PARAM4_IS_NULL;
    else if( NULL == lBytesRead )
        uw_RetVal = API_ERR_PARAM5_IS_NULL;
    else
    {
        loc_bytes_read = 0L;

        rec_stat.status  = 0;
        rec_stat.hfi_cnt = 0L;
        rec_stat.saddr   = 0L;
        rec_stat.size    = 0L;

        {
            /* Set Mutex */
            _SyncWait(bModule, pDevice->_hRecMutex);

            ApiCmdBMRecStatusRead(bModule, biu, &rec_stat);

            if (rec_stat.size != 0L)
            {
                uw_RetVal = ApiCmdBMCopyRecDat(bModule, biu, &rec_stat);
                if( API_OK == uw_RetVal )
                {
                    uw_RetVal = ApiReadBlockMemData( bModule, API_MEMTYPE_SHARED, 0x10000, 1, lpBuf, rec_stat.size*4L, &loc_bytes_read );
                }
            }

            /* Release Mutex */
            _SyncRelease(bModule, pDevice->_hRecMutex);

            *lBytesRead = loc_bytes_read;

            api_rec_stat->status  = rec_stat.status;
            api_rec_stat->hfi_cnt = rec_stat.hfi_cnt;
            api_rec_stat->saddr   = rec_stat.saddr;
            api_rec_stat->size    = rec_stat.size;
        }
    }
    v_ExamineRetVal( "ApiReadRecData", uw_RetVal );
    TRACE_BEGIN
        TRACE("        TY_API_BM_REC api_rec_stat;\n");
    TRACE("        AiUInt32      lpBuf[<VALUE>];\n");
    TRACE("        AiUInt32      lBytesRead;\n");
    TRACE1("        api_rec_stat.hfi_cnt   = 0x%x;\n",  api_rec_stat->hfi_cnt);
    TRACE1("        api_rec_stat.padding1  = 0x%x;\n", api_rec_stat->padding1);
    TRACE1("        api_rec_stat.padding2  = 0x%x;\n", api_rec_stat->padding2);
    TRACE1("        api_rec_stat.saddr     = 0x%x;\n", api_rec_stat->saddr);
    TRACE1("        api_rec_stat.size      = 0x%x;\n", api_rec_stat->size);
    TRACE1("        api_rec_stat.status    = 0x%x;\n", api_rec_stat->status);
    TRACE_FCTA("ApiReadRecData", uw_RetVal); 
    TRACE_PARA(bModule);
    TRACE_PARA(biu);
    TRACE_RPARA("&api_rep_stat");
    TRACE_RPARA("lpBuf");
    TRACE_RPARE("&lBytesRead");
    TRACE_FCTE;
    TRACE1("    --> api_rep_stat.hfi_cnt  = 0x%x\n", api_rec_stat->hfi_cnt);
    TRACE1("    --> api_rep_stat.padding1 = 0x%x\n", api_rec_stat->padding1);
    TRACE1("    --> api_rep_stat.padding2 = 0x%x\n", api_rec_stat->padding2);
    TRACE1("    --> api_rep_stat.saddr    = 0x%x\n", api_rec_stat->saddr);
    TRACE1("    --> api_rep_stat.size     = 0x%x\n", api_rec_stat->size);
    TRACE1("    --> api_rep_stat.status   = 0x%x\n", api_rec_stat->status);
    TRACE1("    --> lBytesRead            = 0x%x\n", *lBytesRead);
    TRACE_END
        return(uw_RetVal);
} // end: ApiReadRecData

//***************************************************************************
//
//   Module : ApiLsSys                 Submodule : ApiCmdSystagDef
//
//   Author : P. Giesel                Project   : API
//
//---------------------------------------------------------------------------
//   Create : 31.07.2000   Update : 31.07.2000 
//---------------------------------------------------------------------------
//    Descriptions
//    ------------
//    Inputs    : none
//
//    Outputs   : none
//
//    Description :
//    This function defines the system dynamic data.
//
//***************************************************************************

AiInt16 ApiCmdSystagDef_( AiUInt32 bModule, AiUInt8 biu, AiUInt8 id, AiUInt8 con, AiUInt8 mode, TY_API_SYSTAG *psystag )
{
    AiInt16 uw_RetVal = API_OK;
    TY_MIL_COM_SYS_SYSTAG_DEF_INPUT xInput;
    TY_MIL_COM_ACK                  xOutput;

    MIL_COM_INIT( &xInput.cmd, API_STREAM(bModule), 0, MIL_COM_SYS_SYSTAG_DEF, sizeof(xInput), sizeof(xOutput) );

    xInput.id        = id;
    xInput.con       = con;
    xInput.mode      = mode;
    xInput.xid_rtsa  = psystag->xid_rtsa;
    xInput.fct       = psystag->fct;
    xInput.min       = psystag->min;
    xInput.max       = psystag->max;
    xInput.step      = psystag->step;
    xInput.wpos      = psystag->wpos;
    xInput.bpos_nbit = psystag->bpos;

    uw_RetVal = API_CMD_STRUCT_SYS_SYSTAG_DEF( bModule, &xInput, &xOutput );

    return uw_RetVal;
}

AI_LIB_FUNC AiReturn AI_CALL_CONV ApiCmdSystagDef( AiUInt32 bModule, AiUInt8 biu,
                                                  AiUInt8 id, AiUInt8 con, AiUInt8 mode, TY_API_SYSTAG *psystag )
{
    AiInt16 uw_RetVal = API_OK;

    if( (0 == (bModule & API_STREAM_MASK)) && ((biu < MIN_BIU) || (biu > MAX_BIU)) )
        uw_RetVal = API_ERR_WRONG_BIU;
    else if( id < MIN_SYSTAG_ID )
        uw_RetVal = API_ERR_PARAM3_NOT_IN_RANGE;
    else if( con > API_ENA_INIT )
        uw_RetVal = API_ERR_PARAM4_NOT_IN_RANGE;
    else if( (0 == mode) || (mode > MAX_SYSTAG_MODE) )
        uw_RetVal = API_ERR_PARAM5_NOT_IN_RANGE;
    else if( NULL == psystag )
        uw_RetVal = API_ERR_PARAM6_IS_NULL;
    else if( (API_BC_MODE == mode) &&
        ((((psystag->xid_rtsa & API_SYSTAG_XFERID_MASK) >> API_SYSTAG_XFERID_POS) < MIN_XFER_ID) ||
        (((psystag->xid_rtsa & API_SYSTAG_XFERID_MASK) >> API_SYSTAG_XFERID_POS) > MAX_XFER_ID)) )
        uw_RetVal = API_ERR_SYSTAG_XIDRTMID_NOT_IN_RANGE;
    else if( (API_RT_MODE == mode) &&
        (((psystag->xid_rtsa & API_SYSTAG_RTADDR_MASK) >> API_SYSTAG_RTADDR_POS) > MAX_RT_ADDR) )
        uw_RetVal = API_ERR_SYSTAG_XIDRTMID_NOT_IN_RANGE;
    else if( (API_RT_MODE == mode) &&
        (((psystag->xid_rtsa & API_SYSTAG_SA_MID_MASK) >> API_SYSTAG_SA_MID_POS) > MAX_RT_MODECODE) )
        uw_RetVal = API_ERR_SYSTAG_XIDRTMID_NOT_IN_RANGE;
    else if( psystag->fct > MAX_SYSTAG_FCT )
        uw_RetVal = API_ERR_SYSTAG_FCT_NOT_IN_RANGE;
    else if( ((API_SYSTAG_FCT_STATES == psystag->fct) || (API_SYSTAG_FCT_DATASET == psystag->fct)) &&
        (psystag->min > MAX_DATASET_BUF_ID) )
        uw_RetVal = API_ERR_SYSTAG_MIN_NOT_IN_RANGE;
    else if( (API_SYSTAG_FCT_COMP == psystag->fct) &&
        (psystag->min > MAX_WPOS) )
        uw_RetVal = API_ERR_SYSTAG_MIN_NOT_IN_RANGE;
    else if( (API_SYSTAG_FCT_STATES == psystag->fct) &&
        ((psystag->max < MIN_DATA_WORDS) ||
        (psystag->max > MAX_DATA_WORDS)) )
        uw_RetVal = API_ERR_SYSTAG_MAX_NOT_IN_RANGE;
    else if( (API_SYSTAG_FCT_DATASET == psystag->fct) &&
        (psystag->step > MAX_SYSTAG_STEP_FCT5) )
        uw_RetVal = API_ERR_SYSTAG_STEP_NOT_IN_RANGE;
    else if( (API_SYSTAG_FCT_STATES == psystag->fct) &&
        (psystag->step > MAX_SYSTAG_STEP_FCT6) )
        uw_RetVal = API_ERR_SYSTAG_STEP_NOT_IN_RANGE;
    else if( (API_SYSTAG_FCT_COMP == psystag->fct) &&
        ((psystag->step < MIN_SYSTAG_STEP_FCT7) ||
        (psystag->step > MAX_SYSTAG_STEP_FCT7)) )
        uw_RetVal = API_ERR_SYSTAG_STEP_NOT_IN_RANGE;
    else if( (API_SYSTAG_FCT_DATASET != psystag->fct) &&
        (psystag->wpos > MAX_SYSTAG_WPOS) )
        uw_RetVal = API_ERR_SYSTAG_WPOS_NOT_IN_RANGE;
    else if( (API_SYSTAG_FCT_DATASET > psystag->fct) &&
        (((psystag->bpos & API_SYSTAG_BITPOS_MASK) >> API_SYSTAG_BITPOS_POS) > MAX_DATA_BITPOS) )
        uw_RetVal = API_ERR_SYSTAG_BPOS_NOT_IN_RANGE;
    else if( (API_SYSTAG_FCT_DATASET > psystag->fct) &&
        (((psystag->bpos & API_SYSTAG_BITNB_MASK) >> API_SYSTAG_BITNB_POS) > MAX_DATA_BITLEN) )
        uw_RetVal = API_ERR_SYSTAG_BPOS_NOT_IN_RANGE;
    else if( (API_SYSTAG_FCT_NEG_TRIANGLE < psystag->fct) && (psystag->bpos != 0) )
        uw_RetVal = API_ERR_SYSTAG_BPOS_NOT_IN_RANGE;
    else
    {
        uw_RetVal = ApiCmdSystagDef_( bModule, biu, id, con, mode, psystag );
    }

    v_ExamineRetVal( "ApiCmdSystagDef", uw_RetVal );

    TRACE_BEGIN
    TRACE("        TY_API_SYSTAG systag;\n");
    TRACE1("        systag.bpos     = 0x%x;\n",  psystag->bpos);
    TRACE1("        systag.fct      = 0x%x;\n",  psystag->fct);
    TRACE1("        systag.max      = 0x%x;\n",  psystag->max);
    TRACE1("        systag.min      = 0x%x;\n",  psystag->min);
    TRACE1("        systag.step     = 0x%x;\n",  psystag->step);
    TRACE1("        systag.wpos     = 0x%x;\n",  psystag->wpos);
    TRACE1("        systag.xid_rtsa = 0x%x;\n",  psystag->xid_rtsa);
    TRACE_FCTA("ApiCmdSystagDef", uw_RetVal); 
    TRACE_PARA(bModule);
    TRACE_PARA(biu);
    TRACE_PARA(id);
    TRACE_PARA(con);
    TRACE_PARA(mode);
    TRACE_RPARE("&systag");
    TRACE_FCTE;
    TRACE_END

    return uw_RetVal;
}


//***************************************************************************
//
//   Module : ApiLsSys                 Submodule : ApiCmdSystagCon
//
//   Author : P. Giesel                Project   : API
//
//---------------------------------------------------------------------------
//   Create : 31.07.2000   Update : 31.07.2000   
//---------------------------------------------------------------------------
//    Descriptions
//    ------------
//    Inputs    : none
//
//    Outputs   : none
//
//    Description :
//    This function controls the system dynamic data.
//
//***************************************************************************
AiInt16 ApiCmdSystagCon_( AiUInt32 bModule, AiUInt8 biu, AiUInt8 id, AiUInt8 con )
{
    AiInt16 uw_RetVal = API_OK;
    TY_MIL_COM_SYS_SYSTAG_CON_INPUT xInput;
    TY_MIL_COM_ACK                  xOutput;

    MIL_COM_INIT( &xInput.cmd, API_STREAM(bModule), 0, MIL_COM_SYS_SYSTAG_CON, sizeof(xInput), sizeof(xOutput) );

    xInput.id   = id;
    xInput.con  = con;

    uw_RetVal = API_CMD_STRUCT_SYS_SYSTAG_CON( bModule, &xInput, &xOutput );

    v_ExamineRetVal( "ApiCmdSystagCon", uw_RetVal );

    TRACE_BEGIN
    TRACE_FCTA("ApiCmdSystagCon", uw_RetVal); 
    TRACE_PARA(bModule);
    TRACE_PARA(biu);
    TRACE_PARA(id);
    TRACE_PARE(con);
    TRACE_FCTE;
    TRACE_END

    return uw_RetVal;
}

AI_LIB_FUNC AiReturn AI_CALL_CONV ApiCmdSystagCon( AiUInt32 bModule, AiUInt8 biu,
                                                  AiUInt8 id, AiUInt8 con )
{
    AiInt16 uw_RetVal = API_OK;
    if( (0 == (bModule & API_STREAM_MASK)) && ((biu < MIN_BIU) || (biu > MAX_BIU)) )
        uw_RetVal = API_ERR_WRONG_BIU;
    else if( id < MIN_SYSTAG_ID )
        uw_RetVal = API_ERR_PARAM3_NOT_IN_RANGE;
    else if( con > MAX_SYSTAG_CON )
        uw_RetVal = API_ERR_PARAM4_NOT_IN_RANGE;
    else
    {
        uw_RetVal = ApiCmdSystagCon_(bModule, biu, id, con);
    }

    v_ExamineRetVal( "ApiCmdSystagCon", uw_RetVal );

    TRACE_BEGIN
    TRACE_FCTA("ApiCmdSystagCon", uw_RetVal); 
    TRACE_PARA(bModule);
    TRACE_PARA(biu);
    TRACE_PARA(id);
    TRACE_PARE(con);
    TRACE_FCTE;
    TRACE_END

    return uw_RetVal;
}



//***************************************************************************
//
//   Module : ApiLsSys
//   Author : Martin Haag
//
//---------------------------------------------------------------------------
//   Create : 23.08.2010
//---------------------------------------------------------------------------
//    Descriptions
//    ------------
//    Inputs    : Long words
//    Outputs   : Reversed Input
//
//    Description :
//    This functions shows how to use the new struct based 
//    host to target communication. With variable argument size.
//***************************************************************************

AI_LIB_FUNC AiReturn AI_CALL_CONV ApiCmdSysPingVariable( AiUInt32 ulModHandle, AiUInt32 ulInputCount, AiUInt32 ulInput[], AiUInt32 ulOutput[] )
{
    AiInt16                   wRetVal  = API_OK;
    TY_MIL_COM_SYS_PING_INPUT   * pxInput  = NULL;
    TY_MIL_COM_SYS_PING_OUTPUT  * pxOutput = NULL;
    AiUInt32 i;

    AiUInt32 ulInputSize  = sizeof(TY_MIL_COM_SYS_PING_INPUT)  +  ulInputCount * sizeof(AiUInt32);
    AiUInt32 ulOutputSize = sizeof(TY_MIL_COM_SYS_PING_OUTPUT) +  ulInputCount * sizeof(AiUInt32);

    // For commands with variable argument sizes we need to allocate the 
    // input and output on the heap. For fixed size argument commands we can
    // define input and output on the stack
    pxInput  = (TY_MIL_COM_SYS_PING_INPUT   *)AiOsMalloc( ulInputSize  );
    pxOutput = (TY_MIL_COM_SYS_PING_OUTPUT  *)AiOsMalloc( ulOutputSize );

    // -- initialize command ---

    MIL_COM_INIT( &pxInput->xCommand, API_STREAM(ulModHandle), 0, MIL_COM_SYS_PING, ulInputSize, ulOutputSize );


    for( i=0; i<ulInputCount; i++ )
        pxInput->aulData[i] = ulInput[i];

    pxInput->ulDataCount = ulInputCount;

    wRetVal = API_CMD_STRUCT_SYS_PING( ulModHandle, pxInput, pxOutput );

    if( wRetVal != API_OK )
    {
        AiOsFree(pxInput);
        AiOsFree(pxOutput);
        return wRetVal;
    }

    for( i=0; i < ulInputCount; i++ )
        ulOutput[i] = pxOutput->aulData[i];

    AiOsFree(pxInput);
    AiOsFree(pxOutput);

    return wRetVal;
}

//***************************************************************************
//
//   Module : ApiLsSys
//   Author : Martin Haag
//
//---------------------------------------------------------------------------
//   Create : 23.08.2010
//---------------------------------------------------------------------------
//    Descriptions
//    ------------
//    Inputs    : Long words
//    Outputs   : Reversed Input
//
//    Description :
//    This functions shows how to use the new struct based 
//    host to target communication. With fixed argument size
//***************************************************************************

AI_LIB_FUNC AiReturn AI_CALL_CONV ApiCmdSysPing( AiUInt32 ulModHandle, AiUInt32 ulInput, AiUInt32 * pulOutput )
{
    AiInt16                 wRetVal  = API_OK;
    TY_MIL_COM_SYS_PING_INPUT   xInput;
    TY_MIL_COM_SYS_PING_OUTPUT  xOutput;


    // -- initialize command ---
    MIL_COM_INIT( &xInput.xCommand, API_STREAM(ulModHandle), 0, MIL_COM_SYS_PING, sizeof(xInput), sizeof(xOutput) );

    xInput.ulDataCount = 1;

    xInput.aulData[0] = ulInput;

    wRetVal = API_CMD_STRUCT_SYS_PING( ulModHandle, (TY_MIL_COM*)&xInput, (TY_MIL_COM_ACK*)&xOutput );

    if( wRetVal != 0 )
        // Error while executing target command
        return wRetVal;

    if( pulOutput )
        *pulOutput = xOutput.aulData[0];

    return wRetVal;
}


//***************************************************************************
//
//   Module : ApiLsSys
//   Author : Martin Haag
//
//---------------------------------------------------------------------------
//   Create : 20.12.2010
//---------------------------------------------------------------------------
//    Descriptions
//    ------------
//
//    Description : Allocate target memory
//    
//***************************************************************************
AI_LIB_FUNC AiReturn AI_CALL_CONV ApiCmdSysMalloc( AiUInt32 ulModule, AiUInt8 ucMemtype, AiUInt32 ulSize, AiUInt32 ulTag, AiUInt32 * pulOffset )
{
    AiInt16                       wRetVal  = API_OK;
    TY_MIL_COM_SYS_MALLOC_INPUT   xInput;
    TY_MIL_COM_SYS_MALLOC_OUTPUT  xOutput;


    // -- initialize command ---
    MIL_COM_INIT( &xInput.xCommand, API_STREAM(ulModule), 0, MIL_COM_SYS_MALLOC, sizeof(xInput), sizeof(xOutput) );

    xInput.ulMemory = ucMemtype;
    xInput.ulSize   = ulSize;
    xInput.ulTag    = ulTag;

    wRetVal = API_CMD_STRUCT_SYS_MALLOC( ulModule, &xInput, &xOutput );

    if( pulOffset )
        *pulOffset = xOutput.ulOffset;

    return wRetVal;
}

//***************************************************************************
//
//   Module : ApiLsSys
//   Author : Martin Haag
//
//---------------------------------------------------------------------------
//   Create : 20.12.2010
//---------------------------------------------------------------------------
//    Descriptions
//    ------------
//
//    Description : Free target memory
//    
//***************************************************************************
AI_LIB_FUNC AiReturn AI_CALL_CONV ApiCmdSysFree( AiUInt32 ulModule, AiUInt8 ucMemtype, AiUInt32 ulOffset, AiUInt32 ulTag )
{
    AiInt16                 wRetVal  = API_OK;
    TY_MIL_COM_SYS_FREE_INPUT   xInput;
    TY_MIL_COM_ACK              xOutput;

    // -- initialize command ---
    MIL_COM_INIT( &xInput.xCommand, API_STREAM(ulModule), 0, MIL_COM_SYS_FREE, sizeof(xInput), sizeof(xOutput) );

    xInput.ulMemory  = ucMemtype;
    xInput.ulOffset  = ulOffset;
    xInput.ulTag     = ulTag;

    wRetVal = API_CMD_STRUCT_SYS_FREE( ulModule, &xInput, &xOutput );

    return wRetVal;
}


//***************************************************************************
//
//   Module : ApiLsSys
//
//---------------------------------------------------------------------------
//    Descriptions
//    ------------
//
//    Description : Gets information about the board from the target software
//    
//***************************************************************************

AI_LIB_FUNC AiReturn AI_CALL_CONV ApiCmdSysGetBoardInfo_(AiUInt32 ulModHandle, AiUInt32 ulDataStart, AiUInt32 ulDataCount, AiUInt32 *aulOutput, AiUInt32 *ulOutputCount)
{
    AiInt16                       wRetVal  = API_OK;
    TY_MIL_COM_SYS_GET_INFO_INPUT     xInput;
    TY_MIL_COM_SYS_GET_INFO_OUTPUT  * pxOutput = NULL;
    AiUInt32 i;

    /* prepare input and output */
    AiUInt32 ulInputSize  = sizeof(xInput);
    AiUInt32 ulOutputSize = sizeof(TY_MIL_COM_SYS_GET_INFO_OUTPUT) +  (ulDataCount * sizeof(AiUInt32));
    pxOutput = (TY_MIL_COM_SYS_GET_INFO_OUTPUT  *)AiOsMalloc(ulOutputSize);

    /* -- initialize command --- */
    MIL_COM_INIT(&xInput.xCommand, API_STREAM(ulModHandle), 0, MIL_COM_SYS_GET_BOARD_INFO, ulInputSize, ulOutputSize);

    xInput.ulDataCount = ulDataCount;
    xInput.ulDataStart = ulDataStart;

    wRetVal = API_CMD_STRUCT_SYS_GET_BOARD_INFO(ulModHandle, &xInput, pxOutput);

    if (wRetVal != API_OK)
    {
        AiOsFree(pxOutput);
        return wRetVal;
    }


    for (i=0; i < pxOutput->ulDataCount; i++)
        aulOutput[i] = pxOutput->aulData[i];

    if (ulOutputCount) *ulOutputCount = pxOutput->ulDataCount;

    /* clean up */
    AiOsFree(pxOutput);


    return wRetVal;
}


//***************************************************************************
//
//   Module : ApiLsSys
//
//---------------------------------------------------------------------------
//    Descriptions
//    ------------
//
//    Description : Gets information about the board from the target software
//    
//***************************************************************************

AI_LIB_FUNC AiReturn AI_CALL_CONV ApiCmdSysGetBoardInfo(AiUInt32 ulModHandle, AiUInt32 ulDataStart, AiUInt32 ulDataCount, AiUInt32 *aulOutput, AiUInt32 *ulOutputCount)
{
    AiReturn ret = API_OK;

    if (Aiul_DbgLevel & DBG_PARAMCHK)
    {
        if (0 == ulDataCount)
            ret = API_ERR_PARAM2_NOT_IN_RANGE;
        else if (NULL == aulOutput)
            ret = API_ERR_PARAM3_IS_NULL;
    }

    if (API_OK == ret)
    {
        ret = ApiCmdSysGetBoardInfo_(ulModHandle, ulDataStart, ulDataCount, aulOutput, ulOutputCount);
    }

    v_ExamineRetVal("ApiCmdSysGetBoardInfo", ret);

    return ret;
}



//***************************************************************************
//
//   Module : ApiLsSys                 Submodule : ApiCmdSysGetMemPArtition
//
//   Author : Marco Motz               Project   : API1553-DS
//
//---------------------------------------------------------------------------
//   Create : 28.08.02   Update : 28.08.02
//---------------------------------------------------------------------------
//    Descriptions
//    ------------
//    Inputs    : 
//
//    Outputs   : Instruction execution status [status]
//
//    Description :
//    This function applies the 'API_SYS_GET_MEM_PARTITION' instruction to the API1553-DS
//    ASP Driver SW to return the ASP target software board memory layout.
//
//***************************************************************************

AiInt16 ApiCmdSysGetMemPartition_(AiUInt32 bModule, AiUInt8 mode, TY_API_GET_MEM_INFO *mem_info)
{
    AiInt16  uw_RetVal = API_OK;

    TY_MIL_COM_SYS_MEM_PART_GET_INPUT  xInput;
    TY_MIL_COM_SYS_MEM_PART_GET_OUTPUT xOutput;

    xInput.mode = mode;

    MIL_COM_INIT( &xInput.cmd, API_STREAM(bModule), 0, MIL_COM_SYS_MEM_PART_GET, sizeof(xInput), sizeof(xOutput) );

    uw_RetVal = API_CMD_STRUCT_SYS_MEM_PART_GET( bModule, &xInput, &xOutput );
    if( API_OK == uw_RetVal )
    {
        memcpy( mem_info, &xOutput.mem_info, sizeof(TY_API_GET_MEM_INFO) );
    }

    return uw_RetVal;

}

AI_LIB_FUNC AiReturn AI_CALL_CONV ApiCmdSysGetMemPartition(AiUInt32 bModule, AiUInt8 mode, TY_API_GET_MEM_INFO *mem_info)
{
    AiUInt32 i         = 0;
    AiInt16  uw_RetVal = API_OK;

    if( NULL == mem_info )
        uw_RetVal = API_ERR_PARAM3_IS_NULL;
    else 
    {
        uw_RetVal = ApiCmdSysGetMemPartition_(bModule, mode, mem_info);
    }

    v_ExamineRetVal( "ApiCmdSysGetMemPartition", uw_RetVal );

    TRACE_BEGIN
    TRACE("        TY_API_GET_MEM_INFO  mem_info;\n");
    TRACE_FCTA("ApiCmdSysGetMemPartition", uw_RetVal); 
    TRACE_PARA(bModule);
    TRACE_PARA(mode);
    TRACE_RPARE("&mem_info");
    TRACE_FCTE;
    TRACE1("    --> mem_info.aul_GlobalMemSize[0]            = 0x%x\n", mem_info->aul_GlobalMemSize[0]);
    TRACE1("    --> mem_info.aul_GlobalMemSize[1]            = 0x%x\n", mem_info->aul_GlobalMemSize[1]);
    for(i=0; i<MAX_API_STREAM; i++)
    {
        TRACE1("    --> mem_info.ax_BiuAddr[%02d].ul_BcAcycInstr = ", i); TRACE2("0x%x\n", mem_info->ax_BiuAddr[i].ul_BcAcycInstr);
        TRACE1("    --> mem_info.ax_BiuAddr[%02d].ul_BcBhArea    = ", i); TRACE2("0x%x\n", mem_info->ax_BiuAddr[i].ul_BcBhArea);
        TRACE1("    --> mem_info.ax_BiuAddr[%02d].ul_BcEqArea    = ", i); TRACE2("0x%x\n", mem_info->ax_BiuAddr[i].ul_BcEqArea);
        TRACE1("    --> mem_info.ax_BiuAddr[%02d].ul_BcHipInstr  = ", i); TRACE2("0x%x\n", mem_info->ax_BiuAddr[i].ul_BcHipInstr);
        TRACE1("    --> mem_info.ax_BiuAddr[%02d].ul_BcLipInstr  = ", i); TRACE2("0x%x\n", mem_info->ax_BiuAddr[i].ul_BcLipInstr);
        TRACE1("    --> mem_info.ax_BiuAddr[%02d].ul_BcSqArea    = ", i); TRACE2("0x%x\n", mem_info->ax_BiuAddr[i].ul_BcSqArea);
        TRACE1("    --> mem_info.ax_BiuAddr[%02d].ul_BcXferDesc  = ", i); TRACE2("0x%x\n", mem_info->ax_BiuAddr[i].ul_BcXferDesc);
        TRACE1("    --> mem_info.ax_BiuAddr[%02d].ul_BmAct       = ", i); TRACE2("0x%x\n", mem_info->ax_BiuAddr[i].ul_BmAct);
        TRACE1("    --> mem_info.ax_BiuAddr[%02d].ul_BmBuf       = ", i); TRACE2("0x%x\n", mem_info->ax_BiuAddr[i].ul_BmBuf);
        TRACE1("    --> mem_info.ax_BiuAddr[%02d].ul_BmTcb       = ", i); TRACE2("0x%x\n", mem_info->ax_BiuAddr[i].ul_BmTcb);
        TRACE1("    --> mem_info.ax_BiuAddr[%02d].ul_Cb          = ", i); TRACE2("0x%x\n", mem_info->ax_BiuAddr[i].ul_Cb);
        TRACE1("    --> mem_info.ax_BiuAddr[%02d].ul_IrLog       = ", i); TRACE2("0x%x\n", mem_info->ax_BiuAddr[i].ul_IrLog);
        TRACE1("    --> mem_info.ax_BiuAddr[%02d].ul_RepBuf      = ", i); TRACE2("0x%x\n", mem_info->ax_BiuAddr[i].ul_RepBuf);
        TRACE1("    --> mem_info.ax_BiuAddr[%02d].ul_RtBhArea    = ", i); TRACE2("0x%x\n", mem_info->ax_BiuAddr[i].ul_RtBhArea);
        TRACE1("    --> mem_info.ax_BiuAddr[%02d].ul_RtDesc      = ", i); TRACE2("0x%x\n", mem_info->ax_BiuAddr[i].ul_RtDesc);
        TRACE1("    --> mem_info.ax_BiuAddr[%02d].ul_RtEqArea    = ", i); TRACE2("0x%x\n", mem_info->ax_BiuAddr[i].ul_RtEqArea);
        TRACE1("    --> mem_info.ax_BiuAddr[%02d].ul_RtSaDesc    = ", i); TRACE2("0x%x\n", mem_info->ax_BiuAddr[i].ul_RtSaDesc);
        TRACE1("    --> mem_info.ax_BiuAddr[%02d].ul_RtSqArea    = ", i); TRACE2("0x%x\n", mem_info->ax_BiuAddr[i].ul_RtSqArea);
    }
    for(i=0; i<2; i++)
    {
        TRACE1("    --> mem_info.x_Sim[%02d].ul_BufBaseAddr     = ", i); TRACE2("0x%x\n", mem_info->x_Sim[i].ul_BufBaseAddr);
        TRACE1("    --> mem_info.x_Sim[%02d].ul_BufCount        = ", i); TRACE2("0x%x\n", mem_info->x_Sim[i].ul_BufCount);
        TRACE1("    --> mem_info.x_Sim[%02d].ul_BufSize         = ", i); TRACE2("0x%x\n", mem_info->x_Sim[i].ul_BufSize);
        TRACE1("    --> mem_info.x_Sim[%02d].ul_HsBufBaseAddr   = ", i); TRACE2("0x%x\n", mem_info->x_Sim[i].ul_HsBufBaseAddr);
        TRACE1("    --> mem_info.x_Sim[%02d].ul_HsBufSize       = ", i); TRACE2("0x%x\n", mem_info->x_Sim[i].ul_HsBufSize);
        TRACE1("    --> mem_info.x_Sim[%02d].ul_HsRes           = ", i); TRACE2("0x%x\n", mem_info->x_Sim[i].ul_HsRes);
    }
    for(i=0; i<MAX_API_STREAM; i++)
    {
        TRACE1("    --> mem_info.ax_BiuCnt[%02d].ul_BcBhArea    = ", i); TRACE2("0x%x\n", mem_info->ax_BiuCnt[i].ul_BcBhArea);
        TRACE1("    --> mem_info.ax_BiuCnt[%02d].ul_BcEqArea    = ", i); TRACE2("0x%x\n", mem_info->ax_BiuCnt[i].ul_BcEqArea);
        TRACE1("    --> mem_info.ax_BiuCnt[%02d].ul_BcSqArea    = ", i); TRACE2("0x%x\n", mem_info->ax_BiuCnt[i].ul_BcSqArea);
        TRACE1("    --> mem_info.ax_BiuCnt[%02d].ul_BcXferDesc  = ", i); TRACE2("0x%x\n", mem_info->ax_BiuCnt[i].ul_BcXferDesc);
        TRACE1("    --> mem_info.ax_BiuCnt[%02d].ul_RtBhArea    = ", i); TRACE2("0x%x\n", mem_info->ax_BiuCnt[i].ul_RtBhArea);
        TRACE1("    --> mem_info.ax_BiuCnt[%02d].ul_RtEqArea    = ", i); TRACE2("0x%x\n", mem_info->ax_BiuCnt[i].ul_RtEqArea);
        TRACE1("    --> mem_info.ax_BiuCnt[%02d].ul_RtSqArea    = ", i); TRACE2("0x%x\n", mem_info->ax_BiuCnt[i].ul_RtSqArea);
    }
    for(i=0; i<MAX_API_STREAM; i++)
    {
        TRACE1("    --> mem_info.ax_BiuSize[%02d].ul_BcAcycInstr = ", i); TRACE2("0x%x\n", mem_info->ax_BiuSize[i].ul_BcAcycInstr);
        TRACE1("    --> mem_info.ax_BiuSize[%02d].ul_BcHipInstr = ", i); TRACE2("0x%x\n", mem_info->ax_BiuSize[i].ul_BcHipInstr);
        TRACE1("    --> mem_info.ax_BiuSize[%02d].ul_BcLipInstr = ", i); TRACE2("0x%x\n", mem_info->ax_BiuSize[i].ul_BcLipInstr);
        TRACE1("    --> mem_info.ax_BiuSize[%02d].ul_BmBuf      = ", i); TRACE2("0x%x\n", mem_info->ax_BiuSize[i].ul_BmBuf);
        TRACE1("    --> mem_info.ax_BiuSize[%02d].ul_RepBuf     = ", i); TRACE2("0x%x\n", mem_info->ax_BiuSize[i].ul_RepBuf);
    }
    for(i=0; i<MAX_API_STREAM; i++)
    {
        TRACE1("    --> mem_info.x_BiuInfo[%02d].ul_MemoryBank  = ", i); TRACE2("0x%x\n", mem_info->x_BiuInfo[i].ul_MemoryBank);
        TRACE1("    --> mem_info.x_BiuInfo[%02d].ul_Protocol    = ", i); TRACE2("0x%x\n", mem_info->x_BiuInfo[i].ul_Protocol);
        TRACE1("    --> mem_info.x_BiuInfo[%02d].ul_StreamNb    = ", i); TRACE2("0x%x\n", mem_info->x_BiuInfo[i].ul_StreamNb);
    }
    TRACE_END

    return uw_RetVal;
}



//***************************************************************************
//
//   Module : ApiLsSys                 Submodule : ApiCmdSysSetMemPartition
//
//   Author : Thomas Jahn              Project   : API1553-DS
//
//---------------------------------------------------------------------------
//   Create : 02.06.03   Update : 02.06.03
//---------------------------------------------------------------------------
//    Descriptions
//    ------------
//    Inputs    : 
//
//    Outputs   : Instruction execution status [status]
//
//    Description :
//    This function applies the 'API_SYS_SET_MEM_PARTITION' instruction to the API1553-DS
//    ASP Driver SW to set the ASP target software board memory layout.
//
//***************************************************************************

AiInt16 ApiCmdSysSetMemPartition_(AiUInt32 bModule, AiUInt8 uc_Mode, TY_API_SET_MEM_INFO *px_MemInfo, AiUInt32 *pul_Status, AiUInt32 aul_MemUsed[2])
{
    AiInt16  uw_RetVal = API_OK;

    TY_MIL_COM_SYS_MEM_PART_SET_INPUT  xInput;
    TY_MIL_COM_SYS_MEM_PART_SET_OUTPUT xOutput;

    /* tbd : check on maximum sizes and amount if ids */
    /* check: allow multiples of 256 only, e.g. 1..16 x256 for hids, xfers */
    /* check: allow multiples of 64k only for sim_buf and mon_buf areas */
    /* note: bh,sq,eq shall be identical */
    /* note: replay shall be investigated : is replay size for set_mem applicable ? */
    xInput.mode = uc_Mode;
    memcpy( &xInput.mem_info, px_MemInfo, sizeof(TY_API_SET_MEM_INFO) );

    MIL_COM_INIT( &xInput.cmd, API_STREAM(bModule), 0, MIL_COM_SYS_MEM_PART_SET, sizeof(xInput), sizeof(xOutput) );

    uw_RetVal = API_CMD_STRUCT_SYS_MEM_PART_SET( bModule, &xInput, &xOutput );
    
    if( API_OK == uw_RetVal )
    {
        /* return set mem status and the requested size */
        *pul_Status     = xOutput.status;
        aul_MemUsed[0]  = xOutput.mem_used[0];
        aul_MemUsed[1]  = xOutput.mem_used[1];
    }

    return uw_RetVal;
}

AI_LIB_FUNC AiReturn AI_CALL_CONV ApiCmdSysSetMemPartition(AiUInt32 bModule, AiUInt8 uc_Mode, TY_API_SET_MEM_INFO *px_MemInfo, AiUInt32 *pul_Status, AiUInt32 aul_MemUsed[2])
{
    int i = 0;
    AiInt16  uw_RetVal = API_OK;


    if( NULL == px_MemInfo )
        uw_RetVal = API_ERR_PARAM3_IS_NULL;
    else if( NULL == pul_Status )
        uw_RetVal = API_ERR_PARAM4_IS_NULL;
    else if( NULL == aul_MemUsed )
        uw_RetVal = API_ERR_PARAM5_IS_NULL;
    else 
    {
        uw_RetVal = ApiCmdSysSetMemPartition_(bModule, uc_Mode, px_MemInfo, pul_Status, aul_MemUsed);
    }

    v_ExamineRetVal( "ApiCmdSysSetMemPartition", uw_RetVal );
    TRACE_BEGIN
    TRACE("        TY_API_SET_MEM_INFO x_MemInfo;\n");
    TRACE("        AiUInt32            ul_Status;\n");
    TRACE("        AiUInt32            aul_MemUsed[2];\n");
    TRACE1("        x_MemInfo.aul_SimBufSize[0]            = 0x%x;\n", px_MemInfo->aul_SimBufSize[0]);
    TRACE1("        x_MemInfo.aul_SimBufSize[1]            = 0x%x;\n", px_MemInfo->aul_SimBufSize[1]);
    for(i=0; i<MAX_API_STREAM; i++)
    {
        TRACE1("        mem_info.ax_BiuCnt[%02d].ul_BcBhArea     = ", i); TRACE2("0x%x;\n", px_MemInfo->ax_BiuCnt[i].ul_BcBhArea);
        TRACE1("        mem_info.ax_BiuCnt[%02d].ul_BcEqArea     = ", i); TRACE2("0x%x;\n", px_MemInfo->ax_BiuCnt[i].ul_BcEqArea);
        TRACE1("        mem_info.ax_BiuCnt[%02d].ul_BcSqArea     = ", i); TRACE2("0x%x;\n", px_MemInfo->ax_BiuCnt[i].ul_BcSqArea);
        TRACE1("        mem_info.ax_BiuCnt[%02d].ul_BcXferDesc   = ", i); TRACE2("0x%x;\n", px_MemInfo->ax_BiuCnt[i].ul_BcXferDesc);
        TRACE1("        mem_info.ax_BiuCnt[%02d].ul_RtBhArea     = ", i); TRACE2("0x%x;\n", px_MemInfo->ax_BiuCnt[i].ul_RtBhArea);
        TRACE1("        mem_info.ax_BiuCnt[%02d].ul_RtEqArea     = ", i); TRACE2("0x%x;\n", px_MemInfo->ax_BiuCnt[i].ul_RtEqArea);
        TRACE1("        mem_info.ax_BiuCnt[%02d].ul_RtSqArea     = ", i); TRACE2("0x%x;\n", px_MemInfo->ax_BiuCnt[i].ul_RtSqArea);
    }
    for(i=0; i<MAX_API_STREAM; i++)
    {
        TRACE1("        mem_info.ax_BiuSize[%02d].ul_BcAcycInstr = ", i); TRACE2("0x%x;\n", px_MemInfo->ax_BiuSize[i].ul_BcAcycInstr);
        TRACE1("        mem_info.ax_BiuSize[%02d].ul_BcHipInstr  = ", i); TRACE2("0x%x;\n", px_MemInfo->ax_BiuSize[i].ul_BcHipInstr);
        TRACE1("        mem_info.ax_BiuSize[%02d].ul_BcLipInstr  = ", i); TRACE2("0x%x;\n", px_MemInfo->ax_BiuSize[i].ul_BcLipInstr);
        TRACE1("        mem_info.ax_BiuSize[%02d].ul_BmBuf       = ", i); TRACE2("0x%x;\n", px_MemInfo->ax_BiuSize[i].ul_BmBuf);
        TRACE1("        mem_info.ax_BiuSize[%02d].ul_RepBuf      = ", i); TRACE2("0x%x;\n", px_MemInfo->ax_BiuSize[i].ul_RepBuf);
    }
    TRACE_FCTA("ApiCmdSysSetMemPartition", uw_RetVal); 
    TRACE_PARA(bModule);
    TRACE_PARA(uc_Mode);
    TRACE_RPARA("&x_MemInfo");
    TRACE_RPARA("&ul_Status");
    TRACE_RPARE("aul_MemUsed");
    TRACE_FCTE;
    TRACE1("    --> mem_info.ul_Status      = 0x%x\n", *pul_Status);
    TRACE1("    --> mem_info.aul_MemUsed[0] = 0x%x\n", aul_MemUsed[0]);
    TRACE1("    --> mem_info.aul_MemUsed[1] = 0x%x\n", aul_MemUsed[1]);
    TRACE_END

    return uw_RetVal;
}


//***************************************************************************
//
//   Module : ApiLsSys                 Submodule : ApiCmdUtility
//
//   Author : Thomas Jahn              Project   : API1553-DS
//
//---------------------------------------------------------------------------
//   Create : 02.06.03   Update : 02.06.03
//---------------------------------------------------------------------------
//    Descriptions
//    ------------
//    Inputs    : 
//
//    Outputs   : Instruction execution status [status]
//
//    Description :
//
//***************************************************************************

AiInt16 ApiCmdUtilityLegacy_(AiUInt32 bModule, AiUInt8 uc_InCnt, AiUInt8 *puc_OutCnt, AiUInt32 *aul_InLongs, AiUInt32 *aul_OutLongs)
{
    AiUInt8 auc_Input[(128 * 4) + 1];
    AiUInt32 aul_Output[128];
    AiInt16 in_byte_size = 0;
    AiInt16 in_lword_size = 0;
    AiInt16 uw_RetVal = API_OK;
    AiUInt32 i;

    auc_Input[0] = uc_InCnt;
    for (i = 0; i < uc_InCnt; i++)
    {
        auc_Input[i * 4 + 1] = (AiUInt8)(aul_InLongs[i] >> 24);
        auc_Input[i * 4 + 2] = (AiUInt8)(aul_InLongs[i] >> 16);
        auc_Input[i * 4 + 3] = (AiUInt8)(aul_InLongs[i] >> 8);
        auc_Input[i * 4 + 4] = (AiUInt8)(aul_InLongs[i]);
    }

    uw_RetVal = ApiIo(bModule & ~API_STREAM_MASK, 0, TG_API_UTIL, TG_ACK2,
        (AiUInt8 *)auc_Input, (uc_InCnt * 4) + 1, w_p, 0,
        puc_OutCnt, &in_byte_size,
        w_p, &tg_os,
        (AiUInt32 *)aul_Output, &in_lword_size);
    if (API_OK == uw_RetVal)
    {
        if ((0 == *puc_OutCnt) || (*puc_OutCnt > 128))
            uw_RetVal = API_ERR;
        else
        {
            for (i = 0; i < *puc_OutCnt; i++)
            {
                aul_OutLongs[i] = aul_Output[i];
            }
        }
    }

    return(uw_RetVal);
}

AiInt16 ApiCmdUtility_(AiUInt32 bModule, AiUInt8 uc_InCnt, AiUInt8 *puc_OutCnt, AiUInt32 *aul_InLongs, AiUInt32 *aul_OutLongs )
{
    AiInt16 uw_RetVal = API_OK;
    AiUInt32 i;

    TY_MIL_COM_SYS_UTIL_INPUT   * pxInput  = NULL;
    TY_MIL_COM_SYS_UTIL_OUTPUT  * pxOutput = NULL;

    AiUInt32 ulInputSize  = sizeof(TY_MIL_COM_SYS_UTIL_INPUT)  +  128*4+1 * sizeof(AiUInt32);
    AiUInt32 ulOutputSize = sizeof(TY_MIL_COM_SYS_UTIL_OUTPUT) +  128     * sizeof(AiUInt32);

    pxInput  = (TY_MIL_COM_SYS_UTIL_INPUT   *)AiOsMalloc( ulInputSize  );
    pxOutput = (TY_MIL_COM_SYS_UTIL_OUTPUT  *)AiOsMalloc( ulOutputSize );

    if( pxInput && pxOutput )
    {
        pxInput->count = uc_InCnt;
        for( i=0; i<uc_InCnt; i++ )
        {
            pxInput->data[i] = aul_InLongs[i];
        }

        MIL_COM_INIT( &pxInput->cmd, API_STREAM(bModule), 0, MIL_COM_SYS_UTIL, ulInputSize, ulOutputSize );

        uw_RetVal = API_CMD_STRUCT_SYS_UTIL( bModule, pxInput, pxOutput );

        if( API_OK == uw_RetVal )
        {
            if( puc_OutCnt && aul_OutLongs )
            {
                for( i=0; (i<pxOutput->count) && (i<*puc_OutCnt) ; i++ )
                {
                    aul_OutLongs[i] = pxOutput->data[i];
                }

                *puc_OutCnt = i;
            }
        }
    }

    return uw_RetVal;
}

AI_LIB_FUNC AiReturn AI_CALL_CONV ApiCmdUtility(AiUInt32 bModule, AiUInt8 uc_InCnt, AiUInt8 *puc_OutCnt, AiUInt32 *aul_InLongs, AiUInt32 *aul_OutLongs )
{
    AiInt16 uw_RetVal = API_OK;
    AiUInt32 i;
    TY_DEVICE_INFO * pDevice = _ApiGetDeviceInfoPtrByModule(bModule);
    if (pDevice == NULL)
        return API_ERR_NO_MODULE_EXTENSION;

    if(IsTswVersionGreaterOrEqual(pDevice, 21, 0))
    {
        uw_RetVal = ApiCmdUtility_(bModule, uc_InCnt, puc_OutCnt, aul_InLongs, aul_OutLongs);
    }
    else
        uw_RetVal = ApiCmdUtilityLegacy_(bModule, uc_InCnt, puc_OutCnt, aul_InLongs, aul_OutLongs);

    v_ExamineRetVal( "ApiCmdUtility", uw_RetVal );

    TRACE_BEGIN
    TRACE1("        AiUInt32 aul_InLongs[%d];\n",  uc_InCnt);
    TRACE1("        AiUInt32 aul_OutLongs[%d];\n", *puc_OutCnt);
    for(i=0; i<uc_InCnt; i++)
    {
        TRACE1("        aul_InLongs[%d] = ", i); TRACE2("0x%x;\n", aul_InLongs[i]);
    }
    for(i=0; i<*puc_OutCnt; i++)
    {
        TRACE1("        aul_OutLongs[%d] = ", i); TRACE2("0x%x;\n", aul_OutLongs[i]);
    }
    TRACE_FCTA("ApiCmdUtility", uw_RetVal); 
    TRACE_PARA(bModule);
    TRACE_END

    return uw_RetVal;
}



//**************************************************************************
//
//   Module : ApiLsSys                 Submodule : ApiCmdReadDiscretes
//
//   Author : Patrick Giesel           Project   : API1553
//
//---------------------------------------------------------------------------
//   Create : 21.06.04     Update : 21.06.04
//---------------------------------------------------------------------------
//    Descriptions
//    ------------
//    Inputs    : ul_Module  - Module Number
//
//    Outputs   : pul_Value  - discrete input value
//
//    Description : Operate with the boards incoming discrete channels
//
//**************************************************************************

AiInt16 ApiCmdReadDiscretes_(AiUInt32 ul_Module, AiUInt32 *pul_Value)
{
    AiInt16 w_RetVal = API_OK;
    TY_MIL_COM                            xInput;
    TY_MIL_COM_SYS_DISCRETES_READ_OUTPUT  xOutput;

    MIL_COM_INIT( &xInput, API_STREAM(ul_Module), 0, MIL_COM_SYS_DISCRETES_READ, sizeof(xInput), sizeof(xOutput) );

    w_RetVal = API_CMD_STRUCT_SYS_DISCRETES_READ( ul_Module, &xInput, &xOutput );

    *pul_Value = xOutput.value;

    return w_RetVal;
}

AI_LIB_FUNC AiReturn AI_CALL_CONV ApiCmdReadDiscretes(AiUInt32 ul_Module, AiUInt32 *pul_Value)
{
    AiInt16 w_RetVal = API_OK;

    if( NULL == pul_Value )
        w_RetVal = API_ERR_PARAM2_IS_NULL;
    else 
    {
        w_RetVal = ApiCmdReadDiscretes_(ul_Module, pul_Value);
    }

    if( (Aiul_DbgLevel & DBG_TRACE_PBAPRO) == 0)
    {
        TRACE_BEGIN
        TRACE("        AiUInt32 ul_Value;\n");
        TRACE_FCTA("ApiCmdReadDiscretes", w_RetVal); 
        TRACE_PARA(ul_Module);
        TRACE_RPARE("&ul_Value");
        TRACE_FCTE;
        TRACE1("    --> ul_Value = 0x%x\n", *pul_Value);
        TRACE_END
    }

    return w_RetVal;
}

//**************************************************************************
//
//   Module : ApiLsSys                 Submodule : ApiCmdReadDiscretesConfig
//
//   Author : Frank Schmidt            Project   : API1553
//
//---------------------------------------------------------------------------
//   Create : 21.08.17     Update : 21.08.17
//---------------------------------------------------------------------------
//    Descriptions
//    ------------
//    Inputs    : ul_Module  - Module Number
//
//    Outputs   : pul_Value  - discrete config value
//
//    Description : Operate with the boards incoming discrete channels
//
//**************************************************************************

AiInt16 ApiCmdReadDiscretesConfig_(AiUInt32 ul_Module, AiUInt32 *ul_DiscreteSetup)
{
    AiInt16 w_RetVal = API_OK;
    TY_MIL_COM                                   xInput;
    TY_MIL_COM_SYS_DISCRETES_CONFIG_READ_OUTPUT  xOutput;

    MIL_COM_INIT( &xInput, API_STREAM(ul_Module), 0, MIL_COM_SYS_DISCRETES_CONFIG_READ, sizeof(xInput), sizeof(xOutput) );

    w_RetVal = API_CMD_STRUCT_SYS_DISCRETES_CONFIG_READ( ul_Module, &xInput, &xOutput );

	*ul_DiscreteSetup = xOutput.value;
    return w_RetVal;
}

AI_LIB_FUNC AiReturn AI_CALL_CONV ApiCmdReadDiscretesConfig(AiUInt32 ul_Module, AiUInt32 *ul_DiscreteSetup)
{
    AiInt16 w_RetVal = API_OK;

	if (NULL == ul_DiscreteSetup)
        w_RetVal = API_ERR_PARAM2_IS_NULL;
    else
    {
		w_RetVal = ApiCmdReadDiscretesConfig_(ul_Module, ul_DiscreteSetup);
    }

    if( (Aiul_DbgLevel & DBG_TRACE_PBAPRO) == 0)
    {
        TRACE_BEGIN
        TRACE("        AiUInt32 ul_Value;\n");
        TRACE_FCTA("ApiCmdReadDiscretesConfig", w_RetVal);
        TRACE_PARA(ul_Module);
        TRACE_RPARE("&ul_Value");
        TRACE_FCTE;
		TRACE1("    --> ul_Value = 0x%x\n", *ul_DiscreteSetup);
        TRACE_END
    }

    return w_RetVal;
}

//**************************************************************************
//
//   Submodule : ApiCmdReadDiscretesInfo
//
//---------------------------------------------------------------------------
//    Descriptions
//    ------------
//    Inputs    : uc_Module        - Module Number
//
//    Outputs   : px_DiscrInfo     - discrete configuration will be stored here
//                channels - number of discrete channels
//                canIn    - 
//                canOut   -  
//
//    Description : Operate with the boards outgoing discrete channels
//
//**************************************************************************
AI_LIB_FUNC AiReturn AI_CALL_CONV ApiCmdReadDiscretesInfo(AiUInt32 ul_module, TY_API_DISCR_INFO *px_DiscrInfo)
{
    AiReturn w_RetVal = API_OK;

    TY_MIL_COM                            xInput;
    TY_MIL_COM_SYS_DISCRETES_INFO_READ_OUTPUT  xOutput;

    if (px_DiscrInfo == NULL)
        return API_ERR_PARAM2_IS_NULL;

    MIL_COM_INIT(&xInput, API_STREAM(ul_module), 0, MIL_COM_SYS_DISCRETES_INFO, sizeof(xInput), sizeof(xOutput));

    w_RetVal = API_CMD_STRUCT_SYS_DISCRETES_INFO(ul_module, &xInput, &xOutput);

    px_DiscrInfo->channels = xOutput.discreteChannels;
    px_DiscrInfo->canIn = xOutput.canIn;
    px_DiscrInfo->canOut = xOutput.canOut;

    return w_RetVal;
}

//**************************************************************************
//
//   Module : ApiLsSys                 Submodule : ApiCmdWriteDiscretes
//
//   Author : Patrick Giesel           Project   : API1553
//
//---------------------------------------------------------------------------
//   Create : 21.06.04     Update : 21.06.04
//---------------------------------------------------------------------------
//    Descriptions
//    ------------
//    Inputs    : ul_Module  - Module Number
//                ul_Mask    - Mask used together with ul_Value to write output discrete channels
//                ul_Value   - Value used together with ul_Mask to write output discrete channels
//
//    Outputs   : none
//
//    Description : Operate with the boards outgoing discrete channels
//
//**************************************************************************
AiInt16 ApiCmdWriteDiscretes_(AiUInt32 ul_Module, AiUInt32 ul_Mask, AiUInt32 ul_Value)
{
    AiInt16 w_RetVal = API_OK;
    TY_MIL_COM_SYS_DISCRETES_WRITE_INPUT xInput;
    TY_MIL_COM_ACK                       xOutput;

    MIL_COM_INIT( &xInput.cmd, API_STREAM(ul_Module), 0, MIL_COM_SYS_DISCRETES_WRITE, sizeof(xInput), sizeof(xOutput) );

    xInput.mask  = ul_Mask;
    xInput.value = ul_Value;

    w_RetVal = API_CMD_STRUCT_SYS_DISCRETES_WRITE( ul_Module, &xInput, &xOutput );

    return w_RetVal;
}

AI_LIB_FUNC AiReturn AI_CALL_CONV ApiCmdWriteDiscretes(AiUInt32 ul_Module, AiUInt32 ul_Mask, AiUInt32 ul_Value)
{
    AiInt16 w_RetVal = API_OK;

    w_RetVal = ApiCmdWriteDiscretes_(ul_Module, ul_Mask, ul_Value);

    TRACE_BEGIN
    TRACE_FCTA("ApiCmdWriteDiscretes", w_RetVal); 
    TRACE_PARA(ul_Module);
    TRACE_PARA(ul_Mask);
    TRACE_PARE(ul_Value);
    TRACE_FCTE;
    TRACE_END

    return w_RetVal;
}

//**************************************************************************
//
//   Module : ApiLsSys                 Submodule : ApiCmdInitDiscretes
//
//   Author : Patrick Giesel           Project   : API1553
//
//---------------------------------------------------------------------------
//   Create : 21.06.04     Update : 21.06.04
//---------------------------------------------------------------------------
//    Descriptions
//    ------------
//    Inputs    : ul_Module         - Module Number
//                ul_DiscreteSetup  - Input / Output channel configuration 
//
//    Outputs   : none
//
//    Description : Configure boards in/out discrete channels.
//
//**************************************************************************
AiInt16 ApiCmdInitDiscretes_(AiUInt32 ul_Module, AiUInt32 ul_DiscreteSetup)
{
	AiInt16 w_RetVal = API_OK;
	TY_MIL_COM_SYS_DISCRETES_INIT_INPUT   xInput;
	TY_MIL_COM_ACK                        xOutput;

	MIL_COM_INIT(&xInput.cmd, API_STREAM(ul_Module), 0, MIL_COM_SYS_DISCRETES_INIT, sizeof(xInput), sizeof(xOutput));
	 
	xInput.setup = ul_DiscreteSetup;

	w_RetVal = API_CMD_STRUCT_SYS_DISCRETES_INIT(ul_Module, &xInput, &xOutput);

	return w_RetVal;
}
AI_LIB_FUNC AiReturn AI_CALL_CONV ApiCmdInitDiscretes(AiUInt32 ul_Module, AiUInt32 ul_DiscreteSetup)
{
	AiInt16 w_RetVal = API_OK;

	w_RetVal = ApiCmdInitDiscretes_(ul_Module, ul_DiscreteSetup);

	TRACE_BEGIN
		TRACE_FCTA("ApiCmdInitDiscretes", w_RetVal);
	TRACE_PARA(ul_Module);
	TRACE_PARE(ul_DiscreteSetup);
	TRACE_FCTE;
	TRACE_END

		return w_RetVal;
}

//**************************************************************************
//
//   Module : ApiLsSys                 Submodule : ApiCmdInitDiscretesRecording
//
//   Author : Patrick Giesel           Project   : API1553
//
//---------------------------------------------------------------------------
//   Create : 08.03.17     Update : 08.03.17
//---------------------------------------------------------------------------
//    Descriptions
//    ------------
//    Inputs    : ul_Module  - Module Number
//                ul_Config  - Enable Discrete Interrupt and Fifo  
//
//    Outputs   : none
//
//    Description : Configure discrete channels for Recording.
//
//**************************************************************************
AiInt16 ApiCmdInitDiscretesRecording_(AiUInt32 ul_Module, AiUInt32 ul_Config)
{
	AiInt16 w_RetVal = API_OK;
	TY_MIL_COM_SYS_DISCRETES_REC_INIT_INPUT   xInput;
	TY_MIL_COM_ACK                            xOutput;

	MIL_COM_INIT(&xInput.cmd, API_STREAM(ul_Module), 0, MIL_COM_SYS_DISCRETES_REC_INIT, sizeof(xInput), sizeof(xOutput));

	xInput.setup = ul_Config;

	w_RetVal = API_CMD_STRUCT_SYS_DISCRETES_REC_INIT(ul_Module, &xInput, &xOutput);

	return w_RetVal;
}

AI_LIB_FUNC AiReturn AI_CALL_CONV ApiCmdInitDiscretesRecording(AiUInt32 ul_Module, AiUInt32 ul_Config)
{
	AiInt16 w_RetVal = API_OK;

	w_RetVal = ApiCmdInitDiscretesRecording_(ul_Module, ul_Config);

	TRACE_BEGIN
		TRACE_FCTA("ApiCmdInitDiscretesRecording", w_RetVal);
	TRACE_PARA(ul_Module);
	TRACE_PARE(ul_Config);
	TRACE_FCTE;
	TRACE_END

		return w_RetVal;
}


//**************************************************************************
//
//   Module : ApiLsSys                 Submodule : ApiCmdSyncCounterSet
//
//   Author : Michael Riedinger        Project   : API1553
//
//---------------------------------------------------------------------------
//   Create : 05.02.07     Update : 
//---------------------------------------------------------------------------
//    Descriptions
//    ------------
//    Inputs    : ul_Module  - Module Number
//                uc_Biu     - Biu
//                ul_SyncCnt - Value for sync counter
//
//    Outputs   : none
//
//    Description : Set value of syncronisation counter (used for mode code 17)
//
//**************************************************************************
AiInt16 ApiCmdSyncCounterSet_(AiUInt32 ul_Module, AiUInt8 uc_Biu, TY_API_SYNC_CNT_SET *px_SyncCntSet)
{
    AiInt16 w_RetVal = API_OK;
    TY_MIL_COM_SYS_SYNC_COUNTER_SET_INPUT  xInput;
    TY_MIL_COM_ACK                         xOutput;

    MIL_COM_INIT( &xInput.cmd, API_STREAM(ul_Module), 0, MIL_COM_SYS_SYNC_COUNTER_SET, sizeof(xInput), sizeof(xOutput) );

    xInput.value   = px_SyncCntSet->ul_SyncCntVal;

    w_RetVal = API_CMD_STRUCT_SYS_SYNC_COUNTER_SET( ul_Module, &xInput, &xOutput );

    return w_RetVal;
}

AI_LIB_FUNC AiReturn AI_CALL_CONV ApiCmdSyncCounterSet(AiUInt32 ul_Module, AiUInt8 uc_Biu, TY_API_SYNC_CNT_SET *px_SyncCntSet)
{
    AiInt16 w_RetVal = API_OK;

    if( (0 == (ul_Module & API_STREAM_MASK)) && ((uc_Biu < MIN_BIU) || (uc_Biu > MAX_BIU)) )
        w_RetVal = API_ERR_WRONG_BIU;
    else if( NULL == px_SyncCntSet)
        w_RetVal = API_ERR_PARAM3_IS_NULL;
    else if( px_SyncCntSet->ul_SyncCntVal & 0xffff0000)
        w_RetVal = API_ERR_PARAM4_NOT_IN_RANGE;
    else 
    {
        w_RetVal = ApiCmdSyncCounterSet_(ul_Module, uc_Biu, px_SyncCntSet);
    }

    v_ExamineRetVal( "ApiCmdSyncCounterSet", w_RetVal );

    TRACE_BEGIN
    TRACE("        TY_API_SYNC_CNT_SET x_SyncCntSet;\n");
    TRACE1("        x_SyncCntSet.ul_SyncCntVal = 0x%x;\n", px_SyncCntSet->ul_SyncCntVal );
    TRACE_FCTA("ApiCmdSyncCounterSet", w_RetVal); 
    TRACE_PARA(ul_Module);
    TRACE_PARA(uc_Biu);
    TRACE_RPARE("&x_SyncCntSet");
    TRACE_FCTE;
    TRACE_END

    return w_RetVal;
}

//**************************************************************************
//
//   Module : ApiLsSys                 Submodule : ApiCmdSyncCounterGet
//
//   Author : Michael Riedinger        Project   : API1553
//
//---------------------------------------------------------------------------
//   Create : 05.02.07     Update : 
//---------------------------------------------------------------------------
//    Descriptions
//    ------------
//    Inputs    : ul_Module  - Module Number
//                uc_Biu     - Biu
//
//    Outputs   : pul_SyncCnt - Value of sync counter
//
//    Description : Read value of syncronisation counter (used for mode code 17)
//
//**************************************************************************

AiInt16 ApiCmdSyncCounterGet_(AiUInt32 ul_Module, AiUInt8 uc_Biu, TY_API_SYNC_CNT_GET *px_SyncCntGet)
{
    AiInt16 w_RetVal = API_OK;
    TY_MIL_COM                             xInput;
    TY_MIL_COM_SYS_SYNC_COUNTER_GET_OUTPUT xOutput;

    MIL_COM_INIT( &xInput, API_STREAM(ul_Module), 0, MIL_COM_SYS_SYNC_COUNTER_GET, sizeof(xInput), sizeof(xOutput) );

    w_RetVal = API_CMD_STRUCT_SYS_SYNC_COUNTER_GET( ul_Module, &xInput, &xOutput );
    if(API_OK == w_RetVal)
    {
        px_SyncCntGet->ul_SyncCntVal      = xOutput.value;
        px_SyncCntGet->ul_SyncCntInit     = xOutput.init;
        px_SyncCntGet->ul_SyncCntInitLow  = xOutput.ttl;
        px_SyncCntGet->ul_SyncCntInitHigh = xOutput.tth;
    }

    return w_RetVal;
}

AI_LIB_FUNC AiReturn AI_CALL_CONV ApiCmdSyncCounterGet(AiUInt32 ul_Module, AiUInt8 uc_Biu, TY_API_SYNC_CNT_GET *px_SyncCntGet)
{
    AiInt16 w_RetVal = API_OK;

    if( (0 == (ul_Module & API_STREAM_MASK)) && ((uc_Biu < MIN_BIU) || (uc_Biu > MAX_BIU)) )
        w_RetVal = API_ERR_WRONG_BIU;
    else if( NULL == px_SyncCntGet )
        w_RetVal = API_ERR_PARAM3_IS_NULL;
    else 
    {
        w_RetVal = ApiCmdSyncCounterGet_(ul_Module, uc_Biu, px_SyncCntGet);
    }

    v_ExamineRetVal( "ApiCmdSyncCounterGet", w_RetVal );

    TRACE_BEGIN
    TRACE("        TY_API_SYNC_CNT_SET x_SyncCntGet;\n");
    TRACE_FCTA("ApiCmdSyncCounterGet", w_RetVal); 
    TRACE_PARA(ul_Module);
    TRACE_PARA(uc_Biu);
    TRACE_RPARE("&x_SyncCntGet");
    TRACE1("    --> x_SyncCntGet.ul_SyncCntInit     = 0x%x\n", px_SyncCntGet->ul_SyncCntInit );
    TRACE1("    --> x_SyncCntGet.ul_SyncCntInitHigh = 0x%x\n", px_SyncCntGet->ul_SyncCntInitHigh );
    TRACE1("    --> x_SyncCntGet.ul_SyncCntInitLow  = 0x%x\n", px_SyncCntGet->ul_SyncCntInitLow );
    TRACE1("    --> x_SyncCntGet.ul_SyncCntVal      = 0x%x\n", px_SyncCntGet->ul_SyncCntVal );
    TRACE_FCTE;
    TRACE_END

    return w_RetVal;
}

//**************************************************************************
//
//   Module : ApiLsSys                 Submodule : ApiCmdSysPXICon
//
//   Author : Patrick Giesel           Project   : API1553
//
//---------------------------------------------------------------------------
//   Create : 13.03.09     Update : 
//---------------------------------------------------------------------------
//    Descriptions
//    ------------
//    Inputs    : ul_Module  - Module Number
//                uc_Biu     - Biu
//
//    Outputs   : pul_SyncCnt - Value of sync counter
//
//    Description : Set PXI Trigger and TT Source Handling
//
//**************************************************************************
AiInt16 ApiCmdSysPXICon_(AiUInt32 ul_Module, TY_API_PXI_CON *px_PxiCon)
{
    AiInt16 w_RetVal = API_OK;
    TY_MIL_COM_SYS_PXI_CON_INPUT xInput;
    TY_MIL_COM_ACK               xOutput;

    MIL_COM_INIT( &xInput.cmd, API_STREAM(ul_Module), 0, MIL_COM_SYS_PXI_CON, sizeof(xInput), sizeof(xOutput) );

    xInput.mode     = px_PxiCon->ul_Mode;
    xInput.source   = px_PxiCon->ul_TrgSource;
    xInput.dest     = px_PxiCon->ul_TrgDest;
    xInput.tt_clear = px_PxiCon->ul_TTClear;

    w_RetVal = API_CMD_STRUCT_SYS_PXI_CON( ul_Module, &xInput, &xOutput );

    return w_RetVal;
}

AI_LIB_FUNC AiReturn AI_CALL_CONV ApiCmdSysPXICon(AiUInt32 ul_Module, TY_API_PXI_CON *px_PxiCon)
{
    AiInt16 w_RetVal = API_OK;

    if( NULL == px_PxiCon )
        w_RetVal = API_ERR_PARAM2_IS_NULL;
    else if( px_PxiCon->ul_Mode > MAX_PXICON_MODE )
        w_RetVal = API_ERR_PARAM3_NOT_IN_RANGE;
    else if( px_PxiCon->ul_TrgSource > MAX_PXICON_TRGSOURCE )
        w_RetVal = API_ERR_PARAM4_NOT_IN_RANGE;
    else if( px_PxiCon->ul_TrgDest > MAX_PXICON_TRGDEST )
        w_RetVal = API_ERR_PARAM5_NOT_IN_RANGE;
    else if( px_PxiCon->ul_TTClear > API_ENA )
        w_RetVal = API_ERR_PARAM6_NOT_IN_RANGE;
    else 
    {
        w_RetVal = ApiCmdSysPXICon_(ul_Module, px_PxiCon);
    }

    v_ExamineRetVal( "ApiCmdSysPXICon", w_RetVal );

    TRACE_BEGIN
    TRACE("        TY_API_PXI_CON x_PxiCon;\n");
    TRACE_FCTA("ApiCmdSysPXICon", w_RetVal); 
    TRACE_PARA(ul_Module);
    TRACE_RPARE("&x_PxiCon");
    TRACE_FCTE;
    if( NULL != px_PxiCon )
    {
        TRACE1("    --> x_PxiCon.ul_Mode      = 0x%x\n", px_PxiCon->ul_Mode );
        TRACE1("    --> x_PxiCon.ul_TrgSource = 0x%x\n", px_PxiCon->ul_TrgSource );
        TRACE1("    --> x_PxiCon.ul_TrgDest   = 0x%x\n", px_PxiCon->ul_TrgDest );
        TRACE1("    --> x_PxiCon.ul_TTClear   = 0x%x\n", px_PxiCon->ul_TTClear );
    }
    TRACE_END

    return w_RetVal;
}

//**************************************************************************
//
//   Module : ApiLsSys                 Submodule : ApiCmdSysPXIGeographicalAddressGet
//
//   Author : Thomas Jahn              Project   : API1553
//
//---------------------------------------------------------------------------
//   Create : 17.03.21     Update : 
//---------------------------------------------------------------------------
//    Descriptions
//    ------------
//    Inputs    : ul_Module  - Module Number
//
//    Outputs   : pxiGeographicalAddress - Value of Geographical Address
//
//    Description : Get PXI Geographical Address
//
//**************************************************************************
AiReturn ApiCmdSysPXIGeographicalAddressGet_(AiUInt32 ul_Module, AiUInt32 *pxiGeographicalAddress)
{
    AiReturn r_RetVal = API_OK;
    TY_MIL_COM xInput;
    TY_MIL_COM_ACK_WITH_VALUE  xOutput;

    MIL_COM_INIT(&xInput, API_STREAM(ul_Module), 0, MIL_COM_SYS_PXI_GEO_ADDR, sizeof(xInput), sizeof(xOutput));

    r_RetVal = API_CMD_STRUCT_SYS_PXI_GEO_ADDR(ul_Module, &xInput, &xOutput);

    *pxiGeographicalAddress = xOutput.value;

    return r_RetVal;
}

AI_LIB_FUNC AiReturn AI_CALL_CONV ApiCmdSysPXIGeographicalAddressGet(AiUInt32 ul_Module, AiUInt32 *pxiGeographicalAddress)
{
    AiReturn r_RetVal = API_OK;

    if (NULL == pxiGeographicalAddress)
    {
        r_RetVal = API_ERR_PARAM2_IS_NULL;
    }
    else
    {
        r_RetVal = ApiCmdSysPXIGeographicalAddressGet_(ul_Module, pxiGeographicalAddress);
    }

    v_ExamineRetVal("ApiCmdSysPXIGeographicalAddressGet", r_RetVal);

    TRACE_BEGIN
        TRACE("        AiUInt32 ul_Value;\n");
    TRACE_FCTA("ApiCmdSysPXIGeographicalAddressGet", r_RetVal);
    TRACE_PARA(ul_Module);
    TRACE_RPARE("&pxiGeographicalAddress");
    TRACE_FCTE;
    if (NULL != pxiGeographicalAddress)
    {
        TRACE1("    --> ul_Value = 0x%x\n", *pxiGeographicalAddress);
    }
    TRACE_END

    return r_RetVal;
}


AiInt16 ApiCmdSysGetMemLocation_(AiUInt32 ul_Module, AiUInt8 uc_Biu, AiUInt32 ul_Mode, AiUInt32 ul_Id, AiUInt32 *pul_MemType, AiUInt32 *pul_Offset)
{
    AiInt16 w_RetVal = API_OK;

    TY_MIL_COM_SYS_GET_MEM_LOCATION_INPUT   xInput;
    TY_MIL_COM_SYS_GET_MEM_LOCATION_OUTPUT  xOutput;


    // -- initialize command ---
    MIL_COM_INIT( &xInput.xCommand, API_STREAM(ul_Module), uc_Biu, MIL_COM_SYS_GET_MEM_LOCATION, sizeof(xInput), sizeof(xOutput) );

    xInput.ulId   = ul_Id;
    xInput.ulMode = ul_Mode;

    w_RetVal = API_CMD_STRUCT_SYS_GET_MEM_LOCATION( ul_Module, &xInput, &xOutput );

    if( w_RetVal == 0 )
    {
        if( pul_MemType )
            *pul_MemType = xOutput.ulMemType;
        if( pul_Offset )
            *pul_Offset = xOutput.ulOffset;
    }

    v_ExamineRetVal( "ApiCmdSysGetMemLocation", w_RetVal );

    return( w_RetVal );
}


AI_LIB_FUNC AiReturn AI_CALL_CONV ApiCmdSysGetMemLocation(AiUInt32 ul_Module, AiUInt8 uc_Biu, AiUInt32 ul_Mode, AiUInt32 ul_Id, AiUInt32 *pul_MemType, AiUInt32 *pul_Offset)
{

    AiInt16 w_RetVal = ApiCmdSysGetMemLocation_(ul_Module, uc_Biu, ul_Mode, ul_Id, pul_MemType, pul_Offset);

    TRACE_BEGIN
    TRACE("        AiUInt32 ul_MemType;\n");
    TRACE("        AiUInt32 ul_Offset;\n");
    TRACE_FCTA("ApiCmdSysGetMemLocation", w_RetVal); 
    TRACE_PARA(ul_Module);
    TRACE_PARA(uc_Biu);
    TRACE_PARA(ul_Mode);
    TRACE_PARA(ul_Id);
    TRACE_RPARA("&ul_MemType");
    TRACE_RPARE("&ul_Offset");
    TRACE_FCTE;
    TRACE1("    --> ul_MemType    = 0x%x\n", *pul_MemType );
    TRACE1("    --> ul_Offset     = 0x%x\n", *pul_Offset );
    TRACE_END

    return w_RetVal;
}




AiReturn ApiCmdSysTriggerEdgeInputSet_(AiUInt32 ulModHandle, AiUInt8 biu, AiUInt32 edge_setup)
{
    AiReturn ret = API_OK;

    TY_MIL_COM_WITH_VALUE   xInput;
    TY_MIL_COM_ACK          xOutput;


    // -- initialize command ---
    MIL_COM_INIT(&xInput.cmd, API_STREAM(ulModHandle), biu, MIL_COM_SYS_TRIGGER_EDGE_INPUT_SET, sizeof(xInput), sizeof(xOutput));

    xInput.value   = edge_setup;

    ret = API_CMD_SYS_TRIGGER_EDGE_INPUT_SET(ulModHandle, &xInput, &xOutput);

    v_ExamineRetVal("ApiCmdSysTriggerEdgeInputSet", ret);

    return(ret);
}


AI_LIB_FUNC AiReturn AI_CALL_CONV ApiCmdSysTriggerEdgeInputSet(AiUInt32 ulModHandle, AiUInt8 biu, AiUInt32 edge_setup)
{
    AiInt16 ret = ApiCmdSysTriggerEdgeInputSet_(ulModHandle, biu, edge_setup);

    TRACE_BEGIN
        TRACE("        AiUInt32 edge_setup;\n");
    TRACE_FCTA("ApiCmdSysTriggerEdgeInputSet", ret);
    TRACE_PARA(ulModHandle);
    TRACE_PARA(biu);
    TRACE_PARA(edge_setup);
    TRACE_FCTE;
    TRACE_END

        return ret;
}


AiReturn ApiCmdSysTriggerEdgeInputGet_(AiUInt32 ulModHandle, AiUInt8 biu, AiUInt32 * edge_setup)
{
    AiReturn ret = API_OK;

    TY_MIL_COM                  xInput;
    TY_MIL_COM_ACK_WITH_VALUE   xOutput;


    // -- initialize command ---
    MIL_COM_INIT(&xInput, API_STREAM(ulModHandle), biu, MIL_COM_SYS_TRIGGER_EDGE_INPUT_GET, sizeof(xInput), sizeof(xOutput));

    ret = API_CMD_SYS_TRIGGER_EDGE_INPUT_GET(ulModHandle, &xInput, &xOutput);

    if(edge_setup)
        *edge_setup = xOutput.value;

    v_ExamineRetVal("ApiCmdSysTriggerEdgeInputSet", ret);

    return(ret);
}


AI_LIB_FUNC AiReturn AI_CALL_CONV ApiCmdSysTriggerEdgeInputGet(AiUInt32 ulModHandle, AiUInt8 biu, AiUInt32 * edge_setup)
{
    AiInt16 ret = ApiCmdSysTriggerEdgeInputGet_(ulModHandle, biu, edge_setup);

    TRACE_BEGIN
        TRACE("        AiUInt32 edge_setup;\n");
    TRACE_FCTA("ApiCmdSysTriggerEdgeInputSet", ret);
    TRACE_PARA(ulModHandle);
    TRACE_PARA(biu);
    TRACE_PPARA(edge_setup);
    TRACE_FCTE;
    TRACE_END

        return ret;
}













AiReturn ApiCmdSysTriggerDigitalLoopCon_(AiUInt32 ulModHandle, AiUInt8 biu, AiUInt32 config)
{
    AiReturn ret = API_OK;

    TY_MIL_COM_WITH_VALUE   xInput;
    TY_MIL_COM_ACK          xOutput;


    // -- initialize command ---
    MIL_COM_INIT(&xInput.cmd, API_STREAM(ulModHandle), biu, MIL_COM_SYS_TRIGGER_DIGITAL_LOOP_CON, sizeof(xInput), sizeof(xOutput));

    xInput.value  = config;

    ret = API_CMD_SYS_TRIGGER_DIGITAL_LOOP_CON(ulModHandle, &xInput, &xOutput);

    v_ExamineRetVal("ApiCmdSysTriggerDigitalLoopCon", ret);

    return(ret);
}


AI_LIB_FUNC AiReturn AI_CALL_CONV ApiCmdSysTriggerDigitalLoopCon(AiUInt32 ulModHandle, AiUInt8 biu, AiUInt32 config)
{
    AiReturn ret = ApiCmdSysTriggerDigitalLoopCon_(ulModHandle, biu, config);

    TRACE_BEGIN
    TRACE("        AiUInt32 config;\n");
    TRACE_FCTA("ApiCmdSysTriggerDigitalLoopCon", ret);
    TRACE_PARA(ulModHandle);
    TRACE_PARA(biu);
    TRACE_PARA(config);
    TRACE_FCTE;
    TRACE_END

        return ret;
}


AiReturn ApiCmdSysTriggerDigitalLoopGet_(AiUInt32 ulModHandle, AiUInt8 biu, AiUInt32* config)
{
    AiReturn ret = API_OK;

    TY_MIL_COM                  xInput;
    TY_MIL_COM_ACK_WITH_VALUE   xOutput;


    // -- initialize command ---
    MIL_COM_INIT(&xInput, API_STREAM(ulModHandle), biu, MIL_COM_SYS_TRIGGER_DIGITAL_LOOP_GET, sizeof(xInput), sizeof(xOutput));

    ret = API_CMD_SYS_TRIGGER_DIGITAL_LOOP_GET(ulModHandle, &xInput, &xOutput);

    if (config)
        *config = xOutput.value;

    v_ExamineRetVal("ApiCmdSysTriggerDigitalLoopGet", ret);

    return(ret);
}


AI_LIB_FUNC AiReturn AI_CALL_CONV ApiCmdSysTriggerDigitalLoopGet(AiUInt32 ulModHandle, AiUInt8 biu, AiUInt32* config)
{
    AiReturn ret = ApiCmdSysTriggerDigitalLoopGet_(ulModHandle, biu, config);

    TRACE_BEGIN
    TRACE("        AiUInt32 config;\n");
    TRACE_FCTA("ApiCmdSysTriggerDigitalLoopGet", ret);
    TRACE_PARA(ulModHandle);
    TRACE_PARA(biu);
    TRACE_PPARA(config);
    TRACE_FCTE;
    TRACE_END

    return ret;
}

