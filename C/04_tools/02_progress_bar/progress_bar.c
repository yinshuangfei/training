/*
 * @Author: Alan Yin
 * @Date: 2023-07-22 19:38:08
 * @LastEditTime: 2024-06-27 22:02:36
 * @LastEditors: Alan Yin
 * @FilePath: /training/C/progress_bar/progress_bar.c
 * @Description:
 * // -*- mode:C; tab-width:8; c-basic-offset:2; indent-tabs-mode:t -*-
 * // vim: ts=8 sw=2 smarttab
 * Copyright (c) 2024 by Alan Yin, All Rights Reserved.
 */

#include <stdio.h>
#include <unistd.h>
#include <stdbool.h>
#include <stdlib.h>

struct ProgressBar {
	int  length;
	char done_char;
	char wait_char;
	int  percent;
};

/**
 * @description: 带默认参数功能
 * @param {ProgressBar} *bar
 * @return {*}
 */
void update_bar(struct ProgressBar *bar)
{
	int len = 0;
	int nchars = 0;
	int percent = 0;
	char done_char = 0;
	char wait_char = 0;

	/** 参数值规范化 */
	if (bar->length < 10)
		len = 10;
	else if (bar->length > 128)
		len = 128;
	else
		len = bar->length;

	if (bar->percent < 0)
		percent = 0;
	else if (bar->percent > 100)
		percent = 100;
	else
		percent = bar->percent;

	if (bar->done_char < 32 || 126 < bar->done_char)
		done_char = '#';
	else
		done_char = bar->done_char;

	if (bar->wait_char < 32 || 126 < bar->wait_char)
		wait_char = ' ';
	else
		wait_char = bar->wait_char;

	nchars = percent * len / 100;

	printf("\r[");

	for (int i = 0; i < nchars; i++) {
		printf("%c", done_char);
	}

	for (int i = 0; i < len - nchars; i++) {
		printf("%c", wait_char);
	}

	printf("] %3d%% Done ", percent);

	/** 刷新缓冲 */
	fflush(stdout);
}

int main(int argc, char *argv[])
{
	struct ProgressBar bar;
	bar.length = 80;
	bar.done_char = '#';
	bar.wait_char = ' ';
	bar.percent = 0;

	for (int i = 0; i <= 100; i++, bar.percent++) {
		update_bar(&bar);
		usleep(10000);
	}

	printf("\n");
	return 0;
}
