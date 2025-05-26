/* SPDX-FileCopyrightText: 2011-2024 AIM GmbH <info@aim-online.com> */
/* SPDX-License-Identifier: MIT  */

/*!
SmartLib.cpp
Contains the interface functions for the SmartLib library.
*/
#include <string.h> /* for strcpy() */
#include <ctype.h> /* for toupper() */
#include <stdio.h> /* for printf(), sscanf(), gets() */
#include <stdlib.h> /* for exit() */
#include "Ai_cdef.h"
#include "Ai_def.h"
#include "Wrapper.h"
#include "SmartLib.h"
#include "SmartUpd.h"
#include "AiOs.h"


#ifndef false
  #define false 0
#endif
#ifndef true
  #define true 1
#endif

char sz_IfValidationNames[SL_MAX_IF_VALIDATIONS][20] = { SL_IF_VALIDATE_TSWVER_NAME,
                                                         SL_IF_VALIDATE_BIU1VER_NAME,
                                                         SL_IF_VALIDATE_BIU2VER_NAME,
                                                         SL_IF_VALIDATE_BIU3VER_NAME,
                                                         SL_IF_VALIDATE_BIU4VER_NAME,
                                                         SL_IF_VALIDATE_IOLCA1VER_NAME,
                                                         SL_IF_VALIDATE_IOLCA2VER_NAME,
                                                         SL_IF_VALIDATE_IOLCA3VER_NAME,
                                                         SL_IF_VALIDATE_IOLCA4VER_NAME,
                                                         SL_IF_VALIDATE_PCILCAVER_NAME,
                                                         SL_IF_VALIDATE_TCPVER_NAME,
                                                         SL_IF_VALIDATE_MONVER_NAME };



int strpos( char * haystack, char * needle )
{
    char * p = strstr( haystack, needle);
    if( p )
        return (int)(p-haystack);
    else
        return -1;
}


/*!
Analyzes the syntax of an update file and returns the result.

@param[in]   ac_FileName   File name of the update file
@param[out]  pul_Result   Result of the analyzation:\n
              0 = Syntax and Checksum are correct\n
              10 = Checksum in update file is NOT valid\n
              100 = HEADER instruction has invalid parameters\n
              200 = FILE instruction has invalid parameter 1\n
              201 = FILE instruction has invalid parameter 2\n
              202 = FILE instruction has invalid parameter 3\n
              203 = FILE instruction has invalid parameter 4\n
              204 = FILE instruction is missing parameters\n
              205 = FILE instruction has too mnay parameters\n
              300 = NVR instruction has invalid parameter 1\n
              301 = NVR instruction has invalid parameter 2\n
              302 = NVR instruction is missing parameters\n
              303 = NVR instruction has too many parameters\n
              400 = COLDBOOT instruction has too many parameters\n
              500 = REBOOT instruction has too many parameters\n
              600 = ABORT instruction has too many parameters\n
              700 = IF instruction has invalid parameter for left part of validation\n
              701 = IF instruction has invalid parameter for operation\n
              702 = IF instruction has invalid parameter for major version\n
              703 = IF instruction has invalid parameter for minor version\n
              704 = IF instruction has invalid parameter for build number\n
              705 = IF instruction has invalid parameter construct\n
              800 = EXEC_REBOOT instruction has too many parameters

@param[out]  pul_ErrorLine   Indicates the line number of the update file where an error occured.\n
                             Indicates 0, if no error was found.

@return 0 Funcion was executed successfully\n
        1 Parameter "ac_FileName" is NULL\n
        2 Parameter "pul_Result" is NULL\n
        3 Parameter "pul_ErrorLine" is NULL\n
        4 File given in Parameter "ac_FileName" cannot be opened

@remarks n/a

@see n/a

@author Patrick Giesel @date 23.06.2011
*/
_API_EXTERN AiUInt16 _API_DLL_FUNC uw_SLAnalyzeUPDFile( AiChar *ac_FileName, 
                                                        AiUInt32 *pul_Result,
                                                        AiUInt32 *pul_ErrorLine )
{
  AiUInt16 uw_RetVal = 0;
  FILE *p_File;
  char ac_Str[500+1];
  char auc_LogStr[300];
  AiBoolean b_Abort;
  AiUInt32 ul_Line;
  AiChar ac_ErrorOut[30] = " SYNTAX ERROR (Line %d): ";
  TY_MODULE_INFO x_ModuleInfo;
  
  memset(&x_ModuleInfo, 0, sizeof(TY_MODULE_INFO));

  v_SLLogFile("\n", 1, 1);
  sprintf(auc_LogStr,"Analyzing update file <%s> \n", ac_FileName);
  v_SLLogFile(auc_LogStr, 1, 1);
  if( ac_FileName == NULL )
  {
    v_SLLogFile("   ERROR: Illegal filename!\n", 0, 1);
    uw_RetVal = 1;
  }
  else if( pul_Result == NULL )
  {
    v_SLLogFile("   ERROR: Input parameter 2 is NULL!\n", 0, 1);
    uw_RetVal = 2;
  }
  else if( pul_ErrorLine == NULL )
  {
    v_SLLogFile("   ERROR: Input parameter 3 is NULL!\n", 0, 1);
    uw_RetVal = 3;
  }
  else
  {
    p_File = fopen(ac_FileName,"r");

    if( NULL == p_File )
    {
      sprintf(auc_LogStr,"   ERROR: Update File <%s> not found!\n", ac_FileName );
      v_SLLogFile(auc_LogStr, 0, 1);
      uw_RetVal = 4;
    }
    else
    {
      rewind(p_File);

      *pul_Result = 0;
      *pul_ErrorLine = 0;

      ul_Line = 0;
      b_Abort = false;
      while( !b_Abort && (fgets(ac_Str,255,p_File) != NULL) )
      {
        ul_Line++;

        /*Eliminate End of Line (LF+CR for Windows, LF for Linux */
        AiOsRemoveLineDelimiters(ac_Str);

        uw_RetVal = uw_SLInterpreteUPDEntry( &x_ModuleInfo, ac_Str );

        if( (uw_RetVal < SL_MIN_INSTR) || (uw_RetVal > SL_MAX_INSTR) )
        {
          *pul_ErrorLine = ul_Line;

          sprintf(auc_LogStr, ac_ErrorOut, ul_Line );
          v_SLLogFile(auc_LogStr, 1, 1);

          switch( uw_RetVal )
          {
            case SL_RETVAL_ERROR_FILE_INSTR_PARAM1                :
              v_SLLogFile("Parameter 1 of FILE instruction is not valid!\n", 0, 1);
              break;
            case SL_RETVAL_ERROR_FILE_INSTR_PARAM2                :
              v_SLLogFile("Parameter 2 of FILE instruction is not valid!\n", 0, 1);
              break;
            case SL_RETVAL_ERROR_FILE_INSTR_PARAM3                :
              v_SLLogFile("Parameter 3 of FILE instruction is not valid!\n", 0, 1);
              break;
            case SL_RETVAL_ERROR_FILE_INSTR_PARAM4                :
              v_SLLogFile("Parameter 4 of FILE instruction is not valid!\n", 0, 1);
              break;
            case SL_RETVAL_ERROR_FILE_INSTR_MISSING_PARAMS        :
              v_SLLogFile("FILE instruction is missing parameters!\n", 0, 1);
              break;
            case SL_RETVAL_ERROR_FILE_INSTR_TOO_MANY_PARAMS       :
              v_SLLogFile("FILE instruction has too many parameters!\n", 0, 1);
              break;

            case SL_RETVAL_ERROR_COMPONENT_INSTR_PARAM1                :
              v_SLLogFile("Parameter 1 of FILE instruction is not valid!\n", 0, 1);
              break;
            case SL_RETVAL_ERROR_COMPONENT_INSTR_PARAM2                :
              v_SLLogFile("Parameter 2 of FILE instruction is not valid!\n", 0, 1);
              break;
            case SL_RETVAL_ERROR_COMPONENT_INSTR_PARAM3                :
              v_SLLogFile("Parameter 3 of FILE instruction is not valid!\n", 0, 1);
              break;
            case SL_RETVAL_ERROR_COMPONENT_INSTR_MISSING_PARAMS        :
              v_SLLogFile("FILE instruction is missing parameters!\n", 0, 1);
              break;
            case SL_RETVAL_ERROR_COMPONENT_INSTR_TOO_MANY_PARAMS       :
              v_SLLogFile("FILE instruction has too many parameters!\n", 0, 1);
              break;

            case SL_RETVAL_ERROR_NVR_INSTR_PARAM1                 :
              v_SLLogFile("Parameter 1 of NVR instruction is not valid!\n", 0, 1);
              break;
            case SL_RETVAL_ERROR_NVR_INSTR_PARAM2                 :
              v_SLLogFile("Parameter 2 of NVR instruction is not valid!\n", 0, 1);
              break;
            case SL_RETVAL_ERROR_NVR_INSTR_MISSING_PARAMS         :
              v_SLLogFile("NVR instruction is missing parameters!\n", 0, 1);
              break;
            case SL_RETVAL_ERROR_NVR_INSTR_TOO_MANY_PARAMS        :
              v_SLLogFile("NVR instruction has too many parameters!\n", 0, 1);
              break;
            case SL_RETVAL_ERROR_COLDBOOT_INSTR_TOO_MANY_PARAMS   :
              v_SLLogFile("COLDBOOT instruction has too many parameters!\n", 0, 1);
              break;
            case SL_RETVAL_ERROR_REBOOT_INSTR_TOO_MANY_PARAMS     :
              v_SLLogFile("REBOOT instruction has too many parameters!\n", 0, 1);
              break;
            case SL_RETVAL_ERROR_ABORT_INSTR_TOO_MANY_PARAMS      :
              v_SLLogFile("ABORT instruction has too many parameters!\n", 0, 1);
              break;
            case SL_RETVAL_ERROR_IF_INSTR_LEFT_PART               :
              v_SLLogFile("Left part of condition in IF instruction is not valid!\n", 0, 1);
              break;
            case SL_RETVAL_ERROR_IF_INSTR_OPERATION               :
              v_SLLogFile("Operation of condition in IF instruction is not valid!\n", 0, 1);
              break;
            case SL_RETVAL_ERROR_IF_INSTR_MAJ_VER                 :
              v_SLLogFile("Major version of condition in IF instruction is not valid!\n", 0, 1);
              break;
            case SL_RETVAL_ERROR_IF_INSTR_MIN_VER                 :
              v_SLLogFile("Minor version of condition in IF instruction is not valid!\n", 0, 1);
              break;
            case SL_RETVAL_ERROR_IF_INSTR_BUILD_NR                :
              v_SLLogFile("Build number of condition in IF instruction is not valid!\n", 0, 1);
              break;
            case SL_RETVAL_ERROR_IF_INSTR_PARAM_CONSTRUCT         :
              v_SLLogFile("Construction of IF instruction is not valid (check correct setting of brackets)!\n", 0, 1);
              break;
            case SL_RETVAL_ERROR_EXEC_REBOOT_INSTR_TOO_MANY_PARAMS:
              v_SLLogFile("EXEC_REBOOT instruction has too many parameters!\n", 0, 1);
              break;
            default:
              sprintf(auc_LogStr, "uw_SLAnalyzeUPDFile returned: %d \n", uw_RetVal);
              v_SLLogFile(auc_LogStr, 0, 1);
              v_SLLogFile("Found unknown instruction, check syntax!\n", 0, 1);
              break;
          }

          *pul_Result = uw_RetVal;
          b_Abort = true;
        }
        else
          uw_RetVal = 0;
      }

      if( !b_Abort )
      {
        uw_RetVal = uw_SLVerifyCSM( ac_FileName );
        if( 10 == uw_RetVal )
          *pul_Result = uw_RetVal;
      }

      if( 0 != *pul_Result )
      {
        v_SLLogFile("Update file is not valid\n", 0, 1);
      }
      
      fclose(p_File);
    }
  }
  return( uw_RetVal );
}


/*!
Calculates the real checksum of an update file and returns also the last stored checksum value.

@param[in]   ac_FileName   File name of the update file
@param[in]   b_RefreshCheckSumInFile   If true, replaces the original checksum of the update file with\n
                                       the calculated one.\n
                                       If false, leave the original checksum unchanged.
@param[out]  pul_CalculatedCheckSum   Calculated Real Checksum
@param[out]  pul_LastCheckSumFoundInFile   Last Stored Checksum Value In Update File

@return 0 Funcion was executed successfully\n
        1 Parameter "ac_FileName" is NULL\n
        2 Parameter "pul_CalculatedCheckSum" is NULL\n
        3 Parameter "pul_LastCheckSumFoundInFile" is NULL\n
        4 File given in Parameter "ac_FileName" cannot be opened\n
        5 Temporary file which is used to calculate the checksum cannot be created\n
        6 Temporary file which is used to calculate the checksum could not be opened for append\n
        7 Checksum could not be updated in update file

@remarks n/a

@see uw_SLVerifyCSM(), uw_SLUpdateCSM()

@author Patrick Giesel @date 14.06.2011
*/

_API_EXTERN AiUInt16 _API_DLL_FUNC uw_SLCalculateCSM( AiChar *ac_FileName, 
                                                      AiBoolean b_RefreshCheckSumInFile,
                                                      AiUInt32 *pul_CalculatedCheckSum, 
                                                      AiUInt32 *pul_LastCheckSumFoundInFile)
{
  AiUInt16 uw_RetVal = 0;
  FILE *p_File;
  FILE *p_FileTemp;
  char ac_FileNameTemp[255];
  AiUInt8 uc_Temp;
  char auc_Str[500+1];
  char auc_LogStr[300];
  AiBoolean b_Abort;
  AiInt32 l_NumRead;


  if( 0 != AiOsGetTempPath( ac_FileNameTemp, 240 ) )
  {
    v_SLLogFile("uw_SLCalculateCSM -> ERROR: unable to get temp path!\n", 1, 1);
    uw_RetVal = 1;
  }
  strcat( ac_FileNameTemp, "upd_tmp.upd" ); /* 11 charracters appended, so we are still below 255 */


  if( ac_FileName == NULL )
  {
    v_SLLogFile("uw_SLCalculateCSM -> ERROR: Illegal filename!\n", 1, 1);
    uw_RetVal = 1;
  }
  else if( pul_CalculatedCheckSum == NULL )
  {
    v_SLLogFile("uw_SLCalculateCSM -> ERROR: Input parameter 2 is NULL!\n", 1, 1);
    uw_RetVal = 2;
  }
  else if( pul_LastCheckSumFoundInFile == NULL )
  {
    v_SLLogFile("uw_SLCalculateCSM -> ERROR: Input parameter 3 is NULL!\n", 1, 1);
    uw_RetVal = 3;
  }
  else
  {
    p_File = fopen(ac_FileName,"r");

    if( NULL == p_File )
    {
      sprintf(auc_LogStr,"uw_SLCalculateCSM -> ERROR: Update File <%s> not found!\n", ac_FileName );
      v_SLLogFile(auc_LogStr, 1, 1);
      uw_RetVal = 4;
    }
    else
    {
      /* delete all CSM statements and create a new temporary update file */
      rewind(p_File);
      p_FileTemp = fopen(ac_FileNameTemp,"w+");
      
      if( NULL == p_FileTemp )
      {
        sprintf(auc_LogStr,"uw_SLCalculateCSM -> ERROR: Temporary update file could not be created!\n");
        v_SLLogFile(auc_LogStr, 1, 1);
        uw_RetVal = 5;
      }
      else
      {
        *pul_CalculatedCheckSum      = 0;
        *pul_LastCheckSumFoundInFile = 0;

        rewind(p_FileTemp);

        /* Read all lines of the original update file in text mode and store them in the */
        /* temporary update file excluding all CSM statements. */
        /* This has to be done in order to get the checksum which is calculated */
        /* in binary mode without the CSM lines. */
        /* In binary mode the lines cannot be well interpreted. */

        while(fgets(auc_Str,255,p_File) != NULL)
        {
          if ( strstr(auc_Str, SL_INSTR_NAME_CSM) != NULL )
          {
            sscanf( auc_Str+4, "%X", pul_LastCheckSumFoundInFile);
            continue; /* ignore CSM statements for temporary update file */
          }
          
          fputs(auc_Str, p_FileTemp);
        }
        fclose(p_FileTemp);
      
        /* open temporary update file in binary mode */
        p_FileTemp = fopen(ac_FileNameTemp,"rb");
        rewind(p_FileTemp);
        
        /* calculate checksum of the temporary update file */
        b_Abort = false;
        while(!b_Abort)
        {
          l_NumRead = (AiUInt32) fread( &uc_Temp, 1, 1, p_FileTemp ); 
          if( l_NumRead != 0 )
          {
            if ( uc_Temp != 0x0d)
            {
              *pul_CalculatedCheckSum += uc_Temp;
            }
          }
          else
            b_Abort = true;
        }

        fclose(p_FileTemp);

        if( b_RefreshCheckSumInFile )
        {
          p_FileTemp = fopen(ac_FileNameTemp,"a+");
          
          if( NULL == p_FileTemp )
          {
            sprintf(auc_LogStr,"uw_SLCalculateCSM -> ERROR: Temporary update file could not be opened for append!\n");
            v_SLLogFile(auc_LogStr, 1, 1);
            uw_RetVal = 6;
          }
          else
          {
            sprintf(auc_LogStr,"%s %08X\n", SL_INSTR_NAME_CSM, *pul_CalculatedCheckSum);
            if( EOF == fputs( auc_LogStr, p_FileTemp ) )
              uw_RetVal = 7;

            fclose( p_FileTemp );
          }
        }

        /* delete temporary file */
      }
      
      fclose(p_File);

      if( 0 == uw_RetVal )
      {
        if( b_RefreshCheckSumInFile )
        {
          /* delete original update file */
          remove( ac_FileName );

          /* rename temporary update file to original file name */
          if (0 != rename( ac_FileNameTemp, ac_FileName ) )
            uw_RetVal = 7;
          else
          {
            sprintf(auc_LogStr,"uw_SLCalculateCSM -> Checksum in update file has been updated.\n");
            v_SLLogFile(auc_LogStr, 1, 1);
          }
        }
        else
        { /* remove temporary update file */
          remove( ac_FileNameTemp );
        }
      }

      if( 7 == uw_RetVal )
      {
        sprintf(auc_LogStr,"uw_SLCalculateCSM -> ERROR: Checksum in update file could not be updated.\n");
        v_SLLogFile(auc_LogStr, 1, 1);
      }
    }
  }
  return( uw_RetVal );
}

/*!
Verifies if the checksum stored in an update file is correct.

@param[in]   ac_FileName   File name of the update file

@return 0 CheckSum is correct\n
        1 Parameter "ac_FileName" is NULL\n
        4 File given in Parameter "ac_FileName" cannot be opened\n
        5 Temporary file which is used to calculate the checksum cannot be created\n
        6 Temporary file which is used to calculate the checksum could not be opened for append\n
        10 Calculated checksum is not equal to the checksum stored in the given update file

@remarks n/a

@see uw_SLCalculateCSM(), uw_SLUpdateCSM()

@author Patrick Giesel @date 14.06.2011
*/
_API_EXTERN AiUInt16 _API_DLL_FUNC uw_SLVerifyCSM( AiChar *ac_FileName )
{
  AiUInt32 ul_CalculatedCheckSum;
  AiUInt32 ul_CheckSumInFile;
  AiUInt16 uw_RetVal;
  char auc_LogStr[300];
  
  if( (uw_RetVal = uw_SLCalculateCSM( ac_FileName, false, &ul_CalculatedCheckSum, &ul_CheckSumInFile )) == 0 )
  {
    if( ul_CheckSumInFile == ul_CalculatedCheckSum )
      uw_RetVal = 0;
    else
    {
      sprintf(auc_LogStr,"   ERROR: Checksum of update file <%s> is not valid!\n", ac_FileName);
      v_SLLogFile(auc_LogStr, 1, 1);
      sprintf(auc_LogStr,"   Please contact support!\n");
      v_SLLogFile(auc_LogStr, 1, 1);

      uw_RetVal = 10;
    }
  }
  return( uw_RetVal );
}

/*!
Updates the checksum of a given update file.

@param[in]   ac_FileName   File name of the update file

@return 0 CheckSum has been updated successfully\n
        1 Parameter "ac_FileName" is NULL\n
        4 File given in Parameter "ac_FileName" cannot be opened\n
        5 Temporary file which is used to calculate the checksum cannot be created\n
        6 Temporary file which is used to calculate the checksum could not be opened for append\n
        7 Checksum could not be updated in update file

@remarks n/a

@see uw_SLCalculateCSM(), uw_SLVerifyCSM()

@author Patrick Giesel @date 30.06.2011
*/
_API_EXTERN AiUInt16 _API_DLL_FUNC uw_SLUpdateCSM( AiChar *ac_FileName )
{
  AiUInt32 ul_CalculatedCheckSum;
  AiUInt32 ul_CheckSumInFile;
  AiUInt16 uw_RetVal;
  
  uw_RetVal = uw_SLCalculateCSM( ac_FileName, true, &ul_CalculatedCheckSum, &ul_CheckSumInFile );

  return( uw_RetVal );
}


void v_SeparateString (char *auc_Str, char c_Seperator, char auc_Params[SL_MAX_PARAMS][SL_MAX_PARAM_SIZE], unsigned char *puc_NbOfParams)
{
  int i_Length = (int) strlen(auc_Str);
  int x;
  int z = 0;
  char ac_Buf[SL_MAX_SECTION_ENTRY_SIZE];
  unsigned char uc_NbOfParams = 0;
  unsigned char b_HasFoundTextMark = false;
  char c_SeperatorText = '"';
  char c_SeperatorTmp = c_Seperator;

  for (x = 0; x < i_Length; x++)
  {
    if( uc_NbOfParams == SL_MAX_PARAMS )
      break;

    /* a text like "abcd efgh" has to be handled as one parameter. */
    /* Therefore we have to set markers where we want to look for */
    /* c_SeperatorText instead of c_Seperator as delimiter between parameters. */
    if (auc_Str[x] == '"')
    {
      if(!b_HasFoundTextMark)
      {
        b_HasFoundTextMark = true;
        c_SeperatorTmp = c_SeperatorText;
      }
      else
      {
        if (auc_Str[x + 1] != 0)
        {
          b_HasFoundTextMark = false;
          c_SeperatorTmp = c_Seperator;
        }
      }
    }

    if (auc_Str[x + 1] == 0)
    {
      if (auc_Str[x] == c_SeperatorTmp)
        ac_Buf[z] =  0;
      else
      {
        ac_Buf[z] =  auc_Str[x];
        ac_Buf[z + 1] =  0;
      }

      if (strlen (ac_Buf))
      {
        strcpy (auc_Params[uc_NbOfParams], ac_Buf);
        uc_NbOfParams++;
      }
    }
    else if (auc_Str[x] != c_SeperatorTmp)
    {
      ac_Buf[z] =  auc_Str[x];
      z++;

      if (auc_Str[x + 1] == c_SeperatorTmp)
      {
        ac_Buf[z] = 0;
        z = 0;
        if (strlen(ac_Buf))
        {
          strcpy (auc_Params[uc_NbOfParams], ac_Buf);
          uc_NbOfParams++;
        }
      }
    }
  }

  *puc_NbOfParams = uc_NbOfParams;
}

_API_EXTERN AiUInt16 _API_DLL_FUNC uw_SLCheckHeaderInstr( TY_MODULE_INFO *px_ModuleInfo,
                                                          AiChar *puc_SectionEntry,
                                                          AiUInt32 *pul_Info )
{
  AiUInt16 uw_RetVal = 100;
  AiUInt32 ul_AA;
  AiUInt32 ul_BBBBBBBB;
  AiUInt32 ul_CCCCCCCC;

  *pul_Info = SL_RETVAL_HEADER_INSTR_IS_INVALID_FOR_BOARD;

  if( NULL == puc_SectionEntry )
    uw_RetVal = SL_RETVAL_ERROR_PARAM2_IS_NULL;
  else if( NULL == pul_Info )
    uw_RetVal = SL_RETVAL_ERROR_PARAM3_IS_NULL;
  else
  {
    if( strstr(puc_SectionEntry, SL_INSTR_NAME_HEADER) != NULL )
    { /* we have found a section header */
      if( 3 == sscanf(puc_SectionEntry+1, "%x,%x,%x", &ul_AA, &ul_BBBBBBBB, &ul_CCCCCCCC ) )
        uw_RetVal = 0;

      if (ul_AA == (px_ModuleInfo->nvr.boardConfig & 0x000000FF))
      { /* we have found a header with the correct platform type */

        /* now let's have a look if the rest of the header is suitable for our board */
          if ((px_ModuleInfo->nvr.boardType & ul_BBBBBBBB) == ul_CCCCCCCC)
        { /* we have found the section header that is suitable for our board */
          /* so we can start capturing */
          *pul_Info = SL_RETVAL_HEADER_INSTR_IS_VALID_FOR_BOARD;
        }
      }
    }
  }

  return( uw_RetVal );
}

_API_EXTERN AiUInt16 _API_DLL_FUNC uw_SLCheckFileInstr( AiChar *puc_SectionEntry,
                                                        AiChar ac_FileName[20],
                                                        AiUInt32 aul_Params[3] )
{
  AiUInt16 uw_RetVal = 0;
   int      i_Result;
  char     aauc_Params[SL_MAX_PARAMS][SL_MAX_PARAM_SIZE];
  unsigned char uc_NbOfParams;

  if( NULL == puc_SectionEntry )
    uw_RetVal = SL_RETVAL_ERROR_PARAM1_IS_NULL;
  else
  {
    v_SeparateString(puc_SectionEntry, ' ', aauc_Params, &uc_NbOfParams );
    if( uc_NbOfParams < 5 )
      uw_RetVal = SL_RETVAL_ERROR_FILE_INSTR_MISSING_PARAMS;
    else if( uc_NbOfParams > 5 )
      uw_RetVal = SL_RETVAL_ERROR_FILE_INSTR_TOO_MANY_PARAMS;
    else
    {
      if( 0 == strcmp(aauc_Params[0], SL_INSTR_NAME_FILE) )
      {
        if( strstr(aauc_Params[1], "0x") != NULL )
          i_Result = sscanf(aauc_Params[1], "0x%x", &aul_Params[0] );
        else
          i_Result = sscanf(aauc_Params[1], "%d", &aul_Params[0] );

        if( 0 == i_Result )
          uw_RetVal = SL_RETVAL_ERROR_FILE_INSTR_PARAM1;
        else
        {
          if( NULL == strcpy( ac_FileName, aauc_Params[2] ) )
            uw_RetVal = SL_RETVAL_ERROR_FILE_INSTR_PARAM2;
          else
          {
            if( strstr(aauc_Params[3], "0x") != NULL )
              sscanf(aauc_Params[3], "0x%x", &aul_Params[1] );
            else
              sscanf(aauc_Params[3], "%d", &aul_Params[1] );

            if( 0 == i_Result )
              uw_RetVal = SL_RETVAL_ERROR_FILE_INSTR_PARAM3;
            else
            {
              if( strstr(aauc_Params[4], "0x") != NULL )
                sscanf(aauc_Params[4], "0x%x", &aul_Params[2] );
              else
                sscanf(aauc_Params[4], "%d", &aul_Params[2] );

              if( 0 == i_Result )
                uw_RetVal = SL_RETVAL_ERROR_FILE_INSTR_PARAM4;
            }
          }
        }
      }
    }
  }

  return( uw_RetVal );
}


_API_EXTERN AiUInt16 _API_DLL_FUNC uw_SLCheckComponentInstr( AiChar *puc_SectionEntry,
                                                             AiChar ac_FileName[20],
                                                             AiUInt32 aul_Params[2] )
{
  AiUInt16 uw_RetVal = 0;
   int      i_Result;
  char     aauc_Params[SL_MAX_PARAMS][SL_MAX_PARAM_SIZE];
  unsigned char uc_NbOfParams;

  if( NULL == puc_SectionEntry )
    uw_RetVal = SL_RETVAL_ERROR_PARAM1_IS_NULL;
  else
  {
    v_SeparateString(puc_SectionEntry, ' ', aauc_Params, &uc_NbOfParams );
    if( uc_NbOfParams < 4 )
      uw_RetVal = SL_RETVAL_ERROR_COMPONENT_INSTR_MISSING_PARAMS;
    else if( uc_NbOfParams > 4 )
      uw_RetVal = SL_RETVAL_ERROR_COMPONENT_INSTR_TOO_MANY_PARAMS;
    else
    {
      if( 0 == strcmp(aauc_Params[0], SL_INSTR_NAME_COMPONENT) )
      {
        if( strstr(aauc_Params[1], "0x") != NULL ) /* component ID to be given to MyMon */
          i_Result = sscanf(aauc_Params[1], "0x%x", &aul_Params[0] );
        else
          i_Result = sscanf(aauc_Params[1], "%d", &aul_Params[0] );

        if( 0 == i_Result )
          uw_RetVal = SL_RETVAL_ERROR_COMPONENT_INSTR_PARAM1;
        else
        {
          if( NULL == strcpy( ac_FileName, aauc_Params[2] ) ) /* Prefix */
            uw_RetVal = SL_RETVAL_ERROR_COMPONENT_INSTR_PARAM2;
          else
          {
            if( strstr(aauc_Params[3], "0x") != NULL )  /* Novram Adr */
              sscanf(aauc_Params[3], "0x%x", &aul_Params[1] );
            else
              sscanf(aauc_Params[3], "%d", &aul_Params[1] );

            if( 0 == i_Result )
              uw_RetVal = SL_RETVAL_ERROR_COMPONENT_INSTR_PARAM3;
          }
        }
      }
    }
  }

  return( uw_RetVal );
}



_API_EXTERN AiUInt16 _API_DLL_FUNC uw_SLCheckBitfileInstr(AiChar *puc_SectionEntry,
    AiChar ac_FileName[20],
    AiUInt32 aul_Params[2],
    AiChar ac_Suffix[20])
{
    int      ret = 0;
    char     aauc_Params[SL_MAX_PARAMS][SL_MAX_PARAM_SIZE];
    unsigned char uc_NbOfParams;

    if (NULL == puc_SectionEntry)
        return SL_RETVAL_ERROR_PARAM1_IS_NULL;

    /* -- split and verify parameters --- */

    v_SeparateString(puc_SectionEntry, ' ', aauc_Params, &uc_NbOfParams);

    if (uc_NbOfParams < 4)
        return SL_RETVAL_ERROR_COMPONENT_INSTR_MISSING_PARAMS;
        
    if (uc_NbOfParams > 5)
        return SL_RETVAL_ERROR_COMPONENT_INSTR_TOO_MANY_PARAMS;

    if (0 != strcmp(aauc_Params[0], SL_INSTR_NAME_BIFI))
        // this should never happen
        return SL_INSTR_COMPONENT;

    /* -- parse parameter 1: flash sector --- */
    if (strstr(aauc_Params[1], "0x") != NULL) /* Flash sector to be given to TCP */
        ret = sscanf(aauc_Params[1], "0x%x", &aul_Params[0]);
    else
        ret = sscanf(aauc_Params[1], "%d", &aul_Params[0]);

    if (0 == ret)
        return SL_RETVAL_ERROR_COMPONENT_INSTR_PARAM1;

    /* -- parse parameter 2: Prefix (File Name) --- */
    if (NULL == strcpy(ac_FileName, aauc_Params[2])) /* Prefix */
        return SL_RETVAL_ERROR_COMPONENT_INSTR_PARAM2;

    /* -- parse parameter 3: Novram Addr (File Name extension) --- */
    if (strstr(aauc_Params[3], "0x") != NULL)  /* Novram Adr */
        sscanf(aauc_Params[3], "0x%x", &aul_Params[1]);
    else
        sscanf(aauc_Params[3], "%d", &aul_Params[1]);

    if (0 == ret)
        return SL_RETVAL_ERROR_COMPONENT_INSTR_PARAM3;

    /* -- parse parameter 4: Suffix --- */
    if (uc_NbOfParams == 5)
    {
        /* use suffix from file */
        if (NULL == strcpy(ac_Suffix, aauc_Params[4]))
            return SL_RETVAL_ERROR_COMPONENT_INSTR_PARAM4;
    }
    else
    {
        /* default is an empty string */
        memset(ac_Suffix, 0, sizeof(ac_Suffix));
    }

    return(0);
}

_API_EXTERN AiUInt16 _API_DLL_FUNC uw_SLCheckPackageInstr(AiChar *puc_SectionEntry,
                                                             AiChar ac_FileName[150] )
{
  AiUInt16 uw_RetVal = 0;
  char     aauc_Params[SL_MAX_PARAMS][SL_MAX_PARAM_SIZE];
  unsigned char uc_NbOfParams;

  if( NULL == puc_SectionEntry )
    uw_RetVal = SL_RETVAL_ERROR_PARAM1_IS_NULL;
  else
  {
    v_SeparateString(puc_SectionEntry, ' ', aauc_Params, &uc_NbOfParams );
    if( uc_NbOfParams < 2 )
      uw_RetVal = SL_RETVAL_ERROR_COMPONENT_INSTR_MISSING_PARAMS;
    else if( uc_NbOfParams > 2 )
      uw_RetVal = SL_RETVAL_ERROR_COMPONENT_INSTR_TOO_MANY_PARAMS;
    else
    {
      if( 0 == strcmp(aauc_Params[0], SL_INSTR_NAME_PACKAGE) )
      {
          if( NULL == strcpy( ac_FileName, aauc_Params[1] ) ) /* Package name */
            uw_RetVal = SL_RETVAL_ERROR_COMPONENT_INSTR_PARAM2;
      }
    }
  }

  return( uw_RetVal );
}



_API_EXTERN AiUInt16 _API_DLL_FUNC uw_SLCheckNvrChkInstr(AiChar *puc_SectionEntry, AiUInt32 aul_Params[3])
{
    int retval = 0;
    AiChar   tempString[SL_MAX_SECTION_ENTRY_SIZE];
    AiUInt32 offset;
    AiUInt32 mask;
    AiUInt32 value;
    char     *stringPtr;
    char     *tokenPtr;

    if (NULL == puc_SectionEntry)
        return SL_RETVAL_ERROR_PARAM1_IS_NULL;


    strcpy(tempString, puc_SectionEntry);


    /* we assume at this point that the incoming line looks like: "NVR_CHK(offset, mask, value)" */


    stringPtr = strstr(tempString, "(");
    /* at this point pc_TempStr is "(offset, mask, value)" */


    /*** read "offset" ***/
    tokenPtr = strtok(stringPtr + 1, ",");
    if (tokenPtr == NULL)
        return SL_RETVAL_ERROR_PARAM1_IS_NULL;

    if (strstr(tokenPtr, "0x") != NULL)
        retval = sscanf(tokenPtr, " 0x%x", &offset);
    else
        retval = sscanf(tokenPtr, " %d", &offset);
    if ( retval == 0)
        return SL_RETVAL_ERROR_PARAM1_IS_NULL;


    /*** read "mask" ***/
    tokenPtr = strtok(NULL, ",");
    if (tokenPtr == NULL)
        return SL_RETVAL_ERROR_PARAM2_IS_NULL;

    if (strstr(tokenPtr, "0x") != NULL)
        retval = sscanf(tokenPtr, " 0x%x", &mask);
    else
        retval = sscanf(tokenPtr, " %d", &mask);
    if (retval == 0)
        return SL_RETVAL_ERROR_PARAM2_IS_NULL;


    /*** read "value" ***/
    tokenPtr = strtok(NULL, ",");
    if (tokenPtr == NULL)
        return SL_RETVAL_ERROR_PARAM3_IS_NULL;

    if (strstr(tokenPtr, "0x") != NULL)
        retval = sscanf(tokenPtr, " 0x%x", &value);
    else
        retval = sscanf(tokenPtr, " %d", &value);
    if (retval == 0)
        return SL_RETVAL_ERROR_PARAM3_IS_NULL;


    /*** return output parameters ***/
    aul_Params[0] = offset;
    aul_Params[1] = mask;
    aul_Params[2] = value;

    return 0;
}

_API_EXTERN AiUInt16 _API_DLL_FUNC uw_SLCheckNvrInstr( AiChar *puc_SectionEntry,
                                                       AiUInt32 aul_Params[2] )
{
  AiUInt16 uw_RetVal = 0;
  AiChar   auc_Str[SL_MAX_SECTION_ENTRY_SIZE];
  int      i_Result;
  char     aauc_Params[SL_MAX_PARAMS][SL_MAX_PARAM_SIZE];
  unsigned char uc_NbOfParams;

  if( NULL == puc_SectionEntry )
    uw_RetVal = SL_RETVAL_ERROR_PARAM1_IS_NULL;
  else
  {
    strcpy( auc_Str, puc_SectionEntry );
    auc_Str[strlen(puc_SectionEntry)] = '\0';

    v_SeparateString(auc_Str, ' ', aauc_Params, &uc_NbOfParams );
    if( uc_NbOfParams < 3 )
      uw_RetVal = SL_RETVAL_ERROR_NVR_INSTR_MISSING_PARAMS;
    else if( uc_NbOfParams > 3 )
      uw_RetVal = SL_RETVAL_ERROR_NVR_INSTR_TOO_MANY_PARAMS;
    else
    {
      if( 0 == strcmp(aauc_Params[0], SL_INSTR_NAME_NVR) )
      {
        if( strstr(aauc_Params[1], "0x") != NULL )
          i_Result = sscanf(aauc_Params[1], "0x%x", &aul_Params[0] );
        else
          i_Result = sscanf(aauc_Params[1], "%d", &aul_Params[0] );

        if( 0 == i_Result )
          uw_RetVal = SL_RETVAL_ERROR_NVR_INSTR_PARAM1;
        else
        {
          if( strstr(aauc_Params[2], "0x") != NULL )
            sscanf(aauc_Params[2], "0x%x", &aul_Params[1] );
          else
            sscanf(aauc_Params[2], "%d", &aul_Params[1] );

          if( 0 == i_Result )
            uw_RetVal = SL_RETVAL_ERROR_NVR_INSTR_PARAM2;
        }
      }
    }
  }

  return( uw_RetVal );
}


_API_EXTERN AiUInt16 _API_DLL_FUNC uw_SLCheckInstrWithTxtOut( AiChar *puc_SectionEntry,
                                                              AiChar *puc_InstrName,
                                                              AiChar ac_OutTxt[SL_MAX_PARAM_SIZE] )
{
  AiUInt16 uw_RetVal = 0;
  AiChar   auc_Str[SL_MAX_SECTION_ENTRY_SIZE];
  char     aauc_Params[SL_MAX_PARAMS][SL_MAX_PARAM_SIZE];
  unsigned char     uc_NbOfParams;

  if( NULL == puc_SectionEntry )
      uw_RetVal = SL_RETVAL_ERROR_PARAM1_IS_NULL;
  else
  {
    ac_OutTxt[0] = 0; /* by default there is no additional message */
    strcpy( auc_Str, puc_SectionEntry );

    v_SeparateString(auc_Str, ' ', aauc_Params, &uc_NbOfParams );
    if( uc_NbOfParams > 2 )
        uw_RetVal = SL_RETVAL_ERROR_ABORT_INSTR_TOO_MANY_PARAMS;
    else
    {
      if( 0 == strcmp(aauc_Params[0], puc_InstrName) )
      {
        if (uc_NbOfParams == 2)
          strcpy( ac_OutTxt, aauc_Params[1] );
      }else
          uw_RetVal = SL_RETVAL_ERROR_NO_VALID_INSTR;

    }
  }

  return( uw_RetVal );
}

_API_EXTERN AiUInt16 _API_DLL_FUNC uw_SLCheckExecRebootInstr( AiChar *puc_SectionEntry,
                                                              AiChar ac_OutTxt[SL_MAX_PARAM_SIZE] )
{
  AiUInt16 uw_RetVal;

  if( NULL == puc_SectionEntry )
    uw_RetVal = SL_RETVAL_ERROR_PARAM1_IS_NULL;
  else
    uw_RetVal = uw_SLCheckInstrWithTxtOut( puc_SectionEntry, SL_INSTR_NAME_EXEC_REBOOT, ac_OutTxt );

  switch( uw_RetVal )
  {
    case 1: uw_RetVal = SL_RETVAL_ERROR_PARAM1_IS_NULL; break;
    case 2: uw_RetVal = SL_RETVAL_ERROR_EXEC_REBOOT_INSTR_TOO_MANY_PARAMS; break;
    default: break;
  }

  return( uw_RetVal );
}

_API_EXTERN AiUInt16 _API_DLL_FUNC uw_SLCheckIfInstr( AiChar *puc_SectionEntry,
                                                      AiUInt32 aul_Params[5] )
{
  AiUInt16 uw_RetVal = 0;
  AiChar   auc_Str[SL_MAX_SECTION_ENTRY_SIZE];
  int      i_Pos;
  char     *pc_TempStr;

  if( NULL == puc_SectionEntry )
    uw_RetVal = SL_RETVAL_ERROR_PARAM1_IS_NULL;
  else
  {
    strcpy( auc_Str, puc_SectionEntry );
    auc_Str[strlen(puc_SectionEntry)] = '\0';

    if( (pc_TempStr = strstr(auc_Str, SL_INSTR_NAME_IF)) != NULL )
    {
      i_Pos = (int) strcspn( pc_TempStr, "(" );

      if( i_Pos != 2)
        uw_RetVal = SL_RETVAL_ERROR_IF_INSTR_PARAM_CONSTRUCT;
      else
      {
        i_Pos = (int) strcspn( pc_TempStr, ")" );
        if( i_Pos == strlen(pc_TempStr) )
          uw_RetVal = SL_RETVAL_ERROR_IF_INSTR_PARAM_CONSTRUCT;
        else
        {
          i_Pos = (int) strcspn( pc_TempStr, "<>=!" );
          if( i_Pos < 6)
            uw_RetVal = SL_RETVAL_ERROR_IF_INSTR_LEFT_PART;
          else
          {
            pc_TempStr = strstr( pc_TempStr, "VER" );
            pc_TempStr += 4;

            if( sscanf( pc_TempStr, "%d.", &aul_Params[2] ) != 1 )
              uw_RetVal = SL_RETVAL_ERROR_IF_INSTR_MAJ_VER;
            else if( sscanf( pc_TempStr, "%d.%d", &aul_Params[2], &aul_Params[3] ) != 2 )
              uw_RetVal = SL_RETVAL_ERROR_IF_INSTR_MIN_VER;
            else if( sscanf( pc_TempStr, "%d.%d.%d", &aul_Params[2], &aul_Params[3], &aul_Params[4] ) != 3 )
              uw_RetVal = SL_RETVAL_ERROR_IF_INSTR_BUILD_NR;
            else
            {
              if( strstr(auc_Str, "<") != NULL )
                aul_Params[1] = SL_IF_OPERATION_LESS_THAN;
              else if( strstr(auc_Str, ">") != NULL )
                aul_Params[1] = SL_IF_OPERATION_GREATER_THAN;
              else if( strstr(auc_Str, "=") != NULL )
                aul_Params[1] = SL_IF_OPERATION_EQUAL;
              else if( strstr(auc_Str, "!") != NULL )
                aul_Params[1] = SL_IF_OPERATION_NOT_EQUAL;

              if( strstr(auc_Str, sz_IfValidationNames[SL_IF_VALIDATE_TSWVER]) != NULL )
                aul_Params[0] = SL_IF_VALIDATE_TSWVER;
              else if( strstr(auc_Str, sz_IfValidationNames[SL_IF_VALIDATE_BIU1VER]) != NULL )
                aul_Params[0] = SL_IF_VALIDATE_BIU1VER;
              else if( strstr(auc_Str, sz_IfValidationNames[SL_IF_VALIDATE_BIU2VER]) != NULL )
                aul_Params[0] = SL_IF_VALIDATE_BIU2VER;
              else if( strstr(auc_Str, sz_IfValidationNames[SL_IF_VALIDATE_BIU3VER]) != NULL )
                aul_Params[0] = SL_IF_VALIDATE_BIU3VER;
              else if( strstr(auc_Str, sz_IfValidationNames[SL_IF_VALIDATE_BIU4VER]) != NULL )
                aul_Params[0] = SL_IF_VALIDATE_BIU4VER;
              else if( strstr(auc_Str, sz_IfValidationNames[SL_IF_VALIDATE_IOLCA1VER]) != NULL )
                aul_Params[0] = SL_IF_VALIDATE_IOLCA1VER;
              else if( strstr(auc_Str, sz_IfValidationNames[SL_IF_VALIDATE_IOLCA2VER]) != NULL )
                aul_Params[0] = SL_IF_VALIDATE_IOLCA2VER;
              else if( strstr(auc_Str, sz_IfValidationNames[SL_IF_VALIDATE_IOLCA3VER]) != NULL )
                aul_Params[0] = SL_IF_VALIDATE_IOLCA3VER;
              else if( strstr(auc_Str, sz_IfValidationNames[SL_IF_VALIDATE_IOLCA4VER]) != NULL )
                aul_Params[0] = SL_IF_VALIDATE_IOLCA4VER;
              else if( strstr(auc_Str, sz_IfValidationNames[SL_IF_VALIDATE_PCILCAVER]) != NULL )
                aul_Params[0] = SL_IF_VALIDATE_PCILCAVER;
              else if( strstr(auc_Str, sz_IfValidationNames[SL_IF_VALIDATE_TCPVER]) != NULL )
                aul_Params[0] = SL_IF_VALIDATE_TCPVER;
              else if( strstr(auc_Str, sz_IfValidationNames[SL_IF_VALIDATE_MONVER]) != NULL )
                aul_Params[0] = SL_IF_VALIDATE_MONVER;
            }
          }
        }
      }
    }
  }

  return( uw_RetVal );
}


AiUInt32 ul_SLVersion2AiUInt32(TY_VER_INFO x)
{
    AiUInt32 ulRetVal;

    ulRetVal = x.ul_MajorVer * 100 * 10000;
    ulRetVal += x.ul_MinorVer * 10000;
    ulRetVal += x.ul_PatchVersion;

    return ulRetVal;
}


_API_EXTERN AiUInt16 _API_DLL_FUNC uw_SLExecuteNvrChk(TY_MODULE_INFO *px_ModuleInfo, AiChar ac_Section[SL_MAX_SECTION_ENTRY_SIZE])
{
    AiUInt16 uw_RetVal = 0;
    AiUInt32 aul_Params[5];
    char auc_LogStr[300];
    AiUInt32 ul_Value = 0;

    uw_RetVal = uw_SLCheckNvrChkInstr(ac_Section, aul_Params);
    if (uw_RetVal)
        return uw_RetVal;

    uw_RetVal = SlWrapGetNovRamValue(px_ModuleInfo->ul_ModuleHandle, aul_Params[0]/*offset*/, &ul_Value);

    if (uw_RetVal)
    {
        sprintf(auc_LogStr, "   SlWrapGetNovRamValue failed with 0x%x\n", uw_RetVal);
        v_SLLogFile(auc_LogStr, 1, 1);
        sprintf(auc_LogStr, "   Please update the system driver and try again.\n");
        v_SLLogFile(auc_LogStr, 1, 1);
        return uw_RetVal;
    }

    /* check if value read from the board equals the expected value (taking the mask into account) */
    if ((aul_Params[1]/*mask*/ & aul_Params[2]/*expected value*/) == (aul_Params[1]/*mask*/ & ul_Value))
        uw_RetVal = SL_RETVAL_IF_INSTR_IS_TRUE;
    else
        uw_RetVal = SL_RETVAL_IF_INSTR_IS_FALSE;


    sprintf(auc_LogStr, "   Checking NOVRAM 0x%x 0x%x 0x%x  (%08x)\n", aul_Params[0], aul_Params[1], aul_Params[2], ul_Value); /* offset mask value*/
    v_SLLogFile(auc_LogStr, 1, 0);
    sprintf(auc_LogStr, "   %s\n", (uw_RetVal == SL_RETVAL_IF_INSTR_IS_TRUE) ? "TRUE" : "FALSE");
    v_SLLogFile(auc_LogStr, 1, 0);

    return uw_RetVal;
}


_API_EXTERN AiUInt16 _API_DLL_FUNC uw_SLExecuteIfInstr(TY_MODULE_INFO *px_ModuleInfo, AiChar ac_Section[SL_MAX_SECTION_ENTRY_SIZE])
{
    AiUInt16 uw_RetVal = 0;
    AiUInt32 aul_Params[5];
    char auc_LogStr[300];
    AiUInt32 ul_IfVersion = 0;
    AiUInt32 ul_BoardVersion = 0;

    if (NULL == ac_Section)
        return 1;


    uw_RetVal = uw_SLCheckIfInstr(ac_Section, aul_Params);
    if (0 == uw_RetVal)
    {
        ul_IfVersion = (aul_Params[2] * 100 + aul_Params[3]) * 10000 + aul_Params[4];
        switch (aul_Params[0])
        {
        case SL_IF_VALIDATE_TSWVER:
            sprintf(auc_LogStr, "   Checking TSWVER V");
            ul_BoardVersion = ul_SLVersion2AiUInt32(px_ModuleInfo->version.target_sw);
            break;
        case SL_IF_VALIDATE_BIU1VER:
            sprintf(auc_LogStr, "   Checking BIU1VER V");
            ul_BoardVersion = ul_SLVersion2AiUInt32(px_ModuleInfo->version.firmware[0]);
            break;
        case SL_IF_VALIDATE_BIU2VER:
            sprintf(auc_LogStr, "   Checking BIU2VER V");
            ul_BoardVersion = ul_SLVersion2AiUInt32(px_ModuleInfo->version.firmware[1]);
            break;
        case SL_IF_VALIDATE_BIU3VER:
            sprintf(auc_LogStr, "   Checking BIU3VER V");
            ul_BoardVersion = ul_SLVersion2AiUInt32(px_ModuleInfo->version.firmware[2]);
            break;
        case SL_IF_VALIDATE_BIU4VER:
            sprintf(auc_LogStr, "   Checking BIU4VER V");
            ul_BoardVersion = ul_SLVersion2AiUInt32(px_ModuleInfo->version.firmware[3]);
            break;
        case SL_IF_VALIDATE_IOLCA1VER:
            sprintf(auc_LogStr, "   Checking IOLCA1VER V");
            ul_BoardVersion = ul_SLVersion2AiUInt32(px_ModuleInfo->version.io_lca[0]);
            break;
        case SL_IF_VALIDATE_IOLCA2VER:
            sprintf(auc_LogStr, "   Checking IOLCA2VER V");
            ul_BoardVersion = ul_SLVersion2AiUInt32(px_ModuleInfo->version.io_lca[1]);
            break;
        case SL_IF_VALIDATE_IOLCA3VER:
            sprintf(auc_LogStr, "   Checking IOLCA3VER V");
            ul_BoardVersion = ul_SLVersion2AiUInt32(px_ModuleInfo->version.io_lca[2]);
            break;
        case SL_IF_VALIDATE_IOLCA4VER:
            sprintf(auc_LogStr, "   Checking IOLCA4VER V");
            ul_BoardVersion = ul_SLVersion2AiUInt32(px_ModuleInfo->version.io_lca[3]);
            break;
        case SL_IF_VALIDATE_PCILCAVER:
            sprintf(auc_LogStr, "   Checking PCILCAVER V");
            ul_BoardVersion = ul_SLVersion2AiUInt32(px_ModuleInfo->version.soc);
            break;
        case SL_IF_VALIDATE_TCPVER:
            sprintf(auc_LogStr, "   Checking TCPVER V");
            ul_BoardVersion = ul_SLVersion2AiUInt32(px_ModuleInfo->version.tcp);
            break;
        case SL_IF_VALIDATE_MONVER:
            sprintf(auc_LogStr, "   Checking MONVER V");
            ul_BoardVersion = ul_SLVersion2AiUInt32(px_ModuleInfo->version.monitor);
            break;
        }

        v_SLLogFile(auc_LogStr, 1, 0);

        sprintf(auc_LogStr, "%08d (%d) against (%08d) V%02d.%02d.%04d\n",
            ul_BoardVersion, aul_Params[1], ul_IfVersion, aul_Params[2], aul_Params[3], aul_Params[4]);
        v_SLLogFile(auc_LogStr, 0, 0);

        if (aul_Params[1] == SL_IF_OPERATION_LESS_THAN)
        {
            if (ul_BoardVersion < ul_IfVersion)
                uw_RetVal = SL_RETVAL_IF_INSTR_IS_TRUE;
            else
                uw_RetVal = SL_RETVAL_IF_INSTR_IS_FALSE;
        }
        else if (aul_Params[1] == SL_IF_OPERATION_GREATER_THAN)
        {
            if (ul_BoardVersion > ul_IfVersion)
                uw_RetVal = SL_RETVAL_IF_INSTR_IS_TRUE;
            else
                uw_RetVal = SL_RETVAL_IF_INSTR_IS_FALSE;
        }
        else if (aul_Params[1] == SL_IF_OPERATION_EQUAL)
        {
            if (ul_BoardVersion == ul_IfVersion)
                uw_RetVal = SL_RETVAL_IF_INSTR_IS_TRUE;
            else
                uw_RetVal = SL_RETVAL_IF_INSTR_IS_FALSE;
        }
        else if (aul_Params[1] == SL_IF_OPERATION_NOT_EQUAL)
        {
            if (ul_BoardVersion != ul_IfVersion)
                uw_RetVal = SL_RETVAL_IF_INSTR_IS_TRUE;
            else
                uw_RetVal = SL_RETVAL_IF_INSTR_IS_FALSE;
        }
        sprintf(auc_LogStr, "   %s\n", (uw_RetVal == SL_RETVAL_IF_INSTR_IS_TRUE) ? "TRUE" : "FALSE");
        v_SLLogFile(auc_LogStr, 1, 0);
    }

 
    return(uw_RetVal);
}

/*!
Interpretes the given section entries to get the corresponding instruction.

@param[in]   x_ModuleInfo   Struct that contains the information about the used board.
@param[in]   puc_SectionEntry   Points to a section entry.

@return 1 Recognized valid HEADER instruction\n
        2 Recognized valid FILE instruction\n
        3 Recognized valid NVR instruction\n
        4 Recognized valid COLDBOOT instruction\n
        5 Recognized valid REBOOT instruction\n
        6 Recognized valid ABORT instruction\n
        7 Recognized valid IF instruction\n
        8 Recognized valid EXEC_REBOOT instruction\n
        9 Recognized valid COMMENT instruction\n
        10 Recognized valid CSM instruction\n
        90 Parameter "puc_SectionEntry" is NULL\n
        99 No valid instruction was recognized\n
        100 HEADER instruction has invalid parameters\n
        200 FILE instruction has invalid parameter 1\n
        201 FILE instruction has invalid parameter 2\n
        202 FILE instruction has invalid parameter 3\n
        203 FILE instruction has invalid parameter 4\n
        204 FILE instruction is missing parameters\n
        205 FILE instruction has too many parameters\n
        300 NVR instruction has invalid parameter 1\n
        301 NVR instruction has invalid parameter 2\n
        302 NVR instruction is missing parameters\n
        303 NVR instruction has too many parameters\n
        400 COLDBOOT instruction has too many parameters\n
        500 REBOOT instruction has too many parameters\n
        600 ABORT instruction has too many parameters\n
        700 IF instruction has invalid parameter for left part of validation\n
        701 IF instruction has invalid parameter for operation\n
        702 IF instruction has invalid parameter for major version\n
        703 IF instruction has invalid parameter for minor version\n
        704 IF instruction has invalid parameter for build number\n
        705 IF instruction has invalid parameter construct\n
        800 EXEC_REBOOT instruction has too many parameters

*/
_API_EXTERN AiUInt16 _API_DLL_FUNC uw_SLInterpreteUPDEntry( TY_MODULE_INFO *px_ModuleInfo,
                                                            AiChar *puc_SectionEntry )
{
  AiInt32  iCommentPos = 0;
  AiUInt16 uw_RetVal = 0;
  AiChar  ac_ParamStr[SL_MAX_SECTION_ENTRY_SIZE] = { '\0' };
  AiChar  ac_Suffix[SL_MAX_SECTION_ENTRY_SIZE] = { '\0' };
  AiUInt32 aul_Params[5];
  AiUInt32 ul_Info;

  if( NULL == puc_SectionEntry )
    uw_RetVal = SL_RETVAL_ERROR_PARAM2_IS_NULL;
  else
  {

      
    iCommentPos = strpos( puc_SectionEntry,SL_INSTR_NAME_COMMENT );

    if( iCommentPos >= 0 )
    {
        /* Remove comment at the end of the line */
        puc_SectionEntry[iCommentPos] = '\0';
    }

    if( strlen(puc_SectionEntry) == 0 )
    {
      uw_RetVal = SL_INSTR_COMMENT;
    }
    else if( strstr(puc_SectionEntry, SL_INSTR_NAME_HEADER) != NULL )
    {
      uw_RetVal = uw_SLCheckHeaderInstr( px_ModuleInfo, puc_SectionEntry, &ul_Info );
      if( 0 == uw_RetVal )
        uw_RetVal = SL_INSTR_HEADER;
    }
    else if( strstr(puc_SectionEntry, SL_INSTR_NAME_FILE) != NULL )
    {
      uw_RetVal = uw_SLCheckFileInstr( puc_SectionEntry, ac_ParamStr, aul_Params );
      if( 0 == uw_RetVal )
        uw_RetVal = SL_INSTR_FILE;
    }
    else if( strstr(puc_SectionEntry, SL_INSTR_NAME_COMPONENT) != NULL )
    {
      uw_RetVal = uw_SLCheckComponentInstr( puc_SectionEntry, ac_ParamStr, aul_Params );
      if( 0 == uw_RetVal )
        uw_RetVal = SL_INSTR_COMPONENT;
    }
    else if (strstr(puc_SectionEntry, SL_INSTR_NAME_BIFI) != NULL)
    {
        uw_RetVal = uw_SLCheckBitfileInstr(puc_SectionEntry, ac_ParamStr, aul_Params, ac_Suffix);
        if (0 == uw_RetVal)
            uw_RetVal = SL_INSTR_BIFI;
    }
    else if (strstr(puc_SectionEntry, SL_INSTR_NAME_PACKAGE) != NULL)
    {
      uw_RetVal = uw_SLCheckPackageInstr( puc_SectionEntry, ac_ParamStr );
      if( 0 == uw_RetVal )
        uw_RetVal = SL_INSTR_PACKAGE;
    }
    else if( strstr(puc_SectionEntry, SL_INSTR_NAME_NVR_CHK) != NULL )
    {
      uw_RetVal = uw_SLCheckNvrChkInstr( puc_SectionEntry, aul_Params );
      if( 0 == uw_RetVal )
        uw_RetVal = SL_INSTR_NVR_CHK;
    }
    else if (strstr(puc_SectionEntry, SL_INSTR_NAME_NVR) != NULL)
    {
      uw_RetVal = uw_SLCheckNvrInstr(puc_SectionEntry, aul_Params);
      if (0 == uw_RetVal)
        uw_RetVal = SL_INSTR_NVR;
    }
    else if( strstr(puc_SectionEntry, SL_INSTR_NAME_COLDBOOT_R) != NULL ) /* command detection is currently done by checking for substrings, so long commands have to be checked first*/
    {
      uw_RetVal = uw_SLCheckInstrWithTxtOut(puc_SectionEntry, SL_INSTR_NAME_COLDBOOT_R, ac_ParamStr);
      if( 0 == uw_RetVal )
        uw_RetVal = SL_INSTR_COLDBOOT_R;
    }
    else if( strstr(puc_SectionEntry, SL_INSTR_NAME_COLDBOOT) != NULL ) /* command detection is currently done by checking for substrings, so long commands have to be checked first*/
    {
      uw_RetVal = uw_SLCheckInstrWithTxtOut(puc_SectionEntry, SL_INSTR_NAME_COLDBOOT, ac_ParamStr);
      if( 0 == uw_RetVal )
        uw_RetVal = SL_INSTR_COLDBOOT;
    }
    else if( strstr(puc_SectionEntry, SL_INSTR_NAME_ABORT) != NULL )
    {
      uw_RetVal = uw_SLCheckInstrWithTxtOut(puc_SectionEntry, SL_INSTR_NAME_ABORT, ac_ParamStr);
      if( 0 == uw_RetVal )
        uw_RetVal = SL_INSTR_ABORT;
    }
    else if( strstr(puc_SectionEntry, SL_INSTR_NAME_EXEC_REBOOT) != NULL )
    {
      uw_RetVal = uw_SLCheckExecRebootInstr( puc_SectionEntry, ac_ParamStr );
      if( 0 == uw_RetVal )
        uw_RetVal = SL_INSTR_EXEC_REBOOT;
    }
    else if( strstr(puc_SectionEntry, SL_INSTR_NAME_IF) != NULL )
    {
      uw_RetVal = uw_SLCheckIfInstr( puc_SectionEntry, aul_Params );
      if( 0 == uw_RetVal )
        uw_RetVal = SL_INSTR_IF;
    }
    else if( strstr(puc_SectionEntry, SL_INSTR_NAME_REBOOT_R) != NULL ) /* command detection is currently done by checking for substrings, so long commands have to be checked first*/
    {
      uw_RetVal = uw_SLCheckInstrWithTxtOut(puc_SectionEntry, SL_INSTR_NAME_REBOOT_R, ac_ParamStr);
      if( 0 == uw_RetVal )
        uw_RetVal = SL_INSTR_REBOOT_R;
    }
    else if( strstr(puc_SectionEntry, SL_INSTR_NAME_REBOOT) != NULL ) /* command detection is currently done by checking for substrings, so long commands have to be checked first*/
    {
      uw_RetVal = uw_SLCheckInstrWithTxtOut(puc_SectionEntry, SL_INSTR_NAME_REBOOT, ac_ParamStr);
      if( 0 == uw_RetVal )
        uw_RetVal = SL_INSTR_REBOOT;
    }
    else if( strstr(puc_SectionEntry, SL_INSTR_NAME_CSM) != NULL )
    {
      uw_RetVal = SL_INSTR_CSM;
    }
    else
    {
      uw_RetVal = SL_RETVAL_ERROR_NO_VALID_INSTR;
    }
  }

  return( uw_RetVal );
}

