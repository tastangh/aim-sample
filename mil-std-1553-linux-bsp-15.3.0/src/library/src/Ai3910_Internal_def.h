/* SPDX-FileCopyrightText: 2001-2022 AIM GmbH <info@aim-online.com> */
/* SPDX-License-Identifier: MIT */

/*! \file Ai3910_Internal_def.c
 *
 *  Internal constants and structures for the 3910 Application Interface.
 *
 */

#ifndef AI3910_INTERNAL_DEF_H
#define AI3910_INTERNAL_DEF_H

/*******************************************/
/***  API3910 Extended Instruction Codes ***/
/*******************************************/

/* FOFE Commands */
#define TG_API_HS_FOFE_BIT_DISC           0x08D5

/* GPIO Commands */ /* added 31.10.00 pg */
#define TG_API_HS_GPIO_RESET              0x0860
#define TG_API_HS_GPIO_BROADC_CMD         0x0861
#define TG_API_HS_GPIO_BROADC_WRITE       0x0862
#define TG_API_HS_GPIO_CMD                0x0863
#define TG_API_HS_GPIO_STATUS             0x0864
#define TG_API_HS_GPIO_READ               0x0865
#define TG_API_HS_GPIO_WRITE              0x0866
#define TG_API_HS_GPIO_READ_BLOCK         0x0867
#define TG_API_HS_GPIO_WRITE_BLOCK        0x0868



/* EFEX RT System Control Block*/

// fix API Firmware Addresses
#define API_FW_BM_ACT_PAGE_ADDRESS    0x4000
#define API_FW_BIU2_ADDRESS_OFFSET    0x10000
#define API_FW_BM_HSACT_PAGE_ADDRESS  0x18000

#define API_FW_HSRT_DESC_AREA         0x10800
#define API_FW_HSRT_DESC_SIZE         0x80
#define API_FW_BC_CPL_AMPL_REG        0x10010

#define API_FW_BC_XFER_DESC_BASE      0x10030
#define API_FW_BC_SYNC_TIME_OFFSET    0x10060
#define API_FW_BC_LAST_RCV_SYNC_TIME  0x10064
#define API_FW_BC_LAST_SYNC_TIME      0x10068

#define API_FW_BC_NOP_TRANS_MAST      0xC0000000


#define HS_EFX_TO_EFX      16
#define HS_EFX_TO_EFX_BR   17
#define HS_EFX_TO_EFA      18
#define HS_EFX_TO_MIX_BR   19
#define HS_EFA_TO_EFX      20
#define HS_EFA_TO_EFX_BR   21
#define HS_EFA_TO_EFA      22
#define HS_EFA_TO_EFA_BR   23
#define HS_EFA_TO_MIX_BR   24
#define HS_EFX_MODE        25
#define HS_EFX_MODE_BR     26

#define HS_BCRT 10
#define HS_RTBC 11
#define HS_BCBR 12
#define HS_RTRT 13
#define HS_RTBR 14
#define HS_MODE 15







#endif // AI3910I_DEF_H
// end: ai3910i_def.h

