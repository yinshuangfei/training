/**
 * @file comm.h
 * @author Alan Yin (yinshuangfei007@163.com)
 * @brief
 * @version 0.0.1
 * @date 2025-05-02
 *
 * @copyright Copyright (c) 2025
 *
 * @node:
 */

#ifndef JOB_COMMON_H
#define JOB_COMMON_H

#include <stdio.h>
#include <stdlib.h>

typedef struct Node {
	int val;
	struct Node *next;
} node_t;

struct Node *init_linked_list(int data[], int num);
void pr_linked_list(char *tag, struct Node *tmp);

#endif /** JOB_COMMON_H */