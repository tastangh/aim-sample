/* SPDX-FileCopyrightText: 2013-2023 AIM GmbH <info@aim-online.com> */
/* SPDX-License-Identifier: MIT  */

/*! \file Ai_Sem.c

Implementation of platform independent semaphore functionality

*******************************************************************************/

#if defined(_MIT_POSIX_THREADS) || defined(KERNELTHREADS) || defined(__linux) || defined(_VXBUS)
# define PTHREADS   1
#else
# define PTHREADS   0
#endif

/* Includes *******************************************************************/

#include <stdlib.h>

#include "Ai_Validate.h"
#include "Ai_Sem.h"

#if defined(__unix) || defined(_VXBUS)
#if PTHREADS
# if defined(_MIT_POSIX_THREADS)
#  include <pthread/mit/pthread.h>
# else
#  include "pthread.h"
# endif
#endif
#include <sys/types.h>
#if !PTHREADS
# include <sys/ipc.h>
# include <sys/sem.h>
#endif
#include <unistd.h>
#endif

#ifdef WIN32
#include <windows.h>
#endif

/* Constants ******************************************************************/

#if defined(__unix) || defined(_VXBUS)
#define KEY_PATH    "."     /*!< Path for creating semaphore key        */
#define SLEEP_TIME  50      /*!< Sleep time between polling calls       */
                            /*!< in milliseconds                        */
#define OP_LOCK     -1      /*!< Value representing locking a           */
                            /*!< sempahore                              */
#define OP_UNLOCK   1       /*!< Value representing unlocking a         */
                            /*!< sempahore                              */
#endif

#define CC_DEBUG    0       /*!< Flag: Enable debug code                */

/* Macros *********************************************************************/

#if defined(__unix) && PTHREADS
# define SPECIFIC_CREATE    mg_unix_pthreads_create
# define SPECIFIC_DELETE    mg_unix_pthreads_delete
# define SPECIFIC_WAIT      mg_unix_pthreads_wait
# define SPECIFIC_SIGNAL    mg_unix_pthreads_signal
#elif defined(__unix) 
# define SPECIFIC_CREATE    mg_unix_create
# define SPECIFIC_DELETE    mg_unix_delete
# define SPECIFIC_WAIT      mg_unix_wait
# define SPECIFIC_SIGNAL    mg_unix_signal
#elif defined(_VXBUS)
# define SPECIFIC_CREATE    mg_unix_pthreads_create
# define SPECIFIC_DELETE    mg_unix_pthreads_delete
# define SPECIFIC_WAIT      mg_unix_pthreads_wait
# define SPECIFIC_SIGNAL    mg_unix_pthreads_signal
#elif defined(WIN32)
# define SPECIFIC_CREATE    mg_win32_create
# define SPECIFIC_DELETE    mg_win32_delete
# define SPECIFIC_WAIT      mg_win32_wait
# define SPECIFIC_SIGNAL    mg_win32_signal
#else
# error "Target OS not supported!"
#endif

/**
 * This macro checks whether the given ntsem-semaphore-identifier
 * is valid
 */
#define IS_VALID_ID(semid)                      \
    (semid > (int) NTSEM_ID_RESERVED && semid < (int) NTSEM_ID_LAST)

#if CC_DEBUG
/**
 * This macro executes the specified command if debug mode is
 * enabled by the CC_DEBUG flag.
 */
#define DEBUGCMD(cmd)   { cmd ; }
#else
#define DEBUGCMD(cmd)   /* nothing */
#endif

/* Types **********************************************************************/

/**
 * Description of an entry of the pthread mutex table.
 */
#if PTHREADS
typedef struct
{
    int     initialized;
    pthread_mutex_t mutex;
} TABLEENTRY;
#endif

/* Variables ******************************************************************/

#ifdef WIN32
/**
 * This table contains the unique key identifiers for each semaphore
 */
static  HANDLE  mg_keytable[NTSEM_ID_LAST];
#endif

#if PTHREADS
/**
 * This table contains the pthreads mutex descriptors.
 */
static  TABLEENTRY  mg_mutextable[NTSEM_ID_LAST];
#endif

/* Prototypes *****************************************************************/

#if defined(__unix) && !PTHREADS
static  NTSEM_RET   mg_unix_create (
    NTSEM_ID    ntsem_id,
    int     initialcounter);
#endif

#if defined(__unix) && !PTHREADS
static  NTSEM_RET   mg_unix_delete (
    NTSEM_ID    ntsem_id);
#endif

#if defined(__unix) && !PTHREADS
static  NTSEM_RET   mg_unix_wait (
    NTSEM_ID    ntsem_id,
    int         timeout);
#endif

#if defined(__unix) && !PTHREADS
static  NTSEM_RET   mg_unix_signal (
    NTSEM_ID    ntsem_id);
#endif

#if PTHREADS
static  NTSEM_RET   mg_unix_pthreads_create (
    NTSEM_ID    ntsem_id,
    int     initialcounter);
#endif

#if PTHREADS
static  NTSEM_RET   mg_unix_pthreads_delete (
    NTSEM_ID    ntsem_id);
#endif

#if PTHREADS
static  NTSEM_RET   mg_unix_pthreads_wait (
    NTSEM_ID    ntsem_id,
    int     timeout);
#endif

#if PTHREADS
static  NTSEM_RET   mg_unix_pthreads_signal (
    NTSEM_ID    ntsem_id);
#endif

#ifdef WIN32
static  NTSEM_RET   mg_win32_create (
    NTSEM_ID    ntsem_id,
    int     initialcounter);
#endif

#ifdef WIN32
static  NTSEM_RET   mg_win32_delete (
    NTSEM_ID    ntsem_id);
#endif

#ifdef WIN32
static  NTSEM_RET   mg_win32_wait (
    NTSEM_ID    ntsem_id,
    int     timeout);
#endif

#ifdef WIN32
static  NTSEM_RET   mg_win32_signal (
    NTSEM_ID    ntsem_id);
#endif

#if defined(__unix) && !PTHREADS
static  int     mg_getsemid (
    NTSEM_ID    semid);
#endif

/* Functions ******************************************************************/

/***************************************************************************//**
 * This function creates a new semaphore.
 *
 * The semaphore is identified by the semaphore identification.
 * The number of events stored by the semaphore is initialized with the
 * specified number. An initialization with a value of '1' creates
 * a binary semaphore (this type of semaphore may be something special
 * on some kinds of systems).
 *
 * The semaphore should be freed by a call to the module's ntsem_delete()
 * function after use. In other cases the semaphore is deleted at
 * program termination time.
 *
 * If the semaphore already exists, an error status is returned by this
 * function.
*******************************************************************************/
extern  NTSEM_RET   ntsem_create (
    NTSEM_ID    semid,      /*!< [in] Semaphore identification              */
    int         counter)    /*!< [in] Initial event counter value           */
{
    NTSEM_RET   rc;

    rc = SPECIFIC_CREATE(semid, counter);

#if defined(__unix) && !PTHREADS
    if (rc != NTSEM_RET_OK)
    {
        SPECIFIC_DELETE(semid);
        return SPECIFIC_CREATE(semid, counter);
    }
#endif

    return rc;              /** \return see function's type                 */
}

/***************************************************************************//**
 * This function deletes a previously created semaphore specified by
 * the semaphore identification. Any allocated resources related to the
 * semaphore are being freed.
 *
 * After a call to this function, the corresponding semaphore
 * cannot be used any longer.
 *
 * One some systems, the call to this function may also succeed if the
 * semaphore has already been deleted or not been created.
*******************************************************************************/
extern NTSEM_RET   ntsem_delete (
    NTSEM_ID        semid)      /*!< [in] Semaphore identification          */
{
    return SPECIFIC_DELETE(semid);  /** \return see function's type         */
}

/***************************************************************************//**
 * This function takes an event out of the specified semaphore's event
 * counter.
 *
 * In case there is no event available the functions waits until one
 * gets available or the specified time-out has expired.
 *
 * In case of a time-out condition this function reports an error.
 *
 * Note: Each executed "wait" call requires exactly one corresponding
 * "signal" call.
*******************************************************************************/
extern NTSEM_RET    ntsem_wait (
    NTSEM_ID    semid,          /*!< [in] Semaphore identification          */
    unsigned long   timeout)    /*!< [in] Timeout in milliseconds,          */
                                /*!< [in] NTSEM_TIMEOUT_NONE or             */
                                /*!< [in] NTSEM_TIMEOUT_INFINITE            */
{
    return SPECIFIC_WAIT(semid, timeout);
                                /** \return see function's type             */
}

/***************************************************************************//**
 * This functions releases a semaphore event previously taken by a call to
 * the "ntsem_wait()" function.
*******************************************************************************/
extern  NTSEM_RET   ntsem_signal (
    NTSEM_ID        semid)      /*!< [in] Semaphore identification          */
{
    return SPECIFIC_SIGNAL(semid);  /** \return see function's type         */
}

/* UNIX SVR4 ******************************************************************/

#if defined(__unix) && !PTHREADS
/***************************************************************************//**
 * This function implements the semaphore creating function under UNIX.
*******************************************************************************/
static  NTSEM_RET   mg_unix_create (
    NTSEM_ID    ntsem_id,   /*!< [in] Sempahore identifier                  */
    int     initialcounter) /*!< [in] Initial counter value                 */
{
    key_t       l_mykey;
    int     l_semid;
    union semun l_semctrl;

    /* Check the validance of the parameters.                               */
    if (!IS_VALID_ID(ntsem_id))
        return NTSEM_RET_ERANGE;    /** \return NTSEM_RET_ERANGE            */

    /* Create the unique key identifier for getting the semaphore handle    */
    if ((l_mykey = ftok(KEY_PATH, ntsem_id)) < 0)
        return NTSEM_RET_EID;       /** \return NTSEM_RET_EID               */

    /* Create the semaphore. If the semaphore already exists or an error    */
    /* occurs, return an error.                                             */
    if ((l_semid = semget(l_mykey, 1, IPC_CREAT | IPC_EXCL | 0660)) < 0)
        return NTSEM_RET_EEXISTS;
                                    /** \return NTSEM_EALREADY_EXISTS       */

    /* Initialize the semaphore with the initial counter value              */
    l_semctrl.val = initialcounter;
    if (semctl(l_semid, 0, SETVAL, l_semctrl) < 0)
        return NTSEM_RET_EINTERNAL; /** \return NTSEM_RET_EINTERNAL         */

    return NTSEM_RET_OK;            /** \return NTSEM_RET_OK                */
}
#endif

#if defined(__unix) && !PTHREADS
/***************************************************************************//**
 * This function implements the semaphore deleting under UNIX.
*******************************************************************************/
static  NTSEM_RET   mg_unix_delete (
    NTSEM_ID    ntsem_id)   /*!< [in] Semaphore identifier                  */
{
    int semid;

    /* Check the validance of the parameters.                               */
    if (!IS_VALID_ID(ntsem_id))
        return NTSEM_RET_ERANGE;    /** \return NTSEM_RET_ERANGE            */

    /* Determine the semaphore identifier for accessing the semaphore       */
    if ((semid = mg_getsemid(ntsem_id)) < 0)
        return NTSEM_RET_EID;       /** \return NTSEM_RET_EEID              */

    /* Delete the semaphore.                                                */
    if (semctl(semid, 0, IPC_RMID, 0) < 0)
        return NTSEM_RET_EEXISTS;   /** \return NTSEM_RET_EEXISTS           */

    return NTSEM_RET_OK;            /** \return NTSEM_RET_OK                */
}
#endif

#if defined(__unix) && !PTHREADS
/***************************************************************************//**
 * This function implements the semaphore waiting function under UNIX.
*******************************************************************************/
static  NTSEM_RET   mg_unix_wait (
    NTSEM_ID    ntsem_id,   /*!< [in] Semaphore identifier                  */
    int         timeout)    /*!< [in] Timeout in milliseconds               */
{
    int     l_semid;
    struct sembuf   l_sembuf;

    /* Check the validance of the parameters.                               */
    if (!IS_VALID_ID(ntsem_id))
        return NTSEM_RET_ERANGE;    /** \return NTSEM_RET_ERANGE            */

    /* Determine the semaphore's identifier for accessing the semaphore.    */
    if ((l_semid = mg_getsemid(ntsem_id)) < 0)
        return NTSEM_RET_EID;       /** \return NTSEM_RET_EID               */

    /* Prepare the control structure for the sleeping operation with the    */
    /* values for timeout and operation.                                    */
    l_sembuf.sem_num = 0;
    l_sembuf.sem_op = OP_LOCK;
    l_sembuf.sem_flg = (timeout == NTSEM_TIMEOUT_INFINITE) ? 0 : IPC_NOWAIT;

    /* Try to lock the semaphore repeatedly.                                */
    while (timeout >= 0)
    {
        if (semop(l_semid, &l_sembuf, 1) >= 0)
            return NTSEM_RET_OK;

        /* If locking the semaphore fails, sleep the defined time and try   */
        /* again                                                            */
        usleep(SLEEP_TIME * 1000);
        timeout -= SLEEP_TIME;
    }

    /* Now it was impossible to lock the semaphore within the timeout time  */
    return NTSEM_RET_ETIMEOUT;
}
#endif

#if defined(__unix) && !PTHREADS
/***************************************************************************//**
 * Diese Funktion implements the semaphore signalling function under UNIX.
*******************************************************************************/
static  NTSEM_RET   mg_unix_signal (
    NTSEM_ID    ntsem_id)   /*!< [in] Semaphore identifier  */
{
    int             l_semid;
    struct sembuf   l_sembuf;

    /* Check the semaphore identifier for validance.                        */
    if (!IS_VALID_ID(ntsem_id))
        return NTSEM_RET_ERANGE;    /** \return NTSEM_RET_ERANGE            */

    /* Determine the system's semaphore identifier for accessing the        */
    /* semaphore.                                                           */
    if ((l_semid = mg_getsemid(ntsem_id)) < 0)
        return NTSEM_RET_EID;       /** \return NTSEM_RET_EID               */

    /* Adjust the semaphore control structure for signalling the semaphore  */
    l_sembuf.sem_num = 0;
    l_sembuf.sem_op = OP_UNLOCK;
    l_sembuf.sem_flg = IPC_NOWAIT;

    /* Call the semaphore control function to signal the semaphore          */
    if (semop(l_semid, &l_sembuf, 1) < 0)
        return NTSEM_RET_EEXISTS;   /** \return NTSEM_RET_EEXISTS           */

    return  NTSEM_RET_OK;           /** \return NTSEM_RET_OK                */
}
#endif

/* UNIX Pthreads **************************************************************/

#if PTHREADS
/***************************************************************************//**
 * This function implements the semaphore creating function under UNIX.
*******************************************************************************/
static  NTSEM_RET   mg_unix_pthreads_create (
    NTSEM_ID    ntsem_id,   /*!< [in] Sempahore identifier                  */
    int     initialcounter) /*!< [in] Initial counter value                 */
{
    DEBUGCMD(printf("sem_create: %d\n", ntsem_id))

    /* Check the validance of the parameters.                               */
    if (!IS_VALID_ID(ntsem_id))
        return NTSEM_RET_ERANGE;    /** \return NTSEM_RET_ERANGE            */

    /* Check if semaphore has already been created.                         */
    if (mg_mutextable[ntsem_id].initialized != 0)
        return NTSEM_RET_EEXISTS;   /** \return NTSEM_RET_EEXISTS           */

    /* Initialize the mutex variable.                                       */
    if (pthread_mutex_init(&mg_mutextable[ntsem_id].mutex, NULL))
        return NTSEM_RET_EINTERNAL; /** \return NTSEM_RET_EINTERNAL         */

    /* Mark the mutex variable being initialized.                           */
    mg_mutextable[ntsem_id].initialized = !0;

    DEBUGCMD(printf("sem_create: %d -> OK\n", ntsem_id))

    return NTSEM_RET_OK;            /** \return NTSEM_RET_OK                */
}
#endif

#if PTHREADS
/***************************************************************************//**
 * This function implements the semaphore deleting under UNIX.
*******************************************************************************/
static  NTSEM_RET   mg_unix_pthreads_delete (
    NTSEM_ID    ntsem_id)   /*!< [in] Semaphore identifier                  */
{
    DEBUGCMD(printf("sem_delete: %d\n", ntsem_id))

    /* Check the validance of the parameters.                               */
    if (!IS_VALID_ID(ntsem_id))
        return NTSEM_RET_ERANGE;    /** \return NTSEM_RET_ERANGE            */

    /* Check if semaphore has really been created.                          */
    if (!mg_mutextable[ntsem_id].initialized)
        return NTSEM_RET_EEXISTS;   /** \return NTSEM_RET_EEXISTS           */

    /* Remove the mutex variable.                                           */
    if (pthread_mutex_destroy(&mg_mutextable[ntsem_id].mutex))
        return NTSEM_RET_EINTERNAL; /** \return NTSEM_RET_EINTERNAL         */

    /* Mark the table entry being erased.                                   */
    mg_mutextable[ntsem_id].initialized = 0;

    DEBUGCMD(printf("sem_delete: %d -> OK\n", ntsem_id))

    return NTSEM_RET_OK;            /** \return NTSEM_RET_OK                */
}
#endif

#if PTHREADS
/***************************************************************************//**
 * This function implements the semaphore waiting function under UNIX.
*******************************************************************************/
static  NTSEM_RET   mg_unix_pthreads_wait (
    NTSEM_ID    ntsem_id,   /*!< [in] Semaphore identifier                  */
    int         timeout)    /*!< [in] Timeout in milliseconds               */
{
    int rc;

    /* Check the validance of the parameters.                               */
    if (!IS_VALID_ID(ntsem_id))
        return NTSEM_RET_ERANGE;    /** \return NTSEM_RET_ERANGE            */

    /* Check if semaphore really exists.                                    */
    if (!mg_mutextable[ntsem_id].initialized)
        return NTSEM_RET_EEXISTS;   /** \return NTSEM_RET_EEXITS            */

    DEBUGCMD(printf("sem_wait: %d\n", ntsem_id))

    /* Perform the blocking wait operation.                                 */
    rc = pthread_mutex_lock(&mg_mutextable[ntsem_id].mutex);

    DEBUGCMD(printf("sem_wait: %d -> %d\n", ntsem_id, rc))

    return rc != 0
        ? NTSEM_RET_EINTERNAL       /** \return NTSEM_RET_EINTERNAL         */
        : NTSEM_RET_OK;             /** \return NTSEM_RET_OK                */
}
#endif

#if PTHREADS
/***************************************************************************//**
 * Diese Funktion implements the semaphore signalling function under UNIX.
*******************************************************************************/
static  NTSEM_RET   mg_unix_pthreads_signal (
    NTSEM_ID        ntsem_id)   /*!< [in] Semaphore identifier              */
{
    int rc;

    DEBUGCMD(printf("sem_signal: %d\n", ntsem_id))

    /* Check the validance of the parameters.                               */
    if (!IS_VALID_ID(ntsem_id))
        return NTSEM_RET_ERANGE;    /** \return NTSEM_RET_ERANGE            */

    /* Check if semaphore really exists.                                    */
    if (!mg_mutextable[ntsem_id].initialized)
        return NTSEM_RET_EEXISTS;   /** \return NTSEM_RET_EEXITS            */

    /* Perform the unlock operation.                                        */
    rc = pthread_mutex_unlock(&mg_mutextable[ntsem_id].mutex);

    DEBUGCMD(printf("sem_signal: %d -> %d\n", ntsem_id, rc))

    return rc != 0
        ? NTSEM_RET_EINTERNAL       /** \return NTSEM_RET_EINTERNAL         */
        : NTSEM_RET_OK;             /** \return NTSEM_RET_OK                */
}
#endif

/* Win32 **********************************************************************/

#ifdef WIN32
/***************************************************************************//**
 * This function implements the semaphore create function for WIN32.
*******************************************************************************/
static  NTSEM_RET   mg_win32_create (
    NTSEM_ID    ntsem_id,   /*!< [in] Semaphore identifier                  */
    int     initialcounter) /*!< [in] Initial counter value                 */
{
    HANDLE  l_handle;

    /* Check the parameters for validancer.                                 */
    if (!IS_VALID_ID(ntsem_id))
        return NTSEM_RET_ERANGE;    /** \return NTSEM_RET_ERANGE            */

    /* Check if semaphore has already been created.                         */
    if (mg_keytable[ntsem_id] != NULL)
        return NTSEM_RET_EEXISTS;

    /* Create the semaphore with the initial counter value                  */
    if ((l_handle = CreateSemaphore(NULL, initialcounter,
                    initialcounter, NULL)) == NULL)
        return NTSEM_RET_EEXISTS;   /** \return NTSEM_RET_EEXISTS           */

    /* Register the semaphore handle into the keytable                      */
    mg_keytable[ntsem_id] = l_handle;

    return NTSEM_RET_OK;        /** \return NTSEM_RET_OK                    */
}
#endif

#ifdef WIN32
/***************************************************************************//**
 * This function implements the semaphore delete function for WIN32
*******************************************************************************/
static  NTSEM_RET   mg_win32_delete (
    NTSEM_ID    ntsem_id)   /*!< [in] Semaphore identifier                  */
{
    /* Check the semaphore identifier for validance.                        */
    if (!IS_VALID_ID(ntsem_id))
        return NTSEM_RET_ERANGE;    /** \return NTSEM_RET_ERANGE            */

    /* Check if semaphore has really been created.                          */
    if (mg_keytable[ntsem_id] == NULL)
        return NTSEM_RET_EEXISTS;   /** \return NTSEM_RET_EEXISTS           */

    /* Delete the semaphore.                                                */
    CloseHandle(mg_keytable[ntsem_id]);

    /* Reset the keytable entry for this semaphore                          */
    mg_keytable[ntsem_id] = NULL;

    return NTSEM_RET_OK;            /** \return NTSEM_RET_OK                */
}
#endif

#ifdef WIN32
/***************************************************************************//**
 * This function implements the semaphore waiting function for WIN32.
*******************************************************************************/
static  NTSEM_RET   mg_win32_wait (
    NTSEM_ID    ntsem_id,   /*!< [in] Semaphore identifier                  */
    int         timeout)    /*!< [in] Timeout in milliseconds               */
{

    /* Check the parameters for validance                                   */
    if (!IS_VALID_ID(ntsem_id))
        return NTSEM_RET_ERANGE;    /** \return NTSEM_RET_ERANGE            */

    /* Check if semaphore really exists.                                    */
    if (mg_keytable[ntsem_id] == NULL)
        return NTSEM_RET_EEXISTS;   /** \return NTSEM_RET_EEXITS            */

    /* In case of infinite timeout, set the timeout value                   */
    /* to the correspondend value for the wait function                     */
    if (timeout == NTSEM_TIMEOUT_INFINITE)
        timeout = INFINITE;

    /* Wait for the semaphore to be signaled.                               */
    if (WaitForSingleObject(mg_keytable[ntsem_id], timeout)
        != WAIT_OBJECT_0)
    {
        return NTSEM_RET_ETIMEOUT;  /** \return NTSEM_RET_ETIMEOUT          */
    }

    return NTSEM_RET_OK;            /** \return NTSEM_RET_OK                */
}
#endif

#ifdef WIN32
/***************************************************************************//**
 * This function implements the semaphore signalling functionf or WIN32.
*******************************************************************************/
static  NTSEM_RET   mg_win32_signal (
    NTSEM_ID        ntsem_id)   /*!< [in] Semaphore identifier              */
{
    /* Check the parameters for validance.                                  */
    if (!IS_VALID_ID(ntsem_id))
        return NTSEM_RET_ERANGE;    /** \return NTSEM_RET_ERANGE            */

    /* Check if semaphore really exists.                                    */
    if (mg_keytable[ntsem_id] == NULL)
        return NTSEM_RET_EEXISTS;   /** \return NTSEM_RET_EEXISTS           */

    /* Signal the semaphore.                                                */
    if (ReleaseSemaphore(mg_keytable[ntsem_id], 1, NULL) == 0)
        return NTSEM_RET_EFREE; /** \return NTSEM_RET_EFREE                 */

    return NTSEM_RET_OK;        /** \return NTSEM_RET_OK                    */
}
#endif

#if defined(__unix) && !PTHREADS
/***************************************************************************//**
 * This function determines the unix-system semaphore identifier for the given
 * ntsem-identifier.
 *
 * The semaphore identifier is returned by this function.
*******************************************************************************/
static  int     mg_getsemid (
    NTSEM_ID    semid)  /*!< [in] ntsem-identifier                          */
{
    key_t   l_mykey;

    /* create the key identifier for accessing the semaphore                */
    if ((l_mykey = ftok(KEY_PATH, semid)) < 0)
        return -1;      /** \return -1, if function fails                   */

    /* determine the semaphore identifier                                   */
    return semget(l_mykey, 1, IPC_CREAT);
                        /** \return otherwise, the semaphore identifier     */
}
#endif


