/* SPDX-FileCopyrightText: 2016-2022 AIM GmbH <info@aim-online.com> */
/* SPDX-License-Identifier: MIT OR GPL-2.0-or-later */

/*! \file mil_utils.h
 *
 * This header file contains declarations of utility functions
 *
 */

#ifndef MIL_UTILS_H_
#define MIL_UTILS_H_


#include "generated/mil_tsw_version.h"


/*! \define bin2bcd_2d
 *
 * converts integer value from binary representation to bcd with a maximum of 2 digits!!!.
 * Attention: value must not exceed 99, otherwise you will loose information
 */
#define bin2bcd_2d(x) \
     ((((x) / 10) << 4) + (x) % 10)

/*! \define bin2bcd_4d
 *
 * converts integer value from binary representation to bcd with a maximum of 4 digits!!!.
 * Attention: value must not exceed 9999, otherwise you will loose information
 */
#define bin2bcd_4d(x) \
    ((bin2bcd_2d((x) / 100) << 8) + bin2bcd_2d((x) % 100))

/*! \def MIL_TSW_LEGACY_VERSION
 *
 * Creates legacy, 2 Byte, BCD encoded target software version that contains \n
 * major and minor number
 */
#define MIL_TSW_LEGACY_VERSION \
        bin2bcd_2d(MIL_TSW_MAJOR_VERSION) << 8 | bin2bcd_2d(MIL_TSW_MINOR_VERSION);

/*! \def MIL_TSW_LEGACY_BUILD
 *
 * Creates legacy, 4 Byte, BCD encoded target software version build from patch \n
 */
#define MIL_TSW_LEGACY_BUILD \
        bin2bcd_4d(MIL_TSW_PATCH_VERSION);



#define GET_HS_STREAM( biu ) (((biu+1) > (short)p_api_dev->ul_NbOfStreams1) ? API39_HS_BIU_2 : API39_HS_BIU_1 )



#endif /* MIL_UTILS_H_ */
