#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/jiffies.h>

void m1_print_jiffies(void) {
	printk("ysf, jiffies is %llu\n", (u64)jiffies);
}
EXPORT_SYMBOL(m1_print_jiffies);

static void 
test_dump(void);


static void 
test_dump(void) {
	dump_stack();
	m1_print_jiffies();
}


static int __init hello_init(void){
	printk(KERN_INFO "YSF: hello m1 init :) \n");
	test_dump();
	return 0;
}

static void __exit hello_exit(void){
	printk(KERN_INFO "YSF: hello m1 exit :) \n");
}

module_init(hello_init);
module_exit(hello_exit);

MODULE_LICENSE("GPL");
