/*
 * This file is used to play with the Qdisc for the net dev queue
 * Nov.12, 2014
 */
#include <linux/module.h>
#include <linux/kernel.h>
#include <net/net_namespace.h>
#include <net/sch_generic.h>
#include <linux/netdevice.h>


static int qdisc_init(void)
{
	struct net_device *dev;
	struct net *net = &init_net;
	struct Qdisc *qdisc;

	printk(KERN_INFO "++%s\n", __func__);
  	for_each_netdev_rcu(net, dev) {
		qdisc = dev->_tx[0].qdisc;
		printk(KERN_INFO "[%s]tx->qdisc = 0x%p[%c]\n", dev->name, 
			qdisc, netif_is_multiqueue(dev) ? 'M' : 'S');
		if (qdisc->ops)
			printk(KERN_INFO "qdisc = %s\n", qdisc->ops->id);
	}
	

	return 0;
}

static void qdisc_exit(void)
{
	printk(KERN_INFO "--%s\n", __func__);
}

module_init(qdisc_init);
module_exit(qdisc_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("dennis chen");
MODULE_DESCRIPTION("Qdisc in the net dev queue");
