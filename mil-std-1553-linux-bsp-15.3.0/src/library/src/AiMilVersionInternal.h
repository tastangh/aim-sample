/* SPDX-FileCopyrightText: 2015-2024 AIM GmbH <info@aim-online.com> */
/* SPDX-License-Identifier: MIT */

/*! \file AiMilVersionInternal.h
 *
 * This file defines the base version wich is used to produce the generated/AiMilVersion.h
 * 
 */


#ifndef _AI1553_VERSION_H__
#define _AI1553_VERSION_H__

/******************************************************************************
 *  Includes
******************************************************************************/

/******************************************************************************
 *  Defines
******************************************************************************/
/*! \def API_MIL_MAJOR_VERSION
    Major Version of API. Must be incremented when interface gets incompatible */
#define API_MIL_MAJOR_VERSION 24

/*! \def API_MIL_MINOR_VERSION
    Minor Version of API. Must be incremented when interface gets new functionality */
#define API_MIL_MINOR_VERSION 22

/*! \def API_MIL_PATCH_VERSION
    Patch Version. Must be incremented with bug fixes or cosmetic changes. \n
    Drops back to zero if either major or minor version is incremented. */
#define API_MIL_PATCH_VERSION 0

/*! \def API_MIL_EXTRA_VERSION
    Extra version that is appended to the version string */
#define API_MIL_EXTRA_VERSION 

#endif /* _AI1553_VERSION_H__ */

