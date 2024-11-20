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
#define _GNU_SOURCE		/** 重要 */

#include <fcntl.h>
#include <sys/syscall.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <errno.h>
#include <string.h>
#include <limits.h>

#include "../../common/utils.h"

const char *filename = "name.txt";

void pr_cred(char *name)
{
	int rc = 0;
	uid_t ruid, euid, suid;
	gid_t rgid, egid, sgid;

	rc = getresuid(&ruid, &euid, &suid);
	if (rc == -1) {
		perror("getresuid error");
	}

	rc = getresgid(&rgid, &egid, &sgid);
	if (rc == -1) {
		perror("getresgid error");
	}

	pr_info("%6s: ruid:%d, euid:%d, suid:%d, rgid:%d, egid:%d, sgid:%d",
		name, ruid, euid, suid, rgid, egid, sgid);
}

void *func(void *args)
{
	char *name = ((char *)args);
	char buff[LINE_MAX] = { 0 };
	ssize_t size = 0;
	int rc = 0;
	int fd = 0;

	/** tid-1 使用底层系统调用修改 euid, 其他线程 euid 不受影响*/
	if (strcmp(name, "tid-1") == 0) {
		pr_cred(name);

		rc = syscall(__NR_setresgid, -1, 1000, -1);
		if (rc == -1) {
			perror("__NR_setresgid failed");
		}

		rc = syscall(__NR_setresuid, -1, 1000, -1);
		if (rc == -1) {
			perror("__NR_setresuid failed");
		}

		pr_info("%6s already modify self euid", name);

		pr_cred(name);
	}

	/** tid-2 使用 libc 库调用修改 euid, 其他线程 euid 受影响*/
	if (strcmp(name, "tid-2") == 0) {
		pr_cred(name);

		sleep(1);

		rc = setresgid(-1, 2000, -1);
		if (rc == -1) {
			perror("setresgid failed");
		}

		rc = setresuid(-1, 2000, -1);
		if (rc == -1) {
			perror("setresuid failed");
		}

		pr_info("%6s already modify all euid", name);

		pr_cred(name);
	}

	/** tid-3 监控自身 euid 的影响*/
	if (strcmp(name, "tid-3") == 0) {
		pr_cred(name);
		int i = 0;

		while (i < 20) {
			usleep(100000);
			pr_cred(name);
			i++;
		}
	}

	fd = open(filename, O_RDWR);
	if (fd == -1) {
		perror("open error");
		return NULL;
	}

	size = read(fd, buff, 1024);
	if (size < 0) {
		perror("read");
	} else {
		printf("%s: | %s", name, buff);
	}

	close(fd);
}

/** 只有该线程能读取 */
void *read_func(void *args)
{
	char *name = ((char *)args);
	char buff[LINE_MAX] = { 0 };
	ssize_t size = 0;
	int rc = 0;
	int fd = 0;

	fd = open(filename, O_RDWR);
	if (fd == -1) {
		perror("open error");
		return NULL;
	}

	size = read(fd, buff, 1024);
	if (size < 0) {
		perror("read");
	} else {
		printf("%s: | %s", name, buff);
	}

	pr_cred(name);

	close(fd);
}

int main(int argc, char const *argv[])
{
	pthread_t tid1, tid2, tid3, tid4;
	int rc = 0;
	int fd = 0;
	ssize_t size = 0;

	fd = open(filename, O_RDWR | O_CREAT, 0600);
	if (fd == -1) {
		perror("open error");
		return -errno;
	}

	size = write(fd, "This is test content\n", 22);
	if (size < 0) {
		perror("write");
	}

	close(fd);

	rc = pthread_create(&tid1, NULL, func, "tid-1");
	if (rc == -1) {
		perror("create tid-1 failed");
		return -errno;
	}

	rc = pthread_create(&tid2, NULL, func, "tid-2");
	if (rc == -1) {
		perror("create tid-2 failed");
		return -errno;
	}

	rc = pthread_create(&tid3, NULL, func, "tid-3");
	if (rc == -1) {
		perror("create tid-3 failed");
		return -errno;
	}

	rc = pthread_create(&tid4, NULL, read_func, "tid-4");
	if (rc == -1) {
		perror("create tid-4 failed");
		return -errno;
	}

	rc = pthread_join(tid1, NULL);
	if (rc == -1) {
		perror("join tid-1 failed");
		return -errno;
	}

	rc = pthread_join(tid2, NULL);
	if (rc == -1) {
		perror("join tid-2 failed");
		return -errno;
	}

	rc = pthread_join(tid3, NULL);
	if (rc == -1) {
		perror("join tid-3 failed");
		return -errno;
	}

	rc = pthread_join(tid4, NULL);
	if (rc == -1) {
		perror("join tid-4 failed");
		return -errno;
	}

	return 0;
}
