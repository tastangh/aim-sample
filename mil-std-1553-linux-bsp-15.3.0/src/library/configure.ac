# SPDX-FileCopyrightText: 2001-2024 AIM GmbH <info@aim-online.com>
# SPDX-License-Identifier: MIT
#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_INIT([AIM MIL Linux API], [24.22.0], [support@aim-online.com])
AM_INIT_AUTOMAKE([subdir-objects foreign])
AC_CONFIG_HEADERS([config.h])
AC_DISABLE_STATIC

# Checks for programs.
AC_PROG_CXX
AC_PROG_CC
AC_PROG_LIBTOOL
AC_PROG_LN_S

#Python Support
AM_PATH_PYTHON

# Checks for libraries.

# Checks for header files.
AC_CHECK_HEADERS([fcntl.h stdlib.h string.h sys/ioctl.h unistd.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_C_INLINE
AC_TYPE_PID_T
AC_TYPE_SIZE_T
AC_TYPE_SSIZE_T

# Checks for library functions.
AC_FUNC_FORK
AC_FUNC_MALLOC
AC_CHECK_FUNCS([gettimeofday memset select strcasecmp strerror])
AC_CHECK_LIB(pthread, pthread_create,,AC_MSG_ERROR([pthread library not found]))
AC_CHECK_LIB(rt, clock_gettime,,AC_MSG_ERROR([rt library not found]))


AC_CONFIG_MACRO_DIR([m4])
AC_CONFIG_FILES([Makefile src/Makefile add-ons/python/Makefile
		add-ons/python/aim_mil/Makefile])

AC_ARG_ENABLE([pnp], AS_HELP_STRING([--disable-pnp], [Disable USB device plug 'n' play detection ]))

AS_IF([test "x$enable_pnp" != "xno"], [
    AC_DEFINE(HAVE_PNP,1,[Set if USB device plug 'n' play detection is enabled])
])

AC_ARG_VAR(AIM_BSP_COMMON,      [Path to AIM BSP common repository])
AC_ARG_VAR(MIL_TSW,             [Path to AIM MIL TSW repository])
AC_ARG_VAR(ANS_COMMON,          [Path to AIM ANS common repository])
AC_ARG_VAR(LINUX_XLNX,          [Path to AIM XLNX repository (--enable-embedded-xlnx)])
AC_ARG_VAR(LINUX_STABLE_RT_AIM, [Path to AIM AT91 repository (--enable-embedded-at91)])
AC_ARG_VAR(PCI_DRIVER_LINUX,    [Path to AIM Linux headers])
AC_ARG_VAR(PCI_DRIVER_XENOMAI,  [Path to AIM Xenomai headers])
AC_ARG_VAR(ESMART,              [Path to AIM eSmart headers])
AC_ARG_VAR(XENOMAI_INCLUDES,    [Path to Xenomai header files (--enable-xenomai)])
AC_ARG_VAR(XENOMAI_LIBS,        [Path to Xenomai library files (--enable-xenomai)])

AC_SUBST(EMBEDDED_LINUX_ROOT_BUILDROOT, ${ANET_ROOTFS}/output/host/usr/arm-buildroot-linux-gnueabihf/sysroot)
AC_SUBST(EMBEDDED_LINUX_ROOT_UNKNOWN,   ${ANET_ROOTFS}/output/host/usr/arm-unknown-linux-gnueabi/sysroot)


AC_ARG_ENABLE(enable-hs, [ --enable-hs       enables build for HS sample],,)
AM_CONDITIONAL(HS, test "$enable_hs" = yes)

AC_ARG_ENABLE(embedded-at91, [  --enable-embedded-at91       enables build for AT91 embedded systems],,)
AM_CONDITIONAL(EMBEDDED_AT91, test "$enable_embedded_at91" = yes)

AC_ARG_ENABLE(embedded-xlnx, [  --enable-embedded-xlnx       enables build for Xilinx embedded systems],,)
AM_CONDITIONAL(EMBEDDED_XLNX, test "$enable_embedded_xlnx" = yes)

AC_ARG_ENABLE(xenomai, [  --enable-xenomai       enables build for Xenomai],,)
AM_CONDITIONAL(AIM_XENOMAI, test "$enable_xenomai" = yes)

AC_ARG_WITH(python, [AS_HELP_STRING([--with-python=VER], [Enables installation of the AIM MIL python packages])],,)
AM_CONDITIONAL(PYTHON, test "x$with_python" != "x" -a "$with_python" != "no")
AM_CONDITIONAL(PYTHON_CUSTOM_PATH, test "x$with_python" != "x" -a "$with_python" != "no" -a "$with_python" != yes)
AC_SUBST(WITH_PYTHON, $with_python)


if (test "$enable_embedded_at91" != "yes" -a "$enable_embedded_xlnx" != "yes" -a "$enable_xenomai" != "yes"); then
    
    PKG_CHECK_MODULES(LIBNL3_GENL, libnl-genl-3.0 >= 3.1, [have_libnl3=yes], [have_libnl3=no])
    
    if (test "${have_libnl3}" = "yes" ); then
        CPPFLAGS+=" $LIBNL3_GENL_CFLAGS"
        LIBS+=" $LIBNL3_GENL_LIBS"
        AC_DEFINE([HAVE_LIBNL3], 1,"Defined to 1 if libnl3 is available")
    fi

    if (test "${have_libnl3}" != "yes"); then
        AC_MSG_ERROR("libnl3 or libnl-genl development package missing", 1)
    fi
fi

AC_CONFIG_SUBDIRS([sample])

#Here the library versions have to be adjusted
library_major=24
library_minor=22
library_revision=0
library_major_so=`expr $library_major + $library_minor`
AC_SUBST(LIBRARY_MAJOR,$library_major)
AC_SUBST(LIBRARY_MINOR,$library_minor)
AC_SUBST(LIBRARY_REVISION,$library_revision)
AC_SUBST(LIBRARY_MAJOR_SO,$library_major_so) 

AC_OUTPUT
