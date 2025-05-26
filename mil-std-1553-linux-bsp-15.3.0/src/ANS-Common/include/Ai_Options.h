/* SPDX-FileCopyrightText: 2013-2021 AIM GmbH <info@aim-online.com> */
/* SPDX-License-Identifier: MIT  */

/*! \file Ai_Options.h


\brief Kommandozeilenoptionen

 Dieses Modul stellt universell verwendbare Funktionalitaeten zur
 Auswertung und Verarbeitung von Kommandozeilenoptionen bereit.

 Alle unter UNIX ueblichen Aufrufkonventionen werden vollstaendig
 unterstuetzt.

 Beispiel:

 Alle Optionen und Argumente einer Kommandozeile der Formen

\   test [-hH?v --help --verbose] [file(s)]
\   test [-hH?] [--varargs [argument(s)]] [--print] [file(s)]

 koennen durch eine entsprechende Definition der Optionstabelle
 durch einen einzigen Aufruf der Parse-Funktion vollstaendig abgearbeitet
 werden.

 Ausfuehrliche Optionen werden bei Bedarf komplettiert, falls mindestens
 drei Zeichen des Optionstextes vorliegen und eine unikate Komplettierung
 stattfinden kann.

 Regeln fuer Optionen der Kommandozeile
 ======================================
 Sind in der Optionstabelle die Optionen

    -a -b -c -d (kurze Optionen, ohne Argumentenverarbeitung)
    -o<arg>     (kurze Option, mit Argumentenverarbeitung)
    --help      (ausfuehrliche Option, ohne Argumentenverarbeitung)
    --verbose   (ausfuehrliche Option, ohne Argumentenverarbeitung)

 definiert, so gelten nachfolgende Regeln:

 1.     -abcd       == -a -bcd  == -a -b -cd    == -a -b -c -d

 2. -omain.o    == -o main.o

 3. -aomain.o   == -a -omain.o  == -a -o main.o

    Aber: -omain.oa == -o main.oa   != -omain.o -a

 4.     -a--help    == -a ---help   != -a --help

 5. --he        != --help

 6. --hel       == --help

 7. --ver       == --verb == --verbo == --verbos == --verbose
*******************************************************************************/
#ifndef AI_OPTIONS_H
#define AI_OPTIONS_H

/* Include-Dateien ************************************************************/

#include <stdlib.h>     /* fuer size_t  */

/* Konstanten *****************************************************************/

/**
 * Dieses Makro spezifiziert den letzten Eintrag einer Optionstabelle
 * (Kann als Vorlage fuer neue Eintraege verwendet werden)
 */
#define OPTIONS_TABLE_END                                       \
{                                                               \
    NULL,                       /* Optionsbezeichner    */      \
    OPTIONS_PREFIX_NONE,        /* Optionspraefix       */      \
    OPTIONS_TYPE_VERBOSE,       /* Optionstyp           */      \
    {                                                           \
        OPTIONS_NOT_REQUIRED,   /* zwingend erforderlich */     \
        OPTIONS_NO_ARGUMENTS    /* Argumente verarbeiten */     \
    },                                                          \
    options_store_none,     /* Funktion                 */      \
    options_prior_none,     /* erste auszufuehrende Funktion*/  \
    options_post_none,      /* nachfolgend auszufuehrende   */  \
                            /* Funktion                 */      \
    {                                                           \
        NULL,               /* Variable                 */      \
        0                   /* Umfang der Variable      */      \
    },                                                          \
    NULL,               /* benutzerdefiniertes Argument */      \
}

/* Konstanten fuer die Verwendung innerhalb der Tabellendefinition  **/

#define OPTIONS_REQUIRED        !0  /*!< Option ist zwingend erforderlich   */
#define OPTIONS_NOT_REQUIRED    0   /*!< Option ist wahlfrei                */

#define OPTIONS_HAS_ARGUMENTS   !0  /*!< Option hat Argumente               */
#define OPTIONS_NO_ARGUMENTS    0   /*!< Option hat keine Argumente         */

/* Makros *********************************************************************/

/* Typen **********************************************************************/

/**
 * Beschreibung der Rueckgabewerte des Moduls.
 */
typedef enum tagOptionsRet_t
{
    OPTIONS_OK,             /* Ausfuehrung in Ordnung                       */
    OPTIONS_EOL,            /* Ende der Kommandozeile erreicht              */
    OPTIONS_ARGUMENTS_FOLLOW,
                            /* nachfolgende Argumente sind separat          */
                            /* auszuwerten                                  */
    OPTIONS_BREAK,          /* Abbruch der Verarbeitung weiterer            */
                            /* Optionen durch eine                          */
                            /* benutzerdefinierte Funktion                  */
    OPTIONS_EABORT,         /* Eintritt einer benutzerdefinierten           */
                            /* Fehlerkondition bei Verarbeitung             */
                            /* einer Option                                 */
    OPTIONS_EPREFIX,        /* Unzulaessiges Praefix                        */
    OPTIONS_ETYPE,          /* Unzulaessiger Optionstyp                     */
    OPTIONS_EFUNCTION,      /* Unzulaessige Auswertungsfunktion             */
    OPTIONS_ENOT_INITIALIZED,
                            /* fehlende Initialisierung der                 */
                            /* Optionstabelle                               */
    OPTIONS_EUNKNOWN,       /* unbekannte Option                            */
    OPTIONS_EMISSING,       /* eine zwingend erforderliche Option           */
                            /* wurde nicht spezifiziert                     */
    OPTIONS_EMISSING_ARGUMENT,
                            /* ein erforderliches Argument wurde            */
                            /* nicht spezifiziert                           */
    OPTIONS_EARGUMENT,      /* unzulaessiges Argument                       */
    OPTIONS_EVARIABLE_SIZE, /* unzulaessiger Umfang einer                   */
                            /* angegebenen Zielvariablen                    */
    OPTIONS_EPROCESSING,    /* interner Fehler einer spezifizierten         */
                            /* argumentenverarbeitenden Funktion            */
    OPTIONS_EMEMORY,        /* zuwenig freier Speicher                      */
    OPTIONS_ENULLPOINTER,   /* unzulaessige NULL-Referenz                   */

    /* Intern verwendete Fehlerqualitaeten                                  */
    OPTIONS_SKIP,           /* Angefordertes Ueberspringen der              */
                            /* anstehenden Option durch eine vorab          */
                            /* auszufuehrende Auswertungsfunktion           */

    /* reservierte Werte    */
    OPTIONS_RESERVED
} OPTIONS;

/**
 * Beschreibung einer zu verwendenden Zielvariablen.
 */
typedef struct tagOptionsVar_t
{
    void    *address;   /* Adresse der Variable                             */
    size_t  size;       /* Gesamtumfang der Variable                        */

} OPTIONS_VAR;

/**
 * Beschreibung einer Funktion, welche als Reaktion auf eine
 * Kommandozeilenoption ausgefuehrt wird. Ist die Option als Option
 * mit Argumenten gekennzeichnet, so wird die Zahl der verarbeiteten Argumente
 * zurueckgeliefert.
 * Die Verarbeitung weiterer Optionen kann durch Rueckgabe einer
 * entsprechenden Fehlerqualitaet unterbunden werden.
 * Wird die Verarbeitung weiterer Optionen durch Anzeige einer Fehlerqualitaet
 * abgebrochen, so enthaelt der dem Aufrufer mitgeteilte Index innerhalb der
 * Kommandozeile die Referenz auf die bei Ausfuehrung dieser Funktion aktive
 * Option.
 */
typedef OPTIONS OPTIONS_FUNC (
    OPTIONS_VAR *variable,  /*!< Variable zur Aufnahme des Wertes           */
    void        *userdefined,
                            /*!< benutzerdefiniertes Argument oder NULL     */
    char        **argv,     /*!< auf die Option folgende Kommandozeile      */
                            /*!< (ohne Option)                              */
    int     *nprocessed);   /*!< Anzahl der durch die Funktion              */
                            /*!< verarbeiteter Argumente                    */

/**
 * Beschreibung einer Funktion, welche vor der Auswertung einer
 * Kommandozeilenoption aufgerufen wird. Dieser Funktion wird durch
 * die Rueckgabe einer entsprechenden Fehlerqualitaet die Gelegenheit
 * gegeben, die anstehende Bearbeitung vorzeitig abzubrechen (BREAK)
 * bzw. die Auswertung der anstehenden Option zu ueberspringen (SKIP).
 * Wird die Verarbeitung weiterer Optionen vorzeitig abgebrochen, so
 * enthaelt der dem Aufrufer mitgeteilte Index innerhalb der
 * Kommandozeile die Referenz auf die bei Ausfuehrung dieser Funktion
 * aktive Option.
 * Das in der Optionstabelle spezifizierte benutzerdefinierte
 * Argument wird der Funktion mitgeteilt.
 * Wurde die Aufteilung der aktuellen Option durchgefuehrt (z.B.
 * -rrev in -r rev), so wird dies mitgeteilt.
 */
typedef OPTIONS OPTIONS_PRIOR (
    void    *userdefined,   /*!< benutzerdefiniertes Argument               */
    char    **argv,         /*!< auf die Option folgende                    */
                            /*!< Kommandozeile (ohne Option)                */
    int is_splitted);       /*!< Information: Option wurde aufgeteilt       */

/**
 * Beschreibung einer Funktion, welche im Anschluss an die
 * Auswertung einer anstehenden Kommandozeilenoption ausgefuehrt wird.
 * Die durch die verarbeitende Funktion belegte Variable und das in
 * der Optionstabelle spezifizierte benutzerdefinierte Argument werden
 * dieser Funktion mitgeteilt.
 * Die Verarbeitung weiterer Optionen kann durch Rueckgabe einer
 * entsprechenden Fehlerqualitaet unterbunden werden.
 */
typedef OPTIONS     OPTIONS_POST (
    OPTIONS_VAR *variable,      /*!< Variable                               */
    void        *userdefined);  /*!< benutzerdefiniert                      */

/**
 * Beschreibung der unterstuetzten Optionspraefixe.
 */
typedef enum tagOptionsPrefix_t
{
    OPTIONS_PREFIX_NONE,    /* kein Praefix                                 */
    OPTIONS_PREFIX_SINGLE,  /* Praefix '-' bzw '/'                          */
    OPTIONS_PREFIX_DOUBLE,  /* Praefix '--'                                 */

} OPTIONS_PREFIX;

/**
 * Beschreibung der unterstuetzten Typen von Optionsbezeichnern
 */
typedef enum tagOptionsType_t
{
    OPTIONS_TYPE_SHORT,     /* kurze Option, z.B. v                         */
    OPTIONS_TYPE_VERBOSE,   /* lange Option, z.B. verbose                   */

} OPTIONS_TYPE;

/**
 * Beschreibung der zu einem Optionseintrag gehoerenden Schaltern.
 */
typedef struct tagOptionsFlags_t
{
    int is_required;    /* Option wird zwingend benoetigt                   */
    int has_arguments;  /* Argumente koennen verarbeitet werden             */

} OPTIONS_FLAGS;

/**
 * Beschreibung eines Eintrags der Tabelle von Optionen zur Steuerung
 * des Programmablaufs.
 */
typedef struct tagOptionsTable_t
{
    char const  *option;    /*!< Optionstext, NULL fuer die Kennzeichnung   */
                            /*!< des Endes der Tabelle, "" fuer die         */
                            /*!< Kennzeichnung des Beginns der              */
                            /*!< Argumentenliste.                           */
                            /*!< Bei 'kurzen' Bezeichnern sind mehrere      */
                            /*!< Eintraege in der Form "hH?" zulaessig.     */
                            /*!< Varianten von 'langen' Optionen koennen    */
                            /*!< durch Whitspaces separiert formuliert      */
                            /*!< werden (z.B. "abc xyz").                   */
    OPTIONS_PREFIX  prefix;     /*!< Typ des zugehoerigen Praefixes         */
    OPTIONS_TYPE    type;       /*!< Typ des Optionsbezeichners             */
    OPTIONS_FLAGS   flags;      /*!< Schalterstellungen                     */
    OPTIONS_FUNC    *function;  /*!< auszufuehrende Funktion                */
    OPTIONS_PRIOR   *priorfunction;
                                /*!< vor der Auswertung der  Option         */
                                /*!< auszufuehrende Funktion                */
    OPTIONS_POST    *postfunction;
                                /*!< im Anschluss an die erfolgreiche       */
                                /*!< Auswertung der Option auszufuehrende   */
                                /*!< Funktion                               */
    OPTIONS_VAR variable;       /*!< Variable zur Aufnahme des Wertes       */
    void        *userdefined;   /*!< benutzerdefiniertes Argument           */

    /* reservierte Eintraege    */
    struct
    {
        int was_found;      /*!< Eintrag wurde gefunden                     */
        int was_error;      /*!< Option ist fehlerhaft                      */
    } reserved;

} OPTIONS_TABLE;

/**
 * Beschreibung des Typs einer Optionstabellenbeschreibung
 */
typedef struct tagOptionsDesc_t
{
    OPTIONS_TABLE       *table;     /*!< Optionstabelle                     */

    /* reservierte Eintraege    */
    struct
    {
        int is_initialized; /*!< Beschreibung wurde initialisiert           */
/*@null@*/ char *unknown;   /*!< Kopie der unbekannten Option               */
/*@null@*/ char *error;     /*!< Kopie der fehlerhaften Option              */
    } reserved;

} OPTIONS_DESC;

/* Prototypen *****************************************************************/

 extern OPTIONS     options_init (
    OPTIONS_DESC    *desc);

 extern OPTIONS     options_deinit (
    OPTIONS_DESC    *desc);

 extern char const  *options_getunknown (/*@returned@*/
    OPTIONS_DESC    *desc);

 extern char const  *options_geterror (/*@returned@*/
    OPTIONS_DESC    *desc);

 extern OPTIONS     options_parse (
    char            **argv,
    int             *index,
    OPTIONS_DESC    *desc);

 extern OPTIONS     options_check4required (/*@out@*/
    int             *index,
    OPTIONS_DESC const *desc);

 extern int         options_iscritical (
    OPTIONS         error);

 extern char const /*@temp@*/ *options_getcurrent (void);

 extern OPTIONS     options_set_int_flag (
    OPTIONS_VAR     *variable,
    void            *userdefined,
    char            **argv,
    int             *nprocessed);

 extern OPTIONS     options_store_string (
    OPTIONS_VAR     *variable,
    void            *userdefined,
    char            **argv,
    int             *nprocessed);

 extern OPTIONS     options_store_shortint (
    OPTIONS_VAR     *variable,
    void            *userdefined,
    char            **argv,
    int             *nprocessed);

 extern OPTIONS     options_store_int (
    OPTIONS_VAR     *variable,
    void            *userdefined,
    char            **argv,
    int             *nprocessed);

 extern OPTIONS     options_store_longint (
    OPTIONS_VAR     *variable,
    void            *userdefined,
    char            **argv,
    int             *nprocessed);

 extern OPTIONS     options_store_none (
    OPTIONS_VAR     *variable,
    void            *userdefined,
    char            **argv,
    int             *nprocessed);

 extern OPTIONS     options_prior_none (
    void            *userdefined,
    char            **argv,
    int             is_split);

 extern OPTIONS     options_post_none (
    OPTIONS_VAR     *variable,
    void            *userdefined);
  

#endif /* OPTIONS_H */
