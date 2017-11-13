#include <linux/module.h>
#include <linux/vermagic.h>
#include <linux/compiler.h>

MODULE_INFO(vermagic, VERMAGIC_STRING);

__visible struct module __this_module
__attribute__((section(".gnu.linkonce.this_module"))) = {
	.name = KBUILD_MODNAME,
	.init = init_module,
#ifdef CONFIG_MODULE_UNLOAD
	.exit = cleanup_module,
#endif
	.arch = MODULE_ARCH_INIT,
};

static const struct modversion_info ____versions[]
__used
__attribute__((section("__versions"))) = {
	{ 0x13fa4f04, __VMLINUX_SYMBOL_STR(module_layout) },
	{ 0x9d82466, __VMLINUX_SYMBOL_STR(class_unregister) },
	{ 0x97b3b34, __VMLINUX_SYMBOL_STR(device_destroy) },
	{ 0xa0433705, __VMLINUX_SYMBOL_STR(class_destroy) },
	{ 0x40acf8ae, __VMLINUX_SYMBOL_STR(device_create) },
	{ 0x6bc3fbc0, __VMLINUX_SYMBOL_STR(__unregister_chrdev) },
	{ 0xdd9af2a7, __VMLINUX_SYMBOL_STR(__class_create) },
	{ 0x9084b09b, __VMLINUX_SYMBOL_STR(__register_chrdev) },
	{ 0x353c50a8, __VMLINUX_SYMBOL_STR(kmem_cache_alloc_trace) },
	{ 0xb0ebad1f, __VMLINUX_SYMBOL_STR(kmalloc_caches) },
	{ 0xc2d711e1, __VMLINUX_SYMBOL_STR(krealloc) },
	{ 0xd0d8621b, __VMLINUX_SYMBOL_STR(strlen) },
	{ 0x37a0cba, __VMLINUX_SYMBOL_STR(kfree) },
	{ 0x50eedeb8, __VMLINUX_SYMBOL_STR(printk) },
	{ 0xc3aaf0a9, __VMLINUX_SYMBOL_STR(__put_user_1) },
	{ 0xb4390f9a, __VMLINUX_SYMBOL_STR(mcount) },
};

static const char __module_depends[]
__used
__attribute__((section(".modinfo"))) =
"depends=";


MODULE_INFO(srcversion, "C6538B8DA66CE4402C3948A");
