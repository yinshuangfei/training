/**
 * @file test_main.c
 * @author Alan Yin (yinshuangfei007@163.com)
 * @brief
 * @version 0.0.1
 * @date 2025-05-08
 *
 * @copyright Copyright (c) 2025
 *
 * @node: 寻找旋转排序数组中的最小值（或最大值）
 *
 * 主要思路，寻找断崖，找到断崖处的两个点。只能用 [mid..right] 区域进行判断, 使用左边
 * 区域会天然拒绝正确的值。
 *
 * (1) 最小值：放弃左边时，去除边界值
 *         断崖         断崖
 *         |            |
 * 3 5 6 7 9 0 1 2  ->  9 0 1 2  ->  9    0  ->  0
 * l     m       r      l m   r      l[m] r     l[m,r]
 *
 * (2) 最大值：放弃左边时，保留边界值，剩两个值时，返回左右值（不考虑无旋转情况）
 *        断崖           断崖           断崖
 *         |              |              |
 * 3 5 6 7 9 0 1 2  ->  7 9 0 1 2  ->  7 9 0  ->  9 0
 * l     m       r      l   m   r      l m r      l r
 *
 */

#include <stdlib.h>
#include <stdio.h>

/**
 * @brief 查找最小值
 *
 * 任意旋转有效
 *
 * @param [in] arr
 * @param [in] len
 * @return int 0: success; <0: failed, posix errno;
 */
int find_min(int arr[], int len)
{
	int left = 0, right = len - 1;

	if (len == 0)
		return -1;

	while (left < right) {
		int mid = left + (right - left) / 2;

		/** 只能用右边进行判断 */
		if (arr[mid] > arr[right]) {
			/** 接受右边时，去除边界值 */
			left = mid + 1;
		} else {
			/** 接受左边时，保留边界值 */
			right = mid;
		}
	}

	return arr[left];
}

/**
 * @brief 查找最大值
 *
 * 在没有旋转的情况下，需要特别处理
 *
 * @param [in] arr
 * @param [in] len
 * @return int 0: success; <0: failed, posix errno;
 */
int find_max(int arr[], int len)
{
	int left = 0, right = len - 1;

	if (len == 0)
		return -1;

	/** 在无旋转时，直接返回 */
	if (arr[0] < arr[len - 1]) {
		return arr[len - 1];
	}

	/** 在有旋转时，二分查找 */
	while (left < right) {
		int mid = left + (right - left) / 2;

		/**
		 * 只能用右边进行判断，在无旋转时，判断失效：
		 * 1, 2, 3, 4, 5
		*/
		if (arr[mid] > arr[right]) {
			/** 接受右边时，保留边界值 */
			left = mid;
		} else {
			/** 接受左边时，保留边界值 */
			right = mid;
		}

		/** 找到 “最大值 - 最小值” 的两个点 */
		if (right == left + 1) {
			break;
		}
	}

	/** 一定要返回左值 */
	return arr[left];
}

int main(int argc, char *argv[])
{
	int arr1[] = {4, 5, 1, 2, 3};
	int arr2[] = {3, 4, 5, 1, 2};
	int arr3[] = {2, 3, 4, 5, 1};
	int arr4[] = {1, 2, 3, 4, 5};
	int arr5[] = {5, 1, 2, 3, 4};

	printf("min:%d\n", find_min(arr1, sizeof(arr1) / sizeof(int)));
	printf("min:%d\n", find_min(arr2, sizeof(arr2) / sizeof(int)));
	printf("min:%d\n", find_min(arr3, sizeof(arr3) / sizeof(int)));
	printf("min:%d\n", find_min(arr4, sizeof(arr4) / sizeof(int)));
	printf("min:%d\n", find_min(arr5, sizeof(arr5) / sizeof(int)));

	printf("max:%d\n", find_max(arr1, sizeof(arr1) / sizeof(int)));
	printf("max:%d\n", find_max(arr2, sizeof(arr2) / sizeof(int)));
	printf("max:%d\n", find_max(arr3, sizeof(arr3) / sizeof(int)));
	printf("max:%d\n", find_max(arr4, sizeof(arr4) / sizeof(int)));
	printf("max:%d\n", find_max(arr5, sizeof(arr5) / sizeof(int)));

	return 0;
}
