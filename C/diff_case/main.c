/*
 * @Author: Alan Yin
 * @Date: 2024-06-08 10:59:44
 * @LastEditTime: 2024-06-22 16:40:35
 * @LastEditors: Alan Yin
 * @FilePath: /training/C/diff_case/main.c
 * @Description:
 * // -*- mode:C; tab-width:8; c-basic-offset:2; indent-tabs-mode:t -*-
 * // vim: ts=8 sw=2 smarttab
 * Copyright (c) 2024 by Alan Yin, All Rights Reserved.
 */

#include "../common/utils.h"

int main()
{
	int nr = 1;
	switch (nr) {
	case 0 ... 16:
		pr_info("0 ... 16");
		break;
	case 17 ... 64:
		pr_info("17 ... 64");
		break;
	case 65 ... 128:
		pr_info("65 ... 128");
		break;
	case 129 ... 256:
		pr_info("129 ... 256");
		break;
	default:
		pr_info("other");
		break;
	}

	return 0;
}