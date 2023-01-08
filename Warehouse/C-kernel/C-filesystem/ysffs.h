#ifndef YSF_FS
#define YSF_FS
#include <linux/string.h>
#include <linux/uaccess.h>
#include <linux/slab.h>
#include <linux/buffer_head.h>
#include <linux/fs.h>
#include <linux/kernel.h>

#define YSFFS_MAGIC                 0x73616d77
#define YSFFS_BLOCKSIZE             4096
#define YSFFS_N_BLOCKS              10
#define YSFFS_INODE_TABLE_START_IDX 4
#define YSFFS_ROOT_INODE_NUM        0
#define YSFFS_FILENAME_MAX_LEN      256
#define RESERVE_BLOCKS              2 //dummy and sb


struct ysffs_super_block {
	uint64_t version;
	uint64_t magic;
	uint64_t block_size;
	uint64_t inodes_count;
	uint64_t free_blocks;
	uint64_t blocks_count;

	uint64_t bmap_block;
	uint64_t imap_block;
	uint64_t inode_table_block;
	uint64_t data_block_number;
	char padding[4016];
};


struct ysffs_inode {
	mode_t mode; //sizeof(mode_t) is 4
	uint64_t inode_no;
	uint64_t blocks;
	uint64_t block[YSFFS_N_BLOCKS];
	union {
		uint64_t file_size;
		uint64_t dir_children_count;
	};
    int32_t i_uid; 
    int32_t i_gid;
    int32_t i_nlink;
    int64_t i_atime;
    int64_t i_mtime;
    int64_t i_ctime;
    char padding[112];
};

//inode_map anf block_map
int set_and_save_imap(struct super_block* sb, uint64_t inode_num, uint8_t value);

//inode operations
int ysffs_get_inode(struct super_block* sb, uint64_t inode_no, struct ysffs_inode* raw_inode);

// super block operations
int save_super(struct super_block* sb);
int ysffs_fill_super(struct super_block *sb, void *data, int silent);
//int ysffs_write_inode(struct inode *inode, struct writeback_control *wbc);
void ysffs_evict_inode(struct inode *vfs_inode);

// file system operations
static struct dentry *ysffs_mount(struct file_system_type *fs_type,
        int flags, const char *dev_name, void *data);
static void ysffs_kill_sb(struct super_block *sb);

#endif 
