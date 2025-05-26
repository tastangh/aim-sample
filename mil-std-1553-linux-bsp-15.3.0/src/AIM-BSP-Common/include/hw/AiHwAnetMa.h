/* SPDX-FileCopyrightText: 2016-2021 AIM GmbH <info@aim-online.com> */
/* SPDX-License-Identifier: MIT OR GPL-2.0-or-later */

/*
Description:

Implement ANET MA specific FPGA registers in this file
*/

/*! \file AiHwAnetMa.h
    Implement ANET MA specific FPGA registers in this file */

#ifndef __AIHWANETMA_H__
#define __AIHWANETMA_H__



#define ANET_MA_REG_TCP_CONTROL          0x0000 /* 0x0000 */
#define ANET_MA_REG_TCP_INPUT_0          0x0001 /* 0x0004 */
#define ANET_MA_REG_TCP_INPUT_1          0x0002 /* 0x0008 */
#define ANET_MA_REG_TCP_INPUT_2          0x0003 /* 0x000C */
#define ANET_MA_REG_TCP_INPUT_3          0x0004 /* 0x0010 */
#define ANET_MA_REG_TCP_INPUT_4          0x0005 /* 0x0014 */

#define ANET_MA_REG_TCP_STATUS           0x0008 /* 0x0020 */
#define ANET_MA_REG_TCP_OUTPUT_0         0x0009 /* 0x0024 */
#define ANET_MA_REG_TCP_OUTPUT_1         0x000A /* 0x0028 */
#define ANET_MA_REG_TCP_OUTPUT_2         0x000B /* 0x002C */

#define ANET_MA_REG_TIME_SOURCE_CONTROL  0x0041 /* 0x0104 */

#define ANET_MA_REG_GPIO_OUTPUT       0x4002 /* 0x10008 */ /* Read/Write */
#define ANET_MA_REG_GPIO_INPUT        0x4003 /* 0x1000C */ /* Read       */
#define ANET_MA_REG_VERSION           0x4004 /* 0x10010 */ /* Read       */
#define ANET_MA_REG_GPIO_CONFIG       0x4007 /* 0x1001C */ /* Read/Write */




/*! \def ANET_MA_REG_TCP_BASE
 * 4-Byte offset of TCP register base in PL
 */
#define ANET_MA_REG_TCP_BASE 0

/*! \def ANET_MA_REG_TT_SET_1553
 * ID of time tag register set that is reserved for use by 1553 protocol implementations
 */
#define ANET_MA_REG_TT_SET_1553 1

/*! \def ANET_MA_REG_TT_SET_429
 * ID of time tag register set that is reserved for use by 429 protocol implementations
 */
#define ANET_MA_REG_TT_SET_429 2

/*! \def ANET_MA_REG_TT_BASE
 * 4-Byte offset of time tag register set with specified ID
 */
#define ANET_MA_REG_TT_BASE(set_id) \
    (set_id == ANET_MA_REG_TT_SET_1553 ? (ANET_MA_REG_TCP_BASE + 0x38) : (ANET_MA_REG_TCP_BASE + 0x3C))

/*! \define ANET_MA_REG_TT
 * 4-Byte offset of time tag register with specified set ID and register index into this set
 */
#define ANET_MA_REG_TT(set_id, index) \
    ((ANET_MA_REG_TT_BASE(set_id)) + (index))

/*! \def ANET_MA_REG_TT_HIGH
 * 4-byte offset of time tag high register for specified set ID
 */
#define ANET_MA_REG_TT_HIGH(set_id)    (ANET_MA_REG_TT(set_id, 0))

/*! \def ANET_MA_REG_TT_LOW
 * 4-byte offset of time tag low register for specified set ID
 */
#define ANET_MA_REG_TT_LOW(set_id)     (ANET_MA_REG_TT(set_id, 1))

/*! \def ANET_MA_REG_TT_STATUS
 * 4-byte offset of time tag status register for specified set ID
 */
#define ANET_MA_REG_TT_STATUS(set_id)  (ANET_MA_REG_TT(set_id, 2))

/*! \def ANET_MA_TT_DAY_OF_YEAR
 * Extracts day of year from time tag high value
 */
#define ANET_MA_TT_DAY_OF_YEAR(time_tag_high) \
    ((time_tag_high >> 17) & 0x1FF)

/*! \def ANET_MA_TT_HOURS
 *  Extracts hours from time tag high value
 */
#define ANET_MA_TT_HOURS(time_tag_high) \
    ((time_tag_high >> 12) & 0x1F)

/*! \def ANET_MA_TT_MINUTES
 * Extracts minutes from time tag low value
 */
#define ANET_MA_TT_MINUTES(time_tag_low) \
    ((time_tag_low >> 26) & 0x3F)

/*! \def ANET_MA_TT_SECONDS
 * Extracts seconds from time tag low value
 */
#define ANET_MA_TT_SECONDS(time_tag_low) \
    ((time_tag_low >> 20) & 0x3F)

/*! \def ANET_MA_TT_USECONDS
 * Extracts microseconds from time tag low value
 */
#define ANET_MA_TT_USECONDS(time_tag_low) \
    (time_tag_low & 0xFFFFF)




/*! \def ANET_MA_REG_IRQ_BASE
 * 4-Byte offset of IRQ register base in PL
 */
#define ANET_MA_REG_IRQ_BASE 0xC000 /* 0x30000 */

/*! \def ANET_MA_REG_IRQ
 * 4-Byte offset of register with given index in IRQ regsiter area
 */
#define ANET_MA_REG_IRQ(index) \
    ((ANET_MA_REG_IRQ_BASE) + (index))

/*! \def ANET_MA_REG_GLOBAL_IRQ_EVENT
 * 4-byte offset of global interrupt event register in PL
 */
#define ANET_MA_REG_GLOBAL_IRQ_EVENT ANET_MA_REG_IRQ(0) /* 0x30000 */

/*! \def ANET_MA_REG_GLOBAL_IRQ_MASK
 * 4-byte offset of global interrupt mask register in PL
 */
#define ANET_MA_REG_GLOBAL_IRQ_MASK  ANET_MA_REG_IRQ(1) /* 0x30004 */

/*! \def ANET_MA_REG_1553_IRQ_EVENT
 * 4-byte offset of 1553 BIU interrupt event register in PL
 */
#define ANET_MA_REG_1553_IRQ_EVENT ANET_MA_REG_IRQ(2) /* 0x30008 */

/*! \def ANET_MA_REG_1553_IRQ_MASK
 * 4-byte offset of 1553 BIU interrupt mask register in PL
 */
#define ANET_MA_REG_1553_IRQ_MASK  ANET_MA_REG_IRQ(3) /* 0x3000C */

/*! \def ANET_MA_REG_429_IRQ_EVENT
 * 4-byte offset of 429 BIU interrupt event register in PL
 */
#define ANET_MA_REG_429_IRQ_EVENT  ANET_MA_REG_IRQ(4) /* 0x30010 */

/*! \def ANET_MA_REG_429_IRQ_MASK
 * 4-byte offset of 429 BIU interrupt mask register in PL
 */
#define ANET_MA_REG_429_IRQ_MASK ANET_MA_REG_IRQ(5)   /* 0x30014 */

/*! \def ANET_MA_IRQ_EVENT_BIU1
 * Event bit in event registers that inidcates BIU 1 interrupt
 */
#define ANET_MA_IRQ_EVENT_BIU1 (1 << 0)

/*! \def ANET_MA_IRQ_EVENT_BIU2
 * Event bit in event registers that inidcates BIU 2 interrupt
 */
#define ANET_MA_IRQ_EVENT_BIU2 (1 << 1)

/*! \def ANET_MA_IRQ_EVENT_FIRMWARE
 * Comprises all Firmware irq events
 */
#define ANET_MA_IRQ_EVENT_FIRMWARE (ANET_MA_IRQ_EVENT_BIU1 | ANET_MA_IRQ_EVENT_BIU2)


#endif /* __AIHWANETMA_H__ */


