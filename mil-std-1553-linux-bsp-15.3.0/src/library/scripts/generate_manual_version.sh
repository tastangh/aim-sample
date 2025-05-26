#!/bin/bash
# SPDX-FileCopyrightText: 2001-2022 AIM GmbH <info@aim-online.com>
# SPDX-License-Identifier: MIT

# Generates the version file for the Latex manuals
# First argument has to be the path to the top-level of the repository


usage() {
    echo "Usage: $0 [repository directory]" >&2
    exit 1
}

if [ $# -lt 1 ];
then
  usage
fi

cd $1 || exit 1;


VERSION=`cat generated/AiMilVersion.h | grep  API_MIL_VERSION_STRING_FULL | cut -d' ' -f 3 | sed 's/"//g'`

echo "\\renewcommand{\\DocVersion}{${VERSION}}" > Doc/python_pg/doc_version_hs.tex
echo "\\renewcommand{\\DocAimNo}{60-13945-37-${VERSION}}" >> Doc/python_pg/doc_version_hs.tex

echo "\\renewcommand{\\DocVersion}{${VERSION}}" > Doc/python_pg/doc_version_ls.tex
echo "\\renewcommand{\\DocAimNo}{60-11945-37-${VERSION}}" >> Doc/python_pg/doc_version_ls.tex

echo "\\renewcommand{\\DocVersion}{${VERSION}}" > Doc/api_pg/programmers_guide_version.tex
echo "\\renewcommand{\\DocAimNo}{60-11900-37-${VERSION}}" >> Doc/api_pg/programmers_guide_version.tex

echo "\\renewcommand{\\DocVersion}{${VERSION}}" > Doc/api_pg/hs_programmers_guide_version.tex
echo "\\renewcommand{\\DocAimNo}{60-13900-37-${VERSION}}" >> Doc/api_pg/hs_programmers_guide_version.tex

echo "\\renewcommand{\\DocVersion}{${VERSION}}"                    > Doc/refman_1553/refman1553_version.tex
echo "\\renewcommand{\\DocAimNo}{60-11900-36-${VERSION}}" >> Doc/refman_1553/refman1553_version.tex

echo "\\renewcommand{\\DocVersion}{${VERSION}}"                    > Doc/refman_3910/refman3910_version.tex
echo "\\renewcommand{\\DocAimNo}{60-13900-36-${VERSION}}" >> Doc/refman_3910/refman3910_version.tex

