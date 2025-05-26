/* SPDX-FileCopyrightText: 2010-2021 AIM GmbH <info@aim-online.com> */
/* SPDX-License-Identifier: MIT OR GPL-2.0-or-later */

/*! \file Ai_borland.h
 * 
 * This header file contains Borland compiler specific macros
 */

#ifndef _AI_BORLAND_H
#define _AI_BORLAND_H


#include <stddef.h>     /* ANSI C header for ptrdiff_t, size_t, NULL */


/* Borland C provides stdint.h since version 5.61 */
#if __BORLANDC__ >= 0x561

    #include <stdint.h>

#else

    typedef signed char         int8_t;
    typedef unsigned char       uint8_t;
    typedef signed short        int16_t;
    typedef unsigned short      uint16_t;
    typedef signed int          int32_t;
    typedef unsigned int        uint32_t;


    /* Borland offers MS Visual Studio __int64 since version 5.30 
       long long is not available though.
       This was tested with PBA2000 using Borland C++ Builder 5    
    */
    #if (__BORLANDC__ >= 0x530) && !defined(__STRICT_ANSI__)

        typedef signed __int64      int64_t;
        typedef unsigned __int64    uint64_t;
    
    #endif

    typedef unsigned long int   uintptr_t;

#endif


/*! \def AI_INLINE
  Macro for marking functions as inline
  Not known if Borland offers inline and in which versions hence we clear it out
*/
#define AI_INLINE

/*! \def AI_UNUSED
  Macro to avoid compiler warnings of unreferenced formal parameter
*/
#define AI_UNUSED(x) (void)(x)

#endif
