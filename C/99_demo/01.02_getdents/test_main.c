#define _GNU_SOURCE

#include <dirent.h>     /* Defines DT_* constants */
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/syscall.h>

/**
 * man getdents64
*/
#define handle_error(msg) \
	do { perror(msg); exit(EXIT_FAILURE); } while (0)

struct linux_dirent {
	long           d_ino;     /* Inode number */
	off_t          d_off;     /* Offset to next linux_dirent */
	unsigned short d_reclen;  /* Length of this linux_dirent */
	char           d_name[];  /* Filename (null-terminated) */
				  /* length is actually (d_reclen - 2 -
				     offsetof(struct linux_dirent, d_name)) */
	/*
	char           pad;       // Zero padding byte
	char           d_type;    // File type (only since Linux
				  // 2.6.4); offset is (d_reclen - 1)
	*/
};

struct linux_dirent64 {
	ino64_t        d_ino;    /* 64-bit inode number */
	off64_t        d_off;    /* 64-bit offset to next structure */
	unsigned short d_reclen; /* Size of this dirent */
	unsigned char  d_type;   /* File type */
	char           d_name[]; /* Filename (null-terminated) */
};

#define DIRENT64

#define BUF_SIZE 1024

int main(int argc, char *argv[])
{
	int fd, nread;
	char buf[BUF_SIZE];
#ifdef DIRENT64
	struct linux_dirent64 *d;
#else
	struct linux_dirent *d;
#endif
	int bpos;
	char d_type;

	fd = open(argc > 1 ? argv[1] : ".", O_RDONLY | O_DIRECTORY);
	if (fd == -1)
		handle_error("open");

	for ( ; ; ) {
#ifdef DIRENT64
		printf("[INFO] getdents64, mix size: %d\n",
			sizeof(struct linux_dirent64));
		nread = syscall(SYS_getdents64, fd, buf, BUF_SIZE);
#else
		printf("[INFO] getdents, mix size: %d\n",
			sizeof(struct linux_dirent));
		nread = syscall(SYS_getdents, fd, buf, BUF_SIZE);
#endif
		if (nread == -1)
			handle_error("getdents");

		if (nread == 0)
			break;

		printf("--------------- nread=%d ---------------\n", nread);
		printf("inode#    file type  d_reclen  d_off   d_name\n");

		for (bpos = 0; bpos < nread;) {
#ifdef DIRENT64
			d = (struct linux_dirent64 *) (buf + bpos);
			d_type = d->d_type;
#else
			d = (struct linux_dirent *) (buf + bpos);
			d_type = *(buf + bpos + d->d_reclen - 1);
#endif
			printf("%8ld  ", d->d_ino);
			printf("%-10s ", (d_type == DT_REG)  ?  "regular" :
					 (d_type == DT_DIR)  ?  "directory" :
					 (d_type == DT_FIFO) ?  "FIFO" :
					 (d_type == DT_SOCK) ?  "socket" :
					 (d_type == DT_LNK)  ?  "symlink" :
					 (d_type == DT_BLK)  ?  "block dev" :
					 (d_type == DT_CHR)  ?  "char dev" : "???");
			printf("%4d %10lld  %s\n", d->d_reclen,
				(long long)d->d_off, d->d_name);
			bpos += d->d_reclen;
		}
	}

	exit(EXIT_SUCCESS);
}
