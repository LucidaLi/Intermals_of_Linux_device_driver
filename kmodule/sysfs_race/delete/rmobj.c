#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/kobject.h>
#include <linux/sysfs.h>

extern struct kobject *test_parent;
struct kobject *child;

static int rmobj_init(void)
{
	child = kobject_create_and_add("child", test_parent);
	return 0;
}

static void rmobj_exit(void)
{
	kobject_del(child);
}

module_init(rmobj_init);
module_exit(rmobj_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("dennis chen");
MODULE_DESCRIPTION("A simple kernel module as fs registration demo");
