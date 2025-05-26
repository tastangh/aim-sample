/* SPDX-FileCopyrightText: 2015-2024 AIM GmbH <info@aim-online.com> */
/* SPDX-License-Identifier: MIT OR GPL-2.0-or-later */

/*
Description:

Implement AYS specific defines in this file
*/

/*! \file AiHwAys.h
    Declare AyS and AyXX specific functions in this file */

#ifndef __AIHWAYS_H__
#define __AIHWAYS_H__

#include "Ai_cdef.h"



/* TCP communication area (Start at Address 0) */
/* All values as register DWORD index          */
#define AYS_REG_TCP_CONTROL          0x0000 /* 0x0000 */
#define AYS_REG_TCP_INPUT_0          0x0001 /* 0x0004 */
#define AYS_REG_TCP_INPUT_1          0x0002 /* 0x0008 */
#define AYS_REG_TCP_INPUT_2          0x0003 /* 0x000C */
#define AYS_REG_TCP_INPUT_3          0x0004 /* 0x0010 */
#define AYS_REG_TCP_INPUT_4          0x0005 /* 0x0014 */

#define AYS_REG_TCP_STATUS           0x0008 /* 0x0020 */
#define AYS_REG_TCP_OUTPUT_0         0x0009 /* 0x0024 */
#define AYS_REG_TCP_OUTPUT_1         0x000A /* 0x0028 */
#define AYS_REG_TCP_OUTPUT_2         0x000B /* 0x002C */

#define AYS_REG_TT_HIGH              0x0038 /* 0x00E0 */
#define AYS_REG_TT_LOW               0x0039 /* 0x00E4 */
#define AYS_REG_TT_STATUS            0x003A /* 0x00E8 */
#define AYS_REG_TT_EXTENSION         0x003B /* 0x00EC */

#define AYS_REG_TCP_SW_VERSION       0x0040 /* 0x0100 */
#define AYS_REG_TIME_SOURCE_CONTROL  0x0041 /* 0x0104 */


/* Global Register Area (Start at Address 0x10000) */
/* All values as register DWORD index          */
#define AYS_REG_GPIO_OUTPUT          0x4002 /* 0x10008 */ /* Read/Write */
#define AYS_REG_GPIO_INPUT           0x4003 /* 0x1000C */ /* Read       */
#define AYS_REG_PL_VERSION           0x4004 /* 0x10010 */ /* Read       */

#define AYS_REG_ASP_EMERGENCY        0x4005 /* 0x10014 */ /* Write      *//*! \def Used for signaling maintenance boot on ANET */
#define AYS_REG_ASP_SETTINGS         0x4006 /* 0x10018 */ /* Write      *//*! \def Used for settings maintenance boot on ANET  */
#define AYS_REG_GPIO_CONFIG          0x4007 /* 0x1001C */ /* Read/Write */

#define AYS_REG_BIU1_BOOT            0x400C /* 0x10030 */ /* Read/Write *//*! \def Used for BIU1 boot handshake                */
#define AYS_REG_BIU2_BOOT            0x400D /* 0x10034 */ /* Read/Write *//*! \def Used for BIU1 boot handshake                */

#define AYS_REG_IO_BIU2_CORE_VERSION 0x4010 /* 0x10040 */ /* Read       *//*! \def The HS IO core version                      */
#define AYS_REG_IO_BIU1_CORE_VERSION 0x4011 /* 0x10044 */ /* Read       *//*! \def The HS IO core version                      */
#define AYS_REG_TRIGGER_DIGITAL_LOOP 0x4013 /* 0x1004C */ /* Read/Write *//*! \def Trigger digital loop register               */

#define AYS_REG_IO_USB_C_STATUS      0x4021 /* 0x10084 */ /* Read/Write *//*! \def The USB-C Status Register                   */


/* IRQ Register Area (Start at Address 0x30000) */
/* All values as register DWORD index          */
#define AYS_REG_IRQ_HOST_EVENT       0xC000 /* 0x30000 */
#define AYS_REG_IRQ_HOST_MASK        0xC001 /* 0x30004 */







/* Interrupts */
#define AYS_IRQ_GPIO_0_HIGH         0x00000001
#define AYS_IRQ_GPIO_1_HIGH         0x00000002
#define AYS_IRQ_GPIO_2_HIGH         0x00000004
#define AYS_IRQ_GPIO_3_HIGH         0x00000008
#define AYS_IRQ_GPIO_4_HIGH         0x00000010
#define AYS_IRQ_GPIO_5_HIGH         0x00000020
#define AYS_IRQ_GPIO_6_HIGH         0x00000040
#define AYS_IRQ_GPIO_7_HIGH         0x00000080

#define AYS_IRQ_GPIO_0_LOW          0x00000100
#define AYS_IRQ_GPIO_1_LOW          0x00000200
#define AYS_IRQ_GPIO_2_LOW          0x00000400
#define AYS_IRQ_GPIO_3_LOW          0x00000800
#define AYS_IRQ_GPIO_4_LOW          0x00001000
#define AYS_IRQ_GPIO_5_LOW          0x00002000
#define AYS_IRQ_GPIO_6_LOW          0x00004000
#define AYS_IRQ_GPIO_7_LOW          0x00008000

#define AYS_IRQ_BIU_1               0x00010000
#define AYS_IRQ_BIU_2               0x00020000
#define AYS_IRQ_BIU_3               0x00040000
#define AYS_IRQ_BIU_4               0x00080000

#define AYS_IRQ_HOST                0x00100000
#define AYS_IRQ_ASP                 0x00200000
#define AYS_IRQ_RTC                 0x00400000

#define AYS_IRQ_ALL                 (AYS_IRQ_BIU_1 | AYS_IRQ_BIU_2)

/*! \def AEC_IRQ_POWER
 * This interrupt event is used by the
 * power controller on ANET boards to request power down
 */
#define AYS_IRQ_POWER               (1 << 27)


/* Ays-FDX specific defines */

/*! \def AIM_APS_IRQ_EVENT
 * Byte offset if IRQ event register in IRQ register area
 */
#define AIM_APS_IRQ_EVENT   AYS_REG_IRQ_HOST_EVENT + 0x10 /* 0x30040 */

/*! \def AIM_APS_IRQ_MASK_OFFSET
 * 4-byte offset of IRQ mask register in IRQ register area
 */
#define AIM_APS_IRQ_MASK    AYS_REG_IRQ_HOST_EVENT + 0x11 /* 0x30044 */


#define AYS_FDX_SGI_IRQ_0          0x00000001
#define AYS_FDX_SGI_IRQ_1          0x00000002
#define AYS_FDX_SGI_IRQ_2          0x00000004
#define AYS_FDX_SGI_IRQ_3          0x00000008
#define AYS_FDX_BIU                0x00000040
#define AYS_FDX_ETH_RX_1           0x00000100
#define AYS_FDX_ETH_RX_2           0x00000200
#define AYS_FDX_ETH_TX_1           0x00001000
#define AYS_FDX_ETH_TX_2           0x00002000
#define AYS_FDX_ETH_RX_1_LOLI_FULL 0x00010000
#define AYS_FDX_ETH_RX_2_LOLI_FULL 0x00020000
#define AYS_FDX_ETH_TX_1_LOLI_FULL 0x00100000
#define AYS_FDX_ETH_TX_2_LOLI_FULL 0x00200000
#define AYS_FDX_DMA_HOST_TO_RAM    0x01000000
#define AYS_FDX_ETH_RAM_TO_HOST    0x02000000
#define AYS_FDX_PCI_RESET          0x80000000

#define AYS_FDX_ALL (AYS_FDX_BIU | \
		             AYS_FDX_ETH_RX_1 | AYS_FDX_ETH_RX_2 | AYS_FDX_ETH_TX_1 | AYS_FDX_ETH_TX_2 | \
		             AYS_FDX_ETH_RX_1_LOLI_FULL | AYS_FDX_ETH_RX_2_LOLI_FULL | \
		             AYS_FDX_ETH_TX_1_LOLI_FULL | AYS_FDX_ETH_TX_2_LOLI_FULL)

#endif /* __AIHWAYS_H__ */


