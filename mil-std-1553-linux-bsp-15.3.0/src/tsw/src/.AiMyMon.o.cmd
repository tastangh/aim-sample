cmd_/home/aim/mil-std-1553-linux-bsp-15.3.0/src/tsw/src/AiMyMon.o := gcc -Wp,-MMD,/home/aim/mil-std-1553-linux-bsp-15.3.0/src/tsw/src/.AiMyMon.o.d -nostdinc -isystem /usr/lib/gcc/x86_64-linux-gnu/13/include -I./arch/x86/include -I./arch/x86/include/generated  -I./include -I./arch/x86/include/uapi -I./arch/x86/include/generated/uapi -I./include/uapi -I./include/generated/uapi -include ./include/linux/kconfig.h -include ./include/linux/compiler_types.h -D__KERNEL__ -fmacro-prefix-map=./= -Wall -Wundef -Werror=strict-prototypes -Wno-trigraphs -fno-strict-aliasing -fno-common -fshort-wchar -fno-PIE -Werror=implicit-function-declaration -Werror=implicit-int -Werror=return-type -Wno-format-security -std=gnu89 -mno-sse -mno-mmx -mno-sse2 -mno-3dnow -mno-avx -fcf-protection=none -m64 -falign-jumps=1 -falign-loops=1 -mno-80387 -mno-fp-ret-in-387 -mpreferred-stack-boundary=3 -mskip-rax-setup -mtune=generic -mno-red-zone -mcmodel=kernel -DCONFIG_X86_X32_ABI -Wno-sign-compare -fno-asynchronous-unwind-tables -mindirect-branch=thunk-extern -mindirect-branch-register -mindirect-branch-cs-prefix -mfunction-return=thunk-extern -fno-jump-tables -fno-delete-null-pointer-checks -Wno-frame-address -Wno-format-truncation -Wno-format-overflow -Wno-address-of-packed-member -O2 -fno-allow-store-data-races -Wframe-larger-than=1024 -fstack-protector-strong -Wimplicit-fallthrough -Wno-unused-but-set-variable -Wno-unused-const-variable -Wno-dangling-pointer -fno-omit-frame-pointer -fno-optimize-sibling-calls -g -gdwarf-4 -fcf-protection=none -pg -mrecord-mcount -mfentry -DCC_USING_FENTRY -Wdeclaration-after-statement -Wvla -Wno-pointer-sign -Wno-stringop-truncation -Wno-zero-length-bounds -Wno-array-bounds -Wno-stringop-overflow -Wno-restrict -Wno-maybe-uninitialized -fno-strict-overflow -fno-stack-check -fconserve-stack -Werror=date-time -Werror=incompatible-pointer-types -Werror=designated-init -Wno-packed-not-aligned -D_AIM_LINUX -D_AIM_SYSDRV -DAPI1553 -D_CMPL4API3910 -D_LINUX -D_AMC1553 -I/home/aim/mil-std-1553-linux-bsp-15.3.0/src/tsw -I/home/aim/mil-std-1553-linux-bsp-15.3.0/src/tsw/include -I/home/aim/mil-std-1553-linux-bsp-15.3.0/src/tsw/src/monitor/pmc -I/home/aim/mil-std-1553-linux-bsp-15.3.0/src/tsw/src -I/home/aim/mil-std-1553-linux-bsp-15.3.0/src/AIM-BSP-Common/include -D_AIM_1553_SYSDRV_USB -I/home/aim/mil-std-1553-linux-bsp-15.3.0/src/drivers/usb/include/memorymirror -I/home/aim/mil-std-1553-linux-bsp-15.3.0/src/drivers/usb/include/hardware -I/home/aim/mil-std-1553-linux-bsp-15.3.0/src/drivers/usb/include/core  -DMODULE  -DKBUILD_BASENAME='"AiMyMon"' -DKBUILD_MODNAME='"libaim_mil_tsw"' -c -o /home/aim/mil-std-1553-linux-bsp-15.3.0/src/tsw/src/AiMyMon.o /home/aim/mil-std-1553-linux-bsp-15.3.0/src/tsw/src/AiMyMon.c

source_/home/aim/mil-std-1553-linux-bsp-15.3.0/src/tsw/src/AiMyMon.o := /home/aim/mil-std-1553-linux-bsp-15.3.0/src/tsw/src/AiMyMon.c

deps_/home/aim/mil-std-1553-linux-bsp-15.3.0/src/tsw/src/AiMyMon.o := \
  include/linux/kconfig.h \
    $(wildcard include/config/cc/version/text.h) \
    $(wildcard include/config/cpu/big/endian.h) \
    $(wildcard include/config/booger.h) \
    $(wildcard include/config/foo.h) \
  include/linux/compiler_types.h \
    $(wildcard include/config/have/arch/compiler/h.h) \
    $(wildcard include/config/enable/must/check.h) \
    $(wildcard include/config/cc/has/asm/inline.h) \
  include/linux/compiler_attributes.h \
  include/linux/compiler-gcc.h \
    $(wildcard include/config/arm64.h) \
    $(wildcard include/config/retpoline.h) \
    $(wildcard include/config/arch/use/builtin/bswap.h) \
    $(wildcard include/config/kcov.h) \
  /home/aim/mil-std-1553-linux-bsp-15.3.0/src/AIM-BSP-Common/include/hw/AiMyMon.h \
  /home/aim/mil-std-1553-linux-bsp-15.3.0/src/AIM-BSP-Common/include/Ai_cdef.h \
  /home/aim/mil-std-1553-linux-bsp-15.3.0/src/AIM-BSP-Common/include/Ai_compiler.h \
  /home/aim/mil-std-1553-linux-bsp-15.3.0/src/AIM-BSP-Common/include/Ai_gcc.h \
  include/linux/types.h \
    $(wildcard include/config/have/uid16.h) \
    $(wildcard include/config/uid16.h) \
    $(wildcard include/config/arch/dma/addr/t/64bit.h) \
    $(wildcard include/config/phys/addr/t/64bit.h) \
    $(wildcard include/config/64bit.h) \
  include/uapi/linux/types.h \
  arch/x86/include/generated/uapi/asm/types.h \
  include/uapi/asm-generic/types.h \
  include/asm-generic/int-ll64.h \
  include/uapi/asm-generic/int-ll64.h \
  arch/x86/include/uapi/asm/bitsperlong.h \
  include/asm-generic/bitsperlong.h \
  include/uapi/asm-generic/bitsperlong.h \
  include/uapi/linux/posix_types.h \
  include/linux/stddef.h \
  include/uapi/linux/stddef.h \
  include/linux/compiler_types.h \
  arch/x86/include/asm/posix_types.h \
    $(wildcard include/config/x86/32.h) \
  arch/x86/include/uapi/asm/posix_types_64.h \
  include/uapi/asm-generic/posix_types.h \
  /home/aim/mil-std-1553-linux-bsp-15.3.0/src/AIM-BSP-Common/include/Ai_types.h \
  /home/aim/mil-std-1553-linux-bsp-15.3.0/src/AIM-BSP-Common/include/Ai_exports.h \

/home/aim/mil-std-1553-linux-bsp-15.3.0/src/tsw/src/AiMyMon.o: $(deps_/home/aim/mil-std-1553-linux-bsp-15.3.0/src/tsw/src/AiMyMon.o)

$(deps_/home/aim/mil-std-1553-linux-bsp-15.3.0/src/tsw/src/AiMyMon.o):
