# SPDX-FileCopyrightText: 2013-2022 AIM GmbH <info@aim-online.com>
# SPDX-License-Identifier: MIT

SUBDIRS=src

.PHONY: git-version FORCE

all: git-version

git-version: generated/version.h

generated/version.h: FORCE
	@echo "Generating Full Version string from Git repository"
	@cat $(top_srcdir)/src/ANS1553_Version.h | grep "#define ANS1553_MAJOR_VERSION" > $@
	@cat $(top_srcdir)/src/ANS1553_Version.h | grep "#define ANS1553_MINOR_VERSION" >> $@
	@cat $(top_srcdir)/src/ANS1553_Version.h | grep "#define ANS1553_PATCH_VERSION" >> $@
	@echo "#define ANS1553_VERSION_STRING_FULL \"`$(top_srcdir)/scripts/setlocalversion $(top_srcdir)`\"" >> $@
	@echo "#define BUILD_DATE \"`date -u`\"" >> $@ 


clean-local:
	rm -f generated/version.h

