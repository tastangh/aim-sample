/* SPDX-FileCopyrightText: 2013-2021 AIM GmbH <info@aim-online.com> */
/* SPDX-License-Identifier: MIT  */

/*! \file Ai_Logindex.c

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

/* Includes *******************************************************************/

#include <stdlib.h>
#include <stdio.h>
#include <limits.h>

#include "Ai_Logindex.h"

/* Constants ******************************************************************/

#define LOGINDEX_FILENAME "logindex"    /*!< Name of the log-index file     */

#define LOGINDEX_LEN        100     /*!< Length of the log index string     */
#define INDEX_BASE          10      /*!< base of index value                */
#define READ_MODE           "r"     /*!< read file access mode              */
#define WRITE_MODE          "w"     /*!< write file access mode             */
#define FMT_LOGINDEX        "%d\n"  /*!< output format for logindex         */

#if defined(WIN32)
# pragma warning(disable:4996)
#endif

/* Macros *********************************************************************/

#ifndef NTASSERT
# define NTASSERT(arg)  /* empty */
#endif

/* Types **********************************************************************/

typedef char    STR_LOGINDEX[LOGINDEX_LEN + 1];

/* Variables ******************************************************************/

/* Prototypes *****************************************************************/

/* Functions ******************************************************************/

/**************************************************************************//**
This function reads the log file index from the log file index file.

If the log file index could not be determined, the invalid log file index
value is returned
*******************************************************************************/
extern  int logindex_read (void)
{
    FILE            *fp;
    STR_LOGINDEX    strindex;
    long int        thisindex;
    char            *end = strindex;

    /* Open the log file index file for reading.                            */
    if ((fp = fopen(LOGINDEX_FILENAME, READ_MODE)) == NULL)
        return LOGINDEX_FAIL;

    /* Read the last used log file index.                                   */
    if (fgets(strindex, LOGINDEX_LEN + 1, fp) <= 0)
    {
        (void) fclose(fp);
        return LOGINDEX_FAIL;
    }

    /* Close the log file index file                                        */
    (void) fclose(fp);

    /* Convert the log index value to an integer                            */
    if (strindex[0] == '\0')
        return LOGINDEX_FAIL;

    thisindex = strtol(strindex, &end, INDEX_BASE);
    if (end == strindex
    || LONG_MIN == thisindex
    || LONG_MAX == thisindex)
        return LOGINDEX_FAIL;   /** \return LOGINDEX_FAIL, if function fails */

    /* return the index of the last used log-file           */
    return (int) thisindex;     /** \return log index from log index file   */
}

/**************************************************************************//**
 This function writes the new log file index to the log file index file.
*******************************************************************************/
extern  int logindex_write (
    int index)      /*!< [in] New log file index                            */
{
    FILE    *fp;

    /* Check the parameters                                                 */
    if (index < 0)
        return !0;      /** \return !0 - invalid index                      */

    /* Open the log file index file for writing or create it, if it does    */
    /* not exist                                                            */
    if ((fp = fopen(LOGINDEX_FILENAME, WRITE_MODE)) == NULL)
        return !0;      /** \return !0 - file open error                    */

    /* Write the new log file index to the log file index file              */
    if (fprintf(fp, FMT_LOGINDEX, index) <= 0)
        return !0;      /** \return !0 - file write error                   */

    /* Close the log file index file                                        */
    if (fclose(fp) != 0)
        return !0;      /** \return !0 - file close error                   */

    return 0;           /** \return 0 - success                             */
}


