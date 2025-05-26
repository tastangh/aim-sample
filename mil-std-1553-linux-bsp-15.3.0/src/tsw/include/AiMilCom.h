/* SPDX-FileCopyrightText: 2015-2022 AIM GmbH <info@aim-online.com> */
/* SPDX-License-Identifier: MIT OR GPL-2.0-or-later */

/*! \file AiMilCom.h
This file contains communication structures for the host to target and host to driver
communication. Each command might define its own command and or acknowlege struct. 
All host to target commands must include the MIL_COM struct as first element. 
All host to target acknowlege structs must include MIL_COM_ACK as first element. 
If a command does not need additional command information it might use MIL_COM as input. 
If a command does not return any data it should use the MIL_COM_ACK stuct as output.

Each struct might use types from Ai1553Com.h which are available at the user API. */

#ifndef __MIL_COM_H__
#define __MIL_COM_H__

#include "Ai_cdef.h"
#include "Ai1553Com.h"
#include "AiMilComSys.h"
#include "AiMilComLsRt.h"
#include "AiMilComHsRt.h"
#include "AiMilComLsBc.h"
#include "AiMilComHsBc.h"
#include "AiMilComEfBc.h"
#include "AiMilComLsBm.h"
#include "AiMilComHsBm.h"
#include "AiMilComHsBuf.h"
#include "AiMilComScope.h"
#include "AiMilComLsReplay.h"
#include "AiMilComHsReplay.h"
#include "AiMilComCustom.h"
#include "AiMilComLsBuf.h"


/* Magic to detect that this version of the Host to target communication is used */
#define MIL_COM_MAGIC             0xA1A1A1A1          

/* This value is written into the hw information struct when the ASP is ready */
#define MIL_COM_ASP_RUNNING       0x52454459    /* REDY */


/*****************************************************************************/
/*                                                                           */
/*                              command definitions                          */
/*                                                                           */
/*****************************************************************************/

/* 
Attention:

command & MIL_COM_GROUP_MASK    => Used to get the command sub table.
command & MIL_COM_CMD_MASK      => Used as array index within the sub table.
command & MIL_COM_GROUP_HS_MASK => Used to detect if command is HS requires HS biu
*/

#define MIL_COM_CMD_MASK             0x0000FFFF
#define MIL_COM_GROUP_MASK           0xFFFF0000
#define MIL_COM_GROUP_HS_MASK        0x00800000

#define MIL_COM_SYS_OFFSET           0x01000000 /* See AiMilComSys.h for device command definitions */
#define MIL_COM_SYS_HS_OFFSET        0x01800000 /* See AiMilComSys.h for device command definitions */
#define MIL_COM_SYS_CAL_OFFSET       0x01010000 /* See AiMilComSys.h for device command definitions */
#define MIL_COM_SYS_TRACK_OFFSET     0x01020000 /* See AiMilComSys.h for device command definitions */
#define MIL_COM_SYS_FIFO_OFFSET      0x01030000 /* See AiMilComSys.h for device command definitions */
#define MIL_COM_SYS_UTIL_OFFSET      0x01040000 /* See AiMilComSys.h for device command definitions */
#define MIL_COM_SYS_VERSIONS_OFFSET  0x01050000 /* See AiMilComSys.h for device command definitions */
#define MIL_COM_SYS_EXEC_OFFSET      0x01060000 /* See AiMilComSys.h for device command definitions */
#define MIL_COM_SYS_BITE_OFFSET      0x01070000 /* See AiMilComSys.h for device command definitions */
#define MIL_COM_SYS_INI_OFFSET       0x01080000 /* See AiMilComSys.h for device command definitions */

#define MIL_COM_SYS_HS_CAL_OFFSET    0x01810000 /* See AiMilComSys.h for device command definitions */
#define MIL_COM_SYS_HS_TRACK_OFFSET  0x01820000 /* See AiMilComSys.h for device command definitions */

#define MIL_COM_BC_OFFSET            0x02000000
#define MIL_COM_BC_LS_OFFSET         0x02000000 /* See AiMilComLsBc.h for LS BC command definitions */
#define MIL_COM_BC_HS_OFFSET         0x02800000 /* See AiMilComHsBc.h for HS BC command definitions */
#define MIL_COM_BC_EF_OFFSET         0x02810000 /* See AiMilComEfBc.h for EF BC command definitions */

#define MIL_COM_RT_OFFSET            0x03000000
#define MIL_COM_RT_LS_OFFSET         0x03000000 /* See AiMilComLsRt.h for LS RT command definitions */
#define MIL_COM_RT_HS_OFFSET         0x03800000 /* See AiMilComHsRt.h for HS RT command definitions */
#define MIL_COM_RT_EF_OFFSET         0x03810000 /* See AiMilComEfRt.h for EF RT command definitions */

#define MIL_COM_BM_OFFSET            0x04000000
#define MIL_COM_BM_LS_OFFSET         0x04000000 /* See AiMilComLsBm.h for LS BM command definitions */
#define MIL_COM_BM_LS_DQUEUE_OFFSET  0x04010000 /* See AiMilComLsBm.h for EF BM command definitions */
#define MIL_COM_BM_HS_OFFSET         0x04800000 /* See AiMilComHsBm.h for HS BM command definitions */
#define MIL_COM_BM_EF_OFFSET         0x04810000 /* See AiMilComEfBm.h for EF BM command definitions */

#define MIL_COM_SCOPE_OFFSET         0x05000000 /* See AiMilComScope.h for MilScope command definitions */

#define MIL_COM_REP_OFFSET           0x06000000 /* See AiMilComLsReplay.h for Replay command definitions */
#define MIL_COM_REP_HS_OFFSET        0x06800000 /* See AiMilComHsReplay.h for Replay command definitions */

#define MIL_COM_BUF_LS_OFFSET        0x07000000 /* See AiMilComLsBuf.h for Buffer command definitions */


#define MIL_COM_BUF_HS_OFFSET        0x07800000 /* See AiMilComHsBuf.h for HS buffer command definitions */


#define MIL_COM_CUSTOM_SYSTEM_OFFSET 0xFF000000 /* See AiMilComCustom.h for Custom System command definitions */
#define MIL_COM_CUSTOM_DTS_OFFSET    0xFF010000 /* See AiMilComCustom.h for Custom System command definitions */





/*****************************************************************************/
/*                                                                           */
/*                    Host to Driver communication                           */
/*                       IOCTL data definitions                              */
/*                                                                           */
/*****************************************************************************/

typedef struct _TY_MIL_COM_USB_MEMORY_TRANSFER
{
    AiUInt64 userBufferAddress; /*!< address of user space buffer to read/write from/to */
    AiUInt32 memtype;            /*!< Memory type to read/write. Either USB_EEPROM or USB_8051_RAM */
    AiUInt32 offset;            /*!< Relative byte offset in memory area to read/write from/to */
    AiUInt32 size;              /*!< Number of bytes to read/write */
    AiUInt32 reserved;          /*!< reserved value for correct padding */
}TY_MIL_COM_USB_MEMORY_TRANSFER;


/*! Structure that represents a MilScope buffer
    which is given from user-space via system-driver to
    board's DMA engine. */
typedef struct _TY_MILSCOPE_BUFFER
{
    TY_API_SCOPE_BUFFER_TYPE eType;  /*!< \ref API_SCOPE_BUFFER_TYPE enum which gives type of buffer */
    AiUInt32 ulSize;                 /*!< size of buffer in bytes */
    AiUInt64 ullVirtualAddress;      /*!< virtual address of user-space buffer */
}TY_MILSCOPE_BUFFER;








#endif /* __MIL_COM_H__ */
