# SPDX-FileCopyrightText: 2009-2021 AIM GmbH <info@aim-online.com>
# SPDX-License-Identifier: MIT

SUBDIRS=. src


.PHONY: git-version


all-local: git-version


git-version:
	@echo "Generating Full Version string from Git repository"
	${top_srcdir}/scripts/generate_version_file.sh ${top_srcdir}


clean-local:
	rm -f generated/SmartConVersion.h