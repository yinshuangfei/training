/**
 * @file 23_top_m_num.c
 * @author Alan Yin (yinshuangfei007@163.com)
 * @brief
 * @version 0.0.1
 * @date 2025-07-09
 *
 * @copyright Copyright (c) 2025
 *
 * @node: 给你一个整数数组 nums 和一个整数 k ，请你返回其中出现频率前 k 高的元素。
 * 你可以按 任意顺序 返回答案。
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_NUM 100

typedef struct num_freq_t {
	int val;
	int freq;
} num_freq ;

void swap(num_freq *a, num_freq *b)
{
	num_freq temp = *a;
	*a = *b;
	*b = temp;
}

int camp(num_freq *a, num_freq *b)
{
	if (a->freq != b->freq) {
		/** 频率降序 */
		return b->freq - a->freq;
	} else {
		/** 数字升序 */
		return a->val - b->val;
	}
}

void heapify(num_freq *arr, int n, int i)
{
	int large = i;
	int left = i * 2 + 1;
	int right = i * 2 + 2;

	if (left < n && camp(arr + left, arr + large) > 0) {
		large = left;
	}

	if (right < n && camp(arr + right, arr + large) > 0) {
		large = right;
	}

	if (large != i) {
		swap(arr + large, arr + i);
		heapify(arr, n, large);
	}
}

void heapify_sort(num_freq *arr, int n)
{
	for (int i = n / 2 - 1; i >= 0 ; i--) {
		heapify(arr, n, i);
	}

	for (int i = n - 1; i > 0; i--) {
		swap(arr, arr + i);
		heapify(arr, i, 0);
	}
}

void print_top_m_num(int *arr, int len, int m)
{
	if (arr == NULL || len == 0) {
		return;
	}

	int freq[MAX_NUM] = {0};
	int num = 0;

	for (int i = 0; i < len; i++) {
		freq[arr[i]]++;
	}

	for (int i = 0; i < MAX_NUM; i++) {
		if (freq[i] > 0)
			num++;
	}

	num_freq *freq_arr;
	freq_arr = calloc(1, sizeof(num_freq) * num);
	if (NULL == freq_arr) {
		return;
	}

	int idx = 0;
	for (int i = 0; i < MAX_NUM; i++) {
		if (freq[i] > 0) {
			freq_arr[idx].freq = freq[i];
			freq_arr[idx].val = i;
			idx++;
		}
	}

	heapify_sort(freq_arr, num);

	for (int i = 0; i < m; i++) {
		printf("val:%d, freq:%d\n", freq_arr[i].val, freq_arr[i].freq);
	}

	free(freq_arr);
}

int main()
{
	int arr[] = {1,1,2,2,2,3,4,4,4,4,4,5,6,6,6,6,6,6,7,8,8,9};
	int n = sizeof(arr) / sizeof(arr[0]);

	print_top_m_num(arr, n, 3);
}