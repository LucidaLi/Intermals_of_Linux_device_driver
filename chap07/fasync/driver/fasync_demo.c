#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <asm/signal.h>
#include <asm/siginfo.h>

static struct cdev *pcdev;
static dev_t ndev;
static struct class *fa_cls;
static struct device *fadev;

static unsigned long flag = 0;
static struct fasync_struct *sigio_list;

static ssize_t read_flag(struct device *dev, struct device_attribute *attr, char *buf)
{
    size_t count = 0;
    count += sprintf(&buf[count], "%lu\n", flag);

    return count;    
}

static ssize_t write_flag(struct device *dev, struct device_attribute *attr, 
                            const char *buf, size_t count)
{
    flag = buf[0] - '0'; 
    //triger the signal to the user space...
    kill_fasync(&sigio_list, SIGIO, POLL_IN);
    return count;    
}

static struct device_attribute flag_attr = 
    __ATTR(flag, S_IRUGO | S_IWUSR, read_flag, write_flag);


static int fa_open(struct inode *inode, struct file *flp)
{
	return 0;
}

static int fa_async(int fd, struct file *filp, int onflag)
{
    return fasync_helper(fd, filp, onflag, &sigio_list);     
}

static struct file_operations ops = {
    .owner = THIS_MODULE,
	.open = fa_open,
    .fasync = fa_async,
};

static int fa_init(void)
{
    int ret = 0;
   
    ret = alloc_chrdev_region(&ndev, 0, 1, "fa_dev");
    if(ret < 0)
        return ret;
	
    pcdev = cdev_alloc();
    cdev_init(pcdev, &ops);
    pcdev->owner = THIS_MODULE;
    cdev_add(pcdev,ndev,1);

    fa_cls = class_create(THIS_MODULE, "fa_dev");
    if(IS_ERR(fa_cls))
        return PTR_ERR(fa_cls);
    fadev = device_create(fa_cls, NULL, ndev, NULL, "fa_dev");
    if(IS_ERR(fadev))
        return PTR_ERR(fadev);

    ret = device_create_file(fadev, &flag_attr);
  
    return ret;
}

static void fa_exit(void)
{
    device_remove_file(fadev, &flag_attr);
    device_destroy(fa_cls, ndev);
    class_destroy(fa_cls);
    cdev_del(pcdev);
    unregister_chrdev_region(ndev, 1);
}

module_init(fa_init);
module_exit(fa_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("dennis chen @ AMDLinuxFGL");
MODULE_DESCRIPTION("A simple character device driver to demo the implementation of fasync method");

