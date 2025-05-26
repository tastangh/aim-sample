/* SPDX-FileCopyrightText: 2017-2021 AIM GmbH <info@aim-online.com> */
/* SPDX-License-Identifier: GPL-2.0-or-later */

/*! \file aim_usb_apu.h
 *
 */

#ifndef AIM_USB_APU_H_
#define AIM_USB_APU_H_


#include "aim_usb_interface.h"




/*! \brief Initializes an APU USB interface
 *
 * @param intf the generic USB interface
 * @return 0 on success, errno code on failure
 */
extern int aim_usb_apu_hw_init(struct aim_usb_interface* intf);


/*! \brief Starts an APU USB interface
 *
 * @param intf the APU interface to start
 * @return 0 on success, errno code on failure
 */
extern int aim_usb_apu_hw_start(struct aim_usb_interface* intf);


/*! \brief Stops an APU USB interface
 *
 * @param intf the APU interface to stop
 */
extern void aim_usb_apu_hw_stop(struct aim_usb_interface* intf);


/*! \brief APU specific device memory read function
 *
 * @param intf The APU interface to read from
 * @param buffer the buffer to read data into
 * @param len number of bytes to read
 * @param mem_type memory type to read from
 * @param offset offset in bytes to read from
 * @return 0 on success, errno code on failure
 */
extern int aim_usb_apu_hw_read(struct aim_usb_interface* intf, void* buffer, size_t len, TY_E_MEM_TYPE mem_type,
                               loff_t offset);


/*! \brief APU specific device memory write function
 *
 * @param intf The APU interface to write to
 * @param buffer the buffer to write data from
 * @param len number of bytes to write
 * @param mem_type memory type to write to
 * @param offset offset in bytes to write to
 * @return 0 on success, errno code on failure
 */
extern int aim_usb_apu_hw_write(struct aim_usb_interface* intf, const void* buffer, size_t len, TY_E_MEM_TYPE mem_type,
                                loff_t offset);


/*! \brief Function for getting value of a NOVRAM setting on APU devices
 *
 * @param intf the interface to get NOVRAM setting of
 * @param address address of the NOVRAM setting to get
 * @param value the NOVRAM setting's value will be stored here
 * @return 0 on success, a negative errno code on failure
 */
extern int aim_usb_apu_novram_get(struct aim_usb_interface* intf, AiUInt32 address, AiUInt32* value);


/*! \brief Function for setting of NOVRAM values on APU devices
 *
 * @param intf the interface to write NOVRAM of
 * @param address the NOVRAM address to write
 * @param value the value to write to NOVRAM;
 * @return 0 on success, a negative errno code on failure
 */
extern int aim_usb_apu_novram_set(struct aim_usb_interface* intf, AiUInt32 address, AiUInt32 value);


/*! \brief Function for reading TCP registers on APU devices
 *
 * @param intf the interface to read TCP register of
 * @param address the TCP register address to read
 * @param value the value read will be stored here
 * @return 0 on success, errno code on failure
 */
extern int aim_usb_apu_tcp_reg_read(struct aim_usb_interface* intf, AiUInt32 port, AiUInt32* value);


/*! \brief Function for writing TCP registers on APU devices
 *
 * @param intf the interface to write TCP register of
 * @param address the TCP register address to write
 * @param value the value to write
 * @return 0 on success, errno code on failure
 */
extern int aim_usb_apu_tcp_reg_write(struct aim_usb_interface* intf, AiUInt32 port, AiUInt32 value);


/*! \brief This function acknowledge all PCI interrupts on APU devices
 *
 * @param intf The APU interface to acknowledge interrupts for
 * @param event The received interrupt event.
 * @return 0 on success, errno code on failure
 */
extern int aim_usb_apu_pci_int_ack(struct aim_usb_interface* intf, AiUInt32 * event);






#endif /* AIM_USB_APU_H_ */
