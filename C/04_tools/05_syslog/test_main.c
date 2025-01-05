/*
 * @Author: Alan Yin
 * @Date: 2024-07-18 19:34:06
 * @LastEditTime: 2024-07-18 19:34:08
 * @LastEditors: Alan Yin
 * @FilePath: \Code\training\C\08_syslog\test_main.c
 * @Description:
 * @// -*- mode:C; tab-width:8; c-basic-offset:2; indent-tabs-mode:t -*-
 * @// vim: ts=8 sw=2 smarttab
 * @Copyright (c) 2024 by Alan Yin, All Rights Reserved.
 */

#include <stdlib.h>

#include "logger.h"

unsigned long debug;

int main(int argc, char *argv[])
{
	char *filename = "demo_log_file.txt";
	open_syslog("demo");


	/** 正常写入系统日志 */
	log_message(LOG_INFO, "normal print info");
	log_message(LOG_ERR, "normal print err");


	/** 停止|开启写入系统日志 */
	__set_bit(NO_SYSLOG_BIT, &debug);
	log_message(LOG_INFO, "this info not write into syslog");
	__clear_bit(NO_SYSLOG_BIT, &debug);


	/** 写入日志至文件 */
	FILE *file = fopen(filename, "aw+");
	if (file == NULL)
		perror("open file error");

	conf_write(file, "log to file ...");

	if (fclose(file))
		perror("fclose file error");


	/** 写入日志至系统日志|控制台 */
	__set_bit(LOG_FILE_OR_CONSOLE_BIT, &debug);
	log_message(LOG_INFO, "print to syslog and console");


	/** 不做任何动作 */
	__set_bit(DO_NULL_BIT, &debug);
	log_message(LOG_INFO, "!!!not log to anywhere!!!");
	__clear_bit(DO_NULL_BIT, &debug);


	/** 写入日志至系统日志、文件或控制台 */
	open_log_file("./", "ttt", "demo", "aaa", "iii");
	log_message(LOG_INFO, "print to syslog and file|console");
	close_log_file();

	log_message(LOG_INFO, "print to syslog and console 2");

	closelog();

	return 0;
}