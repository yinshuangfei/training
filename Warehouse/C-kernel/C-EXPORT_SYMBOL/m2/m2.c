#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>

extern void m1_print_jiffies(void);

static int __init hello_init(void){
	printk(KERN_INFO "YSF: hello m2 init :) \n");
	m1_print_jiffies();
	
	return 0;
}

static void __exit hello_exit(void){
	printk(KERN_INFO "YSF: hello m2 exit :) \n");
}

module_init(hello_init);
module_exit(hello_exit);

MODULE_LICENSE("GPL");
