/* SPDX-FileCopyrightText: 2016-2021 AIM GmbH <info@aim-online.com> */
/* SPDX-License-Identifier: MIT OR GPL-2.0-or-later */

/*! \file Ai_Mutex.h
 *
 *  This header file contains declarations for
 *  a platform independent mutex implementation
 *
 */

#ifndef AI_MUTEX_H_
#define AI_MUTEX_H_


#include "Ai_types.h"


/* Forward declaration
 * Actual definition is platform dependent
 */
struct ai_mutex;


/*! \enum ai_mutex_err
 * Enumeration of possible ai_mutex related error codes
 */
enum ai_mutex_err
{
    AI_MUTEX_OK = 0,    /*!< The function executed successfully */
    AI_MUTEX_INVAL,     /*!< Invalid argument provided */
    AI_MUTEX_LOCKED,    /*!< Mutex is still locked */
    AI_MUTEX_INTERNAL,  /*!< Internal error occurred */
    AI_MUTEX_PERM       /*!< No permission to access the mutex e.g. locked by another process */
};




#ifdef __linux

#include <pthread.h>
#include <stdlib.h>
#include <errno.h>




/*! \struct ai_mutex
 *
 * Generic mutex using pthread library
 */
struct ai_mutex
{
    pthread_mutex_t pm;
};


/*! \brief Create a mutex
 *
 * @return pointer to created mutex on success, NULL on failure
 */
static AI_INLINE struct ai_mutex* ai_mutex_create(void)
{
    struct ai_mutex* mutex = (struct ai_mutex*) malloc(sizeof(struct ai_mutex));
    if(!mutex)
    {
        return NULL;
    }

    if(pthread_mutex_init(&mutex->pm, NULL))
    {
        free(mutex);
        return NULL;
    }

    return mutex;
}


/*! \brief Free a previously created mutex
 *
 * @param mutex mutex to free
 * @return AI_MUTEX_OK on success, an ai_mutex_err otherwise
 */
static AI_INLINE enum ai_mutex_err ai_mutex_free(struct ai_mutex* mutex)
{
    int ret;

    if(!mutex)
    {
        return AI_MUTEX_INVAL;
    }

    ret = pthread_mutex_destroy(&mutex->pm);

    switch(ret)
    {
        case EBUSY:
            return AI_MUTEX_LOCKED;

        case 0:
            break;

        default:
            return AI_MUTEX_INTERNAL;
    }

    free(mutex);

    return AI_MUTEX_OK;
}


/*! \brief Lock a mutex
 *
 * @param mutex the mutex to lock
 * @return AI_MUTEX_OK on success, an ai_mutex_err otherwise
 */
static AI_INLINE enum ai_mutex_err ai_mutex_lock(struct ai_mutex* mutex)
{
    int ret;

    if(!mutex)
    {
        return AI_MUTEX_INVAL;
    }

    ret = pthread_mutex_lock(&mutex->pm);
    switch(ret)
    {
        case EINVAL:
            return AI_MUTEX_INVAL;

        case EDEADLK:
            return AI_MUTEX_LOCKED;

        case 0:
            return AI_MUTEX_OK;

        default:
            return AI_MUTEX_INTERNAL;
    }
}


/*! \brief Release a mmutex
 *
 * @param mutex the mutex to release
 * @return AI_MUTEX_OK on success, an ai_mutex_err otherwise
 */
static AI_INLINE enum ai_mutex_err ai_mutex_release(struct ai_mutex* mutex)
{
    int ret;

    if(!mutex)
    {
        return AI_MUTEX_INVAL;
    }

    ret = pthread_mutex_unlock(&mutex->pm);
    switch(ret)
    {
        case EINVAL:
            return AI_MUTEX_INVAL;

        case EPERM:
            return AI_MUTEX_PERM;

        case 0:
            return AI_MUTEX_OK;

        default:
            return AI_MUTEX_INTERNAL;
    }
}



#elif defined WIN32


#include <Windows.h>


/*! \struct ai_mutex
*
* Generic mutex using WIN32 semaphores
*/
struct ai_mutex
{
    HANDLE handle;
};


static AI_INLINE struct ai_mutex* ai_mutex_create(void)
{
    struct ai_mutex* mutex;

    mutex = (struct ai_mutex*) malloc(sizeof(struct ai_mutex));
    if (!mutex)
    {
        return NULL;
    }

    mutex->handle = CreateMutex(NULL, FALSE, NULL);
    if (!mutex->handle)
    {
        free(mutex);
        return NULL;
    }

    return mutex;
}


static AI_INLINE enum ai_mutex_err ai_mutex_free(struct ai_mutex* mutex)
{
    DWORD ret;

    if (!mutex)
    {
        return AI_MUTEX_INVAL;
    }

    if (!CloseHandle(mutex->handle))
    {
        ret = GetLastError();
        switch (ret)
        {
        case ERROR_INVALID_HANDLE:
            return AI_MUTEX_INVAL;

        default:
            return AI_MUTEX_INTERNAL;
        }
    }

    free(mutex);

    return AI_MUTEX_OK;
}


static AI_INLINE enum ai_mutex_err ai_mutex_lock(struct ai_mutex* mutex)
{
    DWORD ret;

    if (!mutex)
    {
        return AI_MUTEX_INVAL;
    }

    ret = WaitForSingleObject(mutex->handle, INFINITE);
    switch (ret)
    {
    case WAIT_FAILED:
        return AI_MUTEX_INTERNAL;

    default:
        return AI_MUTEX_OK;
    }
}


static AI_INLINE enum ai_mutex_err ai_mutex_release(struct ai_mutex* mutex)
{
    if (!mutex)
    {
        return AI_MUTEX_INVAL;
    }

    if (!ReleaseMutex(mutex->handle))
    {
        return AI_MUTEX_INTERNAL;
    }

    return AI_MUTEX_OK;
}


#elif defined _VXBUS

#include <vxWorks.h>
#include <stdlib.h>
#include <semLib.h>

/**
 * Functions here are Dummy functions so that the AFDX-API library may be compiled for Vxworks
 * Intention is to fill these functions in later as needed by the project  * 
 * 
 */
struct ai_mutex
{

};


static AI_INLINE struct ai_mutex* ai_mutex_create(void)
{
    return NULL;
}


static AI_INLINE enum ai_mutex_err ai_mutex_free(struct ai_mutex* mutex)
{
    return AI_MUTEX_OK;
}


static AI_INLINE enum ai_mutex_err ai_mutex_lock(struct ai_mutex* mutex)
{    
    return AI_MUTEX_OK;
}


static AI_INLINE enum ai_mutex_err ai_mutex_release(struct ai_mutex* mutex)
{
    return AI_MUTEX_OK;
}

#else

#error "Unsupported platform"

#endif

#endif /* AI_MUTEX_H_ */
