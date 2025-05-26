/* SPDX-FileCopyrightText: 2010-2021 AIM GmbH <info@aim-online.com> */
/* SPDX-License-Identifier: MIT OR GPL-2.0-or-later */

/*! \file Ai_armcc.h
 * 
 * This header file contains ARM C Compiler specific macros
 */

#ifndef _AI_ARMCC_H
#define _AI_ARMCC_H
 

#include <stddef.h>     /* ANSI C header for ptrdiff_t, size_t, NULL */
#include <stdint.h>


/*! \def AI_INLINE
Macro for marking functions as inline
*/
#define AI_INLINE __inline

/*! \def AI_UNUSED
  Macro to avoid compiler warnings of unreferenced formal parameter
*/
#define AI_UNUSED(x) (void)(x)

#endif
