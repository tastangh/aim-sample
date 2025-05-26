/* SPDX-FileCopyrightText: 2009-2021 AIM GmbH <info@aim-online.com> */
/* SPDX-License-Identifier: MIT  */

/*!
SmartCon.cpp
This is the SMART console program. It contains all menu related and\n
display parts as well as the entry point for the console program.

@remarks n/a

*/
/* SmartCon.cpp : Definiert den Einstiegspunkt für die Konsolenanwendung. */

#include <string.h> /* for strcpy() */
#include <ctype.h> /* for toupper() */
#include <stdio.h> /* for printf(), sscanf(), gets() */
#include <stdlib.h> /* for exit() */
#include <assert.h>
#include "Ai_cdef.h"
#include "Wrapper.h"
#include "SmartLib.h"
#include "AiOs.h"
#include "generated/SmartConVersion.h"



#define DEFAULT_LICENCE_KEY "0000-0000-0000-0000"
#define DEFAULT_SERVER "local"
#define DEFAULT_UPDATE_FILE "update.upd"


TY_MODULE_INFO ax_ModuleInfo[32];
AiUInt32       ul_AmountofModuleInfoEntries;
char           auc_LogStr[300];


/*! \brief Parses the command line arguments given on start of SmartCon
 *
 *  This function will parse all arguments that are given over via command line.
 *  All extracted options and arguments will be stored in a structure for later use.
 *  \param i_NumArguments number of arguments with which SmartCon was called
 *  \param pac_ArgumentVector array of strings with the program's arguments
 *  \param  px_ProgramSettings pointer to structure in which parsed arguments are saved
 *  \return returns 0 on success, !0 if invalid arguments were encountered
 */
AiInt32 AnalyzeProgramArgs(int i_NumArguments, char* pac_ArgumentVector[], TY_SMARTCON_SETTINGS* px_ProgramSettings );


/*! \brief Checks if a string specifies a program option.
 *
 * Criteria for an option is that string starts with '-' or '--'
 * \param ProgramArgument string to check for being an option
 * \return returns TRUE is string is a program option, FALSE otherwise.
 */
AiBoolean IsOption(const char* ProgramArgument);


/*! \brief Parses an option string.
 *
 * Parses the option plus any possible arguments to the option. The parsed settings are stored in a structure.
 * The function also returns the index of the next argument that shall be checked, as the option following
 * arguments can be arguments to the option itself and therefore must be skipped.
 * \param i_NumArgs number of arguments in argument vector
 * \param pac_ArgumentVector argument vector. Array of strings
 * \param i_OptionIndex index in the argument vector of the option to parse
 * \param px_ProgramArgs structure where parsed option settings will be stored
 * \param pi_NextArgumentIndex pointer to integer that will receive the next argument index that shall be checked
 * \return returns 0 if option is valid.
 */
int CheckOption(int i_NumArgs, char* pac_ArgumentVector[], int i_OptionIndex, TY_SMARTCON_SETTINGS* px_ProgramArgs, int* pi_NextArgumentIndex);


/*! \brief Gets the argument of an option from the argument vector
 *
 * Gets the next string, following the option, from the argument vector.
 * \param i_NumArgs total number of arguments in the argument vector.
 * \param pac_ArgumentVector argument vector. array of strings
 * \param index of the option in the argument vector
 * \return returns the next argument, following the option, NULL if there are no more arguments left.
 */
const char* GetOptionArgument(int i_NumArgs, char* pac_ArgumentVector[], int i_OptionIndex);


/*! \brief Gets the argument of an option interpreted as integer
 *
 * Gets the next string, following the option, from the argument vector and converts
 * it to an integer.
 * \param i_NumArgs total number of arguments in the argument vector.
 * \param pac_ArgumentVector argument vector. array of strings
 * \param index of the option in the argument vector
 * \param pi_OptionArgumentValue pointer to integer that will receive the interpreted value
 * \return returns 0 on success, -1 if no more arguments were left.
 */
int GetOptionArgumentInt(int i_NumArgs, char* pac_ArgumentVector[], int i_OptionIndex, unsigned int* pi_OptionArgumentValue);


/*! \brief Parses a command string that was given over as a program argument
 *
 * Checks if string is a known command and parses it. The extracted settings are stored
 * in a structure
 * \param s_Command command string to check
 * \param px_ProgramArgs structure were interpreted command settings are stored
 * \return returns 0 if command is valid.
 */
int CheckCommand(const char* s_Command, TY_SMARTCON_SETTINGS* px_ProgramArgs);



int _GetChar()
{
    int character  = 0;
    int character2 = 0;

    character = getchar();
    do
    {
        character2 = getchar();
    }
    while ( '\n' != character2);


    return character;
}


/*!
Shows a list of the software part versions of each found board.


@return n/a

@remarks n/a

@see n/a

@author Patrick Giesel @date 08.06.2010
*/
void v_ShowBoards()
{
  AiUInt32 ul_Index;




  printf("\n");
  /*      99. ACX3910-2-6U         1234 20.60.256S 02.30 02.30 01.60.1234 01.60.1234*/
  printf("    Module               SN          TSW BIU1  BIU2     LCA/SOC MON       \n");
  printf("--------------------------------------------------------------------------\n");
  if( 0 == ul_AmountofModuleInfoEntries )
    printf("-> No boards found <-");
  else
  {
    for( ul_Index=0; ul_Index<ul_AmountofModuleInfoEntries; ul_Index++ )
    {
      printf("%2d. %-20s %04d %10s ",
        ul_Index+1,
        ax_ModuleInfo[ul_Index].ac_BoardName, 
        ax_ModuleInfo[ul_Index].ul_SerialNumber,
        ax_ModuleInfo[ul_Index].version.target_sw.ac_FullVersion);

      /* -- BIU 1/2 --- */

      if (ax_ModuleInfo[ul_Index].version.firmware[0].ul_MajorVer)
        printf("%02d.%02d ", ax_ModuleInfo[ul_Index].version.firmware[0].ul_MajorVer,
                             ax_ModuleInfo[ul_Index].version.firmware[0].ul_MinorVer);
      else
        printf("  -   ");

      if (ax_ModuleInfo[ul_Index].version.firmware[1].ul_MajorVer)
        printf("%02d.%02d ", ax_ModuleInfo[ul_Index].version.firmware[1].ul_MajorVer,
                             ax_ModuleInfo[ul_Index].version.firmware[1].ul_MinorVer);
      else
        printf("  -   ");


      /* -- LCA/SOC --- */

      if( ax_ModuleInfo[ul_Index].version.soc.ul_MajorVer )
        printf("%10s ", ax_ModuleInfo[ul_Index].version.soc.ac_FullVersion);
      else
        printf("    -    ");

      /* -- Monitor --- */

      if (ax_ModuleInfo[ul_Index].version.monitor.ul_MajorVer)
          printf("%10s ", ax_ModuleInfo[ul_Index].version.monitor.ac_FullVersion);
      else
        printf("    -    ");


      printf("\n");
    }
  }

  printf("\n");
  printf("===============================================================================\n");
}

/*!
Shows the primary part of any menu display.

@param  ac_Version       Version text that will be shown in the menu
@param  ac_MenuTitle     Menu title that will be shown in the menu
@param  ac_ServerName    Server name that will be shown in the menu

@return n/a

@remarks n/a

@see n/a

@author Patrick Giesel @date 08.06.2010
*/
void v_ShowPrimaryPartOfMenu(char *ac_Version, char *ac_MenuTitle, TY_SMARTCON_SETTINGS* px_ProgramArguments)
{
  printf("\n\n\n");
  printf("===============================================================================\n");
  printf("                     SMART %s (c) AIM GmbH\n", ac_Version);
  printf("===============================================================================\n");
  printf(" %-20s                      Connected to server: <%s>\n", ac_MenuTitle, px_ProgramArguments->s_BatchServer );
  printf("                                           Using update file:   <%s>\n", px_ProgramArguments->s_BatchUpdFile);
  printf("===============================================================================\n");
}

/*!
Shows the second (special) part of the main menu.


@return n/a

@remarks n/a

@see n/a

@author Patrick Giesel @date 08.06.2010
*/
void v_ShowMainMenu(TY_SMARTCON_SETTINGS* px_ProgramSettings)
{
  v_ShowPrimaryPartOfMenu(SMART_VERSION_STRING_FULL, "MAIN MENU", px_ProgramSettings);
  v_ShowBoards();

  printf(" <F>   Change update file\n");
  printf(" <S>   Change Server\n");
  printf(" <U>   Update board\n");
  printf(" <Q>   Quit program\n");
  printf("\n");
  printf("Enter choice: ");
}

/*!
Shows the second (special) part of the update menu.


@return n/a

@remarks n/a

@see n/a

@author Patrick Giesel @date 08.06.2010
*/
void v_ShowUpdateMenu(TY_SMARTCON_SETTINGS* px_ProgramSettings)
{
  v_ShowPrimaryPartOfMenu( SMART_VERSION_STRING_FULL, "BOARD UPDATE MENU", px_ProgramSettings);
  v_ShowBoards();

  printf(" <0>               Update all boards\n");
  printf(" <Nr>              Update selected board\n");
  printf(" <any other key>   Back to main menu\n");
  printf("\n");
  printf("Enter choice: ");
}






/*!
This function handles the board update procedure.

@param  ul_ModuleId    Module identifier to be handled\n
                       0  = All boards are handled\n
                       >0 = The board with the given id is handled

@return n/a

@remarks n/a

@see n/a

@author Patrick Giesel @date 08.06.2010
*/
AiReturn UpdateBoard(AiUInt32 ul_ModuleId, TY_SMARTCON_SETTINGS* px_ProgramSettings, enum sl_post_update_actions* post_update_actions)
{
  AiUInt32 ul_ModuleIndex;
  AiUInt32 ul_MaxModuleIndex;
  AiUInt32 ul_MinModuleIndex;
  AiUInt32 ul_Result;
  AiUInt32 ul_ErrorLine;
  AiReturn ret;

  if( 0 == ul_ModuleId )
  { /* update all boards */
    ul_MinModuleIndex = 0;
    ul_MaxModuleIndex = ul_AmountofModuleInfoEntries;
  }
  else
  {
    ul_MinModuleIndex = ul_ModuleId-1;
    ul_MaxModuleIndex = ul_ModuleId;
  }

  ret = uw_SLAnalyzeUPDFile(px_ProgramSettings->s_BatchUpdFile, &ul_Result, &ul_ErrorLine );
  if (ret)
  {
      return ret;
  }

  for( ul_ModuleIndex=ul_MinModuleIndex; ul_ModuleIndex<ul_MaxModuleIndex; ul_ModuleIndex++ )
  {
      ret = uw_SLUpdateBoard( ax_ModuleInfo[ul_ModuleIndex].ul_ModuleHandle, px_ProgramSettings->s_BatchServer,
                                      px_ProgramSettings->s_BatchUpdFile, px_ProgramSettings->b_Force, post_update_actions );
      if (ret)
      {
            break;
      }
  }

  return ret;

}

/*!
This function handles the refresh of the checksum in the update file.

@param  n/a

@return n/a

@remarks n/a

@see n/a

@author Patrick Giesel @date 30.06.2010
*/
void v_RefreshCheckSum(char* s_UpdateFilePath)
{
  AiUInt16 uw_RetVal;

  uw_RetVal = uw_SLUpdateCSM(s_UpdateFilePath);
}


/*!
This function switches a given board from EFA to EFEX mode or vice-versa.

@param  ul_ModuleId            Module identifier to be handled\n
                               0  = All EFA/EFEX boards are handled\n
                               >0 = The EFA/EFEX board with the given id is handled
@param  uc_AskForLicenceKey    0 = Do not check for licence key\n
                               1 = Check for licence key

@return n/a

@remarks n/a

@see n/a

@author Patrick Giesel @date 08.06.2010
*/
/*
//TODO: SwitchBoard will have to be redesigned.
  design idea currently favoured in the driver team:
      1) function in wrapper to get the amount of modes to switch to
      2) function in wrapper to get the the description for each mode
      3) a generic switch function that allows to switch (according to the two functions above)
*/
void v_SwitchBoard(AiUInt32 ul_ModuleId, TY_SMARTCON_SETTINGS* px_ProgramSettings, AiUInt8 uc_AskForLicenceKey)
{
  AiUInt32 ul_ModuleIndex;
  AiUInt8  uc_Result;
  AiUInt32 ul_MaxModuleIndex;
  AiUInt32 ul_MinModuleIndex;

  if( 0 == ul_ModuleId )
  { /* update all boards */
    ul_MinModuleIndex = 0;
    ul_MaxModuleIndex = ul_AmountofModuleInfoEntries;
  }
  else
  {
    ul_MinModuleIndex = ul_ModuleId-1;
    ul_MaxModuleIndex = ul_ModuleId;
  }

  for( ul_ModuleIndex=ul_MinModuleIndex; ul_ModuleIndex<ul_MaxModuleIndex; ul_ModuleIndex++ )
  {
    ul_SLSwitchBoard(ax_ModuleInfo[ul_ModuleIndex].ul_ModuleHandle, &uc_Result);
  }
}

void v_NvrDumpBoard(AiUInt32 ul_ModuleId, TY_SMARTCON_SETTINGS* px_ProgramSettings)
{
    AiUInt32 ul_ModuleIndex;
    AiUInt32 ul_MaxModuleIndex;
    AiUInt32 ul_MinModuleIndex;

    if (0 == ul_ModuleId)
        { /* update all boards */
        ul_MinModuleIndex = 0;
        ul_MaxModuleIndex = ul_AmountofModuleInfoEntries;
    }
    else
    {
        ul_MinModuleIndex = ul_ModuleId-1;
        ul_MaxModuleIndex = ul_ModuleId;
    }

    for (ul_ModuleIndex=ul_MinModuleIndex; ul_ModuleIndex<ul_MaxModuleIndex; ul_ModuleIndex++)
    {
        ul_SLNvrDump(ax_ModuleInfo[ul_ModuleIndex].ul_ModuleHandle);
    }
}


/*!
This function closes all modules.


@return n/a

@remarks n/a

@see n/a

@author Patrick Giesel @date 08.06.2010
*/
void v_CloseAllModules()
{
  AiUInt32             ul_ModuleIndex;

  for( ul_ModuleIndex=0; ul_ModuleIndex<ul_AmountofModuleInfoEntries ; ul_ModuleIndex++ )
      SlWrapCloseModule(ax_ModuleInfo[ul_ModuleIndex].ul_ModuleHandle);
}


/*!
This function checks if the given serial number is included in a list of batch parameters.

@param  px_Result           List of batch parameters
@param  ul_BoardSerialNr    Serial number that is to be checked

@return Returns "AiTrue", if given serial number was found\n
        Returns "false", if given serial number was not found

@remarks If the batch parameter list does not contain any serial number, this function
         always returns "AiTrue" because any serial number has to be handled.

@see n/a

@author Patrick Giesel @date 08.06.2010
*/
unsigned char b_CheckIfModuleIsAffected(TY_SMARTCON_SETTINGS *px_Result, AiUInt32 ul_BoardSerialNr )
{
  AiUChar uc_ModuleIsAffected = AiFalse;
  AiUInt32 ul_Index;

  if( px_Result->ul_NrOfSn == 0 )
    uc_ModuleIsAffected = AiTrue;
  else
  {
    for( ul_Index=0; ul_Index<px_Result->ul_NrOfSn; ul_Index++ )
    {
      if( px_Result->aul_Sn[ul_Index] == ul_BoardSerialNr )
        uc_ModuleIsAffected = AiTrue;
    }
  }

  return uc_ModuleIsAffected;
}

/*!
This function handles all command line parameters of SMART given in the\n
list "px_Result".

@param  px_Result    List of batch parameters given at command line

@return n/a

@remarks n/a

@see n/a

@author Patrick Giesel @date 08.06.2010
*/
AiReturn HandleBatchMode( TY_SMARTCON_SETTINGS* px_ProgramSettings )
{
    AiUInt32 ul_Index;
    char s_LogString[AI_OS_MAX_PATH];
    enum sl_post_update_actions post_update_actions = SL_POST_UPDATE_NONE;
    int i;
    char str[255];
    AiReturn err;

    v_SLLogFile("Handling batch mode with following settings:\n", 1, 0);

    AiOsSnprintf(s_LogString, sizeof(s_LogString), AiTrue, "Command: %d\n", px_ProgramSettings->ul_BatchMode);
    v_SLLogFile(s_LogString, 1, 0);

    AiOsSnprintf(s_LogString, sizeof(s_LogString), AiTrue, "Server: %s\n", px_ProgramSettings->s_BatchServer);
    v_SLLogFile(s_LogString, 1, 0);

    AiOsSnprintf(s_LogString, sizeof(s_LogString), AiTrue, "Update File: %s\n", px_ProgramSettings->s_BatchUpdFile);
    v_SLLogFile(s_LogString, 1, 0);

    AiOsSnprintf(s_LogString, sizeof(s_LogString), AiTrue, "Licence key: %s\n", px_ProgramSettings->s_BatchLicenceKey);
    v_SLLogFile(s_LogString, 1, 0);

    AiOsSnprintf(s_LogString, sizeof(s_LogString), AiTrue, "Force: %d\n", px_ProgramSettings->b_Force);
    v_SLLogFile(s_LogString, 1, 0);

    AiOsSnprintf(s_LogString, sizeof(s_LogString), AiTrue, "Mode Switch ID: %d\n", px_ProgramSettings->ul_ModeSwitchID);
    v_SLLogFile(s_LogString, 1, 0);

    AiOsSnprintf(s_LogString, sizeof(s_LogString), AiTrue, "Serial: %d\n", px_ProgramSettings->aul_Sn[0]);
    v_SLLogFile(s_LogString, 1, 0);

    /* Handle global, non-board specific commands */
    switch(px_ProgramSettings->ul_BatchMode)
    {
    case SL_BATCHMODE_UPDATE_CHECKSUM:
        v_RefreshCheckSum(px_ProgramSettings->s_BatchUpdFile);
        return 0;

    case SL_BATCHMODE_HELP:
        v_SLShowUsage(SMART_VERSION_STRING_FULL);
        return 0;
        
    default:
        break;
    }

    /* Now handle board specific commands */
    ul_AmountofModuleInfoEntries = uw_SLGetAmountOfModuleInfos(px_ProgramSettings->s_BatchServer);

    for (ul_Index = 0; ul_Index < ul_AmountofModuleInfoEntries; ul_Index++)
    {
        uw_SLGetModuleInfo(px_ProgramSettings->s_BatchServer, ul_Index, &ax_ModuleInfo[ul_Index]);

        if (b_CheckIfModuleIsAffected(px_ProgramSettings, ax_ModuleInfo[ul_Index].ul_SerialNumber))
        {
            switch (px_ProgramSettings->ul_BatchMode)
            {
            case SL_BATCHMODE_MODE_SWITCH:
                v_SwitchBoard(ul_Index + 1, px_ProgramSettings, 0);
                break;

            case SL_BATCHMODE_UPDATE:
                err = UpdateBoard(ul_Index + 1, px_ProgramSettings, &post_update_actions);
                if (err)
                {
                    v_SLLogFile("Update of board failed. Aborting program\n", 1, 1);
                    return err;
                }
                break;

            case SL_BATCHMODE_NVR_DUMP:
                v_NvrDumpBoard(ul_Index + 1, px_ProgramSettings);
                break;

            default:
                v_SLLogFile("Invalid batch mode\n", 1, 0);
                break;
            }
        }
    }

    /* generate final message  */
    if (post_update_actions)
        v_SLLogFile("\n\nSummary:\n", 1, 1);



    if (post_update_actions & SL_POST_UPDATE_SMART_RESTART){
        v_SLLogFile("\n", 1, 1);
        v_SLLogFile("   #########################################################\n", 1, 1);
        v_SLLogFile("   #\n", 1, 1);
        v_SLLogFile("   # IMPORTANT:\n", 1, 1);
        v_SLLogFile("   # The update requires to start SMART again,\n", 1, 1);
        v_SLLogFile("   # after the system has been restarted.\n", 1, 1);
        v_SLLogFile("   #\n", 1, 1);
        v_SLLogFile("   #########################################################\n", 1, 1);


        if (px_ProgramSettings->b_autoboot)
        {
            strcpy(str, " ");

            for (i = 1; i < px_ProgramSettings->argc; i++)
            {
                strcat(str, "\"");
                strcat(str, px_ProgramSettings->argv[i]);
                strcat(str, "\" ");
            }

            SLRegisterRestart(str);
        }
    }

    if (post_update_actions & SL_POST_UPDATE_POWEROFF){
        v_SLLogFile("\n", 1, 1);
        v_SLLogFile("   #########################################################\n", 1, 1);
        v_SLLogFile("   #\n", 1, 1);
        v_SLLogFile("   # The update requires a cold boot of your system to finish.\n", 1, 1);
        v_SLLogFile("   # Please shut down your computer and wait for 10 seconds.\n", 1, 1);
        v_SLLogFile("   # After that please power on your system.\n", 1, 1);
        v_SLLogFile("   #\n", 1, 1);
        v_SLLogFile("   #########################################################\n", 1, 1);

        if (px_ProgramSettings->b_autoboot)
        {
            SLShutdown(SL_POST_UPDATE_POWEROFF);
        }
    }
    else {
        if (post_update_actions & SL_POST_UPDATE_REBOOT){
            v_SLLogFile("\n", 1, 1);
            v_SLLogFile("   #########################################################\n", 1, 1);
            v_SLLogFile("   #\n", 1, 1);
            v_SLLogFile("   # The update requires to reboot the system.\n", 1, 1);
            v_SLLogFile("   #\n", 1, 1);
            v_SLLogFile("   #########################################################\n", 1, 1);

            if (px_ProgramSettings->b_autoboot)
            {
                SLShutdown(SL_POST_UPDATE_REBOOT);
            }
        }
    }

    v_CloseAllModules();

    return 0;
}


/*!
This is the main entry point of SMART

@param  argc    Number of arguments given at command line
@param  argv    List of command line parameters given at command line

@return 0 = Exit succesfully\n
        1 = Exit with error "any command line parameter was wrong"

@remarks n/a

@see n/a

@author Patrick Giesel @date 08.06.2010
*/

#ifdef _WIN32
int __cdecl main( int argc, char *argv[])
#else
int main( int argc, char *argv[])
#endif
{
  AiUInt32             ul_RetVal;
  AiUChar              uc_Key1 = ' '; 
  AiUChar              uc_Key2 = ' '; 
  TY_SMARTCON_SETTINGS  x_ProgramSettings;
  AiUInt32             ul_Index;
  char strOut[100];
  enum sl_post_update_actions post_update_actions = SL_POST_UPDATE_NONE;

  v_SLLogFile("***********************************************************\n", 1, 0);
  sprintf(strOut, "Starting SMART %s\n", SMART_VERSION_STRING_FULL);
  v_SLLogFile(strOut, 1, 0);
  v_SLLogFile("***********************************************************\n", 1, 0);

  memset(&x_ProgramSettings, 0, sizeof(x_ProgramSettings));

  x_ProgramSettings.argc = argc;
  x_ProgramSettings.argv = argv;

  strncpy(x_ProgramSettings.s_BatchLicenceKey, DEFAULT_LICENCE_KEY, sizeof(x_ProgramSettings.s_BatchLicenceKey) - 1);
  strncpy(x_ProgramSettings.s_BatchServer, DEFAULT_SERVER, sizeof(x_ProgramSettings.s_BatchServer) - 1);
  strncpy(x_ProgramSettings.s_BatchUpdFile, DEFAULT_UPDATE_FILE, sizeof(x_ProgramSettings.s_BatchUpdFile) - 1);

  ul_RetVal = AnalyzeProgramArgs(argc, argv, &x_ProgramSettings);
  if(ul_RetVal)
  {
      printf("\n\n");
	  v_SLShowUsage(SMART_VERSION_STRING_FULL);
      exit(-1);
  }

  if(x_ProgramSettings.b_BatchExec)
  {
    exit(HandleBatchMode(&x_ProgramSettings));
  }

  ul_AmountofModuleInfoEntries = uw_SLGetAmountOfModuleInfos(x_ProgramSettings.s_BatchServer);

  for(ul_Index=0; ul_Index<ul_AmountofModuleInfoEntries; ul_Index++)
    uw_SLGetModuleInfo(x_ProgramSettings.s_BatchServer, ul_Index, &ax_ModuleInfo[ul_Index]);

  do
  {
    v_ShowMainMenu(&x_ProgramSettings);

    uc_Key1 = _GetChar();
    uc_Key1 = toupper( uc_Key1 );

    switch( uc_Key1 )
    {
      case 'U':
        v_ShowUpdateMenu(&x_ProgramSettings);
        uc_Key2 = _GetChar();
        if( (uc_Key2 >= '0') && (uc_Key2 <= '9') )
        {
          UpdateBoard( uc_Key2-'0', &x_ProgramSettings, &post_update_actions);
        }
        break;

/*
see comment above function v_SwitchBoard
      case 'E': // switch 3910 board to EFEX or vice versa
        v_ShowEfaEfexSwitchMenu();
        uc_Key2 = _GetChar();
        if( (uc_Key2 >= '0') && (uc_Key2 <= '9') )
        {
          v_SwitchBoard(uc_Key2-'0', 1);
        }
        break;
*/

      case 'S': /* switch server */
        printf("\nEnter new server name (or ip address) to connect to: ");
        AiOsGetString(x_ProgramSettings.s_BatchServer, sizeof(x_ProgramSettings.s_BatchServer), "local");
        v_CloseAllModules();
        break;

      case 'F': /* change update file */
        printf("\nEnter new update file: ");
        AiOsGetString(x_ProgramSettings.s_BatchUpdFile, sizeof(x_ProgramSettings.s_BatchUpdFile), "update.upd");
        break;

      case 'C':
        v_RefreshCheckSum(x_ProgramSettings.s_BatchUpdFile);
        break;

      case 'Q':
      default:
        break;
    }
  }
  while( uc_Key1 != 'Q' );

  v_CloseAllModules();

  return 0;
}


AiInt32 AnalyzeProgramArgs(int i_NumArguments, char* pac_ArgumentVector[], TY_SMARTCON_SETTINGS* px_ProgramSettings )
{
    int i_CurrentArgIndex;
    AiBoolean b_IsOption;
    int i_ValidArgument;
    char s_LogString[100];

    /* Iterate over all program arguments that were given over via command line
     * argument at index 0 is program name, can be skipped
     */
    i_CurrentArgIndex = 1;
    while(i_CurrentArgIndex < i_NumArguments)
    {
        /* Check if argument is an option or a command */
        b_IsOption = IsOption(pac_ArgumentVector[i_CurrentArgIndex]);
        if(b_IsOption)
        {
            /* Parse option */
            i_ValidArgument = CheckOption(i_NumArguments, pac_ArgumentVector, i_CurrentArgIndex, px_ProgramSettings, &i_CurrentArgIndex);
            if(i_ValidArgument)
            {
                sprintf(s_LogString, "\nInvalid option: %s.\n", pac_ArgumentVector[i_CurrentArgIndex]);
                v_SLLogFile(s_LogString, 1, 1);
                return -1;
            }
        }
        else
        {
            /* Parse command */
            i_ValidArgument = CheckCommand(pac_ArgumentVector[i_CurrentArgIndex], px_ProgramSettings);
            if(i_ValidArgument)
            {
                sprintf(s_LogString, "\nInvalid command: %s\n", pac_ArgumentVector[i_CurrentArgIndex]);
                v_SLLogFile(s_LogString, 1, 1);
                return -1;
            }
        }

        i_CurrentArgIndex++;
    }

    return 0;
}


AiBoolean IsOption(const char* ProgramArgument)
{
    size_t NumHyphens;

    /* Count with how many '-' characters the argument starts.
     * If one or two this means it is an option
     */
    NumHyphens = strspn(ProgramArgument, "-");

    if(NumHyphens > 0 && NumHyphens <=2)
    {
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}


int CheckOption(int i_NumArgs, char* pac_ArgumentVector[], int i_OptionIndex, TY_SMARTCON_SETTINGS* px_ProgramArgs, int* pi_NextArgumentIndex)
{
    int i_Result = -1;
    const char* s_Option = pac_ArgumentVector[i_OptionIndex];
    const char* s_OptionArgument = NULL;
    AiBoolean b_SkipNextArgument = FALSE;

    assert(pi_NextArgumentIndex != NULL);
    assert(px_ProgramArgs != NULL);

    /* Check if option string matches any known option,
     * check any possible arguments to the option and also mark the arguments to be skipped for
     * further processing then */
    if(!strcmp(s_Option, "--update-file") || !strcmp(s_Option, "-u"))
    {
        s_OptionArgument = GetOptionArgument(i_NumArgs, pac_ArgumentVector, i_OptionIndex);
        if(s_OptionArgument == NULL)
        {
            fprintf(stderr, "\nNo update file path specified!\n");
            return -1;
        }

        b_SkipNextArgument = TRUE;
        strncpy(px_ProgramArgs->s_BatchUpdFile, s_OptionArgument, sizeof(px_ProgramArgs->s_BatchUpdFile) - 1);
    }
    else if(!strcmp(s_Option, "--serial") || !strcmp(s_Option, "-s"))
    {
        i_Result = GetOptionArgumentInt(i_NumArgs, pac_ArgumentVector, i_OptionIndex, &(px_ProgramArgs->aul_Sn[0]));
        if(i_Result < 0 || px_ProgramArgs->aul_Sn[0] <= 0)
        {
            fprintf(stderr, "\nInvalid serial number specified!\n");
            return -1;
        }
        
        px_ProgramArgs->ul_NrOfSn++;
        b_SkipNextArgument = TRUE;
    }
    else if(!strcmp(s_Option, "--licence-key") || !strcmp(s_Option, "-l"))
    {
        s_OptionArgument = GetOptionArgument(i_NumArgs, pac_ArgumentVector, i_OptionIndex);
        if(s_OptionArgument == NULL)
        {
            fprintf(stderr, "\nNo licence key specified!\n");
            return -1;
        }

        b_SkipNextArgument = TRUE;
        strncpy(px_ProgramArgs->s_BatchLicenceKey, s_OptionArgument, sizeof(px_ProgramArgs->s_BatchLicenceKey) - 1);
        i_Result = sscanf(px_ProgramArgs->s_BatchLicenceKey, "%04X-%04X-%04X-%04X", &px_ProgramArgs->aul_LicenceKey[0],
                          &px_ProgramArgs->aul_LicenceKey[1], &px_ProgramArgs->aul_LicenceKey[2], &px_ProgramArgs->aul_LicenceKey[3]);
        if(i_Result != 4)
        {
            fprintf(stderr, "\nInvalid licence key format: Expected XXXX-XXXX-XXXX-XXXX\n");
            return -1;
        }
    }
    else if(!strcmp(s_Option, "--force") || !strcmp(s_Option, "-f"))
    {
        px_ProgramArgs->b_Force = TRUE;
    }
    else if (!strcmp(s_Option, "--autoboot") || !strcmp(s_Option, "-a"))
    {
        px_ProgramArgs->b_autoboot = TRUE;
    }
    else if(!strcmp(s_Option, "--mode") || !strcmp(s_Option, "-m"))
    {
        i_Result = GetOptionArgumentInt(i_NumArgs, pac_ArgumentVector, i_OptionIndex, &px_ProgramArgs->ul_ModeSwitchID);
        if(i_Result < 0)
        {
            fprintf(stderr, "\nNo mode switch ID specified!\n");
            return -1;
        }

        b_SkipNextArgument = TRUE;
    }
    else if(!strcmp(s_Option, "--server"))
    {
        s_OptionArgument = GetOptionArgument(i_NumArgs, pac_ArgumentVector, i_OptionIndex);
        if(s_OptionArgument == NULL)
        {
            fprintf(stderr, "\nNo remote server specified!\n");
            return -1;
        }

        b_SkipNextArgument = TRUE;
        strncpy(px_ProgramArgs->s_BatchServer, s_OptionArgument, sizeof(px_ProgramArgs->s_BatchServer) - 1);
    }
    else
    {
        return -1;
    }

    if(b_SkipNextArgument)
        *pi_NextArgumentIndex = i_OptionIndex + +1;

    return 0;
}


const char* GetOptionArgument(int i_NumArgs, char* pac_ArgumentVector[], int i_OptionIndex)
{
    const char* s_OptionArgument;

    /* Check if there is still one argument left in the vector */
    if(i_OptionIndex + 1 >= i_NumArgs)
    {
        return NULL;
    }

    s_OptionArgument = pac_ArgumentVector[i_OptionIndex + 1];

    /* Assume that argument of an option must not be another option */
    if(IsOption(s_OptionArgument))
    {
        return NULL;
    }

    return s_OptionArgument;
}


int GetOptionArgumentInt(int i_NumArgs, char* pac_ArgumentVector[], int i_OptionIndex, unsigned int* pi_OptionArgumentValue)
{
    const char* s_OptionArgument;

    assert(pi_OptionArgumentValue != NULL);

    /* Check if there is still one argument left in the vector */
    if(i_OptionIndex + 1 >= i_NumArgs)
    {
        return -1;
    }

    s_OptionArgument = pac_ArgumentVector[i_OptionIndex + 1];

    *pi_OptionArgumentValue = atoi(s_OptionArgument);

    return 0;
}


int CheckCommand(const char* s_Command, TY_SMARTCON_SETTINGS* px_ProgramArgs)
{
    assert(px_ProgramArgs != NULL);

    /* Check if string is a known command and set the
     * corresponding values.
     */
    if(!strcmp(s_Command, "update"))
    {
        px_ProgramArgs->ul_BatchMode = SL_BATCHMODE_UPDATE;
    }
    else if(!strcmp(s_Command, "show-modes"))
    {
        v_SLLogFile("show-modes command not yet implemented\n", 1, 1);
        return -1;
    }
    else if(!strcmp(s_Command, "switch-mode"))
    {
        px_ProgramArgs->ul_BatchMode = SL_BATCHMODE_MODE_SWITCH;
    }
    else if (!strcmp(s_Command, "update-checksum"))
    {
        px_ProgramArgs->ul_BatchMode = SL_BATCHMODE_UPDATE_CHECKSUM;
    }
    else if (!strcmp(s_Command, "help"))
    {
        px_ProgramArgs->ul_BatchMode = SL_BATCHMODE_HELP;
    }
    else if (!strcmp(s_Command, "nvr-dump"))
    {
        px_ProgramArgs->ul_BatchMode = SL_BATCHMODE_NVR_DUMP;
    }
    else
    {
        return -1;
    }

    /* tell caller that correct command was given and no main menu shall be shown */
    px_ProgramArgs->b_BatchExec = TRUE;

    return 0;
}
