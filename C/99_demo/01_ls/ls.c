/*
 * @Author: Alan Yin
 * @Date: 2023-07-22 19:38:08
 * @LastEditTime: 2024-06-27 22:54:22
 * @LastEditors: Alan Yin
 * @FilePath: /training/C/99_demo/01_ls/ls.c
 * @Description:
 * // -*- mode:C; tab-width:8; c-basic-offset:2; indent-tabs-mode:t -*-
 * // vim: ts=8 sw=2 smarttab
 * Copyright (c) 2024 by Alan Yin, All Rights Reserved.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <errno.h>

#include "../../common/utils.h"

int main (int argc, char *argv[])
{
	DIR *dp;
	struct dirent *dirp;
	char *dir = NULL;

	if (argc < 2) {
		pr_info("args number (%d) < 2, list current dir", argc);
		dir = ".";
	} else {
		dir = argv[1];
	}

	if ((dp = opendir(dir)) == NULL) {
		pr_err("opendir '%s' failed (%d:%s)", dir,
			errno, strerror(errno));
		return -errno;
	}

	while ((dirp = readdir(dp)) != NULL) {
		printf("%2d %s\n", dirp->d_type, dirp->d_name);
	}

	closedir(dp);

	exit(EXIT_SUCCESS);
}
