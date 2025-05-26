/* SPDX-FileCopyrightText: 2013-2024 AIM GmbH <info@aim-online.com> */
/* SPDX-License-Identifier: GPL-2.0-or-later */

/*! \file aim_pci_device.h
 *
 *  This file contains declarations for protocol
 *  independent functionality of AIM PCI devices
 *
 */

#ifndef AIM_PCI_DEVICE_H_
#define AIM_PCI_DEVICE_H_


#include <linux/cdev.h>
#include <linux/interrupt.h>
#include <linux/radix-tree.h>
#include "Ai_def.h"
#include "aim_irq.h"
#include "aim_pci_module.h"
#include "aim_debug.h"
#include "aim_pci_com_channel.h"
#include "aim_nl.h"
#include "aim_aio.h"
#include "aim_ref.h"


#ifdef NEED_MONITOR

#include "type_def.h"

#endif


#ifdef NEED_MYMON

#include "aim_pci_mymon.h"

#endif


/*! \def AIM_DEVICE_MAX_AIO_QUEUES
 * Maximum number of asynchronous IO queues
 */
#define AIM_DEVICE_MAX_AIO_QUEUES 4


#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,26)

/* dev_name was introduced in kernel 2.6.26 */
static inline const char *dev_name(const struct device *dev)
{
    return kobject_name(&dev->kobj);
}


#endif


#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,27)

static inline resource_size_t resource_size(struct resource *res)
{
    return res->end - res->start + 1;
}

#endif


/*! \struct aim_pci_aio_ops
 *
 * This structure comprises all asynchronous IO operations for a device
 */
struct aim_pci_aio_ops
{
    aim_aio_request_start_fn start_func;    /*!< start function for aio operations */
    aim_aio_request_cancel_fn cancel_func;  /*!< Cancel functions for aio operations */
};


/*! \struct aim_pci_device_data
 *
 * This structure holds immutable data for a given device. \n
 * The protocol specific part of the driver has to set up \n
 * the correct data for each device it is responsible for.
 */
struct aim_pci_device_data
{
    TY_E_DEVICE_GROUP platform;                       /*!< The device group the device belongs to */
    enum aim_pci_protocols protocol;                  /*!< The data bus protocol the device implements */
    char* name;                                       /*!< Product name of the device */
    struct aim_pci_device_ops* deviceOps;             /*!< Protocol specific handler functions for the device initialization */
    struct aim_pci_ioctl_ops* ioctlOps;               /*!< Protocol specific handler functions for the device IOCTLs */
    struct aim_pci_irq_ops* irqOps;                   /*!< Protocol specific handler functions for IRQ handling */
    struct aim_pci_dma_ops* dmaOps;                   /*!< Protocol specific handler functions for DMA handling */
    unsigned int numComChannels;                      /*!< number of interrupt based host to target communication channels */
    struct aim_pci_com_channel_data* comChannels;  /*!< pointer to array of \ref struct aim_pci_com_channel_data elements. Must have \ref numComChannels entries */
    struct aim_pci_aio_ops* aio_ops;                  /*!< Protocol specific handler functions for asynchronous IO */
};


/*! \struct aim_pci_device_properties
 *
 * This structure comprises basic properties of an AIM device.
 * Usually, these are stored in NOVRAM on-board.
 */
struct aim_pci_device_properties
{
    __u32 serial;       /*!< Serial number of device */
    __u32 config;       /*!< Novram Board Config setting */
    __u32 type;         /*!< Novram Board Type setting */
    __u32 sub_type;     /*!< Novram Board Sub Type setting */
    __u32 hw_variant;   /*!< Novram Hardware Variant setting */
    __u32 part_number;  /*!< Part number of device */
    __u32 app_type;     /*!< Novram Board Application Type setting */
};


/*! \enum aim_dma_channels
 * Enumeration of possible DMA channels.
 */
enum aim_dma_channels
{
    AIM_DMA_CHANNEL_READ_BLOCK_MEM_DATA  = 0, /*!< If this device is either ZynqMP or ArtixUS it has one DMA channel. */
};

/*! \struct aim_pci_device
 *
 * This structure represents a generic, protocol
 * independent AIM PCI device
 */
struct aim_pci_device
{
    struct aim_pci_device_data* deviceData;     /*!< immutable device data, set up by protocol specific driver part */
    struct pci_dev* pciDevice;                  /*!< pointer to corresponding Linux PCI device */

    struct aim_pci_device_properties properties; /*!< Basic properties of the device */

    unsigned int minor;                         /*!< minor number of the device */

    struct cdev characterDev;                   /*!< character device used for user space interface */

    atomic_t referenceCount;                 /*!< reference counter for the device structure */

    void __iomem *sharedMemory;                 /*!< pointer to shared memory of device */
    resource_size_t    sharedMemoryPhysical;       /*!< physical address of shared memory if not allocated */
    resource_size_t sharedMemorySize;           /*!< size of shared memory in bytes */

    void __iomem *globalMemory;                 /*!< pointer to global memory of device */
    resource_size_t    globalMemoryPhysical;       /*!< physical address of global memory  */
    resource_size_t globalMemorySize;           /*!< size of global memory in bytes */

    void __iomem *ioMemory;                     /*!< pointer to IO memory of device */
    resource_size_t    ioMemoryPhysical;           /*!< physical address of io memory  */
    resource_size_t ioMemorySize;               /*!< size of IO memory in bytes */


    void __iomem *   hostIoMemory;               /*!< pointer to Host-IO memory of device */
    resource_size_t  hostIoMemoryPhysical;       /*!< physical address of Host-IO memory */
    resource_size_t  hostIoMemorySize;           /*!< size of Host-IO memory in bytes */

    void __iomem *globalExtMemory;              /*!< pointer to global extension memory of device */
    resource_size_t    globalExtMemoryPhysical;
    resource_size_t globalExtMemorySize;        /*!< size of global extension memory in bytes */

    void __iomem *dmaMemory;                     /*!< pointer to DMA memory of device */
    resource_size_t dmaMemoryPhysical;           /*!< physical address of DMA memory  */
    resource_size_t dmaMemorySize;               /*!< size of DMA memory in bytes */

    atomic_t pendingIrqs[AIM_PCI_MAX_IRQ];      /*!< Counter for all the different pending IRQ types */
    struct tasklet_struct irqTasklet;           /*!< tasklet that runs bottom half of IRQ handler */

    struct mutex targetCmdLock;                 /*!< Mutex for locking access to the target command area */

    struct mutex dmaRequestLock;                /*!< Mutex for locking access to DMA request submission */

    struct radix_tree_root pciComChannels;      /*!< Radix tree that contains all interrupt based communication channels, with their ID as key */
    struct mutex zynqmp_hostio_lock;            /*!< lock for Host-IO interrupt event log list (Host to device)*/

    struct radix_tree_root pciDmaChannels;      /*!< Radix tree that contains host driver DMA channels, with their ID as key (Currently only used for ZynqMP, ArtixUS) */

    struct aim_nl_setup nl_setup;               /*!< Protocol specific Netlink setup that can be as interface to user space */

    bool hasAsp;                                /*!< Indicates if device has ASP */
    bool hasMsiEnabled;                         /*!< Indicates if device has MSI interrupts enabled */

    struct list_head dmaQueue;                  /*!< Device global queue for pending DMA transactions */
    spinlock_t dmaQueueLock;                    /*!< spin-lock for global DMA transaction queue */


#ifdef NEED_MONITOR

    TY_MONITOR_DATA_STRUCT* monitorData;        /*!< Monitor data that is needed on some platforms to initialize target software */

#endif


#ifdef NEED_MYMON

    TyMyMonContext* mymonData;                  /*!< MyMon specific data */
    struct TyMyMonUserData mymonUserData;       /*!< Driver specific context for MyMon */

#endif


#ifdef AIM_AYE_DS
    /* Digital Scope Request Queues */
    spinlock_t         ScopeBufferQueueLock;
    struct list_head   ScopeBufferQueue;
    struct list_head   ScopeBufferPendingQueue;
    struct list_head   ScopeBufferDoneQueue;
    wait_queue_head_t  ScopeBufferDoneWaitQueue;
    atomic_t           ScopeBufferCanceled;
#endif

    struct aim_aio_queue* aio_queues[AIM_DEVICE_MAX_AIO_QUEUES]; /*!< array holding pointers to asynchronous IO queues of a device */
    spinlock_t aio_queues_lock;  /*!< lock for synchronizing access to asynchronous IO queue array */
};


/*! \def aim_to_dev
 * Converts pointer to struct aim_pci_device to corresponding struct dev
 */
#define aim_to_dev(aimDevice) &aimDevice->pciDevice->dev




/*! \struct aim_pci_device_ops
 * This structure holds handler functions where protocol specific driver parts
 * can insert their functions to handle basic device functions.
 */
struct aim_pci_device_ops
{
    struct aim_pci_device* (*create_device) (struct pci_dev*, const struct pci_device_id*); /*!< This function must create the protocol specific
                                                                                                 device structure and return a pointer to the included
                                                                                                 generic AIM PCI device structure */

    void (*free_device) (struct aim_pci_device* aimDevice); /*!< This function must delete the protocol specific device structure */
    int (*init_device)(struct aim_pci_device* aimDevice);   /*!< This function must perform protocol specific initialization of a device */
};




/*! \brief Release function for AIM PCI devices
 *
 * This function is called when the reference count of an AIM PCI device drops to zero.\n
 * It will call the protocol specific handler function which is responsible for freeing \n
 * all allocated resources.
 * @param kref the corresponding reference count when kref_put was called
 */
void release_device(atomic_t* kref);


/*! \brief Sets up the character device user space interface
 *
 * This function sets up the character device based user interface
 * for a given device. \n
 * The device structure has to be initialized before, which means
 * it has to be allocated a minor version
 * @param aimDevice The device to set up the interface for
 * @return returns 0 on success, a negative error code otherwise
 */
int aim_pci_device_cdev_setup(struct aim_pci_device* aimDevice);


/*! \brief Initializes the hardware parts of a device
 *
 * This function will call platform specific hardware initialization functions. \n
 * These will map bar registers and set up interrupt handling.
 * Also, interrupt based target communication channels are set up.
 * @param aimDevice the device to initialize hardware of
 * @return returns 0 on success, a negative error code otherwise
 */
int aim_pci_device_hw_init(struct aim_pci_device* aimDevice);


/*! \brief Shuts down hardware specific parts of a device
 *
 * This function will call platform specific functions that will
 * shutdown the hardware. \n
 * Bar registers will be unmapped and interrupts will be disabled.
 * @param aimDevice aimDevice device to shut down hardware for
 */
void aim_pci_device_hw_remove(struct aim_pci_device* aimDevice);


/*! \brief Enable interrupts of an AIM PCI device
 *
 * This function will call platform specific functions to enable
 * interrupt issuing of a device. \n
 * The IRQ handler function for the device will also be registered.
 * @param aimDevice the device to enable the interrupts for.
 * @return 0 on success, errno code otherwise
 */
int aim_pci_device_enable_interrupts (struct aim_pci_device* aimDevice);


/*! \brief Disables interrupts of an AIM PCI device
 *
 * This function will call platform specific functions to enable
 * interrupt issuing of a device. \n
 * The IRQ handler function for the device will also be registered.
 * @param aimDevice the device to disable the interrupts for.
 */
int aim_pci_device_disable_interrupts (struct aim_pci_device* aimDevice);


/*! \brief Checks if device has ASP
 *
 * Checks the device group of the given device to decide if it has an ASP
 * @param device the device to check
 * @return true if device has ASP, false otherwise
 */
static bool inline aim_pci_device_has_asp(struct aim_pci_device* device)
{
    BUG_ON(!device || !device->deviceData);

    switch(device->deviceData->platform)
    {
    case AI_DEVICE_AYXGNET:
    case AI_DEVICE_AYS_ASP:
    case AI_DEVICE_AMC_ASP:
    case AI_DEVICE_AYX:
    case AI_DEVICE_ZYNQMP_ASP:
        return true;

    case AI_DEVICE_AYE:
    case AI_DEVICE_AMC:
    case AI_DEVICE_AYS:
    case AI_DEVICE_ARTIX7:
    case AI_DEVICE_ARTIXUS:
        return false;

    default:
        aim_dev_error(device, "Can't decide if device has ASP. Unknown platform");
        BUG(); /* We don't know the platform so can't decide. Hence issue bug message */
    }

    return false;
}


/*! \brief Initializes a \ref struct aim_pci_device structure
 *
 * This function performs protocol and platform independent initialization
 * of an AIM PCI device. \n
 * It will just initialize the structure members and the device name.
 * @param aimDevice the AIM PCI device structure to initialize
 * @param aimData the device data to attach to the device
 * @param pciDevice the corresponding PCI device structure
 */
static void inline aim_pci_device_init(struct aim_pci_device* aimDevice, struct aim_pci_device_data* aimData, struct pci_dev* pciDevice)
{
    char newDevName[32];

    BUG_ON(!aimDevice || !pciDevice || !aimData);

    aim_ref_init(&aimDevice->referenceCount);
    mutex_init(&aimDevice->targetCmdLock);
    mutex_init(&aimDevice->dmaRequestLock);
    mutex_init(&aimDevice->zynqmp_hostio_lock);
    spin_lock_init(&aimDevice->dmaQueueLock);
    INIT_LIST_HEAD(&aimDevice->dmaQueue);
    spin_lock_init(&aimDevice->aio_queues_lock);
    aimDevice->pciDevice = pciDevice;
    aimDevice->deviceData = aimData;
    aimDevice->minor = aim_pci_get_minor();
    BUG_ON(aimDevice->minor < 0);

#ifdef AIM_AYE_DS
    /* Digital Scope Request Queues */
    spin_lock_init(&aimDevice->ScopeBufferQueueLock);
    INIT_LIST_HEAD(&aimDevice->ScopeBufferQueue);
    INIT_LIST_HEAD(&aimDevice->ScopeBufferPendingQueue);
    INIT_LIST_HEAD(&aimDevice->ScopeBufferDoneQueue);
    init_waitqueue_head(&aimDevice->ScopeBufferDoneWaitQueue);
    atomic_set( &aimDevice->ScopeBufferCanceled, 0 );
#endif


    aimDevice->hasAsp = aim_pci_device_has_asp(aimDevice);

    /* Rename the device to module name + minor mumber */
    snprintf(newDevName, sizeof(newDevName), "%s%d", AIM_DRV_NAME, aimDevice->minor);
    device_rename(&pciDevice->dev, newDevName);

    /* Set-up default netlink family for the device.
     * It uses the name of the device and will generate its own ID
     */
    snprintf(aimDevice->nl_setup.family.name, sizeof(aimDevice->nl_setup.family.name), dev_name(aim_to_dev(aimDevice)));

    dev_set_drvdata(&pciDevice->dev, aimDevice);
}


/*! \brief Create asynchronous IO queue for a device
 *
 * The caller of this function will get a reference to the created queue.
 * He has to release this reference once he does not use the queue any more
 * @param device the device to create queue for
 * @param id ID of the queue to create
 * @return the queue that has been created on success. ERR_PTR on failure
 */
extern struct aim_aio_queue* aim_pci_device_create_aio_queue(struct aim_pci_device* device, int id, struct file* fhandle);


/*! \brief Delete asynchronous IO queue of a device
 *
 * @param device the device to delete queue of
 * @param id ID of queue to delete
 * @return 0 on success, -EINVAL if queue with given ID does not exist
 */
extern int aim_pci_device_delete_aio_queue(struct aim_pci_device* device, int id);


/*! \brief Get asynchronous IO queue of device
 *
 * The caller will grab a reference count for the queue and has to release
 * it with \ref aim_aio_queue_put once he needs the queue no more
 * @param device the device to get queue of
 * @param id ID of queue to get
 * @return pointer to queue on success. ERR_PTR on failure
 */
extern struct aim_aio_queue* aim_pci_device_get_aio_queue(struct aim_pci_device* device, int id);


#endif /* AIM_PCI_DEVICE_H_ */
