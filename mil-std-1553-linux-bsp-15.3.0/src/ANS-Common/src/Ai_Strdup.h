/* SPDX-FileCopyrightText: 2013-2021 AIM GmbH <info@aim-online.com> */
/* SPDX-License-Identifier: MIT  */

/*! \file Ai_Strdup.h


\brief Portable Ersatzfunktion fuer strdup()

 Dieses Modul stellt eine portable Ersatzfunktion fuer strdup() bereit.
*******************************************************************************/
#ifndef AI_STRDUP_H
#define AI_STRDUP_H

#ifdef __cplusplus
extern "C" {
#endif

/* Include-Dateien ************************************************************/

/* Konstanten *****************************************************************/

/* Makros *********************************************************************/

/* Typen **********************************************************************/

/* Prototypen *****************************************************************/

 extern	char	*str_dup (
    char const	*str);

 extern	char /*@observer@*/	*str_dup_getrcsid (void);

#ifdef __cplusplus
}
#endif


#endif
