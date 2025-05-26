/* SPDX-FileCopyrightText: 2024-2024 AIM GmbH <info@aim-online.com> */
/* SPDX-License-Identifier: GPL-2.0-or-later */

/*! \file aim_artixus_device.h
 *
 *  This header file contains hardware related declarations
 *  specific for Artix UltraScale+ platform
 *
 */

#ifndef AIM_ARTIXUS_DEVICE_H_
#define AIM_ARTIXUS_DEVICE_H_


#ifdef AIM_ARTIXUS



#include "aim_pci_device.h"
#include "aim_rw_interface.h"
#include "aim_dma.h"




/*! \brief Initializes ArtixUS hardware of a device
 *
 * This function initializes the hardware specific parts of an Artix7 device. \n
 * It will map its bar registers and enable interrupts. \
 * After this function returned, the device must be fully operational and accessible.
 * @param aimDevice The device to initialize hardware of
 * @return returns 0 on success, a negative error code otherwise
 */
int aim_artixus_device_hw_init(struct aim_pci_device* aimDevice);


/*! \brief Shuts down ArtixUS hardware of a device
 *
 * This function will completely shut down hardware specific parts of an Artix7 device. \n
 * It will disable interrupts and un-map the device's bar registers.
 * After this function returned, the device hardware must no more being accessed.
 * @param aimDevice device to shut down hardware of
 */
void aim_artixus_device_hw_remove(struct aim_pci_device* aimDevice);


/*! \brief Enables interrupts for an ArtixUS device
 *
 * This function will set the device's top-half IRQ handler. \n
 * Now the driver is ready to handle interrupts for the device and will unmask the interrupts \n
 * on hardware.
 * @param aimDevice device to enable interrupts for
 * @return returns 0 on success, a negative error code otherwise
 */
int aim_artixus_enable_interrupts(struct aim_pci_device* aimDevice);


/*! \brief Disables interrupts of an ArtixUS device
 *
 * This function will mask the interrupts on hardware. \n
 * Then the IRQ handlers for the device will be freed.
 * @param aimDevice the device to disable the interrupts for.
 */
void aim_artixus_disable_interrupts(struct aim_pci_device* aimDevice);





#define AIMPCI_HW_ARTIXUS_DMA_CHANNELS 1
#define AIMPCI_HW_ARTIXUS_DMA_MAX_SGL_ELEMENTS 4096



typedef struct artixus_dma_sgl_entry {
    uint32_t device_address;
    uint64_t host_address;

    union {
        uint32_t all;
        struct {
            uint32_t reserved0   : 1;  /* Bit 0*/
            uint32_t irq         : 1;
            uint32_t last_entry  : 1;
            uint32_t reserved3   : 2;
            uint32_t direction   : 2;
            uint32_t reserved7   : 1;
            uint32_t size        : 24;
        } bit;
    } control;

    uint64_t next_sgl_address;

} __attribute__((__packed__)) ARTIXUS_DMA_SGL_ENTRY __aligned(4);



typedef struct aimpci_hw_artixus_dma_channel {

    int                     id;
    struct aim_pci_device*  aimDevice;
    
    void*      sgl_buffer;
    dma_addr_t sgl_buffer_address;
    size_t     sgl_buffer_size;
} AIMPCI_HW_ARTIXUS_DMA_CHANNEL;


int aim_artixus_start_dma_read_generic(struct aim_pci_device* device, struct aim_dma_buffer* buffer, enum aim_memory_type memType, loff_t offset);

int aim_artixus_dma_channel_allocate(struct aim_pci_device*, AIMPCI_HW_ARTIXUS_DMA_CHANNEL * channel);
int aim_artixus_dma_free(AIMPCI_HW_ARTIXUS_DMA_CHANNEL * channel);
int aim_artixus_dma_channel_feed(AIMPCI_HW_ARTIXUS_DMA_CHANNEL * channel, struct aim_pci_device* device, struct aim_dma_buffer* buffer, u32 ulTargetAddress, u32 ulSize );





#endif /* AIM_ARTIXUS */
#endif /* AIM_ARTIXUS_DEVICE_H_ */
