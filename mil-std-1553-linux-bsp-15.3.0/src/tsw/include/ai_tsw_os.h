/* SPDX-FileCopyrightText: 2015-2022 AIM GmbH <info@aim-online.com> */
/* SPDX-License-Identifier: MIT OR GPL-2.0-or-later */


/*********************************************************************************/
/*! \file ai_tsw_os.h

          This file contains OS layer prototypes. Each of this prototypes must be
          implemented in all of the os specific implementation files.

          ai_tsw_os.h
          ai_tsw_os_windows.c    => Windows OS
          ai_tsw_os_linux.c      => Linux OS
          ai_tsw_os_vxworks.c    => VxWorks OS
          ai_tsw_os_nucleus.c    => Nucleus OS
          ai_tsw_os_noos.c       => No OS
*/
/*********************************************************************************/



#ifndef __AI_TSW_OS_H__
#define __AI_TSW_OS_H__

#include "api_defs.h"

/*****************************************************************************/
/*                                                                           */
/*                            OS dependant includes                          */
/*                                                                           */
/*****************************************************************************/

#if defined(_NUCLEUS)
    /* -- AYX --- */
    #include "nucleus.h"
    #include "ini_defs.h"
    #include <stdlib.h>
    #include <string.h>
    #include <stdio.h>
    #include "NCC_hw.h"

    #define AI_TSW_OS_SPRINTF(dest,size,format,...) snprintf(dest,size,format, ##__VA_ARGS__)
#endif



#if defined _AIM_SYSDRV && defined _AIM_WINDOWS
    /* -- TSW in Windows driver (AMC,AYE) --- */
    #include <ntddk.h>
    #include <initguid.h>
    #include <ntstatus.h>
    #include <ntstrsafe.h>
    #include "amc_compat.h"

    #define AI_TSW_OS_SPRINTF(dest,size,format,...) RtlStringCbPrintfA(dest,size,format, ##__VA_ARGS__)
    #define AI_TSW_FALLTHROUGH
#endif



#if defined _AIM_RTX64
    /* -- TSW in RTX64 rtdll (AYE) --- */
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#define AI_TSW_OS_SPRINTF(dest,size,format,...) snprintf(dest,size,format, ##__VA_ARGS__)
#define AI_TSW_FALLTHROUGH
#endif



#if defined _AIM_LINUX
    #if defined __has_attribute
    #if __has_attribute(__fallthrough__)
    #define AI_TSW_FALLTHROUGH  __attribute__((__fallthrough__))
    #endif
    #endif

    #ifndef AI_TSW_FALLTHROUGH
    #define AI_TSW_FALLTHROUGH do {} while (0)  /* fallthrough */
    #endif
#endif



#if defined _AIM_SYSDRV && defined _AIM_LINUX && !defined EMBEDDED
    /* -- TSW in Linux driver (AMC,AYE) --- */
    #include <linux/types.h>
    typedef bool BOOLEAN;

    #define AI_TSW_OS_SPRINTF(dest,size,format,...) snprintf(dest,size,format, ##__VA_ARGS__)
#endif



#if defined _AIM_SYSDRV && defined _AIM_LINUX && defined EMBEDDED
    /* -- TSW in Linux driver (AMC,AYE) --- */
    #include <linux/types.h>
    #include <asm/io.h>
    #include <stdarg.h>

    typedef bool BOOLEAN;

    #define AI_TSW_OS_SPRINTF(dest,size,format,...)if( dest != 0 ){ \
                                                        char * internal_dummy_ = MALLOC(size); \
                                                        snprintf(internal_dummy_,sizeof(dest),format, ##__VA_ARGS__); \
                                                        memcpy_toio(dest,internal_dummy_,size); \
                                                        FREE(internal_dummy_); \
                                                    };
#endif



#ifdef __VXWORKS__
    #include <stdio.h>
    #include <string.h>
#ifndef __RTP__
    #include <spinLockLib.h>
#endif
    #include <taskLib.h>

    #define AI_TSW_OS_SPRINTF(dest,size,format,...) snprintf(dest,size,format, ##__VA_ARGS__)
    #define AI_TSW_FALLTHROUGH
#endif



#ifdef _AIM_VME
  #include "stdlib.h"
#endif



#if defined(_AIM_VMEWINDOWS)
  #include "windows.h"
#endif



#ifdef _AIM_LVRT
  #include <stdio.h>
  #include <string.h>
  #include <windows.h>
  #include <stdlib.h>
  #include "amc_compat.h"

  #ifdef _LINK_CVI_LVRT_
    #include <rtutil.h>
//    #include <utility.h>
    #define AI_TSW_OS_SPRINTF(dest,size,format,...) snprintf(dest,size,format, ##__VA_ARGS__)
  #else
    #define AI_TSW_OS_SPRINTF(dest,size,format,...) _snprintf(dest,size,format, ##__VA_ARGS__)
  #endif
  #define AI_TSW_FALLTHROUGH
#endif



#include "Ai_cdef.h"

/*****************************************************************************/
/*                                                                           */
/*                      OS dependant functions                               */
/*                                                                           */
/*****************************************************************************/


/*! Forward declaration for os layer timer structure
    The actual structure is defined in the os specific layer */
struct ai_tsw_os_timer;

typedef struct ai_tsw_os_timer TY_AI_TSW_OS_TIMER;


/*! \brief Platform specific function to start a timer

    Will start a platform specific timer that elapses after milliseconds specified.
    The timer has to be stopped with \ref ai_tsw_os_timer_stop even if it elapsed before
    \param p_OsTimer pointer to hardware layer timer structure \ref struct ai_tsw_os_timer
    \param milliseconds milliseconds after which timer elapses
    \return returns 0 if timer was successfully started,
                    1 if p_OsTimer was NULL */
AiUInt32 ai_tsw_os_timer_start( TY_AI_TSW_OS_TIMER* p_OsTimer, AiInt32 milliseconds);


/*! \brief Platform specific function to check if timer elapsed

    \param p_OsTimer pointer to hardware layer timer structure \ref struct ai_tsw_os_timer
    \return returns TRUE if timer has elapsed, FALSE otherwise */
AiBoolean ai_tsw_os_timer_elapsed(TY_AI_TSW_OS_TIMER* p_OsTimer);


/*! \brief Platform specific function to stop a timer

    Will stop a platform specific hardware layer timer
    This function has to be called even if the timer elapsed before
    \param p_OsTimer pointer to hardware layer timer structure \ref struct ai_tsw_os_timer
    \return returns 0 if timer was successfully stopped, an appropriate error code otherwise */
AiUInt32 ai_tsw_os_timer_stop( TY_AI_TSW_OS_TIMER* p_OsTimer);


/*! \brief Creates a platform specific timer

    Allocates a platform specific timer structure. The timer
    can be started with \ref ai_tsw_os_timer_start and it has to be
    freed with \ref ai_tsw_os_timer_destroy when not needed anymore
    \return returns pointer to allocated timer on success, NULL on failure */
TY_AI_TSW_OS_TIMER* ai_tsw_os_timer_create( void * pParent );


/*! \brief Frees a platform specific timer

    \param p_OsTimer pointer to timer structure to free */
void ai_tsw_os_timer_destroy(TY_AI_TSW_OS_TIMER* p_OsTimer);




/*! \brief Platform specific function to wait for a specific time

    \param u_sec number of micro seconds to wait.
    \return returns TRUE if timer has elapsed, FALSE otherwise */
AiUInt32 ai_tsw_os_wait_usec( AiUInt32 u_sec );



/*! \brief Platform specific function to allocate some memory

    \param size ammount in byte the memory chunk should at least have
    \return returns a pointer to the newly allocated memory chunk, NULL on failure */
void* ai_tsw_os_malloc( AiUInt32 size );


/*! \brief Platform specific function to free a previously allocated memory chunk

    \param ptr pointer to the memory chunk that shall be freed */
void ai_tsw_os_free( void *ptr );




/*! Forward declaration for os layer lock
The actual structure is defined in the os specific layer */
struct ai_tsw_os_lock;


/*! Create a TSW lock
\return returns a pointer to the newly allocated lock, NULL on failure */
struct ai_tsw_os_lock * ai_tsw_os_lock_create(void);

/*! Delete a TSW lock */
void ai_tsw_os_lock_free( struct ai_tsw_os_lock * lock );

/*! Aquire a TSW lock
\return returns API ok on success */
AiReturn ai_tsw_os_lock_aquire(struct ai_tsw_os_lock * lock);

/*! Release a TSW lock
\return returns API_OK on success */
AiReturn ai_tsw_os_lock_release(struct ai_tsw_os_lock * lock);



AiUInt32 ai_tsw_os_novram_read(void* p_Device, AiUInt32 ul_start_offset, void* pv_Novram, AiUInt32 ul_MaxSize, AiUInt32 *pul_BytesRead);
AiUInt32 ai_tsw_os_novram_write(void* p_Device, AiUInt32 ul_start_offset, void* pv_Novram, AiUInt32 ul_MaxSize, AiUInt32 *pul_BytesRead);
AiUInt32 ai_tsw_os_novram_finalize(void* p_Device);
AiUInt32 ai_tsw_os_version_read(void* p_Device, TY_E_VERSION_ID eId, TY_VER_INFO * pxVersionValue);

#endif /* __AI_TSW_OS_H__ */

/* end: ai_tsw_os.h */
