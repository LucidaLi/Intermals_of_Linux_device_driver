#include <linux/module.h>
#include <linux/kernel.h>

extern void my_exp_function(void);

static int demodev_init(void)
{
    my_exp_function();
    return 0;
}

static void demodev_exit(void)
{
    printk(KERN_INFO "demodev_exit()\n");
}

module_init(demodev_init);
module_exit(demodev_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("dennis chen @ AMDLinuxFGL");
MODULE_DESCRIPTION("A simple kernel module as an illustration");
