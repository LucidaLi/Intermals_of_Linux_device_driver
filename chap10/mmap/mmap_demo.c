#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>

#include <linux/device.h>
#include <linux/cdev.h>
#include <linux/fs.h>
#include <linux/fcntl.h>
#include <linux/gfp.h>
#include <linux/string.h>
#include <linux/mm_types.h>
#include <linux/mm.h>
#include <linux/highmem.h>

#define KSTR_DEF    "Hello world from kernel virtual space"
    
static struct cdev *pcdev;
static dev_t ndev;
static struct page *pg;
static struct timer_list timer;

static void timer_func(unsigned long data)
{
    printk("timer_func:%s\n", (char *)data);
    timer.expires = jiffies + HZ*10;
    add_timer(&timer);
}

static int demo_open(struct inode *inode, struct file *filp)
{
    return 0;
}

static int demo_release(struct inode *inode, struct file *filp)
{   
    return 0;
}

static int demo_mmap(struct file *filp, struct vm_area_struct *vma)
{
    int err = 0;
    unsigned long start = vma->vm_start;
    unsigned long size = vma->vm_end - vma->vm_start;
  
    err = remap_pfn_range(vma, start, vma->vm_pgoff, size, vma->vm_page_prot);
    return err;
}


static struct file_operations mmap_fops =
{   .owner = THIS_MODULE,
    .open = demo_open,
    .release = demo_release,
    .mmap = demo_mmap,
};



static int demo_map_init(void)
{
    int err = 0;
    char *kstr;

    // allocate a page in HIGH_MEM area
    pg = alloc_pages(GFP_HIGHUSER, 0);
    if(!pg)
        return -ENOMEM;
    SetPageReserved(pg);
    // page comes from the highmem area, so need to call kmap to establish the mapping mechanism...
    kstr = (char *)kmap(pg);
    strcpy(kstr, KSTR_DEF);
    printk("kpa = 0x%lx, kernel string = %s\n", page_to_phys(pg), kstr);

    pcdev = cdev_alloc();
    if(!pcdev){
        err = -ENOMEM; 
        goto cdev_err;  
    }
    cdev_init(pcdev, &mmap_fops);
    err = alloc_chrdev_region(&ndev, 0, 1, "mmap_dev");
    if(err)
        goto region_err;
    printk("major = %d, minor = %d\n", MAJOR(ndev), MINOR(ndev));
    pcdev->owner = THIS_MODULE;
    err = cdev_add(pcdev, ndev, 1);
    if(err)
        goto add_err;

    init_timer(&timer);
    timer.function = timer_func;
    timer.data = (unsigned long)kstr;
    timer.expires = jiffies + HZ*10;
    add_timer(&timer);
    goto done;
    
add_err:
    unregister_chrdev_region(ndev, 1);
region_err:
    cdev_del(pcdev);
cdev_err:
    kunmap(pg);
    ClearPageReserved(pg);
    __free_pages(pg, 0); 
done:         
    return err;
}

static void demo_map_exit(void)
{
    del_timer_sync(&timer);
    cdev_del(pcdev); 
    unregister_chrdev_region(ndev, 1);
    kunmap(pg); 
    ClearPageReserved(pg); 
    __free_pages(pg, 0); 
}

module_init(demo_map_init);
module_exit(demo_map_exit);

MODULE_AUTHOR("dennis chen @ AMDLinuxFGL");
MODULE_DESCRIPTION("Remap a physical page in the kernel space to the user space");
MODULE_LICENSE("GPL");
