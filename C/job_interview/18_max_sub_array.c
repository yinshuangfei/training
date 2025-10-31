/**
 * @file 18_max_sub_array.c
 * @author Alan Yin (yinshuangfei007@163.com)
 * @brief
 * @version 0.0.1
 * @date 2025-05-14
 *
 * @copyright Copyright (c) 2025
 *
 * @node:
 */

#include <stdio.h>

/**
 * @brief 动态规划解法（最优解）
 *
 * @param [in out] nums
 * @param [in out] numsSize
 * @return int 0: success; <0: failed, posix errno;
 */
int maxSubArray_dp(int *nums, int numsSize)
{
	int max_sum = nums[0];
	int current_sum = nums[0];

	for (int i = 1; i < numsSize; i++) {
		current_sum = (current_sum > 0) ?
			      current_sum + nums[i] : nums[i];
		if (current_sum > max_sum) {
			max_sum = current_sum;
		}
	}

	return max_sum;
}

/**
 * @brief 贪心算法解法
 *
 * @param [in out] nums
 * @param [in out] numsSize
 * @return int 0: success; <0: failed, posix errno;
 */
int maxSubArray_tx(int* nums, int numsSize)
{
	int max_sum = nums[0];
	int current_sum = nums[0];

	for (int i = 0; i < numsSize; i++) {
		current_sum += nums[i];

		if (current_sum > max_sum) {
			max_sum = current_sum;
		}

		if (current_sum < 0) {
			current_sum = 0;
		}
	}

	return max_sum;
}

int main()
{
	int nums[] = {-2,1,-3,4,-1,2,1,-5,4};
	int size = sizeof(nums)/sizeof(nums[0]);
	printf("最大子序和: %d\n", maxSubArray_dp(nums, size)); // 输出6
	printf("最大子序和: %d\n", maxSubArray_tx(nums, size)); // 输出6
	return 0;
}
