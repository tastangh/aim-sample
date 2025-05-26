/* SPDX-FileCopyrightText: 2013-2021 AIM GmbH <info@aim-online.com> */
/* SPDX-License-Identifier: MIT  */

/*! \file Ai_Logindex.h

 \brief Log index file management

 This module supplies functions for the management of the log index file.

 The following functions are supplied by this module:
	o Writing an new log-index to the log-index-file.
	o Reading the current log-index from the log-index-file.

 log-index:
	The log-index describes the currently used log file of the
	log-facility.

 log-index-file:
	The log-index-file containts the log-index.
*******************************************************************************/
#ifndef AI_LOGINDEX_H
#define AI_LOGINDEX_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes *******************************************************************/

/* Constants ******************************************************************/

#define	LOGINDEX_FAIL	-1	/*!< Invalid log index			*/

/* Macros *********************************************************************/

/* Types **********************************************************************/

/* Prototypes *****************************************************************/

extern	int	logindex_read (void);

extern	int	logindex_write (
	int	index);		/*FPI New log file index		*/


#ifdef __cplusplus
}
#endif

#endif  /* LOGINDEX_H */

