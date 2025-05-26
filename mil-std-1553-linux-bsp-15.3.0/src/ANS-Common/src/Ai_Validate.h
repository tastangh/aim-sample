/* SPDX-FileCopyrightText: 2013-2021 AIM GmbH <info@aim-online.com> */
/* SPDX-License-Identifier: MIT  */

/*! \file Ai_Validate.h

 \brief Ueberpruefung der Funktionsparameter bzw. Zeigern

  Dieses Modul ermoeglicht die Ueberpruefung von Funktionsparameter.
  Durch eine bedingte Kompilation kann in der Verhaltensweise zwischen
  zwei Modi gewechselt werden:

 o Entwicklungsmodus:
		In diesem Modus werden gepruefte Argumente unter Verwendung
		von NTASSERT zusaetzlich gesichert. Dieser Modus wird
		mit der gesetzten Konstanten DOASSERT zum Zeitpunkt der
		Uebersetzung aktiviert.

 o Betriebsmodus:
		In diesem Modus werden die betroffenen Funktionen wird mit
		einem spezifizierten Fehlerwert als Rueckgabewert abgebrochen.
******************************************************************************/
#ifndef AI_VALIDATE_H
#define AI_VALIDATE_H

/* Include-Dateien ************************************************************/

#include <stdlib.h>
#include "ANS_Log.h"

#ifndef NTASSERT_MSG
# define NTASSERT_MSG	"Validate failed"	/*!< Text der 		*/
						/*!< Fehlermeldung	*/
#endif

#ifdef DOASSERT
# include "ntassert.h"
#endif

/* Konstanten *****************************************************************/

#ifdef DOASSERT
/*!< Dieses Makro prueft eine gegebene Speicherreferenz gegen NULL.	*/
/*!< Ist die Speicherreferenz gleich NULL, so wird die aktuelle		*/
/*!< Funktion mit dem angegebenen Rueckgabewert verlassen.		*/
# define VALIDATE_PTR(ptr,ret)						\
			{						\
				if ((ptr) == NULL)			\
				{					\
					NTASSERT((ptr) != NULL);	\
					return (ret);			\
				}					\
			}
#else
# define VALIDATE_PTR(ptr,ret)                                          \
                {                                                       \
                    if ( NULL == (ptr) )                                \
                    {                                                   \
                        return (ret);                                   \
                    }                                                   \
                }
#endif

#ifdef DOASSERT
/*!< Dieses Makro prueft eine gegebene Speicherreferenz gegen NULL.	*/
/*!< Ist die Speicherreferenz gleich NULL, so wird die aktuelle		*/
/*!< Funktion verlassen.						*/
# define VALIDATE_PTR_VOID(ptr)						\
			{						\
				if ((ptr) == NULL)			\
				{					\
					NTASSERT((ptr) != NULL);	\
					return;				\
				}					\
			}
#else
# define VALIDATE_PTR_VOID(ptr)						\
			{						\
				if ((ptr) == NULL)			\
					return;				\
			}
#endif


#ifdef DOASSERT
/*!< Dieses Makro fuehrt die Pruefung eines Funktionarguments durch.	*/
/*!< Es kann zwischen einer Protokolierung in einer Datei und einem 	*/
/*!< Interaktiven Modus fuer die Entwicklung gewaehlt werden.		*/
# define VALIDATE(condition, ret)					\
			{						\
				if (!(condition))			\
				{					\
					NTASSERT(condition);		\
					return (ret);			\
				}					\
			}
#else
# define VALIDATE(condition, ret)					\
			{						\
				if (!(condition))			\
					return (ret);			\
			}
#endif


#ifdef DOASSERT
/*!< Dieses Makro fuehrt die Pruefung einer angegebenen Bedingung ohne	*/
/*!< Rueckgabequalitaet durch.						*/
# define VALIDATE_VOID(condition)					\
			{						\
				if (!(condition))			\
				{					\
					NTASSERT(condition);		\
					return;				\
				}					\
			}
#else
# define VALIDATE_VOID(condition)					\
			{						\
				if (!(condition))			\
					return;				\
			}
#endif


/* Makros *********************************************************************/

/* Typen **********************************************************************/

/* Prototypen *****************************************************************/


#endif /* VALIDATE_H */
