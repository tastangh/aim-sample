/* SPDX-FileCopyrightText: 2013-2021 AIM GmbH <info@aim-online.com> */
/* SPDX-License-Identifier: MIT  */

/*! \file Ai_Opterror.h

\brief Multilinguale Fehlermeldungen zur Optionsverarbeitung

 Dieses Modul stellt Funktionalitaeten zur Erlangung von Fehlertexten
 zu entsprechenden Fehlerauftritten waehrend der Optionsverarbeitung
 in verschiedenen Landessprachen bereit.
*******************************************************************************/
#ifndef AI_OPTERROR_H
#define AI_OPTERROR_H

/* Include-Dateien ************************************************************/

#include "Ai_Options.h"

/* Konstanten *****************************************************************/

/* Makros *********************************************************************/

/* Typen **********************************************************************/

/**
 * Beschreibung der zur Verfuegung stehenden Sprachen der Fehlertexte.
 */
typedef enum
{
    OPTERROR_LANGUAGE_ENGLISH,  /*!< Texte in Englisch                  */
    OPTERROR_LANGUAGE_GERMAN,   /*!< Texte in Deutsch                   */

    /* reservierte Werte    */
    OPTERROR_LANGUAGE_RESERVED

} OPTERROR_LANGUAGE;

/* Prototypen *****************************************************************/

 extern int     opterror_setlanguage (
    OPTERROR_LANGUAGE   language);

 extern char const /*@observer@*/ *opterror_get (
    OPTIONS         error,
    OPTIONS_DESC    *desc);


#endif
