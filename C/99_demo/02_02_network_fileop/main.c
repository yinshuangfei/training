/*
 * @Author: Alan Yin
 * @Date: 2024-06-06 13:45:57
 * @LastEditTime: 2024-06-06 14:13:37
 * @LastEditors: Alan Yin
 * @FilePath: /nfs_gap/network_fileop/main.c
 * @Description:
 * // -*- mode:C; tab-width:8; c-basic-offset:2; indent-tabs-mode:t -*-
 * // vim: ts=8 sw=2 smarttab
 * Copyright (c) 2024 by Alan Yin, All Rights Reserved.
 */
#define _GNU_SOURCE		// 重要

#include <fcntl.h>
#include <sys/syscall.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

/**
 * @description: 态分配足够空间的 file_handle 结构体
 * @param {unsigned int} handle_bytes
 * @return {*}
 */
struct file_handle *allocate_file_handle(unsigned int handle_bytes)
{
	struct file_handle *fh = malloc(sizeof(struct file_handle) +
					handle_bytes);
	if (!fh) {
		perror("malloc");
		return NULL;
	}
	fh->handle_bytes = handle_bytes;
	// 通常为 0，除非有特定需求
	fh->handle_type = 0;
	return fh;
}

/**
 * @description: 文件名转换为句柄
 * @return {*}
 */
int get_handle_for_path(const char *pathname, struct file_handle **handle_ptr,
			int *mount_id)
{
	// 使用当前工作目录的文件描述符
	int dirfd = AT_FDCWD;
	// 假设句柄的最大长度为128字节（这只是示例，实际长度可能不同）
	unsigned int max_handle = 128;
	struct file_handle *handle = allocate_file_handle(max_handle);
	if (!handle) {
		return -1;
	}

	// 调用 name_to_handle_at 系统调用
	// int ret = syscall(__NR_name_to_handle_at, dirfd, pathname, handle,
	// 		  mount_id, 0);
	// 或者调用 API
	int ret = name_to_handle_at(dirfd, pathname, handle, mount_id, 0);
	if (ret == -1) {
		free(handle);
		perror("name_to_handle_at");
		return -1;
	}

	// 返回句柄的指针
	*handle_ptr = handle;
	return 0;
}

/**
 * @description: 用 open_by_handle_at 打开文件
 * @param {file_handle} *handle
 * @param {int} mount_id
 * @return {*}
 */
int open_file_by_handle(const struct file_handle *handle, int mount_id)
{
	 // 使用当前工作目录的文件描述符
	int dirfd = AT_FDCWD;

	// 调用 open_by_handle_at 系统调用
	// int fd = syscall(__NR_open_by_handle_at, dirfd, handle, O_RDONLY);
	// 或者调用 API
	int fd = open_by_handle_at(dirfd, handle, O_RDONLY);
	if (fd == -1) {
		perror("open_by_handle_at");
		return -1;
	}

	return fd;
}

int main(int argc, char const *argv[])
{

	const char *pathname = "./name.txt";
	struct file_handle *handle = NULL;
	int mount_id;

	if (get_handle_for_path(pathname, &handle, &mount_id) != 0) {
		free(handle);
		exit(EXIT_FAILURE);
	}

	// 现在你可以使用 handle 和 mount_id 来通过 open_by_handle_at 打开文件
	// 使用句柄打开文件
	int fd = open_file_by_handle(handle, mount_id);
	if (fd == -1) {
		free(handle);
		exit(EXIT_FAILURE);
	}

	char buff[1024];
	int rc = 0;
	rc = read(fd, buff, 1024);
	if (rc < 0) {
		perror("read");
	} else {
		printf("size:%d, \n%s\n", rc, buff);
	}

	close(fd);
	free(handle);
	return 0;
}
