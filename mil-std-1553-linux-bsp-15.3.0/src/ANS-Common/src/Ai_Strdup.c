/* SPDX-FileCopyrightText: 2013-2021 AIM GmbH <info@aim-online.com> */
/* SPDX-License-Identifier: MIT  */

/*! \file Ai_Strdup.c

\brief Portable Ersatzfunktion fuer strdup()

 Dieses Modul stellt eine portable Ersatzfunktion fuer strdup() bereit.

*******************************************************************************/
#ifdef WIN32
# pragma warning(disable:4996)  // strcpy unsafe warning
#endif

/* Include-Dateien ************************************************************/

#include <stdlib.h>
#include <string.h>

#include "Ai_Validate.h"
#ifdef TSTHEAP
# include "tstheap.h"
#endif
#include "Ai_Strdup.h"

/* Konstanten *****************************************************************/

/* Makros *********************************************************************/

/* Typen **********************************************************************/

/* Prototypen *****************************************************************/

/* Variablen ******************************************************************/

/* Funktionen *****************************************************************/

/***************************************************************************//**
 Diese Funktion reserviert dynamisch Speicher fuer die Aufnahme
 der Quellzeichenkette und kopiert diese anschliessend in diesen Speicher.

 Eine Referenz auf den Zielspeicher wird zurueckgeliefert.
*******************************************************************************/
extern	char		*str_dup (
    char const	*str)	/*!< [in] Quellzeichenkette			            */
{
    char	*tmp;

    /* Parameter pruefen						*/
    VALIDATE_PTR(str, NULL);

    /* Speicherplatz fuer Zeichenkette reservieren		*/
    if ((tmp = (char *) malloc(strlen(str) + 1)) == NULL)
        return NULL;        /** \return NULL - Fehler   */

    /* Zeichenkette kopieren					*/
    (void) strcpy(tmp, str);

    return tmp;	/** \return Referenz auf Zielspeicher mit kopierter	*/
                /** \return Zeichenkette				*/
}

#if 0
/* Aenderungsjournal ***********************************************************
 *
 * $Log: str_dup.c $
 * Revision 1.4  2002/03/18 08:16:07  duerr
 * Added include of string.h since this was missing.
 *
 * Revision 1.3  1999/08/04 15:02:41  duerr
 * Enhanced support of LCLint.
 *
 * Revision 1.2  1999/08/03 21:51:25  duerr
 * Added include of tstheap.h if TSTHEAP is defined.
 *
 * Revision 1.1  1999/03/22 17:43:33  klingler
 * Initial revision
 *
*******************************************************************************/
#endif
