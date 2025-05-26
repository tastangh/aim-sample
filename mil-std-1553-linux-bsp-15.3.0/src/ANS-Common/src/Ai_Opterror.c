/* SPDX-FileCopyrightText: 2013-2021 AIM GmbH <info@aim-online.com> */
/* SPDX-License-Identifier: MIT  */

/*! \file Ai_Opterror.c

\brief Multilinguale Fehlermeldungen zur Optionsverarbeitung

 Dieses Modul stellt Funktionalitaeten zur Erlangung von Fehlertexten
 zu entsprechenden Fehlerauftritten waehrend der Optionsverarbeitung
 in verschiedenen Landessprachen bereit.

*******************************************************************************/
#ifdef WIN32
# pragma warning(disable:4996)  // function is unsafe (sprintf etc.)
#endif

/* Include-Dateien ************************************************************/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "Ai_Validate.h"

#include "Ai_Opterror.h"

/* Konstanten *****************************************************************/

/* Erkennung der fixen Vorgabe der Landessprache    */
#undef  LANGUAGE_DEFINED
#ifdef LANGUAGE_ENGLISH
# define LANGUAGE_DEFINED
# define DEFAULT_LANGUAGE   OPTERROR_LANGUAGE_ENGLISH
#endif
#ifdef LANGUAGE_GERMAN
# define LANGUAGE_DEFINED
# define DEFAULT_LANGUAGE   OPTERROR_LANGUAGE_GERMAN
#endif
#ifndef LANGUAGE_DEFINED
# define LANGUAGE_ENGLISH
# define LANGUAGE_GERMAN
# define DEFAULT_LANGUAGE   OPTERROR_LANGUAGE_ENGLISH
                    /*MC Voreinstellung der     */
                    /*MC Sprache der Meldetexte */
#endif

#define MSG_BUFFER_SIZE     256 /*MC Umfang des internen Puffers*/
                    /*MC zur Aufnahme der Meldetexte*/

/* Makros *********************************************************************/

/**
 * Beschreibung des letzten Eintrags der Tabelle der Meldetexte
 */
#define MSG_TABLE_END                                       \
    {                                                       \
        OPTERROR_LANGUAGE_RESERVED, OPTIONS_RESERVED, NULL, \
    }

#ifdef DOASSERT
# define CHECKERRORS()  (void) mg_checkerrors(OPTIONS_OK);
#else
# define CHECKERRORS()  /* no mg_checkerrors    */
#endif

/* Typen **********************************************************************/

/**
 * Beschreibung eines Eintrags der Tabelle der Meldetexte.
 */
typedef struct
{
    OPTERROR_LANGUAGE language; /* Sprache          */
    OPTIONS     error;      /* Fehlerart            */
    char const  *message;   /* Meldetext            */

} MSG_TABLE_ENTRY;

/* Variablen ******************************************************************/

/**
 * Beschreibung der Fehlertexte zu den unterstuetzten Sprachen.
 */
static  MSG_TABLE_ENTRY mg_messages[] =
{
/* OPTIONS_OK   */
#ifdef LANGUAGE_GERMAN
    {
        OPTERROR_LANGUAGE_GERMAN,   OPTIONS_OK,
        "ok"
    },
#endif
#ifdef LANGUAGE_ENGLISH
    {
        OPTERROR_LANGUAGE_ENGLISH,  OPTIONS_OK,
        "ok"
    },
#endif
/* OPTIONS_EOL  */
#ifdef LANGUAGE_GERMAN
    {
        OPTERROR_LANGUAGE_GERMAN,   OPTIONS_EOL,
        "keine Argumente"
    },
#endif
#ifdef LANGUAGE_ENGLISH
    {
        OPTERROR_LANGUAGE_ENGLISH,  OPTIONS_EOL,
        "no arguments"
    },
#endif
/* OPTIONS_ARGUMENTS_FOLLOW */
#ifdef LANGUAGE_GERMAN
    {
        OPTERROR_LANGUAGE_GERMAN,   OPTIONS_ARGUMENTS_FOLLOW,
        "anstehende Argumente"
    },
#endif
#ifdef LANGUAGE_ENGLISH
    {
        OPTERROR_LANGUAGE_ENGLISH,  OPTIONS_ARGUMENTS_FOLLOW,
        "pending arguments"
    },
#endif
/* OPTIONS_BREAK    */
#ifdef LANGUAGE_GERMAN
    {
        OPTERROR_LANGUAGE_GERMAN,   OPTIONS_BREAK,
        "Abbruch der Optionsverarbeitung"
    },
#endif
#ifdef LANGUAGE_ENGLISH
    {
        OPTERROR_LANGUAGE_ENGLISH,  OPTIONS_BREAK,
        "option processing break"
    },
#endif
/* OPTIONS_EABORT   */
#ifdef LANGUAGE_GERMAN
    {
        OPTERROR_LANGUAGE_GERMAN,   OPTIONS_EABORT,
        "Fehler bei der Options-Verarbeitung"
    },
#endif
#ifdef LANGUAGE_ENGLISH
    {
        OPTERROR_LANGUAGE_ENGLISH,  OPTIONS_EABORT,
        "error processing '%s'"
    },
#endif
/* OPTIONS_EPREFIX  */
#ifdef LANGUAGE_GERMAN
    {
        OPTERROR_LANGUAGE_GERMAN,   OPTIONS_EPREFIX,
        "interner Fehler: unzulaessiges Praefix"
    },
#endif
#ifdef LANGUAGE_ENGLISH
    {
        OPTERROR_LANGUAGE_ENGLISH,  OPTIONS_EPREFIX,
        "internal error: invalid prefix"
    },
#endif
/* OPTIONS_ETYPE    */
#ifdef LANGUAGE_GERMAN
    {
        OPTERROR_LANGUAGE_GERMAN,   OPTIONS_ETYPE,
        "interner Fehler: unzulaessiger Optionstyp"
    },
#endif
#ifdef LANGUAGE_ENGLISH
    {
        OPTERROR_LANGUAGE_ENGLISH,  OPTIONS_ETYPE,
        "internal error: invalid option type"
    },
#endif
/* OPTIONS_EFUNCTION    */
#ifdef LANGUAGE_GERMAN
    {
        OPTERROR_LANGUAGE_GERMAN,   OPTIONS_EFUNCTION,
        "interner Fehler: unzulaessige Auswertungsfunktion"
    },
#endif
#ifdef LANGUAGE_ENGLISH
    {
        OPTERROR_LANGUAGE_ENGLISH,  OPTIONS_EFUNCTION,
        "internal error: invalid function"
    },
#endif
/* OPTIONS_ENOT_INITIALIZED */
#ifdef LANGUAGE_GERMAN
    {
        OPTERROR_LANGUAGE_GERMAN,   OPTIONS_ENOT_INITIALIZED,
        "interner Fehler: fehlende Initialisierung der Tabelle"
    },
#endif
#ifdef LANGUAGE_ENGLISH
    {
        OPTERROR_LANGUAGE_ENGLISH,  OPTIONS_ENOT_INITIALIZED,
        "internal error: table not initialized"
    },
#endif
/* OPTIONS_EUNKNOWN */
#ifdef LANGUAGE_GERMAN
    {
        OPTERROR_LANGUAGE_GERMAN,   OPTIONS_EUNKNOWN,
        "unbekannte Option"
    },
#endif
#ifdef LANGUAGE_ENGLISH
    {
        OPTERROR_LANGUAGE_ENGLISH,  OPTIONS_EUNKNOWN,
        "unknown option"
    },
#endif
/* OPTIONS_EMISSING */
#ifdef LANGUAGE_GERMAN
    {
        OPTERROR_LANGUAGE_GERMAN,   OPTIONS_EMISSING,
        "Fehlende Angabe der Option"
    },
#endif
#ifdef LANGUAGE_ENGLISH
    {
        OPTERROR_LANGUAGE_ENGLISH,  OPTIONS_EMISSING,
        "required option missing:"
    },
#endif
/* OPTIONS_EMISSING_ARGUMENT    */
#ifdef LANGUAGE_GERMAN
    {
        OPTERROR_LANGUAGE_GERMAN,   OPTIONS_EMISSING_ARGUMENT,
        "fehlendes Argument fuer Option '%s'"
    },
#endif
#ifdef LANGUAGE_ENGLISH
    {
        OPTERROR_LANGUAGE_ENGLISH,  OPTIONS_EMISSING_ARGUMENT,
        "missing argument for option '%s'"
    },
#endif
/* OPTIONS_EARGUMENT    */
#ifdef LANGUAGE_GERMAN
    {
        OPTERROR_LANGUAGE_GERMAN,   OPTIONS_EARGUMENT,
        "unzulaessiges Argument fuer Option '%s'"
    },
#endif
#ifdef LANGUAGE_ENGLISH
    {
        OPTERROR_LANGUAGE_ENGLISH,  OPTIONS_EARGUMENT,
        "invalid argument for option '%s'"
    },
#endif
/* OPTIONS_EVARIABLE_SIZE   */
#ifdef LANGUAGE_GERMAN
    {
        OPTERROR_LANGUAGE_GERMAN,   OPTIONS_EVARIABLE_SIZE,
        "interner Fehler: unzulaessige Zielvariablegroesse"
    },
#endif
#ifdef LANGUAGE_ENGLISH
    {
        OPTERROR_LANGUAGE_ENGLISH,  OPTIONS_EVARIABLE_SIZE,
        "internal error: invalid destination size"
    },
#endif
/* OPTIONS_EPROCESSING  */
#ifdef LANGUAGE_GERMAN
    {
        OPTERROR_LANGUAGE_GERMAN,   OPTIONS_EPROCESSING,
        "interner Fehler: Fehler in spezifizierter Benutzerfunktion"
    },
#endif
#ifdef LANGUAGE_ENGLISH
    {
        OPTERROR_LANGUAGE_ENGLISH,  OPTIONS_EPROCESSING,
        "internal error in user function"
    },
#endif
/* OPTIONS_EMEMORY  */
#ifdef LANGUAGE_GERMAN
    {
        OPTERROR_LANGUAGE_GERMAN,   OPTIONS_EMEMORY,
        "zuwenig freier Speicher"
    },
#endif
#ifdef LANGUAGE_ENGLISH
    {
        OPTERROR_LANGUAGE_ENGLISH,  OPTIONS_EMEMORY,
        "out of memory"
    },
#endif
/* OPTIONS_ENULLPOINTER */
#ifdef LANGUAGE_GERMAN
    {
        OPTERROR_LANGUAGE_GERMAN,   OPTIONS_ENULLPOINTER,
        "interner Fehler: unzulaessige NULL-Referenz"
    },
#endif
#ifdef LANGUAGE_ENGLISH
    {
        OPTERROR_LANGUAGE_ENGLISH,  OPTIONS_ENULLPOINTER,
        "internal error: invalid null pointer"
    },
#endif
/* OPTIONS_ERESERVED    */
#ifdef LANGUAGE_GERMAN
    {
        OPTERROR_LANGUAGE_GERMAN,   OPTIONS_RESERVED,
        "unbekannter Fehler"
    },
#endif
#ifdef LANGUAGE_ENGLISH
    {
        OPTERROR_LANGUAGE_ENGLISH,  OPTIONS_RESERVED,
        "unknown error"
    },
#endif
/*  */

    /* Ende der Tabellendefinition  */
    MSG_TABLE_END
};

/**
 * Vermerk der gesetzten Sprache.
 */
static  OPTERROR_LANGUAGE   mg_language = DEFAULT_LANGUAGE;

/* Prototypen *****************************************************************/

 static char const      *mg_findmessage (
    OPTIONS             error,
    OPTERROR_LANGUAGE language);

 static char const      *mg_fixnull (/*@returned@*/
    char const          *str);

#ifdef DOASSERT
 static int             mg_checkerrors (
    OPTIONS             options);
#endif

/* Funktionen *****************************************************************/

/***************************************************************************//**
 Diese Funktion selektiert die spezifizierte Landessprache fuer
 nachfolgende Anforderungen von Meldetexten.
*******************************************************************************/
extern  int     opterror_setlanguage (
    OPTERROR_LANGUAGE   language)   /*!< [in] Landessprache                 */
{
    switch (language)
    {
    case OPTERROR_LANGUAGE_ENGLISH:
    case OPTERROR_LANGUAGE_GERMAN:
        mg_language = language;
        return 0;           /** \return 0 - Ausfuehrung erfolgreich         */

    case OPTERROR_LANGUAGE_RESERVED:
        break;

    default:
        break;
    }

    return !0;              /** \return !0 - ungueltige Landessprache       */
}

/***************************************************************************//**
 Diese Funktion ermittelt zu einem aufgetreten Fehler waehrend der
 Optionsverarbeitung den entsprechenden Meldetext.
*******************************************************************************/
extern  char const /*@observer@*/ *opterror_get (
    OPTIONS         error,  /*!< [in] gemeldeter Fehler                     */
    OPTIONS_DESC    *desc)  /*!< [in] verwendete Optionsbeschreibung        */
{
    int     index;
static  char        buffer[MSG_BUFFER_SIZE];
    char const  *message;

    CHECKERRORS()

    /* Die Argumente werden verifiziert.                */
    VALIDATE_PTR(desc, "");

    /* Der Meldetext wird angefordert.              */
    if ((message = mg_findmessage(error, mg_language)) == NULL)
    {
        message = mg_fixnull(
            mg_findmessage(OPTIONS_RESERVED, mg_language));
        sprintf(buffer, "%s %d", message, error);
        return buffer;
    }

    /* Der Meldetext wird komplettiert.             */
    switch (error)
    {
    case OPTIONS_EUNKNOWN:
        sprintf(buffer, "%s '%s'",
            message, mg_fixnull(options_getunknown(desc)));
        break;
    case OPTIONS_RESERVED:
        sprintf(buffer, "%s %d", message, OPTIONS_RESERVED);
        break;
    case OPTIONS_EMISSING:
        if ((error = options_check4required(&index, desc))
            != OPTIONS_EMISSING)
            return opterror_get(OPTIONS_RESERVED, desc);

        sprintf(buffer, "%s '%s'",
            message, mg_fixnull(desc->table[index].option));
        break;
    case OPTIONS_EMISSING_ARGUMENT:
    default:
    /*@-formatconst@*/
        sprintf(buffer, message, mg_fixnull(options_geterror(desc)));
    /*@=formatconst@*/
        break;
    }

    return buffer;          /** \return Meldetext           */
}

/***************************************************************************//**
 Diese Funktion ermittelt den Meldungstext zu einem angebenen Fehler in
 der gewahlten Sprache.
*******************************************************************************/
static  char const  *mg_findmessage (
    OPTIONS         error,      /*!< [in] gemeldeter Fehler                 */
    OPTERROR_LANGUAGE language) /*!< [in] Sprache                           */
{
    MSG_TABLE_ENTRY *entry;

    /* Der Text wird in der Tabelle gesucht.                                */
    for (entry = mg_messages; entry->message != NULL; entry++)
    {
        if (entry->language == language
        && entry->error == error)
            return entry->message;
                        /** \return Meldungstext                            */
    }

    return NULL;        /** \return NULL - kein Text vorhanden              */
}

/***************************************************************************//**
 Diese Funktion konvertiert eine angegebene beliebige Referenz auf eine
 Zeichenkette in eine Referenz auf eine darstellbare Zeichenkette.
*******************************************************************************/
static  char const  *mg_fixnull (/*@returned@*/
    char const  *str)   /*!< [in] Referenz auf Zeichenkette                 */
{
    return str != NULL ? str : "(null)";
                        /** \return darstellbare Zeichenkette               */
}

#ifdef DOASSERT
/***************************************************************************//**
 Diese Funktion dient der Auffindung nicht unterstuetzter Fehlerqualitaeten
 des Moduls options waehrend der Entwicklungsphase.
*******************************************************************************/
static  int     mg_checkerrors (
    OPTIONS     options)    /*!< [in] Fehlerqualitaet                       */
{
    switch (options)
    {
    case OPTIONS_OK:
    case OPTIONS_EOL:
    case OPTIONS_ARGUMENTS_FOLLOW:
    case OPTIONS_BREAK:
    case OPTIONS_EABORT:
    case OPTIONS_EPREFIX:
    case OPTIONS_ETYPE:
    case OPTIONS_EFUNCTION:
    case OPTIONS_ENOT_INITIALIZED:
    case OPTIONS_EUNKNOWN:
    case OPTIONS_EMISSING:
    case OPTIONS_EMISSING_ARGUMENT:
    case OPTIONS_EARGUMENT:
    case OPTIONS_EVARIABLE_SIZE:
    case OPTIONS_EPROCESSING:
    case OPTIONS_EMEMORY:
    case OPTIONS_ENULLPOINTER:
    case OPTIONS_SKIP:
    case OPTIONS_RESERVED:
        return 0;
    }
/*@notreached@*/
    NTASSERT("not supported error code found!"==NULL);
/*@notreached@*/
    return !0;
}
#endif


