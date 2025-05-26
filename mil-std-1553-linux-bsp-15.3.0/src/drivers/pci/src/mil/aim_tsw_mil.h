/* SPDX-FileCopyrightText: 2016-2021 AIM GmbH <info@aim-online.com> */
/* SPDX-License-Identifier: GPL-2.0-or-later */

/*! \file aim_tsw_mil.h
 *
 *  This header file contains declarations for handling the 1553 target software
 */

#ifndef AIM_TSW_MIL_H_
#define AIM_TSW_MIL_H_

#include "aim_rw_interface.h"
#include "aim_pci_device_mil.h"




/*! \brief Initializes target software of device
 *
 * Initializes and starts up target software of devices that do not
 * have a ASP
 * @param milDevice device to initialize
 * @return 0 on success, errno code otherwise
 */
int aim_pci_mil_init_tsw(struct aim_pci_mil_device* milDevice);


/*! \brief Process target command on ASP of a device
 *
 * @param device device t process command on
 * @param cmdBuffer buffer that contains command
 * @param cmdSize number of bytes in command buffer
 * @param ack_buffer buffer that will receive acknowledge data
 * @param expectedAckSize size if acknowledge buffer in bytes
 * @return 0 on success, errno code otherwise
 */
int aim_exec_asp_command_mil(struct aim_pci_mil_device* device, void* cmdBuffer, size_t cmdSize, void* ack_buffer, size_t expectedAckSize);
int aim_exec_asp_command_mil_irq(struct aim_pci_mil_device* device, void* cmdBuffer, size_t cmdSize, void* ack_buffer, size_t expectedAckSize);



/*! \brief Retrieve device offset for ZynqMP DMA transfer
 *
 * @param device device t process command on
 * @param memType The memory type global or shared
 * @param offset The obtained device address is returned.
 * @return 0 on success, errno code otherwise
 */
int aim_tsw_mil_get_dma_memory_device_offset(struct aim_pci_device* aimDevice, enum aim_memory_type memType, u64 * offset);




#endif /* AIM_TSW_MIL_H_ */
