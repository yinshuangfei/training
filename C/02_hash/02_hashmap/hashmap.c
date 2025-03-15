/*
 * @Author: Alan Yin
 * @Date: 2024-04-23 20:07:35
 * @LastEditTime: 2024-04-29 22:24:23
 * @LastEditors: Alan Yin
 * @FilePath: /self_project/05_hashmap/hashmap.c
 * @Description:
 * // -*- mode:C; tab-width:8; c-basic-offset:2; indent-tabs-mode:t -*-
 * // vim: ts=8 sw=2 smarttab
 * Copyright (c) 2024 by Alan Yin, All Rights Reserved.
 */


#include <stdio.h>
#include <string.h>
#include <stdlib.h>
// #include <stddef.h>
#include "hashmap.h"

#define GROW_AT		0.60 /* 60% */
#define SHRINK_AT	0.10 /* 10% */

#ifndef HASHMAP_LOAD_FACTOR
#define HASHMAP_LOAD_FACTOR GROW_AT
#endif

/** 内部函数指针，全局变量 */
static void *(*__malloc)(size_t) = NULL;
static void *(*__realloc)(void *, size_t) = NULL;
static void (*__free)(void *) = NULL;

/** 提升可配置性，设置自定义哈希分配和释放的函数 */
void hashmap_set_allocator(void *(*malloc)(size_t), void (*free)(void*))
{
	__malloc = malloc;
	__free = free;
}

/** 哈希桶 */
struct bucket {
	uint64_t hash:48;			// 哈希值
	uint64_t dib:16;			// 哈希计算次数
						// 当前哈希值存储的 element 数目?
};

/**
 * 开放寻址 hashmap, 使用 robinhood hash 算法
 *
 * 结构体在头文件中声明，但是在 C 文件中定义，这样可以保护结构体中的信息，外界不能随
 * 意访问，类似面向对象的 protect 属性。如 map->oom 的访问将导致编译失败。
 */
struct hashmap {
	void *(*malloc)(size_t);
	void *(*realloc)(void *, size_t);
	void (*free)(void *);
	size_t elsize;				// element size（存储对象）
	/** nbuckets > cap 才进行自动缩容 */
	size_t cap;				// 默认桶的数量（初始容量）
	uint64_t seed0;				// 种子0
	uint64_t seed1;				// 种子1
	/** 哈希计算函数 */
	uint64_t (*hash)(const void *item, uint64_t seed0, uint64_t seed1);
	/** 哈希比较函数 */
	int (*compare)(const void *a, const void *b, void *udata);
	/** element 释放函数, 针对复杂的结构体需要专门的释放函数 */
	void (*elfree)(void *item);
	void *udata;				// 用户数据
	/** bucketsz = sizeof(struct bucket) + elsize */
	size_t bucketsz;			// 单个桶的大小
	size_t nbuckets;			// 桶的数量，表示 buckets 的数目
						// 数量会动态变化
	size_t count;				// 存储的元素计数
	size_t mask;				// 掩码，桶的数量 -1，最大哈希值
	/**
	 * map->count >= map->growat 时，既元素数量大于 growat 时，调用 resize
	 * 元素数量到达桶数的比例，增长哈希表
	 */
	size_t growat;				// 扩充桶的阈值线
	/** 元素数目只有桶数的 10% 时，缩减桶 */
	size_t shrinkat;			// 缩减桶的阈值线
	/**
	 * 装载因子表示哈希表中已存储键值对的数量与桶的总数之比。装载因子越大，冲突可能
	 * 越多，性能可能下降。为了保持性能，通常需要在装载因子达到某个阈值时，对哈希表
	 * 进行调整
	 * */
	uint8_t loadfactor;			// 装载因子(item 占桶的数量)
	/** growpower 的用法 nbuckets*(1<<map->growpower) */
	uint8_t growpower;			// 增长单位：2 的幂
	bool oom;				// 内存不足（out of memery）
	void *buckets;				// 桶的空间指针
	void *spare;				// spare 桶指针, 暂存桶
	void *edata;				// edata 桶指针, element data
	/**
	 * -----------------
	 * bucketsz -> spare
	 * -----------------
	 * bucketsz -> edata
	 * -----------------
	*/
};

/** 扩展哈希空间 */
void hashmap_set_grow_by_power(struct hashmap *map, size_t power)
{
	map->growpower = power < 1 ? 1 : power > 16 ? 16 : power;
}

/** 标准化（两端压缩）load factor */
static double clamp_load_factor(double factor, double default_factor)
{
	// Check for NaN and clamp between 50% and 90%
	/** 如果 factor 未初始化，会导致 factor != factor ??? */
	return factor != factor ? default_factor :
		factor < 0.50 ? 0.50 :
		factor > 0.95 ? 0.95 :
		factor;
}

/** 设置 load factor */
void hashmap_set_load_factor(struct hashmap *map, double factor)
{
	factor = clamp_load_factor(factor, map->loadfactor / 100.0);
	map->loadfactor = factor * 100;
	map->growat = map->nbuckets * (map->loadfactor / 100.0);
}

/** 内部计算哈希桶的位置（所有的哈希类似一个数组） */
static struct bucket *bucket_at0(void *buckets, size_t bucketsz, size_t i)
{
	return (struct bucket*)(((char*)buckets)+(bucketsz*i));
}

/** 根据 index 索引哈希桶的地址 */
static struct bucket *bucket_at(struct hashmap *map, size_t index)
{
	return bucket_at0(map->buckets, map->bucketsz, index);
}

/** 当前桶后的元素指针???，指向 item | element */
static void *bucket_item(struct bucket *entry)
{
	return ((char*)entry)+sizeof(struct bucket);
}

/** 限定哈希范围 */
static uint64_t clip_hash(uint64_t hash)
{
	return hash & 0xFFFFFFFFFFFF;
}

/** 获取|计算哈希值 */
static uint64_t get_hash(struct hashmap *map, const void *key)
{
	return clip_hash(map->hash(key, map->seed0, map->seed1));
}

/** 使用自定义分配器创建哈希 map */
// hashmap_new_with_allocator returns a new hash map using a custom allocator.
// See hashmap_new for more information information
struct hashmap *hashmap_new_with_allocator(void *(*_malloc)(size_t),
	void *(*_realloc)(void*, size_t), void (*_free)(void*),
	size_t elsize, size_t cap, uint64_t seed0, uint64_t seed1,
	uint64_t (*hash)(const void *item, uint64_t seed0, uint64_t seed1),
	int (*compare)(const void *a, const void *b, void *udata),
	void (*elfree)(void *item),
	void *udata)
{
	/** 两重自定义函数的选择 */
	_malloc = _malloc ? _malloc : __malloc ? __malloc : malloc;
	_realloc = _realloc ? _realloc : __realloc ? __realloc : realloc;
	_free = _free ? _free : __free ? __free : free;

	/** 16 对齐 */
	size_t ncap = 16;
	if (cap < ncap) {
		cap = ncap;
	} else {
		while (ncap < cap) {
			ncap *= 2;
		}
		cap = ncap;
	}
	/** 桶size = 桶的结构体 + element size */
	size_t bucketsz = sizeof(struct bucket) + elsize;
	/** 对齐 uintptr_t 结构体的大小 */
	while (bucketsz & (sizeof(uintptr_t)-1)) {
		bucketsz++;
	}
	/** hashmap + spare + edata
	 * 额外分配了两个桶的空间
	*/
	size_t size = sizeof(struct hashmap)+bucketsz*2;
	struct hashmap *map = _malloc(size);
	if (!map) {
		return NULL;
	}
	memset(map, 0, sizeof(struct hashmap));
	map->elsize = elsize;
	map->bucketsz = bucketsz;
	map->seed0 = seed0;
	map->seed1 = seed1;
	map->hash = hash;
	map->compare = compare;
	map->elfree = elfree;
	map->udata = udata;
	map->spare = ((char*)map)+sizeof(struct hashmap);
	map->edata = (char*)map->spare+bucketsz;
	map->cap = cap;
	map->nbuckets = cap;
	map->mask = map->nbuckets-1;
	map->buckets = _malloc(map->bucketsz*map->nbuckets);
	if (!map->buckets) {
		_free(map);
		return NULL;
	}
	memset(map->buckets, 0, map->bucketsz*map->nbuckets);
	map->growpower = 1;
	map->loadfactor = clamp_load_factor(HASHMAP_LOAD_FACTOR, GROW_AT) * 100;
	map->growat = map->nbuckets * (map->loadfactor / 100.0);
	map->shrinkat = map->nbuckets * SHRINK_AT;
	map->malloc = _malloc;
	map->realloc = _realloc;
	map->free = _free;
	return map;
}

/** 哈希创建函数
 * elsize：每个元素在 tree 中的大小
 * cap：默认最小容量
 * seed0|seed1：可选的随机种子
 * hash：哈希生成函数
 * compare：哈希比较函数，比较两个 item 在哈希冲突的时候是否相同
 * elfree：element 的释放函数；对于复杂的结构体，成员变量指针指向的地址需要对应的释放
 * 	   函数，如结构体内的成员结构体指针
 */
// hashmap_new returns a new hash map.
// Param `elsize` is the size of each element in the tree. Every element that
// is inserted, deleted, or retrieved will be this size.
// Param `cap` is the default lower capacity of the hashmap. Setting this to
// zero will default to 16.
// Params `seed0` and `seed1` are optional seed values that are passed to the
// following `hash` function. These can be any value you wish but it's often
// best to use randomly generated values.
// Param `hash` is a function that generates a hash value for an item. It's
// important that you provide a good hash function, otherwise it will perform
// poorly or be vulnerable to Denial-of-service attacks. This implementation
// comes with two helper functions `hashmap_sip()` and `hashmap_murmur()`.
// Param `compare` is a function that compares items in the tree. See the
// qsort stdlib function for an example of how this function works.
// The hashmap must be freed with hashmap_free().
// Param `elfree` is a function that frees a specific item. This should be NULL
// unless you're storing some kind of reference data in the hash.
struct hashmap *hashmap_new(size_t elsize, size_t cap, uint64_t seed0,
	uint64_t seed1,
	uint64_t (*hash)(const void *item, uint64_t seed0, uint64_t seed1),
	int (*compare)(const void *a, const void *b, void *udata),
	void (*elfree)(void *item),
	void *udata)
{
	return hashmap_new_with_allocator(NULL, NULL, NULL, elsize, cap, seed0,
		seed1, hash, compare, elfree, udata);
}

/** 释放元素 */
static void free_elements(struct hashmap *map)
{
	if (map->elfree) {
		for (size_t i = 0; i < map->nbuckets; i++) {
			struct bucket *bucket = bucket_at(map, i);
			if (bucket->dib) map->elfree(bucket_item(bucket));
		}
	}
}

/** 清空|更新哈希表 */
// hashmap_clear quickly clears the map.
// Every item is called with the element-freeing function given in hashmap_new,
// if present, to free any data referenced in the elements of the hashmap.
// When the update_cap is provided, the map's capacity will be updated to match
// the currently number of allocated buckets. This is an optimization to ensure
// that this operation does not perform any allocations.
void hashmap_clear(struct hashmap *map, bool update_cap)
{
	map->count = 0;
	free_elements(map);
	if (update_cap) {
		map->cap = map->nbuckets;
	} else if (map->nbuckets != map->cap) {
		void *new_buckets = map->malloc(map->bucketsz*map->cap);
		if (new_buckets) {
			map->free(map->buckets);
			map->buckets = new_buckets;
		}
		map->nbuckets = map->cap;
	}
	memset(map->buckets, 0, map->bucketsz*map->nbuckets);
	map->mask = map->nbuckets-1;
	map->growat = map->nbuckets * (map->loadfactor / 100.0) ;
	map->shrinkat = map->nbuckets * SHRINK_AT;
}

/** 改变哈希表的大小（扩容或者缩容），保留哈希表的内容 */
static bool resize0(struct hashmap *map, size_t new_cap)
{
	struct hashmap *map2 = hashmap_new_with_allocator(map->malloc,
		map->realloc, map->free, map->elsize, new_cap, map->seed0,
		map->seed1, map->hash, map->compare, map->elfree, map->udata);
	if (!map2) return false;
	for (size_t i = 0; i < map->nbuckets; i++) {
		struct bucket *entry = bucket_at(map, i);
		/** 未存储元素 */
		if (!entry->dib) {
			continue;
		}
		entry->dib = 1;
		/** 找到哈希值 j <= 后的桶， */
		/** 计算在新 map 中的哈希偏移 */
		size_t j = entry->hash & map2->mask;
		while(1) {
			/** 找到哈希值对应的桶 */
			struct bucket *bucket = bucket_at(map2, j);
			/** dib = 0，未被使用，复制 entry 至该位置 */
			if (bucket->dib == 0) {
				memcpy(bucket, entry, map->bucketsz);
				break;
			}
			/**
			 * 交换 entry 和 bucket
			 * dib 大的值优先级高
			*/
			if (bucket->dib < entry->dib) {
				memcpy(map2->spare, bucket, map->bucketsz);
				memcpy(bucket, entry, map->bucketsz);
				memcpy(entry, map2->spare, map->bucketsz);
			}
			j = (j + 1) & map2->mask;
			/** entry 始终被当做待插入的元素 */
			entry->dib += 1;
		}
	}
	map->free(map->buckets);
	map->buckets = map2->buckets;
	map->nbuckets = map2->nbuckets;
	map->mask = map2->mask;
	map->growat = map2->growat;
	map->shrinkat = map2->shrinkat;
	map->free(map2);
	return true;
}

/** 改变哈希表大小 */
static bool resize(struct hashmap *map, size_t new_cap)
{
	return resize0(map, new_cap);
}

/**
 * 使用指定 hash 值设置 item 的哈希值
 * item 的值在内部已经使用 memcpy 进行复制，因此，不需要对 item 本身进行 free
 * */
// hashmap_set_with_hash works like hashmap_set but you provide your
// own hash. The 'hash' callback provided to the hashmap_new function
// will not be called
const void *hashmap_set_with_hash(struct hashmap *map, const void *item,
				  uint64_t hash)
{
	hash = clip_hash(hash);
	map->oom = false;
	if (map->count >= map->growat) {
		if (!resize(map, map->nbuckets*(1<<map->growpower))) {
			map->oom = true;
			return NULL;
		}
	}

	struct bucket *entry = map->edata;
	entry->hash = hash;
	entry->dib = 1;
	void *eitem = bucket_item(entry);
	memcpy(eitem, item, map->elsize);

	void *bitem;
	size_t i = entry->hash & map->mask;
	while(1) {
		struct bucket *bucket = bucket_at(map, i);
		/** 桶未使用 */
		if (bucket->dib == 0) {
			memcpy(bucket, entry, map->bucketsz);
			map->count++;
			return NULL;
		}
		bitem = bucket_item(bucket);
		/** 找到 item */
		if (entry->hash == bucket->hash && (!map->compare ||
		    map->compare(eitem, bitem, map->udata) == 0))
		{
			memcpy(map->spare, bitem, map->elsize);
			memcpy(bitem, eitem, map->elsize);
			return map->spare;
		}
		/**
		 * bucket 位置已经存储其他元素，调换 entry 和 bucket 的内容
		 * 获取 bucket 的元素 eitem；
		 * entry 始终被当做待插入的元素；
		 * */
		if (bucket->dib < entry->dib) {
			memcpy(map->spare, bucket, map->bucketsz);
			memcpy(bucket, entry, map->bucketsz);
			memcpy(entry, map->spare, map->bucketsz);
			eitem = bucket_item(entry);
		}
		i = (i + 1) & map->mask;
		/** 更新计算次数 */
		entry->dib += 1;
	}
}

/**
 * @brief 设置 item 的哈希值
 *
 * item 的值在内部已经使用 memcpy 进行复制，因此，传入参数中 item 不一定是动态分配的
 * 内存地址，可以是栈上的变量地址。因此，也需要对 item 本身进行 free。
 *
 * @param [in] map
 * @param [in] item
 * @return const void*, 若设置的 item 值在哈希表中已经存在，则返回之前的 item
 */
// hashmap_set inserts or replaces an item in the hash map. If an item is
// replaced then it is returned otherwise NULL is returned. This operation
// may allocate memory. If the system is unable to allocate additional
// memory then NULL is returned and hashmap_oom() returns true.
const void *hashmap_set(struct hashmap *map, const void *item)
{
	return hashmap_set_with_hash(map, item, get_hash(map, item));
}

// hashmap_get_with_hash works like hashmap_get but you provide your
// own hash. The 'hash' callback provided to the hashmap_new function
// will not be called
const void *hashmap_get_with_hash(struct hashmap *map, const void *key,
				  uint64_t hash)
{
	hash = clip_hash(hash);
	size_t i = hash & map->mask;
	while(1) {
		struct bucket *bucket = bucket_at(map, i);
		if (!bucket->dib) return NULL;
		/** 向后查找 */
		if (bucket->hash == hash) {
			void *bitem = bucket_item(bucket);
			if (!map->compare ||
			     map->compare(key, bitem, map->udata) == 0) {
				return bitem;
			}
		}
		i = (i + 1) & map->mask;
	}
}

/** 获取 key 对应的 item */
const void *hashmap_get(struct hashmap *map, const void *key)
{
	return hashmap_get_with_hash(map, key, get_hash(map, key));
}

/** 获取（探测）指定哈希位置对应的元素 */
// hashmap_probe returns the item in the bucket at position or NULL if an item
// is not set for that bucket. The position is 'moduloed' by the number of
// buckets in the hashmap.
const void *hashmap_probe(struct hashmap *map, uint64_t position)
{
	size_t i = position & map->mask;
	struct bucket *bucket = bucket_at(map, i);
	if (!bucket->dib) {
		return NULL;
	}
	return bucket_item(bucket);
}

/** 删除指定哈希值对应的元素 */
// hashmap_delete_with_hash works like hashmap_delete but you provide your
// own hash. The 'hash' callback provided to the hashmap_new function
// will not be called
const void *hashmap_delete_with_hash(struct hashmap *map, const void *key,
				     uint64_t hash)
{
	hash = clip_hash(hash);
	map->oom = false;
	size_t i = hash & map->mask;
	while(1) {
		struct bucket *bucket = bucket_at(map, i);
		if (!bucket->dib) {
			return NULL;
		}
		void *bitem = bucket_item(bucket);
		/** 找到元素 */
		if (bucket->hash == hash && (!map->compare ||
		    map->compare(key, bitem, map->udata) == 0)) {
			memcpy(map->spare, bitem, map->elsize);
			bucket->dib = 0;
			while (1) {
				struct bucket *prev = bucket;
				i = (i + 1) & map->mask;
				bucket = bucket_at(map, i);
				if (bucket->dib <= 1) {
					prev->dib = 0;
					break;
				}
				/** bucket 往前移动 */
				memcpy(prev, bucket, map->bucketsz);
				prev->dib--;
			}
			map->count--;
			if (map->nbuckets > map->cap &&
			    map->count <= map->shrinkat) {
				// Ignore the return value. It's ok for the
				// resize operation to fail to allocate enough
				// memory because a shrink operation does not
				// change the integrity of the data.
				resize(map, map->nbuckets/2);
			}
			return map->spare;
		}
		i = (i + 1) & map->mask;
	}
}

/**
 * @brief 删除 key 指定的元素
 * 该函数返回的 item 指针不需要使用 free 释放，因为该 item 指针地址为哈希表内的地址，
 * 由哈希表自动维护，直接 free 会导致 coredump。该 item 在 delete 之后，在哈希表中会
 * 标记为空闲，在函数调用 hashmap_free 的时候，会释放所有 item 对应的地址空间。
 * 用户需要做的就是释放 item 结构中自定义分配的内存。
 *
 * @param [in] map
 * @param [in] key
 * @return const void*
 */
// hashmap_delete removes an item from the hash map and returns it. If the
// item is not found then NULL is returned.
const void *hashmap_delete(struct hashmap *map, const void *key)
{
	return hashmap_delete_with_hash(map, key, get_hash(map, key));
}

/** 返回哈希表中的元素数量 */
size_t hashmap_count(struct hashmap *map)
{
	return map->count;
}

/** 释放哈希表 */
void hashmap_free(struct hashmap *map)
{
	if (!map) return;
	free_elements(map);
	map->free(map->buckets);
	map->free(map);
}

/** 获取内存不足的状态 */
bool hashmap_oom(struct hashmap *map)
{
	return map->oom;
}

/** 使用迭代器扫描 hashmap
 * 迭代器返回 0 （false） 则停止迭代
 * 正常结束返回 true, 提前结束返回 false
*/
// hashmap_scan iterates over all items in the hash map
// Param `iter` can return false to stop iteration early.
// Returns false if the iteration has been stopped early.
bool hashmap_scan(struct hashmap *map,
		  bool (*iter)(const void *item, void *udata), void *udata)
{
	for (size_t i = 0; i < map->nbuckets; i++) {
		struct bucket *bucket = bucket_at(map, i);
		if (bucket->dib && !iter(bucket_item(bucket), udata)) {
			return false;
		}
	}
	return true;
}

// hashmap_iter iterates one key at a time yielding a reference to an
// entry at each iteration. Useful to write simple loops and avoid writing
// dedicated callbacks and udata structures, as in hashmap_scan.
//
// map is a hash map handle. i is a pointer to a size_t cursor that
// should be initialized to 0 at the beginning of the loop. item is a void
// pointer pointer that is populated with the retrieved item. Note that this
// is NOT a copy of the item stored in the hash map and can be directly
// modified.
//
// Note that if hashmap_delete() is called on the hashmap being iterated,
// the buckets are rearranged and the iterator must be reset to 0, otherwise
// unexpected results may be returned after deletion.
//
// This function has not been tested for thread safety.
//
// The function returns true if an item was retrieved; false if the end of the
// iteration has been reached.
/**
 * 从指定哈希位置处开始迭代，i 应该从 0 开始，返回 item 的指针
 * !!!没有确保是线程安全的函数!!!
*/
bool hashmap_iter(struct hashmap *map, size_t *i, void **item)
{
	struct bucket *bucket;
	/** 找到有效元素退出循环 */
	do {
		if (*i >= map->nbuckets) return false;
		bucket = bucket_at(map, *i);
		(*i)++;
	} while (!bucket->dib);
	*item = bucket_item(bucket);
	return true;
}

//-----------------------------------------------------------------------------
// SipHash reference C implementation
//
// Copyright (c) 2012-2016 Jean-Philippe Aumasson
// <jeanphilippe.aumasson@gmail.com>
// Copyright (c) 2012-2014 Daniel J. Bernstein <djb@cr.yp.to>
//
// To the extent possible under law, the author(s) have dedicated all copyright
// and related and neighboring rights to this software to the public domain
// worldwide. This software is distributed without any warranty.
//
// You should have received a copy of the CC0 Public Domain Dedication along
// with this software. If not, see
// <http://creativecommons.org/publicdomain/zero/1.0/>.
//
// default: SipHash-2-4
//-----------------------------------------------------------------------------
static uint64_t SIP64(const uint8_t *in, const size_t inlen, uint64_t seed0,
		      uint64_t seed1)
{
#define U8TO64_LE(p) \
	{ (((uint64_t)((p)[0])) | ((uint64_t)((p)[1]) << 8) | \
	   ((uint64_t)((p)[2]) << 16) | ((uint64_t)((p)[3]) << 24) | \
	   ((uint64_t)((p)[4]) << 32) | ((uint64_t)((p)[5]) << 40) | \
	   ((uint64_t)((p)[6]) << 48) | ((uint64_t)((p)[7]) << 56)) }
#define U64TO8_LE(p, v) \
	{ U32TO8_LE((p), (uint32_t)((v))); \
	  U32TO8_LE((p) + 4, (uint32_t)((v) >> 32)); }
#define U32TO8_LE(p, v) \
	{ (p)[0] = (uint8_t)((v)); \
	  (p)[1] = (uint8_t)((v) >> 8); \
	  (p)[2] = (uint8_t)((v) >> 16); \
	  (p)[3] = (uint8_t)((v) >> 24); }
#define ROTL(x, b) (uint64_t)(((x) << (b)) | ((x) >> (64 - (b))))
#define SIPROUND \
	{ v0 += v1; v1 = ROTL(v1, 13); \
	  v1 ^= v0; v0 = ROTL(v0, 32); \
	  v2 += v3; v3 = ROTL(v3, 16); \
	  v3 ^= v2; \
	  v0 += v3; v3 = ROTL(v3, 21); \
	  v3 ^= v0; \
	  v2 += v1; v1 = ROTL(v1, 17); \
	  v1 ^= v2; v2 = ROTL(v2, 32); }
	uint64_t k0 = U8TO64_LE((uint8_t*)&seed0);
	uint64_t k1 = U8TO64_LE((uint8_t*)&seed1);
	uint64_t v3 = UINT64_C(0x7465646279746573) ^ k1;
	uint64_t v2 = UINT64_C(0x6c7967656e657261) ^ k0;
	uint64_t v1 = UINT64_C(0x646f72616e646f6d) ^ k1;
	uint64_t v0 = UINT64_C(0x736f6d6570736575) ^ k0;
	const uint8_t *end = in + inlen - (inlen % sizeof(uint64_t));
	for (; in != end; in += 8) {
		uint64_t m = U8TO64_LE(in);
		v3 ^= m;
		SIPROUND; SIPROUND;
		v0 ^= m;
	}
	const int left = inlen & 7;
	uint64_t b = ((uint64_t)inlen) << 56;
	switch (left) {
	case 7: b |= ((uint64_t)in[6]) << 48; /* fall through */
	case 6: b |= ((uint64_t)in[5]) << 40; /* fall through */
	case 5: b |= ((uint64_t)in[4]) << 32; /* fall through */
	case 4: b |= ((uint64_t)in[3]) << 24; /* fall through */
	case 3: b |= ((uint64_t)in[2]) << 16; /* fall through */
	case 2: b |= ((uint64_t)in[1]) << 8; /* fall through */
	case 1: b |= ((uint64_t)in[0]); break;
	case 0: break;
	}
	v3 ^= b;
	SIPROUND; SIPROUND;
	v0 ^= b;
	v2 ^= 0xff;
	SIPROUND; SIPROUND; SIPROUND; SIPROUND;
	b = v0 ^ v1 ^ v2 ^ v3;
	uint64_t out = 0;
	U64TO8_LE((uint8_t*)&out, b);
	return out;
}

//-----------------------------------------------------------------------------
// MurmurHash3 was written by Austin Appleby, and is placed in the public
// domain. The author hereby disclaims copyright to this source code.
//
// Murmur3_86_128
//-----------------------------------------------------------------------------
static uint64_t MM86128(const void *key, const int len, uint32_t seed)
{
#define	ROTL32(x, r) ((x << r) | (x >> (32 - r)))
#define FMIX32(h) h^=h>>16; h*=0x85ebca6b; h^=h>>13; h*=0xc2b2ae35; h^=h>>16;
	const uint8_t * data = (const uint8_t*)key;
	const int nblocks = len / 16;
	uint32_t h1 = seed;
	uint32_t h2 = seed;
	uint32_t h3 = seed;
	uint32_t h4 = seed;
	uint32_t c1 = 0x239b961b;
	uint32_t c2 = 0xab0e9789;
	uint32_t c3 = 0x38b34ae5;
	uint32_t c4 = 0xa1e38b93;
	const uint32_t * blocks = (const uint32_t *)(data + nblocks*16);
	for (int i = -nblocks; i; i++) {
		uint32_t k1 = blocks[i*4+0];
		uint32_t k2 = blocks[i*4+1];
		uint32_t k3 = blocks[i*4+2];
		uint32_t k4 = blocks[i*4+3];
		k1 *= c1; k1  = ROTL32(k1,15); k1 *= c2; h1 ^= k1;
		h1 = ROTL32(h1,19); h1 += h2; h1 = h1*5+0x561ccd1b;
		k2 *= c2; k2  = ROTL32(k2,16); k2 *= c3; h2 ^= k2;
		h2 = ROTL32(h2,17); h2 += h3; h2 = h2*5+0x0bcaa747;
		k3 *= c3; k3  = ROTL32(k3,17); k3 *= c4; h3 ^= k3;
		h3 = ROTL32(h3,15); h3 += h4; h3 = h3*5+0x96cd1c35;
		k4 *= c4; k4  = ROTL32(k4,18); k4 *= c1; h4 ^= k4;
		h4 = ROTL32(h4,13); h4 += h1; h4 = h4*5+0x32ac3b17;
	}
	const uint8_t * tail = (const uint8_t*)(data + nblocks*16);
	uint32_t k1 = 0;
	uint32_t k2 = 0;
	uint32_t k3 = 0;
	uint32_t k4 = 0;
	switch(len & 15) {
	case 15: k4 ^= tail[14] << 16; /* fall through */
	case 14: k4 ^= tail[13] << 8; /* fall through */
	case 13: k4 ^= tail[12] << 0;
		k4 *= c4; k4  = ROTL32(k4,18); k4 *= c1; h4 ^= k4;
		/* fall through */
	case 12: k3 ^= tail[11] << 24; /* fall through */
	case 11: k3 ^= tail[10] << 16; /* fall through */
	case 10: k3 ^= tail[ 9] << 8; /* fall through */
	case  9: k3 ^= tail[ 8] << 0;
		k3 *= c3; k3  = ROTL32(k3,17); k3 *= c4; h3 ^= k3;
		/* fall through */
	case  8: k2 ^= tail[ 7] << 24; /* fall through */
	case  7: k2 ^= tail[ 6] << 16; /* fall through */
	case  6: k2 ^= tail[ 5] << 8; /* fall through */
	case  5: k2 ^= tail[ 4] << 0;
		k2 *= c2; k2  = ROTL32(k2,16); k2 *= c3; h2 ^= k2;
		/* fall through */
	case  4: k1 ^= tail[ 3] << 24; /* fall through */
	case  3: k1 ^= tail[ 2] << 16; /* fall through */
	case  2: k1 ^= tail[ 1] << 8; /* fall through */
	case  1: k1 ^= tail[ 0] << 0;
		k1 *= c1; k1  = ROTL32(k1,15); k1 *= c2; h1 ^= k1;
		/* fall through */
	};
	h1 ^= len; h2 ^= len; h3 ^= len; h4 ^= len;
	h1 += h2; h1 += h3; h1 += h4;
	h2 += h1; h3 += h1; h4 += h1;
	FMIX32(h1); FMIX32(h2); FMIX32(h3); FMIX32(h4);
	h1 += h2; h1 += h3; h1 += h4;
	h2 += h1; h3 += h1; h4 += h1;
	return (((uint64_t)h2)<<32)|h1;
}

//-----------------------------------------------------------------------------
// xxHash Library
// Copyright (c) 2012-2021 Yann Collet
// All rights reserved.
//
// BSD 2-Clause License (https://www.opensource.org/licenses/bsd-license.php)
//
// xxHash3
//-----------------------------------------------------------------------------
#define XXH_PRIME_1 11400714785074694791ULL
#define XXH_PRIME_2 14029467366897019727ULL
#define XXH_PRIME_3 1609587929392839161ULL
#define XXH_PRIME_4 9650029242287828579ULL
#define XXH_PRIME_5 2870177450012600261ULL

static uint64_t XXH_read64(const void* memptr)
{
	uint64_t val;
	memcpy(&val, memptr, sizeof(val));
	return val;
}

static uint32_t XXH_read32(const void* memptr)
{
	uint32_t val;
	memcpy(&val, memptr, sizeof(val));
	return val;
}

static uint64_t XXH_rotl64(uint64_t x, int r)
{
	return (x << r) | (x >> (64 - r));
}

static uint64_t xxh3(const void* data, size_t len, uint64_t seed)
{
	const uint8_t* p = (const uint8_t*)data;
	const uint8_t* const end = p + len;
	uint64_t h64;

	if (len >= 32) {
		const uint8_t* const limit = end - 32;
		uint64_t v1 = seed + XXH_PRIME_1 + XXH_PRIME_2;
		uint64_t v2 = seed + XXH_PRIME_2;
		uint64_t v3 = seed + 0;
		uint64_t v4 = seed - XXH_PRIME_1;

		do {
			v1 += XXH_read64(p) * XXH_PRIME_2;
			v1 = XXH_rotl64(v1, 31);
			v1 *= XXH_PRIME_1;

			v2 += XXH_read64(p + 8) * XXH_PRIME_2;
			v2 = XXH_rotl64(v2, 31);
			v2 *= XXH_PRIME_1;

			v3 += XXH_read64(p + 16) * XXH_PRIME_2;
			v3 = XXH_rotl64(v3, 31);
			v3 *= XXH_PRIME_1;

			v4 += XXH_read64(p + 24) * XXH_PRIME_2;
			v4 = XXH_rotl64(v4, 31);
			v4 *= XXH_PRIME_1;

			p += 32;
		} while (p <= limit);

		h64 = XXH_rotl64(v1, 1) + XXH_rotl64(v2, 7) +
		      XXH_rotl64(v3, 12) + XXH_rotl64(v4, 18);

		v1 *= XXH_PRIME_2;
		v1 = XXH_rotl64(v1, 31);
		v1 *= XXH_PRIME_1;
		h64 ^= v1;
		h64 = h64 * XXH_PRIME_1 + XXH_PRIME_4;

		v2 *= XXH_PRIME_2;
		v2 = XXH_rotl64(v2, 31);
		v2 *= XXH_PRIME_1;
		h64 ^= v2;
		h64 = h64 * XXH_PRIME_1 + XXH_PRIME_4;

		v3 *= XXH_PRIME_2;
		v3 = XXH_rotl64(v3, 31);
		v3 *= XXH_PRIME_1;
		h64 ^= v3;
		h64 = h64 * XXH_PRIME_1 + XXH_PRIME_4;

		v4 *= XXH_PRIME_2;
		v4 = XXH_rotl64(v4, 31);
		v4 *= XXH_PRIME_1;
		h64 ^= v4;
		h64 = h64 * XXH_PRIME_1 + XXH_PRIME_4;
	} else {
		h64 = seed + XXH_PRIME_5;
	}

	h64 += (uint64_t)len;

	while (p + 8 <= end) {
		uint64_t k1 = XXH_read64(p);
		k1 *= XXH_PRIME_2;
		k1 = XXH_rotl64(k1, 31);
		k1 *= XXH_PRIME_1;
		h64 ^= k1;
		h64 = XXH_rotl64(h64, 27) * XXH_PRIME_1 + XXH_PRIME_4;
		p += 8;
	}

	if (p + 4 <= end) {
		h64 ^= (uint64_t)(XXH_read32(p)) * XXH_PRIME_1;
		h64 = XXH_rotl64(h64, 23) * XXH_PRIME_2 + XXH_PRIME_3;
		p += 4;
	}

	while (p < end) {
		h64 ^= (*p) * XXH_PRIME_5;
		h64 = XXH_rotl64(h64, 11) * XXH_PRIME_1;
		p++;
	}

	h64 ^= h64 >> 33;
	h64 *= XXH_PRIME_2;
	h64 ^= h64 >> 29;
	h64 *= XXH_PRIME_3;
	h64 ^= h64 >> 32;

	return h64;
}

// hashmap_sip returns a hash value for `data` using SipHash-2-4.
uint64_t hashmap_sip(const void *data, size_t len, uint64_t seed0,
		     uint64_t seed1)
{
	return SIP64((uint8_t*)data, len, seed0, seed1);
}

// hashmap_murmur returns a hash value for `data` using Murmur3_86_128.
uint64_t hashmap_murmur(const void *data, size_t len, uint64_t seed0,
			uint64_t seed1)
{
	(void)seed1;
	return MM86128(data, len, seed0);
}

/**
 * @brief 使用内部 data 指向的地址内部长度为 len 的数据进行哈希值计算
 *
 * 注意：
 * typedef struct car {
 * 	int	id;
 * 	char	name[64];
 * 	int	band;
 * 	int	price;
 * } car_t;
 *
 * 对于上述结构体，若两个 id 相同的条目，其他任意成员变量有不同（如：price），哈希计算
 * 的 len 值长度为 sizeof(car_t), 即整个结构体长度，则即使 id 相同，计算的哈希值也
 * 不同，此处的哈希计算包含了整个结构体的特性，若两个结构体的内容完全相同，则哈希值相同。
 *
 * @param [in] data
 * @param [in] len
 * @param [in] seed0
 * @param [in] seed1
 * @return uint64_t 哈希值
 */
uint64_t hashmap_xxhash3(const void *data, size_t len, uint64_t seed0,
			 uint64_t seed1)
{
	(void)seed1;
	return xxh3(data, len ,seed0);
}

//==============================================================================
// TESTS AND BENCHMARKS
// $ cc -DHASHMAP_TEST hashmap.c && ./a.out              # run tests
// $ cc -DHASHMAP_TEST -O3 hashmap.c && BENCH=1 ./a.out  # run benchmarks
//==============================================================================
#ifdef HASHMAP_TEST

static size_t deepcount(struct hashmap *map)
{
	size_t count = 0;
	for (size_t i = 0; i < map->nbuckets; i++) {
		if (bucket_at(map, i)->dib) {
			count++;
		}
	}
	return count;
}

#ifdef __GNUC__
#pragma GCC diagnostic ignored "-Wpedantic"
#endif
#ifdef __clang__
#pragma GCC diagnostic ignored "-Wunknown-warning-option"
#pragma GCC diagnostic ignored "-Wcompound-token-split-by-macro"
#pragma GCC diagnostic ignored "-Wgnu-statement-expression-from-macro-expansion"
#endif
#ifdef __GNUC__
#pragma GCC diagnostic ignored "-Wunused-parameter"
#endif

#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <assert.h>
#include <stdio.h>
#include "hashmap.h"

static bool rand_alloc_fail = false;
static int rand_alloc_fail_odds = 3; // 1 in 3 chance malloc will fail.
static uintptr_t total_allocs = 0;
static uintptr_t total_mem = 0;

static void *xmalloc(size_t size)
{
	if (rand_alloc_fail && rand()%rand_alloc_fail_odds == 0) {
		return NULL;
	}
	void *mem = malloc(sizeof(uintptr_t)+size);
	assert(mem);
	*(uintptr_t*)mem = size;
	total_allocs++;
	total_mem += size;
	return (char*)mem+sizeof(uintptr_t);
}

static void xfree(void *ptr)
{
	if (ptr) {
		total_mem -= *(uintptr_t*)((char*)ptr-sizeof(uintptr_t));
		free((char*)ptr-sizeof(uintptr_t));
		total_allocs--;
	}
}

static void shuffle(void *array, size_t numels, size_t elsize)
{
	char tmp[elsize];
	char *arr = array;
	for (size_t i = 0; i < numels - 1; i++) {
		int j = i + rand() / (RAND_MAX / (numels - i) + 1);
		memcpy(tmp, arr + j * elsize, elsize);
		memcpy(arr + j * elsize, arr + i * elsize, elsize);
		memcpy(arr + i * elsize, tmp, elsize);
	}
}

static bool iter_ints(const void *item, void *udata)
{
	int *vals = *(int**)udata;
	vals[*(int*)item] = 1;
	return true;
}

static int compare_ints_udata(const void *a, const void *b, void *udata)
{
	return *(int*)a - *(int*)b;
}

static int compare_strs(const void *a, const void *b, void *udata)
{
	return strcmp(*(char**)a, *(char**)b);
}

static uint64_t hash_int(const void *item, uint64_t seed0, uint64_t seed1)
{
	return hashmap_xxhash3(item, sizeof(int), seed0, seed1);
	// return hashmap_sip(item, sizeof(int), seed0, seed1);
	// return hashmap_murmur(item, sizeof(int), seed0, seed1);
}

static uint64_t hash_str(const void *item, uint64_t seed0, uint64_t seed1)
{
	return hashmap_xxhash3(*(char**)item, strlen(*(char**)item), seed0,
			       seed1);
	// return hashmap_sip(*(char**)item, strlen(*(char**)item), seed0,
	// 		   seed1);
	// return hashmap_murmur(*(char**)item, strlen(*(char**)item), seed0,
	// 		      seed1);
}

static void free_str(void *item)
{
	xfree(*(char**)item);
}

static void all(void)
{
	int seed = getenv("SEED")?atoi(getenv("SEED")):time(NULL);
	int N = getenv("N")?atoi(getenv("N")):2000;
	printf("seed=%d, count=%d, item_size=%zu\n", seed, N, sizeof(int));
	srand(seed);

	rand_alloc_fail = true;

	// test sip and murmur hashes
	assert(hashmap_sip("hello", 5, 1, 2) == 2957200328589801622);
	assert(hashmap_murmur("hello", 5, 1, 2) == 1682575153221130884);
	assert(hashmap_xxhash3("hello", 5, 1, 2) == 2584346877953614258);

	int *vals;
	while (!(vals = xmalloc(N * sizeof(int)))) {}
	for (int i = 0; i < N; i++) {
		vals[i] = i;
	}

	struct hashmap *map;

	while (!(map = hashmap_new(sizeof(int), 0, seed, seed,
				   hash_int, compare_ints_udata, NULL, NULL)))
	{}
	shuffle(vals, N, sizeof(int));
	for (int i = 0; i < N; i++) {
		// // printf("== %d ==\n", vals[i]);
		assert(map->count == (size_t)i);
		assert(map->count == hashmap_count(map));
		assert(map->count == deepcount(map));
		const int *v;
		assert(!hashmap_get(map, &vals[i]));
		assert(!hashmap_delete(map, &vals[i]));
		while (true) {
		assert(!hashmap_set(map, &vals[i]));
		if (!hashmap_oom(map)) {
			break;
		}
		}

		for (int j = 0; j < i; j++) {
			v = hashmap_get(map, &vals[j]);
			assert(v && *v == vals[j]);
		}
		while (true) {
			v = hashmap_set(map, &vals[i]);
			if (!v) {
				assert(hashmap_oom(map));
				continue;
			} else {
				assert(!hashmap_oom(map));
				assert(v && *v == vals[i]);
				break;
			}
		}
		v = hashmap_get(map, &vals[i]);
		assert(v && *v == vals[i]);
		v = hashmap_delete(map, &vals[i]);
		assert(v && *v == vals[i]);
		assert(!hashmap_get(map, &vals[i]));
		assert(!hashmap_delete(map, &vals[i]));
		assert(!hashmap_set(map, &vals[i]));
		assert(map->count == (size_t)(i+1));
		assert(map->count == hashmap_count(map));
		assert(map->count == deepcount(map));
	}

	int *vals2;
	while (!(vals2 = xmalloc(N * sizeof(int)))) {}
	memset(vals2, 0, N * sizeof(int));
	assert(hashmap_scan(map, iter_ints, &vals2));

	// Test hashmap_iter. This does the same as hashmap_scan above.
	size_t iter = 0;
	void *iter_val;
	while (hashmap_iter (map, &iter, &iter_val)) {
		assert (iter_ints(iter_val, &vals2));
	}
	for (int i = 0; i < N; i++) {
		assert(vals2[i] == 1);
	}
	xfree(vals2);

	shuffle(vals, N, sizeof(int));
	for (int i = 0; i < N; i++) {
		const int *v;
		v = hashmap_delete(map, &vals[i]);
		assert(v && *v == vals[i]);
		assert(!hashmap_get(map, &vals[i]));
		assert(map->count == (size_t)(N-i-1));
		assert(map->count == hashmap_count(map));
		assert(map->count == deepcount(map));
		for (int j = N-1; j > i; j--) {
			v = hashmap_get(map, &vals[j]);
			assert(v && *v == vals[j]);
		}
	}

	for (int i = 0; i < N; i++) {
		while (true) {
			assert(!hashmap_set(map, &vals[i]));
			if (!hashmap_oom(map)) {
				break;
			}
		}
	}

	assert(map->count != 0);
	size_t prev_cap = map->cap;
	hashmap_clear(map, true);
	assert(prev_cap < map->cap);
	assert(map->count == 0);


	for (int i = 0; i < N; i++) {
		while (true) {
			assert(!hashmap_set(map, &vals[i]));
			if (!hashmap_oom(map)) {
				break;
			}
		}
	}

	prev_cap = map->cap;
	hashmap_clear(map, false);
	assert(prev_cap == map->cap);

	hashmap_free(map);

	xfree(vals);


	while (!(map = hashmap_new(sizeof(char*), 0, seed, seed,
				   hash_str, compare_strs, free_str, NULL)));

	for (int i = 0; i < N; i++) {
		char *str;
		while (!(str = xmalloc(16)));
		snprintf(str, 16, "s%i", i);
		while(!hashmap_set(map, &str));
	}

	hashmap_clear(map, false);
	assert(hashmap_count(map) == 0);

	for (int i = 0; i < N; i++) {
		char *str;
		while (!(str = xmalloc(16)));
		snprintf(str, 16, "s%i", i);
		while(!hashmap_set(map, &str));
	}

	hashmap_free(map);

	if (total_allocs != 0) {
		fprintf(stderr, "total_allocs: expected 0, got %lu\n",
			total_allocs);
		exit(1);
	}
}

#define bench(name, N, code) {{ \
	if (strlen(name) > 0) { \
		printf("%-14s ", name); \
	} \
	size_t tmem = total_mem; \
	size_t tallocs = total_allocs; \
	uint64_t bytes = 0; \
	clock_t begin = clock(); \
	for (int i = 0; i < N; i++) { \
		(code); \
	} \
	clock_t end = clock(); \
	double elapsed_secs = (double)(end - begin) / CLOCKS_PER_SEC; \
	double bytes_sec = (double)bytes/elapsed_secs; \
	printf("%d ops in %.3f secs, %.0f ns/op, %.0f op/sec", \
		N, elapsed_secs, \
		elapsed_secs/(double)N*1e9, \
		(double)N/elapsed_secs \
	); \
	if (bytes > 0) { \
		printf(", %.1f GB/sec", bytes_sec/1024/1024/1024); \
	} \
	if (total_mem > tmem) { \
		size_t used_mem = total_mem-tmem; \
		printf(", %.2f bytes/op", (double)used_mem/N); \
	} \
	if (total_allocs > tallocs) { \
		size_t used_allocs = total_allocs-tallocs; \
		printf(", %.2f allocs/op", (double)used_allocs/N); \
	} \
	printf("\n"); \
}}

static void benchmarks(void)
{
	int seed = getenv("SEED")?atoi(getenv("SEED")):time(NULL);
	int N = getenv("N")?atoi(getenv("N")):5000000;
	printf("seed=%d, count=%d, item_size=%zu\n", seed, N, sizeof(int));
	srand(seed);


	int *vals = xmalloc(N * sizeof(int));
	for (int i = 0; i < N; i++) {
		vals[i] = i;
	}

	shuffle(vals, N, sizeof(int));

	struct hashmap *map;
	shuffle(vals, N, sizeof(int));

	map = hashmap_new(sizeof(int), 0, seed, seed, hash_int,
			  compare_ints_udata, NULL, NULL);
	bench("set", N, {
		const int *v = hashmap_set(map, &vals[i]);
		assert(!v);
	})
	shuffle(vals, N, sizeof(int));
	bench("get", N, {
		const int *v = hashmap_get(map, &vals[i]);
		assert(v && *v == vals[i]);
	})
	shuffle(vals, N, sizeof(int));
	bench("delete", N, {
		const int *v = hashmap_delete(map, &vals[i]);
		assert(v && *v == vals[i]);
	})
	hashmap_free(map);

	map = hashmap_new(sizeof(int), N, seed, seed, hash_int,
			  compare_ints_udata, NULL, NULL);
	bench("set (cap)", N, {
		const int *v = hashmap_set(map, &vals[i]);
		assert(!v);
	})
	shuffle(vals, N, sizeof(int));
	bench("get (cap)", N, {
		const int *v = hashmap_get(map, &vals[i]);
		assert(v && *v == vals[i]);
	})
	shuffle(vals, N, sizeof(int));
	bench("delete (cap)" , N, {
		const int *v = hashmap_delete(map, &vals[i]);
		assert(v && *v == vals[i]);
	})

	hashmap_free(map);


	xfree(vals);

	if (total_allocs != 0) {
		fprintf(stderr, "total_allocs: expected 0, got %lu\n", total_allocs);
		exit(1);
	}
}

int main(void)
{
	hashmap_set_allocator(xmalloc, xfree);

	if (getenv("BENCH")) {
		printf("Running hashmap.c benchmarks...\n");
		benchmarks();
	} else {
		printf("Running hashmap.c tests...\n");
		all();
		printf("PASSED\n");
	}
}

#endif