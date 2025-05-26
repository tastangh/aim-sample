/* SPDX-FileCopyrightText: 2013-2021 AIM GmbH <info@aim-online.com> */
/* SPDX-License-Identifier: MIT  */

/*! \file Ai_Cmdline.h


\brief Dynamische Erzeugung von Kommandozeilen

Dieses Modul stellt Funktionalitaeten zur dynamischen Erzeugung von
Kommandozeilen bereit.
*******************************************************************************/
#ifndef AI_CMDLINE_H
#define AI_CMDLINE_H

#ifdef __cplusplus
extern "C" {
#endif

/* Include-Dateien ************************************************************/

/* Konstanten *****************************************************************/

/* Makros *********************************************************************/

/* Typen **********************************************************************/

/* Prototypen *****************************************************************/

extern  int     cmdline_append (
    /*@in@*/   int      *argc,
    /*@in@*/   char     ***argv,
    /*@null@*/ char const   *str);

extern  int     cmdline_duplicate (
/*@out@*/ int   *newargc,
/*@out@*/ char  ***newargv,
/*@in@*/  char  **argv);

 extern void    cmdline_free (/*@only@*//*@null@*/
    char        **argv);

 extern int     cmdline_free_entry (
    int         *argc,
    char        ***argv,
    int         index);

 extern int     cmdline_merge (
    int         *newargc,
    char        ***destination,
    char        **source);

 extern int     cmdline_getentries (
    char        **argv);

 extern size_t  cmdline_length (
    char        **argv);

#ifdef __cplusplus
}
#endif


#endif
