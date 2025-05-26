/* SPDX-FileCopyrightText: 2017-2021 AIM GmbH <info@aim-online.com> */
/* SPDX-License-Identifier: GPL-2.0-or-later */

/*! \file aim_usb_nc.c
 *
 */


#include "aim_usb_nc.h"
#include "aim_usb_debug.h"
#include "aim_usb_com_channel.h"



/*! \def NC_SPACE_SELECT_SHIFT
 * Shift value for space select field in Netchip config out control word
 */
#define NC_SPACE_SELECT_SHIFT 4


/*! \def NC_PCI_MASTER_CMD_SELECT_SHIFT
 * Shift value for PCI command field in Netchip PCI out control word
 */
#define NC_PCI_MASTER_CMD_SELECT_SHIFT 6


/*! \def NC_PCI_CONFIGURATION_CMD
 * Selects PCI configuration space in Netchip PCI out control word
 */
#define NC_PCI_CONFIGURATION_CMD 2


/*! \def NC_PCI_MEMORY_MAPPED_CMD
 * Selects PCI configuration space in Netchip PCI out control word
 */
#define NC_PCI_MEMORY_MAPPED_CMD 0


/*! \def NC_IDSEL
 * Shift value for ID select field for PCI configuration space commands in Netchip PCI out address word
 */
#define NC_IDSEL_SHIFT 30


/*! \def NC_BYTE_ENABLE_SHIFT
 * Shift value for Byte Enable field in Netchip config/PCI out control word
 */
#define NC_BYTE_ENABLE_SHIFT 0


/*! \def NC_BYTE0_ENABLE
 * Enables retrieval of first byte in Netchip config/PCI out control word
 */
#define NC_BYTE0_ENABLE 0x1


/*! \def NC_ALL_BYTE_ENABLE
 * Enables retrieval of all bytes in Netchip config/PCI out control word
 */
#define NC_ALL_BYTE_ENABLE 0xf


/*! \enum nc_memory_space
 * Enumeration of all possible memory spaces
 * that can be specified via the space select field in Netchip config out control words
 */
enum nc_memory_space
{
    NCMS_PCI_CONFIG,        //!< NCMS_PCI_CONFIG
    NCMS_MEMORY_MAPPED,     //!< NCMS_MEMORY_MAPPED
    NCMS_8051_PROGRAM_RAM   //!< NCMS_8051_PROGRAM_RAM
};


/*! \union ncdma_control_reg
 * Bitfield that describes the control register
 * of Netchip DMA controller
 */
union ncdma_control_reg
{
    AiUInt32  value;
    struct
    {
        AiUInt32 addr_hold          : 1;
        AiUInt32 enable             : 1;
        AiUInt32 fifo_validate      : 1;
        AiUInt32 preempt            : 1;
        AiUInt32 auto_start         : 1;
        AiUInt32 reserved           : 11;
        AiUInt32 scatter_gather     : 1;
        AiUInt32 valid_bit          : 1;
        AiUInt32 valid_bit_polling  : 1;
        AiUInt32 polling_rate       : 2;
        AiUInt32 clear_count        : 1;
        AiUInt32 reserved2          : 3;
        AiUInt32 scatter_gather_irq : 1;
        AiUInt32 reserved3          : 6;
    }bit;
};



/*! \struct aim_usb_nc_cfg_out_write
 *
 * This structure comprises the URB command data
 * for single write access to Netchip configuration registers
 */
struct aim_usb_nc_cfg_out_write
{
    AiUInt8 ctrl;           /*!< control byte e.g. space select */
    AiUInt8 reserved;
    AiUInt16 addr;          /*!< Address to write to */
    AiUInt16 reserved2;
    AiUInt32 data;          /*!< Data to write */
}__attribute__((packed));


struct aim_usb_nc_cfg_out_read
{
    AiUInt8 ctrl;           /*!< control byte e.g. space select */
    AiUInt8 reserved;
    AiUInt16 addr;          /*!< Address to write to */
    AiUInt16 reserved2;
}__attribute__((packed));


/*! \struct aim_usb_nc_pci_out_write
 *
 * This structure comprises the URB command data
 * for single write access to PCI bus
 */
struct aim_usb_nc_pci_out_write
{
    AiUInt16 ctrl;          /*!< PCI control and status word */
    AiUInt32 addr;          /*!< Address to write to */
    AiUInt32 data;          /*!< Data to write */
}__attribute__((packed));


/*! \struct aim_usb_nc_pci_out_read
 *
 * This structure comprises the URB command data
 * for single read access to PCI bus
 */
struct aim_usb_nc_pci_out_read
{
    AiUInt16 ctrl;          /*!< PCI control and status word */
    AiUInt32 addr;          /*!< Address to write to */
}__attribute__((packed));




int aim_usb_ncdmac_init(struct aim_usb_ncdmac* dmac, struct aim_usb_ncc* ncc)
{
    BUG_ON(!dmac || !ncc);

    dmac->ncc = ncc;

    mutex_init(&dmac->lock);

    return 0;
}


void aim_usb_ncdmac_free(struct aim_usb_ncdmac* ncdmac)
{
    ncdmac->ncc = NULL;
}


int aim_usb_ncdmac_write(struct aim_usb_ncdmac* ncdmac, AiUInt32 address, void* data, size_t len)
{
    int err;
    const AiUInt16 ep_status_reg_addr = 0x32C;
    const AiUInt16 dma_control_reg_addr = 0x180;
    const AiUInt16 dma_addr_reg_addr = 0x194;
    const AiUInt16 dma_count_reg_addr = 0x190;
    AiUInt32 dma_count;
    union ncdma_control_reg control_reg;

    BUG_ON(!ncdmac || !ncdmac->ncc || !data);

    mutex_lock(&ncdmac->lock);

    err = 0;

    do
    {
        dma_count = len | 0x90000000;

        // Flush FIFO and clear Interrupts
        err = aim_usb_ncc_reg_write(ncdmac->ncc, ep_status_reg_addr, 0x0000023F);
        if(err)
        {
            break;
        }

        // Setup PCI Address on DMA Channel
        err = aim_usb_ncc_reg_write(ncdmac->ncc, dma_addr_reg_addr, address);
        if(err)
        {
            break;
        }

        // Setup Byte Count
        err = aim_usb_ncc_reg_write(ncdmac->ncc, dma_count_reg_addr, dma_count);
        if(err)
        {
            break;
        }

        /* Setup control register */
        control_reg.value = 0;
        control_reg.bit.auto_start = 1;
        control_reg.bit.enable = 1;
        control_reg.bit.clear_count = 1;
        control_reg.bit.fifo_validate = 1;

        err = aim_usb_ncc_reg_write(ncdmac->ncc, dma_control_reg_addr, control_reg.value);
        if(err)
        {
            break;
        }

        err = aim_usb_ncc_fifo_write(ncdmac->ncc, data, len);
        if(err)
        {
            break;
        }

    }while(0);

    mutex_unlock(&ncdmac->lock);

    return err;
}


int aim_usb_ncdmac_read(struct aim_usb_ncdmac* ncdmac, AiUInt32 address, void* data, size_t len)
{
    int err;
    const AiUInt16 ep_status_reg_addr = 0x34C;
    const AiUInt16 dma_control_reg_addr = 0x1A0;
    const AiUInt16 dma_addr_reg_addr = 0x1B4;
    const AiUInt16 dma_count_reg_addr = 0x1B0;
    const AiUInt16 dma_status_reg_addr = 0x1A4;
    AiUInt32 dma_count;
    union ncdma_control_reg control_reg;

    BUG_ON(!ncdmac || !ncdmac->ncc || !data);

    mutex_lock(&ncdmac->lock);

    err = 0;

    do
    {
        dma_count = len | 0xD0000000;

        // Flush FIFO and clear Interrupts
        err = aim_usb_ncc_reg_write(ncdmac->ncc, ep_status_reg_addr, 0x0000023F);
        if( err )
        {
            break;
        }

        // Setup PCI Address on DMA Channel
        err = aim_usb_ncc_reg_write(ncdmac->ncc, dma_addr_reg_addr, address);
        if( err )
        {
            break;
        }

        // Setup Byte Count
        err = aim_usb_ncc_reg_write(ncdmac->ncc, dma_count_reg_addr, dma_count);
        if( err )
        {
            break;
        }

        control_reg.value = 0;
        control_reg.bit.auto_start = 1;
        control_reg.bit.enable = 1;
        control_reg.bit.clear_count = 1;
        control_reg.bit.fifo_validate = 1;

        err = aim_usb_ncc_reg_write(ncdmac->ncc, dma_control_reg_addr, control_reg.value);
        if( err )
        {
            break;
        }

        err = aim_usb_ncc_reg_write(ncdmac->ncc, dma_status_reg_addr, 0x03000001);
        if( err )
        {
            break;
        }

        err = aim_usb_ncc_fifo_read(ncdmac->ncc, data, len);
        if( err )
        {
            aim_error("Failed to read DMA FIFO");
            break;
        }

    } while( 0 );

    mutex_unlock(&ncdmac->lock);

    return err;
}


int aim_usb_ncc_init(struct aim_usb_ncc* ncc, struct aim_usb_com_channel* config_channel,
                     struct aim_usb_com_channel* pci_channel, struct aim_usb_com_channel* fifo_channel)
{
    int err;

    BUG_ON(!ncc || !config_channel || !pci_channel || !fifo_channel);

    aim_usb_intf_dbg(config_channel->intf, "Initializing Netchip controller");

    ncc->config_channel = config_channel;
    ncc->pci_channel = pci_channel;
    ncc->fifo_channel = fifo_channel;
    ncc->buffer = NULL;
    err = 0;

    do
    {
        /* Allocate URB buffer for config/PCI endpoint transfers
         * sizeof(struct aim_usb_nc_cfg_out_write) will suffice for all types of transfers
         */
        ncc->buffer = kmalloc(sizeof(struct aim_usb_nc_cfg_out_write), GFP_KERNEL);
        if(!ncc->buffer)
        {
            aim_usb_intf_err(config_channel->intf, "Failed to initialize Netchip controller");
            err = -ENOMEM;
            break;
        }

        mutex_init(&ncc->lock);
    }while(0);

    if(err && ncc->buffer)
    {
        kfree(ncc->buffer);
        ncc->buffer = NULL;
    }

    return err;
}


void aim_usb_ncc_free(struct aim_usb_ncc* ncc)
{
    if(ncc->buffer)
    {
        kfree(ncc->buffer);
        ncc->buffer = NULL;
    }
}


int aim_usb_ncc_pci_config_write(struct aim_usb_ncc* ncc, AiUInt32 address, AiUInt32 value)
{
    int err;
    ssize_t response;
    struct aim_usb_nc_pci_out_write* write_command;

    BUG_ON(!ncc || !ncc->pci_channel);

    mutex_lock(&ncc->lock);

    err = 0;
    write_command = ncc->buffer;
    do
    {
        write_command->ctrl = cpu_to_le16((NC_PCI_CONFIGURATION_CMD << NC_PCI_MASTER_CMD_SELECT_SHIFT) |
                                            (NC_ALL_BYTE_ENABLE << NC_BYTE_ENABLE_SHIFT));
        write_command->addr = cpu_to_le32((address | (1 << NC_IDSEL_SHIFT)));
        write_command->data = cpu_to_le32(value);

        response = aim_usb_com_channel_issue_cmd(ncc->pci_channel, write_command, sizeof(*write_command), NULL, 0);
        if(response < 0)
        {
            err = response;
            break;
        }
    }while(0);

    mutex_unlock(&ncc->lock);

    return err;
}


int aim_usb_ncc_pci_config_read(struct aim_usb_ncc* ncc, AiUInt32 address, AiUInt32* value)
{
    int err;
    ssize_t response;
    struct aim_usb_nc_pci_out_read* read_command;

    BUG_ON(!ncc || !ncc->pci_channel || !value);

    mutex_lock(&ncc->lock);

    err = 0;
    response = 0;
    read_command = ncc->buffer;
    do
    {
        read_command->ctrl = cpu_to_le16((NC_PCI_CONFIGURATION_CMD << NC_PCI_MASTER_CMD_SELECT_SHIFT)
                                            | (NC_ALL_BYTE_ENABLE << NC_BYTE_ENABLE_SHIFT));
        read_command->addr = cpu_to_le32((address | (1 << NC_IDSEL_SHIFT)));

        response = aim_usb_com_channel_issue_cmd(ncc->pci_channel, read_command, sizeof(*read_command), ncc->buffer, sizeof(AiUInt32));
        if( response < 0 )
        {
            err = response;
            break;
        }

        if(response != sizeof(AiUInt32))
        {
            err = -EAGAIN;
        }

        *value = le32_to_cpu(((AiUInt32*) ncc->buffer)[0]);

    } while( 0 );

    mutex_unlock(&ncc->lock);

    return err;
}


int aim_usb_ncc_pci_read(struct aim_usb_ncc* ncc, AiUInt32 address, AiUInt32* value)
{
    int err;
    ssize_t response;
    struct aim_usb_nc_pci_out_read* read_command;

    BUG_ON(!ncc || !ncc->pci_channel || !value);

    mutex_lock(&ncc->lock);

    err = 0;
    response = 0;
    read_command = ncc->buffer;
    do
    {
        read_command->ctrl = cpu_to_le16((NC_PCI_MEMORY_MAPPED_CMD << NC_PCI_MASTER_CMD_SELECT_SHIFT)
                                               | (NC_ALL_BYTE_ENABLE << NC_BYTE_ENABLE_SHIFT));
        read_command->addr = cpu_to_le32(address);

        response = aim_usb_com_channel_issue_cmd(ncc->pci_channel, read_command, sizeof(*read_command),
                                                 ncc->buffer, sizeof(AiUInt32));
        if( response < 0 )
        {
            err = response;
            break;
        }

        if(response != sizeof(AiUInt32))
        {
            err = -EAGAIN;
        }

        *value = le32_to_cpu(((AiUInt32*) ncc->buffer)[0]);

    } while( 0 );

    mutex_unlock(&ncc->lock);

    return err;
}


int aim_usb_ncc_pci_write(struct aim_usb_ncc* ncc, AiUInt32 address, AiUInt32 value)
{
    int err;
    ssize_t response;
    struct aim_usb_nc_pci_out_write* write_command;

    BUG_ON(!ncc || !ncc->pci_channel);

    mutex_lock(&ncc->lock);

    err = 0;
    write_command = ncc->buffer;
    do
    {
        write_command->ctrl = cpu_to_le16((NC_PCI_MEMORY_MAPPED_CMD << NC_PCI_MASTER_CMD_SELECT_SHIFT) |
                                                (NC_ALL_BYTE_ENABLE << NC_BYTE_ENABLE_SHIFT));
        write_command->addr = cpu_to_le32(address);
        write_command->data = cpu_to_le32(value);

        response = aim_usb_com_channel_issue_cmd(ncc->pci_channel, write_command, sizeof(*write_command), NULL, 0);
        if(response < 0)
        {
            err = response;
            break;
        }
    }while(0);

    mutex_unlock(&ncc->lock);

    return err;
}


int aim_usb_ncc_reg_write(struct aim_usb_ncc* ncc, AiUInt16 address, AiUInt32 value)
{
    int err;
    struct aim_usb_nc_cfg_out_write* write_command;
    ssize_t response;

    BUG_ON(!ncc || !ncc->config_channel);

    mutex_lock(&ncc->lock);

    err = 0;
    write_command = ncc->buffer;
    do
    {
        write_command->ctrl = (NCMS_MEMORY_MAPPED << NC_SPACE_SELECT_SHIFT) | (NC_ALL_BYTE_ENABLE << NC_BYTE_ENABLE_SHIFT);
        write_command->addr = cpu_to_le16(address);
        write_command->data = cpu_to_le32(value);

        response = aim_usb_com_channel_issue_cmd(ncc->config_channel, write_command, sizeof(*write_command), NULL, 0);
        if(response < 0)
        {
            err = response;
            break;
        }

    }while(0);

    mutex_unlock(&ncc->lock);

    return err;
}


int aim_usb_ncc_reg_read(struct aim_usb_ncc* ncc, AiUInt16 address, AiUInt32* value)
{
    int err;
    struct aim_usb_nc_cfg_out_read* read_command;
    ssize_t response;

    BUG_ON(!ncc || !ncc->config_channel);

    mutex_lock(&ncc->lock);

    err = 0;
    read_command = ncc->buffer;
    do
    {
        read_command->ctrl = (NCMS_MEMORY_MAPPED << NC_SPACE_SELECT_SHIFT) | (NC_ALL_BYTE_ENABLE << NC_BYTE_ENABLE_SHIFT);
        read_command->addr = cpu_to_le16(address);

        response = aim_usb_com_channel_issue_cmd(ncc->config_channel, read_command, sizeof(*read_command),
                                                 ncc->buffer, sizeof(AiUInt32));
        if(response < 0)
        {
            err = response;
            break;
        }

        if(response != sizeof(AiUInt32))
        {
            err = -EAGAIN;
            break;
        }

        *value = le32_to_cpu(((AiUInt32*) ncc->buffer)[0]);

    }while(0);

    mutex_unlock(&ncc->lock);

    return err;
}


int aim_usb_ncc_fifo_write(struct aim_usb_ncc* ncc, void* data, size_t len)
{
    int err;

    BUG_ON(!ncc || !ncc->fifo_channel);

    mutex_lock(&ncc->lock);

    err = 0;
    do
    {
        err = aim_usb_com_channel_issue_cmd(ncc->fifo_channel, data, len, NULL, 0);
        if(err < 0)
        {
            break;
        }

    }while(0);

    mutex_unlock(&ncc->lock);

    return err;
}


int aim_usb_ncc_fifo_read(struct aim_usb_ncc* ncc, void* data, size_t len)
{
    int err;
    ssize_t received;

    BUG_ON(!ncc || !ncc->fifo_channel);

    mutex_lock(&ncc->lock);
    aim_usb_com_channel_lock(ncc->fifo_channel);

    err = 0;
    do
    {
        received = aim_usb_com_channel_receive(ncc->fifo_channel, data, len);
        if(received < 0)
        {
            err = received;
            break;
        }

        if(received != len)
        {
            err = -EIO;
            break;
        }

    }while(0);

    aim_usb_com_channel_unlock(ncc->fifo_channel);
    mutex_unlock(&ncc->lock);

    return err;
}







