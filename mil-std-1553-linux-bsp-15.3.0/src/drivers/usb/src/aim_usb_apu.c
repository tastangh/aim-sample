/* SPDX-FileCopyrightText: 2017-2021 AIM GmbH <info@aim-online.com> */
/* SPDX-License-Identifier: GPL-2.0-or-later */

/*! \file aim_usb_apu.c
 *
 */


#include <linux/firmware.h>
#include "aim_usb_apu.h"




/*! \def APU_FIFO_OUT_ENDPOINT_NUM
 * Number of FIFO out endpoint on APU
 */
#define APU_FIFO_OUT_ENDPOINT_NUM 1


/*! \def APU_FIFO_IN_ENDPOINT_NUM
 * Number of FIFO in endpoint on APU
 */
#define APU_FIFO_IN_ENDPOINT_NUM 2


/*! \def APU_CONFIG_ENDPOINT_NUM
 * Number of config endpoint on APU
 */
#define APU_CONFIG_ENDPOINT_NUM 13


/*! \def APU_PCI_ENDPOINT_NUM
 * Number of PCI access endpoint on APU
 */
#define APU_PCI_ENDPOINT_NUM 14


/*! \def APU_INTERRUPT_ENDPOINT_NUM
 * Number of interrupt endpoint on APU
 */
#define APU_INTERRUPT_ENDPOINT_NUM 15


/*! \def APU_CONFIG_CHANNEL_ID
 * ID of configuration channel on APU.
 * Comprises the Config in and out endpoints
 */
#define APU_CONFIG_CHANNEL_ID 0


/*! \def APU_PCI_CHANNEL_ID
 * ID of PCI access channel on APU
 * Comprises the PCI in and out endpoints
 */
#define APU_PCI_CHANNEL_ID 1


/*! \def APU_FIFO_CHANNEL_ID
 * ID of FIFO channel on APU
 * Comprises the FIFO in and out endpoints
 */
#define APU_FIFO_CHANNEL_ID 2


/*! \def APU_GLOBAL_MEM_BAR_ID
 * ID of global memory PCI bar on APU devices
 */
#define APU_GLOBAL_MEM_BAR_ID 0


/*! \def APU_IO_MEM_BAR_ID
 * ID of IO memory bar on APU devices
 */
#define APU_IO_MEM_BAR_ID 1


/*! \def PCI_CMD_STATUS_ADDRESS
 * Address of PCI Command/Stats Register in PCI Config Space
 */
#define PCI_CMD_STATUS_ADDRESS 0x0004


/*! \def PCI_BAR_0_ADDRESS
 * Address of Bar 0 Register in PCI Config Space
 */
#define PCI_BAR_0_ADDRESS 0x0010


/*! \def PCI_BAR_1_ADDRESS 0x0014
 * Address of Bar 1 Register in PCI Config Space
 */
#define PCI_BAR_1_ADDRESS 0x0014


/*! \def PCI_BAR_2_ADDRESS
 * Address of Bar 2 Register in PCI Config Space
 */
#define PCI_BAR_2_ADDRESS 0x0018


/*! \def APU_RESET_REG_ADDR
 * 32-Byte offset of reset register in I/O memory
 */
#define APU_RESET_REG_OFFSET 0


/*! \def APU_TCP_PORT_REG_OFFSET
 * 32-Byte offset of TCP port read/write register in I/O memory
 */
#define APU_TCP_PORT_REG_OFFSET 0x40


/*! \def APU_TCP_VERSION_PORT
 * TCP port to read the TCP version
 */
#define APU_TCP_VERSION_PORT 0x1f


/*! \def APU_TCP_EEPROM_STATUS
 * EEPROM status port
 */
#define APU_TCP_EEPROM_STATUS 0x5


/*! \def APU_TCP_EEPROM_CTRL
 * EEPROM control port
 */
#define APU_TCP_EEPROM_CTRL 0x6


/*! \def APU_TCP_EEPROM_WRENA
 * Bit in \ref APU_TCP_EEPROM_CTRL that write enables EEPROM.
 */
#define APU_TCP_EEPROM_WRENA (1 << 0)


/*! \def APU_TCP_EEPROM_BUSY
 * Bit in \ref APU_TCP_EEPROM_STATUS that indicates if EEPROM write is in progress
 */
#define APU_TCP_EEPROM_BUSY (1 << 1)


/*! \def APU_TCP_ADDRLO
 * Port to set EEPROM address low
 */
#define APU_TCP_ADDRLO 0x8


/*! \def APU_TCP_ADDRHI
 * Port to set EEPROM address high
 */
#define APU_TCP_ADDRHI 0x7


/*! \def APU_TCP_EEPROM_READ
 * TCP Port to read EEPROM data at current address
 */
#define APU_TCP_EEPROM_READ 0xa


/*! \def APU_TCP_EEPROM_WRITE
 *  TCP Port to write EEPROM data at current address
 */
#define APU_TCP_EEPROM_WRITE 0x9


/*! \def NVR_MAGIC_ADDR
 * Address of magic value in NOVRAM
 */
#define NVR_MAGIC_ADDR 0x000


/*! \def NVR_MAGIC2_ADDR
 * Address of second magic value in NOVRAM
 */
#define NVR_MAGIC2_ADDR 0x1f8


/*! \def NVR_MAGIC
 * Expected NOVRAM magic value
 */
#define NVR_MAGIC 0x4c616d70


/*! \def NVR_SERIAL_ADDR
 * Address of serial number in NOVRAM
 */
#define NVR_SERIAL_ADDR 0xC


/*! \def NVR_BOARD_CONFIG_ADDR
 * Address of board config value in NOVRAM
 */
#define NVR_BOARD_CONFIG_ADDR 0x28


/*! \def NVR_BOARD_TYPE_ADDR
 * Address of board type value in NOVRAM
 */
#define NVR_BOARD_TYPE_ADDR 0x2c


/*! \def NVR_BOARD_SUB_TYPE_ADDR
 * Address of board sub type value in NOVRAM
 */
#define NVR_BOARD_SUB_TYPE_ADDR 0xc8


/*! \def NVR_BOARD_PART_NO_ADDR
 * Address of board type value in NOVRAM
 */
#define NVR_BOARD_PART_NO_ADDR 0x48


/*! \def NVR_HW_VARIANT_ADDR
 * Address of board type value in NOVRAM
 */
#define NVR_BOARD_HW_VARIANT_ADDR 0xcc


/*! \def NVR_CHECKSUM_ADDR
 * Address of checksum in NOVRAM
 */
#define NVR_CHECKSUM_ADDR 0x1fc


/*! \def NVR_BIP1EX_ADDR
 * Address of BIU1 File extension in NOVRAM
 */
#define NVR_BIP1EX_ADDR 0x070


/*! \def NVR_CPUCLK_ADDR
 *
 * Address of CPU clock value in NOVRAM
 */
#define NVR_CPUCLK_ADDR 0x050


/*! \def APU_IRQ_EVENT_REG_OFFSET
 * 32-Bit offset of IRQ event register in I/O memory
 */
#define APU_IRQ_EVENT_REG_OFFSET 0x60


/*! \def APU_IRQ_MASK_REG_OFFSET
 * 32-Bit offset of IRQ mask register in I/O memory
 */
#define APU_IRQ_MASK_REG_OFFSET 0x62


/*! \def APU_IRQ_EVENT_ENABLE
 * Bit in APU IRQ event register that enables interrupts
 */
#define APU_IRQ_EVENT_ENABLE (1 << 31)


/*! \def APU_IRQ_EVENT_BIU1
 * Bit in APU IRQ event/mask register that controls BIU 1 interrupt
 */
#define APU_IRQ_EVENT_BIU1 (1 << 0)


/*! \def APU_IRQ_EVENT_BIU2
 * Bit in APU IRQ event/mask register that controls BIU 2 interrupt
 */
#define APU_IRQ_EVENT_BIU2 (1 << 1)


/*! \def APU_IRQ_EVENTS
 * All IRQ events the driver handles for APU devices
 */
#define APU_IRQ_EVENTS ( APU_IRQ_EVENT_BIU1 | APU_IRQ_EVENT_BIU2 )


/*! \def IS_32BIT_ALIGNED
 * Checks if particular address is aligned to 32bit
 */
#define IS_32BIT_ALIGNED(address) \
    ((address) & 0x3 ? AiFalse : AiTrue)




/*! \union apu_reset_reg
 * Bitfield that describes the reset register at \ref APU_RESET_REG_OFFSET in I/O memory
 */
union apu_reset_reg
{
    AiUInt32  all;
    struct
    {
        AiUInt32 biu1_reset    : 1;  /* BIU1 Prozessor Reset*/
        AiUInt32 biu2_reset    : 1;  /* BIU2 Prozessor Reset*/
        AiUInt32 fpga1_reset   : 1;  /* IO-FPGA 1 Reset*/
        AiUInt32 fpga2_reset   : 1;  /* IO_FPGA 2 Reset*/
        AiUInt32 tcp_reset     : 1;  /* Resets the ATMEl (TCP)*/
        AiUInt32 irig_source   : 1;  /* IRIG xINTERN / EXTERN*/
        AiUInt32 reserved      : 1;  /* reserved*/
        AiUInt32 asp_reset     : 1;  /* ASP Reset    (v01.01)*/
        AiUInt32 reserved2     : 7;  /* reserved*/
        AiUInt32 sdram_start   : 1;  /* write one to start SDRAM (GLOBAL and LOCAL) only once v01.01 */
        AiUInt32 reserved3     : 14; /* reserved*/
        AiUInt32 relais        : 1;
        AiUInt32 irig_reset    : 1;  /* IRIG control in RESET (0)*/
    }Reg;
};


/*! \union apu_tcp_port_reg
 * Bitfield that describes the TCP port register at \ref APU_TCP_PORT_REG_OFFSET on APU devices
 */
union apu_tcp_port_reg
{
    AiUInt32 all;
    struct
    {
        AiUInt32 data       : 8;     /* 8 Bit Data*/
        AiUInt32 command    : 5;     /* 5 Bit Command*/
        AiUInt32            : 3;     /* reserved*/
        AiUInt32 tcp_ready  : 1;     /* status bit Atmel ready*/
        AiUInt32 tcp_csx    : 1;     /* Interrupt Atmel*/
        AiUInt32 tcp_write  : 1;     /* Atmel write access (read not)*/
        AiUInt32            : 13;    /* reserved*/
        AiUInt32            : 0;     /* alignment*/
    }Reg;
};




int aim_usb_apu_hw_init(struct aim_usb_interface* intf)
{
    int i;
    int err;
    struct usb_host_interface* interface;
    struct usb_host_endpoint* config_response_ep = NULL;
    struct usb_host_endpoint* config_cmd_ep = NULL;
    struct usb_host_endpoint* pci_response_ep = NULL;
    struct usb_host_endpoint* pci_cmd_ep = NULL;
    struct usb_host_endpoint* fifo_response_ep = NULL;
    struct usb_host_endpoint* fifo_cmd_ep = NULL;
    struct usb_host_endpoint* interrupt_ep = NULL;
    struct aim_usb_com_channel* config_channel;
    struct aim_usb_com_channel* pci_channel;
    struct aim_usb_com_channel* fifo_channel;
    struct aim_usb_int_ep* irq_endpoint;

    BUG_ON(!intf);

    aim_usb_intf_dbg(intf, "Initializing APU hardware");

    intf->has_asp = AiFalse;
    interface = intf->interface->cur_altsetting;

    for(i = 0; i < interface->desc.bNumEndpoints; i++)
    {
        aim_usb_intf_dbg(intf, "Found endpoint with addr: 0x%x", interface->endpoint[i].desc.bEndpointAddress);

        if(usb_endpoint_num(&interface->endpoint[i].desc) == APU_CONFIG_ENDPOINT_NUM)
        {
            if(usb_endpoint_is_bulk_in(&interface->endpoint[i].desc))
            {
                config_response_ep = &interface->endpoint[i];
            }
            else if(usb_endpoint_is_bulk_out(&interface->endpoint[i].desc))
            {
                config_cmd_ep = &interface->endpoint[i];
            }
        }

        if(usb_endpoint_num(&interface->endpoint[i].desc) == APU_PCI_ENDPOINT_NUM)
        {
            if(usb_endpoint_is_bulk_in(&interface->endpoint[i].desc))
            {
                pci_response_ep = &interface->endpoint[i];
            }
            else if(usb_endpoint_is_bulk_out(&interface->endpoint[i].desc))
            {
                pci_cmd_ep = &interface->endpoint[i];
            }
        }

        if(usb_endpoint_num(&interface->endpoint[i].desc) == APU_FIFO_IN_ENDPOINT_NUM)
        {
            fifo_response_ep = &interface->endpoint[i];
        }

        if(usb_endpoint_num(&interface->endpoint[i].desc) == APU_FIFO_OUT_ENDPOINT_NUM)
        {
            fifo_cmd_ep = &interface->endpoint[i];
        }

        if(usb_endpoint_num(&interface->endpoint[i].desc) == APU_INTERRUPT_ENDPOINT_NUM)
        {
            interrupt_ep = &interface->endpoint[i];
        }
    }

    if(!config_cmd_ep || !config_response_ep)
    {
        aim_usb_intf_err(intf, "No config endpoints found on APU platform");
        return -ENODEV;
    }

    config_channel = aim_usb_interface_create_com_channel(intf, APU_CONFIG_CHANNEL_ID, config_cmd_ep, config_response_ep);
    if(IS_ERR(config_channel))
    {
        aim_usb_intf_err(intf, "Failed to create config channel on APU platform");
        return PTR_ERR(config_channel);
    }

    if(!pci_cmd_ep || !pci_response_ep)
    {
        aim_usb_intf_err(intf, "No PCI endpoints found on APU platform");
        return -ENODEV;
    }

    pci_channel = aim_usb_interface_create_com_channel(intf, APU_PCI_CHANNEL_ID, pci_cmd_ep, pci_response_ep);
    if(IS_ERR(pci_channel))
    {
        aim_usb_intf_err(intf, "Failed to create PCI channel on APU platform");
        return PTR_ERR(pci_channel);
    }

    if(!fifo_cmd_ep || !fifo_response_ep)
    {
        aim_usb_intf_err(intf, "No FIFO endpoints found on APU platform");
        return -ENODEV;
    }

    fifo_channel = aim_usb_interface_create_com_channel(intf, APU_FIFO_CHANNEL_ID, fifo_cmd_ep, fifo_response_ep);
    if(IS_ERR(fifo_channel))
    {
        aim_usb_intf_err(intf, "Failed to create FIFO channel on APU platform");
        return PTR_ERR(fifo_channel);
    }

    if(!interrupt_ep)
    {
        aim_usb_intf_err(intf, "No Interrupt endpoint found on APU platform");
        return -ENODEV;
    }

    irq_endpoint = aim_usb_interface_create_int_ep(intf, interrupt_ep);
    if(IS_ERR(irq_endpoint))
    {
        aim_usb_intf_err(intf, "Failed to create Interrupt Endpoint on APU platform");
        return PTR_ERR(irq_endpoint);
    }

    err = aim_usb_ncc_init(&intf->ncc, config_channel, pci_channel, fifo_channel);
    if(err)
    {
        aim_usb_intf_err(intf, "Failed to initialize Netchip controller");
        return err;
    }

    err = aim_usb_ncdmac_init(&intf->ncdmac, &intf->ncc);
    if(err)
    {
        aim_usb_intf_err(intf, "Failed to initialize Netchip controller");
        return err;
    }


    /* We do not know of a transfer size limit for APU devices. So we set it to zero */
    config_channel->max_transfer_length = 0;

    return 0;
}


/*! \brief Detects size of specific PCI bar
 *
 * @param intf the APU USB interface
 * @param bar_id ID of bar to detect size of
 * @return size of bar in bytes on success, negative errno code on failure
 */
static __inline__ ssize_t get_bar_size(struct aim_usb_interface* intf, int bar_id)
{
    AiUInt32 address;
    AiUInt32 size;
    int err;

    switch(bar_id)
    {
        case 0:
            address = PCI_BAR_0_ADDRESS;
            break;
        case 1:
            address = PCI_BAR_1_ADDRESS;
            break;
        case 2:
            address = PCI_BAR_2_ADDRESS;
            break;
        default:
            return -EINVAL;
    }

    err = aim_usb_ncc_pci_config_write(&intf->ncc, address, 0xFFFFFFFF);
    if(err)
    {
        return err;
    }

    err = aim_usb_ncc_pci_config_read(&intf->ncc, address, &size);
    if( err )
    {
       return err;
    }

    size = ~(size);
    size += 1;

    return size;
}


/*! \brief Maps PCI bar to specific PCI bus address
 * 
 * @param intf the USB interface to map the PCI bar for
 * @param bar_id ID of bar to map
 * @param address the PCI bus address to map the bar to
 * @return 0 on success, errno code on failure
 */
static __inline__ int map_pci_bar(struct aim_usb_interface* intf, int bar_id, phys_addr_t address)
{
    AiUInt32 config_address;
    int err;

    switch(bar_id)
    {
        case 0:
            config_address = PCI_BAR_0_ADDRESS;
            break;
        case 1:
            config_address = PCI_BAR_1_ADDRESS;
            break;
        case 2:
            config_address = PCI_BAR_2_ADDRESS;
            break;
        default:
            return -EINVAL;
    }

    err = aim_usb_ncc_pci_config_write(&intf->ncc, config_address, address);
    if(err)
    {
        return err;
    }

    return err;
}


/*! \brief Set-up PCI access for APU devices
 *
 * @param intf the USB interface to set-up PCI access for
 * @return 0 on success, errno code on failure
 */
static int setup_pci_access(struct aim_usb_interface* intf)
{
    ssize_t bar_size;
    off_t last_offset = 0;
    int err;
    union apu_reset_reg reset_reg;

    BUG_ON(!intf);

    aim_usb_intf_dbg(intf, "Mapping API bar registers");

    bar_size = get_bar_size(intf, APU_GLOBAL_MEM_BAR_ID);
    if(bar_size < 0)
    {
        aim_usb_intf_err(intf, "Failed to detect global memory size");
        return bar_size;
    }

    err = map_pci_bar(intf, APU_GLOBAL_MEM_BAR_ID, last_offset);
    if(err)
    {
        aim_usb_intf_err(intf, "Failed to map global memory");
        return -EFAULT;
    }

    intf->global_memory.size = bar_size;
    intf->global_memory.bus_address = last_offset;

    aim_usb_intf_dbg(intf, "Mapped global memory with size of %zu KB to address %pa", intf->global_memory.size / 1024,
                           &intf->global_memory.bus_address);

    last_offset += bar_size;

    bar_size = get_bar_size(intf, APU_IO_MEM_BAR_ID);
    if(bar_size < 0)
    {
        aim_usb_intf_err(intf, "Failed to detect I/O memory size");
        return bar_size;
    }

    err = map_pci_bar(intf, APU_IO_MEM_BAR_ID, last_offset);
    if(err)
    {
        aim_usb_intf_err(intf, "Failed to map I/O memory");
        return -EFAULT;
    }

    intf->io_memory.size = bar_size;
    intf->io_memory.bus_address = last_offset;

       aim_usb_intf_dbg(intf, "Mapped I/O memory with size of %zu KB to address %pa", intf->io_memory.size / 1024,
                              &intf->io_memory.bus_address);

    // Enable the PCI-Device in the PCI-LCA
    err = aim_usb_ncc_pci_config_write(&intf->ncc, PCI_CMD_STATUS_ADDRESS, 0x00000006);
    if(err)
    {
        aim_usb_intf_err(intf, "Failed to enable APU PCI access");
        return err;
    }

    // Reset the DRAM-Controller
    err = aim_usb_ncc_pci_read(&intf->ncc, intf->io_memory.bus_address + (APU_RESET_REG_OFFSET * 4), &reset_reg.all);
    if(err)
    {
        aim_usb_intf_err(intf, "Failed to reset APU DRAM controller");
        return err;
    }

    aim_usb_intf_dbg(intf, "SDRAM enabled %s", reset_reg.Reg.sdram_start ? "Yes" : "No");

    reset_reg.Reg.sdram_start = 1;

    err = aim_usb_ncc_pci_write(&intf->ncc, intf->io_memory.bus_address + (APU_RESET_REG_OFFSET * 4), reset_reg.all);
    if (err)
    {
        aim_usb_intf_err(intf, "Failed to reset APU DRAM controller");
        return err;
    }

    return 0;
}


/*! \brief Wait for a maximum time until TCP gets ready for new command
 *
 * @param intf The interface to wait for its TCP to get ready
 * @param msecs Number of millisconds to wait for TCP getting ready
 * @return 0 if TCP is ready, errno code on failure. -ETIMEDOUT if timed-out
 */
static int wait_for_tcp_ready(struct aim_usb_interface* intf, int msecs)
{
    bool ready;
    int try;
    int err;
    union apu_tcp_port_reg tcp_reg;

    BUG_ON(!intf);

    try = 0;
    ready = false;
    err = 0;

    do
    {
        err = aim_usb_ncc_pci_read(&intf->ncc, intf->io_memory.bus_address + (APU_TCP_PORT_REG_OFFSET * 4), &tcp_reg.all);
        if(err)
        {
            break;
        }

        if(tcp_reg.Reg.tcp_ready)
        {
            break;
        }

        if(++try > msecs)
        {
            err = -ETIMEDOUT;
            break;
        }

        mdelay(1);

    }while(!ready);


    return err;
}


int aim_usb_apu_tcp_reg_read(struct aim_usb_interface* intf, AiUInt32 port, AiUInt32* value)
{
    union apu_tcp_port_reg tcp_reg;
    int err;

    BUG_ON(!intf || !value);

    tcp_reg.Reg.command = port;
    tcp_reg.Reg.tcp_write = 0;
    tcp_reg.Reg.tcp_ready = 0;

    err = aim_usb_ncc_pci_write(&intf->ncc, intf->io_memory.bus_address + (APU_TCP_PORT_REG_OFFSET * 4), tcp_reg.all);
    if(err)
    {
        return err;
    }

    err = wait_for_tcp_ready(intf, 300);
    if(err)
    {
        return err;
    }

    err = aim_usb_ncc_pci_read(&intf->ncc, intf->io_memory.bus_address + (APU_TCP_PORT_REG_OFFSET * 4), &tcp_reg.all);
    if(err)
    {
        return err;
    }

    *value = tcp_reg.Reg.data;

    return 0;
}


int aim_usb_apu_tcp_reg_write(struct aim_usb_interface* intf, AiUInt32 port, AiUInt32 value)
{
    union apu_tcp_port_reg tcp_reg;
    int err;

    BUG_ON(!intf);

    tcp_reg.Reg.command = port;
    tcp_reg.Reg.data = value;
    tcp_reg.Reg.tcp_write = 1;
    tcp_reg.Reg.tcp_ready = 0;

    err = aim_usb_ncc_pci_write(&intf->ncc, intf->io_memory.bus_address + (APU_TCP_PORT_REG_OFFSET * 4), tcp_reg.all);
    if(err)
    {
        return err;
    }

    err = wait_for_tcp_ready(intf, 300);
    if(err)
    {
        return err;
    }

    return 0;
}


/*! \brief Starts TCP on APU device
 *
 * @param intf The interface that hosts the TCP to start
 * @return 0 on success, errno code on failure
 */
static int start_tcp(struct aim_usb_interface* intf)
{
    union apu_reset_reg reset_reg;
    int err;
    AiUInt32 tcp_data;

    BUG_ON(!intf);

    aim_usb_intf_dbg(intf, "Starting APU TCP");

    err = aim_usb_ncc_pci_read(&intf->ncc, intf->io_memory.bus_address + (APU_RESET_REG_OFFSET * 4), &reset_reg.all);
    if(err)
    {
        return err;
    }

    if(reset_reg.Reg.tcp_reset)
    {
        aim_usb_intf_dbg(intf, "TCP already started. Skipping boot");
        return 0;
    }


    /* Clear the TCP data register which contains the ready bit */
    err = aim_usb_ncc_pci_write(&intf->ncc, intf->io_memory.bus_address + (APU_TCP_PORT_REG_OFFSET * 4), 0);
    if(err)
    {
        return err;
    }

    reset_reg.Reg.tcp_reset = 1;

    err = aim_usb_ncc_pci_write(&intf->ncc, intf->io_memory.bus_address + (APU_RESET_REG_OFFSET * 4), reset_reg.all); /* ATMEL not longer in reset*/
    if(err)
    {
        return err;
    }

    err = wait_for_tcp_ready(intf, 300);
    if(err)
    {
        return err;
    }


    /* IRIG control enable*/
    reset_reg.Reg.irig_reset = 1; /* IRIG control enabled (in function with TCP)*/

    err = aim_usb_ncc_pci_write(&intf->ncc, intf->io_memory.bus_address + (APU_RESET_REG_OFFSET * 4), reset_reg.all); /* ATMEL not longer in reset*/
    if(err)
    {
        return err;
    }

    mdelay(1);

    err = aim_usb_apu_tcp_reg_read(intf, APU_TCP_VERSION_PORT, &tcp_data);
    if(err)
    {
        return err;
    }

    aim_usb_intf_info(intf, "TCP successfully started. Version is 0x%x\n", tcp_data);

    return 0;
}


int aim_usb_apu_novram_get(struct aim_usb_interface* intf, AiUInt32 address, AiUInt32* value)
{
    int i;
    AiUInt32 tcp_data;

    BUG_ON(!intf || !value);

    *value = 0;
    for (i=0;i<4;i++)
    {
        /*Write address to read from to TCP address registers */
        aim_usb_apu_tcp_reg_write(intf, APU_TCP_ADDRLO, address & 0xFF);
        aim_usb_apu_tcp_reg_write(intf, APU_TCP_ADDRHI, (address >> 8) & 0x1);

        aim_usb_apu_tcp_reg_read(intf, APU_TCP_EEPROM_READ, &tcp_data);

        *value = (*value >> 8) | (tcp_data << 24);

        address++;
    }

    return 0;
}


/*! \brief Calculate the checksum of current NOVRAM contents
 *
 * @param intf the interface that host the NOVRAM to calculate checksum of
 * @return the calculated checksum
 */
static AiUInt32 novram_checksum_calc(struct aim_usb_interface* intf)
{
    AiUInt32 sum;
    AiUInt32 current_value;
    int i;

    sum = 0;

    /*128=max NOVRAM AiUInt32 locations*/
    for(i = 0; i<128-1; i++)
    {
        aim_usb_apu_novram_get(intf, i * 4, &current_value);
        sum += current_value;
    }

    return sum;
}


/*! \brief Checks if NOVRAM of an APU device is valid
 *
 * @param intf the interface to check NOVRAM of
 * @return 0 if NOVRAM is valid, errno code otherwise
 */
static int novram_check(struct aim_usb_interface* intf)
{
    AiUInt32 magic1;
    AiUInt32 magic2;
    AiUInt32 checksum;
    AiUInt32 novram_value;

    BUG_ON(!intf);

    aim_usb_apu_novram_get(intf, NVR_MAGIC_ADDR, &magic1);    /*read magic 1*/
    aim_usb_apu_novram_get(intf, NVR_MAGIC2_ADDR, &magic2);    /*read magic 2*/

    aim_usb_intf_dbg(intf, "Novram Magic: 0x%x", magic1);
    aim_usb_intf_dbg(intf, "Novram Magic 2: 0x%x", magic2);

    if(magic1 != NVR_MAGIC || magic2 != NVR_MAGIC)
    {
        aim_usb_intf_err(intf, "Novram Magic invalid");
        return -ENODEV;
    }

    checksum = novram_checksum_calc(intf);
    aim_usb_apu_novram_get(intf, NVR_CHECKSUM_ADDR, &novram_value);

    if(checksum != novram_value)
    {
        aim_usb_intf_err(intf, "Invalid Novram Checksum. Novram has to be restored manually");
        return -ENODEV;
    }

    return 0;
}


/*! \brief Converts ASCII character to integer representation
 *
 * @param single_char the ASCII character to convert
 * @return the converted integer
 */
static int char_to_int (char single_char)
{
    switch (single_char)
    {
    case '0'   : return(0x0);
    case '1'   : return(0x1);
    case '2'   : return(0x2);
    case '3'   : return(0x3);
    case '4'   : return(0x4);
    case '5'   : return(0x5);
    case '6'   : return(0x6);
    case '7'   : return(0x7);
    case '8'   : return(0x8);
    case '9'   : return(0x9);
    case 'a'   :
    case 'A'   : return(0xa);
    case 'b'   :
    case 'B'   : return(0xb);
    case 'c'   :
    case 'C'   : return(0xc);
    case 'd'   :
    case 'D'   : return(0xd);
    case 'e'   :
    case 'E'   : return(0xe);
    case 'f'   :
    case 'F'   : return(0xf);
    default    : return(0);
    }
}


/*! \brief Loads firmware image in SRE format and decodes it to binary format
 *
 * @param intf the interface to load firmware image for
 * @param file_name the name of the firmware image to load. Linux firmware subsystem will be used to detect the file
 * @param buffer the decoded firmware image will be stored here
 * @return the size of the decoded image in bytes on success. Negative errno code on failure
 */
static ssize_t firmware_load_decode(struct aim_usb_interface* intf, const char* file_name, void* buffer)
{
    int err;
    const struct firmware* firmware;
    AiUInt8 ch;
    unsigned int x, j, count, i, length, chk_sum, mem_adr, first_line,
            mem_offset, mem_count;
    AiUInt8 ibuf[256];
    AiUInt8* byte_stream;
    AiUInt8 nibble[0x104];

    BUG_ON(!intf || !file_name || !buffer);

    err = request_firmware(&firmware, file_name, intf->dev);

    if (err)
    {
        aim_usb_intf_err(intf, "Firmware Request of file %s failed\n", file_name);
        return err;
    }

    aim_usb_intf_dbg(intf, "Firmware file %s successfully requested\n", file_name);

    byte_stream = (AiUInt8*) firmware->data;

    x = 0;
    j = 0;
    count = 0;
    first_line = 0;
    mem_offset = 0;
    mem_count = 0;
    ch = 0xff;

    while (ch != '\0' && count < firmware->size)
    {
        for (i = 0; i < 255; i++)
        {
            ch = *(byte_stream + count);
            ibuf[i] = ch;
            count++;

            if (ch == '\n' || ch == '\0')
            {
                break;
            }
        }

        /* Auswerten der S-Record Zeile, Typ, ggf. Daten Speichern */
        /* the general format of an S-Record */
        /*  2      2       2..4      ?             2           */
        /* +------+-------+---/ /---+-----/ /-----+----------+ */
        /* | type | count | address |     data    | checksum | */
        /* +------+-------+---/ /---+----/ /------+----------+ */

        if (ibuf[0] == 0x53) /*big 'S'*/
        {
            length = 0; /* count (pairwaise)*/

            length = (char_to_int((char) ibuf[2]) << 4) | char_to_int((char) ibuf[3]); /*bitwise or*/
            chk_sum = length;

            for (i = 4; i <= (3 + (length * 2)); i += 2)
            {
                if ((((char) ibuf[i]) == '\n')
                    || (((char) ibuf[i + 1]) == '\n'))
                {
                    break;
                }

                nibble[(i - 4) / 2] = (char_to_int((char) ibuf[i]) << 4) | char_to_int((char) ibuf[i + 1]);

                chk_sum += nibble[(i - 4) / 2];
            }

            if ((chk_sum & 0xff) != 255) /*mask lower 8 bits*/
            {
                break;
            }

            /* switch on S-Record type */
            switch (ibuf[1])
            /*S-Record type*/
            {
            case '0': /*S0 Version information, not implemented*/
                break;

            case '1': /*S1 2byte address field, memory loadable data*/
                mem_adr = (((unsigned int) nibble[0]) << 8) | ((unsigned int) nibble[1]);

                if (first_line == 0) /* if the first line is read calculate the offset*/
                {
                    mem_offset = mem_adr;
                    first_line = 1;
                }
                else if (mem_adr != (mem_count + mem_offset)) /*es gibt die Möglichkeit von Luecken / Sprüngen*/
                {
                    if (mem_adr < (mem_count + mem_offset)) /*eigenlich der einzige Fall*/
                    {
                        return (-EFAULT);
                        break;
                    }

                    for (i = 0; (mem_count + mem_offset) < mem_adr; i++) /*fill unused memblocks*/
                    {
                        *((char*) buffer + mem_count) = 0x1;
                        mem_count++;
                    }
                }

                for (i = 2; i < (length - 1); i++)
                {
                    *((char*) buffer + mem_count) = nibble[i];
                    mem_count++;
                }

                break;

            case '2': /*S2 3byte address field, memory loadable data*/
                mem_adr = (unsigned int) ((((unsigned int) (nibble[0])) << 16)
                           | (((unsigned int) (nibble[1])) << 8)
                           | (unsigned int) nibble[2]);

                if (first_line == 0) /* if the first line is read calculate the offset*/
                {
                    mem_offset = mem_adr;
                    first_line = 1;
                }
                else if (mem_adr != (unsigned int) (mem_count + mem_offset)) /*es gibt die Möglichkeit von Luecken / Sprüngen*/
                {
                    if (mem_adr < (unsigned int) (mem_count + mem_offset)) /*eigenlich der einzige Fall*/
                    {
                        return (-EFAULT);
                        break;
                    }
                    for (i = 0; (mem_count + mem_offset) < mem_adr; i++) /*fill unused memblocks*/
                    {
                        *((char*) buffer + mem_count) = 0x1;
                        mem_count++;
                    }
                }
                /*-----------------           */
                for (i = 3; i < (length - 1); i++)
                {
                    *((char*) buffer + mem_count) = nibble[i];
                    mem_count++;
                }
                break;

                /*-----------------*/
            case '3': /*S3 4byte address field, memory loadable data*/
                mem_adr = (((unsigned int) (nibble[0])) << 24)
                        | (((unsigned int) (nibble[1])) << 16)
                        | (((unsigned int) (nibble[2])) << 8)
                        | ((unsigned int) (nibble[3]));

                if (first_line == 0) /* if the first line is read calculate the offset*/
                {
                    mem_offset = mem_adr;
                    first_line = 1;
                }
                else if (mem_adr != (mem_count + mem_offset)) /*es gibt die Möglichkeit von Luecken / Sprüngen*/
                {
                    if (mem_adr < (mem_count + mem_offset)) /*eigenlich der einzige Fall*/
                    {
                        return (-EFAULT);
                        break;
                    }
                    for (i = 0; (mem_count + mem_offset) < mem_adr; i++) /*fill unused memblocks*/
                    {
                        *((char*) buffer + mem_count) = 0x1;
                        mem_count++;
                    }
                }

                for (i = 4; i < (length - 1); i++)
                {
                    *((char*) buffer + mem_count) = nibble[i];
                    mem_count++;
                }

                break;

            case '4': /*S4 does not exists (by definition)*/

                return (-EFAULT);
                break;

            case '5': /*S5 counting information, not implemented*/
                break;

            case '6': /*S6 does not exists (by definition)*/
                /* return (FAIL) */
                break;

            case '7': /*S7 address field = start execution address (4byte), no data field*/
                mem_adr = ((unsigned int) (nibble[0]) << 24)
                        | ((unsigned int) (nibble[1]) << 16)
                        | ((unsigned int) (nibble[2]) << 8)
                        | ((unsigned int) (nibble[3]));
                break;

            case '8': /*S8 address field = start execution address (3byte), no data field*/
                mem_adr = ((unsigned int) (nibble[0]) << 16)
                        | ((unsigned int) (nibble[1]) << 8)
                        | ((unsigned int) (nibble[2]));
                break;

            case '9': /*S9 address field = start execution address (2byte), no data field*/
                mem_adr = ((unsigned int) (nibble[0]) << 8)
                        | ((unsigned int) (nibble[1]));
                break;

            default:
                break;
            }/*switch   */
        }/* if S*/
    }

    release_firmware(firmware);

    return mem_count - 1;
}


/*! \brief Waits until BIU 1 interrupt is raised
 *
 * @param intf the interface that hosts BIU to wait for
 * @return 0 if interrupt was raised, errno code if not
 */
static int biu_wait_irq(struct aim_usb_interface* intf)
{
    AiUInt32 ul_Irqevent = 0;
    int err;
    int LoopCount = 0;

    BUG_ON(!intf);

    /* pull register to wait for IRQ*/
    while((ul_Irqevent & 0x00000001) == 0)   /* BIU 1 sends Interrupt (boot OK)*/
    {
        err = aim_usb_ncc_pci_read(&intf->ncc, intf->io_memory.bus_address + 0x0180,       // (IRQ-Event Register)
                                   &ul_Irqevent);
        if(err)
        {
            break;
        }

        if(LoopCount >= 50000)
        {
            err = -ETIMEDOUT;
            break;
        }

        LoopCount++;
    }

    // clear BIP 1 IRQ
    if(!err)
    {
        err = aim_usb_ncc_pci_write(&intf->ncc, intf->io_memory.bus_address + 0x0180, (ul_Irqevent & 0x00000001));
        if(err)
        {

           return err;
        }
    }

    return err;
}


/*! \brief Start BIU 1 of APU device
 *
 * @param intf the interface that hosts BIU 1 to start
 * @return 0 if BIU was started successfully, errno code on failure
 */
static int biu_start(struct aim_usb_interface* intf)
{
    int err;
    union apu_reset_reg reset_reg;
    void* firmware = NULL;
    ssize_t firmware_size;
    char firmware_file[32];
    AiUInt32 novram_value;

    BUG_ON(!intf);

    aim_usb_intf_dbg(intf, "Starting BIU");

    err = 0;
    do
    {
        /* 1: Initialization, IRQ things, processor in RESET, FPGA out of RESET*/
        err = aim_usb_ncc_pci_write(&intf->ncc, intf->io_memory.bus_address + 0x0180, // (IRQ-Event Register)
                                   0x0000ffff);
        if(err)
        {
            break;
        }

        err = aim_usb_ncc_pci_write(&intf->ncc, intf->io_memory.bus_address + 0x0188, // (IRQ-Mask Register)
                                   0x0000FFFC);
        if(err)
        {
            break;
        }

        firmware = kmalloc(1024 * 1024 * 4, GFP_KERNEL);
        if(!firmware)
        {
            aim_usb_intf_dbg(intf, "Allocation of Firmware buffer memory failed");
            err = -ENOMEM;
            break;
        }

        aim_usb_apu_novram_get(intf, NVR_BIP1EX_ADDR, &novram_value);

        snprintf(firmware_file, sizeof(firmware_file), "BIP_%04x.sre", novram_value);

        aim_usb_intf_dbg(intf, "Loading firmware: %s", firmware_file);

        firmware_size = firmware_load_decode(intf, firmware_file, firmware);
        if(firmware_size < 0)
        {
            err = firmware_size;
            break;
        }

        err = aim_usb_ncdmac_write(&intf->ncdmac, intf->global_memory.bus_address, firmware, firmware_size);
        if(err)
        {
            break;
        }

        err = aim_usb_ncc_pci_read(&intf->ncc, intf->io_memory.bus_address + (APU_RESET_REG_OFFSET * 4), // (Reset Register)
                                   &reset_reg.all);
        if(err)
        {
            break;
        }

        aim_usb_intf_dbg(intf, "BIU 1 in reset? %s", reset_reg.Reg.biu1_reset ? "No" : "Yes");

        reset_reg.Reg.biu1_reset = 1;

        err = aim_usb_ncc_pci_write(&intf->ncc, intf->io_memory.bus_address + (APU_RESET_REG_OFFSET * 4), // (Reset Register)
                                   reset_reg.all);
        if(err)
        {
            break;
        }

        err = biu_wait_irq(intf);
        if(err)
        {
            break;
        }

        aim_usb_apu_novram_get(intf, NVR_CPUCLK_ADDR, &novram_value);
        err = aim_usb_ncc_pci_write(&intf->ncc, intf->global_memory.bus_address + 0, novram_value); // Give Speed to BIU
        if (err)
        {
            break;
        }

        aim_usb_apu_novram_get(intf, NVR_BOARD_TYPE_ADDR, &novram_value);

        aim_usb_intf_dbg(intf, "Board Type: 0x%x", novram_value);

        /* For 429 boards, we have to tell the Firmware the board type */
        if((novram_value & 0xF0) == 0x20)
        {
            /* Write BoardType value from NOVRAM to Global RAM
               LCA needs the BoardType to determine number of channels of the board */
            err = aim_usb_ncc_pci_write(&intf->ncc, intf->global_memory.bus_address + 0x3c, novram_value);
            if(err)
            {
                break;
            }
        }

        err = aim_usb_ncc_pci_write(&intf->ncc, intf->io_memory.bus_address + 0x0180, 0x00010000); // Start BIU 1 by Interrupt
        if(err)
        {
            break;
        }

        err = biu_wait_irq(intf);
        if(err)
        {
            break;
        }

        /* Switch Relais on */
        reset_reg.Reg.relais = 1;

        err = aim_usb_ncc_pci_write(&intf->ncc, intf->io_memory.bus_address + (APU_RESET_REG_OFFSET * 4), // (Reset Register)
                                    reset_reg.all);
        if (err)
        {
            break;
        }

    }while(0);

    if(firmware)
    {
        kfree(firmware);
    }

    return err;

}


/*! \brief Stops BIU on APU devices
 *
 * The on-board PCI access must have been set-up so this function works
 * @param intf the APU interface to stop BIU on
 */
static void biu_stop(struct aim_usb_interface* intf)
{
    int err;
    union apu_reset_reg reset_reg;

    BUG_ON(!intf);

    aim_usb_intf_dbg(intf, "Stopping BIU");

    err = aim_usb_ncc_pci_read(&intf->ncc, intf->io_memory.bus_address + (APU_RESET_REG_OFFSET * 4), // (Reset Register)
                               &reset_reg.all);
    if(err)
    {
        aim_usb_intf_err(intf, "Failed to read reset register");
        return;
    }

    aim_usb_intf_dbg(intf, "BIU 1 in reset? %s", reset_reg.Reg.biu1_reset ? "No" : "Yes");

    reset_reg.Reg.biu1_reset = 0;

    err = aim_usb_ncc_pci_write(&intf->ncc, intf->io_memory.bus_address + (APU_RESET_REG_OFFSET * 4), // (Reset Register)
                                reset_reg.all);
    if(err)
    {
        aim_usb_intf_err(intf, "Failed to write reset register");
        return;
    }
}


/*! \brief Allocates and initializes APU global memory mirror
 *
 * @param intf the APU interface to create global memory mirror for
 * @return 0 on success, errno code on failure
 */
static int global_memory_mirror_create(struct aim_usb_interface* intf)
{
    int err;

    BUG_ON(!intf || intf->global_memory.base);

    aim_usb_intf_dbg(intf, "Creating global memory mirror");

    err = 0;
    do
    {
        intf->global_memory.base = kmalloc(intf->global_memory.size, GFP_KERNEL | GFP_DMA);
        if(!intf->global_memory.base)
        {
            aim_usb_intf_err(intf, "Failed to allocate global memory");
            err = -ENOMEM;
            break;
        }

        /* Do initial synchronization of the memory mirror by reading whole global memory */
        err = aim_usb_ncdmac_read(&intf->ncdmac, intf->global_memory.bus_address, intf->global_memory.base,
                                  intf->global_memory.size);
        if(err)
        {
            aim_usb_intf_err(intf, "Failed initial sync of gmm");
            break;
        }
    }while(0);

    if(err && intf->global_memory.base)
    {
        kfree(intf->global_memory.base);
        intf->global_memory.base = NULL;
    }

    return err;
}


/*! \brief Enables Netchip controller interrupts used by APU
 *
 * @param intf The interface to enable Netchip interrupts for
 * @return 0 on success, errno code on failure
 */
static __inline__ int enable_nc_interrupts(struct aim_usb_interface* intf)
{
    int err;
    union nc_usb_irq_reg irq_reg = {0};
    union nc_irqstat1_reg irq_stat_reg = {0};

    BUG_ON(!intf);

    aim_usb_intf_dbg(intf, "Enabling Netchip interrupts");

    irq_stat_reg.fields.pci_int_a_irq = 1;

    /* Clear pending interrupts */
    err = aim_usb_ncc_reg_write(&intf->ncc, NC_IRQSTAT1_REG_ADDR, irq_stat_reg.value);
    if(err)
    {
        return err;
    }

    /* Enable PCI INT A interrupts */
    err = aim_usb_ncc_reg_read(&intf->ncc, NC_USB_IRQ_REG_ADDR, &irq_reg.value);
    if(err)
    {
        return err;
    }

    irq_reg.fields.pci_int_a_enable = 1;
    irq_reg.fields.usb_interrupt_enable = 1;

    err = aim_usb_ncc_reg_write(&intf->ncc, NC_USB_IRQ_REG_ADDR, irq_reg.value);
    if(err)
    {
        return err;
    }

    return err;
}


/*! \brief Enables PCI interrupts of APU devices
 *
 * @param intf The interface to enable interrupts of
 * @return 0 on success, a negative error code on failure
 */
static __inline__ int enable_pci_interrupts(struct aim_usb_interface* intf)
{
    int err;

    BUG_ON(!intf);

    aim_usb_intf_dbg(intf, "Enabling PCI interrupts");

    /* Clear pending interrupts */
    err = aim_usb_ncc_pci_write(&intf->ncc, intf->io_memory.bus_address + (APU_IRQ_EVENT_REG_OFFSET * 4),
                                APU_IRQ_EVENTS);
    if(err)
    {
        return err;
    }

    /* Set BIU Interrupt Mask (0=Enabled)*/
    err = aim_usb_ncc_pci_write(&intf->ncc, intf->io_memory.bus_address + (APU_IRQ_MASK_REG_OFFSET * 4),
                                ~APU_IRQ_EVENTS);
    if(err)
    {
        return err;
    }

    /* The highest bit in the event register is used to enable and disable the
       device interrupts

        0x80000000 => interrupts enabled  (PMC_IRG_EVENT_ENABLE_BITMASK)
        0x00000000 => interrupts disabled
     */
     err = aim_usb_ncc_pci_write(&intf->ncc, intf->io_memory.bus_address + (APU_IRQ_EVENT_REG_OFFSET * 4),
                                 APU_IRQ_EVENT_ENABLE);
     if(err)
     {
         return err;
     }

     return err;
}


/*! \brief Enables interrupts for APU devices
 *
 * @param intf The interface to enable interrupts for
 * @return 0 on success, a negative errno code otherwise
 */
static int enable_interrupts(struct aim_usb_interface* intf)
{
    int err;

    BUG_ON(!intf);

    aim_usb_intf_dbg(intf, "Enabling APU interrupts");

    err = enable_nc_interrupts(intf);
    if(err)
    {
        return err;
    }

    /* Set BIU Interrupt Mask (0=Enabled)*/
    err = enable_pci_interrupts(intf);
    if(err)
    {
        return err;
    }

    return err;
}


/*! \brief Disables interrupts on APU devices
 *
 * @param intf The interface to disable interrupts of
 */
static void disable_interrupts(struct aim_usb_interface* intf)
{
    union nc_usb_irq_reg irq_reg;

    BUG_ON(!intf);

    aim_usb_intf_dbg(intf, "Disabling APU interrupts");

    aim_usb_ncc_reg_read(&intf->ncc, NC_USB_IRQ_REG_ADDR, &irq_reg.value);

    irq_reg.fields.pci_int_a_enable = 0;
    irq_reg.fields.usb_interrupt_enable = 0;

    aim_usb_ncc_reg_write(&intf->ncc, NC_USB_IRQ_REG_ADDR, irq_reg.value);
}



/*! \brief Switches on the Power LED on APU devices
 *
 * @param intf the APU interface to switch on LED for
 */
static void __inline__ switch_on_led(struct aim_usb_interface* intf)
{
    union nc_gpio_ctrl_reg gpio_reg;

    BUG_ON(!intf);

    aim_usb_intf_dbg(intf, "Switch on Power LED");

    aim_usb_ncc_reg_read(&intf->ncc, NC_GPIO_CTRL_REG_ADDR, &gpio_reg.value);

    gpio_reg.fields.gpio3_out_enable = 1;
    gpio_reg.fields.gpio3_data = 0;
    gpio_reg.fields.gpio3_irq_enable = 0;
    gpio_reg.fields.gpio3_led_select = 0;

    aim_usb_ncc_reg_write(&intf->ncc, NC_GPIO_CTRL_REG_ADDR, gpio_reg.value);
}


/*! \brief Switches on the Power LED on APU devices
 *
 * @param intf the APU interface to switch off LED for
 */
static void __inline__ switch_off_led(struct aim_usb_interface* intf)
{
    union nc_gpio_ctrl_reg gpio_reg;

    BUG_ON(!intf);

    aim_usb_intf_dbg(intf, "Switch off Power LED");

    aim_usb_ncc_reg_read(&intf->ncc, NC_GPIO_CTRL_REG_ADDR, &gpio_reg.value);

    gpio_reg.fields.gpio3_out_enable = 0;
    gpio_reg.fields.gpio3_data = 0;
    gpio_reg.fields.gpio3_irq_enable = 0;
    gpio_reg.fields.gpio3_led_select = 0;

    aim_usb_ncc_reg_write(&intf->ncc, NC_GPIO_CTRL_REG_ADDR, gpio_reg.value);
}


int aim_usb_apu_hw_start(struct aim_usb_interface* intf)
{
    int err;

    BUG_ON(!intf);

    aim_usb_intf_dbg(intf, "Starting APU");

    err = 0;

    do
    {
        err = setup_pci_access(intf);
        if(err)
        {
            aim_usb_intf_err(intf, "Failed to map APU bar registers");
            break;
        }

        err = start_tcp(intf);
        if(err)
        {
            aim_usb_intf_err(intf, "Failed to start APU TCP");
            break;
        }

        err = novram_check(intf);
        if(err)
        {
            aim_usb_intf_err(intf, "NOVRAM check failed");
            break;
        }

        err = biu_start(intf);
        if(err)
        {
            aim_usb_intf_err(intf, "BIU start failed");
            break;
        }

        err = global_memory_mirror_create(intf);
        if(err)
        {
            aim_usb_intf_err(intf, "Creation of Global Memory Mirror failed\n");
            break;
        }

        err = enable_interrupts(intf);
        if(err)
        {
            aim_usb_intf_err(intf, "Failed to enable interrupts");
            break;
        }

        err = aim_usb_apu_novram_get(intf, NVR_SERIAL_ADDR, &intf->serial);
        if(err)
        {
            aim_usb_intf_err(intf, "Failed to read serial number from NOVRAM");
            break;
        }

        /* only 2 bytes are valid as serial number */
        intf->serial = intf->serial & 0xFFFF;

        err = aim_usb_apu_novram_get(intf, NVR_BOARD_CONFIG_ADDR, &intf->board_config);
        if(err)
        {
            aim_usb_intf_err(intf, "Failed to read board config from NOVRAM");
            break;
        }

        err = aim_usb_apu_novram_get(intf, NVR_BOARD_TYPE_ADDR, &intf->board_type);
        if(err)
        {
            aim_usb_intf_err(intf, "Failed to read board type from NOVRAM");
            break;
        }

        err = aim_usb_apu_novram_get(intf, NVR_BOARD_SUB_TYPE_ADDR, &intf->board_sub_type);
        if(err)
        {
            aim_usb_intf_err(intf, "Failed to read board sub type from NOVRAM");
            break;
        }

        err = aim_usb_apu_novram_get(intf, NVR_BOARD_PART_NO_ADDR, &intf->board_part_no);
        if(err)
        {
            aim_usb_intf_err(intf, "Failed to read board part no from NOVRAM");
            break;
        }

        err = aim_usb_apu_novram_get(intf, NVR_BOARD_HW_VARIANT_ADDR, &intf->hw_variant);
        if(err)
        {
            aim_usb_intf_err(intf, "Failed to read hw variant from NOVRAM");
            break;
        }


        aim_usb_intf_dbg(intf, "Serial: %d", intf->serial);
        aim_usb_intf_dbg(intf, "board config 0x%x", intf->board_config);
        aim_usb_intf_dbg(intf, "board type 0x%x", intf->board_type);

        switch_on_led(intf);

    }while(0);

    return err;
}


void aim_usb_apu_hw_stop(struct aim_usb_interface* intf)
{
    BUG_ON(!intf);

    aim_usb_intf_dbg(intf, "Stopping APU device");

    disable_interrupts(intf);

    if(intf->global_memory.base)
    {
        aim_usb_intf_dbg(intf, "Destroying APU memory mirror");
        kfree(intf->global_memory.base);
        intf->global_memory.base = NULL;
    }

    biu_stop(intf);

    switch_off_led(intf);
}


/*! \brief Reads data from PCI mapped memory space on APU devices
 *
 * @param intf The APU interface to read memory from
 * @param address the PCI address to read from
 * @param data buffer to read data into
 * @param len number of bytes to read
 * @return 0 on success, errno code on failure
 */
static int pci_space_read(struct aim_usb_interface* intf, AiUInt32 address, void* data, size_t len)
{
    int err;

    BUG_ON(!intf || !data);

    aim_usb_intf_dbg(intf, "APU PCI space read address %u len %zu", address, len);

    if(len == sizeof(AiUInt32) && IS_32BIT_ALIGNED(address))
    {
        err =  aim_usb_ncc_pci_read(&intf->ncc, address, (AiUInt32*) data);
        if(err)
        {
            return -EIO;
        }

        return 0;
    }
    else
    {
        /* For sizes > 32bit or unaligned access use the Netchip DMA engine */
        return aim_usb_ncdmac_read(&intf->ncdmac, address, data, len);
    }
}


/*! \brief Writes data to PCI mapped memory space on APU devices
 *
 * @param intf The APU interface to write memory to
 * @param address the PCI address to write to
 * @param data buffer to write data from
 * @param len number of bytes to write
 * @return 0 on success, errno code on failure
 */
static int pci_space_write(struct aim_usb_interface* intf, AiUInt32 address, const void* data, size_t len)
{
    int err;

    BUG_ON(!intf || !data);

    aim_usb_intf_dbg(intf, "APU PCI space write addr %d len %zu", address, len);

    if(len == sizeof(AiUInt32) && IS_32BIT_ALIGNED(address))
    {
        err =  aim_usb_ncc_pci_write(&intf->ncc, address, *((AiUInt32*) data));
        if(err)
        {
            return -EIO;
        }

        return 0;
    }
    else
    {
        /* For sizes > 32bit or unaligned access use the Netchip DMA engine */
        return aim_usb_ncdmac_write(&intf->ncdmac, address, (void*) data, len);
    }
}


int aim_usb_apu_hw_read(struct aim_usb_interface* intf, void* buffer, size_t len, TY_E_MEM_TYPE mem_type,
                        loff_t offset)
{

    BUG_ON(!intf || !buffer);

    aim_usb_intf_dbg(intf, "Reading memory on APU");

    switch (mem_type)
    {
        case AI_MEMTYPE_SHARED:
           if(len + offset > intf->shared_memory.size)
           {
               aim_usb_intf_err(intf, "Shared memory read out of bounds");
               return -ESPIPE;
           }
           memcpy(buffer, intf->shared_memory.base + offset, len);
           return 0;

        case AI_MEMTYPE_GLOBAL:
            if(len + offset > intf->global_memory.size)
            {
                aim_usb_intf_err(intf, "Global memory read out of bounds");
                return -ESPIPE;
            }

            intf->ioctl_ops->sync_active_memory( intf, offset, len );

            memcpy(buffer, intf->global_memory.base + offset, len);
            return 0;

        case AI_MEMTYPE_IO:
            if(len + offset > intf->io_memory.size)
            {
                aim_usb_intf_err(intf, "I/O memory read out of bounds");
                return -ESPIPE;
            }
            return pci_space_read(intf, intf->io_memory.bus_address + offset, buffer, len);

        case AI_MEMTYPE_GLOBAL_DIRECT:
            if(len + offset > intf->global_memory.size)
            {
                aim_usb_intf_err(intf, "Global memory read out of bounds");
                return -ESPIPE;
            }
            return pci_space_read(intf, intf->global_memory.bus_address + offset, buffer, len);

        default:
            aim_usb_intf_err(intf, "No valid memory type specified");
            return -EINVAL;
        }
}


int aim_usb_apu_hw_write(struct aim_usb_interface* intf, const void* buffer, size_t len, TY_E_MEM_TYPE mem_type,
                         loff_t offset)
{
    BUG_ON(!intf || !buffer);

    aim_usb_intf_dbg(intf, "Writing memory on APU");

    switch (mem_type)
    {
        case AI_MEMTYPE_SHARED:
            if(len + offset > intf->shared_memory.size)
            {
                aim_usb_intf_err(intf, "Shared memory write out of bounds");
                return -ESPIPE;
            }
            memcpy(intf->shared_memory.base + offset, buffer, len);
            return 0;

        case AI_MEMTYPE_IO:
            if(len + offset > intf->io_memory.size)
            {
                aim_usb_intf_err(intf, "I/O memory write out of bounds");
                return -ESPIPE;
            }
            return pci_space_write(intf, intf->io_memory.bus_address + offset, buffer, len);

        case AI_MEMTYPE_GLOBAL:
        case AI_MEMTYPE_GLOBAL_DIRECT:
            if(len + offset > intf->global_memory.size)
            {
                aim_usb_intf_err(intf, "Global memory write out of bounds");
                return -ESPIPE;
            }
            memcpy(intf->global_memory.base + offset, buffer, len);
            return pci_space_write(intf, intf->global_memory.bus_address + offset, buffer, len);

        default:
            aim_usb_intf_err(intf, "No valid memory type specified");
            return -EINVAL;
    }
}


int aim_usb_apu_novram_set(struct aim_usb_interface* intf, AiUInt32 address, AiUInt32 value)
{
    int i;
    AiUInt32 jtagtmp;
    int err;

    err = aim_usb_apu_tcp_reg_write(intf, APU_TCP_EEPROM_CTRL, APU_TCP_EEPROM_WRENA); /* EEProm write enable */
    if(err)
    {
        return err;
    }

    for(i=0; i<4; i++)
    {
        jtagtmp=0xff;

        while((jtagtmp & APU_TCP_EEPROM_BUSY) != 0)
        {
            err = aim_usb_apu_tcp_reg_read(intf, APU_TCP_EEPROM_STATUS, &jtagtmp);
            if(err)
            {
                return err;
            }
        }

        err = aim_usb_apu_tcp_reg_write(intf,APU_TCP_ADDRLO, address & 0xff);
        if(err)
        {
            return err;
        }

        err = aim_usb_apu_tcp_reg_write(intf, APU_TCP_ADDRHI, (address >> 8) & 0x1);
        if(err)
        {
            return err;
        }

        err = aim_usb_apu_tcp_reg_write(intf, APU_TCP_EEPROM_WRITE, value & 0xFF);
        if(err)
        {
            return err;
        }

        value = value >> 8;
        address++;

    } /*end for*/

    do
    {
        err = aim_usb_apu_tcp_reg_read(intf, APU_TCP_EEPROM_STATUS, &jtagtmp);
        if(err)
        {
            return err;
        }
    }
    while((jtagtmp & APU_TCP_EEPROM_BUSY) != 0);       /*ML*/

    /* EEProm write disable */
    err = aim_usb_apu_tcp_reg_write(intf, APU_TCP_EEPROM_CTRL, 0);
    if(err)
    {
        return err;
    }

    return err;
}


int aim_usb_apu_pci_int_ack(struct aim_usb_interface* intf, AiUInt32 * event)
{
    int err;
    union nc_irqstat1_reg irq_status = {0};

    BUG_ON(!intf);

    aim_usb_intf_dbg(intf, "Acknowledging APU interrupt");

    if(event)
    {
        err = aim_usb_ncc_pci_read(&intf->ncc, intf->io_memory.bus_address + (APU_IRQ_EVENT_REG_OFFSET * 4), event);

        if(err)
        {
            return err;
        }
    }

    /* Acknowledge all BIU interrupts */
    err = aim_usb_ncc_pci_write(&intf->ncc, intf->io_memory.bus_address + (APU_IRQ_EVENT_REG_OFFSET * 4),
                                (APU_IRQ_EVENTS | APU_IRQ_EVENT_ENABLE));
    if(err)
    {
        return err;
    }

    /* Acknowledge Netchip PCI INT A USB interrupt */
    irq_status.fields.pci_int_a_irq = 1;

    err = aim_usb_ncc_reg_write(&intf->ncc, NC_IRQSTAT1_REG_ADDR, irq_status.value);
    if(err)
    {
        return err;
    }

    return err;
}




