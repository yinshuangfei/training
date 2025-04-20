/**
 * @file test_main.c
 * @author Alan Yin (yinshuangfei007@163.com)
 * @brief
 * @version 0.0.1
 * @date 2025-04-20
 *
 * @copyright Copyright (c) 2025
 *
 * @node:
 */

#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>

#include "log.h"

int main(int argc, char **argv)
{
	FILE *file;

	/**
	 * 在其他项目中，可使用 init_log 初始化日志名，
	 * init_log(argv[0]);
	*/

	file = fopen("mylog.log", "aw");
	if (NULL == file) {
		log_error("can't open file %s (%s)", "mylog.log",
			  strerror(errno));
		exit(-1);
	}

	log_add_fp(file, LOG_LEVEL_INFO);

	log_info("default log level [%s]",
		 log_level_string(log_get_level()));
	log_info("file log level [%s]",
		 log_level_string(log_fp_get_level(file)));

	log_trace("this is trace");
	log_debug("this is debug");
	log_info("this is info");
	log_warn("this is warn");

	log_set_stdout_quiet(true);
	log_set_syslog_quiet(true);
	log_info("log invisiable");
	log_set_stdout_quiet(false);
	log_set_syslog_quiet(false);

	log_set_level(LOG_LEVEL_ERROR);
	log_info("syslog | stdout log info not print");

	log_fp_set_level(file, LOG_LEVEL_FATAL);

	log_error("this is error");
	log_fatal("this is fatal");

	log_del_fp(file);
	log_fatal("will not write into file");

	fclose(file);

	// deinit_log();

	return 0;
}