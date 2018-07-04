#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/workqueue.h>
#include <linux/slab.h>

static void test_work(struct work_struct *work)
{
	printk(KERN_INFO "++test_work--\n");
	return;
}

struct work_struct *t_work;
struct workqueue_struct *wq;
static int demodev_init(void)
{
	t_work = kzalloc(sizeof(struct work_struct), GFP_KERNEL);
	if (!t_work) {
		printk(KERN_INFO "kzalloc failed\n");
		return -1;
	}
	INIT_WORK(t_work, test_work);
	wq = alloc_workqueue("ceph-writeback", 0, 1);
	if (!wq) {
		printk(KERN_INFO "alloc_workqueue failed\n");
		return -1;
	}

	queue_work(wq, t_work);
	destroy_workqueue(wq);
	kfree(t_work);
	return 0;
}

static void demodev_exit(void)
{
	//destroy_workqueue(wq);
	//kfree(t_work);
	printk(KERN_INFO "demodev_exit()\n");
}

module_init(demodev_init);
module_exit(demodev_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("dennis chen @ AMDLinuxFGL");
MODULE_DESCRIPTION("A simple kernel module as an illustration");
