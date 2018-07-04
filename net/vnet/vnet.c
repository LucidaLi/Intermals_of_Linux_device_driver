/* 
 * A software defined network device...
 * Mar.30, 2015
 */
#include <linux/module.h>
#include <linux/kernel.h>
#include <asm/string.h>
#include <net/net_namespace.h>
#include <linux/netdevice.h>
#include <linux/etherdevice.h>


struct vnet_private {
	int a;	
	int reserved;
};

int vnet_open(struct net_device *dev)
{
	printk(KERN_INFO "++%s\n", __func__);
	//dump_stack();
	netif_carrier_on(dev);
	return 0;
}

int vnet_close(struct net_device *dev)
{
        printk(KERN_INFO "++%s\n", __func__);

        return 0;
}


netdev_tx_t vnet_start_xmit(struct sk_buff *skb, struct net_device *dev)
{
	struct ethhdr *hdr, *eth;
	u16 ether_type;
	printk(KERN_INFO "++%s\n", __func__);
	//dev->stats.tx_dropped++;
	hdr = (struct ethhdr *)skb->data;
	eth = eth_hdr(skb);
	ether_type = ntohs(hdr->h_proto);
	printk(KERN_INFO "ether_type = 0x%x, protocol = 0x%x\n", ether_type, ntohs(skb->protocol));
	printk(KERN_INFO "h_proto = 0x%x, dest = %s, src = %s\n", ntohs(eth->h_proto), eth->h_dest, eth->h_source);
	dev_kfree_skb_any(skb);
	//netif_stop_queue(dev);
        return NETDEV_TX_OK;
}

void vnet_tx_timeout(struct net_device *dev)
{
	printk(KERN_INFO "++%s\n", __func__);
	
	return;
}

static const struct net_device_ops vnet_ops = {
	.ndo_open = vnet_open,
	.ndo_stop = vnet_close,
	.ndo_start_xmit = vnet_start_xmit,
	.ndo_tx_timeout = vnet_tx_timeout,
};

static struct net_device *ndev;

static int vnet_init(void)
{
	struct vnet_private *vnet_priv;
	int rc;

	printk(KERN_INFO "++%s\n", __func__);
        
	ndev = alloc_etherdev(sizeof(struct vnet_private));
	if (!ndev)
		return -ENOMEM;

	//SET_NETDEV_DEV(ndev, &pdev->dev);
	ndev->netdev_ops = &vnet_ops;
	vnet_priv = netdev_priv(ndev);
	ndev->watchdog_timeo = 5 * HZ;
		
	rc = register_netdev(ndev);

	netif_carrier_off(ndev);
	
	return rc;	


}

static void vnet_exit(void)
{
        printk(KERN_INFO "--%s\n", __func__);
	unregister_netdev(ndev);
	if (ndev)
		free_netdev(ndev);
}

module_init(vnet_init);
module_exit(vnet_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("dennis chen");
MODULE_DESCRIPTION("A software defined network device driver");
