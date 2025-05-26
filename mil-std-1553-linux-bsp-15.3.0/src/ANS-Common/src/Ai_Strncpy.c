/* SPDX-FileCopyrightText: 2013-2021 AIM GmbH <info@aim-online.com> */
/* SPDX-License-Identifier: MIT  */

/*! \file Ai_Strncpy.c

 \brief strncpy Ersatzfunktion

  Dieses Modul stellt eine sichere Ersatzfunktion fuer strncpy aus der
  C-Standardbibliothek bereit.

*******************************************************************************/

/* Include-Dateien ************************************************************/

#include <stdlib.h>
#include <string.h>

#include "Ai_Strncpy.h"

/* Konstanten *****************************************************************/

/* Makros *********************************************************************/

/* Typen **********************************************************************/

/* Variablen ******************************************************************/

/* Prototypen *****************************************************************/

/* Funktionen *****************************************************************/

/***************************************************************************//**
 *  Diese Funktion kopiert die Zeichen aus der Quellzeichenkette
 *  in die Zielzeichenkette angegebenen Umfangs. Eine Null-Terminierung der
 *  Zielzeichenkette im Zielspeicherbereich wird durchgefuehrt.
*******************************************************************************/
char* strncpy_ (/*@returned@*//*@out@*/
    char        *dest,  /*!< [out] Speicherplatz fuer Ziel                  */
/*@unique@*/
    char const  *src,   /*!< [in] Quelle                                    */
    size_t      n)      /*!< [in] Umfang des Zielspeicherbereichs           */
{
    size_t  len2cpy;

    /* Eine unzulaessige Kommandierung wird erkannt.                        */
    if (dest == NULL
    || n < 1)
        return dest;

    /* Falls Anzahl der Zeichen 0 ist, die Zeichenkette terminieren         */
    if (src == NULL
    || n < 2)
    {
        *dest = '\0';
        return dest;
    }

    len2cpy = strlen(src) + 1;
    if (len2cpy > n)
        len2cpy = n;

    /* Zeichenkette kopieren und terminieren                                */
    memmove(dest, src, len2cpy);
    dest[n - 1] = '\0';

    return dest;                /** \return Zielzeichenkette                */
}

