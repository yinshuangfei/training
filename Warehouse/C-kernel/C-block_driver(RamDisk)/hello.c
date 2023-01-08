//writed by 殷双飞，块设备驱动不容易调试通啊。
#include<linux/init.h>
#include<linux/module.h>
#include<linux/fs.h>
#include<linux/kernel.h>
#include<linux/types.h> 	//保存设备类型编号
#include<linux/kdev_t.h>	//
#include<linux/cdev.h>  	//注册设备结构体
#include<linux/slab.h>  	//内存管理
#include<linux/genhd.h> 	//gendisk结构
#include<linux/vmalloc.h> 	//vmalloc
#include<linux/blkdev.h>	//块设备操作的重要头文件
#include<linux/bio.h>
#include<linux/fcntl.h>
#include<linux/hdreg.h>
//#include<linux/blkpg.h>

#define Major 125
#define DevName "hello_in_proc_devices"
#define KERNEL_SECTOR_SIZE 512
#define HARDSECTOR_SIZE 512
#define NSECTORS 1024*1024*2

MODULE_LICENSE("Dual BSD/GPL");

struct hello_dev{
    int size;
    u8 *data;
    short users;
    short media_change;
    spinlock_t lock;
    struct request_queue * queue;
    struct gendisk *gd;
    struct timer_list timer;
};

int hello_open(struct block_device* bdev,fmode_t mode);
void hello_release(struct gendisk * disk,fmode_t mode);
//void hello_request(struct request_queue *queue);
static void hello_setup(struct hello_dev * dev);
static blk_qc_t hello_make_request(struct request_queue *q,struct bio *bio);
static int hello_disk_xfer_bio(struct hello_dev *dev,struct bio *bio);
static int hello_disk_getgeo(struct block_device *bdev,struct hd_geometry *geo);
static void hello_disk_transfer(struct hello_dev *dev,unsigned long sector,
                        unsigned long nsect,char * buffer , int write);
                
//文件操作函数结构体
static struct block_device_operations hello_ops={
    .owner=THIS_MODULE,
    .getgeo=hello_disk_getgeo,
    //.llseek=NULL,
    //.ioctl=NULL,
    .open=hello_open,
    .release=hello_release,
};

//全局变量
struct hello_dev * dev;
struct gendisk * disk;
struct request_queue * queue;

//设备打开
int hello_open(struct block_device* bdev,fmode_t mode){
    printk(KERN_ALERT "YSF: open done. \n");
    struct hello_dev *dev=bdev->bd_disk->private_data;
    return 0;
}

//设备释放
void hello_release(struct gendisk * disk,fmode_t mode){
    printk(KERN_ALERT "YSF: release done. \n");
    struct hello_dev *dev=disk->private_data;

    return 0;
}

// 请求处理函数
// 与 make_request 切换 使用
//void hello_request(struct request_queue *queue){
//    printk(KERN_ALERT "YSF: request function working. \n");
    
//    struct request * req;

//    while( (req=blk_fetch_request(queue))!=NULL ){
        //end_request(req,1);
//        printk(KERN_ALERT "YSF: end 1 req. \n");
//    }
//}

// 返回固定的 geo 值，md raid 里面也是这么做的
static int hello_disk_getgeo(struct block_device *bdev,struct hd_geometry *geo){
    long size;
    struct hello_dev *dev=bdev->bd_disk->private_data;

    size=dev->size*(HARDSECTOR_SIZE/KERNEL_SECTOR_SIZE);
    geo->cylinders=(size & ~0x3f ) >> 6;
    geo->heads=4;
    geo->sectors=16;
    geo->start=4;
    return 0;
}

static void hello_disk_transfer(struct hello_dev *dev,unsigned long sector,
                        unsigned long nsect,char * buffer , int write){
    
    unsigned long offset=sector*KERNEL_SECTOR_SIZE; //开始字节
    unsigned long nbytes=nsect*KERNEL_SECTOR_SIZE;  //字节数

    if ((offset + nbytes)> dev->size){
        printk(KERN_ALERT "YSF: request too big. \n");
        return ; 
    }

    if(write){
        memcpy(dev->data+offset,buffer,nbytes);
        //printk(KERN_ALERT "YSF: hello write. \n");
    } 
    else{
        memcpy(buffer,dev->data + offset,nbytes);
        //printk(KERN_ALERT "YSF: hello read. \n");
    }
        

}


static int hello_disk_xfer_bio(struct hello_dev *dev,struct bio *bio){
    struct bio_vec bvec;
    struct bvec_iter iter;
    sector_t sector =bio->bi_iter.bi_sector;

    bio_for_each_segment(bvec,bio,iter){
        char *buffer= __bio_kmap_atomic(bio, iter);
        hello_disk_transfer(dev, sector, bio_cur_bytes(bio)>>9, buffer, bio_data_dir(bio)==WRITE);
        sector+=bio_cur_bytes(bio)>>9;
        __bio_kunmap_atomic(buffer);
    }
    return 0;
}

static blk_qc_t  hello_make_request(struct request_queue *q,struct bio *bio){
   
    struct hello_dev *dev= q->queuedata;

    hello_disk_xfer_bio(dev,bio);
    bio_endio(bio);
    return 0;
}


//初始化函数
static void hello_setup(struct hello_dev * dev){
   // printk(KERN_ALERT "YSF: dev=0x%x\n",dev);
    memset(dev,0,sizeof(struct hello_dev));
    
    dev->size=NSECTORS * HARDSECTOR_SIZE;
    printk(KERN_ALERT "YSF: dev_size=0x%x\n",dev->size);
    
    dev->data=vmalloc(dev->size);//以字节分配
    if(NULL==dev->data){
        printk(KERN_ALERT "YSF: vmalloc dev->data error. \n");
        return ;
    }
    printk(KERN_ALERT "YSF: dev->data=0x%x\n",dev->data);
   
    //初始化自旋锁
    spin_lock_init(&dev->lock); 
    //dev->queue=blk_init_queue(hello_request,&dev->lock);//初始化请求队列，指定请求处理函数(绑定队列及其处理函数)，自旋锁.这是需要使用内核queue的做法
    dev->queue=blk_alloc_queue(GFP_KERNEL);
    if(NULL==dev->queue){
        printk(KERN_ALERT "YSF: alloc queue error. \n");
        goto out_vfree;
    }
    blk_queue_make_request(dev->queue,hello_make_request);

    blk_queue_logical_block_size(dev->queue,HARDSECTOR_SIZE);
    dev->queue->queuedata=dev;	//指向设备描述符
    
    //添加设备到内核中,需要执行两个关键操作
	// alloc_disk
	// add_disk
	//指定允许的分区数，值为1表示不允许被分区；
    dev->gd=alloc_disk(1);
    if(!dev->gd){
        printk(KERN_ALERT "YSF: alloc_disk error. \n");
        goto out_vfree;
    }
    
    dev->gd->major=Major;//设置主设备号
    dev->gd->first_minor=0;
    dev->gd->fops=&hello_ops;
    dev->gd->queue=dev->queue;
    dev->gd->private_data=dev;

    // 名字显示在 lsblk 里
    snprintf(dev->gd->disk_name,32,"YSF_ram_disk");
    set_capacity(dev->gd,NSECTORS * (HARDSECTOR_SIZE/KERNEL_SECTOR_SIZE));
    add_disk(dev->gd);
    return ;

out_vfree:
    if(dev->data){
        vfree(dev->data);
    }
 
}


//初始化程序
static int __init hello_init(void){    
    int ret=0;
    printk(KERN_ALERT "YSF: hello init :) \n");
    
    //注册块设备
    ret=register_blkdev(Major,DevName);         
    if(ret<0){ 
        printk(KERN_ALERT "YSF: regester error. ret=%d\n",ret);
        return -EBUSY;
    }
   
    dev=kmalloc(sizeof(struct hello_dev),GFP_KERNEL);
    if( !dev ){
		printk(KERN_ALERT "YSF: kmalloc deb error. dev=0x%x\n",(int *)dev);
        goto error;
    }

    hello_setup(dev);

    return 0;
error:
    unregister_blkdev(Major,DevName);        //注销开设备  
    return -ENOMEM;
}



//卸载程序
static void __exit hello_exit(void){    
    printk(KERN_ALERT "YSF: byebye hello exit :) \n");

    if(dev->gd){
        del_gendisk(dev->gd);//这句话容易把系统搞死
        put_disk(dev->gd); 
        printk(KERN_ALERT "YSF: del good! :) \n");
    }
    
    if(dev->queue){
        printk(KERN_ALERT "YSF: free(dev->queue). \n");
        blk_cleanup_queue(dev->queue);

    }

    if(dev->data){
        printk(KERN_ALERT "YSF: vfree(dev->data). \n");
        vfree(dev->data);
    }
    if(dev){
        printk(KERN_ALERT "YSF: kfree(dev). \n");
        kfree(dev);
    }


    printk(KERN_ALERT "YSF: byebye hello exit!!!!! :) \n");
    unregister_blkdev(Major,DevName);        //注销开设备
}


module_init(hello_init);
module_exit(hello_exit);

