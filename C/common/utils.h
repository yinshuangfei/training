/*
 * @Author: Alan Yin
 * @Date: 2024-04-20 23:35:55
 * @LastEditTime: 2024-06-27 20:53:18
 * @LastEditors: Alan Yin
 * @FilePath: /training/C/common/utils.h
 * @Description:
 * // -*- mode:C; tab-width:8; c-basic-offset:2; indent-tabs-mode:t -*-
 * // vim: ts=8 sw=2 smarttab
 * Copyright (c) 2024 by Alan Yin, All Rights Reserved.
 */

#ifndef YSF_UTILS_H
#define YSF_UTILS_H

#include <stdio.h>

#define ARRAY_SIZE(array) (sizeof(array) / sizeof(array[0]))

#define pr(format, ...)							\
	fprintf(stdout, "[DEBUG] (%s:%s:%d): "format"\n",		\
		__FILE__, __FUNCTION__, __LINE__, ##__VA_ARGS__)

#define pr_info(format, ...)						\
	fprintf(stdout, "[INFO] "format"\n",				\
		##__VA_ARGS__)

#define pr_warn(format, ...)						\
	fprintf(stdout, "[WARN] "format"\n",				\
		##__VA_ARGS__)

#define pr_err(format, ...)						\
	fprintf(stderr, "[ERR] "format"\n",				\
		##__VA_ARGS__)

#define SAFE_FREE(ptr)							\
	if (ptr) {							\
		free(ptr);						\
		ptr = NULL;						\
	}

#endif /** YSF_UTILS_H */