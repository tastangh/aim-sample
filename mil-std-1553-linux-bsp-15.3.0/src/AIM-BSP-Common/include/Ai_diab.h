/* SPDX-FileCopyrightText: 2010-2021 AIM GmbH <info@aim-online.com> */
/* SPDX-License-Identifier: MIT OR GPL-2.0-or-later */

/*! \file Ai_diab.h
 * 
 * This header file contains Diab specific macros
 */

#ifndef _AI_DCC_H
#define _AI_DCC_H
 
 
#include <stdint.h>         /* Header for integer types with fixed width (since C99) */
#include <stddef.h>         /* ANSI C header for ptrdiff_t, size_t, NULL */
    

/*! \def AI_INLINE
    Macro for marking functions as inline
*/
#define AI_INLINE __inline__



/*! \def AI_UNUSED
  Macro to avoid compiler warnings of unreferenced formal parameter
*/
#define AI_UNUSED(x) (void)(x)

#endif
