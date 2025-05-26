/* SPDX-FileCopyrightText: 2017-2024 AIM GmbH <info@aim-online.com> */
/* SPDX-License-Identifier: MIT */

/*! \file ApiHw.c
 *
 * HW dependent functions
 * 
 */

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



AiUInt32 ApiHwBiuIsHs(AiUInt32 device_type, AiUInt8 biu )
{
    switch (device_type)
    {
    case TYPE_ACX3910_2:
    case TYPE_ACX3910Xp_2:
    case TYPE_AVX3910_2:
    case TYPE_AVX3910Xp_2:
        if ((1 == biu) || (3 == biu))
            return 1;
        break;
    case TYPE_ACX_EFA_2_TWO_PBIS:
    case TYPE_ACX_EFAXp_2_TWO_PBIS:
    case TYPE_ACX_EFA_2_DS_TWO_PBIS:
    case TYPE_ACX_EFAXp_2_DS_TWO_PBIS:
    case TYPE_AVX_EFA_2_TWO_PBIS:
    case TYPE_AVX_EFAXp_2_TWO_PBIS:
    case TYPE_AVX_EFA_2_DS_TWO_PBIS:
    case TYPE_AVX_EFAXp_2_DS_TWO_PBIS:
        if (3 == biu)
            return 1;
        break;
    case TYPE_ACX_EFA_4_TWO_PBIS:
    case TYPE_ACX_EFAXp_4_TWO_PBIS:
    case TYPE_AVX_EFA_4_TWO_PBIS:
    case TYPE_AVX_EFAXp_4_TWO_PBIS:
        if (5 == biu)
            return 1;
        break;
    case TYPE_ACX_EFA_1_TWO_PBIS:
    case TYPE_ACX_EFAXp_1_TWO_PBIS:
    case TYPE_ACX_EFA_1_DS_TWO_PBIS:
    case TYPE_ACX_EFAXp_1_DS_TWO_PBIS:
    case TYPE_AVX_EFA_1_TWO_PBIS:
    case TYPE_AVX_EFAXp_1_TWO_PBIS:
    case TYPE_AVX_EFA_1_DS_TWO_PBIS:
    case TYPE_AVX_EFAXp_1_DS_TWO_PBIS:
        if (2 == biu)
            return 1;
        break;
    case TYPE_APX3910:
    case TYPE_APX3910Xp:
    case TYPE_APXX3910:
    case TYPE_APXX3910Xp:
    case TYPE_APEX3910:
    case TYPE_APEX3910Xp:
    case TYPE_ACE3910:
    case TYPE_ACE3910Xp:
    case TYPE_AXC3910:
    case TYPE_AXC3910Xp:
    case TYPE_ACX3910:
    case TYPE_ACX3910Xp:
    case TYPE_AVX3910:
    case TYPE_AVX3910Xp:
        if (1 == biu)
            return 1;
        break;
    default:
        return 0;
        break;
    }

    return 0;
}





AiUInt32 ApiHwBiuIntHsFlag(AiUInt32 device_type, AiUInt8 biu, AiUInt32 stream )
{

    switch (device_type)
    {
    case TYPE_ACX3910_2:
    case TYPE_ACX3910Xp_2:
    case TYPE_AVX3910_2:
    case TYPE_AVX3910Xp_2:
        if (API_INT_HS == biu)
            return 1; // set HS flag in module handle
        break;
    case TYPE_ACX_EFA_2_TWO_PBIS:
    case TYPE_ACX_EFAXp_2_TWO_PBIS:
    case TYPE_ACX_EFA_2_DS_TWO_PBIS:
    case TYPE_ACX_EFAXp_2_DS_TWO_PBIS:
    case TYPE_AVX_EFA_2_TWO_PBIS:
    case TYPE_AVX_EFAXp_2_TWO_PBIS:
    case TYPE_AVX_EFA_2_DS_TWO_PBIS:
    case TYPE_AVX_EFAXp_2_DS_TWO_PBIS:
        if (API_STREAM_3 == stream)
        {
            if (API_INT_HS == biu)
                return 1; // set HS flag in module handle
        }
        break;
    case TYPE_ACX_EFA_4_TWO_PBIS:
    case TYPE_ACX_EFAXp_4_TWO_PBIS:
    case TYPE_AVX_EFA_4_TWO_PBIS:
    case TYPE_AVX_EFAXp_4_TWO_PBIS:
        if (API_STREAM_5 == stream)
        {
            if (API_INT_HS == biu)
                return 1; // set HS flag in module handle
        }
        break;
    case TYPE_ACX_EFA_1_TWO_PBIS:
    case TYPE_ACX_EFAXp_1_TWO_PBIS:
    case TYPE_ACX_EFA_1_DS_TWO_PBIS:
    case TYPE_ACX_EFAXp_1_DS_TWO_PBIS:
    case TYPE_AVX_EFA_1_TWO_PBIS:
    case TYPE_AVX_EFAXp_1_TWO_PBIS:
    case TYPE_AVX_EFA_1_DS_TWO_PBIS:
    case TYPE_AVX_EFAXp_1_DS_TWO_PBIS:
        if (API_STREAM_2 == stream)
        {
            if (API_INT_HS == biu)
                return 1; // set HS flag in module handle
        }
        break;
    case TYPE_APX3910:
    case TYPE_APX3910Xp:
    case TYPE_APXX3910:
    case TYPE_APXX3910Xp:
    case TYPE_APEX3910:
    case TYPE_APEX3910Xp:
    case TYPE_ACE3910:
    case TYPE_ACE3910Xp:
    case TYPE_AXC3910:
    case TYPE_AXC3910Xp:
    case TYPE_ACX3910:
    case TYPE_ACX3910Xp:
    case TYPE_AVX3910:
    case TYPE_AVX3910Xp:
    case TYPE_ANET3910:
    case TYPE_ANET3910Xp:
        if (API_INT_HS == biu)
            return 1; // set HS flag in module handle
        break;
    default:
        return 0;
        break;
    }

    return 0;
}





AiUInt32 ApiHwBiuCbOffset(TY_API_BOARD_INFO * board_info, AiUInt8 prot, AiUInt8 biu )
{
    AiUInt32 ul_BiuOffset = 0;
    
    /* First check baords with more than one PBI */
    switch(board_info->ul_DeviceType)
    {
    case TYPE_ACX1553_2_TWO_PBIS:
    case TYPE_AVX1553_2_TWO_PBIS:
        switch (biu)
        {
        case API_BIU_1:
            ul_BiuOffset = 0;
            break;
        case API_BIU_2:
        default:
            ul_BiuOffset = board_info->ul_GlobalRamStartOffset[2];
            break;
        }
        break;
    case TYPE_ACX1553_4_TWO_PBIS:
    case TYPE_ACX1553_4_DS_TWO_PBIS:
    case TYPE_AVX1553_4_TWO_PBIS:
    case TYPE_AVX1553_4_DS_TWO_PBIS:
    case TYPE_ACX3910_2:
    case TYPE_ACX3910Xp_2:
    case TYPE_AVX3910_2:
    case TYPE_AVX3910Xp_2:
    case TYPE_ACX_EFA_2_TWO_PBIS:
    case TYPE_ACX_EFAXp_2_TWO_PBIS:
    case TYPE_ACX_EFA_2_DS_TWO_PBIS:
    case TYPE_ACX_EFAXp_2_DS_TWO_PBIS:
    case TYPE_AVX_EFA_2_TWO_PBIS:
    case TYPE_AVX_EFAXp_2_TWO_PBIS:
    case TYPE_AVX_EFA_2_DS_TWO_PBIS:
    case TYPE_AVX_EFAXp_2_DS_TWO_PBIS:
        switch (biu)
        {
        case API_BIU_1:
            ul_BiuOffset = 0;
            break;
        case API_BIU_2:
            ul_BiuOffset = 0x10000;
            break;
        case API_BIU_3:
            ul_BiuOffset = board_info->ul_GlobalRamStartOffset[2];
            break;
        case API_BIU_4:
        default:
            ul_BiuOffset = board_info->ul_GlobalRamStartOffset[2];
            ul_BiuOffset += 0x10000;
            break;
        }
        break;
    case TYPE_ACX_EFA_1_TWO_PBIS:
    case TYPE_ACX_EFAXp_1_TWO_PBIS:
    case TYPE_ACX_EFA_1_DS_TWO_PBIS:
    case TYPE_ACX_EFAXp_1_DS_TWO_PBIS:
    case TYPE_AVX_EFA_1_TWO_PBIS:
    case TYPE_AVX_EFAXp_1_TWO_PBIS:
    case TYPE_AVX_EFA_1_DS_TWO_PBIS:
    case TYPE_AVX_EFAXp_1_DS_TWO_PBIS:
        switch (biu)
        {
        case API_BIU_1:
            ul_BiuOffset = 0;
            break;
        case API_BIU_2:
            ul_BiuOffset = board_info->ul_GlobalRamStartOffset[2];
            break;
        case API_BIU_3:
        default:
            ul_BiuOffset = board_info->ul_GlobalRamStartOffset[2];
            ul_BiuOffset += 0x10000;
            break;
        }
        break;
    case TYPE_ACX_EFA_4_TWO_PBIS:
    case TYPE_ACX_EFAXp_4_TWO_PBIS:
    case TYPE_AVX_EFA_4_TWO_PBIS:
    case TYPE_AVX_EFAXp_4_TWO_PBIS:
        switch (biu)
        {
        case API_BIU_1:
            ul_BiuOffset = 0;
            break;
        case API_BIU_2:
            ul_BiuOffset = 0x10000;
            break;
        case API_BIU_3:
            ul_BiuOffset = 0x20000;
            break;
        case API_BIU_4:
            ul_BiuOffset = 0x30000;
            break;
        case API_BIU_5:
            ul_BiuOffset = board_info->ul_GlobalRamStartOffset[2];
            break;
        case API_BIU_6:
        default:
            ul_BiuOffset = board_info->ul_GlobalRamStartOffset[2];
            ul_BiuOffset += 0x10000;
            break;
        }
        break;
    case TYPE_ACX1553_8:
    case TYPE_AVX1553_8:
        switch (biu)
        {
        case API_BIU_1:
            ul_BiuOffset = 0;
            break;
        case API_BIU_2:
            ul_BiuOffset = 0x10000;
            break;
        case API_BIU_3:
            ul_BiuOffset = 0x20000;
            break;
        case API_BIU_4:
            ul_BiuOffset = 0x30000;
            break;
        case API_BIU_5:
            ul_BiuOffset = board_info->ul_GlobalRamStartOffset[2];
            break;
        case API_BIU_6:
            ul_BiuOffset = board_info->ul_GlobalRamStartOffset[2];
            ul_BiuOffset += 0x10000;
            break;
        case API_BIU_7:
            ul_BiuOffset = board_info->ul_GlobalRamStartOffset[2];
            ul_BiuOffset += 0x20000;
            break;
        case API_BIU_8:
        default:
            ul_BiuOffset = board_info->ul_GlobalRamStartOffset[2];
            ul_BiuOffset += 0x30000;
            break;
        }
        break;
    default:
        /* Boards with a single PBI are the default case */
        ul_BiuOffset = board_info->ul_GlobalRamStartOffset[0];

        switch (biu)
        {
        case API_BIU_1:
            ul_BiuOffset += 0;
            break;
        case API_BIU_2:
            ul_BiuOffset += 0x10000;
            break;
        case API_BIU_3:
            ul_BiuOffset += 0x20000;
            break;
        case API_BIU_4:
        default:
            ul_BiuOffset += 0x30000;
            break;
        }
    }

    return ul_BiuOffset;
}




AiReturn ApiHwStreamToBiu(TY_API_BOARD_INFO * board_info, AiUInt32 stream, AiUInt32 hs, AiUInt32 * biu)
{
    if (stream == 0)
        /* valid values are 1..8 */
        return API_ERR_WRONG_STREAM;

    /* First check all 3910 boards. 
       One stream consists of one 1553 BIU and one 3910 BIU. 
       The hs flag controls which BIU is returned. 
    */
    switch (board_info->ul_DeviceType)
    {
    case TYPE_ACX3910_2:
    case TYPE_ACX3910Xp_2:
    case TYPE_AVX3910_2:
    case TYPE_AVX3910Xp_2:
        if (stream > API_STREAM_2)
            return API_ERR_WRONG_STREAM;
        else
        {
            if (1 == stream)
                *biu = API_BIU_1 + hs/*ul_OffsetTemp*/;
            if (2 == stream)
                *biu = API_BIU_3 + hs/*ul_OffsetTemp*/;
        }
        break;
    case TYPE_ACX_EFA_2_TWO_PBIS:
    case TYPE_ACX_EFAXp_2_TWO_PBIS:
    case TYPE_ACX_EFA_2_DS_TWO_PBIS:
    case TYPE_ACX_EFAXp_2_DS_TWO_PBIS:
    case TYPE_AVX_EFA_2_TWO_PBIS:
    case TYPE_AVX_EFAXp_2_TWO_PBIS:
    case TYPE_AVX_EFA_2_DS_TWO_PBIS:
    case TYPE_AVX_EFAXp_2_DS_TWO_PBIS:
        if (stream > API_STREAM_3)
            return API_ERR_WRONG_STREAM;
        else
        {
            if ((1 == stream) || (2 == stream))
                *biu = stream;
            else
                *biu = API_BIU_3 + hs/*ul_OffsetTemp*/;
        }
        break;

    case TYPE_ACX_EFA_4_TWO_PBIS:
    case TYPE_ACX_EFAXp_4_TWO_PBIS:
    case TYPE_AVX_EFA_4_TWO_PBIS:
    case TYPE_AVX_EFAXp_4_TWO_PBIS:
        if (stream > API_STREAM_5)
            return API_ERR_WRONG_STREAM;
        else
        {
            if ((1 == stream) || (2 == stream) || (3 == stream) || (4 == stream))
                *biu = stream;
            else
                *biu = API_BIU_5 + hs/*ul_OffsetTemp*/;
        }
        break;
    case TYPE_ACX_EFA_1_TWO_PBIS:
    case TYPE_ACX_EFAXp_1_TWO_PBIS:
    case TYPE_ACX_EFA_1_DS_TWO_PBIS:
    case TYPE_ACX_EFAXp_1_DS_TWO_PBIS:
    case TYPE_AVX_EFA_1_TWO_PBIS:
    case TYPE_AVX_EFAXp_1_TWO_PBIS:
    case TYPE_AVX_EFA_1_DS_TWO_PBIS:
    case TYPE_AVX_EFAXp_1_DS_TWO_PBIS:
        if (stream > API_STREAM_2)
            return API_ERR_WRONG_STREAM;
        else
        {
            if (1 == stream)
                *biu = stream;
            else
                *biu = API_BIU_3 + hs/*ul_OffsetTemp*/;
        }
        break;

    case TYPE_APX3910:
    case TYPE_APX3910Xp:
    case TYPE_APXX3910:
    case TYPE_APXX3910Xp:
    case TYPE_APEX3910:
    case TYPE_APEX3910Xp:
    case TYPE_ACE3910:
    case TYPE_ACE3910Xp:
    case TYPE_AXC3910:
    case TYPE_AXC3910Xp:
    case TYPE_ACX3910:
    case TYPE_ACX3910Xp:
    case TYPE_AVX3910:
    case TYPE_AVX3910Xp:
    case TYPE_ANET3910:
    case TYPE_ANET3910Xp:
        if (stream > API_STREAM_1)
            return API_ERR_WRONG_STREAM;
        else
            *biu = API_BIU_1 + hs/*ul_OffsetTemp*/;
        break;
    default:
        /* For 1553 boards the mapping is biu equals stream */
        if (stream > board_info->ul_NumberOfChannels)
            return API_ERR_WRONG_STREAM;
        else
            *biu = stream;
        break;
    }

    return 0;
}

AiBoolean ApiHwIsUsb(AiUInt32 BoardConfig)
{
    AiInt8 uc_DeviceGroup = 0;
    switch (BoardConfig & 0x000000FF)
    {
    case AI_PLATFORM_USB:
    case AI_PLATFORM_ASC:
    case AI_PLATFORM_USB3:
        return AiTrue;
    default:
        return AiFalse;
    }
}

void ApiHwDeviceInfoStreamInit(TY_DEVICE_INFO * p_DeviceInfo)
{
    memset(&p_DeviceInfo->x_Stream[0], 0, sizeof(p_DeviceInfo->x_Stream[MAX_API_STREAM]));

    switch (p_DeviceInfo->x_BoardInfo.ul_DeviceType)
    {

    case TYPE_ACX1553_8:
    case TYPE_AVX1553_8:
        p_DeviceInfo->x_Stream[4].ul_PbiBase = 0x02000000;
        p_DeviceInfo->x_Stream[5].ul_PbiBase = 0x02000000;
        p_DeviceInfo->x_Stream[6].ul_PbiBase = 0x02000000;
        p_DeviceInfo->x_Stream[7].ul_PbiBase = 0x02000000;
        break;
    case TYPE_ACX1553_4_TWO_PBIS:
    case TYPE_ACX1553_4_DS_TWO_PBIS:
    case TYPE_AVX1553_4_TWO_PBIS:
    case TYPE_AVX1553_4_DS_TWO_PBIS:
        p_DeviceInfo->x_Stream[2].ul_PbiBase = 0x02000000;
        p_DeviceInfo->x_Stream[3].ul_PbiBase = 0x02000000;
        break;
    case TYPE_ACX3910_2:
    case TYPE_ACX3910Xp_2:
    case TYPE_AVX3910_2:
    case TYPE_AVX3910Xp_2:
        p_DeviceInfo->x_Stream[0].ul_HighSpeed = 1;
        p_DeviceInfo->x_Stream[1].ul_HighSpeed = 1;
        p_DeviceInfo->x_Stream[1].ul_PbiBase = 0x02000000;
        break;
    case TYPE_ACX_EFA_2_TWO_PBIS:
    case TYPE_ACX_EFAXp_2_TWO_PBIS:
    case TYPE_ACX_EFA_2_DS_TWO_PBIS:
    case TYPE_ACX_EFAXp_2_DS_TWO_PBIS:
    case TYPE_AVX_EFA_2_TWO_PBIS:
    case TYPE_AVX_EFAXp_2_TWO_PBIS:
    case TYPE_AVX_EFA_2_DS_TWO_PBIS:
    case TYPE_AVX_EFAXp_2_DS_TWO_PBIS:
        p_DeviceInfo->x_Stream[0].ul_HighSpeed = 0;
        p_DeviceInfo->x_Stream[1].ul_HighSpeed = 0;
        p_DeviceInfo->x_Stream[2].ul_HighSpeed = 1;
        p_DeviceInfo->x_Stream[2].ul_PbiBase = 0x02000000;
        break;
    case TYPE_ACX_EFA_4_TWO_PBIS:
    case TYPE_ACX_EFAXp_4_TWO_PBIS:
    case TYPE_AVX_EFA_4_TWO_PBIS:
    case TYPE_AVX_EFAXp_4_TWO_PBIS:
        p_DeviceInfo->x_Stream[0].ul_HighSpeed = 0;
        p_DeviceInfo->x_Stream[1].ul_HighSpeed = 0;
        p_DeviceInfo->x_Stream[2].ul_HighSpeed = 0;
        p_DeviceInfo->x_Stream[3].ul_HighSpeed = 0;
        p_DeviceInfo->x_Stream[4].ul_HighSpeed = 1;
        p_DeviceInfo->x_Stream[4].ul_PbiBase = 0x02000000;
        break;
    case TYPE_ACX_EFA_1_TWO_PBIS:
    case TYPE_ACX_EFAXp_1_TWO_PBIS:
    case TYPE_ACX_EFA_1_DS_TWO_PBIS:
    case TYPE_ACX_EFAXp_1_DS_TWO_PBIS:
    case TYPE_AVX_EFA_1_TWO_PBIS:
    case TYPE_AVX_EFAXp_1_TWO_PBIS:
    case TYPE_AVX_EFA_1_DS_TWO_PBIS:
    case TYPE_AVX_EFAXp_1_DS_TWO_PBIS:
        p_DeviceInfo->x_Stream[0].ul_HighSpeed = 0;
        p_DeviceInfo->x_Stream[1].ul_HighSpeed = 1;
        p_DeviceInfo->x_Stream[1].ul_PbiBase = 0x02000000;
        break;
    case TYPE_APX3910:
    case TYPE_APX3910Xp:
    case TYPE_APXX3910:
    case TYPE_APXX3910Xp:
    case TYPE_APEX3910:
    case TYPE_APEX3910Xp:
    case TYPE_ACE3910:
    case TYPE_ACE3910Xp:
    case TYPE_AXC3910:
    case TYPE_AXC3910Xp:
    case TYPE_ACX3910:
    case TYPE_ACX3910Xp:
    case TYPE_AVX3910:
    case TYPE_AVX3910Xp:
        p_DeviceInfo->x_Stream[0].ul_HighSpeed = 1;
        break;
    }
}


const char * ApiHwPlatformName( AiUInt32 platform )
{
    switch (platform)
    {
    case AI_PLATFORM_CPCIX_3U:
    case AI_PLATFORM_CPCIX_6U:
        return "ACX";
        break;
    case AI_PLATFORM_VMEX_B:
    case AI_PLATFORM_VMEX_A:
        return "AVX";
        break;
    case AI_PLATFORM_PCIE_CARD:
        return "AEC";
        break;
    case AI_PLATFORM_XMC:
        return "AXC";
        break;
    case AI_PLATFORM_XMC_EBD:
        return "AXE";
        break;
    case AI_PLATFORM_XMC_NOREPLAY_NOERROR:
        return "ASE";
        break;
    case AI_PLATFORM_PMC_XMC_BASED:
        return "AMCX";
        break;
    case AI_PLATFORM_PMC_BASED_EBD:
        return "AMCE";
        break;
    case AI_PLATFORM_PCIE:
        return "APE";
        break;
    case AI_PLATFORM_PCIX_PCIE_BASED:
        return "APXX";
        break;
    case AI_PLATFORM_CPCIE_3U:
        return "ACE";
        break;
    case AI_PLATFORM_CPCIX_3U_PCIE_BASED:
        return "ACXX";
        break;
    case AI_PLATFORM_USB:
        return "APU";
        break;
    case AI_PLATFORM_PCIX:
        return "APX";
        break;
    case AI_PLATFORM_PCIE_PCIX_BASED:
        return "APE";
        break;
    case AI_PLATFORM_ANET:
    case AI_PLATFORM_ANET_AYS:
    case AI_PLATFORM_ANET_AYS_MA:
        return "ANET";
        break;
    case AI_PLATFORM_ASC:
    case AI_PLATFORM_USB3:
        return "ASC";
        break;
    case AI_PLATFORM_PCI_SHORT_ZYNQMP:
        return "APXX";
        break;
    case AI_PLATFORM_PCI_E_1L_ZYNQMP:
        return "APEX";
        break;
    case AI_PLATFORM_CPCIE_3U_ZYNQMP:
        return "ACE";
        break;
    case AI_PLATFORM_MINI_PCIE_CARD:
    case AI_PLATFORM_MINI_PCIE_CARD_AP:
        return "AME";
        break;
    case AI_PLATFORM_MINI_PCIE_ARTIX7:
        return "AMEE";
        break;
    case AI_PLATFORM_M2_B_M_ARTIX7:
        return "AM2E";
        break;
    case AI_PLATFORM_XMC_ZYNQMP:
        return "AXC";
        break;
    case AI_PLATFORM_XMC_ARTIXUS:
        return "AXCX";
        break;
    default:
        return "???";
        break;
    }
    return "???";
}

const char * ApiHwPlatformExtension(AiUInt32 platform)
{
    switch (platform)
    {
    case AI_PLATFORM_CPCIX_3U:
        return "3U";
        break;
    case AI_PLATFORM_CPCIX_6U:
        return "6U";
        break;
    case AI_PLATFORM_PCIE_PCIX_BASED:
        return "(AyX based)";
        break;
    case AI_PLATFORM_MINI_PCIE_CARD_AP:
        return "AP";
        break;
    default:
        return "";
        break;
    }
    return "";
}




AiReturn ApiHwQueryInformation(TY_HW_INFO ax_HardwareInfos[], AiUInt32 *ul_Count)
{
    AiReturn r_RetVal = 0;
    AiUInt32 devindex = 0;
    TY_DEVICE_INFO      * pDevice = NULL;
    TY_DEVICE_INFO_LIST * px_CurrentEntry = NULL;

    /* -- check input parameter -- */

    if (NULL == ax_HardwareInfos)
        return API_ERR_PARAM1_IS_NULL;

    for (px_CurrentEntry = px_MilDeviceInfoListStart;
        px_CurrentEntry != NULL;
        px_CurrentEntry = px_CurrentEntry->px_Next)
    {
        pDevice = &px_CurrentEntry->x_DeviceInfo;
        ax_HardwareInfos[devindex].e_type = pDevice->HardwareInfo.e_type;
        ax_HardwareInfos[devindex].ul_ModuleNo = pDevice->HardwareInfo.ul_ModuleNo;
        ax_HardwareInfos[devindex].hw_types.pci.ul_Bus = pDevice->HardwareInfo.hw_types.pci.ul_Bus;
        ax_HardwareInfos[devindex].hw_types.pci.ul_Slot = pDevice->HardwareInfo.hw_types.pci.ul_Slot;
        ax_HardwareInfos[devindex].hw_types.pci.ul_Func = pDevice->HardwareInfo.hw_types.pci.ul_Func;
        ax_HardwareInfos[devindex].hw_types.pci.ul_Res  = pDevice->HardwareInfo.hw_types.pci.ul_Res;
        devindex++;
    }
    *ul_Count = devindex;

    return r_RetVal;

}
