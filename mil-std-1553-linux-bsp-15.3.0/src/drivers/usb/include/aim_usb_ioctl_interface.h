/* SPDX-FileCopyrightText: 2017-2021 AIM GmbH <info@aim-online.com> */
/* SPDX-License-Identifier: MIT OR GPL-2.0-or-later */

/*! \file aim_usb_ioctl_interface.h
 *
 */

#ifndef AIM_USB_IOCTL_INTERFACE_H_
#define AIM_USB_IOCTL_INTERFACE_H_



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
#endif


/*! \def MAX_DRIVER_VERSION_LENGTH_USER
 * Maximum length of version string that can be obtained with
 * IOCTL for getting driver version
 */
#define MAX_DRIVER_VERSION_LENGTH_USER  64


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


/*! \struct aim_usb_com_chn_cmd
 *
 * This structure is used as inut parameter
 * for the IOCTL to issue a generic USB communication channel command
 */
struct aim_usb_com_chn_cmd
{
    AiUInt8 id;                                 /*!< ID of the USB communication channel to use */
    struct aim_target_command commandParams;    /*!< Parameters of the command/response sequence. See \ref struct aim_target_command. */
};


/*! \struct aim_get_mem_size
 * This structure is used as input/output parameter for the \ref AIM_IOCTL_GET_MEM_SIZE ioctl
 */
struct aim_get_mem_size
{
    TY_E_MEM_TYPE type; /*!< memory type to get size of */
    AiUInt64 size;      /*!< requested (mapped) size of memory bar is returned here */
};


/*! \def AIM_IOCTL_TARGET_COMMAND
 * IOCTL for issuing target commands to a board
 */
#define AIM_IOCTL_TARGET_COMMAND           _IOWR(AIM_IOCTL_BASE,  1, struct aim_target_command)



/*! \def AIM_IOCTL_GET_DRV_VERS
 * IOCTL for reading the version of the driver
 */
#define AIM_IOCTL_GET_DRV_VERS             _IOR(AIM_IOCTL_BASE,   3, struct aim_driver_version_user)


/*! \def AIM_IOCTL_USB_COM_CHN_CMD
 * IOCTL for command/response sequences over a USB based
 * communication channel to the target device.
 */
#define AIM_IOCTL_USB_COM_CHN_CMD          _IOWR(AIM_IOCTL_BASE, 4, struct aim_usb_com_chn_cmd)


/*! \def AIM_IOCTL_GENERIC_BOARD_INFO
 * IOCTL for getting protocol independent information about a device.
 */
#define AIM_IOCTL_GENERIC_BOARD_INFO        _IOWR(AIM_IOCTL_BASE, 7, TY_DRIVER_INFO)


/*! \def AIM_IOCTL_GET_MEM_SIZE
    IoControl code for getting the size that is usable for read/write of a specific memory type */
#define AIM_IOCTL_GET_MEM_SIZE              _IOR(AIM_IOCTL_BASE, 8, struct aim_get_mem_size)



#endif /* AIM_USB_IOCTL_INTERFACE_H_ */
