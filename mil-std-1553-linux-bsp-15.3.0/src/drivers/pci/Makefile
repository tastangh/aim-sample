# SPDX-FileCopyrightText: 2013-2024 AIM GmbH <info@aim-online.com>
# SPDX-License-Identifier: GPL-2.0-or-later

.PHONY: all cleaner clean git-version FORCE


########## This part is called from within the kernel build system ################################
ifneq (${KERNELRELEASE},)


# Include protocol specific Makefile
# which has to set AIM_MODULE to its module name,
# VERSION_FILE to the version file it uses and
# $(AIM_MODULE)-prot-objs to its protocol specific object files

ifneq ($(CONFIG_AIM_FC),)
      -include $(M)/src/fc/Makefile.in
else ifneq ($(CONFIG_AIM_FDX),)
      -include $(M)/src/fdx/Makefile.in
else ifneq ($(CONFIG_AIM_MIL),)
      -include $(M)/src/mil/Makefile.in
else ifneq ($(CONFIG_AIM_CIV),)
      -include $(M)/src/civ/Makefile.in
else ifneq ($(CONFIG_AIM_SERIAL),)
      -include $(M)/src/serial/Makefile.in
endif

# Compiler flags used by the generic driver part
    EXTRA_CFLAGS += -D_AIM_LINUX -D_AIM_SYSDRV \
                    -I$(M) -I$(M)/include -I$(M)/src \
                    -I$(AIM_BSP_COMMON)/include \
                    -I$(ESMART)/esmartd/include
                    
# Now set actual module object file
    obj-m := $(AIM_MODULE).o
    
# Add the protocol independent source files
    $(AIM_MODULE)-y := src/aim_pci_module.o src/aim_pci_device.o src/aim_apxe_device.o \
                          src/aim_ioctl.o src/aim_timer.o src/aim_dma.o \
                          src/aim_rw.o src/aim_pci_mem.o src/aim_ays_device.o src/aim_pci_com_channel.o \
                          src/aim_nl.o src/aim_aio.o
    
    
# Add the protocol specific source files
    $(AIM_MODULE)-y += $($(AIM_MODULE)-prot-objs)
    
# Add internal monitor files if no target software library with included monitor is available
    pmc-monitor-objs := src/pmc-monitor/prog_misc.o src/pmc-monitor/ini_stio.o \
                        src/pmc-monitor/mon_prog.o src/pmc-monitor/mon_wait.o
                        
    $(AIM_MODULE)-$(INTERNAL_MONITOR_FILES) += $(pmc-monitor-objs)
    ccflags-$(INTERNAL_MONITOR_FILES) += -I$(M)/src/pmc-monitor
    
# Add PMC support if necessary
    $(AIM_MODULE)-$(PMC_SUPPORT) += src/aim_amc_device.o
    ccflags-$(PMC_SUPPORT) += -DAIM_AMC
    
# Add AYE support if necessary
    $(AIM_MODULE)-$(AYE_SUPPORT) += src/aim_aye_device.o
    ccflags-$(AYE_SUPPORT) += -DAIM_AYE
    
# Add AYE Digital Scope (MIL Scope) support if necessary
    $(AIM_MODULE)-$(AYE_DS_SUPPORT) += src/aim_aye_ds_device.o
    ccflags-$(AYE_DS_SUPPORT) += -DAIM_AYE_DS
    
# Add AYX support if necessary
    $(AIM_MODULE)-$(AYX_SUPPORT) += src/aim_ayx_device.o
    ccflags-$(AYX_SUPPORT) += -DAIM_AYX
    
# Add ZynqMP support if necessary
    $(AIM_MODULE)-$(ZYNQMP_SUPPORT) += src/aim_zynqmp_device.o
    $(AIM_MODULE)-$(ZYNQMP_SUPPORT) += src/aim_zynqmp_dma.o
    ccflags-$(ZYNQMP_SUPPORT) += -DAIM_ZYNQMP

# Add Artix7 support if necessary
    $(AIM_MODULE)-$(ARTIX7_SUPPORT) += src/aim_artix7_device.o
    ccflags-$(ARTIX7_SUPPORT) += -DAIM_ARTIX7

# Add ArtixUS support if necessary
    $(AIM_MODULE)-$(ARTIXUS_SUPPORT) += src/aim_artixus_device.o
    $(AIM_MODULE)-$(ARTIXUS_SUPPORT) += src/aim_artixus_dma.o
    ccflags-$(ARTIXUS_SUPPORT) += -DAIM_ARTIXUS

# Add driver files that are needed when platforms with monitor support are required to build
    ifneq (,$(filter y,$(PMC_SUPPORT)))
    NEED_MONITOR := y
    endif
    
    $(AIM_MODULE)-$(NEED_MONITOR) += src/aim_monitor.o
    ccflags-$(NEED_MONITOR) += -DNEED_MONITOR
    
# Add driver files that are needed when platforms with MyMon support are required to build
    ifneq (,$(filter y,$(AYE_SUPPORT)))
    NEED_MYMON := y
    endif

     $(AIM_MODULE)-$(NEED_MYMON) += src/aim_pci_mymon.o
    ccflags-$(NEED_MYMON) += -DNEED_MYMON


else

########## This part is called directly when running make ################################

# Include protocol specific Makefile. Drop out with error if no valid one is specifified
 
ifneq ($(CONFIG_AIM_FC),)
-include src/fc/Makefile.in
CONFIG := CONFIG_AIM_FC=y
else ifneq ($(CONFIG_AIM_FDX),)
-include src/fdx/Makefile.in
CONFIG := CONFIG_AIM_FDX=y
else ifneq ($(CONFIG_AIM_MIL),)
-include src/mil/Makefile.in
CONFIG := CONFIG_AIM_MIL=y
else ifneq ($(CONFIG_AIM_CIV),)
-include src/civ/Makefile.in
CONFIG := CONFIG_AIM_CIV=y
else ifneq ($(CONFIG_AIM_SERIAL),)
-include src/serial/Makefile.in
CONFIG := CONFIG_AIM_SERIAL=y
else
$(error No Configuration specified)
endif

    KERNELDIR ?=  /lib/modules/$(shell uname -r)/build
    PWD := $(shell pwd)

    all: git-version
		$(MAKE) -C $(KERNELDIR) M=$(PWD) $(CONFIG) modules

    git-version: generated/version.h
   
   # Create the version header file
    generated/version.h: FORCE
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
