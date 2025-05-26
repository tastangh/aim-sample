/* SPDX-FileCopyrightText: 2013-2021 AIM GmbH <info@aim-online.com> */
/* SPDX-License-Identifier: GPL-2.0-or-later */

/*! \file aim_rw.h
 *
 *  This file contains the protocol independent declarations
 *  for the read/write system call on AIM PCI drivers
 *
 */

#ifndef AIM_RW_H_
#define AIM_RW_H_


#include <linux/fs.h>





/*! \brief Handler for character device read function
 *
 *  This function is called when a user space application uses the read system call on a device file.\n
 *  It will copy the read parameters from user space and transfer the data from the requested \n
 *  memory type to the user application.
 *  \param file file pointer of the device the read call was issued on
 *  \param parameters pointer to read parameters in user space
 *  \param parameterSize size of read parameters in bytes
 *  \param offset not used
 *  \return returns number of bytes read on success, negative error code on failure
 */
ssize_t aim_read(struct file* file, char __user *parameters, size_t parameterSize, loff_t* offset);


/*! \brief Handler for character device write function
 *
 *  This function is called when a user space application uses the write system call on a device file.\n
 *  It will copy the write parameters from user space and transfer the data from the user application \n
 *  to the requested memory type.
 *  \param file file pointer of the device the write call was issued on
 *  \param parameters pointer to write parameters in user space
 *  \param parameterSize size of write parameters in bytes
 *  \param offset not used
 *  \return returns number of bytes written on success, negative error code on failure
 */
ssize_t aim_write(struct file* file, const char __user* parameters, size_t parameterSize, loff_t* offset);


/*! \brief Handler for character device mmap function
 *
 *  This function can be used to map the global,shared or IO RAM into the user space
 *  \param file file pointer of the device the write call was issued on
 *  \param vma virtual memory map
 *  \return returns 0 on success or negative error value
 */
int aim_mmap(struct file *file, struct vm_area_struct *vma );


#endif /* AIM_RW_H_ */
