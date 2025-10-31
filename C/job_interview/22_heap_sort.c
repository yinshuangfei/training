/**
 * @file 22_heap_sort.c
 * @author Alan Yin (yinshuangfei007@163.com)
 * @brief
 * @version 0.0.1
 * @date 2025-07-08
 *
 * @copyright Copyright (c) 2025
 *
 * @node: 堆排序
 */

#include <stdio.h>
#include <stdlib.h>

void swap(int* a, int* b)
{
	int temp = *a;
	*a = *b;
	*b = temp;
}

void heapify(int *arr, int n, int i)
{
	int large = i;
	int left = i * 2 + 1;
	int right = i * 2 + 2;

	if (left < n && arr[left] > arr[large]) {
		large = left;
	}

	if (right < n && arr[right] > arr[large]) {
		large = right;
	}

	if (large != i) {
		swap(&arr[i], &arr[large]);
		heapify(arr, n, large);
	}
}

void heapify_sort(int *arr, int n)
{
	for (int i = n / 2 - 1; i >= 0; i--) {
		heapify(arr, n, i);
	}

	for (int i = n - 1; i > 0; i--) {
		swap(&arr[i], &arr[0]);
		heapify(arr, i, 0);
	}
}

int main()
{
	int arr[] = {1, 7, 2, 10, 8, 5};
	int n = sizeof(arr) / sizeof(arr[0]);

	printf("排序前: ");
	for (int i = 0; i < n; i++) {
		printf("%d ", arr[i]);
	}
	printf("\n");

	heapify_sort(arr, n);

	printf("排序后: ");
	for (int i = 0; i < n; i++) {
		printf("%d ", arr[i]);
	}
	printf("\n");

	return 0;
}
