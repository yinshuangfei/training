/**
 * @file bubble_sort.c
 * @author Alan Yin (yinshuangfei007@163.com)
 * @brief
 * @version 0.0.1
 * @date 2025-01-18
 *
 * @copyright Copyright (c) 2025
 *
 * @node:
 */

#include "sort_alg.h"

/**
 * @brief 冒泡排序
 * 平均时间复杂度 O(n**2)
 * 最好时间复杂度 O(n)
 * 最坏时间复杂度 O(n**2)
 * 稳定
 * 空间复杂度 O(1)
 *
 * @param [out] name
 * @param [in out] data
 * @param [in] total
 */
void bubble_sort(char *name, long *data, long total)
{
	long i, j, tmp;

	sprintf(name, "%s", __func__);

	for (i = 0; i < total - 1; i++) {
		for (j = 0; j < total - 1 - i; j++) {
			if (data[j] > data[j + 1]) {
				tmp = data[j + 1];
				data[j + 1] = data[j];
				data[j] = tmp;
			}
		}
	}
}