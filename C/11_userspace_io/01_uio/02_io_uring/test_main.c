/**
 * @file test_main.c
 * @author Alan Yin (yinshuangfei007@163.com)
 * @brief
 * @version 0.0.1
 * @date 2025-10-01
 *
 * @copyright Copyright (c) 2025
 *
 * @node: Linux Native AIO (io_uring), io_uring 用法.
 * io_uring 是 Linux 5.1 引入的现代高性能异步 I/O 框架
 */

#include <aio.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <errno.h>

#include "../../common/utils.h"

int main(int argc, char *argv[])
{
	if (argc != 2) {
		fprintf(stderr, "Usage: %s <destination>\n", argv[0]);
		return -EINVAL;
	}

	pr_info("%s is not alive", argv[1]);

	return 0;
}
