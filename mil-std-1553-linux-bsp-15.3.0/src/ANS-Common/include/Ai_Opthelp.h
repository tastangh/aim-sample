/* SPDX-FileCopyrightText: 2013-2021 AIM GmbH <info@aim-online.com> */
/* SPDX-License-Identifier: MIT  */

/*! \file Ai_Opthelp.h


\brief Hilfsmittel zur Definition von Optionstabellen

 Dieses Modul stellt Hilfsmittel zur Definition von Eintraegen in den
 Optionstabellen zur Verfuegung. Die Definitionen dieses Moduls dienen
 der Erleichterung der Anwendung von Optionstabellen und erleichtern die
 Wartbarkeit bei Anpassungen der Schnittstellen.
*******************************************************************************/
#ifndef AI_OPTHELP_H
#define AI_OPTHELP_H

#ifdef __cplusplus
extern "C" {
#endif

/* Include-Dateien ************************************************************/

/* Konstanten *****************************************************************/

/* Makros *********************************************************************/

/**
 * Dieses Makro definiert die angegebene Option als ausfuehrliche Option mit
 * doppelten Praefixen (Bsp.: "help" fuer --help).
 */
#define OPTHELP_DEFINE_DOUBLE_VERBOSE(text)             \
        text,                                           \
        OPTIONS_PREFIX_DOUBLE,                          \
        OPTIONS_TYPE_VERBOSE

/**
 * Dieses Makro definiert die angegebene Option als ausfuehrliche
 * Option mit einfachem Praefix (Bsp.: "dummy" fuer -dummy).
 */
#define OPTHELP_DEFINE_SINGLE_VERBOSE(text)             \
        text,                                           \
        OPTIONS_PREFIX_SINGLE,                          \
        OPTIONS_TYPE_VERBOSE

/**
 * Dieses Makro definiert die angegebene Option als kurze Option mit einfachem
 * Praefix (Bsp.: "h" fuer -h, auch: "hH?" fuer -h -H -?).
 */
#define OPTHELP_DEFINE_SINGLE_SHORT(text)               \
        text,                                           \
        OPTIONS_PREFIX_SINGLE,                          \
        OPTIONS_TYPE_SHORT

/**
 * Dieses Makro definiert keine Zielvariable.
 */
#define OPTHELP_DEFINE_VARIABLE_NONE()                  \
        {                                               \
            NULL,                                       \
            0                                           \
        }

/**
 * Dieses Makro definiert die angegebene Zielvariable eines beliebigen
 * Integer-Typs. Der Umfang wird implizit festgestellt.
 */
#define OPTHELP_DEFINE_VARIABLE_INT(var)                \
        {                                               \
            &(var),                                     \
            sizeof(var)                                 \
        }

/**
 * Dieses Makro definiert die angegebene Zielvariable eines beliebigen
 * Struktur-Typs. Der Umfang wird implizit festgestellt.
 */
#define OPTHELP_DEFINE_VARIABLE_STRUCT  OPTHELP_DEFINE_VARIABLE_INT

/**
 * Dieses Makro definiert die angegebene Referenz auf eine
 * Zielvariable bestimmten Umfangs.
 */
#define OPTHELP_DEFINE_VARIABLE(var,size)               \
        {                                               \
            (var),                                      \
            (size)                                      \
        }

/**
 * Dieses Makro definiert das benutzerdefiniertes Argument.
 */
#define OPTHELP_DEFINE_USERDEFINED(var)     (var)

/**
 * Dieses Makro definiert kein benutzerdefiniertes Argument.
 */
#define OPTHELP_DEFINE_USERDEFINED_NONE()   NULL

/**
 * Dieses Makro definiert eine optionsverarbeitende Funktion.
 */
#define OPTHELP_DEFINE_FUNC(func)       func

/**
 * Dieses Makro definiert keine optionsverarbeitende Funktion.
 */
#define OPTHELP_DEFINE_FUNC_NONE()      options_store_none

/**
 * Dieses Makro definiert eine vorweg auszufuehrende Funktion.
 */
#define OPTHELP_DEFINE_PRIOR(func)      func

/**
 * Dieses Makro definiert keine vorweg auszufuehrende Funktion.
 */
#define OPTHELP_DEFINE_PRIOR_NONE()     options_prior_none

/**
 * Dieses Makro definiert eine nachtraeglich auszufuehrende Funktion.
 */
#define OPTHELP_DEFINE_POST(func)       func

/**
 * Dieses Makro definiert keine nachtraeglich auszufuehrende Funktion.
 */
#define OPTHELP_DEFINE_POST_NONE()      options_post_none

/* Typen **********************************************************************/

/* Prototypen *****************************************************************/

#ifdef __cplusplus
}
#endif

#endif
