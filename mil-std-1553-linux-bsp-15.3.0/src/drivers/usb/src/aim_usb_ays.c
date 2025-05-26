/* SPDX-FileCopyrightText: 2017-2024 AIM GmbH <info@aim-online.com> */
/* SPDX-License-Identifier: GPL-2.0-or-later */

/*! \file aim_usb_ays.c
 *
 */


#include <linux/slab.h>
#include "aim_usb.h"
#include "aim_usb_ays.h"
#include "aim_usb_com_channel.h"
#include "esmart.h"




/*! \def AYS_EXPECTED_MAJOR_VERSION_STR
 * The AYS interface major version string supported by the driver
 */
#define AYS_EXPECTED_MAJOR_VERSION_STR "7"


/*! \def AYS_MAJOR_VER_STRING_DESC_IDX
 * Index in string descriptor table were AYS interface major version is encoded
 */
#define AYS_MAJOR_VER_STRING_DESC_IDX 4


/*! \def AYS_COMMAND_EP_NUMBER
 * Number of the USB endpoints on the AYS that is used for issuing commands
 */
#define AYS_COMMAND_EP_NUMBER 1


/*! \def AYS_INTERRUPT_EP_NUMBER
 * Number of USB interrupt endpoint on AYS
 */
#define AYS_INTERRUPT_EP_NUMBER 2


/*! \def AYS_MAX_COMMAND_EP_SIZE
 * Maximum size of USB transfers in bytes that may be used on
 * the AYS command endpoints
 */
#define AYS_MAX_COMMAND_EP_SIZE 0x4000UL

/*! \def AYS_MAX_COMMAND_EXPANDED_EP_SIZE
 * Maximum size of an expanded USB transfer, the board has to be informed that a longer command is incomming
 */
#define AYS_MAX_COMMAND_EXPANDED_EP_SIZE 0xC000UL

/*! \def AYS_COMMAND_CHANNEL_ID
 * ID of command communication channel on AYS devices
 */
#define AYS_COMMAND_CHANNEL_ID 0


/*! \def AYS_USB_COMMAND_MAGIC
 * Magic number used to identify all AYS USB commands
 */
#define AYS_USB_COMMAND_MAGIC 0x45434D44


/*! \def AYS_USB_COMMAND_INIT
 * Basic initialization of AYS USB commands. Just sets the magic number
 */
#define AYS_USB_COMMAND_INIT(command) \
    { memset((command), 0, sizeof(struct ays_usb_command_header)); (command)->magic = AYS_USB_COMMAND_MAGIC;}


/*! \def AYS_BOARD_CONFIG_NVR_ADDR
 * Address of Board Config NOVRAM value on AYS platform
 */
#define AYS_BOARD_CONFIG_NVR_ADDR 0x8


/*! \def AYS_BOARD_TYPE_NVR_ADDR
 * Address of Board Type NOVRAM value on AYS platform
 */
#define AYS_BOARD_TYPE_NVR_ADDR 0x10


/*! \def AYS_SERIAL_NVR_ADDR
 * Address of Serial NOVRAM value on AYS platform
 */
#define AYS_SERIAL_NVR_ADDR 0x4


/*! \def AYS_BOARD_PART_NO_NVR_ADDR
 * Address of Board Part No NOVRAM value on AYS platform
 */
#define AYS_BOARD_PART_NO_NVR_ADDR 0x1C

/*! \def AYS_HW_VARIANT_NVR_ADDR
 * Address of HW Variant NOVRAM value on AYS platform
 */
#define AYS_HW_VARIANT_NVR_ADDR 0xC

/*! \def AYS_GLOBAL_MEMORY_SIZE
 * Size of global memory on AYS in bytes
 */
#define AYS_GLOBAL_MEMORY_SIZE  (8 << 20)


/*! \def AYS_IO_MEMORY_SIZE
 * Size of IO memory on AYS in bytes
 */
#define AYS_IO_MEMORY_SIZE  (256 << 10)

/*! \def AYS_INPUT_OFFSET
 * Legacy TSW Values, they expect this to be set !!!!!
 */
#define AYS_INPUT_OFFSET 0x5000

/*! \def AYS_OUTPUT_OFFSET
 * Legacy TSW Values, they expect this to be set
 */
#define AYS_OUTPUT_OFFSET 0x2000


/*! \enum ays_usb_command_id
 * Enumeration of all possible AYS USB commands
 */
enum ays_usb_command_id
{
    AYS_USB_CMD_ACK,             //!< AYS_USB_CMD_ACK
    AYS_USB_CMD_MEMORY,          //!< AYS_USB_CMD_MEMORY
    AYS_USB_CMD_TSW,             //!< AYS_USB_CMD_TSW
    AYS_USB_CMD_HOST_IO,         //!< AYS_USB_CMD_HOST_IO
    AYS_USB_CMD_HOST_IO_COMMAND, //!< AYS_USB_CMD_HOST_IO_COMMAND
    AYS_USB_CMD_NOVRAM_READ,      //!< AYS_USB_CMD_NOVRAM_READ
    AYS_USB_CMD_LENGTH,             //!< AYS_USB_CMD_LENGTH
    AYS_USB_CMD_LENGTH_HANDSHAKE        //!< AYS_USB_CMD_LENGTH_HANDSHAKE
};


/*! \enum ays_usb_direction
 * Enumeration of all possible direction types used by AYS USB commands
 */
enum ays_usb_direction
{
    AYS_USB_DIRECTION_IN,       //!< AYS_USB_DIRECTION_IN
    AYS_USB_DIRECTION_OUT,      //!< AYS_USB_DIRECTION_OUT
    AYS_USB_DIRECTION_INOUT,    //!< AYS_USB_DIRECTION_INOUT
};


/*! \struct ays_usb_command_header
 *
 * This structure defines a command header
 * that is used by all AYS USB commands.
 */
struct ays_usb_command_header
{
    AiUInt32 magic;         /*!< Magic number that identifies the command header */
    AiUInt32 id;            /*!< ID of command. */
    AiUInt32 direction;     /*!< Direction field. Use is command specific */
    AiUInt32 type;          /*!< Type field. Use is command specific */
    AiUInt32 in_offset;     /*!< Command specific field for in offset*/
    AiUInt32 in_size;       /*!< Command specific field for in size */
    AiUInt32 out_offset;    /*!< Command specific field for out offset*/
    AiUInt32 out_size;      /*!< Command specific field for out size*/
};

/*! \static u32 aim_ays_usb_transfer_size
 *
 * This variable defines the expanded size of usb commands used for checks.
 * Set to the standard 0x4000 and can be (and should only) be changed by aim_usb_length_handshake_command(..)
 */
static u32 aim_ays_usb_transfer_size = AYS_MAX_COMMAND_EP_SIZE;


int aim_usb_ays_hw_init(struct aim_usb_interface* aim_intf)
{
    struct usb_device* usb_device;
    char string_buf[32];
    int err;
    struct aim_usb_com_channel* cmd_channel;
    struct usb_host_endpoint* cmd_ep = NULL;
    struct usb_host_endpoint* response_ep = NULL;
    struct usb_host_endpoint* interrupt_ep = NULL;
    struct usb_host_interface* interface;
    int i;
    struct aim_usb_int_ep* interrupt_endpoint;

    aim_usb_intf_dbg(aim_intf, "AYS hardware initialization");

    aim_intf->has_asp = AiTrue;

    usb_device = interface_to_usbdev(aim_intf->interface);
    interface = aim_intf->interface->cur_altsetting;

    err = usb_string(usb_device, AYS_MAJOR_VER_STRING_DESC_IDX, string_buf, sizeof(string_buf));
    if(err < 0)
    {
        aim_usb_intf_err(aim_intf, "Failed to get AYS interface major number");
        return err;
    }

    aim_usb_intf_dbg(aim_intf, "AYS major interface version: %s", string_buf);

    if(strncmp(string_buf, AYS_EXPECTED_MAJOR_VERSION_STR, sizeof(string_buf)))
    {
        aim_usb_intf_err(aim_intf, "Incompatible AYS interface major version: %s", string_buf);
        return -EPFNOSUPPORT;
    }

    for(i = 0; i < interface->desc.bNumEndpoints; i++)
    {
        aim_usb_intf_dbg(aim_intf, "Found endpoint with addr: 0x%x", interface->endpoint[i].desc.bEndpointAddress);

        if(usb_endpoint_num(&interface->endpoint[i].desc) == AYS_COMMAND_EP_NUMBER)
        {
            if(usb_endpoint_is_bulk_in(&interface->endpoint[i].desc))
            {
                response_ep = &interface->endpoint[i];
            }
            else if(usb_endpoint_is_bulk_out(&interface->endpoint[i].desc))
            {
                cmd_ep = &interface->endpoint[i];
            }
        }
        else if(usb_endpoint_num(&interface->endpoint[i].desc) == AYS_INTERRUPT_EP_NUMBER)
        {
            if(usb_endpoint_is_int_in(&interface->endpoint[i].desc))
            {
                interrupt_ep = &interface->endpoint[i];
            }

        }
    }


    if(!cmd_ep || !response_ep)
    {
        aim_usb_intf_err(aim_intf, "AYS: Could not find mandatory endpoints");
        return -ENODEV;
    }

    if(!interrupt_ep)
    {
        aim_usb_intf_warn(aim_intf, "No interrupt endpoint detected");
    }

    cmd_channel = aim_usb_interface_create_com_channel(aim_intf, AYS_COMMAND_CHANNEL_ID, cmd_ep, response_ep);

    if(IS_ERR(cmd_channel))
    {
        aim_usb_intf_err(aim_intf, "AYS: Could not find mandatory endpoints");
        return PTR_ERR(cmd_channel);
    }

    interrupt_endpoint = aim_usb_interface_create_int_ep(aim_intf, interrupt_ep);
    if(IS_ERR(interrupt_endpoint))
    {
        aim_usb_intf_err(aim_intf, "AYS: Failed to set up interrupt endpoint");
        return PTR_ERR(interrupt_endpoint);
    }

    if(aim_intf->platform == AI_DEVICE_AYS_ASP )
    {
        /* The Zynq based UDC can transfer a maximum of AYS_MAX_COMMAND_EP_SIZE per USB transfer. 
           AYS_MAX_COMMAND_EP_SIZE is also the size of the buffer allocated for each USB device request.
           We do not send a ZLP package if we transfer exactly AYS_MAX_COMMAND_EP_SIZE bytes of data. */
        cmd_channel->max_transfer_length = aim_ays_usb_transfer_size;
    }
    else
    {
        /* The ZynqMP based DWC3 UDC can transfer more than AYS_MAX_COMMAND_EP_SIZE per USB transfer. 
           AYS_MAX_COMMAND_EP_SIZE is still the size of the buffer allocated for each USB device request.
           We have to send a ZLP package even if we transfer AYS_MAX_COMMAND_EP_SIZE bytes of data. */
        cmd_channel->max_transfer_length = 0; // unknown but >AYS_MAX_COMMAND_EP_SIZE 
    }


    return 0;
}


int aim_usb_ays_hw_read(struct aim_usb_interface* aim_intf, void* buffer, size_t len, TY_E_MEM_TYPE mem_type,
                        loff_t offset)
{
    struct aim_usb_com_channel* com_channel = NULL;
    size_t transferred_bytes = 0;
    struct ays_usb_command_header* command_buffer = NULL;
    int err = 0;
    ssize_t bytes_read;

    BUG_ON(!aim_intf || !buffer);

    aim_usb_intf_dbg(aim_intf, "Using AYS raw data read");

    com_channel = aim_usb_interface_get_com_channel(aim_intf, AYS_COMMAND_CHANNEL_ID);
    if(IS_ERR(com_channel))
    {
        return -ENODEV;
    }

    /* Allocate USB buffer where read command can be put to.
     * The actual data read will be directly written to the buffer given as argument to this function
     */
    command_buffer = kmalloc(sizeof(struct ays_usb_command_header), GFP_KERNEL);
    if(!command_buffer)
    {
        aim_usb_intf_err(aim_intf, "Failed to allocate command buffer");
        return -ENOMEM;
    }

    AYS_USB_COMMAND_INIT(command_buffer);

    command_buffer->id = AYS_USB_CMD_MEMORY;
    command_buffer->direction = AYS_USB_DIRECTION_IN;
    command_buffer->type = mem_type;

    /* The AYS command endpoints have a maximum transfer size.
     * So we might have to split up the data transfer to multiple chunks
     */
    transferred_bytes = 0;
    err = 0;
    while(transferred_bytes < len)
    {
        command_buffer->in_offset = offset + transferred_bytes;

        /* Either set size of bytes to transfer to number of bytes left or
         * if these exceed maximum transfer size to maximum transfer size */
        command_buffer->in_size = min(aim_ays_usb_transfer_size, len - transferred_bytes);

        aim_usb_intf_dbg(aim_intf, "Reading chunk with offset %u and size %u", command_buffer->in_offset,
                         command_buffer->in_size);

        bytes_read = aim_usb_com_channel_issue_cmd(com_channel, command_buffer, sizeof(struct ays_usb_command_header),
                                                   buffer + transferred_bytes, command_buffer->in_size);
        if(bytes_read < 0)
        {
            aim_usb_intf_err(aim_intf, "Failed to read data chunk");
            err = bytes_read;
            break;
        }

        if(bytes_read != command_buffer->in_size)
        {
            aim_usb_intf_warn(aim_intf, "Not all data could be read");
            err = -EAGAIN;
            break;
        }

        transferred_bytes += bytes_read;
    }

    kfree(command_buffer);

    return err;
}


int aim_usb_ays_hw_write(struct aim_usb_interface* aim_intf, const void* buffer, size_t len, TY_E_MEM_TYPE mem_type,
                         loff_t offset)
{
    struct aim_usb_com_channel* com_channel = NULL;
    size_t transferred_bytes = 0;
    struct ays_usb_command_header* command_buffer= NULL;
    int err = 0;
    size_t chunk_payload_size = 0;
    ssize_t response_len = 0;

    BUG_ON(!aim_intf || !buffer);

    aim_usb_intf_dbg(aim_intf, "Using AYS raw data write");

    com_channel = aim_usb_interface_get_com_channel(aim_intf, AYS_COMMAND_CHANNEL_ID);
    if(IS_ERR(com_channel))
    {
        return -ENODEV;
    }

    /* Allocate a buffer for the write command with maximum transfer size
     * as the data to be written has to be appended to the command header
     */
    command_buffer = kmalloc(aim_ays_usb_transfer_size, GFP_KERNEL);
    if(!command_buffer)
    {
        aim_usb_intf_err(aim_intf, "Failed to allocate command buffer");
        return -ENOMEM;
    }

    /* The AYS command endpoints have a maximum transfer size.
     * So we might have to split up the data transfer to multiple chunks
     */
    transferred_bytes = 0;
    err = 0;
    while(transferred_bytes < len)
    {
        /* The chunk payload size is limited by maximum transfer size minus command header size */
        chunk_payload_size = min(aim_ays_usb_transfer_size - sizeof(struct ays_usb_command_header), len - transferred_bytes);

        AYS_USB_COMMAND_INIT(command_buffer);

        command_buffer->id = AYS_USB_CMD_MEMORY;
        command_buffer->direction = AYS_USB_DIRECTION_OUT;
        command_buffer->type = mem_type;
        command_buffer->in_offset = 0;
        command_buffer->in_size = sizeof(AiUInt32); /* We expect a 4 byte acknowledge response */
        command_buffer->out_offset = offset + transferred_bytes;
        command_buffer->out_size = chunk_payload_size;

        /* Copy the payload to write with this chunk to the command buffer */
        memcpy((void*) command_buffer + sizeof(struct ays_usb_command_header), buffer + transferred_bytes,
                command_buffer->out_size);

        aim_usb_intf_dbg(aim_intf, "Writing chunk with offset %u and size %u", command_buffer->out_offset,
                         command_buffer->out_size);

        response_len = aim_usb_com_channel_issue_cmd(com_channel, command_buffer,
                                                     sizeof(struct ays_usb_command_header)
                                                     + command_buffer->out_size, command_buffer,
                                                     sizeof(AiUInt32));
        if(response_len < 0)
        {
            aim_usb_intf_err(aim_intf, "Failed to write data chunk");
            err = response_len;
            break;
        }

        if(response_len != sizeof(AiUInt32))
        {
            aim_usb_intf_warn(aim_intf, "Invalid response for write of data chunk received");
            err = -EAGAIN;
            break;
        }

        transferred_bytes += chunk_payload_size;
    }

    kfree(command_buffer);

    return err;
}

ssize_t aim_usb_ays_target_command(struct aim_usb_interface* intf, void* command, size_t command_len,
                                   void* response, size_t response_len)
{
    void* command_buffer = NULL;
    size_t command_buffer_size = 0;
    struct ays_usb_command_header* command_header = NULL;
    struct aim_usb_com_channel* command_channel = NULL;
    int err = 0;
    ssize_t actual_response = 0;

    BUG_ON(!intf || !command);

    aim_usb_intf_dbg(intf, "Processing target command on AYS with command len %ld and response size %ld",
                     command_len, response_len);

    /* Allocate buffer that can hold the command data as well as
     * the USB command header
     */
    command_buffer_size = command_len + sizeof(struct ays_usb_command_header);
    aim_usb_intf_dbg(intf, "Comand buffer_size:%ld", command_buffer_size);

    if(command_buffer_size > aim_ays_usb_transfer_size || response_len > aim_ays_usb_transfer_size)
    {
        aim_usb_intf_err(intf, "Target command or response length exceeds maximum transfer size");
        return -EINVAL;
    }

    command_buffer = kmalloc(command_buffer_size, GFP_KERNEL);
    if(!command_buffer)
    {
        aim_usb_intf_err(intf, "Failed to allocate command buffer");
        return -ENOMEM;

    }

    // critically checks against the original size
    if(command_buffer_size > AYS_MAX_COMMAND_EP_SIZE)
    {
        aim_usb_intf_dbg(intf, "Sending length cmd");
        err = aim_usb_length_info_command(intf, command_buffer_size);
        if(!err)
        {
            aim_usb_intf_err(intf, "Error sending length cmd % d", err);
            kfree(command_buffer);
            return err;
        }
    }

    command_header = command_buffer;
    err = 0;
    do
    {
        command_channel = aim_usb_interface_get_com_channel(intf, AYS_COMMAND_CHANNEL_ID);
        if(!command_channel)
        {
            aim_usb_intf_err(intf, "Failed to get target command channel");
            err = -EIO;
            break;
        }

        AYS_USB_COMMAND_INIT(command_header);
        command_header->id = AYS_USB_CMD_TSW;
        command_header->direction = AYS_USB_DIRECTION_INOUT;
        command_header->type = AI_MEMTYPE_SHARED;
        command_header->in_offset = AYS_INPUT_OFFSET;   /* Is actually no longer used but needs to stay on this value as some old tsw versions expect this to be set!!! */
        command_header->in_size = response_len;
        command_header->out_offset = AYS_OUTPUT_OFFSET;  /* Is actually no longer used but needs to stay on this value as some old tsw versions expect this to be set!!! */
        command_header->out_size = command_len;

        /* Copy the command payload after the USB command header */
        memcpy(command_buffer + sizeof(struct ays_usb_command_header), command, command_len);
        actual_response = aim_usb_com_channel_issue_cmd(command_channel, command_buffer, command_buffer_size, response, response_len);
        if(actual_response < 0)
        {
            err = actual_response;
            break;
        }
        aim_usb_intf_dbg(intf, "Received %ld bytes of response", actual_response);
    }while(0);

    kfree(command_buffer);

    return err ? err : actual_response;
}

ssize_t aim_usb_length_info_command(struct aim_usb_interface *intf, size_t command_len)
{
    void *command_buffer = NULL;
    void *response_buffer = NULL;
    size_t command_buffer_size = 0;
    struct ays_usb_command_header *command_header = NULL;
    struct aim_usb_com_channel *command_channel = NULL;
    int err = 0;
    ssize_t actual_response = 0;
    u32 cmd_len = (u32) command_len; // shortend from the size_t type
    BUG_ON(!intf);
    aim_usb_intf_dbg(intf, "Writing a length cmd with size of %ld", command_len);

    if(command_len > aim_ays_usb_transfer_size)
    {
      aim_usb_intf_err(intf, "Exceeded the command buffer size");
      return -EIO;
    }
    command_buffer_size = sizeof(u32) + sizeof(struct ays_usb_command_header);
    aim_usb_intf_dbg(intf, "Command buffer_size:%ld", command_buffer_size);
    command_buffer = kmalloc(command_buffer_size, GFP_KERNEL);
    if (!command_buffer) {
        aim_usb_intf_err(intf, "Failed to allocate command buffer");
        return -ENOMEM;

    }

    response_buffer = kmalloc(sizeof(u32), GFP_KERNEL);
    if (!response_buffer) {
      aim_usb_intf_err(intf, "Failed to allocate command buffer");
      kfree(command_buffer);
      return -ENOMEM;
    }

    command_header = command_buffer;
    err = 0;
    do {
        command_channel = aim_usb_interface_get_com_channel(intf,
                AYS_COMMAND_CHANNEL_ID);
        if (!command_channel) {
            aim_usb_intf_err(intf, "Failed to get target command channel");
            err = -EIO;
            break;
        }

        AYS_USB_COMMAND_INIT(command_header);
        command_header->id = AYS_USB_CMD_LENGTH;
        command_header->direction = AYS_USB_DIRECTION_INOUT;
        command_header->type = AI_MEMTYPE_SHARED;
        command_header->in_offset = AYS_INPUT_OFFSET; /* Is actually no longer used but needs to stay on this value as some old tsw versions expect this to be set!!! */
        command_header->in_size = sizeof(u32);
        command_header->out_offset = AYS_OUTPUT_OFFSET; /* Is actually no longer used but needs to stay on this value as some old tsw versions expect this to be set!!! */
        command_header->out_size = sizeof(u32);

        /* Copy the command payload after the USB command header */

        memcpy(command_buffer + sizeof(struct ays_usb_command_header), &cmd_len,sizeof(u32));

        actual_response = aim_usb_com_channel_issue_cmd(command_channel,
                command_buffer, command_buffer_size, response_buffer, sizeof(u32));
        if (actual_response < 0) {
            err = actual_response;
            break;
        }
        aim_usb_intf_dbg(intf, "Received %ld bytes of response for a length cmd",
                actual_response);
    } while (0);

    kfree(command_buffer);
    kfree(response_buffer);

    return err ? err : actual_response;
}

ssize_t aim_usb_length_handshake_command(struct aim_usb_interface* intf, size_t command_len) {
    void *command_buffer = NULL;
    void *response_buffer = NULL;
    size_t command_buffer_size = 0;
    struct ays_usb_command_header *command_header = NULL;
    struct aim_usb_com_channel *command_channel = NULL;
    int err = 0;
    ssize_t actual_response = 0;
    u32 cmd_len = (u32) command_len; // shortend from the size_t type
    BUG_ON(!intf);
    aim_usb_intf_dbg(intf, "Doing a length handshake with the size of %ld", command_len);

    command_buffer_size = sizeof(u32) + sizeof(struct ays_usb_command_header);
    command_buffer = kmalloc(command_buffer_size, GFP_KERNEL);
    if (!command_buffer) {
        aim_usb_intf_err(intf, "Failed to allocate command buffer");
        return -ENOMEM;

    }

    response_buffer = kmalloc(sizeof(u32), GFP_KERNEL);
    if (!response_buffer) {
        aim_usb_intf_err(intf, "Failed to allocate command buffer");
        kfree(command_buffer);
        return -ENOMEM;
    }

    command_header = command_buffer;
    err = 0;
    do {
        command_channel = aim_usb_interface_get_com_channel(intf,
                AYS_COMMAND_CHANNEL_ID);
        if (!command_channel) {
            aim_usb_intf_err(intf, "Failed to get target command channel");
            err = -EIO;
            break;
        }

        AYS_USB_COMMAND_INIT(command_header);
        command_header->id = AYS_USB_CMD_LENGTH_HANDSHAKE;
        command_header->direction = AYS_USB_DIRECTION_INOUT;
        command_header->type = AI_MEMTYPE_SHARED;
        command_header->in_offset = AYS_INPUT_OFFSET; /* Is actually no longer used but needs to stay on this value as some old tsw versions expect this to be set!!! */
        command_header->in_size = sizeof(u32);
        command_header->out_offset = AYS_OUTPUT_OFFSET; /* Is actually no longer used but needs to stay on this value as some old tsw versions expect this to be set!!! */
        command_header->out_size = sizeof(u32);

        /* Copy the command payload after the USB command header */

        memcpy(command_buffer + sizeof(struct ays_usb_command_header), &cmd_len,sizeof(u32));

        actual_response = aim_usb_com_channel_issue_cmd(command_channel,
                command_buffer, command_buffer_size, response_buffer, sizeof(u32));

        if (actual_response < 0) {
            err = actual_response;
            break;
        }
        cmd_len = *(u32*) response_buffer;
        aim_usb_intf_dbg(intf, "Actual Response %d", cmd_len);
        aim_usb_intf_dbg(intf, "Received %ld bytes of response for a length cmd",
                actual_response);
        if (cmd_len == command_len) {
            aim_ays_usb_transfer_size = command_len;
            command_channel->max_transfer_length = command_len;
        }

    } while (0);
    aim_usb_intf_info(intf,"Setting expanded usb transfer size to %d.", aim_ays_usb_transfer_size);
    kfree(command_buffer);
    kfree(response_buffer);

    return err ? err : actual_response;
}

/*! \brief Allocates a buffer for host IO on AYS boards
 *
 * This function allocates a buffer that will be large enough to be used for ays_host_io_write, ays_host_io_read
 * and ays_host_io_trigger calls. The maximum data size for ays_host_io_write calls must be specified
 * with the max_payload_size parameter
 * @param max_payload_size maximum data size that can be used in ays_host_io_write calls with the allocated bufefr
 * @return allocated buffer on success, ERR_PTR on failure
 */
static __inline__ void* ays_host_io_alloc_buffer(size_t max_payload_size)
{
    void* buffer = kmalloc(sizeof(struct ays_usb_command_header) + max_payload_size, GFP_KERNEL);

    return buffer ? buffer : ERR_PTR(-ENOMEM);
}


/*! \brief Writes host IO data
 *
 * @param channel the USB communication channel to use for host IO
 * @param urb_buffer the URB buffer to use for the IO sequence. Must be allocated using \ref ays_host_io_alloc_buffer
 * @param data_buffer the actual host IO data
 * @param data_len length of host IO data in bytes
 * @return 0 on success, errno code on failure
 */
static __inline__ int ays_host_io_write(struct aim_usb_com_channel* channel, void* urb_buffer, const void* data_buffer,
                                        size_t data_len)
{
    struct ays_usb_command_header* command_header = NULL;
    int err = 0;
    ssize_t actual_response_len = 0;

    command_header = urb_buffer;

    AYS_USB_COMMAND_INIT(command_header);

    command_header->id = AYS_USB_CMD_HOST_IO;
    command_header->direction = AYS_USB_DIRECTION_OUT;
    command_header->in_size = sizeof(AiUInt32);
    command_header->out_size = data_len;

    memcpy(urb_buffer + sizeof(struct ays_usb_command_header), data_buffer, data_len);

    err = aim_usb_com_channel_send(channel, urb_buffer, data_len + sizeof(struct ays_usb_command_header));
    if(err)
    {
        aim_usb_intf_err(channel->intf, "Failed to send host io data");
        return err;
    }

    actual_response_len = aim_usb_com_channel_receive(channel, urb_buffer, sizeof(AiUInt32));
    if(actual_response_len < 0)
    {
        aim_usb_intf_err(channel->intf, "Failed to receive host io response");
        return actual_response_len;
    }
    return 0;
}


/*! \brief Reads host IO data
 *
 * @param channel the USB communication channel to use for host IO
 * @param urb_buffer the URB buffer to use for the IO sequence. Must be allocated using \ref ays_host_io_alloc_buffer
 * @param data_buffer the data read will be stored here
 * @param data_len length of data buffer in bytes
 * @return number of bytes read to data_buffer on success, errno code on failure
 */
static __inline__ ssize_t ays_host_io_read(struct aim_usb_com_channel* channel, void* command_buffer, void* data_buffer,
                                           size_t data_len)
{
    struct ays_usb_command_header* command_header = NULL;
    int err = 0;

    command_header = command_buffer;

    AYS_USB_COMMAND_INIT(command_header);

    command_header->id = AYS_USB_CMD_HOST_IO;
    command_header->direction = AYS_USB_DIRECTION_IN;
    command_header->in_size = data_len;

    err = aim_usb_com_channel_send(channel, command_buffer, sizeof(struct ays_usb_command_header));
    if(err)
    {
        aim_usb_intf_err(channel->intf, "Failed to send host io data");
        return err;
    }

    return aim_usb_com_channel_receive(channel, data_buffer, data_len);
}


/*! \brief Triggers host io command on AYS boards
 *
 * @param channel the USB communication channel to use
 * @param urb_buffer buffer to use for the USB communication. Must be allocated using \ref ays_host_io_alloc_buffer
 * @return 0 on success, errno code on failure
 */
static __inline__ int ays_host_io_trigger(struct aim_usb_com_channel* channel, void* urb_buffer)
{
    struct ays_usb_command_header* command_header = NULL;
    int err = 0;
    ssize_t actual_response_len = 0;

    command_header = urb_buffer;

    AYS_USB_COMMAND_INIT(command_header);

    command_header->id = AYS_USB_CMD_HOST_IO_COMMAND;
    command_header->direction = AYS_USB_DIRECTION_OUT;
    command_header->in_size = sizeof(AiUInt32);
    command_header->out_size = 0;

    err = aim_usb_com_channel_send(channel, urb_buffer, sizeof(struct ays_usb_command_header));
    if(err)
    {
        aim_usb_intf_err(channel->intf, "Failed to send host io data");
        return err;
    }

    actual_response_len = aim_usb_com_channel_receive(channel, urb_buffer, sizeof(AiUInt32));
    if(actual_response_len < 0)
    {
        aim_usb_intf_err(channel->intf, "Failed to receive host io response");
        return actual_response_len;
    }

    return 0;

}

ssize_t aim_usb_ays_com_channel_cmd(struct aim_usb_com_channel* channel, const void* command, size_t cmd_len,
                                    void* response, size_t resp_len)
{
    void* host_io_buffer = NULL;
    size_t max_transfer_len = 0;
    ssize_t actual_response_len = 0;
    int err = 0;

    BUG_ON(!channel || !channel->intf || !command);

    aim_usb_intf_dbg(channel->intf, "AYS USB channel command");

    aim_usb_com_channel_lock(channel);

    host_io_buffer = NULL;
    err = 0;
    actual_response_len = 0;
    do
    {
        /* The maximum transfer length that may occur during host IO sequence.
         * This is the maximum command length + command header or the response length
         */
        max_transfer_len = max(cmd_len + sizeof(struct ays_usb_command_header), resp_len);
        aim_usb_intf_dbg(channel->intf, "Actual transfer_length %ld", max_transfer_len);
        if(max_transfer_len > aim_ays_usb_transfer_size)
        {
            aim_usb_intf_err(channel->intf, "Max transfer length exceeded for AYS USB command");
            err = -EMSGSIZE;
            break;
        }

        host_io_buffer = ays_host_io_alloc_buffer(cmd_len);
        if(IS_ERR(host_io_buffer))
        {
            aim_usb_intf_err(channel->intf, "Failed to allocate host io URB buffer");
            err = PTR_ERR(host_io_buffer);
            break;
        }
        aim_usb_intf_dbg(channel->intf, "Writing the cmd %d", (int)max_transfer_len);
        err = ays_host_io_write(channel, host_io_buffer, command, cmd_len);
        if(err)
        {
            aim_usb_intf_err(channel->intf, "Failed to send host io data");
            break;
        }

        err = ays_host_io_trigger(channel, host_io_buffer);
        if(err)
        {
            aim_usb_intf_err(channel->intf, "Failed to trigger host io command");
            break;
        }

        actual_response_len = ays_host_io_read(channel, host_io_buffer, response, resp_len);
        if(actual_response_len < 0)
        {
            aim_usb_intf_err(channel->intf, "Failed to receive host io command response");
            err = actual_response_len;
            break;
        }

    }while(0);

    if(!IS_ERR_OR_NULL(host_io_buffer))
    {
        kfree(host_io_buffer);
    }

    aim_usb_com_channel_unlock(channel);

    return err ? err : actual_response_len;
}


int aim_usb_ays_novram_get(struct aim_usb_interface* intf, AiUInt32 address, AiUInt32* value)
{
    struct esmart_novram_get_req esmart_req;
    struct esmart_novram_get_resp* esmart_response = NULL;
    struct aim_usb_com_channel* esmart_com_channel = NULL;
    int err = 0;
    ssize_t response = 0;
    AiUInt32* novram_data = NULL;

    BUG_ON(!intf || !value);

    err = 0;
    do
    {
        esmart_response = kmalloc(esmart_novram_get_resp_size(1), GFP_KERNEL);
        if(!esmart_response)
        {
            err = -ENOMEM;
            break;
        }

        esmart_com_channel = aim_usb_interface_get_com_channel(intf, AYS_COMMAND_CHANNEL_ID);
        if(IS_ERR(esmart_com_channel))
        {
            err = PTR_ERR(esmart_com_channel);
            break;
        }

        esmart_req.header.magic = ESMART_MAGIC;
        esmart_req.header.request_id = ESMART_REQ_NOVRAM_GET;
        esmart_req.header.request_size = sizeof(struct esmart_novram_get_req);

        esmart_req.start_offset = address;
        esmart_req.num_entries = 1;

        response = aim_usb_ays_com_channel_cmd(esmart_com_channel, &esmart_req, sizeof(esmart_req), esmart_response,
                                               esmart_novram_get_resp_size(1));
        if(response < 0)
        {
            err = response;
            break;
        }

        if(esmart_response->header.magic != ESMART_MAGIC || esmart_response->status != ESMART_OK)
        {
            err = -EIO;
            break;
        }

        novram_data = (AiUInt32*) &esmart_response->novram_data;
        *value = *novram_data;

    }while(0);

    if(esmart_response)
    {
        kfree(esmart_response);
    }
    return err;
}



int aim_usb_ays_hw_start(struct aim_usb_interface* intf)
{
    int err = 0;
    AiUInt32* novram_value = kmalloc(sizeof(AiUInt32), GFP_KERNEL);

    if (novram_value == NULL)
    {
        return -ENOMEM;
    }
    BUG_ON(!intf);
    aim_usb_intf_dbg(intf, "Starting AYS device");

    err = aim_usb_ays_novram_get(intf, AYS_BOARD_CONFIG_NVR_ADDR, novram_value);
    if(err)
    {
        aim_usb_intf_err(intf, "Failed to read AYS board config from NOVRAM");
        goto endread;
    }
    intf->board_config = *novram_value;

    err = aim_usb_ays_novram_get(intf, AYS_BOARD_TYPE_NVR_ADDR, novram_value);
    if(err)
    {
        aim_usb_intf_err(intf, "Failed to read AYS board type from NOVRAM");
        goto endread;
    }

    intf->board_type = *novram_value;

    err = aim_usb_ays_novram_get(intf, AYS_BOARD_PART_NO_NVR_ADDR, novram_value);
    if(err)
    {
        aim_usb_intf_err(intf, "Failed to read AYS board part no from NOVRAM");
        goto endread;
    }

    intf->board_part_no = *novram_value;

    err = aim_usb_ays_novram_get(intf, AYS_HW_VARIANT_NVR_ADDR, novram_value);
    if(err)
    {
        aim_usb_intf_err(intf, "Failed to read AYS hw variant no from NOVRAM");
        goto endread;
    }

    intf->hw_variant = *novram_value;

    err = aim_usb_ays_novram_get(intf, AYS_SERIAL_NVR_ADDR, novram_value);
    if(err)
    {
        aim_usb_intf_err(intf, "Failed to read AYS serial from NOVRAM");
        goto endread;
    }
    // check whether or not long cmds are supported by the embedded Linux (only relevant for fdx asc)
    // in case of failure the aim_ays_usb_transfer_size is unchanged as this function is allowed to fail
    aim_usb_length_handshake_command(intf, AYS_MAX_COMMAND_EXPANDED_EP_SIZE);

    intf->serial = *novram_value;

    aim_usb_intf_dbg(intf, "Serial: %d", intf->serial);
    aim_usb_intf_dbg(intf, "board config 0x%x", intf->board_config);
    aim_usb_intf_dbg(intf, "board type 0x%x", intf->board_type);

    intf->global_memory.size = AYS_GLOBAL_MEMORY_SIZE;
    intf->io_memory.size = AYS_IO_MEMORY_SIZE;

    endread:
    kfree(novram_value);

    return err;
}
