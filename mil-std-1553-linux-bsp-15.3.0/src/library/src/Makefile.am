# SPDX-FileCopyrightText: 2001-2024 AIM GmbH <info@aim-online.com>
# SPDX-License-Identifier: MIT

lib_LTLIBRARIES =
if AIM_XENOMAI
    lib_LTLIBRARIES += libaim_mil_xenomai.la
else
    lib_LTLIBRARIES += libaim_mil.la
endif

libaim_mil_la_SOURCES =  ApiLsBc.c ApiLsBm.c ApiGen.c\
                        ApiLsBuf.c ApiLsCal.c ApiLsFifo.c ApiLsQue.c \
                        ApiLsRep.c ApiLsRt.c ApiLsSys.c Dll_Fnc.c \
                        Dll_Ini.c Glob.c ApiLsDQue.c ApiLsScope.c ApiLsTrack.c \
                        AiGenIo.c  DeviceNotification.c ApiVersions.c\
                        Ai1553Cvi.c hw/ApiHw.c \
                        Ai1553i_def.h Aim1553_esmart_update.h ApiGen.h \
                        ApiLsCal.h ApiLsRt.h ApiVersions.h \
                        Ai1553i_fnc.h Api1553Cvi.h  ApiGenIo.h  ApiLsDQue.h \
                        ApiLsScope.h Api1553.h \
                        ApiLsBc.h ApiLsFifo.h ApiLsSys.h \
                        ApiLsBm.h ApiLsQue.h ApiLsTrack.h \
                        ApiLsBuf.h ApiLsReplay.h ApiNetwork.h
if AIM_XENOMAI
    aim_includedir = @includedir@/aim_mil_xenomai@LIBRARY_MAJOR@.@LIBRARY_MINOR@
else
    aim_includedir = @includedir@/aim_mil_@LIBRARY_MAJOR@.@LIBRARY_MINOR@
endif

aim_include_HEADERS = @AIM_BSP_COMMON@/include/Ai_cdef.h \
                      @AIM_BSP_COMMON@/include/Ai_def.h \
                      @AIM_BSP_COMMON@/include/Ai_gcc.h \
                      @AIM_BSP_COMMON@/include/Ai_types.h \
                      @AIM_BSP_COMMON@/include/Ai_container.h \
                      @AIM_BSP_COMMON@/include/Ai_exports.h \
                      @AIM_BSP_COMMON@/include/AiOs.h \
                      @AIM_BSP_COMMON@/include/AiOsLinux.h \
                      @AIM_BSP_COMMON@/include/Ai_compiler.h \
                      $(top_srcdir)/include/Api1553.h \
                      $(top_srcdir)/include/Ai1553i_def.h \
                      $(top_srcdir)/include/Ai1553i_fnc.h \
                      $(top_srcdir)/include/ApiLsDQue.h \
                      $(top_srcdir)/include/ApiLsBc.h \
                      $(top_srcdir)/include/ApiLsBm.h \
                      $(top_srcdir)/include/ApiLsRt.h \
                      $(top_srcdir)/include/ApiLsBuf.h \
                      $(top_srcdir)/include/ApiLsScope.h \
                      $(top_srcdir)/include/ApiLsTrack.h \
                      $(top_srcdir)/include/Aim1553_esmart_update.h \
                      $(top_srcdir)/include/ApiGenIo.h \
                      $(top_srcdir)/include/ApiLsQue.h \
                      $(top_srcdir)/include/ApiVersions.h \
                      $(top_srcdir)/include/ApiLsCal.h \
                      $(top_srcdir)/include/ApiLsReplay.h \
                      $(top_srcdir)/include/ApiGen.h \
                      $(top_srcdir)/include/ApiLsSys.h \
                      $(top_srcdir)/include/ApiLsFifo.h \
                      $(top_srcdir)/include/ApiNetwork.h \
                      @MIL_TSW@/include/Ai1553Com.h \
                      @MIL_TSW@/include/AiMilErrors.h 

                      
                      
# Compiling the API without this HS headers does not yet work
aim_include_HEADERS += $(top_srcdir)/include/Api3910.h \
                       $(top_srcdir)/include/Ai3910i_def.h \
                       $(top_srcdir)/include/Ai3910i_fnc.h

libaim_mil_la_CPPFLAGS = -I../include -I@AIM_BSP_COMMON@/include -I@MIL_TSW@/include
libaim_mil_la_CPPFLAGS += -D_AIM_LINUX -D_API1553 -DAIM_MIL
libaim_mil_la_LDFLAGS = @LIBS@



if HS
libaim_mil_la_SOURCES +=  hs/ApiEfBc.c hs/ApiEfBm.c hs/ApiEfRt.c \
                          hs/ApiEfSys.c hs/ApiHsBc.c hs/ApiHsBm.c \
                          hs/ApiHsBuf.c hs/ApiHsCal.c hs/ApiHsGpio.c hs/ApiHsRep.c \
                          hs/ApiHsRt.c hs/ApiHsSys.c hs/ApiHsTrack.c \
                          hs/Ai3910Cvi.c hs/AiEfCvi.c hs/ApiEfCal.c \
                          hs/Ai3910i_def.h hs/Ai3910i_fnc.h hs/Api3910Cvi.h \
                          hs/AiEfCvi.h hs/Api3910.h 
                      
libaim_mil_la_CPPFLAGS += -D_API3910
else
libaim_mil_la_SOURCES +=  ApiHsEmptyStub.c
endif                        




#Library version number
#first number is major number (is decremented by third number which is minor number e.g. 23:4:1 means major 22 minor 1)
#second number is revision number so 23:4:1 is 22.14 in AIM notation
libaim_mil_la_LDFLAGS += -version-info @LIBRARY_MAJOR_SO@:@LIBRARY_REVISION@:@LIBRARY_MINOR@



if EMBEDDED_XLNX
libaim_mil_la_SOURCES  += ./os/embedded/Aim1553_embedded_ays_io.c
libaim_mil_la_SOURCES  += Net_io_dummy.c 
libaim_mil_la_CPPFLAGS += -I./os/embedded/
libaim_mil_la_CPPFLAGS += -I@ANS_COMMON@/include
libaim_mil_la_CPPFLAGS += -I@LINUX_XLNX@/include/uapi/linux/aim/ \
                          -I@EMBEDDED_LINUX_ROOT_BUILDROOT@/usr/include/libnl3
libaim_mil_la_LDFLAGS += -lnl-genl-3
else
if EMBEDDED_AT91
libaim_mil_la_SOURCES  += ./os/embedded/Aim1553_embedded_io.c
libaim_mil_la_SOURCES  += Net_io_dummy.c 
libaim_mil_la_CPPFLAGS += -I./os/embedded/
libaim_mil_la_CPPFLAGS += -I@ANS_COMMON@/include
libaim_mil_la_CPPFLAGS += -I@LINUX_STABLE_RT_AIM@/include/linux/aim/ \
                          -I@EMBEDDED_LINUX_ROOT_UNKNOWN@/usr/include/libnl3
libaim_mil_la_LDFLAGS += -lnl-genl-3
else
if AIM_XENOMAI
libaim_mil_la_CPPFLAGS += -I=@XENOMAI_INCLUDES@ -D_GNU_SOURCE -D_REENTRANT -D__XENO__
libaim_mil_la_LDFLAGS  += -lnative -L@XENOMAI_LIBS@ -lxenomai -lpthread -lrt \
                          -lrtdm -L@XENOMAI_LIBS@ -lxenomai -lpthread -lrt
libaim_mil_la_SOURCES  += ./os/xenomai/Aim1553_Xenomai_io.c
libaim_mil_la_SOURCES  += Net_io_dummy.c 
libaim_mil_la_CPPFLAGS += -I./os/xenomai/
libaim_mil_la_CPPFLAGS += -I@PCI_DRIVER_XENOMAI@/include -I@PCI_DRIVER_LINUX@/include/mil
libaim_mil_la_CPPFLAGS += -D_AIM_XENOMAI
else
libaim_mil_la_SOURCES  += ./os/linux/Aim1553_io.c
libaim_mil_la_SOURCES  += Aim1553_esmart_update.c
libaim_mil_la_SOURCES  += Net_io.c 
libaim_mil_la_CPPFLAGS += -I./os/linux/
libaim_mil_la_CPPFLAGS += -I@PCI_DRIVER_LINUX@/include -I@PCI_DRIVER_LINUX@/include/mil
libaim_mil_la_CPPFLAGS += -I@ESMART@/esmartd/include
libaim_mil_la_CPPFLAGS += -I@ANS_COMMON@/include
libaim_mil_la_LIBADD    = @ANS_COMMON@/src/libans-common.a
endif
endif
endif

libaim_mil_xenomai_la_SOURCES = $(libaim_mil_la_SOURCES)
libaim_mil_xenomai_la_CPPFLAGS = $(libaim_mil_la_CPPFLAGS)
libaim_mil_xenomai_la_LDFLAGS = $(libaim_mil_la_LDFLAGS)


if AIM_XENOMAI
install-data-hook:
	$(RM) $(DESTDIR)$(includedir)/aim_mil_xenomai
	$(LN_S) aim_mil_xenomai@LIBRARY_MAJOR@.@LIBRARY_MINOR@ $(DESTDIR)$(includedir)/aim_mil_xenomai
else
install-data-hook:
	$(RM) $(DESTDIR)$(includedir)/aim_mil
	$(LN_S) aim_mil_@LIBRARY_MAJOR@.@LIBRARY_MINOR@ $(DESTDIR)$(includedir)/aim_mil
endif

uninstall-hook:
	$(RM) -rf $(DESTDIR)$(includedir)/aim_mil*
	$(RM) $(DESTDIR)$(libdir)/libaim_mil.*
