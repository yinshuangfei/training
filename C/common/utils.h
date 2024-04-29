/*
 * @Author: Alan Yin
 * @Date: 2024-04-20 23:35:55
 * @LastEditTime: 2024-04-21 09:44:55
 * @LastEditors: Alan Yin
 * @FilePath: /self_project/03_thread_pool/utils.h
 * @Description:
 * // -*- mode:C; tab-width:8; c-basic-offset:2; indent-tabs-mode:t -*-
 * // vim: ts=8 sw=2 smarttab
 * Copyright (c) 2024 by HT706, All Rights Reserved.
 */

#ifndef YSF_UTILS_H
#define YSF_UTILS_H

#include <stdio.h>

#define pr(format, ...)					\
	printf("[DEBUG] (%s:%s:%d): "format"\n",	\
	__FILE__, __FUNCTION__, __LINE__, ##__VA_ARGS__)

#define pr_info(format, ...)				\
	printf("[INFO] : "format"\n",			\
	##__VA_ARGS__)

#define pr_err(format, ...)				\
	printf("[ERR] : "format"\n",			\
	##__VA_ARGS__)

#define SAFE_FREE(ptr)					\
	if (ptr) {					\
		free(ptr);				\
		ptr = NULL;				\
	}

#endif /** YSF_UTILS_H */