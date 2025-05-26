/* SPDX-FileCopyrightText: 2017-2021 AIM GmbH <info@aim-online.com> */
/* SPDX-License-Identifier: GPL-2.0-or-later */

/*! \file aim_zynqmp_device.h
 *
 *  This header file contains hardware related declarations
 *  specific for AYXX platform
 *
 */

#ifndef AIM_ZYNQMP_DEVICE_H_
#define AIM_ZYNQMP_DEVICE_H_

#include <linux/spinlock.h>

#include "aim_pci_device.h"
#include "aim_rw_interface.h"
#include "aim_dma.h"
#include "hw/AiHwZynqMP.h"

/*! \def ZYNQMP_GLOBAL_SHARED_MEM_BAR
 * ID of shared memory bar on AYX devices
 */
#define ZYNQMP_GLOBAL_SHARED_MEM_BAR 2
#define ZYNQMP_GLOBAL_MEM_SIZE       0x1000000LL
#define ZYNQMP_HOSTIO_MEM_SIZE       0x0100000LL
#define ZYNQMP_SHARED_MEM_SIZE       0x0F00000LL


/*! \def ZYNQMP_IO_MEM_BAR
 * ID of IO memory bar on AYX devices
 */
#define ZYNQMP_IO_MEM_BAR 4

/*! \def ZYNQMP_DMA_MEM_BAR
 * ID of DMA memory bar on AYX devices
 */
#define ZYNQMP_DMA_MEM_BAR 0

/*! \brief Initializes AYXX hardware of a device
 *
 * This function initializes the hardware specific parts of an APXX device. \n
 * It will map its bar registers, enable interrupts and DMA availability. \
 * After this function returned, the device must be fully operational and accessible.
 * @param aimDevice The device to initialize hardware of
 * @return returns 0 on success, a negative error code otherwise
 */
int aim_zynqmp_device_hw_init(struct aim_pci_device* aimDevice);


/*! \brief Shuts down AYXX hardware of a device
 *
 * This function will completely shut down hardware specific parts of an AYXX device. \n
 * It will disable interrupts and un-map the device's bar registers.
 * After this function returned, the device hardware must no more being accessed.
 * @param aimDevice device to shut down hardware of
 */
void aim_zynqmp_device_hw_remove(struct aim_pci_device* aimDevice);


/*! \brief Enables interrupts for an AYXX device
 *
 * This function will set the device's top-half IRQ handler. \n
 * Now the driver is ready to handle interrupts for the device and will unmask the interrupts \n
 * on hardware.
 * @param aimDevice device to enable interrupts for
 * @return returns 0 on success, a negative error code otherwise
 */
int aim_zynqmp_enable_interrupts(struct aim_pci_device* aimDevice);


/*! \brief Disables interrupts of an AYX device
 *
 * This function will mask the interrupts on hardware. \n
 * Then the IRQ handlers for the device will be freed.
 * @param aimDevice the device to disable the interrupts for.
 */
void aim_zynqmp_disable_interrupts(struct aim_pci_device* aimDevice);


/*! \brief Write interrupt event to device loglist and create interrupt to the device
 *
 * @param aimDevice the device to disable the interrupts for.
 * @param event The interrupt event to write to the loglist.
 */
int aim_zynqmp_create_interrupt_on_device(struct aim_pci_device* device, enum ai_hw_zynqmp_hostio_event event );



#define AIMPCI_HW_ZYNQMP_DMA_CHANNELS 1
#define AIMPCI_HW_ZYNQMP_DMA_MAX_SGL_ELEMENTS 512*4
#define AIMPCI_HW_ZYNQMP_DMA_SGL_ALIGNMENT 64

#define AIMPCI_HW_ZYNQMP_DMA_LOCATION_PCI 0
#define AIMPCI_HW_ZYNQMP_DMA_LOCATION_AXI 1

#define AIMPCI_HW_ZYNQMP_DMA_AXI_ATTRIBUTE       0x3
#define AIMPCI_HW_ZYNQMP_DMA_PCIE_ATTRIBUTE      0x2


#define AIMPCI_HW_ZYNQMP_DMA_CHANNEL0_OFFSET     0x0
#define AIMPCI_HW_ZYNQMP_DMA_CHANNEL1_OFFSET     0x80
#define AIMPCI_HW_ZYNQMP_DMA_CHANNEL2_OFFSET     0x100
#define AIMPCI_HW_ZYNQMP_DMA_CHANNEL3_OFFSET     0x180


/*
 * DMA channel registers
 */
typedef struct zynqmp_dma_registers {
    union _SRC_Q_LOW {
        uint32_t UINT;
        struct {
            uint32_t QUEUE_LOCATION :1;         // Bit 0
            uint32_t QUEUE_ENABLE :1;           // Bit 1
            uint32_t READ_ATTR :4;              // Bit 2 - 5
            uint32_t START_ADDR_LOW :26;        // Bit 6-31
        } BIT;
    } SRC_Q_LOW;                     //0x00
    uint32_t SRC_Q_HI;               //0x04
    uint32_t SRC_Q_SIZE;             //0x08
    uint32_t SRC_Q_LIMIT;            //0x0C
    union _DST_Q_LOW {
        uint32_t UINT;
        struct {
            uint32_t QUEUE_LOCATION :1;         // Bit 0
            uint32_t QUEUE_ENABLE :1;           // Bit 1
            uint32_t READ_ATTR :4;              // Bit 2 - 5
            uint32_t START_ADDR_LOW :26;        // Bit 6-31
        } BIT;
    } DST_Q_LOW;                       //0x10
    uint32_t DST_Q_HI;                 //0x14
    uint32_t DST_Q_SIZE;               //0x18
    uint32_t DST_Q_LIMIT;              //0x1c
    union _STAS_Q_LOW {
        uint32_t UINT;
        struct {
            uint32_t QUEUE_LOCATION :1;         // Bit 0
            uint32_t QUEUE_ENABLE :1;           // Bit 1
            uint32_t READ_ATTR :4;              // Bit 2 - 5
            uint32_t START_ADDR_LOW :26;        // Bit 6-31
        } BIT;
    } STAS_Q_LOW;                       //0x20
    uint32_t STAS_Q_HI;                 //0x24
    uint32_t STAS_Q_SIZE;               //0x28
    uint32_t STAS_Q_LIMIT;              //0x2C
    union _STAD_Q_LOW {
        uint32_t UINT;
        struct {
            uint32_t QUEUE_LOCATION :1;         // Bit 0
            uint32_t QUEUE_ENABLE :1;           // Bit 1
            uint32_t READ_ATTR :4;              // Bit 2 - 5
            uint32_t START_ADDR_LOW :26;        // Bit 6 - 31
        } BIT;
    } STAD_Q_LOW;                      //0x30
    uint32_t STAD_Q_HI;                //0x34
    uint32_t STAD_Q_SIZE;              //0x38
    uint32_t STAD_Q_LIMIT;             //0x3C
    uint32_t SRC_Q_NEXT;               //0x40
    uint32_t DST_Q_NEXT;               //0x44
    uint32_t STAS_Q_NEXT;              //0x48
    uint32_t STAD_Q_NEXT;              //0x4C
    uint32_t SCRATHC0;                 //0x50
    uint32_t SCRATHC1;                 //0x54
    uint32_t SCRATHC2;                 //0x58
    uint32_t SCRATHC3;                 //0x5C
    union _PCIE_INTR_CNTRL {
        uint32_t UINT;
        struct {
            uint32_t INTR_MASK :1;          // Bit 0
            uint32_t DMA_ERR_INT_ENABLE :1; // Bit 1
            uint32_t SGL_INT_ENABLE :1;     // Bit 2
            uint32_t :13;                   // Bit 3-15
            uint32_t COALESCE_COUNT :8;     // Bit 16-23
            uint32_t :8;                    // Bit 24-31
        } BIT;
    } PCIE_INTR_CNTRL;                       //0x60
    union _PCIE_INTR_STATUS {
        uint32_t UINT;
        struct {
            uint32_t :1;                   // Bit 0
            uint32_t DMA_ERR_INT :1;       // Bit 1
            uint32_t DMA_SGL_INT :1;       // Bit 2
            uint32_t DMA_SW_INT :1;        // Bit 3
            uint32_t :28;                  // Bit 4-31
        } BIT;
    } PCIE_INTR_STATUS;                      //0x64
    union _AXI_INTR_CNTRL {
        uint32_t UINT;
        struct {
            uint32_t INTR_ENABLE :1;          // Bit 0
            uint32_t DMA_ERR_INT_ENABLE :1;   // Bit 1
            uint32_t SGL_INT_ENABLE :1;       // Bit 2
            uint32_t :13;                     // Bit 3-15
            uint32_t COAlLESCE_COUNT :8;      // Bit 16-23
            uint32_t :8;                      // Bit 24-31
        } BIT;
    } AXI_INTR_CNTRL;                       //0x68
    union _AXI_INTR_STATUS {
        uint32_t UINT;
        struct {
            uint32_t :1;                   // Bit 0
            uint32_t DMA_ERR_INT :1;       // Bit 1
            uint32_t DMA_SGL_INT :1;       // Bit 2
            uint32_t DMA_SW_INT :1;        // Bit 3
            uint32_t :28;                  // Bit 4-31
        } BIT;
    } AXI_INTR_STATUS;                      //0x6C
    union _PCIE_INTR_ASSERT {
        uint32_t UINT;
        struct {
            uint32_t :3;                      // Bit 0 - 2
            uint32_t SOFTWARE_INTRPT :1;      // Bit 3
            uint32_t :28;                     // Bit 4-31
        } BIT;
    } PCIE_INTR_ASSERT;                     //0x70
    union _AXI_INTR_ASSERT {
        uint32_t UINT;
        struct {
            uint32_t :3;                   // Bit 0 - 2
            uint32_t SOFTWARE_INTRPT :1;   // Bit 3
            uint32_t :28;                  // Bit 4-31
        } BIT;
    } AXI_INTR_ASSERT;                      //0x74
    union _DMA_CHANNEL_CTRL {
        uint32_t UINT;
        struct {
            uint32_t DMA_ENABLE :1;            // Bit 0
            uint32_t DMA_RESET :1;             // Bit 1
            uint32_t STA_Q_ELE_SIZE :1;        // Bit 2
            uint32_t :29;                      // Bit 3-31
        } BIT;
    } DMA_CHANNEL_CTRL;                     //0x78
    union _DMA_CHANNEL_STATUS {
        uint32_t UINT;
        struct {
            uint32_t DMA_RUNNING_BIT :1;           // Bit 0
            uint32_t :3;                       // Bit 1-3
            uint32_t CHANNEL_NUMBER :10;       // Bit 4-13
            uint32_t :1;                       // Bit 14
            uint32_t CHANNEL_PRESENT :1;       // Bit 15
            uint32_t :16;                      // Bit 16-31
        } BIT;
    } DMA_CHANNEL_STATUS;                   //0x7C
}__attribute__((__packed__)) ZYNQMP_DMA_REGISTERS;



typedef struct zynqmp_dma_src_queue_entry {
    uint64_t address;

    union {
        uint32_t uint32_reg;
        struct {
            uint32_t byte_count :24;
            uint32_t location :1;
            uint32_t eop :1;
            uint32_t interrupt :1;
            uint32_t:1;
            uint32_t dma_read_attributes :4;
        } bit;
    } control_byte_count;

    union {
        uint32_t uint32_reg;
        struct { /*  Bit  Access */
            uint32_t user_handle :16;
            uint32_t user_id :16;
        } bit;
    } user;
} __attribute__((__packed__)) ZYNQMP_DMA_SRC_QUEUE_ENTRY __aligned(64);


typedef struct zynqmp_dma_dst_queue_entry {
    uint64_t address;

    union {
        uint32_t uint32_reg;
        struct {
            uint32_t byte_count:24;
            uint32_t location:1;
            uint32_t back_to_back:1;
            uint32_t:2;
            uint32_t dma_write_attributes :4;
        } bit;
    } control_byte_count;

    union {
        uint32_t uint32_reg;
        struct { /*  Bit  Access */
            uint32_t user_handle :16;
            uint32_t:16;
        } bit;
    } user;
} __attribute__((__packed__)) ZYNQMP_DMA_DST_QUEUE_ENTRY __aligned(64);


typedef struct zynqmp_dma_stat_queue_entry {
    union {
        uint32_t uint32_reg;
        struct { /*  Bit  Access */
            uint32_t completed :1;
            uint32_t source_error :1;
            uint32_t destination_error :1;
            uint32_t internal_error :1;
            uint32_t byte_count :27;
            uint32_t upper_status_non_zero :1;
        } bit;
    } status_flag_byte_count;
    union {
        uint32_t uint32_reg;
        struct { /*  Bit  Access */
            uint32_t user_handle :16;
            uint32_t user_id :16;
        } bit;
    } user;
}  __attribute__((__packed__)) ZYNQMP_DMA_STAT_QUEUE_ENTRY __aligned(64);


typedef enum zynqmp_dma_channel_state {
    CHANNEL_RESOURCE_UNALLOCATED = 0, // DMA channel resources are not allocated
    CHANNEL_INITIALIZING,             // DMA channel is getting initialized.
    CHANNEL_FAILED,                   // DMA channel initialization failed.
    CHANNEL_AVAILABLE,                // DMA channel is available for transfers
    CHANNEL_RESET,                    // DMA channel is in reset
    CHANNEL_CONFIGURING,              // DMA channel is getting configured
    CHANNEL_SHUTDOWN                  // DMA channel is getting shutdown.
} TY_ZYNQMP_DMA_CHANNEL_STATE;



typedef struct aimpci_hw_zynqmp_dma_channel{

    int                     id;
    struct aim_pci_device*  aimDevice;
    u32                     control_offset;
    enum aim_dma_direction  direction;
    TY_ZYNQMP_DMA_CHANNEL_STATE state;
    spinlock_t              lock;

    u32                     src_sgl_size;
    u32                     dst_sgl_size;
    u32                     stat_sgl_size;

    void* src_sgl_buffer;
    void* dst_sgl_buffer;
    void* stas_sgl_buffer;
    void* stad_sgl_buffer;

    dma_addr_t src_sgl_buffer_address;
    dma_addr_t dst_sgl_buffer_address;
    dma_addr_t stas_sgl_buffer_address;
    dma_addr_t stad_sgl_buffer_address;

    size_t src_sgl_buffer_size;
    size_t dst_sgl_buffer_size;
    size_t stat_sgl_buffer_size;


    ZYNQMP_DMA_REGISTERS  * reg;
} AIMPCI_HW_ZYNQMP_DMA_CHANNEL;



int aim_zynqmp_start_dma_read_generic(struct aim_pci_device* device, struct aim_dma_buffer* buffer, enum aim_memory_type memType, loff_t offset);

int aim_zynqmp_dma_channel_allocate(struct aim_pci_device*, AIMPCI_HW_ZYNQMP_DMA_CHANNEL * channel);
int aim_zynqmp_dma_free(AIMPCI_HW_ZYNQMP_DMA_CHANNEL * channel);
int aim_zynqmp_dma_channel_stop(AIMPCI_HW_ZYNQMP_DMA_CHANNEL * channel);
int aim_zynqmp_dma_channel_start(AIMPCI_HW_ZYNQMP_DMA_CHANNEL * channel);

int aim_zynqmp_dma_channel_feed(AIMPCI_HW_ZYNQMP_DMA_CHANNEL * channel, struct aim_pci_device* device, struct aim_dma_buffer* buffer, u64 ullTargetAddress, u32 ulSize );

#endif /* AIM_ZYNQMP_DEVICE_H_ */
