# SPDX-FileCopyrightText: 2013-2022 AIM GmbH <info@aim-online.com>
# SPDX-License-Identifier: MIT


	echo "Generating Full Version string from Git repository"
	cat src/ANS1553_Version.h | grep "#define ANS1553_MAJOR_VERSION" > generated/version.h
	cat src/ANS1553_Version.h | grep "#define ANS1553_MINOR_VERSION" >> generated/version.h
	cat src/ANS1553_Version.h | grep "#define ANS1553_PATCH_VERSION" >> generated/version.h
	echo "#define ANS1553_VERSION_STRING_FULL \"`scripts/setlocalversion .`\"" >> generated/version.h
	echo "#define BUILD_DATE \"`date -u`\"" >> generated/version.h
