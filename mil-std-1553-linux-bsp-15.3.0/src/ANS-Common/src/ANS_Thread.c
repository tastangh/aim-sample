/* SPDX-FileCopyrightText: 2013-2021 AIM GmbH <info@aim-online.com> */
/* SPDX-License-Identifier: MIT  */

/*! \file ANS_Thread.c
 *
 * This module provides ANS thread management interfaces.
 *
 ******************************************************************************/

/* Includes *******************************************************************/

#include <stdlib.h>
#include <string.h>
#ifndef WIN32
# include <pthread.h>
#endif
#include <time.h>

//#include <tchar.h>

#include "Ai_Defines.h"
#include "Ai_NotUsed.h"
#include "Ai_Validate.h"
#include "Ai_Sem.h"
#include "Ai_Socket.h"
#include "ANS_Log.h"
#include "ANS_Thread.h"
#include "ANS_Server.h"

/* Constants ******************************************************************/

/* Macros *********************************************************************/

/* Type definitions ***********************************************************/

/*************************************************************************//**
 * Definition of the thread table entry states.
 * Table entry selection is based on the entry states.
 * The election of the next free takes place in following order:
 *   a) Entries marked "unused" have highest priority
 *   b) Entries marked "task ended" have lowest priority (here the "oldest"
 *      entry found is used)
 * Entries with other states are not elected.
 *****************************************************************************/
typedef enum tagEntryState_t
{
    EntryStateUnused,               /*!< entry not in use (or already joined) */
    EntryStateStartingUp,           /*!< task startup in progress       */
    EntryStateActive,               /*!< task alive and active          */
    EntryStateTaskEnded,            /*!< task ended but not joined      */

} EntryState;

/**
 * Definiton of a thread table entry.
 */
typedef struct tagThreadTableEntry_t
{
    EntryState      entryState;     /*!< state of this entry            */
    AnsThreadId     threadId;       /*!< thread id                      */
//#if defined _AIM_LINUX
//#else
//    HANDLE          threadHdl;      /*!< thread handle                  */
//#endif
    ANSThreadType   threadType;     /*!< thread type                    */
    ANSThread_Func  *pFunction;     /*!< thread function                */
    void            *pThreadArg;    /*!< destination thread argument    */
    ANSThreadState  threadState;    /*!< current thread state           */
    void            *pRetVal;       /*!< thread return value            */
    time_t          threadEndTime;  /*!< thread ending time (seconds    */
                                    /*!< since 1970)                    */
    struct AnsConnection* connection;   /*!< Connection the thread is servicing */

} ThreadTableEntry;

/* Prototypes *****************************************************************/

 static int  allocateNewTableEntry (void);
 static void lockTable (AiBool32 *pHaveLock);
 static void unlockTable (AiBool32 haveLock);
 static void *threadWrapper (void *pTableEntry);

 static AiBoolean    findThreadId (
         AnsThreadId    threadId,
         int            *pIndex);



/* Variables ******************************************************************/

/**
 * Thread table.
 */
static ThreadTableEntry g_threadTable[ANSTHREAD_MAX_THREADS];

/**
 * Module initialization marker flag.
 */
static AiBool32 g_isInitialized = AiFalse;

/* Functions ******************************************************************/

/*************************************************************************//**
 * Initialize the module.
 ****************************************************************************/
extern ANSThreadStatus ANSThread_initializeModule (void)
{
    NTSEM_RET   ntsemRet;

    if ( g_isInitialized )
        return ANSThreadStatusOK;      /** \return ANSThreadStatusOK      */

    /* Try to generate the semaphore for serializing table accesses.        */
    ntsemRet = ntsem_create(NTSEM_ID_THREADTABLE, 1);
    if ( NTSEM_RET_OK != ntsemRet )
    {
        ANSLogError("ANSThread_initializeModule: ntsem_create error (%d)", (int) ntsemRet);
        return ANSThreadStatusFail;    /** \return ANSThreadStatusFail    */
    }

    g_isInitialized = AiTrue;

    return ANSThreadStatusOK;
}

/*************************************************************************//**
 * Create the specified ANS thread and register it to the thread table.
 ****************************************************************************/
extern	ANSThreadStatus    ANSThread_createThread (
    ANSThread_Func         pFunction,      /*!< [in] thread function       */
    void*                  args,           /*!< thread arguments */
    ANSThreadType           threadType,     /*!< [in] thread type           */
    AiBoolean               doDetachThread, /*!< [in] flag: detach the thread */
    int                     *pThreadIndex)  /*!< [out] Index of thread in thread table */
{
    ANSThreadStatus         rc;
    ThreadTableEntry        *pEntry;
    int                     tableIndex;
    int                     pthreadRc;
#if WIN32
    HANDLE                  threadHdl;
#endif
    
    VALIDATE_PTR(pThreadIndex, ANSThreadStatusFail);
    *pThreadIndex = -1;
    VALIDATE_PTR(pFunction, ANSThreadStatusFail);
                                        /** \return ANSThreadStatusFail    */

    /*
     * Allocate the next free thread table entry.
     */
    tableIndex = allocateNewTableEntry();
    if ( tableIndex < 0 )
    {
        ANSLogError("ANSThread_createThread: allocateNewTableEntry failed!");
        return ANSThreadStatusFail;        /** \return ANSThreadStatusFail    */
    }
    *pThreadIndex = tableIndex;

    /*
     * Store the thread parameters into the allocated table entry.
     */
    pEntry = &g_threadTable[tableIndex];
    pEntry->threadState = ANSThreadStateNone;
    pEntry->threadType  = threadType;
    pEntry->pFunction   = pFunction;
    pEntry->pThreadArg = args;

    /*
     * Start the thread wrapper that will act as caller of the destination thread.
     * The wrapper is passed the table entry as thread argument here.
     */
#if PTHREADS
    pthreadRc = pthread_create(&pEntry->threadId, NULL, threadWrapper, (void *) pEntry);

    if ( 0 != pthreadRc )
    {
        ANSLogError("ANSThread_createThread: pthread_create failed (%d)", pthreadRc);
        rc = ANSThreadStatusFail;
    }
#elif WIN32
    pthreadRc = 0;
    threadHdl = CreateThread(NULL,0,(LPTHREAD_START_ROUTINE)threadWrapper,(void *) pEntry,0,&pEntry->threadId);

//    if ( NULL == pEntry->threadHdl )
    if ( NULL == threadHdl )
    {
        ANSLogError("ANSThread_createThread: pthread_create failed");
        rc = ANSThreadStatusFail;
    }
#else
    #error Threads not supported
#endif
    else
    {
        rc = ANSThreadStatusOK;

        if ( doDetachThread )
        {
#if PTHREADS
            pthreadRc = pthread_detach(pEntry->threadId);

            if ( 0 != pthreadRc )
            {
                ANSLogError("ANSThread_createThread: pthread_detach failed (%d)", pthreadRc);
            }
#elif WIN32
            // TODO MR: replace for Windows (equivalent???)
            {
//                HANDLE threadHdl;
//                threadHdl = OpenThread( SYNCHRONIZE, FALSE, pEntry->threadId);
//            if ( CloseHandle(pEntry->threadHdl))
            if ( CloseHandle(threadHdl))
            {
              rc = 0;  // success
            }
            else
            {
              rc = -1;
              ANSLogError("ANSThread_createThread: pthread_detach failed");
            }
            }
#else
    #error Threads not supported
#endif
        }
    }

    return rc;              /** \return ANSThreadStatusOK      */
}

/*************************************************************************//**
 * \brief Detach a thread
 * Detach an active thread. This ensures that all thread resources
 * immediately get released when then thread terminates.
 * Note: A detached thread can't be join()'ed by another thread.
 * (Calls pthread_detach() using the registered thread handle)
 ****************************************************************************/
extern int ANSThread_detach (
    int     threadIndex)    /*!< [in] thread index                           */
{
    AiBool32            haveLock;
    ThreadTableEntry    *pEntry;
    int                 rc;

    if ( threadIndex < 0 || threadIndex >= ARRAY_SIZE(g_threadTable) )
        return -1;          /** \return -1 - failure                    */

    lockTable(&haveLock);
    pEntry = &g_threadTable[threadIndex];
    if ( EntryStateUnused != pEntry->entryState )
    {
#if PTHREADS
        rc = pthread_detach(pEntry->threadId);
#elif WIN32
        // nothing to do for Windows, handle is closed direct after CreateThread 
        //{
        //  HANDLE threadHdl = NULL;
        //  threadHdl = OpenThread( SYNCHRONIZE, FALSE, pEntry->threadId);
        //  if ( threadHdl)
        //  {
        //     CloseHandle(threadHdl);  // success
        //  }
        //  else
        //  {
        //     rc = -1;
        //     ANSLogError("ANSThread_createThread: pthread_detach failed");
        //  }
        //}
#else
    #error Threads not supported
#endif
    }
    else
    {
        rc = 0;
    }
    unlockTable(haveLock);

    return rc;             /** \return 0 - success                      */
}

/*************************************************************************//**
 * \brief Detach a thread
 * Wait for the addressed task to terminate and return its return value.
 * If that thread has already terminated, then this function returns immediately.
 * (Calls pthread_join() using the registered thread handle and 'pRetval')
 ****************************************************************************/
extern int ANSThread_join (
    int     threadIndex,    /*!< [in] thread index                           */
    void    **pRetval)      /*!< [out] thread return code for pthread_join() */
{
    AiBool32            haveLock = AiFalse;
    ThreadTableEntry    *pEntry;
    int                 rc;

    if ( threadIndex < 0 || threadIndex >= ARRAY_SIZE(g_threadTable) )
        return -1;          /** \return -1 - failure                        */

    lockTable(&haveLock);
    pEntry = &g_threadTable[threadIndex];
    unlockTable(haveLock);

    switch ( pEntry->entryState )
    {
    case EntryStateActive:
    case EntryStateStartingUp:
#if PTHREADS
        rc = pthread_join(g_threadTable[threadIndex].threadId, pRetval);
#elif WIN32
        // Wait until thread finished
        {
                HANDLE threadHdl;

                static DWORD dwCheckPoint = 1;

                threadHdl = OpenThread( SYNCHRONIZE, FALSE, g_threadTable[threadIndex].threadId);

                rc = WaitForSingleObject( threadHdl, INFINITE);  

                CloseHandle(threadHdl);
        }

#else
    #error Threads not supported
#endif
        if ( 0 == rc )
        {
            pEntry->entryState = EntryStateUnused;
        }
        break;

    case EntryStateTaskEnded:
        *pRetval = pEntry->pRetVal;
        pEntry->entryState = EntryStateUnused;
        rc = 0;             /** \return 0 - success                         */
        break;

    case EntryStateUnused:
    default:
        rc = -1;
        break;
    }

    return rc;              /** \return 0 - success (see pthread_join)      */
}

/*************************************************************************//**
 * \brief ANS thread wrapper function.
 * This function calls the configured ANS thread and updates its thread state
 * in the thread table. In case the called thread returns, the thread's 
 * state automatically gets changed from running to terminated in the thread
 * table.
 ****************************************************************************/
static void     *threadWrapper (
    void        *pTableEntry)   /*!< [in] pointer to thread table entry */
{
    AiBool32            haveLock = AiFalse;
    ThreadTableEntry    *pEntry;

    VALIDATE_PTR(pTableEntry, (void *) EXIT_FAILURE);

    /*
     * Complete the thread registration.
     */
    pEntry = (ThreadTableEntry *)   pTableEntry;
    pEntry->threadState = ANSThreadStateRunning;

    /*
     * Call the destination thread function.
     */
    if ( NULL == pEntry->pFunction )
    {
        ANSLogError("ANSThread.threadWapper: NULL thread configured!");
        pEntry->pRetVal = (void *) EXIT_FAILURE;
    }
    else
    {
        pEntry->pRetVal = pEntry->pFunction(pEntry->pThreadArg);
    }

    /*
     * Deregister the thread. Additionally record the task end time
     * that may be of interest when searching the best free entry
     * in the allocateNewTableEntry() function.
     */
    lockTable(&haveLock);
    pEntry->threadState = ANSThreadStateTerminated;
    pEntry->entryState  = EntryStateTaskEnded;
    time(&pEntry->threadEndTime);
    unlockTable(haveLock);

    return pEntry->pRetVal;     /** \return Return value of destination thread. */
}

/*************************************************************************//**
 * Update the specified thread's information in the thread table.
 ****************************************************************************/
extern AiBoolean    ANSThread_update (
    ANSThreadType  threadType,      /*!< [in] Thread type               */
    ANSThreadState threadState,     /*!< [in] Thread state              */
    AnsThreadId     threadId,       /*!< [in] Thread ID                 */
    struct AnsConnection* connection)  /*!< [in] connection */
{
    ThreadTableEntry *pEntry;
    int             index;
    AiBoolean       boolRc;
    AiBool32        haveLock = AiFalse;


    lockTable(&haveLock);

    /* Reuse an exisiting thread table if one is found.             */
    boolRc = findThreadId(threadId, &index);
    if ( AiTrue == boolRc )
    {
        pEntry = &g_threadTable[index];
        pEntry->connection = connection;
        pEntry->entryState   = EntryStateActive;
        pEntry->threadState  = threadState;
        pEntry->threadType   = threadType;
    }
    else
    {
        ANSLogError("ANSThread_update: Can't find thread x%lX!", (unsigned long) threadId);
    }

    unlockTable(haveLock);

    return boolRc;              /** \return AiTrue - success            */
}


/*************************************************************************//**
 * Allocate a new table entry and return the resulting table index.
 ****************************************************************************/
static int  allocateNewTableEntry (void)
{
    ThreadTableEntry    *pEntry;
    time_t              oldestEndedTime;
    EntryState          entryState;
    int                 firstUnusedIndex = -1;
    int                 bestEndedIndex = -1;
    int                 allocatedIndex = -1;
    int                 ii;
    AiBool32            haveLock;

    time(&oldestEndedTime);

    /* Try to ensure that the thread table is locked.   */
    lockTable(&haveLock);

    /* Search for the first free table entry.           */
    for ( ii = 0; ii < ARRAY_SIZE(g_threadTable); ii += 1 )
    {
        pEntry = &g_threadTable[ii];
        entryState = pEntry->entryState;

        switch ( entryState )
        {
        case EntryStateActive:
        case EntryStateStartingUp:
            break;

        case EntryStateUnused:
            if ( firstUnusedIndex < 0 )
                firstUnusedIndex = ii;
            break;

        case EntryStateTaskEnded:
            if ( bestEndedIndex < 0 
            ||  pEntry->threadEndTime <= oldestEndedTime )
            {
                bestEndedIndex = ii;
            }
            break;

        default:
            ANSLogError("ANSThread.allocateEntry: unknown entry state = %d!", (int) entryState);
            break;
        }

        /* A free entry always is used immediately */
        if ( firstUnusedIndex >= 0 )
            break;
    }

    if ( firstUnusedIndex >= 0 )
        allocatedIndex = firstUnusedIndex;
    else
        allocatedIndex = bestEndedIndex;

    if ( allocatedIndex >= 0 )
    {
        /* Reset the table entry.       */
        (void) memset(&g_threadTable[allocatedIndex], 0, sizeof(g_threadTable[allocatedIndex]));

        /* Mark the entry used.         */
        g_threadTable[allocatedIndex].entryState = EntryStateStartingUp;
    }

    /* Release the table mutex. */
    unlockTable(haveLock);

    return allocatedIndex;      /** \return Index of allocated table entry, -1 if full */
}


/*************************************************************************//**
 * Find the active table entry that matches the specified thread ID.
 * Note: The table is not getting locked while searching.
 ****************************************************************************/
static AiBoolean    findThreadId (
    AnsThreadId	    threadId,   /*!< [in] thread ID to find              */
    int             *pIndex)    /*!< [out] table index if found, else -1 */
{
    AiBoolean           rc = AiFalse;
    int                 ii;
    ThreadTableEntry    *pEntry;

    *pIndex = -1;

    for ( ii = 0; ii < ARRAY_SIZE(g_threadTable); ii += 1 )
    {
        pEntry = &g_threadTable[ii];
        if ( EntryStateActive != pEntry->entryState
        &&   EntryStateStartingUp != pEntry->entryState )
            continue;

        if ( pEntry->threadId == threadId )
        {
            *pIndex = ii;
            rc = AiTrue;
            break;
        }
    }

    return rc;
}


/*************************************************************************//**
 * Lock the thread table.
 ****************************************************************************/
static void lockTable (AiBool32 *pHaveLock) /*!< [out] flag: lock succeeded */
{
    NTSEM_RET   ntsemRet;

    /*
     * Try to ensure that the thread table is locked.
     */
    ntsemRet = ntsem_wait(NTSEM_ID_THREADTABLE, NTSEM_TIMEOUT_INFINITE);
    if ( NTSEM_RET_OK != ntsemRet )
    {
        ANSLogError("ANS_Thread.lockTable: ntsem_wait error (%d)",  (int) ntsemRet);
        *pHaveLock = AiFalse;
    }
    else
        *pHaveLock = AiTrue;
}

/*************************************************************************//**
 * Unlock the thread table in case a lock is indicated.
 ****************************************************************************/
static void unlockTable (AiBool32 haveLock) /*!< flag: mutex lock is present    */
{
    /*
     * Release the table mutex if necessary.
     */
    if ( haveLock )
    {
        (void) ntsem_signal(NTSEM_ID_THREADTABLE);
    }
}



