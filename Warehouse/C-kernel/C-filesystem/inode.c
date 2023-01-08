#include <linux/time.h>
#include "ysffs.h"

void ysffs_evict_inode(struct inode *vfs_inode)
{
    struct super_block *sb = vfs_inode->i_sb;
    printk(KERN_INFO "ysffs evict: Clearing inode [%lu]\n", vfs_inode->i_ino);
    
    truncate_inode_pages_final(&vfs_inode->i_data);
    clear_inode(vfs_inode);
    if (vfs_inode->i_nlink)
    {
        printk(KERN_INFO "ysffs evict: Inode [%lu] still has links\n", vfs_inode->i_ino);
        return;
    }
    printk(KERN_INFO "ysffs evict: Inode [%lu] has no links!\n", vfs_inode->i_ino);
    set_and_save_imap(sb, vfs_inode->i_ino, 0);
    return;
}

int set_and_save_imap(struct super_block* sb, uint64_t inode_num, uint8_t value){

}

int ysffs_get_inode(struct super_block *sb,
		      uint64_t inode_no, struct ysffs_inode *raw_inode)
{
	if (!raw_inode) {
		printk(KERN_ERR "inode is null");
		return -1;
	}
	struct ysffs_super_block *H_sb = sb->s_fs_info;
	struct ysffs_inode *H_inode_array = NULL;

	int i;
	struct buffer_head *bh;
	bh = sb_bread(sb,
		      H_sb->inode_table_block
		      + inode_no * sizeof(struct ysffs_inode) / YSFFS_BLOCKSIZE);
	printk(KERN_INFO "H_sb->inode_table_block is %lld",
	       H_sb->inode_table_block);
	BUG_ON(!bh);
	//TODO
	H_inode_array = (struct ysffs_inode *)bh->b_data;
	int idx = inode_no % (YSFFS_BLOCKSIZE / sizeof(struct ysffs_inode));
	ssize_t inode_array_size = YSFFS_BLOCKSIZE / sizeof(struct ysffs_inode);
	if (idx > inode_array_size) {
		printk(KERN_ERR "in get_inode: out of index");
		return -1;
	}
	memcpy(raw_inode, H_inode_array + idx, sizeof(struct ysffs_inode));
	if (raw_inode->inode_no != inode_no) {
		printk(KERN_ERR "inode not init");
	}
	return 0;
}
