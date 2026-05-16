/**
 * @file 17_LFU.c
 * @author Alan Yin (yinshuangfei007@163.com)
 * @brief
 * @version 0.0.1
 * @date 2026-05-13
 *
 * @copyright Copyright (c) 2026
 *
 * @node:
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define HASH_SIZE 20001 // 哈希表容量（开大一点减少冲突，保证O(1)）
#define INF 2000000000  // 表示哈希表槽位为空

// 1. 双向链表节点
typedef struct Node {
    int key, val, freq;
    struct Node *prev, *next;
} Node;

// 2. 频率桶（双向链表的哨兵节点）
typedef struct {
    Node *head, *tail;
} FreqList;

// 3. LFU 缓存主结构
typedef struct {
    int capacity;
    int size;
    int minFreq;

    // 手动实现的两个静态哈希表
    Node* key_map[HASH_SIZE];      // key -> Node*
    FreqList* freq_map[HASH_SIZE]; // freq -> FreqList*
} LFUCache;

// --- 基础工具函数 ---

// 简易哈希函数
int getHashIndex(int key) {
    return (key % HASH_SIZE + HASH_SIZE) % HASH_SIZE;
}

// 创建新节点
Node* createNode(int key, int val, int freq) {
    Node* node = (Node*)malloc(sizeof(Node));
    node->key = key; node->val = val; node->freq = freq;
    node->prev = node->next = NULL;
    return node;
}

// 初始化频率桶（带哨兵节点）
FreqList* createFreqList() {
    FreqList* list = (FreqList*)malloc(sizeof(FreqList));
    list->head = createNode(0, 0, 0);
    list->tail = createNode(0, 0, 0);
    list->head->next = list->tail;
    list->tail->prev = list->head;
    return list;
}

// 从链表中删除节点
void removeNode(Node* node) {
    node->prev->next = node->next;
    node->next->prev = node->prev;
}

// 将节点插入到链表头部
void addNodeToHead(FreqList* list, Node* node) {
    node->next = list->head->next;
    node->prev = list->head;
    list->head->next->prev = node;
    list->head->next = node;
}

// --- LFU 核心逻辑 ---

LFUCache* lFUCacheCreate(int capacity) {
    LFUCache* obj = (LFUCache*)malloc(sizeof(LFUCache));
    obj->capacity = capacity;
    obj->size = 0;
    obj->minFreq = 0;
    // 初始化哈希表为空
    memset(obj->key_map, 0, sizeof(obj->key_map));
    memset(obj->freq_map, 0, sizeof(obj->freq_map));
    return obj;
}

// 增加节点频率的核心逻辑
void increaseFreq(LFUCache* obj, Node* node) {
    int oldFreq = node->freq;
    int newFreq = oldFreq + 1;

    // 1. 从旧的频率桶中移除
    FreqList* oldList = obj->freq_map[getHashIndex(oldFreq)];
    removeNode(node);

    // 如果旧桶空了，且旧桶的频率正好是当前最小频率，则更新 minFreq
    if (oldList->head->next == oldList->tail) {
        if (obj->minFreq == oldFreq) {
            obj->minFreq = newFreq;
        }
        // 释放空的频率桶内存
        free(oldList->head); free(oldList->tail); free(oldList);
        obj->freq_map[getHashIndex(oldFreq)] = NULL;
    }

    // 2. 加入新的频率桶
    FreqList* newList = obj->freq_map[getHashIndex(newFreq)];
    if (!newList) {
        newList = createFreqList();
        obj->freq_map[getHashIndex(newFreq)] = newList;
    }
    addNodeToHead(newList, node);

    // 3. 更新节点的频率
    node->freq = newFreq;
}

int lFUCacheGet(LFUCache* obj, int key) {
    if (obj->capacity == 0) return -1;

    int idx = getHashIndex(key);
    // 线性探测查找 key
    while (obj->key_map[idx] != NULL) {
        if (obj->key_map[idx]->key == key) {
            Node* node = obj->key_map[idx];
            increaseFreq(obj, node); // 命中，增加频率
            return node->val;
        }
        idx = (idx + 1) % HASH_SIZE;
    }
    return -1;
}

void lFUCachePut(LFUCache* obj, int key, int value) {
    if (obj->capacity == 0) return;

    int idx = getHashIndex(key);
    int startIdx = idx;
    Node* existNode = NULL;
    int existIdx = -1;

    // 线性探测查找 key 是否存在
    while (obj->key_map[idx] != NULL) {
        if (obj->key_map[idx]->key == key) {
            existNode = obj->key_map[idx];
            existIdx = idx;
            break;
        }
        idx = (idx + 1) % HASH_SIZE;
    }

    // 情况1：Key 已存在，更新值并增加频率
    if (existNode) {
        existNode->val = value;
        increaseFreq(obj, existNode);
        return;
    }

    // 情况2：Key 不存在，且缓存已满，需要淘汰
    if (obj->size >= obj->capacity) {
        // 找到最小频率的桶
        FreqList* minList = obj->freq_map[getHashIndex(obj->minFreq)];

        // 淘汰该桶尾部的节点（最久未使用）
        Node* evictNode = minList->tail->prev;
        removeNode(evictNode);

        // 从 key_map 中删除
        int evictIdx = getHashIndex(evictNode->key);
        while (obj->key_map[evictIdx]->key != evictNode->key) {
            evictIdx = (evictIdx + 1) % HASH_SIZE;
        }
        obj->key_map[evictIdx] = NULL;

        free(evictNode);
        obj->size--;
    }

    // 插入新节点（频率为 1）
    Node* newNode = createNode(key, value, 1);
    // 线性探测寻找空槽位插入
    int insertIdx = getHashIndex(key);
    while (obj->key_map[insertIdx] != NULL) {
        insertIdx = (insertIdx + 1) % HASH_SIZE;
    }
    obj->key_map[insertIdx] = newNode;

    // 放入频率为 1 的桶
    FreqList* freq1List = obj->freq_map[getHashIndex(1)];
    if (!freq1List) {
        freq1List = createFreqList();
        obj->freq_map[getHashIndex(1)] = freq1List;
    }
    addNodeToHead(freq1List, newNode);

    obj->minFreq = 1; // 新插入的节点频率必为 1
    obj->size++;
}

void lFUCacheFree(LFUCache* obj) {
    // 释放所有频率桶和节点内存
    for (int i = 0; i < HASH_SIZE; i++) {
        if (obj->freq_map[i] != NULL) {
            Node* cur = obj->freq_map[i]->head;
            while (cur) {
                Node* tmp = cur;
                cur = cur->next;
                free(tmp);
            }
            free(obj->freq_map[i]);
        }
    }
    free(obj);
}

// --- 测试 Main 函数 ---
int main() {
    LFUCache* cache = lFUCacheCreate(2);

    lFUCachePut(cache, 1, 1);
    lFUCachePut(cache, 2, 2);
    printf("get(1): %d\n", lFUCacheGet(cache, 1)); // 返回 1

    lFUCachePut(cache, 3, 3); // 淘汰键 2
    printf("get(2): %d\n", lFUCacheGet(cache, 2)); // 返回 -1
    printf("get(3): %d\n", lFUCacheGet(cache, 3)); // 返回 3

    lFUCachePut(cache, 4, 4); // 淘汰键 1
    printf("get(1): %d\n", lFUCacheGet(cache, 1)); // 返回 -1
    printf("get(3): %d\n", lFUCacheGet(cache, 3)); // 返回 3
    printf("get(4): %d\n", lFUCacheGet(cache, 4)); // 返回 4

    lFUCacheFree(cache);
    return 0;
}