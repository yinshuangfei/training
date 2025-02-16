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
 * 不稳定
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

	/**
	 * - 步长一点一点缩小至 1
	 * - 将大序列拆分成多个小序列进行排序
	*/
	for (step = total / 2; step > 0; step = step / 2) {
		/**
		 * - 内部排序方法使用插入排序
		 * - 单次排序的元素集合为相隔为 step 的各个分组，相隔为 step 的集合为
		 *   一个分组，共有 step 个分组，分组内进行插入排序
		 * - step 为几，就分为几个组，i++ 的时候，就在几个分组间交错进行排序
		 */
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
}