// /*
//  * @Author: Alan Yin
//  * @Date: 2024-04-21 22:42:50
//  * @LastEditTime: 2024-04-22 00:01:04
//  * @LastEditors: Alan Yin
//  * @FilePath: /self_project/04_lockfree_queue/lockfree_queue.c
//  * @Description:
//  * // -*- mode:C; tab-width:8; c-basic-offset:2; indent-tabs-mode:t -*-
//  * // vim: ts=8 sw=2 smarttab
//  * Copyright (c) 2024 by HT706, All Rights Reserved.
//  */

// https://legacy.cplusplus.com/reference/atomic/atomic_load_explicit/
// atomic_load_explicit
// 在头文件<stdatomic.h>中定义
// atomic_load（const volatile A * obj）;
// atomic_load_explicit（const volatile A * obj，memory_order order）;
// explicit：明确的，一目了然的
// 以原子方式加载并返回指向的原子变量的当前值obj。
// 第一个版本根据命令对内存进行访问memory_order_seq_cst，第二个版本根据内存访问内存访问order
// order必须是一个memory_order_relaxed，memory_order_consume，memory_order_acquire或memory_order_seq_cst。否则，行为是不确定的。

// 所有 order 如下：
// enum memory_order {
	// memory_order_relaxed，它表示不需要任何额外的同步，对内存顺序没有要求。这意味着对原子操作的操作可以以任意顺序进行，不会引入数据竞争或同步问题。
	// memory_order_consume，
	// memory_order_acquire，它确保该原子操作的读取操作具有获取语义，即确保所有在memory_order_acquire之前的读取操作在此原子操作之前完成。
	// memory_order_release，它确保该原子操作的写入操作具有释放语义，即确保此原子操作完成之后，所有在memory_order_release之后的写入操作可见。
	// memory_order_acq_rel，
	// memory_order_seq_cst
// };

// atomic_compare_exchange_strong(volatile A * obj，C * expected，C desired，memory_order succ，memory_order fail) 的行为就好像下面的代码是以原子方式执行的：
// if (memcmp(obj, expected, sizeof *obj) == 0)
    // memcpy(obj, &desired, sizeof *obj);
// else
    // memcpy(expected, obj, sizeof *obj);

#include <stdatomic.h>
#include <stdbool.h>
#include <stdlib.h>

#include "lockfree_queue.h"
#include "../common/utils.h"

// /** 队列节点 */
// typedef struct node {
// 	atomic_uintptr_t head;
// 	atomic_uintptr_t tail;
// 	int data;
// } node_t;

// /** 无锁队列结构 */
// typedef struct lockfree_queue {
// 	/** 需要放首位 */
// 	queue_ctr_t queue_ctr;

// 	struct node head;
// } lockfree_queue_t;


/** 队列节点 */
typedef struct node {
	// void *data;
	int data;
	struct node *next;
} node_t;

/** 无锁队列结构 */
typedef struct lockfree_queue {
	/** 需要放首位 */
	queue_ctr_t queue_ctr;

	atomic_uintptr_t head;
	atomic_uintptr_t tail;
} lockfree_queue_t;

lockfree_queue_t lockfree_queue;

void init_lockfree_queue()
{
	lockfree_queue_t *queue = &lockfree_queue;

	queue->queue_ctr.count = 0;
	queue->queue_ctr.total_enqueue = 0;
	queue->queue_ctr.shutdown = 0;

	node_t *node = malloc(sizeof(node_t));
	node->next = NULL;
	node->data = -1;

	queue->head = atomic_load(&node);
	queue->tail = atomic_load(&node);
}

void destroy_lockfree_queue()
{
	lockfree_queue_t *queue = &lockfree_queue;
	// free();
}

void lockfree_en_queue(int val, size_t total_queue_nodes)
{
	lockfree_queue_t *queue = &lockfree_queue;

	node_t *node = malloc(sizeof(node_t));
	node->data = val;
	node->next = NULL;

	/** 使用原子操作更新尾指针 */
	node_t *prev_tail;
	node_t *expected_node = NULL;

	while (true) {
		prev_tail = atomic_load_explicit(&queue->tail,
						 memory_order_relaxed);
		expected_node = atomic_load(&prev_tail->next);
		if (atomic_compare_exchange_weak_explicit(
				&prev_tail->next, &expected_node, node,
				memory_order_release, memory_order_relaxed)) {
			/** 入队成功，退出循环 */
			break;
		} else {
			/** 更新尾指针失败，可能有其他线程正在更新尾指针，重试 */
			atomic_compare_exchange_weak_explicit(
				&queue->tail, &prev_tail, prev_tail->next,
				memory_order_relaxed, memory_order_relaxed);
		}
	}

	/** 更新尾指针 */
	atomic_compare_exchange_weak_explicit(
		&queue->tail, &prev_tail,
		node, memory_order_relaxed, memory_order_relaxed);
}

int lockfree_de_queue(size_t total_queue_nodes)
{
	lockfree_queue_t *queue = &lockfree_queue;

	node_t *prev_head;
	int data;

	while (true) {
		prev_head = atomic_load_explicit(&queue->head,
				memory_order_acquire);
		node_t *next = atomic_load_explicit(&prev_head->next,
				memory_order_relaxed);

		if (next == NULL) {
			return NULL;
		}

		data = next->data;

		/** 出队成功，退出循环 */
		if (atomic_compare_exchange_weak_explicit(
				&queue->head, &prev_head, next,
				memory_order_release, memory_order_relaxed)) {
			break;
		}
	}

	free(prev_head);
	return data;
}

/**
 * @brief 生产者线程
 *
 * @param [in] arg
 * @return void*
 */
void *lockfree_queue_produce_func(void *arg)
{
	size_t total_queue_nodes = *(size_t *)arg;
	lockfree_queue_t *queue = &lockfree_queue;

	/** while 入口中进行大致的判断 */
	while (queue->queue_ctr.total_enqueue < total_queue_nodes) {
		lockfree_en_queue((int)random(), total_queue_nodes);
	}
}

/**
 * @brief 消费者线程
 *
 * @param [in] arg
 * @return void*
 */
void *lockfree_queue_consume_func(void *arg)
{
	size_t total_queue_nodes = *(size_t *)arg;
	lockfree_queue_t *queue = &lockfree_queue;
	node_t *node = NULL;

	while (0 == queue->queue_ctr.shutdown && 0 != total_queue_nodes) {
		lockfree_de_queue(total_queue_nodes);
		// node = lockfree_de_queue(total_queue_nodes);
		// if (NULL != node) {
		// 	/** pr_info("val:%d", node->data); */
		// 	free(node);
		// }
	}
}

queue_ctr_t *get_lockfree_queue_ctr()
{
	return (queue_ctr_t *)(&lockfree_queue);
}

// #include <stdio.h>
// #include <stdlib.h>
// #include <stdbool.h>
// #include <stdatomic.h>

// #include "../common/utils.h"

// // 定义链表节点结构
// typedef struct Node {
// 	int data;
// 	struct Node *next;
// } Node;

// // 定义无锁队列结构
// typedef struct LockFreeQueue {
//     _Atomic Node* head;  // 队列头部原子指针
//     _Atomic Node* tail;  // 队列尾部原子指针
// } LockFreeQueue;

// // 初始化队列
// void initQueue(LockFreeQueue* queue)
// {
//     Node* dummy = (Node*)malloc(sizeof(Node));
//     dummy->next = NULL;
//     atomic_init(&queue->head, dummy);
//     atomic_init(&queue->tail, dummy);
// }

// // 入队操作
// void enqueue(LockFreeQueue* queue, int data) {
//     Node* newNode = (Node*)malloc(sizeof(Node));
//     newNode->data = data;
//     newNode->next = NULL;

//     // 获取当前队列尾部
//     Node* currentTail = atomic_load(&queue->tail);

//     // 将新节点链接到队列尾部（使用CAS循环确保原子性）
//     while (true) {
//         atomic_store(&newNode->next, atomic_load(&currentTail->next));
//         if (atomic_compare_exchange_weak(&currentTail->next, &newNode->next, newNode)) {
//             // 更新队列尾部
//             atomic_store(&queue->tail, newNode);
//             break;
//         }
//         // 如果失败，重新获取当前队列尾部
//         currentTail = atomic_load(&queue->tail);
//     }
// }

// // 出队操作
// bool dequeue(LockFreeQueue* queue, int* data) {
//     // 获取当前队列头部和尾部
//     Node* currentHead = atomic_load(&queue->head);
//     Node* currentTail = atomic_load(&queue->tail);
//     Node* nextNode = atomic_load(&currentHead->next);

//     // 如果队列为空，返回false
//     if (currentHead == currentTail) {
//         return false;
//     }

//     // 尝试移动队列头部（使用CAS循环确保原子性）
//     while (true) {
//         if (atomic_compare_exchange_weak(&queue->head, &currentHead, nextNode)) {
//             // 读取数据
//             *data = nextNode->data;
//             // 释放旧头部节点
//             free(currentHead);
//             return true;
//         }
//         // 如果失败，重新获取当前队列头部和下一个节点
//         currentHead = atomic_load(&queue->head);
//         nextNode = atomic_load(&currentHead->next);
//     }
// }

// // 销毁队列
// void destroyQueue(LockFreeQueue* queue) {
//     // 释放所有剩余节点
//     Node* current = atomic_load(&queue->head);
//     while (current != NULL) {
//         Node* next = current->next;
//         free(current);
//         current = next;
//     }
// }

// int main() {
//     LockFreeQueue queue;
//     initQueue(&queue);

//     // 示例：多线程环境下使用队列
//     // 注意：此处仅为示例，实际多线程使用需要配合线程创建和同步机制
//     enqueue(&queue, 10);
//     enqueue(&queue, 20);
//     enqueue(&queue, 30);

//     int data;
//     while (dequeue(&queue, &data)) {
//         printf("Dequeued data: %d\n", data);
//     }

//     destroyQueue(&queue);
//     return 0;
// }