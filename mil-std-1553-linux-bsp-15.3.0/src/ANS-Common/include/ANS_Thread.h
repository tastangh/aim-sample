/* SPDX-FileCopyrightText: 2013-2023 AIM GmbH <info@aim-online.com> */
/* SPDX-License-Identifier: MIT  */

/*! \file ANS_Thread.h
 * \brief ANS Thread Management
 *
 * This module provides ANS thread management interfaces.
 *
 *
 ******************************************************************************/
#ifndef ANS_THREAD_H
#define ANS_THREAD_H


/* Decide if we have Posix thread support */
#if defined(_MIT_POSIX_THREADS) || defined(KERNELTHREADS) || defined(__linux) || defined(_VXBUS)
    #define PTHREADS   1
#else
    #define PTHREADS   0
#endif


/* Includes *******************************************************************/



#include "ANS_Types.h"
#include "ANS_ServerPeer.h"
#include "ANS_Connection.h"

#if PTHREADS
    #include <pthread.h>

#elif defined WIN32
    #include <windows.h>
#elif defined _VXBUS
#else
    #error "Threads not supported on this platform"
#endif



/* Constants ******************************************************************/

#define ANSTHREAD_MAX_THREADS  256
                                    /*!< Max. number of concurrent      */
                                    /*!< threads that is supported by   */
                                    /*!< this module.                   */

/* Macros *********************************************************************/

/* Type definitions ***********************************************************/

/**
 * Description of possible thread states.
 */
typedef	enum tagAnsThreadState_t
{
    ANSThreadStateNone,		    /*!< no thread			                */
    ANSThreadStateRunning,		/*!< running thread		                */
    ANSThreadStateTerminated,	/*!< terminated thread		            */
    ANSThreadStateIo,			/*!< possibly temporarily blocked	    */

    /* reserved status	*/
    ANSThreadStateReserved
} ANSThreadState;

/**
 * Description of possible ANS thread types.
 */
typedef	enum tagAnsThreadType_t
{
    ANSThreadTypeNone,              /*!< no ANS thread			            */
    ANSThreadTypeServer,            /*!< ANS server thread		            */
    ANSThreadTypeMultiplexer,       /*!< ANS connection multiplexer thread */
    ANSThreadTypeAdminWorker,       /*!< ANS administrative command thread */
    ANSThreadTypeBoardWorker,       /*!< ANS board connection thread	    */

    /* reserved status	*/
    ANSThreadTypeReserved
} ANSThreadType;

/**
 * Definition of the module's return types.
 */
typedef enum tagAnsThreadStatus_t
{
    ANSThreadStatusOK,             /*!< success                            */
    ANSThreadStatusFail,           /*!< function failed                    */
    ANSThreadStatusNotFound        /*!< the requested entry was not found  */

} ANSThreadStatus;

/**
 * Interface of a thread function that is compatible with PThreads.
 */
typedef void *ANSThread_Func (void *pThreadParam);

/**
 * variable to store thread information
 */
#ifdef WIN32 
    typedef DWORD       AnsThreadId;        /*!< Thread identifier */

    #define ANS_THREAD_RETURN DWORD
    #define ANS_THREAD_CALL_CONV WINAPI
#elif PTHREADS
    #define ANS_THREAD_RETURN void*
    #define ANS_THREAD_CALL_CONV
    typedef pthread_t   AnsThreadId ;       /*!< Thread identifier */
#else
    #error Threads not supported
#endif

/**
 * Interface of a thread function that runs as callback in the multiplexer
 * thread context. This function should return EXIT_SUCCESS or EXIT_FAILURE.
 */
typedef long int ANSThread_MxCallback (
    AnsThreadId         threadId, 
    struct AnsServerPeer* client,
    struct AnsConnection* connection);

/* Prototypes *****************************************************************/

extern ANSThreadStatus     ANSThread_initializeModule (void);

extern	ANSThreadStatus     ANSThread_createThread (
    ANSThread_Func          pFunction,
    void*                   args,
    ANSThreadType           threadType,
    AiBoolean               doDetachThread,
    int                     *pThreadIndex);

extern int ANSThread_join (int threadIndex, void **pRetval);

extern int ANSThread_detach (int threadIndex);

extern AiBoolean    ANSThread_update (
    ANSThreadType   threadType,
    ANSThreadState  threadState,
    AnsThreadId     threadId,
    struct AnsConnection* connection);


#if PTHREADS


static AI_INLINE int ANS_CREATE_OS_THREAD(void *(*func) (void *) , void* arg, AiHandle* thread_handle)
{
    return pthread_create((pthread_t*) thread_handle, NULL, func, arg);
}


static AI_INLINE int ANS_JOIN_OS_THREAD(AiHandle thread_handle)
{
    return pthread_join((pthread_t) thread_handle, NULL);
}

#elif defined WIN32


static AI_INLINE int ANS_CREATE_OS_THREAD(LPTHREAD_START_ROUTINE  func, LPVOID arg, AiHandle* thread_handle)
{
    HANDLE h = CreateThread(NULL, 0, func, arg, 0, NULL);

    if (h)
    {
        *thread_handle = h;
        return 0;
    }

    return -1;
}


static AI_INLINE int ANS_JOIN_OS_THREAD(AiHandle thread_handle)
{
    DWORD ret = WaitForSingleObject(thread_handle, INFINITE);

    if (ret == WAIT_OBJECT_0)
    {
        return 0;
    }

    return -1;
}



#endif




#endif  /* ANS_THREAD_H */
