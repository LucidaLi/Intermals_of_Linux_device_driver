#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/kobject.h>
#include <linux/sysfs.h>

extern struct kobject *test_parent;
static int addobj_init(void)
{
	test_parent = kobject_create_and_add("parent", NULL);
	return 0;
}

static void addobj_exit(void)
{
	kobject_del(test_parent);
}

module_init(addobj_init);
module_exit(addobj_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("dennis chen");
MODULE_DESCRIPTION("A simple kernel module as fs registration demo");
