/*
 * Soft:        Keepalived is a failover program for the LVS project
 *              <www.linuxvirtualserver.org>. It monitor & manipulate
 *              a loadbalanced server pool using multi-layer checks.
 *
 * Part:        logging facility.
 *
 * Author:      Alexandre Cassen, <acassen@linux-vs.org>
 *
 *              This program is distributed in the hope that it will be useful,
 *              but WITHOUT ANY WARRANTY; without even the implied warranty of
 *              MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 *              See the GNU General Public License for more details.
 *
 *              This program is free software; you can redistribute it and/or
 *              modify it under the terms of the GNU General Public License
 *              as published by the Free Software Foundation; either version
 *              2 of the License, or (at your option) any later version.
 *
 * Copyright (C) 2001-2017 Alexandre Cassen, <acassen@gmail.com>
 */

#include <stdio.h>
#include <stdbool.h>
#include <time.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <memory.h>
#include <syslog.h>
#include <stdlib.h>

#include "logger.h"
#include "bitops.h"

/** Optional logging facilities
 * LOG_KERN
 * LOG_USER
 * LOG_MAIL
 * LOG_DAEMON
 * LOG_AUTH
 * LOG_SYSLOG
 * LOG_LPR
 * LOG_NEWS
 * LOG_UUCP
 * LOG_CRON
 * LOG_AUTHPRIV
 * LOG_FTP
 */
int log_facility = LOG_DAEMON;

#ifdef ENABLE_LOG_TO_FILE
/* File to write log messages to */
const char *log_file_name;
static FILE *log_file;
bool always_flush_log_file;	// 每次写日志都下刷至文件
#endif

/*
 * @description: 打开系统日志
 *   #define LOG_PID         // log the pid with each message
 *   #define LOG_CONS        // log on the console if errors in sending
 *     当日志系统本身出现故障或无法正常工作时，LOG_CONS 提供了一种回退机制，确保重要的
 *     日志信息不会丢失，而是能够显示在终端
 *   #define LOG_ODELAY      // delay open until first syslog() (default)
 *   #define LOG_NDELAY      // don't delay open
 *   #define LOG_NOWAIT      // don't wait for console forks: DEPRECATED
 *   #define LOG_PERROR      // log to stderr as well
 * @param {char} *ident
 * @return {*}
 */
void open_syslog(const char *ident)
{
	openlog(ident, LOG_PID | ((__test_bit(LOG_CONSOLE_BIT, &debug)) ?
				 LOG_CONS : 0), log_facility);
}

#ifdef ENABLE_LOG_TO_FILE
void
set_flush_log_file(void)
{
	always_flush_log_file = true;
}

void close_log_file(void)
{
	if (log_file) {
		fclose(log_file);
		log_file = NULL;
	}
}

const char *make_file_name(const char *tmp_dir, const char *name,
			   const char *prog, const char *namespace,
			   const char *instance)
{
	const char *extn_start;
	const char *dir_end;
	size_t len;
	char *file_name;

	if (!name)
		return NULL;

	if (name[0] != '/')
		len = strlen(tmp_dir) + 1;
	else
		len = 0;
	len += strlen(name);
	if (prog)
		len += strlen(prog) + 1;
	if (namespace)
		len += strlen(namespace) + 1;
	if (instance)
		len += strlen(instance) + 1;

	file_name = malloc(len + 1);
	dir_end = strrchr(name, '/');
	extn_start = strrchr(dir_end ? dir_end : name, '.');

	if (name[0] != '/') {
		strcpy(file_name, tmp_dir);
		strcat(file_name, "/");
	} else
		file_name[0] = '\0';

	strncat(file_name, name, extn_start ? (size_t)(extn_start - name) : len);

	if (prog) {
		strcat(file_name, "_");
		strcat(file_name, prog);
	}
	if (namespace) {
		strcat(file_name, "_");
		strcat(file_name, namespace);
	}
	if (instance) {
		strcat(file_name, "_");
		strcat(file_name, instance);
	}
	if (extn_start)
		strcat(file_name, extn_start);

	return file_name;
}

/*
 * @description: 打开文件
 * @param {char} *tmp_dir
 * @param {char} *name
 * @param {char} *prog
 * @param {char} *namespace
 * @param {char} *instance
 * @return {*}
 */
void open_log_file(const char *tmp_dir, const char *name, const char *prog,
		   const char *namespace, const char *instance)
{
	const char *file_name;

	if (log_file) {
		fclose(log_file);
		log_file = NULL;
	}

	if (!name)
		return;

	file_name = make_file_name(tmp_dir, name, prog, namespace, instance);

	log_file = fopen(file_name, "a");
	if (log_file) {
		int n = fileno(log_file);
		if (fcntl(n, F_SETFD, FD_CLOEXEC | fcntl(n, F_GETFD)) == -1)
			log_message(LOG_INFO, "Failed to set CLOEXEC on log file %s", file_name);
		if (fcntl(n, F_SETFL, O_NONBLOCK | fcntl(n, F_GETFL)) == -1)
			log_message(LOG_INFO, "Failed to set NONBLOCK on log file %s", file_name);
	}
}

void flush_log_file(void)
{
	if (log_file)
		fflush(log_file);
}

void update_log_file_perms(mode_t umask_bits)
{
	if (log_file)
		fchmod(fileno(log_file), (S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH) & ~umask_bits);
}
#endif

/*
 * @description: va_list 写入系统日志
 * @param {int} facility
 * @param {char*} format
 * @param {va_list} args
 * @return {*}
 */
void vlog_message(const int facility, const char* format, va_list args)
{
#if !HAVE_VSYSLOG	// 没有 vsyslog
	char buf[MAX_LOG_MSG+1];
#endif

	/* Don't write syslog if testing configuration */
	if (__test_bit(DO_NULL_BIT, &debug))
		return;

#if !HAVE_VSYSLOG
	vsnprintf(buf, sizeof(buf), format, args);
#endif

	/** 输出至控制台或者文件 */
	if (
#ifdef ENABLE_LOG_TO_FILE
	    log_file ||
#endif
			__test_bit(LOG_FILE_OR_CONSOLE_BIT, &debug)) {
#if HAVE_VSYSLOG
		va_list args1;
		char buf[2 * MAX_LOG_MSG + 1];

		va_copy(args1, args);
		vsnprintf(buf, sizeof(buf), format, args1);
		va_end(args1);
#endif

		/* timestamp setup */
#ifdef ENABLE_LOG_TO_FILE
		struct timespec ts;
		time_t t;
		char *p;

		clock_gettime(CLOCK_REALTIME, &ts);
		t = ts.tv_sec;
#else
		time_t t = time(NULL);
#endif
		struct tm tm;
		char timestamp[64];

		localtime_r(&t, &tm);

		if (__test_bit(LOG_FILE_OR_CONSOLE_BIT, &debug)) {
			strftime(timestamp, sizeof(timestamp), "%c", &tm);
			fprintf(stderr, "%s: %s\n", timestamp, buf);
		}
#ifdef ENABLE_LOG_TO_FILE
		if (log_file) {
			p = timestamp;
			p += strftime(timestamp, sizeof(timestamp), "%a %b %d %T", &tm);
			p += snprintf(p, timestamp + sizeof(timestamp) - p, ".%9.9ld", ts.tv_nsec);
			strftime(p, timestamp + sizeof(timestamp) - p, " %Y", &tm);
			fprintf(log_file, "%s: %s\n", timestamp, buf);
			if (always_flush_log_file)
				fflush(log_file);
		}
#endif
	}

	/** 判断是否写入至系统日志 */
	if (!__test_bit(NO_SYSLOG_BIT, &debug)) {
#if HAVE_VSYSLOG
		vsyslog(facility, format, args);
#else
		syslog(facility, "%s", buf);
#endif
	}
}

/*
 * @description: 写入系统日志
 * @param {int} facility
 * @param {char} *format
 * @return {*}
 */
void log_message(const int facility, const char *format, ...)
{
	va_list args;

	va_start(args, format);
	vlog_message(facility, format, args);
	va_end(args);
}

/*
 * @description: 若文件句柄有效，输出内容至文件；否则退化输出至系统日志
 * @param {FILE} *fp
 * @param {char} *format
 * @return {*}
 */
void conf_write(FILE *fp, const char *format, ...)
{
	va_list args;

	va_start(args, format);
	if (fp) {
		vfprintf(fp, format, args);
		fprintf(fp, "\n");
	}
	else
		vlog_message(LOG_INFO, format, args);

	va_end(args);
}
