/**
 * @file mutex_queue.h
 * @author Alan Yin (yinshuangfei007@163.com)
 * @brief
 * @version 0.0.1
 * @date 2025-03-08
 *
 * @copyright Copyright (c) 2025
 *
 * @node:
 */

#ifndef MUTEX_LOCK_QUEUE_H
#define MUTEX_LOCK_QUEUE_H

#include "queue.h"

/** 队列节点 */
typedef struct node {
	struct node *next;
	struct node *prev;
	int data;
} node_t;

typedef struct mutex_queue {
	/** 需要放首位 */
	queue_ctr_t queue_ctr;

	struct node head;
	pthread_mutex_t mutex;
	pthread_cond_t cond;
} mutex_queue_t;

void init_mutex_queue();
void destroy_mutex_queue();

void iter_queue();
void reverse_iter_queue();

int mutex_en_queue(int val, size_t total_queue_nodes);
node_t *mutex_de_queue(size_t total_queue_nodes);

void *mutex_queue_produce_func(void *arg);
void *mutex_queue_consume_func(void *arg);

queue_ctr_t *get_mutex_queue_ctr();

#endif /** MUTEX_LOCK_QUEUE_H */