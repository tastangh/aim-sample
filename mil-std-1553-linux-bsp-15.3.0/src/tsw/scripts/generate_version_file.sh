#!/bin/sh
# SPDX-FileCopyrightText: 2015-2022 AIM GmbH <info@aim-online.com>
# SPDX-License-Identifier: MIT OR GPL-2.0-or-later
#
# This script creates the generated/version.h version header file
#
#


# Get software version from MIL TSW Version file
cat include/mil_tsw_version_internal.h | grep "#define MIL_TSW_MAJOR_VERSION" > generated/mil_tsw_version.h
cat include/mil_tsw_version_internal.h | grep "#define MIL_TSW_MINOR_VERSION" >> generated/mil_tsw_version.h
cat include/mil_tsw_version_internal.h | grep "#define MIL_TSW_PATCH_VERSION" >> generated/mil_tsw_version.h
cat include/mil_tsw_version_internal.h | grep "#define MIL_TSW_EXTRA_VERSION" >> generated/mil_tsw_version.h
echo "#define MIL_TSW_VERSION_STRING_FULL \"`scripts/setlocalversion.sh .`\"" >> generated/mil_tsw_version.h
echo "#define MIL_TSW_BUILD_DATE \"`date -R`\"" >> generated/mil_tsw_version.h
