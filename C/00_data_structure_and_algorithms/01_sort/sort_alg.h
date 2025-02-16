/**
 * @file sort_alg.h
 * @author Alan Yin (yinshuangfei007@163.com)
 * @brief
 * @version 0.0.1
 * @date 2025-01-15
 *
 * @copyright Copyright (c) 2025
 *
 * @node:
 */

#ifndef __SORT_ALG__
#define __SORT_ALG__

#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>

#include <string.h>
#include <limits.h>
#include <errno.h>

#include "../../common/utils.h"

/**
 * @brief
 * https://blog.csdn.net/qing_tong_tong/article/details/135760029
 */

typedef void(*sort_fn)(char *name, long *data, long total);

void insert_sort(char *name, long *data, long total);
void shell_sort(char *name, long *data, long total);
void bubble_sort(char *name, long *data, long total);

static inline void check_sort(long *data, long total)
{
	for (long i = 0; i < total; i++) {
		if (data[i] > data[i + 1] && (i + 1 < total)) {
			pr_err("!!! error !!!");
		}
	}
}

#endif /** __SORT_ALG__ */