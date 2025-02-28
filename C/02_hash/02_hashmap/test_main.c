/*
 * @Author: Alan Yin
 * @Date: 2024-04-20 23:12:37
 * @LastEditTime: 2024-04-29 22:28:49
 * @LastEditors: Alan Yin
 * @FilePath: /self_project/05_hashmap/test_main.c
 * @Description:
 * // -*- mode:C; tab-width:8; c-basic-offset:2; indent-tabs-mode:t -*-
 * // vim: ts=8 sw=2 smarttab
 * Copyright (c) 2024 by Alan Yin, All Rights Reserved.
 */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <time.h>

#include "../../common/utils.h"
#include "hashmap.h"

typedef struct car {
	int	id;
	char	*name;
	int	band;
	int	price;
	int	duration;
} car_t;

uint64_t car_hash(const void *item, uint64_t seed0, uint64_t seed1)
{
	return hashmap_xxhash3(item, sizeof(car_t), seed0, seed1);
	// return hashmap_sip(item, sizeof(car_t), seed0, seed1);
	// return hashmap_murmur(item, sizeof(car_t), seed0, seed1);
}

int car_compare(const void *a, const void *b, void *udata)
{
	pr_info("car_compare a:%s, b:%s",
		((car_t *)a)->name,
		((car_t *)b)->name);

	return ((car_t *)a)->id - ((car_t *)b)->id;
}

void car_free(void *item)
{
	char *name = ((car_t *)item)->name;

	printf("car_auto_free scan car id: %d\n", ((car_t *)item)->id);

	/**
	 * @brief 对于复杂的结构体，成员变量指针指向的地址需要对应的释放函数，如结构体
	 * 内的成员结构体指针
	 */
	if (name) {
		printf("auto free name:%s at id:%d\n", name,
			((car_t *)item)->id);

		free(name);
	}
}

void iter_car(struct hashmap *map)
{
	car_t *car_p = NULL;
	size_t index = 0;

	while (hashmap_iter(map, &index, (void **)&car_p)) {
		printf("[%ld] iter car id:%d, name:%p, '%s', band:%d, price:%d, duration:%d\n",
			index,
			car_p->id,
			car_p->name,
			car_p->name,
			car_p->band,
			car_p->price,
			car_p->duration);
	}
}

void iter_delete_car_all(struct hashmap *map)
{
	car_t *car_p = NULL;
	car_t *car_p_delete = NULL;
	size_t index = 0;

	while (hashmap_iter(map, &index, (void **)&car_p)) {
		/** 删除操作 */
		car_p_delete = (car_t *)hashmap_delete(map, car_p);
		if (NULL != car_p_delete) {
			if (car_p_delete->name) {
				printf("call free %s\n", car_p_delete->name);
				free(car_p_delete->name);
			}
		}

		/** 重置 index ！！！重要！！！ */
		index = 0;
	}
}

int main(void)
{
	car_t car;
	struct hashmap *map = NULL;
	char str[128] = {0};

	/** 使用当前时间作为随机种子 */
	srand(time(0));

	map = hashmap_new(sizeof(car_t), 10, 234, 5456,
			  car_hash, car_compare, car_free, NULL);

	for (int i = 0; i < 10; i++) {
		memset(&car, 0, sizeof(car_t));

		car.id = i % 5;

 		/** 对于上述结构体，若两个 id 相同的条目，其他任意成员变量有不同
		 * （如：price），哈希计算的 len 值长度为 sizeof(car_t), 即整个结构
		 * 体长度，则即使 id 相同，计算的哈希值也不同，此处的哈希计算包含了整
		 * 个结构体的特性。若两个结构体的内容完全相同，则哈希值相同。
		 */
		// car.price = rand() % 100000;
		car.price = 100000;
		sprintf(str, "id=%d", i );
		/** 手动分配内存 */
		car.name = strdup(str);

		printf("car_hash: %ld\n", car_hash(&car, 234, 5456));

		/** 插入操作 */
		if (NULL == hashmap_set(map, (void *)(&car))) {
			if (hashmap_oom(map)) {
				pr_err("out of memory");
			}
		} else {
			/** replaces item, do something */
		}
	}

	pr_info("total count:%ld", hashmap_count(map));

	/** 遍历 map */
	iter_car(map);

	/** 手动清空 map */
	iter_delete_car_all(map);

	/** 自动清空 map */
	hashmap_free(map);

	return 0;
}
