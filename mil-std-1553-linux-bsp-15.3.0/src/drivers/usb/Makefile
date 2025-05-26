# SPDX-FileCopyrightText: 2017-2021 AIM GmbH <info@aim-online.com>
# SPDX-License-Identifier: GPL-2.0-or-later

.PHONY: generated/version.h


########## This part is called from within the kernel build system ################################
ifneq (${KERNELRELEASE},)


# Include protocol specific Makefile
# which has to set AIM_MODULE to its module name,
# VERSION_FILE to the version file it uses and
# $(AIM_MODULE)-prot-objs to its protocol specific object files

ifneq ($(CONFIG_AIM_CIV),)
    -include $(M)/src/civ/Makefile.in
endif

ifneq ($(CONFIG_AIM_MIL),)
    -include $(M)/src/mil/Makefile.in
endif

ifneq ($(CONFIG_AIM_FDX),)
    -include $(M)/src/fdx/Makefile.in
endif

                    
# Now set actual module object file
    obj-m := $(AIM_MODULE).o
    
# Add the protocol independent source files
    $(AIM_MODULE)-y := src/aim_usb.o src/aim_usb_fs.o src/aim_usb_interface.o src/aim_usb_com_channel.o \
                       src/aim_usb_hw.o src/aim_usb_ays.o src/aim_usb_interrupt.o src/aim_usb_nl.o \
                       src/aim_usb_apu.o src/apu/aim_usb_nc.o
    
# Add the protocol specific source files
    $(AIM_MODULE)-y += $($(AIM_MODULE)-prot-objs)
    
# Compiler flags used by the generic driver part
    EXTRA_CFLAGS += -D_AIM_LINUX -D_AIM_SYSDRV -I$(M) -I$(M)/include -I$(M)/src \
                    -I$(AIM_BSP_COMMON)/include -I$(ESMART)/esmartd/include

else

########## This part is called directly when running make ################################

# Include protocol specific Makefile. Drop out with error if no valid one is specifified
 
ifneq ($(CONFIG_AIM_CIV),)
    -include src/civ/Makefile.in
    CONFIG := CONFIG_AIM_CIV=y
else ifneq ($(CONFIG_AIM_MIL),)
    -include src/mil/Makefile.in
    CONFIG := CONFIG_AIM_MIL=y
else ifneq ($(CONFIG_AIM_FDX),)
    -include src/fdx/Makefile.in
    CONFIG := CONFIG_AIM_FDX=y
else
    $(error No Configuration specified)
endif

    KERNELDIR ?=  /lib/modules/$(shell uname -r)/build
    PWD := $(shell pwd)

    all: generated/version.h
		$(MAKE) -C $(KERNELDIR) M=$(PWD) $(CONFIG) modules

    generated/version.h:
		@echo "Generating Full Version string from Git repository"
		@cat $(VERSION_FILE) | grep "#define MAJOR_VERSION" > $@
		@cat $(VERSION_FILE) | grep "#define MINOR_VERSION" >> $@
		@cat $(VERSION_FILE) | grep "#define PATCH_VERSION" >> $@
		@cat $(VERSION_FILE) | grep "#define EXTRA_VERSION" >> $@
		@echo "#define VERSION_STRING_FULL \"`scripts/setlocalversion .`\"" >> $@
		@echo "#define BUILD_DATE \"`date -R`\"" >> $@
		

    clean:
		$(MAKE) -C $(KERNELDIR) M=$(PWD) clean

    install: all
		@echo "Installing driver module $(AIM_MODULE).ko to /lib/modules/`uname -r`/extra/"
		$(MAKE) -C $(KERNELDIR) M=$(PWD) modules_install;
		
    uninstall:
		@echo "Uninstalling driver module $(AIM_MODULE).ko..."
		@if [ -f /lib/modules/`uname -r`/extra/$(AIM_MODULE).ko ]; \
		then \
			$(RM) /lib/modules/`uname -r`/extra/$(AIM_MODULE).ko && depmod; \
		fi
		
    distclean: clean
		
	
endif
