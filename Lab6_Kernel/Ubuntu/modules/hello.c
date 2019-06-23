#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Jessie Peng <jessie_p@yeah.net>");
MODULE_DESCRIPTION("Kernel module for Lab6");

static int __init hello(void)
{
	printk(KERN_INFO "Hello World\n");
	return 0;
}

static void __exit goodbye(void)
{
	printk(KERN_INFO "Goodbye World\n");
}

module_init(hello);
module_exit(goodbye);
