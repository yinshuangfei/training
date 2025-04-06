/**
 * @file queue.h
 * @author Alan Yin (yinshuangfei007@163.com)
 * @brief
 * @version 0.0.1
 * @date 2025-03-09
 *
 * @copyright Copyright (c) 2025
 *
 * @node:
 */

#ifndef LOCK_QUEUE_H
#define LOCK_QUEUE_H

#include <stdlib.h>

/** 队列控制结构 */
typedef struct queue_ctr {
	size_t count;
	size_t total_enqueue;
	int shutdown;		/** 消费者退出 */
} queue_ctr_t;

#endif /** LOCK_QUEUE_H */