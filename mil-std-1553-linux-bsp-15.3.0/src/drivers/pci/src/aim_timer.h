/* SPDX-FileCopyrightText: 2013-2021 AIM GmbH <info@aim-online.com> */
/* SPDX-License-Identifier: GPL-2.0-or-later */

/*! \file aim_timer.h
 * contains timer related function declarations for Linux system drivers
 */

#ifndef AIM_TIMER_H_
#define AIM_TIMER_H_


#include <linux/timer.h>
#include <linux/types.h>
#include <linux/version.h>


/* the init_timer_on_stack function was introduced in kernel 2.6.26 */
#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,26)

static inline void init_timer_on_stack(struct timer_list *timer)
{
    init_timer(timer);
}


#endif



/*! \struct aim_expiration_timer
 *
 * Timer structure that can be used for measuring time outs
 * The duration after timer expires is variable.
 */
struct aim_expiration_timer
{
    struct timer_list ktimer;  /*!< kernel timer structure used for measuring time out */
    atomic_t isExpired;        /*!< signifies if timer has expired */
};




/*! \brief Initializes an expiration timer structure with a given time out
 *
 *  \param expTimer timer to initialize
 *  \param timeoutMs time out in milliseconds after timer expires
 */
void aim_init_expiration_timer(struct aim_expiration_timer* expTimer, unsigned int timeoutMs);


/*! \brief Starts an expiration timer
 *
 * \param expTimer timer that will be started
 */
void aim_start_expiration_timer(struct aim_expiration_timer* expTimer);


/*! \brief Tests if a timer expired
 *
 * \param expTimer timer to test
 * \return return true if timer expired, false otherwise
 */
bool aim_is_expiration_timer_expired(struct aim_expiration_timer* expTimer);


/*! \brief stops an expiration timer
 *
 * \param expTimer timer to stop
 */
void aim_stop_expiration_timer(struct aim_expiration_timer* expTimer);




#endif /* AIM_TIMER_H_ */
