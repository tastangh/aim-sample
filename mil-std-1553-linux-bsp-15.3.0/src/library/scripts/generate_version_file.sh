#!/bin/sh
# SPDX-FileCopyrightText: 2001-2022 AIM GmbH <info@aim-online.com>
# SPDX-License-Identifier: MIT
#
# This script creates the generated/version.h version header file
#
#

# Get software version from MIL LIB Version file
cat src/AiMilVersionInternal.h | grep "#define API_MIL_MAJOR_VERSION" > generated/AiMilVersion.h
cat src/AiMilVersionInternal.h | grep "#define API_MIL_MINOR_VERSION" >> generated/AiMilVersion.h
cat src/AiMilVersionInternal.h | grep "#define API_MIL_PATCH_VERSION" >> generated/AiMilVersion.h
cat src/AiMilVersionInternal.h | grep "#define API_MIL_EXTRA_VERSION" >> generated/AiMilVersion.h
echo "#define API_MIL_VERSION_STRING_FULL \"`scripts/setlocalversion.sh .`\"" >> generated/AiMilVersion.h
echo "#define API_MIL_COPYRIGHT_STRING \"Copyright (c) 2009-`date +%Y` by AIM GmbH\"" >> generated/AiMilVersion.h
echo "#define API_MIL_BUILD_DATE \"`date -R`\"" >> generated/AiMilVersion.h
