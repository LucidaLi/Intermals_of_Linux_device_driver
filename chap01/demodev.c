#include <linux/module.h>
#include <linux/kernel.h>

//exported function
static void my_exp_function(void)
{
    printk(KERN_INFO "A function exported to the external world...\n");
}

EXPORT_SYMBOL(my_exp_function);

//module parameters
int dolphin;
module_param(dolphin, int, 0);

static char demodev_str[64] = "kernel module:demodev";
module_param_string(demodev_str, demodev_str, sizeof(demodev_str), 0);//(name, string, length, perm)

static int demodev_init(void)
{
    printk(KERN_INFO "demodev_init(): dolphin=%d, demodev_str=%s\n", dolphin, demodev_str);  
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
