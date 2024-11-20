/*
 * @Author: Alan Yin
 * @Date: 2024-07-19 09:43:27
 * @LastEditTime: 2024-07-19 11:24:26
 * @LastEditors: Alan Yin
 * @FilePath: /02_nas_mgmt/yy/test_main.c
 * @Description:
 * // -*- mode:C; tab-width:8; c-basic-offset:2; indent-tabs-mode:t -*-
 * // vim: ts=8 sw=2 smarttab
 * Copyright (c) 2024 by HT706, All Rights Reserved.
 */

#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <stdbool.h>
#include <errno.h>
#include <string.h>

#include "rbtree_ka.h"
#include "container.h"

// TODO: root 和 mytree 区分一下

typedef struct mytype {
	struct rb_node node;
	char keystring[NAME_MAX];
	long int key;
} mytype_t;

static struct mytype *mytype_search(struct rb_root *root, char *string)
{
	struct rb_node *node = root->rb_node;

	while (node) {
		struct mytype *data = container_of(node, struct mytype, node);
		int result;

		/** for string */
		// result = strcmp(string, data->keystring);

		/** for long int */
		long int key = atol(string);
		if (key == data->key)
			result = 0;
		else if (key < data->key)
			result = -1;
		else
			result = 1;

		if (result < 0)
			node = node->rb_left;
		else if (result > 0)
			node = node->rb_right;
		else
			return data;
	}
	return NULL;
}

/**
 * @description: 插入红黑树
 * @param {rb_root} *root
 * @param {mytype} *data
 * @return {*}
 */
static int mytype_insert(struct rb_root *root, struct mytype *data)
{
	struct rb_node **new = &(root->rb_node), *parent = NULL;

	/* Figure out where to put new node */
	while (*new) {
		struct mytype *this = container_of(*new, struct mytype, node);

		/** for string */
		// int result = strcmp(data->keystring, this->keystring);

		/** for long int */
		int result = 0;
		if (data->key == this->key)
			result = 0;
		else if (data->key < this->key)
			result = -1;
		else
			result = 1;

		parent = *new;
		if (result < 0)
			new = &((*new)->rb_left);
		else if (result > 0)
			new = &((*new)->rb_right);
		else
			return false;
	}

	/* Add new node and rebalance tree. */
	rb_link_node(&data->node, parent, new);
	rb_insert_color(&data->node, root);

	return true;
}

int test_inster_mynode(struct rb_root *root, long int key)
{
	struct mytype *mynode = NULL;

	mynode = calloc(1, sizeof(struct mytype));
	if (mynode == NULL) {
		perror("no memory");
		return -ENOMEM;
	}

	sprintf(mynode->keystring, "%ld", key);
	mynode->key = key;
	if (!mytype_insert(root, mynode)) {
		printf("inster %s error\n", mynode->keystring);
		free(mynode);
		return -EPERM;
	}

	return 0;
}

void test_search_mytree(struct rb_root *root, char *string)
{
	struct mytype *node = NULL;

	node = mytype_search(root, string);
	if (node == NULL)
		printf("not find ley %s\n", string);
	else
		printf("find key %s\n", node->keystring);
}

void test_erase_mynode(struct rb_root *root, char *string)
{
	struct mytype *data = mytype_search(root, string);

	if (data) {
		rb_erase(&data->node, root);
		free(data);
	}
}

void test_replace_mynode(struct rb_root *root, char *string)
{
	struct mytype *new_node = NULL;
	struct mytype *data = mytype_search(root, string);

	if (!data) {
		printf("not find ley %s\n", string);
		return ;
	}

	new_node = calloc(1, sizeof(struct mytype));
	if (new_node == NULL) {
		perror("no memory");
		return ;
	}

	sprintf(new_node->keystring, "%s333", string);
	new_node->key = atol(string);

	rb_replace_node(&data->node, &new_node->node, root);

	free(data);
}

void test_iter_mytree(struct rb_root *root)
{
	struct mytype *my = NULL;

	rb_for_each_entry(my, root, node) {
		printf("key=%s\n", my->keystring);
	}

	printf("--------------------\n");
}

void test_free_all(struct rb_root *root)
{
	struct mytype *my = NULL;
	struct mytype *my_next = NULL;

	rb_for_each_entry_safe(my, my_next, root, node) {
		printf("key=%s\n", my->keystring);

		rb_erase(&my->node, root);
		free(my);
	}

	printf("x-------------------\n");
}

int main()
{
	struct rb_root mytree = RB_ROOT;

	/** 插入操作 */
	test_inster_mynode(&mytree, 10);
	test_inster_mynode(&mytree, 1);
	test_inster_mynode(&mytree, 11111);
	test_inster_mynode(&mytree, 5);
	test_inster_mynode(&mytree, 27);
	test_inster_mynode(&mytree, 5);

	/** 查询操作 */
	test_search_mytree(&mytree, "10");

	/** 遍历操作 */
	test_iter_mytree(&mytree);

	/** 删除操作 */
	test_erase_mynode(&mytree, "10");
	test_iter_mytree(&mytree);

	/** 覆盖操作 */
	test_replace_mynode(&mytree, "11111");
	test_iter_mytree(&mytree);

	/** 释放所有节点 */
	test_free_all(&mytree);

	return 0;
}
