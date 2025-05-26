/* SPDX-FileCopyrightText: 2009-2021 AIM GmbH <info@aim-online.com> */
/* SPDX-License-Identifier: MIT  */

/*!
Mil_Wrap.cpp
Contains common interface functions for the SmartWrp library.
*/


#include <stdlib.h>
#include <string.h>
#ifdef _WIN32
#include <rpc.h> /* was necessary because GUID structure is used in Ai1553i_fnc.h and this structure wasn't able to be resolved */
#endif
#include "AiOs.h"
#include "Api3910.h"
#include "Mil_Wrap.h"
#include "Wrapper.h"
#include "WrapList.h"

#ifndef false
  #define false 0
#endif
#ifndef true
  #define true 1
#endif

/****************************************************/
/* DLL functions                                    */
/****************************************************/
/*!
Connects to an AIM ANS server.

@param[in]  ac_SrvName   Server name or ip address of the AIM ANS server which shall be connected.\n
                         This parameter should be set to "local", if current PC is to be connected.

@return 0 (API_OK)  Successfully connected to server\n
        1 (API_ERR) ANS server could not be connected

@remarks n/a

@see n/a

@author Patrick Giesel @date 07.10.2010
*/
_API_EXTERN AiReturn _API_DLL_FUNC SlWrapConnectToServer(AiChar *ac_SrvName)
{
  AiReturn retVal      = 0;
  AiInt16  w_ServerBoards = 0;

  if( strcmp( (const char*)ac_SrvName, "local") != 0 )
      retVal = ApiConnectToServer( (AiUChar *)ac_SrvName, &w_ServerBoards );
  else
      w_ServerBoards = ApiInit();

  return w_ServerBoards;
}

/*!
Disconnects from an AIM ANS server.

@param[in]  ac_SrvName   Server name or ip address of the AIM ANS server which shall be disconnected.\n
                         This parameter should be set to "local", if current PC is to be disconnected.

@return 0 (API_OK)  Successfully disconnected from server\n
        1 (API_ERR) ANS server could not be disconnected

@remarks n/a

@see n/a

@author Patrick Giesel @date 07.10.2010
*/
_API_EXTERN AiReturn _API_DLL_FUNC SlWrapDisconnectFromServer(AiChar *ac_SrvName)
{
  AiReturn retVal = 0;

  if( strcmp((const char*)ac_SrvName, "local") != 0 )
    retVal = ApiDisconnectFromServer( (AiUChar *)ac_SrvName );

  return retVal;
}

/*!
Opens a new board communication session.

@param[in]  ul_ModuleId        Module ID of the board that is to be opened\n
                               API_MODULE_1 (0) = first board\n
                               API_MODULE_2 (1) = second board\n
                               ...\n
                               API_MODULE_32 (31) = thirty second board\n
@param[in]  ul_StreamId        Stream ID of the stream on the board that is to be opened:\n
                               API_STREAM_1 (1) = first stream\n
                               API_STREAM_2 (2) = second stream\n
                               ...\n
                               API_STREAM_8 (8) = eights stream\n
@param[in]  ac_SrvName         Server name or ip address of the AIM ANS server which shall be connected.\n
                               This parameter should be set to "local", if current PC is to be connected.
@param[out] pul_ModuleHandle   Board Module Handle. This has to be used as input parameter on any function used
                               to communicate with the board.

@return 0 (API_OK)  Successfully opened the board and stream communication on the given ANS server\n
        1 (API_ERR) Board and stream communication could not established

@remarks n/a

@see SlWrapCloseModule()

@author Patrick Giesel @date 07.10.2010
*/
_API_EXTERN AiReturn _API_DLL_FUNC SlWrapOpenModule(AiUInt32 ul_ModuleId, AiUInt32 ul_StreamId,
                                                AiChar *ac_SrvName, AiUInt32 *pul_ModuleHandle)
{
  AiReturn retVal = API_ERR;
  TY_API_OPEN x_Open;
  TY_DLL_MODULE_INFO_LIST *px_DLLModuleInfo;

  /* Disable message box because Jenkins hangs and message box is invisible. */
  ApiSetDllDbgLevel(DBG_ERROR | DBG_PARAMCHK);

  x_Open.ul_Module = ul_ModuleId;
  x_Open.ul_Stream = ul_StreamId;
  strcpy( x_Open.ac_SrvName, (char*)ac_SrvName );
  retVal = ApiOpenEx( &x_Open, pul_ModuleHandle );
  if( API_OK == retVal )
  {
    if( AiFalse == uc_IsAlreadyOpened( *pul_ModuleHandle, &px_DLLModuleInfo ) )
      retVal = uc_AddDLLModuleInfoListEntry( *pul_ModuleHandle );

    retVal = uc_GetDLLModuleInfoListEntry( *pul_ModuleHandle, &px_DLLModuleInfo );
    if( API_OK == retVal )
      retVal = SlWrapReadModuleInfo( *pul_ModuleHandle, &px_DLLModuleInfo->x_ModuleInfo );
  }
  return retVal;
}

/*!
Closes a previosly established board communication session.

@param[in]  ul_ModuleHandle   Board Module Handle that is returned
                              by the protocol specific AIM API!

@return 0 (false) Board communcation session has been closed successfully.\n
        1 (true)  Board communication session could not be closed.

@remarks n/a

@see SlWrapOpenModule()

@author Patrick Giesel @date 07.10.2010
*/
_API_EXTERN AiReturn _API_DLL_FUNC SlWrapCloseModule(AiUInt32 ul_ModuleHandle)
{
  AiUInt16 retVal = API_OK;
  TY_DLL_MODULE_INFO_LIST *px_DLLModuleInfo;

  if( AiTrue == uc_IsAlreadyOpened( ul_ModuleHandle, &px_DLLModuleInfo ) )
    retVal = uc_RemoveDLLModuleInfoListEntry( ul_ModuleHandle );
  if( API_OK == retVal )
    retVal = ApiClose( ul_ModuleHandle );
  return retVal;
}

/*!
Examines if the board software is able to read a detailed board information.

@param[in]  ul_ModuleHandle   Board Module Handle that is returned
by the protocol specific AIM API!

@return 0 (false) Functionality is not available with the current board software\n
1 (true)  Functionality is available with the current board software

@remarks n/a

@see n/a

@author Patrick Giesel @date 07.10.2010
*/
_API_EXTERN AiUInt8 _API_DLL_FUNC uc_CanApiBoardInfo(AiUInt32 ul_ModuleHandle)
{
    AiUInt32 ul_RetVal;
    TY_DLL_MODULE_INFO_LIST *px_DLLModuleInfo;
    AiUInt32 ul_Version;

    ul_RetVal = uc_GetDLLModuleInfoListEntry(ul_ModuleHandle, &px_DLLModuleInfo);

    if (API_OK == ul_RetVal)
    {
        ul_Version = (px_DLLModuleInfo->x_ModuleInfo.version.target_sw.ul_MajorVer * 100) +
            px_DLLModuleInfo->x_ModuleInfo.version.target_sw.ul_MinorVer;
        if (ul_Version >= FIRST_TSW_VER_BOARDINFO)
            return AiTrue;
    }

    return AiFalse;
}


/*!
Executes an undocumented API function to set some parameters on the board

@param[in]   ul_ModuleHandle   Board Module Handle that is returned
by the protocol specific AIM API!
@param [in]       uc_InCnt     Amount of input array element given with parameter aul_InLongs
@param [out]      puc_OutCnt   Amount of output array element returned by parameter aul_InLongs
@param [in]       aul_InLongs  Input array of long values
@param [out]      aul_OutLongs Output array of long values

@return 0 (API_OK)  Succesful\n
1 (API_ERR) Function executed with error

@remarks n/a

@see n/a

@author Patrick Giesel @date 25.10.2010
*/
_API_EXTERN AiReturn _API_DLL_FUNC ul_AiUtil(AiUInt32 ul_ModuleHandle, AiUInt8 uc_InCnt, AiUInt8 *puc_OutCnt,
    AiUInt32 aul_InLongs[64], AiUInt32 aul_OutLongs[64])
{
    AiUInt16 uw_RetVal = API_ERR;
    AiUInt32 ulTswVersion = 0;
    TY_DLL_MODULE_INFO_LIST *px_DLLModuleInfo;

    if (AiFalse == uc_IsAlreadyOpened(ul_ModuleHandle, &px_DLLModuleInfo))
        return uw_RetVal;

    if (aul_InLongs[0] == 0x07)	/* Finalize */
    {
        ulTswVersion = (px_DLLModuleInfo->x_ModuleInfo.version.target_sw.ul_MajorVer * 100) +
            px_DLLModuleInfo->x_ModuleInfo.version.target_sw.ul_MinorVer;
        if (ulTswVersion <= VERSION_NOVRAM_FINALIZE_SUPPORTED)
        {
            return API_OK;
        }
    }


    uw_RetVal = ApiCmdUtility(ul_ModuleHandle, uc_InCnt, puc_OutCnt, aul_InLongs, aul_OutLongs);

    return uw_RetVal;
}

/*!
Collects all available information of a board.

@param[in]  ul_ModuleHandle   Board Module Handle that is returned
                              by the protocol specific AIM API!
@param [in, out]  px_ModuleInfo   Struct to return the information about the requested board.

@return 0 (API_OK)  Succesful\n
        1 (API_ERR) Function executed with error

@remarks n/a

@see n/a

@author Patrick Giesel @date 25.10.2010
*/
_API_EXTERN AiReturn _API_DLL_FUNC SlWrapReadModuleInfo(AiUInt32 ul_ModuleHandle, TY_MODULE_INFO *px_ModuleInfo)
{
  AiReturn            retVal = API_ERR;
  TY_DLL_MODULE_INFO_LIST *px_DLLModuleInfo;
  TY_API_VERSION_INFO *px_VersionTemp;
  TY_API_BOARD_INFO   *px_BoardInfoTemp;
  TY_API_INI_INFO     *px_IniInfoTemp;
  TY_API_DRIVER_INFO  *px_DriverInfo;
  AiChar              *pszDeviceName;
  TY_VER_INFO         version;

  px_VersionTemp   = (TY_API_VERSION_INFO *)malloc(sizeof(TY_API_VERSION_INFO));
  px_BoardInfoTemp = (TY_API_BOARD_INFO   *)malloc(sizeof(TY_API_BOARD_INFO)  );
  px_IniInfoTemp   = (TY_API_INI_INFO     *)malloc(sizeof(TY_API_INI_INFO)    );
  px_DriverInfo    = (TY_API_DRIVER_INFO  *)malloc(sizeof(TY_API_DRIVER_INFO) );

  memset( px_VersionTemp,   0, sizeof(TY_API_VERSION_INFO) );
  memset( px_BoardInfoTemp, 0, sizeof(TY_API_BOARD_INFO)   );
  memset( px_IniInfoTemp,   0, sizeof(TY_API_INI_INFO)     );
  memset( px_DriverInfo,    0, sizeof(TY_API_DRIVER_INFO)  );

  if( AiFalse == uc_IsAlreadyOpened( ul_ModuleHandle, &px_DLLModuleInfo ) )
    return retVal;


  retVal = ApiReadVersion(ul_ModuleHandle, AI_FIRMWARE_VER_BIU1, &version);
  if (retVal == API_OK)
  {
      memcpy(&px_ModuleInfo->version.firmware[0], &version, sizeof(version));
  }

  retVal = ApiReadVersion(ul_ModuleHandle, AI_FIRMWARE_VER_BIU2, &version);
  if (retVal == API_OK)
  {
      memcpy(&px_ModuleInfo->version.firmware[1], &version, sizeof(version));
  }

  retVal = ApiReadVersion(ul_ModuleHandle, AI_FIRMWARE_VER_BIU3, &version);
  if (retVal == API_OK)
  {
      memcpy(&px_ModuleInfo->version.firmware[2], &version, sizeof(version));
  }

  retVal = ApiReadVersion(ul_ModuleHandle, AI_FIRMWARE_VER_BIU4, &version);
  if (retVal == API_OK)
  {
      memcpy(&px_ModuleInfo->version.firmware[3], &version, sizeof(version));
  }

  retVal = ApiReadVersion(ul_ModuleHandle, AI_IO_LCA_VER_BIU1, &version);
  if (retVal == API_OK)
  {
      memcpy(&px_ModuleInfo->version.io_lca[0], &version, sizeof(version));
  }

  retVal = ApiReadVersion(ul_ModuleHandle, AI_IO_LCA_VER_BIU2, &version);
  if (retVal == API_OK)
  {
      memcpy(&px_ModuleInfo->version.io_lca[1], &version, sizeof(version));
  }

  retVal = ApiReadVersion(ul_ModuleHandle, AI_IO_LCA_VER_BIU3, &version);
  if (retVal == API_OK)
  {
      memcpy(&px_ModuleInfo->version.io_lca[2], &version, sizeof(version));
  }

  retVal = ApiReadVersion(ul_ModuleHandle, AI_IO_LCA_VER_BIU4, &version);
  if (retVal == API_OK)
  {
      memcpy(&px_ModuleInfo->version.io_lca[3], &version, sizeof(version));
  }


  retVal = ApiReadVersion(ul_ModuleHandle, AI_MAIN_LCA_VER, &version);
  if (retVal == API_OK)
  {
      memcpy(&px_ModuleInfo->version.soc, &version, sizeof(version));
  }


  retVal = ApiReadVersion(ul_ModuleHandle, AI_TCP_VER, &version);
  if (retVal == API_OK)
  {
      memcpy(&px_ModuleInfo->version.tcp, &version, sizeof(version));
  }


  retVal = ApiReadVersion(ul_ModuleHandle, AI_MONITOR_VER, &version);
  if (retVal == API_OK)
  {
      memcpy(&px_ModuleInfo->version.monitor, &version, sizeof(version));
  }


  retVal = ApiReadVersion(ul_ModuleHandle, AI_TARGET_VER, &version);
  if (retVal == API_OK)
  {
      memcpy(&px_ModuleInfo->version.target_sw, &version, sizeof(version));
  }





    if( uc_CanApiBoardInfo(ul_ModuleHandle) )
      retVal = ApiGetBoardInfo(ul_ModuleHandle, px_BoardInfoTemp);
    else
    {
      AiUInt32 aul_InLongs[64];
      AiUInt32 aul_OutLongs[64];
      AiUInt8  uc_OutCount = 0;

      memset(aul_InLongs,0,sizeof(aul_InLongs));
      memset(aul_OutLongs,0,sizeof(aul_OutLongs));
      aul_InLongs[0] = 0x01;          /* Read Board Type Command */
      aul_InLongs[1] = NVR_BOARDTYPE; /* Novram address of board type */
      ul_AiUtil( ul_ModuleHandle, 2, &uc_OutCount, aul_InLongs , aul_OutLongs );
      px_BoardInfoTemp->ul_NovRamBoardType         = aul_OutLongs[0];

      memset(aul_InLongs,0,sizeof(aul_InLongs));
      memset(aul_OutLongs,0,sizeof(aul_OutLongs));
      aul_InLongs[0] = 0x01;            /* Read Board Config Command */
      aul_InLongs[1] = NVR_BOARDCONFIG; /* Novram address of board config */
      ul_AiUtil( ul_ModuleHandle, 2, &uc_OutCount, aul_InLongs , aul_OutLongs );
      px_BoardInfoTemp->ul_NovRamBoardConfig       = aul_OutLongs[0];


      memset(aul_InLongs,0,sizeof(aul_InLongs));
      memset(aul_OutLongs,0,sizeof(aul_OutLongs));
      aul_InLongs[0] = 0x01;            /* Read Serial Number Command */
      aul_InLongs[1] = NVR_SERIALNO;    /* Novram address of Serial Number */
      ul_AiUtil( ul_ModuleHandle, 2, &uc_OutCount, aul_InLongs , aul_OutLongs );
      px_BoardInfoTemp->ul_SerialNumber            = aul_OutLongs[0];
    }

  if( API_OK == retVal )
  {
    px_ModuleInfo->nvr.boardConfig       = px_BoardInfoTemp->ul_NovRamBoardConfig;
    px_ModuleInfo->nvr.boardType         = px_BoardInfoTemp->ul_NovRamBoardType;
    px_ModuleInfo->ul_SerialNumber                   = px_BoardInfoTemp->ul_SerialNumber;

    retVal = ApiCmdIni(ul_ModuleHandle, API_INIT_MODE_READ, px_IniInfoTemp);
    if( API_OK == retVal )
    {
      px_ModuleInfo->ul_Prot           = px_IniInfoTemp->prot;
    }

    /* in case of not working ApiGetBoardType() and not working AiUtil() we have to use the value read out by ApiCmdIni() */
    if (0 == px_ModuleInfo->nvr.boardConfig)
        px_ModuleInfo->nvr.boardConfig = px_IniInfoTemp->board_config;
    if (0 == px_ModuleInfo->nvr.boardType)
        px_ModuleInfo->nvr.boardType = px_IniInfoTemp->board_type;











    pszDeviceName = (char *) ApiGetBoardName( ul_ModuleHandle );

    strncpy( px_ModuleInfo->ac_BoardName, pszDeviceName, sizeof(px_ModuleInfo->ac_BoardName) );

    px_ModuleInfo->ul_ModuleHandle = ul_ModuleHandle;
  }


  free( px_VersionTemp   );
  free( px_BoardInfoTemp );
  free( px_IniInfoTemp   );
  free( px_DriverInfo    );

  return retVal;
}


void changeS0toS3line( AiUInt8 *S0Line )
{
    /* The first two chars of the SREC line are not calculated into the 
     * checksum, so we can change it as we like */
    S0Line[1] = '3';
}


/*!
Loads a SREC file on the board.

@param[in]   ul_ModuleHandle   Board Module Handle that is returned
                               by the protocol specific AIM API!
@param [in]  uc_Mode           0 = Initialize Download Function\n
                               1 = Download S-Record characters
@param [in]  uc_Cnt            Amount of characters to download. This parameter has no effect if "uc_Mode" is set to 0!
@param [in]  ul_Offset         Add offset value to S-Record address. This parameter has no effect if "uc_Mode" is set to 0!
@param [in]  auc_SRec          Array of characters / Line from S-Record file
@param [out] puc_St            Returns the checksum status:\n
                               0 = Checksum was ok\n
                               1 = No S-Record string\n
                               2 = Wrong S-Record type\n
                               3 = Checksum error\n
                               4 = Addressing error
@param [out] pul_FSize         Returns the amount of downloaded data bytes.

@return 0 (API_OK)  Succesful\n
        1 (API_ERR) Function executed with error
*/
_API_EXTERN AiReturn _API_DLL_FUNC SlWrapLoadSREC(AiUInt32 ul_ModuleHandle, AiUInt8 uc_Mode, AiUInt8 uc_Cnt,
                                               AiUInt32 ul_Offset, AiUInt8 *auc_SRec, AiUInt8 *puc_St,
                                               AiUInt32 *pul_FSize )
{
    AiUInt16 uw_RetVal = API_ERR;
    TY_DLL_MODULE_INFO_LIST *px_DLLModuleInfo;
    AiUInt32 ulTswVersion = 0;

    uw_RetVal = uc_GetDLLModuleInfoListEntry(ul_ModuleHandle, &px_DLLModuleInfo);
    if( API_OK != uw_RetVal )
        return uw_RetVal;


    /* TSW does not support S0 lines, if version is too old  */
    ulTswVersion = (px_DLLModuleInfo->x_ModuleInfo.version.target_sw.ul_MajorVer * 100) +
                    px_DLLModuleInfo->x_ModuleInfo.version.target_sw.ul_MinorVer;
    if( ulTswVersion <= VERSION_LOADSREC_S0SUPPORTED )
        if( (auc_SRec[0] == 'S') && (auc_SRec[1] == '0') )
            changeS0toS3line(auc_SRec);
    

    uw_RetVal = ApiCmdLoadSRec( ul_ModuleHandle, uc_Mode, uc_Cnt, ul_Offset, auc_SRec, puc_St, pul_FSize );
    
    return uw_RetVal;
}

/*!
Flashes a previously loaded SREC file into the board's flash memory.

@param[in]   ul_ModuleHandle   Board Module Handle that is returned
                               by the protocol specific AIM API!
@param [in]  ul_SrcAddr        Source RAM address to flash from
@param [in]  ul_Sector         Sector of the Flash-PROM which shall be programmed
@param [in]  ul_Size           Number of bytes to be programmed
@param [out] puc_St            Returns the flash programming status:\n
                               0 = successful programmed\n
                               1 = flash erase error\n
                               2 = flash programming error

@return 0 (API_OK)  Succesful\n
        1 (API_ERR) Function executed with error
*/
_API_EXTERN AiReturn _API_DLL_FUNC SlWrapProgFlash(AiUInt32 ul_ModuleHandle, AiUInt32 ul_SrcAddr,
                                                AiUInt32 ul_Sector, AiUInt32 ul_Size, AiUInt8 *puc_St )
{
  return ApiCmdProgFlash( ul_ModuleHandle, ul_SrcAddr, ul_Sector, ul_Size, puc_St );
}



/**********************************************************************************************************
***                                        E-Smart udpate                                               ***
***********************************************************************************************************/


static AiBoolean INLINE(is_update_in_progress(TY_API_E_UPDATE_STATUS update_status))
{
	switch (update_status)
	{
	case API_UPDATE_DEV_CON_FAILURE:
	case API_UPDATE_FAILED:
	case API_UPDATE_FINISHED:
	case API_UPDATE_IDLE:
		return FALSE;
	case API_UPDATE_IN_PROGRESS:
	case API_UPDATE_START_REQUESTED:
		return TRUE;
	}

	return FALSE;
}

static AiReturn INLINE(is_update_error(TY_API_E_UPDATE_STATUS update_status))
{
	switch (update_status)
	{
	case API_UPDATE_DEV_CON_FAILURE:
	case API_UPDATE_FAILED:
		return 1;
	case API_UPDATE_FINISHED:
	case API_UPDATE_IDLE:
	case API_UPDATE_IN_PROGRESS:
	case API_UPDATE_START_REQUESTED:
		return 0;
	}

	return 1;
}

/*!
Downloads the given file and updates it by using the on-board esmart.

@param[in]   ul_ModuleHandle   Board Module Handle that is returned
by the protocol specific AIM API!
@param [in]  file              The esmart upate package

@return 0 (API_OK)  Succesful\n
1 (API_ERR) Function executed with error

@remarks n/a
*/
_API_EXTERN AiReturn _API_DLL_FUNC SlWrapUpdatePackageStart( AiUInt32 module, const char* file)
{
	AiUInt16 ret = 0;
	TY_API_PACKET_UPDATE_STATUS_OUT UpdateStatus;

	/* -- Waiting for board being ready for update... --- */
	do
	{
		ret = ApiUpdateStatus(module, &UpdateStatus);

		if (ret != 0)
			return ret;

		if (UpdateStatus.e_Status == API_UPDATE_DEV_CON_FAILURE)
			return 1;

		AiOsSleep(100);
	} while (is_update_in_progress(UpdateStatus.e_Status));



	/* -- Ready / Download File --- */

	ret = ApiFileDownload(module, file);

	if (ret != 0)
		return ret;

	/* -- Start the update --- */
	return ApiUpdateStart(module, file);
}


_API_EXTERN AiReturn _API_DLL_FUNC SlWrapUpdatePackageStatus( AiUInt32 module, AiBoolean * in_progress, int * components, int * total)
{
	AiUInt32 ret = 0;
	TY_API_PACKET_UPDATE_STATUS_OUT UpdateStatus;

	ret = ApiUpdateStatus(module, &UpdateStatus);

	*in_progress = is_update_in_progress(UpdateStatus.e_Status);
	*components = UpdateStatus.ul_ComponentsDone;
	*total = UpdateStatus.ul_TotalComponents;

	if (ret != 0)
		return ret;

	return is_update_error(UpdateStatus.e_Status);
}


_API_EXTERN AiReturn _API_DLL_FUNC SlWrapUpdatePackageFinish( AiUInt32 module, const char* file)
{
	TY_API_FILE_UPLOAD_IN FileUpload;

	FileUpload.pc_DestFilePath = file;
	FileUpload.pc_SourceFileName = "update.log";
	FileUpload.ul_Flags = API_FILE_FLAG_OVERWRITE;

	return ApiFileUpload(module, &FileUpload);
}





/*!
Reads a LONG value out of the board's memory.

@param[in]   ul_ModuleHandle   Board Module Handle that is returned
                               by the protocol specific AIM API!
@param [in]       uc_MemType   Memory type that wants to be accessed:\n
                               0 = GLOBAL memory\n
                               1 = SHARED memory\n
                               2 = LOCAL memory\n
                               3 = IO memory
@param [in]       ul_Offset    Memory offset in bytes to read from
@param [out]      pul_Value    Returned value that was read

@return 0 (API_OK)  Succesful\n
        1 (API_ERR) Function executed with error
*/
_API_EXTERN AiReturn _API_DLL_FUNC SlWrapReadMemLWord(AiUInt32 ul_ModuleHandle, AiUInt8 uc_MemType, AiUInt32 ul_Offset, AiUInt32 *pul_Value)
{
  TY_DLL_MODULE_INFO_LIST *px_DLLModuleInfo;

  if( AiFalse == uc_IsAlreadyOpened( ul_ModuleHandle, &px_DLLModuleInfo ) )
    return API_ERR;

  return ApiReadMemData( ul_ModuleHandle, uc_MemType, ul_Offset, 4, pul_Value );
}

/*!
Writes a LONG value into the board's memory.

@param[in]   ul_ModuleHandle   Board Module Handle that is returned
                               by the protocol specific AIM API!
@param [in]       uc_MemType   Memory type that wants to be accessed:\n
                               0 = GLOBAL memory\n
                               1 = SHARED memory\n
                               2 = LOCAL memory\n
                               3 = IO memory
@param [in]       ul_Offset    Memory offset in bytes to write to
@param [in]       ul_Value     Value that shall be written into the memory

@return 0 (API_OK)  Succesful\n
        1 (API_ERR) Function executed with error
*/
_API_EXTERN AiReturn _API_DLL_FUNC SlWrapWriteMemLWord( AiUInt32 ul_ModuleHandle, AiUInt8 uc_MemType, AiUInt32 ul_Offset, AiUInt32 ul_Value )
{
  TY_DLL_MODULE_INFO_LIST *px_DLLModuleInfo;

  if( AiFalse == uc_IsAlreadyOpened( ul_ModuleHandle, &px_DLLModuleInfo ) )
    return API_ERR;

  return ApiWriteMemData( ul_ModuleHandle, uc_MemType, ul_Offset, 4, &ul_Value );
}

/*!
Reads a block out of the board's memory.

@param[in]   ul_ModuleHandle   Board Module Handle that is returned
                               by the protocol specific AIM API!
@param [in]       uc_MemType   Memory type that wants to be accessed:\n
                               0 = GLOBAL memory\n
                               1 = SHARED memory\n
                               2 = LOCAL memory\n
                               3 = IO memory
@param [in]       ul_Offset    Memory offset in bytes to read from
@param [in]       uc_Width     Width of the values to be read (1 = Byte, 2 = Word, 4 = Long)
@param [in]       ul_Size      Amount of values to be read
@param [out]      p_Data       Returned block that was read

@return 0 (API_OK)  Succesful\n
        1 (API_ERR) Function executed with error
*/
_API_EXTERN AiReturn _API_DLL_FUNC SlWrapReadBlockMemData(AiUInt32 ul_ModuleHandle, AiUInt8 uc_MemType, AiUInt32 ul_Offset, AiUInt8 uc_Width,
                                                          AiUInt32 ul_Size, void *p_Data )
{
  TY_DLL_MODULE_INFO_LIST *px_DLLModuleInfo;
  AiUInt32 ul_BytesRead;

  if( AiFalse == uc_IsAlreadyOpened( ul_ModuleHandle, &px_DLLModuleInfo ) )
    return API_ERR;

  return ApiReadBlockMemData( ul_ModuleHandle, uc_MemType, ul_Offset, uc_Width, p_Data, ul_Size, &ul_BytesRead );
}

/*!
Writes a block into the board's memory.

@param[in]   ul_ModuleHandle   Board Module Handle that is returned
                               by the protocol specific AIM API!
@param [in]       uc_MemType   Memory type that wants to be accessed:\n
                               0 = GLOBAL memory\n
                               1 = SHARED memory\n
                               2 = LOCAL memory\n
                               3 = IO memory
@param [in]       ul_Offset    Memory offset in bytes to write to
@param [in]       uc_Width     Width of the values to be written (1 = Byte, 2 = Word, 4 = Long)
@param [in]       ul_Size      Amount of values to be written
@param [in]       p_Data       Block that shall be written into the memory

@return 0 (API_OK)  Succesful\n
        1 (API_ERR) Function executed with error
*/
_API_EXTERN AiReturn _API_DLL_FUNC SlWrapWriteBlockMemData(AiUInt32 ul_ModuleHandle, AiUInt8 uc_MemType, AiUInt32 ul_Offset, AiUInt8 uc_Width,
                                                           AiUInt32 ul_Size, void *p_Data )
{
  TY_DLL_MODULE_INFO_LIST *px_DLLModuleInfo;
  AiUInt32 ul_BytesWritten;

  if( AiFalse == uc_IsAlreadyOpened( ul_ModuleHandle, &px_DLLModuleInfo ) )
    return API_ERR;

  return ApiWriteBlockMemData( ul_ModuleHandle, uc_MemType, ul_Offset, uc_Width, p_Data, ul_Size, &ul_BytesWritten );
}

/*!
Executes a system related function on the board

@param[in]   ul_ModuleHandle   Board Module Handle that is returned
                               by the protocol specific AIM API!
@param [in]       uc_Mode      Function to be executed:\n
                               1 = Change time tag source\n
                               2 = Change connection type of D-Sub connector\n
                               8 = Set HS sub address\n
                               13 = Switch interrupt handling to transparent state
@param [in]       uc_Con       Depending on the function given with the "uc_Mode" parameter,
                               this value sets the following sub parameters:\n
                               - Mode 1
                                  -# 0 = Use onboard time tag source
                                  -# 1 = Use external time tag source
                               - Mode 2
                                  -# 0 = RS-232 interface (ASP) is connected with the corresponding bits
                                         of the D-Sub connector of the PBI
                                  -# 1 = RT trigger inputs are connected with the corresponding bits of the
                                         D-Sub connector of the PBI
                               - Mode 8
                                  -# 0 = Disable HS Subaddress (all sub addresses are acting like pure 1553
                                         sub adresses)
                                  -# 1 = HS Subaddress to be set
                               - Mode 13
                                  -# 0 = Disable transparent interrupt mode. ASP interrupts are processed normally.
                                  -# 1 = Enable transparent interrupt mode. ASP interrupts are processed in a
                                         pass-through manner, where normal processing is not performed.

@return 0 (API_OK)  Succesful\n
        1 (API_ERR) Function executed with error

@remarks n/a 

@see n/a

@author Patrick Giesel @date 25.10.2010
*/
_API_EXTERN AiUInt32 _API_DLL_FUNC ul_AiExecSys( AiUInt32 ul_ModuleHandle, AiUInt8 uc_Mode, AiUInt8 uc_Con )
{
  AiUInt16 uw_RetVal = API_ERR;
  TY_DLL_MODULE_INFO_LIST *px_DLLModuleInfo;

  if( AiFalse == uc_IsAlreadyOpened( ul_ModuleHandle, &px_DLLModuleInfo ) )
    return uw_RetVal;

  uw_RetVal = ApiCmdExecSys( ul_ModuleHandle, uc_Mode, uc_Con );

  return uw_RetVal;
}


/*!
Switches the board protocol between EFA and EFEX.

@param[in]   ul_ModuleHandle   Board Module Handle that is returned
                               by the protocol specific AIM API!
@param [in]    uc_SwitchType    Board switch type:\n
                                0 = Switch to EFA bus protocol\n
                                1 = Switch to EFEX bus protocol
@param [in]    aul_LicenseKey   Array that contains the Switch License Key
@param [out]   puc_SwitchResult Switch result:\n
                                0 = Bus protocol could not be switched\n
                                1 = Bus protocol has been switched

@return 0 (API_OK)  Succesful\n
        1 (API_ERR) Function executed with error

@remarks Only applicable on 3910/EFEX boards!\n
         On APX and ACX boards, there is no licence key needed to switch the bus protocols!
         The value of the licence key parameter is ignored!
*/
_API_EXTERN AiReturn _API_DLL_FUNC SlWrapSwitchBoard( AiUInt32 ul_ModuleHandle, AiUInt8 *puc_SwitchResult)
{
  AiUInt16 uw_RetVal = API_ERR;

#ifdef _API3910
  TY_DLL_MODULE_INFO_LIST *px_DLLModuleInfo;
  AiUInt8 uc_SwitchType;
  AiUInt32 aul_LicenseKey[4];

  if( AiFalse == uc_IsAlreadyOpened( ul_ModuleHandle, &px_DLLModuleInfo ) )
    return uw_RetVal;

  if (px_DLLModuleInfo->x_ModuleInfo.ul_Prot == API_PROTOCOL_1553)
    return 0;

  if ((px_DLLModuleInfo->x_ModuleInfo.ul_Prot == API_PROTOCOL_EFEX)
   || (px_DLLModuleInfo->x_ModuleInfo.ul_Prot == API_PROTOCOL_1553_EFEX) )
      uc_SwitchType = 1;
  else
      uc_SwitchType = 2;

  uw_RetVal = Api3910CmdEfSwitchEfaEfex(ul_ModuleHandle, uc_SwitchType, aul_LicenseKey, puc_SwitchResult);
#endif

  return uw_RetVal;
}

/*!
Examines if the board is able to get rebooted by a software function.

@param[in]  ul_ModuleHandle   Board Module Handle that is returned
                              by the protocol specific AIM API!

@return 0 (false) Functionality is not available with the current board software\n
        1 (true)  Functionality is available with the current board software

@remarks n/a

@see n/a

@author Patrick Giesel @date 07.10.2010
*/
_API_EXTERN AiUInt8 _API_DLL_FUNC uc_CanApiReboot(AiUInt32 ul_ModuleHandle)
{
  AiUInt32 ul_RetVal;
  TY_DLL_MODULE_INFO_LIST *px_DLLModuleInfo;
  AiUInt32 ul_Version;

  ul_RetVal = uc_GetDLLModuleInfoListEntry( ul_ModuleHandle, &px_DLLModuleInfo );

  if(API_OK == ul_RetVal)
  {
    ul_Version = (px_DLLModuleInfo->x_ModuleInfo.version.target_sw.ul_MajorVer * 100) +
                  px_DLLModuleInfo->x_ModuleInfo.version.target_sw.ul_MinorVer;
    if( ul_Version >= FIRST_TSW_VER_REBOOT )
      return AiTrue;
  }

  return AiFalse;
}








_API_EXTERN AiReturn _API_DLL_FUNC SlWrapBoardMemAlloc(AiUInt32 ul_ModuleHandle, AiUInt32 ulSize, AiUInt32 * pulOffset)
{
    return ApiCmdSysMalloc( ul_ModuleHandle, 0/*Glob*/, ulSize, 0x534D5254 /*'SMRT'*/, pulOffset);
}



_API_EXTERN AiReturn _API_DLL_FUNC SlWrapBoardMemFree(AiUInt32 ul_ModuleHandle, AiUInt32 ulOffset)
{
    return ApiCmdSysFree( ul_ModuleHandle, 0/*Glob*/, ulOffset, 0x534D5254 /*'SMRT'*/ );
}


_API_EXTERN void _API_DLL_FUNC SlWrapReset(AiUInt32 ul_ModuleHandle)
{
    TY_API_RESET_INFO info;
    ApiCmdReset(ul_ModuleHandle, 0, API_RESET_ALL, &info);
}


_API_EXTERN AiReturn _API_DLL_FUNC SlWrapRebootBoard(AiUInt32 ul_ModuleHandle)
{
    if(uc_CanApiReboot(ul_ModuleHandle))
    {
        return ul_AiExecSys(ul_ModuleHandle, 10, 0) + 10;
    }
    else
        return 0;
}



_API_EXTERN AiReturn _API_DLL_FUNC SlWrapSetNovRamValue(AiUInt32 ulModuleHandle,
    AiUInt32 ulNovRamAddr,
    AiUInt32 ulNovRamValue)
{
    AiUInt32 aul_InLongs[64];
    AiUInt32 aul_OutLongs[64];
    AiUInt8  uc_OutCount = 0;

    memset(aul_InLongs, 0, sizeof(aul_InLongs));
    memset(aul_OutLongs, 0, sizeof(aul_OutLongs));
    aul_InLongs[0] = 0x00;
    aul_InLongs[1] = ulNovRamAddr;
    aul_InLongs[2] = ulNovRamValue;

    return ul_AiUtil(ulModuleHandle, 3, &uc_OutCount, aul_InLongs, aul_OutLongs);
}


_API_EXTERN AiReturn _API_DLL_FUNC SlWrapGetNovRamValue(AiUInt32 ulModuleHandle, AiUInt32 ulNovRamAddr, AiUInt32 *pulNovRamValue)
{
    AiReturn retval;
    AiUInt32 aul_InLongs[64];
    AiUInt32 aul_OutLongs[64];
    AiUInt8  uc_OutCount = 1;

    memset(aul_InLongs, 0, sizeof(aul_InLongs));
    memset(aul_OutLongs, 0, sizeof(aul_OutLongs));
    aul_InLongs[0] = 0x01;          /* Read Board Type Command */
    aul_InLongs[1] = ulNovRamAddr;
    retval = ul_AiUtil(ulModuleHandle, 2, &uc_OutCount, aul_InLongs, aul_OutLongs);

    *pulNovRamValue = aul_OutLongs[0];

    return retval;
}

_API_EXTERN AiReturn _API_DLL_FUNC SlWrapStartNovRam(AiUInt32 ul_ModuleHandle)
{
    AiUInt32 aul_InLongs[64];
    AiUInt32 aul_OutLongs[64];
    AiUInt8  uc_OutCount = 0;

    memset(aul_InLongs, 0, sizeof(aul_InLongs));
    memset(aul_OutLongs, 0, sizeof(aul_OutLongs));

    aul_InLongs[0] = 0x06; /* Start NOVRAM update */

    return ul_AiUtil(ul_ModuleHandle, 1, &uc_OutCount, aul_InLongs, aul_OutLongs);
}



_API_EXTERN AiReturn _API_DLL_FUNC SlWrapFinalizeNovRam(AiUInt32 ul_ModuleHandle)
{
    AiUInt32 aul_InLongs[64];
    AiUInt32 aul_OutLongs[64];
    AiUInt8  uc_OutCount = 0;

    memset(aul_InLongs, 0, sizeof(aul_InLongs));
    memset(aul_OutLongs, 0, sizeof(aul_OutLongs));

    aul_InLongs[0] = 0x07; /* Finalize NOVRAM update */

    return ul_AiUtil(ul_ModuleHandle, 1, &uc_OutCount, aul_InLongs, aul_OutLongs);
}
