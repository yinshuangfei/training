/**
 * @file 13_quick_sort.c
 * @author Alan Yin (yinshuangfei007@163.com)
 * @brief
 * @version 0.0.1
 * @date 2025-05-11
 *
 * @copyright Copyright (c) 2025
 *
 * @node:
 */

#include <stdio.h>

/**
 * @brief 交换两个元素
 *
 * @param [in] a
 * @param [in] b
 */
void swap(int* a, int* b)
{
	int temp = *a;
	*a = *b;
	*b = temp;
}

/**
 * @brief 分区函数（核心逻辑）
 *
 */
int partition(int arr[], int low, int high)
{
	int pivot = arr[high];  // 选择最右侧元素作为基准
	int i = (low - 1);      // 小于基准的元素的边界索引

	/**
	 *           1, 7, 2, 10, 8, 5
	 *        i  low             high
	 * 第0遍：i = -1，j = 0;  [1, 7, 2, 10, 8, 5]
	 * 第1遍：i =  0，j = 1;  [1, 7, 2, 10, 8, 5]
	 * 第2遍：i =  0，j = 2;  [1, 2, 7, 10, 8, 5]
	 * 第3遍：i =  1，j = 3;  [1, 2, 7, 10, 8, 5]
	 * 第4遍：i =  1，j = 4;  [1, 2, 7, 10, 8, 5]
	 *
	 * 退出 ：                [1, 2, 5, 10, 8, 7]
	 */
	for (int j = low; j <= high - 1; j++) {
		/** 如果当前元素小于基准 */
		if (arr[j] < pivot) {
			i++;  /** 扩展小于基准的区域 */
			swap(&arr[i], &arr[j]);
		}
	}

	/** 将基准放到正确位置 */
	swap(&arr[i + 1], &arr[high]);

	/** 返回基准的最终索引 */
	return (i + 1);
}

/**
 * @brief 快速排序主函数
 *
 * @param [in] arr
 * @param [in] low
 * @param [in] high
 */
void quickSort(int arr[], int low, int high)
{
	if (low < high) {
		/** pi是分区索引，arr[pi]现在在正确位置 */
		int pi = partition(arr, low, high);

		/** 递归排序分区 */
		quickSort(arr, low, pi - 1);   // 左子数组
		quickSort(arr, pi + 1, high);  // 右子数组
	}
}

int main(void)
{
	int arr[] = {1, 7, 2, 10, 8, 5};
	int n = sizeof(arr) / sizeof(arr[0]);

	printf("排序前: ");
	for (int i = 0; i < n; i++) {
		printf("%d ", arr[i]);
	}
	printf("\n");

	quickSort(arr, 0, n - 1);

	printf("排序后: ");
	for (int i = 0; i < n; i++) {
		printf("%d ", arr[i]);
	}
	printf("\n");

	return 0;
}
