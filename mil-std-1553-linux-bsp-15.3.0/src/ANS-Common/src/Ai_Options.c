/* SPDX-FileCopyrightText: 2013-2021 AIM GmbH <info@aim-online.com> */
/* SPDX-License-Identifier: MIT  */

/*! \file Ai_Options.c

This file implements functions for parsing
command line options of programs

*******************************************************************************/
#ifdef WIN32
# pragma warning(disable:4996)  // sprintf unsafe warning
#endif

/* Include-Dateien ************************************************************/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include "Ai_Validate.h"
#ifdef TSTHEAP
# include "tstheap.h"
#endif
#include "Ai_NotUsed.h"
#include "Ai_Strncpy.h"
#include "Ai_Cmdline.h"
#include "Ai_Strdup.h"
#include "Ai_Options.h"

/* Konstanten *****************************************************************/

#define OPTIONSTORAGE_SIZE  128 /*!< Umfang des statischen Puffers zum      */
                                /*!< Vermerk der aktuellen Option           */
#define MIN_REQUIRED_LETTERS 3  /*!< Minimale Anzahl benoetigter Zeichen    */
                                /*!< zur Komplettierung einer 'langen'      */
                                /*!< Option                                 */

/* Options-Praefixe **/
#define STR_PREFIX_NONE     ""      /*!< kein Praefix                       */
#define STR_PREFIX_SINGLE   "-"     /*!< Praefix fuer kurze Option          */
#define STR_PREFIX_DOUBLE   "--"    /*!< Praefix fuer lange Option          */

/* Formatzeichenketten **/
#define FORMAT_SHORTINT         "%hd"
#define FORMAT_INT              "%d"
#define FORMAT_LONGINT          "%ld"
#define FORMAT_LONGLONGINT      "%Ld"
#define FORMAT_SHORTUNSIGNED    "%hu"
#define FORMAT_UNSIGNED         "%u"
#define FORMAT_LONGUNSIGNED     "%lu"
#define FORMAT_LONGLONGUNSIGNED "%Lu"

/* Makros *********************************************************************/

/**
 * Dieses Makro prueft, ob ein angegebener Tabelleneintrag bereits
 * initialisiert wurde. Ist dies nicht der Fall, so wird die Funktion
 * mit der entsprechenden Fehlerqualitaet verlassen.
 */
#define CHECK_INITIALIZED(entry, err)                   \
    {                                                   \
        if ((entry)->reserved.is_initialized == 0)      \
            return err;                                 \
    }

/**
 * Dieses Makro prueft, ob der spezifizierte Umfang einer Variablen dem
 * erforderlichen Umfang entspricht. Ist dies nicht der Fall, so wird die
 * Funktion mit der entsprechenden Fehlerqualitaet verlassen.
 */
#define VALIDATE_SIZE(this, must, err)                  \
    {                                                   \
        if ((this) != (must))                           \
            return err;                                 \
    }

/**
 * This macro adds LCLint control comments to the specified call of a reference
 * to a typified function to avoid unnecessary LCLint warnings.
 */
#define CALLFUNC(func, args)                            \
    /*@-type@*/ (func) /*@=type@*/ args

#undef DEBUG
#ifdef DODEBUG
# define DEBUG(msg) printf msg ;
#else
# define DEBUG(msg) /* no DEBUG */
#endif

#ifndef NTASSERT
# define NTASSERT(op)   /* empty */
#endif

/* Typen **********************************************************************/

/* Prototypen *****************************************************************/

 static OPTIONS     mg_parse (
    char            ***pargv,
    int             *sourceindex,
    OPTIONS_DESC    *desc);

 static OPTIONS     mg_processoption (
    char            **argv,
    int             *index,
/*@out@*/ int       *dindex,
    int             is_split,
    OPTIONS_TABLE   *table);

 static OPTIONS_TABLE   *mg_findoption (/*@returned@*/
    OPTIONS_TABLE   *table,
    char const      *option,
    OPTIONS_PREFIX  prefix);


 static char    **mg_genvector (
    char const  *tableentry);

 static int     mg_cmp_verbose_option (
    char const  *option,
    char const  *tableentry);

 static void        mg_complete (/*@unique@*/
    char            option[],
    size_t          buffersize,
    OPTIONS_PREFIX  prefix,
    OPTIONS_TABLE const *table,
    size_t          letters);

 static int     mg_optionfitsonce (
    OPTIONS_TABLE const *table,
    char const      *option,
    OPTIONS_PREFIX  prefix,
    size_t          letters);

 static size_t  mg_getoptionlength (
    OPTIONS_TABLE const *table);

 static char    **mg_argvdup (
    char        **argv);

 static void    mg_argvfree (/*@only@*//*@null@*/
    char        **argv);

 static int     mg_argvsplit (/*@unique@*/
    char        ***argv,
    int         index,
    OPTIONS_PREFIX  prefix,
    char        *thisarg,
    size_t      arglength,
    int         has_arguments,
/*@out@*/
    int         *is_split);

 static OPTIONS     mg_validatetable (
    OPTIONS_TABLE const *table);

 static size_t  mg_getargc (
    char        **argv);

 static int     mg_isvalidprefix (
    OPTIONS_PREFIX  prefix);

 static int     mg_isvalidtype (
    OPTIONS_TYPE    type);

 static OPTIONS_PREFIX  mg_getprefixof (
    char const  *option);

 static char /*@observer@*/ *mg_getprefixstr (
    OPTIONS_PREFIX  prefix);

 static char    *mg_skipprefix (/*@returned@*/
    char        *option);

 static OPTIONS mg_storenumeric (
    OPTIONS_VAR *variable,
    size_t      required,
    char const  *format,
    char        **argv);

/* Variablen ******************************************************************/

/**
 * Vermerk der aktuell in Arbeit befindlichen Option als unmittelbare Kopie
 * des entsprechenden Arguments der Kommandozeile.
 */
static  char    mg_optionstorage[OPTIONSTORAGE_SIZE];

/* Funktionen *****************************************************************/

/***************************************************************************//**
 Diese Funktion bereitet die angegebene Optionsbeschreibung fuer die
 nachfolgende Verwendung vor.
*******************************************************************************/
extern  OPTIONS     options_init (
    OPTIONS_DESC    *desc)      /*!< [in,out] Optionsbeschreibung           */
{
    OPTIONS_TABLE   *table;
    OPTIONS     options;

    /* Die Beschreibung wird verifiziert.                                   */
    VALIDATE_PTR(desc, OPTIONS_ENULLPOINTER);

    /* Die Tabelle wird verifiziert.                                        */
    if ((options = mg_validatetable(desc->table)) != OPTIONS_OK)
        return options;         /** \return siehe Funktionstyp              */

    desc->reserved.is_initialized   = !0;
    desc->reserved.unknown      = NULL;
    desc->reserved.error        = NULL;

    for (table = desc->table; table->option != NULL; table++)
    {
        table->reserved.was_found   = 0;
        table->reserved.was_error   = 0;
    }

    return OPTIONS_OK;          /** \return OPTIONS_OK                      */
}

/***************************************************************************//**
 Diese Funktion gibt den durch einer Optionsbeschreibungstabelle belegten
 dynamisch allokierten Speicher nach der Verwendung wieder frei.
*******************************************************************************/
extern  OPTIONS     options_deinit (
    OPTIONS_DESC    *desc)      /*!< [in,out] Optionsbeschreibung           */
{
    OPTIONS     options;

    /* Die Beschreibung wird verifiziert.                                   */
    VALIDATE_PTR(desc, OPTIONS_ENULLPOINTER);
    CHECK_INITIALIZED(desc, OPTIONS_ENOT_INITIALIZED);
                                /** \return OPTIONS_ENOT_INITIALIZED        */

    /* Die Tabelle wird verifiziert.                                        */
    if ((options = mg_validatetable(desc->table)) != OPTIONS_OK)
        return options;         /** \return siehe Funktionstyp              */

    if (desc->reserved.unknown != NULL)
    {
        free(desc->reserved.unknown);
        desc->reserved.unknown = NULL;
    }

    if (desc->reserved.error != NULL)
    {
        free(desc->reserved.error);
        desc->reserved.error = NULL;
    }

    desc->reserved.is_initialized   = 0;

    return OPTIONS_OK;          /** \return OPTIONS_OK                      */
}

/***************************************************************************//**
 Diese Funktion ermittelt den Text der unbekannten Option.
*******************************************************************************/
extern  char const  *options_getunknown (/*@returned@*/
    OPTIONS_DESC    *desc)      /*!< [in] Optionsbeschreibung               */
{
    /* Die Beschreibung wird verifiziert.                                   */
    VALIDATE_PTR(desc, NULL);
    CHECK_INITIALIZED(desc, NULL);

    /* Die Tabelle wird verifiziert.                                        */
    if (mg_validatetable(desc->table) != OPTIONS_OK)
        return NULL;            /** \return NULL - keine Option             */

    return desc->reserved.unknown;
                                /** \return Text der unbekannten Option     */
}

/***************************************************************************//**
 Diese Funktion ermittelt den in der Kommandozeile angegebenen Text der
 fehlerhaften Option.
*******************************************************************************/
extern  char const  *options_geterror (/*@returned@*/
    OPTIONS_DESC    *desc)      /*!< [in] Optionsbeschreibung               */
{
    /* Die Beschreibung wird verifiziert.                                   */
    VALIDATE_PTR(desc, NULL);
    CHECK_INITIALIZED(desc, NULL);

    /* Die Tabelle wird verifiziert.                                        */
    if (mg_validatetable(desc->table) != OPTIONS_OK)
        return NULL;            /** \return NULL - keine Option             */

    return desc->reserved.error;
                                /** \return Text der fehlerhaften Option    */
}

/***************************************************************************//**
 Diese Funktion wertet die Argumentenliste anhand der Optionsbeschreibung
 aus. Der Index innerhalb der Kommandozeile wird als Ausgangspunkt
 verwendet und entsprechend aktualisiert. Nach der Ausfuehrung bezeichnet
 der Index die erste Stelle der Kommandozeile, welche nicht ausgewertet
 werden konnte bzw. zum Abbruch der Optionsverarbeitung durch einen
 Fehlerauftritt fuehrte.
*******************************************************************************/
extern  OPTIONS     options_parse (
    char            **argv, /*!< [in] Argumentenliste                       */
    int             *index, /*!< [in,out] aktueller Index                   */
    OPTIONS_DESC    *desc)  /*!< [in] Optionsbeschreibung                   */
{
    OPTIONS     options;
    char        **newargv = NULL;

    /* NULL-Referenzen werden erkannt.                                      */
    VALIDATE_PTR(argv, OPTIONS_ENULLPOINTER);
    VALIDATE_PTR(index, OPTIONS_ENULLPOINTER);
    VALIDATE_PTR(desc, OPTIONS_ENULLPOINTER);
    VALIDATE_PTR(desc->table, OPTIONS_ENULLPOINTER);
                                /** \return OPTIONS_ENULLPOINTER            */

    CHECK_INITIALIZED(desc, OPTIONS_ENOT_INITIALIZED);
                                /** \return OPTIONS_ENOT_INITIALIZED        */

    /* Die Kommandozeile wird dupliziert. (Im folgenden wird                */
    /* ausschliesslich die Kopie verwendet.                                 */
    if ((newargv = mg_argvdup(argv)) == NULL)
        return OPTIONS_EMEMORY; /** \return OPTIONS_EMEMORY                 */

    /* Die Kommandozeile wird ausgewertet.                                  */
    options = mg_parse(&newargv, index, desc);

    /* Die duplizierte Kommandozeile wird freigegeben.                      */
    mg_argvfree(newargv);

    return options;             /** \return siehe Funktionstyp              */
}

/***************************************************************************//**
 Diese Funktion wertet die Argumentenliste anhand der Optionsbeschreibung
 aus. Der Index innerhalb der Kommandozeile wird als Ausgangspunkt
 verwendet und entsprechend aktualisiert. Nach der Ausfuehrung bezeichnet
 der Index die erste Stelle der Kommandozeile, welche nicht ausgewertet
 werden konnte bzw. zum Abbruch der Optionsverarbeitung durch einen
 Fehlerauftritt fuehrte.
*******************************************************************************/
static  OPTIONS     mg_parse (
    char            ***pargv,       /*!< [in,out] Argumentenliste           */
    int             *sourceindex,   /*!< [in,out] aktueller Index           */
    OPTIONS_DESC    *desc)          /*!< [in,out] Optionsbeschreibung       */
{
    char            argbuffer[OPTIONSTORAGE_SIZE];
    int             index;
    int             dindex;
    int             is_split;
    OPTIONS         options;
    OPTIONS_TABLE   *thisoption;
    char            *thisarg;
    size_t          length;
    OPTIONS_PREFIX  prefix;

    VALIDATE_PTR(pargv, OPTIONS_ENULLPOINTER);
    VALIDATE_PTR(*pargv, OPTIONS_ENULLPOINTER);
    VALIDATE_PTR(sourceindex, OPTIONS_ENULLPOINTER);
    VALIDATE_PTR(desc, OPTIONS_ENULLPOINTER);
    VALIDATE_PTR(desc->table, OPTIONS_ENULLPOINTER);

    index   = *sourceindex;

    /* Der angegebene Index wird verifiziert.                               */
    if (index >= (int) mg_getargc(*pargv))
        return OPTIONS_EOL;     /** \return OPTIONS_EOL                     */

    /* Die Kommandozeile wird ausgewertet.                                  */
    while ((*pargv)[index] != NULL)
    {
        /* Die anstehende Option wird referenziert und vermerkt.            */
        thisarg = (*pargv)[index];
        (void) STRNCPY(mg_optionstorage,
            thisarg, sizeof(mg_optionstorage));

        /* Das Optionspraefix wird festgestellt und anschliessend           */
        /* ueberlesen.                                                      */
        prefix  = mg_getprefixof(thisarg);
        thisarg = mg_skipprefix(thisarg);

        /* Die anstehende Option wird lokal zwischengespeichert             */
        /* und anschliessend komplettiert.                                  */
        thisarg = STRNCPY(argbuffer, thisarg, sizeof(argbuffer));
        mg_complete(argbuffer, sizeof(argbuffer), prefix, desc->table,
            MIN_REQUIRED_LETTERS);

        /* Der entsprechende Tabelleneintrag wird ermittelt.                */
        thisoption = mg_findoption(desc->table, thisarg, prefix);
        if (thisoption == NULL)
        {
            /* Ein evtl. anstehendes, nicht einem Eintrag                   */
            /* der Optionstabelle zugeordnetes Argument                     */
            /* wird dem Aufrufer mitgeteilt.                                */
            if (prefix == OPTIONS_PREFIX_NONE)
                return OPTIONS_ARGUMENTS_FOLLOW;
                                /** \return OPTIONS_ARGUMENTS_FOLLOW        */

            /* Eine unbekannte Option wird vermerkt und                     */
            /* der weitere Parse-Vorgang abgebrochen.                       */
            desc->reserved.unknown = str_dup((*pargv)[index]);
            return OPTIONS_EUNKNOWN;    /** \return OPTIONS_EUNKNOWN        */
        }

        NTASSERT(thisoption->function != NULL);
        NTASSERT(thisoption->priorfunction != NULL);
        NTASSERT(thisoption->postfunction != NULL);

        /* Die Laenge des Bezeichners wird ermittelt.                       */
        length = mg_getoptionlength(thisoption);

        /* Das aktuelle Argument wird gesplittet, falls notwendig.          */
        if (mg_argvsplit(pargv, index, prefix, thisarg, length,
            thisoption->flags.has_arguments, &is_split))
            return OPTIONS_EMEMORY; /** \return OPTIONS_EMEMORY */

        NTASSERT(is_split == 0 || is_split == !0);

        /* Die Option wird abgearbeitet.                                    */
        options = mg_processoption(
            *pargv, &index, &dindex, is_split, thisoption);
        if (options != OPTIONS_OK)
        {
            /* Bei einem Fehlerauftritt wird die Option als Fehlerhaft      */
            /* markiert und die Option vermerkt.                            */
            thisoption->reserved.was_error = !0;
            desc->reserved.error = str_dup((*pargv)[index]);

            /* Unzulaessige Fehlerqualitaeten werden erkannt.               */
            NTASSERT(options != OPTIONS_SKIP);
            if (options == OPTIONS_SKIP)
                return OPTIONS_EPROCESSING;

            return options;     /** \return siehe Funktionstyp              */
        }

        /* Bei erfolgreicher Abarbeitung wird die Option als gefunden       */
        /* gekennzeichnet.                                                  */
        thisoption->reserved.was_found = !0;

        /* Der Quellindex wird justiert.                                    */
        *sourceindex += dindex - (is_split ? 1 : 0);
    }

    return OPTIONS_OK;          /** \return OPTIONS_OK                      */
}

/***************************************************************************//**
 Diese Funktion prueft im Anschluss an die Ausfuehrung, ob alle
 dringend benoetigten Optionen spezifiziert wurden.
 Wird eine nicht spezifizierte Option gefunden, so wird deren
 Index innerhalb der Optionstabelle zurueckgegeben.
*******************************************************************************/
extern  OPTIONS     options_check4required (/*@out@*/
    int                 *index, /*!< [out] gefundener Index                 */
    OPTIONS_DESC const  *desc)  /*!< [in] Optionsbeschreibung               */
{
    OPTIONS_TABLE const *table;
    int                 i;

    /* NULL-Referenzen werden erkannt.                                      */
/*@-mustdefine@*/
    VALIDATE_PTR(index, OPTIONS_ENULLPOINTER);
    *index = 0;
/*@=mustdefine@*/
    VALIDATE_PTR(desc, OPTIONS_ENULLPOINTER);
    VALIDATE_PTR(desc->table, OPTIONS_ENULLPOINTER);
                            /** \return OPTIONS_ENULLPOINTER                */

    CHECK_INITIALIZED(desc, OPTIONS_ENOT_INITIALIZED);
                            /** \return OPTIONS_ENOT_INITIALIZED            */

    /* Nicht spezifizierte Optionen werden gesucht.                         */
    table = desc->table;
    for (i = 0; table[i].option != NULL; i++)
    {
        if (table[i].flags.is_required == 0
        || table[i].reserved.was_found)
            continue;

        *index = i;

        return OPTIONS_EMISSING;    /** \return OPTIONS_EMISSING            */
    }

    return OPTIONS_OK;              /** \return OPTIONS_OK                  */
}

/***************************************************************************//**
 Diese Funktion prueft, ob die angegebene Fehlerqualitaet einen
 aufgetretenen kritischen Fehler spezifiziert.
*******************************************************************************/
extern  int     options_iscritical (
    OPTIONS     error)  /*!< [in] Fehlerqualitaet                           */
{
    /* Nichtkritische Fehlerqualitaeten werden ignoriert.                   */
    return error != OPTIONS_OK
    && error != OPTIONS_EOL
    && error != OPTIONS_ARGUMENTS_FOLLOW
    && error != OPTIONS_BREAK
    && error != OPTIONS_EOL;
                        /** \return !0 - Fehlerqualitaet spezifiziert       */
                        /** \return einen kritischen Fehler, sonst 0        */
}

/***************************************************************************//**
 Diese Funktion ermittelt die waehrend der Optionsverarbeitung aktuell in
 Arbeit befindliche Option.

 Bsp.:

    Aufruf  Rueckgabe
    ------- --------------------------
    -t  -t
    -tx (1) -tx
        (2) -x
    -targ   -targ

 Bem.:
 (1) - Die Verwendung dieser Funktion ist nur aus einer optionsverarbeitenden
 Funktion zulaessig. Wird aus einer solchen Funktion eine weitere
 Optionsverarbeitung initiiert, so wird der aktuelle Wert mit der zuletzt
 verarbeiteten Option ueberschrieben.

 (2) - Die durch Verwendung dieser Funktion ermittelte Referenz bezieht
 sich _nicht_ auf die entsprechende Speicherstelle innerhalb der
 Kommandozeile. D.h., weitere Argumente der Kommandozeile koennen hiermit
 nicht adressiert werden.

 Komplettierte 'lange' Optionen werden in der in der Kommandozeile
 angegebenen Form zurueckgegeben.
 Bsp.: '--hel' fuer '--help' liefert '--hel' zurueck.
*******************************************************************************/
extern  char const /*@temp@*/ *options_getcurrent (void)
{
    return mg_optionstorage;    /** \return Referenz auf aktuelle Option    */
}

/***************************************************************************//**
 Diese Funktion arbeitet eine gefundene Option anhand deren Spezifikation
 ab. Der Index innerhalb der Kommandozeile wird bei erfolgreicher
 Ausfuehrung auf das nachfolgende Argument justiert. Die Anzahl verarbeiteter
 Argumente wird dem Aufrufer mitgeteilt.
 Eine evtl. stattgefundene Aufteilung der aktuellen Option ist bei Aufruf
 dieser Funktion anzugeben.
*******************************************************************************/
static  OPTIONS     mg_processoption (
    char            **argv,     /*!< [in] Argumentenliste                   */
    int             *index,     /*!< [in,out] aktueller Index               */
/*@out@*/ int       *dindex,    /*!< [out] Anzahl verarbeiteter             */
                                /*!< [out] Argumente                        */
    int             is_split,   /*!< [in] Schalter: Aufteilung der          */
                                /*!< [in] Option hat stattgefunden          */
    OPTIONS_TABLE   *table)     /*!< [in] Optionstabellen-Eintrag           */
{
    OPTIONS options;
    int argc;
    int nprocessed = 0;
    int args2skip;
    int has_no_optiontext;
/*@-mustdefine@*/
    VALIDATE_PTR(dindex, OPTIONS_ENULLPOINTER);
    *dindex = 0;
/*@=mustdefine@*/
    VALIDATE_PTR(argv, OPTIONS_ENULLPOINTER);
    VALIDATE_PTR(index, OPTIONS_ENULLPOINTER);
    VALIDATE_PTR(table, OPTIONS_ENULLPOINTER);
    VALIDATE_PTR(table->function, OPTIONS_ENULLPOINTER);
    VALIDATE_PTR(table->postfunction, OPTIONS_ENULLPOINTER);
    VALIDATE_PTR(table->priorfunction, OPTIONS_ENULLPOINTER);
                                /** \return OPTIONS_ENULLPOINTER            */

    NTASSERT(is_split == 0 || is_split == !0);

    /* Falls die aktuelle Option kein Optionstext enthaelt, so              */
    /* wird dies fuer die spaetere Beachtung festgehalten.                  */
    has_no_optiontext = mg_getoptionlength(table) == 0;

    /* Die verzeichnete zuvorderst auszufuehrende Funktion wird             */
    /* ausgefuehrt.                                                         */
    options = CALLFUNC(
        table->priorfunction,
            (table->userdefined, argv + *index, is_split));
    switch (options)
    {
    case OPTIONS_OK:
        break;
    case OPTIONS_SKIP:
        /* Falls die aktuelle Option ueberlesen werden soll,                */
        /* so wird die Verarbeitung nach Korrektur der Indizes              */
        /* abgebrochen.                                                     */
        ++*index;
        ++*dindex;
        return OPTIONS_OK;
    default:
        return options;
    }

    /* Falls die Option keine Argumente erwartet, so wird die               */
    /* verzeichnete Funktion aktiviert.                                     */
    if (!table->flags.has_arguments)
    {
        if ((options = CALLFUNC(table->function,
            (
            &table->variable,
            table->userdefined,
            argv + *index + (has_no_optiontext ? 0 : 1),
            &nprocessed
            )
            )) != OPTIONS_OK)
            return options;         /** \return siehe Funktionstyp          */

        /* Die verzeichnete nachfolgende Funktion wird ausgefuehrt.         */
        if ((options = CALLFUNC(
            table->postfunction,
            (&table->variable, table->userdefined)))
            != OPTIONS_OK)
            return options;

        /* Der Index wird auf das Folgeargument justiert.                   */
        ++*index;
        ++*dindex;

        return OPTIONS_OK;
    }

    /* Verfuegt die Spezifikation der aktuellen Option weder ueber den      */
    /* Eintrag eines Praefixes noch des eines Optionstextes, so wird die    */
    /* anstehende Option unmittelbar als zu verarbeitendes Argument         */
    /* verwendet.                                                           */
    args2skip = table->prefix != OPTIONS_PREFIX_NONE
        || table->option[0] != '\0';

    /* Die Anzahl maximal verarbeitbarer Argumente wird festgehalten.       */
    argc = mg_getargc(argv) - args2skip;
    NTASSERT(argc >= 0);

    /* Die Option mit Argumenten wird verarbeitet.                          */
    if ((options = CALLFUNC(table->function,
        (
        &table->variable, table->userdefined,
        argv + *index + args2skip, &nprocessed))) != OPTIONS_OK)
        return options;

    /* Die Anzahl verarbeiteter Argumente wird verifiziert.                 */
    NTASSERT(nprocessed >= 0 && nprocessed <= argc);
    if (nprocessed < 0 || nprocessed > argc)
        return OPTIONS_EPROCESSING;
                    /** \return OPTIONS_EPROCESSING */

    /* Die verzeichnete nachfolgende Funktion wird ausgefuehrt.             */
    if ((options = CALLFUNC(table->postfunction,
        (&table->variable, table->userdefined))) != OPTIONS_OK)
        return options;

    /* Der Index innerhalb der Kommandozeile wird justiert.                 */
    *dindex = args2skip + nprocessed;
    *index += *dindex;

    return OPTIONS_OK;          /** \return OPTIONS_OK                      */
}

/***************************************************************************//**
 Diese Funktion sucht zur angegebenen Optionszeichenkette den entsprechenden
 Eintrag innerhalb der Optionstabelle.
 Unvollstaendig angegebene Optionen werden hierbei nicht vervollstaendigt.
*******************************************************************************/
static  OPTIONS_TABLE   *mg_findoption (/*@returned@*/
    OPTIONS_TABLE   *table,     /*!< [in] Tabelle der Optionen              */
    char const      *option,    /*!< [in] Optionszeichenkette               */
    OPTIONS_PREFIX  prefix)     /*!< [in] Typ des zugehoerigen              */
                                /*!< [in] Praefixes                         */
{
    VALIDATE_PTR(table, NULL);
    VALIDATE_PTR(option, NULL);

    for ( ; table->option != NULL; table++)
    {
        /* Das Praefix wird verglichen.                                     */
        if (table->prefix != prefix)
            continue;

        /* Der Bezeichner wird verglichen.                                  */
        /* Verfuegt der aktuelle Tabelleneintrag mit identischem Praefix    */
        /* nicht ueber einen Bezeichner, dann gilt die Option als gefunden. */
        if (table->option[0] == '\0')
            return table;

        switch (table->type)
        {
        case OPTIONS_TYPE_VERBOSE:
            if (mg_cmp_verbose_option(option, table->option) == 0)
                return table;       /** \return Referenz auf Tabelleneintrag */
            break;

        case OPTIONS_TYPE_SHORT:
            if (option[0] != '\0'
            && strchr(table->option, option[0]) != NULL)
                return table;
            break;

        default:
            NTASSERT("Unknown options type"==NULL);
            return NULL;
        }
    }

    return NULL;                /** \return NULL - Eintrag nicht gefunden   */
}

/***************************************************************************//**
 Diese Funktion generiert einen NULL-terminierten Vektor aus
 allen Optionen der angegebenen Whitespace-separierten Optionsliste
 des Tabelleneintrags.
*******************************************************************************/
static char     **mg_genvector (
    char const  *tableentry)    /*!< [in] Tabelleneintrag       */
{
    char        *buffer;
    char        ch;
/*@dependent@*/
    char        *writeptr;
    char const  *readptr;
    int         newargc = 0;
    char        **newargv = NULL;

    VALIDATE_PTR(tableentry, NULL);

    /* Ein temporaerer Puffer im Umfang des Tabelleneintrags wird erzeugt.  */
    if ((buffer = str_dup(tableentry)) == NULL)
        return NULL;            /** \return NULL - zuwenig Speicher         */

    DEBUG(("genvector: '%s'\n", tableentry))

    readptr = tableentry;
    writeptr = buffer;

    /* Die einzelnen Whitespace-separierten Tabellen-Eintraege werden       */
    /* gesucht.                                                             */
    for (;;)
    {
        ch = *readptr;

        /* Alle Zeichen bis zum naechsten Whitespace bzw.                   */
        /* dem Ende der Zeichenkette werden kopiert.                        */
        if (ch != '\0' && !isspace((int) ch))
        {
            *writeptr++ = *readptr++;
            continue;
        }

        /* Die gelesene Zeichenkette wird terminiert und mit                */
        /* dem Optionstext verglichen.                                      */
        *writeptr = '\0';

        DEBUG(("genvector: + '%s'\n", buffer))

        if (cmdline_append(&newargc, &newargv, buffer))
        {
            DEBUG(("genvector: cmdline_append out of memory!\n"))
            free(buffer);
            cmdline_free(newargv);
            return NULL;
        }

        /* Alle nachfolgenden Whitespaces werden ueberlesen.                */
        while (isspace((int) *readptr) && *readptr != '\0')
            ++readptr;

        writeptr = buffer;

        if ('\0' == *readptr)
            break;
    }

    free(buffer);

    return newargv;         /** \return Vektor                              */
}

/***************************************************************************//**
 Diese Funktion vergleicht die angegebene Optionszeichenkette mit
 dem angegebenen Eintrag der Optionstabelle des Typs OPTIONS_TYPE_VERBOSE.
*******************************************************************************/
static  int     mg_cmp_verbose_option (
    char const  *option,        /*!< [in] Optionszeichenkette               */
    char const  *tableentry)    /*!< [in] Tabelleneintrag                   */
{
    char        **vector;
    char        **thisptr;

    VALIDATE_PTR(option, !0);
    VALIDATE_PTR(tableentry, !0);

    /* Ein Vektor mit den enthaltenen einzelnen Optionsvarianten            */
    /* wird erzeugt.                                                        */
    if ((vector = mg_genvector(tableentry)) == NULL)
        return !0;

    /* Die einzelnen Optionsvarianten werden verglichen.                    */
/*@-usereleased@*/
    for (thisptr = vector; *thisptr != NULL; thisptr++)
    {
        if (strcmp(option, *thisptr) == 0)
        {
            cmdline_free(vector);
            return 0;
        }
    }
/*@=usereleased@*/

    cmdline_free(vector);

    return !0;                  /** \return !0 - nicht gefunden             */
}

/***************************************************************************//**
 Diese Funktion komplettiert die im angegebenen Speicherbereich
 spezifizierter Groesse abgelegte Option anhand der Optionstabelle,
 falls dies moeglich ist.
 Komplettiert werden koennen nur 'lange' Optionen mit einer gegebenen
 Zahl mindestens zutreffender Zeichen.
*******************************************************************************/
static void         mg_complete (/*@unique@*/
    char            option[],   /*!< [in,out] Optionszeichenkette           */
    size_t          buffersize, /*!< [in] Umfang des Puffers                */
    OPTIONS_PREFIX  prefix,     /*!< [in] Typ des zugehoerigen              */
                                /*!< [in] Praefixes                         */
    OPTIONS_TABLE const *table, /*!< [in] Tabelle der Optionen              */
    size_t          letters)    /*!< [in] signifikante Laenge               */
{
    char        **vector;
/*@dependent@*/
    char        **thisptr;

    VALIDATE_PTR_VOID(option);
    VALIDATE_PTR_VOID(table);

    NTASSERT(buffersize > 0);
    NTASSERT(letters > 0);

    /* Die Zulaessigkeit der Option fuer eine Komplettierung                */
    /* wird festgestellt.                                                   */
    if (strlen(option) < letters)
        return;

    /* Die Laenge der bestehenden Optionszeichenkette wird                  */
    /* fuer nachfolgende Vergleiche festgestellt.                           */
    letters = strlen(option);

    /* Die Komplettierung anhand der Optionstabelle wird durchgefuehrt.     */
    for ( ; table->option != NULL; table++)
    {
        /* Das Praefix sowie der Optionstyp werden verglichen.              */
        if (table->prefix != prefix
        || table->type != OPTIONS_TYPE_VERBOSE)
            continue;

        /* Ein Vektor mit den Optionsvarianten wird erzeugt.                */
        if ((vector = mg_genvector(table->option)) == NULL)
            continue;

/*@-usereleased@*/
        for (thisptr = vector; *thisptr != NULL; thisptr++)
        {
            /* Generell unzutreffende Tabelleneintraege werden ueberlesen.  */
            if (strncmp(*thisptr, option, letters) != 0)
                continue;

            /* Falls die gegebene Option nicht genau einmal zutreffend      */
            /* ist, dann kann keine Komplettierung erfolgen.                */
            if (!mg_optionfitsonce(table, option, prefix, letters))
            {
                cmdline_free(vector);
                return;
            }

            /* Die gefundene Option wird komplettiert.                      */
            (void) STRNCPY(option, *thisptr, buffersize);

            cmdline_free(vector);
            return;
        }
/*@=usereleased@*/

        cmdline_free(vector);
    }
}

/***************************************************************************//**
 Diese Funktion prueft, ob die angegebene Optionszeichenkette einer 'langen'
 Option genau einmal mit der gegebenen signifikanten Zeichenkettenlaenge
 auf einen Eintrag der Optionstabelle zutrifft.
*******************************************************************************/
static  int     mg_optionfitsonce (
    OPTIONS_TABLE const *table, /*!< [in] Tabelle der Optionen              */
    char const      *option,    /*!< [in] Optionszeichenkette               */
    OPTIONS_PREFIX  prefix,     /*!< [in] Typ des zugehoerigen              */
                                /*!< [in] Praefixes                         */
    size_t      letters)        /*!< [in] signifikante Laenge               */
{
    char        **vector;
    char        **thisptr;
    int     hits = 0;

    VALIDATE_PTR(table, 0);
    VALIDATE_PTR(option, 0);

    /* Optionszeichenketten unzureichender Laenge werden erkannt.           */
    if (strlen(option) < letters)
        return 0;

    /* Die Optionstabelle wird durchsucht.                                  */
    for ( ; table->option != NULL; table++)
    {
        /* Das Praefix wird verglichen.                                     */
        /* Der passende Optionstyp wird sichergestellt.                     */
        if (table->prefix != prefix
        || table->type != OPTIONS_TYPE_VERBOSE)
            continue;

        /* Ein Vektor mit den Optionsvarianten wird erzeugt.                */
        if ((vector = mg_genvector(table->option)) == NULL)
            return 0;

/*@-usereleased@*/
        /* Zutreffende Tabelleneintraege werden gezaehlt.                   */
        for (thisptr = vector; *thisptr != NULL; thisptr++)
        {
            if (strncmp(*thisptr, option, letters) == 0)
                ++hits;
        }
/*@=usereleased@*/

        cmdline_free(vector);
    }

    return hits == 1;       /** \return !0 - Option trifft genau            */
                            /** \return einmal zu, sonst 0                  */
}

/***************************************************************************//**
 Diese Funktion ermittelt die Laenge eines Optionsbezeichners.
*******************************************************************************/
static  size_t      mg_getoptionlength (
    OPTIONS_TABLE const *table)     /*!< [in] Optionseintrag                */
{
    size_t  length;

    VALIDATE_PTR(table, 0);
    VALIDATE_PTR(table->option, 0);

    /* Die Laenge der Optionszeichenkette wird ermittelt.                   */
    length = strlen(table->option);

    /* Ist keine Optionszeichenkette vorhanden, bzw. stellt die             */
    /* Option eine 'ausfuehrliche' Option dar, so ist die                   */
    /* ermittelte Laenge verbindlich.                                       */
    if (length == 0
    || table->type == OPTIONS_TYPE_VERBOSE)
        return length;

    /* Sonst liegt eine 'kurze' Option vor. D.h., die Laenge ist            */
    /* generell ein Zeichen.                                                */
    return sizeof(char);        /** \return Laenge des Bezeichners          */
}

/***************************************************************************//**
 Diese Funktion dupliziert eine Kommandozeile. Der Speicherplatz zur Aufnahme
 der Kommandozeile wird dynamisch allokiert und muss deshalb nach
 Gebrauch wieder freigegeben werden.
*******************************************************************************/
static char     **mg_argvdup (
    char        **argv)     /*!< [in] Kommandozeile                         */
{
    char    **newargv = NULL;
    size_t  entries;
    size_t  ii;

    VALIDATE_PTR(argv, NULL);

    /* Die Anzahl Elemente der Kommandozeile wird ermittelt.                */
    if ((entries = mg_getargc(argv)) < 1)
        return NULL;    /** \return NULL - leere Kommandozeile              */

    /* Die Kommandozeile wird dupliziert.                                   */
    if ((newargv = (char **) calloc(entries + 1, sizeof(char *))) == NULL)
        return NULL;    /** \return NULL - zuwenig Speicher                 */

    for (ii = 0; ii < entries; ii++)
    {
        /* Die Argumente werden umkopiert.                                  */
        if ((newargv[ii] = str_dup(argv[ii])) == NULL)
        {
            mg_argvfree(newargv);
            return NULL;
        }

        /* Die Argumentenliste wird terminiert.                             */
        newargv[ii + 1] = NULL;
    }

    return newargv;         /** \return duplizierte Kommandozeile           */
}

/***************************************************************************//**
 Diese Funktion gibt den durch eine dynamisch allokierte Kommandozeile
 belegten Speicher wieder frei.
*******************************************************************************/
static void     mg_argvfree (/*@only@*//*@null@*/
    char        **argv)     /*!< [in,out] Kommandozeile                     */
{
    int ii;

    if (argv == NULL)
        return;

/*@-unqualifiedtrans@*/

    /* Die Elemente der Kommandozeile werden freigegeben.                   */
    for (ii = 0; argv[ii] != NULL; ii++)
        free(argv[ii]);

/*@=unqualifiedtrans@*/

    /* Die Kommandozeile wird freigegeben.                                  */
    free(argv);
}

/***************************************************************************//**
 Diese Funktion splittet die Kommandozeile an der Position der
 aktuellen Option, falls die Option nicht als separates Argument
 vorliegt.

 Bem.: Optionen in ausfuehrlicher Schreibweise werden nicht gesplittet.
*******************************************************************************/
static int      mg_argvsplit (/*@unique@*/
    char            ***argv,    /*!< [in,out] Kommandozeile                 */
    int             index,      /*!< [in] aktueller Index                   */
    OPTIONS_PREFIX  prefix,     /*!< [in] aktueller Praefix                 */
    char            *thisarg,   /*!< [in] Referenz auf Beginn des           */
                                /*!< [in] Optionsbezeichners                */
    size_t          arglength,  /*!< [in] Laenge des Bezeichners            */
    int         has_arguments,  /*!< [in] Flag: Option verarbeitet          */
                                /*!< [in] Argumente                         */
/*@out@*/ int       *is_split)  /*!< [out] Indikator fuer erfolgte          */
                                /*!< [out] Durchfuehrung                    */
{
    char    *prefixstr = "";
    size_t  prefixlength;
    size_t  ii;
    size_t  entries;
    char    **newargv = NULL;

/*@-mustdefine@*/
    VALIDATE_PTR(is_split, !0);
    *is_split = 0;
/*@=mustdefine@*/
    VALIDATE_PTR(argv, !0);
    VALIDATE_PTR(*argv, !0);
    VALIDATE_PTR(thisarg, !0);

    NTASSERT(index >= 0);

    /* Die Laenge des Bezeichners wird verifiziert.                         */
    /* Optionsbezeichner, welche nicht der Laenge eines einzelnen           */
    /* Zeichen entsprechen, werden nicht gesplittet.                        */
    /* Liegt die Option gesondert vor, so ist die Aufteilung                */
    /* nicht erforderlich.                                                  */
    if (arglength != 1 || thisarg[arglength] == '\0')
        return 0;   /** \return 0 - Ausfuehrung in Ordnung                  */

    /* Die Laenge des Praefixes wird vermerkt.                              */
    prefixstr = mg_getprefixstr(prefix);
    prefixlength = strlen(prefixstr);

    /* Speicher fuer die Aufnahme der expandierten Kommandozeile            */
    /* wird allokiert.                                                      */
    entries = mg_getargc(*argv);
    NTASSERT(entries > 0);

    if ((newargv = (char **) calloc(entries + 2, sizeof(char *))) == NULL)
        return !0;  /** \return !0 - zuwenig Speicher                       */

    /* Die Argumente vor der aktuellen Position werden einkopiert.          */
    for (ii = 0; (int) ii < index; ii++)
    {
        DEBUG(("copy (1): '%s' at %d\n", (*argv)[ii], (int) ii))
        newargv[ii] = (*argv)[ii];
    }

    /* Das aktuelle Argument wird generiert.                                */
    if ((newargv[index] = str_dup((*argv)[index])) == NULL)
    {
        free(newargv);
        return !0;
    }
    newargv[index][arglength + prefixlength] = '\0';
    DEBUG(("new current arg is '%s' at %d\n", newargv[index], index))

    /* Das Folgeargument wird generiert. Verarbeitet die aktuelle           */
    /* Option keine Argumente, so wird das Optionspraefix                   */
    /* dem Folgeargument vorangestellt.                                     */
    newargv[index + 1] = (char *) malloc(
        prefixlength + strlen(thisarg + arglength) + 1);
    if (newargv[index + 1] == NULL)
    {
        free(newargv[index]);
        free(newargv);
        return !0;
    }

    sprintf(newargv[index + 1], "%s%s",
        (has_arguments ? "" : prefixstr),
        thisarg + arglength);
    DEBUG(("new next arg    is '%s' at %d\n", newargv[index+1], index+1))

    /* Die nachfolgenden Argumente werden verschoben.                       */
    for (ii = (size_t) index + 1; ii < entries; ii++)
    {
        newargv[ii + 1] = (*argv)[ii];
        DEBUG(("copy (2): '%s' at %d\n", newargv[ii+1], ii+1))
    }
    newargv[entries + 1] = NULL;

/*@-unqualifiedtrans@*/
    /* Die aktuelle Option wird freigegeben.                                */
    free((*argv)[index]);

    /* Die neue Kommandozeile wird aktiviert.                               */
    free(*argv);
    *argv = newargv;
/*@=unqualifiedtrans@*/

    *is_split = !0;

/*@-nullstate@*/
    return 0;
/*@=nullstate@*/
}

/***************************************************************************//**
 Diese Funktion prueft eine vorliegende (nicht initialisierte Tabelle)
 auf ihre Zulaessigkeit.
*******************************************************************************/
static  OPTIONS     mg_validatetable (
    OPTIONS_TABLE const *table) /*!< [in] Tabelle der Optionen              */
{
    OPTIONS_TABLE const *entry = table;

    /* NULL-Referenzen werden erkannt.                                      */
    VALIDATE_PTR(table, OPTIONS_ENULLPOINTER);
                            /** \return OPTIONS_ENULLPOINTER                */

    /* Die Tabelleneintraege werden verifiziert.                            */
    for (entry = table; entry->option != NULL; entry++)
    {
        /* Der Praefix wird geprueft.                                       */
        if (!mg_isvalidprefix(entry->prefix))
            return OPTIONS_EPREFIX; /** \return OPTIONS_EPREFIX             */

        /* Der Optionstyp wird geprueft.                                    */
        if (!mg_isvalidtype(entry->type))
            return OPTIONS_ETYPE;   /** \return OPTIONS_ETYPE               */

        /* Die installierte Funktion wird geprueft.                         */
        if (entry->function == NULL
        || entry->priorfunction == NULL
        || entry->postfunction == NULL)
            return OPTIONS_EFUNCTION;
                            /** \return OPTIONS_EFUNCTION                   */
    }

    return OPTIONS_OK;      /** \return OPTIONS_OK                          */
}

/***************************************************************************//**
 Diese Funktion ermittelt die Anzahl in einer Kommandozeile enthaltener
 Argumente.
*******************************************************************************/
static size_t       mg_getargc (
    char            **argv) /*!< [in] Kommandozeile                         */
{
    size_t  entries = 0;

    VALIDATE_PTR(argv, 0);

    for (entries = 0; *argv != NULL; entries++)
        ++argv;

    return entries;     /** \return Anzahl enthaltener Argumente            */
}

/***************************************************************************//**
 Diese Funktion prueft, ob ein angegebener Optionspraefix zulaessig ist.
*******************************************************************************/
static  int     mg_isvalidprefix (
    OPTIONS_PREFIX  prefix)     /*!< [in] Optionspreaefix                   */
{
    return prefix == OPTIONS_PREFIX_NONE
    || prefix == OPTIONS_PREFIX_SINGLE
    || prefix == OPTIONS_PREFIX_DOUBLE;
                        /** \return 0 - unzulaessiger Praefix, sonst !0     */
}

/***************************************************************************//**
 Diese Funktion prueft, ob ein angegebener Optionstyp zulaessig ist.
*******************************************************************************/
static int          mg_isvalidtype (
    OPTIONS_TYPE    type)       /*!< [in] Optionstyp                        */
{
    return type == OPTIONS_TYPE_SHORT
    || type == OPTIONS_TYPE_VERBOSE;
                        /** \return 0 - unzulaessiger Typ, sonst !0         */
}

/***************************************************************************//**
 Diese Funktion ermittelt den Typ des Praefixes einer anliegenden
 Option.
*******************************************************************************/
static OPTIONS_PREFIX   mg_getprefixof (
    char const  *option)    /*!< [in] Option                                */
{
    VALIDATE_PTR(option, OPTIONS_PREFIX_NONE);

    /* Das Vorliegen von langen Optionen wird geprueft.                     */
    if (strncmp(option, STR_PREFIX_DOUBLE, strlen(STR_PREFIX_DOUBLE)) == 0)
        return OPTIONS_PREFIX_DOUBLE;
                            /** \return OPTIONS_PREFIX_DOUBLE               */

    /* Das Vorliegen von kurzen Optionen im UNIX-Stil wird geprueft.        */
    if (strncmp(option, STR_PREFIX_SINGLE, strlen(STR_PREFIX_SINGLE)) == 0)
        return OPTIONS_PREFIX_SINGLE;
                            /** \return OPTIONS_PREFIX_SINGLE               */

    return OPTIONS_PREFIX_NONE; /** \return OPTIONS_PREFIX_NONE             */
}

/***************************************************************************//**
 Diese Funktion ermittelt die zu einem Praefix gehoerende Zeichenkette.
*******************************************************************************/
static char /*@observer@*/ *mg_getprefixstr (
    OPTIONS_PREFIX  prefix)     /*!< [in] Praefix                           */
{
    if (prefix == OPTIONS_PREFIX_DOUBLE)
        return STR_PREFIX_DOUBLE;

    if (prefix == OPTIONS_PREFIX_SINGLE)
        return STR_PREFIX_SINGLE;

    return STR_PREFIX_NONE;     /** \return Zeichenkette                    */
}

/***************************************************************************//**
 Diese Funktion positioniert auf das dem Praefix folgende Zeichen
 innerhalb der Zeichenkette.
*******************************************************************************/
static char     *mg_skipprefix (/*@returned@*/
    char        *option)    /*!< [in] Option                                */
{
    char    *nextpos = option;

    VALIDATE_PTR(option, NULL);

    switch (mg_getprefixof(option))
    {
    case OPTIONS_PREFIX_SINGLE:
        nextpos = option + 1;
        break;
    case OPTIONS_PREFIX_DOUBLE:
        nextpos = option + 2;
        break;
    case OPTIONS_PREFIX_NONE:
        break;
    }

    NTASSERT(nextpos - option <= (int) strlen(option));

    return nextpos;     /** \return Referenz auf nachfolgende Position      */
}

/* Konvertierfunktionen *******************************************************/

/***************************************************************************//**
 Diese Funktion aktiviert einen 'Schalter' vom Typ int.
*******************************************************************************/
extern OPTIONS  options_set_int_flag (
    OPTIONS_VAR *variable,      /*!< [in,out] Variable zur Aufnahme des     */
                                /*!< [in,out] Wertes                        */
    void        *userdefined,   /*!< [in] benutzerdefiniert                 */
    char        **argv,         /*!< [in] Kommandozeile (ohne Option)       */
    int         *nprocessed)    /*!< [out] Anzahl verarbeiteter             */
                                /*!< [out] Argumente                        */
{
    int *flag;

    NOTUSED_PTR(argv);
    NOTUSED_PTR(userdefined);

    /* NULL-Referenzen werden erkannt.                                      */
    VALIDATE_PTR(variable, OPTIONS_ENULLPOINTER);
    VALIDATE_PTR(variable->address, OPTIONS_ENULLPOINTER);
                            /** \return OPTIONS_ENULLPOINTER                */

    /* Die Groesse der Zielvariablen wird verifiziert.                      */
    VALIDATE_SIZE(variable->size, sizeof(int), OPTIONS_EVARIABLE_SIZE);
                            /** \return OPTIONS_EVARIABLE_SIZE              */

    /* Der Wert der Variablen wird gesetzt, die Anzahl                      */
    /* verarbeiteter Argumente vermerkt.                                    */
    flag = (int *) variable->address;
    *flag = !0;
    *nprocessed = 0;

    return OPTIONS_OK;      /** \return OPTIONS_OK                          */
}

/***************************************************************************//**
 Diese Funktion speichert eine als Argument spezifizierte Zeichenkette
 im angegebenen Speicherplatz definierten Umfangs.
*******************************************************************************/
extern OPTIONS  options_store_string (
    OPTIONS_VAR *variable,  /*!< [in,out] Variable zur Aufnahme des         */
                            /*!< [in,out] Wertes                            */
    void        *userdefined, /*!< [in] benutzerdefiniert                   */
    char        **argv,     /*!< [in] Kommandozeile (ohne Option)           */
    int     *nprocessed)    /*!< [out] Anzahl verarbeiteter                 */
                            /*!< [out] Argumente                            */
{
    char    *destination;

    NOTUSED_PTR(userdefined);

    /* NULL-Referenzen werden erkannt.                                      */
    VALIDATE_PTR(variable, OPTIONS_ENULLPOINTER);
    VALIDATE_PTR(variable->address, OPTIONS_ENULLPOINTER);
    VALIDATE_PTR(argv, OPTIONS_ENULLPOINTER);
    VALIDATE_PTR(nprocessed, OPTIONS_ENULLPOINTER);
                            /** \return OPTIONS_ENULLPOINTER                */
    *nprocessed = 0;

    /* Das Vorhandensein eines Arguments wird geprueft.                     */
    if (argv[0] == NULL)
        return OPTIONS_EMISSING_ARGUMENT;
                            /** \return OPTIONS_EMISSING_ARGUMENT           */

    /* Die Groesse der Zielvariablen wird verifiziert.                      */
    if (variable->size <= strlen(argv[0]))
        return OPTIONS_EVARIABLE_SIZE;
                            /** \return OPTIONS_EVARIABLE_SIZE              */

    /* Der Wert der Variablen wird gesetzt, die Anzahl                      */
    /* verarbeiteter Argumente vermerkt.                                    */
    destination = (char *) variable->address;
    (void) STRNCPY(destination, argv[0], variable->size);
    *nprocessed = 1;

    return OPTIONS_OK;      /** \return OPTIONS_OK                          */
}

/***************************************************************************//**
 Diese Funktion speichert ein als Argument spezifizierter Integer-Wert
 im angegebenen Speicherplatz des Typs 'short int'.
*******************************************************************************/
extern OPTIONS      options_store_shortint (
    OPTIONS_VAR *variable,  /*!< [in,out] Variable zur Aufnahme des         */
                            /*!< [in,out] Wertes                            */
    void        *userdefined,   /*!< [in] benutzerdefiniert                 */
    char        **argv,     /*!< [in] Kommandozeile (ohne Option)           */
    int        *nprocessed) /*!< [out] Anzahl verarbeiteter                 */
                            /*!< [out] Argumente                            */
{
    OPTIONS     options;

    NOTUSED_PTR(userdefined);

    /* NULL-Referenzen werden erkannt.                                      */
    VALIDATE_PTR(variable, OPTIONS_ENULLPOINTER);
    VALIDATE_PTR(nprocessed, OPTIONS_ENULLPOINTER);
                            /** \return OPTIONS_ENULLPOINTER                */
    *nprocessed = 0;

    if ((options = mg_storenumeric(
        variable, sizeof(short int), FORMAT_SHORTINT, argv))
        != OPTIONS_OK)
        return options;     /** \return siehe Funktionstyp                  */

    *nprocessed = 1;

    return OPTIONS_OK;      /** \return OPTIONS_OK                          */
}

/***************************************************************************//**
 Diese Funktion speichert ein als Argument spezifizierter Integer-Wert
 im angegebenen Speicherplatz des Typs 'int'.
*******************************************************************************/
extern  OPTIONS     options_store_int (
    OPTIONS_VAR     *variable,      /*!< [in,out] Variable zur Aufnahme des */
                                    /*!< [in,out] Wertes                    */
    void            *userdefined,   /*!< [in] benutzerdefiniert             */
    char            **argv,         /*!< [in] Kommandozeile (ohne Option)   */
    int             *nprocessed)    /*!< [out] Anzahl verarbeiteter         */
                                    /*!< [out] Argumente                    */
{
    OPTIONS options;

    NOTUSED_PTR(userdefined);

    /* NULL-Referenzen werden erkannt.                                      */
    VALIDATE_PTR(variable, OPTIONS_ENULLPOINTER);
    VALIDATE_PTR(nprocessed, OPTIONS_ENULLPOINTER);
                            /** \return OPTIONS_ENULLPOINTER                */
    *nprocessed = 0;

    if ((options = mg_storenumeric(
        variable, sizeof(int), FORMAT_INT, argv)) != OPTIONS_OK)
        return options;     /** \return siehe Funktionstyp                  */

    *nprocessed = 1;

    return OPTIONS_OK;      /** \return OPTIONS_OK                          */
}

/***************************************************************************//**
 Diese Funktion speichert ein als Argument spezifizierter Integer-Wert
 im angegebenen Speicherplatz des Typs 'long int'.
*******************************************************************************/
extern OPTIONS      options_store_longint (
    OPTIONS_VAR *variable,      /*!< [in,out] Variable zur Aufnahme des     */
                                /*!< [in,out] Wertes                        */
    void        *userdefined,   /*!< [in] benutzerdefiniert                 */
    char        **argv,         /*!< [in] Kommandozeile (ohne Option)       */
    int         *nprocessed)    /*!< [out] Anzahl verarbeiteter             */
                                /*!< [out] Argumente                        */
{
    OPTIONS options;

    NOTUSED_PTR(userdefined);

    /* NULL-Referenzen werden erkannt.                                      */
    VALIDATE_PTR(variable, OPTIONS_ENULLPOINTER);
    VALIDATE_PTR(nprocessed, OPTIONS_ENULLPOINTER);
                            /** \return OPTIONS_ENULLPOINTER                */
    *nprocessed = 0;

    if ((options = mg_storenumeric(variable,
        sizeof(long int), FORMAT_LONGINT, argv)) != OPTIONS_OK)
        return options;     /** \return siehe Funktionstyp                  */

    *nprocessed = 1;

    return OPTIONS_OK;      /** \return OPTIONS_OK                          */
}

/***************************************************************************//**
 Diese Funktion speichert im angegebenen Speicherplatz den numerischen
 Wert des ersten Arguments der Argumentenliste.
 NULL-Referenzen und unzutreffende Groessenangaben werden erkannt.
*******************************************************************************/
static OPTIONS      mg_storenumeric (
    OPTIONS_VAR *variable,  /*!< [in,out] Variable zur Aufnahme des Wertes  */
    size_t      required,   /*!< [in] benoetigter Umfang der Variablen      */
    char const  *format,    /*!< [in] Formatzeichenkette                    */
    char        **argv)     /*!< [in] Argumentenliste                       */
{
    NTASSERT(required > 0 && (required % 2) == 0);

    /* NULL-Referenzen werden erkannt.                                      */
    VALIDATE_PTR(format, OPTIONS_ENULLPOINTER);
    VALIDATE_PTR(variable, OPTIONS_ENULLPOINTER);
    VALIDATE_PTR(variable->address, OPTIONS_ENULLPOINTER);
    VALIDATE_PTR(argv, OPTIONS_ENULLPOINTER);
                            /** \return OPTIONS_ENULLPOINTER                */

    /* Die Groesse der Zielvariablen wird verifiziert.                      */
    VALIDATE_SIZE(variable->size, required, OPTIONS_EVARIABLE_SIZE);
                            /** \return OPTIONS_EVARIABLE_SIZE              */

    /* Das Vorhandensein des Arguments wird geprueft.                       */
    if (argv[0] == NULL || *argv[0] == '\0')
        return OPTIONS_EMISSING_ARGUMENT;
                            /** \return OPTIONS_EMISSING_ARGUMENT           */

    /* Der Wert der Variablen wird gesetzt.                                 */
    return sscanf(argv[0], format, variable->address) != 1
        ? OPTIONS_EARGUMENT /** \return OPTIONS_EARGUMENT                   */
        : OPTIONS_OK;       /** \return OPTIONS_OK                          */
}

/***************************************************************************//**
 Diese Funktion aktiviert nichts.
*******************************************************************************/
extern  OPTIONS     options_store_none (
    OPTIONS_VAR *variable,      /*!< [in,out] Variable zur Aufnahme des     */
                                /*!< Wertes                                 */
    void        *userdefined,   /*!< [in] benutzerdefiniert                 */
    char        **argv,         /*!< [in] Kommandozeile (ohne Option)       */
    int         *nprocessed)    /*!< [out] Anzahl verarbeiteter Argumente   */
{
    VALIDATE_PTR(nprocessed, OPTIONS_ENULLPOINTER);
                                /** \return OPTIONS_ENULLPOINTER            */
    NOTUSED_PTR(variable);
    NOTUSED_PTR(userdefined);
    NOTUSED_PTR(argv);

    /* Die Anzahl verarbeiteter Argumente wird vermerkt.                    */
    *nprocessed = 0;

    return OPTIONS_OK;          /** \return OPTIONS_OK                      */
}

/***************************************************************************//**
 Diese Funktion fuehrt vor der Auswertung einer Kommandozeilenoption
 nichts aus.
*******************************************************************************/
extern OPTIONS  options_prior_none (
    void        *userdefined, /*!< [in] benutzerdefiniert                   */
    char        **argv,     /*!< [in] auf die Option folgende               */
                            /*!< [in] Kommandozeile (inkl. Option)          */
    int         is_split)   /*!< [in] Information: Option wurde             */
                            /*!< [in] aufgeteilt                            */
{
    NOTUSED_PTR(userdefined);
    NOTUSED_PTR(argv);
    NOTUSED_INT(is_split);

    return OPTIONS_OK;      /** \return OPTIONS_OK                          */
}

/***************************************************************************//**
 Diese Funktion fuehrt im Anschluss an eine erfolgreiche Auswertung
 einer Kommandozeilenoption nichts aus.
*******************************************************************************/
extern OPTIONS  options_post_none (
    OPTIONS_VAR *variable,      /*!< [in] Variable                          */
    void        *userdefined)   /*!< [in] benutzerdefiniert                 */
{
    NOTUSED_PTR(variable);
    NOTUSED_PTR(userdefined);

    return OPTIONS_OK;      /** \return OPTIONS_OK                          */
}

