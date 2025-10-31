/**
 * @file test_main.c
 * @author Alan Yin (yinshuangfei007@163.com)
 * @brief
 * @version 0.0.1
 * @date 2025-05-08
 *
 * @copyright Copyright (c) 2025
 *
 * @node: 寻找旋转排序数组中的目标值
 *
 * 主要思路，先分段，再判断是否有序，接着判断是否在范围内
 *
 * 放弃左边时，去除边界值
 *        断崖         断崖
 *         |            |
 * 3 5 6 7 9 0 1 2  ->  9 0 1 2  ->  9    0  ->  0
 * l     m       r      l m   r      l[m] r     l[m,r]
 *
 */

#include <stdlib.h>
#include <stdio.h>

/**
 * @brief 查找目标值（推荐）
 *
 * 任意旋转有效
 *
 * @param [in] nums
 * @param [in] numsSize
 * @param [in] target
 * @return int 0: success; <0: failed, posix errno;
 */
int search1(int *nums, int numsSize, int target)
{
	int left = 0, right = numsSize - 1;

	/**
	 * 边界条件至关重要
	*/
	while (left <= right) {
		int mid = left + (right - left) / 2;

		if (nums[mid] == target)
			return mid;

		/** 左半部分有序 */
		if (nums[left] <= nums[mid]) {
			/** 在范围内 */
			if (nums[left] <= target && target < nums[mid])
				right = mid - 1;
			/** 不在范围内 */
			else
				left = mid + 1;
		}
		/** 右半部分有序 */
		else {
			/** 在范围内 */
			if (nums[mid] < target && target <= nums[right])
				left = mid + 1;
			/** 不在范围内 */
			else
				right = mid - 1;
		}
	}

	return -1;
}

/**
 * @brief 查找目标值
 *
 * 任意旋转有效
 *
 * @param [in] arr
 * @param [in] len
 * @param [in] target
 * @return int 0: success; <0: failed, posix errno;
 */
int search2(int arr[], int len, int target)
{
	int left = 0, right = len - 1;

	if (len == 0)
		return -1;

	while (left < right) {
		int mid = left + (right - left) / 2;

		if (arr[left] < arr[mid] &&
		    (arr[left] <= target && target <= arr[mid])) {
			/** 左侧递增，目标值在范围内 */
			right = mid;	/** 接受左侧，保留边界 */
		} else if (arr[left] > arr[mid] &&
		           (arr[left] <= target || target <= arr[mid])) {
			/** 左侧螺旋，目标值在范围内 */
			right = mid;	/** 接受左侧，保留边界 */
		} else {
			/** 接受右边时，去除边界值 */
			left = mid + 1;
		}

		if (arr[left] == target)
			return left;
	}

	return -1;
}

int main(int argc, char *argv[])
{
	int arr1[] = {4, 5, 1, 2, 3};
	int arr2[] = {3, 4, 5, 1, 2};
	int arr3[] = {2, 3, 4, 5, 1};
	int arr4[] = {1, 2, 3, 4, 5};
	int arr5[] = {5, 1, 2, 3, 4};

	printf("target idx:%d\n", search2(arr1, sizeof(arr1) / sizeof(int), 1));
	printf("target idx:%d\n", search2(arr2, sizeof(arr2) / sizeof(int), 1));
	printf("target idx:%d\n", search2(arr3, sizeof(arr3) / sizeof(int), 1));
	printf("target idx:%d\n", search2(arr4, sizeof(arr4) / sizeof(int), 1));
	printf("target idx:%d\n", search2(arr5, sizeof(arr5) / sizeof(int), 1));

	return 0;
}
