/* SPDX-FileCopyrightText: 2013-2023 AIM GmbH <info@aim-online.com> */
/* SPDX-License-Identifier: GPL-2.0-or-later */

/*! \file aim_pci_module.h
 *
 *  This header file contains generic declarations for
 *  all AIM PCI driver modules.
 *
 */

#ifndef AIM_PCI_MODULE_H_
#define AIM_PCI_MODULE_H_


#include <linux/pci.h>
#include <linux/version.h>
#include <linux/bcd.h>

/* reinit_completion replaced INIT_COMPLETION macro since
   Kernel 3.13
*/
#if LINUX_VERSION_CODE < KERNEL_VERSION(3,13,0)
static inline void reinit_completion(struct completion* c)
{
    INIT_COMPLETION(*c);
}
#endif


/* The bcd2bin and bin2bcd macros were introduced in kernel version 2.6.27 */
#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,27)

#define bcd2bin(val) BCD2BIN(val)
#define bin2bcd(val) BIN2BCD(val)

#endif


/* The IS_ERR_OR_NULL macro was introduced in kernel 2.6.33 */
#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,33)

static inline long IS_ERR_OR_NULL(const void *ptr)
{
    return !ptr || IS_ERR_VALUE((unsigned long)ptr);
}

#endif




/*! \def ZYNQMP_MIL_HOSTIO_CHN1_CMD_OFFSET
 * Offset of the TSW host-io communicaton channel relative to the Host-IO area within the shared memory
 */
#define ZYNQMP_MIL_HOSTIO_CHN1_CMD_OFFSET  0x00011000

/*! \def ZYNQMP_MIL_HOSTIO_CHN1_ACK_OFFSET
 * Offset of the TSW host-io communicaton channel relative to the Host-IO area within the shared memory
 */
#define ZYNQMP_MIL_HOSTIO_CHN1_ACK_OFFSET  0x00011000 + 0x40000

/*! \def ZYNQMP_MIL_HOSTIO_CHN1_SIZE
 * Size of the TSW host-io communicaton channel
 */
#define ZYNQMP_MIL_HOSTIO_CHN1_SIZE        0x3000




/*! \def AIM_DRV_NAME
 * Name of the AIM PCI driver
 */
#define AIM_DRV_NAME    KBUILD_MODNAME


/*! \def AIM_MAX_DEVICES
 * Maximum number of devices the PCI driver supports at the same time
 */
#define AIM_MAX_DEVICES 32



/*! \def MAX_DRIVER_VERSION_LENGTH
 * Maximum length of driver version string including trailing zero
 */
#define MAX_DRIVER_VERSION_LENGTH 64


/*! \def AIM_PCI_ID
 * PCI Vendor ID of AIM GmbH
 */
#define AIM_PCI_ID  0x1447


/*! \def AIM_APXE_DEVICE
 * PCI IDs of AIM APXE platform
 */
#define AIM_APXE_DEVICE \
    .vendor = PCI_VENDOR_ID_INTEL, .device = 0x338A


/*! \def AIM_APS_DEVICE
 * PCI device ID of AIM APS platform
 */
#define AIM_APS_DEVICE \
    .vendor = AIM_PCI_ID, .device = 0x2310


/*! \def AIM_AMC_DEVICE
 * PCI device ID of AIM PMC platform without ASP
 */
#define AIM_AMC_DEVICE \
    .vendor = AIM_PCI_ID, .device = 0x4252


/*! \def AIM_AMC_ML3_DEVICE
 * PCI device ID of AIM PMC platform without ASP
 */
#define AIM_AMC_ML3_DEVICE \
    .vendor = AIM_PCI_ID, .device = 0x4252


/*! \def AIM_AMC64_ASP_DEVICE
 * PCI device ID of AIM 64-Bit PMC platform with ASP
 */
#define AIM_AMC64_ASP_DEVICE \
    .vendor = AIM_PCI_ID, .device = 0x4857

/*! \def AIM_PCIE_DEVICE
 * Vendor and device ID of PCIe (APE) based devices
 */
#define AIM_PCIE_DEVICE \
    .vendor = AIM_PCI_ID, .device = 0x1310

/*! \def AIM_PXIE_PCI_DEVICE
 * Vendor and device ID of PXI(e) or PCI (ACXX/ACE/APXX) based devices
 */
#define AIM_PXIE_PCI_DEVICE \
    .vendor = AIM_PCI_ID, .device = 0x0000

/*! \def AIM_XMC_DEVICE
 * PCI device ID of AIM XMC (AXC/AMCX < 2.00) platform
 */
#define AIM_XMC_DEVICE \
    .vendor = AIM_PCI_ID, .device = 0x1200

/*! \def AIM_PMCX2_DEVICE
 * PCI device ID of AIM PMC (AMCX >= 2.00) platform
 */
#define AIM_PMCX2_DEVICE \
    .vendor = AIM_PCI_ID, .device = 0x0001

/*! \def AIM_AEC_DEVICE
 * PCI device ID of AIM Express Card (AEC) platform
 */
#define AIM_AEC_DEVICE \
    .vendor = AIM_PCI_ID, .device = 0x1100


/*! \def AIM_PCIX_DEVICE
 * PCI device ID of AIM PCI-X (APX) platform
 */
#define AIM_PCIX_DEVICE \
    .vendor = PCI_VENDOR_ID_PLX, .device = 0x6541

/*! \def AIM_PCIX_OLD_DEVICE
 * PCI device ID of AIM PCI-X (APX) platform
 */
#define AIM_PCIX_MAIN1_DEVICE \
    .vendor = PCI_VENDOR_ID_HINT, .device = 0x0029


/*! \def AIM_PCIE_INTEL_DEVICE
 * PCIe device with Intel bridge (PCIE APX based) platform
 */
#define AIM_PCIE_INTEL_DEVICE \
    .vendor = PCI_VENDOR_ID_INTEL, .device = 0x031A



/*! \def AIM_PCIE_INTEL_318_DEVICE
 * PCIe device with Intel bridge (PCIE APX based) platform
 */
#define AIM_PCIE_INTEL_318_DEVICE \
    .vendor = PCI_VENDOR_ID_INTEL, .device = 0x0318



/*! \def AIM_PCCARD_DEVICE
 * PCI device ID of AIM PC-Card (APM) platform
 */
#define AIM_PCCARD_DEVICE \
    .vendor = AIM_PCI_ID, .device = 0x4A57


/*! \def AIM_PC104_DEVICE
 * PCI device ID of AIM PC-104 (AP-104) platform
 */
#define AIM_PC104_DEVICE \
    .vendor = AIM_PCI_ID, .device = 0x4b41


/*! \def AIM_AYXX_DEVICE
 * PCI device ID of AIM APXX platform
 */
#define AIM_AYXX_DEVICE \
    .vendor = AIM_PCI_ID, .device = 0x0003


/*! \def AIM_MINIPCIE_DEVICE
 * PCI device ID of AIM AYS MINIPCIE platform
 */
#define AIM_MINIPCIE_DEVICE \
    .vendor = AIM_PCI_ID, .device = 0x2100    
    
/*! \def AIM_MINIPCIE_DEVICE
 * PCI device ID of AIM AYS MINIPCIE platform
 */
#define AIM_MINIPCIE_ARTIX7_DEVICE \
    .vendor = AIM_PCI_ID, .device = 0x4100    
  
/*! \def AIM_XMC_ARTIXUS
 * PCI device ID of AIM ArtixUS XMC platform
 */
#define AIM_XMC_ARTIXUS_DEVICE \
    .vendor = AIM_PCI_ID, .device = 0x5200    

/*! \def AIM_APE_ETH_DEVICE
 * PCI device ID of AIM APXX platform
 */
#define AIM_APE_ETH_DEVICE \
    .vendor = AIM_PCI_ID, .device = 0x2010

/*! \def AIM_AMCX_ETH_DEVICE
 * PCI device ID of AIM AMCX platform
 */
#define AIM_AMCX_ETH_DEVICE \
    .vendor = AIM_PCI_ID, .device = 0x0002

#define AIM_DEVICE_SUB(subvend, subdev) \
         .subvendor = (subvend), .subdevice = (subdev)


/*! \enum aim_pci_protocols
 * Enumeration of all protocols the generic PCI driver part supports
 */
enum aim_pci_protocols
{
    PROTOCOL_FC = 0,    /*!< Fibre Channel protocol */
    PROTOCOL_FDX,       /*!< AFDX Protocol */
    PROTOCOL_429,       /*!< 429 Protocol */
    PROTOCOL_1553,      /*!< 1553 Protocol */
    PROTOCOL_SERIAL     /*!< Generic Serial Protocol */
};


/*! \struct aim_pci_driver_data
 *  This structure holds driver specific data
 *  during runtime of the dirver module.
 */
struct aim_pci_driver_data
{
    dev_t major;                /*!< The character device major number the driver uses */
    struct class* deviceClass;  /*!< The class the driver belongs to */
};


/*! \struct aim_driver_version
 *
 * This structure comprises the version of an
 * AIM PCI driver module
 */
struct aim_driver_version
{
    u32 major;
    u32 minor;
    u32 patch;
    u32 build;
    char full_version[MAX_DRIVER_VERSION_LENGTH];
};




extern unsigned long deviceMinors[];




/*! \brief Determines minor number for a device
 *
 * This function returns the first free minor number for a device
 * \return returns minor number on success, a negative error on failure
 */
extern long aim_pci_get_minor(void);


/*! \brief Releases a minor number of a device
 *
 * @param minor the minor number to release
 */
extern void aim_pci_put_minor(unsigned long minor);




/*! This is the global version of the PCI driver
 * It uses the versions from the file generated/version.h
 */
extern struct aim_driver_version driverVersion;


/*! This is a global structure that holds driver specific data \n
 * of the driver module
 */
extern struct aim_pci_driver_data driver_data;


/*! This is the PCI device table the driver supports. \n
 *  The protocol specific part of the driver must provide this one.
 */
extern const struct pci_device_id aim_pci_table[];




#endif /* AIM_PCI_MODULE_H_ */
