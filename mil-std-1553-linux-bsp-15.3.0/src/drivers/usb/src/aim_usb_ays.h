/* SPDX-FileCopyrightText: 2017-2024 AIM GmbH <info@aim-online.com> */
/* SPDX-License-Identifier: GPL-2.0-or-later */

/*! \file aim_usb_ays.h
 *
 */

#ifndef AIM_USB_AYS_H_
#define AIM_USB_AYS_H_


#include "aim_usb_interface.h"




/*! \brief AYS specific hardware initialization function
 *
 * This function will parse the interface endpoints
 * and establish basic communication channels
 * @param aim_intf the AYS based USB interface to initialize
 * @return 0 on success, errno code on failure
 */
extern int aim_usb_ays_hw_init(struct aim_usb_interface* aim_intf);


/*! \brief AYS specific memory read function
 *
 * @param aim_intf The USB interface to read from
 * @param buffer the buffer to read data to
 * @param len number of bytes to read
 * @param mem_type memory type to read from
 * @param offset offset in bytes relative to memory type to read from
 * @return 0 on success, errno code on failure
 */
extern int aim_usb_ays_hw_read(struct aim_usb_interface* aim_intf, void* buffer, size_t len, TY_E_MEM_TYPE mem_type,
                               loff_t offset);


/*! \brief AYS specific memory write function
 *
 * @param aim_intf The USB interface to write to
 * @param buffer the buffer to write data from
 * @param len number of bytes to write
 * @param mem_type memory type to write to
 * @param offset offset in bytes relative to memory type to write to
 * @return 0 on success, errno code on failure
 */
extern int aim_usb_ays_hw_write(struct aim_usb_interface* aim_intf, const void* buffer, size_t len, TY_E_MEM_TYPE mem_type,
                                loff_t offset);


/*! \brief AYS specific function for issuing target commands
 *
 * @param intf The USB interface to issue target command for
 * @param command This buffer holds the command data
 * @param command_len size of command in bytes
 * @param response the target response will be stored into this buffer
 * @param response_len size of response buffer in bytes. This defines the maximum response length
 * @return Number of valid bytes in response buffer on success. Negative error code on failure
 */
extern ssize_t aim_usb_ays_target_command(struct aim_usb_interface* intf, void* command, size_t command_len,
                                          void* response, size_t response_len);

/*! \brief AYS specific function for directly issuing a command size
 *
 * @param intf The USB interface to issue target command for
 * @param command_len size of command in bytes
 * @return Number of valid bytes in response buffer on success. Negative error code on failure
 */
extern ssize_t aim_usb_length_info_command(struct aim_usb_interface* intf, size_t command_len);

/*! \brief AYS specific function for a handshake between board and driver on what cmd size is possible
 * @param intf The USB interface to issue target command for
 * @param command_len size of new cmd buffers on the board
 * @return Number of valid bytes in response buffer on success. Negative error code on failure
 */
extern ssize_t aim_usb_length_handshake_command(struct aim_usb_interface* intf, size_t command_len);

/*! \brief Issues a generic USB command on AYS boards
 *
 * @param channel the USB communication channel to use
 * @param command the command to issue
 * @param cmd_len length of command in bytes
 * @param response the response will be stored in this buffer
 * @param resp_len Maximum response length in bytes. Must equal response buffer size
 * @return number of valid bytes in response buffer on success. Negative errno code on failure
 */
extern ssize_t aim_usb_ays_com_channel_cmd(struct aim_usb_com_channel* channel, const void* command, size_t cmd_len,
                                           void* response, size_t resp_len);

/*! \brief Start-up function for AYS boards
 *
 * @param intf the interface to start
 * @return 0 on success, errno code on failure
 */
extern int aim_usb_ays_hw_start(struct aim_usb_interface* intf);


/*! \brief Function for getting value of a NOVRAM setting on AYS devices
 *
 * @param intf the interface to get NOVRAM setting of
 * @param address address of the NOVRAM setting to get
 * @param value the NOVRAM setting's value will be stored here
 * @return 0 on success, a negative errno code on failure
 */
extern int aim_usb_ays_novram_get(struct aim_usb_interface* intf, AiUInt32 address, AiUInt32* value);




#endif /* AIM_USB_AYS_H_ */
