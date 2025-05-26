#!/bin/sh

# SPDX-FileCopyrightText: 2011-2021 AIM GmbH <info@aim-online.com>
# SPDX-License-Identifier: MIT

#
# This script creates the generated/SmartConVersion.h version header file
#
#

usage() {
    echo "Usage: $0 [top-level repository]" >&2
    exit 1
}

if [ $# -ne 1 ]
then
    usage
fi

cd $1/SmartCon || usage;


# Get software version from SmartCon Version file
cat src/SmartConVersion.h | grep "#define SMART_MAJOR_VERSION" > generated/SmartConVersion.h
cat src/SmartConVersion.h | grep "#define SMART_MINOR_VERSION" >> generated/SmartConVersion.h
cat src/SmartConVersion.h | grep "#define SMART_PATCH_VERSION" >> generated/SmartConVersion.h
cat src/SmartConVersion.h | grep "#define SMART_EXTRA_VERSION" >> generated/SmartConVersion.h
echo "#define SMART_VERSION_STRING_FULL \"`$1/scripts/setlocalversion.sh $1`\"" >> generated/SmartConVersion.h
echo "#define SMART_BUILD_DATE \"`date -R`\"" >> generated/SmartConVersion.h


echo "#define AISMART_RC_COPYRIGHT_STRING \"Copyright (c) 1999-`date +%Y` by AIM GmbH\"" >> generated/SmartConVersion.h
