#include<linux/init.h>
#include<linux/module.h>
#include<linux/fs.h>
#include<linux/types.h> //保存设备类型编号
#include<linux/kdev_t.h>//
#include<linux/cdev.h>  //注册设备结构体
#include<linux/slab.h>  //内存管理

MODULE_LICENSE("Dual BSD/GPL");

//全局变量
dev_t dev;              //设备编号，设备inode结构
struct cdev *my_cdev;   //字符设备内核内部结构

//设备打开
int ysf_open(struct inode* inode,struct file *filp){
    printk(KERN_ALERT "YSF: open done. \n");
    return 0;
}

//设备释放
int ysf_release(struct inode* inode,struct file *filp){
    printk(KERN_ALERT "YSF: release done. \n");
    return 0;
}

//设备读取
int ysf_read(struct file *filp, char __user * buff ,size_t count, loff_t *offp ){
    printk(KERN_ALERT "YSF: read done. \n");
    return 0;
}

//设备写入
int ysf_write(struct file *filp, const char __user * buff ,size_t count, loff_t *offp ){
    printk(KERN_ALERT "YSF: write done. \n");
    return 0;
}


//文件操作函数结构体
struct file_operations ysf_fops={
    .owner=THIS_MODULE,
    //.llseek=NULL,
    .read=ysf_read,
    .write=ysf_write,
    //.ioctl=NULL,
    .open=ysf_open,
    .release=ysf_release,
};

//初始化程序
static int __init hello_init(void){    
    int ret=0;
    printk(KERN_ALERT "YSF: hello ysf init :) \n");

    //ret=alloc_chrdev_region(&dev,0,1,"ysf_md");       //动态分配
    
    dev=MKDEV(123,0);
    ret=register_chrdev_region(dev,1,"ysf_md");         //静态分配
    if(0!=ret){ 
        printk(KERN_ALERT "YSF: regester error. \n");
    }
    printk(KERN_ALERT "YSF: major_dev=%d \n",MAJOR(dev));
    printk(KERN_ALERT "YSF: minor_dev=%d \n",MINOR(dev));
    
    //分配字符设备内部结构体
    my_cdev=cdev_alloc();
    //设置成员函数
    my_cdev->owner=THIS_MODULE;
    my_cdev->ops=&ysf_fops;

    //添加设备到内核中
    ret=cdev_add(my_cdev,dev,1);
    if(0!=ret){
        printk(KERN_ALERT "YSF: cdev_add error. \n");
    }

    return 0;
}

//卸载程序
static void __exit hello_exit(void){
    printk(KERN_ALERT "YSF: byebye ysf exit :) \n");
    cdev_del(my_cdev);
    unregister_chrdev_region(dev,1);
}


module_init(hello_init);
module_exit(hello_exit);
