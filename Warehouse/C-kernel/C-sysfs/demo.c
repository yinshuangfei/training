#include <linux/module.h>
// #include <linux/kobject.h>
// #include <linux/sysfs.h>
// #include <linux/kobject.h>

static struct kobject *kobj;

int __init demo_init(void){
    printk(KERN_INFO "Alan Yin, demo_init\n");
    /* create attribute interface */
    kobj = kobject_create_and_add("filter", NULL);
    if ( !kobj ) {
        printk(KERN_ERR "Fail, create kobject for filter\n");
        return -1; 
    }   

    return 0;
}

void demo_exit(void)
{
    printk(KERN_INFO "Alan Yin, demo_exit\n");
    // dm_unregister_target(&filter_target);
    kobject_put(kobj);
}


module_init(demo_init);
module_exit(demo_exit);

MODULE_DESCRIPTION("Demo of exporting sysfs dir");
MODULE_AUTHOR("Alan Yin <yinshuangfei007@163.com>");
MODULE_LICENSE("GPL");