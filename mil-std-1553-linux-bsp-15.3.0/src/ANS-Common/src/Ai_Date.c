/* SPDX-FileCopyrightText: 2013-2023 AIM GmbH <info@aim-online.com> */
/* SPDX-License-Identifier: MIT  */

/*! \file Ai_Date.c

\brief Systemunabhaengige Verwaltung von Datumsangaben

*******************************************************************************/

/* Include-Dateien ************************************************************/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#ifdef TARGET_DOS
# include <dos.h>
#endif
#ifdef TARGET_OS2
# define INCL_DOSDATETIME
# include <os2.h>
#endif
#if defined(__unix) || defined(WIN32) || defined(_VXBUS)

# include <time.h>
#endif

#include "Ai_NotUsed.h"
#include "Ai_Validate.h"
#include "Ai_Strncpy.h"
#include "Ai_Date.h"

/* Verifikation des Zielsystems ***********************************************/

#undef  TARGET_DEFINED
#if defined(TARGET_DOS) || defined(TARGET_OS2) || defined(__unix) || defined(_VXBUS)
# define TARGET_DEFINED
#endif
#if defined(WIN32)
# pragma warning(disable:4996)
# define TARGET_DEFINED
#endif
#ifndef TARGET_DEFINED
# error "CDATE Not implemented for this OS or no TARGET defined!"
#endif
#undef  TARGET_DEFINED

/* Konstanten *****************************************************************/

/* Beschreibung der Parameterzahl beim Lesen                */
#define PARAM_DATE_D    1   /*!< nur Tag vorhanden                          */
#define PARAM_DATE_DM   2   /*!< nur Tag und Monat vorhanden                */
#define PARAM_DATE_DMY  3   /*!< Tag, Monat und Jahr vorhanden              */

/* Beschreibung der Formate beim Lesen                  */
#define FMT_COM     "%02d%02d%04d"      /*!< Kommunikationsformat           */
#define FMT_CMD     "%02d.%02d.%04d"    /*!< Kommandoformat                 */
#define FMT_OUT     "%02d.%02d.%04d"    /*!< Standard Ausgabeformat         */
#define FMT_OUT_ISO "%04d-%02d-%02d"    /*!< Ausgabeformat nach ISO8601     */
#define OFFSET_CENTURY  1900            /*!< Offset fuer des per time()     */
                                        /*!< ermittelte aktuelle Jahr       */

#define MAX_COLONS_IN_DATE_STR  2       /*!< maximale Anzahl Doppelpunkte   */
                                        /*!< im Datum                       */

/* Bereichsgrenzen fuer Tag, Monat und Jahr             */
#define DAY_MIN         1   /*!< erster Tag in jedem Monat                  */
#define MONTH_MIN       1   /*!< Januar                                     */
#define MONTH_MAX       12  /*!< Dezember                                   */

/* Beschreibung der Indize in der Monatstabelle             */
#define DIM_MONTHS      2   /*!< Dimensionen der Monatstabelle              */
#define INDEXDAYS       0   /*!< Index: Tage in Monatstabelle               */
#define INDEXLEAPDAYS   1   /*!< Index: Zusaetzliche Schalttage             */
                            /*!< in Monatstabelle                           */

/* Makros *********************************************************************/

#ifndef NTASSERT
# define NTASSERT(arg)  /* empty */
#endif

/* Typen **********************************************************************/

/* Variablen ******************************************************************/

/**
 * Monatstabelle mit ihrer maximalen Zahl Tagen und die relative Abweichung
 * der Tagesanzahl pro Monat bei einem Schaltjahr
 */
static  int mg_months[][DIM_MONTHS] =
{
    {   -1, -1  },
    {   31, 0   },
    {   28, 1   },
    {   31, 0   },
    {   30, 0   },
    {   31, 0   },
    {   30, 0   },
    {   31, 0   },
    {   31, 0   },
    {   30, 0   },
    {   31, 0   },
    {   30, 0   },
    {   31, 0   }
};

/**
 * Diese Variable beinhaltet den oberen Schwellenwert fuer die Expansion
 * zweistellig vorgegebener Jahreszahlen. Ein Wert von 0 bezeichnet einen
 * nicht konfigurierten Schwellenwert. Gueltige Schwellenwerte sind
 * vierstellige Jahreszahlen im Bereich des Intervalls
 * [CDATE_YEAR_MIN + 99 ... CDATE_YEAR_MAX].
 */
static  int mg_expansion_threshold = 0;

/* Simulation **/

/**
 * Diese Variable speichert die explizite Vorgabe des Systemdatums
 * fuer Test und Simulationszwecke.
 */
static  CDATE   mg_sim_date;

/**
 * Diese Variable speichert den Zustand des Simulationsmodus fuer die
 * explizite Vorgabe des Systemdatums fuer Test und Simulationszwecke.
 */
static  int mg_sim_flag = 0;

/* Prototypen *****************************************************************/

static  int     cdate_is (CDATE const   *cdate);

static  int     mg_is_leap_year (int        year);

static  int     colon2point (char       *str);

static  int     mg_expand_century (int      year);

/* Funktionen *****************************************************************/

/**************************************************************************//**
 Diese Funktion setzt Tag, Monat und Jahr eines Datums in der
 angegebenen Struktur.

 Falls die Jahreszahl ohne Jahrhundert-Angabe erfolgt, wird
 das aktuelle Jahrhundert gesetzt.
*******************************************************************************/
extern  CDATE_RET   cdate_setdate (/*@out@*/
    CDATE       *cdate, /*!< [out] Speicherplatz fuer Datum                 */
    int         day,    /*!< [in] Tag                                       */
    int         month,  /*!< [in] Monat                                     */
    int         year)   /*!< [in] Jahr                                      */
{
    CDATE       hdate;

    NTASSERT(cdate != NULL);
    VALIDATE_PTR(cdate, CDATE_ENULL);   /** \return CDATE_ENULL             */

    /* Bei Bedarf wird eine Jahrhundert-Expansion durchgefuehrt.            */
    hdate.day   = day;
    hdate.month = month;
    hdate.year  = mg_expand_century(year);

    /* ueberpruefe ob Datum korrekt ist                                     */
    if (!cdate_is(&hdate))
        return CDATE_EPARAM;    /** \return CDATE_EPARAM                    */

    *cdate = hdate;

    return CDATE_OK;            /** \return CDATE_OK                        */
}

/**************************************************************************//**
 Diese Funktion kopiert das Quell-Datum in das Ziel-Datum.
*******************************************************************************/
extern  void        cdate_cpy (/*@out@*/
    CDATE           *destdate,  /*!< [out] Ziel-Datum                       */
    CDATE const     *srcdate)   /*!< [in] Quell-Datum                       */
{
    VALIDATE_PTR_VOID(destdate);
    VALIDATE_PTR_VOID(srcdate);

    *destdate = *srcdate;
}

/**************************************************************************//**
 Diese Funktion ermittelt den Tag des Datums.
*******************************************************************************/
extern  int     cdate_getday (
    CDATE const *cdate)     /*!< [in] Datum                                 */
{
    VALIDATE_PTR(cdate, -1);    /** \return -1 bei NULL-Referenz            */

    return cdate->day;          /** \return Tageszahl                       */
}

/**************************************************************************//**
 Diese Funktion ermittlet den Monat des Datums.
*******************************************************************************/
extern  int     cdate_getmonth (
    CDATE const *cdate)     /*!< [in] Datum                                 */
{
    VALIDATE_PTR(cdate, -1);    /** \return -1 bei NULL-Referenz            */

    return cdate->month;        /** \return Monatszahl                      */
}

/**************************************************************************//**
 Diese Funktion ermittelt das Jahr des Datums.
*******************************************************************************/
extern  int     cdate_getyear (
    CDATE const *cdate)     /*!< [in] Datum                                 */
{
    VALIDATE_PTR(cdate, -1);    /** \return -1 bei NULL-Referenz            */

    return cdate->year;         /** \return Jahreszahl                      */
}

/**************************************************************************//**
 Diese Funktion liest das aktuelle Datum von der Systemuhr.
*******************************************************************************/
extern  void        cdate_get (/*@out@*/
    CDATE       *cdate)     /*!< [out] Speicherplatz fuer das Datum         */
{
#if defined(TARGET_DOS) && defined(__HIGHC__)
    struct  _dosdate_t  d;
#elif defined(TARGET_DOS) && defined(__TURBOC__)
    struct dosdate_t    d;
#elif defined(TARGET_OS2)
    DATETIME        d;
#elif defined(__unix) || defined(WIN32) || defined(_VXBUS)
    time_t          time_since_1970;
    struct tm       *d;
#endif
    VALIDATE_PTR_VOID(cdate);

    /* Falls eine Simulation konfiguriert wurde, dann wird das explizit     */
    /* vorgegebene Datum berichtet.                                         */
    if (mg_sim_flag)
    {
        *cdate = mg_sim_date;
        return;
    }

    /* Datum ermitteln und konvertieren                                     */
#if defined(TARGET_DOS)
    _dos_getdate(&d);
#elif defined(TARGET_OS2)
    DosGetDateTime(&d);
#elif defined(__unix) || defined(WIN32) || defined(_VXBUS)
    (void) time(&time_since_1970);
    d = localtime(&time_since_1970);
#endif

#if defined(TARGET_DOS) || defined(TARGET_OS2)
    cdate->day  = d.day;
    cdate->month    = d.month;
    cdate->year     = d.year;
#elif defined(__unix) || defined(WIN32) || defined(_VXBUS) 
    /* UNIX: Die Monatsangabe in der Datenstruktur d liegt zwischen 0 und   */
    /* 11                                                                   */
    VALIDATE_PTR_VOID(d);
    cdate->day  = d->tm_mday;
    cdate->month    = d->tm_mon + 1;
    cdate->year     = d->tm_year + OFFSET_CENTURY;
#endif
}

/**************************************************************************//**
 Diese Funktion setzt das Datum der Systemuhr.
 Unter Unix und Windows NT erfordert das Setzen des System-Datums
 erweiterte Benutzerprivilegien, weshalb hier eine solche
 Funktion nicht implementiert ist.
*******************************************************************************/
extern  CDATE_RET   cdate_set (
    CDATE const     *cdate)     /*!< [in] Datum                             */
{
#if defined(__unix) || defined(WIN32) || defined(_VXBUS)
    NOTUSED_PTR(cdate);

    return CDATE_ESET;      /** \return CDATE_ESET immer unter UNIX und     */
                            /** Win32                                       */
#else
# if defined(TARGET_DOS) && defined(__HIGHC__)
    struct  _dosdate_t  d;
# elif defined(TARGET_DOS) && defined(__TURBOC__)
    struct dosdate_t    d;
# elif defined(TARGET_OS2)
    DATETIME        d;
    DosGetDateTime(&d);
# endif
    NTASSERT(cdate != NULL);
    VALIDATE_PTR(cdate, CDATE_ENULL);   /** \return CDATE_ENULL             */

    /* Datum konvertieren und setzen                                        */
    d.day   = cdate->day;
    d.month = cdate->month;
    d.year  = cdate->year;

# if defined(TARGET_DOS)
    if (_dos_setdate(&d) != 0)
        return CDATE_ESET;          /** \return CDATE_ESET                  */
# elif defined(TARGET_OS2)
    if (DosSetDateTime(&d))
        return CDATE_ESET;
# endif
    return CDATE_OK;                /** \return CDATE_OK                    */
#endif
}

/**************************************************************************//**
 Diese Funktion konvertiert die Datumszeichenkette im Kommunikationsformat
 in das Datum.
*******************************************************************************/
extern  CDATE_RET   cdate_com2date (/*@out@*/
    CDATE           *cdate,     /*!< [out] Speicherplatz des Datums         */
    CDATE_COM const str)        /*!< [in] Datumszeichenkette                */
{
    int     i;
    CDATE       hdate;
    CDATE       sysdate;

    VALIDATE_PTR(cdate, CDATE_ENULL);   /** \return CDATE_ENULL             */
    VALIDATE_PTR(str, CDATE_ENULL);     /** \return CDATE_ENULL             */

    if ((i = sscanf(str, FMT_COM, &hdate.day, &hdate.month, &hdate.year))
        != PARAM_DATE_DMY)
    {
        cdate_get(&sysdate);
        switch (i)
        {
          case PARAM_DATE_D:
            hdate.month = sysdate.month;
            hdate.year = sysdate.year;
            break;
          case PARAM_DATE_DM:
            hdate.year = sysdate.year;
            break;
          default:
            return CDATE_EFRMT; /** \return CDATE_EFRMT                     */
        }
    }

    /* ueberpruefe ob Datum korrekt ist                                     */
    if (!cdate_is(&hdate))
        return CDATE_EPARAM;        /** \return CDATE_EPARAM                */

    *cdate = hdate;

    return CDATE_OK;                /** \return CDATE_OK                    */
}

/**************************************************************************//**
 Diese Funktion konvertiert das angegebene Datum in eine Zeichenkette
 im Kommunikationsformat.
*******************************************************************************/
extern  void        cdate_date2com (/*@out@*/
    CDATE_COM   str,        /*!< [out] Speicherplatz der Datumszeichenkette */
    CDATE const *cdate)     /*!< [in] Datum                                 */
{
    VALIDATE_PTR_VOID(str);
    VALIDATE_PTR_VOID(cdate);

    sprintf(str, FMT_COM, cdate->day, cdate->month, cdate->year);
    NTASSERT(strlen(str) <= CDATE_COM_LEN);
}

/**************************************************************************//**
 Diese Funktion konvertiert die angegebene Datumszeichenkette im Ausgabeformat
 in das Datum.
*******************************************************************************/
extern  CDATE_RET   cdate_out2date(/*@out@*/
        CDATE           *cdate, /*!< [out] Speicherplatz des Datums         */
        CDATE_OUT const str)    /*!< [in] Datumszeichenkette                */
{
    int         i;          /* Parameteranzahl                              */
    CDATE       hdate;      /* Puffer fuer pruefe Datum                     */
    CDATE       sysdate;    /* enthaelt Systemdatum                         */
    CDATE_OUT   cdate_out;

    VALIDATE_PTR(cdate, CDATE_ENULL);   /** \return CDATE_ENULL             */
    VALIDATE_PTR(str, CDATE_ENULL);     /** \return CDATE_ENULL             */

    /* Die Datumszeichenkette wird kopiert.                                 */
    (void) STRNCPY(cdate_out, str, sizeof(cdate_out));

    /* konvertiere falls notwenig US-Format in Europa-Format                */
    if (colon2point(cdate_out) > MAX_COLONS_IN_DATE_STR)
        return CDATE_EFRMT;

    if ((i = sscanf(cdate_out, FMT_OUT, &hdate.day, &hdate.month, &hdate.year))
        != PARAM_DATE_DMY)
    {
        cdate_get(&sysdate);

        switch (i)
        {
          case PARAM_DATE_D:
            hdate.month = sysdate.month;
            hdate.year = sysdate.year;
            break;
              case PARAM_DATE_DM:
            hdate.year = sysdate.year;
            break;

          default:
            return CDATE_EFRMT;  /** \return CDATE_EFRMT                    */
        }
    }

    /* Bei Bedarf wird eine Jahrhundert-Expansion durchgefuehrt.            */
    hdate.year = mg_expand_century(hdate.year);

    /* ueberpruefe ob Datum korrekt ist                                     */
    if (!cdate_is(&hdate))
        return CDATE_EPARAM;        /** \return CDATE_EPARAM                */

    *cdate = hdate;

    return CDATE_OK;            /** \return CDATE_OK                        */
}

/**************************************************************************//**
 Diese Funktion konvertiert die angegebene Datumszeichenkette im
 ISO 8601 Format in das Datum.
*******************************************************************************/
extern  CDATE_RET   cdate_out2date_iso(/*@out@*/
        CDATE           *cdate, /*!< [out] Speicherplatz des Datums         */
        CDATE_OUT const str)    /*!< [in] Datumszeichenkette                */
{
    int         i;          /* Parameteranzahl                              */
    CDATE       hdate;      /* Puffer fuer pruefe Datum                     */
    CDATE       sysdate;    /* enthaelt Systemdatum                         */
    CDATE_OUT   cdate_out;

    VALIDATE_PTR(cdate, CDATE_ENULL);   /** \return CDATE_ENULL             */
    VALIDATE_PTR(str, CDATE_ENULL);     /** \return CDATE_ENULL             */

    /* Die Datumszeichenkette wird kopiert.                                 */
    (void) STRNCPY(cdate_out, str, sizeof(cdate_out));

    if ((i = sscanf(cdate_out,
        FMT_OUT_ISO, &hdate.year, &hdate.month, &hdate.day))
        != PARAM_DATE_DMY)
    {
        cdate_get(&sysdate);

        switch (i)
        {
          case PARAM_DATE_D:
            hdate.month = sysdate.month;
            hdate.year = sysdate.year;
            break;
              case PARAM_DATE_DM:
            hdate.year = sysdate.year;
            break;
          default:
            return CDATE_EFRMT;  /** \return CDATE_EFRMT                    */
        }
    }

    /* Bei Bedarf wird eine Jahrhundert-Expansion durchgefuehrt.            */
    hdate.year = mg_expand_century(hdate.year);

    /* ueberpruefe ob Datum korrekt ist                                     */
    if (!cdate_is(&hdate))
        return CDATE_EPARAM;        /** \return CDATE_EPARAM                */

    *cdate = hdate;

    return CDATE_OK;            /** \return CDATE_OK                        */
}

/**************************************************************************//**
 Diese Funktion konvertiert das angegebene Datum in eine Zeichenkette im
 Ausgabeformat.
******************************************************************************/
extern  void        cdate_date2out (/*@out@                                 */
    CDATE_OUT       str,    /*!< [out] Speicherplatz der Datumszeichenkette */
    CDATE const     *cdate) /*!< [in] Datum                                 */
{
    VALIDATE_PTR_VOID(str);
    VALIDATE_PTR_VOID(cdate);

    sprintf(str, FMT_OUT, cdate->day, cdate->month, cdate->year);
    NTASSERT(strlen(str) <= CDATE_OUT_LEN);
}

/**************************************************************************//**
 Diese Funktion konvertiert das Datum in die Datumszeichenkette
 des ISO 8601 Ausgabeformates.
*******************************************************************************/
extern  void        cdate_date2out_iso (/*@out@*/
    CDATE_OUT   str,        /*!< [out] Speicherplatz der Datumszeichenkette */
    CDATE const *cdate)     /*!< [in] Datum                                 */
{
    VALIDATE_PTR_VOID(str);
    VALIDATE_PTR_VOID(cdate);

    sprintf(str, FMT_OUT_ISO, cdate->year, cdate->month, cdate->day);
    NTASSERT(strlen(str) <= CDATE_OUT_LEN);
}

/**************************************************************************//**
 Diese Funktion konvertiert die Datumszeichenkette im Kommandoformat in das
 Datum.
*******************************************************************************/
extern  CDATE_RET   cdate_cmd2date(/*@out@*/
    CDATE           *cdate,     /*!< [out] Speicherplatz des Datums         */
    CDATE_CMD const str)        /*!< [in] Datumszeichenkette                */
{
    int     ii;
    CDATE       hdate;
    CDATE       sysdate;
    CDATE_CMD   cdate_cmd;

    VALIDATE_PTR(cdate, CDATE_ENULL);   /** \return CDATE_ENULL             */
    VALIDATE_PTR(str, CDATE_ENULL);     /** \return CDATE_ENULL             */

    /* Die Datumszeichenkette wird kopiert.                                 */
    (void) STRNCPY(cdate_cmd, str, sizeof(cdate_cmd));

    /* convertiere falls notwenig US-Format -> Europa-Format                */
    if (colon2point(cdate_cmd) > MAX_COLONS_IN_DATE_STR)
        return CDATE_EFRMT;

    if ((ii = sscanf(cdate_cmd, FMT_CMD,
        &hdate.day, &hdate.month, &hdate.year)) != PARAM_DATE_DMY)
    {
        cdate_get(&sysdate);
        switch (ii)
        {
            case PARAM_DATE_D:
            hdate.month = sysdate.month;
            hdate.year = sysdate.year;
            break;
            case PARAM_DATE_DM:
            hdate.year = sysdate.year;
            break;
            default:
            return CDATE_EFRMT;     /** \return CDATE_EFRMT                 */
        }
    }

    /* ueberpruefe ob Datum korrekt ist                                     */
    if (!cdate_is(&hdate))
        return CDATE_EPARAM;        /** \return CDATE_EPARAM                */

    *cdate = hdate;

    return CDATE_OK;                /** \return CDATE_OK                    */
}

/**************************************************************************//**
 Diese Funktion konvertiert das angegebene Datum in die Datumszeichenkette
 im Kommandoformat.
*******************************************************************************/
extern  void        cdate_date2cmd (/*@out@*/
    CDATE_CMD   str,        /*!< [out] Datumszeichenkette                   */
    CDATE const *cdate)     /*!< [in] Datum                                 */
{
    VALIDATE_PTR_VOID(str);
    VALIDATE_PTR_VOID(cdate);

    sprintf(str, FMT_CMD, cdate->day, cdate->month, cdate->year);
    NTASSERT(strlen(str) <= CDATE_CMD_LEN);
}

/**************************************************************************//**
 Diese Funktion prueft, ob das angegebene Datum mit gueltigen Werten
 belegt ist.
*******************************************************************************/
static  int     cdate_is (
    CDATE const *cdate)     /*!< [in] Datum                                 */
{
    int     days;

    VALIDATE_PTR(cdate, 0);

    /* pruefe auf Unter- oder Ueberschreitung                               */
    if (cdate->day < DAY_MIN
    || cdate->year < CDATE_YEAR_MIN
    || cdate->year > CDATE_YEAR_MAX
    || cdate->month < MONTH_MIN
    || cdate->month > MONTH_MAX)
        return 0;

    /* ermittle Anzahl der Tage in Abhaengigkeit vom Schaltjahr             */
    days = mg_months[cdate->month][INDEXDAYS];
    if (mg_is_leap_year(cdate->year))
        days += mg_months[cdate->month][INDEXLEAPDAYS];

    return cdate->day > days
        ? 0         /** \return 0 - kein gueltiges Datum    */
        : !0;           /** \return !0 - gueltiges Datum    */
}

/**************************************************************************//**
 Diese Funktion vergleicht Datum eins mit Datum zwei.
*******************************************************************************/
extern  int     cdate_cmp (
    CDATE const *date1,     /*!< [in] Datum eins                            */
    CDATE const *date2)     /*!< [in] Datum zwei                            */
{
    CDATE_OUT   datebuf1;
    CDATE_OUT   datebuf2;

    VALIDATE_PTR(date1, -2);
    VALIDATE_PTR(date2, -2);

    /* erzeuge ISO 8601 Format von Datum eins und Datum zwei                */
    cdate_date2out_iso(datebuf1, date1);
    cdate_date2out_iso(datebuf2, date2);

    /* vergleiche Datum eins mit Datum 2                                    */
    return strcmp(datebuf1, datebuf2);
                /** \return   0 - Datum eins == Datum zwei,                 */
                /** \return < 0 - Datum eins < Datum zwei,                  */
                /** \return > 0 - Datum eins > Datum zwei                   */
}

/**************************************************************************//**
 Diese Funktion setzt fuer Test- und Simulationszwecke die Vorgabe
 des zu berichtenden aktuellen Systemdatums auf das Datum der
 angegebenen Zeichenkette im Ausgabeformat (dd.mm.yyy).

 Nachfolgende Aufrufe der Funktion cdate_get() werden bei aktivierter
 Simulation immer nur das konfigurierte Datum berichten.

 Die Simulation kann unter Spezifikation einer NULL-Referenz jederzeit
 wieder deaktiviert werden.
*******************************************************************************/
extern  int     cdate_simulation (
    CDATE_OUT const datestring) /*!< [in] Datumszeichenkette oder NULL      */
{
    mg_sim_flag = 0;

    /* Bei Bedarf wird die Simulation deaktiviert.                          */
    if (NULL == datestring)
        return 0;       /** \return 0 - okay                                */

    /* Sonst wird die Simulation aktiviert, falls die Zeichenkette ein      */
    /* gueltiges Datum enthaelt.                                            */
    if (cdate_out2date(&mg_sim_date, datestring) != CDATE_OK)
        return !0;      /** \return !0 - Fehlerhaftes Format                */

    mg_sim_flag = !0;

    return 0;
}

/**************************************************************************//**
 Diese Funktion setzt fuer Test- und Simulationszwecke die Vorgabe
 des zu berichtenden aktuellen Systemdatums auf das Datum der
 angegebenen Zeichenkette im ISO 8601 Format (YYYY-MM-DD).

 Nachfolgende Aufrufe der Funktion cdate_get() werden bei aktivierter
 Simulation immer nur das konfigurierte Datum berichten.

 Die Simulation kann unter Spezifikation einer NULL-Referenz jederzeit
 wieder deaktiviert werden.
*******************************************************************************/
extern  int     cdate_simulation_iso (
    CDATE_OUT const datestring) /*!< [in] Datumszeichenkette oder NULL      */
{
    mg_sim_flag = 0;

    /* Bei Bedarf wird die Simulation deaktiviert.                          */
    if (NULL == datestring)
        return 0;       /** \return 0 - okay                                */

    /* Sonst wird die Simulation aktiviert, falls die Zeichenkette ein      */
    /* gueltiges Datum enthaelt.                                            */
    if (cdate_out2date_iso(&mg_sim_date, datestring) != CDATE_OK)
        return !0;      /** \return !0 - Fehlerhaftes Format                */

    mg_sim_flag = !0;

    return 0;
}

/**************************************************************************//**
 Diese Funktion setzt den angegebenen oberen Schwellenwert fuer die Expansion
 zweistellig angegebener Jahreszahlen.

 Eine zweistellig angegebene Jahreszahl wird in einen Wert des
 Intervalls
    [threshold - 99 ... threshold]
 expandiert.

 Ein angegebener Schwellwert von 0 fuehrt zur Aktivierung der
 Voreinstellung bei der Expansion, d.h. mit das aktuelle Jahrhundert
 wird zugefuegt.
*******************************************************************************/
extern  int     cdate_set_threshold_value (
    int         threshold)  /*!< [in] Oberer Schwellenwert in Form einer    */
                            /*!< vierstelligen Jahreszahl                   */
{
    /* Ein Wert von 0 erzwingt das Setzen der Voreinstellung.               */
    if (0 == threshold)
    {
        mg_expansion_threshold = 0;
        return 0;           /** \return 0 - Ausfuehrung erfolgreich         */
    }

    /* Der Bereich der angegebenen Jahreszahl wird geprueft.                */
    if (threshold < CDATE_YEAR_MIN + 99
    ||  threshold > CDATE_YEAR_MAX)
        return !0;          /** \return !0 - ungueltiger Wert               */

    /* Ein gueltiger Wert wird in die statische Konfiguration uebernommen.  */
    mg_expansion_threshold = threshold;

    return 0;
}

/**************************************************************************//**
 Diese Funktion prueft, ob das angegebene Jahr ein Schaltjahr ist.
*******************************************************************************/
static  int     mg_is_leap_year (
        int     year)       /*!< [in] Jahreszahl                            */
{
    /* Ein Schaltjahr ist dann, wenn:                                       */
    /* a) year MOD 4 = 0 AND year MOD 100 <> 0, oder                        */
    /* b) year MOD 400 = 0                                                  */
    if ((year % 400) == 0)
        return !0;      /** \return !0 - Schaltjahr                         */

    return (year % 4) == 0 && (year % 100) != 0
        ? !0            /** \return !0 - Schaltjahr                         */
        : 0;            /** \return 0 - kein Schaltjahr                     */
}

/**************************************************************************//**
 Diese Funktion erstetzt alle Doppelpunkte einer Zeichenkette mit Punkten.
*******************************************************************************/
static  int     colon2point (
    char        *str)       /*!< [in,out] Zeichenkette                      */
{
    int     nr = 0;

    VALIDATE_PTR(str, 0);

    /* Doppelpunkt in Zeichenkette suchen                                   */
    while ((str = strchr(str, ':')) != NULL)
    {
        *str = '.';
        nr++;
    }
    return nr;          /** \return Anzahl ersetzter Doppelpunkte           */
}

/**************************************************************************//**
 Diese Funktion konvertiert bei Bedarf die angegebene Jahreszahl in
 ein 4-stelliges Format.

 Bei einer erforderlichen Expansion wird in der Voreinstellung
 mit dem aktuellen Jahrhundert expandiert.

 Bei einem konfigurierten oberen Schwellenwert via _set_threshold_value
 wird die Expansion in dem konfigurierten Intervall
\   [threshold - 99 ... threshold]
 durchgefuehrt.

 Eventuelle dreistellig angegebene Jahreszahlen werden nicht expandiert.
*******************************************************************************/
static  int     mg_expand_century (
    int         year)       /*!< [in] Jahreszahl                            */
{
    CDATE       thisdate;
    int     century;

    /* Bei einer ungueltigen Jahreszahl bzw. bereits 4-stellig              */
    /* vorliegenden Zahl wird keine Expansion durchgefuehrt.                */
    if (year < 0 ||  year > 99)
        return year;            /** \return angegebenes Jahr                */

    /* Bei Bedarf wird die Jahreszahl unter Verwendung des                  */
    /* konfigurierten Schwellenwertes expandiert.                           */
    if (mg_expansion_threshold > 0)
    {
        /* 28 bei 1930...2029 -> 2028                                       */
        /* 31 bei 1930...2029 -> 1931                                       */
        century = mg_expansion_threshold / 100;
        if ((century * 100) + year > mg_expansion_threshold)
            --century;

        return (century * 100) + year;  /** \return expandiertes Jahr       */
    }

    /* Das aktuelle Datum wird ermittelt.                                   */
    cdate_get(&thisdate);

    /* Das aktuelle Jahrhundert wird extrahiert.                            */
    century = thisdate.year / 100;

    /* Eine 2-stellige Jahreszahl wird expandiert.                          */
    return (century * 100) + year;      /** \return expandiertes Jahr       */
}


