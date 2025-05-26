# SPDX-FileCopyrightText: 2011-2022 AIM GmbH <info@aim-online.com>
# SPDX-License-Identifier: MIT

noinst_LIBRARIES = libsmart.a

libsmart_a_SOURCES = AiMyMon.c \
                     AiArtix7Mon.c \
                     S0Handling.c\
                     SmartAIM.c \
                     SmartAyE.c \
                     SmartBat.c \
                     SmartLib.c \
                     SmartLibAutoUpdateNone.c \
                     SmartUpd.c \
                     SmrtFile.c \
                     WrapList.c 

libsmart_a_CPPFLAGS = -D_AIM_LINUX -D_SMARTLIB
libsmart_a_CPPFLAGS += -I$(srcdir)/../include -I@AIM_BSP_COMMON@/include


if CIV

libsmart_a_SOURCES += civ/Civ_Wrap.c

libsmart_a_CPPFLAGS += -I@CIV_API@/include -I@CIV_TSW@/include

endif


if MIL

libsmart_a_SOURCES += mil/Mil_Wrap.c

libsmart_a_CPPFLAGS += -I@MIL_API@/include -I@MIL_TSW@/include
libsmart_a_CPPFLAGS += -D_API3910

endif

if RS485

libsmart_a_SOURCES += aim485/Aim485_Wrap.c

libsmart_a_CPPFLAGS += -I@AIM485@/Api485/include -I@AIM485@/shared/include

endif

if CMRB

libsmart_a_SOURCES += cmrb/AimCmrb_Wrap.c

libsmart_a_CPPFLAGS += -I@AIM485@/ApiCmrb/include -I@AIM485@/shared/include

endif

if RS485_AH

libsmart_a_SOURCES += aim485ah/Aim485Ah_Wrap.c

libsmart_a_CPPFLAGS += -I@AIM485_AH@/Api485-ah/include

endif
