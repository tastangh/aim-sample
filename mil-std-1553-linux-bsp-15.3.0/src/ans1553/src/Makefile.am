# SPDX-FileCopyrightText: 2013-2022 AIM GmbH <info@aim-online.com>
# SPDX-License-Identifier: MIT


bin_PROGRAMS=ANS1553
ANS1553_SOURCES=ANS1553.c BoardCommandHandlers.c BoardProtocolHandlers.c \
               ANS1553_Version.h


ANS1553_CPPFLAGS = -D_AIM_LINUX -I@AIM_BSP_COMMON@/include -I@ANS_COMMON@/include -I@MIL_API@/include \
                  -I@MIL_TSW@/include


ANS1553_LDADD = @ANS_COMMON@/src/libans-common.a
ANS1553_LDFLAGS = "-Wl,--no-as-needed" -L@MIL_API@/src/.libs

if AIM_XENOMAI
    ANS1553_LDFLAGS += -laim_mil_xenomai
    ANS1553_LDFLAGS += -Wl,@@CC_SYSROOT@@XENOMAI_LIBS@/posix.wrappers -L@XENOMAI_LIBS@ -lpthread_rt -lxenomai -lpthread -lrt
    ANS1553_CFLAGS   = -D_AIM_XENOMAI
    ANS1553_CFLAGS  += -I=@XENOMAI_INCLUDES@/posix -I=@XENOMAI_INCLUDES@ -D_GNU_SOURCE -D_REENTRANT -D__XENO__
else
    ANS1553_LDFLAGS += -laim_mil
endif



if SYSVINIT

# sysconfdir usually points to $(prefix)/etc. Use --sysconfdir= in ./configure
# to override this behaviour
initdir = $(sysconfdir)/init.d

dist_init_SCRIPTS = S98ANS1553

endif


if WEBCONFIG

# This file is used for registering the ANS with
# the web config tool
webconfdir = $(sysconfdir)/services.d
dist_webconf_SCRIPTS = ans1553-web.conf

endif
