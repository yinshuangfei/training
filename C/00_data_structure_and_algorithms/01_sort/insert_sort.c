/**
 * @file insert_sort.c
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
 * @brief 插入排序
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
void insert_sort(char *name, long *data, long total)
{
	long i, j, tmp;

	sprintf(name, "%s", __func__);

	/**
	 *  有序区       无序区
	 * |_2_3_|_4_1_3_4__..._____|
	 * i 指向无序区的第一个元素（初始时为第二个元素）
	 * */
	for (i = 1; i < total; i++) {
		tmp = data[i];

		/**
		 * j 指向有序区的最后一个元素, data[j] > tmp 控制增序还是降序.
		 * 以下方法二选一
		 * for (j = i - 1; j >= 0 && tmp < data[j]; j--)
		 *      data[j + 1] = data[j];
		 * */
		j = i - 1;
		while (j >= 0 && tmp < data[j]) {
			data[j + 1] = data[j];
			j--;
		}

		data[j + 1] = tmp;
	}
}