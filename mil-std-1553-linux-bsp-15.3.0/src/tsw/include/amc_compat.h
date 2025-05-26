/* SPDX-FileCopyrightText: 2015-2022 AIM GmbH <info@aim-online.com> */
/* SPDX-License-Identifier: MIT OR GPL-2.0-or-later */

#ifndef _AMC_COMPAT_H
#define _AMC_COMPAT_H

#if defined(_WIN32) && !defined(_AIM_VMEWINDOWS) && !defined(_AIM_LVRT)
   #if _DEBUG
      #define PRINTF(STRING,VAR1,VAR2,VAR3) ApiPrint(DBG_TARGET_SW,FKT_TARGET_SW,(STRING,VAR1,VAR2,VAR3))
   #else
      #define PRINTF(STRING,VAR1,VAR2,VAR3) ApiPrint(0,0,(STRING,VAR1,VAR2,VAR3))
   #endif
#else
   #define PMC_IRG_EVENT_BITMASK        0x00000047
   #define PMC_IRG_EVENT_BIU1_BITMASK   0x00000001
   #define PMC_IRG_EVENT_BIU2_BITMASK   0x00000002
   #define PMC_IRG_EVENT_BIU3_BITMASK   0x01000000
   #define PMC_IRG_EVENT_BIU4_BITMASK   0x02000000
   #define PMC_IRG_EVENT_ENABLE_BITMASK 0x80000000
   #define PMC_IRG_EVENT_RT_BITMASK     0x00000004
   #define PMC_IRG_EVENT_ASP_BITMASK    0x00000040
   #define PMC_IRG_MASK_BITMASK         0x00000347
   #define PMC_IRG_MASK_BIU1_BITMASK    0x00000001
   #define PMC_IRG_MASK_BIU2_BITMASK    0x00000002
   #define PMC_IRG_MASK_RT_BITMASK      0x00000004
   #define PMC_IRG_MASK_ASP_BITMASK     0x00000040
#endif

#if !defined(_AIM_VMEWINDOWS) && !defined(_AIM_VME) && !defined(_AIM_LVRT)
  #define wbflush() ;
#endif
#endif
