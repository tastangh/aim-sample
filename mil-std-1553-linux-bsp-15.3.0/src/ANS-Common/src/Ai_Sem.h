/* SPDX-FileCopyrightText: 2013-2023 AIM GmbH <info@aim-online.com> */
/* SPDX-License-Identifier: MIT  */

/*! \file Ai_Sem.h

 \brief Platform Independent Semaphore Implementations

 This module provides a generic interface to the system's mutual
 exclusion facility 'semaphores' if supported by the system.
 On systems with no such facility this module emulates semaphores.

 Functions are provided for:

    o Creating a new semaphore
    o Destroying an existing semaphore
    o Incrementing a semaphore's counter (releasing a semaphore)
    o Decrementing a semaphore's counter (waiting for a semaphore)

 Semaphores are identified by "identification keys". These keys
 are user-defined numeric values which internally are used to
 identify an existing semaphore. Identification keys should be
 centrally defined in the header file 'ntsem.h'.

 On systems with system-wide semaphores it is made sure, that
 a identification key's corresponding semaphore is exactly the
 same, no matter which process does the query.

 This module currently supports the following systems (required
 compilation-time defines listed in brackets):

    Windows 95/NT       (TARGET_WIN32)
    Linux           (TARGET_UNIX, __unix)

\section Glossary

\subsection Sem Semaphore
 A semaphore is a special type of a counter variable initially set to a
 defined value. As long as the counter is non-zero the semaphore is
 accessible by a task. The number stored into a semaphore is also
 called the number of "events".
 Each time a semaphore is accessed ("wait"), the counter is decremented as
 long as its value is non-zero. In the case of zero the semaphore
 is blocked. This means the querying task changes its status to
 "sleep", waiting for the event that another accessing task signals
 it access being complete ("signal").
 The context of a semaphore ensures that there are no concurrent accesses
 at the semaphore's counter value.

\subsection DL Dead-lock
 This term describes the system status, in which two ore more tasks
 are blocked by waiting for the other task(s) releasing a semaphore.
 This means one task is holding a semaphore which is required by the
 other to advance, the task itself holds another required semaphore
 The involved tasks are permanently blocked waiting for each other.

\subsection Event Event
 An event in the context of semaphores specifies an available number
 greater than zero in the semaphore's counter variable.
 Releasing a semaphore via "signal" stores an event into a semaphore,
 "waiting" for a semaphore removes a event out of the semaphore.
*******************************************************************************/
#ifndef AI_SEM_H
#define AI_SEM_H

/* Includes *******************************************************************/

#include "ANS_Client.h"

/* Constants ******************************************************************/

#undef TARGET_DEFINED

#if defined(TARGET_LINUX) || defined(__unix) || defined(_VXBUS)
#define TARGET_DEFINED
#endif

#if defined(WIN32)
#define TARGET_DEFINED
#endif

#if !defined(TARGET_DEFINED)
#error "This tool currently not supports this operating system"
#endif

/* Predefined time-out values                                           */
#define NTSEM_TIMEOUT_NONE  0L  /*!< Time-out value indicating          */
                                /*!< immediate processing               */
#define NTSEM_TIMEOUT_INFINITE  0xFFFFFFFFL
                                /*!< Time-out value indicating          */
                                /*!< "wait forever"                     */

/* Macros *********************************************************************/

/* Types **********************************************************************/

/**
 * This enumeration describes the possible status codes
 * of the module's functions.
 */
typedef enum tagNTSemRet_t
{
    NTSEM_RET_OK,           /*!< The function executed successfully     */
    NTSEM_RET_EEXISTS,      /*!< The specified semaphore already        */
                            /*!< or could not been found                */

    NTSEM_RET_ETIMEOUT,     /*!< There was no event available within    */
                            /*!< the specified time-out                 */
    NTSEM_RET_EID,          /*!< Could not create semphore identifier   */
    NTSEM_RET_EFREE,        /*!< The semaphore is already is freed      */
    NTSEM_RET_EINTERNAL,    /*!< internal error occured                 */
    NTSEM_RET_ERANGE        /*!< parameter(s) is/are out of range       */

} NTSEM_RET;

/**
 * This enumeration describes the semaphore identifications.
 * Note: These identifications are project specific.
 */
typedef enum tagNTSemID_t
{
    NTSEM_ID_RESERVED = 0,  /*!< reserved semaphore identifier          */
    NTSEM_ID_TEST,          /*!< semaphore for test purposes            */
    NTSEM_ID_NTLOG,         /*!< semaphore for ntlog                    */
    NTSEM_ID_MEMORY,        /*!< semaphore for dynamic memory access    */
    NTSEM_ID_THREADTABLE,   /*!< semaphore to serialize access to the   */
                            /*!< task thread table                      */

    /* Reserved entry   */
    NTSEM_ID_LAST           /*!< Identifier for last the last entry     */
                            /*!< in this table                          */
} NTSEM_ID;

/* Prototypes *****************************************************************/

 extern  NTSEM_RET   ntsem_create (
    NTSEM_ID    semid,
    int     counter);

 extern  NTSEM_RET   ntsem_delete (
    NTSEM_ID    semid);

 extern  NTSEM_RET   ntsem_wait (
    NTSEM_ID    semid,
    unsigned long   timeout);

 extern  NTSEM_RET   ntsem_signal (
    NTSEM_ID    semid);


#endif
