/* SPDX-FileCopyrightText: 2017-2021 AIM GmbH <info@aim-online.com> */
/* SPDX-License-Identifier: GPL-2.0-or-later */

/*! \file aim_usb_hw.h
 *
 */

#ifndef AIM_USB_HW_H_
#define AIM_USB_HW_H_


#include "aim_usb_interface.h"




/*! \brief Architecture dependent hardware initialization
 *
 * This function is called during interface probing
 * and will initialize all architecture dependent properties
 * of an interface.
 * @param aim_intf the interface to initialize
 * @return 0 on success, errno code on failure
 */
extern int aim_usb_hw_init(struct aim_usb_interface* aim_intf);


/*! \brief Architecture dependent hardware start-up function
 *
 * This is a protocol independent function that will start-up an interface.
 * It is called once first user opens the device.
 * @param aim_intf the interface to start-up
 * @return 0 on success, errno code on failure
 */
extern int aim_usb_hw_start(struct aim_usb_interface* aim_intf);


/*! \brief Architecture dependent hardware stop function
 *
 * This is a protocol independent function that will stop an interface.
 * It is called once last user closed the device.
 * @param aim_intf the interface to stop
 * @return None
 */
extern void aim_usb_hw_stop(struct aim_usb_interface* aim_intf);


/*! \brief Architecture dependent read function
 *
 * This function will read data from a specific memory area on a device
 * @param aim_intf the USB interface to read from
 * @param buffer the buffer to read data to
 * @param len number of bytes to read
 * @param mem_type memory type to read from
 * @param offset the offset in bytes to read from
 * @return 0 on success, errno code on failure
 */
extern int aim_usb_hw_read(struct aim_usb_interface* aim_intf, void* buffer, size_t len, TY_E_MEM_TYPE mem_type,
                           loff_t offset);


/*! \brief Architecture dependent write function
 *
 * This function will write data to a specific memory area on a device
 * @param aim_intf the USB interface to write to
 * @param buffer the buffer to write data from
 * @param len number of bytes to write
 * @param mem_type memory type to write to
 * @param offset the offset in bytes to write to
 * @return 0 on success, errno code on failure
 */
extern int aim_usb_hw_write(struct aim_usb_interface* aim_intf, const void* buffer, size_t len, TY_E_MEM_TYPE mem_type,
                            loff_t offset);


/*! \brief Architecture dependent function for issuing generic USB commands
 *
 * @param channel the USB communication channel to issue command on
 * @param command the command
 * @param cmd_len length of command in bytes
 * @param response buffer where response will be stored to
 * @param resp_len maximum response length. Equals size of response_buffer in bytes
 * @return number of valid bytes in response buffer on success. Negative errno code on failure
 */
extern ssize_t aim_usb_hw_com_channel_cmd(struct aim_usb_com_channel* channel, const void* command, size_t cmd_len,
                                          void* response, size_t resp_len);


/*! \brief Architecture dependent function for getting a specific NOVRAM setting
 *
 * @param intf the interface to get NOVRAM setting of
 * @param address address of the NOVRAM setting to get
 * @param value the NOVRAM setting's value will be stored here
 * @return 0 on success, a negative errno code on failure
 */
extern int aim_usb_hw_novram_get(struct aim_usb_interface* intf, AiUInt32 address, AiUInt32* value);


/*! \brief Architecture dependent function for setting a specific NOVRAM setting
 *
 * @param intf the interface to set NOVRAM of
 * @param address address of the NOVRAM setting to write
 * @param value the NOVRAM value to set;
 * @return 0 on success, a negative errno code on failure
 */
extern int aim_usb_hw_novram_set(struct aim_usb_interface* intf, AiUInt32 address, AiUInt32 value);


/*! \brief Architecture dependent function for reading TCP registers
 *
 * @param intf the interface to read TCP register of
 * @param address the TCP register address to read
 * @param value the value read will be stored here
 * @return 0 on success, errno code on failure
 */
extern int aim_usb_hw_tcp_reg_read(struct aim_usb_interface* intf, AiUInt32 address, AiUInt32* value);


/*! \brief Architecture dependent function for writing TCP registers
 *
 * @param intf the interface to write TCP register of
 * @param address the TCP register address to write
 * @param value the value to write
 * @return 0 on success, errno code on failure
 */
extern int aim_usb_hw_tcp_reg_write(struct aim_usb_interface* intf, AiUInt32 address, AiUInt32 value);




#endif /* AIM_USB_HW_H_ */
