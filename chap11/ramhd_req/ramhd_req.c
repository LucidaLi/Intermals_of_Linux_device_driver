#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>

#include <linux/fs.h>
#include <linux/types.h>
#include <linux/fcntl.h>
#include <linux/vmalloc.h>
#include <linux/blkdev.h>
#include <linux/hdreg.h>

#define RAMHD_NAME              "ramsd"
#define RAMHD_MAX_DEVICE        2
#define RAMHD_MAX_PARTITIONS    4

#define RAMHD_SECTOR_SIZE       512
#define RAMHD_SECTORS           16
#define RAMHD_HEADS             4
#define RAMHD_CYLINDERS         256

#define RAMHD_SECTOR_TOTAL      (RAMHD_SECTORS * RAMHD_HEADS * RAMHD_CYLINDERS)
#define RAMHD_SIZE              (RAMHD_SECTOR_SIZE * RAMHD_SECTOR_TOTAL) //8MB

typedef struct{
    unsigned char   *data;
    struct request_queue *queue;
    spinlock_t      lock;
    struct gendisk  *gd;
}RAMHD_DEV;

static char *sdisk[RAMHD_MAX_DEVICE];
static RAMHD_DEV *rdev[RAMHD_MAX_DEVICE];

static dev_t ramhd_major;

static int ramhd_space_init(void)
{
    int i;
    int err = 0;
    for(i = 0; i < RAMHD_MAX_DEVICE; i++){
        sdisk[i] = vmalloc(RAMHD_SIZE);
        if(!sdisk[i]){
            err = -ENOMEM;
            return err;
        }
        memset(sdisk[i], 0, RAMHD_SIZE);
    }

    return err;
}

static void ramhd_space_clean(void)
{
    int i;
    for(i = 0; i < RAMHD_MAX_DEVICE; i++){
        vfree(sdisk[i]);
    }
}

static int alloc_ramdev(void)
{
    int i;
    for(i = 0; i < RAMHD_MAX_DEVICE; i++){
        rdev[i] = kzalloc(sizeof(RAMHD_DEV), GFP_KERNEL);
        if(!rdev[i])
            return -ENOMEM;
    }
    return 0;
}

static void clean_ramdev(void)
{
    int i;
    for(i = 0; i < RAMHD_MAX_DEVICE; i++){
        if(rdev[i])
            kfree(rdev[i]);
    }   
}       

int ramhd_open(struct block_device *bdev, fmode_t mode)
{
    return 0;
}

int ramhd_release(struct gendisk *gd, fmode_t mode)
{   
    return 0;
}

static int ramhd_ioctl(struct block_device *bdev, fmode_t mode, unsigned int cmd, unsigned long arg)
{
    int err;
    struct hd_geometry geo;

    switch(cmd)
    {
        case HDIO_GETGEO:
            err = !access_ok(VERIFY_WRITE, arg, sizeof(geo));
            if(err) return -EFAULT;
        
            geo.cylinders = RAMHD_CYLINDERS;
            geo.heads = RAMHD_HEADS;
            geo.sectors = RAMHD_SECTORS;
            geo.start = get_start_sect(bdev);
            if(copy_to_user((void *)arg, &geo, sizeof(geo)))
                return -EFAULT;
            return 0;
    }
            
    return -ENOTTY;
}

static struct block_device_operations ramhd_fops =
{   .owner = THIS_MODULE,
    .open = ramhd_open,
    .release = ramhd_release,
    .ioctl = ramhd_ioctl,
};

void ramhd_req_func (struct request_queue *q)
{
    struct request *req;
	RAMHD_DEV *pdev;
	char *pData;
	unsigned long addr, size, start;
	req = blk_fetch_request(q);
	while (req) {
		start = blk_rq_pos(req); // The sector cursor of the current request
		pdev = (RAMHD_DEV *)req->rq_disk->private_data;
		pData = pdev->data;
		addr = (unsigned long)pData + start * RAMHD_SECTOR_SIZE;
        size = blk_rq_cur_bytes(req);
		if (rq_data_dir(req) == READ)
			memcpy(req->buffer, (char *)addr, size);
		else
			memcpy((char *)addr, req->buffer, size);
	
		if(!__blk_end_request_cur(req, 0))
			req = blk_fetch_request(q);
	}
}

int ramhd_init(void)
{
    int i;
    ramhd_space_init();
    alloc_ramdev();
    
    ramhd_major = register_blkdev(0, RAMHD_NAME);
    
    for(i = 0; i < RAMHD_MAX_DEVICE; i++)
    {
        rdev[i]->data = sdisk[i]; 
        rdev[i]->gd = alloc_disk(RAMHD_MAX_PARTITIONS);
        spin_lock_init(&rdev[i]->lock);
        rdev[i]->queue = blk_init_queue(ramhd_req_func, &rdev[i]->lock);
        rdev[i]->gd->major = ramhd_major;
        rdev[i]->gd->first_minor = i * RAMHD_MAX_PARTITIONS;
        rdev[i]->gd->fops = &ramhd_fops;
        rdev[i]->gd->queue = rdev[i]->queue;
        rdev[i]->gd->private_data = rdev[i];
        sprintf(rdev[i]->gd->disk_name, "ramsd%c", 'a'+i);
        set_capacity(rdev[i]->gd, RAMHD_SECTOR_TOTAL);
        add_disk(rdev[i]->gd);
    }
        
    return 0;
}

void ramhd_exit(void)
{
    int i;
    for(i = 0; i < RAMHD_MAX_DEVICE; i++)
    {
        del_gendisk(rdev[i]->gd);
        put_disk(rdev[i]->gd);     
        blk_cleanup_queue(rdev[i]->queue);  
    }
    unregister_blkdev(ramhd_major,RAMHD_NAME);  
    clean_ramdev();
    ramhd_space_clean();  
}

module_init(ramhd_init);
module_exit(ramhd_exit);

MODULE_AUTHOR("dennis chen @ AMDLinuxFGL");
MODULE_DESCRIPTION("The ramdisk implementation with request function");
MODULE_LICENSE("GPL");
