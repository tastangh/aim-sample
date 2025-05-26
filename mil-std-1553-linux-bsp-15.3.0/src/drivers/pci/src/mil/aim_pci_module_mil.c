/* SPDX-FileCopyrightText: 2016-2024 AIM GmbH <info@aim-online.com> */
/* SPDX-License-Identifier: GPL-2.0-or-later */

/*! \file aim_pci_module_mil.c
 *
 *  This file contains definitions specific
 *  for the 1553 PCI driver
 */


#include <linux/module.h>
#include <linux/pci.h>
#include "generated/version.h"
#include "aim_pci_module.h"
#include "aim_pci_device_mil.h"
#include "aim_ioctl.h"
#include "aim_ioctl_mil.h"
#include "aim_irq_mil.h"

#include "aim_zynqmp_device.h"
#include "aim_tsw_mil.h"
#include "aim_ays_device.h"


/*! 1553 specific functions for basic device operations */
static struct aim_pci_device_ops aim_mil_device_ops = {
        .create_device = aim_pci_device_create_mil,
        .free_device = aim_pci_device_free_mil,
        .init_device = aim_pci_device_init_mil,
};


/*! 1553 specific IOCTL handlers */
static struct aim_pci_ioctl_ops aim_mil_ioctl_ops = {
        .target_command = aim_handle_target_cmd_mil,
        .target_command_direct = NULL,
};


/*! 1553 specific interrupt handlers */
static struct aim_pci_irq_ops aim_mil_irq_ops = {
        .handle_biu_irq = aim_pci_mil_biu_irq,
        .handle_asp_irq = aim_pci_mil_asp_irq,
};


/*! 1553 AyE specific DMA handlers */
static struct aim_pci_dma_ops aim_mil_aye_dma_ops = {
        .start_dma_read = aim_pci_device_start_dma_read_generic
};

/*! 1553 ZynqMP specific DMA handlers */
static struct aim_pci_dma_ops aim_mil_zynqmp_dma_ops = {
        .start_dma_read = aim_pci_device_start_dma_read_generic,
        .get_dma_device_offset = aim_tsw_mil_get_dma_memory_device_offset,
};

/*! 1553 AYSP specific DMA handlers */
static struct aim_pci_dma_ops aim_mil_ays_dma_ops = {
        .start_dma_read = aim_pci_device_start_dma_read_generic,
        .get_dma_device_offset = aim_tsw_mil_get_dma_memory_device_offset,
};

/*! 1553 ArtixUS specific DMA handlers */
static struct aim_pci_dma_ops aim_mil_artixus_dma_ops = {
        .start_dma_read = aim_pci_device_start_dma_read_generic
};


/*! Specific data for APE/ACE/ACXX 1553 boards */
static struct aim_pci_device_data pcie_mil_data = {
        .platform = AI_DEVICE_AYE,
        .protocol = PROTOCOL_1553,
        .name = "APE/ACE/APXX/ACXX-1553",
        .deviceOps = &aim_mil_device_ops,
        .ioctlOps = &aim_mil_ioctl_ops,
        .irqOps = &aim_mil_irq_ops,
        .dmaOps = &aim_mil_aye_dma_ops
};


/*! Specific data for AXC/AMCX 1553 boards */
static struct aim_pci_device_data xmc_mil_data = {
        .platform = AI_DEVICE_AYE,
        .protocol = PROTOCOL_1553,
        .name = "AXC/AMCX-1553",
        .deviceOps = &aim_mil_device_ops,
        .ioctlOps = &aim_mil_ioctl_ops,
        .irqOps = &aim_mil_irq_ops,
        .dmaOps = &aim_mil_aye_dma_ops
};


/*! Specific data for AEC 1553 boards */
static struct aim_pci_device_data aec_mil_data = {
        .platform = AI_DEVICE_AYE,
        .protocol = PROTOCOL_1553,
        .name = "AEC-1553",
        .deviceOps = &aim_mil_device_ops,
        .ioctlOps = &aim_mil_ioctl_ops,
        .irqOps = &aim_mil_irq_ops,
        .dmaOps = &aim_mil_aye_dma_ops
};


/*! Interrupt based PCI communication channels for ZynqMP boards */
static struct aim_pci_com_channel_data zynqmp_com_channels[] = {
        [0] = {  /* This is the generic AYS update communication channel */
                .id = 0,
                .commandOffset        = ZYNQMP_GLOBAL_MEM_SIZE + AI_HW_ZYNQMP_HOSTIO_CHANNEL0_OFFSET,
                .commandSize          = AI_HW_ZYNQMP_HOSTIO_CHANNEL0_SIZE,
                .responseOffset       = ZYNQMP_GLOBAL_MEM_SIZE + AI_HW_ZYNQMP_HOSTIO_CHANNEL0_OFFSET,
                .responseSize         = AI_HW_ZYNQMP_HOSTIO_CHANNEL0_SIZE,
                .resourceID           = ZYNQMP_GLOBAL_SHARED_MEM_BAR,
                .signalRegisterOffset = 0, /* unused on ZynqMP */
        },

        [1] = { /* This is the MIL specific communication channel */
                .id = 1,
                .commandOffset        = ZYNQMP_GLOBAL_MEM_SIZE + ZYNQMP_MIL_HOSTIO_CHN1_CMD_OFFSET,
                .commandSize          = ZYNQMP_MIL_HOSTIO_CHN1_SIZE,
                .responseOffset       = ZYNQMP_GLOBAL_MEM_SIZE + ZYNQMP_MIL_HOSTIO_CHN1_ACK_OFFSET,
                .responseSize         = ZYNQMP_MIL_HOSTIO_CHN1_SIZE,
                .resourceID           = ZYNQMP_GLOBAL_SHARED_MEM_BAR,
                .signalRegisterOffset = 0, /* unused on ZynqMP */
        },
};


/*! Interrupt based PCI communication channels for AYS/Zynq boards */
static struct aim_pci_com_channel_data ays_com_channels[] = {
        [0] = {  /* This is the generic AYS update communication channel */
                .id = 0,                                                /* 0 = AimGenCon (Generic Communicaton) */
                .commandOffset        = AYS_COM_CHANNEL0_OFFSET,        /* 0 = Offset for GenCon */
                .commandSize          = AYS_COM_CHANNEL0_SIZE,          /* 64Kb */
                .responseOffset       = AYS_COM_CHANNEL0_OFFSET,        /* 0 = Offset for GenCon */
                .responseSize         = AYS_COM_CHANNEL0_SIZE,          /* 64Kb */
                .resourceID           = AYS_SHARED_MEM_BAR,             /*(BAR 0 containing generic Host-IO channel with 64k)*/
                .signalRegisterOffset = AYS_AME_TARGET_SIGNAL_REG_OFFSET,
        },
};



/*! Specific data for APX 1553 boards */
static struct aim_pci_device_data zynqmp_mil_data = {
        .platform = AI_DEVICE_ZYNQMP_ASP,
        .protocol = PROTOCOL_1553,
        .name = "APXX3910",
        .deviceOps = &aim_mil_device_ops,
        .comChannels = zynqmp_com_channels,
        .numComChannels = ARRAY_SIZE(zynqmp_com_channels),
        .ioctlOps = &aim_mil_ioctl_ops,
        .irqOps = &aim_mil_irq_ops,
        .dmaOps = &aim_mil_zynqmp_dma_ops
};

/*! Specific data for AME1553 boards */
static struct aim_pci_device_data ays_mil_data = {
        .platform = AI_DEVICE_AYS,
        .protocol = PROTOCOL_1553,
        .name = "AME1553",
        .deviceOps = &aim_mil_device_ops,
        .comChannels = ays_com_channels,
        .numComChannels = ARRAY_SIZE(ays_com_channels),
        .ioctlOps = &aim_mil_ioctl_ops,
        .irqOps = &aim_mil_irq_ops,
        .dmaOps = &aim_mil_ays_dma_ops
};


/*! Specific data for AMEE/AM2E1553 boards */
static struct aim_pci_device_data artix7_mil_data = {
        .platform = AI_DEVICE_ARTIX7,
        .protocol = PROTOCOL_1553,
        .name = "AMEE/AM2E1553",
        .deviceOps = &aim_mil_device_ops,
        .ioctlOps = &aim_mil_ioctl_ops,
        .irqOps = &aim_mil_irq_ops
};


/*! Specific data for AXCX1553 boards */
static struct aim_pci_device_data artixus_mil_data = {
        .platform = AI_DEVICE_ARTIXUS,
        .protocol = PROTOCOL_1553,
        .name = "AXCX1553",
        .deviceOps = &aim_mil_device_ops,
        .ioctlOps = &aim_mil_ioctl_ops,
        .irqOps = &aim_mil_irq_ops,
        .dmaOps = &aim_mil_artixus_dma_ops
};

#define AIM_PCI_DEVICE_SUB( vendor_dev, subvend, subdev) \
         d, .subvendor = (subvend), .subdevice = (subdev)



#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,25)
const struct pci_device_id aim_pci_table[] = {
#elif LINUX_VERSION_CODE < KERNEL_VERSION(3,17,0)
DEFINE_PCI_DEVICE_TABLE(aim_pci_table) = {
#else
const struct pci_device_id aim_pci_table[] = {
#endif
        /* AYE */
        {AIM_AEC_DEVICE,        AIM_DEVICE_SUB(AIM_PCI_ID, 0x0001), 0,0,(kernel_ulong_t) &aec_mil_data},  /*!< AEC1553          */
        {AIM_XMC_DEVICE,        AIM_DEVICE_SUB(AIM_PCI_ID, 0x0011), 0,0,(kernel_ulong_t) &xmc_mil_data},  /*!< AXC1553/AMCX1553 */
        {AIM_PMCX2_DEVICE, AIM_DEVICE_SUB(AIM_PCI_ID, 0x0011), 0,0,(kernel_ulong_t) &xmc_mil_data},  /*!< AXC1553/AMCX1553 new device id 1 */
        {AIM_PCIE_DEVICE,       AIM_DEVICE_SUB(AIM_PCI_ID, 0x0011), 0,0,(kernel_ulong_t) &pcie_mil_data}, /*!< APE1553          */
        {AIM_PXIE_PCI_DEVICE,   AIM_DEVICE_SUB(AIM_PCI_ID, 0x0011), 0,0,(kernel_ulong_t) &pcie_mil_data}, /*!< ACE1553/ACXX1553/APXX1553 */

        /* AYXX */
        {AIM_AYXX_DEVICE,       AIM_DEVICE_SUB(AIM_PCI_ID, 0x0031), 0,0,(kernel_ulong_t) &zynqmp_mil_data},  /*!< APXX3910        */
        
        /* AYS */
        {AIM_MINIPCIE_DEVICE,   AIM_DEVICE_SUB(AIM_PCI_ID, 0x0001), 0,0,(kernel_ulong_t) &ays_mil_data},  /*!< AME1553        */
        
        /* Artix7 */
        {AIM_MINIPCIE_ARTIX7_DEVICE,   AIM_DEVICE_SUB(AIM_PCI_ID, 0x0011), 0,0,(kernel_ulong_t) &artix7_mil_data},  /*!< AMEE1553/AM2E1553 */

        /* ArtixUS */
        {AIM_XMC_ARTIXUS_DEVICE,   AIM_DEVICE_SUB(AIM_PCI_ID, 0x0011), 0,0,(kernel_ulong_t) &artixus_mil_data},  /*!< AXCX1553-nT */
        {AIM_XMC_ARTIXUS_DEVICE,   AIM_DEVICE_SUB(AIM_PCI_ID, 0x1111), 0,0,(kernel_ulong_t) &artixus_mil_data},  /*!< AXCX1553-n */

        /* end of list */
        {0,0,0,0,0,0,0},

};




MODULE_DEVICE_TABLE(pci,aim_pci_table);

MODULE_DESCRIPTION("AIM PCI driver for 1553 boards");
MODULE_VERSION(VERSION_STRING_FULL);

