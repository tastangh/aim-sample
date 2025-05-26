/* SPDX-FileCopyrightText: 2016-2022 AIM GmbH <info@aim-online.com> */
/* SPDX-License-Identifier: MIT OR GPL-2.0-or-later */


#ifndef __MIL_TSW_INCLUDES__
#define __MIL_TSW_INCLUDES__

#include "Ai_cdef.h"
#include "Ai_def.h"
#include "ai_tsw_os.h"

#include "mil_mon_includes.h"

#include "api_defs.h"
#include "api_defv.h"
#include "api_prot.h"

#include "mil_hw.h"
#include "mil_fw.h"
#include "mil_utils.h"
#include "mil_tasks.h"

#include "AiMilErrors.h"
#include "Ai1553Com.h"
#include "AiMilCom.h"
#include "generated/mil_tsw_version.h"


#include "i39_defs.h"
#include "i39_defv.h"
#include "i39_prot.h"


#ifdef _AIM_1553_SYSDRV_USB
  #include "mil_usb_mem_mirror.h"
  #include "mil_usb_io.h"
#else
  #include "no_usb_driver.h"
#endif

#ifndef _AMC1553
  #ifdef _NUCLEUS
    #include "MilScope_prot.h"
    #include "MilScope_var.h"
    #include "MilScope_def.h"
  #endif
#endif 


#ifdef _CMPL4S1I1553
  #include "s1i_defs.h"
  #include "s1i_defv.h"
  #include "s1i_prot.h"
#endif 


#endif /* __MIL_TSW_INCLUDES__ */
