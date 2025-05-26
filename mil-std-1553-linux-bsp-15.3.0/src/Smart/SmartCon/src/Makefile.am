# SPDX-FileCopyrightText: 2009-2022 AIM GmbH <info@aim-online.com>
# SPDX-License-Identifier: MIT

bin_PROGRAMS = SmartCon

SmartCon_SOURCES = SmartCon.c

SmartCon_CPPFLAGS = -D_AIM_LINUX
SmartCon_CPPFLAGS += -I$(top_srcdir)/SmartCon -I$(top_srcdir)/SmartLib/include -I@AIM_BSP_COMMON@/include

SmartCon_LDADD = $(top_srcdir)/SmartLib/src/libsmart.a


if CIV

SmartCon_LDADD += -L@CIV_API@/src/.libs -laim_civ

endif

if MIL

SmartCon_LDADD += -L@MIL_API@/src/.libs -laim_mil

endif

if RS485

SmartCon_LDADD += -L@AIM485@/Api485/src/.libs -laim485

endif
if CMRB

SmartCon_LDADD += -L@AIM485@/ApiCmrb/src/.libs -laimcmrb

endif

if RS485_AH

SmartCon_LDADD += -L@AIM485_AH@/Api485-ah/src/.libs -laim485ah

endif
