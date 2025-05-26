/* SPDX-FileCopyrightText: 2010-2021 AIM GmbH <info@aim-online.com> */
/* SPDX-License-Identifier: MIT OR GPL-2.0-or-later */

/*! \file Ai_msvc.h
 * 
 * This header file contains Microsoft Visual C specific macros
 */

#ifndef _AI_MSVC_H
#define _AI_MSVC_H


  #include <stddef.h>     /* ANSI C header for ptrdiff_t, size_t, NULL */

  /* MSVC provides stdint.h since Visual Studio 2010 */
  #if _MSC_VER >= 1600 && !defined KMDF_VERSION_MAJOR
  
    #include <stdint.h>

  #else
    
    typedef __int8              int8_t;
    typedef unsigned __int8     uint8_t;
    typedef __int16             int16_t;
    typedef unsigned __int16    uint16_t;
    typedef __int32             int32_t;
    typedef unsigned __int32    uint32_t;
    typedef __int64             int64_t;
    typedef unsigned __int64    uint64_t;
  
  #endif


/*! \def AI_INLINE
  Macro for marking functions as inline
*/
#define AI_INLINE __inline


/*! \def AI_UNUSED
  Macro to avoid compiler warnings of unreferenced formal parameter
*/
#define AI_UNUSED(x) (void)(x)

#endif
