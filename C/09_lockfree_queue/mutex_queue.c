/**
 * @file mutex_queue.c
 * @author Alan Yin (yinshuangfei007@163.com)
 * @brief
 * @version 0.0.1
 * @date 2025-03-08
 *
 * @copyright Copyright (c) 2025
 *
 * @node:
 */

#include <stdlib.h>
#include <pthread.h>

#include "mutex_queue.h"
#include "../common/utils.h"

mutex_queue_t mutex_queue;

/**
 * @brief 初始化队列
 *
 * @param [in] queue
 */
void init_mutex_queue()
{
	mutex_queue_t *queue = &mutex_queue;

	pthread_mutex_init(&queue->mutex, NULL);
	pthread_cond_init(&queue->cond, NULL);

	queue->queue_ctr.count = 0;
	queue->queue_ctr.total_enqueue = 0;
	queue->queue_ctr.shutdown = 0;
	queue->head.next = &queue->head;
	queue->head.prev = &queue->head;
	queue->head.data = -1;
}

/**
 * @brief 销毁队列
 *
 * @param [in] queue
 */
void destroy_mutex_queue()
{
	/** TODO: 销毁剩余 node */
	mutex_queue_t *queue = &mutex_queue;

	pthread_mutex_destroy(&queue->mutex);
	pthread_cond_destroy(&queue->cond);
}

/**
 * @brief 正序遍历队列
 *
 * @param [in] queue
 */
void iter_queue()
{
	mutex_queue_t *queue = &mutex_queue;
	node_t *node = NULL;
	int idx = 1;

	node = queue->head.next;

	while (node != &queue->head) {
		pr_info("[%d/%ld] val:%d, %p", idx, queue->queue_ctr.count,
			node->data, node);
		node = node->next;
		idx++;
	}
}

/**
 * @brief 逆序遍历队列
 *
 * @param [in] queue
 */
void reverse_iter_queue()
{
	mutex_queue_t *queue = &mutex_queue;
	node_t *node = NULL;
	int idx = 1;

	node = queue->head.prev;

	while (node != &queue->head) {
		pr_info("[%d/%ld] val:%d, %p", idx,
			queue->queue_ctr.count, node->data, node);
		node = node->prev;
		idx++;
	}
}

/**
 * @brief 入队
 *
 * @param [in] queue
 * @param [in] val
 * @return int
 */
int mutex_en_queue(int val, size_t total_queue_nodes)
{
	mutex_queue_t *queue = &mutex_queue;
	node_t *node = NULL;

	node = (node_t *)calloc(sizeof(node_t), 1);
	if (NULL == node) {
		pr_stderr("calloc node failed");
		return -errno;
	}

	node->data = val;

	pthread_mutex_lock(&queue->mutex);

	if (queue->queue_ctr.total_enqueue >= total_queue_nodes) {
		free(node);

		/** 达到生产者退出条件 */
		pthread_mutex_unlock(&queue->mutex);
		return 0;
	}

	/** sequence is very important */
	queue->head.next->prev = node;
	node->next = queue->head.next;
	node->prev = &queue->head;
	queue->head.next = node;
	queue->queue_ctr.count += 1;
	queue->queue_ctr.total_enqueue += 1;

	/** 唤醒消费者 */
	pthread_cond_signal(&queue->cond);

	pthread_mutex_unlock(&queue->mutex);

	return 0;
}

/**
 * @brief 出队
 *
 * @param [in] queue
 * @return node_t*
 */
node_t *mutex_de_queue(size_t total_queue_nodes)
{
	mutex_queue_t *queue = &mutex_queue;
	node_t *node = NULL;

	/** 锁定互斥锁 */
	pthread_mutex_lock(&queue->mutex);

	/** 等待条件成立 */
	while (queue->queue_ctr.count == 0 && queue->queue_ctr.shutdown == 0) {
		pthread_cond_wait(&queue->cond, &queue->mutex);
	}

	if (queue->head.prev == &queue->head) {
		pthread_mutex_unlock(&queue->mutex);
		return node;
	}

	node = queue->head.prev;

	/** update ptr */
	node->prev->next = node->next;
	node->next->prev = node->prev;
	queue->queue_ctr.count -= 1;

	/** 达到消费者退出条件 */
	if (queue->queue_ctr.total_enqueue == total_queue_nodes &&
	    queue->queue_ctr.count == 0) {
		pr_info("shutdown!!! total_enqueue:%ld, count:%ld",
			queue->queue_ctr.total_enqueue,
			queue->queue_ctr.count);

		queue->queue_ctr.shutdown = 1;
		pthread_cond_broadcast(&queue->cond);
	}

	/** 解锁互斥锁 */
	pthread_mutex_unlock(&queue->mutex);

	return node;
}

/**
 * @brief 生产者线程
 *
 * @param [in] arg
 * @return void*
 */
void *mutex_queue_produce_func(void *arg)
{
	// queue_t *queue = (queue_t *)arg;
	size_t total_queue_nodes = *(size_t *)arg;
	mutex_queue_t *queue = &mutex_queue;

	/** while 入口中进行大致的判断 */
	while (queue->queue_ctr.total_enqueue < total_queue_nodes) {
		mutex_en_queue((int)random(), total_queue_nodes);
	}
}

/**
 * @brief 消费者线程
 *
 * @param [in] arg
 * @return void*
 */
void *mutex_queue_consume_func(void *arg)
{
	// queue_t *queue = (queue_t *)arg;
	size_t total_queue_nodes = *(size_t *)arg;
	mutex_queue_t *queue = &mutex_queue;
	node_t *node = NULL;

	while (0 == queue->queue_ctr.shutdown && 0 != total_queue_nodes) {
		node = mutex_de_queue(total_queue_nodes);
		if (NULL != node) {
			/** pr_info("val:%d", node->data); */
			free(node);
		}
	}
}

/**
 * @brief Get the mutex queue object
 *
 * @return queue_ctr_t*
 */
queue_ctr_t *get_mutex_queue_ctr()
{
	return (queue_ctr_t *)(&mutex_queue);
}