#include <linux/module.h>
#include <linux/kernel.h>

#include "ext_func.h"

static int vmxtest_init(void)
{
	printk(KERN_INFO "vmxtest_init()\n");
	just_a_func();
	return 0;
}

static void vmxtest_exit(void)
{
	printk(KERN_INFO "vmxtest_exit()\n");
}

module_init(vmxtest_init);
module_exit(vmxtest_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("kernel.org.gnu@gmail.com");
MODULE_DESCRIPTION("A simple kernel module to test the kvm module");
