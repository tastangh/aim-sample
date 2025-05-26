/* SPDX-FileCopyrightText: 2013-2021 AIM GmbH <info@aim-online.com> */
/* SPDX-License-Identifier: MIT  */

/*! \file Ai_Date.h

\brief Systemunabhaengige Verwaltung von Datumsangaben

Dieses Modul stellt Funktionalitaeten zur systemunabhaengigen
Verwaltung von Datumsangeben zur Verfuegung.

Die Funktionalitaeten umfassen:
o Ermittlung und Setzen des aktuellen Systemdatums
o Erzeugung von Zeichenketten aus Datumsangaben
o Vergleich von Datumsangaben
o konfigurierbare Expansion von zweistellig angegebenen Jahreszahlen
o Simulation eines konfigurierbaren Systemdatums

Schaltjahre werden nach den gueltigen Regeln korrekt verarbeitet.
Ungueltige Datumsangaben werden zuverlaessig erkannt und von der
Verarbeitung ausgeschlossen.

Die Funktionalitaeten dieses Moduls sind vollstaendig Jahr 2000-konform.

Unterstuetzte Betriebssysteme sind:

    System  Compiler-Define (-Dxxx)
    ------- -------------------------
    DOS TARGET_DOS
    OS/2    TARGET_OS2
    UNIX    TARGET_UNIX
    WIN16   TARGET_WIN16
    WIN32   TARGET_WIN32

Die Definition der Konstanten 'TARGET_XXX' selektiert zum Zeitpunkt
der Kompilation die entsprechende Systemunterstuetzung.

Ausgabeformat des Datums:
    Zeichenkette des Datums im Format dd.mm.yyyy.

Datum im ISO 8601 Format:
    Zeichenkette im Format YYYY-MM-DD.

Datum im Kommandoformat:
    Zeichenkette des Datums im Format ddmmyyyy.

Datum im Kommunikationsformat:
    Siehe Datum im Kommandoformat.
*******************************************************************************/
#ifndef AI_DATE_H
#define AI_DATE_H

#ifdef __cplusplus
extern "C" {
#endif

/* Include-Dateien ************************************************************/

/* Konstanten *****************************************************************/

#define CDATE_YEAR_MIN  1900    /*!< Erste zulaessige Jahreszahl            */
#define CDATE_YEAR_MAX  9999    /*!< Letzte zulaessige Jahreszahl           */

#define CDATE_COM_LEN   8       /*!< Laenge der Datums im                   */
                                /*!< Kommunikationsformat                   */
#define CDATE_OUT_LEN   10      /*!< Laenge der Datums im Ausgabeformat     */
#define CDATE_CMD_LEN   10      /*!< Laenge der Datums im Kommandoformat    */

/* Makros *********************************************************************/

/* Typen **********************************************************************/

/**
 * Beschreibung des Datums
 */
typedef struct tagCDate_t
{
    int day;        /* Tag                                                  */
    int month;      /* Monat                                                */
    int year;       /* Jahr                                                 */

} CDATE;

/**
 * Beschreibung der Rueckgabewerte dieses Moduls
 */
typedef enum tagCDateRet_t
{
    CDATE_OK,       /* korrekte Ausfuehrung                                 */
    CDATE_ENULL,    /* unzulaessige NULL-Referenz                           */
    CDATE_EFRMT,    /* Formatfehler                                         */
    CDATE_ESET,     /* Fehler beim Setzen                                   */
    CDATE_EPARAM    /* Aufrufparameter fehlerhaft                           */

} CDATE_RET;

/* Makros *********************************************************************/

/* Typen **********************************************************************/

/**
 * Typ der Datums im Kommunikationsformat
 */
typedef char    CDATE_COM[CDATE_COM_LEN + 1];

/**
 * Typ der Datums im Ausgabeformat (Standard + ISO 8601)
 */
typedef char    CDATE_OUT[CDATE_OUT_LEN + 1];

/**
 * Typ der Datums im Kommandoformat
 */
typedef char    CDATE_CMD[CDATE_CMD_LEN + 1];

/* Prototypen *****************************************************************/

extern CDATE_RET    cdate_setdate (/*@out@*/
    CDATE           *cdate,
    int             day,
    int             month,
    int             year);

extern  void        cdate_cpy (/*@out@*/
    CDATE       *destdate,
    CDATE const *srcdate);

extern  int     cdate_getday (
    CDATE const *cdate);

extern  int     cdate_getmonth (
    CDATE const *cdate);

extern  int     cdate_getyear (
    CDATE const *cdate);

extern  void    cdate_get (/*@out@*/
    CDATE       *cdate);

extern  CDATE_RET   cdate_set (CDATE const *cdate);

extern  CDATE_RET   cdate_com2date (/*@out@*/
    CDATE       *cdate,
    CDATE_COM const str);

extern  void        cdate_date2com (/*@out@*/
    CDATE_COM   str,
    CDATE const *cdate);

extern  CDATE_RET   cdate_out2date(/*@out@*/
        CDATE       *cdate,
    CDATE_OUT const str);

extern  CDATE_RET   cdate_out2date_iso(/*@out@*/
        CDATE       *cdate,
    CDATE_OUT const str);

extern  void        cdate_date2out (/*@out@*/
    CDATE_OUT   str,
    CDATE const *cdate);

extern  void        cdate_date2out_iso (/*@out@*/
    CDATE_OUT   str,
    CDATE const *cdate);

extern  CDATE_RET   cdate_cmd2date(/*@out@*/
    CDATE       *cdate,
    CDATE_CMD const str);

extern  void        cdate_date2cmd (/*@out@*/
    CDATE_CMD   str,
    CDATE const *cdate);

extern  int     cdate_cmp (
    CDATE const *date1,
    CDATE const *date2);

extern  int     cdate_simulation (CDATE_OUT const datestring);

extern  int     cdate_simulation_iso (CDATE_OUT const datestring);

extern  int     cdate_set_threshold_value (int threshold);

#ifdef __cplusplus
}
#endif


#endif
