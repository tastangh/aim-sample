/* SPDX-FileCopyrightText: 2014-2021 AIM GmbH <info@aim-online.com> */
/* SPDX-License-Identifier: GPL-2.0-or-later */

/*! \file aim_ays_device.h
 *
 *  This header file contains hardware related declarations
 *  specific for AYS (Zynq) platform
 *
 */

#ifndef AIM_AYS_DEVICE_H_
#define AIM_AYS_DEVICE_H_


#include "aim_pci_device.h"




/*! \def AYS_SHARED_MEM_BAR
 *
 * Bar register ID of shared memory on AYS devices
 */
#define AYS_SHARED_MEM_BAR  0

/*! \def AYS_GLOBAL_MEM_BAR
 *
 * Bar register ID of shared memory on AYS devices
 */
#define AYS_GLOBAL_MEM_BAR  1


/*! \def AYS_IO_MEM_BAR
 * Bar register ID of IO memory on AYS devices
 */
#define AYS_IO_MEM_BAR      2


/*! \def AYS_COM_CHANNEL0_OFFSET
 *
 * Offset in bytes to start of generic PCI communication channel 0
 * in shared memory bar
 */
#define AYS_COM_CHANNEL0_OFFSET    0


/*! \def AYS_COM_CHANNEL0_SIZE
 *
 * Size of generic PCI communication channel 0 in bytes
 */
#define AYS_COM_CHANNEL0_SIZE      0x00010000   /* 64KB */


/*! \def AYS_TARGET_SIGNAL_REG_OFFSET
 *
 * Byte offset in IO memory bar to the register that is used
 * to signal events for the target device.
 */
#define AYS_TARGET_SIGNAL_REG_OFFSET    0x40



/*! \def AYS_HOST_IRQ_REG_OFFSET
 *
 * Byte offset in IO memory bar to the register area containing
 * interrupt event and mask and control registers
 */
#define AYS_HOST_IRQ_REG_OFFSET 0x30000


/*! \def AYS_AME_TARGET_SIGNAL_REG_OFFSET
 *
 * Byte offset in IO memory bar to the register that is used
 * to signal events for the target device.
 */
#define AYS_AME_TARGET_SIGNAL_REG_OFFSET    0x30040


/*! \def AYS_SHARED_MEMORY_SIZE
 *
 * Size of virtual shared memory in bytes
 */
#define AYS_SHARED_MEMORY_SIZE (16 << 20) /* 16MB */

/*! \brief Initializes devices based on AYS (Zynq) platform
 *
 * This function maps the bar registers of AYS devices
 * and enables interrupt availability.
 *
 * @param aimDevice device to initialize hardware of
 * @return returns 0 on success, a negative errno code otherwise
 */
int aim_ays_device_hw_init(struct aim_pci_device* aimDevice);


/*! \brief Removes an AYS (Zynq) platform device
 *
 * This function will release all resources that have been
 * allocated for this device.
 * E.g. PCI Bar Registers are unmapped and interrupts are disabled
 * @param aimDevice device to remove
 */
void aim_ays_device_hw_remove(struct aim_pci_device* aimDevice);


/*! \brief Enables interrupts for an AYS device
 *
 * This function will set the device's top-half IRQ handler. \n
 * Now the driver is ready to handle interrupts for the device and will unmask the interrupts \n
 * on hardware.
 * @param aimDevice device to enable interrupts for
 * @param offset The offset of the interrupt register area.
 * @return returns 0 on success, a negative error code otherwise
 */
int aim_ays_enable_interrupts(struct aim_pci_device* aimDevice, unsigned long offset);


/*! \brief Disables interrupts of an AYS device
 *
 * This function will mask the interrupts on hardware. \n
 * Then the IRQ handlers for the device will be freed.
 * @param aimDevice the device to disable the interrupts for.
 * @param offset The offset of the interrupt register area.
 */
void aim_ays_disable_interrupts(struct aim_pci_device* aimDevice, unsigned long offset);

int aim_gen_ays_novram_get(void* aimDevice, AiUInt32 address, AiUInt32 max_size,  AiUInt32 *pul_BytesRead, void* pv_Novram);
int aim_gen_ays_novram_set(void* Device, AiUInt32 address, AiUInt32 max_size,  AiUInt32 *pul_BytesWritten, void* pv_Novram);
int aim_gen_ays_novram_finalize(void* Device);
int aim_gen_ays_version_read(void* Device, TY_E_VERSION_ID eId, TY_VER_INFO * pxVersionValue);

#endif /* AIM_AYS_DEVICE_H_ */
