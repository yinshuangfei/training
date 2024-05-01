#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/kobject.h>
#include <linux/sysfs.h>
#include <linux/slab.h>

#define __DEBUG__ 1

#if (0 < __DEBUG__)
	#define debug_info_fmt(fmt)		"[DC_GPIO] %s:%d "fmt
	#define debug_warn_fmt(fmt)		"[DC_GPIO] %s:%d "fmt
	#define debug_info(fmt, ...)	printk(KERN_INFO  debug_info_fmt(fmt),__FILE__,__LINE__,##__VA_ARGS__)
	#define debug_warn(fmt, ...)	printk(KERN_ALERT debug_warn_fmt(fmt),__FILE__,__LINE__,##__VA_ARGS__)
#else
	#define debug_info(fmt, ...)
	#define debug_warn(fmt, ...)
#endif /*#define __DEBUG__*/


static struct kobject * parent;
static struct kobject * child;
static struct kset * c_kset;


static unsigned long flag = 1;


static ssize_t attr_show(struct kobject *kobj, struct attribute *attr, char *buf){
	size_t count = 0;
	count += sprintf(&buf[count], "%lu\n", flag);
	return count;
}

static ssize_t attr_store(struct kobject *kobj, struct attribute *attr,
											  const char *buf, size_t count){
	flag = buf[0] - '0';
	return count;
}

static struct attribute child_att = {
	.name = "cldattr",
	.mode = S_IRUGO | S_IWUSR,
};

static const struct sysfs_ops attr_ops = {
	.show = attr_show,
	.store = attr_store,
};

static struct kobj_type cld_ktype = {
	.sysfs_ops = &attr_ops,
};


static int dc_gpio_init(void){
	debug_info("Driver init\n");
	int err;

	parent = kobject_create_and_add("gpio", NULL);

	child = kzalloc(sizeof(*child), GFP_KERNEL);
	if (!child)
		return -1;

	c_kset = kset_create_and_add("c_kset", NULL, parent);
	if(!c_kset)
		return -1;

	child->kset = c_kset;

	err = kobject_init_and_add(child, &cld_ktype, parent, "child_obj");
	if (err)
		return err;

	err = sysfs_create_file(child, &child_att);

	return 0;
}

static void dc_gpio_exit(void){
	sysfs_remove_file(child, &child_att);
	kset_unregister(c_kset);

	kobject_del(child);
	kobject_del(parent);
}

module_init(dc_gpio_init);
module_exit(dc_gpio_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Alan Yin@7_StorageGroup");
MODULE_DESCRIPTION("This is a FT-S2500 Double-Controller disk array \
GPIO driver. It control the GPIO via SMC.");


