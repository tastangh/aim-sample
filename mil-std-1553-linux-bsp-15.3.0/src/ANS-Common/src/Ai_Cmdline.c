/* SPDX-FileCopyrightText: 2013-2021 AIM GmbH <info@aim-online.com> */
/* SPDX-License-Identifier: MIT  */

/*! \file Ai_Cmdline.c**************************************************************************//**

\brief Dynamische Erzeugung von Kommandozeilen

Dieses Modul stellt Funktionalitaeten zur dynamischen Erzeugung von
Kommandozeilen bereit.

*******************************************************************************/

/* Include-Dateien ************************************************************/

#include <stdlib.h>
#include <string.h>

#ifdef TSTHEAP
# include "tstheap.h"
#endif
#include "Ai_Validate.h"
#include "Ai_Strdup.h"
#include "Ai_Cmdline.h"

/* Konstanten *****************************************************************/

/* Makros *********************************************************************/

/**
 * Dieses Makro gibt einen dynamisch allokierten Speicherbereich frei
 * und initialisiert die zugehoerige Referenz mit dem Wert NULL.
 */
#define FREE(ptr)   {                           \
                        free(ptr);              \
                        (ptr) = NULL;           \
                    }

#ifndef NTASSERT
# define NTASSERT(op)   /* empty */
#endif

/* Typen **********************************************************************/

/* Prototypen *****************************************************************/

/* Variablen ******************************************************************/

/* Funktionen *****************************************************************/

/***************************************************************************//**
 Diese Funktion fuegt die angegebene Zeichenkette an die Kommandozeile
 an. Fuer die automatische Erzeugung der Kommandozeile beim ersten Aufruf
 muss die Kommandozeile mit 'NULL' und der Umfang der Liste mit dem Wert 0
 initialisiert sein.

 Eine 'leere' Kommandozeile kann durch Spezifikation von NULL als Zeichenkette
 bei der ersten Verwendung erzeugt werden.

 Bsp. fuer die Erzeugung der Kommandozeile:

    int newargc = 0;
    char    **newargv = NULL;

    if (cmdlist_append(&newargc, &newargv, argv[0]))
    {
        error("out of memory!");
        ...
    }

 Bem.:
 Da der Speicher fuer die Kommandozeile dynamisch allokiert wird,
 muss diese nach Verwendung wieder freigegeben werden.
 (siehe cmdline_free)
*******************************************************************************/
extern  int     cmdline_append (
/*@in@*/   int          *argc,      /*!< [in,out] Umfang der Liste          */
/*@in@*/   char         ***argv,    /*!< [in,out] Kommandozeile             */
/*@null@*/ char const   *str)       /*!< [in] Zeichenkette oder NULL        */
{
    char        **tmplist;

    /* NULL-Referenzen werden erkannt.                                      */
    VALIDATE_PTR(argv, !0);
    VALIDATE_PTR(argc, !0);     /** \return !0 - angegebene NULL-Referenz   */

    NTASSERT(*argc >= 0);

    /* Falls die Quell-Kommandozeile ohne Inhalt ist, dann wird             */
    /* diese nur kopiert, wenn die Ziel-Kommandozeile noch nicht            */
    /* existiert.                           */
    if (NULL == str
    && *argc > 0)
        return 0;

/*@-usereleased@*/
/*@-unqualifiedtrans@*/
    /* Die Zeichenkette wird angefuegt.                                     */
    if ((tmplist = (char **) realloc(*argv, (*argc + 1 + 1) * sizeof(char *)))
        == NULL)
        return !0;              /** \return !0 - zuwenig freier Speicher    */
/*@=unqualifiedtrans@*/
/*@=usereleased@*/

    /* Die erweiterte Dateiliste wird terminiert.                           */
    tmplist[*argc + 1] = NULL;

    /* Der Eintrag wird kopiert.                                            */
    tmplist[*argc] = (str == NULL) ? NULL : str_dup(str);

    /* Die Dateiliste wird aktualisiert.                                    */
    *argv = tmplist;
    ++*argc;

/*@-usereleased@*/
/*@-nullstate@*/
    return 0;                   /** \return 0 - Ausfuehrung in Ordnung      */
/*@=nullstate@*/
/*@=usereleased@*/
}

/***************************************************************************//**
 Diese Funktion dupliziert eine bestehende Kommandozeile mit mindestens
 einem enthaltenen Argument in eine neue Kommandozeile. Die ermittelte
 Anzahl Argumente der Kommandozeile wird dem Aufrufer mitgeteilt.

 Alle in der urspruenglichen Kommandozeile enthaltenen Argumente werden
 dupliziert.

 Die Spezifikation einer 'leeren' Kommandozeile ist zulaessig.

 Bem.: Da der Speicher fuer die Kommandozeile dynamisch allokiert wird,
 muss die Kommandozeile nach der Verwendung wieder freigegeben werden.
 (siehe cmdline_free)
*******************************************************************************/
extern  int     cmdline_duplicate (
/*@out@*/ int   *newargc,   /*!< [out] Anzahl ermittelter Argumente         */
/*@out@*/ char  ***newargv, /*!< [out] neue Kommandozeile                   */
/*@in@*/  char  **argv)     /*!< [in] bestehende Kommandozeile              */
{
    int     i;

/*@-mustdefine@*/
    /* NULL-Referenzen werden erkannt.                                      */
    VALIDATE_PTR(newargc, !0);
    *newargc = 0;
    VALIDATE_PTR(newargv, !0);
    *newargv = NULL;
    VALIDATE_PTR(argv, !0); /** \return !0 - angegebene NULL-Referenz       */
/*@=mustdefine@*/

/*@-nullstate@*/
    /* Ist die Kommandozeile ohne Inhalt, so wird diese dupliziert.         */
    if (NULL == argv[0])
        return cmdline_append(newargc, newargv, NULL);

    /* Sonst werden die einzelnen Elemente dupliziert.      */
    for (i = 0; argv[i] != NULL; i++)
    {
        if (cmdline_append(newargc, newargv, argv[i]) == 0)
            continue;

        /* Im Fehlerfall wird die bereits erzeugte                          */
        /* Kommandozeile wieder freigegeben.                                */
        cmdline_free(*newargv);
        *newargc = 0;
        *newargv = NULL;

        return !0;      /** \return !0 - zuwenig freier Speicher            */
    }

    return 0;           /** \return 0 - Ausfuehrung erfolgreich             */
/*@=nullstate@*/
}

/***************************************************************************//**
 Diese Funktion gibt die dynamisch generierte Kommandozeile nach der
 erfolgten Verwendung wieder frei. Eine bereits freigegebene Kommandozeile
 wird ignoriert.
*******************************************************************************/
extern void     cmdline_free (/*@only@*//*@null@*/
    char        **argv)     /*!< [in,out] Kommandozeile                     */
{
    int ii;

    /* NULL-Referenzen werden erkannt.                                      */
    if (NULL == argv)
        return;

/*@-unqualifiedtrans@*/
    /* Die Elemente der Kommandozeile werden freigegeben.                   */
    for (ii = 0; argv[ii] != NULL; ii++)
    {
        FREE(argv[ii]);
    }

    /* Die Kommandozeile wird freigegeben.                                  */
    FREE(argv);
/*@=unqualifiedtrans@*/
}

/***************************************************************************//**
 Diese Funktion entfernt das Element mit dem angegebenen Array-Index
 aus der Kommandozeile.
*******************************************************************************/
extern int      cmdline_free_entry (
    int         *argc,      /*!< [in,out] Anzahl Argumente                  */
    char        ***argv,    /*!< [in,out] Referenz auf Array                */
    int         index)      /*!< [in] Array-Index                           */
{
    int ii;

    VALIDATE_PTR(argc, !0);     /** \return !0 - ungueltiger Aufruf         */
    VALIDATE_PTR(argv, !0);
    VALIDATE(*argc >= 0, !0);
    VALIDATE(index >= 0, !0);

    if (index >= *argc)
        return !0;

    /* Das Element wird freigegeben.                                        */
/*@-unqualifiedtrans@*/
    FREE((*argv)[index]);
/*@=unqualifiedtrans@*/
    for (ii = index; ii < *argc; ii++)
    {
        (*argv)[ii] = (*argv)[ii + 1];
    }
    --*argc;

/*@-nullstate@*/
    return 0;
/*@=nullstate@*/
}

/***************************************************************************//**
 Diese Funktion fuegt an den Inhalt der Zielkommandozeile den Inhalt der
 Quellkommandozeile an. Der Inhalt wird elementweise _kopiert_.
 Die Quell-Kommandozeile wird _nicht freigegeben_.

 Die Spezifikation einer 'leeren' Quell-Kommandozeile ist zulaessig.

 Bem.: Da der Speicher fuer die Kommandozeile dynamisch allokiert wird,
 muss die Kommandozeile nach der Verwendung wieder freigegeben werden.
 (siehe cmdline_free)
*******************************************************************************/
extern int      cmdline_merge (
    int     *newargc,       /*!< [in,out] Anzahl ermittelter Argumente      */
    char    ***destination, /*!< [in,out] Ziel-Kommandozeile                */
    char    **source)       /*!< [in] Quell-Kommandozeile                   */
{
    /* NULL-Referenzen werden erkannt.                                      */
    VALIDATE_PTR(newargc, !0);
    VALIDATE_PTR(destination, !0);
    VALIDATE_PTR(source, !0);

    NTASSERT(*newargc >= 0);

    /* Ist die Kommandozeile ohne Inhalt, so wird diese dupliziert.         */
    /* Existiert die Ziel-Kommandozeile bereits, so wird nichts             */
    /* ausgefuehrt.                                                         */
    if (source[0] == NULL)
        return (*destination)[0] != NULL
            ? 0 : cmdline_append(newargc, destination, NULL);

    /* Die Inhalte der Quell-Kommandozeile werden kopiert.                  */
    while (*source != NULL)
    {
        if (cmdline_append(newargc, destination, *source))
            return !0;      /** \return !0 - zuwenig freier Speicher        */
        source++;
    }

    return 0;               /** \return 0 - Ausfuehrung erfolgreich         */
}

/***************************************************************************//**
 Diese Funktion ermittelt die Anzahl enthaltener Argumente in einer
 Kommandozeile.
*******************************************************************************/
extern  int     cmdline_getentries (
    char        **argv) /*!< [in] Kommandozeile         */
{
    int i;
    int ret = 0;

    VALIDATE_PTR(argv, 0);

    for (i = 0; argv[i] != NULL; i++)
        ++ret;

    return ret;         /** \return Anzahl Elemente     */
}

/***************************************************************************//**
 Diese Funktion ermittelt die effektive Laenge der angegebenen Kommandozeile
 in Zeichen.
*******************************************************************************/
extern size_t   cmdline_length (
    char        **argv)     /*!< [in] Kommandozeile                         */
{
    size_t  length;

    if (argv == NULL)
        return 0;

    /* Die Laenge der resultierenden, durch Leerzeichen getrennten          */
    /* Zeichenkette wird berechnet. Die Verwendung des sizeof-Operators in  */
    /* Verbindung mit der Zeichenkonstante (entspricht sizeof(int)) sorgt   */
    /* fuer eine additive Reserve.                                          */
    for (length = 0; *argv != NULL; argv++)
        length += strlen(*argv) + sizeof(' ');

    return length;          /** \return Laenge der Kommandozeile            */
}

#if 0
/* Aenderungsjournal ***********************************************************
 *
 * $Log: cmdline.c $
 * Revision 1.13  2002/05/19 12:01:10  duerr
 * Neue Schnittstellen _free_entry und _length implementiert.
 *
 * Revision 1.12  2002/04/05 06:36:19  duerr
 * Neue Funktion _free_entry implementiert.
 *
 * Revision 1.11  2002/03/25 09:36:22  duerr
 * Removed splint warnings
 *
 * Revision 1.10  1999/08/15 08:31:06  duerr
 * Used new module str_dup
 *
 * Revision 1.9  1999/07/28 11:23:12  duerr
 * Made code Lint-clean
 *
 * Revision 1.8  1999/02/25 09:29:44  duerr
 * Lint-Schluesselwoerter ueberarbeitet.
 *
 * Revision 1.7  1999/02/22 15:36:45  duerr
 * Lint-Schluesselwoerter ueberarbeitet
 *
 * Revision 1.6  1999/02/18 01:13:59  duerr
 * Code fuer den Einsatz fuer LCLint ueberarbeitet.
 *
 * Revision 1.5  1996/05/11 07:44:10  duerr
 * Funktion cmdline_free ignoriert nun vollstaendig bereits freigegebene
 * Kommandozeilen (vormals: Assert).
 *
 * Revision 1.4  1996/04/15 22:51:36  duerr
 * Expansion bestehender Kommandozeilen mit 'leeren' Kommandozeilen
 * wird nun nicht mehr durchgefuehrt.
 *
 * Revision 1.3  1996/04/15 20:30:19  duerr
 * Duplizierung 'leerer' Kommandozeilen (argv[0] == NULL) allgemein
 * zugelassen.
 *
 * Revision 1.2  1996/04/04 06:21:07  duerr
 * Neue Funktion cmdline_merge implementiert
 *
 * Revision 1.1  1996/03/29 08:32:53  duerr
 * Initial revision
 *
*******************************************************************************/
#endif
