# SPDX-FileCopyrightText: 2010-2022 AIM GmbH <info@aim-online.com>
# SPDX-License-Identifier: MIT OR GPL-2.0-or-later

.PHONY: all cleaner clean

KBUILD_MODPOST_WARN=1
ifneq (${KERNELRELEASE},)

    EXTRA_CFLAGS += -D_AIM_LINUX -D_AIM_SYSDRV -DAPI1553 -D_CMPL4API3910 \
                    -D_LINUX -D_AMC1553 \
                    -I$(M) -I$(M)/include -I$(M)/src/monitor/pmc -I$(M)/src \
                    -I$(AIM_BSP_COMMON)/include

                    
    ifdef DEBUG
        # driver debug
        EXTRA_CFLAGS += -DDEBUG  -O0 -g
    endif

    ifdef MDEBUG
        # monitor debug
        EXTRA_CFLAGS += -D_DEBUG
    endif
                
    
    libaim_mil_tsw-objs := src/api_bc.o src/api_bite.o src/api_bm.o src/api_buf.o \
                    src/api_cal.o src/api_dque.o src/api_glob.o \
                    src/api_ini.o src/api_io.o src/api_ir.o src/api_lic.o \
                    src/api_main.o src/api_opr.o src/api_rep.o src/api_rt.o \
                    src/api_sys.o src/api_track.o src/api_dts.o \
                    src/amc_hw.o src/AiMyMon.o \
                    src/mil_tasks_linux.o \
                    src/mil_scope.o \
                    src/api_utility.o src/api_exec_sys.o src/api_versions.o src/api_fifo.o src/mil_fw.o
                     
    ifdef CONFIG_AIM_PCI
        obj-m += libaim_mil_tsw.o
        libaim_mil_tsw-objs += src/ai_tsw_os_linux.o
        libaim_mil_tsw-objs += src/mil_linux_module.o
        libaim_mil_tsw-objs += src/mil_hw_pci_driver.o
        libaim_mil_tsw-objs += src/mil_hw_pci_driver_aye.o
        libaim_mil_tsw-objs += src/mil_hw_pci_driver_aye_pxi.o
        libaim_mil_tsw-objs += src/mil_hw_pci_driver_aye_milscope.o
        libaim_mil_tsw-objs += src/mil_hw_pci_driver_ays.o
        libaim_mil_tsw-objs += src/mil_hw_pci_driver_artix7.o
        libaim_mil_tsw-objs += src/mil_hw_pci_driver_artixus.o
        libaim_mil_tsw-objs += src/i_x_ini.o src/i_x_buf.o src/i_x_sys.o src/i39_glob.o src/i39_empty_stub.o
    endif 

    ifdef CONFIG_AIM_PCI_XENOMAI
        obj-m += libaim_mil_tsw.o
        libaim_mil_tsw-objs += src/ai_tsw_os_xenomai.o
        EXTRA_CFLAGS += -I./include/xenomai -I./include/xenomai/posix
        EXTRA_CFLAGS += -D_AIM_XENOMAI
        libaim_mil_tsw-objs += src/mil_linux_module.o
        libaim_mil_tsw-objs += src/mil_hw_pci_xenomai_driver.o
        libaim_mil_tsw-objs += src/i_x_ini.o src/i_x_buf.o src/i_x_sys.o src/i39_glob.o src/i39_empty_stub.o
    endif 

    ifdef CONFIG_AIM_USB
        obj-m += libaim_mil_tsw.o
        libaim_mil_tsw-objs += src/ai_tsw_os_linux.o
        libaim_mil_tsw-objs += src/mil_linux_module.o
        libaim_mil_tsw-objs += src/mil_hw_usb_driver.o
        libaim_mil_tsw-objs += src/i_x_ini.o src/i_x_buf.o src/i_x_sys.o src/i39_glob.o src/i39_empty_stub.o
        libaim_mil_tsw-objs += src/mil_usb_mem_mirror.o
        EXTRA_CFLAGS        += -D_AIM_1553_SYSDRV_USB
        EXTRA_CFLAGS        += -I$(USB_DRIVER_LINUX)/include/memorymirror  \
                               -I$(USB_DRIVER_LINUX)/include/hardware \
                               -I$(USB_DRIVER_LINUX)/include/core 
    endif
 
    ifdef CONFIG_AIM_XLNX
        ifdef CONFIG_MODULES
            # Kernel 5.10 requires modules support enabled to build out of tree components
            obj-m += libaim_mil_tsw.o
            libaim_mil_tsw-objs += src/mil_linux_module.o
        else
            obj-y += libaim_mil_tsw.o
        endif
        libaim_mil_tsw-objs += src/ai_tsw_os_linux.o
        libaim_mil_tsw-objs += src/mil_hw_ays_embedded.o
        EXTRA_CFLAGS        += -I$(srctree)/include/linux/ \
                               -I$(srctree)/include/linux/aim \
                               -I$(srctree)/include/uapi/linux/aim \
                               -DEMBEDDED

        ifdef CONFIG_AIM_XLNX_ENABLE_HS
            # Include HS files in driver (ANET3910 AYXX3910)
            EXTRA_CFLAGS        += -D_API3910
            libaim_mil_tsw-objs += src/i39_bc.o src/i39_bite.o src/i39_bm.o src/i39_buf.o \
                        src/i39_cal.o src/i39_gpio.o \
                        src/i39_ini.o src/i39_io.o \
                        src/i39_main.o src/i39_mem.o src/i39_rep.o src/i39_rt.o \
                        src/i39_sys.o src/i39_track.o src/i39_ir.o \
                        src/i_x_buf.o src/i_x_ini.o src/i_x_sys.o src/i39_glob.o\
                        src/ief_bm.o src/ief_io.o src/ief_bc.o
        else
            libaim_mil_tsw-objs += src/i_x_ini.o src/i_x_buf.o src/i_x_sys.o src/i39_glob.o src/i39_empty_stub.o
        endif

        ifdef CONFIG_AIM_XLNX_DISABLE_STORE_MERGING
            # The option store-merging was introduced with GCC7
            # It needs to be disabled when using GCC10 which is part of the Xilinx toolchain 2021.2. 
            # If not disabled the TSW will produce page faults when accessing the shared or global memory which are mapped via ioremap.
            EXTRA_CFLAGS        += -fno-store-merging
        endif
    endif

    ifdef CONFIG_AIM_AT91
        obj-y += libaim_mil_tsw.o
        libaim_mil_tsw-objs += src/ai_tsw_os_linux.o
        libaim_mil_tsw-objs += src/mil_hw_aye_embedded.o
        libaim_mil_tsw-objs += src/i_x_ini.o src/i_x_buf.o src/i_x_sys.o src/i39_glob.o src/i39_empty_stub.o
        EXTRA_CFLAGS      += -I$(srctree)/include/linux/aim -DEMBEDDED
    endif



else
PWD := $(shell pwd)

ifneq ($(CONFIG_AIM_PCI),)
KERNELDIR  ?=  /lib/modules/$(shell uname -r)/build
CONFIG     :=  CONFIG_AIM_PCI=y
INSTALLDIR :=  $(PCI_DRIVER_LINUX)/src
else ifneq ($(CONFIG_AIM_PCI_XENOMAI),)
KERNELDIR  ?=  /lib/modules/$(shell uname -r)/build
CONFIG     :=  CONFIG_AIM_PCI_XENOMAI=y
INSTALLDIR :=  $(PCI_DRIVER_XENOMAI)/src
else ifneq ($(CONFIG_AIM_USB),)
KERNELDIR  ?=  /lib/modules/$(shell uname -r)/build
CONFIG     :=  CONFIG_AIM_USB=y
INSTALLDIR :=  $(USB_DRIVER_LINUX)/src
else ifneq ($(CONFIG_AIM_XLNX),)
KERNELDIR  ?=  $(LINUX_XLNX)
CONFIG     :=  CONFIG_AIM_XLNX=y
INSTALLDIR :=  $(LINUX_XLNX)/drivers/aim/protocol/mil
else ifneq ($(CONFIG_AIM_XLNX_HS),)
KERNELDIR  ?=  $(LINUX_XLNX)
CONFIG     :=  CONFIG_AIM_XLNX=y CONFIG_AIM_XLNX_ENABLE_HS=y
INSTALLDIR :=  $(LINUX_XLNX)/drivers/aim/protocol/mil
else ifneq ($(CONFIG_AIM_XLNX_ZYNQMP),)
KERNELDIR  ?=  $(LINUX_XLNX)
CONFIG     :=  CONFIG_AIM_XLNX=y CONFIG_AIM_XLNX_DISABLE_STORE_MERGING=y
INSTALLDIR :=  $(LINUX_XLNX)/drivers/aim/protocol/mil
else ifneq ($(CONFIG_AIM_XLNX_ZYNQMP_HS),)
KERNELDIR  ?=  $(LINUX_XLNX)
CONFIG     :=  CONFIG_AIM_XLNX=y CONFIG_AIM_XLNX_ENABLE_HS=y CONFIG_AIM_XLNX_DISABLE_STORE_MERGING=y
INSTALLDIR :=  $(LINUX_XLNX)/drivers/aim/protocol/mil
else ifneq ($(CONFIG_AIM_AT91),)
KERNELDIR  ?=  $(LINUX_STABLE_RT_AIM)
CONFIG     :=  CONFIG_AIM_AT91=y
INSTALLDIR :=  $(LINUX_STABLE_RT_AIM)/drivers/aim/mil
else
$(error No Configuration specified)
endif

ifeq ($(KERNELDIR),)
$(error Missing kernel directory for this configuration)
endif


all: git-version
	$(MAKE) -C $(KERNELDIR) M=$(PWD) $(CONFIG)
	
clean:	
	$(MAKE) -C $(KERNELDIR) M=$(PWD) clean

install: all uninstall
	@touch $(INSTALLDIR)/.libaim_mil_tsw.o.cmd
	install -m 664 $(PWD)/libaim_mil_tsw.o $(INSTALLDIR)/libaim_mil_tsw.o_shipped;
	
uninstall:
	@if [ -f $(INSTALLDIR)/libaim_mil_tsw.o_shipped  ]; then \
		$(RM) $(INSTALLDIR)/libaim_mil_tsw.o_shipped; \
	fi

git-version:
	scripts/generate_version_file.sh


endif

