/* SPDX-FileCopyrightText: 2013-2021 AIM GmbH <info@aim-online.com> */
/* SPDX-License-Identifier: GPL-2.0-or-later */

/*! \file aim_timer.c
 *
 * Contains timer related function definitions for Linux system drivers
 */

#include "aim_timer.h"




/*! \brief Handler function that is called when timer is expired
 *
 * @param ul_Data pointer to \ref struct _ExpirationTimer that elapsed
 */
#if LINUX_VERSION_CODE < KERNEL_VERSION(4,15,0)
static void ExpirationTimerHandler(unsigned long ul_Data)
{
    struct aim_expiration_timer* expTimer = (struct aim_expiration_timer*) ul_Data;

    atomic_set(&expTimer->isExpired, true);
}
#else
static void ExpirationTimerHandler(struct timer_list *timer)
{
    struct aim_expiration_timer* expTimer = from_timer(expTimer,timer,ktimer);

    atomic_set(&expTimer->isExpired, true);
}
#endif




void aim_init_expiration_timer(struct aim_expiration_timer* expTimer, unsigned int timeoutMs)
{
#if LINUX_VERSION_CODE < KERNEL_VERSION(4,15,0)
    init_timer_on_stack(&expTimer->ktimer);
    atomic_set(&expTimer->isExpired, false);

    expTimer->ktimer.function = ExpirationTimerHandler;
    expTimer->ktimer.data = (unsigned long) expTimer;
#else
    timer_setup_on_stack(&expTimer->ktimer,ExpirationTimerHandler,0);
    atomic_set(&expTimer->isExpired, false);
#endif

    expTimer->ktimer.expires = jiffies + msecs_to_jiffies(timeoutMs);
}





void aim_start_expiration_timer(struct aim_expiration_timer* expTimer)
{
    add_timer(&expTimer->ktimer);
}


bool aim_is_expiration_timer_expired(struct aim_expiration_timer* expTimer)
{
    int isExpired;

    isExpired = atomic_read(&expTimer->isExpired);

    if(isExpired)
        return true;
    else
        return false;
}


void aim_stop_expiration_timer(struct aim_expiration_timer* expTimer)
{
    del_timer(&expTimer->ktimer);
}

