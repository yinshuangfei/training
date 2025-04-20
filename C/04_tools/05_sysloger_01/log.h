/**
 * Copyright (c) 2020 rxi
 *
 * This library is free software; you can redistribute it and/or modify it
 * under the terms of the MIT license. See `log.c` for details.
 *
 * quote from: https://github.com/rxi/log.c/tree/master
 *
 */

#ifndef LOG_H
#define LOG_H

#include <stdio.h>
#include <stdarg.h>
#include <stdbool.h>
#include <time.h>

#define LOG_VERSION "0.1.0"

typedef enum _LOG_LEVEL {
	LOG_LEVEL_TRACE,
	LOG_LEVEL_DEBUG,
	LOG_LEVEL_INFO,
	LOG_LEVEL_WARN,
	LOG_LEVEL_ERROR,
	LOG_LEVEL_FATAL
} LOG_LEVEL;

typedef void (*loglock_fn)(bool lock, void *udata);

#define log_trace(...)				\
	log_log(LOG_LEVEL_TRACE, __FILE__, __LINE__, __VA_ARGS__)
#define log_debug(...)				\
	log_log(LOG_LEVEL_DEBUG, __FILE__, __LINE__, __VA_ARGS__)
#define log_info(...)				\
	log_log(LOG_LEVEL_INFO,  __FILE__, __LINE__, __VA_ARGS__)
#define log_warn(...)				\
	log_log(LOG_LEVEL_WARN,  __FILE__, __LINE__, __VA_ARGS__)
#define log_error(...)				\
	log_log(LOG_LEVEL_ERROR, __FILE__, __LINE__, __VA_ARGS__)
#define log_fatal(...)				\
	log_log(LOG_LEVEL_FATAL, __FILE__, __LINE__, __VA_ARGS__)

const char *log_level_string(int level);
void log_set_lock(loglock_fn fn, void *udata);

/** 日志级别设定 */
void log_set_level(int level);
int log_get_level(void);
void log_fp_set_level(FILE *file, int level);
int log_fp_get_level(FILE *file);

/** 开启和关闭 stdout | syslog */
void log_set_stdout_quiet(bool enable);
void log_set_syslog_quiet(bool enable);

/** 文件日志管理 */
int log_add_fp(FILE *fp, int level);
void log_del_fp(FILE *fp);

void log_log(int level, const char *file, int line, const char *fmt, ...);

void init_log(char *name);
void deinit_log(void);

#endif /** LOG_H */