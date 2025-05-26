/* SPDX-FileCopyrightText: 2016-2021 AIM GmbH <info@aim-online.com> */
/* SPDX-License-Identifier: MIT  */

/*! \file resource.h

This file contains additional defines for MS VS resources */


#ifndef __RESOURCE_H__
#define __RESOURCE_H__


#include "generated/SmartConVersion.h"



#define AISMART_RC_PRIVATE_BUILD_STRING  "\0"
#define AISMART_RC_SPECIAL_BUILD_STRING  "\0"
#ifndef _WIN64
  #define AISMART_RC_PRODUCT_DESCRIPTION_STRING "AIM system maintenance and recovery tool(32bit)\0"
#else
  #define AISMART_RC_PRODUCT_DESCRIPTION_STRING "AIM system maintenance and recovery tool(64bit)\0"
#endif

#define AISMART_RC_VERSION_NO   SMART_MAJOR_VERSION,SMART_MINOR_VERSION,SMART_PATCH_VERSION,0



#endif /* __RESOURCE_H__ */
