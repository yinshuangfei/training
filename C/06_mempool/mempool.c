/*
 * @Author: Alan Yin
 * @Date: 2024-07-13 22:34:29
 * @LastEditTime: 2024-07-13 22:34:37
 * @LastEditors: Alan Yin
 * @FilePath: \Code\training\C\06_mempool\mempool.c
 * @Description:
 * @// -*- mode:C; tab-width:8; c-basic-offset:2; indent-tabs-mode:t -*-
 * @// vim: ts=8 sw=2 smarttab
 * @Copyright (c) 2024 by Alan Yin, All Rights Reserved.
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>

#define MP_ALIGNMENT			32
/** 4K */
#define MP_PAGE_SIZE			4096
#define MP_MAX_ALLOC_FROM_POOL		(MP_PAGE_SIZE - 1)

/** 数值对齐 */
#define mp_align(n, alignment)						\
	(((n) + (alignment - 1)) & ~(alignment - 1))
/** 指针地址对齐 */
#define mp_align_ptr(p, alignment)					\
	(void *)((((size_t)p) + (alignment - 1)) & ~(alignment - 1))

typedef struct mp_large_s {
	struct mp_large_s *next;
	void *alloc;
} mp_large_t;

/** 内存池节点 */
typedef struct mp_node_s {
	unsigned char *last;	// last 之前为已使用的内存
	unsigned char *end;	// last 到 end 之间为可分配内存
	struct mp_node_s *next;
	size_t failed;
} mp_node_t;

/** 内存池 */
typedef struct mp_pool_s {
	size_t max;		// 内存池单次分配最大大小

	mp_node_t* current;	// 当前内存节点头
	mp_large_t* large;	// 大内存链表

	mp_node_t head[0];	// 每个内存节点包含了 head 和 size，链接多个节点
} mp_pool_t;

/*
 * @description: 创建内存池
 * @param {size_t} size
 * @return {*}
 */
mp_pool_t *mp_create_pool(size_t size)
{
	mp_pool_t *p;
	// malloc无法分配超过4k的内存,size + sizeof(mp_pool_t) + sizeof(mp_node_s)保证有size大小可用
	/** 预对齐内存分配 */
	int ret = posix_memalign((void*)&p,
				 MP_ALIGNMENT,
				 size + sizeof(mp_pool_t) + sizeof(mp_node_t));
	if (ret)
		return NULL;

	p->max = size;
	p->current = p->head;
	p->large = NULL;

	/**
	 * |    pool    |
	 * --------------
	 * |  mem head  |
	 * -------------- <-- (p->head->last)
	 * | mem <size> |
	 * -------------- <-- (p->head->end)
	 * */
	p->head->last = (unsigned char*)p +
			sizeof(mp_pool_t) +
			sizeof(mp_node_t);
	p->head->end = p->head->last + size;
	p->head->failed = 0;

	return p;
}

/*
 * @description: 销毁内存池
 * @param {mp_pool_t} *pool
 * @return {*}
 */
void mp_destory_pool(mp_pool_t *pool)
{
	mp_node_t *h, *n;
	mp_large_t *l;

	for (l = pool->large; l; l = l->next)
		if (l->alloc)
			free(l->alloc);

	h = pool->head->next;

	while (h) {
		n = h->next;
		free(h);
		h = n;
	}

	free(pool);
}

/*
 * @description: 分配小型空间
 * @param {mp_pool_t} *pool
 * @param {size_t} size
 * @return {*}
 */
void *mp_alloc_small(mp_pool_t *pool, size_t size)
{
	unsigned char *m;
	struct mp_node_s *h = pool->head;

	/** psize 包括了 mem head 和 mem size 的大小 */
	size_t psize = (size_t)(h->end - (unsigned char *)h);
	int ret = posix_memalign((void*)&m, MP_ALIGNMENT, psize);
	if (ret)
		return NULL;

	mp_node_t *p, *new_node, *current;

	new_node = (mp_node_t *)m;
	new_node->next = NULL;
	new_node->end = m + psize;
	new_node->failed = 0;
	m += sizeof(mp_node_t);
	m = mp_align_ptr(m, MP_ALIGNMENT);
	/** ???  */
	new_node->last += size;

	current = pool->current;
	// ???
	for (p = current; p->next; p = p->next) {
		// 如存在多次分配失败，current 不再指向此 node
		if (p->failed++ > 4) {
			current = p->next;
		}
	}
	/** 加入链表 */
	p->next = new_node;
	pool->current = current ? current : new_node;

	return m;
}

static void *mp_alloc_large(mp_pool_t *pool, size_t size)
{
	void *p = NULL;
	int ret = posix_memalign((void*)&p, MP_ALIGNMENT, size);
	if (ret)
		return NULL;

	mp_large_t *large;

	// 查找是否有已经释放的 large，在 large list 里面找到一个 null 的节点
	size_t n = 0;
	for (large = pool->large; large; large = large->next) {
		if (large->alloc == NULL) {
			large->alloc = p;
			return p;
		}
		// 避免遍历链条太长
		if (n++ > 3)
			break;
	}

	// 大内存块的头作为小块保存在small中
	large = mp_alloc_small(pool, sizeof(mp_large_t));

	// 头插法
	large->alloc = p;
	large->next = pool->large;
	pool->large = large;
}

/*
 * @description: 从内存池分配内存
 * @param {mp_pool_t} *pool
 * @param {size_t} size
 * @return {*}
 */
void *mp_malloc(mp_pool_t *pool, size_t size)
{
	if (size > pool->max)
		return mp_alloc_large(pool, size);

	mp_node_t *p = pool->current;

	while (p) {
		/** 检查节点的空闲空间可用状态 */
		if (p->end - p->last < size) {
			p = p->next;
			continue;
		}

		/** 找到空间可用的节点，返回对齐后的地址 */
		unsigned char *m = mp_align_ptr(p->last, MP_ALIGNMENT);
		p->last = m + size;
		return m;
	}

	/** 未找到，分配空间 */
	return mp_alloc_small(pool, size);
}

/*
 * @description: 分配内存并初始化为 0
 * @param {mp_pool_t} *pool
 * @param {size_t} size
 * @return {*}
 */
void *mp_calloc(mp_pool_t *pool, size_t size)
{
	void *p = mp_malloc(pool, size);
	if (p)
		memset(p, 0, size);

	return p;

}

/*
 * @description: 释放 large 空间
 * @param {mp_pool_t} *pool
 * @param {void} *p
 * @return {*}
 */
void mp_free(mp_pool_t *pool, void *p)
{
	mp_large_t *l;
	for (l = pool->large; l; l = l->next) {
		if (p == l->alloc) {
			free(l->alloc);
			l->alloc = NULL;
			return;
		}
	}
}

/*
 * @description: 重置内存池
 * @param {mp_pool_t} *pool
 * @return {*}
 */
void mp_reset_pool(mp_pool_t *pool)
{
	mp_node_t *h;
	mp_large_t *l;

	for (l = pool->large; l; l = l->next) {
		if (l->alloc) {
			free(l->alloc);
		}
	}

	pool->large = NULL;

	/** 重置使用空间为最大空间 */
	for (h = pool->head; h; h = h->next) {
		h->last = (unsigned char *)h + sizeof(mp_node_t);
	}

}

/******************* TEST *********************/
int main(int argc, char *argv[]) {

	int size = 1 << 12;

	mp_pool_t *p = mp_create_pool(size);

	int i = 0;
	for (i = 0; i < 10; i++) {

		void *mp = mp_malloc(p, 512);
		//		mp_free(mp);
	}

	printf("mp_align(123, 32): %d, mp_align(17, 32): %d\n", mp_align(24, 32), mp_align(17, 32));


	int j = 0;
	for (i = 0; i < 5; i++) {

		char *pp = mp_calloc(p, 32);
		for (j = 0; j < 32; j++) {
			if (pp[j]) {
				printf("calloc wrong\n");
			}
			printf("calloc success\n");
		}
	}

	//printf("mp_reset_pool\n");

	for (i = 0; i < 5; i++) {
		void *l = mp_malloc(p, 8192);
		mp_free(p, l);
	}

	mp_reset_pool(p);

	//printf("mp_destory_pool\n");
	for (i = 0; i < 58; i++) {
		mp_malloc(p, 256);
	}

	mp_destory_pool(p);

	return 0;
}