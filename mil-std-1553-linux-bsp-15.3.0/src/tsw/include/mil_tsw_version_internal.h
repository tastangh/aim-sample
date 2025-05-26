/* SPDX-FileCopyrightText: 2016-2024 AIM GmbH <info@aim-online.com> */
/* SPDX-License-Identifier: MIT OR GPL-2.0-or-later */


/*! \file mil_tsw_version_internal.h
 *
 *  This header file contains the version information
 *  for the MIL target software component
 *
 */

#ifndef MIL_TSW_VERSION_INTERNAL_H_
#define MIL_TSW_VERSION_INTERNAL_H_


/*! \def MIL_TSW_MAJOR_VERSION
 *
 * Major version of target software. \n
 * Shall be increased when target software interface gets incompatible
 */
#define MIL_TSW_MAJOR_VERSION 22


/*! \def MIL_TSW_MINOR_VERSION
 *
 * Minor version of target software. \n
 * shall be increased when target software interface gets new functionality
 */
#define MIL_TSW_MINOR_VERSION 32


/*! \def MIL_TSW_PATCH_VERSION
 *
 * Patch version of target software. \n
 * Shall be increased on bug fixes or cosmetic changes. \n
 * Must drop back to zero, if major or minor number are increased.
 */
#define MIL_TSW_PATCH_VERSION 0


/*! \def MIL_TSW_EXTRA_VERSION
 *
 * Extra version string that will be appended to the version string
 */
#define MIL_TSW_EXTRA_VERSION


#if MIL_TSW_MAJOR_VERSION > 99

#error "Legacy representation of version does not allow more than 2 digits "

#endif


#if MIL_TSW_MINOR_VERSION > 99

#error "Legacy representation of version does not allow more than 2 digits "

#endif


#if MIL_TSW_PATCH_VERSION > 9999

#error "Legacy representation of version does not allow more than 4 digits "

#endif




#endif /* MIL_TSW_VERSION_INTERNAL_H_ */
