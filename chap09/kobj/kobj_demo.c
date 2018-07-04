#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/kobject.h>
#include <linux/sysfs.h>
#include <linux/slab.h>

static struct kobject *parent;
static struct kobject *child;
static struct kset *c_kset;

static unsigned long flag = 1;

static ssize_t att_show(struct kobject *kobj, struct attribute *attr, char *buf)
{
    size_t count = 0;
    count += sprintf(&buf[count], "%lu\n", flag);

    return count;  
}

static ssize_t att_store(struct kobject *kobj, struct attribute *attr, 
                                        const char *buf, size_t count)
{
    flag = buf[0] - '0';
    switch(flag){
    case 0:
        kobject_uevent(kobj, KOBJ_ADD);
        break;  
    case 1:
        kobject_uevent(kobj, KOBJ_REMOVE);
        break; 
    case 2:
        kobject_uevent(kobj, KOBJ_CHANGE);
        break;
    case 3:
        kobject_uevent(kobj, KOBJ_MOVE);
        break;
    case 4:
        kobject_uevent(kobj, KOBJ_ONLINE);
        break;
    case 5:
        kobject_uevent(kobj, KOBJ_OFFLINE);
        break;
    case 6:
        kobj->uevent_suppress = 1;
        break;
    case 7:
        kobj->uevent_suppress = 0;
        break;
    }
    return count;
}

static struct attribute cld_att = {
    .name = "cldatt",
    .mode = S_IRUGO | S_IWUSR,
};

static const struct sysfs_ops att_ops = {
    .show = att_show,
    .store = att_store,
};

static struct kobj_type cld_ktype = {
    .sysfs_ops = &att_ops,
};

static int kobj_demo_init(void)
{
    int err;

    parent = kobject_create_and_add("pa_obj", NULL);
    child = kzalloc(sizeof(*child), GFP_KERNEL);
    if(!child)
        return PTR_ERR(child);

    c_kset = kset_create_and_add("c_kset", NULL, parent);
    if(!c_kset)
        return -1;
    child->kset = c_kset;

    err = kobject_init_and_add(child, &cld_ktype, parent, "cld_obj");
    if(err)
        return err;
    err = sysfs_create_file(child, &cld_att);
    
    return err;
}

static void kobj_demo_exit(void)
{
    sysfs_remove_file(child, &cld_att);

    kset_unregister(c_kset);
    kobject_del(child);
    kobject_del(parent);
}

module_init(kobj_demo_init);
module_exit(kobj_demo_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("dennis chen @ AMDLinuxFGL");
MODULE_DESCRIPTION("A simple kernel module to demo the kobject behavior");

