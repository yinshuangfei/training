/**
 * @file 11_sum_of_two.c
 * @author Alan Yin (yinshuangfei007@163.com)
 * @brief
 * @version 0.0.1
 * @date 2025-05-11
 *
 * @copyright Copyright (c) 2025
 *
 * @node: 两数之和是一个经典的面试题，题目通常描述为：
 * 给定一个整数数组和一个目标值，找出数组中两个数的和等于目标值的下标。
 * (1) 暴力解法（时间复杂度O(n²)）, 不推荐;
 * (2) 哈希表优化解法（时间复杂度O(n)）, 推荐;
 *
 * 时间复杂度分析‌：暴力解法O(n²)，哈希表解法O(n)；
 * 空间复杂度‌：暴力解法O(1)，哈希表解法O(n)；
 */

#include <stdio.h>
#include <stdlib.h>

#define HASH_SIZE 10000

typedef struct HashNode {
	int key;
	int value;
	struct HashNode* next;
} HashNode;

HashNode* hashTable[HASH_SIZE];

/**
 * @brief 哈希函数
 *
 * @param [in] key
 * @return int 0: success; <0: failed, posix errno;
 */
int hash(int key)
{
	return abs(key) % HASH_SIZE;
}

void insert(int key, int value)
{
	int index = hash(key);
	HashNode* node = (HashNode*)malloc(sizeof(HashNode));

	/** 插入头部 */
	node->key = key;
	node->value = value;
	node->next = hashTable[index];
	hashTable[index] = node;
}

int search(int key)
{
	int index = hash(key);
	HashNode* node = hashTable[index];
	while (node != NULL) {
		if (node->key == key) {
			return node->value;
		}
		node = node->next;
	}
	return -1;
}

void twoSum(int* nums, int numsSize, int target, int* result)
{
	for (int i = 0; i < numsSize; i++) {
		/** 计算构成和的另一个值 */
		int complement = target - nums[i];
		int complementIndex = search(complement);

		/** 找到值 */
		if (complementIndex != -1 && complementIndex != i) {
			result[0] = complementIndex;
			result[1] = i;
			return;
		}

		/** 没找到，插入哈希表 */
		insert(nums[i], i);
	}
}

int main()
{
	int nums[] = {4, 5, 1, 2, 3, -1, -29, 28, 6, 11};
	int target = -31;
	int result[2] = {-1, -1};

	twoSum(nums, sizeof(nums)/sizeof(nums[0]), target, result);

	printf("Indices: [%d, %d]\n", result[0], result[1]);

	/** 清理哈希表 */
	for (int i = 0; i < HASH_SIZE; i++) {
		HashNode* node = hashTable[i];
		while (node != NULL) {
			HashNode* temp = node;
			node = node->next;
			free(temp);
		}
	}

	return 0;
}