/* SPDX-FileCopyrightText: 2011-2024 AIM GmbH <info@aim-online.com> */
/* SPDX-License-Identifier: MIT  */

/*!
SmartLib.cpp
Contains the interface functions for the SmartLib library.
*/

#ifdef _AIM_WINDOWS
#include "Windows.h"
#endif

#include <stdio.h>
#include "Ai_cdef.h"
#include "Ai_def.h"
#include "Wrapper.h"
#include "SmartLib.h"
#include "SmartUpd.h"
#include "hw/AiMyMon.h"
#include "SmartAyE.h"
#include "S0Handling.h"
#include "AiOs.h"
#include "SmartNvr.h"

/****************************************************/
/* DLL internal structure                           */
/****************************************************/


char ac_ActualServer[80] = "local";

/****************************************************/
/* DLL internal prototypes                          */
/****************************************************/



/****************************************************/
/* External prototypes                          */
/****************************************************/
extern AiReturn AiArtix7MonCmdFlashPartitionErase(AiUInt32 ul_ModuleHandle,
    AiUInt32         ulPartition,
    AiUInt32       * pulStatus);
extern AiReturn AiArtix7MonCmdBitfileUpdateChunk(AiUInt32 ul_ModuleHandle,
    AiUInt32         ulPartition,
    AiUInt32         ulFlashOffset,
    AiUInt32         ulSramOffset,
    AiUInt32         ulSramSize,
    AiUInt32       * pulStatus);
extern AiReturn AiArtix7MonCmdFlashPartitionVerify(AiUInt32 ul_ModuleHandle,
    AiUInt32         ulPartition,
    AiUInt32         ulFileSize,
    AiUInt32         ulChecksum,
    AiUInt32       * pulStatus);

/****************************************************/
/* DLL functions                                    */
/****************************************************/
/*!
Writes the actual time stamp into the SMART log file ("smart.log").

@return n/a

@remarks This functionality is only implemented for Windows!

@see v_SLLogFile()

@author Patrick Giesel @date 21.06.2010
*/
_API_EXTERN void _API_DLL_FUNC v_SLLogFileTime( FILE *p_LogFile )
{
#ifdef _WIN32
  if(p_LogFile)
  {
    char auc_Temp[30];
    SYSTEMTIME SystemTime;
    GetLocalTime(&SystemTime);
    sprintf(auc_Temp, "%02d.%02d.%04d %02d:%02d:%02d.%03d: ",
            SystemTime.wDay, SystemTime.wMonth, SystemTime.wYear,
            SystemTime.wHour, SystemTime.wMinute, SystemTime.wSecond, SystemTime.wMilliseconds);
    fwrite(auc_Temp, sizeof(char), strlen(auc_Temp), p_LogFile);
  }
#endif
}

/*!
Writes the given string into the SMART log file ("smart.log"). Optionally it prints
the string also to the console.

@param[in]  sz_Text   String to be written into the log file
@param[in]  uc_PrintTime   0 Do not print the actual time stamp before the given string.\n
                           1 Print the actual time stamp before the given string.
@param[in]  uc_PrintOnConsole   0 Do not print the given string also to the console.\n
                                1 Print the given string also to the console.

@return n/a

@remarks n/a

@see v_SLLogFileTime()

@author Patrick Giesel @date 21.06.2010
*/
_API_EXTERN void _API_DLL_FUNC v_SLLogFile(char *sz_Text, char uc_PrintTime, char uc_PrintOnConsole)
{
#ifndef _AIM_VME
  FILE *p_LogFile;
  char szLogFile[AI_OS_MAX_PATH];
  
  if( 0 != AiOsGetAppDataPath( szLogFile, AI_OS_MAX_PATH - sizeof(SL_LOGFILE_NAME)) )
      return;

  strcat( szLogFile, SL_LOGFILE_NAME );

  p_LogFile = fopen(szLogFile,"a");

  if(p_LogFile)
  {
    if( uc_PrintTime != 0 )
      v_SLLogFileTime( p_LogFile );

    fwrite(sz_Text, sizeof(char), strlen(sz_Text), p_LogFile);
    fclose(p_LogFile);
  }
#endif

  if( uc_PrintOnConsole != 0 )
  {
    printf("%s", sz_Text);
    fflush(stdout);
  }
}

int char_to_int(char single_char)
{
  switch (single_char)
  {
    case '0':
      return (0x0);
    case '1':
      return (0x1);
    case '2':
      return (0x2);
    case '3':
      return (0x3);
    case '4':
      return (0x4);
    case '5':
      return (0x5);
    case '6':
      return (0x6);
    case '7':
      return (0x7);
    case '8':
      return (0x8);
    case '9':
      return (0x9);
    case 'a':
    case 'A':
      return (0xa);
    case 'b':
    case 'B':
      return (0xb);
    case 'c':
    case 'C':
      return (0xc);
    case 'd':
    case 'D':
      return (0xd);
    case 'e':
    case 'E':
      return (0xe);
    case 'f':
    case 'F':
      return (0xf);
    default:
      return (0);
  }
}

_API_EXTERN AiUInt16 _API_DLL_FUNC uw_SLDownloadSREDirectly( AiUInt32 ul_ModuleHandle,
                                                             char *pcFileName,
                                                             AiUInt8 uc_Sector,
                                                             AiUInt32 ul_Addr,
                                                             AiUInt32 ul_Offset)
{
  FILE *p_File;
  char auc_LogStr[300];
  char auc_Str[500+1];
  AiUInt32 ul_MaxLines;
  AiUInt8 uc_Status = 0;
  AiUInt32 ul_FSize = 0;
  AiUInt32 ul_Lines;
  AiUInt32 ul_Len;
  AiUInt16 uw_RetVal = 1;
  AiUInt32 ul_RetValTemp;
  AiUInt32 ul_ProgSize = 0;
  int i;
  TY_MODULE_INFO x_ModuleInfo;

  uw_RetVal = SlWrapReadModuleInfo (ul_ModuleHandle, &x_ModuleInfo);
  if( 0 != uw_RetVal )
      return 0;

  if( 0 == strcmp( " ", pcFileName ) )
    uw_RetVal = 0;
  else if( 0xF0 == uc_Sector )
    uw_RetVal = 0;
  else
  {
    sprintf(auc_LogStr,"   Download SRE (SN:%d) \"%s\", sec=%d, addr=%08X, offs=%08X\n",
        x_ModuleInfo.ul_SerialNumber, pcFileName, uc_Sector, ul_Addr, ul_Offset);
    v_SLLogFile(auc_LogStr, 1, 1);

    p_File = fopen(pcFileName,"r");

    if( NULL == p_File )
    {
      sprintf(auc_LogStr,"   ERROR: File not found\n");
      v_SLLogFile(auc_LogStr, 1, 1);
    }
    else
    {
      /* get number of lines in file */
      rewind(p_File);
      ul_MaxLines = 0;
      while(fgets(auc_Str,255,p_File) != NULL)
        ul_MaxLines++;

      rewind(p_File);

      ul_RetValTemp = SlWrapLoadSREC( ul_ModuleHandle, 0, 0, 0L, (AiUInt8 *)"", &uc_Status, &ul_FSize);
      if( 0 != ul_RetValTemp )
      {
        sprintf(auc_LogStr,"   ERROR: SlWrapLoadSREC(mode=1) --> %d\n", ul_RetValTemp);
        v_SLLogFile(auc_LogStr, 1, 1);
        return uw_RetVal;
      }

      /* Read SRecords from file */
      ul_Lines  = 0L; 
      ul_FSize  = 0L;
      uc_Status = 0;

      while(fgets(auc_Str,255,p_File) != NULL)
      {
        ul_Lines++;
        if ((auc_Str[0] == 'S') &&
            ((auc_Str[1] == '0') || (auc_Str[1] == '1') || (auc_Str[1] == '2') || (auc_Str[1] == '3') ||
            (auc_Str[1] == '4') || (auc_Str[1] == '5') || (auc_Str[1] == '6') || (auc_Str[1] == '7') ||
            (auc_Str[1] == '8') || (auc_Str[1] == '9') ))
        {
          ul_Len = 0;
          for( i=2; i<4; i++ )
          {
            ul_Len *= 16;
            ul_Len += char_to_int(auc_Str[i]);
          }

          ul_Len *= 2;
          ul_Len += 4;

          if(uc_Status == 0)
          {
            ul_RetValTemp = SlWrapLoadSREC( ul_ModuleHandle, 1, (AiUInt8)ul_Len, ul_Offset, (AiUInt8 *)auc_Str, &uc_Status, &ul_FSize);
            if( 0 != ul_RetValTemp )
            {
              sprintf(auc_LogStr,"   ERROR: SlWrapLoadSREC(mode=1) --> %d\n", ul_RetValTemp);
              v_SLLogFile(auc_LogStr, 1, 1);
            }
          }

          if(uc_Status != 0)
          {
            sprintf(auc_LogStr,"   Download Error (%d)\n",uc_Status);
            v_SLLogFile(auc_LogStr, 1, 1);
            break;
          }
        }
      } /* end while */

      if (uc_Status == 0)
      {
        sprintf(auc_LogStr,"   Downloaded %d Lines (%d Bytes). Now programming flash...\n",ul_Lines,ul_FSize);
        v_SLLogFile(auc_LogStr, 1, 1);
        ul_ProgSize = ul_FSize;
      }
      else
      {
        v_SLLogFile("   Download Failed\n", 1, 1);
        ul_ProgSize = 0;
      }

      fclose(p_File);
    }

    if( ul_ProgSize != 0 )
    {

      ul_RetValTemp = SlWrapProgFlash( ul_ModuleHandle, ul_Addr, uc_Sector, ul_ProgSize, &uc_Status);
      if( 0 != ul_RetValTemp )
      {
        sprintf(auc_LogStr,"   ERROR: SlWrapProgFlash --> %d\n", ul_RetValTemp);
        v_SLLogFile(auc_LogStr, 1, 1);
        uc_Status = 0xff;
      }

      if(0 == uc_Status)
      {
        uw_RetVal = 0;
        v_SLLogFile("   Flash programmed successfully!\n", 1, 1);
      }
      else
      {
        if( 1 == uc_Status )
          sprintf(auc_LogStr,"   Flash erasing failed! (retval=%d)\n",uc_Status);
        else if( 2 == uc_Status )
          sprintf(auc_LogStr,"   Flash was not programmed because there was no correct SREC mark identified for this board! (retval=%d)\n",uc_Status);
        else
          sprintf(auc_LogStr,"   Flash programming failed! (retval=%d)\n",uc_Status);
        v_SLLogFile(auc_LogStr, 1, 1);
      }
    }
  }
  return uw_RetVal;
}

/* like atoi, but converts into BCD */
AiUInt32 _atox( char *input, int chars )
{
    AiChar ulTemp[9];
    AiUInt32 ulRetVal;
    
    if( chars > 8 ) chars = 8;

    strncpy( ulTemp, input, chars );
    ulTemp[chars] = '\0';

    sscanf( ulTemp, "%x", &ulRetVal);

    return ulRetVal;
}

/* converts normal (dec) number to BCD */
AiUInt32 _decToBcd(AiUInt32 decimal)
{
    AiUInt32 ulRetVal;
    char *tmp;
    tmp = (char*) malloc( 30 );
    sprintf(tmp, "%d", decimal);
    ulRetVal = _atox(tmp,8);
    free(tmp);

    return ulRetVal;
}

/* converts version struct to BCD format */
AiUInt32 _VersionToBcd(TY_VER_INFO x)
{
    AiUInt32 ulRetVal;

    ulRetVal  = _decToBcd(x.ul_MajorVer & 0xff) << 24;
    ulRetVal += _decToBcd(x.ul_MinorVer & 0xff) << 16;
    ulRetVal += _decToBcd(x.ul_BuildNr & 0xffff);

    return ulRetVal;
}


static AiUInt16 uw_SLUpdateComponent_GetS0Record(char *pcFileName, AiUInt32 *pulVersionS0, char *pcNameS0)
{
    FILE     *p_File;
    char pcS0Line[255];
    char pcVersionS0[40];
    char pcInfoS0[80];
    char auc_LogStr[255];

    *pulVersionS0 = pcNameS0[0] = pcVersionS0[0] = pcInfoS0[0] = 0;

    p_File = fopen(pcFileName,"r");
    if( NULL == p_File )
    {
        sprintf(auc_LogStr,"   uw_SLUpdateComponent_GetS0Record ERROR: File not found\n");
        v_SLLogFile(auc_LogStr, 1, 1);
        return -1;
    }

    if( fgets(pcS0Line,255,p_File) != NULL ) /* check first line only */
    {
        AiOsRemoveLineDelimiters(pcS0Line);

        if ((pcS0Line[0] == 'S') && (pcS0Line[1] == '0'))
        {
            iExtractInformationFromAIM_S0Record(pcS0Line,
                pcNameS0, 40,
                pcVersionS0, 40,
                pcInfoS0, 80);

            sprintf(auc_LogStr,"   Filename:  \'%s\'\n", pcNameS0);    v_SLLogFile(auc_LogStr, 1, 0);
            sprintf(auc_LogStr,"   Version:   \'%s\'\n", pcVersionS0); v_SLLogFile(auc_LogStr, 1, 0);
            sprintf(auc_LogStr,"   Add. Info: \'%s\'\n", pcInfoS0);    v_SLLogFile(auc_LogStr, 1, 0);

            if(    (pcVersionS0[0] != 'V')
                || (pcVersionS0[3] != '.')
                || (pcVersionS0[6] != '.') )
            {
                sprintf(auc_LogStr,"   Warning: Invalid S0 secord version\n" );    v_SLLogFile(auc_LogStr, 1, 0 );
                *pulVersionS0 = 0;
                fclose( p_File );
                return 1;
            }

            /* convert version string (e.g. "V01.12.0004") to BCD value according to the AyE version specification */
            *pulVersionS0  = _atox( pcVersionS0+7, 4 ); /* get build number */
            *pulVersionS0 += _atox( pcVersionS0+4, 2 )<<16; /* get minor number */
            *pulVersionS0 += _atox( pcVersionS0+1, 2 )<<24; /* get major number */
        }
    }
    fclose( p_File );

    return 0;
}


_API_EXTERN AiUInt16 _API_DLL_FUNC uw_SLUpdateComponent( AiUInt32 ul_ModuleHandle,
                                                             AiUInt32 ulComponentId,
                                                             const char     *pcFilePrefix,
                                                             AiUInt32 ulNovRamAddr,
                                                             const char *sz_DirPath,
                                                             AiUInt32 ulForce, 
                                                             enum sl_post_update_actions* post_update_actions)
{
    char pcFilePath[AI_OS_MAX_PATH]    = {'\0'};
    char pcFileName[AI_OS_MAX_PATH]    = {'\0'};
    char pcNameS0[40]                  = {'\0'};
    char pcExpectedNameS0[40]          = {'\0'};
    char auc_LogStr[255]               = {'\0'};
    AiUInt32 ulVersionS0;
    AiUInt32 ulVersionOnboard;
    AiUInt32 ul_NovramExtension=0;
    AiUInt32 ulOffset;
    AiUInt32 ulFileSize;
    AiUInt32 ulCopySize;
    AiUInt8  ucValue;
    AiUInt8  *pFileContent;
    AiUInt16 wRetVal;
    AiUInt32 ulUpdateIterationCount;
    AiUInt32 ulLastStatus = 0;
    FILE     *p_File;

    /* for MyMon commands */
    TyMyMonContext *pxContext;
    AiUInt32 ulReturn;
    AiUInt32 ulStatus;
    TY_MODULE_INFO x_ModuleInfo;

    wRetVal = SlWrapReadModuleInfo (ul_ModuleHandle, &x_ModuleInfo);
    if( 0 != wRetVal )
        return 0;

    pxContext = CreateContext( ul_ModuleHandle );

    /* Create file name */
    wRetVal = SlWrapReadBlockMemData( ul_ModuleHandle, 5 /* Glob Ext*/, ulNovRamAddr, 4, 1, (void*)&ul_NovramExtension);
    if( wRetVal != 0 )
    {
      sprintf(auc_LogStr,"   uw_SLUpdateComponent ERROR: SlWrapReadBlockMemData failed with %d\n", wRetVal);
      v_SLLogFile(auc_LogStr, 1, 1);
      return -1;
    }


    sprintf( pcFileName,       "%s%04X.sre", pcFilePrefix, ul_NovramExtension & 0xFFFF );
    sprintf( pcExpectedNameS0, "%s%04X",     pcFilePrefix, ul_NovramExtension & 0xFFFF );


    strncat( pcFilePath, sz_DirPath, AI_OS_MAX_PATH - strlen(pcFilePath));
    strncat( pcFilePath, pcFileName, AI_OS_MAX_PATH - strlen(pcFilePath));

    sprintf(auc_LogStr,"   Updating component %d with file \"%s\".\n", ulComponentId, pcFilePath );
    v_SLLogFile(auc_LogStr, 1, 1);



    if(    ((ul_NovramExtension & 0xFFFF) == 0xFFFF )
        || ((ul_NovramExtension & 0xFFFF) == 0      ) )
    {
        /* This enables us to list all AyE components in one section */
        /* Basically a disabled component is marked with 0xFFFF
           reserved components are however initialized with 0.
        */
        /* E.G. we always put component BIU2 in the update file. but if
           the device has no extension for this BIU1 E.G. on AEC1553-1
           the update is skipped.
        */


        sprintf( auc_LogStr, "   File extension 0x%04X detected.\n", (ul_NovramExtension & 0xFFFF) );
        v_SLLogFile(auc_LogStr, 1, 1);
        sprintf( auc_LogStr, "   Skipping update.\n" );
        v_SLLogFile(auc_LogStr, 1, 1);
        return 0;
    }









    /**************************************/
    /* Check S0-Rec of the update file    */
    /**************************************/

    /* get version of the component onboard that shall be updated. */
    switch( ulComponentId )
    {
    case 1: ulVersionOnboard = _VersionToBcd(x_ModuleInfo.version.firmware[0]); break;
    case 2: ulVersionOnboard = _VersionToBcd(x_ModuleInfo.version.firmware[1]); break;
    case 3: ulVersionOnboard = _VersionToBcd(x_ModuleInfo.version.firmware[2]); break;
    case 4: ulVersionOnboard = _VersionToBcd(x_ModuleInfo.version.firmware[3]); break;
    case 6: ulVersionOnboard = _VersionToBcd(x_ModuleInfo.version.soc); break;
    }

    /* get version and filename information of the S0 record */
    wRetVal = uw_SLUpdateComponent_GetS0Record( pcFilePath, &ulVersionS0, pcNameS0);


    /* compare version written into S0 with onboard version. 
     * in case the versions are equal, don't update this component (unless forced) */
    if( wRetVal != 0 )
    {
        sprintf(auc_LogStr,"   Failed to parse S0 record.\n");
        v_SLLogFile(auc_LogStr, 1, 1);
        if(ulForce != 0)
        {
            sprintf( auc_LogStr, "   But being forced to download\n");
            v_SLLogFile(auc_LogStr, 1, 1);
        }
        else
        {
            sprintf( auc_LogStr, "   Skipping update.\n");
            v_SLLogFile(auc_LogStr, 1, 1);
            return 0;
        }
    }

    /* compare filename written into S0 with real filename */
    if( strcmp( pcExpectedNameS0, pcNameS0 ) )
    {
        sprintf( auc_LogStr, "   Invalid SRE file name (\"%s\" instead of \"%s\")\n", pcNameS0, pcExpectedNameS0);
        v_SLLogFile(auc_LogStr, 1, 1);
        if(ulForce != 0)
        {
            sprintf( auc_LogStr, "   But being forced to download\n");
            v_SLLogFile(auc_LogStr, 1, 1);
        }
        else
        {
            return -2;
        }
    }



    /* compare version written into S0 with onboard version. 
     * in case the versions are equal, don't update this component (unless forced) */
    if( ulVersionOnboard == ulVersionS0 )
    {
        sprintf(auc_LogStr,"   Version 0x%08X is already onboard.\n", ulVersionS0);
        v_SLLogFile(auc_LogStr, 1, 1);
        if(ulForce != 0)
        {
            sprintf( auc_LogStr, "   But being forced to download\n");
            v_SLLogFile(auc_LogStr, 1, 1);
        }
        else
        {
            sprintf( auc_LogStr, "   Skipping update.\n");
            v_SLLogFile(auc_LogStr, 1, 1);
            return 0;
        }
    }

    sprintf(auc_LogStr, "   Replacing 0x%x with 0x%x.\n", ulVersionOnboard, ulVersionS0);
    v_SLLogFile(auc_LogStr, 1, 1);










    /**************************************/
    /* try to update (up to five times)   */
    /**************************************/

    if( ulComponentId == 6 )
    {
        v_SLLogFile("   ATTENTION: updating SoC. This update may take several minutes.\n", 1, 1 );
        v_SLLogFile("              Do not abort this update, even if it appears to hang.\n", 1, 1 );
    }

    for( ulUpdateIterationCount = 0; ulUpdateIterationCount < 5; ulUpdateIterationCount++)
    {

        p_File = fopen(pcFilePath,"rb");
        if( NULL == p_File )
        {
            sprintf(auc_LogStr,"   uw_SLUpdateComponent ERROR: File not found\n");
            v_SLLogFile(auc_LogStr, 1, 1);
            continue; /* try again */
        }

        /* Allocate global memory area */
        fseek( p_File, 0, SEEK_END);
        ulFileSize = ftell( p_File );
        rewind( p_File );

        pFileContent = malloc(ulFileSize);
        ulCopySize = (AiUInt32)fread( pFileContent, 1, ulFileSize, p_File);
        if( ulCopySize != ulFileSize )
        {
            sprintf(auc_LogStr,"   uw_SLUpdateComponent ERROR: copied only %d bytes of %d\n", ulCopySize, ulFileSize);
            v_SLLogFile(auc_LogStr, 1, 1);
            free(pFileContent);
            fclose( p_File );
            continue; /* try again */
        }

        wRetVal = SlWrapBoardMemAlloc( ul_ModuleHandle, ulFileSize * 2 + 2 /*EOT*/, &ulOffset );
        
        if( wRetVal != 0 )
        {
            sprintf(auc_LogStr,"   uw_SLUpdateComponent ERROR: SlWrapBoardMemAlloc returned 0x%x\n", wRetVal );
            v_SLLogFile(auc_LogStr, 1, 1);
            free(pFileContent);
            fclose( p_File );
            continue; /* try again */
        }
        /* sprintf(auc_LogStr,"   0x%08X <= %s\n", ulOffset, pcFileName); */
        /* v_SLLogFile( auc_LogStr, 1, 0); */

        /* Write SRE file into allocated memory area */
        wRetVal = SlWrapWriteBlockMemData( ul_ModuleHandle, 0, ulOffset, 1, ulFileSize, pFileContent);

        if( wRetVal != 0 )
        {
            sprintf(auc_LogStr,"   uw_SLUpdateComponent ERROR: SlWrapWriteBlockMemData failed with %d\n", wRetVal);
            v_SLLogFile(auc_LogStr, 1, 1);
            free(pFileContent);
            fclose( p_File );
            continue; /* try again */
        }
        free(pFileContent);
        fclose( p_File );

        if( ulCopySize != ulFileSize )
        {
            sprintf(auc_LogStr,"   uw_SLUpdateComponent ERROR: failed to write content to target\n");
            v_SLLogFile(auc_LogStr, 1, 1);
            continue; /* try again */
        }

        /* write end of file mark */
        ucValue = 0x27; /* EOT */
        wRetVal = SlWrapWriteBlockMemData( ul_ModuleHandle, 0, ulOffset+ulFileSize, 1, 1, &ucValue);
        if( wRetVal != 0 )
        {
            sprintf(auc_LogStr,"   uw_SLUpdateComponent ERROR: SlWrapWriteBlockMemData failed with %d\n", wRetVal);
            v_SLLogFile(auc_LogStr, 1, 1);
            continue; /* try again */
        }

        /* update component */
        wRetVal = MyMonCmdComponentUpdateAsync(pxContext,
                                                ulComponentId,
                                                ulOffset,
                                                ulFileSize,
                                                ulOffset + ulFileSize + 1 /*EOT*/,
                                                ulFileSize);

        while( AiTrue )
        {
            wRetVal = MyMonCmdComponentUpdateAsyncCheck(pxContext, &ulReturn, &ulStatus);

            if (wRetVal != MYMON_ERR_BUSY)
            {
                sprintf(auc_LogStr, "\n");
                v_SLLogFile(auc_LogStr, 0, 1);
                break;
            }

            if (ulStatus == ulLastStatus )
            {
                sprintf(auc_LogStr, ".");
                v_SLLogFile(auc_LogStr, 0, 1);
                AiOsSleep(1000);
                continue;
            }

            ulLastStatus = ulStatus;

            sprintf(auc_LogStr, "\n");
            v_SLLogFile(auc_LogStr, 0, 1);

            switch (ulStatus)
            {
            case 0x10000000: sprintf(auc_LogStr, "   check file "); break;
            case 0x01000000: sprintf(auc_LogStr, "   erase flash"); break;
            case 0x00100000: sprintf(auc_LogStr, "   program flash"); break;
            case 0x00010000: sprintf(auc_LogStr, "   verify flash"); break;
            case 0x00001000: sprintf(auc_LogStr, "   program eeprom"); break;
            case 0x00000100: sprintf(auc_LogStr, "   verify eeprom"); break;
            case 0x00000010: sprintf(auc_LogStr, "   decode file"); break;
            }
            v_SLLogFile(auc_LogStr, 1, 1);
        }

        if (wRetVal != 0)
        {
            sprintf(auc_LogStr, "   uw_SLUpdateComponent ERROR: MyMonCmdComponentUpdate failed with %d (%x %x)\n", wRetVal, ulReturn, ulStatus);
            v_SLLogFile(auc_LogStr, 1, 1);
            continue; /* try again */
        }

        /* free previously allocated onboard memory */
        SlWrapBoardMemFree( ul_ModuleHandle, ulOffset );

        /* We succeeded, so we don't need any further iterations of the for-loop*/
        break;

    }






    /* Check if the update was successful */
    if( ulUpdateIterationCount < 5 )
    {
        sprintf(auc_LogStr,"   Version 0x%08X was downloaded.\n", ulVersionS0);
        v_SLLogFile(auc_LogStr, 1, 1);


        /* after the update a cold boot is required */
        *post_update_actions |= SL_POST_UPDATE_POWEROFF;

    }
    else
    {
        sprintf(auc_LogStr,"   uw_SLUpdateComponent ERROR: failed to write content to target\n");
        v_SLLogFile(auc_LogStr, 1, 1);
        CloseContext(pxContext);

        return 1;
    }


    /* if SoC was downloaded, update version register */
    if( 6 == ulComponentId )
    {

        wRetVal = MyMonCmdNovramMirrorWrite( pxContext, (MYMON_GREG_NOVRAM_SOC-0xF400)/4, ulVersionS0, &ulStatus );
        if( wRetVal != 0 )
        {
            sprintf(auc_LogStr,"   uw_SLUpdateComponent ERROR: MyMonCmdNovramMirrorWrite failed with %d (%x)\n", wRetVal, ulStatus);
            v_SLLogFile(auc_LogStr, 1, 1);
            CloseContext(pxContext);
            return 2;
        }

        wRetVal = MyMonCmdNovramMirrorFlush( pxContext, &ulStatus );
        if( wRetVal != 0 )
        {
            sprintf(auc_LogStr,"   uw_SLUpdateComponent ERROR: MyMonCmdNovramMirrorFlush failed with %d (%x)\n", wRetVal, ulStatus);
            v_SLLogFile(auc_LogStr, 1, 1);
            CloseContext(pxContext);
            return 3;
        }

    }

    CloseContext(pxContext);

    return 0;

}


#define BIFI_UPDATE_CHUNK_SIZE (512*1024)

_API_EXTERN AiUInt16 _API_DLL_FUNC uw_SLUpdateBitfile_WriteChunks(AiUInt32 ul_ModuleHandle, AiUInt32 ulPartition, AiUInt32 ulOffset, AiUInt8  *pFileContent, AiUInt32 ulFileSize)
{
    char auc_LogStr[255]               ={ '\0' };
    AiUInt32 ulChunkOffset;
    AiUInt32 ulChunkSize;
    AiReturn ret;
    AiUInt32 retry;
    AiUInt32 status;
    AiUInt32 current_chunk, chunk_count;

    chunk_count = ulFileSize / BIFI_UPDATE_CHUNK_SIZE;
    if ((ulFileSize % BIFI_UPDATE_CHUNK_SIZE) != 0)
        chunk_count++;

    for (current_chunk = 0; current_chunk<chunk_count; current_chunk++)
    {
        for (retry=0; retry<3; retry++)
        {
            if( retry == 2 )
                /* Abort single chunk retry and go back to update start*/
                return 1;
            
            ulChunkOffset = current_chunk * BIFI_UPDATE_CHUNK_SIZE;
            if (current_chunk == chunk_count-1)
                ulChunkSize = ulFileSize % BIFI_UPDATE_CHUNK_SIZE;
            else
                ulChunkSize = BIFI_UPDATE_CHUNK_SIZE;

            sprintf(auc_LogStr, "   writing  partition %d [0x%08X | %7.3fKB] step [%d/%d]\n", ulPartition, ulChunkOffset, (float)ulChunkSize/1024, current_chunk+1, chunk_count);
            v_SLLogFile(auc_LogStr, 1, 1);

            /* Write SRE file into allocated memory area */
            ret = SlWrapWriteBlockMemData(ul_ModuleHandle, 0, ulOffset, 1, ulChunkSize, pFileContent + ulChunkOffset);

            if (ret != 0)
            {
                sprintf(auc_LogStr, "   uw_SLUpdateBitfile_WriteChunks ERROR: SlWrapWriteBlockMemData failed with %d\n", ret);
                v_SLLogFile(auc_LogStr, 1, 1);
                continue; /*retry*/
            }

            ret = AiArtix7MonCmdBitfileUpdateChunk(ul_ModuleHandle,
                ulPartition,
                ulChunkOffset,
                ulOffset,
                ulChunkSize,
                &status);

            if (ret != 0)
            {
                sprintf(auc_LogStr, "   uw_SLUpdateBitfile_WriteChunks ERROR: AiArtix7MonCmdBitfileUpdateChunk failed with %d and TCP status 0x%x\n", ret, status);
                v_SLLogFile(auc_LogStr, 1, 1);
                continue; /*retry*/
            }

            /*Chunk update was successfull, no retry neccessary*/
            break;
        }
    }

    return 0;
}





_API_EXTERN AiUInt16 _API_DLL_FUNC uw_SLUpdateBitfile(AiUInt32 ul_ModuleHandle,
                                                        AiUInt32 ulPartition,
                                                        const char     *pcFilePrefix,
                                                        AiUInt32 ulNovRamAddr,
                                                        const char* sz_Suffix,
                                                        const char *sz_DirPath,
                                                        AiUInt32 ulForce,
                                                        enum sl_post_update_actions* post_update_actions)
{
    char pcFilePath[AI_OS_MAX_PATH]    ={ '\0' };
    char pcFileName[AI_OS_MAX_PATH]    ={ '\0' };
    char auc_LogStr[255]               ={ '\0' };
    AiUInt32 ul_NovramExtension=0;
    AiUInt32 ulOffset;
    AiUInt32 ulChecksum;
    AiUInt32 ulFileSize;
    AiUInt32 ulCopySize;
    AiUInt32 ulStatus;
    AiUInt8  *pFileContent;
    AiReturn ret;
    AiUInt32 i;
    AiUInt32 ulUpdateIterationCount;
    FILE     *p_File;
    TY_MODULE_INFO x_ModuleInfo;

    ret = SlWrapReadModuleInfo(ul_ModuleHandle, &x_ModuleInfo);
    if (0 != ret)
        return 0;

    /* Create file name */
    ret = SlWrapGetNovRamValue(x_ModuleInfo.ul_ModuleHandle, ulNovRamAddr, &ul_NovramExtension);

    if (ret != 0)
    {
        sprintf(auc_LogStr, "   uw_SLUpdateBitfile ERROR: SlWrapGetNovRamValue failed with %d\n", ret);
        v_SLLogFile(auc_LogStr, 1, 1);
        return -1;
    }

    /*
    The boot image consists out of several files that are merged into this file during BSP creation.
    MLCAxxxx.bin VHDL code
    TCP_xxxx.elf Microblaze MCS TCP implementation
    BIP_xxxx.elf Microblaze MCS implementation BIU0
    BIP_xxxx.elf Microblaze MCS implementation BIU1
    */
    sprintf(pcFileName, "%s%04X%s.bin", pcFilePrefix, ul_NovramExtension & 0xFFFF, sz_Suffix);

    strncat(pcFilePath, sz_DirPath, AI_OS_MAX_PATH - strlen(pcFilePath));
    strncat(pcFilePath, pcFileName, AI_OS_MAX_PATH - strlen(pcFilePath));

    /**************************************/
    /* try to update (up to five times)   */
    /**************************************/

    for (ulUpdateIterationCount = 0; ulUpdateIterationCount < 5; ulUpdateIterationCount++)
    {
        sprintf(auc_LogStr, "   Updating partition %d with file \"%s\". [#%d]\n", ulPartition, pcFilePath, ulUpdateIterationCount );
        v_SLLogFile(auc_LogStr, 1, 1);

        p_File = fopen(pcFilePath, "rb");
        if (NULL == p_File)
        {
            sprintf(auc_LogStr, "   uw_SLUpdateBitfile ERROR: File not found\n");
            v_SLLogFile(auc_LogStr, 1, 1);
            continue; /* try again */
        }

        /* Allocate global memory area */
        fseek(p_File, 0, SEEK_END);
        ulFileSize = ftell(p_File);
        rewind(p_File);

        pFileContent = malloc(ulFileSize);

        if (pFileContent == NULL )
        {
            sprintf(auc_LogStr, "   uw_SLUpdateBitfile malloc failed\n");
            v_SLLogFile(auc_LogStr, 1, 1);
            fclose(p_File);
            continue; /* try again */
        }

        ulCopySize = (AiUInt32)fread(pFileContent, 1, ulFileSize, p_File);
        if (ulCopySize != ulFileSize)
        {
            sprintf(auc_LogStr, "   uw_SLUpdateBitfile ERROR: copied only %d bytes of %d\n", ulCopySize, ulFileSize);
            v_SLLogFile(auc_LogStr, 1, 1);
            free(pFileContent);
            fclose(p_File);
            continue; /* try again */
        }

        ret = SlWrapBoardMemAlloc(ul_ModuleHandle, BIFI_UPDATE_CHUNK_SIZE, &ulOffset);

        if (ret != 0)
        {
            sprintf(auc_LogStr, "   uw_SLUpdateBitfile ERROR: SlWrapBoardMemAlloc returned 0x%x\n", ret);
            v_SLLogFile(auc_LogStr, 1, 1);
            free(pFileContent);
            fclose(p_File);
            continue; // try again
        }

        sprintf(auc_LogStr, "   erasing  partition %d\n", ulPartition);
        v_SLLogFile(auc_LogStr, 1, 1);

        ret = AiArtix7MonCmdFlashPartitionErase(ul_ModuleHandle, ulPartition, &ulStatus);

        if (ret != 0)
        {
            sprintf(auc_LogStr, "   uw_SLUpdateBitfile ERROR: AiArtix7MonCmdFlashPartitionErase failed with %d and TCP status 0x%x\n", ret, ulStatus);
            v_SLLogFile(auc_LogStr, 1, 1);
            free(pFileContent);
            fclose(p_File);
            continue; /* try again */
        }

        ret = uw_SLUpdateBitfile_WriteChunks(ul_ModuleHandle, ulPartition, ulOffset, pFileContent, ulFileSize);

        if (ret != 0)
        {
            sprintf(auc_LogStr, "   uw_SLUpdateBitfile ERROR: uw_SLUpdateBitfile_WriteChunks failed with %d\n", ret);
            v_SLLogFile(auc_LogStr, 1, 1);
            free(pFileContent);
            fclose(p_File);
            continue; /* try again */
        }

        /* free previously allocated onboard memory */
        SlWrapBoardMemFree(ul_ModuleHandle, ulOffset);

        sprintf(auc_LogStr, "   checking partition %d\n", ulPartition);
        v_SLLogFile(auc_LogStr, 1, 1);

        ulChecksum = 0;
        for(i=0; i<ulFileSize; i++ )
            ulChecksum += pFileContent[i];

        ret = AiArtix7MonCmdFlashPartitionVerify(ul_ModuleHandle, ulPartition, ulFileSize, ulChecksum, &ulStatus);

        if (ret != 0)
        {
            sprintf(auc_LogStr, "   uw_SLUpdateBitfile ERROR: AiArtix7MonCmdFlashPartitionVerify failed with %d and TCP status 0x%x\n", ret, ulStatus);
            v_SLLogFile(auc_LogStr, 1, 1);
            free(pFileContent);
            fclose(p_File);
            continue; /* try again */
        }

        free(pFileContent);
        fclose(p_File);

        /* We succeeded, so we don't need any further iterations of the for-loop*/
        break;
    }


    /* Check if the update was successful */
    if (ulUpdateIterationCount < 5)
    {
        sprintf(auc_LogStr, "   Bitfile was downloaded.\n");
        v_SLLogFile(auc_LogStr, 1, 1);

        /* after the update a cold boot is required */
        *post_update_actions |= SL_POST_UPDATE_POWEROFF;
    }
    else
    {
        sprintf(auc_LogStr, "   uw_SLUpdateBitfile ERROR: failed to write content to target\n");
        v_SLLogFile(auc_LogStr, 1, 1);

        return 1;
    }

    return 0;

}


_API_EXTERN AiUInt16 _API_DLL_FUNC uw_SLUpdatePackage(AiUInt32 module,
                                                               const char *sz_FileName,
                                                               const char *sz_FilePath,
                                                               AiUInt32 ul_Force)
{
    AiUInt16 ret      = 0;
    AiUInt16 ret_temp = 0;
    char fqn[AI_OS_MAX_PATH] = {'\0'};
    char auc_LogStr[255]     = {'\0'};
    char log_file[AI_OS_MAX_PATH];
    AiBoolean in_progress;
    int       components;
    int       total;

    
    TY_MODULE_INFO x_ModuleInfo;

    ret = SlWrapReadModuleInfo(module, &x_ModuleInfo);

    if( 0 != ret )
        return ret;

    strncat( fqn, sz_FilePath, AI_OS_MAX_PATH);
    strncat( fqn, sz_FileName, AI_OS_MAX_PATH - strlen(fqn));

    sprintf(auc_LogStr,"   Update package with file \"%s\"\n", fqn );
    v_SLLogFile(auc_LogStr, 1, 1);

    v_SLLogFile("   ATTENTION: updating  an onboard package. This update may take several minutes.\n", 1, 1);
    v_SLLogFile("              Do not abort this update, even if it appears to hang.\n", 1, 1);

    /* -- Start update with package --- */

    ret = SlWrapUpdatePackageStart( module, fqn );

    if( 0 != ret )
    {
        sprintf(auc_LogStr,"   Update package start failed with %d\n", ret );
        v_SLLogFile(auc_LogStr, 1, 1);
        return ret;
    }

    /* -- Update running / wait for finished --- */
    
    do
    {
        /* Do not read status to often because every USB access interrupts the update */
        AiOsSleep(1000); 
        
        ret = SlWrapUpdatePackageStatus( module, &in_progress, &components, &total );

        if (ret != 0)
            break;

        /* Components is actually the amount of components finished and not the one currently in progress. 
           We add 1 to both components and total to avoid printing (0/5) a long time (MLCA update). */
        sprintf(auc_LogStr,"   Update package in progress (%d/%d)\n", ++components, ++total );
        v_SLLogFile(auc_LogStr, 1, 1);

    } while( in_progress );


    if( ret != 0 )
    {
        sprintf(auc_LogStr,"   Update package failed with %d\n", ret );
        v_SLLogFile(auc_LogStr, 1, 1);
    }


    /* -- Read log file --- */
    /* Do not overwrite ret from here to preserve update error status */

    AiOsGetAppDataPath(log_file, AI_OS_MAX_PATH);
    strncat(log_file, "update.log", AI_OS_MAX_PATH - strlen(log_file) );
    
    sprintf(auc_LogStr,"   Update package log %s\n", log_file );
    v_SLLogFile(auc_LogStr, 1, 1);

    ret_temp = SlWrapUpdatePackageFinish( module, log_file );

    if( ret_temp != 0 )
    {
        sprintf(auc_LogStr,"   Update package log failed with %d\n", ret_temp );
        v_SLLogFile(auc_LogStr, 1, 1);
    }
    return ret;

}


_API_EXTERN AiUInt16 _API_DLL_FUNC uw_SLUpdateBoard(AiUInt32 ul_ModuleHandle, 
                                                    AiChar *sz_SrvName,
                                                    AiChar *pcFileName,
                                                    AiUInt32 ul_Force,
                                                    enum sl_post_update_actions* post_update_actions)
{
    char          auc_LogStr[200];
    char          auc_Path[AI_OS_MAX_PATH];
    TY_MODULE_INFO x_ModuleInfo;
    AiUInt16      uw_RetVal                = 0;
    AiBoolean     b_Abort;
    AiUInt16      uw_InstrCode             = 0;
    AiUInt32      ul_Info                  = 0;
    FILE          *p_File;
    char          ac_Str[500 + 1];
    AiChar        ac_ParamStr[SL_MAX_PARAM_SIZE];
    AiChar        ac_Suffix[SL_MAX_PARAM_SIZE];
    AiUInt32      aul_Params[5];
    AiChar        ac_TmpPath[AI_OS_MAX_PATH];
    AiBoolean     b_InterpreteEntries      = AiTrue;
    AiBoolean     b_SkipNextInstruction    = AiFalse;
    AiBoolean     b_HeaderFoundAndValid    = AiFalse;

    strncpy( auc_Path, pcFileName, sizeof(auc_Path));
    AiOsDirname( auc_Path, sizeof(auc_Path) );

    v_SLLogFile("\n", 1, 1);
    v_SLLogFile("***********************************************************\n", 1, 1 );
    v_SLLogFile("Updating Board ", 1, 1 );
    if( NULL == sz_SrvName )
    {
        uw_RetVal = 10;
        v_SLLogFile("   ERROR: Illegal server name!\n", 0, 1);
    }
    else if( NULL == pcFileName )
    {
        uw_RetVal = 11;
        v_SLLogFile("   ERROR: Illegal update file name!\n", 0, 1);
    }
    else if( NULL == post_update_actions )
    {
        uw_RetVal = 12;
        v_SLLogFile("   ERROR: Parameter 3 is NULL!\n", 0, 1);
    }
    else
    {
        if( (uw_RetVal = SlWrapReadModuleInfo( ul_ModuleHandle, &x_ModuleInfo)) != 0 )
        {
            v_SLLogFile("   ERROR: Module could not be opened!\n", 0, 1);
        }
        else
        {
            /* We must not abort the update in case SlWrapReset fails*/
            /* If the update is called with a golden image running the FW is not loaded and the reset command might fail.*/
            SlWrapReset(x_ModuleInfo.ul_ModuleHandle);

            sprintf(auc_LogStr, "%s (SN:%d) on server <%s>\n", x_ModuleInfo.ac_BoardName,
                x_ModuleInfo.ul_SerialNumber, sz_SrvName);
            v_SLLogFile(auc_LogStr, 0, 1);
            v_SLLogFile("***********************************************************\n", 1, 1 );

            p_File = fopen(pcFileName,"r");

            if( NULL == p_File )
            {
                sprintf(auc_LogStr,"   ERROR: Update File <%s> not found!\n", pcFileName );
                v_SLLogFile(auc_LogStr, 1, 1);
                uw_RetVal = 4;
            }
            else
            {
                rewind(p_File);

                sprintf(auc_LogStr,"Searching entries for boardconfig <0x%08X> and boardtype <0x%08X>\n", 
                    x_ModuleInfo.nvr.boardConfig,
                    x_ModuleInfo.nvr.boardType);
                v_SLLogFile(auc_LogStr, 1, 1);

                /* start interpreting entries, by default the interpretation is enabled at start */
                b_InterpreteEntries = AiTrue;
                b_Abort             = AiFalse;
                while( !b_Abort && (fgets(ac_Str,255,p_File) != NULL) )
                {
                    /* if previously recognized IF instruction and if clause is FALSE */
                    /* discard the next instruction */
                    if (b_SkipNextInstruction)
                    {
                        b_SkipNextInstruction = AiFalse;
                        continue;
                    }

                    /*Eliminate End of Line (LF+CR for Windows, LF for Linux */
                    AiOsRemoveLineDelimiters(ac_Str);

                    uw_InstrCode = uw_SLInterpreteUPDEntry( &x_ModuleInfo , ac_Str );

                    if( b_InterpreteEntries && (uw_InstrCode != SL_INSTR_HEADER) && (uw_InstrCode != SL_INSTR_COMMENT))
                    {
                        if( strlen(ac_Str) != 0 ){ /* don't show empty lines */
                            sprintf(auc_LogStr,"%s\n", ac_Str );
                            v_SLLogFile(auc_LogStr, 1, 0);
                        }
                    }

                    switch( uw_InstrCode )
                    {
                    case SL_INSTR_HEADER:
                            if( !b_HeaderFoundAndValid )
                            {
                                uw_RetVal = uw_SLCheckHeaderInstr(&x_ModuleInfo, ac_Str, &ul_Info);

                                if (SL_RETVAL_HEADER_INSTR_IS_VALID_FOR_BOARD == ul_Info)
                                {
                                    sprintf(auc_LogStr,"%s\n", ac_Str );
                                    v_SLLogFile(auc_LogStr, 1, 0);

                                    b_HeaderFoundAndValid = AiTrue;
                                    b_InterpreteEntries = AiTrue;
                                }
                                else if (SL_RETVAL_HEADER_INSTR_IS_INVALID_FOR_BOARD == ul_Info)
                                {
                                    sprintf(auc_LogStr,"%s\n", ac_Str );
                                    v_SLLogFile(auc_LogStr, 1, 0);
                                    if(b_InterpreteEntries)
                                        v_SLLogFile("   Skipping entries until next valid header is found for this board\n", 1, 0);

                                    b_HeaderFoundAndValid = AiFalse;
                                    b_InterpreteEntries   = AiFalse;
                                }
                        }
                        break;
                    case SL_INSTR_IF:
                        if( b_InterpreteEntries )
                        {
                            uw_RetVal = uw_SLExecuteIfInstr( &x_ModuleInfo, ac_Str );
                            if( SL_RETVAL_IF_INSTR_IS_TRUE == uw_RetVal ) /* Execute second line of IF instruction, if IF operation was true */
                            {
                                uw_RetVal = 0;
                            }
                            else if( SL_RETVAL_IF_INSTR_IS_FALSE == uw_RetVal )
                            {
                                b_SkipNextInstruction = AiTrue;
                                uw_RetVal = 0;
                            }
                        }
                        b_HeaderFoundAndValid = AiFalse;
                        break;
                    case SL_INSTR_NVR_CHK:
                        if (b_InterpreteEntries)
                        {
                            uw_RetVal = uw_SLExecuteNvrChk(&x_ModuleInfo, ac_Str);
                            if (SL_RETVAL_IF_INSTR_IS_TRUE == uw_RetVal) /* Execute next line, only if the NovRam check operation was true */
                            {
                                uw_RetVal = 0;
                            }
                            else if (SL_RETVAL_IF_INSTR_IS_FALSE == uw_RetVal)
                            {
                                b_SkipNextInstruction = AiTrue;
                                uw_RetVal = 0;
                            }
                        }
                        b_HeaderFoundAndValid = AiFalse;
                        break;

                    case SL_INSTR_EXEC_REBOOT:
                        if( b_InterpreteEntries )
                        {
                            uw_RetVal = uw_SLCheckExecRebootInstr(ac_Str, ac_ParamStr);
                            if (0 == uw_RetVal)
                            {
                                v_SLLogFile("   #########################################################\n", 1, 1);
                                v_SLLogFile("   #\n", 1, 1);
                                v_SLLogFile("   # The update will now reboot the board.\n", 1, 1);
                                v_SLLogFile("   #\n", 1, 1);
                                if (strlen(ac_ParamStr))
                                {
                                    sprintf(auc_LogStr, "   %s\n", ac_ParamStr);
                                    v_SLLogFile(auc_LogStr, 1, 1);
                                    v_SLLogFile("   #\n", 1, 1);
                                }
                                v_SLLogFile("   #########################################################\n", 1, 1);

                                uw_RetVal = SlWrapRebootBoard(x_ModuleInfo.ul_ModuleHandle);
                            }

                            if (uw_RetVal) v_SLLogFile("   ERROR\n", 1, 1);
                        }
                        b_HeaderFoundAndValid = AiFalse;
                        break;

                    case SL_INSTR_NVR: /* write data to NOVRAM */
                        if (b_InterpreteEntries)
                        {
                            uw_RetVal = uw_SLCheckNvrInstr(ac_Str, aul_Params);
                            if (0 == uw_RetVal)
                            {
                                if (x_ModuleInfo.ul_NovramChanged == 0)
                                {
                                    x_ModuleInfo.ul_NovramChanged = 1;
                                    uw_RetVal = SlWrapStartNovRam(x_ModuleInfo.ul_ModuleHandle);
                                }

                                uw_RetVal = SlWrapSetNovRamValue(x_ModuleInfo.ul_ModuleHandle, aul_Params[0], aul_Params[1]);

                                if (0 != uw_RetVal)
                                    v_SLLogFile("NovRam Update FAILED\n", 0, 1);
                            }
                        }
                        b_HeaderFoundAndValid = AiFalse;
                        break;

                    case SL_INSTR_PACKAGE:
                        if (b_InterpreteEntries)
                        {
                            uw_RetVal = uw_SLCheckPackageInstr(ac_Str, ac_ParamStr);
                            if (0 == uw_RetVal)
                            {
                                uw_RetVal = uw_SLUpdatePackage(x_ModuleInfo.ul_ModuleHandle, ac_ParamStr, auc_Path, ul_Force);
                            }
                            if (uw_RetVal) v_SLLogFile("   ERROR\n", 1, 1);
                        }
                        b_HeaderFoundAndValid = AiFalse;
                        break;

                    case SL_INSTR_COMPONENT: /* update an onboard component */
                        if (b_InterpreteEntries)
                        {
                            uw_RetVal = uw_SLCheckComponentInstr(ac_Str, ac_ParamStr, aul_Params);
                            if (0 == uw_RetVal)
                            {
                                uw_RetVal = uw_SLUpdateComponent(x_ModuleInfo.ul_ModuleHandle, aul_Params[0], ac_ParamStr, aul_Params[1], auc_Path, ul_Force, post_update_actions);
                            }
                            if (uw_RetVal) v_SLLogFile("   ERROR\n", 1, 1);
                        }
                        b_HeaderFoundAndValid = AiFalse;
                        break;

                    case SL_INSTR_BIFI: /* Artix7 Bitfile (BIFI) */
                        if (b_InterpreteEntries)
                        {
                            uw_RetVal = uw_SLCheckBitfileInstr(ac_Str, ac_ParamStr, aul_Params, ac_Suffix);
                            if (0 == uw_RetVal)
                            {
                                uw_RetVal = uw_SLUpdateBitfile(x_ModuleInfo.ul_ModuleHandle, aul_Params[0], ac_ParamStr, aul_Params[1], ac_Suffix, auc_Path, ul_Force, post_update_actions);
                            }
                            if (uw_RetVal) v_SLLogFile("   ERROR\n", 1, 1);
                        }
                        b_HeaderFoundAndValid = AiFalse;
                        break;

                    case SL_INSTR_FILE:
                        if (b_InterpreteEntries)
                        {
                            memset(ac_ParamStr, 0, SL_MAX_PARAM_SIZE);
                            uw_RetVal = uw_SLCheckFileInstr(ac_Str, ac_ParamStr, aul_Params);
                            if (0 == uw_RetVal)
                            {
                                memcpy(ac_TmpPath, auc_Path, AI_OS_MAX_PATH);
                                strncat(ac_TmpPath, ac_ParamStr, AI_OS_MAX_PATH - strlen(ac_TmpPath));

                                uw_RetVal = uw_SLDownloadSREDirectly(x_ModuleInfo.ul_ModuleHandle, ac_TmpPath,
                                    (AiUInt8)aul_Params[0], aul_Params[1], aul_Params[2]);
                            }

                            if (uw_RetVal) v_SLLogFile("   ERROR\n", 1, 1);
                        }
                        b_HeaderFoundAndValid = AiFalse;
                        break;

                    case SL_INSTR_COLDBOOT: /* print cold boot message */
                    case SL_INSTR_COLDBOOT_R:
                        if (b_InterpreteEntries)
                        {
                            if (uw_InstrCode == SL_INSTR_COLDBOOT)   uw_RetVal = uw_SLCheckInstrWithTxtOut(ac_Str, SL_INSTR_NAME_COLDBOOT, ac_ParamStr);
                            if (uw_InstrCode == SL_INSTR_COLDBOOT_R) uw_RetVal = uw_SLCheckInstrWithTxtOut(ac_Str, SL_INSTR_NAME_COLDBOOT_R, ac_ParamStr);

                            if (0 == uw_RetVal)
                            {
                                if (strlen(ac_ParamStr))
                                {
                                    v_SLLogFile("\n", 1, 1);
                                    v_SLLogFile("   #########################################################\n", 1, 1);
                                    v_SLLogFile("   # The update requires a cold boot of your system.\n", 1, 1);
                                    v_SLLogFile("   # Please shut down your computer and wait for 10 seconds.\n", 1, 1);
                                    v_SLLogFile("   # After that please power on your system.\n", 1, 1);
                                    v_SLLogFile("   #\n", 1, 1);

                                    sprintf(auc_LogStr, "   # %s\n", ac_ParamStr);
                                    v_SLLogFile(auc_LogStr, 1, 1);
                                    v_SLLogFile("   #\n", 1, 1);
                                    v_SLLogFile("   #########################################################\n", 1, 1);
                                    v_SLLogFile("\n", 1, 1);
                                }

                                if (uw_InstrCode == SL_INSTR_COLDBOOT)   *post_update_actions |= SL_POST_UPDATE_POWEROFF;
                                if (uw_InstrCode == SL_INSTR_COLDBOOT_R) *post_update_actions |= SL_POST_UPDATE_POWEROFF | SL_POST_UPDATE_SMART_RESTART;

                                b_Abort = AiTrue;
                            }

                            if (uw_RetVal) v_SLLogFile("   ERROR\n", 1, 1);
                        }
                        b_HeaderFoundAndValid = AiFalse;
                        break;

                    case SL_INSTR_REBOOT: /* print reboot message */
                    case SL_INSTR_REBOOT_R:
                        if (b_InterpreteEntries)
                        {
                            if (uw_InstrCode == SL_INSTR_REBOOT)   uw_RetVal = uw_SLCheckInstrWithTxtOut(ac_Str, SL_INSTR_NAME_REBOOT, ac_ParamStr);
                            if (uw_InstrCode == SL_INSTR_REBOOT_R) uw_RetVal = uw_SLCheckInstrWithTxtOut(ac_Str, SL_INSTR_NAME_REBOOT_R, ac_ParamStr);
                            if (0 == uw_RetVal)
                            {
                                if (strlen(ac_ParamStr))
                                {
                                    v_SLLogFile("\n", 1, 1);
                                    v_SLLogFile("   #########################################################\n", 1, 1);
                                    v_SLLogFile("   #\n", 1, 1);
                                    v_SLLogFile("   # The update requires to reboot the system.\n", 1, 1);
                                    v_SLLogFile("   #\n", 1, 1);
                                    sprintf(auc_LogStr, "   # %s\n", ac_ParamStr);
                                    v_SLLogFile(auc_LogStr, 1, 1);
                                    v_SLLogFile("   #\n", 1, 1);
                                    v_SLLogFile("   #########################################################\n", 1, 1);
                                    v_SLLogFile("\n", 1, 1);
                                }

                                if (uw_InstrCode == SL_INSTR_REBOOT)     *post_update_actions |= SL_POST_UPDATE_REBOOT;
                                if (uw_InstrCode == SL_INSTR_REBOOT_R)   *post_update_actions |= SL_POST_UPDATE_REBOOT | SL_POST_UPDATE_SMART_RESTART;

                            }

                            if (uw_RetVal) v_SLLogFile("   ERROR\n", 1, 1);
                            b_Abort = AiTrue;
                        }
                        b_HeaderFoundAndValid = AiFalse;
                        break;

                    case SL_INSTR_ABORT: /* print abort message */
                        if (b_InterpreteEntries)
                        {
                            uw_RetVal = uw_SLCheckInstrWithTxtOut(ac_Str, SL_INSTR_NAME_ABORT, ac_ParamStr);
                            if (0 == uw_RetVal)
                            {
                                if (strlen(ac_ParamStr))
                                {
                                    v_SLLogFile("   #########################################################\n", 1, 1);
                                    v_SLLogFile("   #\n", 1, 1);
                                    sprintf(auc_LogStr, "   # %s\n", ac_ParamStr);
                                    v_SLLogFile(auc_LogStr, 1, 1);
                                    v_SLLogFile("   #\n", 1, 1);
                                    v_SLLogFile("   #########################################################\n", 1, 1);
                                }
                                b_Abort = AiTrue;
                            }

                            if (uw_RetVal) v_SLLogFile("   ERROR\n", 1, 1);
                        }
                        b_HeaderFoundAndValid = AiFalse;
                        break;

                    case SL_INSTR_CSM:
                        b_HeaderFoundAndValid = AiFalse;
                        break;
                    case SL_INSTR_COMMENT:
                    default:
                        break;
                    }

                    if( uw_RetVal != 0 )
                        b_Abort = AiTrue;
                }

                fclose( p_File );

                /* If NOVRAM was changed we have finalize it.
                   For AyX cards this command will program the LCA clocks. 
                   For AyE cards this command will flush the NOVRAM and calculate the checksum.
                */
                if( x_ModuleInfo.ul_NovramChanged == 1 )
                {
                    SlWrapFinalizeNovRam( ul_ModuleHandle );
                    x_ModuleInfo.ul_NovramChanged = 0;
                }
            }
        }
    }

    return uw_RetVal;
}

_API_EXTERN AiUInt32 _API_DLL_FUNC uw_SLGetAmountOfModuleInfos(AiChar *ac_SrvName)
{
    /* ignore the return value because we call the disconnect from server just to be sure */
    SlWrapDisconnectFromServer(ac_ActualServer);

    return SlWrapConnectToServer(ac_SrvName);
}

_API_EXTERN AiUInt16 _API_DLL_FUNC uw_SLGetModuleInfo(AiChar *ac_SrvName, AiUInt32 ul_ModuleId, TY_MODULE_INFO *px_ModuleInfo)
{
  AiUInt32 ul_ApiModuleHandle;
  AiUInt32 ul_RetVal = 0;

/*  ul_RetVal = ul_AiDisconnectFromServer((AiUChar *)ac_ActualServer); */
  if( 0 == ul_RetVal )
  {
    /* connect to the server we want to get the info from */
/*    ul_RetVal = ul_AiConnectToServer(ac_SrvName); */
    if( 0 == ul_RetVal )
    {
      /* set the given server as actually connected server */
      strncpy(ac_ActualServer, (char *)ac_SrvName, sizeof(ac_ActualServer) );

      ul_RetVal = SlWrapOpenModule(ul_ModuleId, 1, ac_SrvName, &ul_ApiModuleHandle);
      if( 0 == ul_RetVal )
      {
        ul_RetVal = SlWrapReadModuleInfo (ul_ApiModuleHandle, px_ModuleInfo);
      }
    }
  }

  return(AiTrue);
}

_API_EXTERN AiUInt32 _API_DLL_FUNC ul_SLSwitchBoard(AiUInt32 ul_ModuleHandle, AiUInt8 *puc_SwitchResult)
{
    char auc_LogStr[200];
    AiUInt32 ul_RetVal = 0;
    TY_MODULE_INFO x_ModuleInfo;

    ul_RetVal = SlWrapReadModuleInfo(ul_ModuleHandle, &x_ModuleInfo);
    if (0 == ul_RetVal)
    {


        v_SLLogFile("\n", 1, 1);
        sprintf(auc_LogStr, "Switching Board %s (SN:%d)...",
            x_ModuleInfo.ac_BoardName,
            x_ModuleInfo.ul_SerialNumber);
        v_SLLogFile(auc_LogStr, 1, 1);

        ul_RetVal = SlWrapSwitchBoard(ul_ModuleHandle, puc_SwitchResult);
        if (0 == ul_RetVal)
        {
            if (1 == *puc_SwitchResult)
            {
                v_SLLogFile("OK\n", 0, 1);
            }
            else
                v_SLLogFile("ERROR (Key may be wrong)\n", 0, 1);
        }
        else
            v_SLLogFile("ERROR\n", 0, 1);
    }
    else
        v_SLLogFile("Switching Board Error!!!\n", 1, 1);

    return(AiTrue);
}



_API_EXTERN AiUInt32 _API_DLL_FUNC ul_SLNvrDump(AiUInt32 ul_ModuleHandle)
{
    AiUInt32 i;
    char auc_LogStr[200];
    AiUInt32 ul_RetVal = 0;
    AiUInt32 ul_Value  = 0;
    struct sl_nvr_entry * lookup;
    TY_MODULE_INFO x_ModuleInfo;

    ul_RetVal = SlWrapReadModuleInfo(ul_ModuleHandle, &x_ModuleInfo);
    if (0 == ul_RetVal)
    {
        sprintf(auc_LogStr, "Novram Dump for %s SN%d\n", x_ModuleInfo.ac_BoardName, x_ModuleInfo.ul_SerialNumber );
        v_SLLogFile(auc_LogStr, 1, 1);


        for (i=0; i<SL_MAX_NVR_ENTRIES; i++ )
        {
            lookup = ls_get_nvr_lookup( x_ModuleInfo.nvr.boardConfig & 0xFF );

            if (lookup[i].offset == SL_MAX_NVR_EOL )
                // End of list reached 
                break;

            ul_RetVal = SlWrapGetNovRamValue(x_ModuleInfo.ul_ModuleHandle, lookup[i].offset, &ul_Value);

            if (ul_RetVal)
            {
                sprintf(auc_LogStr, "   SlWrapGetNovRamValue failed with 0x%x\n", ul_RetVal);
                v_SLLogFile(auc_LogStr, 1, 1);
                sprintf(auc_LogStr, "   Please update the system driver and try again.\n");
                v_SLLogFile(auc_LogStr, 1, 1);
                return ul_RetVal;
            }

            sprintf(auc_LogStr, "%s @0x%04x: 0x%08X\n", lookup[i].description, lookup[i].offset, ul_Value);
            v_SLLogFile(auc_LogStr, 1, 1);
        }
    }

    return(AiTrue);
}
