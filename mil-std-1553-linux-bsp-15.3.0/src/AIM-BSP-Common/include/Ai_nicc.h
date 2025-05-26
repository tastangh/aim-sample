/* SPDX-FileCopyrightText: 2010-2021 AIM GmbH <info@aim-online.com> */
/* SPDX-License-Identifier: MIT OR GPL-2.0-or-later */

/*! \file Ai_nicc.h
 * 
 * This header file contains National Instruments C Compiler specific macros
 */

#ifndef _AI_NICC_H
#define _AI_NICC_H
 

#include <stddef.h>     /* ANSI C header for ptrdiff_t, size_t, NULL */
#include <stdint.h>     /* Header for integer types with fixed width (since C99) */
    


 /*! \def AI_INLINE
   Macro for inline functions
   C99 extensions must be enabled for this to work with NI compiler
 */
#define AI_INLINE inline



/*! \def AI_UNUSED
  Macro to avoid compiler warnings of unreferenced formal parameter
*/
#define AI_UNUSED(x) (void)(x)

#endif
