# SPDX-FileCopyrightText: 2001-2022 AIM GmbH <info@aim-online.com>
# SPDX-License-Identifier: MIT

dnl Process this file with autoconf to produce a configure script.

AC_PREREQ(2.59)
AC_INIT(MIL API Sample, 0.0.0,support@aim-online.com)


AC_CANONICAL_SYSTEM

AM_INIT_AUTOMAKE([foreign subdir-objects])
AC_CONFIG_HEADERS([config.h])

AC_PROG_CC
AM_PROG_CC_C_O

AC_ARG_ENABLE(hs, AS_HELP_STRING(--enable-hs, [Enables build of 3910/EFEx samples]),)
AM_CONDITIONAL([HS], [test "x$enable_hs" = xyes])

AC_ARG_ENABLE(xenomai, AS_HELP_STRING(--enable-xenomai, [Enables build for Xenomai samples]),)
AM_CONDITIONAL([AIM_XENOMAI], [test "x$enable_xenomai" = xyes])

AC_ARG_VAR(AIM_BSP_COMMON,      [Path to AIM BSP common repository])
AC_ARG_VAR(MIL_TSW,             [Path to AIM MIL TSW repository])
AC_ARG_VAR(XENOMAI_INCLUDES,    [Path to Xenomai header files (--enable-xenomai)])
AC_ARG_VAR(XENOMAI_LIBS,        [Path to Xenomai library files (--enable-xenomai)])

AC_CONFIG_FILES(Makefile src/Makefile)


AC_OUTPUT

