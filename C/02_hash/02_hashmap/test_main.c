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
	char	name[64];
	int	band;
	int	price;
	int	duration;
} car_t;

uint64_t simple_hash(const void *item, uint64_t seed0, uint64_t seed1)
{
	return (uint64_t)item % 0xffff;
}

void elfree(void *item)
{
	printf("elfree scan car price: %d\n", ((car_t *)item)->price);
}

int main(void)
{
	car_t *car;
	car_t *car_p;
	size_t index = 0;

	// 使用当前时间作为随机种子
	srand(time(0));

	struct hashmap *map = hashmap_new(sizeof(car_t), 10, 234, 5456,
					  simple_hash, NULL, elfree, NULL);

	for (int i = 0; i < 10; i++) {
		car = malloc(sizeof(car_t));
		printf("simple_hash: %ld\n", simple_hash(car, 0, 0));
		car->price = rand() % 100000;
		hashmap_set(map, (void *)(car));
		free(car);
	}

	while (hashmap_iter(map, &index, (void **)&car_p)) {
		printf("car [%ld] price: %d\n", index, car_p->price);
	}

	hashmap_free(map);
	return 0;
}
