/* SPDX-FileCopyrightText: 2017-2021 AIM GmbH <info@aim-online.com> */
/* SPDX-License-Identifier: MIT OR GPL-2.0-or-later */

/*
Description:

Implement ZynqMP specific defines in this file
*/

/*! \file AiHwZynqMp.h
    Declare ZynqMP platform specific functions in this file */

#ifndef __AIHWZYNQMP_H__
#define __AIHWZYNQMP_H__



enum ai_hw_zynqmp_hostio_event
{
    AIM_ZYNQMP_HOSTIO_INVALID      = 0,
    AIM_ZYNQMP_HOSTIO_CHN0         = 1<<0,
    AIM_ZYNQMP_HOSTIO_CHN1         = 1<<1,
    AIM_ZYNQMP_HOSTIO_CHN2         = 1<<2,
    AIM_ZYNQMP_HOSTIO_CHN3         = 1<<3,
    AIM_ZYNQMP_HOSTIO_REBOOT       = 1<<30,
    AIM_ZYNQMP_HOSTIO_ASP2HOST_INT = 1<<31
};



/*****************************************************************************
 *                                                                           *
 *                                                                           *
 *                  -- PL registers (0x80000000)   ---                       *
 *                                                                           *
 *                                                                           *
 *****************************************************************************/
#define AI_HW_ZYNQMP_PL_FAIL_LEDS                 (0x00010038)
#define AI_HW_ZYNQMP_PL_FAIL_LEDS_ENUMERATION_BIT (0)
#define AI_HW_ZYNQMP_PL_FAIL_LEDS_PANIC_BIT       (1)



/*****************************************************************************
*                                                                           *
*                                                                           *
*                     -- Host-IO CHN0 (0x3F000000)   ---                    *
*                                                                           *
*                                                                           *
*****************************************************************************/
#define AI_HW_ZYNQMP_HOSTIO_CHANNEL0_OFFSET    0
#define AI_HW_ZYNQMP_HOSTIO_CHANNEL0_SIZE      (64 << 10) /* 64KB */


/*****************************************************************************
 *                                                                           *
 *                                                                           *
 *              -- Host-IO loglist registers (0x3F010000)   ---              *
 *                        azmpa2p_hostio_readl                               *
 *                                                                           *
 *****************************************************************************/
#define AI_HW_ZYNQMP_HOSTIO_IRQ_LOG              (AI_HW_ZYNQMP_HOSTIO_CHANNEL0_OFFSET + AI_HW_ZYNQMP_HOSTIO_CHANNEL0_SIZE)
#define AI_HW_ZYNQMP_HOSTIO_IRQ_LOG_SIZE         (4 << 10) /* 4KB */

#define AI_HW_ZYNQMP_HOSTIO_LOG_PCIE_GET         (0x0)
#define AI_HW_ZYNQMP_HOSTIO_LOG_PCIE_PUT         (0x4)
#define AI_HW_ZYNQMP_HOSTIO_LOG_PCIE_ERROR       (0x8)
#define AI_HW_ZYNQMP_HOSTIO_LOG_PCIE_BASE        (0xC)

#define AI_HW_ZYNQMP_HOSTIO_LOG_AXI_GET          (0x800 + 0x0)
#define AI_HW_ZYNQMP_HOSTIO_LOG_AXI_PUT          (0x800 + 0x4)
#define AI_HW_ZYNQMP_HOSTIO_LOG_AXI_ERROR        (0x800 + 0x8)
#define AI_HW_ZYNQMP_HOSTIO_LOG_AXI_BASE         (0x800 + 0xC)

#define AI_HW_ZYNQMP_HOSTIO_LOG_ENTRY_SIZE       (4)
#define AI_HW_ZYNQMP_HOSTIO_LOG_ENTRY_COUNT      (509) /* 2KB: 512 -PUT -GET -ERROR = 509 entries*/
#define AI_HW_ZYNQMP_HOSTIO_LOG_BYTE_SIZE        (AI_HW_ZYNQMP_HOSTIO_LOG_ENTRY_SIZE * AI_HW_ZYNQMP_HOSTIO_LOG_ENTRY_COUNT)



/*****************************************************************************
 *                                                                           *
 *                                                                           *
 *                  -- DMA registers (0xFD0F0000)   ---                      *
 *                        azmpa2p_dmareg_writel                              *
 *                                                                           *
 *****************************************************************************/
#define AI_HW_ZYNQMP_DMAREG_SCRATCH0                      (0x50U)
#define AI_HW_ZYNQMP_DMAREG_PCIE_INT_MASK_OFFSET          (0x60U)
#define AI_HW_ZYNQMP_DMAREG_DMA_INT_MASK_OFFSET          (0x464U)
#define AI_HW_ZYNQMP_DMAREG_DMA0_CHANNEL_PCIE_EVENT_REG   (0x64U)
#define AI_HW_ZYNQMP_DMAREG_DMA0_CHANNEL_AXI_INT_STATUS   (0x6CU)
#define AI_HW_ZYNQMP_DMAREG_DMA0_CHANNEL_AXI_INT_CONTROL  (0x68U)
#define AI_HW_ZYNQMP_DMAREG_DMA0_CHANNEL_PCIE_INT_CONTROL (0x60U)
#define AI_HW_ZYNQMP_DMAREG_DMA0_CHANNEL_PCIE_INT_ASSERT  (0x70U)
#define AI_HW_ZYNQMP_DMAREG_DMA0_CHANNEL_AXI_EVENT_REG    (0x74U)


/*****************************************************************************
 *                                                                           *
 *                                                                           *
 *                         -- Register values ---                            *
 *                                                                           *
 *                                                                           *
 *****************************************************************************/
#define AI_HW_ZYNQMP_HOSTIO_LOG_OVERFLOW         0x1U
#define AI_HW_ZYNQMP_AZMPA2P_BRIDGE_READY        0x52454459U /*REDY*/
#define AI_HW_ZYNQMP_DMAREG_SWINT                0x8U        /*1<<3*/
#define AI_HW_ZYNQMP_DMAREG_SGLINT               0x4U        /*1<<2*/
#define AI_HW_ZYNQMP_DMAREG_ERRINT               0x2U        /*1<<1*/
#define AI_HW_ZYNQMP_DMAREG_INT_ENABLE           0x1U
#define AI_HW_ZYNQMP_DMAREG_PCIE_INT_MASK        0x1U

/*****************************************************************************
 *                                                                           *
 *                                                                           *
 *                         -- PXI Register values ---                        *
 *                                                                           *
 *                                                                           *
 *****************************************************************************/
#define AI_HW_ZYNQMP_REG_PXI_TRIGGER_OUT_SELECT  0x40C0     /* 0x10300 Read/Write */
#define AI_HW_ZYNQMP_REG_PXI_TRIGGER_OUT_ENABLE  0x40C1     /* 0x10304 Read/Write */
#define AI_HW_ZYNQMP_REG_PXI_TRIGGER_IN1_SELECT  0x40C2     /* 0x10308 Read/Write */
#define AI_HW_ZYNQMP_REG_PXI_TRIGGER_IN2_SELECT  0x40C3     /* 0x1030C Read/Write */
#define AI_HW_ZYNQMP_REG_PXI_IRIG_SOURCE_CONTROL 0x40C5     /* 0x10314 Read/Write */
#define AI_HW_ZYNQMP_REG_PXI_GEOGRAPHICAL_ADDR   0x40C6     /* 0x10318 Read       */

#define AI_HW_ZYNQMP_REG_PXI_GEOGRAPHICAL_ADDR_MASK 0x1FL    /* valid address bits mask*/

#endif /* __AIHWZYNQMP_H__ */


