/* SPDX-FileCopyrightText: 2020-2024 AIM GmbH <info@aim-online.com> */
/* SPDX-License-Identifier: MIT OR GPL-2.0-or-later */

/*
Description:

Implement Artix7 specific defines in this file
*/

/*! \file AiHwArtix7.h
    Declare Artix7 specific functions in this file */

#ifndef __AIHWARTIX_H__
#define __AIHWARTIX_H__

#include "Ai_cdef.h"

/* TCP communication area (Start at Address 0) */
/* All values as register DWORD index          */
#define ARTIX7_REG_TCP_CONTROL          0x0000 /* 0x0000 */
#define ARTIX7_REG_TCP_INPUT_0          0x0001 /* 0x0004 */
#define ARTIX7_REG_TCP_INPUT_1          0x0002 /* 0x0008 */
#define ARTIX7_REG_TCP_INPUT_2          0x0003 /* 0x000C */
#define ARTIX7_REG_TCP_INPUT_3          0x0004 /* 0x0010 */
#define ARTIX7_REG_TCP_INPUT_4          0x0005 /* 0x0014 */

#define ARTIX7_REG_TCP_STATUS           0x0008 /* 0x0020 */
#define ARTIX7_REG_TCP_OUTPUT_0         0x0009 /* 0x0024 */
#define ARTIX7_REG_TCP_OUTPUT_1         0x000A /* 0x0028 */
#define ARTIX7_REG_TCP_OUTPUT_2         0x000B /* 0x002C */
#define ARTIX7_REG_TCP_BOOT_STATUS      0x002F /* 0x00BC */

#define ARTIX7_REG_TCP_BOOT_STATUS_READY    0x52454459U /*REDY*/

#define ARTIX7_REG_TT_HIGH              0x0038 /* 0x00E0 */
#define ARTIX7_REG_TT_LOW               0x0039 /* 0x00E4 */
#define ARTIX7_REG_TT_STATUS            0x003A /* 0x00E8 */
#define ARTIX7_REG_TT_EXTENSION         0x003B /* 0x00EC */

#define ARTIX7_REG_TCP_SW_VERSION       0x0040 /* 0x0100 */
#define ARTIX7_REG_TIME_SOURCE_CONTROL  0x0041 /* 0x0104 */

#define ARTIX7_REG_NOVRAM_BASE_OFFSET   0x80 /* 0x0200 */
#define ARTIX7_REG_NOVRAM_MAGIC         (ARTIX7_REG_NOVRAM_BASE_OFFSET)
#define ARTIX7_REG_NOVRAM_SERIAL        (ARTIX7_REG_NOVRAM_BASE_OFFSET + 0x04/4)
#define ARTIX7_REG_NOVRAM_BOARDCFG      (ARTIX7_REG_NOVRAM_BASE_OFFSET + 0x08/4)
#define ARTIX7_REG_NOVRAM_HWVARIANT     (ARTIX7_REG_NOVRAM_BASE_OFFSET + 0x0C/4)
#define ARTIX7_REG_NOVRAM_BOARDTYPE     (ARTIX7_REG_NOVRAM_BASE_OFFSET + 0x10/4)
#define ARTIX7_REG_NOVRAM_FRONTEND_CAP  (ARTIX7_REG_NOVRAM_BASE_OFFSET + 0x14/4)
#define ARTIX7_REG_NOVRAM_BOARDAPP_TYP  (ARTIX7_REG_NOVRAM_BASE_OFFSET + 0x18/4)
#define ARTIX7_REG_NOVRAM_PARTNO        (ARTIX7_REG_NOVRAM_BASE_OFFSET + 0x1C/4)
#define ARTIX7_REG_NOVRAM_PARTNO_EXT    (ARTIX7_REG_NOVRAM_BASE_OFFSET + 0x20/4)
#define ARTIX7_REG_NOVRAM_FIRSTDATE     (ARTIX7_REG_NOVRAM_BASE_OFFSET + 0x28/4)
#define ARTIX7_REG_NOVRAM_ACTDATE       (ARTIX7_REG_NOVRAM_BASE_OFFSET + 0x2C/4)
#define ARTIX7_REG_NOVRAM_PHY1RAM_SIZE  (ARTIX7_REG_NOVRAM_BASE_OFFSET + 0x40/4)
#define ARTIX7_REG_NOVRAM_PHY2RAM_SIZE  (ARTIX7_REG_NOVRAM_BASE_OFFSET + 0x44/4)
#define ARTIX7_REG_NOVRAM_GRAM_SIZE     (ARTIX7_REG_NOVRAM_BASE_OFFSET + 0x50/4)
#define ARTIX7_REG_NOVRAM_MLCAEX        (ARTIX7_REG_NOVRAM_BASE_OFFSET + 0x70/4)
#define ARTIX7_REG_NOVRAM_BIP1EX        (ARTIX7_REG_NOVRAM_BASE_OFFSET + 0x80/4)
#define ARTIX7_REG_NOVRAM_BIP2EX        (ARTIX7_REG_NOVRAM_BASE_OFFSET + 0x84/4)
#define ARTIX7_REG_NOVRAM_IRIG_OFFSET   (ARTIX7_REG_NOVRAM_BASE_OFFSET + 0xF4/4)
#define ARTIX7_REG_NOVRAM_CHECK_SUM     (ARTIX7_REG_NOVRAM_BASE_OFFSET + 0xFC/4)






/* Global Register Area (Start at Address 0x400) */
/* All values as register DWORD index          */
#define ARTIX7_REG_GPIO_OUTPUT          0x102 /* 0x408 */ /* Read/Write */
#define ARTIX7_REG_GPIO_INPUT           0x103 /* 0x40C */ /* Read       */
#define ARTIX7_REG_PL_VERSION           0x104 /* 0x410 */ /* Read       */
#define ARTIX7_REG_GPIO_CONFIG          0x107 /* 0x41C */ /* Read/Write */


/* Interrupt Register Area */
#define ARTIX7_REG_IRQ_HOST_EVENT       0x112 /* 0x448 */
#define ARTIX7_REG_IRQ_HOST_MASK        0x113 /* 0x44C */

#define ARTIX7_IRQ_BIU_1               (1<<8)
#define ARTIX7_IRQ_BIU_2               (1<<9)

#define ARTIX7_IRQ_ALL                 (ARTIX7_IRQ_BIU_1 | ARTIX7_IRQ_BIU_2)


/* IRIG */
#define ARTIX7_REG_TIME_SOURCE_CONTROL_BIT_EXTERN     (1<<8)
#define ARTIX7_REG_TIME_SOURCE_CONTROL_BIT_OUT_SIGNAL (1<<1)
#define ARTIX7_REG_TIME_SOURCE_CONTROL_BIT_INSTANT    (1<<0)

/* Trigger Loop */
#define ARTIX7_REG_TRIGGER_DIGITAL_LOOP 0x133 /* 0x4CC */



/* Artix7 Monitor/TCP commands and defines */
#define ARTIX7_MON_CMD_MYMON_VER           0x01
#define ARTIX7_MON_CMD_NOVRAM_FLUSH        0x31
#define ARTIX7_MON_CMD_IRIG_SET            0x40
#define ARTIX7_MON_CMD_IRIG_SET_DECODER    0x49
#define ARTIX7_MON_CMD_FLASH_BLOCK_WRITE   0x50
#define ARTIX7_MON_CMD_FLASH_SECTOR_ERASE  0x51
#define ARTIX7_MON_CMD_FLASH_SECTOR_VERIFY 0x52


#define ARTIX7_MON_OK                    0x0000
#define ARTIX7_MON_PASS                  0x01

#define ARTIX7_MON_ERR_MEM_ACCESS_FAILED 0x0005
#define ARTIX7_MON_ERR_TIMEOUT           0x0006
#define ARTIX7_MON_ERR_STATUS            0x0007
#define ARTIX7_MON_ERR_RANGE_CHECK       0x0008
#define ARTIX7_MON_ERR_BUSY              0x0009

#define ARTIX7_MON_STATUS_FREE           0x00400000
#define ARTIX7_MON_STATUS_WORKING        0x00800000
#define ARTIX7_MON_STATUS_MASK           0x00C00000
#define ARTIX7_MON_STATUS_CMD            0xFF000000


#endif /* __AIHWARTIX_H__ */


