/* SPDX-FileCopyrightText: 2013-2021 AIM GmbH <info@aim-online.com> */
/* SPDX-License-Identifier: MIT OR GPL-2.0-or-later */

/*! \file aim_ioctl_interface.h
 *
 *  This header file contains the protocol independent
 *  IOCTL interface for the AIM PCI driver
 *
 */

#ifndef AIM_IOCTL_INTERFACE_H_
#define AIM_IOCTL_INTERFACE_H_


#include <linux/fs.h>
#include "Ai_cdef.h"
#include "Ai_def.h"


#ifndef __KERNEL__

#define __user

#endif





/*! \def MAX_DMA_BUFFERS_USER
 * Maximum number of userspace buffers that can be used for direct target commands
 */
#define MAX_DMA_BUFFERS_USER 16


/*! \def MAX_DRIVER_VERSION_LENGTH_USER
 * Maximum length of version string that can be obtained with
 * IOCTL for getting driver version
 */
#define MAX_DRIVER_VERSION_LENGTH_USER  64




/*! \struct aim_target_command
 *
 * This structure is used as input parameter
 * for the target command IOCTL
 */
struct aim_target_command
{
    size_t cmdSize;             /*!< total size of the target command in bytes */
    void __user*  command;      /*!< user space pointer to command stream */
    size_t expectedAckSize;     /*!< Expected acknowledge size in bytes */
    void __user*  acknowledge;  /*!< user space buffer that will receive the acknowledge. Must be at least \ref expectedAckSize bytes long */
};


/*! \enum aim_dma_direction
 * Enumeration of possible DMA directions for user space buffers
 * during direct target commands
 */
enum aim_dma_direction
{
    AIM_DMA_FROM_DEVICE = 0, /*!< Target writes to user space buffer */
    AIM_DMA_TO_DEVICE,       /*!< Target reads from user space buffer */
    AIM_DMA_BOTH,            /*!< Target reads and writes the user space buffer */
};


/*! \struct aim_dma_buffer_user
 * User space buffer that can be used for direct target commands.
 * Buffer is read/written by target using DMA
 */
struct aim_dma_buffer_user
{
    void __user* base;                  /*!< virtual base address of user space buffer */
    size_t size;                        /*!< size of user space buffer in bytes */
    enum aim_dma_direction direction;   /*!< DMA direction the user space buffer is used for */
};

/*! \struct aim_target_command_direct
 * IOCTL parameters for the direct target command IOCTL
 */
struct aim_target_command_direct
{
    struct aim_target_command commandParams; /*!< the parameters of the command as used in buffered target commands */
    size_t numDmaBuffers;                    /*!< number of user space DMA buffers for the command */
    struct aim_dma_buffer_user dmaBuffers[MAX_DMA_BUFFERS_USER];    /*!< fixed array of DMA user space buffers */
};

struct aim_target_command_direct_dynamic
{
    struct aim_target_command commandParams; /*!< the parameters of the command as used in buffered target commands */
    size_t numDmaBuffers;                    /*!< number of user space DMA buffers for the command */
    struct aim_dma_buffer_user dmaBuffers[];    /*!< array of DMA user space buffers */
};
/*! \struct aim_driver_version_user
 *  This structure is used as output parameter for the get driver version
 *  IOCTL.
 */
struct aim_driver_version_user
{
    AiUInt32 major;                                     /*!< Major version of the driver */
    AiUInt32 minor;                                     /*!< Minor version of the driver */
    AiUInt32 patch;                                     /*!< Patch version of the driver */
    AiUInt32 build;                                     /*!< Build number of the driver */
    char full_version[MAX_DRIVER_VERSION_LENGTH_USER];  /*!< Full version string of the driver */
};


/*! \struct aim_pci_com_chn_cmd
 * This structure is used as input/output parameter for the \ref AIM_IOCTL_PCI_COM_CHN_CMD IOCTL
 */
struct aim_pci_com_chn_cmd
{
    AiUInt8 id;                                 /*!< ID of the PCI communication channel to use */
    struct aim_target_command commandParams;    /*!< Parameters of the command/response sequence. See \ref struct aim_target_command. */
};


/*! \struct aim_pci_com_chn_cmd_dma
 * This structure is used for input/output parameter for the \ref AIM_IOCTL_PCI_COM_CHN_CMD_DMA IOCTL
 */
struct aim_pci_com_chn_cmd_dma
{
    struct aim_pci_com_chn_cmd channelCommand;  /*!< Normal PCI communication channel command */
    struct aim_dma_buffer_user dmaBuffer;       /*!< user space DMA buffer that will be appended to command */
};


/*! \struct aim_get_mem_size
 * This structure is used as input/output parameter for the \ref AIM_IOCTL_GET_MEM_SIZE ioctl
 */
struct aim_get_mem_size
{
    TY_E_MEM_TYPE type; /*!< memory type to get size of */
    AiUInt64 size;      /*!< requested (mapped) size of memory bar is returned here */
};


/*! \struct aim_aio_op
 * This structure represents one specific asynchronous I/O operation.
 * It is used as parameter for the AIM_IOCTL_AIO_START ioctl.
 */
struct aim_aio_op
{
    __u64 id;                               /*!< Unique Identifier of operation. Will be set by driver after operation start */
    struct aim_dma_buffer_user buffer;      /*!< Buffer that will be used for the asynchronous IO operation. It is DMA accessible */
    void __user* attachment;                /*!< Buffer that contains user/protocol specific data for the operation */
    size_t attachment_size;                 /*!< size of attachment data in bytes */
    __u32 flags;                            /*!< Flags for future use */
};


/*! \struct aim_aio_start
 * This structure is used as parameter for the AIM_IOCTL_AIO_START ioctl.
 * It comprises a set of asynchronous I/O operations that will be submitted on a specific queue.
 */
struct aim_aio_start
{
    int queue_id;                           /*!< ID of asynchronous I/O queue to use */
    int num_ops;                            /*!< number of asynchronous I/O operations to start */
    struct aim_aio_op __user* __user* ops;  /*!< array of ‘num_ops’ pointers to struct aim_aio_op instances to start */
};


/*!\enum  aim_aio_op_state
 *
 * Enumeration of all possible states for asynchronous IO operations
 */
enum aim_aio_op_state
{
    AIO_OP_NOT_PROCESSED,   //!< Operation processing has not begun
    AIO_OP_PENDING,         //!< Operation is just being processed
    AIO_OP_COMPLETED,       //!< Operation processing has been successfully completed
    AIO_OP_CANCELLED,       //!< Operation has been cancelled
    AIO_OP_FAILED           //!< Operation processing has failed
};


/*! \struct aim_aio_event
 *
 * This structure is used for signaling of completed asynchronous IO operations.
 * It is used in the \ref AIM_IOCTL_AIO_GET_EVENTS ioctl.
 * For each completed asynchronous IO operation one event is returned.
 */
struct aim_aio_event
{
    __u64 id;                       /*!< ID of the asynchronous IO operation that signaled the event */
    enum aim_aio_op_state state;    /*!< State of the asynchronous IO operation */
    size_t bytes_transferred;       /*!< Number of bytes that have been transferred during the operation */
    void __user* attachment;        /*!< User space buffer for additional, protocol specific data about the IO operation */
    size_t attachment_size;         /*!< Size of attachment buffer in bytes. After ioctl returned, actual number of bytes in buffer */
    __u32 flags;                    /*!< Flags for future use */
};


/*! \struct aim_aio_get_events
 *
 * Parameter structure for the \ref AIM_IOCTL_AIO_GET_EVENTS ioctl.
 * Holds a set of \ref struct aim_aio_event objects to be signaled about completed
 * asynchronous IO operations
 */
struct aim_aio_get_events
{
    int queue_id;                           /*!< ID of asynchronous I/O queue to get events from */
    int max_events;                         /*!< Maximum number of events to get */
    struct aim_aio_event __user* events;    /*!< array of ‘max_events’ struct aim_aio_event instances where events will be stored */
    int timeout_ms;                         /*!< maximum time to wait for any event in milliseconds. 0 for just peeking for
                                                 events, without blocking */
};


/*! \struct aim_aio_cancel
 *
 * Parameter structure for the \ref AIM_IOCTL_AIO_CANCEL ioctl
 */
struct aim_aio_cancel
{
    int queue_id;       /*!< ID of asynchronous I/O queue to cancel operation of*/
    __u64 aio_op_id;    /*!< ID of AIO operation to cancel */
};


/*! \struct aim_scope_buffer */
struct aim_scope_buffer
{
    __u32 eType;                /*!< \ref API_SCOPE_BUFFER_TYPE enum which gives type of buffer */
    __u32 ulSize;               /*!< size of buffer in bytes */
    __u64 ullVirtualAddress;    /*!< virtual address of user-space buffer */
};


/*! \def AIM_IOCTL_BASE
 *
 * This is the base code for all IOCTL calls.
 * Each protocol should provide its own code here
 */
#ifdef AIM_FC
    #define AIM_IOCTL_BASE                     0x1
#elif defined AIM_FDX
    #define AIM_IOCTL_BASE                     0x2
#elif defined AIM_CIV
    #define AIM_IOCTL_BASE                     0x3
#elif defined AIM_MIL
    #define AIM_IOCTL_BASE                     0x4
#elif defined AIM_SERIAL
    #define AIM_IOCTL_BASE                     0x5
#endif



/*! \def AIM_IOCTL_TARGET_COMMAND
 * IOCTL for issuing target commands to a board
 */
#define AIM_IOCTL_TARGET_COMMAND           _IOWR(AIM_IOCTL_BASE,  1, struct aim_target_command)


/*! \def AIM_IOCTL_TARGET_COMMAND_DIRECT
 * IOCTL for direct target commands using user space DMA buffers
 */
#define AIM_IOCTL_TARGET_COMMAND_DIRECT    _IOWR(AIM_IOCTL_BASE,  2, struct aim_target_command_direct)


/*! \def AIM_IOCTL_GET_DRV_VERS
 * IOCTL for reading the version of the driver
 */
#define AIM_IOCTL_GET_DRV_VERS             _IOR(AIM_IOCTL_BASE,   3, struct aim_driver_version_user)


/*! \def AIM_IOCTL_PCI_COM_CHN_CMD
 * IOCTL for command/response sequences over a PCI based, interrupt driven
 * communication channel to the target device.
 */
#define AIM_IOCTL_PCI_COM_CHN_CMD          _IOWR(AIM_IOCTL_BASE, 4, struct aim_pci_com_chn_cmd)


/*! \def AIM_IOCTL_PCI_COM_CHN_CMD_DMA
 * IOCTL for command response sequences over a PCI based, interrupt driven
 * communication channel to the target device.
 * The user space DMA buffer will be split up into a scatter/gather DMA list,
 * and appended to the origin command data.
 */
#define AIM_IOCTL_PCI_COM_CHN_CMD_DMA       _IOWR(AIM_IOCTL_BASE, 5, struct aim_pci_com_chn_cmd_dma)


/*! \def AIM_IOCTL_DEACTIVATE_IRQ
 * IOCTL for dynamically deactivating the IRQ handler and interrupts for a specific device. \n
 * IRQ handling is always enabled by default when device is probed \n
 * and this IOCTL should NEVER be called during normal device operation. \n
 * It is for example needed by PMC Modules for PROM and TCP Update \n
 * as this must not be disturbed by register access during IRQ handling. \n
 * The IOCTL parameter flag can be used to activate IRQ handling again. \n
 * When set to 'true', IRQ will be deactivated, setting to 'false' will activate them.
 */
#define AIM_IOCTL_DEACTIVATE_IRQ            _IOWR(AIM_IOCTL_BASE, 6, AiBoolean)


/*! \def AIM_IOCTL_GENERIC_BOARD_INFO
 * IOCTL for getting protocol independent information about a device.
 */
#define AIM_IOCTL_GENERIC_BOARD_INFO        _IOWR(AIM_IOCTL_BASE, 7, TY_DRIVER_INFO)


/*! \def AIM_IOCTL_GET_MEM_SIZE
    IoControl code for getting the size that is usable for read/write of a specific memory type */
#define AIM_IOCTL_GET_MEM_SIZE              _IOR(AIM_IOCTL_BASE, 8, struct aim_get_mem_size)


/*! \def AIM_IOCTL_AIO_QUEUE_CREATE
 * IoControl code for creating asynchronous I/O queues. Takes the ID of the queue to create as parameter
 */
#define AIM_IOCTL_AIO_QUEUE_CREATE          _IOW(AIM_IOCTL_BASE, 9, int)


/*! \def AIM_IOCTL_AIO_QUEUE_DELETE
 * IoControl code for deleting asynchronous IO queues. Takes the ID of the queue to delete
 */
#define AIM_IOCTL_AIO_QUEUE_DELETE          _IOW(AIM_IOCTL_BASE, 10, int)


/*! \def AIM_IOCTL_AIO_START
 * IoControl code for starting asynchronous I/O operations.
 * Takes an object of \ref struct aim_aio_start.
 * Returns number of actually started operations
 */
#define AIM_IOCTL_AIO_START                 _IOWR(AIM_IOCTL_BASE, 11, struct aim_aio_start)


/*! \def AIM_IOCTL_AIO_GET_EVENTS
 * IoControl code for getting notifications about completed asynchronous IO operation in a specific queue.
 * Takes an object of \ref struct aim_aio_get_events as parameter.
 * Returns number of actually completed events if any asynchronous IOwere completed.
 * Returns -ETIME if no operation has been completed during specified time.
 * Returns other negative errno code on failure
 */
#define AIM_IOCTL_AIO_GET_EVENTS            _IOR(AIM_IOCTL_BASE, 12, struct aim_aio_get_events)


/*! \def AIM_IOCTL_AIO_CANCEL
 * IoControl code for canceling specific asynchronous IO operations
 * Takes an object of \ref struct aim_aio_cancel as input parameter
 * Returns 0 on success, a negative errno code on failure
 */
#define AIM_IOCTL_AIO_CANCEL                _IOW(AIM_IOCTL_BASE, 13, struct aim_aio_cancel)

/*! \def AIM_IOCTL_SCOPE_BUFFERS_PROVIDE
 * IoControl code to provide digital scope buffers.
 * Returns 0 on success, a negative errno code on failure
 */
#define AIM_IOCTL_SCOPE_BUFFERS_PROVIDE                _IOR(AIM_IOCTL_BASE, 14, struct aim_scope_buffer )

/*! \def AIM_IOCTL_RELEASE_SCOPE_BUFFERS
 * IoControl code to release digital scope buffers.
 * Returns 0 on success, a negative errno code on failure
 */
#define AIM_IOCTL_SCOPE_BUFFERS_RELEASE                _IO(AIM_IOCTL_BASE, 15)

/*! \def AIM_IOCTL_SCOPE_BUFFERS_WAIT
 * IoControl code to wait for digital scope buffers.
 * Returns 0 on success, a negative errno code on failure
 */
#define AIM_IOCTL_SCOPE_BUFFERS_WAIT                   _IO(AIM_IOCTL_BASE, 16)

/*! \def AIM_IOCTL_SCOPE_BUFFERS_SYNC
 * IoControl code to synchronize digital scope buffers.
 * Returns 0 on success, a negative errno code on failure
 */
#define AIM_IOCTL_SCOPE_BUFFERS_SYNC                  _IOR(AIM_IOCTL_BASE, 17, unsigned int)

#define AIM_IOCTL_TARGET_COMMAND_DIRECT_DYNAMIC      _IOWR(AIM_IOCTL_BASE, 18, struct aim_target_command_direct_dynamic)
#endif /* AIM_IOCTL_INTERFACE_H_ */
