/*
 * @Author: Alan Yin
 * @Date: 2022-10-10 09:24:19
 * @LastEditTime: 2022-10-10 06:16:40
 * @LastEditors: Alan Yin
 * @FilePath: /kernel_read_user/vfs_operate_example.c
 * @Description: 内核读写磁盘文件。在用户态，即我们一般编写的C语言程序中，
 * 	可以使用open、close、read、write等系统调用对磁盘文件进行操作，那么在内核态呢？
 * 	同样有相应的函数可以使用，而这些函数却是open这些系统调用会使用到的，故而我们还是
 * 	在内核面向应用的层级进入，实现对磁盘文件的操作。我们用到的函数定义或原型信息，都在
 * 	include/linux/fs.h文件中有声明，分别在fs/open.c和fs/read_write.c文件中。
 *
 * 	ssize_t vfs_read(struct file *file, char __user *buf, size_t count, loff_t *pos);
 * 	ssize_t vfs_write(struct file *file, const char __user *buf, size_t count, loff_t *pos) ;
 *
 * 	https://www.jianshu.com/p/2cbd07e22a9f
 *
 * Copyright (c) 2022 by HT706, All Rights Reserved.
 */
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/kobject.h>
#include <linux/sysfs.h>
#include <linux/slab.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/string.h>

#define MARK_LABLE 	"[VFS_DEMO]"

#define __DEBUG__ 	1

#if (0 < __DEBUG__)
	#define debug_info_fmt(fmt)		MARK_LABLE "(%s:%d): " fmt
	#define debug_warn_fmt(fmt)		MARK_LABLE "(%s:%d): " fmt
	#define debug_info(fmt, ...)		printk(KERN_INFO  debug_info_fmt(fmt),__FILE__,__LINE__,##__VA_ARGS__)
	#define debug_warn(fmt, ...)		printk(KERN_ALERT debug_warn_fmt(fmt),__FILE__,__LINE__,##__VA_ARGS__)
#else
	#define debug_info(fmt, ...)
	#define debug_warn(fmt, ...)
#endif /*#define __DEBUG__*/

enum Action {
	ACTION_INIT = 0,
	ACTION_READ = 1,
	ACTION_WRITE = 2,
};

typedef enum Action action_t;

#define DEMO_VERSION	"V1.0.1-2022.10.10"

char *cfg_file = "/root/ysf/kernel_read_user/test.cfg";
static char write_buff[] = "Hello Alan Yin";

mm_segment_t oldfs;

static int vfs_demo_init(void){
	char buff[100];
	int size;

	loff_t pos;
	struct file *fp;
	action_t action = ACTION_INIT;

	debug_info("Driver init\n");

	fp = filp_open(cfg_file, O_RDWR, 0);
	if (IS_ERR(fp)) {
		debug_info("file '%s' not exist, create it\n", cfg_file);

		fp = filp_open(cfg_file, O_RDWR | O_CREAT, 0644);
		if (IS_ERR(fp)) {
			printk(KERN_ERR "create file error\n");
			return -1;
		}

		action = ACTION_WRITE;

		oldfs = get_fs();
		set_fs(KERNEL_DS);
		pos = 0;
		size = vfs_write(fp, write_buff, sizeof(write_buff), &pos);
		set_fs(oldfs);
		filp_close(fp, NULL);
	} else {
		action = ACTION_READ;

		oldfs = get_fs();
		set_fs(KERNEL_DS);

		pos = fp->f_pos;
		memset(buff, 0, sizeof(buff));

		size = vfs_read(fp, buff, sizeof(buff), &pos);

		set_fs(oldfs);
		filp_close(fp, NULL);
	}

	if (action == ACTION_WRITE)
		debug_info("init write '%s', content: %s, len: %ld\n", cfg_file, write_buff, sizeof(write_buff));
	else if (action == ACTION_READ)
		debug_info("read from '%s', content: %s, len: %ld\n", cfg_file, buff, strlen(buff));

	return 0;
}

static void vfs_demo_exit(void){
	debug_info("Driver remove\n");
}

module_init(vfs_demo_init);
module_exit(vfs_demo_exit);


/* 模块声明位于文件 kernel/include/linux/module.h 中 */
MODULE_AUTHOR("Alan Yin");
MODULE_VERSION(DEMO_VERSION);
MODULE_ALIAS("GPIO-FEIFEI");
MODULE_SOFTDEP("raid456");

/* 内核可以识别的许可证有GPL（任意版本GNU通用公共许可证）、GPL v2等, 列表如下 */
/*
 * The following license idents are currently accepted as indicating free
 * software modules
 *
 *      "GPL"                           [GNU Public License v2 or later]
 *      "GPL v2"                        [GNU Public License v2]
 *      "GPL and additional rights"     [GNU Public License v2 rights and more]
 *      "Dual BSD/GPL"                  [GNU Public License v2
 *                                       or BSD license choice]
 *      "Dual MIT/GPL"                  [GNU Public License v2
 *                                       or MIT license choice]
 *      "Dual MPL/GPL"                  [GNU Public License v2
 *                                       or Mozilla license choice]
 *
 * The following other idents are available
 *
 *      "Proprietary"                   [Non free products]
 *
 * There are dual licensed components, but when running with Linux it is the
 * GPL that is relevant so this is a non issue. Similarly LGPL linked with GPL
 * is a GPL combined work.
 *
 * This exists for several reasons
 * 1.   So modinfo can show license info for users wanting to vet their setup
 *      is free
 * 2.   So the community can ignore bug reports including proprietary modules
 * 3.   So vendors can do likewise based on their own policies
 */
MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("This is a Test.");