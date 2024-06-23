/*
 * @Author: Alan Yin
 * @Date: 2024-05-11 21:56:48
 * @LastEditTime: 2024-05-11 22:06:53
 * @LastEditors: Alan Yin
 * @FilePath: /myOS/test/test_pwrite.c
 * @Description:
 * // -*- mode:C; tab-width:8; c-basic-offset:2; indent-tabs-mode:t -*-
 * // vim: ts=8 sw=2 smarttab
 * Copyright (c) 2024 by Alan Yin, All Rights Reserved.
 */

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>

int main(void)
{
	char *filename = "xxx_test_file";
	char *data = "this is a toy.\n";
	int rc, fd;

	rc = open(filename, O_CREAT | O_RDWR | O_APPEND, 0644);
	if (rc == -1) {
		printf("open err (%d:%s)\n", errno, strerror(errno));
		return -1;
	}

	fd = rc;

	ftruncate(fd, 0);

	pwrite(fd, "this is a toy1\n", strlen(data), 0);
	pwrite(fd, "this is a toy2\n", strlen(data), 0);
	pwrite(fd, "this is a toy3\n", strlen(data), 0);
	pwrite(fd, "this is a toy4\n", strlen(data), 0);
	pwrite(fd, "this is a toy5\n", strlen(data), 0);

	close(fd);

	return 0;
}