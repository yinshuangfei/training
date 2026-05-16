/**
 * @file test_main.c
 * @author Alan Yin (yinshuangfei007@163.com)
 * @brief
 * @version 0.0.1
 * @date 2026-05-12
 *
 * @copyright Copyright (c) 2026
 *
 * @node: 布隆过滤器
 * 简单来说，布隆过滤器就是一个“守门员”。它的作用不是给你确切的答案，而是帮你快速过滤掉
 * 那些“绝对不可能”的情况，从而节省大量的计算资源和时间。
 *
 * 布隆过滤器允许极小概率的“误判”（即判断一个不存在的元素存在），但绝不会“漏判”。
 * - 如果查询返回 0：该元素 100% 不存在。
 * - 如果查询返回 1：该元素 可能存在（有极小概率是其他元素的哈希位冲突导致的）。
 *
 * 我们没有使用 bool 数组（每个元素占 1 字节），而是使用了 uint8_t 数组。
 * - 设置位: array[i/8] |= (1 << (i%8))
 * - 查询位: (array[i/8] >> (i%8)) & 1
 *
 * 我们利用数学性质，通过两个基础哈希值 h1(x) 和 h2(x)，可以组合出 k 个哈希值：
 * hi(x) = h1(x) + i * h2(x)
 * 这种方法在数学上被证明可以达到与使用 k 个独立哈希函数几乎相同的分布效果，但计算速度
 * 更快。
 *
 * 用途：
 * (1) 解决“缓存穿透”问题（最常用）：
 * 在 Web 开发中，如果有人恶意请求数据库中不存在的Key（例如 id=-1），请求会直接穿透缓存
 * 打到数据库，导致数据库压力过大甚至崩溃。
 * 布隆过滤器的作用：在缓存前加一层布隆过滤器。当请求进来时，先问布隆过滤器“这个Key存在
 * 吗？”。如果回答“不存在”，直接拦截，不再查询数据库；如果回答“可能存在”，再去查缓存和
 * 数据库。这能极大地保护后端数据库。
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <math.h>

// --- 配置参数 ---
#define BLOOM_SIZE 1000000  // 位数组大小 (1MB ≈ 8,388,608 bits)
#define HASH_SEED_1 131     // 哈希种子 1
#define HASH_SEED_2 137     // 哈希种子 2
#define NUM_HASHES 7        // 哈希函数个数 (k)

// --- 数据结构 ---
typedef struct {
	uint8_t *bit_array;     // 位数组
	size_t size;            // 位数组大小（字节）
} BloomFilter;

// --- 核心哈希函数 (BKDRHash 变种) ---
// 使用双哈希策略：h(i) = h1 + i * h2
// 这样只需要计算两个基础哈希，就能模拟出 k 个独立哈希函数的效果
uint64_t hash_base(const char *str, uint64_t seed)
{
	uint64_t hash = 0;
	while (*str) {
		hash = hash * seed + (*str++);
	}
	return hash;
}

// --- 初始化布隆过滤器 ---
BloomFilter *bloom_create(size_t size)
{
	BloomFilter *bf = (BloomFilter*)malloc(sizeof(BloomFilter));
	if (!bf)
		return NULL;

	bf->size = size;
	// calloc 会自动将内存初始化为 0
	bf->bit_array = (uint8_t*)calloc(size, sizeof(uint8_t));

	if (!bf->bit_array) {
		free(bf);
		return NULL;
	}
	return bf;
}

// --- 添加元素 ---
void bloom_add(BloomFilter *bf, const char *key)
{
	uint64_t h1 = hash_base(key, HASH_SEED_1);
	uint64_t h2 = hash_base(key, HASH_SEED_2);

	for (int i = 0; i < NUM_HASHES; i++) {
		// 计算第 i 个哈希值的索引位置
		uint64_t index = (h1 + i * h2) % (bf->size * 8);

		// 设置对应的位为 1
		// index / 8 找到字节位置，index % 8 找到字节内的位
		bf->bit_array[index / 8] |= (1 << (index % 8));
	}
}

// --- 查询元素 ---
int bloom_contains(BloomFilter *bf, const char *key)
{
	uint64_t h1 = hash_base(key, HASH_SEED_1);
	uint64_t h2 = hash_base(key, HASH_SEED_2);

	for (int i = 0; i < NUM_HASHES; i++) {
		uint64_t index = (h1 + i * h2) % (bf->size * 8);

		// 检查对应的位是否为 1
		if (!(bf->bit_array[index / 8] & (1 << (index % 8)))) {
			return 0; // 只要有一个位是 0，说明绝对不存在
		}
	}
	return 1; // 所有位都是 1，说明“可能存在”
}

// --- 释放资源 ---
void bloom_free(BloomFilter *bf)
{
	if (bf) {
		if (bf->bit_array)
			free(bf->bit_array);
		free(bf);
	}
}

// --- 测试主函数 ---
int main()
{
	// 1. 创建过滤器 (分配 1MB 内存)
	BloomFilter *bf = bloom_create(BLOOM_SIZE);
	if (!bf) {
		printf("内存分配失败！\n");
		return 1;
	}

	// 2. 插入数据
	const char *fruits[] = {"apple", "banana", "grape", "orange"};
	int count = sizeof(fruits) / sizeof(fruits[0]);

	printf("正在插入数据...\n");
	for (int i = 0; i < count; i++) {
		bloom_add(bf, fruits[i]);
	}

	// 3. 查询测试
	printf("\n--- 查询结果 ---\n");

	// 存在的元素
	if (bloom_contains(bf, "apple")) {
		printf("✅ 'apple' 可能存在\n");
	}

	// 不存在的元素
	if (!bloom_contains(bf, "watermelon")) {
		printf("✅ 'watermelon' 一定不存在\n");
	} else {
		printf("⚠️ 'watermelon' 误判为存在 (False Positive)\n");
	}

	// 4. 清理
	bloom_free(bf);
	return 0;
}