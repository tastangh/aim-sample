#!/bin/sh

BSP_DIR=`pwd`
BSP_SRC_DIR=${BSP_DIR}/src
PREFIX=/usr/local
COMMAND="install"
VERBOSE="no"
ENABLE_SIGNING="no"
BUILDLOG=${BSP_DIR}/build.log
FORCE="no"
AIMLIBDIR=${PREFIX}/lib
KERNEL_INC=""

export AIM_BSP_COMMON=${BSP_SRC_DIR}/AIM-BSP-Common
export MIL_TSW=${BSP_SRC_DIR}/tsw
export MIL_API=${BSP_SRC_DIR}/library
export ANS_COMMON=${BSP_SRC_DIR}/ANS-Common
export ESMART=${BSP_SRC_DIR}/eSmart
export PCI_DRIVER_LINUX=${BSP_SRC_DIR}/drivers/pci
export USB_DRIVER_LINUX=${BSP_SRC_DIR}/drivers/usb


# Prints argument $1 to stdout and exits with error code 1
failure() {

	echo
	echo
	echo "**** $1 ****"
	echo
	echo
	exit 1
}


# Prints dependency $1 and available version $2
# to stdout and exits with error code 1
dep_failure() {
    echo
    echo "**** \"$1\" required, you have \"$2\", exiting"
    echo
    exit 1
}


# Tests some install dependencies like kernel headers
do_test_external_dep() {
    if [ "x${FORCE}" = "xyes" ]; then
        return
    fi

    echo Using kernel headers $KERNEL_INC 

    # check linux version
    LINUX_VERSION=`uname -r`
    echo ${LINUX_VERSION} | awk -F. '{if ($1 == 2 && $2 < 6) exit 1 }' || dep_failure "Linux 2.6.x" ${LINUX_VERSION}

    if [ ! -e ${KERNEL_INC} ]; then
        dep_failure "Kernel include files ${KERNEL_INC}" \
            "`ls -d /lib/modules/*`"
    fi

    if [ -z `which make` ]; then
        failure "Error: GNU make not installed."
    fi

    if [ -z `which gcc` ] || [ -z `which g++` ]; then
        failure "Error: GNU C/C++ compiler not installed."
    fi

    if [ -z `which pkg-config` ]; then
        failure "Error: Package 'pkg-config' missing. Please install it"
    fi

    pkg-config --atleast-version=3.1 libnl-genl-3.0
    if [ $? -ne 0 ]; then
        failure "Error: Package 'libnl-genl-3.0-dev' missing. Please install it"
    fi

	if [ ! -d sample ]; then
        echo "Creating sample link"
        ln -s src/library/sample sample
    fi
	# TODO check automake
}


# Configures autoconf based BSP components
configure_bsp() {
	echo "Checking dependencies"
	do_test_external_dep
	echo "Configuring BSP"

	# Configure ANS-Common library
	if [ ! -f ${BSP_SRC_DIR}/ANS-Common/Makefile ]
	then
		cd ${BSP_SRC_DIR}/ANS-Common
		./configure >> ${BUILDLOG} 2>&1 || failure "Configuring of ANS Common library Failed"
	fi


	# Configure API library
	if [ ! -f ${BSP_SRC_DIR}/library/Makefile ]
	then
		cd ${BSP_SRC_DIR}/library
		./configure ${PNP_OPTION} --prefix=${PREFIX} >> ${BUILDLOG} 2>&1 || failure "Configuring of API library Failed"
	fi

	# Configure ANS1553 server with special ${PREFIX} prefix
	if [ ! -f ${BSP_SRC_DIR}/ans1553/Makefile ]
	then
		cd ${BSP_SRC_DIR}/ans1553 || failure "ANS1553 directory missing"
		./configure --prefix=${PREFIX} CPPFLAGS="-I${BSP_SRC_DIR}/library/include" LDFLAGS="-L${BSP_SRC_DIR}/library/src/.libs" >> ${BUILDLOG} 2>&1 || failure "Configuring of ANS1553 server Failed"
	fi

    # Configure Smart
	if [ ! -f ${BSP_SRC_DIR}/Smart/Makefile ]
	then
		cd ${BSP_SRC_DIR}/Smart
		./configure --enable-mil --prefix=/ >> ${BUILDLOG} 2>&1 || failure "Configuring of Smart failed"
	fi

	cd ${BSP_DIR}

}


# Just builds all of the BSP components without installing them
build_bsp() {

    echo "Building PCI tsw module"
    make -C ${BSP_SRC_DIR}/tsw CONFIG_AIM_PCI=y install >> ${BUILDLOG} 2>&1 || failure "Failed to build PCI tsw module"

    echo "Building AIM PCI module"
    make -C ${BSP_SRC_DIR}/drivers/pci CONFIG_AIM_MIL=y >> ${BUILDLOG} 2>&1 || failure "Failed to build PCI driver module"

    echo "Building USB tsw module"
    make -C ${BSP_SRC_DIR}/tsw CONFIG_AIM_USB=y install >> ${BUILDLOG} 2>&1 || failure "Failed to build USB tsw module"

    echo "Building AIM USB module"
    make -C ${BSP_SRC_DIR}/drivers/usb CONFIG_AIM_MIL=y >> ${BUILDLOG} 2>&1 || failure "Failed to build USB driver module"

    echo "Building library and samples"
    make -C ${BSP_SRC_DIR}/ANS-Common >> ${BUILDLOG} 2>&1 || failure "Failed to build ANS-Common library"
    make -C ${BSP_SRC_DIR}/library >> ${BUILDLOG} 2>&1 || failure "Failed to build library"
    chmod 777 ${BSP_SRC_DIR}/library/sample/src/.deps
    chmod 777 ${BSP_SRC_DIR}/library/sample/src/hs/.deps

    echo "Building AIM Network Server"
    make -C ${BSP_SRC_DIR}/ans1553 >> ${BUILDLOG} 2>&1 || failure "Failed to build ANS1553"
        
    echo "Building Smart"
    make -C ${BSP_SRC_DIR}/Smart >> ${BUILDLOG} 2>&1 || failure "Failed to build Smart"
}


# Installs all of the BSP components. They must have been build before
install_bsp() {
    # Install PCI driver
    echo "Installing AIM PCI module"
    make -C ${BSP_SRC_DIR}/drivers/pci CONFIG_AIM_MIL=y install >> ${BUILDLOG} 2>&1 || failure "Failed to install MIL PCI driver module"

    # Install USB driver
    echo "Installing AIM USB module"
    make -C ${BSP_SRC_DIR}/drivers/usb CONFIG_AIM_MIL=y install >> ${BUILDLOG} 2>&1 || failure "Failed to install MIL USB driver module"

    # Try to locate the kernel modules directory
    if [ -f "/lib/modules/`uname -r`/updates/aim_mil.ko" ]
    then
        MODULES_DIR="/lib/modules/`uname -r`/updates/"
    else
        MODULES_DIR="/lib/modules/`uname -r`/extra/"
    fi

    echo "Modules have been installed in $MODULES_DIR"

    # Now load PCI driver module
    depmod || failure "Building of module dependencies failed"
    cat /proc/modules | grep aim_mil > /dev/null
    if [ $? -eq 0 ]; then
        rmmod aim_mil || failure "Removal of aim_mil module failed"
    fi
    if [ ${ENABLE_SIGNING} = "yes" ]
    then
        echo "Signing aim_mil.ko"
        kmodsign sha512 MOK.priv MOK.der $MODULES_DIR/aim_mil.ko
    fi
    modprobe aim_mil || failure "Loading of aim_mil driver module failed"

    # Now load USB driver module
    cat /proc/modules | grep aim1553usb > /dev/null
    if [ $? -eq 0 ]; then
        rmmod aim1553usb || failure "Removal of aim1553usb module failed"
    fi
    if [ ${ENABLE_SIGNING} = "yes" ]
    then
        echo "Signing aim1553usb.ko"
        kmodsign sha512 MOK.priv MOK.der $MODULES_DIR/aim1553usb.ko
    fi
    modprobe aim1553usb || failure "Loading of aim1553usb driver module failed"

    echo "Installing library to ${PREFIX}/lib and header files to ${PREFIX}/include/aim_mil"
    make -C ${BSP_SRC_DIR}/library uninstall >> ${BUILDLOG} 2>&1 || failure "Failed to clean up old libraries and headers"
    make -C ${BSP_SRC_DIR}/library install >> ${BUILDLOG} 2>&1 || failure "Failed to install library"

    echo "Installing ANS1553 to ${PREFIX}/bin"
    make -C ${BSP_SRC_DIR}/ans1553 install >> ${BUILDLOG} 2>&1 || failure "Failed to install ANS1553"

    echo "Installing Smart to ${BSP_DIR}/Smart/bin"
    make -C ${BSP_SRC_DIR}/Smart DESTDIR=${BSP_SRC_DIR}/Smart install >> ${BUILDLOG} 2>&1 || failure "Failed to install library"

    echo
    echo "  **** Please note: \"${AIMLIBDIR}\" needs to be in your library search path:"
    echo "  If your aplication cannot find libaim_mil.so, either add ${AIMLIBDIR} to /etc/ld.so.conf"
    echo "  and run ldconfig or add ${AIMLIBDIR} to your LD_LIBRARY_PATH ****"
    echo
}

uninstall_bsp() {

	# Unload PCI and USB driver module
	cat /proc/modules | grep aim_mil > /dev/null
	if [ $? -eq 0 ]; then
		rmmod aim_mil || failure "Removal of aim_mil module failed"
	fi

	cat /proc/modules | grep aim1553usb > /dev/null
	if [ $? -eq 0 ]; then
		rmmod aim1553usb || failure "Removal of AIM USB driver module failed"
	fi

    # uninstall PCI driver
    make -C ${BSP_SRC_DIR}/drivers/pci CONFIG_AIM_MIL=y uninstall >> ${BUILDLOG} 2>&1 || failure "Uninstallation of PCI driver failed"

    # uninstall USB driver
    make -C ${BSP_SRC_DIR}/drivers/usb CONFIG_AIM_MIL=y uninstall >> ${BUILDLOG} 2>&1 || failure "Uninstallation of USB driver failed"

	# uninstall ANS1553
	make -C ${BSP_SRC_DIR}/ans1553 uninstall >> ${BUILDLOG} 2>&1 || failure "Uninstallation of ANS1553 failed"

	# uninstall library and sample
	make -C ${BSP_SRC_DIR}/library uninstall >> ${BUILDLOG} 2>&1 || failure "Uninstallation of library failed"

}


clean_bsp() {

    # Clean tsw
	make -C ${BSP_SRC_DIR}/tsw CONFIG_AIM_PCI=y clean >> ${BUILDLOG} 2>&1 || failure "Failed to clean tsw driver module"

    # Clean PCI driver
    make -C ${BSP_SRC_DIR}/drivers/pci CONFIG_AIM_MIL=y clean >> ${BUILDLOG} 2>&1 || failure "Failed to clean PCI driver module"

    # Clean USB driver
    make -C ${BSP_DIR}/src/drivers/usb CONFIG_AIM_MIL=y clean >> ${BUILDLOG} 2>&1 || failure "Failed to clean USB driver module"

    # Clean ANS-Common library
	make -C ${BSP_SRC_DIR}/ANS-Common clean >> ${BUILDLOG} 2>&1 || failure "Failed to clean ANS-Common library"

	# Clean library
	make -C ${BSP_SRC_DIR}/library clean >> ${BUILDLOG} 2>&1 || failure "Failed to clean library"
}


distclean_bsp() {

    # Clean tsw
	make -C ${BSP_SRC_DIR}/tsw CONFIG_AIM_PCI=y clean >> ${BUILDLOG} 2>&1 || failure "Failed to distclean tsw driver module"

    # Clean PCI driver
    make -C ${BSP_SRC_DIR}/drivers/pci CONFIG_AIM_MIL=y distclean >> ${BUILDLOG} 2>&1 || failure "Failed to distclean PCI driver module"

    # Clean USB driver
    make -C ${BSP_SRC_DIR}/drivers/usb CONFIG_AIM_MIL=y distclean >> ${BUILDLOG} 2>&1 || failure "Failed to distclean USB driver module"

    # Clean ANS-Common library
	make -C ${BSP_SRC_DIR}/ANS-Common distclean >> ${BUILDLOG} 2>&1 || failure "Failed to distclean ANS-Common library"

    # Clean Smart
	make -C ${BSP_SRC_DIR}/Smart distclean >> ${BUILDLOG} 2>&1 || failure "Failed to distclean Smart"

	# Clean library
	make -C ${BSP_SRC_DIR}/library distclean >> ${BUILDLOG} 2>&1 || failure "Failed to distclean library"

}


update_devices() {

    echo "Updating firmware of all installed MIL devices."
    echo "This process may take a lot of time"
    echo "Do not abort this procedure as it may render your devices useless!!!"
    cd ${BSP_DIR}/Onboard-SW
    ./update.sh
    cat smart.log >> ${BUILDLOG}
    rm smart.log
    echo "Device update done"
}


patch_bsp() {
    #################################################################################
    # Note: This patches are only for Red Hat 9.x and Rocky 9.x Kernels.            #
    #       If you use a vanilla Kernel from kernel.org or RedHawk Kernel           #
    #       remove this patch call from the installer.                              #
    #################################################################################
    if [ -f "/etc/redhat-release" ]; then
        echo  "Detected RHEL based OS, checking for patches ..."; 

        echo $KERNEL_INC/include/generated/uapi/linux/version.h  >> ${BUILDLOG} 2>&1
        cat $KERNEL_INC/include/generated/uapi/linux/version.h   >> ${BUILDLOG} 2>&1

        grep RHEL_MAJOR $KERNEL_INC/include/generated/uapi/linux/version.h  >> ${BUILDLOG} 2>&1
        if [ $? -ne 0 ]; then
            echo  "Not a Red Hat kernel, skip the patch"; 
            return
        fi

        major=`awk -F' ' '/RHEL_MAJOR/{print $3}' $KERNEL_INC/include/generated/uapi/linux/version.h`
        minor=`awk -F' ' '/RHEL_MINOR/{print $3}' $KERNEL_INC/include/generated/uapi/linux/version.h`

        # The RHEL 7.3 patch should be applied since RHEL 7.1
        if [ $major -eq 7 -a $minor -ge 1 ]; then
            echo "Applying BSP patches for Red Hat $major.$minor"
            patch -d ${BSP_SRC_DIR}/drivers/pci -N -p1 < ${BSP_SRC_DIR}/drivers/pci/patches/rhel7.3.patch >> ${BUILDLOG} 2>&1
            if [ $? -gt 1 ]; then
                failure "Failed to patch PCI driver"
            fi
            patch -d ${BSP_SRC_DIR}/drivers/usb -N -p1  < ${BSP_SRC_DIR}/drivers/usb/patches/rhel7.3.patch >> ${BUILDLOG} 2>&1
            if [ $? -gt 1 ]; then
                failure "Failed to patch USB driver"
            fi
        fi

        # Patch the system if RHEL version is 9.4 or newer
        if [ $major -eq 9 -a $minor -ge 4 ]; then
            echo "Applying patches for Red Hat $major.$minor"
            patch -d ${BSP_SRC_DIR}/drivers/pci -N -p1 < ${BSP_SRC_DIR}/drivers/pci/patches/rhel9.4.patch >> ${BUILDLOG} 2>&1
            if [ $? -gt 1 ]; then
                failure "Failed to patch PCI driver"
            fi
        fi
    fi
}


############### Script execution starts here #####################################################


# Parse command line arguments
while [ "$1" ]
do
	case $1 in
		"install")
			COMMAND="install"
			;;
		"uninstall")
			COMMAND="uninstall"
			;;
		"clean")
			COMMAND="clean"
			;;
		"distclean")
			COMMAND="distclean"
			;;
		"build")
			COMMAND="build"
			;;
		"--verbose")
			VERBOSE="yes"
			;;
		"--disable-pnp")
			PNP_OPTION="--disable-pnp"
			;;
		"--no-dependency-check")
		        FORCE="yes"
		        ;;
                "--enable-signing")
                       ENABLE_SIGNING="yes"
		        ;;
                      
		*)
	esac

	shift
done


# In verbose mode, redirect output and errors to stdout,
# otherwise to build log file
if [ ${VERBOSE} = "yes" ]
then
	BUILDLOG=/dev/stdout
else
	echo "**** Performing $COMMAND of MIL Linux BSP at `date` ****" > ${BUILDLOG}
	echo >> ${BUILDLOG}
fi


# Allow to overwrite kernel headers directory for compile tests
#E.g. KERNELDIR=/usr/src/linux-headers-x.y.z
if [ -z ${KERNELDIR+x} ]; then
    KERNEL_INC=/lib/modules/`uname -r`/build
else
    KERNEL_INC=${KERNELDIR}
fi


# Add our API library path to library search path, so we can use
# it during install script
export LD_LIBRARY_PATH=${LIBRARY_PATH}:${AIMLIBDIR}


# Execute requested command
case $COMMAND in
	"install")
		echo "**** Installing MIL-STD-1553 Linux BSP ****"
		patch_bsp || failure "Patching of BSP failed"
		configure_bsp || failure "Configuration of BSP failed"
		build_bsp || failure "Building of BSP failed"
		install_bsp || failure "Installation of BSP failed"
		echo "**** Installation successful ****"
		echo ""
		echo "Do you want to update the firmware of your devices now? (y/n)"
		read update
		if [ "y${update}" = "yy" ] ; then
		    update_devices
		fi
		;;
	"build")
		echo "**** Building MIL-STD-1553 Linux BSP ****"
		patch_bsp || failure "Patching of BSP failed"
		configure_bsp || failure "Configuration of BSP failed"
		build_bsp || failure "Building of BSP failed"
		echo "**** Building successful ****"
		;;
	"clean")
		echo "**** Cleaning MIL-STD-1553 Linux BSP ****"
		clean_bsp || failure "Cleaning of BSP failed"
		echo "**** Cleaning successful ****"
		;;
	"distclean")
		echo "**** Distcleaning MIL-STD-1553 Linux BSP ****"
		distclean_bsp || failure "Distcleaning of BSP failed"
		echo "**** Distcleaning successful ****"
		;;
	"uninstall")
		echo "**** Uninstalling MIL-STD-1553 Linux BSP ****"
		# We need to configure the BSP, so we have the Makefiles for uninstall
		configure_bsp || failure "Configuration of BSP failed"
		uninstall_bsp
		echo "**** Uninstalling successful ****"
		;;
	*)
		echo "Invalid command"
		exit 1
esac
