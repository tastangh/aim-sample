/* SPDX-FileCopyrightText: 2013-2023 AIM GmbH <info@aim-online.com> */
/* SPDX-License-Identifier: MIT  */

/*! \file Ai_Time.c

\brief Systemunabhaengige Ermittlung und Konvertierung der Zeit des Systems

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

#include "Ai_Validate.h"
#include "Ai_NotUsed.h"
#include "Ai_Time.h"

/* Verifikation des Zielsystems ***********************************************/

#undef  TARGET_DEFINED
#if defined(TARGET_DOS) || defined(TARGET_OS2) || defined(__unix) || defined(_VXBUS)
# define    TARGET_DEFINED
#endif
#if defined(WIN32)
# define    TARGET_DEFINED
#endif
#ifndef TARGET_DEFINED
#   error   "Not implemented for this OS or no TARGET defined!"
#endif
#undef  TARGET_DEFINED

/* Konstanten *****************************************************************/

#define CMD_FRMT    "%02d:%02d"         /*!< Kommandoformat                 */
#define OUT_FRMT    "%02d:%02d:%02d"    /*!< Ausgabeformat                  */

#ifndef NTASSERT
# define NTASSERT(arg)  /* empty */
#endif

#if defined(WIN32)
# pragma warning(disable:4996)
#endif

/* Makros *********************************************************************/

/* Typen **********************************************************************/

/* Variablen ******************************************************************/

/* Prototypen *****************************************************************/

/* Funktionen *****************************************************************/

/***************************************************************************//**
 Diese Funktion setzt die Sekundenzahl der Zeit.
*******************************************************************************/
extern  void    ctime_setsec (
    CTIME       *ctime,     /*!< [in,out] Zeit                              */
    int         sec)        /*!< [in] Sekundenzahl                          */
{
    VALIDATE_PTR_VOID(ctime);

    ctime->sec = sec;
}

/***************************************************************************//**
 Diese Funktion ermittelt die Sekundenzahl der Zeit.
*******************************************************************************/
extern  int     ctime_getsec (
    CTIME const *ctime)     /*!< [in] Zeit                                  */
{
    VALIDATE_PTR(ctime, -1);    /** \return -1 bei NULL-Referenz            */

    return ctime->sec;          /** \return Anzahl Sekunden                 */
}

/***************************************************************************//**
 Diese Funktion setzt die Minutenzahl der Zeit.
*******************************************************************************/
extern  void    ctime_setmin (
    CTIME       *ctime, /*!< [in,out] Zeit                                  */
    int         min)    /*!< [in] Minutenzahl                               */
{
    VALIDATE_PTR_VOID(ctime);

    ctime->min = min;
}

/***************************************************************************//**
 Diese Funktion ermittelt die Minutenzahl der Zeit.
*******************************************************************************/
extern  int     ctime_getmin (
    CTIME const *ctime)     /*!< [in] Zeit                                  */
{
    VALIDATE_PTR(ctime, -1);    /** \return -1 bei NULL-Referenz            */

    return ctime->min;
}

/***************************************************************************//**
 Diese Funktion setzt die Stundenzahl der Zeit.
*******************************************************************************/
extern  void    ctime_sethour (
    CTIME       *ctime, /*!< [in,out] Zeit                                  */
    int         hour)   /*!< [in] Stundenzahl                               */
{
    VALIDATE_PTR_VOID(ctime);

    ctime->hour = hour;
}

/***************************************************************************//**
 Diese Funktion ermittelt die Stundenzahl der Zeit.
*******************************************************************************/
extern  int     ctime_gethour (
    CTIME const *ctime)     /*!< [in] Zeit                                  */
{
    VALIDATE_PTR(ctime, -1);    /** \return -1 bei NULL-Referenz            */

    return ctime->hour;         /** \return Stundenzahl                     */
}

/***************************************************************************//**
 Diese Funktion ermittelt die aktuelle Systemzeit.
*******************************************************************************/
extern  void    ctime_get (/*@out@*/
    CTIME       *ctime)     /*!< [out] Speicher fuer die aktuelle Zeit      */
{
#if defined(TARGET_DOS)
    struct dostime_t    t;
#elif defined(TARGET_OS2)
    DATETIME        t;
#elif defined(__unix) || defined(WIN32) || defined(_VXBUS)
    time_t          time_since_1970;
    struct tm       *t;
#endif
    /*FA Das Argument wird geprueft.                */
    VALIDATE_PTR_VOID(ctime);

    /*FA Zeit ermitteln und konvertieren                */
#if defined(TARGET_DOS)
    _dos_gettime(&t);
    ctime->hour     = t.hour;
    ctime->min  = t.minute;
    ctime->sec  = t.second;
#elif defined(TARGET_OS2)
    DosGetDateTime(&t);
    ctime->hour     = t.hours;
    ctime->min  = t.minutes;
    ctime->sec  = t.seconds;
#elif defined(__unix) || defined(WIN32) || defined(_VXBUS)
    (void) time(&time_since_1970);
    t = localtime(&time_since_1970);
    VALIDATE_PTR_VOID(t);
    ctime->hour     = t->tm_hour;
    ctime->min  = t->tm_min;
    ctime->sec  = t->tm_sec;
#endif
}

/***************************************************************************//**
 Diese Funktion setzt die Zeit der Systemuhr.
 Unter Unix ist das setzen des Systemzeit nur als ROOT moeglich. Deshalb
 ist diese Funktion fuer Unix nicht implementiert.
*******************************************************************************/
extern  CTIME_RET   ctime_set (
    CTIME const     *ctime)     /*!< [in] Zeit                              */
{
#if defined(TARGET_DOS)
    struct dostime_t    t;
#elif defined(TARGET_OS2)
    DATETIME        t;
    DosGetDateTime(&t);
#endif

    NOTUSED_PTR(ctime);
    NTASSERT(ctime != NULL);

    /*FA Zeit konvertieren und setzen   */
#if defined(TARGET_DOS)
    t.hour      = ctime->hour;
    t.minute    = ctime->min;
    t.second    = ctime->sec;
    if (_dos_settime(&t) != 0)
        return CTIME_ESET;  /** \return CTIME_ESET                          */

#elif defined(TARGET_OS2)
    t.hours     = ctime->hour;
    t.minutes   = ctime->min;
    t.seconds   = ctime->sec;
    if (DosSetDateTime(&t))
        return CTIME_ESET;
#endif
    return CTIME_OK;        /** \return CTIME_OK                            */
}

/***************************************************************************//**
 Diese Funktion konvertiert die Zeitzeichenkette im Kommandoformat in die
 Zeit.
*******************************************************************************/
extern  CTIME_RET   ctime_cmd2time (/*@out@*/
    CTIME           *ctime, /*!< [out] Speicherplatz der Zeit               */
    CTIME_CMD const str)    /*!< [in] Zeitzeichenkette                      */
{
    VALIDATE_PTR(ctime, CTIME_ENULL);
    VALIDATE_PTR(str, CTIME_ENULL); /** \return CTIME_ENULL                 */

    if (sscanf(str, CMD_FRMT, &ctime->hour, &ctime->min) != 2)
        return CTIME_EFRMT;         /** \return CTIME_EFRMT                 */

    return CTIME_OK;                /** \return CTIME_OK                    */
}

/***************************************************************************//**
 Diese Funktion konvertiert die Zeit in die Zeitzeichenkette
 im Kommandoformat.
*******************************************************************************/
extern  void        ctime_time2cmd (/*@out@*/
    CTIME_CMD       str,    /*!< [out] Speicherplatz der Zeitzeichenkette   */
/*@observer@*/
    CTIME const     *ctime) /*!< [in] Zeit                                  */
{
    VALIDATE_PTR_VOID(ctime);
    VALIDATE_PTR_VOID(str);

    sprintf(str, CMD_FRMT, ctime->hour, ctime->min);
    NTASSERT(strlen(str) <= CTIME_CMD_LEN);
}

/***************************************************************************//**
 Diese Funktion konvertiert die Zeitzeichenkette im Ausgabeformat in die
 Zeit.
*******************************************************************************/
extern  CTIME_RET   ctime_out2time (/*@out@*/
    CTIME           *ctime, /*!< [out] Speicherplatz der Zeit               */
    CTIME_OUT const str)    /*!< [in] Zeitzeichenkette                      */
{
    VALIDATE_PTR(ctime, CTIME_ENULL);
    VALIDATE_PTR(str, CTIME_ENULL); /** \return CTIME_ENULL                 */

    if (sscanf(str, OUT_FRMT, &ctime->hour, &ctime->min, &ctime->sec) != 3)
        return CTIME_EFRMT;     /** \return CTIME_EFRMT                     */

    return CTIME_OK;            /** \return CTIME_OK                        */
}

/***************************************************************************//**
 Diese Funktion konvertiert die Zeit in die Zeitzeichenkette (Ausgabeformat).
*******************************************************************************/
extern  void    ctime_time2out (/*@out@*/
    CTIME_OUT   str,    /*!< [out] Speicherplatz der Zeitzeichenkette       */
/*@observer@*/
    CTIME const *ctime) /*!< [in] Zeit                                      */
{
    VALIDATE_PTR_VOID(ctime);
    VALIDATE_PTR_VOID(str);

    sprintf(str, OUT_FRMT, ctime->hour, ctime->min, ctime->sec);
    NTASSERT(strlen(str) <= CTIME_OUT_LEN);
}

/***************************************************************************//**
 Diese Funktion initialisiert die Struktur zur Aufnahme der Zeit.
*******************************************************************************/
extern  void    ctime_init (/*@out@*/
    CTIME       *ctime)     /*!< [in,out] Zeitstruktur                      */
{
    VALIDATE_PTR_VOID(ctime);

    ctime->sec  = 0;
    ctime->min  = 0;
    ctime->hour = 0;
}


