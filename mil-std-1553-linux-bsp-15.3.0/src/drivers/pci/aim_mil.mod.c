#include <linux/module.h>
#define INCLUDE_VERMAGIC
#include <linux/build-salt.h>
#include <linux/vermagic.h>
#include <linux/compiler.h>

BUILD_SALT;

MODULE_INFO(vermagic, VERMAGIC_STRING);
MODULE_INFO(name, KBUILD_MODNAME);

__visible struct module __this_module
__section(".gnu.linkonce.this_module") = {
	.name = KBUILD_MODNAME,
	.init = init_module,
#ifdef CONFIG_MODULE_UNLOAD
	.exit = cleanup_module,
#endif
	.arch = MODULE_ARCH_INIT,
};

#ifdef CONFIG_RETPOLINE
MODULE_INFO(retpoline, "Y");
#endif

MODULE_INFO(depends, "");

MODULE_ALIAS("pci:v00001447d00001100sv00001447sd00000001bc*sc*i*");
MODULE_ALIAS("pci:v00001447d00001200sv00001447sd00000011bc*sc*i*");
MODULE_ALIAS("pci:v00001447d00000001sv00001447sd00000011bc*sc*i*");
MODULE_ALIAS("pci:v00001447d00001310sv00001447sd00000011bc*sc*i*");
MODULE_ALIAS("pci:v00001447d00000000sv00001447sd00000011bc*sc*i*");
MODULE_ALIAS("pci:v00001447d00000003sv00001447sd00000031bc*sc*i*");
MODULE_ALIAS("pci:v00001447d00002100sv00001447sd00000001bc*sc*i*");
MODULE_ALIAS("pci:v00001447d00004100sv00001447sd00000011bc*sc*i*");
MODULE_ALIAS("pci:v00001447d00005200sv00001447sd00000011bc*sc*i*");
MODULE_ALIAS("pci:v00001447d00005200sv00001447sd00001111bc*sc*i*");

MODULE_INFO(srcversion, "12B5AE70F7185010768F2A7");
