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
#include <errno.h>
#include <string.h>

#define ARRAY_SIZE(array) (sizeof(array) / sizeof(array[0]))

/**
 * 输出颜色：
 * echo -e "\e[31m红色文本\e[0m"
 * echo -e "\e[32m绿色文本\e[0m"
 * echo -e "\e[33m黄色文本\e[0m"
 * echo -e "\e[34m蓝色文本\e[0m"
 * echo -e "\e[35m紫色文本\e[0m"
 * echo -e "\e[36m青色文本\e[0m"
 * echo -e "\e[37m白色文本\e[0m"
*/
#define ENTRY_PREFIX	"\e[35m[ENTRY ----->]\e[0m"
#define DEBUG_PREFIX	"\e[34m[DEBUG]\e[0m"
#define INFO_PREFIX	"\e[32m[INFO]\e[0m"
#define WARN_PREFIX	"\e[33m[WARN]\e[0m"
#define ERR_PREFIX	"\e[31m[ERR]\e[0m"

#define pr_entry()							\
	do {								\
		fprintf(stdout, ENTRY_PREFIX				\
			" ========>>> [   %-20s   ] <<<========\n",	\
			__FUNCTION__);					\
	} while (0)

#define pr(format, ...)							\
	do {								\
		fprintf(stdout, DEBUG_PREFIX" (%s:%d): "format"\n",	\
			__FUNCTION__, __LINE__, ##__VA_ARGS__);		\
	} while (0)

#define pr_dbg pr

#define pr_info(format, ...)						\
	do {								\
		fprintf(stdout, INFO_PREFIX" (%s:%d): "format"\n",	\
			__FUNCTION__, __LINE__, ##__VA_ARGS__);		\
	} while (0)

#define pr_warn(format, ...)						\
	do {								\
		fprintf(stdout, WARN_PREFIX" (%s:%d): "format"\n",	\
			__FUNCTION__, __LINE__, ##__VA_ARGS__);		\
	} while (0)

#define pr_err(format, ...)						\
	do {								\
		fprintf(stderr, ERR_PREFIX" (%s:%d): "format"\n",	\
			__FUNCTION__, __LINE__, ##__VA_ARGS__);		\
	} while (0)

#define pr_stdwarn(format, ...)						\
	do {								\
		fprintf(stdout, WARN_PREFIX" (%s:%d): "format" (%d:%s)\n",\
			__FUNCTION__, __LINE__, ##__VA_ARGS__,		\
			errno, strerror(errno));			\
	} while (0)

#define pr_stderr(format, ...)						\
	do {								\
		fprintf(stderr, ERR_PREFIX" (%s:%d): "format" (%d:%s)\n",\
			__FUNCTION__, __LINE__, ##__VA_ARGS__,		\
			errno, strerror(errno));			\
	} while (0)

#define SAFE_FREE(ptr)							\
	if (ptr) {							\
		free(ptr);						\
		ptr = NULL;						\
	}

#endif /** YSF_UTILS_H */