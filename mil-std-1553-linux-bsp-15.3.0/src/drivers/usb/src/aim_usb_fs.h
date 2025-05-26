/* SPDX-FileCopyrightText: 2017-2021 AIM GmbH <info@aim-online.com> */
/* SPDX-License-Identifier: GPL-2.0-or-later */

/*! \file aim_usb_fs.h
 *
 */

#ifndef AIM_USB_FS_H_
#define AIM_USB_FS_H_




/*! \brief Generic implementation of the open file operation
 *
 * This is the handler function for the open syscall on the /dev fs interface.
 * It is invoked on an 'open' call to a specific AIM USB device file
 * @param inode the inode the open call was issued on.
 * @param file the file pointer the operation is targeted for
 * @return 0 on success, errno on failure
 */
extern int aim_usb_interface_open(struct inode* inode, struct file* file);


/*! \brief Generic implementation of the release file operation
 *
 * This is the handler function for the close syscall on the /dev fs interface.
 * It is invoked on an 'close' call to a specific AIM USB device file
 * @param inode the inode the close call was issued on.
 * @param file the file pointer the operation is targeted for
 * @return 0 on success, errno on failure
 */
extern int aim_usb_interface_release(struct inode* inode, struct file* file);


/*! \brief Generic implementation of the read file operation
 *
 * This function handles the read syscall on the /dev fs interface
 * @param file the file pointer the operation is targeted for
 * @param buffer read parameters provided by user space
 * @param len length of user space parameters in buffer in bytes
 * @param offset not used
 * @return number of bytes read on success, negative errno code on failure
 */
extern ssize_t aim_usb_interface_read(struct file* file , char __user * buffer, size_t len, loff_t* offset);


/*! \brief Generic implementation of the write file operation
 *
 * This function handles the write syscall on the /dev fs interface
 * @param file the file pointer the operation is targeted for
 * @param buffer write parameters provided by user space
 * @param len length of user space parameters in buffer in bytes
 * @param offset not used
 * @return number of bytes written on success, negative errno code on failure
 */
extern ssize_t aim_usb_interface_write(struct file *, const char __user * buffer, size_t len, loff_t* offset);


/*! \brief Generic function for all IOCTLs the driver can handle
 *
 * @param file the file the ioctl is targeted on
 * @param command the IOCTL command code
 * @param params the IOCTL parameters
 * @return 0 on success, errno code on failure
 */
extern long aim_usb_interface_ioctl(struct file* file, unsigned int command, unsigned long params);




#endif /* AIM_USB_FS_H_ */
