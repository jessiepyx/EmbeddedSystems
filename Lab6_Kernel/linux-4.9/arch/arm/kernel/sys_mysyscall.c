#include <linux/kernel.h>

void sys_mysyscall(void) {
	printk("This is my system call for Lab6\n");
}
