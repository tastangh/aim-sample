/* SPDX-FileCopyrightText: 2016-2022 AIM GmbH <info@aim-online.com> */
/* SPDX-License-Identifier: MIT OR GPL-2.0-or-later */


#ifndef __MIL_MON_INCLUDES__
#define __MIL_MON_INCLUDES__


#if defined(_NUCLEUS)
    /* -- AYX --- */
    #include "ini_defs.h"
    #include "ini_vars.h"
    #include "ini_prot.h"

    typedef struct
    {
      UINT64 reg[ 96 ];
    } EXFRAME_TY;

    #undef __MON__
#endif


#if defined _AIM_SYSDRV && defined _AIM_WINDOWS
    /* -- TSW in Windows driver (APU,AYE,AME,AMEE) --- */
    #include "hw/AiMyMon.h"
#endif

#if defined _AIM_RTX64
    /* -- TSW in RTX64 rtdll (AYE) --- */
    /* No monitor required */
    typedef struct
    {
        AiUInt64 reg[96];
    } EXFRAME_TY;
#define wbflush()
#include "hw/AiMyMon.h"
#endif


#if defined _AIM_SYSDRV && !defined(_AIM_1553_SYSDRV_USB) && defined _AIM_LINUX && !defined(EMBEDDED) && !defined(_AIM_XENOMAI)
    /* -- TSW in Linux driver (AYE,AYS) --- */
    #include "hw/AiMyMon.h"
#endif



#if defined _AIM_1553_SYSDRV_USB && defined _AIM_LINUX && !defined(EMBEDDED) && !defined(_AIM_XENOMAI)
    /* -- TSW in Linux driver (USB) --- */
    /* No monitor required - USB driver has its own Monitor,
     * we do however need some types from the monitor.  */
    #include "hw/AiMyMon.h"
#endif



#if defined _AIM_SYSDRV && defined _AIM_LINUX && (defined(EMBEDDED) || defined(_AIM_XENOMAI))
    /* -- TSW in Embedded Linux driver (AYE,AYS) or Xenomai environment (AYE) --- */
    /* No monitor required */
    #include "hw/AiMyMon.h"
#endif



#ifdef __VXWORKS__
    #include "hw/AiMyMon.h"
#endif




#ifdef _AIM_VME
#endif




#ifdef _AIM_LVRT
    #include "hw/AiMyMon.h"
#endif

#endif /* __MIL_MON_INCLUDES__ */
