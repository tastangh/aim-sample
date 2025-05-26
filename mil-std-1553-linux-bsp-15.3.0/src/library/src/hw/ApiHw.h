/* SPDX-FileCopyrightText: 2017-2022 AIM GmbH <info@aim-online.com> */
/* SPDX-License-Identifier: MIT */

/*! \file ApiHw.h
 *
 *  Wrap HW related functions
 *
 */

#ifndef AI_HW_H
#define AI_HW_H

#include "AiOs.h"
#include "Ai_cdef.h"
#include "Ai_def.h"

#ifdef _API3910
#include "Ai3910i_def.h"
#include "Ai3910i_fnc.h"
#include "Ai1553_Internal_def.h"
#include "Ai3910_Internal_def.h"
#else
#include "Ai1553i_def.h"
#include "Ai1553i_fnc.h"
#include "Ai1553_Internal_def.h"
#endif
#include "DEFS_V.H"
#include "Dll_Fnc.h"


AiUInt32 ApiHwBiuIsHs(AiUInt32 device_type, AiUInt8 biu);
AiUInt32 ApiHwBiuIntHsFlag(AiUInt32 device_type, AiUInt8 biu, AiUInt32 stream);
AiUInt32 ApiHwBiuCbOffset(TY_API_BOARD_INFO* board_info, AiUInt8 prot, AiUInt8 biu);
AiReturn ApiHwStreamToBiu(TY_API_BOARD_INFO * board_info, AiUInt32 stream, AiUInt32 hs, AiUInt32 * biu);
void ApiHwCapabilitiesGet(AiUInt32 device_type, TY_API_BOARD_INFO * board_info);
AiBoolean ApiHwIsUsb(AiUInt32 BoardConfig);
void ApiHwDeviceInfoStreamInit(TY_DEVICE_INFO * p_DeviceInfo);

const char * ApiHwPlatformName(AiUInt32 platform);
const char * ApiHwPlatformExtension(AiUInt32 platform);

AiInt8 ApiHwBoardConfigToDeviceType(AiUInt32 BoardConfig, AiUInt32 BoardType);


#endif /*AI_HW_H*/
