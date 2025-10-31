#pragma once

#include <stdint.h>

#define POOL_BLOCKS_INITIAL 1

typedef struct poolFreed{
	struct poolFreed *nextFree;
} poolFreed;

/** 防止编辑器报错 */
typedef __uint8_t uint8_t;
typedef __uint16_t uint16_t;
typedef __uint32_t uint32_t;
typedef __uint64_t uint64_t;

typedef struct {
	uint32_t elementSize;
	uint32_t blockSize;
	uint32_t used;
	int32_t block;
	poolFreed *freed;
	uint32_t blocksUsed;
	uint8_t **blocks;
} pool;

void poolInitialize(pool *p, const uint32_t elementSize, const uint32_t blockSize);
void poolFreePool(pool *p);

#ifndef DISABLE_MEMORY_POOLING
void *poolMalloc(pool *p);
void poolFree(pool *p, void *ptr);
#else
#include <stdlib.h>
#define poolMalloc(p) malloc((p)->blockSize)
#define poolFree(p, d) free(d)
#endif
void poolFreeAll(pool *p);