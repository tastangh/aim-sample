# SPDX-FileCopyrightText: 2013-2022 AIM GmbH <info@aim-online.com>
# SPDX-License-Identifier: MIT


dnl Process this file with autoconf to produce a configure script.

AC_PREREQ(2.59)
AC_INIT(ANS1553, 0.9,support@aim-online.com)


AC_CANONICAL_SYSTEM
AM_INIT_AUTOMAKE([foreign])

AC_CONFIG_HEADERS([config.h])

AC_PROG_CC
AM_PROG_CC_C_O

AC_CHECK_HEADERS(pthread.h,,AC_MSG_ERROR([Not all necessary headers found]))

AC_CHECK_LIB(pthread, pthread_join,, AC_MSG_ERROR([pthread library not available]))

AC_ARG_VAR(AIM_BSP_COMMON, [Path to BSP Common Repository to build server with])
AC_ARG_VAR(ANS_COMMON, [Path to ANS-Common Repository to build server with])
AC_ARG_VAR(MIL_API, [Path to 1553 API Repository to build server against])
AC_ARG_VAR(MIL_TSW, [Path to 1553 Target Software Repository to build server against])
AC_ARG_VAR(XENOMAI_INCLUDES,    [Path to Xenomai header files (--enable-xenomai)])
AC_ARG_VAR(XENOMAI_LIBS,        [Path to Xenomai library files (--enable-xenomai)])
AC_ARG_VAR(CC_SYSROOT,        [Path to sysroot (--enable-xenomai)])

AC_ARG_ENABLE(webconf,AS_HELP_STRING(--enable-webconf,[Installs a configuration file that registers ANS with the web config tool]),,)
AC_ARG_ENABLE(sysvinit,AS_HELP_STRING(--enable-sysvinit,[Installs a SysVInit compliant init file for starting the service]),,)

AC_ARG_ENABLE(xenomai, [  --enable-xenomai       enables build for Xenomai],,)
AM_CONDITIONAL(AIM_XENOMAI, test "$enable_xenomai" = yes)


AM_CONDITIONAL(WEBCONFIG, [test "x$enable_webconf" == "xyes"])
AM_CONDITIONAL(SYSVINIT, [test "x$enable_sysvinit" == "xyes"])


AC_CONFIG_FILES(Makefile src/Makefile)


AC_OUTPUT
