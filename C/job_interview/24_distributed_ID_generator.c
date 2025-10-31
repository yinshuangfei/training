/**
 * @file 21_distributed_ID_generator.c
 * @author Alan Yin (yinshuangfei007@163.com)
 * @brief
 * @version 0.0.1
 * @date 2025-05-14
 *
 * @copyright Copyright (c) 2025
 *
 * @node: 分布式ID生成器
 *
 * 全局唯一且自增的分布式ID生成器（C语言实现）
 */

/**
 * | 时间戳(42位) | 工作节点ID(10位) | 序列号(12位) |
 * ‌- 时间戳‌：42位，约139年不重复（从2021年开始）
 * - ‌工作节点ID‌：10位，支持1024个节点
 * - 序列号‌：12位，每毫秒4096个ID
 *
 * 全局唯一性保证‌：
 * - 通过工作节点ID区分不同机器
 * - 同一节点内通过时间戳+序列号保证唯一
 *
 * ‌趋势自增实现‌：
 * - 时间戳在高位，确保整体趋势递增
 * - 同一毫秒内序列号递增
 *
 * 线程安全‌：
 * - 使用互斥锁(pthread_mutex_t)保护共享状态
 * - 确保多线程环境下正确生成ID
 *
 * 时钟回拨处理‌：
 * - 检测到时钟回拨时等待直到时间恢复
 * - 避免生成重复ID
*/

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <time.h>
#include <sys/time.h>
#include <pthread.h>

#define EPOCH 1609459200000ULL  // 2021-01-01 00:00:00 UTC
#define SEQUENCE_BITS 12
#define WORKER_ID_BITS 10
#define MAX_WORKER_ID ((1 << WORKER_ID_BITS) - 1)
#define MAX_SEQUENCE ((1 << SEQUENCE_BITS) - 1)

typedef struct {
	uint64_t last_timestamp;
	uint16_t worker_id;
	uint32_t sequence;
	pthread_mutex_t lock;
} DistributedIDGenerator;

// 初始化ID生成器
int id_generator_init(DistributedIDGenerator* gen, uint16_t worker_id)
{
	if (worker_id > MAX_WORKER_ID) {
		fprintf(stderr, "Worker ID can't be greater than %d\n",
			MAX_WORKER_ID);
		return -1;
	}

	gen->last_timestamp = 0;
	gen->worker_id = worker_id;
	gen->sequence = 0;
	pthread_mutex_init(&gen->lock, NULL);
	return 0;
}

// 获取当前毫秒时间戳
uint64_t current_millis()
{
	struct timeval tv;
	gettimeofday(&tv, NULL);
	return (uint64_t)(tv.tv_sec) * 1000 + (uint64_t)(tv.tv_usec) / 1000;
}

// 等待下一毫秒
uint64_t wait_next_millis(uint64_t last_timestamp)
{
	uint64_t timestamp = current_millis();
	while (timestamp <= last_timestamp) {
		timestamp = current_millis();
	}
	return timestamp;
}

// 生成分布式ID
uint64_t generate_id(DistributedIDGenerator* gen)
{
	pthread_mutex_lock(&gen->lock);

	uint64_t timestamp = current_millis();

	// 处理时钟回拨
	if (timestamp < gen->last_timestamp) {
		fprintf(stderr, "Clock moved backwards. Waiting...\n");
		timestamp = wait_next_millis(gen->last_timestamp);
	}

	// 同一毫秒内生成多个ID
	if (gen->last_timestamp == timestamp) {
		gen->sequence = (gen->sequence + 1) & MAX_SEQUENCE;
		if (gen->sequence == 0) {
			timestamp = wait_next_millis(gen->last_timestamp);
		}
	} else {
		gen->sequence = 0;
	}

	gen->last_timestamp = timestamp;

	uint64_t id = ((timestamp - EPOCH) << (WORKER_ID_BITS + SEQUENCE_BITS)) |
		      ((uint64_t)gen->worker_id << SEQUENCE_BITS) |
		      (gen->sequence);

	pthread_mutex_unlock(&gen->lock);
	return id;
}

// 销毁ID生成器
void id_generator_destroy(DistributedIDGenerator* gen)
{
	pthread_mutex_destroy(&gen->lock);
}

// 测试函数
void* test_generator(void* arg)
{
	DistributedIDGenerator* gen = (DistributedIDGenerator*)arg;
	for (int i = 0; i < 5; i++) {
		uint64_t id = generate_id(gen);
		printf("Thread %lu: ID = %llu\n", pthread_self(), id);
	}
	return NULL;
}

int main()
{
	DistributedIDGenerator gen;

	if (id_generator_init(&gen, 1) != 0) {
		return EXIT_FAILURE;
	}

	// 单线程测试
	printf("Single thread test:\n");
	for (int i = 0; i < 5; i++) {
		uint64_t id = generate_id(&gen);
		printf("Generated ID: %llu\n", id);
	}

	// 多线程测试
	printf("\nMulti-thread test:\n");
	pthread_t threads[3];
	for (int i = 0; i < 3; i++) {
		pthread_create(&threads[i], NULL, test_generator, &gen);
	}
	for (int i = 0; i < 3; i++) {
		pthread_join(threads[i], NULL);
	}

	id_generator_destroy(&gen);
	return EXIT_SUCCESS;
}
