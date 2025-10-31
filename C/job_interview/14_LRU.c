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
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// 双向链表节点
typedef struct DLinkedNode {
	int key;
	int value;
	struct DLinkedNode* prev;
	struct DLinkedNode* next;
} DLinkedNode;

// LRU缓存结构
typedef struct {
	int capacity;
	int size;
	DLinkedNode* head;  // 伪头部节点
	DLinkedNode* tail;  // 伪尾部节点
	DLinkedNode** cache; // 哈希表
} LRUCache;

// 创建新节点
DLinkedNode* createNode(int key, int value)
{
	DLinkedNode* node = (DLinkedNode*)malloc(sizeof(DLinkedNode));
	node->key = key;
	node->value = value;
	node->prev = NULL;
	node->next = NULL;
	return node;
}

// 初始化LRU缓存
LRUCache* lRUCacheCreate(int capacity)
{
	LRUCache* obj = (LRUCache*)malloc(sizeof(LRUCache));
	obj->capacity = capacity;
	obj->size = 0;
	obj->head = createNode(0, 0);
	obj->tail = createNode(0, 0);
	obj->head->next = obj->tail;
	obj->tail->prev = obj->head;

	// 初始化哈希表（简单起见，使用数组实现，实际应用中可能需要更复杂的哈希表）
	obj->cache = (DLinkedNode**)calloc(10000, sizeof(DLinkedNode*));
	return obj;
}

// 添加节点到头部
void addToHead(LRUCache* obj, DLinkedNode* node) {
	node->prev = obj->head;
	node->next = obj->head->next;
	obj->head->next->prev = node;
	obj->head->next = node;
}

// 移除节点
void removeNode(DLinkedNode* node) {
	node->prev->next = node->next;
	node->next->prev = node->prev;
}

// 移动节点到头部
void moveToHead(LRUCache* obj, DLinkedNode* node) {
	removeNode(node);
	addToHead(obj, node);
}

// 移除尾部节点
DLinkedNode* removeTail(LRUCache* obj) {
	DLinkedNode* node = obj->tail->prev;
	removeNode(node);
	return node;
}

// 获取值
int lRUCacheGet(LRUCache* obj, int key) {
	if (obj->cache[key] == NULL) {
		return -1;
	}

	// 如果key存在，先通过哈希表定位，再移到头部
	DLinkedNode* node = obj->cache[key];
	moveToHead(obj, node);
	return node->value;
}

// 插入或更新值
void lRUCachePut(LRUCache* obj, int key, int value) {
	if (obj->cache[key] != NULL) {
		// 如果key存在，先通过哈希表定位，再修改value，并移到头部
		DLinkedNode* node = obj->cache[key];
		node->value = value;
		moveToHead(obj, node);
	} else {
		// 如果key不存在，创建一个新的节点
		DLinkedNode* node = createNode(key, value);
		// 添加进哈希表
		obj->cache[key] = node;
		// 添加至双向链表的头部
		addToHead(obj, node);
		obj->size++;
		if (obj->size > obj->capacity) {
			// 如果超出容量，删除双向链表的尾部节点
			DLinkedNode* removed = removeTail(obj);
			// 删除哈希表中对应的项
			obj->cache[removed->key] = NULL;
			free(removed);
			obj->size--;
		}
	}
}

// 释放LRU缓存
void lRUCacheFree(LRUCache* obj) {
	DLinkedNode* curr = obj->head->next;
	while (curr != obj->tail) {
		DLinkedNode* temp = curr;
		curr = curr->next;
		free(temp);
	}
	free(obj->head);
	free(obj->tail);
	free(obj->cache);
	free(obj);
}

int main() {
	LRUCache* cache = lRUCacheCreate(100);

	lRUCachePut(cache, 1, 1);
	lRUCachePut(cache, 2, 2);
	lRUCachePut(cache, 3, 3);
	lRUCachePut(cache, 4, 4);
	lRUCachePut(cache, 5, 5);
	lRUCachePut(cache, 6, 6);
	// printf("%d\n", lRUCacheGet(cache, 1)); // 返回 1
	// lRUCachePut(cache, 3, 3);             // 该操作会使得关键字 2 作废
	// printf("%d\n", lRUCacheGet(cache, 2)); // 返回 -1 (未找到)
	// lRUCachePut(cache, 4, 4);             // 该操作会使得关键字 1 作废
	// printf("%d\n", lRUCacheGet(cache, 1)); // 返回 -1 (未找到)
	// printf("%d\n", lRUCacheGet(cache, 3)); // 返回 3
	// printf("%d\n", lRUCacheGet(cache, 4)); // 返回 4

	printf("tail %d\n", cache->tail->prev->key);

	lRUCacheFree(cache);
	return 0;
}


