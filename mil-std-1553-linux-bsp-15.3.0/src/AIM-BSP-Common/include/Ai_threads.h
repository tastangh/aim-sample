/* SPDX-FileCopyrightText: 2016-2024 AIM GmbH <info@aim-online.com> */
/* SPDX-License-Identifier: MIT OR GPL-2.0-or-later */

/*! \file Ai_threads.h
 *
 *  This header file contains declarations for
 *  a platform independent threads implementation
 *
 */

#ifndef AI_THREADS_H_
#define AI_THREADS_H_

#include "Ai_types.h"

/* Forward declaration
 * Similar to how mutexes are implemented in Ai_mutex.h
 * The implementation is simple and just covers create and join functions
 */
struct ai_thread;

#ifdef __linux
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>

struct ai_thread
{
    pthread_t h_Thread;
};

static AI_INLINE struct ai_thread* ai_create_thread(void* thread_function, void* thread_parameters)
{
    struct ai_thread* thread = NULL;
    thread = (struct ai_thread*) malloc(sizeof(struct ai_thread));
    if (!thread)
    {
        return NULL;
    }
    if (pthread_create(&thread->h_Thread, NULL, thread_function, thread_parameters))
    {
        free(thread);
        thread = NULL;
    }
    return thread;
}

static AI_INLINE int ai_join_thread(struct ai_thread* thread)
{
    int return_value = 0;
    if (!thread)
    {
        return 1;
    }
    return_value = pthread_join(thread->h_Thread, NULL);
    free(thread);
    return return_value;
}

static AI_INLINE int ai_gethostname(char* name, size_t len)
{
    return gethostname(name, len);
}

#elif defined WIN32

#include <Windows.h>
#include <stdlib.h>

struct ai_thread
{
    HANDLE h_Thread;
};

static AI_INLINE struct ai_thread* ai_create_thread(void* thread_function, void* thread_parameters)
{
    struct ai_thread* thread = NULL;
    DWORD dwThreadId = 0;
    thread = (struct ai_thread*) malloc(sizeof(struct ai_thread));
    if (!thread)
    {
        return NULL;
    }
    thread->h_Thread = CreateThread(
        NULL,
        0,
        (LPTHREAD_START_ROUTINE) thread_function,
        (LPVOID) thread_parameters,
        0,
        &dwThreadId);
    if (!thread->h_Thread)
    {
        free(thread);
        thread = NULL;
    }
    return thread;
}

static AI_INLINE int ai_join_thread(struct ai_thread* thread)
{
    DWORD dwStatus = 0;
    if (!thread)
    {
        return 1;
    }
    dwStatus = WaitForSingleObject(thread->h_Thread, INFINITE);
    CloseHandle(thread->h_Thread);
    free(thread);
    return dwStatus;
}

static AI_INLINE int ai_gethostname(char* name, size_t len)
{
    if (!GetComputerName(name, (LPDWORD) &len))
    {
        return -1;
    }
    return 0;
}

// #elif defined _VXBUS

#else

#error "Unsupported platform"

#endif

#endif /* AI_THREADS_H_ */
