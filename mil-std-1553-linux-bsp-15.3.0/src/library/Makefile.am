# SPDX-FileCopyrightText: 2001-2022 AIM GmbH <info@aim-online.com>
# SPDX-License-Identifier: MIT

ACLOCAL_AMFLAGS = -I m4


SUBDIRS = src sample

if PYTHON

SUBDIRS += add-ons/python

endif


.PHONY: git-version FORCE


all: git-version


git-version: generated/version.h


generated/version.h:
	@echo "Generating Full Version string from Git repository"
	${top_srcdir}/scripts/generate_version_file.sh


clean-local:
	rm -f generated/version.h
