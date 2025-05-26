# SPDX-FileCopyrightText: 2011-2022 AIM GmbH <info@aim-online.com>
# SPDX-License-Identifier: MIT

#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_INIT([Smart], [0.0.0], [support@aim-online.com])
AM_INIT_AUTOMAKE([foreign])
AC_CONFIG_HEADERS([config.h])

# Checks for programs.
AC_PROG_CC
AC_PROG_RANLIB


AC_ARG_VAR(AIM_BSP_COMMON, [Path to AIM-BSP-Common Repository])

AC_ARG_VAR(CIV_API, [Path to AIM-Arinc 429 repository])
AC_ARG_VAR(CIV_TSW, [Path to AIM-Arinc 429 target software repository])

AC_ARG_VAR(MIL_API, [Path to AIM-MIL 1553 repository])
AC_ARG_VAR(MIL_TSW, [Path to AIM-MIL 1553 target software repository])

AC_ARG_VAR(AIM485, [Path to AIM-485/CMRB repository])
AC_ARG_VAR(AIM485_AH, [Path to AIM-485-AH repository])

AC_ARG_ENABLE(civ, AS_HELP_STRING(--enable-civ, [enables build for Arinc 429 boards]),,)
AM_CONDITIONAL(CIV, test "$enable_civ" = yes)

AC_ARG_ENABLE(mil, AS_HELP_STRING(--enable-mil, [enables build for MIL boards]),,)
AM_CONDITIONAL(MIL, test "$enable_mil" = yes)

AC_ARG_ENABLE(rs485, AS_HELP_STRING(--enable-485, [enables build for 485 boards]),,)
AM_CONDITIONAL(RS485, test "$enable_485" = yes)

AC_ARG_ENABLE(rs485ah, AS_HELP_STRING(--enable-485-ah, [enables build for 485-AH boards]),,)
AM_CONDITIONAL(RS485_AH, test "$enable_485_ah" = yes)

AC_ARG_ENABLE(cmrb, AS_HELP_STRING(--enable-cmrb, [enables build for CMRB boards]),,)
AM_CONDITIONAL(CMRB, test "$enable_cmrb" = yes)


AC_CONFIG_FILES([Makefile SmartLib/Makefile SmartLib/src/Makefile SmartCon/Makefile SmartCon/src/Makefile])


AC_OUTPUT
