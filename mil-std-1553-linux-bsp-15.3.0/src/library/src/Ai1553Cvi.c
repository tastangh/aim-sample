/* SPDX-FileCopyrightText: 2001-2022 AIM GmbH <info@aim-online.com> */
/* SPDX-License-Identifier: MIT */

/*! \file Ai1553Cvi.c
 *
 *  This module contain wrapper functions, which are used for for CVI access.
 *
 */

//***************************************************************************
//  API1553-DS Functions - External References
//***************************************************************************

//***************************************************************************
//    API Driver Functions
//***************************************************************************

#include "AiOs.h"
#include "Ai1553Cvi.h"
#include "Ai1553i_fnc.h"
#include "Ai1553_Internal_def.h"


#ifdef _AIM_WINDOWS
#include "os/windows/labview/extcode.h"
#endif

#pragma warning( disable : 4996 ) // disable sprintf, strcpy warning

#ifdef _AIM_LINUX
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
#endif


AI_LIB_FUNC M_RETVAL_TYPE AI_CALL_CONV M_APIINIT(void)
{
    M_RETVAL_TYPE   retVal = M_RETVAL_INI;

    if( API_OK == (retVal=ApiInit()) )
        retVal = M_RETVAL_OK;

    return retVal;
}

AI_LIB_FUNC M_RETVAL_TYPE AI_CALL_CONV M_APIEXIT(void)
{
    M_RETVAL_TYPE   retVal = M_RETVAL_INI;

    if( API_OK == (retVal=ApiExit()) )
        retVal = M_RETVAL_OK;

    return retVal;
}

#if defined(_AIM_WINDOWS) || defined(_AIM_VXI) || defined(_AIM_VMEWINDOWS) || defined(_AIM_LINUX)
AI_LIB_FUNC M_RETVAL_TYPE AI_CALL_CONV M_APIOPEN(VI_SESSION M_BOARDID, AiChar *SrvName, AiUInt32 *ModuleHandle)
{
    AiUInt32         tempBModule;
    M_RETVAL_TYPE   retVal = M_RETVAL_INI;

    if( TRUE == M_GETMODULE( M_BOARDID, &tempBModule) )
    {

        if( API_OK == (retVal=ApiOpen(tempBModule, SrvName, ModuleHandle)) )  // changed 20.03.2001
            retVal = M_RETVAL_OK;
    }
    else
        retVal = M_RETVAL_ERR;

    return retVal;
}
#else
AI_LIB_FUNC M_RETVAL_TYPE AI_CALL_CONV M_APIOPEN(VI_SESSION M_BOARDID)
{
    AiUInt32         tempBModule;
    M_RETVAL_TYPE   retVal = M_RETVAL_INI;

    if( TRUE == M_GETMODULE( M_BOARDID, &tempBModule) )
    {
        if( API_OK == (retVal=ApiOpen(tempBModule)) )
            retVal = M_RETVAL_OK;
    }
    else
        retVal = M_RETVAL_ERR;

    return retVal;
}
#endif

AI_LIB_FUNC M_RETVAL_TYPE AI_CALL_CONV M_APIOPENEX(VI_SESSION M_BOARDID, AiUInt32 ul_Stream, AiChar *ac_SrvName, AiUInt32 *pul_ModuleHandle)
{
    AiUInt32      tempBModule;
    M_RETVAL_TYPE retVal = M_RETVAL_INI;
    TY_API_OPEN   x_ApiOpen;

    if( TRUE == M_GETMODULE( M_BOARDID, &tempBModule) )
    {
        x_ApiOpen.ul_Module = tempBModule;
        x_ApiOpen.ul_Stream = ul_Stream;
        strcpy(x_ApiOpen.ac_SrvName, ac_SrvName);

        if( API_OK == (retVal=ApiOpenEx(&x_ApiOpen, pul_ModuleHandle)) )
            retVal = M_RETVAL_OK;
    }
    else
        retVal = M_RETVAL_ERR;

    return retVal;
}

AI_LIB_FUNC M_RETVAL_TYPE AI_CALL_CONV M_APICLOSE(VI_SESSION M_BOARDID)
{
    AiUInt32         tempBModule;
    M_RETVAL_TYPE   retVal = M_RETVAL_INI;

    if( TRUE == M_GETMODULE( M_BOARDID, &tempBModule) )
    {
        if( API_OK == (retVal=ApiClose(tempBModule)) )
            retVal = M_RETVAL_OK;
    }
    else
        retVal = M_RETVAL_ERR;

    return retVal;
}



AI_LIB_FUNC M_RETVAL_TYPE AI_CALL_CONV M_APIGETDRIVERINFO(VI_SESSION M_BOARDID, 
                                                           AiUInt8  * puc_DeviceGroup,
                                                           AiUInt8  * puc_ReservedB2, 
                                                           AiUInt8  * puc_ReservedB3,
                                                           AiUInt8  * puc_ReservedB4, 
                                                           AiUInt16 * puw_ReservedW1,
                                                           AiUInt16 * puw_ReservedW2, 
                                                           AiUInt32 * pul_DriverFlags,
                                                           AiUInt32 * pul_SN, 
                                                           AiUInt32 * pul_BoardConfig,
                                                           AiUInt32 * pul_BoardType, 
                                                           AiUInt32 * pul_OpenConnections,
                                                           AiUInt32 * pul_ReservedLW6,
                                                           AiUInt32 * pul_ReservedLW7,
                                                           AiUInt32 * pul_ReservedLW8 )
{
    AiUInt32          tempBModule;
    M_RETVAL_TYPE     retVal = M_RETVAL_INI;
    TY_API_DRIVER_INFO x_DriverInfo;

    if( TRUE == M_GETMODULE( M_BOARDID, &tempBModule) )
    {
        if( API_OK == (retVal=ApiGetDriverInfo(tempBModule,&x_DriverInfo)) )
        {
            if(puc_DeviceGroup)    *puc_DeviceGroup    = x_DriverInfo.uc_DeviceGroup;
            if(puc_ReservedB2)     *puc_ReservedB2     = x_DriverInfo.uc_ReservedB2;
            if(puc_ReservedB3)     *puc_ReservedB3     = x_DriverInfo.uc_ReservedB3;
            if(puc_ReservedB4)     *puc_ReservedB4     = x_DriverInfo.uc_ReservedB4;
            if(puw_ReservedW1)     *puw_ReservedW1     = x_DriverInfo.uw_ReservedW1;
            if(puw_ReservedW2)     *puw_ReservedW2     = x_DriverInfo.uw_ReservedW2;
            if(pul_DriverFlags)    *pul_DriverFlags    = x_DriverInfo.ul_DriverFlags;
            if(pul_SN)             *pul_SN             = x_DriverInfo.ul_SN;
            if(pul_BoardConfig)    *pul_BoardConfig    = x_DriverInfo.ul_BoardConfig;
            if(pul_BoardType)      *pul_BoardType      = x_DriverInfo.ul_BoardType;
            if(pul_OpenConnections)*pul_OpenConnections= x_DriverInfo.ul_OpenConnections;
            if(pul_ReservedLW6)    *pul_ReservedLW6    = x_DriverInfo.ul_ReservedLW6;
            if(pul_ReservedLW7)    *pul_ReservedLW7    = x_DriverInfo.ul_ReservedLW7;
            if(pul_ReservedLW8)    *pul_ReservedLW8    = x_DriverInfo.ul_ReservedLW8;
            retVal = M_RETVAL_OK;
        }
    }
    else
        retVal = M_RETVAL_ERR;

    return retVal;
}

AI_LIB_FUNC M_RETVAL_TYPE AI_CALL_CONV M_APIGETDEVICECONFIG(VI_SESSION M_BOARDID, AiUInt8 *uc_DmaEnabled, AiUInt8 *uc_DataQueueMemoryType,
                                                             AiUInt8 *uc_DataQueueMode, AiUInt8 *uc_ReservedB4, AiUInt16 *uw_ReservedW1, AiUInt16 *uw_ReservedW2, 
                                                             AiUInt32 *ul_DmaMinimumSize, AiUInt32 *ul_IntRequestCount, AiUInt32 *ul_DriverFlags,
                                                             AiUInt32 *ul_ReservedLW4, AiUInt32 *ul_ReservedLW5, AiUInt32 *ul_ReservedLW6, 
                                                             AiUInt32 *ul_ReservedLW7, AiUInt32 *ul_ReservedLW8 )
{
    AiUInt32         tempBModule;
    M_RETVAL_TYPE   retVal = M_RETVAL_INI;
    TY_API_DEVICE_CONFIG x_DeviceConfig;

    memset( &x_DeviceConfig, 0, sizeof(x_DeviceConfig));

    if( TRUE == M_GETMODULE( M_BOARDID, &tempBModule) )
    {
        if( API_OK == (retVal=ApiGetDeviceConfig(tempBModule,&x_DeviceConfig)) )
        {
            *uc_DmaEnabled          = x_DeviceConfig.uc_DmaEnabled;
            *uc_DataQueueMemoryType    = x_DeviceConfig.uc_DataQueueMemoryType;
            *uc_DataQueueMode         = x_DeviceConfig.uc_DataQueueMode;
            *uc_ReservedB4            = x_DeviceConfig.uc_ReservedB4;
            *uw_ReservedW1            = x_DeviceConfig.uw_ReservedW1;
            *uw_ReservedW2            = x_DeviceConfig.uw_ReservedW2;
            *ul_DmaMinimumSize        = x_DeviceConfig.ul_DmaMinimumSize;
            *ul_IntRequestCount        = x_DeviceConfig.ul_IntRequestCount;
            *ul_DriverFlags            = x_DeviceConfig.ul_DriverFlags;
            *ul_ReservedLW4            = x_DeviceConfig.ul_ReservedLW4;
            *ul_ReservedLW5            = x_DeviceConfig.ul_ReservedLW5;
            *ul_ReservedLW6            = x_DeviceConfig.ul_ReservedLW6;
            *ul_ReservedLW7            = x_DeviceConfig.ul_ReservedLW7;
            *ul_ReservedLW8            = x_DeviceConfig.ul_ReservedLW8;

            retVal = M_RETVAL_OK;
        }
    }
    else
        retVal = M_RETVAL_ERR;

    return retVal;
}

AI_LIB_FUNC M_RETVAL_TYPE AI_CALL_CONV M_APISETDEVICECONFIG(VI_SESSION M_BOARDID, AiUInt8 uc_DmaEnabled, AiUInt8 uc_DataQueueMemoryType,
                                                             AiUInt8 uc_DataQueueMode, AiUInt8 uc_ReservedB4, AiUInt16 uw_ReservedW1, AiUInt16 uw_ReservedW2, 
                                                             AiUInt32 ul_DmaMinimumSize, AiUInt32 ul_IntRequestCount, AiUInt32 ul_DriverFlags,
                                                             AiUInt32 ul_ReservedLW4, AiUInt32 ul_ReservedLW5, AiUInt32 ul_ReservedLW6, 
                                                             AiUInt32 ul_ReservedLW7, AiUInt32 ul_ReservedLW8 )
{
    AiUInt32         tempBModule;
    M_RETVAL_TYPE   retVal = M_RETVAL_INI;
    TY_API_DEVICE_CONFIG x_DeviceConfig;

    memset( &x_DeviceConfig, 0, sizeof(x_DeviceConfig));

    if( TRUE == M_GETMODULE( M_BOARDID, &tempBModule) )
    {
        x_DeviceConfig.uc_DmaEnabled             = uc_DmaEnabled;
        x_DeviceConfig.uc_DataQueueMemoryType    = uc_DataQueueMemoryType;
        x_DeviceConfig.uc_DataQueueMode          = uc_DataQueueMode;
        x_DeviceConfig.uc_ReservedB4             = uc_ReservedB4;
        x_DeviceConfig.uw_ReservedW1             = uw_ReservedW1;
        x_DeviceConfig.uw_ReservedW2             = uw_ReservedW2;
        x_DeviceConfig.ul_DmaMinimumSize         = ul_DmaMinimumSize;
        x_DeviceConfig.ul_IntRequestCount        = ul_IntRequestCount;
        x_DeviceConfig.ul_DriverFlags            = ul_DriverFlags;
        x_DeviceConfig.ul_ReservedLW4            = ul_ReservedLW4;
        x_DeviceConfig.ul_ReservedLW5            = ul_ReservedLW5;
        x_DeviceConfig.ul_ReservedLW6            = ul_ReservedLW6;
        x_DeviceConfig.ul_ReservedLW7            = ul_ReservedLW7;
        x_DeviceConfig.ul_ReservedLW8            = ul_ReservedLW8;

        if( API_OK == (retVal=ApiSetDeviceConfig(tempBModule,&x_DeviceConfig)) )
        {
            retVal = M_RETVAL_OK;
        }
    }
    else
        retVal = M_RETVAL_ERR;

    return retVal;
}



AI_LIB_FUNC M_RETVAL_TYPE AI_CALL_CONV M_APIGETBOARDINFO(VI_SESSION M_BOARDID, AiUInt32 *pul_DeviceType,
                                                          AiUInt32 *pul_NumberOfChannels, AiUInt32 *pul_NumberOfBiu,
                                                          AiUInt32 *pul_NovRamBoardType, AiUInt32 *pul_NovRamBoardConfig,
                                                          AiUInt32 *pul_SerialNumber, AiUInt32 *pul_PartNumber,
                                                          AiUInt32 *pul_GlobalRamSize, AiUInt32 *pul_SharedRamSize,
                                                          AiUInt32 *pul_CanChangeAmplitude, AiUInt32 *pul_CouplingCapabilities,
                                                          AiUInt32 *pul_IrigCapabilities )
{
    AiUInt32         tempBModule;
    M_RETVAL_TYPE   retVal = M_RETVAL_INI;
    TY_API_BOARD_INFO x_BoardInfo;

    if( TRUE == M_GETMODULE( M_BOARDID, &tempBModule) )
    {
        if( API_OK == (retVal=ApiGetBoardInfo(tempBModule,&x_BoardInfo)) )
        {
            *pul_DeviceType           = x_BoardInfo.ul_DeviceType;
            *pul_NumberOfChannels     = x_BoardInfo.ul_NumberOfChannels;
            *pul_NumberOfBiu          = x_BoardInfo.ul_NumberOfBiu;
            *pul_NovRamBoardType      = x_BoardInfo.ul_NovRamBoardType;
            *pul_NovRamBoardConfig    = x_BoardInfo.ul_NovRamBoardConfig;
            *pul_SerialNumber         = x_BoardInfo.ul_SerialNumber;
            *pul_PartNumber           = x_BoardInfo.ul_PartNumber;
            *pul_GlobalRamSize        = x_BoardInfo.ul_GlobalRamSize;
            *pul_SharedRamSize        = x_BoardInfo.ul_SharedRamSize;
            *pul_CanChangeAmplitude   = x_BoardInfo.x_BoardCapabilities.ul_CanChangeAmplitude;
            *pul_CouplingCapabilities = x_BoardInfo.x_BoardCapabilities.x_CouplingCapabilities.ul_All;
            *pul_IrigCapabilities     = x_BoardInfo.x_BoardCapabilities.x_IrigCapabilities.ul_All;
            retVal = M_RETVAL_OK;
        }
    }
    else
        retVal = M_RETVAL_ERR;

    return retVal;
}

AI_LIB_FUNC M_RETVAL_TYPE AI_CALL_CONV M_APIGETLIBRARYINFO(AiUInt32 *pul_AttachedApps)
{
    M_RETVAL_TYPE   retVal = M_RETVAL_INI;
    TY_API_LIB_INFO x_LibInfo;

    if( API_OK == (retVal=ApiGetLibraryInfo(&x_LibInfo)) )
    {
        *pul_AttachedApps = x_LibInfo.ul_AttachedApps;
        retVal = M_RETVAL_OK;
    }
    else
        retVal = M_RETVAL_ERR;

    return retVal;
}

AI_LIB_FUNC const char * AI_CALL_CONV M_APIGETERRORDESCRIPTION(AiInt16 wErrorCode)
{
    return ApiGetErrorDescription( wErrorCode );
}

AI_LIB_FUNC const char * AI_CALL_CONV M_APIGETERRORMESSAGE(AiReturn error_code)
{
    return ApiGetErrorMessage(error_code);
}


AI_LIB_FUNC const char * AI_CALL_CONV M_APIGETBOARDNAME(AiUInt32 ulModHandle)
{
    return ApiGetBoardName( ulModHandle );
}


AI_LIB_FUNC M_RETVAL_TYPE AI_CALL_CONV M_APICMDINI(VI_SESSION M_BOARDID, AiUInt8 mode,
                                                    AiUInt8 bt[4],
                                                    AiUInt8* chns,
                                                    AiUInt8* prot,
                                                    AiUInt8* emod,
                                                    AiUInt8* irg,
                                                    AiUInt8* res1,
                                                    AiUInt16* pbi_id_biu1,
                                                    AiUInt16* asp_mon_id,
                                                    AiUInt16* asp_bite_id,
                                                    AiUInt16* pbi_id_biu2,
                                                    AiUInt32* board_config,
                                                    AiUInt32* glb_mem_size,
                                                    AiUInt32* glb_mem_addr,
                                                    AiUInt32* loc_dram_size,
                                                    AiUInt32* loc_dram_addr,
                                                    AiUInt32* shared_dram_size,
                                                    AiUInt32* shared_dram_addr,
                                                    AiUInt32* flash_ram_size,
                                                    AiUInt32* flash_ram_addr,
                                                    AiUInt32 pci[16],
                                                    AiUInt32 * board_type,
                                                    AiUInt32 * board_sub_type,
                                                    AiUInt32 * hardware_variant )
{
    AiUInt32         tempBModule;
    M_RETVAL_TYPE   retVal = M_RETVAL_INI;

    TY_API_INI_INFO pini;
    AiUInt8         i;

    if( TRUE == M_GETMODULE( M_BOARDID, &tempBModule) )
    {
        if( API_OK == (retVal=ApiCmdIni( tempBModule, mode, &pini)) )
        {
            for(i=0;i<4;i++)                           // CVI bisher in jedem FALL / VXI nur bei OK
            {
                bt[i] = pini.bt[i];
            }
            *chns = pini.chns;
            *prot =pini.prot;
            *emod = pini.emod;
            *irg = pini.irg;
            *res1 = pini.res1;
            *pbi_id_biu1 = pini.pbi_id_biu1;
            *asp_mon_id = pini.asp_mon_id;
            *asp_bite_id = pini.asp_bite_id;
            *pbi_id_biu2 = pini.pbi_id_biu2;
            *board_config = pini.board_config;
            *glb_mem_size = pini.glb_mem_size;
            *glb_mem_addr = pini.glb_mem_addr;
            *loc_dram_size = pini.loc_dram_size;
            *loc_dram_addr = pini.loc_dram_addr;
            *shared_dram_size = pini.shared_dram_size;
            *shared_dram_addr = pini.shared_dram_addr;
            *flash_ram_size = pini.flash_ram_size;
            *flash_ram_addr = pini.flash_ram_addr;
            for(i=0;i<16;i++)
            {
                pci[i] = pini.pci[i];
            }

            *board_type       = pini.board_type;
            *board_sub_type   = pini.board_sub_type;
            *hardware_variant = pini.hardware_variant;

            retVal = M_RETVAL_OK;
        }
        else
            retVal = M_RETVAL_ERR;
    }

    return retVal;
}


AI_LIB_FUNC M_RETVAL_TYPE AI_CALL_CONV M_APICMDRESET(VI_SESSION M_BOARDID, AiUInt8 biu, AiUInt8 rc,
                                                      AiUInt8* mbufs,
                                                      AiUInt8* sbufs,
                                                      AiUInt32* mon_addr,
                                                      AiUInt32* sim_addr)
{
    AiUInt32         tempBModule;

    M_RETVAL_TYPE   retVal = M_RETVAL_INI;

    TY_API_RESET_INFO pres;

    if( TRUE == M_GETMODULE( M_BOARDID, &tempBModule) )
    {

        if( API_OK == (retVal=ApiCmdReset(tempBModule,  biu,  rc, &pres)) )
        {

            *mbufs = pres.mbufs;
            *sbufs = pres.sbufs;
            *mon_addr = pres.mon_addr;
            *sim_addr= pres.sim_addr;
            retVal = M_RETVAL_OK;
        }
        else
            retVal = M_RETVAL_ERR;

    }



    return retVal;
}

AI_LIB_FUNC M_RETVAL_TYPE AI_CALL_CONV M_APICMDBITE(VI_SESSION M_BOARDID, AiUInt8 biu, AiUInt8 sc, AiUInt8 bite_status[])
{
    AiUInt32         tempBModule;
    M_RETVAL_TYPE   retVal = M_RETVAL_INI;

    if( TRUE == M_GETMODULE( M_BOARDID, &tempBModule) )
    {
        if( API_OK == (retVal=ApiCmdBite(tempBModule, biu, sc, bite_status)) )
        {
            retVal = M_RETVAL_OK;
        }
        else
            retVal = M_RETVAL_ERR;
    }

    return retVal;
}

AI_LIB_FUNC M_RETVAL_TYPE AI_CALL_CONV M_APICMDDEFRESPTOUT(VI_SESSION M_BOARDID, AiUInt8 biu, AiFloat resp_tout)
{
    AiUInt32         tempBModule;
    M_RETVAL_TYPE   retVal = M_RETVAL_INI;

    if( TRUE == M_GETMODULE( M_BOARDID, &tempBModule) )
    {
        if( API_OK == (retVal=ApiCmdDefRespTout(tempBModule, biu, resp_tout)) )
        {
            retVal = M_RETVAL_OK;
        }
        else
            retVal = M_RETVAL_ERR;
    }

    return retVal;
}

AI_LIB_FUNC M_RETVAL_TYPE AI_CALL_CONV M_APICMDREADSWVERSION(VI_SESSION M_BOARDID, AiUInt8 biu, AiUInt16* fw_id, AiUInt16* sw_id, AiUInt16* lca_id, AiUInt32* lca_chks)
{
    AiUInt32         tempBModule;
    M_RETVAL_TYPE   retVal = M_RETVAL_INI;

    if( TRUE == M_GETMODULE( M_BOARDID, &tempBModule) )
    {
        if( API_OK == (retVal=ApiCmdReadSWVersion(tempBModule, biu, fw_id, sw_id, lca_id, lca_chks)) )
        {
            retVal = M_RETVAL_OK;
        }
        else
            retVal = M_RETVAL_ERR;
    }

    return retVal;
}


AI_LIB_FUNC M_RETVAL_TYPE AI_CALL_CONV M_APICMDLOADSREC(VI_SESSION M_BOARDID, AiUInt8 mode, AiUInt8 cnt, AiUInt32 offset, AiUInt8* srec, AiUInt8* st, AiUInt32* fsize)
{
    AiUInt32         tempBModule;
    M_RETVAL_TYPE   retVal = M_RETVAL_INI;

    if( TRUE == M_GETMODULE( M_BOARDID, &tempBModule) )
    {
        if( API_OK == (retVal=ApiCmdLoadSRec(tempBModule, mode, cnt, offset, srec, st, fsize)) )
        {
            retVal = M_RETVAL_OK;
        }
        else
            retVal = M_RETVAL_ERR;
    }

    return retVal;
}

AI_LIB_FUNC M_RETVAL_TYPE AI_CALL_CONV M_APICMDPROGFLASH(VI_SESSION M_BOARDID, AiUInt32 src_addr, AiUInt32 sector, AiUInt32 size, AiUInt8* st)
{
    AiUInt32         tempBModule;
    M_RETVAL_TYPE   retVal = M_RETVAL_INI;

    if( TRUE == M_GETMODULE( M_BOARDID, &tempBModule) )
    {
        if( API_OK == (retVal=ApiCmdProgFlash(tempBModule, src_addr, sector, size, st)) )
        {
            retVal = M_RETVAL_OK;
        }
        else
            retVal = M_RETVAL_ERR;
    }

    return retVal;
}


AI_LIB_FUNC M_RETVAL_TYPE AI_CALL_CONV M_APICMDEXECSYS(VI_SESSION M_BOARDID, AiUInt8 mode, AiUInt8 con)
{
    AiUInt32         tempBModule;
    M_RETVAL_TYPE   retVal = M_RETVAL_INI;

    if( TRUE == M_GETMODULE( M_BOARDID, &tempBModule) )
    {
        if( API_OK == (retVal=ApiCmdExecSys(tempBModule, mode, con)) )
        {
            retVal = M_RETVAL_OK;
        }
        else
            retVal = M_RETVAL_ERR;
    }

    return retVal;
}

AI_LIB_FUNC M_RETVAL_TYPE AI_CALL_CONV M_APICMDSETIRIGTIME(VI_SESSION M_BOARDID,
    AiUInt32 day,
    AiUInt32 hour,
    AiUInt32 minute,
    AiUInt32 second,
    AiUInt32 microsecond)
{
    AiUInt32         tempBModule;

    M_RETVAL_TYPE   retVal = M_RETVAL_INI;

    TY_API_IRIG_TIME time;
    time.day = day;
    time.hour = hour;
    time.minute = minute;
    time.second = second;
    time.microsecond = microsecond;

    if (TRUE == M_GETMODULE(M_BOARDID, &tempBModule))
    {
        if (API_OK == (retVal=ApiCmdSetIrigTime(tempBModule, &time)))
        {

            retVal = M_RETVAL_OK;
        }
        else
            retVal = M_RETVAL_ERR;
    }

    return retVal;
}

AI_LIB_FUNC M_RETVAL_TYPE AI_CALL_CONV M_APICMDSETIRIGSTATUS(VI_SESSION M_BOARDID, TY_API_IRIG_SOURCE irig_src )
{
    AiUInt32         tempBModule;

    M_RETVAL_TYPE   retVal = M_RETVAL_INI;

    if (TRUE == M_GETMODULE(M_BOARDID, &tempBModule))
    {

        if (API_OK == (retVal=ApiCmdSetIrigStatus(tempBModule, irig_src)))
        {
            retVal = M_RETVAL_OK;
        }
        else
            retVal = M_RETVAL_ERR;
    }

    return retVal;
}

AI_LIB_FUNC M_RETVAL_TYPE AI_CALL_CONV M_APICMDGETIRIGTIME(VI_SESSION M_BOARDID,
    AiUInt32* day,
    AiUInt32* hour,
    AiUInt32* minute,
    AiUInt32* second,
    AiUInt32* microsecond)
{
    AiUInt32         tempBModule;
    M_RETVAL_TYPE   retVal = M_RETVAL_INI;

    TY_API_IRIG_TIME time;

    if (TRUE == M_GETMODULE(M_BOARDID, &tempBModule))
    {
        if (API_OK == (retVal= ApiCmdGetIrigTime(tempBModule, &time)))
        {
            *day         = time.day;
            *hour        = time.hour;
            *minute      = time.minute;
            *second      = time.second;
            *microsecond = time.microsecond;

            retVal = M_RETVAL_OK;
        }
        else
            retVal = M_RETVAL_ERR;
    }

    return retVal;
}

AI_LIB_FUNC M_RETVAL_TYPE AI_CALL_CONV M_APICMDGETIRIGSTATUS(VI_SESSION M_BOARDID, TY_API_IRIG_SOURCE* irig_src, AiBoolean * in_sync)
{
    AiUInt32        tempBModule;
    M_RETVAL_TYPE   retVal = M_RETVAL_INI;

    if (TRUE == M_GETMODULE(M_BOARDID, &tempBModule))
    {
        if (API_OK == (retVal= ApiCmdGetIrigStatus(tempBModule, irig_src, in_sync)))
        {
            retVal = M_RETVAL_OK;
        }
        else
            retVal = M_RETVAL_ERR;
    }

    return retVal;
}

AI_LIB_FUNC M_RETVAL_TYPE AI_CALL_CONV M_APICMDDEFMILBUSPROTOCOL(VI_SESSION M_BOARDID, AiUInt8 biu, AiUInt8 mil_prot, AiUInt8 mode, AiUInt8 rt)
{
    AiUInt32         tempBModule;
    M_RETVAL_TYPE   retVal = M_RETVAL_INI;

    if( TRUE == M_GETMODULE( M_BOARDID, &tempBModule) )
    {
        if( API_OK == (retVal=ApiCmdDefMilbusProtocol(tempBModule, biu, mil_prot, mode, rt)) )
            retVal = M_RETVAL_OK;
    }
    else
        retVal = M_RETVAL_ERR;

    return retVal;

}

AI_LIB_FUNC M_RETVAL_TYPE AI_CALL_CONV M_APIREADRECDATA(VI_SESSION M_BOARDID, AiUInt8 biu,
                                                         LPVOID lpBuf,
                                                         AiUInt8* status,
                                                         AiUInt32* hfi_cnt,
                                                         AiUInt32* saddr,
                                                         AiUInt32* size,
                                                         AiUInt32* lBytesRead)
{
    AiUInt32         tempBModule;
    M_RETVAL_TYPE   retVal = M_RETVAL_INI;

    TY_API_BM_REC api_rec_stat;



    if( TRUE == M_GETMODULE( M_BOARDID, &tempBModule) )
    {
        if( API_OK == (retVal=ApiReadRecData(tempBModule, biu,  &api_rec_stat, lpBuf, lBytesRead)) )
        {

            *status = api_rec_stat.status;
            *hfi_cnt = api_rec_stat.hfi_cnt;
            *saddr = api_rec_stat.saddr;
            *size = api_rec_stat.size;


            retVal = M_RETVAL_OK;
        }
        else
            retVal = M_RETVAL_ERR;
    }

    return retVal;
}

AI_LIB_FUNC M_RETVAL_TYPE AI_CALL_CONV M_APIWRITEREPDATA(VI_SESSION M_BOARDID, AiUInt8 biu,
                                                          AiUInt8*  status,
                                                          AiUInt8*  padding1,
                                                          AiUInt16* padding2,
                                                          AiUInt32* rpi_cnt,
                                                          AiUInt32 saddr,
                                                          AiUInt32 size,
                                                          AiUInt32* entry_cnt,
                                                          AiAddr lpBuf,
                                                          AiUInt32* lBytesWritten)
{
    AiUInt32         tempBModule;
    M_RETVAL_TYPE   retVal = M_RETVAL_INI;

    TY_API_REP_STATUS api_rep_stat;

    api_rep_stat.size = size;
    api_rep_stat.saddr = saddr;

    if( TRUE == M_GETMODULE( M_BOARDID, &tempBModule) )
    {
        if( API_OK == (retVal=ApiWriteRepData(tempBModule, biu, &api_rep_stat,lpBuf,lBytesWritten)) )
        {
            *status = api_rep_stat.status;
            *padding1 = api_rep_stat.padding1;
            *padding2 = api_rep_stat.padding2;
            *status = api_rep_stat.status;
            *rpi_cnt = api_rep_stat.rpi_cnt;
            *entry_cnt = api_rep_stat.entry_cnt;

            retVal = M_RETVAL_OK;
        }
        else
            retVal = M_RETVAL_ERR;
    }

    return retVal;
}

// AJR Added 26-JUN-2000
AI_LIB_FUNC M_RETVAL_TYPE AI_CALL_CONV M_APICMDFIFOINI(VI_SESSION M_BOARDID, AiUInt8 biu, AiUInt8 fifo_nbr, AiUInt16 buf_nbr)
{
    AiUInt32         tempBModule;
    M_RETVAL_TYPE   retVal = M_RETVAL_INI;

    if( TRUE == M_GETMODULE( M_BOARDID, &tempBModule) )
    {
        if( API_OK == (retVal=ApiCmdFifoIni(tempBModule, biu, fifo_nbr, buf_nbr)) )
        {
            retVal = M_RETVAL_OK;
        }
        else
            retVal = M_RETVAL_ERR;
    }

    return retVal;
}


AI_LIB_FUNC M_RETVAL_TYPE AI_CALL_CONV M_APIREADBSPVERSION(VI_SESSION M_BOARDID,
                                                            AiUInt32* pul_FirmwareVer, AiUInt32* pul_TargetVer, AiUInt32 *pul_TargetBuild,
                                                            AiUInt32* pul_LcaVer1, AiUInt32* pul_LcaVer2,
                                                            AiUInt32* pul_LcaCheckSum,
                                                            AiUInt32* pul_SysDrvVer, AiUInt32 *pul_SysDrvBuild, AiUInt32* pul_DllVer,
                                                            AiUInt32* pul_DllBuild, AiUInt32* pul_BoardSerialNr,
                                                            AiUInt8*  puc_BspCompatibility)
{
    AiUInt32         tempBModule;
    M_RETVAL_TYPE   retVal = M_RETVAL_INI;

    if( TRUE == M_GETMODULE( M_BOARDID, &tempBModule) )
    {
        if( API_OK == (retVal= ApiReadBSPVersion(tempBModule, pul_FirmwareVer, pul_TargetVer, pul_TargetBuild, pul_LcaVer1, pul_LcaVer2,
            pul_LcaCheckSum, pul_SysDrvVer, pul_SysDrvBuild, pul_DllVer, pul_DllBuild, pul_BoardSerialNr, puc_BspCompatibility)) )        //added MR 31.10.2000
        {
            retVal = M_RETVAL_OK;
        }
        else
            retVal = M_RETVAL_ERR;
    }

    return retVal;
}

AI_LIB_FUNC AiReturn AI_CALL_CONV ApiReadBSPVersionExCvi(AiUInt32 bModule, AiUInt32 aul_TcpVer[5], AiUInt32 aul_AslLcaVer[5], AiUInt32 aul_PciLcaVer[5],
                                                         AiUInt32 aul_IoLcaBiu1Ver[5], AiUInt32 aul_CoreLcaBiu1Ver[5], AiUInt32 aul_IoLcaBiu2Ver[5],
                                                         AiUInt32 aul_EndDecLcaBiu2Ver[5], AiUInt32 aul_IoLcaBiu3Ver[5], AiUInt32 aul_CoreLcaBiu3Ver[5],
                                                         AiUInt32 aul_IoLcaBiu4Ver[5], AiUInt32 aul_EndDecLcaBiu4Ver[5], AiUInt32 aul_FirmwareBiu1Ver[5],
                                                         AiUInt32 aul_FirmwareBiu2Ver[5], AiUInt32 aul_FirmwareBiu3Ver[5], AiUInt32 aul_FirmwareBiu4Ver[5],
                                                         AiUInt32 aul_TargetSWVer[5], AiUInt32 aul_SysDrvVer[5], AiUInt32 aul_DllVer[5], AiUInt32 aul_MonitorVer[5],
                                                         AiUInt32 *pul_BoardSerialNo, AiUInt8  *puc_BspCompatibility)
{
    AiUInt32         tempBModule;
    M_RETVAL_TYPE   retVal = M_RETVAL_INI;

    TY_API_VERSION_INFO xVersionInfo;

    memset( &xVersionInfo, 0, sizeof(xVersionInfo) );

    if( TRUE == M_GETMODULE( M_BOARDID, &tempBModule) )
    {
        if( API_OK == (retVal= ApiReadBSPVersionEx(tempBModule, &xVersionInfo, puc_BspCompatibility)) )
        {
            retVal = M_RETVAL_OK;

            /*            aul_TcpVer[0]            = xVersionInfo.x_TcpVer.ul_MajorVer;
            aul_TcpVer[1]            = xVersionInfo.x_TcpVer.ul_MinorVer;
            aul_TcpVer[2]            = xVersionInfo.x_TcpVer.ul_BuildNo;
            aul_TcpVer[3]            = xVersionInfo.x_TcpVer.ul_MajorSpecialVer;
            aul_TcpVer[4]            = xVersionInfo.x_TcpVer.ul_MinorSpecialVer; */
            memcpy( aul_TcpVer, &xVersionInfo.x_TcpVer.ul_MajorVer, 5 * sizeof(AiUInt32));
            memcpy( aul_AslLcaVer, &xVersionInfo.x_AslLcaVer.ul_MajorVer, 5 * sizeof(AiUInt32));
            memcpy( aul_PciLcaVer, &xVersionInfo.x_PciLcaVer.ul_MajorVer, 5 * sizeof(AiUInt32));
            memcpy( aul_IoLcaBiu1Ver, &xVersionInfo.x_IoLcaBiu1Ver.ul_MajorVer, 5 * sizeof(AiUInt32));
            memcpy( aul_CoreLcaBiu1Ver,    &xVersionInfo.x_CoreLcaBiu1Ver.ul_MajorVer, 5 * sizeof(AiUInt32));
            memcpy( aul_IoLcaBiu2Ver, &xVersionInfo.x_IoLcaBiu2Ver.ul_MajorVer, 5 * sizeof(AiUInt32));
            memcpy( aul_EndDecLcaBiu2Ver, &xVersionInfo.x_EndDecLcaBiu2Ver.ul_MajorVer, 5 * sizeof(AiUInt32));
            memcpy( aul_IoLcaBiu3Ver, &xVersionInfo.x_IoLcaBiu3Ver.ul_MajorVer, 5 * sizeof(AiUInt32));
            memcpy( aul_CoreLcaBiu3Ver,    &xVersionInfo.x_CoreLcaBiu3Ver.ul_MajorVer, 5 * sizeof(AiUInt32));
            memcpy( aul_IoLcaBiu4Ver, &xVersionInfo.x_IoLcaBiu4Ver.ul_MajorVer, 5 * sizeof(AiUInt32) );
            memcpy( aul_EndDecLcaBiu4Ver, &xVersionInfo.x_EndDecLcaBiu4Ver.ul_MajorVer, 5 * sizeof(AiUInt32));
            memcpy( aul_FirmwareBiu1Ver, &xVersionInfo.x_FirmwareBiu1Ver.ul_MajorVer, 5 * sizeof(AiUInt32));
            memcpy( aul_FirmwareBiu2Ver, &xVersionInfo.x_FirmwareBiu2Ver.ul_MajorVer, 5 * sizeof(AiUInt32));
            memcpy( aul_FirmwareBiu3Ver, &xVersionInfo.x_FirmwareBiu3Ver.ul_MajorVer, 5 * sizeof(AiUInt32));
            memcpy( aul_FirmwareBiu4Ver, &xVersionInfo.x_FirmwareBiu4Ver.ul_MajorVer, 5 * sizeof(AiUInt32));
            memcpy( aul_TargetSWVer, &xVersionInfo.x_TargetSWVer.ul_MajorVer, 5 * sizeof(AiUInt32));
            memcpy( aul_SysDrvVer, &xVersionInfo.x_SysDrvVer.ul_MajorVer, 5 * sizeof(AiUInt32));
            memcpy( aul_DllVer,    &xVersionInfo.x_DllVer.ul_MajorVer, 5 * sizeof(AiUInt32));
            memcpy( aul_MonitorVer,    &xVersionInfo.x_MonitorVer.ul_MajorVer, 5 * sizeof(AiUInt32)); 
            *pul_BoardSerialNo        = xVersionInfo.ul_BoardSerialNo;
        }
        else
            retVal = M_RETVAL_ERR;
    }

    return retVal;
}

AI_LIB_FUNC AiReturn AI_CALL_CONV ApiReadVersionCvi(AiUInt32 board_handle, TY_E_VERSION_ID eId, AiUInt32* version_type, AiChar description[AI_DESCRIPTION_STRINGL], AiUInt32* major,                              /*!< Major version. Must be increased when component interface gets incompatible (e.g. API call signature change) */
                                                      AiUInt32* minor, AiUInt32* patch, AiUInt32* build, AiChar full_version[AI_VERSION_STRINGL])
{
    TY_VER_INFO version;
    AiReturn ret;

    ret = ApiReadVersion(board_handle, eId, &version);
    if (ret != API_OK)
    {
        return ret;
    }

    *version_type = version.ul_VersionType;
    AiOsStrncpy(description, version.ac_Description, AI_DESCRIPTION_STRINGL, AiTrue);
    *major = version.ul_MajorVer;
    *minor = version.ul_MinorVer;
    *patch = version.ul_PatchVersion;
    *build = version.ul_BuildNr;
    AiOsStrncpy(full_version, version.ac_FullVersion, AI_VERSION_STRINGL, AiTrue);

    return ret;
}



// AJR Added 26-JUN-2000
AI_LIB_FUNC M_RETVAL_TYPE AI_CALL_CONV M_APICMDSYSTAGDEF(VI_SESSION M_BOARDID, AiUInt8 biu,
                                                          AiUInt8 id, AiUInt8 con, AiUInt8 mode,
                                                          AiUInt16 xid_rtsa,
                                                          AiUInt16 fct,
                                                          AiUInt16 min,
                                                          AiUInt16 max,
                                                          AiUInt16 step,
                                                          AiUInt16 wpos,
                                                          AiUInt16 bpos
                                                          )
{
    AiUInt32         tempBModule;
    M_RETVAL_TYPE   retVal = M_RETVAL_INI;

    TY_API_SYSTAG psystag;
    psystag.xid_rtsa = xid_rtsa;
    psystag.fct = fct;
    psystag.min = min;
    psystag.max = max;
    psystag.step = step;
    psystag.wpos = wpos;
    psystag.bpos = bpos;

    if( TRUE == M_GETMODULE( M_BOARDID, &tempBModule) )
    {
        if( API_OK == (retVal=ApiCmdSystagDef(tempBModule, biu, id, con, mode, &psystag)) )
        {
            retVal = M_RETVAL_OK;
        }
        else
            retVal = M_RETVAL_ERR;
    }

    return retVal;
}

// AJR Added 26-JUN-2000
AI_LIB_FUNC M_RETVAL_TYPE AI_CALL_CONV M_APICMDSYSTAGCON(VI_SESSION M_BOARDID, AiUInt8 biu, AiUInt8 id, AiUInt8 con)
{
    AiUInt32         tempBModule;
    M_RETVAL_TYPE   retVal = M_RETVAL_INI;

    if( TRUE == M_GETMODULE( M_BOARDID, &tempBModule) )
    {
        if( API_OK == (retVal=ApiCmdSystagCon(tempBModule, biu, id, con)) )
        {
            retVal = M_RETVAL_OK;
        }
        else
            retVal = M_RETVAL_ERR;
    }

    return retVal;
}


AI_LIB_FUNC M_RETVAL_TYPE AI_CALL_CONV M_APICMDTRACKDEF(VI_SESSION M_BOARDID, AiUInt8 biu, AiUInt8 uc_TrackId, AiUInt8 uc_BT,
                                                         AiUInt16 uw_XidRtSa, AiUInt8 uc_Mode, AiUInt16 uw_TrackStartPos,
                                                         AiUInt8 uc_TrackBPos, AiUInt8 uc_TrackBLen, AiUInt16 uw_TrackLen,
                                                         AiUInt16 uw_MultiplexedTrackNb, AiUInt16 uw_ContinousTrackNb,
                                                         AiUInt32 *pul_TrackBufferStartAddr)
{
    AiUInt32         tempBModule;
    M_RETVAL_TYPE   retVal = M_RETVAL_INI;

    if( TRUE == M_GETMODULE( M_BOARDID, &tempBModule) )
    {
        if( API_OK == (retVal=ApiCmdTrackDef(tempBModule, biu, uc_TrackId, uc_BT, uw_XidRtSa, uc_Mode, uw_TrackStartPos,
            uc_TrackBPos, uc_TrackBLen, uw_TrackLen, uw_MultiplexedTrackNb,
            uw_ContinousTrackNb, pul_TrackBufferStartAddr)) )
        {
            retVal = M_RETVAL_OK;
        }
        else
            retVal = M_RETVAL_ERR;
    }

    return retVal;
}

AI_LIB_FUNC M_RETVAL_TYPE AI_CALL_CONV M_APICMDTRACKDEFEX(VI_SESSION M_BOARDID, AiUInt8 biu, AiUInt32 ul_TrackId,
                                                           AiUInt32 ul_BT,
                                                           AiUInt32 ul_XidRtSa, AiUInt32 ul_Mode, AiUInt32 ul_TrackStartPos,
                                                           AiUInt32 ul_TrackBPos, AiUInt32 ul_TrackBLen, AiUInt32 ul_TrackLen,
                                                           AiUInt32 ul_MultiplexedTrackNb, AiUInt32 ul_ContinousTrackNb,
                                                           AiInt32 l_Offset, AiUInt32 *pul_TrackBufferStartAddr)
{
    AiUInt32         tempBModule;
    M_RETVAL_TYPE   retVal = M_RETVAL_INI;
    TY_API_TRACK_DEF_IN  x_TrackDefIn;
    TY_API_TRACK_DEF_OUT x_TrackDefOut;

    if( TRUE == M_GETMODULE( M_BOARDID, &tempBModule) )
    {
        x_TrackDefIn.ul_TrackId            = ul_TrackId;
        x_TrackDefIn.ul_BT                 = ul_BT;
        x_TrackDefIn.ul_XidRtSa            = ul_XidRtSa;
        x_TrackDefIn.ul_Mode               = ul_Mode;
        x_TrackDefIn.ul_TrackStartPos      = ul_TrackStartPos;
        x_TrackDefIn.ul_TrackBPos          = ul_TrackBPos;
        x_TrackDefIn.ul_TrackBLen          = ul_TrackBLen;
        x_TrackDefIn.ul_TrackLen           = ul_TrackLen;
        x_TrackDefIn.ul_MultiplexedTrackNb = ul_MultiplexedTrackNb;
        x_TrackDefIn.ul_ContinousTrackNb   = ul_ContinousTrackNb;
        x_TrackDefIn.l_Offset              = l_Offset;
        if( API_OK == (retVal=ApiCmdTrackDefEx(tempBModule, biu, &x_TrackDefIn, &x_TrackDefOut)) )
        {
            *pul_TrackBufferStartAddr = x_TrackDefOut.ul_TrackBufferStartAddr;
            retVal = M_RETVAL_OK;
        }
        else
            retVal = M_RETVAL_ERR;
    }

    return retVal;
}

AI_LIB_FUNC M_RETVAL_TYPE AI_CALL_CONV M_APICMDTRACKREAD(VI_SESSION M_BOARDID, AiUInt8 biu, AiUInt8 uc_TrackId, AiUInt16 uw_MultiplexedTrackIndex,
                                                          AiUInt8 uc_ReadMode, AiUInt8 *puc_DataValid, AiUInt16 *puw_TrackDataWords)
{
    AiUInt32         tempBModule;
    M_RETVAL_TYPE   retVal = M_RETVAL_INI;

    if( TRUE == M_GETMODULE( M_BOARDID, &tempBModule) )
    {
        if( API_OK == (retVal=ApiCmdTrackRead(tempBModule, biu, uc_TrackId, uw_MultiplexedTrackIndex,
            uc_ReadMode, puc_DataValid, puw_TrackDataWords)) )
        {
            retVal = M_RETVAL_OK;
        }
        else
            retVal = M_RETVAL_ERR;
    }

    return retVal;
}

AI_LIB_FUNC M_RETVAL_TYPE AI_CALL_CONV M_APICMDTRACKREADEX(VI_SESSION M_BOARDID, AiUInt8 biu, AiUInt32 ul_TrackId,
                                                            AiUInt32 ul_MultiplexedTrackIndex,
                                                            AiUInt32 ul_ReadMode, AiUInt32 *pul_DataValid, AiUInt32 *pul_LastTT,
                                                            AiUInt16 *puw_TrackDataWords)
{
    AiUInt32         tempBModule;
    M_RETVAL_TYPE   retVal = M_RETVAL_INI;
    TY_API_TRACK_READ_IN  x_TrackReadIn;
    TY_API_TRACK_READ_OUT x_TrackReadOut;

    if( TRUE == M_GETMODULE( M_BOARDID, &tempBModule) )
    {
        x_TrackReadOut.puw_TrackDataWords      = puw_TrackDataWords;
        x_TrackReadIn.ul_TrackId               = ul_TrackId;
        x_TrackReadIn.ul_MultiplexedTrackIndex = ul_MultiplexedTrackIndex;
        x_TrackReadIn.ul_ReadMode              = ul_ReadMode;
        if( API_OK == (retVal=ApiCmdTrackReadEx(tempBModule, biu, &x_TrackReadIn, &x_TrackReadOut)) )
        {
            *pul_DataValid = x_TrackReadOut.ul_DataValid;
            *pul_LastTT = x_TrackReadOut.ul_LastTT;
            retVal = M_RETVAL_OK;
        }
        else
            retVal = M_RETVAL_ERR;
    }

    return retVal;
}

AI_LIB_FUNC M_RETVAL_TYPE AI_CALL_CONV M_APICMDTRACKSCAN(VI_SESSION M_BOARDID, AiUInt8 biu, AiUInt32 ul_TrackId,
                                                          AiUInt32 ul_ChunkNb, AiUInt32 ul_ChunkSize, 
                                                          AiUInt32 *pul_NumberOfReturnedStates,
                                                          AiUInt32 *pul_MoreData,
                                                          AiUInt32 *pul_ReturnedStates)
{
    AiUInt32         tempBModule;
    M_RETVAL_TYPE   retVal = M_RETVAL_INI;
    TY_API_TRACK_SCAN_IN  x_TrackScanIn;
    TY_API_TRACK_SCAN_OUT x_TrackScanOut;

    if( TRUE == M_GETMODULE( M_BOARDID, &tempBModule) )
    {
        x_TrackScanOut.pul_ReturnedStates = pul_ReturnedStates;
        x_TrackScanIn.ul_TrackId  = ul_TrackId;
        x_TrackScanIn.ul_ChunkNb  = ul_ChunkNb;
        x_TrackScanIn.ul_ChunkSize= ul_ChunkSize;
        if( API_OK == (retVal=ApiCmdTrackScan(tempBModule, biu, &x_TrackScanIn, &x_TrackScanOut)) )
        {
            *pul_NumberOfReturnedStates = x_TrackScanOut.ul_NumberOfReturnedStates;
            *pul_MoreData = x_TrackScanOut.ul_MoreData;
            retVal = M_RETVAL_OK;
        }
        else
            retVal = M_RETVAL_ERR;
    }

    return retVal;
}

AI_LIB_FUNC M_RETVAL_TYPE AI_CALL_CONV M_APICMDTRACKPREALLOC(VI_SESSION M_BOARDID, AiUInt8 biu, AiUInt32 ul_TrackId,
                                                              AiUInt32 ul_PreAllocNb, AiUInt32 *pul_MuxStates, 
                                                              AiUInt32 *pul_TrackBufferStartAddr)
{
    AiUInt32         tempBModule;
    M_RETVAL_TYPE   retVal = M_RETVAL_INI;
    TY_API_TRACK_PREALLOC_IN  x_TrackPreAllocIn;
    TY_API_TRACK_PREALLOC_OUT x_TrackPreAllocOut;

    if( TRUE == M_GETMODULE( M_BOARDID, &tempBModule) )
    {
        x_TrackPreAllocOut.pul_TrackBufferStartAddr = pul_TrackBufferStartAddr;
        x_TrackPreAllocIn.ul_TrackId    = ul_TrackId;
        x_TrackPreAllocIn.ul_PreAllocNb = ul_PreAllocNb;
        x_TrackPreAllocIn.pul_MuxStates = pul_MuxStates;
        if( API_OK == (retVal=ApiCmdTrackPreAlloc(tempBModule, biu, &x_TrackPreAllocIn, &x_TrackPreAllocOut)) )
            retVal = M_RETVAL_OK;
        else
            retVal = M_RETVAL_ERR;
    }

    return retVal;
}


AI_LIB_FUNC M_RETVAL_TYPE AI_CALL_CONV M_APICMDREADDISCRETES(VI_SESSION M_BOARDID, AiUInt32 *pul_Value)
{
    AiUInt32         tempBModule;
    M_RETVAL_TYPE   retVal = M_RETVAL_INI;

    if( TRUE == M_GETMODULE( M_BOARDID, &tempBModule) )
    {
        if( API_OK == (retVal=ApiCmdReadDiscretes(tempBModule, pul_Value)) )
        {
            retVal = M_RETVAL_OK;
        }
        else
            retVal = M_RETVAL_ERR;
    }

    return retVal;
}

AI_LIB_FUNC M_RETVAL_TYPE AI_CALL_CONV M_APICMDREADDISCRETESCONFIG(VI_SESSION M_BOARDID, AiUInt32 *pul_Value)
{
    AiUInt32         tempBModule;
    M_RETVAL_TYPE   retVal = M_RETVAL_INI;

    if( TRUE == M_GETMODULE( M_BOARDID, &tempBModule) )
    {
        if( API_OK == (retVal=ApiCmdReadDiscretesConfig(tempBModule, pul_Value)) )
        {
            retVal = M_RETVAL_OK;
        }
        else
            retVal = M_RETVAL_ERR;
    }

    return retVal;
}


AI_LIB_FUNC M_RETVAL_TYPE AI_CALL_CONV M_APICMDWRITEDISCRETES(VI_SESSION M_BOARDID, AiUInt32 ul_Mask, AiUInt32 ul_Value)
{
    AiUInt32         tempBModule;
    M_RETVAL_TYPE   retVal = M_RETVAL_INI;

    if( TRUE == M_GETMODULE( M_BOARDID, &tempBModule) )
    {
        if( API_OK == (retVal=ApiCmdWriteDiscretes(tempBModule, ul_Mask, ul_Value)) )
        {
            retVal = M_RETVAL_OK;
        }
        else
            retVal = M_RETVAL_ERR;
    }

    return retVal;
}

AI_LIB_FUNC M_RETVAL_TYPE AI_CALL_CONV M_APICMDINITDISCRETES(VI_SESSION M_BOARDID, AiUInt32 ul_DiscreteSetup)
{
    AiUInt32         tempBModule;
    M_RETVAL_TYPE   retVal = M_RETVAL_INI;

    if (TRUE == M_GETMODULE(M_BOARDID, &tempBModule))
    {
        if (API_OK == (retVal=ApiCmdInitDiscretes(tempBModule, ul_DiscreteSetup)))
        {
            retVal = M_RETVAL_OK;
        }
        else
            retVal = M_RETVAL_ERR;
    }

    return retVal;
}

AI_LIB_FUNC M_RETVAL_TYPE AI_CALL_CONV M_APICMDSYSTRIGGEREDGEINPUTSET(VI_SESSION M_BOARDID, AiUInt8 biu, AiUInt32 edge_setup)
{
    AiUInt32         tempBModule;
    M_RETVAL_TYPE   retVal = M_RETVAL_INI;

    if (TRUE == M_GETMODULE(M_BOARDID, &tempBModule))
    {
        if (API_OK == (retVal=ApiCmdSysTriggerEdgeInputSet(tempBModule, biu, edge_setup)))
        {
            retVal = M_RETVAL_OK;
        }
        else
            retVal = M_RETVAL_ERR;
    }

    return retVal;
}

AI_LIB_FUNC M_RETVAL_TYPE AI_CALL_CONV M_APICMDSYSTRIGGEREDGEINPUTGET(VI_SESSION M_BOARDID, AiUInt8 biu, AiUInt32 * edge_setup)
{
    AiUInt32         tempBModule;
    M_RETVAL_TYPE   retVal = M_RETVAL_INI;

    if (TRUE == M_GETMODULE(M_BOARDID, &tempBModule))
    {
        if (API_OK == (retVal=ApiCmdSysTriggerEdgeInputGet(tempBModule, biu, edge_setup)))
        {
            retVal = M_RETVAL_OK;
        }
        else
            retVal = M_RETVAL_ERR;
    }

    return retVal;
}

AI_LIB_FUNC M_RETVAL_TYPE AI_CALL_CONV M_APICMDSYSMALLOC(VI_SESSION M_BOARDID, AiUInt8 ucMemtype, AiUInt32 ulSize,   AiUInt32 ulTag, AiUInt32 * pulOffset)
{
    AiUInt32         tempBModule;
    M_RETVAL_TYPE   retVal = M_RETVAL_INI;

    if( TRUE == M_GETMODULE( M_BOARDID, &tempBModule) )
    {
        if( API_OK == (retVal=ApiCmdSysMalloc(tempBModule, ucMemtype, ulSize, ulTag, pulOffset)) )
        {
            retVal = M_RETVAL_OK;
        }
        else
            retVal = M_RETVAL_ERR;
    }

    return retVal;
}

AI_LIB_FUNC M_RETVAL_TYPE AI_CALL_CONV M_APICMDSYSFREE(VI_SESSION M_BOARDID, AiUInt8 ucMemtype, AiUInt32 ulOffset, AiUInt32 ulTag)
{
    AiUInt32         tempBModule;
    M_RETVAL_TYPE   retVal = M_RETVAL_INI;

    if( TRUE == M_GETMODULE( M_BOARDID, &tempBModule) )
    {
        if( API_OK == (retVal=ApiCmdSysFree(tempBModule, ucMemtype, ulOffset, ulTag)) )
        {

            retVal = M_RETVAL_OK;
        }
        else
            retVal = M_RETVAL_ERR;
    }

    return retVal;
}


//***************************************************************************
//    API Buffer Commands
//***************************************************************************
AI_LIB_FUNC M_RETVAL_TYPE AI_CALL_CONV M_APICMDBUFDEF(VI_SESSION M_BOARDID, AiUInt8 biu, AiUInt8 bt, AiUInt16 hid,
                                                       AiUInt16 bid, AiUInt8 len, AiUInt16* data, AiUInt16* rid, AiUInt32* raddr)
{
    AiUInt32         tempBModule;
    M_RETVAL_TYPE   retVal = M_RETVAL_INI;

    if( TRUE == M_GETMODULE( M_BOARDID, &tempBModule) )
    {
        if( API_OK == (retVal=ApiCmdBufDef(tempBModule, biu, bt, hid, bid, len, data, rid, raddr)) )
        {
            retVal = M_RETVAL_OK;
        }
        else
            retVal = M_RETVAL_ERR;
    }

    return retVal;
}

AI_LIB_FUNC M_RETVAL_TYPE AI_CALL_CONV M_APICMDBUFREAD(VI_SESSION M_BOARDID, AiUInt8 biu, AiUInt8 bt, AiUInt16 hid,
                                                        AiUInt16 bid, AiUInt8 len, AiUInt16* data, AiUInt16* rid, AiUInt32* raddr)
{
    AiUInt32         tempBModule;
    M_RETVAL_TYPE   retVal = M_RETVAL_INI;

    if( TRUE == M_GETMODULE( M_BOARDID, &tempBModule) )
    {
        if( API_OK == (retVal=ApiCmdBufRead(tempBModule, biu, bt, hid, bid, len, data, rid, raddr)) )
        {
            retVal = M_RETVAL_OK;
        }
        else
            retVal = M_RETVAL_ERR;
    }

    return retVal;
}

/* 01.07.99 */
AI_LIB_FUNC M_RETVAL_TYPE AI_CALL_CONV M_APICMDBUFWRITE(VI_SESSION M_BOARDID, AiUInt8 biu, AiUInt8 bt, AiUInt16 hid, AiUInt16 bid,
                                                         AiUInt8 d_pos, AiUInt8 b_pos, AiUInt8 b_len, AiUInt16 data)
{
    AiUInt32         tempBModule;
    M_RETVAL_TYPE   retVal = M_RETVAL_INI;

    if( TRUE == M_GETMODULE( M_BOARDID, &tempBModule) )
    {
        if( API_OK == (retVal=ApiCmdBufWrite(tempBModule, biu, bt, hid, bid, d_pos, b_pos, b_len, data)) )
        {
            retVal = M_RETVAL_OK;
        }
        else
            retVal = M_RETVAL_ERR;
    }

    return retVal;
}


AI_LIB_FUNC M_RETVAL_TYPE AI_CALL_CONV M_APICMDBUFWRITEBLOCK(VI_SESSION M_BOARDID, AiUInt8 biu, AiUInt8 bt, AiUInt16 hid,
    AiUInt16 bid, AiUInt8 offset, AiUInt8 len, AiUInt16* data)
{
    AiUInt32         tempBModule;
    M_RETVAL_TYPE   retVal = M_RETVAL_INI;

    if (TRUE == M_GETMODULE(M_BOARDID, &tempBModule))
    {
        if (API_OK == (retVal = ApiCmdBufWriteBlock(tempBModule, biu, bt, hid, bid, offset, len, data)))
        {
            retVal = M_RETVAL_OK;
        }
        else
            retVal = M_RETVAL_ERR;
    }

    return retVal;
}

AI_LIB_FUNC M_RETVAL_TYPE AI_CALL_CONV M_APICMDBUFHOSTCONTROL(VI_SESSION M_BOARDID, AiUInt8 biu, AiUInt8 bt, AiUInt16 hid, AiUInt32 con)
{
    AiUInt32         tempBModule;
    M_RETVAL_TYPE   retVal = M_RETVAL_INI;

    if (TRUE == M_GETMODULE(M_BOARDID, &tempBModule))
    {
        if (API_OK == (retVal = ApiCmdBufHostControl(tempBModule, biu, bt, hid, con)))
        {
            retVal = M_RETVAL_OK;
        }
        else
            retVal = M_RETVAL_ERR;
    }

    return retVal;
}

AI_LIB_FUNC M_RETVAL_TYPE AI_CALL_CONV M_APICMDRAMWRITEDATASET(VI_SESSION M_BOARDID, AiUInt16 dsid, AiUInt16* data)
{
    AiUInt32         tempBModule;
    M_RETVAL_TYPE   retVal = M_RETVAL_INI;

    if( TRUE == M_GETMODULE( M_BOARDID, &tempBModule) )
    {
        if( API_OK == (retVal=ApiCmdRamWriteDataset(tempBModule, dsid, data)) )
        {
            retVal = M_RETVAL_OK;
        }
        else
            retVal = M_RETVAL_ERR;
    }

    return retVal;
}

AI_LIB_FUNC M_RETVAL_TYPE AI_CALL_CONV M_APICMDRAMREADDATASET(VI_SESSION M_BOARDID, AiUInt16 dsid, AiUInt16* data)
{
    AiUInt32         tempBModule;
    M_RETVAL_TYPE   retVal = M_RETVAL_INI;

    if( TRUE == M_GETMODULE( M_BOARDID, &tempBModule) )
    {
        if( API_OK == (retVal=ApiCmdRamReadDataset(tempBModule, dsid, data)) )
        {
            retVal = M_RETVAL_OK;
        }
        else
            retVal = M_RETVAL_ERR;
    }

    return retVal;
}

/* 13.10.99 */
AI_LIB_FUNC M_RETVAL_TYPE AI_CALL_CONV M_APICMDBUFC1760CON(VI_SESSION M_BOARDID, AiUInt8 biu,
                                                            AiUInt8  mode,
                                                            AiUInt16 buf_id,
                                                            AiUInt8  c01,
                                                            AiUInt8  c02[MAX_API_C1760_C02])
{
    AiUInt32         tempBModule;
    M_RETVAL_TYPE   retVal = M_RETVAL_INI;

    TY_API_C1760_CON pc1760;
    AiInt j;

    pc1760.mode = mode;
    pc1760.buf_id = buf_id;
    pc1760.c01 = c01;
    for (j=0; j<MAX_API_C1760_C02; j++) {
        pc1760.c02[j] = c02[j];
    }

    if( TRUE == M_GETMODULE( M_BOARDID, &tempBModule) )
    {
        if( API_OK == (retVal=ApiCmdBufC1760Con(tempBModule, biu, &pc1760)) )
        {
            retVal = M_RETVAL_OK;
        }
        else
            retVal = M_RETVAL_ERR;
    }

    return retVal;
}

// AJR Added 26-JUN-2000
AI_LIB_FUNC M_RETVAL_TYPE AI_CALL_CONV M_APIREADMEMDATA(VI_SESSION M_BOARDID, AiUInt8 memtype, AiUInt32 offset,
                                                         AiUInt8 width, AiAddr  data_p)
{
    AiUInt32         tempBModule;
    M_RETVAL_TYPE   retVal = M_RETVAL_INI;

    if( TRUE == M_GETMODULE( M_BOARDID, &tempBModule) )
    {
        if( API_OK == (retVal=ApiReadMemData(tempBModule, memtype, offset, width, data_p)) )
        {
            retVal = M_RETVAL_OK;
        }
        else
            retVal = M_RETVAL_ERR;
    }

    return retVal;
}

// AJR Added 26-JUN-2000
AI_LIB_FUNC M_RETVAL_TYPE AI_CALL_CONV M_APIWRITEMEMDATA(VI_SESSION M_BOARDID, AiUInt8 memtype, AiUInt32 offset,
                                                          AiUInt8 width, AiAddr  data_p)
{
    AiUInt32         tempBModule;
    M_RETVAL_TYPE   retVal = M_RETVAL_INI;

    if( TRUE == M_GETMODULE( M_BOARDID, &tempBModule) )
    {
        if( API_OK == (retVal=ApiWriteMemData(tempBModule, memtype, offset, width, data_p)) )
        {
            retVal = M_RETVAL_OK;
        }
        else
            retVal = M_RETVAL_ERR;
    }

    return retVal;
}

// AJR Added 26-JUN-2000
AI_LIB_FUNC M_RETVAL_TYPE AI_CALL_CONV M_APIREADBLOCKMEMDATA(VI_SESSION M_BOARDID, AiUInt8 memtype, AiUInt32 offset, AiUInt8 width,
                                                              AiAddr  data_p, AiUInt32 size, AiUInt32 *pul_BytesRead)
{
    AiUInt32         tempBModule;
    M_RETVAL_TYPE   retVal = M_RETVAL_INI;

    if( TRUE == M_GETMODULE( M_BOARDID, &tempBModule) )
    {
        if( API_OK == (retVal=ApiReadBlockMemData(tempBModule, memtype, offset, width, data_p, size, pul_BytesRead)) )
        {
            retVal = M_RETVAL_OK;
        }
        else
            retVal = M_RETVAL_ERR;
    }

    return retVal;
}

// AJR Added 26-JUN-2000
AI_LIB_FUNC M_RETVAL_TYPE AI_CALL_CONV M_APIWRITEBLOCKMEMDATA(VI_SESSION M_BOARDID, AiUInt8 memtype, AiUInt32 offset,
                                                               AiUInt8 width, AiAddr  data_p, AiUInt32 size, AiUInt32 *pul_BytesWritten)
{
    AiUInt32         tempBModule;
    M_RETVAL_TYPE   retVal = M_RETVAL_INI;

    if( TRUE == M_GETMODULE( M_BOARDID, &tempBModule) )
    {
        if( API_OK == (retVal=ApiWriteBlockMemData(tempBModule, memtype, offset, width, data_p, size, pul_BytesWritten)) )
        {
            retVal = M_RETVAL_OK;
        }
        else
            retVal = M_RETVAL_ERR;
    }

    return retVal;
}


// AJR Added 26-JUN-2000
AI_LIB_FUNC M_RETVAL_TYPE AI_CALL_CONV M_APICMDFIFOWRITE(VI_SESSION M_BOARDID, AiUInt8 biu, AiUInt8 f_id,
                                                          AiUInt16 size, AiUInt16* data)
{
    AiUInt32         tempBModule;
    M_RETVAL_TYPE   retVal = M_RETVAL_INI;

    if( TRUE == M_GETMODULE( M_BOARDID, &tempBModule) )
    {
        if( API_OK == (retVal=ApiCmdFifoWrite(tempBModule, biu, f_id, size, data)) )
        {
            retVal = M_RETVAL_OK;
        }
        else
            retVal = M_RETVAL_ERR;
    }

    return retVal;
}

// AJR Added 26-JUN-2000
AI_LIB_FUNC M_RETVAL_TYPE AI_CALL_CONV M_APICMDFIFOREADSTATUS(VI_SESSION M_BOARDID, AiUInt8 biu,
                                                               AiUInt8 f_id, AiUInt16* status)
{
    AiUInt32         tempBModule;
    M_RETVAL_TYPE   retVal = M_RETVAL_INI;

    if( TRUE == M_GETMODULE( M_BOARDID, &tempBModule) )
    {
        if( API_OK == (retVal=ApiCmdFifoReadStatus(tempBModule, biu, f_id, status)) )
        {
            retVal = M_RETVAL_OK;
        }
        else
            retVal = M_RETVAL_ERR;
    }

    return retVal;
}


AI_LIB_FUNC M_RETVAL_TYPE AI_CALL_CONV M_APIBHMODIFY(VI_SESSION M_BOARDID, AiUInt32 ul_BHAddr, AiUInt32 ul_DataBufferStoreMode, AiUInt32 ul_BufferSizeMode,
                                                      AiUInt32 ul_BufferSize, AiUInt32 ul_BufferQueueMode, AiUInt32 ul_BufferQueueSize,
                                                      AiUInt32 ul_StatusQueueSize, AiUInt32 ul_EventQueueSize, AiUInt32 ul_CurrentBufferIndex)
{
    AiUInt32         tempBModule;
    M_RETVAL_TYPE   retVal = M_RETVAL_INI;
    TY_API_BH_MODIFY x_BHInfo;

    if( TRUE == M_GETMODULE( M_BOARDID, &tempBModule) )
    {
        x_BHInfo.ul_BHAddr              = ul_BHAddr;
        x_BHInfo.ul_DataBufferStoreMode = ul_DataBufferStoreMode;
        x_BHInfo.ul_BufferSizeMode      = ul_BufferSizeMode;
        x_BHInfo.ul_BufferSize          = ul_BufferSize;
        x_BHInfo.ul_BufferQueueMode     = ul_BufferQueueMode;
        x_BHInfo.ul_BufferQueueSize     = ul_BufferQueueSize;
        x_BHInfo.ul_StatusQueueSize     = ul_StatusQueueSize;
        x_BHInfo.ul_EventQueueSize      = ul_EventQueueSize;
        x_BHInfo.ul_CurrentBufferIndex  = ul_CurrentBufferIndex;
        if( API_OK == (retVal=ApiBHModify(tempBModule, &x_BHInfo)) )
        {
            retVal = M_RETVAL_OK;
        }
        else
            retVal = M_RETVAL_ERR;
    }

    return retVal;
}


AI_LIB_FUNC M_RETVAL_TYPE AI_CALL_CONV M_APICMDSYNCCOUNTERSET(VI_SESSION M_BOARDID, AiUInt8 uc_Biu, 
                                                               AiUInt32 ul_SyncCntVal)
{
    AiUInt32         tempBModule;
    M_RETVAL_TYPE   retVal = M_RETVAL_INI;
    TY_API_SYNC_CNT_SET x_SyncCntSet;

    if( TRUE == M_GETMODULE( M_BOARDID, &tempBModule) )
    {
        x_SyncCntSet.ul_SyncCntVal      = ul_SyncCntVal;
        if( API_OK == (retVal=ApiCmdSyncCounterSet(tempBModule, uc_Biu, &x_SyncCntSet)) )
        {
            retVal = M_RETVAL_OK;
        }
        else
            retVal = M_RETVAL_ERR;
    }

    return retVal;
}

AI_LIB_FUNC M_RETVAL_TYPE AI_CALL_CONV M_APICMDSYNCCOUNTERGET(VI_SESSION M_BOARDID, AiUInt8 uc_Biu, 
                                                               AiUInt32 *pul_SyncCntVal, AiUInt32 *pul_SyncCntInit, AiUInt32 *pul_SyncCntInitLow, AiUInt32 *pul_SyncCntInitHigh)
{
    AiUInt32         tempBModule;
    M_RETVAL_TYPE   retVal = M_RETVAL_INI;
    TY_API_SYNC_CNT_GET x_SyncCntGet;

    if( TRUE == M_GETMODULE( M_BOARDID, &tempBModule) )
    {
        if( API_OK == (retVal=ApiCmdSyncCounterGet(tempBModule, uc_Biu, &x_SyncCntGet)) )
        {
            *pul_SyncCntVal       = x_SyncCntGet.ul_SyncCntVal;
            *pul_SyncCntInit      = x_SyncCntGet.ul_SyncCntInit;
            *pul_SyncCntInitHigh  = x_SyncCntGet.ul_SyncCntInitHigh;
            *pul_SyncCntInitLow   = x_SyncCntGet.ul_SyncCntInitLow;

            retVal = M_RETVAL_OK;
        }
        else
            retVal = M_RETVAL_ERR;
    }

    return retVal;
}

AI_LIB_FUNC M_RETVAL_TYPE AI_CALL_CONV M_APICMDSYSPXICON(VI_SESSION M_BOARDID, 
                                                          AiUInt32 ul_Mode, AiUInt32 ul_TrgSource, AiUInt32 ul_TrgDest, AiUInt32 ul_TTClear)
{
    AiUInt32         tempBModule;
    M_RETVAL_TYPE   retVal = M_RETVAL_INI;
    TY_API_PXI_CON x_PXICon;

    if( TRUE == M_GETMODULE( M_BOARDID, &tempBModule) )
    {
        x_PXICon.ul_Mode      = ul_Mode;
        x_PXICon.ul_TrgSource = ul_TrgSource;
        x_PXICon.ul_TrgDest   = ul_TrgDest;
        x_PXICon.ul_TTClear   = ul_TTClear;
        if( API_OK == (retVal=ApiCmdSysPXICon(tempBModule, &x_PXICon)) )
        {
            retVal = M_RETVAL_OK;
        }
        else
            retVal = M_RETVAL_ERR;
    }

    return retVal;
}

AI_LIB_FUNC M_RETVAL_TYPE AI_CALL_CONV M_APICMDSYSPXIGEOGRAPHICALADDRESSGET(VI_SESSION M_BOARDID, AiUInt32 *pxiGeographicalAddress)
{
    AiUInt32         tempBModule;
    M_RETVAL_TYPE   retVal = M_RETVAL_INI;

    if (TRUE == M_GETMODULE(M_BOARDID, &tempBModule))
    {
        if (API_OK == (retVal = ApiCmdSysPXIGeographicalAddressGet(tempBModule, pxiGeographicalAddress)))
        {
            retVal = M_RETVAL_OK;
        }
        else
            retVal = M_RETVAL_ERR;
    }
    return retVal;
}


AI_LIB_FUNC M_RETVAL_TYPE AI_CALL_CONV M_APICMDSYSGETMEMLOC(VI_SESSION M_BOARDID, 
                                                             AiUInt8 uc_Biu, AiUInt32 ul_Mode, AiUInt32 ul_Id, AiUInt32 *pul_MemType, AiUInt32 *pul_Offset)
{
    AiUInt32         tempBModule;
    M_RETVAL_TYPE   retVal = M_RETVAL_INI;

    if( TRUE == M_GETMODULE( M_BOARDID, &tempBModule) )
    {
        if( API_OK == (retVal=ApiCmdSysGetMemLocation(tempBModule, uc_Biu, ul_Mode, ul_Id, pul_MemType, pul_Offset)) )
        {
            retVal = M_RETVAL_OK;
        }
        else
            retVal = M_RETVAL_ERR;
    }

    return retVal;
}



//***************************************************************************
//    API  Bus Controller Commands
//***************************************************************************
AI_LIB_FUNC M_RETVAL_TYPE AI_CALL_CONV M_APICMDBCINI(VI_SESSION M_BOARDID, AiUInt8 biu, AiUInt8 retr, AiUInt8 svrq, AiUInt8 tbm, AiUInt8 gsb)
{
    AiUInt32         tempBModule;
    M_RETVAL_TYPE   retVal = M_RETVAL_INI;

    if( TRUE == M_GETMODULE( M_BOARDID, &tempBModule) )
    {
        if( API_OK == (retVal=ApiCmdBCIni(tempBModule, biu, retr, svrq, tbm, gsb)) )
        {
            retVal = M_RETVAL_OK;
        }
        else
            retVal = M_RETVAL_ERR;
    }

    return retVal;
}

AI_LIB_FUNC M_RETVAL_TYPE AI_CALL_CONV M_APICMDBCBHDEF(VI_SESSION M_BOARDID, AiUInt8 biu, AiUInt16 hid,
                                                        AiUInt16 bid, AiUInt16 sid, AiUInt16 eid, AiUInt8 qsize, AiUInt8 bqm, AiUInt8 bsm,
                                                        AiUInt8 sqm, AiUInt8 eqm, AiUInt8 res,
                                                        AiUInt16* bh_bid,
                                                        AiUInt16* bh_sid,
                                                        AiUInt16* bh_eid,
                                                        AiUInt16* bh_nbufs,
                                                        AiUInt32* bh_hid_addr,
                                                        AiUInt32* bh_bq_addr,
                                                        AiUInt32* bh_sq_addr,
                                                        AiUInt32* bh_eq_addr)
{
    AiUInt32         tempBModule;
    M_RETVAL_TYPE   retVal = M_RETVAL_INI;

    TY_API_BC_BH_INFO pbh;

    if( TRUE == M_GETMODULE( M_BOARDID, &tempBModule) )
    {
        if( API_OK == (retVal=ApiCmdBCBHDef(tempBModule, biu, hid, bid, sid, eid, qsize, bqm, bsm,
            sqm, eqm, res, &pbh)) )
        {
            *bh_bid = pbh.bid;
            *bh_sid = pbh.sid;
            *bh_eid = pbh.eid;
            *bh_nbufs = pbh.nbufs;
            *bh_hid_addr = pbh.hid_addr;
            *bh_bq_addr = pbh.bq_addr;
            *bh_sq_addr = pbh.sq_addr;
            *bh_eq_addr = pbh.eq_addr;

            retVal = M_RETVAL_OK;
        }
        else
            retVal = M_RETVAL_ERR;
    }

    return retVal;
}

AI_LIB_FUNC M_RETVAL_TYPE AI_CALL_CONV M_APICMDBCBHREAD(VI_SESSION M_BOARDID, AiUInt8 biu, AiUInt16 hid,
                                                         AiUInt16* bh_bid,
                                                         AiUInt16* bh_sid,
                                                         AiUInt16* bh_eid,
                                                         AiUInt16* bh_nbufs,
                                                         AiUInt32* bh_hid_addr,
                                                         AiUInt32* bh_bq_addr,
                                                         AiUInt32* bh_sq_addr,
                                                         AiUInt32* bh_eq_addr)
{
    AiUInt32         tempBModule;
    M_RETVAL_TYPE   retVal = M_RETVAL_INI;

    TY_API_BC_BH_INFO pbh;

    if( TRUE == M_GETMODULE( M_BOARDID, &tempBModule) )
    {
        if( API_OK == (retVal=ApiCmdBCBHRead(tempBModule, biu, hid, &pbh)) )
        {
            *bh_bid = pbh.bid;
            *bh_sid = pbh.sid;
            *bh_eid = pbh.eid;
            *bh_nbufs = pbh.nbufs;
            *bh_hid_addr = pbh.hid_addr;
            *bh_bq_addr = pbh.bq_addr;
            *bh_sq_addr = pbh.sq_addr;
            *bh_eq_addr = pbh.eq_addr;

            retVal = M_RETVAL_OK;
        }
        else
            retVal = M_RETVAL_ERR;
    }

    return retVal;
}


AI_LIB_FUNC M_RETVAL_TYPE AI_CALL_CONV M_APICMDBCXFERDEF(VI_SESSION M_BOARDID, AiUInt8 biu,
                                                          AiUInt16 xid,
                                                          AiUInt16 hid,
                                                          AiUInt8 type,
                                                          AiUInt8 chn,
                                                          AiUInt8 xmt_rt,
                                                          AiUInt8 rcv_rt,
                                                          AiUInt8 xmt_sa,
                                                          AiUInt8 rcv_sa,
                                                          AiUInt8 wcnt,
                                                          AiUInt8 tic,
                                                          AiUInt8 hlt,
                                                          AiUInt8 rte,
                                                          AiUInt8 res,
                                                          AiUInt8 sxh,
                                                          AiUInt8 rsp,
                                                          AiUInt8 gap_mode,
                                                          AiUInt16 swxm,
                                                          AiUInt8 err_type,
                                                          AiUInt8 err_sync,
                                                          AiUInt8 err_contig,
                                                          AiUInt8 err_padding1,
                                                          AiUInt32 err_spec,
                                                          AiUInt16 gap,
                                                          AiUInt32* desc_addr)
{
    AiUInt32         tempBModule;
    M_RETVAL_TYPE   retVal = M_RETVAL_INI;

    TY_API_BC_XFER pxfer;
    struct ty_api_bc_err err;
    pxfer.xid = xid;
    pxfer.hid = hid;
    pxfer.type = type;
    pxfer.chn = chn;
    pxfer.xmt_rt = xmt_rt;
    pxfer.rcv_rt = rcv_rt;
    pxfer.xmt_sa = xmt_sa;
    pxfer.rcv_sa = rcv_sa;
    pxfer.wcnt = wcnt;
    pxfer.tic = tic;
    pxfer.hlt = hlt;
    pxfer.rte = rte;
    pxfer.res = res; // changed pg
    pxfer.sxh = sxh;
    pxfer.rsp = rsp; // changed pg
    pxfer.gap_mode = gap_mode;
    pxfer.swxm = swxm;
    err.type = err_type;
    err.sync = err_sync;
    err.contig = err_contig;
    err.padding1 = err_padding1;
    err.err_spec = err_spec;
    pxfer.gap = gap;
    pxfer.err = err;

    if( TRUE == M_GETMODULE( M_BOARDID, &tempBModule) )
    {
        if( API_OK == (retVal=ApiCmdBCXferDef(tempBModule, biu, &pxfer, desc_addr)) )
        {
            retVal = M_RETVAL_OK;
        }
        else
            retVal = M_RETVAL_ERR;
    }

    return retVal;
}


AI_LIB_FUNC M_RETVAL_TYPE AI_CALL_CONV M_APICMDBCXFERDEFERR(VI_SESSION M_BOARDID, AiUInt8 biu,
                                                             AiUInt16 xid,
                                                             AiUInt8 err_type,
                                                             AiUInt8 err_sync,
                                                             AiUInt8 err_contig,
                                                             AiUInt8 err_padding1,
                                                             AiUInt32 err_spec )
{
    AiUInt32        tempBModule;
    M_RETVAL_TYPE   retVal = M_RETVAL_INI;

    TY_API_BC_ERR xError;

    xError.type     = err_type;
    xError.sync     = err_sync;
    xError.contig   = err_contig;
    xError.padding1 = err_padding1;
    xError.err_spec = err_spec;

    if( TRUE == M_GETMODULE( M_BOARDID, &tempBModule) )
    {
        if( API_OK == (retVal=ApiCmdBCXferDefErr(tempBModule, biu, xid, &xError)) )
        {
            retVal = M_RETVAL_OK;
        }
        else
            retVal = M_RETVAL_ERR;
    }

    return retVal;
}

AI_LIB_FUNC M_RETVAL_TYPE AI_CALL_CONV M_APICMDBCGETXFERDEF(VI_SESSION M_BOARDID, AiUInt8 biu,
                                                             AiUInt16 uw_XferId,
                                                             AiUInt16 *xid,
                                                             AiUInt16 *hid,
                                                             AiUInt8 *type,
                                                             AiUInt8 *chn,
                                                             AiUInt8 *xmt_rt,
                                                             AiUInt8 *rcv_rt,
                                                             AiUInt8 *xmt_sa,
                                                             AiUInt8 *rcv_sa,
                                                             AiUInt8 *wcnt,
                                                             AiUInt8 *tic,
                                                             AiUInt8 *hlt,
                                                             AiUInt8 *rte,
                                                             AiUInt8 *res,
                                                             AiUInt8 *sxh,
                                                             AiUInt8 *rsp,
                                                             AiUInt8 *gap_mode,
                                                             AiUInt16 *swxm,
                                                             AiUInt8 *err_type,
                                                             AiUInt8 *err_sync,
                                                             AiUInt8 *err_contig,
                                                             AiUInt8 *err_padding1,
                                                             AiUInt32 *err_spec,
                                                             AiUInt16 *gap)
{
    AiUInt32         tempBModule;
    M_RETVAL_TYPE   retVal = M_RETVAL_INI;

    TY_API_BC_XFER pxfer;

    if( TRUE == M_GETMODULE( M_BOARDID, &tempBModule) )
    {
        if( API_OK == (retVal=ApiCmdBCGetXferDef(tempBModule, biu, uw_XferId, &pxfer)) )
        {
            *xid = pxfer.xid;
            *hid = pxfer.hid;
            *type = pxfer.type;
            *chn = pxfer.chn;
            *xmt_rt = pxfer.xmt_rt;
            *rcv_rt = pxfer.rcv_rt;
            *xmt_sa = pxfer.xmt_sa;
            *rcv_sa = pxfer.rcv_sa;
            *wcnt = pxfer.wcnt;
            *tic = pxfer.tic;
            *hlt = pxfer.hlt;
            *rte = pxfer.rte;
            *res = pxfer.res;
            *sxh = pxfer.sxh;
            *rsp = pxfer.rsp;
            *gap_mode = pxfer.gap_mode;
            *swxm = pxfer.swxm;
            *err_type = pxfer.err.type;
            *err_sync = pxfer.err.sync;
            *err_contig = pxfer.err.contig;
            *err_padding1 = pxfer.err.padding1;
            *err_spec = pxfer.err.err_spec;
            *gap = pxfer.gap;
            retVal = M_RETVAL_OK;
        }
        else
            retVal = M_RETVAL_ERR;
    }

    return retVal;
}

AI_LIB_FUNC M_RETVAL_TYPE AI_CALL_CONV M_APICMDBCXFERCTRL(VI_SESSION M_BOARDID, AiUInt8 biu, AiUInt16 xid, AiUInt8 mode)
{
    AiUInt32         tempBModule;
    M_RETVAL_TYPE   retVal = M_RETVAL_INI;

    if( TRUE == M_GETMODULE( M_BOARDID, &tempBModule) )
    {
        if( API_OK == (retVal=ApiCmdBCXferCtrl(tempBModule, biu, xid, mode)) )
        {
            retVal = M_RETVAL_OK;
        }
        else
            retVal = M_RETVAL_ERR;
    }

    return retVal;
}


AI_LIB_FUNC M_RETVAL_TYPE AI_CALL_CONV M_APICMDBCXFERDESCGET(VI_SESSION M_BOARDID, AiUInt8 biu, AiUInt16 xid, AiUInt32 ulMode, AiUInt32 * pulValue1, AiUInt32 * pulValue2, AiUInt32 * pulValue3, AiUInt32 * pulValue4 )
{
    AiUInt32        tempBModule;
    AiUInt32        aulValue[4];
    M_RETVAL_TYPE   retVal = M_RETVAL_INI;

    if( TRUE == M_GETMODULE( M_BOARDID, &tempBModule) )
    {
        if( API_OK == (retVal=ApiCmdBCXferDescGet(tempBModule, biu, xid, ulMode, aulValue)) )
        {
            retVal     = M_RETVAL_OK;
            *pulValue1 = aulValue[0];
            *pulValue2 = aulValue[1];
            *pulValue3 = aulValue[2];
            *pulValue4 = aulValue[3];
        }
        else
            retVal = M_RETVAL_ERR;
    }

    return retVal;
}


AI_LIB_FUNC M_RETVAL_TYPE AI_CALL_CONV M_APICMDBCXFERDESCMOD(VI_SESSION M_BOARDID, AiUInt8 biu, AiUInt16 xid, AiUInt32 ulField, AiUInt32 ulValue1, AiUInt32 ulValue2, AiUInt32 ulValue3, AiUInt32 ulValue4 )
{
    AiUInt32         tempBModule;
    AiUInt32        aulValue[4];
    M_RETVAL_TYPE   retVal = M_RETVAL_INI;

    if( TRUE == M_GETMODULE( M_BOARDID, &tempBModule) )
    {
        aulValue[0] = ulValue1;
        aulValue[1] = ulValue2;
        aulValue[2] = ulValue3;
        aulValue[3] = ulValue4;

        if( API_OK == (retVal=ApiCmdBCXferDescMod(tempBModule, biu, xid, ulField, aulValue)) )
        {
            retVal = M_RETVAL_OK;
        }
        else
            retVal = M_RETVAL_ERR;
    }

    return retVal;
}

AI_LIB_FUNC M_RETVAL_TYPE AI_CALL_CONV M_APICMDBCDYTAGDEF(VI_SESSION M_BOARDID, AiUInt8 biu, AiUInt8 con,
                                                           AiUInt16 bc_hid,
                                                           AiUInt16 mode,
                                                           AiUInt16 tag_fct[4],
                                                           AiUInt16 min[4],
                                                           AiUInt16 max[4],
                                                           AiUInt16 step[4],
                                                           AiUInt16 wpos[4]
)
{
    AiUInt32         tempBModule;
    M_RETVAL_TYPE   retVal = M_RETVAL_INI;

    AiInt i;
    TY_API_BC_DYTAG pdytag[4];
    for(i=0;i<4;i++)
    {
        pdytag[i].tag_fct = tag_fct[i];
        pdytag[i].min = min[i];
        pdytag[i].max = max[i];
        pdytag[i].step = step[i];
        pdytag[i].wpos = wpos[i];
    }

    if( TRUE == M_GETMODULE( M_BOARDID, &tempBModule) )
    {
        if( API_OK == (retVal=ApiCmdBCDytagDef(tempBModule, biu, con, bc_hid,  mode, pdytag)) )
        {

            retVal = M_RETVAL_OK;
        }
        else
            retVal = M_RETVAL_ERR;
    }

    return retVal;
}

AI_LIB_FUNC M_RETVAL_TYPE AI_CALL_CONV M_APICMDBCGETDYTAGDEF(VI_SESSION M_BOARDID, AiUInt8 biu, 
                                                              AiUInt16 bc_hid,
                                                              AiUInt16 *mode,
                                                              AiUInt16 tag_fct[4],
                                                              AiUInt16 min[4],
                                                              AiUInt16 max[4],
                                                              AiUInt16 step[4],
                                                              AiUInt16 wpos[4]
)
{
    AiUInt32         tempBModule;
    M_RETVAL_TYPE   retVal = M_RETVAL_INI;

    AiInt i;
    TY_API_BC_DYTAG pdytag[4];

    if( TRUE == M_GETMODULE( M_BOARDID, &tempBModule) )
    {
        if( API_OK == (retVal=ApiCmdBCGetDytagDef(tempBModule, biu, bc_hid, mode, pdytag)) )
        {
            for(i=0;i<4;i++)
            {
                tag_fct[i] = pdytag[i].tag_fct;
                min[i] = pdytag[i].min;
                max[i] = pdytag[i].max;
                step[i] = pdytag[i].step;
                wpos[i] = pdytag[i].wpos;
            }

            retVal = M_RETVAL_OK;
        }
        else
            retVal = M_RETVAL_ERR;
    }

    return retVal;
}

AI_LIB_FUNC M_RETVAL_TYPE AI_CALL_CONV M_APICMDBCFRAMEDEF(VI_SESSION M_BOARDID, AiUInt8 biu,
                                                           AiUInt8 id,
                                                           AiUInt8 cnt,
                                                           AiUInt16 instr[MAX_API_BC_XFRAME],
                                                           AiUInt16 xid[MAX_API_BC_XFRAME])
{
    AiUInt32         tempBModule;
    M_RETVAL_TYPE   retVal = M_RETVAL_INI;

    AiInt j;
    TY_API_BC_FRAME pframe;

    pframe.id = id;
    pframe.cnt = cnt;
    for(j=0;j<MAX_API_BC_XFRAME;j++) {
        pframe.instr[j] = instr[j];
        pframe.xid[j] = xid[j];
    }

    if( TRUE == M_GETMODULE( M_BOARDID, &tempBModule) )
    {
        if( API_OK == (retVal=ApiCmdBCFrameDef(tempBModule, biu, &pframe)) )
        {
            retVal = M_RETVAL_OK;
        }
        else
            retVal = M_RETVAL_ERR;
    }

    return retVal;
}

AI_LIB_FUNC M_RETVAL_TYPE AI_CALL_CONV M_APICMDBCGETFRAMEDEF(VI_SESSION M_BOARDID, AiUInt8 biu,
                                                              AiUInt8 id,
                                                              AiUInt8 *cnt,
                                                              AiUInt16 *instr,
                                                              AiUInt16 *xid)
{
    AiUInt32         tempBModule;
    M_RETVAL_TYPE   retVal = M_RETVAL_INI;

    AiInt j;
    TY_API_BC_FRAME pframe;

    if( TRUE == M_GETMODULE( M_BOARDID, &tempBModule) )
    {
        if( API_OK == (retVal=ApiCmdBCGetMinorFrameDefinition(tempBModule, biu, id, &pframe)) )
        {
            *cnt = pframe.cnt;
            for(j=0;j<*cnt;j++) {
                instr[j] = pframe.instr[j];
                xid[j] = pframe.xid[j];
            }

            retVal = M_RETVAL_OK;
        }
        else
            retVal = M_RETVAL_ERR;
    }

    return retVal;
}

AI_LIB_FUNC M_RETVAL_TYPE AI_CALL_CONV M_APICMDBCMFRAMEDEF(VI_SESSION M_BOARDID, AiUInt8 biu,
                                                            AiUInt8 cnt,
                                                            AiUInt8 fid[MAX_API_BC_MFRAME])
{
    AiUInt32         tempBModule;
    M_RETVAL_TYPE   retVal = M_RETVAL_INI;

    AiInt j;
    TY_API_BC_MFRAME pmframe;
    pmframe.cnt = cnt;
    for(j=0;((j<cnt) && (j<MAX_API_BC_MFRAME));j++)
    {
        pmframe.fid[j] = fid[j];
    }

    if( TRUE == M_GETMODULE( M_BOARDID, &tempBModule) )
    {
        if( API_OK == (retVal=ApiCmdBCMFrameDef(tempBModule, biu, &pmframe)) )
        {
            retVal = M_RETVAL_OK;
        }
        else
            retVal = M_RETVAL_ERR;
    }

    return retVal;
}

AI_LIB_FUNC M_RETVAL_TYPE AI_CALL_CONV M_APICMDBCGETMAJFRAMEDEF(VI_SESSION M_BOARDID, AiUInt8 biu,
                                                                 AiUInt8 *cnt, AiUInt8 *fid)
{
    AiUInt32         tempBModule;
    M_RETVAL_TYPE   retVal = M_RETVAL_INI;

    AiInt j;
    TY_API_BC_MFRAME pmframe;

    if( TRUE == M_GETMODULE( M_BOARDID, &tempBModule) )
    {
        if( API_OK == (retVal=ApiCmdBCGetMajorFrameDefinition(tempBModule, biu, &pmframe)) )
        {
            *cnt = pmframe.cnt;
            for(j=0;j<*cnt;j++)
            {
                fid[j] = pmframe.fid[j];
            }

            retVal = M_RETVAL_OK;
        }
        else
            retVal = M_RETVAL_ERR;
    }

    return retVal;
}


AI_LIB_FUNC M_RETVAL_TYPE AI_CALL_CONV M_APICMDBCMFRAMEDEFEX(VI_SESSION M_BOARDID, AiUInt8 biu,
                                                              AiUInt8 cnt,
                                                              AiUInt8 fid[MAX_API_BC_MFRAME])
{
    AiUInt32         tempBModule;
    M_RETVAL_TYPE   retVal = M_RETVAL_INI;

    AiUInt32 j;
    TY_API_BC_MFRAME_EX pmframe;
    pmframe.cnt = cnt;
    for(j=0; ((j<cnt) && (j<MAX_API_BC_MFRAME)) ;j++)
    {
        pmframe.fid[j] = fid[j];
    }

    if( TRUE == M_GETMODULE( M_BOARDID, &tempBModule) )
    {
        if( API_OK == (retVal=ApiCmdBCMFrameDefEx(tempBModule, biu, &pmframe)) )
        {
            retVal = M_RETVAL_OK;
        }
        else
            retVal = M_RETVAL_ERR;
    }

    return retVal;
}


AI_LIB_FUNC M_RETVAL_TYPE AI_CALL_CONV M_APICMDBCSTART(VI_SESSION M_BOARDID, AiUInt8 biu, AiUInt8 smod, AiUInt32 cnt, AiFloat frame_time,
                                                        AiUInt32 saddr, AiUInt32 *maj_frame_addr, AiUInt32 *min_frame_addr)
{
    AiUInt32         tempBModule;
    M_RETVAL_TYPE   retVal = M_RETVAL_INI;

    if( TRUE == M_GETMODULE( M_BOARDID, &tempBModule) )
    {
        if( API_OK == (retVal=ApiCmdBCStart(tempBModule, biu, smod, cnt, frame_time, saddr, maj_frame_addr, min_frame_addr)) )
        {
            retVal = M_RETVAL_OK;
        }
        else
            retVal = M_RETVAL_ERR;
    }

    return retVal;
}

AI_LIB_FUNC M_RETVAL_TYPE AI_CALL_CONV M_APICMDBCHALT(VI_SESSION M_BOARDID, AiUInt8 biu)
{
    AiUInt32         tempBModule;
    M_RETVAL_TYPE   retVal = M_RETVAL_INI;

    if( TRUE == M_GETMODULE( M_BOARDID, &tempBModule) )
    {
        if( API_OK == (retVal=ApiCmdBCHalt(tempBModule, biu)) )
        {
            retVal = M_RETVAL_OK;
        }
        else
            retVal = M_RETVAL_ERR;
    }

    return retVal;
}

AI_LIB_FUNC M_RETVAL_TYPE AI_CALL_CONV M_APICMDBCSTATUSREAD(VI_SESSION M_BOARDID, AiUInt8 biu,
                                                             AiUInt8*  status,
                                                             AiUInt16* hxfer,
                                                             AiUInt32* glb_msg_cnt,
                                                             AiUInt32* glb_err_cnt,
                                                             AiUInt32* hip,
                                                             AiUInt32* mfc)
{
    AiUInt32         tempBModule;
    M_RETVAL_TYPE   retVal = M_RETVAL_INI;

    TY_API_BC_STATUS_DSP pdsp;

    if( TRUE == M_GETMODULE( M_BOARDID, &tempBModule) )
    {
        if( API_OK == (retVal=ApiCmdBCStatusRead(tempBModule, biu, &pdsp)) )
        {
            *status = pdsp.status;
            *hxfer = pdsp.hxfer;
            *glb_msg_cnt = pdsp.glb_msg_cnt;
            *glb_err_cnt = pdsp.glb_err_cnt;
            *hip = pdsp.hip;
            *mfc = pdsp.mfc;

            retVal = M_RETVAL_OK;
        }
        else
            retVal = M_RETVAL_ERR;
    }

    return retVal;
}

AI_LIB_FUNC M_RETVAL_TYPE AI_CALL_CONV M_APICMDBCXFERREAD(VI_SESSION M_BOARDID, AiUInt8 biu, AiUInt16 xid, AiUInt16 clr,
                                                           AiUInt16* cw1,
                                                           AiUInt16* st1,
                                                           AiUInt16* cw2,
                                                           AiUInt16* st2,
                                                           AiUInt16* bid,
                                                           AiUInt16* brw,
                                                           AiUInt32* bufp,
                                                           AiUInt32* ttag,
                                                           AiUInt32* msg_cnt,
                                                           AiUInt32* err_cnt,
                                                           AiUInt32* lvw,
                                                           AiUInt32* srvreq_cnt)
{
    AiUInt32         tempBModule;
    M_RETVAL_TYPE   retVal = M_RETVAL_INI;

    TY_API_BC_XFER_DSP pxfer_dsp;

    if( TRUE == M_GETMODULE( M_BOARDID, &tempBModule) )
    {
        if( API_OK == (retVal=ApiCmdBCXferRead(tempBModule, biu, xid, clr, &pxfer_dsp)) )
        {
            *cw1        = pxfer_dsp.cw1;
            *st1        = pxfer_dsp.st1;
            *cw2        = pxfer_dsp.cw2;
            *st2        = pxfer_dsp.st2;
            *bid        = pxfer_dsp.bid;
            *brw        = pxfer_dsp.brw;
            *bufp       = pxfer_dsp.bufp;
            *ttag       = pxfer_dsp.ttag;
            *msg_cnt    = pxfer_dsp.msg_cnt;
            *err_cnt    = pxfer_dsp.err_cnt;
            *lvw        = pxfer_dsp.lvw;
            *srvreq_cnt = pxfer_dsp.srvreq_cnt;

            retVal = M_RETVAL_OK;
        }
        else
            retVal = M_RETVAL_ERR;
    }

    return retVal;
}

AI_LIB_FUNC M_RETVAL_TYPE AI_CALL_CONV M_APICMDBCXFERREADEX(VI_SESSION M_BOARDID, 
                                                             AiUInt32 ul_XferId, 
                                                             AiUInt32 ul_Clear,
                                                             AiUInt32 ul_Biu,
                                                             AiUInt32 aul_SqCtrlWord[256],
                                                             AiUInt32 aul_SqStatusWords[256],
                                                             AiUInt32 aul_SqActBufPtr[256],
                                                             AiUInt32 aul_SqTimeTag[256],
                                                             AiUInt32 aul_EqEntryWord1[1],
                                                             AiUInt32 aul_EqEntryWord2[1],
                                                             AiUInt32 aul_EqEntryWord3[1],
                                                             AiUInt32 aul_EqEntryWord4[1],
                                                             AiUInt32 *pul_ActBufferId,
                                                             AiUInt32 *pul_ActionWord,
                                                             AiUInt32 *pul_XferCnt,
                                                             AiUInt32 *pul_ErrCnt,
                                                             AiUInt32 *pul_LastVectorWord,
                                                             AiUInt32 *pul_ServiceRequestCnt,
                                                             AiUInt32 *pul_BufferQueueSize)
{
    AiUInt32         tempBModule;
    AiUInt32 i;
    M_RETVAL_TYPE   retVal = M_RETVAL_INI;

    TY_API_BC_XFER_READ_IN x_XferRead;
    TY_API_BC_XFER_STATUS_EX x_XferStat;

    if( TRUE == M_GETMODULE( M_BOARDID, &tempBModule) )
    {
        x_XferRead.ul_XferId = ul_XferId;
        x_XferRead.ul_Clear = ul_Clear;
        x_XferRead.ul_Biu = ul_Biu;

        if( API_OK == (retVal=ApiCmdBCXferReadEx(tempBModule, &x_XferRead, &x_XferStat)) )
        {
            *pul_BufferQueueSize    = x_XferStat.ul_BufferQueueSize;
            *pul_ActBufferId        = x_XferStat.x_XferInfo.ul_ActBufferId;
            *pul_ActionWord         = x_XferStat.x_XferInfo.ul_ActionWord;
            *pul_XferCnt            = x_XferStat.x_XferInfo.ul_XferCnt;
            *pul_ErrCnt             = x_XferStat.x_XferInfo.ul_ErrCnt;
            *pul_LastVectorWord     = x_XferStat.x_XferInfo.ul_LastVectorWord;
            *pul_ServiceRequestCnt  = x_XferStat.x_XferInfo.ul_ServiceRequestCnt;

            aul_EqEntryWord1[0]    = x_XferStat.x_XferEQueue[0].ul_EqEntryWord1;
            aul_EqEntryWord2[0]    = x_XferStat.x_XferEQueue[0].ul_EqEntryWord2;
            aul_EqEntryWord3[0]    = x_XferStat.x_XferEQueue[0].ul_EqEntryWord3;
            aul_EqEntryWord4[0]    = x_XferStat.x_XferEQueue[0].ul_EqEntryWord4;

            for( i=0; i<x_XferStat.ul_BufferQueueSize; i++ )
            {
                aul_SqCtrlWord[i]    = x_XferStat.x_XferSQueue[i].ul_SqCtrlWord;
                aul_SqStatusWords[i] = x_XferStat.x_XferSQueue[i].ul_SqStatusWords;
                aul_SqActBufPtr[i]   = x_XferStat.x_XferSQueue[i].ul_SqActBufPtr;
                aul_SqTimeTag[i]     = x_XferStat.x_XferSQueue[i].ul_SqTimeTag;
            }

            retVal = M_RETVAL_OK;
        }
        else
            retVal = M_RETVAL_ERR;
    }

    return retVal;
}

AI_LIB_FUNC M_RETVAL_TYPE AI_CALL_CONV M_APICMDBCACYCPREP(VI_SESSION M_BOARDID, AiUInt8 biu,
                                                           AiUInt8 cnt,
                                                           AiUInt16 instr[MAX_API_BC_XACYC],
                                                           AiUInt16 xid[MAX_API_BC_XACYC])
{
    AiUInt32         tempBModule;
    M_RETVAL_TYPE   retVal = M_RETVAL_INI;

    AiInt j;
    TY_API_BC_ACYC pacyc;
    pacyc.cnt = cnt;
    for(j=0;j<MAX_API_BC_XACYC;j++)
    {
        pacyc.instr[j] = instr[j];
        pacyc.xid[j] = xid[j];
    }

    if( TRUE == M_GETMODULE( M_BOARDID, &tempBModule) )
    {
        if( API_OK == (retVal=ApiCmdBCAcycPrep(tempBModule, biu, &pacyc)) )
        {
            retVal = M_RETVAL_OK;
        }
        else
            retVal = M_RETVAL_ERR;
    }

    return retVal;
}

AI_LIB_FUNC M_RETVAL_TYPE AI_CALL_CONV M_APICMDBCACYCSEND(VI_SESSION M_BOARDID, AiUInt8 biu, AiUInt8 mode, AiUInt32 timetag_high, AiUInt32 timetag_low )
{
    AiUInt32         tempBModule;
    M_RETVAL_TYPE   retVal = M_RETVAL_INI;

    if( TRUE == M_GETMODULE( M_BOARDID, &tempBModule) )
    {
        if( API_OK == (retVal=ApiCmdBCAcycSend(tempBModule, biu, mode, timetag_high, timetag_low )) )
        {
            retVal = M_RETVAL_OK;
        }
        else
            retVal = M_RETVAL_ERR;
    }

    return retVal;
}

AI_LIB_FUNC M_RETVAL_TYPE AI_CALL_CONV M_APICMDBCASSIGNFIFO(VI_SESSION M_BOARDID, AiUInt8 biu, AiUInt8 con,
                                                             AiUInt8 f_id, AiUInt16 xid)
{
    AiUInt32         tempBModule;
    M_RETVAL_TYPE   retVal = M_RETVAL_INI;

    if( TRUE == M_GETMODULE( M_BOARDID, &tempBModule) )
    {
        if( API_OK == (retVal=ApiCmdBCAssignFifo(tempBModule, biu, con, f_id, xid)) )
        {
            retVal = M_RETVAL_OK;
        }
        else
            retVal = M_RETVAL_ERR;
    }

    return retVal;
}

AI_LIB_FUNC M_RETVAL_TYPE AI_CALL_CONV M_APICMDBCSRVREQVECSTATUS(VI_SESSION M_BOARDID, AiUInt8 uc_Biu, AiUInt8 uc_RtAddr,
                                                                  AiUInt16* uw_XferId, AiUInt16* uw_LastVecWord,
                                                                  AiUInt32* ul_SrvReqCnt)
{
    AiUInt32         tempBModule;
    M_RETVAL_TYPE   retVal = M_RETVAL_INI;

    TY_API_BC_SRVW x_SrvReqVecStatus;

    if( TRUE == M_GETMODULE( M_BOARDID, &tempBModule) )
    {
        if( API_OK == (retVal=ApiCmdBCSrvReqVecStatus( tempBModule, uc_Biu, uc_RtAddr, &x_SrvReqVecStatus )) )
        {
            *uw_XferId = x_SrvReqVecStatus.uw_XferId;
            *uw_LastVecWord = x_SrvReqVecStatus.uw_LastVecWord;
            *ul_SrvReqCnt = x_SrvReqVecStatus.ul_SrvReqCnt;

            retVal = M_RETVAL_OK;
        }
        else
            retVal = M_RETVAL_ERR;
    }

    return retVal;
} // end: ApiCmdBCSrvReqVecStatus

AI_LIB_FUNC M_RETVAL_TYPE AI_CALL_CONV M_APICMDBCSRVREQVECCON(VI_SESSION M_BOARDID, AiUInt8 uc_Biu, AiUInt8 uc_RtAddr,
                                                               AiUInt8 uc_SubAddress, AiUInt32 ul_Reserved1,
                                                               AiUInt32 ul_Reserved2, AiUInt32 ul_Reserved3)
{
    AiUInt32         tempBModule;
    M_RETVAL_TYPE   retVal = M_RETVAL_INI;

    TY_API_BC_SRVW_CON x_SrvReqVecCon;

    if( TRUE == M_GETMODULE( M_BOARDID, &tempBModule) )
    {
        x_SrvReqVecCon.uc_SubAddress = uc_SubAddress;
        x_SrvReqVecCon.ul_Reserved1  = ul_Reserved1;
        x_SrvReqVecCon.ul_Reserved2  = ul_Reserved2;
        x_SrvReqVecCon.ul_Reserved3  = ul_Reserved3;
        if( API_OK == (retVal=ApiCmdBCSrvReqVecCon( tempBModule, uc_Biu, uc_RtAddr, &x_SrvReqVecCon )) )
            retVal = M_RETVAL_OK;
        else
            retVal = M_RETVAL_ERR;
    }

    return retVal;
} // end: ApiCmdBCSrvReqVecCon

AI_LIB_FUNC M_RETVAL_TYPE AI_CALL_CONV M_APICMDBCGETXFERBHINFO(VI_SESSION M_BOARDID, AiUInt8 biu, AiUInt32 XferId,
                                                                AiUInt32 *BufHeaderIndex, AiUInt32 *BufHeaderAddr)
{
    AiUInt32         tempBModule;
    M_RETVAL_TYPE   retVal = M_RETVAL_INI;

    if( TRUE == M_GETMODULE( M_BOARDID, &tempBModule) )
    {
        if( API_OK == (retVal=ApiCmdBCGetXferBufferHeaderInfo(tempBModule, biu, XferId, BufHeaderIndex, BufHeaderAddr)) )
            retVal = M_RETVAL_OK;
        else
            retVal = M_RETVAL_ERR;
    }

    return retVal;
}


// Brett Crossley AIM-USA Added 15Apr2004
AI_LIB_FUNC M_RETVAL_TYPE AI_CALL_CONV M_APICMDBCINSTRTBLINI(VI_SESSION M_BOARDID, AiUInt8 uc_Biu)
{
    AiUInt32         tempBModule;
    M_RETVAL_TYPE   retVal = M_RETVAL_INI;

    if( TRUE == M_GETMODULE( M_BOARDID, &tempBModule) )
    {
        if( API_OK == (retVal=ApiCmdBCInstrTblIni( tempBModule, uc_Biu )) )
            retVal = M_RETVAL_OK;
        else
            retVal = M_RETVAL_ERR;
    }

    return retVal;

} // end: M_APICMDBCINSTRTBLINI


// Brett Crossley AIM-USA Added 15Apr2004
AI_LIB_FUNC M_RETVAL_TYPE AI_CALL_CONV M_APICMDBCINSTRTBLGETADDRFROMLABEL(VI_SESSION M_BOARDID, AiUInt8 uc_Biu,
                                                                           AiUInt32 label, AiUInt32 cnt, 
                                                                           AiUInt16 *tbl_label, AiUInt8  *tbl_op,
                                                                           AiUInt8  *tbl_res,   AiUInt32 *tbl_par1,  
                                                                           AiUInt32 *tbl_par2,  AiUInt32 *tbl_laddr,
                                                                           AiUInt32 *raddr, AiUInt32 *line)
{
    AiUInt32         tempBModule;
    M_RETVAL_TYPE   retVal = M_RETVAL_INI;
    AiUInt32        ul_Index;
    TY_API_BC_FW_INSTR *px_Tbl;

    px_Tbl = (TY_API_BC_FW_INSTR *)malloc(sizeof(TY_API_BC_FW_INSTR) * cnt);
    if( NULL == px_Tbl )
        retVal = M_RETVAL_ERR;
    else
    {
        for( ul_Index=0; ul_Index < cnt; ul_Index++ )
        {
            px_Tbl[ul_Index].label = tbl_label[ul_Index];
            px_Tbl[ul_Index].op    = tbl_op[ul_Index];
            px_Tbl[ul_Index].res   = tbl_res[ul_Index];
            px_Tbl[ul_Index].par1  = tbl_par1[ul_Index];  
            px_Tbl[ul_Index].par2  = tbl_par2[ul_Index];  
            px_Tbl[ul_Index].laddr = tbl_laddr[ul_Index];
        }

        if( TRUE == M_GETMODULE( M_BOARDID, &tempBModule) )
        {
            if( API_OK == (retVal=ApiCmdBCInstrTblGetAddrFromLabel( tempBModule, uc_Biu,
                label, 
                cnt, 
                px_Tbl, 
                raddr, 
                line)) )
                retVal = M_RETVAL_OK;
            else
                retVal = M_RETVAL_ERR;
        }
    }

    return retVal;

} // end: M_APICMDBCINSTRTBLGETADDRFROMLABEL


// Brett Crossley AIM-USA Added 15Apr2004
AI_LIB_FUNC M_RETVAL_TYPE AI_CALL_CONV M_APICMDBCINSTRTBLGEN(VI_SESSION M_BOARDID, AiUInt8 uc_Biu,
                                                              AiUInt8 mode, AiUInt32 cnt, 
                                                              AiUInt32 dest_cnt, AiUInt32 dest_offset, 
                                                              AiUInt16 *tbl_label, AiUInt8  *tbl_op,
                                                              AiUInt8  *tbl_res,   AiUInt32 *tbl_par1,
                                                              AiUInt32 *tbl_par2,  AiUInt32 *tbl_laddr,
                                                              AiUInt32 *ctbl, 
                                                              AiUInt32 *err_line, AiUInt8 *status)
{
    AiUInt32         tempBModule;
    M_RETVAL_TYPE   retVal = M_RETVAL_INI;
    AiUInt32        ul_Index;
    TY_API_BC_FW_INSTR *px_Tbl;

    px_Tbl = (TY_API_BC_FW_INSTR *)malloc(sizeof(TY_API_BC_FW_INSTR) * cnt);
    if( NULL == px_Tbl )
        retVal = M_RETVAL_ERR;
    else
    {
        for( ul_Index=0; ul_Index < cnt; ul_Index++ )
        {
            px_Tbl[ul_Index].label = tbl_label[ul_Index];
            px_Tbl[ul_Index].op    = tbl_op[ul_Index];
            px_Tbl[ul_Index].res   = tbl_res[ul_Index];
            px_Tbl[ul_Index].par1  = tbl_par1[ul_Index];  
            px_Tbl[ul_Index].par2  = tbl_par2[ul_Index];  
            px_Tbl[ul_Index].laddr = tbl_laddr[ul_Index];
        }

        if( TRUE == M_GETMODULE( M_BOARDID, &tempBModule) )
        {
            if( API_OK == (retVal=ApiCmdBCInstrTblGen( tempBModule, uc_Biu,
                mode, 
                cnt, 
                dest_cnt, 
                dest_offset,
                px_Tbl, 
                ctbl, 
                err_line, 
                status)) )
                retVal = M_RETVAL_OK;
            else
                retVal = M_RETVAL_ERR;
        }
    }

    return retVal;

} // end: M_APICMDBCINSTRTBLGEN


AI_LIB_FUNC M_RETVAL_TYPE AI_CALL_CONV M_APICMDBCMODECTRL(VI_SESSION M_BOARDID, AiUInt8 uc_Biu,
                                                           AiUInt32 ul_BcMode,
                                                           AiUInt32 ul_Ctrl,
                                                           AiUInt32 ul_Param1,
                                                           AiUInt32 ul_Param2,
                                                           AiUInt32 ul_Param3)
{
    AiUInt32         tempBModule;
    M_RETVAL_TYPE   retVal = M_RETVAL_INI;
    TY_API_BC_MODE_CTRL x_BcModeCtrl;

    if( TRUE == M_GETMODULE( M_BOARDID, &tempBModule) )
    {
        x_BcModeCtrl.ul_BcMode  = ul_BcMode;
        x_BcModeCtrl.ul_Ctrl    = ul_Ctrl;
        x_BcModeCtrl.ul_Param1  = ul_Param1;
        x_BcModeCtrl.ul_Param2  = ul_Param2;
        x_BcModeCtrl.ul_Param3  = ul_Param3;
        if( API_OK == (retVal=ApiCmdBCModeCtrl( tempBModule, uc_Biu, &x_BcModeCtrl )) )
            retVal = M_RETVAL_OK;
        else
            retVal = M_RETVAL_ERR;
    }

    return retVal;

} // end: M_APICMDBCMODECTRL


//***************************************************************************
//    API Remote Terminal Commands
//***************************************************************************
AI_LIB_FUNC M_RETVAL_TYPE AI_CALL_CONV M_APICMDRTINI(VI_SESSION M_BOARDID, AiUInt8 biu, AiUInt8 rt_addr,
                                                      AiUInt8 con, AiUInt8 bus, AiFloat resp_time, AiUInt16 nxw)
{
    AiUInt32         tempBModule;
    M_RETVAL_TYPE   retVal = M_RETVAL_INI;

    if( TRUE == M_GETMODULE( M_BOARDID, &tempBModule) )
    {
        if( API_OK == (retVal=ApiCmdRTIni(tempBModule, biu, rt_addr, con, bus, resp_time, nxw)) )
        {
            retVal = M_RETVAL_OK;
        }
        else
            retVal = M_RETVAL_ERR;
    }

    return retVal;
}

AI_LIB_FUNC M_RETVAL_TYPE AI_CALL_CONV M_APICMDRTGETSIMULATIONINFO(VI_SESSION M_BOARDID, AiUInt8 biu, AiUInt8 rt_addr,
                                                                    AiUInt8 *puc_Mode, AiUInt32 *pul_RxSA, AiUInt32 *pul_TxSA, AiUInt32 *pul_RxMC,
                                                                    AiUInt32 *pul_TxMC, AiUInt32 aul_HSRxMID[8], AiUInt32 aul_HSTxMID[8], AiUInt32 ul_HSMC)
{
    AiUInt32         tempBModule;
    M_RETVAL_TYPE   retVal = M_RETVAL_INI;
    TY_RT_INFO x_RT_Info;
    int i;

    if( TRUE == M_GETMODULE( M_BOARDID, &tempBModule) )
    {
        if( API_OK == (retVal=ApiCmdRTGetSimulationInfo(tempBModule, biu, rt_addr, &x_RT_Info)) )
        {
            *puc_Mode = x_RT_Info.uc_Mode;
            *pul_RxSA = x_RT_Info.ul_RxSA;
            *pul_TxSA = x_RT_Info.ul_TxSA;
            *pul_RxMC = x_RT_Info.ul_RxMC;
            *pul_TxMC = x_RT_Info.ul_TxMC;
            ul_HSMC   = x_RT_Info.ul_HSMC;
            for( i=0; i<8; i++)
            {
                aul_HSRxMID[i] = x_RT_Info.ul_HSRxMID[i];
                aul_HSTxMID[i] = x_RT_Info.ul_HSTxMID[i];
            }

            retVal = M_RETVAL_OK;
        }
        else
            retVal = M_RETVAL_ERR;
    }

    return retVal;
}

AI_LIB_FUNC M_RETVAL_TYPE AI_CALL_CONV M_APICMDRTENADIS(VI_SESSION M_BOARDID, AiUInt8 biu, AiUInt8 rt_addr,
                                                         AiUInt8 con)
{
    AiUInt32         tempBModule;
    M_RETVAL_TYPE   retVal = M_RETVAL_INI;

    if( TRUE == M_GETMODULE( M_BOARDID, &tempBModule) )
    {
        if( API_OK == (retVal=ApiCmdRTEnaDis(tempBModule, biu, rt_addr, con)) )
        {
            retVal = M_RETVAL_OK;
        }
        else
            retVal = M_RETVAL_ERR;
    }

    return retVal;
}

AI_LIB_FUNC M_RETVAL_TYPE AI_CALL_CONV M_APICMDRTRESPTIME(VI_SESSION M_BOARDID, AiUInt8 biu, AiUInt8 rt_addr, AiFloat resp_time)
{
    AiUInt32         tempBModule;
    M_RETVAL_TYPE   retVal = M_RETVAL_INI;

    if( TRUE == M_GETMODULE( M_BOARDID, &tempBModule) )
    {
        if( API_OK == (retVal=ApiCmdRTRespTime(tempBModule, biu, rt_addr, resp_time)) )
        {
            retVal = M_RETVAL_OK;
        }
        else
            retVal = M_RETVAL_ERR;
    }

    return retVal;
}

AI_LIB_FUNC M_RETVAL_TYPE AI_CALL_CONV M_APICMDRTRESPTIMEGET(VI_SESSION M_BOARDID, AiUInt8 biu, AiUInt8 rt_addr, AiFloat *pResp_time)
{
    AiUInt32         tempBModule;
    M_RETVAL_TYPE   retVal = M_RETVAL_INI;

    if( TRUE == M_GETMODULE( M_BOARDID, &tempBModule) )
    {
        if( API_OK == (retVal=ApiCmdRTRespTimeGet(tempBModule, biu, rt_addr, pResp_time)) )
        {
            retVal = M_RETVAL_OK;
        }
        else
            retVal = M_RETVAL_ERR;
    }

    return retVal;
}


AI_LIB_FUNC M_RETVAL_TYPE AI_CALL_CONV M_APICMDRTNXW(VI_SESSION M_BOARDID, AiUInt8 biu, AiUInt8 rt_addr, AiUInt16 nxw)
{
    AiUInt32         tempBModule;
    M_RETVAL_TYPE   retVal = M_RETVAL_INI;

    if( TRUE == M_GETMODULE( M_BOARDID, &tempBModule) )
    {
        if( API_OK == (retVal=ApiCmdRTNXW(tempBModule, biu, rt_addr, nxw)) )
        {
            retVal = M_RETVAL_OK;
        }
        else
            retVal = M_RETVAL_ERR;
    }

    return retVal;
}

AI_LIB_FUNC M_RETVAL_TYPE AI_CALL_CONV M_APICMDRTLCW(VI_SESSION M_BOARDID, AiUInt8 biu, AiUInt8 rt_addr, AiUInt16 lcw)
{
    AiUInt32         tempBModule;
    M_RETVAL_TYPE   retVal = M_RETVAL_INI;

    if( TRUE == M_GETMODULE( M_BOARDID, &tempBModule) )
    {
        if( API_OK == (retVal=ApiCmdRTLCW(tempBModule, biu, rt_addr, lcw)) )
        {
            retVal = M_RETVAL_OK;
        }
        else
            retVal = M_RETVAL_ERR;
    }

    return retVal;
}

AI_LIB_FUNC M_RETVAL_TYPE AI_CALL_CONV M_APICMDRTLSW(VI_SESSION M_BOARDID, AiUInt8 biu, AiUInt8 rt_addr, AiUInt16 lsw)
{
    AiUInt32         tempBModule;
    M_RETVAL_TYPE   retVal = M_RETVAL_INI;

    if( TRUE == M_GETMODULE( M_BOARDID, &tempBModule) )
    {
        if( API_OK == (retVal=ApiCmdRTLSW(tempBModule, biu, rt_addr, lsw)) )
        {
            retVal = M_RETVAL_OK;
        }
        else
            retVal = M_RETVAL_ERR;
    }

    return retVal;
}

AI_LIB_FUNC M_RETVAL_TYPE AI_CALL_CONV M_APICMDRTBHDEF(VI_SESSION M_BOARDID, AiUInt8 biu, AiUInt16 hid,
                                                        AiUInt16 bid, AiUInt16 sid, AiUInt16 eid, AiUInt8 qsize, AiUInt8 bqm, AiUInt8 bsm,
                                                        AiUInt8 sqm, AiUInt8 eqm, AiUInt8 res,
                                                        AiUInt16* bh_bid,
                                                        AiUInt16* bh_sid,
                                                        AiUInt16* bh_eid,
                                                        AiUInt16* bh_nbufs,
                                                        AiUInt32* bh_hid_addr,
                                                        AiUInt32* bh_bq_addr,
                                                        AiUInt32* bh_sq_addr,
                                                        AiUInt32* bh_eq_addr)
{
    AiUInt32         tempBModule;
    M_RETVAL_TYPE   retVal = M_RETVAL_INI;

    TY_API_RT_BH_INFO pbh;

    if( TRUE == M_GETMODULE( M_BOARDID, &tempBModule) )
    {
        if( API_OK == (retVal=ApiCmdRTBHDef(tempBModule, biu,  hid,  bid,  sid,  eid, qsize, bqm,
            bsm, sqm, eqm, res, &pbh)) )
        {
            *bh_bid = pbh.bid;
            *bh_sid = pbh.sid;
            *bh_eid = pbh.eid;
            *bh_nbufs = pbh.nbufs;
            *bh_hid_addr = pbh.hid_addr;
            *bh_bq_addr = pbh.bq_addr;
            *bh_sq_addr = pbh.sq_addr;
            *bh_eq_addr = pbh.eq_addr;

            retVal = M_RETVAL_OK;
        }
        else
            retVal = M_RETVAL_ERR;
    }

    return retVal;
}

AI_LIB_FUNC M_RETVAL_TYPE AI_CALL_CONV M_APICMDRTBHREAD(VI_SESSION M_BOARDID, AiUInt8 biu, AiUInt16 hid,
                                                         AiUInt16* bh_bid,
                                                         AiUInt16* bh_sid,
                                                         AiUInt16* bh_eid,
                                                         AiUInt16* bh_nbufs,
                                                         AiUInt32* bh_hid_addr,
                                                         AiUInt32* bh_bq_addr,
                                                         AiUInt32* bh_sq_addr,
                                                         AiUInt32* bh_eq_addr)
{
    AiUInt32         tempBModule;
    M_RETVAL_TYPE   retVal = M_RETVAL_INI;

    TY_API_RT_BH_INFO pbh;

    if( TRUE == M_GETMODULE( M_BOARDID, &tempBModule) )
    {
        if( API_OK == (retVal=ApiCmdRTBHRead(tempBModule, biu,  hid,  &pbh)) )
        {
            *bh_bid = pbh.bid;
            *bh_sid = pbh.sid;
            *bh_eid = pbh.eid;
            *bh_nbufs = pbh.nbufs;
            *bh_hid_addr = pbh.hid_addr;
            *bh_bq_addr = pbh.bq_addr;
            *bh_sq_addr = pbh.sq_addr;
            *bh_eq_addr = pbh.eq_addr;

            retVal = M_RETVAL_OK;
        }
        else
            retVal = M_RETVAL_ERR;
    }

    return retVal;
}

AI_LIB_FUNC M_RETVAL_TYPE AI_CALL_CONV M_APICMDRTSACON(VI_SESSION M_BOARDID, AiUInt8 biu, AiUInt8 rt_addr,
                                                        AiUInt8 sa, AiUInt16 hid, AiUInt8 sa_type, AiUInt8 con,
                                                        AiUInt8 rmod, AiUInt8 smod, AiUInt16 swm)
{
    AiUInt32         tempBModule;
    M_RETVAL_TYPE   retVal = M_RETVAL_INI;

    if( TRUE == M_GETMODULE( M_BOARDID, &tempBModule) )
    {
        if( API_OK == (retVal=ApiCmdRTSACon( tempBModule,  biu,  rt_addr, sa, hid,  sa_type,  con, rmod,  smod,  swm)) )
        {
            retVal = M_RETVAL_OK;
        }
        else
            retVal = M_RETVAL_ERR;
    }

    return retVal;
}

AI_LIB_FUNC M_RETVAL_TYPE AI_CALL_CONV M_APICMDRTSACONERR(VI_SESSION M_BOARDID, AiUInt8 biu, AiUInt8 rt_addr,
                                                           AiUInt8 sa, AiUInt8 sa_type,
                                                           AiUInt8 type,
                                                           AiUInt8 sync,
                                                           AiUInt8 config,
                                                           AiUInt8 padding1,
                                                           AiUInt32 err_spec
                                                           )
{
    AiUInt32         tempBModule;
    M_RETVAL_TYPE   retVal = M_RETVAL_INI;

    TY_API_RT_ERR perr;
    perr.type = type;
    perr.sync = sync;
    perr.contig = config;
    perr.padding1 = padding1;
    perr.err_spec = err_spec;

    if( TRUE == M_GETMODULE( M_BOARDID, &tempBModule) )
    {
        if( API_OK == (retVal=ApiCmdRTSAConErr(tempBModule, biu, rt_addr, sa, sa_type, &perr)) )
        {
            retVal = M_RETVAL_OK;
        }
        else
            retVal = M_RETVAL_ERR;
    }

    return retVal;
}

AI_LIB_FUNC M_RETVAL_TYPE AI_CALL_CONV M_APICMDRTGETSACONERR(VI_SESSION M_BOARDID, AiUInt8 biu, AiUInt8 rt_addr,
                                                              AiUInt8 sa, AiUInt8 sa_type,
                                                              AiUInt8 *type,
                                                              AiUInt8 *sync,
                                                              AiUInt8 *config,
                                                              AiUInt8 *padding1,
                                                              AiUInt32 *err_spec
                                                              )
{
    AiUInt32         tempBModule;
    M_RETVAL_TYPE   retVal = M_RETVAL_INI;

    TY_API_RT_ERR perr;

    if( TRUE == M_GETMODULE( M_BOARDID, &tempBModule) )
    {
        if( API_OK == (retVal=ApiCmdRTGetSAConErr(tempBModule, biu, rt_addr, sa, sa_type, &perr)) )
        {
            *type = perr.type;
            *sync = perr.sync;
            *config = perr.contig;
            *padding1 = perr.padding1;
            *err_spec = perr.err_spec;
            retVal = M_RETVAL_OK;
        }
        else
            retVal = M_RETVAL_ERR;
    }

    return retVal;
}

AI_LIB_FUNC M_RETVAL_TYPE AI_CALL_CONV M_APICMDRTGETSABHINFO(VI_SESSION M_BOARDID, AiUInt8 biu, AiUInt8 rt_addr,
                                                              AiUInt8 type,    AiUInt8 SA_MC, AiUInt32 *pul_BufHeaderIndex,    AiUInt32 *pul_BufHeaderAddr)
{
    AiUInt32         tempBModule;
    M_RETVAL_TYPE   retVal = M_RETVAL_INI;

    if( TRUE == M_GETMODULE( M_BOARDID, &tempBModule) )
    {
        if( API_OK == (retVal=ApiCmdRTGetSABufferHeaderInfo(tempBModule, biu, rt_addr, type, SA_MC, pul_BufHeaderIndex, pul_BufHeaderAddr)) )
        {
            retVal = M_RETVAL_OK;
        }
        else
            retVal = M_RETVAL_ERR;
    }

    return retVal;
}

AI_LIB_FUNC M_RETVAL_TYPE AI_CALL_CONV M_APICMDRTDYTAGDEF(VI_SESSION M_BOARDID, AiUInt8 biu, AiUInt8 con,
                                                           AiUInt16 rt_hid, AiUInt16 mode,
                                                           AiUInt16 tag_fct[4],
                                                           AiUInt16 min[4],
                                                           AiUInt16 max[4],
                                                           AiUInt16 step[4],
                                                           AiUInt16 wpos[4]
)
{
    AiUInt32         tempBModule;
    M_RETVAL_TYPE   retVal = M_RETVAL_INI;

    AiInt i;
    TY_API_RT_DYTAG rt_dytag[4];
    for(i=0;i<4;i++)
    {
        rt_dytag[i].tag_fct = tag_fct[i];
        rt_dytag[i].min = min[i];
        rt_dytag[i].max = max[i];
        rt_dytag[i].step = step[i];
        rt_dytag[i].wpos = wpos[i];
    }

    if( TRUE == M_GETMODULE( M_BOARDID, &tempBModule) )
    {
        if( API_OK == (retVal=ApiCmdRTDytagDef(tempBModule, biu, con, rt_hid, mode, rt_dytag)) )
        {
            retVal = M_RETVAL_OK;
        }
        else
            retVal = M_RETVAL_ERR;
    }

    return retVal;
}

AI_LIB_FUNC M_RETVAL_TYPE AI_CALL_CONV M_APICMDRTGETDYTAGDEF(VI_SESSION M_BOARDID, AiUInt8 biu, AiUInt16 rt_hid, 
                                                              AiUInt16 *mode,
                                                              AiUInt16 tag_fct[4],
                                                              AiUInt16 min[4],
                                                              AiUInt16 max[4],
                                                              AiUInt16 step[4],
                                                              AiUInt16 wpos[4]
)
{
    AiUInt32         tempBModule;
    M_RETVAL_TYPE   retVal = M_RETVAL_INI;

    AiInt i;
    TY_API_RT_DYTAG rt_dytag[4];

    if( TRUE == M_GETMODULE( M_BOARDID, &tempBModule) )
    {
        if( API_OK == (retVal=ApiCmdRTGetDytagDef(tempBModule, biu, rt_hid, mode, rt_dytag)) )
        {
            for(i=0;i<4;i++)
            {
                tag_fct[i] = rt_dytag[i].tag_fct;
                min[i] = rt_dytag[i].min;
                max[i] = rt_dytag[i].max;
                step[i] = rt_dytag[i].step;
                wpos[i] = rt_dytag[i].wpos;
            }

            retVal = M_RETVAL_OK;
        }
        else
            retVal = M_RETVAL_ERR;
    }

    return retVal;
}

AI_LIB_FUNC M_RETVAL_TYPE AI_CALL_CONV M_APICMDRTSTART(VI_SESSION M_BOARDID, AiUInt8 biu)
{
    AiUInt32         tempBModule;
    M_RETVAL_TYPE   retVal = M_RETVAL_INI;

    if( TRUE == M_GETMODULE( M_BOARDID, &tempBModule) )
    {
        if( API_OK == (retVal=ApiCmdRTStart(tempBModule, biu)) )
        {
            retVal = M_RETVAL_OK;
        }
        else
            retVal = M_RETVAL_ERR;
    }

    return retVal;
}

AI_LIB_FUNC M_RETVAL_TYPE AI_CALL_CONV M_APICMDRTHALT(VI_SESSION M_BOARDID, AiUInt8 biu)
{
    AiUInt32         tempBModule;
    M_RETVAL_TYPE   retVal = M_RETVAL_INI;

    if( TRUE == M_GETMODULE( M_BOARDID, &tempBModule) )
    {
        if( API_OK == (retVal=ApiCmdRTHalt(tempBModule, biu)) )
        {
            retVal = M_RETVAL_OK;
        }
        else
            retVal = M_RETVAL_ERR;
    }

    return retVal;
}

AI_LIB_FUNC M_RETVAL_TYPE AI_CALL_CONV M_APICMDRTSTATUSREAD(VI_SESSION M_BOARDID, AiUInt8 biu,
                                                             AiUInt8*  status,
                                                             AiUInt32* glb_msg_cnt,
                                                             AiUInt32* glb_err_cnt)
{
    AiUInt32         tempBModule;
    M_RETVAL_TYPE   retVal = M_RETVAL_INI;

    TY_API_RT_STATUS_DSP pdsp;

    if( TRUE == M_GETMODULE( M_BOARDID, &tempBModule) )
    {
        if( API_OK == (retVal=ApiCmdRTStatusRead(tempBModule, biu, &pdsp)) )
        {
            *status = pdsp.status;
            *glb_msg_cnt = pdsp.glb_msg_cnt;
            *glb_err_cnt = pdsp.glb_err_cnt;

            retVal = M_RETVAL_OK;
        }
        else
            retVal = M_RETVAL_ERR;
    }

    return retVal;
}

AI_LIB_FUNC M_RETVAL_TYPE AI_CALL_CONV M_APICMDRTMSGREAD(VI_SESSION M_BOARDID, AiUInt8 biu, AiUInt8 rt_addr,
                                                          AiUInt16* nxw,
                                                          AiUInt16* lsw,
                                                          AiUInt16* lcw,
                                                          AiUInt32* msg_cnt,
                                                          AiUInt32* err_cnt)
{
    AiUInt32         tempBModule;
    M_RETVAL_TYPE   retVal = M_RETVAL_INI;

    TY_API_RT_MSG_DSP pmsg_dsp;

    if( TRUE == M_GETMODULE( M_BOARDID, &tempBModule) )
    {
        if( API_OK == (retVal=ApiCmdRTMsgRead(tempBModule, biu, rt_addr, &pmsg_dsp)) )
        {
            *nxw = pmsg_dsp.nxw;
            *lsw = pmsg_dsp.lsw;
            *lcw = pmsg_dsp.lcw;
            *msg_cnt = pmsg_dsp.msg_cnt;
            *err_cnt = pmsg_dsp.err_cnt;

            retVal = M_RETVAL_OK;
        }
        else
            retVal = M_RETVAL_ERR;
    }

    return retVal;
}

AI_LIB_FUNC M_RETVAL_TYPE AI_CALL_CONV M_APICMDRTSAMSGREADEX(VI_SESSION M_BOARDID, 
                                                              AiUInt32 ul_RtAddr, 
                                                              AiUInt32 ul_SA,
                                                              AiUInt32 ul_SaType,
                                                              AiUInt32 ul_Biu,
                                                              AiUInt32 aul_SqCtrlWord[256],
                                                              AiUInt32 aul_SqStatusWords[256],
                                                              AiUInt32 aul_SqActBufPtr[256],
                                                              AiUInt32 aul_SqTimeTag[256],
                                                              AiUInt32 aul_EqEntryWord1[1],
                                                              AiUInt32 aul_EqEntryWord2[1],
                                                              AiUInt32 aul_EqEntryWord3[1],
                                                              AiUInt32 aul_EqEntryWord4[1],
                                                              AiUInt32 *pul_ActBufferId,
                                                              AiUInt32 *pul_ActionWord,
                                                              AiUInt32 *pul_XferCnt,
                                                              AiUInt32 *pul_ErrCnt,
                                                              AiUInt32 *pul_BufferQueueSize)
{
    AiUInt32         tempBModule;
    AiUInt32 i;
    M_RETVAL_TYPE   retVal = M_RETVAL_INI;

    TY_API_RT_SA_MSG_READ_IN x_RtSaMsgReadIn;
    TY_API_RT_SA_STATUS_EX x_RtSaStat;

    if( TRUE == M_GETMODULE( M_BOARDID, &tempBModule) )
    {
        x_RtSaMsgReadIn.ul_RtAddr = ul_RtAddr;
        x_RtSaMsgReadIn.ul_SA = ul_SA;
        x_RtSaMsgReadIn.ul_SaType = ul_SaType;
        x_RtSaMsgReadIn.ul_Biu = ul_Biu;

        if( API_OK == (retVal=ApiCmdRTSAMsgReadEx(tempBModule, &x_RtSaMsgReadIn, &x_RtSaStat)) )
        {
            *pul_BufferQueueSize    = x_RtSaStat.ul_BufferQueueSize;
            *pul_ActBufferId        = x_RtSaStat.x_XferInfo.ul_ActBufferId;
            *pul_ActionWord         = x_RtSaStat.x_XferInfo.ul_ActionWord;
            *pul_XferCnt            = x_RtSaStat.x_XferInfo.ul_XferCnt;
            *pul_ErrCnt             = x_RtSaStat.x_XferInfo.ul_ErrCnt;

            aul_EqEntryWord1[0]    = x_RtSaStat.x_XferEQueue[0].ul_EqEntryWord1;
            aul_EqEntryWord2[0]    = x_RtSaStat.x_XferEQueue[0].ul_EqEntryWord2;
            aul_EqEntryWord3[0]    = x_RtSaStat.x_XferEQueue[0].ul_EqEntryWord3;
            aul_EqEntryWord4[0]    = x_RtSaStat.x_XferEQueue[0].ul_EqEntryWord4;

            for( i=0; i<x_RtSaStat.ul_BufferQueueSize; i++ )
            {
                aul_SqCtrlWord[i]    = x_RtSaStat.x_XferSQueue[i].ul_SqCtrlWord;
                aul_SqStatusWords[i] = x_RtSaStat.x_XferSQueue[i].ul_SqStatusWords;
                aul_SqActBufPtr[i]   = x_RtSaStat.x_XferSQueue[i].ul_SqActBufPtr;
                aul_SqTimeTag[i]     = x_RtSaStat.x_XferSQueue[i].ul_SqTimeTag;
            }

            retVal = M_RETVAL_OK;
        }
        else
            retVal = M_RETVAL_ERR;
    }

    return retVal;
}

AI_LIB_FUNC M_RETVAL_TYPE AI_CALL_CONV M_APICMDRTMSGREADALL(VI_SESSION M_BOARDID, AiUInt8 biu,
                                                             AiUInt32 rt_msg[32],
                                                             AiUInt32 rt_err[32])
{
    AiUInt32         tempBModule;
    M_RETVAL_TYPE   retVal = M_RETVAL_INI;

    AiInt j;
    TY_API_RT_MSG_ALL_DSP pall_dsp;

    if( TRUE == M_GETMODULE( M_BOARDID, &tempBModule) )
    {
        if( API_OK == (retVal=ApiCmdRTMsgReadAll(tempBModule, biu, &pall_dsp)) )
        {
            for(j=0;j<32;j++)
            {
                rt_msg[j] = pall_dsp.rt[j].rt_msg;
                rt_err[j] = pall_dsp.rt[j].rt_err;
            }

            retVal = M_RETVAL_OK;
        }
        else
            retVal = M_RETVAL_ERR;
    }

    return retVal;
}

AI_LIB_FUNC M_RETVAL_TYPE AI_CALL_CONV M_APICMDRTSAMSGREAD(VI_SESSION M_BOARDID, AiUInt8 biu, AiUInt8 rt_addr,
                                                            AiUInt8 sa, AiUInt8 sa_type, AiUInt8 clr,
                                                            AiUInt16* bid,
                                                            AiUInt16* trw,
                                                            AiUInt16* lcw,
                                                            AiUInt16* lsw,
                                                            AiUInt32* bufp,
                                                            AiUInt32* ttag)
{
    AiUInt32         tempBModule;
    M_RETVAL_TYPE   retVal = M_RETVAL_INI;

    TY_API_RT_SA_MSG_DSP psa_dsp;

    if( TRUE == M_GETMODULE( M_BOARDID, &tempBModule) )
    {
        if( API_OK == (retVal=ApiCmdRTSAMsgRead(tempBModule, biu, rt_addr, sa, sa_type, clr, &psa_dsp)) )
        {
            *bid = psa_dsp.bid;
            *trw = psa_dsp.trw;
            *lcw = psa_dsp.lcw;
            *lsw = psa_dsp.lsw;
            *bufp = psa_dsp.bufp;
            *ttag = psa_dsp.ttag;

            retVal = M_RETVAL_OK;
        }
        else
            retVal = M_RETVAL_ERR;
    }

    return retVal;
}

AI_LIB_FUNC M_RETVAL_TYPE AI_CALL_CONV M_APICMDRTGLOBALCON(VI_SESSION M_BOARDID, AiUInt8 biu, AiUInt16 count,
                                                            AiUInt16 buffer[512][32],
                                                            AiUInt8  mode[512],
                                                            AiUInt8  rt[512],
                                                            AiUInt8  rt_con[512],
                                                            AiUInt8  sa_mc[512],
                                                            AiUInt8  sa_type[512],
                                                            AiUInt8  sa_con[512],
                                                            AiUInt8  resp_time[512],
                                                            AiUInt8  smod[512],
                                                            AiUInt16 nxw[512],
                                                            AiUInt16 swm[512],
                                                            AiUInt16 hid[512],
                                                            AiUInt16 bid[512])
{
    AiUInt32         tempBModule;
    M_RETVAL_TYPE   retVal = M_RETVAL_INI;

    AiInt i;
    AiInt j;
    TY_API_RT_SA lpBuf[512];

    for(i=0;i<count;i++)
    {
        for(j=0;j<32;j++)
        {
            lpBuf[i].buffer[j] = buffer[i][j];
        }
        lpBuf[i].mode = mode[i];
        lpBuf[i].rt = rt[i];
        lpBuf[i].rt_con = rt_con[i];
        lpBuf[i].sa_mc = sa_mc[i];
        lpBuf[i].sa_type = sa_type[i];
        lpBuf[i].sa_con = sa_con[i];
        lpBuf[i].resp_time = resp_time[i];
        lpBuf[i].smod = smod[i];
        lpBuf[i].nxw = nxw[i];
        lpBuf[i].swm = swm[i];
        lpBuf[i].hid = hid[i];
        lpBuf[i].bid = bid[i];
    }
    if( TRUE == M_GETMODULE( M_BOARDID, &tempBModule) )
    {
        if( API_OK == (retVal=ApiCmdRTGlobalCon(tempBModule, biu, count, lpBuf)) )
        {
            retVal = M_RETVAL_OK;
        }
        else
            retVal = M_RETVAL_ERR;
    }

    return retVal;
}

AI_LIB_FUNC M_RETVAL_TYPE AI_CALL_CONV M_APICMDRTSAASSIGNFIFO(VI_SESSION M_BOARDID, AiUInt8 biu, AiUInt8 con,
                                                               AiUInt8 f_id, AiUInt8 rt, AiUInt8 sa)
{
    AiUInt32         tempBModule;
    M_RETVAL_TYPE   retVal = M_RETVAL_INI;

    if( TRUE == M_GETMODULE( M_BOARDID, &tempBModule) )
    {
        if( API_OK == (retVal=ApiCmdRTSAAssignFifo(tempBModule, biu, con, f_id, rt, sa)) )
        {
            retVal = M_RETVAL_OK;
        }
        else
            retVal = M_RETVAL_ERR;
    }

    return retVal;
}

AI_LIB_FUNC M_RETVAL_TYPE AI_CALL_CONV M_APICMDRTMODECTRL(VI_SESSION M_BOARDID, AiUInt8 uc_Biu,
                                                           AiUInt32 ul_RtMode,
                                                           AiUInt32 ul_Ctrl,
                                                           AiUInt32 ul_Param1,
                                                           AiUInt32 ul_Param2,
                                                           AiUInt32 ul_Param3)
{
    AiUInt32         tempBModule;
    M_RETVAL_TYPE   retVal = M_RETVAL_INI;
    TY_API_RT_MODE_CTRL x_RtModeCtrl;

    if( TRUE == M_GETMODULE( M_BOARDID, &tempBModule) )
    {
        x_RtModeCtrl.ul_RtMode  = ul_RtMode;
        x_RtModeCtrl.ul_Ctrl    = ul_Ctrl;
        x_RtModeCtrl.ul_Param1  = ul_Param1;
        x_RtModeCtrl.ul_Param2  = ul_Param2;
        x_RtModeCtrl.ul_Param3  = ul_Param3;
        if( API_OK == (retVal=ApiCmdRTModeCtrl( tempBModule, uc_Biu, &x_RtModeCtrl )) )
            retVal = M_RETVAL_OK;
        else
            retVal = M_RETVAL_ERR;
    }

    return retVal;

} // end: M_APICMDRTMODECTRL


//***************************************************************************
//    API Bus Monitor Commands
//***************************************************************************
AI_LIB_FUNC M_RETVAL_TYPE AI_CALL_CONV M_APICMDBMINI(VI_SESSION M_BOARDID, AiUInt8 biu)
{
    AiUInt32         tempBModule;
    M_RETVAL_TYPE   retVal = M_RETVAL_INI;

    if( TRUE == M_GETMODULE( M_BOARDID, &tempBModule) )
    {
        if( API_OK == (retVal=ApiCmdBMIni(tempBModule, biu)) )
        {
            retVal = M_RETVAL_OK;
        }
        else
            retVal = M_RETVAL_ERR;
    }

    return retVal;
}

AI_LIB_FUNC M_RETVAL_TYPE AI_CALL_CONV M_APICMDBMINTRMODE(VI_SESSION M_BOARDID, AiUInt8 biu, AiUInt8 imod, AiUInt8 smod, AiUInt8 res)
{
    AiUInt32         tempBModule;
    M_RETVAL_TYPE   retVal = M_RETVAL_INI;

    if( TRUE == M_GETMODULE( M_BOARDID, &tempBModule) )
    {
        if( API_OK == (retVal=ApiCmdBMIntrMode(tempBModule, biu, imod, smod, res)) )
        {
            retVal = M_RETVAL_OK;
        }
        else
            retVal = M_RETVAL_ERR;
    }

    return retVal;
}

AI_LIB_FUNC M_RETVAL_TYPE AI_CALL_CONV M_APICMDBMCAPMODE(VI_SESSION M_BOARDID, AiUInt8 biu,
                                                          AiUInt8  cap_mode,
                                                          AiUInt32 cap_tat,
                                                          AiUInt16 cap_mcc,
                                                          AiUInt16 cap_fsize)
{
    AiUInt32         tempBModule;
    M_RETVAL_TYPE   retVal = M_RETVAL_INI;

    TY_API_BM_CAP_SETUP pcap;
    pcap.cap_mode = cap_mode;
    pcap.cap_tat = cap_tat;
    pcap.cap_mcc = cap_mcc;
    pcap.cap_fsize = cap_fsize;

    if( TRUE == M_GETMODULE( M_BOARDID, &tempBModule) )
    {
        if( API_OK == (retVal=ApiCmdBMCapMode(tempBModule, biu, &pcap)) )
        {
            retVal = M_RETVAL_OK;
        }
        else
            retVal = M_RETVAL_ERR;
    }

    return retVal;
}

AI_LIB_FUNC M_RETVAL_TYPE AI_CALL_CONV M_APICMDBMSWXMINI(VI_SESSION M_BOARDID, AiUInt8 biu, AiUInt16 swxm)
{
    AiUInt32         tempBModule;
    M_RETVAL_TYPE   retVal = M_RETVAL_INI;

    if( TRUE == M_GETMODULE( M_BOARDID, &tempBModule) )
    {
        if( API_OK == (retVal=ApiCmdBMSWXMIni(tempBModule, biu, swxm)) )
        {
            retVal = M_RETVAL_OK;
        }
        else
            retVal = M_RETVAL_ERR;
    }

    return retVal;
}

AI_LIB_FUNC M_RETVAL_TYPE AI_CALL_CONV M_APICMDBMSTART(VI_SESSION M_BOARDID, AiUInt8 biu)
{
    AiUInt32         tempBModule;
    M_RETVAL_TYPE   retVal = M_RETVAL_INI;

    if( TRUE == M_GETMODULE( M_BOARDID, &tempBModule) )
    {
        if( API_OK == (retVal=ApiCmdBMStart(tempBModule, biu)) )
        {
            retVal = M_RETVAL_OK;
        }
        else
            retVal = M_RETVAL_ERR;
    }

    return retVal;
}

AI_LIB_FUNC M_RETVAL_TYPE AI_CALL_CONV M_APICMDBMHALT(VI_SESSION M_BOARDID, AiUInt8 biu)
{
    AiUInt32         tempBModule;
    M_RETVAL_TYPE   retVal = M_RETVAL_INI;

    if( TRUE == M_GETMODULE( M_BOARDID, &tempBModule) )
    {
        if( API_OK == (retVal=ApiCmdBMHalt(tempBModule, biu)) )
        {
            retVal = M_RETVAL_OK;
        }
        else
            retVal = M_RETVAL_ERR;
    }

    return retVal;
}

AI_LIB_FUNC M_RETVAL_TYPE AI_CALL_CONV M_APICMDBMSTATUSREAD(VI_SESSION M_BOARDID, AiUInt8 biu,
                                                             AiUInt8*  men,
                                                             AiUInt8*  msw,
                                                             AiUInt8*  msp,
                                                             AiUInt32* glb_msg_cnt,
                                                             AiUInt32* glb_err_cnt,
                                                             AiUInt32* glb_word_cnt_pri,
                                                             AiUInt32* glb_word_cnt_sec,
                                                             AiUInt32* glb_msg_cnt_pri,
                                                             AiUInt32* glb_msg_cnt_sec,
                                                             AiUInt32* glb_err_cnt_pri,
                                                             AiUInt32* glb_err_cnt_sec,
                                                             AiUInt32* bus_load_pri,
                                                             AiUInt32* bus_load_sec,
                                                             AiUInt32* bus_load_pri_avg,
                                                             AiUInt32* bus_load_sec_avg )
{
    AiUInt32         tempBModule;
    M_RETVAL_TYPE   retVal = M_RETVAL_INI;

    TY_API_BM_STATUS_DSP pdsp;

    if( TRUE == M_GETMODULE( M_BOARDID, &tempBModule) )
    {
        if( API_OK == (retVal=ApiCmdBMStatusRead(tempBModule, biu, &pdsp)) )
        {
            *men = pdsp.men;
            *msw = pdsp.msw;
            *msp = pdsp.msp;
            *glb_msg_cnt = pdsp.glb_msg_cnt;
            *glb_err_cnt = pdsp.glb_err_cnt;
            *glb_word_cnt_pri = pdsp.glb_word_cnt_pri;
            *glb_word_cnt_sec = pdsp.glb_word_cnt_sec;
            *glb_msg_cnt_pri  = pdsp.glb_msg_cnt_pri;
            *glb_msg_cnt_sec  = pdsp.glb_msg_cnt_sec;
            *glb_err_cnt_pri  = pdsp.glb_err_cnt_pri;
            *glb_err_cnt_sec  = pdsp.glb_err_cnt_sec;
            *bus_load_pri     = pdsp.bus_load_pri;
            *bus_load_sec     = pdsp.bus_load_sec;
            *bus_load_pri_avg = pdsp.bus_load_pri_avg;
            *bus_load_sec_avg = pdsp.bus_load_sec_avg;

            retVal = M_RETVAL_OK;
        }
        else
            retVal = M_RETVAL_ERR;
    }

    return retVal;
}

AI_LIB_FUNC M_RETVAL_TYPE AI_CALL_CONV M_APICMDBMACTREAD(VI_SESSION M_BOARDID, AiUInt8 biu, AiUInt16 entry_no,
                                                          AiUInt8*  fnd,
                                                          AiUInt8*  rt,
                                                          AiUInt8*  tr,
                                                          AiUInt8*  sa,
                                                          AiUInt32* cc,
                                                          AiUInt32* ec,
                                                          AiUInt32* et)
{
    AiUInt32         tempBModule;
    M_RETVAL_TYPE   retVal = M_RETVAL_INI;

    TY_API_BM_ACT_DSP pact;

    if( TRUE == M_GETMODULE( M_BOARDID, &tempBModule) )
    {
        if( API_OK == (retVal=ApiCmdBMActRead(tempBModule, biu, entry_no, &pact)) )
        {
            *fnd = pact.fnd;
            *rt = pact.rt;
            *tr = pact.tr;
            *sa = pact.sa;
            *cc = pact.cc;
            *ec = pact.ec;
            *et = pact.et;

            retVal = M_RETVAL_OK;
        }
        else
            retVal = M_RETVAL_ERR;
    }

    return retVal;
}

AI_LIB_FUNC M_RETVAL_TYPE AI_CALL_CONV  M_APICMDBMSTACKPREAD(VI_SESSION M_BOARDID, AiUInt8 biu,
                                                              AiUInt8*  status,
                                                              AiUInt32* stp,
                                                              AiUInt32* ctp,
                                                              AiUInt32* etp)
{
    AiUInt32         tempBModule;
    M_RETVAL_TYPE   retVal = M_RETVAL_INI;

    TY_API_BM_STACKP_DSP pstackp;

    if( TRUE == M_GETMODULE( M_BOARDID, &tempBModule) )
    {
        if( API_OK == (retVal=ApiCmdBMStackpRead(tempBModule, biu, &pstackp)) )
        {
            *status = pstackp.status;
            *stp = pstackp.stp;
            *ctp = pstackp.ctp;
            *etp = pstackp.etp;

            retVal = M_RETVAL_OK;
        }
        else
            retVal = M_RETVAL_ERR;
    }

    return retVal;
}

AI_LIB_FUNC M_RETVAL_TYPE AI_CALL_CONV M_APICMDBMSTACKENTRYREAD(VI_SESSION M_BOARDID, AiUInt8 biu, AiUInt8 ptype,
                                                                 AiUInt8 sgn, AiUInt32 offset,
                                                                 AiUInt32* entry)
{
    AiUInt32         tempBModule;
    M_RETVAL_TYPE   retVal = M_RETVAL_INI;

    TY_API_BM_STACK_DSP pentry;

    if( TRUE == M_GETMODULE( M_BOARDID, &tempBModule) )
    {
        if( API_OK == (retVal=ApiCmdBMStackEntryRead(tempBModule, biu, ptype, sgn, offset, &pentry)) )
        {
            *entry = pentry.entry;

            retVal = M_RETVAL_OK;
        }
        else
            retVal = M_RETVAL_ERR;
    }

    return retVal;
}

AI_LIB_FUNC M_RETVAL_TYPE AI_CALL_CONV M_APICMDBMSTACKENTRYFIND(VI_SESSION M_BOARDID, AiUInt8 biu, AiUInt8 ptype,
                                                                 AiUInt8 sgn, AiUInt32 offset,
                                                                 AiUInt16 fspec, AiUInt16 fdata, AiUInt16 fmask,
                                                                 AiUInt8*  efnd,
                                                                 AiUInt32* eptr,
                                                                 AiUInt32* entry)
{
    AiUInt32         tempBModule;
    M_RETVAL_TYPE   retVal = M_RETVAL_INI;

    TY_API_BM_STACK_FND pfnd;

    if( TRUE == M_GETMODULE( M_BOARDID, &tempBModule) )
    {
        if( API_OK == (retVal=ApiCmdBMStackEntryFind(tempBModule, biu, ptype, sgn, offset, fspec, fdata, fmask, &pfnd)) )
        {
            *efnd = pfnd.efnd;
            *eptr = pfnd.eptr;
            *entry = pfnd.entry;

            retVal = M_RETVAL_OK;
        }
        else
            retVal = M_RETVAL_ERR;
    }

    return retVal;
}

AI_LIB_FUNC M_RETVAL_TYPE AI_CALL_CONV M_APICMDBMRECSTATUSREAD(VI_SESSION M_BOARDID, AiUInt8 biu,
                                                                AiUInt8*  status,
                                                                AiUInt32* hfi_cnt,
                                                                AiUInt32* saddr,
                                                                AiUInt32* size)
{
    AiUInt32         tempBModule;
    M_RETVAL_TYPE   retVal = M_RETVAL_INI;

    TY_API_BM_REC prec;

    if( TRUE == M_GETMODULE( M_BOARDID, &tempBModule) )
    {
        if( API_OK == (retVal=ApiCmdBMRecStatusRead(tempBModule, biu, &prec)) )
        {
            *status = prec.status;
            *hfi_cnt = prec.hfi_cnt;
            *saddr = prec.saddr;
            *size = prec.size;

            retVal = M_RETVAL_OK;
        }
        else
            retVal = M_RETVAL_ERR;
    }

    return retVal;
}

AI_LIB_FUNC M_RETVAL_TYPE AI_CALL_CONV M_APICMDBMCOPYRECDAT(VI_SESSION M_BOARDID, AiUInt8 biu,
                                                             AiUInt8  status,
                                                             AiUInt32 hfi_cnt,
                                                             AiUInt32 saddr,
                                                             AiUInt32 size)
{
    AiUInt32         tempBModule;
    M_RETVAL_TYPE   retVal = M_RETVAL_INI;

    TY_API_BM_REC prec;
    prec.status = status;
    prec.hfi_cnt = hfi_cnt;
    prec.saddr = saddr;
    prec.size = size;

    if( TRUE == M_GETMODULE( M_BOARDID, &tempBModule) )
    {
        if( API_OK == (retVal=ApiCmdBMCopyRecDat(tempBModule, biu, &prec)) )
        {
            retVal = M_RETVAL_OK;
        }
        else
            retVal = M_RETVAL_ERR;
    }

    return retVal;
}

AI_LIB_FUNC M_RETVAL_TYPE AI_CALL_CONV M_APICMDBMFILTERINI(VI_SESSION M_BOARDID, AiUInt8 biu, AiUInt8 rt_addr,
                                                            AiUInt32 rx_sa, AiUInt32 tx_sa, AiUInt32 rx_mc, AiUInt32 tx_mc)
{
    AiUInt32         tempBModule;
    M_RETVAL_TYPE   retVal = M_RETVAL_INI;

    if( TRUE == M_GETMODULE( M_BOARDID, &tempBModule) )
    {
        if( API_OK == (retVal=ApiCmdBMFilterIni(tempBModule, biu, rt_addr, rx_sa, tx_sa, rx_mc, tx_mc)) )
        {
            retVal = M_RETVAL_OK;
        }
        else
            retVal = M_RETVAL_ERR;
    }

    return retVal;
}

AI_LIB_FUNC M_RETVAL_TYPE AI_CALL_CONV M_APICMDBMILLEGALINI(VI_SESSION M_BOARDID, AiUInt8 biu, AiUInt8 rt_addr,
                                                             AiUInt32 rx_sa, AiUInt32 tx_sa, AiUInt32 rx_mc, AiUInt32 tx_mc)
{
    AiUInt32         tempBModule;
    M_RETVAL_TYPE   retVal = M_RETVAL_INI;

    if( TRUE == M_GETMODULE( M_BOARDID, &tempBModule) )
    {
        if( API_OK == (retVal=ApiCmdBMIllegalIni(tempBModule, biu, rt_addr, rx_sa, tx_sa, rx_mc, tx_mc)) )
        {
            retVal = M_RETVAL_OK;
        }
        else
            retVal = M_RETVAL_ERR;
    }

    return retVal;
}

AI_LIB_FUNC M_RETVAL_TYPE AI_CALL_CONV M_APICMDBMFTWINI(VI_SESSION M_BOARDID, AiUInt8 biu, AiUInt8 con, AiUInt8 htm, AiUInt8 htc, AiUInt8 stm, AiUInt8 stc)
{
    AiUInt32         tempBModule;
    M_RETVAL_TYPE   retVal = M_RETVAL_INI;

    if( TRUE == M_GETMODULE( M_BOARDID, &tempBModule) )
    {
        if( API_OK == (retVal=ApiCmdBMFTWIni(tempBModule, biu, con, htm, htc, stm, stc)) )
        {
            retVal = M_RETVAL_OK;
        }
        else
            retVal = M_RETVAL_ERR;
    }

    return retVal;
}

AI_LIB_FUNC M_RETVAL_TYPE AI_CALL_CONV M_APICMDBMTIWINI(VI_SESSION M_BOARDID, AiUInt8 biu, AiUInt8 con, AiUInt8 eti, AiUInt8 aei, AiUInt8 ti0, AiUInt8 ti1)
{
    AiUInt32         tempBModule;
    M_RETVAL_TYPE   retVal = M_RETVAL_INI;

    if( TRUE == M_GETMODULE( M_BOARDID, &tempBModule) )
    {
        if( API_OK == (retVal=ApiCmdBMTIWIni(tempBModule, biu, con, eti, aei, ti0, ti1)) )
        {
            retVal = M_RETVAL_OK;
        }
        else
            retVal = M_RETVAL_ERR;
    }

    return retVal;
}

AI_LIB_FUNC M_RETVAL_TYPE AI_CALL_CONV M_APICMDBMTCIINI(VI_SESSION M_BOARDID, AiUInt8 biu, AiUInt8 rt, AiUInt8 sa,
                                                         AiUInt8 sa_type, AiUInt8 tagm, AiUInt8 tfct, AiUInt8 tid)
{
    AiUInt32         tempBModule;
    M_RETVAL_TYPE   retVal = M_RETVAL_INI;

    if( TRUE == M_GETMODULE( M_BOARDID, &tempBModule) )
    {
        if( API_OK == (retVal=ApiCmdBMTCIIni(tempBModule, biu, rt, sa, sa_type, tagm, tfct, tid)) )
        {
            retVal = M_RETVAL_OK;
        }
        else
            retVal = M_RETVAL_ERR;
    }

    return retVal;
}

AI_LIB_FUNC M_RETVAL_TYPE AI_CALL_CONV M_APICMDBMTCBINI(VI_SESSION M_BOARDID, AiUInt8 biu, AiUInt8 tid, AiUInt8 ten,
                                                         AiUInt8  tt,
                                                         AiUInt8  sot,
                                                         AiUInt8  tri,
                                                         AiUInt8  inv,
                                                         AiUInt8  tres,
                                                         AiUInt8  tset,
                                                         AiUInt8  tsp,
                                                         AiUInt8  next,
                                                         AiUInt8  eom,
                                                         AiUInt16 tdw,
                                                         AiUInt16 tmw,
                                                         AiUInt16 tuli,
                                                         AiUInt16 tlli)
{
    AiUInt32         tempBModule;
    M_RETVAL_TYPE   retVal = M_RETVAL_INI;

    TY_API_BM_TCB ptcb;
    ptcb.tt = tt;
    ptcb.sot = sot;
    ptcb.tri = tri;
    ptcb.inv = inv;
    ptcb.tres = tres;
    ptcb.tset = tset;
    ptcb.tsp = tsp;
    ptcb.next = next;
    ptcb.eom = eom;
    ptcb.tdw = tdw;
    ptcb.tmw = tmw;
    ptcb.tuli = tuli;
    ptcb.tlli = tlli;

    if( TRUE == M_GETMODULE( M_BOARDID, &tempBModule) )
    {
        if( API_OK == (retVal=ApiCmdBMTCBIni(tempBModule, biu, tid, ten, &ptcb)) )
        {
            retVal = M_RETVAL_OK;
        }
        else
            retVal = M_RETVAL_ERR;
    }

    return retVal;
}

AI_LIB_FUNC M_RETVAL_TYPE AI_CALL_CONV M_APICMDBMRTACTREAD(VI_SESSION M_BOARDID, AiUInt8 biu, AiUInt8 rt,
                                                            AiUInt32* mc,
                                                            AiUInt32* ec,
                                                            AiUInt32* et)
{
    AiUInt32         tempBModule;
    M_RETVAL_TYPE   retVal = M_RETVAL_INI;

    TY_API_BM_RT_ACT pact;

    if( TRUE == M_GETMODULE( M_BOARDID, &tempBModule) )
    {
        if( API_OK == (retVal=ApiCmdBMRTActRead(tempBModule, biu, rt, &pact)) )
        {
            *mc = pact.mc;
            *ec = pact.ec;
            *et = pact.et;

            retVal = M_RETVAL_OK;
        }
        else
            retVal = M_RETVAL_ERR;
    }

    return retVal;
}

AI_LIB_FUNC M_RETVAL_TYPE AI_CALL_CONV M_APICMDBMRTSAACTREAD(VI_SESSION M_BOARDID, AiUInt8 biu, AiUInt8 rt, AiUInt8 sa, AiUInt8 sa_type,
                                                              AiUInt32* mc,
                                                              AiUInt32* ec,
                                                              AiUInt32* et)
{
    AiUInt32         tempBModule;
    M_RETVAL_TYPE   retVal = M_RETVAL_INI;

    TY_API_BM_RT_ACT pact;

    if( TRUE == M_GETMODULE( M_BOARDID, &tempBModule) )
    {
        if( API_OK == (retVal=ApiCmdBMRTSAActRead(tempBModule, biu, rt, sa, sa_type, &pact)) )
        {
            *mc = pact.mc;
            *ec = pact.ec;
            *et = pact.et;

            retVal = M_RETVAL_OK;
        }
        else
            retVal = M_RETVAL_ERR;
    }

    return retVal;
}

/* 01.05.99 */
AI_LIB_FUNC M_RETVAL_TYPE AI_CALL_CONV M_APICMDBMINIMSGFLTREC(VI_SESSION M_BOARDID, AiUInt8 biu, AiUInt8 cnt,
                                                               AiUInt16 cw[40],
                                                               AiUInt8 pos1[40],
                                                               AiUInt8 pos2[40])
{
    AiUInt32         tempBModule;
    M_RETVAL_TYPE   retVal = M_RETVAL_INI;

    AiInt i;
    TY_API_BM_INI_MSG_FLT_REC pmrec[40];
    for(i=0;(i<cnt) && (i<MAX_API_BM_MSG_FLT_REC);i++)
    {
        pmrec[i].cw = cw[i];
        pmrec[i].pos1 = pos1[i];
        pmrec[i].pos2 = pos2[i];
    }

    if( TRUE == M_GETMODULE( M_BOARDID, &tempBModule) )
    {
        if( API_OK == (retVal=ApiCmdBMIniMsgFltRec(tempBModule, biu, cnt, pmrec)) )
        {
            retVal = M_RETVAL_OK;
        }
        else
            retVal = M_RETVAL_ERR;
    }

    return retVal;
}

AI_LIB_FUNC M_RETVAL_TYPE AI_CALL_CONV M_APICMDBMREADMSGFLTREC(VI_SESSION M_BOARDID, AiUInt8 biu, AiUInt8 mode,
                                                                AiUInt8 con, AiUInt32 max_size, AiUInt16 max_msg,
                                                                AiAddr lpBuf, AiUInt8* ovfl, AiUInt32* lBytesRead)
{
    AiUInt32         tempBModule;
    M_RETVAL_TYPE   retVal = M_RETVAL_INI;

    if( TRUE == M_GETMODULE( M_BOARDID, &tempBModule) )
    {
        if( API_OK == (retVal=ApiCmdBMReadMsgFltRec(tempBModule, biu, mode, con, max_size, max_msg,
            lpBuf, ovfl, lBytesRead)) )
        {
            retVal = M_RETVAL_OK;
        }
        else
            retVal = M_RETVAL_ERR;
    }

    return retVal;
}

AI_LIB_FUNC M_RETVAL_TYPE AI_CALL_CONV M_APICMDBMDYTAGMONDEF(VI_SESSION M_BOARDID, AiUInt8 biu, AiUInt8 uc_Id,
                                                              AiUInt8 uc_Con, AiUInt8 uc_RtAddr, AiUInt8 uc_SubAddrMsgId, AiUInt8 uc_SubAddrMsgIdType,
                                                              AiUInt16 uw_DytagType, AiUInt16 uw_DytagWPos, AiUInt16 uw_DytagBPos,
                                                              AiUInt16 uw_DytagBLen, AiUInt16 uw_DytagStep)
{
    AiUInt32         tempBModule;
    M_RETVAL_TYPE   retVal = M_RETVAL_INI;
    TY_API_BM_DYTAG_MON_DEF x_DytagMon;

    if( TRUE == M_GETMODULE( M_BOARDID, &tempBModule) )
    {
        x_DytagMon.uc_Id          = uc_Id;
        x_DytagMon.uc_Con         = uc_Con;
        x_DytagMon.uc_RtAddr      = uc_RtAddr;
        x_DytagMon.uc_SubAddrMsgId= uc_SubAddrMsgId;
        x_DytagMon.uc_SubAddrMsgIdType = uc_SubAddrMsgIdType;
        x_DytagMon.uw_DytagType   = uw_DytagType;
        x_DytagMon.uw_DytagWPos   = uw_DytagWPos;
        x_DytagMon.uw_DytagBPos   = uw_DytagBPos;
        x_DytagMon.uw_DytagBLen   = uw_DytagBLen;
        x_DytagMon.uw_DytagStep   = uw_DytagStep;
        if( API_OK == (retVal=ApiCmdBMDytagMonDef(tempBModule, biu, &x_DytagMon)) )
        {
            retVal = M_RETVAL_OK;
        }
        else
            retVal = M_RETVAL_ERR;
    }

    return retVal;
}


AI_LIB_FUNC M_RETVAL_TYPE AI_CALL_CONV M_APICMDBMDYTAGMONREAD(VI_SESSION M_BOARDID, AiUInt8 biu, AiUInt8 uc_Id,
                                                               AiUInt32 *ul_Stale, AiUInt32 *ul_Bad, AiUInt32 *ul_Good)
{
    AiUInt32         tempBModule;
    M_RETVAL_TYPE   retVal = M_RETVAL_INI;
    TY_API_BM_DYTAG_MON_READ_CTRL x_DytagMonCtrl;
    TY_API_BM_DYTAG_MON_ACT x_DytagMonAct;

    if( TRUE == M_GETMODULE( M_BOARDID, &tempBModule) )
    {
        x_DytagMonCtrl.uc_Id = uc_Id;
        if( API_OK == (retVal=ApiCmdBMDytagMonRead(tempBModule, biu, &x_DytagMonCtrl, &x_DytagMonAct)) )
        {
            retVal = M_RETVAL_OK;
            *ul_Stale = x_DytagMonAct.ul_Stale;
            *ul_Bad   = x_DytagMonAct.ul_Bad;
            *ul_Good  = x_DytagMonAct.ul_Good;
        }
        else
            retVal = M_RETVAL_ERR;
    }

    return retVal;
}


/* Queueing Commands */

AI_LIB_FUNC M_RETVAL_TYPE AI_CALL_CONV M_APICMDQUEUEINI(VI_SESSION M_BOARDID, AiUInt8 biu)
{
    AiUInt32         tempBModule;
    M_RETVAL_TYPE   retVal = M_RETVAL_INI;

    if( TRUE == M_GETMODULE( M_BOARDID, &tempBModule) )
    {
        if( API_OK == (retVal=ApiCmdQueueIni(tempBModule, biu)) )
        {
            retVal = M_RETVAL_OK;
        }
        else
            retVal = M_RETVAL_ERR;
    }

    return retVal;
}

AI_LIB_FUNC M_RETVAL_TYPE AI_CALL_CONV M_APICMDQUEUEREAD(VI_SESSION M_BOARDID, AiUInt8 biu,
                                                          AiUInt8*  rt_addr,
                                                          AiUInt8*  sa_mc,
                                                          AiUInt8*  sa_type,
                                                          AiUInt8*  rt_addr2,
                                                          AiUInt8*  sa_mc2,
                                                          AiUInt8*  sa_type2,
                                                          AiUInt8*  word_cnt,
                                                          AiUInt8*  rbf_trw,
                                                          AiUInt16* msg_trw,
                                                          AiUInt16* status_word1,
                                                          AiUInt16* status_word2,
                                                          AiUInt16 buffer[32],
                                                          AiUInt32* ttag)
{
    AiUInt32         tempBModule;
    M_RETVAL_TYPE   retVal = M_RETVAL_INI;

    AiInt i;
    TY_API_QUEUE_BUF qmesg;

    if( TRUE == M_GETMODULE( M_BOARDID, &tempBModule) )
    {
        if( API_OK == (retVal=ApiCmdQueueRead(tempBModule, biu, &qmesg)) )
        {
            *rt_addr      = qmesg.rt_addr;
            *sa_mc        = qmesg.sa_mc;
            *sa_type      = qmesg.sa_type;
            *rt_addr2     = qmesg.rt_addr2;
            *sa_mc2       = qmesg.sa_mc2;
            *sa_type2     = qmesg.sa_type2;
            *word_cnt     = qmesg.word_cnt;
            *rbf_trw      = qmesg.rbf_trw;
            *msg_trw      = qmesg.msg_trw;
            *status_word1 = qmesg.status_word1;
            *status_word2 = qmesg.status_word2;
            for(i=0;i<32;i++)
            {
                buffer[i] = qmesg.buffer[i];
            }
            *ttag         = qmesg.ttag;

            retVal = M_RETVAL_OK;
        }
    }

    return retVal;
}

AI_LIB_FUNC M_RETVAL_TYPE AI_CALL_CONV M_APICMDQUEUEFLUSH(VI_SESSION M_BOARDID, AiUInt8 biu)
{
    AiUInt32         tempBModule;
    M_RETVAL_TYPE   retVal = M_RETVAL_INI;

    if( TRUE == M_GETMODULE( M_BOARDID, &tempBModule) )
    {
        if( API_OK == (retVal=ApiCmdQueueFlush(tempBModule, biu)) )
        {
            retVal = M_RETVAL_OK;
        }
        else
            retVal = M_RETVAL_ERR;
    }

    return retVal;
}
AI_LIB_FUNC M_RETVAL_TYPE AI_CALL_CONV M_APICMDQUEUESTART(VI_SESSION M_BOARDID, AiUInt8 biu)
{
    AiUInt32         tempBModule;
    M_RETVAL_TYPE   retVal = M_RETVAL_INI;

    if( TRUE == M_GETMODULE( M_BOARDID, &tempBModule) )
    {
        if( API_OK == (retVal=ApiCmdQueueStart(tempBModule, biu)) )
        {
            retVal = M_RETVAL_OK;
        }
        else
            retVal = M_RETVAL_ERR;
    }

    return retVal;
}
AI_LIB_FUNC M_RETVAL_TYPE AI_CALL_CONV M_APICMDQUEUEHALT(VI_SESSION M_BOARDID, AiUInt8 biu)
{
    AiUInt32         tempBModule;
    M_RETVAL_TYPE   retVal = M_RETVAL_INI;

    if( TRUE == M_GETMODULE( M_BOARDID, &tempBModule) )
    {
        if( API_OK == (retVal=ApiCmdQueueHalt(tempBModule, biu)) )
        {
            retVal = M_RETVAL_OK;
        }
        else
            retVal = M_RETVAL_ERR;
    }

    return retVal;
}

//***************************************************************************
//    API Data Queue Funktions (Milscope)
//***************************************************************************
AI_LIB_FUNC M_RETVAL_TYPE AI_CALL_CONV M_APICMDDATAQUEUECLOSE(VI_SESSION M_BOARDID, AiUInt32 id)
{
    AiUInt32         tempBModule;
    M_RETVAL_TYPE   retVal = M_RETVAL_INI;

    if( TRUE == M_GETMODULE( M_BOARDID, &tempBModule) )
    {
        if( API_OK == (retVal=ApiCmdDataQueueClose(tempBModule, id)) )
        {
            retVal = M_RETVAL_OK;
        }
        else
            retVal = M_RETVAL_ERR;
    }

    return retVal;
}

AI_LIB_FUNC M_RETVAL_TYPE AI_CALL_CONV M_APICMDDATAQUEUECONTROL(VI_SESSION M_BOARDID, AiUInt32 id, AiUInt32 mode)
{
    AiUInt32         tempBModule;
    M_RETVAL_TYPE   retVal = M_RETVAL_INI;

    if( TRUE == M_GETMODULE( M_BOARDID, &tempBModule) )
    {
        if( API_OK == (retVal=ApiCmdDataQueueControl(tempBModule, id, mode )) )
        {
            retVal = M_RETVAL_OK;
        }
        else
            retVal = M_RETVAL_ERR;
    }

    return retVal;
}

AI_LIB_FUNC M_RETVAL_TYPE AI_CALL_CONV M_APICMDDATAQUEUEOPEN(VI_SESSION M_BOARDID, AiUInt32 id, AiUInt32 * queue_size)
{
    AiUInt32         tempBModule;
    M_RETVAL_TYPE   retVal = M_RETVAL_INI;

    if( TRUE == M_GETMODULE( M_BOARDID, &tempBModule) )
    {
        if( API_OK == (retVal=ApiCmdDataQueueOpen(tempBModule, id, queue_size)) )
        {
            retVal = M_RETVAL_OK;
        }
        else
            retVal = M_RETVAL_ERR;
    }

    return retVal;
}

AI_LIB_FUNC M_RETVAL_TYPE AI_CALL_CONV M_APICMDDATAQUEUEREAD(VI_SESSION M_BOARDID, 
                                                                                AiUInt32 id, 
                                                                                void * buffer, 
                                                                                AiUInt32 bytes_to_read,
                                                                                AiUInt32 *status, 
                                                                                AiUInt32 *bytes_transfered,
                                                                                AiUInt32 *bytes_in_queue, 
                                                                                AiUInt64 *total_bytes_transfered)
{
    AiUInt32         tempBModule;
    M_RETVAL_TYPE   retVal = M_RETVAL_INI;

    TY_API_DATA_QUEUE_READ   queue_read;
    TY_API_DATA_QUEUE_STATUS info;

    memset( &info, 0, sizeof(info) );

    /* Input Parameters */
    queue_read.id            = id;
    queue_read.buffer        = buffer;
    queue_read.bytes_to_read = bytes_to_read;

    if( TRUE == M_GETMODULE( M_BOARDID, &tempBModule) )
    {
        if( API_OK == (retVal=ApiCmdDataQueueRead(tempBModule, &queue_read, &info)) )
        {
            /* Output Parameters */
            *status                 = info.status;
            *bytes_transfered       = info.bytes_transfered; 
            *bytes_in_queue         = info.bytes_in_queue;
            *total_bytes_transfered = info.total_bytes_transfered;

            retVal = M_RETVAL_OK;
        }
        else
            retVal = M_RETVAL_ERR;
    }

    return retVal;
}

AI_LIB_FUNC M_RETVAL_TYPE AI_CALL_CONV M_APICMDSCOPESETUP(VI_SESSION M_BOARDID, AiUInt32 ul_CouplingPri, AiUInt32 ul_CouplingSec, 
                                                           AiUInt32 ul_SamplingRate, AiUInt32 ul_CaptureMode, AiUInt32 ul_OperationMode, 
                                                           AiUInt32 ul_SingleShotBuf)
{
    AiUInt32         tempBModule;
    M_RETVAL_TYPE   retVal = M_RETVAL_INI;

    TY_API_SCOPE_SETUP scpsup;

    scpsup.ul_CouplingPri = ul_CouplingPri;
    scpsup.ul_CouplingSec = ul_CouplingSec;
    scpsup.ul_SamplingRate = ul_SamplingRate;
    scpsup.ul_CaptureMode = ul_CaptureMode;
    scpsup.ul_OperationMode = ul_OperationMode;
    scpsup.ul_SingleShotBuf = ul_SingleShotBuf;

    if( TRUE == M_GETMODULE( M_BOARDID, &tempBModule) )
    {
        if( API_OK == (retVal=ApiCmdScopeSetup(tempBModule, &scpsup)) )
        {
            retVal = M_RETVAL_OK;
        }
        else
            retVal = M_RETVAL_ERR;
    }

    return retVal;
}

AI_LIB_FUNC M_RETVAL_TYPE AI_CALL_CONV M_APICMDSCOPETRIGGERDEF(VI_SESSION M_BOARDID, AiUInt32 ul_TrgMode, AiUInt32 ul_TrgSrc, AiUInt32 ul_TrgValue, 
                                                                AiUInt32 ul_TrgNbSamples, AiUInt32 ul_TrgFlags, AiUInt32 ul_TrgDelay, AiUInt32 ul_TrgTbt)
{
    AiUInt32         tempBModule;
    M_RETVAL_TYPE   retVal = M_RETVAL_INI;

    TY_API_SCOPE_TRG scptrgdef;

    scptrgdef.ul_TrgMode = ul_TrgMode;
    scptrgdef.ul_TrgSrc = ul_TrgSrc;
    scptrgdef.ul_TrgValue = ul_TrgValue;
    scptrgdef.ul_TrgNbSamples = ul_TrgNbSamples;
    scptrgdef.ul_TrgFlags = ul_TrgFlags;
    scptrgdef.ul_TrgDelay = ul_TrgDelay;
    scptrgdef.ul_TrgTbt = ul_TrgTbt;

    if( TRUE == M_GETMODULE( M_BOARDID, &tempBModule) )
    {
        if( API_OK == (retVal=ApiCmdScopeTriggerDef(tempBModule, &scptrgdef)) )
        {
            retVal = M_RETVAL_OK;
        }
        else
            retVal = M_RETVAL_ERR;
    }

    return retVal;
}

AI_LIB_FUNC M_RETVAL_TYPE AI_CALL_CONV M_APICMDSCOPECALIBRATE(VI_SESSION M_BOARDID, AiUInt8 ucMode, AiUInt32 *pul_OffsetA, AiUInt32 *pul_OffsetB,
                                                               AiUInt32 *pul_OffsetA100, AiUInt32 *pul_OffsetB100, AiUInt32 *pul_OffsetA100Sec, 
                                                               AiUInt32 *pul_OffsetB100Sec)
{
    AiUInt32         tempBModule;
    M_RETVAL_TYPE   retVal = M_RETVAL_INI;

    TY_API_SCOPE_CALIBRATION_INFO scpcalinfo;

    if( TRUE == M_GETMODULE( M_BOARDID, &tempBModule) )
    {
        if( API_OK == (retVal=ApiCmdScopeCalibrate(tempBModule, ucMode, &scpcalinfo)) )
        {
            *pul_OffsetA = scpcalinfo.lOffsetA;
            *pul_OffsetB = scpcalinfo.lOffsetB;
            *pul_OffsetA100 = scpcalinfo.lOffsetA100;
            *pul_OffsetB100 = scpcalinfo.lOffsetB100;
            *pul_OffsetA100Sec = scpcalinfo.lOffsetA100Sec;
            *pul_OffsetB100Sec = scpcalinfo.lOffsetB100Sec;

            retVal = M_RETVAL_OK;
        }
        else
            retVal = M_RETVAL_ERR;
    }

    return retVal;
}


AI_LIB_FUNC M_RETVAL_TYPE AI_CALL_CONV M_APICMDSCOPESTART(VI_SESSION M_BOARDID)
{
    AiUInt32         tempBModule;
    M_RETVAL_TYPE   retVal = M_RETVAL_INI;

    if( TRUE == M_GETMODULE( M_BOARDID, &tempBModule) )
    {
        if( API_OK == (retVal=ApiCmdScopeStart(tempBModule)) )
        {
            retVal = M_RETVAL_OK;
        }
        else
            retVal = M_RETVAL_ERR;
    }

    return retVal;
}



//***************************************************************************
//    API Calibration Commands
//***************************************************************************
AI_LIB_FUNC M_RETVAL_TYPE AI_CALL_CONV M_APICMDCALSIGCON(VI_SESSION M_BOARDID, AiUInt8 biu, AiUInt8 bus, AiUInt8 con)
{
    AiUInt32         tempBModule;
    M_RETVAL_TYPE   retVal = M_RETVAL_INI;

    if( TRUE == M_GETMODULE( M_BOARDID, &tempBModule) )
    {
        if( API_OK == (retVal=ApiCmdCalSigCon(tempBModule, biu, bus, con)) )
        {
            retVal = M_RETVAL_OK;
        }
        else
            retVal = M_RETVAL_ERR;
    }

    return retVal;
}

AI_LIB_FUNC M_RETVAL_TYPE AI_CALL_CONV M_APICMDCALCPLCON(VI_SESSION M_BOARDID, AiUInt8 biu, AiUInt8 bus, AiUInt8 cpl)
{
    AiUInt32         tempBModule;
    M_RETVAL_TYPE   retVal = M_RETVAL_INI;

    if( TRUE == M_GETMODULE( M_BOARDID, &tempBModule) )
    {
        if( API_OK == (retVal=ApiCmdCalCplCon(tempBModule, biu, bus, cpl)) )
        {
            retVal = M_RETVAL_OK;
        }
        else
            retVal = M_RETVAL_ERR;
    }

    return retVal;
}

AI_LIB_FUNC M_RETVAL_TYPE AI_CALL_CONV M_APICMDCALXMTCON(VI_SESSION M_BOARDID, AiUInt8 biu, AiUInt8 bus, AiUInt8 amp)
{
    AiUInt32         tempBModule;
    M_RETVAL_TYPE   retVal = M_RETVAL_INI;

    if (TRUE == M_GETMODULE(M_BOARDID, &tempBModule))
    {
        if (API_OK == (retVal=ApiCmdCalXmtCon(tempBModule, biu, bus, amp)))
        {
            retVal = M_RETVAL_OK;
        }
        else
            retVal = M_RETVAL_ERR;
    }

    return retVal;
}

AI_LIB_FUNC M_RETVAL_TYPE AI_CALL_CONV M_APICMDCALTRANSCON(VI_SESSION M_BOARDID, AiUInt8 biu, AiUInt8 trans_mode)
{
    AiUInt32         tempBModule;
    M_RETVAL_TYPE   retVal = M_RETVAL_INI;

    if (TRUE == M_GETMODULE(M_BOARDID, &tempBModule))
    {
        if (API_OK == (retVal=ApiCmdCalTransCon(tempBModule, biu, trans_mode)))
        {
            retVal = M_RETVAL_OK;
        }
        else
            retVal = M_RETVAL_ERR;
    }

    return retVal;
}

//***************************************************************************
//    API Replay Commands
//***************************************************************************

/*01.08.99 */
AI_LIB_FUNC M_RETVAL_TYPE AI_CALL_CONV M_APICMDREPLAYINI(VI_SESSION M_BOARDID, AiUInt8 biu, AiUInt8 cet,
                                                          AiUInt8 nct, AiUInt8 cyc, AiUInt8 nimg, AiUInt8 alt, AiUInt8 rlt,
                                                          AiUInt8 rint, AiUInt32 min, AiUInt32 msec, AiUInt8 sign,
                                                          AiUInt32 fsize)
{
    AiUInt32         tempBModule;
    M_RETVAL_TYPE   retVal = M_RETVAL_INI;

    if( TRUE == M_GETMODULE( M_BOARDID, &tempBModule) )
    {
        if( API_OK == (retVal=ApiCmdReplayIni(tempBModule, biu, cet, nct, cyc, nimg, alt, rlt, rint, min, msec, sign, fsize)) )
        {
            retVal = M_RETVAL_OK;
        }
        else
            retVal = M_RETVAL_ERR;
    }

    return retVal;
}

AI_LIB_FUNC M_RETVAL_TYPE AI_CALL_CONV M_APICMDREPLAYSTART(VI_SESSION M_BOARDID, AiUInt8 biu)
{
    AiUInt32         tempBModule;
    M_RETVAL_TYPE   retVal = M_RETVAL_INI;

    if( TRUE == M_GETMODULE( M_BOARDID, &tempBModule) )
    {
        if( API_OK == (retVal=ApiCmdReplayStart(tempBModule, biu)) )
        {
            retVal = M_RETVAL_OK;
        }
        else
            retVal = M_RETVAL_ERR;
    }

    return retVal;
}

AI_LIB_FUNC M_RETVAL_TYPE AI_CALL_CONV M_APICMDREPLAYSTOP(VI_SESSION M_BOARDID, AiUInt8 biu)
{
    AiUInt32         tempBModule;
    M_RETVAL_TYPE   retVal = M_RETVAL_INI;

    if( TRUE == M_GETMODULE( M_BOARDID, &tempBModule) )
    {
        if( API_OK == (retVal=ApiCmdReplayStop(tempBModule, biu)) )
        {
            retVal = M_RETVAL_OK;
        }
        else
            retVal = M_RETVAL_ERR;
    }

    return retVal;
}

AI_LIB_FUNC M_RETVAL_TYPE AI_CALL_CONV M_APICMDREPLAYSTATUS(VI_SESSION M_BOARDID, AiUInt8 biu,
                                                             AiUInt8*  status,
                                                             AiUInt8*  padding1,
                                                             AiUInt16* padding2,
                                                             AiUInt32* rpi_cnt,
                                                             AiUInt32* saddr,
                                                             AiUInt32* size,
                                                             AiUInt32* entry_cnt) // added 16.01.01 pg
{
    AiUInt32         tempBModule;
    M_RETVAL_TYPE   retVal = M_RETVAL_INI;

    TY_API_REP_STATUS prep;

    if( TRUE == M_GETMODULE( M_BOARDID, &tempBModule) )
    {
        if( API_OK == (retVal=ApiCmdReplayStatus(tempBModule, biu, &prep)) )
        {
            *status = prep.status;
            *padding1 = prep.padding1;
            *padding2 = prep.padding2;
            *rpi_cnt = prep.rpi_cnt;
            *saddr = prep.saddr;
            *size = prep.size;
            *entry_cnt = prep.entry_cnt;

            retVal = M_RETVAL_OK;
        }
        else
            retVal = M_RETVAL_ERR;
    }

    return retVal;
}

AI_LIB_FUNC M_RETVAL_TYPE AI_CALL_CONV M_APICMDREPLAYRT(VI_SESSION M_BOARDID, AiUInt8 biu, AiUInt8 con, AiUInt8 mode, AiUInt8 rt)
{
    AiUInt32         tempBModule;
    M_RETVAL_TYPE   retVal = M_RETVAL_INI;

    if( TRUE == M_GETMODULE( M_BOARDID, &tempBModule) )
    {
        if( API_OK == (retVal=ApiCmdReplayRT(tempBModule, biu, con, mode, rt)) )
        {
            retVal = M_RETVAL_OK;
        }
        else
            retVal = M_RETVAL_ERR;
    }

    return retVal;
}

//###############################################################
//    GenIo Functions
//###############################################################
AI_LIB_FUNC AiReturn AI_CALL_CONV ApiCmdGenIoAddrInitCvi(VI_SESSION M_BOARDID,  AiUInt8 genio_type, 
                                                         AiUInt8 io_type, AiUInt8 chn, AiUInt8 res, AiUInt32 addr)
{
    AiUInt32         tempBModule;
    M_RETVAL_TYPE   retVal = M_RETVAL_INI;

    if( TRUE == M_GETMODULE( M_BOARDID, &tempBModule) )
    {
        if( API_OK == (retVal=ApiCmdGenIoAddrInit(tempBModule, genio_type, io_type, chn, res, addr)) )
        {
            retVal = M_RETVAL_OK;
        }
        else
            retVal = M_RETVAL_ERR;
    }

    return retVal;
}

AI_LIB_FUNC AiReturn AI_CALL_CONV ApiCmdGenIoOutChnWriteCvi(VI_SESSION M_BOARDID, AiUInt8 genio_type, 
                                                            AiUInt8 chn, AiUInt32 res, AiUInt32 val)
{
    AiUInt32         tempBModule;
    M_RETVAL_TYPE   retVal = M_RETVAL_INI;

    if( TRUE == M_GETMODULE( M_BOARDID, &tempBModule) )
    {
        if( API_OK == (retVal=ApiCmdGenIoOutChnWrite(tempBModule, genio_type, chn, res, val)) )
        {
            retVal = M_RETVAL_OK;
        }
        else
            retVal = M_RETVAL_ERR;
    }

    return retVal;
}

AI_LIB_FUNC AiReturn AI_CALL_CONV ApiCmdGenIoOutChnReadCvi(VI_SESSION M_BOARDID, AiUInt8 genio_type, 
                                                           AiUInt8 chn, AiUInt32 ctl, AiUInt32 *val)
{
    AiUInt32         tempBModule;
    M_RETVAL_TYPE   retVal = M_RETVAL_INI;

    if( TRUE == M_GETMODULE( M_BOARDID, &tempBModule) )
    {
        if( API_OK == (retVal=ApiCmdGenIoOutChnRead(tempBModule, genio_type, chn, ctl, val)) )
        {
            retVal = M_RETVAL_OK;
        }
        else
            retVal = M_RETVAL_ERR;
    }

    return retVal;
}

AI_LIB_FUNC AiReturn AI_CALL_CONV ApiCmdGenIoInChnReadCvi(VI_SESSION M_BOARDID, AiUInt8 genio_type, 
                                                          AiUInt8 chn, AiUInt32 ctl, AiUInt32 *val)
{
    AiUInt32         tempBModule;
    M_RETVAL_TYPE   retVal = M_RETVAL_INI;

    if( TRUE == M_GETMODULE( M_BOARDID, &tempBModule) )
    {
        if( API_OK == (retVal=ApiCmdGenIoInChnRead(tempBModule, genio_type, chn, ctl, val)) )
        {
            retVal = M_RETVAL_OK;
        }
        else
            retVal = M_RETVAL_ERR;
    }

    return retVal;
}

AI_LIB_FUNC AiReturn AI_CALL_CONV ApiCmdGenIoSysSTaskCtrlCvi(VI_SESSION M_BOARDID, AiUInt8 genio_type, 
                                                             AiUInt8 con, AiUInt32 rate, AiUInt32 rsync, AiUInt32 ustep)
{
    AiUInt32         tempBModule;
    M_RETVAL_TYPE   retVal = M_RETVAL_INI;

    if( TRUE == M_GETMODULE( M_BOARDID, &tempBModule) )
    {
        if( API_OK == (retVal=ApiCmdGenIoSysSTaskCtrl(tempBModule, genio_type, con, rate, rsync, ustep)) )
        {
            retVal = M_RETVAL_OK;
        }
        else
            retVal = M_RETVAL_ERR;
    }

    return retVal;
}



//***************************************************************************
//    Interrupt Handler Functions
//***************************************************************************
#if defined (_AIM_VXI) && ! defined (_AIM_LVRT)                                           //AXI-SECTION
/*******************************************************************************
*  Function : Ai1553AxiInstIntHandler                 Author : Thomas Jahn
*------------------------------------------------------------------------------
*  Create : 24.05.00      last Update : 24.05.00      Status: [i][t][ ]
*------------------------------------------------------------------------------
*  Descriptions
*  ------------
*     Text
*
*  Inputs  : -
*
*  Outputs : -
*
*  Return  :
*
******************************************************************************/
AI_LIB_FUNC M_RETVAL_TYPE AI_CALL_CONV M_AXI_INSTINTHANDLER(AiSession instrumentHandle,
                                                             AiUInt8 biu,
                                                             AiUInt8 uc_Type,
                                                             AiUInt8 level,
                                                             Ai1553AxiHndlr Ai1553AxiIntFunc)
{
    AiUInt32 bModule/*,index*/;
    AiInt32 retVal = _VI_ERROR;

    if (GetModuleNo (instrumentHandle, &bModule) == TRUE) {
        /* Copy Parameters to a Structure if necessary */
        if (_Win32AxiInstIntHandler(bModule, biu, uc_Type, level, Ai1553AxiIntFunc) == API_OK){
            /* Copy Parameters from Structure if necessary */
            retVal = VI_SUCCESS;
        }
    }
    return(retVal);
}
/*******************************************************************************
*  Function :                             Author : Thomas Jahn
*------------------------------------------------------------------------------
*  Create : 03.03.00      last Update : 03.03.00      Status: [i][t][ ]
*------------------------------------------------------------------------------
*  Descriptions
*  ------------
*     Text
*
*  Inputs  : -
*
*  Outputs : -
*
*  Return  :
*
******************************************************************************/
AI_LIB_FUNC M_RETVAL_TYPE AI_CALL_CONV M_AXI_DELINTHANDLER(AiSession instrumentHandle,
                                                            AiUInt8 biu,
                                                            AiUInt8 uc_Type
                                                            )
{
    AiUInt32 bModule/*,index*/;
    AiInt32 retVal = _VI_ERROR;

    if (GetModuleNo (instrumentHandle, &bModule) == TRUE) {
        /* Copy Parameters to a Structure if necessary */
        if (_Win32AxiDelIntHandler(bModule, biu, uc_Type) == API_OK){
            /* Copy Parameters from Structure if necessary */
            retVal = VI_SUCCESS;
        }
    }
    return(retVal);
}

#elif defined (_AIM_WINDOWS)                                          //AVI-SECTION
//***************************************************************************
//    API Driver Functions
//***************************************************************************

//***************************************************************************
// The following functions are to establish a callback to the
// LabView interface and are only available for Windows
//***************************************************************************
// The following support is only valid on 32bit LabVIEW systems
// because we do not support LabVIEW 64bit yet
#if defined(_AIM_WINDOWS)
#if !defined(_WIN64)

LVUserEventRef* GlobalRef = NULL;

static void  AI_CALL_CONV __mil_internal_lv_callback(AiUInt32 ulModHandle, AiUInt8 ucBiu, AiUInt8 ucType, TY_API_INTR_LOGLIST_ENTRY * x_Info)
{
    typedef struct {
        AiUInt32 board_handle;
        AiUInt32 biu;
        AiUInt32 type;
        AiUInt32 lla;
        AiUInt32 llb;
        AiUInt32 llc;
        AiUInt32 lld;
    } int_info;

    int_info x_IntInfo ={ 0 };

    x_IntInfo.board_handle = ulModHandle;
    x_IntInfo.biu = ucBiu;
    x_IntInfo.type = ucType;
    x_IntInfo.lla = x_Info->ul_Lla;
    x_IntInfo.llb = x_Info->ul_Llb;
    x_IntInfo.llc = x_Info->x_Llc.ul_All;
    x_IntInfo.lld = x_Info->x_Lld.ul_All;
    if (GlobalRef)
        PostLVUserEvent(*GlobalRef, &x_IntInfo);
}


AI_LIB_FUNC AiReturn AI_CALL_CONV ApiInstIntHandlerLV(AiUInt32 ulModHandle, AiUInt8 biu, AiUInt8 uc_Type, LVUserEventRef* ref)
{
    if (!ref)
        return API_ERR_PARAM4_IS_NULL;

    GlobalRef = ref;
    return ApiInstIntHandler(ulModHandle, biu, uc_Type, __mil_internal_lv_callback);
}

AI_LIB_FUNC AiReturn AI_CALL_CONV ApiDelIntHandlerLV(AiUInt32 ulModHandle, AiUInt8 biu, AiUInt8 uc_Type)
{
    AiReturn ret = ApiDelIntHandler(ulModHandle, biu, uc_Type);
    GlobalRef = NULL;

    return ret;
}
#else /* !defined(_WIN64) */
AI_LIB_FUNC AiReturn AI_CALL_CONV ApiInstIntHandlerLV(AiUInt32 ulModHandle, AiUInt8 biu, AiUInt8 uc_Type, LVUserEventRef* ref)
{
    return API_ERR_FUNCTION_NOT_SUPPORTED;
}

AI_LIB_FUNC AiReturn AI_CALL_CONV ApiDelIntHandlerLV(AiUInt32 ulModHandle, AiUInt8 biu, AiUInt8 uc_Type)
{
    return API_ERR_FUNCTION_NOT_SUPPORTED;
}

#endif
#endif



AI_LIB_FUNC AiReturn AI_CALL_CONV ApiInstIntHandlerCvi(AiUInt32 bModule, AiUInt8 biu, AiUInt8 uc_Type, TY_INT_FUNC_PTR pf_IntFunc)
{
    return ApiInstIntHandler(bModule, biu, uc_Type, pf_IntFunc);
}

AI_LIB_FUNC AiReturn AI_CALL_CONV ApiDelIntHandlerCvi(AiUInt32 bModule, AiUInt8 biu, AiUInt8 uc_Type)
{
    return ApiDelIntHandler(bModule, biu, uc_Type);
}

AI_LIB_FUNC AiReturn AI_CALL_CONV ApiConnectToServerCvi( AiUChar * pszNetworkAdress, AiInt16 *w_ServerBoards)
{
    return ApiConnectToServer( pszNetworkAdress, w_ServerBoards);
}

AI_LIB_FUNC AiReturn AI_CALL_CONV ApiDisconnectFromServerCvi( AiUChar * pszNetworkAdress )
{
    return ApiDisconnectFromServer( pszNetworkAdress);
}



#elif defined _AIM_LVRT
AI_LIB_FUNC AiStatus AI_CALL_CONV ApiWaitForInterruptsCvi(AiUInt32 bModule, AiUInt32 ul_Timeout, 
                                                           AiInt32* pl_NumInterrupts, 
                                                           AiUInt32 ul_Lla[MAX_IR_EVENT_BUF],
                                                           AiUInt32 ul_Llb[MAX_IR_EVENT_BUF],
                                                           AiUInt32 ul_Llc[MAX_IR_EVENT_BUF],
                                                           AiUInt32 ul_Lld[MAX_IR_EVENT_BUF])
{
    AiStatus uw_RetVal = API_OK;

    if( (bModule & API_MODULE_MASK) >= MAX_API_MODULE )
        uw_RetVal = API_ERR_WRONG_MODULE;
    else if(pl_NumInterrupts == NULL)
        uw_RetVal = API_ERR_PARAM3_IS_NULL;
    else if(ul_Lla == NULL)
        uw_RetVal = API_ERR_PARAM4_IS_NULL;
    else if(ul_Llb == NULL)
        uw_RetVal = API_ERR_PARAM5_IS_NULL;
    else if(ul_Llc == NULL)
        uw_RetVal = API_ERR_PARAM6_IS_NULL;
    else if(ul_Lld == NULL)
        uw_RetVal = API_ERR_PARAM7_IS_NULL;

    if(uw_RetVal == API_OK)
    {
        uw_RetVal = _LvRTWaitForInterrupts(bModule & API_MODULE_MASK, ul_Timeout, pl_NumInterrupts, ul_Lla, ul_Llb, ul_Llc, ul_Lld);
    }

    return uw_RetVal;
}

#endif

#ifdef _AIM_LINUX
#pragma GCC diagnostic pop
#endif
