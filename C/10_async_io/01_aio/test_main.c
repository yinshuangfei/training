/**
 * @file test_main.c
 * @author Alan Yin (yinshuangfei007@163.com)
 * @brief
 * @version 0.0.1
 * @date 2025-10-01
 *
 * @copyright Copyright (c) 2025
 *
 * @node: POSIX AIO (POSIX Asynchronous I/O), AIO 用法
 */

#include <aio.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <errno.h>
#include <fcntl.h>

#include "../../common/utils.h"

#define BUFF_MAX 1024

int aio_func(int fd, void *buff, size_t size, off_t offset)
{
	struct aiocb aiocb;
	int rc = 0;
	ssize_t result;

	memset(&aiocb, 0, sizeof(aiocb));

	aiocb.aio_fildes = fd;
	aiocb.aio_buf = buff;
	aiocb.aio_nbytes = size;
	aiocb.aio_offset = offset;

	rc = aio_write(&aiocb);
	if (-1 == rc) {
		pr_err("aio_write failed, %s (%d)", strerror(errno), errno);
		return -errno;
	}

	// 继续做其他事
	pr_info("do other things");

	// 等待完成
	while (aio_error(&aiocb) == EINPROGRESS) {
		// 可做其他事或短暂休眠
		pr_info("waitting ...");
	}

	/**
	 * If the asynchronous I/O operation has not yet completed, the return
	 * value and effect of aio_return() are undefined.
	 */
	result = aio_return(&aiocb);
	if (-1 == result) {
		pr_err("aio_return with error, %s (%d)",
			strerror(errno), errno);
		return -errno;
	}

	return rc;
}

int main(int argc, char *argv[])
{
	if (argc != 2) {
		fprintf(stderr, "Usage: %s <destination>\n", argv[0]);
		return -EINVAL;
	}

	char *filename = argv[1];
	char buff[BUFF_MAX] = { "abcdefg" };
	int fd = -1;

	fd = open(filename, O_RDWR | O_CREAT, 0644);
	if (-1 == fd) {
		pr_err("open failed, %s (%d)", strerror(errno), errno);
		return -errno;
	}

	aio_func(fd, (void *)buff, BUFF_MAX, 0);

	close(fd);

	pr_info("%s is not alive", argv[1]);

	return 0;
}
