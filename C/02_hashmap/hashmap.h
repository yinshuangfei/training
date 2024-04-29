/*
 * @Author: Alan Yin
 * @Date: 2024-04-23 20:07:49
 * @LastEditTime: 2024-04-27 23:11:10
 * @LastEditors: Alan Yin
 * @FilePath: /self_project/05_hashmap/hashmap.h
 * @Description:
 * // -*- mode:C; tab-width:8; c-basic-offset:2; indent-tabs-mode:t -*-
 * // vim: ts=8 sw=2 smarttab
 * Copyright (c) 2024 by HT706, All Rights Reserved.
 */

/**
 * hashmap 的作用：
 * 显著减少链表元素很多的时候查找开销的问题。
 *
 * hashmap 的关键元素：
 * （1）hash 函数的设计；
 * （2）hash 冲突的解决方法；
 *
 * 实现哈希表的方法有两种方法：
 * （1）开放寻址法：在开放寻址法中，所有的元素都存储在哈希表的数组中，冲突发生时会探测
 * 下一个可用的位置，直到找到一个空闲的位置。这种方法保持了元素的顺序，但可能导致聚集
 * （clustering）。
 * （2）链地址法：链地址法使用一个数组来存储指向链表头部的指针，每个链表存储具有相同哈希
 * 值的元素。如果发生冲突，新的元素将被添加到该链表的末尾。这种方法可以避免聚集，但不保
 * 持元素的顺序。
 *
 * 本程序使用开放寻址法。
*/

/**
 * https://github.com/tidwall/hashmap.c
*/

#ifndef YSF_HASHMAP_H
#define YSF_HASHMAP_H

#include <stdbool.h>
// #include <stddef.h>
#include <stdint.h>

#if defined(__cplusplus)
extern "C" {
#endif  // __cplusplus

/**
 * 结构体在头文件中声明，但是在 C 文件中中定义，这样可以保护结构体中的信息，外界不能随
 * 意访问，类似面向对象的 protect 属性。
 */
struct hashmap;

struct hashmap *hashmap_new(size_t elsize, size_t cap, uint64_t seed0,
	uint64_t seed1,
	uint64_t (*hash)(const void *item, uint64_t seed0, uint64_t seed1),
	int (*compare)(const void *a, const void *b, void *udata),
	void (*elfree)(void *item),
	void *udata);

struct hashmap *hashmap_new_with_allocator(void *(*malloc)(size_t),
	void *(*realloc)(void *, size_t), void (*free)(void*), size_t elsize,
	size_t cap, uint64_t seed0, uint64_t seed1,
	uint64_t (*hash)(const void *item, uint64_t seed0, uint64_t seed1),
	int (*compare)(const void *a, const void *b, void *udata),
	void (*elfree)(void *item),
	void *udata);

void hashmap_free(struct hashmap *map);
void hashmap_clear(struct hashmap *map, bool update_cap);
size_t hashmap_count(struct hashmap *map);
bool hashmap_oom(struct hashmap *map);
const void *hashmap_get(struct hashmap *map, const void *item);
const void *hashmap_set(struct hashmap *map, const void *item);
const void *hashmap_delete(struct hashmap *map, const void *item);
const void *hashmap_probe(struct hashmap *map, uint64_t position);
bool hashmap_scan(struct hashmap *map,
		  bool (*iter)(const void *item, void *udata), void *udata);
bool hashmap_iter(struct hashmap *map, size_t *i, void **item);

uint64_t hashmap_sip(const void *data, size_t len, uint64_t seed0,
		     uint64_t seed1);
uint64_t hashmap_murmur(const void *data, size_t len, uint64_t seed0,
			uint64_t seed1);
uint64_t hashmap_xxhash3(const void *data, size_t len, uint64_t seed0,
			 uint64_t seed1);

const void *hashmap_get_with_hash(struct hashmap *map, const void *key,
				  uint64_t hash);
const void *hashmap_delete_with_hash(struct hashmap *map, const void *key,
				     uint64_t hash);
const void *hashmap_set_with_hash(struct hashmap *map, const void *item,
				  uint64_t hash);
void hashmap_set_grow_by_power(struct hashmap *map, size_t power);
void hashmap_set_load_factor(struct hashmap *map, double load_factor);

// DEPRECATED: use `hashmap_new_with_allocator`
void hashmap_set_allocator(void *(*malloc)(size_t), void (*free)(void*));

#if defined(__cplusplus)
}
#endif  // __cplusplus

#endif /** YSF_HASHMAP_H */
