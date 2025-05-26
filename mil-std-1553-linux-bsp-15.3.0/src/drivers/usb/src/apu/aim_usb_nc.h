/* SPDX-FileCopyrightText: 2017-2021 AIM GmbH <info@aim-online.com> */
/* SPDX-License-Identifier: GPL-2.0-or-later */

/*! \file aim_usb_nc.h
 *
 */

#ifndef AIM_USB_NC_H_
#define AIM_USB_NC_H_


#include <linux/types.h>
#include <linux/mutex.h>
#include "Ai_types.h"



/*! \def NC_USB_IRQ_REG_ADDR
 * Address of Netchip USB Interrupt register
 */
#define NC_USB_IRQ_REG_ADDR 0x24


/*! \def NC_IRQSTAT1_REG_ADDR
 * Address of Netchip Interrupt Status 1 register
 */
#define NC_IRQSTAT1_REG_ADDR 0x2C


/*! \def NC_GPIO_CTRL_REG_ADDR
 * Addres of GPIO control register on Netchip
 */
#define NC_GPIO_CTRL_REG_ADDR 0x50




/*! \union nc_gpio_ctrl_reg
 *
 * This union describes all available fields
 * in the Netchip GPIO control register
 */
union nc_gpio_ctrl_reg
{
    AiUInt32 value;

    struct
    {
        AiUInt32 gpio0_data         : 1;
        AiUInt32 gpio1_data         : 1;
        AiUInt32 gpio2_data         : 1;
        AiUInt32 gpio3_data         : 1;
        AiUInt32 gpio0_out_enable   : 1;
        AiUInt32 gpio1_out_enable   : 1;
        AiUInt32 gpio2_out_enable   : 1;
        AiUInt32 gpio3_out_enable   : 1;
        AiUInt32 gpio0_irq_enable   : 1;
        AiUInt32 gpio1_irq_enable   : 1;
        AiUInt32 gpio2_irq_enable   : 1;
        AiUInt32 gpio3_irq_enable   : 1;
        AiUInt32 gpio3_led_select   : 1;
        AiUInt32 reserved           : 19;
    }fields;
};


/*! \union nc_usb_irq_reg
 *
 * This union describes all available fields
 * in the Netchip USB Interrupt register
 */
union nc_usb_irq_reg
{
    AiUInt32 value;

    struct
    {
        AiUInt32 sof_enable : 1;
        AiUInt32 resume_enable : 1;
        AiUInt32 suspend_request_change_enable : 1;
        AiUInt32 suspend_request_enable : 1;
        AiUInt32 root_port_reset_enable : 1;
        AiUInt32 reserved : 1;
        AiUInt32 control_status_enable : 1;
        AiUInt32 vbus_enable : 1;
        AiUInt32 eeprom_done_enable : 1;
        AiUInt32 dma_a_enable : 1;
        AiUInt32 dma_b_enable : 1;
        AiUInt32 dma_c_enable : 1;
        AiUInt32 dma_d_enable : 1;
        AiUInt32 gpio_enable : 1;
        AiUInt32 reserved2 : 2;
        AiUInt32 pci_master_cycle_done : 1;
        AiUInt32 pci_retry_abort_enable : 1;
        AiUInt32 reserved3 : 1;
        AiUInt32 pci_target_abort_enable : 1;
        AiUInt32 pci_master_abort_enable : 1;
        AiUInt32 pci_perr_enable : 1;
        AiUInt32 pci_serr_enable : 1;
        AiUInt32 pci_pme_enable : 1;
        AiUInt32 pci_int_a_enable : 1;
        AiUInt32 pci_parity_error_enable : 1;
        AiUInt32 pci_arbiter_timeout_enable : 1;
        AiUInt32 power_state_change_enable : 1;
        AiUInt32 reserved4 : 3;
        AiUInt32 usb_interrupt_enable : 1;
    }fields;
};


/*! \union nc_irqstat1_reg
 *
 * This union describes all fields in the Netchip IRQSTAT1 register
 */
union nc_irqstat1_reg
{
    AiUInt32 value;

    struct
    {
        AiUInt32 sof_irq :1;
        AiUInt32 resume_irq :1;
        AiUInt32 suspend_request_change_irq :1;
        AiUInt32 suspend_request_irq :1;
        AiUInt32 root_port_reset_irq :1;
        AiUInt32 reserved :1;
        AiUInt32 control_status_irq :1;
        AiUInt32 vbus_irq :1;
        AiUInt32 eeprom_done_irq :1;
        AiUInt32 dma_a_irq :1;
        AiUInt32 dma_b_irq :1;
        AiUInt32 dma_c_irq :1;
        AiUInt32 dma_d_irq :1;
        AiUInt32 gpio_irq :1;
        AiUInt32 reserved2 :2;
        AiUInt32 pci_master_cycle_done :1;
        AiUInt32 pci_retry_abort_irq :1;
        AiUInt32 reserved3 :1;
        AiUInt32 pci_target_abort_irq :1;
        AiUInt32 pci_master_abort_irq :1;
        AiUInt32 pci_perr_irq :1;
        AiUInt32 pci_serr_irq :1;
        AiUInt32 pci_pme_irq :1;
        AiUInt32 pci_int_a_irq :1;
        AiUInt32 pci_parity_error_irq :1;
        AiUInt32 pci_arbiter_timeout_irq :1;
        AiUInt32 power_state_change_irq :1;
        AiUInt32 reserved4 :4;
    }fields;
};




/*! \struct aim_usb_ncc
 *
 * This structure comprises settings
 * for accessing a Netchip 2280 USB controller
 */
struct aim_usb_ncc
{
    struct aim_usb_com_channel* config_channel; /*!< The configuration channel to access Netchip registers */
    struct aim_usb_com_channel* pci_channel;    /*!< The PCI channel for single read/write access to PCI bus */
    struct aim_usb_com_channel* fifo_channel;   /*!< The FIFO channel for PCI DMA read/write transactions */

    void* buffer;                               /*!< buffer used for URB requests */
    struct mutex lock;                          /*!< mutex for synchronizing access to the controller */
};


/*! \struct aim_usb_ncdmac
 *
 * This structure defines a DMA controller instance
 * for writing/reading PCI space on Netchip based devices
 */
struct aim_usb_ncdmac
{
    struct aim_usb_ncc* ncc;  /*!< The Netchip controller used for setting/up and processing DMA transactions */
    struct mutex lock;        /*!< Mutex for locking access to the DMA controller */
};


/*! \brief Initialize Netchip controller
 *
 * @param ncc the Netchip controller instance to initialize
 * @param config_channel the config channel to use for the controller
 * @param pci_channel the pci channel to use for the controller
 * @param fifo_channel the FIFO channel for DMA access to use for the controller
 * @return 0 on success, errno code on failure
 */
extern int aim_usb_ncc_init(struct aim_usb_ncc* ncc, struct aim_usb_com_channel* config_channel,
                            struct aim_usb_com_channel* pci_channel, struct aim_usb_com_channel* fifo_channel);


/*! \brief Free resources of Netchip controller
 *
 * @param ncc the Netchip controller instance to free resources of
 */
extern void aim_usb_ncc_free(struct aim_usb_ncc* ncc);


/*! \brief Write PCI config space register via Netchip controller
 *
 * @param ncc the Netchip controller to use for write
 * @param address the PCI config space register address to write
 * @param value the value to write
 * @return 0 on success, errno code on failure
 */
extern int aim_usb_ncc_pci_config_write(struct aim_usb_ncc* ncc, AiUInt32 address, AiUInt32 value);


/*! \brief Read PCI config space register via Netchip controller
 *
 * @param ncc the Netchip controller to use for read
 * @param address the PCI config space register address to read
 * @param value the value read will be stored here
 * @return 0 on success, errno code on failure
 */
extern int aim_usb_ncc_pci_config_read(struct aim_usb_ncc* ncc, AiUInt32 address, AiUInt32* value);


/*! \brief Read 32-Byte value from specific PCI address via Netchip controller
 *
 * @param ncc the Netchip controller to use
 * @param address the PCI address to read from
 * @param value value read will be stored here
 * @return 0 on success, errno code on failure
 */
extern int aim_usb_ncc_pci_read(struct aim_usb_ncc* ncc, AiUInt32 address, AiUInt32* value);


/*! \brief Write 32-Byte value to specific PCI address via Netchip controller
 *
 * @param ncc the Netchip controller to use
 * @param address the PCI address to write to
 * @param value value to write
 * @return 0 on success, errno code on failure
 */
extern int aim_usb_ncc_pci_write(struct aim_usb_ncc* ncc, AiUInt32 address, AiUInt32 value);


/*! \brief Writes Netchip configuration register
 *
 * @param ncc the Netchip controller to use for write
 * @param address the configuration register address to write
 * @param value the value to write
 * @return 0 on success, errno code on failure
 */
extern int aim_usb_ncc_reg_write(struct aim_usb_ncc* ncc, AiUInt16 address, AiUInt32 value);


/*! \brief Reads Netchip configuration register
 *
 * @param ncc the Netchip controller to use for read
 * @param address the configuration register address to read
 * @param value the value read will be stored here
 * @return 0 on success, errno code on failure
 */
extern int aim_usb_ncc_reg_read(struct aim_usb_ncc* ncc, AiUInt16 address, AiUInt32* value);


/*! \brief Writes to Netchip DMA FIFO endpoint
 *
 * @param ncc the Netchip controller to use for write
 * @param data the data to write
 * @param len number of bytes to write
 * @return 0 on success, negative errno code on failure
 */
extern int aim_usb_ncc_fifo_write(struct aim_usb_ncc* ncc, void* data, size_t len);


/*! \brief Reads from Netchip DMA FIFO endpoint
 *
 * @param ncc the Netchip controller to use for read
 * @param data the data to read
 * @param len number of bytes to read
 * @return 0 on success, negative errno code on failure
 */
extern int aim_usb_ncc_fifo_read(struct aim_usb_ncc* ncc, void* data, size_t len);


/*! \brief Initialize Netchip DMA controller
 *
 * @param dmac the DMA controller to initialize
 * @param ncc the Netchip controller to use for DMA transactions
 * @return 0 on success, errno code on failure
 */
extern int aim_usb_ncdmac_init(struct aim_usb_ncdmac* dmac, struct aim_usb_ncc* ncc);


/*! \brief Frees Netchip DMA controller instance
 *
 * @param ncdmac the Netchip DMA controller instance to free
 */
extern void aim_usb_ncdmac_free(struct aim_usb_ncdmac* ncdmac);


/*! \brief Write data to specific PCI address using Netchip DMA controller
 *
 * @param ncdmac Netchip DMA controller to use for write
 * @param address the PCI address to write to
 * @param data data to write
 * @param len number of bytes to write
 * @return 0 on success, errno code on failure
 */
extern int aim_usb_ncdmac_write(struct aim_usb_ncdmac* ncdmac, AiUInt32 address, void* data, size_t len);


/*! \brief Read data from specific PCI address using Netchip DMA controller
 *
 * @param ncdmac Netchip DMA controller to use for read
 * @param address the PCI address to read from
 * @param data the buffer data will be read to
 * @param len number of bytes to read
 * @return 0 on success, errno code on failure
 */
extern int aim_usb_ncdmac_read(struct aim_usb_ncdmac* ncdmac, AiUInt32 address, void* data, size_t len);




#endif /* AIM_USB_NC_H_ */
