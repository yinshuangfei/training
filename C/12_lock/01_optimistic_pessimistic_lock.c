/**
 * @file 01_optimistic_pessimistic_lock.c
 * @author Alan Yin (yinshuangfei007@163.com)
 * @brief
 * @version 0.0.1
 * @date 2026-05-16
 *
 * @copyright Copyright (c) 2026
 *
 * @node:
 *
 * 性能特点：涉及用户态到内核态的切换以及线程的上下文切换，开销较大。在竞争极其激烈
 * （写多读少）的情况下，它能避免无意义的 CPU 空转，表现相对稳定。
 *
 * 性能特点：完全在用户态完成，没有线程阻塞和唤醒的开销，因此在大多数高并发、冲突不
 * 极端的情况下，CAS 的速度通常会快于互斥锁。但如果冲突非常频繁，大量的重试（自旋）
 * 会疯狂消耗 CPU 资源，反而导致性能下降。
 * - 在冲突频繁（高并发写）的情况下，你当前的 CAS 乐观锁代码性能会急剧下降。这是因为
 * 大量的线程同时在 do-while 循环中不断重试（自旋），疯狂消耗 CPU 资源却很难抢到更新权。
 *
 * thrd: 16, total: 50000000 (8核CPU)
 * - 悲观锁: 50000000, time: 1146.92 ms
 * - 乐观锁: 50000000, time: 1134.80 ms
 * - 分段CAS: 50000000, time: 717.07 ms
 * - 终极优化分段CAS: 50000000, time: 37.04 ms
 */

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/time.h>
#include <string.h>

#define NUM_THREADS 5000          // 线程数量
#define OPS_PER_TOTAL 50000000000LL // 总操作次数
#define OPS_PER_THREAD (OPS_PER_TOTAL / NUM_THREADS)  // 每个线程的操作次数
#define SEGMENT_COUNT 128 // 分段数量（通常设置为 CPU 核心数或线程数的倍数）
#define MAX_SPIN_RETRY 10 // 自旋最大次数，超过则让出CPU
#define BATCH_SIZE 100          // 本地累加阈值（批次大小）

// 共享数据
int shared_data_pessimistic = 0; // 悲观锁保护的变量
int shared_data_optimistic = 0;  // 乐观锁(CAS)保护的变量

// 优化后的分段共享数据
// alignas(64) 是为了避免伪共享(False Sharing)，提升CPU缓存命中率
__attribute__((aligned(64))) int segmented_data[SEGMENT_COUNT] = {0};

// 解决伪共享的填充结构体
typedef struct {
	__attribute__((aligned(64))) int counter;
	char padding[64 - sizeof(int)];
} PaddedCounter;

PaddedCounter pad_segmented_data[SEGMENT_COUNT] = {0};
PaddedCounter global_segmented_data[SEGMENT_COUNT] = {0};

// 1. 悲观锁：初始化互斥锁
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

// 获取当前时间戳（毫秒）
double get_time_ms()
{
	struct timeval tv;
	gettimeofday(&tv, NULL);
	return tv.tv_sec * 1000.0 + tv.tv_usec / 1000.0;
}

// ================== 悲观锁测试 ==================
void *pessimistic_thread(void* arg)
{
	for (int i = 0; i < OPS_PER_THREAD; i++) {
		pthread_mutex_lock(&mutex);      // 加锁
		shared_data_pessimistic++;       // 临界区操作
		pthread_mutex_unlock(&mutex);    // 解锁
	}
	return NULL;
}

// ================== 乐观锁 (CAS) 测试 ==================
// GCC 内置的 CAS 原子操作函数：__atomic_compare_exchange_n
void *optimistic_thread(void* arg)
{
	for (int i = 0; i < OPS_PER_THREAD; i++) {
		int expected, desired;
		do {
			expected = shared_data_optimistic; // 1. 读取旧值（预期值）
			desired = expected + 1;            // 2. 计算新值
			// 3. CAS 操作：如果 shared_data_optimistic == expected，则更新为 desired
			// 返回 true 表示更新成功，false 表示被其他线程修改过，需要重试
		} while (!__atomic_compare_exchange_n(&shared_data_optimistic,
						      &expected, desired,
						      0, __ATOMIC_SEQ_CST,
						      __ATOMIC_SEQ_CST));
	}
	return NULL;
}

void *segmented_cas_thread(void *arg)
{
	// 获取当前线程的索引（0 到 NUM_THREADS-1）
	long thread_id = (long)arg;
	// 根据线程ID映射到对应的分段（取模），让不同线程尽量操作不同的分段
	int segment_index = thread_id % SEGMENT_COUNT;

	for (int i = 0; i < OPS_PER_THREAD; i++) {
		int expected, desired;
		do {
			expected = segmented_data[segment_index];
			desired = expected + 1;
		// 只对当前线程分配到的那个分段进行 CAS 操作
		} while (!__atomic_compare_exchange_n(&segmented_data[segment_index], &expected, desired,
						0, __ATOMIC_SEQ_CST, __ATOMIC_SEQ_CST));
	}
	return NULL;
}

void *optimized_segmented_thread(void *arg)
{
	long thread_id = (long)arg;
	int segment_index = thread_id % SEGMENT_COUNT;
	int spin_count = 0; // 记录当前分段上的自旋次数

	for (int i = 0; i < OPS_PER_THREAD; i++) {
		int expected, desired;
		while (1) {
			expected = pad_segmented_data[segment_index].counter;
			desired = expected + 1;

			if (__atomic_compare_exchange_n(&pad_segmented_data[segment_index].counter, &expected, desired,
							0, __ATOMIC_SEQ_CST, __ATOMIC_SEQ_CST)) {
				spin_count = 0; // CAS成功，重置自旋计数
				break;
			}

			// 引入自适应退避策略
			spin_count++;
			if (spin_count > MAX_SPIN_RETRY) {
				sched_yield(); // 连续失败多次，主动让出CPU时间片
				spin_count = 0;
			}
		}
	}
	return NULL;
}

void* ultra_high_concurrency_thread(void* arg) {
	long thread_id = (long)arg;
	// 初始分段映射
	int segment_index = thread_id % SEGMENT_COUNT;
	int spin_count = 0;

	for (int i = 0; i < OPS_PER_THREAD; i++) {
		int expected, desired;
		while (1) {
			expected = pad_segmented_data[segment_index].counter;
			desired = expected + 1;

			if (__atomic_compare_exchange_n(&pad_segmented_data[segment_index].counter, &expected, desired,
							0, __ATOMIC_SEQ_CST, __ATOMIC_SEQ_CST)) {
				spin_count = 0; // CAS成功，重置计数
				break;
			}

			// 硬件级优化：插入 pause 指令，缓解超线程竞争和流水线惩罚
			__builtin_ia32_pause();

			spin_count++;
			// 自适应退避策略
			if (spin_count > MAX_SPIN_RETRY) {
				// 阶梯化让步：先短暂让出，若依然失败则考虑迁移分段
				sched_yield();
				// 模拟 LongAdder 的自动迁移：换个坑位继续蹲
				segment_index = (segment_index + 1) % SEGMENT_COUNT;
				spin_count = 0;
			}
		}
	}
	return NULL;
}

void test_pessimistic()
{
	pthread_t threads[NUM_THREADS];
	shared_data_pessimistic = 0;

	double start = get_time_ms();
	for (int i = 0; i < NUM_THREADS; i++) {
		pthread_create(&threads[i], NULL, pessimistic_thread, NULL);
	}
	for (int i = 0; i < NUM_THREADS; i++) {
		pthread_join(threads[i], NULL);
	}
	double end = get_time_ms();

	printf("[悲观锁] 最终结果: %d, 耗时: %.2f ms\n",
		shared_data_pessimistic, end - start);
}

void test_optimistic()
{
	pthread_t threads[NUM_THREADS];
	shared_data_optimistic = 0;

	double start = get_time_ms();
	for (int i = 0; i < NUM_THREADS; i++) {
		pthread_create(&threads[i], NULL, optimistic_thread, NULL);
	}
	for (int i = 0; i < NUM_THREADS; i++) {
		pthread_join(threads[i], NULL);
	}
	double end = get_time_ms();

	printf("[乐观锁] 最终结果: %d, 耗时: %.2f ms\n",
		shared_data_optimistic, end - start);
}

void test_segmented_cas()
{
	pthread_t threads[NUM_THREADS];
	memset(segmented_data, 0, sizeof(segmented_data)); // 清空分段数组

	double start = get_time_ms();
	for (long i = 0; i < NUM_THREADS; i++) {
		pthread_create(&threads[i], NULL, segmented_cas_thread, (void*)i); // 传入线程ID
	}
	for (int i = 0; i < NUM_THREADS; i++) {
		pthread_join(threads[i], NULL);
	}
	double end = get_time_ms();

	// 计算最终总和
	long final_sum = 0;
	for (int i = 0; i < SEGMENT_COUNT; i++) {
		final_sum += segmented_data[i];
	}

	printf("[分段CAS] 最终结果: %ld, 耗时: %.2f ms\n",
		final_sum, end - start);
}

void test_optimized_segmented_cas()
{
	pthread_t threads[NUM_THREADS];
	memset(pad_segmented_data, 0, sizeof(pad_segmented_data));

	double start = get_time_ms();
	for (long i = 0; i < NUM_THREADS; i++) {
		pthread_create(&threads[i], NULL, optimized_segmented_thread, (void*)i);
	}
	for (int i = 0; i < NUM_THREADS; i++) {
		pthread_join(threads[i], NULL);
	}
	double end = get_time_ms();

	long final_sum = 0;
	for (int i = 0; i < SEGMENT_COUNT; i++) {
		final_sum += pad_segmented_data[i].counter;
	}

	printf("[终极优化分段CAS] 最终结果: %ld, 耗时: %.2f ms\n",
		final_sum, end - start);
}

void test_ultra_high_concurrency()
{
	pthread_t threads[NUM_THREADS];
	memset(pad_segmented_data, 0, sizeof(pad_segmented_data));

	double start = get_time_ms();
	for (long i = 0; i < NUM_THREADS; i++) {
		pthread_create(&threads[i], NULL, ultra_high_concurrency_thread, (void*)i);
	}
	for (int i = 0; i < NUM_THREADS; i++) {
		pthread_join(threads[i], NULL);
	}
	double end = get_time_ms();

	long final_sum = 0;
	for (int i = 0; i < SEGMENT_COUNT; i++) {
		final_sum += pad_segmented_data[i].counter;
	}

	printf("[5000并发极致优化] 最终结果: %ld, 耗时: %.2f ms\n",
		final_sum, end - start);
}

void update_global_counter_with_cas(int value_to_add) {
	// 简单轮询选择一个分段进行更新
	static __thread int last_index = 0;
	int segment_index = (last_index++) % SEGMENT_COUNT;

	long expected, desired;
	do {
		expected = global_segmented_data[segment_index].counter;
		desired = expected + value_to_add;
	} while (!__atomic_compare_exchange_n(&global_segmented_data[segment_index].counter,
						&expected, desired,
						0, __ATOMIC_SEQ_CST, __ATOMIC_SEQ_CST));
}

void* batch_update_thread(void* arg) {
	// 模拟 ThreadLocal 的本地私有缓冲区
	int local_buffer = 0;

	for (int i = 0; i < OPS_PER_THREAD; i++) {
		// 1. 先在本地极速累加，完全不涉及任何锁或原子操作（纯内存计算）
		local_buffer++;

		// 2. 达到批次大小，才发起一次全局的 CAS 更新
		if (local_buffer >= BATCH_SIZE) {
			update_global_counter_with_cas(BATCH_SIZE);
			local_buffer = 0; // 清空本地缓冲
		}
	}

	// 3. 线程退出前，把剩余的零头也提交上去
	if (local_buffer > 0) {
		update_global_counter_with_cas(local_buffer);
	}
	return NULL;
}

void test_batch_update()
{
	pthread_t threads[NUM_THREADS];
	memset(global_segmented_data, 0, sizeof(global_segmented_data));

	double start = get_time_ms();
	for (int i = 0; i < NUM_THREADS; i++) {
		pthread_create(&threads[i], NULL, batch_update_thread, NULL);
	}
	for (int i = 0; i < NUM_THREADS; i++) {
		pthread_join(threads[i], NULL);
	}
	double end = get_time_ms();

	long final_sum = 0;
	for (int i = 0; i < SEGMENT_COUNT; i++) {
		final_sum += global_segmented_data[i].counter;
	}

	printf("[异步批量处理] 最终结果: %ld, 耗时: %.2f ms\n",
		final_sum, end - start);
}

int main()
{
	printf("开始性能对比测试 (线程数: %d, 每线程操作: %ld)\n\n",
		NUM_THREADS, (long)OPS_PER_THREAD);

	// test_pessimistic();
	// test_optimistic();
	test_segmented_cas();
	test_optimized_segmented_cas();
	test_ultra_high_concurrency();
	test_batch_update();

	return 0;
}