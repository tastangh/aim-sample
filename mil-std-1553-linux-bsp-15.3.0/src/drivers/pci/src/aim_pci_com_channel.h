/* SPDX-FileCopyrightText: 2014-2021 AIM GmbH <info@aim-online.com> */
/* SPDX-License-Identifier: GPL-2.0-or-later */

/*! \file aim_pci_com_channel.h
 *
 *  This file contains declarations for a new, interrupt based
 *  host to target communication channels over PCI bus
 *
 */

#ifndef AIM_PCI_COM_CHANNEL_H_
#define AIM_PCI_COM_CHANNEL_H_

#define AIM_PCI_UNKNOWN_ACK_SIZE 0

#include <linux/types.h>
#include <linux/mutex.h>
#include <linux/completion.h>
#include "aim_pci_device.h"




/*! \struct aim_pci_com_channel
 * This structure represents a specific channel
 * for interrupt based communication between host and target device
 * over PCI
 *
 */
struct aim_pci_com_channel
{
    __u8 id;                                /*!< The ID of the channel. Must correspond to the bit in the interrupt event register */
    void __iomem *commandBase;              /*!< pointer to data area on target where command data will be written to */
    size_t commandSize;                     /*!< size of command data area in bytes */
    void __iomem *responseBase;             /*!< pointer to data area on target where target writes response data to */
    size_t responseSize;                    /*!< size of response data area in bytes */
    __u32 __iomem *signalRegister;          /*!< pointer to event signal register in IO memory. Is used for signaling start of command to target */
    struct mutex lock;                      /*!< mutex for mutual access to this channel */
    struct completion commandCompletion;    /*!< completion used to wait for completion of commands. Must be set asynchronously by ISR or soft-IRQ */
    struct aim_pci_device* aimDevice;       /*!< The PCI device this channel belongs to */
};


/*! \struct aim_pci_com_channel_data
 *
 * This structure can be used to register host to target
 * communication channels for a specific device
 */
struct aim_pci_com_channel_data
{
    __u8 id;                     /*!< The ID of the channel. Must correspond to the bit in the interrupt event register */
    __u8 resourceID;             /*!< ID of the PCI Bar register that holds the command and data area */
    __u32 commandOffset;         /*!< Offset in bytes to start of command data area in PCI bar register specified by \ref resourceID */
    size_t commandSize;          /*!< Size of command data area in bytes */
    __u32 responseOffset;        /*!< Offset in bytes to start of response data area in PCI bar register specified by \ref resourceID */
    size_t responseSize;         /*!< Size of response data area in bytes */
    __u32 signalRegisterOffset;  /*!< Offset in bytes to register in IO memory that is used for signaling events to target device */
};

/*! \brief Initializes a PCI communication channel structure
 *
 * @param channel the communication channel structure to initialize
 * @param id the ID of the channel to set
 */
static void inline aim_pci_com_channel_init(struct aim_pci_com_channel* channel, struct aim_pci_device* device, __u8 id)
{
    memset(channel,0,sizeof(*channel));
    channel->id = id;
    channel->aimDevice = device;
    mutex_init(&channel->lock);
    init_completion(&channel->commandCompletion);
}


/*! \brief Sets up the hardware resources of a communication channel
 *
 * This function will map necessary PCI bar registers of a communication channel
 * @param channel the communication channel to set up
 * @param channelData corresponding channel data
 * @param aimDevice the device the channel shall be set up for
 * @return returns 0 on success, a negative error code otherwise
 */
int aim_pci_com_channel_hw_setup(struct aim_pci_com_channel* channel, struct aim_pci_com_channel_data* channelData,
                                 struct aim_pci_device* aimDevice);


/*! \brief Releases the hardware resources of a communication channel
 *
 * It will unmap the PCI Bar registers used by this communication channel
 * @param channel the channel to release the resources of
 */
void aim_pci_com_channel_hw_release(struct aim_pci_com_channel* channel);


/*! \brief Issues a command on a PCI communication channel
 *
 * @param channel the PCI communication channel to issue command on
 * @param command pointer to buffer that holds command data
 * @param commandSize size of command in bytes
 * @param response pointer to buffer that will receive response
 * @param responseSize size of expected response in bytes
 * @return returns 0 on success, a negative error code otherwise
 */
int aim_pci_com_channel_issue_cmd( struct aim_pci_com_channel* channel, void* command, size_t commandSize, void* response, size_t );


long aim_pci_read_ack_size(const struct aim_pci_com_channel* channel, void* response);

#endif /* AIM_PCI_COM_CHANNEL_H_ */
