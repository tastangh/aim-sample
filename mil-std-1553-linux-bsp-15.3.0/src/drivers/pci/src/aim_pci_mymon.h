/* SPDX-FileCopyrightText: 2016-2021 AIM GmbH <info@aim-online.com> */
/* SPDX-License-Identifier: GPL-2.0-or-later */

/*!aim_pci_mymon.h
 *
 *  This header file contains declarations used for
 *  driver specific MyMon handling
 *
 */

#ifndef AIM_PCI_MYMON_H_
#define AIM_PCI_MYMON_H_


#include "hw/AiMyMon.h"




/*! \struct TyMyMonUserData
 *
 * Driver specific MyMon data
 */
struct TyMyMonUserData
{
    struct aim_pci_device* device;  /*!< The device the data belongs to */
};


/*! \brief MyMon handler function for reading IO memory
 *
 * @param context MyMon context
 * @param byteOffset byte offset to read from
 * @param data data array to read into
 * @param num_lwords number of 32-bit words to read
 * @return AiTrue on success, AiFalse otherwise
 */
AiBoolean aim_pci_mymon_io_read( struct TyMyMonContext * context, AiUInt32 byteOffset, AiUInt32 data[], AiUInt32 num_lwords);


/*! \brief MyMon handler function for writing IO memory
 *
 * @param context MyMon context
 * @param byteOffset byte offset to write to
 * @param data data array to write from
 * @param num_lwords number of 32-bit words to write
 * @return AiTrue on success, AiFalse otherwise
 */
AiBoolean aim_pci_mymon_io_write( struct TyMyMonContext * context, AiUInt32 byteOffset, AiUInt32 data[], AiUInt32 num_lwords);


/*! \brief MyMon handler function for reading global extension memory
 *
 * @param context MyMon context
 * @param byteOffset byte offset to read from
 * @param data data array to read into
 * @param num_lwords number of 32-bit words to read
 * @return AiTrue on success, AiFalse otherwise
 */
AiBoolean aim_pci_mymon_glob_ext_read( struct TyMyMonContext * context, AiUInt32 byteOffset, AiUInt32 data[], AiUInt32 num_lwords );


/*! \brief MyMon handler function for writing global extension memory
 *
 * @param context MyMon context
 * @param byteOffset byte offset to write to
 * @param data data array to write from
 * @param num_lwords number of 32-bit words to write
 * @return AiTrue on success, AiFalse otherwise
 */
AiBoolean aim_pci_mymon_glob_ext_write( struct TyMyMonContext * context, AiUInt32 byteOffset, AiUInt32 data[], AiUInt32 num_lwords);


/*! \brief MyMon handler function for delaying execution thread
 *
 * @param context MyMon context
 * @param microseconds microseconds to delay execution
 * @return AiTrue on success, AiFalse on error
 */
AiBoolean aim_pci_mymon_wait( struct TyMyMonContext * context, AiUInt32 microseconds );


#endif /* AIM_PCI_MYMON_H_ */
