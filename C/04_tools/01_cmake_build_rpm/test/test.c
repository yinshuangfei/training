/*
 * @Author: Alan Yin
 * @Date: 2024-06-20 10:59:45
 * @LastEditTime: 2024-06-20 11:13:43
 * @LastEditors: Alan Yin
 * @FilePath: /nas_mgmt/test/test.c
 * @Description:
 * // -*- mode:C; tab-width:8; c-basic-offset:2; indent-tabs-mode:t -*-
 * // vim: ts=8 sw=2 smarttab
 * Copyright (c) 2024 by Alan Yin, All Rights Reserved.
 */

#include <stdio.h>

#include "../include/libnasmgmt.h"

int main(int argc, char *argv[])
{
	int a = 1, b = 2;
	printf("c=%d\n", add(a, b));

	return 0;
}