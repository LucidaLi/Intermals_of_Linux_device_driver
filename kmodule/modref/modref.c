#include <linux/module.h>
#include <linux/kernel.h>

static struct module *mod = NULL;
static int modref_init(void)
{
    mod = find_module("demodev");
    if (!mod) {
		printk("can't find module\n");
		return -1;
    }
    __module_get(mod);
    printk("find the module, refcnt = %lu\n", module_refcount(mod));
    return 0;
}

static void modref_exit(void)
{
    if (mod) {
		module_put(mod);
		printk("mod refcnt = %lu\n", module_refcount(mod));
    }	
    printk(KERN_INFO "modref_exit()\n");
}

module_init(modref_init);
module_exit(modref_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("dennis chen @ AMDLinuxFGL");
MODULE_DESCRIPTION("A simple kernel module as an illustration");
