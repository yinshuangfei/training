/**
 * @file shell_sort.c
 * @author Alan Yin (yinshuangfei007@163.com)
 * @brief
 * @version 0.0.1
 * @date 2025-01-15
 *
 * @copyright Copyright (c) 2025
 *
 * @node:
 */

#include "sort_alg.h"

/**
 * @brief 希尔排序
 * 平均时间复杂度 O(n**1.3)
 * 最好时间复杂度 O(n)
 * 最坏时间复杂度 O(n**2)
 * 空间复杂度 O(1)
 *
 * 先将整个待排序的记录序列分割成为若干子序列分别进行直接插入排序，待整个序列中的记录
 * “基本有序”时，再对全体记录进行依次直接插入排序。
 *
 * @param [out] name
 * @param [in out] data
 * @param [in] total
 */
void shell_sort(char *name, long *data, long total)
{
	long i, j, step, tmp;

	sprintf(name, "%s", __func__);

	// for (long i = 0; i < total; i++) {
	// 	pr_info("-- [%8d] %ld", i + 1, data[i]);
	// }

	/** 步长一点一点缩小至 1 */
	for (step = total / 2; step > 0; step = step / 2) {
		for (i = step; i < total; i++) {
			tmp = data[i];

			j = i - step;
			while (j >= 0 && tmp < data[j]) {
				data[j + step] = data[j];
				j = j - step;
			}
			data[j + step] = tmp;
		}
	}

	// for (i = 0; i < total; i++) {
	// 	pr_info("=> [%8d] %ld", i + 1, data[i]);
	// }
}