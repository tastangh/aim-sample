/* SPDX-FileCopyrightText: 2013-2021 AIM GmbH <info@aim-online.com> */
/* SPDX-License-Identifier: MIT  */

/*! \file Ai_Defines.h
 *
 * \brief Common definitions and macros
 *
 * This file provides common definitions and macros.
 *
 *
 ******************************************************************************/
#ifndef AI_DEFINES_H
#define AI_DEFINES_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes *******************************************************************/

/* Constants ******************************************************************/

/* Macros *********************************************************************/

/**
 * This macro frees the memory referenced by the given pointer
 * and reinitializes this pointer
 * Note: Not applicable to expressions which result in pointers.
 */
#define	FREE(ptr)	{	                                            \
                        if ( NULL != (ptr) )                        \
                        {                                           \
                            free(ptr);                              \
                            (ptr) = NULL;	                        \
                        }                                           \
                    }

/**
 * This macro prevents compiler warnings in case of an unused parameter.
 */
#define NOTUSED_PARAM(p)    ((void) (p))

/*
 * Determine the number of elements of an array.
 */
#define ARRAY_SIZE(arr)     (sizeof(arr) / sizeof((arr)[0]))

/**
 * Return '(null)' in case the specified string is null.
 * Otherwise return the string as it is.
 */
#define FIXNULL(p)  ((NULL == (p)) ? "(null)" : (p))

/* Type definitions ***********************************************************/

/* Prototypes *****************************************************************/

#ifdef __cplusplus
}
#endif

#endif  /* AI_DEFINES_H */
