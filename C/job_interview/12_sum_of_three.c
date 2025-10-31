/**
 * @file 11_sum_of_two.c
 * @author Alan Yin (yinshuangfei007@163.com)
 * @brief
 * @version 0.0.1
 * @date 2025-05-11
 *
 * @copyright Copyright (c) 2025
 *
 * @node: 三数之和是一个经典的面试题，题目要求：
 * 给定一个整数数组，找出所有不重复的三元组，使得三个数的和为0。
 * (1) 双指针解法（推荐）
 * - 这是最优解法，时间复杂度O(n²)，空间复杂度O(1);
 * (2) 暴力解法（不推荐）
 * - 时间复杂度O(n³)，仅用于理解问题;
 */

#include <stdio.h>
#include <stdlib.h>

#include <stdio.h>
#include <stdlib.h>

int compare(const void* a, const void* b)
{
	return (*(int*)a - *(int*)b);
}

/**
 * @brief
 *
 * @param [in] nums 数组
 * @param [in] numsSize 数组大小
 * @param [in] result 结果集合数组地址（二维指针）
 * @param [in] returnSize 结果集合的数组个数
 */
void threeSum(int* nums, int numsSize, int** result, int* returnSize)
{
	/**
	 * 先排序数组
	 * C语言标准库中的qsort函数使用的是‌快速排序算法
	 **/
	qsort(nums, numsSize, sizeof(int), compare);

	/**
	 * numsSize * 3 看做一个结果，最多存储 numsSize 个结果
	 */
	*result = (int*)malloc(sizeof(int) * numsSize * numsSize * 3);
	*returnSize = 0;

	for (int i = 0; i < numsSize - 2; i++) {
		/** 跳过重复元素 */
		if (i > 0 && nums[i] == nums[i - 1])
			continue;

		/** 指定双指针 */
		int left = i + 1;
		int right = numsSize - 1;

		while (left < right) {
			int sum = nums[i] + nums[left] + nums[right];

			if (sum == 0) {
				/** 找到有效三元组 */
				(*result)[(*returnSize) * 3] = nums[i];
				(*result)[(*returnSize) * 3 + 1] = nums[left];
				(*result)[(*returnSize) * 3 + 2] = nums[right];
				(*returnSize)++;

				/** 跳过重复元素 */
				while ((left < right) &&
				       (nums[left] == nums[left + 1])) {
					left++;
				}

				while ((left < right) &&
				       (nums[right] == nums[right - 1])) {
					right--;
				}

				left++;
				right--;
			} else if (sum < 0) {
				left++;
			} else {
				right--;
			}
		}
	}
}

int main()
{
	int nums[] = {-1, 0, 1, 2, -1, -4};
	int numsSize = sizeof(nums) / sizeof(nums[0]);

	int* result;
	int returnSize;

	threeSum(nums, numsSize, &result, &returnSize);

	printf("Found %d triplets:\n", returnSize);
	for (int i = 0; i < returnSize; i++) {
		printf("[%d, %d, %d]\n",
			result[i * 3],
			result[i * 3 + 1],
			result[i * 3 + 2]);
	}

	free(result);
	// free(returnColumnSizes);

	return 0;
}
