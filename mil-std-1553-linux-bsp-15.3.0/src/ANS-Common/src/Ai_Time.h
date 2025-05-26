/* SPDX-FileCopyrightText: 2013-2021 AIM GmbH <info@aim-online.com> */
/* SPDX-License-Identifier: MIT  */

/*! \file Ai_Time.h

 \brief Systemunabhaengige Ermittlung und Konvertierung der Zeit des Systems

 Das Modul stellt die Funktionalitaet zur systemunabhaengigen Verwaltung
 der Systemzeit bereit. Durch setzten der Umgebungsvariable __CTIME ist es
 moeglich eine bestimmte Systemzeit fuer Testzwecke einzustellen.

 Unterstuetzte Betriebssysteme sind:

 DOS    - TARGET_DOS
 OS/2   - TARGET_OS2
 UNIX   - __unix
 WIN16  - TARGET_WIN16
 WIN32  - TARGET_WIN32

 Kommandoformat der Zeit: Zeichenkette der Zeit im Format hh:mm.

 Ausgabeformat der Zeit: Zeichenkette der Zeit im Format hh:mm:ss.
*******************************************************************************/
#ifndef AI_TIME_H
#define AI_TIME_H

#ifdef __cplusplus
extern "C" {
#endif

/* Include-Dateien ************************************************************/

/* Konstanten *****************************************************************/

#define CTIME_CMD_LEN   5   /*!< Laenge der Zeit im Kommandoformat          */
#define CTIME_OUT_LEN   8   /*!< Laenge der Zeit im Ausgabeformat           */

/* Makros *********************************************************************/

/* Typen **********************************************************************/

typedef struct tagCTime_t
{
    int hour;       /*!< Stunde                                             */
    int min;        /*!< Minute                                             */
    int sec;        /*!< Sekunde                                            */

} CTIME;

/**
 * Beschreibung der Rueckgabewerte
 */
typedef enum tagCTimeRet_t
{
    CTIME_OK,       /*!< korrekte Ausfuehrung                               */
    CTIME_ENULL,    /*!< unzulaessige NULL-Referenz                         */
    CTIME_EFRMT,    /*!< Formatfehler                                       */
    CTIME_ESET,     /*!< Fehler beim Setzen                                 */

} CTIME_RET;

/* Makros *********************************************************************/

/* Typen **********************************************************************/

/**
 * Typ der Zeit im Kommandoformat
 */
typedef char    CTIME_CMD[CTIME_CMD_LEN + 1];

/**
 * Typ der Zeit im Ausgabeformat
 */
typedef char    CTIME_OUT[CTIME_OUT_LEN + 1];

/* Prototypen *****************************************************************/

extern  void    ctime_setsec (CTIME *ctime, int sec);

extern  int     ctime_getsec (CTIME const   *ctime);

extern  void    ctime_setmin (CTIME *ctime, int min);

extern  int     ctime_getmin (CTIME const *ctime);

extern  void    ctime_sethour (CTIME *ctime, int hour);

extern  int     ctime_gethour (CTIME const  *ctime);

extern  void    ctime_get (/*@out@*/
    CTIME       *ctime);

extern  CTIME_RET   ctime_set (CTIME const  *ctime);

extern  CTIME_RET   ctime_cmd2time (/*@out@*/
    CTIME       *ctime,
    CTIME_CMD const str);

extern  void    ctime_time2cmd (/*@out@*/
    CTIME_CMD   str,
/*@observer@*/
    CTIME const *ctime);

extern  CTIME_RET   ctime_out2time (/*@out@*/
    CTIME       *ctime,
    CTIME_OUT const str);

extern  void        ctime_time2out (/*@out@*/
    CTIME_OUT   str,
/*@observer@*/
    CTIME const *ctime);

extern  void        ctime_init (/*@out@*/
    CTIME       *ctime);

#ifdef __cplusplus
}
#endif


#endif
