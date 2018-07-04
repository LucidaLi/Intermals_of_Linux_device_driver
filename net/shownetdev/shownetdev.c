/*
 * This file is used to display all the net_device which have been registered
 * into the system
 * Nov.7, 2014
 */
#include <linux/module.h>
#include <linux/kernel.h>
#include <net/net_namespace.h>
#include <net/addrconf.h>
#include <linux/inet.h>
#include <linux/netdevice.h>
#include <linux/inetdevice.h>
#include <net/sch_generic.h>

static void dump_indev(struct in_device *idev)
{
	struct in_ifaddr *iifa = idev->ifa_list;
	if (iifa) {
		printk(KERN_INFO "local = %pI4, address = %pI4\n", &iifa->ifa_local, &iifa->ifa_address);
		printk(KERN_INFO "Broadcast = %pI4, Mask = %pI4\n", &iifa->ifa_broadcast, &iifa->ifa_mask);
		printk(KERN_INFO "ifa_label = %s, scope = %d\n", iifa->ifa_label, iifa->ifa_scope);
	}
}

static int netdev_show_init(void)
{
	struct net_device *dev;
	struct net *net = &init_net;
	struct in_device *in_dev;
	struct inet6_dev *ip6;
	int ns_cnt = 0;

	printk(KERN_INFO "++%s\n", __func__);
  	for_each_netdev_rcu(net, dev) {
		printk(KERN_INFO "%s[%d], type = %d, qdisc = %s\n", dev->name,  
				dev->ifindex, dev->type, dev->qdisc->ops->id);
		ip6 = __in6_dev_get(dev);
		if (ip6) 
			printk(KERN_INFO "%s has ipv6 address\n", dev->name);
		else
			printk(KERN_INFO "%s hasn't ipv6 address\n", dev->name);

		in_dev = __in_dev_get_rtnl(dev);
		if (in_dev) 
			dump_indev(in_dev);	
	}

	for_each_net_rcu(net) {
		ns_cnt++;
	}
	printk(KERN_INFO "ns count = %d, page_sieze = 0x%lx, bits_long = %x\n", ns_cnt, PAGE_SIZE, ~(BITS_PER_LONG-1));

	return 0;
}

static void netdev_show_exit(void)
{
	printk(KERN_INFO "--%s\n", __func__);
}

module_init(netdev_show_init);
module_exit(netdev_show_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("dennis chen");
MODULE_DESCRIPTION("To display all the net devices registered into the system");
