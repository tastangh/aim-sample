/* SPDX-FileCopyrightText: 2017-2022 AIM GmbH <info@aim-online.com> */
/* SPDX-License-Identifier: MIT OR GPL-2.0-or-later */

/*! \file mil_usb_io.h
 *
 *  This file maps functions needed for communication with USB
 *  devices to external functions provided by specific device driver implementations
 *  The target software hardware layer, as well as the APU memory mirror may use
 *  these functions
 *
 */

#ifndef MIL_USB_IO_H_
#define MIL_USB_IO_H_


#include "mil_tsw_includes.h"

/* Linux kernel driver section */
#if defined _AIM_LINUX && defined _AIM_SYSDRV

/* Forward declaration of generic USB interface structure of Linux USB driver */
struct aim_usb_interface;


/* These are forward declarations of USB access functions provided by Linux USB driver */
extern int aim_usb_hw_read(struct aim_usb_interface* aim_intf, void* buffer, size_t len, TY_E_MEM_TYPE mem_type,
                           loff_t offset);

extern int aim_usb_hw_write(struct aim_usb_interface* aim_intf, const void* buffer, size_t len, TY_E_MEM_TYPE mem_type,
                            loff_t offset);

extern int aim_usb_hw_novram_get(struct aim_usb_interface* intf, AiUInt32 address, AiUInt32* value);

extern int aim_usb_hw_novram_set(struct aim_usb_interface* intf, AiUInt32 address, AiUInt32 value);

extern int aim_usb_hw_tcp_reg_read(struct aim_usb_interface* aim_intf, AiUInt32 address, AiUInt32* value);

extern int aim_usb_hw_tcp_reg_write(struct aim_usb_interface* intf, AiUInt32 address, AiUInt32 value);




/*! \brief Global memory read function via USB
 *
 * @param driver_data driver specific context data
 * @param address address relative to global memory start to read from
 * @param data read data will be stored here
 * @return 0 on success
 */
static __inline__ AiInt32 usb_global_mem_read(void* driver_data, AiUInt32 address, void* data, AiSize len)
{
    return aim_usb_hw_read((struct aim_usb_interface*) driver_data, data, len, AI_MEMTYPE_GLOBAL_DIRECT, address);
}


/*! \brief Global memory write function via USB
 *
 * @param driver_data driver specific context data
 * @param address address relative to global memory start to write to
 * @param data data to write
 * @return 0 on success
 */
static __inline__ AiInt32 usb_global_mem_write(void* driver_data, AiUInt32 address, void* data, AiSize len)
{
    return aim_usb_hw_write((struct aim_usb_interface*) driver_data, data, len, AI_MEMTYPE_GLOBAL_DIRECT, address);
}


/*! \brief I/O memory read function via USB
 *
 * @param driver_data driver specific context data
 * @param address address relative to I/O memory start to read from
 * @param data read data will be stored here
 * @return 0 on success
 */
static __inline__ AiInt32 usb_io_read(void* driver_data, AiUInt32 address, void* data, AiSize len)
{
    return aim_usb_hw_read((struct aim_usb_interface*) driver_data, data, len, AI_MEMTYPE_IO, address);
}


/*! \brief I/O memory write function via USB
 *
 * @param driver_data driver specific context data
 * @param address address relative to I/O memory start to write to
 * @param data data to write
 * @return 0 on success
 */
static __inline__ AiInt32 usb_io_write(void* driver_data, AiUInt32 address, void* data, AiSize len)
{
    return aim_usb_hw_write((struct aim_usb_interface*) driver_data, data, len, AI_MEMTYPE_IO, address);
}


/*! \brief Get specific NOVRAM value via USB
 *
 * @param driver_data driver specific context data
 * @param address NOVRAM address to get
 * @return the NOVRAM value gotten from specified address
 */
static __inline__ AiUInt32 usb_novram_get(void* driver_data, AiUInt32 address)
{
    AiUInt32 value;

    aim_usb_hw_novram_get((struct aim_usb_interface*) driver_data, address, &value);

    return value;
}


/*! \brief Set specific NOVRAM value via USB
 *
 * @param driver_data driver specific context data
 * @param address address of NOVRAM setting to set value for
 * @param value the value to set
 * @return 0 on success
 */
static __inline__ AiInt32 usb_novram_set(void* driver_data, AiUInt32 address, AiUInt32 value)
{
    return aim_usb_hw_novram_set((struct aim_usb_interface*) driver_data, address, value);
}


/*! \brief Read a TCP port via USB
 *
 * @param driver_data driver specific context data
 * @param port the TCP port to read
 * @param value the read data will be stored here
 * @return 0 on success
 */
static __inline__ AiInt32 usb_tcp_port_read(void* driver_data, AiUInt32 port, AiUInt32* value)
{
    return aim_usb_hw_tcp_reg_read((struct aim_usb_interface*) driver_data, port, value);
}


/*! \brief Write a TCP port via USB
 *
 * @param driver_data driver specific context data
 * @param port the TCP port to write
 * @param value data to write
 * @return 0 on success
 */
static __inline__ AiInt32 usb_tcp_port_write(void* driver_data, AiUInt32 port, AiUInt32 value)
{
    return aim_usb_hw_tcp_reg_write((struct aim_usb_interface*) driver_data, port, value);
}

#elif defined _AIM_WINDOWS

struct _DEVICE_CONTEXT;

extern AiUInt32 NOVRamReadUsb(struct _DEVICE_CONTEXT* p_DeviceContext, AiUInt32 ul_Address);

extern AiInt32 NOVRamWriteUsb(struct _DEVICE_CONTEXT* p_DeviceContext, AiUInt32 ul_Address, AiUInt32 ul_Data);

extern AiInt32 UsbTcpPortWrite(struct _DEVICE_CONTEXT* p_DeviceContext, AiUInt32 ul_Port, AiUInt32 ul_Data);

extern AiInt32 UsbTcpPortRead(struct _DEVICE_CONTEXT* p_DeviceContext, AiUInt32 ul_Port, AiUInt32* pul_Data);

extern AiInt32 UsbPciLcaRead(struct _DEVICE_CONTEXT* p_DeviceContext, AiUInt32 address, void* data, AiSize len);

extern AiInt32 UsbPciLcaWrite(struct _DEVICE_CONTEXT* p_DeviceContext, AiUInt32 address, void* data, AiSize len);

extern AiInt32 UsbGlobalMemRead(struct _DEVICE_CONTEXT* p_DeviceContext, AiUInt32 address, void* data, AiSize len);

extern AiInt32 UsbGlobalMemWrite(struct _DEVICE_CONTEXT* p_DeviceContext, AiUInt32 address, void* data, AiSize len);


/*! \brief Get specific NOVRAM value via USB
*
* @param driver_data driver specific context data
* @param address NOVRAM address to get
* @return the NOVRAM value gotten from specified address
*/
static __inline AiUInt32 usb_novram_get(void* driver_data, AiUInt32 address)
{
    return NOVRamReadUsb((struct _DEVICE_CONTEXT*) driver_data, address);
}

/*! \brief Set specific NOVRAM value via USB
*
* @param driver_data driver specific context data
* @param address address of NOVRAM setting to set value for
* @param value the value to set
* @return 0 on success
*/
static __inline AiInt32 usb_novram_set(void* driver_data, AiUInt32 address, AiUInt32 value)
{
    return NOVRamWriteUsb((struct _DEVICE_CONTEXT*) driver_data, address, value);
}

/*! \brief Write a TCP port via USB
*
* @param driver_data driver specific context data
* @param port the TCP port to write
* @param value data to write
* @return 0 on success
*/
static __inline AiInt32 usb_tcp_port_write(void* driver_data, AiUInt32 port, AiUInt32 value)
{
    return UsbTcpPortWrite((struct _DEVICE_CONTEXT*) driver_data, port, value);
}


/*! \brief Read a TCP port via USB
*
* @param driver_data driver specific context data
* @param port the TCP port to read
* @param value the read data will be stored here
* @return 0 on success
*/
static __inline AiInt32 usb_tcp_port_read(void* driver_data, AiUInt32 port, AiUInt32* value)
{
    return UsbTcpPortRead((struct _DEVICE_CONTEXT*) driver_data, port, value);
}


/*! \brief I/O memory read function via USB
*
* @param driver_data driver specific context data
* @param address address relative to I/O memory start to read from
* @param data read data will be stored here
* @return 0 on success
*/
static __inline AiInt32 usb_io_read(void* driver_data, AiUInt32 address, void* data, AiSize len)
{
    return UsbPciLcaRead((struct _DEVICE_CONTEXT*) driver_data, address, data, len);
}


/*! \brief I/O memory write function via USB
*
* @param driver_data driver specific context data
* @param address address relative to I/O memory start to write to
* @param data data to write
* @return 0 on success
*/
static __inline AiInt32 usb_io_write(void* driver_data, AiUInt32 address, void* data, AiSize len)
{
    return UsbPciLcaWrite((struct _DEVICE_CONTEXT*) driver_data, address, data, len);
}


/*! \brief Global memory read function via USB
*
* @param driver_data driver specific context data
* @param address address relative to global memory start to read from
* @param data read data will be stored here
* @return 0 on success
*/
static __inline AiInt32 usb_global_mem_read(void* driver_data, AiUInt32 address, void* data, AiSize len)
{
    return UsbGlobalMemRead((struct _DEVICE_CONTEXT*) driver_data, address, data, len);
}


/*! \brief Global memory write function via USB
*
* @param driver_data driver specific context data
* @param address address relative to global memory start to write to
* @param data data to write
* @return 0 on success
*/
static __inline AiInt32 usb_global_mem_write(void* driver_data, AiUInt32 address, void* data, AiSize len)
{
    return UsbGlobalMemWrite((struct _DEVICE_CONTEXT*) driver_data, address, data, len);
}

#else

    #error "No valid USB I/O layer implementation"

#endif





#endif /* MIL_USB_IO_H_ */
