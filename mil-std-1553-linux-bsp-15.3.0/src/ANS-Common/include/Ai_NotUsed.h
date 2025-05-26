/* SPDX-FileCopyrightText: 2013-2021 AIM GmbH <info@aim-online.com> */
/* SPDX-License-Identifier: MIT  */

/*! \file Ai_NotUsed.h


\brief Vermeidung von Compilerwarnungen

Dieses Modul dient dazu, definierte aber nicht verwendete Variablen
mit einem Makro zu umgeben, um damit dementsprechende Compilerwarnungen
auszuschalten.

Die Makros bestehen in zweierlei Varianten: Varianten mit resultierendem
Aufruf einer Funktion und Varianten ohne den Overhead eines zusaetzlichen
Funktionsaufrufes. Die Konstante NOTUSED_USES_FUNC versucht
zur Compile-Zeit in Abhaengigkeit des erkannten Compilers die
geeignete Variante zu selektieren.

Das in den Makros verwendete Kommentar-Schluesselwort "-noeffect"
dient der entsprechenden Steuerung des MIT LCLint-Werkzeugs.
*******************************************************************************/
#ifndef AI_NOTUSED_H
#define AI_NOTUSED_H

#ifdef __cplusplus
extern "C" {
#endif

/* Include-Dateien ************************************************************/

/* Konstanten *****************************************************************/

#undef	NOTUSED_USES_FUNC
#if defined(__TURBOC__)	|| defined(_MSC_VER)
# define NOTUSED_USES_FUNC	0
#elif defined(__HIGHC__) || defined(__GNUC__)
# define NOTUSED_USES_FUNC	0
#else
# define NOTUSED_USES_FUNC	1
#endif

/* Makros *********************************************************************/

#if NOTUSED_USES_FUNC

/*!< Makro zur Ausschaltung von Compilerwarnungen bei nicht 		*/
/*!< verwendeten Integer-Variablen (Variante mit Funktionsaufruf)	*/
#define	NOTUSED_INT(val)						\
	{								\
		(void) notused((void const *) val);			\
	}

/*!< Makro zur Ausschaltung von Compilerwarnungen bei nicht 		*/
/*!< verwendeten Zeiger-Variablen (Variante mit Funktionsaufruf)	*/
#define	NOTUSED_PTR(val)						\
	{								\
		(void) notused((void const *) val);			\
	}

#else

/*!< Makro zur Ausschaltung von Compilerwarnungen bei nicht 		*/
/*!< verwendeten Integer-Variablen (Variante ohne Funktionsaufruf)	*/
#define	NOTUSED_INT(val)	/*@-noeffect@*/((void) (val))

/*!< Makro zur Ausschaltung von Compilerwarnungen bei nicht 		*/
/*!< verwendeten Zeiger-Variablen (Variante ohne Funktionsaufruf)	*/
#define	NOTUSED_PTR(val)	/*@-noeffect@*/((void) (val))

#endif

/* Typen **********************************************************************/

/* Prototypen *****************************************************************/

#if NOTUSED_USES_FUNC
extern	int		notused (/*@temp@*//*@null@*/
	void const	*ptr);	/*FPI dummy Variable			*/
#endif

extern	char /*@observer@*/ 	*notused_getrcsid (void);

#ifdef __cplusplus
}
#endif


#endif
