/* SPDX-FileCopyrightText: 2009-2021 AIM GmbH <info@aim-online.com> */
/* SPDX-License-Identifier: MIT  */

/* SmartNvr.h */

#pragma once

#include "Ai_cdef.h"
#include "Ai_def.h"

#define SL_MAX_NVR_ENTRIES 0x1000
#define SL_MAX_NVR_EOL     0xFFFFFFFF

struct sl_nvr_entry
{
    const char * description;
    AiUInt32     offset;
};


static struct sl_nvr_entry sl_empty[] ={
    { "EOL", SL_MAX_NVR_EOL }
};

static struct sl_nvr_entry sl_platform_aye[] ={
    { "Board Serial Number                       ", 0x00000004 },
    { "Build DATE(xxCWYYYY)                      ", 0x00000028 },
    { "NOVRAM Enable Code                        ", 0x00000024 },
    { "Board Config                              ", 0x00000008 },
    { "Board Type                                ", 0x00000010 },
    { "Board Sub Type                            ", 0x00000014 },
    { "Board Application Type                    ", 0x00000018 },
    { "Board Part Number                         ", 0x0000001C },
    { "Board Part Number Extension               ", 0x00000020 },
    { "SOC-Version / Revision / Buildnumber(FPGA)", 0x00000034 },
    { "Hardware Variant Word Definition          ", 0x0000000c },
    { "RS232 DUMP mode                           ", 0x00000030 },
    { "Physical RAM 1 size                       ", 0x00000040 },
    { "Physical RAM 2 size                       ", 0x00000044 },
    { "Global RAM Size(BAR0)                     ", 0x00000050 },
    { "LCA File Name Extension                   ", 0x00000070 },
    { "TCP/myMon File Name Ext                   ", 0x00000074 },
    { "ASP File Name Extension                   ", 0x00000078 },
    { "BIU 1 File Name Extension                 ", 0x00000080 },
    { "BIU 2 File Name Extension                 ", 0x00000084 },
    { "(XMC)IPMI-File Name Extension             ", 0x00000088 },
    { "IRIG                                      ", 0x000000F4 },
    { "Date of last NVR change                   ", 0x0000002C },
    { "EOL", SL_MAX_NVR_EOL }
};


static struct sl_nvr_entry sl_platform_ays[] ={

    { "Board Serial Number             ", 0x04 },
    { "Build DATE(xxCWYYYY)            ", 0x28 },
    { "NOVRAM Enable Code              ", 0x24 },
    { "Board Config                    ", 0x08 },
    { "Board Type                      ", 0x10 },
    { "Board Sub Type / Frontend Cap.  ", 0x14 },
    { "Board Application Type          ", 0x18 },
    { "Board Part Number               ", 0x1C },
    { "Board Part Number Extension     ", 0x20 },
    { "Hardware Variant Word Definition", 0x0C },
    { "RS232 DUMP mode                 ", 0x30 },
    { "SOC Version                     ", 0x34 },
    { "Physical RAM 1 size             ", 0x40 },
    { "Physical RAM 2 size             ", 0x44 },
    { "Global RAM Size(BAR0)           ", 0x50 },
    { "MLCA File Name Extension        ", 0x70 },
    { "TCP/myMon File Name Ext         ", 0x74 },
    { "ASP File Name Extension         ", 0x78 },
    { "BIU 1 File Name Extension       ", 0x80 },
    { "BIU 2 File Name Extension       ", 0x84 },
    { "(XMC)IPMI-File Name Extension   ", 0x88 },
    { "BIU 3 File Name Extension       ", 0x90 },
    { "BIU 4 File Name Extension       ", 0x94 },
    { "ZGI File Name Extension         ", 0xA0 },
    { "IRIG                            ", 0xF4 },
    { "Date of last NVR change         ", 0x2C },
    { "EOL", SL_MAX_NVR_EOL }
};




static struct sl_nvr_entry sl_platform_artix7[] ={

    { "Board Serial Number             ", 0x04 },
    { "Build DATE(xxCWYYYY)            ", 0x28 },
    { "NOVRAM Enable Code              ", 0x24 },
    { "Board Config                    ", 0x08 },
    { "Board Type                      ", 0x10 },
    { "Frontend Capabilities           ", 0x14 },
    { "Board Application Type          ", 0x18 },
    { "Board Part Number               ", 0x1C },
    { "Board Part Number Extension     ", 0x20 },
    { "Hardware Variant Word Definition", 0x0C },
    { "Physical RAM 1 size             ", 0x40 },
    { "Physical RAM 2 size             ", 0x44 },
    { "Global RAM Size(BAR0)           ", 0x50 },
    { "MLCA File Name Extension        ", 0x70 },
    { "BIU 1 File Name Extension       ", 0x80 },
    { "BIU 2 File Name Extension       ", 0x84 },
    { "IRIG                            ", 0xF4 },
    { "Date of last NVR change         ", 0x2C },
    { "EOL", SL_MAX_NVR_EOL }
};




static struct sl_nvr_entry sl_platform_ayx[] ={
    { "Board Serial Number               ", 0x0000000c },
    { "Build DATE(xxCWYYYY)              ", 0x0000003c },
    { "Licence Key                       ", 0x000000b0 },
    { "NOVRAM ENABLE                     ", 0x00000030 },
    { "Board Config                      ", 0x00000028 },
    { "Board Type                        ", 0x0000002c },
    { "Board Sub Type                    ", 0x000000c8 },
    { "Board Part Number                 ", 0x00000048 },
    { "PCI Header Word Definition        ", 0x00000098 },
    { "IRIG-Offset                       ", 0x000000c0 },
    { "IRIG-Offset Sign-Bit              ", 0x000000c4 },
    { "Hardware Variant Word Definition  ", 0x000000cc },
    { "Board System Clock[MHz]           ", 0x0000004c },
    { "Autostart1 Address                ", 0x00000020 },
    { "Autostart2 Address                ", 0x00000024 },
    { "LCA1/2 Boot                       ", 0x00000050 },
    { "LCA1/2 ChkSum                     ", 0x00000058 },
    { "LCA3/4 Boot                       ", 0x00000100 },
    { "LCA3/4 ChkSum                     ", 0x000000FC },
    { "LCA1 Reference Clock              ", 0x00000060 },
    { "LCA1 I/O Clock                    ", 0x00000068 },
    { "LCA2 Reference Clock              ", 0x00000064 },
    { "LCA2 I/O Clock                    ", 0x0000006C },
    { "LCA3 Reference Clock              ", 0x000000E8 },
    { "LCA3 I/O Clock                    ", 0x000000F0 },
    { "LCA4 Reference Clock              ", 0x000000EC },
    { "LCA4 I/O Clock                    ", 0x000000F4 },
    { "BIP1 Boot                         ", 0x00000080 },
    { "BIP1 FW Vers.                     ", 0x00000088 },
    { "BIP1 FW ChkSum                    ", 0x00000090 },
    { "BIP1 ClockFrequency               ", 0x00000104 },
    { "BIP2 Boot                         ", 0x00000084 },
    { "BIP2 FW Vers.                     ", 0x0000008C },
    { "BIP2 FW ChkSum                    ", 0x00000094 },
    { "BIP2 ClockFrequency               ", 0x00000108 },
    { "BIP3 Boot                         ", 0x000000D0 },
    { "BIP3 FW Vers.                     ", 0x000000D8 },
    { "BIP3 FW ChkSum                    ", 0x000000E0 },
    { "BIP3 ClockFrequency               ", 0x0000010C },
    { "BIP4 Boot                         ", 0x000000D4 },
    { "BIP4 FW Vers.                     ", 0x000000DC },
    { "BIP4 FW ChkSum                    ", 0x000000E4 },
    { "BIP4 ClockFrequency               ", 0x00000110 },
    { "ASP SW Vers.                      ", 0x000000A8 },
    { "ASP SW ChkSum                     ", 0x000000AC },
    { "ASP IO Channel                    ", 0x000000BC },
    { "ASP ClockFrequency                ", 0x00000114 },
    { "ASL Chksum                        ", 0x00000120 },
    { "MON SW Vers.                      ", 0x000000A0 },
    { "MON SW ChkSum                     ", 0x000000A4 },
    { "MON IO Channel                    ", 0x000000B8 },
    { "ASL-FPGA File Extension           ", 0x00000150 },
    { "LCA1 File Extension IO-FPGA1+2    ", 0x00000140 },
    { "LCA2 File Extension IO-FPGA3+4    ", 0x00000144 },
    { "LCA3 File Extension(not used)     ", 0x00000148 },
    { "LCA4 File Extension(not used)     ", 0x0000014C },
    { "BIP 1 File Extension(1553)        ", 0x00000130 },
    { "BIP 2 File Extension(LS 3838)     ", 0x00000134 },
    { "BIP 3 File Extension(HS 3910)     ", 0x00000138 },
    { "BIP 4 File Extension(HS EFEX)     ", 0x0000013C },
    { "ASP File Extension                ", 0x0000012C },
    { "TCP File Extension                ", 0x00000128 },
    { "Actual DATE(DDMMYYYY)             ", 0x00000040 },
    { "EOL", SL_MAX_NVR_EOL }
};





struct sl_nvr_entry * ls_get_nvr_lookup(AiUInt32 platform)
{
    struct sl_nvr_entry * lookup = sl_empty;

    switch(platform)
    {
    case AI_PLATFORM_PCIE_CARD:
    case AI_PLATFORM_XMC:
    case AI_PLATFORM_PMC_XMC_BASED:
    case AI_PLATFORM_PCIE:
    case AI_PLATFORM_CPCIE_3U:
    case AI_PLATFORM_CPCIX_3U_PCIE_BASED:
    case AI_PLATFORM_PCIX_PCIE_BASED:
            lookup = sl_platform_aye;
            break;
    case AI_PLATFORM_MINI_PCIE_CARD:
    case AI_PLATFORM_MINI_PCIE_CARD_AP:
    case AI_PLATFORM_ASC:
    case AI_PLATFORM_PCI_SHORT_ZYNQMP:
    case AI_PLATFORM_PCI_E_1L_ZYNQMP:
    case AI_PLATFORM_CPCIE_3U_ZYNQMP:
    case AI_PLATFORM_XMC_ZYNQMP:
    case AI_PLATFORM_USB3:
        lookup = sl_platform_ays;
            break;
    case AI_PLATFORM_MINI_PCIE_ARTIX7:
            lookup = sl_platform_artix7;
            break;
    case AI_PLATFORM_PCIX:
    case AI_PLATFORM_PCIE_PCIX_BASED:
    case AI_PLATFORM_CPCIX_3U:
    case AI_PLATFORM_CPCIX_6U:
            lookup = sl_platform_ayx;
            break;
    }

    return lookup;
}
