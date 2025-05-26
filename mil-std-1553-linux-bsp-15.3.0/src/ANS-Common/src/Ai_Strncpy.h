/* SPDX-FileCopyrightText: 2013-2023 AIM GmbH <info@aim-online.com> */
/* SPDX-License-Identifier: MIT  */

/*! \file Ai_Strncpy.h

 \brief strncpy Ersatzfunktion

  Dieses Modul stellt eine sichere Ersatzfunktion fuer strncpy aus der
  C-Standardbibliothek bereit.
*******************************************************************************/
#ifndef AI_STRNCPY_H
#define AI_STRNCPY_H

#ifdef __cplusplus
extern "C" {
#endif

/* Include-Dateien ************************************************************/
#include <string.h>
/* Konstanten *****************************************************************/

/* Makros *********************************************************************/

/*!< Dieses Makro kopiert eine Zeichenkette, wobei die Zielzeichenkette */
/*!< in jedem Fall nullterminiert wird.                                 */
#define STRNCPY(d,s,n)  strncpy_((d), (s), (n))

#ifdef WIN32
    #define STRTOK(s,d,c) strtok_s(s,d,c)
#elif defined(__unix) || defined(_VXBUS) 
    #define STRTOK(s,d,c) strtok_r(s,d,c)
#else
    #error "STRTOK not defined on this platform"
#endif


/* Typen **********************************************************************/

/* Prototypen *****************************************************************/

 char* strncpy_ (/*@returned@*//*@out@*/
    char        *dest,
/*@unique@*/
    char const  *src,
    size_t      n);

#ifdef __cplusplus
}
#endif


#endif
