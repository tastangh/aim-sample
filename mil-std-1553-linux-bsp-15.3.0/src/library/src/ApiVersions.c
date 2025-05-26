/* SPDX-FileCopyrightText: 2014-2023 AIM GmbH <info@aim-online.com> */
/* SPDX-License-Identifier: MIT */

/*! \file ApiVersions.c
 *
 * version related functions
 * 
 */
#include "net_io.h"
#include "AiOs.h"
#include "Ai_cdef.h"
#include "Ai_def.h"
#include "hw/ApiHw.h"

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
#include "generated/AiMilVersion.h"
#include "Dll_Fnc.h"
#include "hw/AiMyMon.h"

#define BCD2BIN_B(x)        ( (x & 0xF) + ( ((x>>4) & 0xF) * 10) )
#define BCD2BIN_W(x)        ( BCD2BIN_B(x) + BCD2BIN_B(x>>8)*100 )
#define EXTRACT_BITS(x,o,m) ((x>>o)&m)



/*! \brief This function reads versions from the device (obsolete) */
/*!
    This function applies the 'API_READ_SW_VERS' instruction to the API1553-DS
    ASP Driver SW to read the API1553-DS Target SW version information.

    \deprecated This function is only available for compatibility (up/downgrade). 
                For new developments use \see ApiReadVersions
    
    \param[in]  bModule        The module handle.
    \param[in]  biu            The biu or 0 if opened with ApiOpenEx.
    \param[out] fw_id          Firmware version.
    \param[out] sw_id          Targetsoftware version (partly).
    \param[out] lca_id         LCA version.
    \param[out] lca_chks       LCA checksum.
    \return                    Zero on normal termination
*/

AI_LIB_FUNC AiReturn AI_CALL_CONV ApiCmdReadSWVersion(AiUInt32 bModule, AiUInt8 biu, AiUInt16 *fw_id,
                                                      AiUInt16 *sw_id, AiUInt16 *lca_id,
                                                      AiUInt32 *lca_chks )
{
    AiInt16 uw_RetVal = API_OK;

    uw_RetVal = ApiCmdReadSWVersion_(bModule, biu, fw_id, sw_id, lca_id, lca_chks);

    v_ExamineRetVal( "ApiCmdReadSWVersion", uw_RetVal );

    TRACE_BEGIN
    TRACE("        AiUInt16 fw_id;\n");
    TRACE("        AiUInt16 sw_id;\n");
    TRACE("        AiUInt16 lca_id;\n");
    TRACE("        AiUInt16 lca_chks;\n");
    TRACE_FCTA("ApiCmdReadSWVersion", uw_RetVal); 
    TRACE_PARA(bModule);
    TRACE_PARA(biu);
    TRACE_RPARA("&fw_id");
    TRACE_RPARA("&sw_id");
    TRACE_RPARA("&lca_id");
    TRACE_RPARE("&lca_chks");
    TRACE_FCTE;
    TRACE_END
    
    return uw_RetVal;
}

AiInt16 ApiCmdReadSWVersion_(AiUInt32 bModule, AiUInt8 biu, AiUInt16 *fw_id,
                                                      AiUInt16 *sw_id, AiUInt16 *lca_id,
                                                      AiUInt32 *lca_chks )
{
    AiInt16 in_word_size = 0;
    AiInt16 in_lword_size = 0;
    AiUInt16 infow[2];
    AiUInt32 infolw[5];
    AiInt16 uw_RetVal = API_OK;

    uw_RetVal = ApiIo(bModule, biu, TG_API_READ_SW_VERS, TG_ACK2,
    		b_p, 0, w_p, 0,
    		b_p, &tg_os,
    		&infow[0], &in_word_size,
    		&infolw[0], &in_lword_size);

    if( API_OK == uw_RetVal )
    {
    	if( 1 == in_word_size )
    	{
    		if(fw_id)    *fw_id    = infow[0];
    		if(sw_id)    *sw_id    = (AiUInt16)infolw[0];
    		if(lca_id)   *lca_id   = (AiUInt16)infolw[1]; // ACHTUNG: ENC/DEC Version fehlt hier noch!!!!! pg
    		if(lca_chks) *lca_chks = (AiUInt16)infolw[2];
    	}
    	else
    	{ // for compatibility with versions < V18.00
    		if(fw_id)    *fw_id = infow[0];
    		if(sw_id)    *sw_id = infow[1];
    		if(lca_id)   *lca_id = (AiUInt16)infolw[0];
    		if(lca_chks) *lca_chks = infolw[1];
    	}
    }
    
    return uw_RetVal;
}


/*! \brief This function reads versions from the device (obsolete) */
/*!
    This function applies the 'API_READ_SW_VERS' instruction to the API1553-DS
    ASP Driver SW to read the API1553-DS Target SW version information.

    \deprecated This function is only available for compatibility (up/downgrade). 
                For new developments use \see ApiReadVersions

    
    \param [in]  bModule               The module handle.
    \param [in]  biu                   The biu or 0 if opened with ApiOpenEx.

    \param [out] pul_FirmwareVer       Firmware BIU1 version.
    \param [out] pul_TargetVer         TSW version.
    \param [out] pul_TargetBuild       TSW Build.
    \param [out] pul_LcaVer1           LCA1 version.
    \param [out] pul_LcaVer2           LCA2 version.
    \param [out] pul_LcaCheckSum       LCA checksum.
    \param [out] pul_SysDrvVer         System Driver version.
    \param [out] pul_SysDrvBuild       System Driver Build.
    \param [out] pul_DllVer            Library version.
    \param [out] pul_DllBuild          Library Build.
    \param [out] pul_BoardSerialNr     Board serial number.
    \param [out] puc_BspCompatibility  Board versions compatibility flag -> returns always API_BSP_COMPATIBLE .
    
    \return                            Zero on normal termination
*/
AI_LIB_FUNC AiReturn AI_CALL_CONV ApiReadBSPVersion( AiUInt32 ulModHandle,
                                                    AiUInt32 *pul_FirmwareVer,   AiUInt32 *pul_TargetVer,
                                                    AiUInt32 *pul_TargetBuild,   AiUInt32 *pul_LcaVer1,
                                                    AiUInt32 *pul_LcaVer2,       AiUInt32 *pul_LcaCheckSum,
                                                    AiUInt32 *pul_SysDrvVer,     AiUInt32 *pul_SysDrvBuild,
                                                    AiUInt32 *pul_DllVer,        AiUInt32 *pul_DllBuild,
                                                    AiUInt32 *pul_BoardSerialNr, AiUInt8  *puc_BspCompatibility )
{
    AiInt16 wRetVal            = 0;
    AiUInt8 ucBspCompatibility = 0;

    TY_API_VERSION_INFO xVersionInfo;

    memset( &xVersionInfo, 0, sizeof(xVersionInfo) );


    if( pul_FirmwareVer )      *pul_FirmwareVer      = 0;
    if( pul_TargetVer )        *pul_TargetVer        = 0;
    if( pul_TargetBuild )      *pul_TargetBuild      = 0;
    if( pul_SysDrvVer )        *pul_SysDrvVer        = 0;
    if( pul_SysDrvBuild )      *pul_SysDrvBuild      = 0;
    if( pul_DllVer )           *pul_DllVer           = 0;
    if( pul_DllBuild )         *pul_DllBuild         = 0;
    if( pul_BoardSerialNr )    *pul_BoardSerialNr    = 0;
    if( puc_BspCompatibility ) *puc_BspCompatibility = 0;
    if( pul_LcaVer1 )          *pul_LcaVer1          = 0;
    if( pul_LcaVer2 )          *pul_LcaVer2          = 0;
    if( pul_LcaCheckSum )      *pul_LcaCheckSum      = 0;


    wRetVal = ApiReadBSPVersionEx_( ulModHandle, &xVersionInfo, &ucBspCompatibility );

    if( wRetVal == API_OK )
    {
        if( pul_FirmwareVer )      *pul_FirmwareVer        = xVersionInfo.x_FirmwareBiu2Ver.ul_MajorVer/10 * 0x10000000 +
            xVersionInfo.x_FirmwareBiu2Ver.ul_MajorVer%10 *  0x1000000 +
            xVersionInfo.x_FirmwareBiu2Ver.ul_MinorVer/10 *   0x100000 +
            xVersionInfo.x_FirmwareBiu2Ver.ul_MinorVer%10 *    0x10000 +
            xVersionInfo.x_FirmwareBiu1Ver.ul_MajorVer/10 *     0x1000 +
            xVersionInfo.x_FirmwareBiu1Ver.ul_MajorVer%10 *      0x100 +
            xVersionInfo.x_FirmwareBiu1Ver.ul_MinorVer/10 *       0x10 +
            xVersionInfo.x_FirmwareBiu1Ver.ul_MinorVer%10;

        if( pul_TargetVer )        *pul_TargetVer          = xVersionInfo.x_TargetSWVer.ul_MajorSpecialVer/10  * 0x10000000 +
            xVersionInfo.x_TargetSWVer.ul_MajorSpecialVer%10  *  0x1000000 +
            xVersionInfo.x_TargetSWVer.ul_MinorSpecialVer/10  *   0x100000 +
            xVersionInfo.x_TargetSWVer.ul_MinorSpecialVer%10  *    0x10000 +
            xVersionInfo.x_TargetSWVer.ul_MajorVer/10         *     0x1000 +
            xVersionInfo.x_TargetSWVer.ul_MajorVer%10         *      0x100 +
            xVersionInfo.x_TargetSWVer.ul_MinorVer/10         *       0x10 +
            xVersionInfo.x_TargetSWVer.ul_MinorVer%10;


        if( pul_TargetBuild )      *pul_TargetBuild       =  (xVersionInfo.x_TargetSWVer.ul_BuildNo     )/1000  *   0x1000 +
            (xVersionInfo.x_TargetSWVer.ul_BuildNo%1000)/100   *    0x100 +
            (xVersionInfo.x_TargetSWVer.ul_BuildNo% 100)/10    *     0x10 +
            (xVersionInfo.x_TargetSWVer.ul_BuildNo%  10);


        if( pul_SysDrvVer )        *pul_SysDrvVer        =   xVersionInfo.x_SysDrvVer.ul_MajorSpecialVer/10   * 0x10000000 +
            xVersionInfo.x_SysDrvVer.ul_MajorSpecialVer%10   *  0x1000000 +
            xVersionInfo.x_SysDrvVer.ul_MinorSpecialVer/10   *   0x100000 +
            xVersionInfo.x_SysDrvVer.ul_MinorSpecialVer%10   *    0x10000 +
            xVersionInfo.x_SysDrvVer.ul_MajorVer/10          *     0x1000 +
            xVersionInfo.x_SysDrvVer.ul_MajorVer%10          *      0x100 +
            xVersionInfo.x_SysDrvVer.ul_MinorVer/10          *       0x10 +
            xVersionInfo.x_SysDrvVer.ul_MinorVer%10;

        if( pul_SysDrvBuild )      *pul_SysDrvBuild      =   (xVersionInfo.x_SysDrvVer.ul_BuildNo     )/1000  *   0x1000 +
            (xVersionInfo.x_SysDrvVer.ul_BuildNo%1000)/100   *    0x100 +
            (xVersionInfo.x_SysDrvVer.ul_BuildNo% 100)/10    *     0x10 +
            (xVersionInfo.x_SysDrvVer.ul_BuildNo%  10);

        if( pul_DllVer )           *pul_DllVer           =   xVersionInfo.x_DllVer.ul_MajorSpecialVer/10   * 0x10000000 +
            xVersionInfo.x_DllVer.ul_MajorSpecialVer%10   *  0x1000000 +
            xVersionInfo.x_DllVer.ul_MinorSpecialVer/10   *   0x100000 +
            xVersionInfo.x_DllVer.ul_MinorSpecialVer%10   *    0x10000 +
            xVersionInfo.x_DllVer.ul_MajorVer/10          *     0x1000 +
            xVersionInfo.x_DllVer.ul_MajorVer%10          *      0x100 +
            xVersionInfo.x_DllVer.ul_MinorVer/10          *       0x10 +
            xVersionInfo.x_DllVer.ul_MinorVer%10;

        if( pul_DllBuild )         *pul_DllBuild         =   (xVersionInfo.x_DllVer.ul_BuildNo     )/1000  *   0x1000 +
            (xVersionInfo.x_DllVer.ul_BuildNo%1000)/100   *    0x100 +
            (xVersionInfo.x_DllVer.ul_BuildNo% 100)/10    *     0x10 +
            (xVersionInfo.x_DllVer.ul_BuildNo%  10);

        if( pul_BoardSerialNr    ) *pul_BoardSerialNr    = xVersionInfo.ul_BoardSerialNo;
        if( puc_BspCompatibility ) *puc_BspCompatibility = ucBspCompatibility;
    }


    return(wRetVal);
} // end: ApiCmdReadSWVersion



/*! \brief This function reads versions from the Global RAM Extension (AyE devices) */
/*!
    \param [in]  ulModHandle    The module handle.
    \param [out] pxVersionInfo  Version information package.
    
    \return                     Zero on normal termination
*/
AiInt16 _MilApiHwGramExtReadVersion( AiUInt32 ulModHandle, TY_API_VERSION_INFO * pxVersionInfo )
{
    AiUInt32 ulVersion = 0;
    AiInt16  wRetVal   = 0;

    /* -- novram --- */

    wRetVal = _ApiReadMemData( ulModHandle, 
        API_MEMTYPE_GLOBAL_EXTENSION, 
        MYMON_GREG_NOVRAM_SERIAL, 
        4, 
        &pxVersionInfo->ul_BoardSerialNo );

    if( wRetVal != API_OK )
        return wRetVal;

    /* -- firmware --- */

    wRetVal = _ApiReadMemData( ulModHandle, 
        API_MEMTYPE_GLOBAL_EXTENSION, 
        MYMON_GREG_VERSION_BIU1, 
        4, 
        &ulVersion );

    if( ulVersion != 0xFFFFFFFF )
    {
        pxVersionInfo->x_FirmwareBiu1Ver.ul_MajorVer        = ((ulVersion & 0x0F000000) >> 24)    + ((ulVersion & 0xF0000000) >> 28) * 10;
        pxVersionInfo->x_FirmwareBiu1Ver.ul_MinorVer        = ((ulVersion & 0x000F0000) >> 16)    + ((ulVersion & 0x00F00000) >> 20) * 10;
        pxVersionInfo->x_FirmwareBiu1Ver.ul_BuildNo         = ( ulVersion & 0x0000000F)           + ((ulVersion & 0x000000F0) >>  4) * 10
            + ((ulVersion & 0x00000F00) >> 8)*100 + ((ulVersion & 0x0000F000) >> 12) * 1000;
    }

    if( wRetVal != API_OK )
        return wRetVal;


    wRetVal = _ApiReadMemData( ulModHandle, 
        API_MEMTYPE_GLOBAL_EXTENSION, 
        MYMON_GREG_VERSION_BIU2, 
        4, 
        &ulVersion );

    if( ulVersion != 0xFFFFFFFF )
    {
        pxVersionInfo->x_FirmwareBiu2Ver.ul_MajorVer        = ((ulVersion & 0x0F000000) >> 24)    + ((ulVersion & 0xF0000000) >> 28) * 10;
        pxVersionInfo->x_FirmwareBiu2Ver.ul_MinorVer        = ((ulVersion & 0x000F0000) >> 16)    + ((ulVersion & 0x00F00000) >> 20) * 10;
        pxVersionInfo->x_FirmwareBiu2Ver.ul_BuildNo         = ( ulVersion & 0x0000000F)           + ((ulVersion & 0x000000F0) >>  4) * 10
            + ((ulVersion & 0x00000F00) >> 8)*100 + ((ulVersion & 0x0000F000) >> 12) * 1000;
    }

    if( wRetVal != API_OK )
        return wRetVal;

    wRetVal = _ApiReadMemData( ulModHandle, 
        API_MEMTYPE_GLOBAL_EXTENSION, 
        MYMON_GREG_VERSION_BIU3, 
        4, 
        &ulVersion );

    if( ulVersion != 0xFFFFFFFF )
    {
        pxVersionInfo->x_FirmwareBiu3Ver.ul_MajorVer        = ((ulVersion & 0x0F000000) >> 24)    + ((ulVersion & 0xF0000000) >> 28) * 10;
        pxVersionInfo->x_FirmwareBiu3Ver.ul_MinorVer        = ((ulVersion & 0x000F0000) >> 16)    + ((ulVersion & 0x00F00000) >> 20) * 10;
        pxVersionInfo->x_FirmwareBiu3Ver.ul_BuildNo         = ( ulVersion & 0x0000000F)           + ((ulVersion & 0x000000F0) >>  4) * 10
            + ((ulVersion & 0x00000F00) >> 8)*100 + ((ulVersion & 0x0000F000) >> 12) * 1000;
    }

    if( wRetVal != API_OK )
        return wRetVal;


    wRetVal = _ApiReadMemData( ulModHandle, 
        API_MEMTYPE_GLOBAL_EXTENSION, 
        MYMON_GREG_VERSION_BIU4, 
        4, 
        &ulVersion );

    if( ulVersion != 0xFFFFFFFF )
    {
        pxVersionInfo->x_FirmwareBiu4Ver.ul_MajorVer        = ((ulVersion & 0x0F000000) >> 24)    + ((ulVersion & 0xF0000000) >> 28) * 10;
        pxVersionInfo->x_FirmwareBiu4Ver.ul_MinorVer        = ((ulVersion & 0x000F0000) >> 16)    + ((ulVersion & 0x00F00000) >> 20) * 10;
        pxVersionInfo->x_FirmwareBiu4Ver.ul_BuildNo         = ( ulVersion & 0x0000000F)           + ((ulVersion & 0x000000F0) >>  4) * 10
            + ((ulVersion & 0x00000F00) >> 8)*100 + ((ulVersion & 0x0000F000) >> 12) * 1000;
    }

    if( wRetVal != API_OK )
        return wRetVal;

    /* -- FPGA overall  --- */


    wRetVal = _ApiReadMemData( ulModHandle, 
        API_MEMTYPE_GLOBAL_EXTENSION, 
        MYMON_GREG_VERSION_FPGA, 
        4, 
        &ulVersion );

    pxVersionInfo->x_PciLcaVer.ul_MajorVer        = ((ulVersion & 0x0F000000) >> 24)    + ((ulVersion & 0xF0000000) >> 28) * 10;
    pxVersionInfo->x_PciLcaVer.ul_MinorVer        = ((ulVersion & 0x000F0000) >> 16)    + ((ulVersion & 0x00F00000) >> 20) * 10;
    pxVersionInfo->x_PciLcaVer.ul_BuildNo         = ( ulVersion & 0x0000000F)           + ((ulVersion & 0x000000F0) >>  4) * 10
        + ((ulVersion & 0x00000F00) >> 8)*100 + ((ulVersion & 0x0000F000) >> 12) * 1000;

    if( wRetVal != API_OK )
        return wRetVal;



    /* -- SOC --- */

    wRetVal = _ApiReadMemData( ulModHandle, 
        API_MEMTYPE_GLOBAL_EXTENSION, 
        MYMON_GREG_VERSION_SOC, 
        4, 
        &ulVersion );

    pxVersionInfo->x_AslLcaVer.ul_MajorVer        = ((ulVersion & 0x0F000000) >> 24)    + ((ulVersion & 0xF0000000) >> 28) * 10;
    pxVersionInfo->x_AslLcaVer.ul_MinorVer        = ((ulVersion & 0x000F0000) >> 16)    + ((ulVersion & 0x00F00000) >> 20) * 10;
    pxVersionInfo->x_AslLcaVer.ul_BuildNo         = ( ulVersion & 0x0000000F)           + ((ulVersion & 0x000000F0) >>  4) * 10
        + ((ulVersion & 0x00000F00) >> 8)*100 + ((ulVersion & 0x0000F000) >> 12) * 1000;

    if( wRetVal != API_OK )
        return wRetVal;


    /* -- MyMON  --- */


    wRetVal = _ApiReadMemData( ulModHandle, 
        API_MEMTYPE_GLOBAL_EXTENSION, 
        MYMON_GREG_VERSION_MYMON, 
        4, 
        &ulVersion );

    /* mymon version */

    pxVersionInfo->x_MonitorVer.ul_MajorVer        = ((ulVersion & 0x0F000000) >> 24)    + ((ulVersion & 0xF0000000) >> 28) * 10;
    pxVersionInfo->x_MonitorVer.ul_MinorVer        = ((ulVersion & 0x000F0000) >> 16)    + ((ulVersion & 0x00F00000) >> 20) * 10;
    pxVersionInfo->x_MonitorVer.ul_BuildNo         = ( ulVersion & 0x0000000F)           + ((ulVersion & 0x000000F0) >>  4) * 10
        + ((ulVersion & 0x00000F00) >> 8)*100 + ((ulVersion & 0x0000F000) >> 12) * 1000;



    return API_OK;
}




/*! \brief This function reads versions through ApiReadAllVersions (AyS devices) */
/*!
    \param [in]  ulModHandle    The module handle.
    \param [out] pxVersionInfo  Version information package.
    
    \return                     Zero on normal termination
*/
AiInt16 _ApiHwAysReadVersion( AiUInt32 ulModHandle, TY_API_VERSION_INFO * pxVersionInfo )
{
	unsigned int i;
    AiInt16 wRetVal;
    TY_VER_INFO         x_Versions[AI_MAX_VERSIONS];
    TY_VERSION_OUT      x_VersionInfoOut;


    memset( &x_VersionInfoOut,   0, sizeof(x_VersionInfoOut)   );

    /* -- Read all versions from TSW --- */

	wRetVal = ApiReadAllVersions(ulModHandle, AI_MAX_VERSIONS, x_Versions, &x_VersionInfoOut);

    if( wRetVal )
        return wRetVal;

    /* -- Read SN from shared RAM --- */

    wRetVal = _ApiReadMemData( ulModHandle, API_MEMTYPE_SHARED,  11*4, 4, &pxVersionInfo->ul_BoardSerialNo );

    if( wRetVal != API_OK )
        return wRetVal;

	for (i = 0; i < x_VersionInfoOut.ul_Count; i++)
	{
		switch (x_Versions[i].ul_VersionType)
		{
		case AI_FIRMWARE_VER_BIU1:
			pxVersionInfo->x_FirmwareBiu1Ver.ul_MajorVer = x_Versions[i].ul_MajorVer;
			pxVersionInfo->x_FirmwareBiu1Ver.ul_MinorVer = x_Versions[i].ul_MinorVer;
			pxVersionInfo->x_FirmwareBiu1Ver.ul_BuildNo = x_Versions[i].ul_BuildNr;
			break;
		case AI_FIRMWARE_VER_BIU2:
			pxVersionInfo->x_FirmwareBiu2Ver.ul_MajorVer = x_Versions[i].ul_MajorVer;
			pxVersionInfo->x_FirmwareBiu2Ver.ul_MinorVer = x_Versions[i].ul_MinorVer;
			pxVersionInfo->x_FirmwareBiu2Ver.ul_BuildNo = x_Versions[i].ul_BuildNr;
			break;
		case AI_TARGET_VER:
			pxVersionInfo->x_TargetSWVer.ul_MajorVer = x_Versions[i].ul_MajorVer;
			pxVersionInfo->x_TargetSWVer.ul_MinorVer = x_Versions[i].ul_MinorVer;
			pxVersionInfo->x_TargetSWVer.ul_BuildNo = x_Versions[i].ul_BuildNr;
			break;
		case AI_FPGA_VER:
			pxVersionInfo->x_PciLcaVer.ul_MajorVer = x_Versions[i].ul_MajorVer;
			pxVersionInfo->x_PciLcaVer.ul_MinorVer = x_Versions[i].ul_MinorVer;
			pxVersionInfo->x_PciLcaVer.ul_BuildNo = x_Versions[i].ul_BuildNr;
			break;
		case AI_MAIN_LCA_VER:
			pxVersionInfo->x_AslLcaVer.ul_MajorVer = x_Versions[i].ul_MajorVer;
			pxVersionInfo->x_AslLcaVer.ul_MinorVer = x_Versions[i].ul_MinorVer;
			pxVersionInfo->x_AslLcaVer.ul_BuildNo = x_Versions[i].ul_BuildNr;
			break;
		case AI_TCP_VER:
			pxVersionInfo->x_TcpVer.ul_MajorVer = x_Versions[i].ul_MajorVer;
			pxVersionInfo->x_TcpVer.ul_MinorVer = x_Versions[i].ul_MinorVer;
			pxVersionInfo->x_TcpVer.ul_BuildNo = x_Versions[i].ul_BuildNr;
			break;
		}
	}



    return API_OK;
}

/*! \brief This function reads the serial number from the device */
/*!
    This function uses the old \see ApiReadBspVersion command to read the serial
    Number of the board. Normally this is not required but if we have a very
    old target software and ApiReadBspVersionEx is not available, then we get
    all the versions we need with ApiReadSwVersion. The SN however is not returned.

    \param [in]  ulModHandle       The module handle.
    \param [out] pulBoardSerialNr  The serial number.
    
    \return                        Zero on normal termination
*/
AiInt16 _API_DLL_FUNC _ApiReadSerialNumber(  AiUInt32 ulModHandle, AiUInt32 * pulBoardSerialNr )
{
    AiInt16 in_word_size  = 0;
    AiInt16 in_lword_size = 0;
    AiInt16 wRetVal       = API_OK;

    AiUInt16 infow[2];
    AiUInt32 infolw[5];

    TY_DEVICE_INFO * pDevice = _ApiGetDeviceInfoPtrByModule( ulModHandle );

    if( pDevice == NULL )
        return API_ERR_NO_MODULE_EXTENSION;

    if(pulBoardSerialNr) *pulBoardSerialNr = 0;

    wRetVal = ApiIo(ulModHandle & ~API_STREAM_MASK, MIN_BIU, TG_API_READ_SW_VERS, TG_ACK2,
        b_p, 0, w_p, 0,
        b_p, &tg_os,
        &infow[0], &in_word_size,
        &infolw[0], &in_lword_size);

    if( API_OK == wRetVal )
    {
        if( 1 == in_word_size )
        {
            if(pulBoardSerialNr) *pulBoardSerialNr = infolw[4];
        }
        else
        {
            /* for compatibility with versions < V18.00 */
            if(pulBoardSerialNr) *pulBoardSerialNr = infolw[3];
        }
    }

    return(wRetVal);
} // end: _ApiReadSerialNumber




/*! \brief This function reads versions from the TSW ( !AyE devices) */
/*!
    \param [in]  ulModHandle    The module handle.
    \param [out] pxVersionInfo  Version information package.
    
    \return                     Zero on normal termination
*/
AiInt16 _MilApiHwTswReadVersion( AiUInt32 ulModHandle, TY_API_VERSION_INFO * pxVersionInfo )
{
    AiUInt16 uwFirmware  = 0;
    AiUInt16 uwTargetSw  = 0;
    AiUInt32 ulDummy     = 0;
    AiUInt16 uwDummy     = 0;
    AiInt16  wRetVal     = 0;
    AiInt16  wInWordSize = 0;


    AiUInt32 infolw[12];

    TY_DEVICE_INFO * pDevice = _ApiGetDeviceInfoPtrByModule( ulModHandle );


    /* use old command ApiCmdReadSWVersion first to read basic versions 
    * and check if ApiReadBspVersionEx is available                     */

    wRetVal = ApiCmdReadSWVersion( ulModHandle, API_BIU_1, &uwFirmware, &uwTargetSw, &uwDummy, &ulDummy );


    if( wRetVal != API_OK )
        return wRetVal;

    /* -- firmware version --- */

    pxVersionInfo->x_FirmwareBiu1Ver.ul_MajorVer = ((uwFirmware & 0x0F00) >> 8) + ((uwFirmware & 0xF000) >> 12)*10;
    pxVersionInfo->x_FirmwareBiu1Ver.ul_MinorVer = ( uwFirmware & 0x000F)       + ((uwFirmware & 0x00F0) >>  4)*10;

    /* -- TSW version --- */

    pxVersionInfo->x_TargetSWVer.ul_MajorVer     = ((uwTargetSw & 0x0F00) >> 8) + ((uwTargetSw & 0xF000) >> 12)*10;
    pxVersionInfo->x_TargetSWVer.ul_MinorVer     = ( uwTargetSw & 0x000F)       + ((uwTargetSw & 0x00F0) >>  4)*10;





    /* -- check for extended version information --- */

    if( ((pxVersionInfo->x_TargetSWVer.ul_MajorVer*100)+pxVersionInfo->x_TargetSWVer.ul_MinorVer) < 1810 )
    {
        /* We have all the versions that we need for the update. 
        We however not have the SN of the device.
        Use the old readbspversion command to read the SN and return.
        */
        wRetVal = _ApiReadSerialNumber( ulModHandle, &pxVersionInfo->ul_BoardSerialNo );

        /* No extended version info available */
        return wRetVal;

    }

    /* -- read extended version information --- */

    wRetVal = ApiIo( ulModHandle & ~API_STREAM_MASK, MIN_BIU, 
        TG_API_READ_SW_VERS_EX,         TG_ACK2,
        b_p,                            0,
        w_p,                            0,
        b_p,                            &tg_os,
        w_p,                            &tg_os,
        &infolw[0],                     &wInWordSize );

    if( wRetVal != API_OK )
        /* read extended version info failed */
        return wRetVal;

    /* -- PCI LCA --- */

    pxVersionInfo->x_PciLcaVer.ul_MajorVer = ((infolw[1] & 0x000000F0) >> 4) + ((infolw[1] & 0x00000F00) >> 8)*10;
    pxVersionInfo->x_PciLcaVer.ul_MinorVer = ( infolw[1] & 0x0000000F);

    /* -- Firmware --- */

    pxVersionInfo->x_FirmwareBiu1Ver.ul_MajorVer = ((infolw[ 2] & 0x00000F00) >>  8)+ ((infolw[ 2] & 0x0000F000) >> 12) * 10;
    pxVersionInfo->x_FirmwareBiu1Ver.ul_MinorVer = ( infolw[ 2] & 0x0000000F)       + ((infolw[ 2] & 0x000000F0) >>  4) * 10;
    pxVersionInfo->x_FirmwareBiu2Ver.ul_MajorVer = ((infolw[ 2] & 0x0F000000) >> 24)+ ((infolw[ 2] & 0xF0000000) >> 28) * 10;
    pxVersionInfo->x_FirmwareBiu2Ver.ul_MinorVer = ((infolw[ 2] & 0x000F0000) >> 16)+ ((infolw[ 2] & 0x00F00000) >> 20) * 10;
    pxVersionInfo->x_FirmwareBiu3Ver.ul_MajorVer = ((infolw[10] & 0x00000F00) >>  8)+ ((infolw[10] & 0x0000F000) >> 12) * 10;
    pxVersionInfo->x_FirmwareBiu3Ver.ul_MinorVer = ( infolw[10] & 0x0000000F)       + ((infolw[10] & 0x000000F0) >>  4) * 10;
    pxVersionInfo->x_FirmwareBiu4Ver.ul_MajorVer = ((infolw[10] & 0x0F000000) >> 24)+ ((infolw[10] & 0xF0000000) >> 28) * 10;
    pxVersionInfo->x_FirmwareBiu4Ver.ul_MinorVer = ((infolw[10] & 0x000F0000) >> 16)+ ((infolw[10] & 0x00F00000) >> 20) * 10;

    /* -- TSW --- */

    pxVersionInfo->x_TargetSWVer.ul_MajorVer        = ((infolw[3] & 0x00000F00) >>  8)    + ((infolw[3] & 0x0000F000) >> 12) * 10;
    pxVersionInfo->x_TargetSWVer.ul_MinorVer        = ( infolw[3] & 0x0000000F)           + ((infolw[3] & 0x000000F0) >>  4) * 10;
    pxVersionInfo->x_TargetSWVer.ul_MajorSpecialVer = ((infolw[3] & 0x0F000000) >> 24)    + ((infolw[3] & 0xF0000000) >> 28) * 10;
    pxVersionInfo->x_TargetSWVer.ul_MinorSpecialVer = ((infolw[3] & 0x000F0000) >> 16)    + ((infolw[3] & 0x00F00000) >> 20) * 10;
    pxVersionInfo->x_TargetSWVer.ul_BuildNo         = ( infolw[4] & 0x0000000F)           + ((infolw[4] & 0x000000F0) >>  4) * 10
        + ((infolw[4] & 0x00000F00) >> 8)*100 + ((infolw[4] & 0x0000F000) >> 12) * 1000;

    /* -- Serial --- */

    pxVersionInfo->ul_BoardSerialNo = infolw[5];

    /* -- ASL LCA --- */

    pxVersionInfo->x_AslLcaVer.ul_MajorVer  = ((infolw[7] & 0x000000F0) >> 4) + ((infolw[7] & 0x00000F00) >> 8) * 10;
    pxVersionInfo->x_AslLcaVer.ul_MinorVer  = ( infolw[7] & 0x0000000F);
    pxVersionInfo->x_AslLcaVer.ul_BuildNo  =  ( infolw[7] & 0xFFFF0000) >> 16;

    /* -- Monitor --- */

    pxVersionInfo->x_MonitorVer.ul_MajorVer = ((infolw[11] & 0x00000F00) >> 8)+ ((infolw[11] & 0x0000F000) >> 12) * 10;
    pxVersionInfo->x_MonitorVer.ul_MinorVer = ( infolw[11] & 0x0000000F)      + ((infolw[11] & 0x000000F0) >>  4) * 10;



    if(pDevice->x_Stream[0].ul_HighSpeed)
    {
        pxVersionInfo->x_IoLcaBiu1Ver.ul_MajorVer       = ((infolw[0] & 0x00F00000) >> 20) + ((infolw[0] & 0x0F000000) >> 24) * 10;
        pxVersionInfo->x_IoLcaBiu1Ver.ul_MinorVer       = ( infolw[0] & 0x000F0000) >> 16;
        pxVersionInfo->x_CoreLcaBiu1Ver.ul_MajorVer     = ((infolw[0] & 0x000000F0) >>  4) + ((infolw[0] & 0x00000F00) >>  8) * 10;
        pxVersionInfo->x_CoreLcaBiu1Ver.ul_MinorVer     = ( infolw[0] & 0x0000000F);

        pxVersionInfo->x_IoLcaBiu2Ver.ul_MajorVer       = ((infolw[6] & 0x000000F0) >>  4) + ((infolw[6] & 0x00000F00) >>  8) * 10;
        pxVersionInfo->x_IoLcaBiu2Ver.ul_MinorVer       = ( infolw[6] & 0x0000000F);
        pxVersionInfo->x_EndDecLcaBiu2Ver.ul_MajorVer   = ((infolw[6] & 0x00F00000) >> 20) + ((infolw[6] & 0x0F000000) >> 24) * 10;
        pxVersionInfo->x_EndDecLcaBiu2Ver.ul_MinorVer   = ( infolw[6] & 0x000F0000) >> 16;
    }
    else
    {
        pxVersionInfo->x_IoLcaBiu1Ver.ul_MajorVer       = ((infolw[0] & 0x00F00000) >> 20) + ((infolw[0] & 0x0F000000) >> 24) * 10;
        pxVersionInfo->x_IoLcaBiu1Ver.ul_MinorVer       = ( infolw[0] & 0x000F0000) >> 16;
        pxVersionInfo->x_CoreLcaBiu1Ver.ul_MajorVer     = ((infolw[0] & 0x000000F0) >>  4) + ((infolw[0] & 0x00000F00) >>  8) * 10;
        pxVersionInfo->x_CoreLcaBiu1Ver.ul_MinorVer     = ( infolw[0] & 0x0000000F);

        pxVersionInfo->x_IoLcaBiu2Ver.ul_MajorVer       = ((infolw[6] & 0x00F00000) >> 20) + ((infolw[6] & 0x0F000000) >> 24) * 10;
        pxVersionInfo->x_IoLcaBiu2Ver.ul_MinorVer       = ( infolw[6] & 0x000F0000) >> 16;
        pxVersionInfo->x_EndDecLcaBiu2Ver.ul_MajorVer   = ((infolw[6] & 0x000000F0) >>  4) + ((infolw[6] & 0x00000F00) >>  8) * 10;
        pxVersionInfo->x_EndDecLcaBiu2Ver.ul_MinorVer   = ( infolw[6] & 0x0000000F);
    }

    /* For AyX3910-2 or AyX-EFA, the second PBI is high speed */
    if(    (pDevice->x_Stream[1].ul_HighSpeed) 
        || (pDevice->x_IniPar.prot == API_PROTOCOL_1553_3910) 
        || (pDevice->x_IniPar.prot == API_PROTOCOL_1553_EFEX) )
    {
        pxVersionInfo->x_IoLcaBiu3Ver.ul_MajorVer       = ((infolw[8] & 0x00F00000) >> 20) + ((infolw[8] & 0x0F000000) >> 24) * 10;
        pxVersionInfo->x_IoLcaBiu3Ver.ul_MinorVer       = ( infolw[8] & 0x000F0000) >> 16;
        pxVersionInfo->x_CoreLcaBiu3Ver.ul_MajorVer     = ((infolw[8] & 0x000000F0) >>  4) + ((infolw[8] & 0x00000F00) >>  8) * 10;
        pxVersionInfo->x_CoreLcaBiu3Ver.ul_MinorVer     = ( infolw[8] & 0x0000000F);

        pxVersionInfo->x_IoLcaBiu4Ver.ul_MajorVer       = ((infolw[9] & 0x000000F0) >>  4) + ((infolw[9] & 0x00000F00) >>  8) * 10;
        pxVersionInfo->x_IoLcaBiu4Ver.ul_MinorVer       = ( infolw[9] & 0x0000000F);
        pxVersionInfo->x_EndDecLcaBiu4Ver.ul_MajorVer   = ((infolw[9] & 0x00F00000) >> 20) + ((infolw[9] & 0x0F000000) >> 24) * 10;
        pxVersionInfo->x_EndDecLcaBiu4Ver.ul_MinorVer   = ( infolw[9] & 0x000F0000) >> 16;
    }
    else
    {
        pxVersionInfo->x_IoLcaBiu3Ver.ul_MajorVer       = ((infolw[8] & 0x00F00000) >> 20) + ((infolw[8] & 0x0F000000) >> 24) * 10;
        pxVersionInfo->x_IoLcaBiu3Ver.ul_MinorVer       = ( infolw[8] & 0x000F0000) >> 16;
        pxVersionInfo->x_CoreLcaBiu3Ver.ul_MajorVer     = ((infolw[8] & 0x000000F0) >>  4) + ((infolw[8] & 0x00000F00) >>  8) * 10;
        pxVersionInfo->x_CoreLcaBiu3Ver.ul_MinorVer     = ( infolw[8] & 0x0000000F);

        pxVersionInfo->x_IoLcaBiu4Ver.ul_MajorVer       = ((infolw[9] & 0x00F00000) >> 20) + ((infolw[9] & 0x0F000000) >> 24) * 10;
        pxVersionInfo->x_IoLcaBiu4Ver.ul_MinorVer       = ( infolw[9] & 0x000F0000) >> 16;
        pxVersionInfo->x_EndDecLcaBiu4Ver.ul_MajorVer   = ((infolw[9] & 0x000000F0) >>  4) + ((infolw[9] & 0x00000F00) >>  8) * 10;
        pxVersionInfo->x_EndDecLcaBiu4Ver.ul_MinorVer   = ( infolw[9] & 0x0000000F);
    }




    return API_OK;
}



/*! \brief This function reads extended versions from the device (internal) */
/*!

    \deprecated This function is only available for compatibility (up/downgrade). 
                For new developments use \see ApiReadVersions

    \param [in]  ulModHandle           The module handle.
    \param [out] px_VersionInfo        Version information package.
    \param [out] puc_BspCompatibility  The compatibility flag -> returns always API_BSP_COMPATIBLE.
    
    \return                            Zero on normal termination
*/
AiInt16 ApiReadBSPVersionEx_( AiUInt32              ulModHandle,
                             TY_API_VERSION_INFO * px_VersionInfo, 
                             AiUInt8             * puc_BspCompatibility )
{
    AiInt16  wRetVal          = API_OK;
    AiUInt32 ulTempVersion    = 0;

	TY_VER_INFO      driver_version;
    TY_DEVICE_INFO * pDevice = _ApiGetDeviceInfoPtrByModule( ulModHandle );

    if( pDevice == NULL )
        return API_ERR_NO_MODULE_EXTENSION;

    // -- Check input parameters ---

    if( !px_VersionInfo )
        return API_ERR_PARAM2_IS_NULL;

    if( !puc_BspCompatibility )
        return API_ERR_PARAM3_IS_NULL;


    memset( px_VersionInfo, 0, sizeof(TY_API_VERSION_INFO) );


    // -- Library ---

    px_VersionInfo->x_DllVer.ul_MajorVer        = API_MIL_MAJOR_VERSION;
    px_VersionInfo->x_DllVer.ul_MinorVer        = API_MIL_MINOR_VERSION;
    px_VersionInfo->x_DllVer.ul_BuildNo         = API_MIL_PATCH_VERSION;
    px_VersionInfo->x_DllVer.ul_MajorSpecialVer = 0;
    px_VersionInfo->x_DllVer.ul_MinorSpecialVer = 0;


    // -- Legacy TSW version read required for new host to target communication ---

    wRetVal = ApiReadTswCompatVersion(ulModHandle, &px_VersionInfo->x_TargetSWVer);

    if (wRetVal != API_OK)
        return wRetVal;


    // -- Driver ---
    memset(&driver_version, 0, sizeof(TY_VER_INFO));
    wRetVal = _ApiCmdGetSysDrvVersion(ulModHandle, &driver_version);

    if (wRetVal == API_OK)
    {
        px_VersionInfo->x_SysDrvVer.ul_MajorVer = driver_version.ul_MajorVer;
        px_VersionInfo->x_SysDrvVer.ul_MinorVer = driver_version.ul_MinorVer;
        px_VersionInfo->x_SysDrvVer.ul_BuildNo  = driver_version.ul_PatchVersion;
    }
    else
    {
        /* No driver version available E.g driver is part of embedded linux version */
        memset(&px_VersionInfo->x_SysDrvVer, 0, sizeof(TY_API_VERSION));
    }

    // -- Get on-board versions ---

    if( pDevice->uc_DeviceGroup == AI_DEVICE_AYE )
    {
        // Read versions from global ram extenstion for AEC
        wRetVal = _MilApiHwGramExtReadVersion( ulModHandle, px_VersionInfo );

        if( wRetVal != API_OK )
            return wRetVal;
    }
    else if( pDevice->uc_DeviceGroup == AI_DEVICE_AYS_ASP )
    {
        wRetVal = _ApiHwAysReadVersion( ulModHandle, px_VersionInfo );

        if( wRetVal != API_OK )
            return wRetVal;
    }
    else if (pDevice->uc_DeviceGroup == AI_DEVICE_AYS)
    {
        wRetVal = _ApiHwAysReadVersion(ulModHandle, px_VersionInfo);

        if (wRetVal != API_OK)
            return wRetVal;
    }
    else if (pDevice->uc_DeviceGroup == AI_DEVICE_AYS_ASP_MA)
    {
        wRetVal = _ApiHwAysReadVersion(ulModHandle, px_VersionInfo);

        if (wRetVal != API_OK)
            return wRetVal;
    }
    else if (pDevice->uc_DeviceGroup == AI_DEVICE_ZYNQMP_ASP)
    {
        wRetVal = _ApiHwAysReadVersion(ulModHandle, px_VersionInfo);

        if (wRetVal != API_OK)
            return wRetVal;
    }
    else
    {
        // Read versions from TSW for AyX/Amc/AmcAsp generation
        wRetVal = _MilApiHwTswReadVersion( ulModHandle, px_VersionInfo );

        if( wRetVal != API_OK )
            return wRetVal;
    }

    // -- Check compatibility removed, return always API_BSP_COMPATIBLE ---
    *puc_BspCompatibility = API_BSP_COMPATIBLE;


    return(wRetVal);

}




/*! \brief This function reads extended versions from the device (obsolete) */
/*!

    \deprecated This function is only available for compatibility (up/downgrade). 
                For new developments use \see ApiReadVersions

    \param [in]  ulModHandle           The module handle.
    \param [out] px_VersionInfo        Version information package.
    \param [out] puc_BspCompatibility  The compatibility flag.
    
    \return                            Zero on normal termination
*/
AI_LIB_FUNC AiReturn AI_CALL_CONV ApiReadBSPVersionEx( AiUInt32              ulModHandle,
                                                      TY_API_VERSION_INFO * px_VersionInfo, 
                                                      AiUInt8             * puc_BspCompatibility )
{
    AiInt16 wRetVal = API_OK;

    wRetVal = ApiReadBSPVersionEx_( ulModHandle, px_VersionInfo, puc_BspCompatibility );


    TRACE_BEGIN
        TRACE("        AiUInt8             uc_BspCompatibility;\n");
    TRACE("        TY_API_VERSION_INFO x_VersionInfo;\n");
    TRACE_FCTA("ApiReadBSPVersionEx", wRetVal); 
    TRACE_PARA(ulModHandle);
    TRACE_RPARA("&x_VersionInfo");
    TRACE_RPARE("&uc_BspCompatibility");
    TRACE_FCTE;
    TRACE1("    --> x_VersionInfo.ul_BoardSerialNo                      = 0x%x;\n", px_VersionInfo->ul_BoardSerialNo);
    TRACE1("    --> x_VersionInfo.x_AslLcaVer.ul_BuildNo                = 0x%x;\n", px_VersionInfo->x_AslLcaVer.ul_BuildNo);
    TRACE1("    --> x_VersionInfo.x_AslLcaVer.ul_MajorSpecialVer        = 0x%x;\n", px_VersionInfo->x_AslLcaVer.ul_MajorSpecialVer);
    TRACE1("    --> x_VersionInfo.x_AslLcaVer.ul_MajorVer               = 0x%x;\n", px_VersionInfo->x_AslLcaVer.ul_MajorVer);
    TRACE1("    --> x_VersionInfo.x_AslLcaVer.ul_MinorSpecialVer        = 0x%x;\n", px_VersionInfo->x_AslLcaVer.ul_MinorSpecialVer);
    TRACE1("    --> x_VersionInfo.x_AslLcaVer.ul_MinorVer               = 0x%x;\n", px_VersionInfo->x_AslLcaVer.ul_MinorVer);
    TRACE1("    --> x_VersionInfo.x_CoreLcaBiu1Ver.ul_BuildNo           = 0x%x;\n", px_VersionInfo->x_CoreLcaBiu1Ver.ul_BuildNo);
    TRACE1("    --> x_VersionInfo.x_CoreLcaBiu1Ver.ul_MajorSpecialVer   = 0x%x;\n", px_VersionInfo->x_CoreLcaBiu1Ver.ul_MajorSpecialVer);
    TRACE1("    --> x_VersionInfo.x_CoreLcaBiu1Ver.ul_MajorVer          = 0x%x;\n", px_VersionInfo->x_CoreLcaBiu1Ver.ul_MajorVer);
    TRACE1("    --> x_VersionInfo.x_CoreLcaBiu1Ver.ul_MinorSpecialVer   = 0x%x;\n", px_VersionInfo->x_CoreLcaBiu1Ver.ul_MinorSpecialVer);
    TRACE1("    --> x_VersionInfo.x_CoreLcaBiu1Ver.ul_MinorVer          = 0x%x;\n", px_VersionInfo->x_CoreLcaBiu1Ver.ul_MinorVer);
    TRACE1("    --> x_VersionInfo.x_CoreLcaBiu3Ver.ul_BuildNo           = 0x%x;\n", px_VersionInfo->x_CoreLcaBiu3Ver.ul_BuildNo);
    TRACE1("    --> x_VersionInfo.x_CoreLcaBiu3Ver.ul_MajorSpecialVer   = 0x%x;\n", px_VersionInfo->x_CoreLcaBiu3Ver.ul_MajorSpecialVer);
    TRACE1("    --> x_VersionInfo.x_CoreLcaBiu3Ver.ul_MajorVer          = 0x%x;\n", px_VersionInfo->x_CoreLcaBiu3Ver.ul_MajorVer);
    TRACE1("    --> x_VersionInfo.x_CoreLcaBiu3Ver.ul_MinorSpecialVer   = 0x%x;\n", px_VersionInfo->x_CoreLcaBiu3Ver.ul_MinorSpecialVer);
    TRACE1("    --> x_VersionInfo.x_CoreLcaBiu3Ver.ul_MinorVer          = 0x%x;\n", px_VersionInfo->x_CoreLcaBiu3Ver.ul_MinorVer);
    TRACE1("    --> x_VersionInfo.x_DllVer.ul_BuildNo                   = 0x%x;\n", px_VersionInfo->x_DllVer.ul_BuildNo);
    TRACE1("    --> x_VersionInfo.x_DllVer.ul_MajorSpecialVer           = 0x%x;\n", px_VersionInfo->x_DllVer.ul_MajorSpecialVer);
    TRACE1("    --> x_VersionInfo.x_DllVer.ul_MajorVer                  = 0x%x;\n", px_VersionInfo->x_DllVer.ul_MajorVer);
    TRACE1("    --> x_VersionInfo.x_DllVer.ul_MinorSpecialVer           = 0x%x;\n", px_VersionInfo->x_DllVer.ul_MinorSpecialVer);
    TRACE1("    --> x_VersionInfo.x_DllVer.ul_MinorVer                  = 0x%x;\n", px_VersionInfo->x_DllVer.ul_MinorVer);
    TRACE1("    --> x_VersionInfo.x_EndDecLcaBiu2Ver.ul_BuildNo         = 0x%x;\n", px_VersionInfo->x_EndDecLcaBiu2Ver.ul_BuildNo);
    TRACE1("    --> x_VersionInfo.x_EndDecLcaBiu2Ver.ul_MajorSpecialVer = 0x%x;\n", px_VersionInfo->x_EndDecLcaBiu2Ver.ul_MajorSpecialVer);
    TRACE1("    --> x_VersionInfo.x_EndDecLcaBiu2Ver.ul_MajorVer        = 0x%x;\n", px_VersionInfo->x_EndDecLcaBiu2Ver.ul_MajorVer);
    TRACE1("    --> x_VersionInfo.x_EndDecLcaBiu2Ver.ul_MinorSpecialVer = 0x%x;\n", px_VersionInfo->x_EndDecLcaBiu2Ver.ul_MinorSpecialVer);
    TRACE1("    --> x_VersionInfo.x_EndDecLcaBiu2Ver.ul_MinorVer        = 0x%x;\n", px_VersionInfo->x_EndDecLcaBiu2Ver.ul_MinorVer);
    TRACE1("    --> x_VersionInfo.x_EndDecLcaBiu4Ver.ul_BuildNo         = 0x%x;\n", px_VersionInfo->x_EndDecLcaBiu4Ver.ul_BuildNo);
    TRACE1("    --> x_VersionInfo.x_EndDecLcaBiu4Ver.ul_MajorSpecialVer = 0x%x;\n", px_VersionInfo->x_EndDecLcaBiu4Ver.ul_MajorSpecialVer);
    TRACE1("    --> x_VersionInfo.x_EndDecLcaBiu4Ver.ul_MajorVer        = 0x%x;\n", px_VersionInfo->x_EndDecLcaBiu4Ver.ul_MajorVer);
    TRACE1("    --> x_VersionInfo.x_EndDecLcaBiu4Ver.ul_MinorSpecialVer = 0x%x;\n", px_VersionInfo->x_EndDecLcaBiu4Ver.ul_MinorSpecialVer);
    TRACE1("    --> x_VersionInfo.x_EndDecLcaBiu4Ver.ul_MinorVer        = 0x%x;\n", px_VersionInfo->x_EndDecLcaBiu4Ver.ul_MinorVer);
    TRACE1("    --> x_VersionInfo.x_FirmwareBiu1Ver.ul_BuildNo          = 0x%x;\n", px_VersionInfo->x_FirmwareBiu1Ver.ul_BuildNo);
    TRACE1("    --> x_VersionInfo.x_FirmwareBiu1Ver.ul_MajorSpecialVer  = 0x%x;\n", px_VersionInfo->x_FirmwareBiu1Ver.ul_MajorSpecialVer);
    TRACE1("    --> x_VersionInfo.x_FirmwareBiu1Ver.ul_MajorVer         = 0x%x;\n", px_VersionInfo->x_FirmwareBiu1Ver.ul_MajorVer);
    TRACE1("    --> x_VersionInfo.x_FirmwareBiu1Ver.ul_MinorSpecialVer  = 0x%x;\n", px_VersionInfo->x_FirmwareBiu1Ver.ul_MinorSpecialVer);
    TRACE1("    --> x_VersionInfo.x_FirmwareBiu1Ver.ul_MinorVer         = 0x%x;\n", px_VersionInfo->x_FirmwareBiu1Ver.ul_MinorVer);
    TRACE1("    --> x_VersionInfo.x_FirmwareBiu2Ver.ul_BuildNo          = 0x%x;\n", px_VersionInfo->x_FirmwareBiu2Ver.ul_BuildNo);
    TRACE1("    --> x_VersionInfo.x_FirmwareBiu2Ver.ul_MajorSpecialVer  = 0x%x;\n", px_VersionInfo->x_FirmwareBiu2Ver.ul_MajorSpecialVer);
    TRACE1("    --> x_VersionInfo.x_FirmwareBiu2Ver.ul_MajorVer         = 0x%x;\n", px_VersionInfo->x_FirmwareBiu2Ver.ul_MajorVer);
    TRACE1("    --> x_VersionInfo.x_FirmwareBiu2Ver.ul_MinorSpecialVer  = 0x%x;\n", px_VersionInfo->x_FirmwareBiu2Ver.ul_MinorSpecialVer);
    TRACE1("    --> x_VersionInfo.x_FirmwareBiu2Ver.ul_MinorVer         = 0x%x;\n", px_VersionInfo->x_FirmwareBiu2Ver.ul_MinorVer);
    TRACE1("    --> x_VersionInfo.x_FirmwareBiu3Ver.ul_BuildNo          = 0x%x;\n", px_VersionInfo->x_FirmwareBiu3Ver.ul_BuildNo);
    TRACE1("    --> x_VersionInfo.x_FirmwareBiu3Ver.ul_MajorSpecialVer  = 0x%x;\n", px_VersionInfo->x_FirmwareBiu3Ver.ul_MajorSpecialVer);
    TRACE1("    --> x_VersionInfo.x_FirmwareBiu3Ver.ul_MajorVer         = 0x%x;\n", px_VersionInfo->x_FirmwareBiu3Ver.ul_MajorVer);
    TRACE1("    --> x_VersionInfo.x_FirmwareBiu3Ver.ul_MinorSpecialVer  = 0x%x;\n", px_VersionInfo->x_FirmwareBiu3Ver.ul_MinorSpecialVer);
    TRACE1("    --> x_VersionInfo.x_FirmwareBiu3Ver.ul_MinorVer         = 0x%x;\n", px_VersionInfo->x_FirmwareBiu3Ver.ul_MinorVer);
    TRACE1("    --> x_VersionInfo.x_FirmwareBiu4Ver.ul_BuildNo          = 0x%x;\n", px_VersionInfo->x_FirmwareBiu4Ver.ul_BuildNo);
    TRACE1("    --> x_VersionInfo.x_FirmwareBiu4Ver.ul_MajorSpecialVer  = 0x%x;\n", px_VersionInfo->x_FirmwareBiu4Ver.ul_MajorSpecialVer);
    TRACE1("    --> x_VersionInfo.x_FirmwareBiu4Ver.ul_MajorVer         = 0x%x;\n", px_VersionInfo->x_FirmwareBiu4Ver.ul_MajorVer);
    TRACE1("    --> x_VersionInfo.x_FirmwareBiu4Ver.ul_MinorSpecialVer  = 0x%x;\n", px_VersionInfo->x_FirmwareBiu4Ver.ul_MinorSpecialVer);
    TRACE1("    --> x_VersionInfo.x_FirmwareBiu4Ver.ul_MinorVer         = 0x%x;\n", px_VersionInfo->x_FirmwareBiu4Ver.ul_MinorVer);
    TRACE1("    --> x_VersionInfo.x_IoLcaBiu1Ver.ul_BuildNo             = 0x%x;\n", px_VersionInfo->x_IoLcaBiu1Ver.ul_BuildNo);
    TRACE1("    --> x_VersionInfo.x_IoLcaBiu1Ver.ul_MajorSpecialVer     = 0x%x;\n", px_VersionInfo->x_IoLcaBiu1Ver.ul_MajorSpecialVer);
    TRACE1("    --> x_VersionInfo.x_IoLcaBiu1Ver.ul_MajorVer            = 0x%x;\n", px_VersionInfo->x_IoLcaBiu1Ver.ul_MajorVer);
    TRACE1("    --> x_VersionInfo.x_IoLcaBiu1Ver.ul_MinorSpecialVer     = 0x%x;\n", px_VersionInfo->x_IoLcaBiu1Ver.ul_MinorSpecialVer);
    TRACE1("    --> x_VersionInfo.x_IoLcaBiu1Ver.ul_MinorVer            = 0x%x;\n", px_VersionInfo->x_IoLcaBiu1Ver.ul_MinorVer);
    TRACE1("    --> x_VersionInfo.x_IoLcaBiu2Ver.ul_BuildNo             = 0x%x;\n", px_VersionInfo->x_IoLcaBiu2Ver.ul_BuildNo);
    TRACE1("    --> x_VersionInfo.x_IoLcaBiu2Ver.ul_MajorSpecialVer     = 0x%x;\n", px_VersionInfo->x_IoLcaBiu2Ver.ul_MajorSpecialVer);
    TRACE1("    --> x_VersionInfo.x_IoLcaBiu2Ver.ul_MajorVer            = 0x%x;\n", px_VersionInfo->x_IoLcaBiu2Ver.ul_MajorVer);
    TRACE1("    --> x_VersionInfo.x_IoLcaBiu2Ver.ul_MinorSpecialVer     = 0x%x;\n", px_VersionInfo->x_IoLcaBiu2Ver.ul_MinorSpecialVer);
    TRACE1("    --> x_VersionInfo.x_IoLcaBiu2Ver.ul_MinorVer            = 0x%x;\n", px_VersionInfo->x_IoLcaBiu2Ver.ul_MinorVer);
    TRACE1("    --> x_VersionInfo.x_IoLcaBiu3Ver.ul_BuildNo             = 0x%x;\n", px_VersionInfo->x_IoLcaBiu3Ver.ul_BuildNo);
    TRACE1("    --> x_VersionInfo.x_IoLcaBiu3Ver.ul_MajorSpecialVer     = 0x%x;\n", px_VersionInfo->x_IoLcaBiu3Ver.ul_MajorSpecialVer);
    TRACE1("    --> x_VersionInfo.x_IoLcaBiu3Ver.ul_MajorVer            = 0x%x;\n", px_VersionInfo->x_IoLcaBiu3Ver.ul_MajorVer);
    TRACE1("    --> x_VersionInfo.x_IoLcaBiu3Ver.ul_MinorSpecialVer     = 0x%x;\n", px_VersionInfo->x_IoLcaBiu3Ver.ul_MinorSpecialVer);
    TRACE1("    --> x_VersionInfo.x_IoLcaBiu3Ver.ul_MinorVer            = 0x%x;\n", px_VersionInfo->x_IoLcaBiu3Ver.ul_MinorVer);
    TRACE1("    --> x_VersionInfo.x_IoLcaBiu4Ver.ul_BuildNo             = 0x%x;\n", px_VersionInfo->x_IoLcaBiu4Ver.ul_BuildNo);
    TRACE1("    --> x_VersionInfo.x_IoLcaBiu4Ver.ul_MajorSpecialVer     = 0x%x;\n", px_VersionInfo->x_IoLcaBiu4Ver.ul_MajorSpecialVer);
    TRACE1("    --> x_VersionInfo.x_IoLcaBiu4Ver.ul_MajorVer            = 0x%x;\n", px_VersionInfo->x_IoLcaBiu4Ver.ul_MajorVer);
    TRACE1("    --> x_VersionInfo.x_IoLcaBiu4Ver.ul_MinorSpecialVer     = 0x%x;\n", px_VersionInfo->x_IoLcaBiu4Ver.ul_MinorSpecialVer);
    TRACE1("    --> x_VersionInfo.x_IoLcaBiu4Ver.ul_MinorVer            = 0x%x;\n", px_VersionInfo->x_IoLcaBiu4Ver.ul_MinorVer);
    TRACE1("    --> x_VersionInfo.x_MonitorVer.ul_BuildNo               = 0x%x;\n", px_VersionInfo->x_MonitorVer.ul_BuildNo);
    TRACE1("    --> x_VersionInfo.x_MonitorVer.ul_MajorSpecialVer       = 0x%x;\n", px_VersionInfo->x_MonitorVer.ul_MajorSpecialVer);
    TRACE1("    --> x_VersionInfo.x_MonitorVer.ul_MajorVer              = 0x%x;\n", px_VersionInfo->x_MonitorVer.ul_MajorVer);
    TRACE1("    --> x_VersionInfo.x_MonitorVer.ul_MinorSpecialVer       = 0x%x;\n", px_VersionInfo->x_MonitorVer.ul_MinorSpecialVer);
    TRACE1("    --> x_VersionInfo.x_MonitorVer.ul_MinorVer              = 0x%x;\n", px_VersionInfo->x_MonitorVer.ul_MinorVer);
    TRACE1("    --> x_VersionInfo.x_PciLcaVer.ul_BuildNo                = 0x%x;\n", px_VersionInfo->x_PciLcaVer.ul_BuildNo);
    TRACE1("    --> x_VersionInfo.x_PciLcaVer.ul_MajorSpecialVer        = 0x%x;\n", px_VersionInfo->x_PciLcaVer.ul_MajorSpecialVer);
    TRACE1("    --> x_VersionInfo.x_PciLcaVer.ul_MajorVer               = 0x%x;\n", px_VersionInfo->x_PciLcaVer.ul_MajorVer);
    TRACE1("    --> x_VersionInfo.x_PciLcaVer.ul_MinorSpecialVer        = 0x%x;\n", px_VersionInfo->x_PciLcaVer.ul_MinorSpecialVer);
    TRACE1("    --> x_VersionInfo.x_PciLcaVer.ul_MinorVer               = 0x%x;\n", px_VersionInfo->x_PciLcaVer.ul_MinorVer);
    TRACE1("    --> x_VersionInfo.x_SysDrvVer.ul_BuildNo                = 0x%x;\n", px_VersionInfo->x_SysDrvVer.ul_BuildNo);
    TRACE1("    --> x_VersionInfo.x_SysDrvVer.ul_MajorSpecialVer        = 0x%x;\n", px_VersionInfo->x_SysDrvVer.ul_MajorSpecialVer);
    TRACE1("    --> x_VersionInfo.x_SysDrvVer.ul_MajorVer               = 0x%x;\n", px_VersionInfo->x_SysDrvVer.ul_MajorVer);
    TRACE1("    --> x_VersionInfo.x_SysDrvVer.ul_MinorSpecialVer        = 0x%x;\n", px_VersionInfo->x_SysDrvVer.ul_MinorSpecialVer);
    TRACE1("    --> x_VersionInfo.x_SysDrvVer.ul_MinorVer               = 0x%x;\n", px_VersionInfo->x_SysDrvVer.ul_MinorVer);
    TRACE1("    --> x_VersionInfo.x_TargetSWVer.ul_BuildNo              = 0x%x;\n", px_VersionInfo->x_TargetSWVer.ul_BuildNo);
    TRACE1("    --> x_VersionInfo.x_TargetSWVer.ul_MajorSpecialVer      = 0x%x;\n", px_VersionInfo->x_TargetSWVer.ul_MajorSpecialVer);
    TRACE1("    --> x_VersionInfo.x_TargetSWVer.ul_MajorVer             = 0x%x;\n", px_VersionInfo->x_TargetSWVer.ul_MajorVer);
    TRACE1("    --> x_VersionInfo.x_TargetSWVer.ul_MinorSpecialVer      = 0x%x;\n", px_VersionInfo->x_TargetSWVer.ul_MinorSpecialVer);
    TRACE1("    --> x_VersionInfo.x_TargetSWVer.ul_MinorVer             = 0x%x;\n", px_VersionInfo->x_TargetSWVer.ul_MinorVer);
    TRACE1("    --> x_VersionInfo.x_TcpVer.ul_BuildNo                   = 0x%x;\n", px_VersionInfo->x_TcpVer.ul_BuildNo);
    TRACE1("    --> x_VersionInfo.x_TcpVer.ul_MajorSpecialVer           = 0x%x;\n", px_VersionInfo->x_TcpVer.ul_MajorSpecialVer);
    TRACE1("    --> x_VersionInfo.x_TcpVer.ul_MajorVer                  = 0x%x;\n", px_VersionInfo->x_TcpVer.ul_MajorVer);
    TRACE1("    --> x_VersionInfo.x_TcpVer.ul_MinorSpecialVer           = 0x%x;\n", px_VersionInfo->x_TcpVer.ul_MinorSpecialVer);
    TRACE1("    --> x_VersionInfo.x_TcpVer.ul_MinorVer                  = 0x%x;\n", px_VersionInfo->x_TcpVer.ul_MinorVer);
    TRACE_END

        return(wRetVal);

} // end: ApiReadBSPVersionEx


AI_LIB_FUNC AiReturn AI_CALL_CONV ApiReadTswCompatVersion(AiUInt32 ulModHandle, TY_API_VERSION * version)
{
	AiInt16 ret;
	AiUInt16 tsw_compat;

	ret = ApiCmdReadSWVersion(ulModHandle, 0, NULL, &tsw_compat, NULL, NULL);

	version->ul_MajorVer = ((tsw_compat & 0x0F00) >> 8) + ((tsw_compat & 0xF000) >> 12) * 10;
	version->ul_MinorVer = (tsw_compat & 0x000F) + ((tsw_compat & 0x00F0) >> 4) * 10;

    return ret;
}

AiInt16 _ApiReadVersionFromDll( AiUInt32 ul_Handle, TY_E_VERSION_ID eId, TY_VER_INFO * pxVersion )
{
    if( eId != AI_DLL_VER )
        return API_ERR_INVALID_MODE;

    pxVersion->ul_VersionType  = AI_DLL_VER;
    pxVersion->ul_MajorVer     = API_MIL_MAJOR_VERSION;
    pxVersion->ul_MinorVer     = API_MIL_MINOR_VERSION;
    pxVersion->ul_PatchVersion = API_MIL_PATCH_VERSION;
    pxVersion->ul_BuildNr      = 0;

    strcpy( pxVersion->ac_Description, "API" );
    strcpy( pxVersion->ac_FullVersion, API_MIL_VERSION_STRING_FULL   );

    return 0;
}

AiInt16 _ApiReadVersionFromServer( AiUInt32 ul_Handle, TY_E_VERSION_ID eId, TY_VER_INFO * pxVersion )
{
    AiInt16 retval = 0;
    TY_API_SERVERINFO xServerInfo;

    if( GET_SERVER_ID(ul_Handle) )
    {
        /* We are connected to a server */

        /* Check if version read command is available */
        retval = ApiGetServerInfo( ul_Handle, &xServerInfo );

        if( retval != 0 )
            return retval;

        if( eId == AI_ANS_VER )
        {
            /* -- Read server -- */
            memcpy( pxVersion, &xServerInfo.server_version, sizeof(TY_VER_INFO) );

            return 0;
        }
        else if( eId == AI_REMOTE_DLL_VER )
        {
            if ((xServerInfo.protocol_major>1)
            || ((xServerInfo.protocol_major==1) && (xServerInfo.protocol_minor>0)))
            {
                    /* Version read command is available  */
                    /* -- Read server LIB (local lib from server point of view) -- */
                    retval = _MilNetReadVersion( ul_Handle, AI_DLL_VER, pxVersion );

                    /* -- Patch id and description --- */
                    pxVersion->ul_VersionType = AI_REMOTE_DLL_VER;
                    strcpy( pxVersion->ac_Description, "ANS API" );
                    return retval;
                
            }
        }
    }

    return API_ERR_INVALID_MODE;
}

AiReturn _ApiReadVersion( AiUInt32 ul_Handle, TY_E_VERSION_ID eId, TY_VER_INFO * pxVersion )
{
    AiInt16 retval = 0;
    TY_MIL_COM_VERSION_INPUT    xTswInput;
    TY_MIL_COM_VERSION_OUTPUT   xTswOutput;


    memset(pxVersion, 0, sizeof(TY_VER_INFO));
    memset(&xTswInput, 0, sizeof(xTswInput));
    memset(&xTswOutput, 0, sizeof(xTswOutput));

    MIL_COM_INIT(&xTswInput.xCommand, API_STREAM(ul_Handle), 0, MIL_COM_READ_VERSION, sizeof(xTswInput), sizeof(xTswOutput));

    xTswInput.eId = eId;

    retval = ApiIoStruct(ul_Handle, (TY_MIL_COM*)&xTswInput, (TY_MIL_COM_ACK*)&xTswOutput);

    if (retval != API_OK)
        return retval;

    pxVersion->ul_VersionType  = xTswOutput.xVersionValues.ul_VersionType;
    pxVersion->ul_MajorVer     = xTswOutput.xVersionValues.ul_MajorVer;
    pxVersion->ul_MinorVer     = xTswOutput.xVersionValues.ul_MinorVer;
    pxVersion->ul_PatchVersion = xTswOutput.xVersionValues.ul_PatchVersion;
    pxVersion->ul_BuildNr      = xTswOutput.xVersionValues.ul_BuildNr;

    strcpy(pxVersion->ac_Description, xTswOutput.xVersionStrings.ac_Description);
    strcpy(pxVersion->ac_FullVersion, xTswOutput.xVersionStrings.ac_FullVersion);


    return retval;
}



/*! \brief This function reads a specific version */
/*!
    This function returns the version in px_Version. If the version does
    not exist the functions returns API_ERR_INVALID_MODE.

    \param [in]  ul_Handle          The module handle.
    \param [in]  eId                The version id to read.
    \param [out] px_Version         Pointer to store the version info.
    
    \return                         Zero if the version is valid or API_ERR_INVALID_MODE if the version does not exist
*/
AI_LIB_FUNC AiReturn AI_CALL_CONV ApiReadVersion( AiUInt32 ul_Handle, TY_E_VERSION_ID eId, TY_VER_INFO * pxVersion )
{
    AiInt16  retval   = 0;
    TY_DEVICE_INFO* pDevice = _ApiGetDeviceInfoPtrByModule(ul_Handle);

    if (pDevice == NULL)
    {
        return API_ERR_NO_MODULE_EXTENSION;
    }

    /* -- check input parameters -- */

    if (NULL == pxVersion) 
        return API_ERR_PARAM3_IS_NULL;

    memset(pxVersion, 0, sizeof(TY_VER_INFO));

    switch (eId)
    {
    case AI_DLL_VER:
        retval = _ApiReadVersionFromDll(ul_Handle, eId, pxVersion);
        break;

    case AI_SYS_DRV_VER:
        retval = _ApiCmdGetSysDrvVersion(ul_Handle, pxVersion);
        break;

    case AI_ANS_VER:
        retval = _ApiReadVersionFromServer(ul_Handle, eId, pxVersion);
        break;

    case AI_REMOTE_DLL_VER:
        if (GET_SERVER_ID(ul_Handle))
        {
            retval = _ApiReadVersionFromServer(ul_Handle, eId, pxVersion);
        }
        else
        {
            retval = API_ERR_INVALID_MODE;
        }
        break;

    case AI_TARGET_VER:
        if ( !IsTswVersionGreaterOrEqual(pDevice, 22, 00) )
        {
            pxVersion->ul_VersionType  = AI_TARGET_VER;
            pxVersion->ul_MajorVer     = pDevice->ax_VersionInfo.x_TargetSWVer.ul_MajorVer;
            pxVersion->ul_MinorVer     = pDevice->ax_VersionInfo.x_TargetSWVer.ul_MinorVer;
            pxVersion->ul_PatchVersion = 0;
            pxVersion->ul_BuildNr      = pDevice->ax_VersionInfo.x_TargetSWVer.ul_BuildNo;

            sprintf(pxVersion->ac_Description, "TSW (compat)");
            sprintf(pxVersion->ac_FullVersion, "%d.%d.%d", pxVersion->ul_MajorVer, pxVersion->ul_MinorVer, pxVersion->ul_BuildNr);

            retval = API_OK;
        }
        else
        {
            retval = _ApiReadVersion(ul_Handle, eId, pxVersion);
        }
        break;


    default:

        if (!IsTswVersionGreaterOrEqual(pDevice, 22, 00))
        {
            retval = API_ERR_INVALID_MODE;
        }
        else
        {
            retval = _ApiReadVersion(ul_Handle, eId, pxVersion);
        }

        break;
    }

    return retval;
}

/*! \brief This function reads all available versions */
/*!
    A count of 0 can be used to obtain the number of available
    versions in px_VersionInfoOut. The array px_VersionInfoInOut has
    to be pre allocated by the application. The size must be big enought
    to hold count items. The array shall be cleared with memset before
    this function is called.

    \param [in]  ul_Handle            The module handle.
    \param [in]  px_VersionInfoIn     Defines how many versions to read.
    \param [out] px_VersionInfoInOut  The pre allocated version arry (size=count).
    \param [out] px_VersionInfoOut    Information about the available and returned versions.
    
    \return                           Zero on normal termination
*/
AI_LIB_FUNC AiReturn AI_CALL_CONV ApiReadAllVersions(AiUInt32             ul_ModuleHandle,
														AiUInt32          ul_Count,
														TY_VER_INFO       ax_Versions[],
														TY_VERSION_OUT*   px_VersionInfoOut)
{
	AiInt16  version_found = 0;
	unsigned int current_index = 0;
	int current_version_id = 0;
	TY_DEVICE_INFO * device;

	device = _ApiGetDeviceInfoPtrByModule(ul_ModuleHandle);
	if (device == NULL)
	{
		return API_ERR_NO_MODULE_EXTENSION;
	}

	if (!px_VersionInfoOut)
	{
		return API_ERR_PARAM3_IS_NULL;
	}

	/* -- Set default values -- */
	px_VersionInfoOut->ul_Count = 0;
	px_VersionInfoOut->ul_MaxCount = AI_MAX_VERSIONS;

	/* Now iterate over all known versions and try to read them.
	* Abort if no more output structures are left for storing versions */
	while (current_index < ul_Count && current_version_id < AI_MAX_VERSIONS)
	{
		version_found = ApiReadVersion(ul_ModuleHandle, (TY_E_VERSION_ID)current_version_id, &ax_Versions[current_index]);

		if (version_found == API_OK)
		{
			/* Version was found, forward to next version structure */
			current_index++;
		}
		else if (version_found != API_ERR_INVALID_MODE)
		{
			/* Version should be existent but could not be read
			* So abort reading and return the error
			* */
			return version_found;
		}

		/* Forward to next version ID in every case */
		current_version_id++;
	}

	/* store the number of versions read */
	px_VersionInfoOut->ul_Count = current_index;

	return API_OK;
}


/*! \brief This function reads the system driver version */
/*!
    \param [in]  ul_ModuleHandle   The module handle.
    \param [in]  pul_SysDrvVer     The system driver version.
    \param [out] pul_SysDrvBuild   The system driver build.
    
    \return                        Zero on normal termination.
*/
AI_LIB_FUNC AiReturn AI_CALL_CONV ApiCmdGetSysDrvVersion(AiUInt32 handle, AiUInt32* pul_SysDrvVer, AiUInt32* pul_SysDrvBuild)
{
	AiInt16 ret = API_OK;
	TY_VER_INFO     version;

    if(!pul_SysDrvVer)
		return API_ERR_PARAM2_IS_NULL;
    
	if(!pul_SysDrvBuild)
		return API_ERR_PARAM3_IS_NULL;

    ret = _ApiCmdGetSysDrvVersion(handle, &version );
    
    *pul_SysDrvVer   = version.ul_MajorVer * 100;
	*pul_SysDrvVer  += version.ul_MinorVer;
	*pul_SysDrvBuild = version.ul_BuildNr;

	return ret;
}



