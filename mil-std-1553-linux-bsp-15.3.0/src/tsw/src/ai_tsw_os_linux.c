/* SPDX-FileCopyrightText: 2013-2022 AIM GmbH <info@aim-online.com> */
/* SPDX-License-Identifier: MIT OR GPL-2.0-or-later */

/*! \file ai_tsw_os_linux.c
This file contains os layer implementation for Windows.

ai_tsw_os.h
ai_tsw_os_windows.c => Windows specific implementations
*/

#if defined(_AIM_SYSDRV) && defined(_AIM_LINUX)
/* Linux system driver PCI/USB only */

#include "Ai_def.h"

#include "ai_tsw_os.h"
#include "AiMilErrors.h"


#include <linux/slab.h>
#include <linux/timer.h>
#include <linux/sched.h>
#include <linux/delay.h>

#include <linux/module.h>

#if !defined(EMBEDDED) && !defined(_AIM_1553_SYSDRV_USB)
int aim_gen_ays_novram_get(void* Device, AiUInt32 address, AiUInt32 max_size,  AiUInt32 *pul_BytesRead, void* pv_Novram);
int aim_gen_ays_novram_set(void* Device, AiUInt32 address, AiUInt32 max_size,  AiUInt32 *pul_BytesWritten, void* pv_Novram);
int aim_gen_ays_novram_finalize(void* Device);
int aim_gen_ays_version_read(void* Device, TY_E_VERSION_ID eId, TY_VER_INFO * pxVersionValue);
#endif

/*! \brief Timer event handler for os layer timers

    Is called when os layer timer elapses and marks the
    corresponding timer object as elapsed */

#if LINUX_VERSION_CODE < KERNEL_VERSION(4,15,0)
void ai_tsw_os_timer_handler(unsigned long data);
#else
void ai_tsw_os_timer_handler(struct timer_list *unused);
#endif

/*! Platform specific os layer timer structure */
struct ai_tsw_os_timer
{
    struct timer_list timer;   /*!< Linux Kernel timer object that is used as os layer timer */
    AiBoolean is_elapsed;       /*!< signifies if timer has elapsed */
    void* parent;		/* Parent pointer used for >= 4.15 timers */
};


AiUInt32 ai_tsw_os_timer_start( TY_AI_TSW_OS_TIMER* p_OsTimer, AiInt32 milliseconds)
{
    if( p_OsTimer == NULL )
        return 1;

    mod_timer( &p_OsTimer->timer, jiffies + msecs_to_jiffies(milliseconds) );

    return 0;
}

#if LINUX_VERSION_CODE < KERNEL_VERSION(4,15,0)
void ai_tsw_os_timer_handler(unsigned long data)
{
    TY_AI_TSW_OS_TIMER * p_Timer = (TY_AI_TSW_OS_TIMER*) data;
    p_Timer->is_elapsed = TRUE;
}
#else
void ai_tsw_os_timer_handler(struct timer_list *t)
{
    TY_AI_TSW_OS_TIMER *p_Timer = from_timer(p_Timer,t,timer);
    p_Timer->is_elapsed = TRUE;
}
#endif


AiBoolean ai_tsw_os_timer_elapsed(TY_AI_TSW_OS_TIMER* p_OsTimer)
{
    if(!p_OsTimer)
    {
        return FALSE;
    }

    return p_OsTimer->is_elapsed;
}


AiUInt32 ai_tsw_os_timer_stop( TY_AI_TSW_OS_TIMER* p_OsTimer)
{
    if( !p_OsTimer )
    {
        return 1;
    }


    del_timer_sync( &p_OsTimer->timer );

    return 0;
}


TY_AI_TSW_OS_TIMER* ai_tsw_os_timer_create( void * pParent )
{
    TY_AI_TSW_OS_TIMER * p_Timer = kzalloc(sizeof(TY_AI_TSW_OS_TIMER), GFP_KERNEL);

    if( p_Timer == NULL )
        return NULL;

    p_Timer->is_elapsed = 0;

#if LINUX_VERSION_CODE < KERNEL_VERSION(4,15,0)
    init_timer( &p_Timer->timer );
    p_Timer->timer.function = ai_tsw_os_timer_handler;
    p_Timer->timer.data     = (unsigned long)p_Timer;
#else
    timer_setup(&p_Timer->timer, ai_tsw_os_timer_handler, 0);
#endif

    return p_Timer;
}


void ai_tsw_os_timer_destroy(TY_AI_TSW_OS_TIMER* p_OsTimer)
{
  if( p_OsTimer != NULL )
    {
      del_timer_sync( &p_OsTimer->timer );

      // @FIXME: kzfree not available in kernel 2.6.24-16 (introduced in 2.6.32)
      kfree( p_OsTimer );
    }
}


AiUInt32 ai_tsw_os_wait_usec( AiUInt32 u_sec )
{
    if( u_sec > MAX_UDELAY_MS * 1000 )
        msleep( u_sec/1000);
    else
        udelay(u_sec);

     return 0;
}


void* ai_tsw_os_malloc( AiUInt32 size )
{
    return  kmalloc(size, GFP_ATOMIC);
}

void ai_tsw_os_free( void *ptr )
{
    kfree( ptr );
}

#ifdef _AIM_1553_SYSDRV_USB
  /*
  The USB driver executes all function calls on PASSIVE_LEVEL,
  the reason for this is that the blocking USB functions are used.
  */
  #define AI_TSW_OS_LINUX_LOCK_TYPE          struct mutex
  #define AI_TSW_OS_LINUX_LOCK_CREATE(l)     mutex_init(l)
  #define AI_TSW_OS_LINUX_LOCK_ACQUIRE(l)    mutex_lock(l)
  #define AI_TSW_OS_LINUX_LOCK_RELEASE(l)    mutex_unlock(l)
#else
  /*
  The PCI driver executes most functions on PASSIVE_LEVEL,
  but api_ir on DISPATCH_LEVEL.
  */
  #define AI_TSW_OS_LINUX_LOCK_TYPE          spinlock_t
  #define AI_TSW_OS_LINUX_LOCK_CREATE(l)     spin_lock_init(l)
  #define AI_TSW_OS_LINUX_LOCK_ACQUIRE(l)    spin_lock_bh(l)
  #define AI_TSW_OS_LINUX_LOCK_RELEASE(l)    spin_unlock_bh(l)
#endif


/*! Declaration for os layer lock */
struct ai_tsw_os_lock
{
    AI_TSW_OS_LINUX_LOCK_TYPE o;
};



/*! Create a TSW lock
\return returns a pointer to the newly allocated lock, NULL on failure */
struct ai_tsw_os_lock * ai_tsw_os_lock_create()
{
    struct ai_tsw_os_lock* lock;

    lock = (struct ai_tsw_os_lock*)ai_tsw_os_malloc(sizeof(struct ai_tsw_os_lock));

    if( !lock ) return NULL;

    AI_TSW_OS_LINUX_LOCK_CREATE(&lock->o);

    return lock;
}

/*! Create a TSW lock
\return returns a pointer to the newly allocated lock, NULL on failure */
void ai_tsw_os_lock_free( struct ai_tsw_os_lock * lock )
{
    ai_tsw_os_free(lock);
}

/*! Aquire a TSW lock
\return returns API ok on success */
AiReturn ai_tsw_os_lock_aquire(struct ai_tsw_os_lock * lock)
{
    if (lock == NULL) return API_ERR;

    AI_TSW_OS_LINUX_LOCK_ACQUIRE(&lock->o);

    return 0;
}

/*! Release a TSW lock
\return returns API_OK on success */
AiReturn ai_tsw_os_lock_release(struct ai_tsw_os_lock * lock)
{
    if (lock == NULL) return API_ERR;

    AI_TSW_OS_LINUX_LOCK_RELEASE(&lock->o);

    return 0;
}


AiUInt32 ai_tsw_os_novram_read(void * p_Device, AiUInt32 ul_start_offset, void* pv_Novram, AiUInt32 ul_MaxSize, AiUInt32 *pul_BytesRead)
{
#if !defined(EMBEDDED) && !defined(_AIM_1553_SYSDRV_USB)
	return(aim_gen_ays_novram_get(p_Device,ul_start_offset,ul_MaxSize,pul_BytesRead,pv_Novram));
#else
	return API_ERR_OS_NOT_SUPPORTED;
#endif
}

AiUInt32 ai_tsw_os_novram_write(void * p_Device, AiUInt32 ul_start_offset, void* pv_Novram, AiUInt32 ul_MaxSize, AiUInt32 *pul_BytesWritten)
{
#if !defined(EMBEDDED) && !defined(_AIM_1553_SYSDRV_USB)
	return(aim_gen_ays_novram_set(p_Device, ul_start_offset, ul_MaxSize,  pul_BytesWritten, pv_Novram));
#else
	return API_ERR_OS_NOT_SUPPORTED;
#endif
}

AiUInt32 ai_tsw_os_novram_finalize(void * p_Device)
{
#if !defined(EMBEDDED) && !defined(_AIM_1553_SYSDRV_USB)
    return(aim_gen_ays_novram_finalize(p_Device));
#else
	return API_ERR_OS_NOT_SUPPORTED;
#endif
}

AiUInt32 ai_tsw_os_version_read(void * p_Device, TY_E_VERSION_ID eId, TY_VER_INFO * pxVersionValue)
{
#if !defined(EMBEDDED) && !defined(_AIM_1553_SYSDRV_USB)
	return(aim_gen_ays_version_read(p_Device, eId, pxVersionValue));
#else
	return API_ERR_OS_NOT_SUPPORTED;
#endif
}


#endif /* defined(_AIM_SYSDRV) */
