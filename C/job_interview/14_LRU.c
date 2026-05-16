/**
 * @file 14_LRU.c
 * @author Alan Yin (yinshuangfei007@163.com)
 * @brief
 * @version 0.0.1
 * @date 2025-05-12
 *
 * @copyright Copyright (c) 2025
 *
 * @node: 设计和实现一个  LRU (最近最少使用) 缓存机制。
 *
 * 设计和实现一个 LRU (最近最少使用) 缓存机制。支持以下操作：
 * - 获取数据 get;
 * 	获取数据 get(key) - 如果关键字 (key) 存在于缓存中，则获取关键字的值
 * 	（总是正数），否则返回 -1。
 * - 写入数据 put;
 * 	写入数据 put(key, value) - 如果关键字已经存在，则变更其数据值；
 * 	如果关键字不存在，则插入该组「关键字/值」。当缓存容量达到上限时，它应该在写入
 * 	新数据之前删除最久未使用的数据值，从而为新的数据值留出空间。
 *
 * 进阶:
 * 你是否可以在 O(1) 时间复杂度内完成这两种操作？
 *
 * 核心思路：哈希链表，即HashMap+双向链表
 *
 * https://blog.csdn.net/hlx20080808/article/details/139079942
 * https://zhuanlan.zhihu.com/p/23737526562
 *
 * 哨兵节点（Dummy Head/Tail）：
 * 在双向链表中引入伪头部和伪尾部，可以极大简化链表在空、满或头尾插入/删除时的边界判断
 * 逻辑，面试官会认为你的代码非常健壮。
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/**
 * @brief 双向链表节点结构
 *
 */
typedef struct DLinkedNode {
	int key;
	int value;
	struct DLinkedNode *prev;
	struct DLinkedNode *next;
} DLinkedNode;

/**
 * @brief LRU缓存结构
 *
 */
typedef struct {
	int capacity;
	int size;
	DLinkedNode *head;  // 伪头部节点
	DLinkedNode *tail;  // 伪尾部节点
	DLinkedNode **cache; // 哈希表
} LRUCache;

/**
 * @brief 创建新节点
 *
 * @param [in out] key
 * @param [in out] value
 * @return
 */
DLinkedNode *create_node(int key, int value)
{
	DLinkedNode *node = (DLinkedNode*)malloc(sizeof(DLinkedNode));
	node->key = key;
	node->value = value;
	node->prev = NULL;
	node->next = NULL;
	return node;
}

/**
 * @brief 初始化 LRU 缓存
 *
 * @param [in out] capacity
 * @return
 */
LRUCache *create_lru(int capacity)
{
	LRUCache *obj = (LRUCache*)malloc(sizeof(LRUCache));
	obj->capacity = capacity;
	obj->size = 0;
	obj->head = create_node(0, 0);
	obj->tail = create_node(0, 0);
	obj->head->next = obj->tail;
	obj->tail->prev = obj->head;

	/**
	 * 初始化哈希表（简单起见，使用数组实现，实际应用中需要更复杂的哈希表）
	 */
	obj->cache = (DLinkedNode**)calloc(10000, sizeof(DLinkedNode*));
	return obj;
}

/**
 * @brief 添加节点到头部
 *
 * @param [in out] obj
 * @param [in out] node
 */
void add_to_head(LRUCache *obj, DLinkedNode *node)
{
	node->prev = obj->head;
	node->next = obj->head->next;
	obj->head->next->prev = node;
	obj->head->next = node;
}

/**
 * @brief 移除节点
 *
 * @param [in out] node
 */
void remove_node(DLinkedNode *node)
{
	node->prev->next = node->next;
	node->next->prev = node->prev;
}

/**
 * @brief 移动节点到头部
 *
 * @param [in out] obj
 * @param [in out] node
 */
void move_to_head(LRUCache *obj, DLinkedNode *node)
{
	remove_node(node);
	add_to_head(obj, node);
}

/**
 * @brief 移除尾部节点
 *
 * @param [in out] obj
 * @return
 */
DLinkedNode *remove_tail(LRUCache *obj)
{
	DLinkedNode *node = obj->tail->prev;
	remove_node(node);
	return node;
}

/**
 * @brief 获取值
 *
 * @param [in out] obj
 * @param [in out] key
 * @return
 */
int get(LRUCache *obj, int key)
{
	if (obj->cache[key] == NULL) {
		return -1;
	}

	// 如果key存在，先通过哈希表定位，再移到头部
	DLinkedNode *node = obj->cache[key];
	move_to_head(obj, node);
	return node->value;
}

/**
 * @brief 插入或更新值
 *
 * @param [in out] obj
 * @param [in out] key
 * @param [in out] value
 */
void put(LRUCache *obj, int key, int value)
{
	if (obj->cache[key] != NULL) {
		// 如果key存在，先通过哈希表定位，再修改value，并移到头部
		DLinkedNode *node = obj->cache[key];
		node->value = value;
		move_to_head(obj, node);
	} else {
		// 如果key不存在，创建一个新的节点
		DLinkedNode *node = create_node(key, value);
		// 添加进哈希表
		obj->cache[key] = node;
		// 添加至双向链表的头部
		add_to_head(obj, node);
		obj->size++;
		if (obj->size > obj->capacity) {
			// 如果超出容量，删除双向链表的尾部节点
			DLinkedNode *removed = remove_tail(obj);
			// 删除哈希表中对应的项
			obj->cache[removed->key] = NULL;
			free(removed);
			obj->size--;
		}
	}
}

/**
 * @brief 释放 LRU 缓存
 *
 * @param [in out] obj
 */
void free_lru(LRUCache *obj)
{
	DLinkedNode *curr = obj->head->next;
	while (curr != obj->tail) {
		DLinkedNode *temp = curr;
		curr = curr->next;
		free(temp);
	}
	free(obj->head);
	free(obj->tail);
	free(obj->cache);
	free(obj);
}

/**
 * @brief 打印 LRU 缓存
 *
 * @param [in out] obj
 */
void print_cache(LRUCache *obj)
{
	DLinkedNode *curr = obj->head->next;
	while (curr != obj->tail) {
		printf("(%d, %d) -> ", curr->key, curr->value);
		curr = curr->next;
	}
	printf("NULL\n");
}

int main()
{
	LRUCache *cache = create_lru(5);

	put(cache, 1, 1);
	put(cache, 2, 2);
	put(cache, 3, 3);
	put(cache, 4, 4);
	put(cache, 5, 5);
	put(cache, 6, 6);

	/** 打印 */
	print_cache(cache);

	/** 获取值 */
	printf("get key:%d -> value: %d\n", 2, get(cache, 2));
	/** 更新值 */
	put(cache, 3, 13);
	/** 获取值 */
	printf("get key:%d -> value: %d\n", 12, get(cache, 12));

	/** 打印 */
	print_cache(cache);

	free_lru(cache);
	return 0;
}


