/* SPDX-FileCopyrightText: 2013-2023 AIM GmbH <info@aim-online.com> */
/* SPDX-License-Identifier: MIT  */

/*! \file ANS_Startup.c
 *
 * This file contains common definitions for
 * starting the ANS either as a service, or as a console application
 *
 */

#include "ANS_Startup.h"
#include "ANS_Thread.h"
#include "ANS_Log.h"
#include "ANS_Server.h"
#include <stdlib.h>

#ifdef WIN32
    #include <Windows.h>
#endif


int AnsServerStart(void)
{
    int rc;
    ANSThreadStatus threadStatus;
    int threadIndex = -1;
    int threadUdpIndex = -1;
    void* threadRc = (void *)EXIT_SUCCESS;

    AnsServer_init(&g_AnsServer);

    /* Initialize the thread table. */
    threadStatus = ANSThread_initializeModule();
    if ( ANSThreadStatusOK != threadStatus )
    {
        ANSLogError("ANSThread_initalizeModule error (%d)!", threadStatus);
        return EXIT_FAILURE;
    }

    rc = ntsocket_startup();
    if ( 0 != rc )
    {
        ANSLogError("ERROR: ntsocket_startup failed with error %d!",rc );
        return EXIT_FAILURE;
    }

    /*
     * Start the server thread. Any further threads will be created from
     * this server thread.
     */
    ANSLogInfo("Starting server thread...");

    /* Check System for Boards */

    threadStatus = ANSThread_createThread(AnsServer_receive_broadcast_request, NULL, ANSThreadTypeNone, AiTrue, &threadUdpIndex);

    if (ANSThreadStatusOK != threadStatus)
    {
        ANSLogError("ERROR: ANSThread_createThread error (%d) for UDP packets!", (int)threadStatus);
    }


    threadStatus = ANSThread_createThread(AnsServer_thread, NULL, ANSThreadTypeServer,
                                          AiFalse, &threadIndex);
    if ( ANSThreadStatusOK != threadStatus )
    {
        ANSLogError("ERROR: ANSThread_createThread error (%d)!", (int) threadStatus);
        return EXIT_FAILURE;
    }

    /* Wait for the server thread to terminate. */
    rc = ANSThread_join(threadIndex, &threadRc);
    if ( 0 != rc )
    {
        ANSLogError("ANSThread_join(server) failed (%d)!", rc);
        return EXIT_FAILURE;
    }

    return 0;
}


#if defined(__linux) || defined(_VXBUS)


int AnsServiceStart(const char* serviceName)
{
    /* TODO: We shall daemonize here and
     * start the server thread
     */
    ANSLogError("Running as a service is currently not supported. Exiting server");

    return EXIT_FAILURE;
}





#elif defined WIN32


static const char* g_ServiceNameWin = NULL;

// global variables

// internal variables
SERVICE_STATUS          ssStatus;       // current status of the service
SERVICE_STATUS_HANDLE   sshStatusHandle;
DWORD                   dwErr = 0;
//TCHAR                   szErr[256];


//
//  FUNCTION: ReportStatusToSCMgr()
//
//  PURPOSE: Sets the current status of the service and
//           reports it to the Service Control Manager
//
//  PARAMETERS:
//    dwCurrentState - the state of the service
//    dwWin32ExitCode - error code to report
//    dwWaitHint - worst case estimate to next checkpoint
//
//  RETURN VALUE:
//    TRUE  - success
//    FALSE - failure
//
//  COMMENTS:
//
BOOL ReportStatusToSCMgr(DWORD dwCurrentState,
                         DWORD dwWin32ExitCode,
                         DWORD dwWaitHint)
{
    static DWORD dwCheckPoint = 1;
    BOOL fResult = TRUE;


    if (dwCurrentState == SERVICE_START_PENDING)
        ssStatus.dwControlsAccepted = 0;
    else
        ssStatus.dwControlsAccepted = SERVICE_ACCEPT_STOP;

    ssStatus.dwCurrentState = dwCurrentState;
    ssStatus.dwWin32ExitCode = dwWin32ExitCode;
    ssStatus.dwWaitHint = dwWaitHint;

    if ( ( dwCurrentState == SERVICE_RUNNING ) ||
         ( dwCurrentState == SERVICE_STOPPED ) )
        ssStatus.dwCheckPoint = 0;
    else
        ssStatus.dwCheckPoint = dwCheckPoint++;


    // Report the status of the service to the service control manager.
    //
    if (!(fResult = SetServiceStatus( sshStatusHandle, &ssStatus))) {
        ANSLogDebug("SetServiceStatus");
    }

    return fResult;
}

//
//  FUNCTION: AnsServiceStop
//
//  PURPOSE: Stops the service
//
//  PARAMETERS:
//    none
//
//  RETURN VALUE:
//    none
//
//  COMMENTS:
//    If a AnsServiceStop procedure is going to
//    take longer than 3 seconds to execute,
//    it should spawn a thread to execute the
//    stop code, and return.  Otherwise, the
//    ServiceControlManager will believe that
//    the service has stopped responding.
//
VOID AnsServiceStop()
{
    // Stop's the server, wakes the main thread.

    // Report the status to the service control manager.
    //
    if (!ReportStatusToSCMgr(
        SERVICE_STOPPED,       // service state
        NO_ERROR,              // exit code
        0))                    // wait hint
        return;
}


//
//  FUNCTION: service_ctrl
//
//  PURPOSE: This function is called by the SCM whenever
//           ControlService() is called on this service.
//
//  PARAMETERS:
//    dwCtrlCode - type of control requested
//
//  RETURN VALUE:
//    none
//
//  COMMENTS:
//
VOID WINAPI service_ctrl(DWORD dwCtrlCode)
{
    // Handle the requested control code.
    //
    switch(dwCtrlCode)
    {
        // Stop the service.
        //
        case SERVICE_CONTROL_STOP:
            ReportStatusToSCMgr(SERVICE_STOP_PENDING, NO_ERROR, 0);
            AnsServiceStop();
            return;

        // Update the service status.
        //
        case SERVICE_CONTROL_INTERROGATE:
            break;

        // invalid control code
        //
        default:
            break;

    }

    ReportStatusToSCMgr(ssStatus.dwCurrentState, NO_ERROR, 0);
}

//
//  FUNCTION: service_main
//
//  PURPOSE: To perform actual initialization of the service
//
//  PARAMETERS:
//    dwArgc   - number of command line arguments
//    lpszArgv - array of command line arguments
//
//  RETURN VALUE:
//    none
//
//  COMMENTS:
//    This routine performs the service initialization and then calls
//    the user defined StartAns() routine to perform majority
//    of the work.
//
void WINAPI service_main(DWORD dwArgc, LPTSTR *lpszArgv)
{
    // register our service control handler:
    //
    sshStatusHandle = RegisterServiceCtrlHandlerA(g_ServiceNameWin, service_ctrl);

    if (!sshStatusHandle)
        goto cleanup;

    // SERVICE_STATUS members that don't change in example
    //
    ssStatus.dwServiceType = SERVICE_WIN32_OWN_PROCESS;
    ssStatus.dwServiceSpecificExitCode = 0;

    // report the status to the service control manager.
    //
    if (!ReportStatusToSCMgr(
        SERVICE_RUNNING, // service state
        NO_ERROR,              // exit code
        0))                 // wait hint
        goto cleanup;

    AnsServerStart();

cleanup:

    // try to report the stopped status to the service control manager.
    //
    if (sshStatusHandle)
        (VOID)ReportStatusToSCMgr(
                            SERVICE_STOPPED,
                            dwErr,
                            0);

    return;
}

int AnsServiceStart(const char* serviceName)
{
    SERVICE_TABLE_ENTRY dispatchTable[] =
    {
        { (LPSTR)serviceName, service_main },
        { NULL, NULL }
    };

    /* Set global service name, so service main function
     * has access to it
     */
    g_ServiceNameWin = serviceName;

    if (!StartServiceCtrlDispatcher(dispatchTable))
    {
        ANSLogError("StartServiceCtrlDispatcher failed.");
        return (EXIT_FAILURE);
    }

    return (0);
}




#else
    #error "No system services supported on this platform"
#endif
