/* SPDX-FileCopyrightText: 2011-2021 AIM GmbH <info@aim-online.com> */
/* SPDX-License-Identifier: MIT  */

/*!
SmartLibAutoUpdateWindows.c
Contains the interface functions for the SmartLib library to automate the reboots required during update
*/

#ifdef _AIM_WINDOWS
#include "Windows.h"
#endif

#include <stdio.h>
#include "Ai_cdef.h"
#include "Ai_def.h"
#include "Wrapper.h"
#include "SmartLib.h"
#include "AiOs.h"







_API_EXTERN void _API_DLL_FUNC  SLShutdown(enum sl_post_update_actions shutdown_flags)
{
    char key = 0;
    DWORD retval;
    char auc_LogStr[200];
    const char * pczMenu = "\n\nThe system is now shutting down to continue the board update process\n" \
        "Press 'p' - to postpone shutdown\n" \
        "Press 'n' - to shutdown now\n";

    DWORD dwShutdownFlags;

    HANDLE hToken;
    TOKEN_PRIVILEGES tkp;

    // Get a token for this process. 
    // Seems to be not required on Windows 10 (but for example for Windows 7)

    if (!OpenProcessToken(GetCurrentProcess(),
        TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken))
        return;

    // Get the LUID for the shutdown privilege. 

    LookupPrivilegeValue(NULL, SE_SHUTDOWN_NAME,
        &tkp.Privileges[0].Luid);

    tkp.PrivilegeCount = 1;  // one privilege to set    
    tkp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;

    // Get the shutdown privilege for this process. 

    AdjustTokenPrivileges(hToken, FALSE, &tkp, 0,
        (PTOKEN_PRIVILEGES)NULL, 0);

    if (GetLastError() != ERROR_SUCCESS)
        return;


    /* convert input shutdown_flags to Windows flags */
    switch (shutdown_flags)
    {
    case SL_POST_UPDATE_POWEROFF:
        dwShutdownFlags = SHUTDOWN_RESTARTAPPS;
        break;

    case SL_POST_UPDATE_REBOOT:
        dwShutdownFlags = SHUTDOWN_RESTARTAPPS | SHUTDOWN_RESTART;
        break;

    default:
        return;
    }


    sprintf(auc_LogStr, "InitiateShutdown with flags %08x\n", dwShutdownFlags);
    v_SLLogFile(auc_LogStr, 1, 0);

    retval = InitiateShutdown(NULL, NULL, 30, dwShutdownFlags, 0);
    if (retval != ERROR_SUCCESS)
    {
        sprintf(auc_LogStr, "InitiateShutdown returned %08x\n", retval);
        v_SLLogFile(auc_LogStr, 1, 1);
    }

    /* --- Print error menu -- */
    printf("%s", pczMenu);

    do
    {
        key = AiOsGetChar();

        printf("%c\r\n", key);

        if (key == 'q')
            return;

        /* -- install selected error --- */
        switch (key)
        {
        case 'n':
            AbortSystemShutdown(NULL);
            retval = InitiateShutdown(NULL, NULL, 1, dwShutdownFlags | SHUTDOWN_FORCE_OTHERS, 0);
            if (retval != ERROR_SUCCESS)
            {
                sprintf(auc_LogStr, "InitiateShutdown returned %08x\n", retval);
                v_SLLogFile(auc_LogStr, 1, 1);
            }
            else
            {
                v_SLLogFile("shutting down now!\n\n", 1, 1);
            }
            return;
        case 'p':
            if (!AbortSystemShutdown(NULL))
            {
                sprintf(auc_LogStr, "AbortSystemShutdown failed: GetLastError provides %08x\n", GetLastError());
                v_SLLogFile(auc_LogStr, 1, 1);
            }
            else
            {
                v_SLLogFile("shutdown postponed!\n\n", 1, 1);
            }
            return;
        default:
            // invalid choice
            break;
        }
    } while (AiTrue);
}


_API_EXTERN void _API_DLL_FUNC  SLRegisterRestart(char * command_line)
{
    wchar_t pwc[255];

    v_SLLogFile("v_SLRegisterRestart\n", 1, 0);
    v_SLLogFile(command_line, 1, 0);
    v_SLLogFile("\n", 1, 0);


    mbstowcs(pwc, command_line, 255);

    RegisterApplicationRestart(pwc, 0);
}