/*
 * @Author: Alan Yin
 * @Date: 2023-07-22 19:38:08
 * @LastEditTime: 2024-07-02 22:40:54
 * @LastEditors: Alan Yin
 * @FilePath: /training/C/02_01_glibc_hash/hash_glib.c
 * @Description:
 * // -*- mode:C; tab-width:8; c-basic-offset:2; indent-tabs-mode:t -*-
 * // vim: ts=8 sw=2 smarttab
 * Copyright (c) 2024 by Alan Yin, All Rights Reserved.
 */
#include <stdio.h>
#include <glib.h>
#include <string.h>
#include <errno.h>

#include "../../common/utils.h"

static int hash_remove(gpointer key, gpointer value, gpointer user_data)
{
	char *dev = (char *)key;
	if (strlen(dev) > 2 && dev[0] == 's' && dev[1] == 'd') {
		printf("remove: %s\n", (char *)key);
		return 1;
	}
	return 0;
}

void print_key_value(gpointer key, gpointer value, gpointer user_data)
{
	printf("%-8s ---> %s\n", (char *)key, (char *)value);
}

const char *devs[] = {
	"sda",
	"sdb",
	"sdc",
	"loop-1",
	"sdd",
	"dm-1",
	"dm-2",
	"dm-3"
};

int main(int argc, char *argv[])
{
	char **a = (char **)devs;
	GHashTable *hash = NULL;

	hash = g_hash_table_new(g_str_hash, g_str_equal);
	if (!hash)
		return -ENOMEM;

	for (int i = 0; i < ARRAY_SIZE(devs); i++) {
		/**
		 * 使用 glibc 的哈希表，需要保证 insert 后，插入的 key 和 value 指
		 * 向的空间是稳定的，即不会被随意释放，变量不随函数退出而销毁。
		*/
		g_hash_table_insert(hash, a[i], a[i]);
	}

	g_hash_table_foreach_remove(hash, hash_remove, NULL);
	g_hash_table_foreach(hash, print_key_value, NULL);

	return 0;
}
