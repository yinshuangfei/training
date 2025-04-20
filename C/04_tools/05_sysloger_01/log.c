/*
 * Copyright (c) 2020 rxi
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to
 * deal in the Software without restriction, including without limitation the
 * rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
 * sell copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
 * IN THE SOFTWARE.
 */
#include <sys/syslog.h>

#include "log.h"

#define MAX_CALLBACKS 4

/**
 * 日志事件描述结构体
 */
typedef struct {
	int level;			/** 本次事假日志级别 */
	struct tm *time;		/** 日志时间戳 */
	const char *file;		/** 日志打印函数 - 所在文件 */
	int line;			/** 日志打印函数 - 所在文件行数 */
	const char *fmt;		/** 可变字符串-格式 */
	va_list ap;			/** 可变字符串-参数 */
	void *udata;			/** 输出流 */
} log_event_t;

typedef void (*log_fn)(log_event_t *ev);

/**
 * 回调参数描述结构体
 */
typedef struct {
	log_fn fn;			/** 该回调的打印函数 */
	void *udata;			/** 该回调的输出流 */
	int level;			/** 该回调的日志级别 */
} callback_t;

/**
 * 全局日志结构体
*/
static struct {
	void *udata;
	loglock_fn lock;
	int level;			/** stdout | syslog 日志级别 */
	bool stdout_quiet;		/** stdout 禁止：不输出至标准输出 */
	bool syslog_quiet;		/** syslog 禁止: 不输出至 syslog */
	callback_t callbacks[MAX_CALLBACKS];
} Logger;

/**
 * 日志字符串定义
*/
static const char *level_strings[] = {
	"TRACE",
	"DEBUG",
	"INFO",
	"WARN",
	"ERROR",
	"FATAL"
};

#ifdef LOG_USE_COLOR
/**
 * 颜色定义
*/
static const char *level_colors[] = {
	"\x1b[94m",		/** 蓝色 */
	"\x1b[36m",		/** 靛蓝色 */
	"\x1b[32m",		/** 绿色 */
	"\x1b[33m",		/** 黄色 */
	"\x1b[31m",		/** 红色 */
	"\x1b[35m"		/** 紫色 */
	// "\x1b[90m"		/** 浅灰色，文件信息使用 */
};
#endif

/**
 * @brief 标准输出回调函数
 *
 * @param [in] ev
 */
static void stdout_callback(log_event_t *ev)
{
	char buf[64];
	buf[strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", ev->time)] = '\0';

#ifdef LOG_USE_COLOR
	fprintf(ev->udata,
		"%s %s[%-5s]\x1b[0m \x1b[90m(%s:%d)\x1b[0m ",
		buf,
		level_colors[ev->level],
		level_strings[ev->level],
		ev->file,
		ev->line);
#else
	fprintf(ev->udata,
		"%s [%-5s] (%s:%d) ",
		buf,
		level_strings[ev->level],
		ev->file,
		ev->line);
#endif
	vfprintf(ev->udata, ev->fmt, ev->ap);
	fprintf(ev->udata, "\n");
	fflush(ev->udata);
}

/**
 * @brief syslog 输出回调函数
 *
 * @param [in] ev
 */
static void syslog_callback(log_event_t *ev)
{
	char msg[2048 + 1];
	int level;

	/** syslog 不用输出时间 */
	vsnprintf(msg, 2048, ev->fmt, ev->ap);

	/** 映射日志等级 */
	switch (ev->level)
	{
	case LOG_LEVEL_TRACE:
	case LOG_LEVEL_DEBUG:
		level = LOG_DEBUG;
		break;
	case LOG_LEVEL_INFO:
		level = LOG_INFO;
		break;
	case LOG_LEVEL_WARN:
		level = LOG_WARNING;
		break;
	case LOG_LEVEL_ERROR:
		level = LOG_ERR;
		break;
	case LOG_LEVEL_FATAL:
		level = LOG_CRIT;
		break;
	default:
		level = LOG_INFO;
		break;
	}

	syslog(level,
	       "[%-5s] (%s:%d) %s",
	       level_strings[ev->level],
	       ev->file,
	       ev->line,
	       msg);
}

/**
 * @brief 文件输出回调函数
 *
 * @param [in] ev
 */
static void file_callback(log_event_t *ev)
{
	char buf[64];
	buf[strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", ev->time)] = '\0';

	fprintf(ev->udata,
		"%s [%-5s] (%s:%d) ",
		buf, level_strings[ev->level],
		ev->file,
		ev->line);

	vfprintf(ev->udata, ev->fmt, ev->ap);
	fprintf(ev->udata, "\n");
	fflush(ev->udata);
}

/**
 * @brief 设置日志锁
 *
 * @param [in] fn
 * @param [in] udata
 */
void log_set_lock(loglock_fn fn, void *udata)
{
	Logger.lock = fn;
	Logger.udata = udata;
}

/**
 * @brief 日志加锁
 *
 */
static void lock(void)
{
	if (Logger.lock) {
		Logger.lock(true, Logger.udata);
	}
}

/**
 * @brief 日志解锁
 *
 */
static void unlock(void)
{
	if (Logger.lock) {
		Logger.lock(false, Logger.udata);
	}
}

/**
 * @brief 输出日志级别对应的字符串
 *
 * @param [in] level
 * @return const char* 日志字符串
 */
const char *log_level_string(int level)
{
	return level_strings[level];
}

/**
 * @brief 设置全局输出日志级别
 *
 * @param [in] level
 */
void log_set_level(int level)
{
	Logger.level = level;
}

/**
 * @brief 获取全局输出日志级别
 *
 * @return int, return log level
 */
int log_get_level(void)
{
	return Logger.level;
}

/**
 * @brief 设置对应文件日志的等级
 *
 * @param [in] fp
 * @param [in] level
 */
void log_fp_set_level(FILE *fp, int level)
{
	for (int i = 0; i < MAX_CALLBACKS; i++) {
		if (Logger.callbacks[i].fn) {
			if ((FILE *)(Logger.callbacks[i].udata) == fp) {
				Logger.callbacks[i].level = level;
			}
		}
	}
}

/**
 * @brief 获取对应文件日志的等级
 *
 * @param [in] fp
 * @return int 0: success; <0: failed;
 */
int log_fp_get_level(FILE *fp)
{
	for (int i = 0; i < MAX_CALLBACKS; i++) {
		if (Logger.callbacks[i].fn) {
			if ((FILE *)(Logger.callbacks[i].udata) == fp) {
				return Logger.callbacks[i].level;
			}
		}
	}
	return -1;
}

/**
 * @brief 是否让 stdout 保持安静（即不输出至 stdout ）
 *
 * @param [in] enable
 */
void log_set_stdout_quiet(bool enable)
{
	Logger.stdout_quiet = enable;
}

/**
 * @brief 是否让 syslog 保持安静（即不输出至 syslog ）
 *
 * @param [in] enable
 */
void log_set_syslog_quiet(bool enable)
{
	Logger.syslog_quiet = enable;
}

/**
 * @brief 添加回调函数
 *
 * @param [in] fn
 * @param [in] udata
 * @param [in] level
 * @return int 0: success; <0: failed, posix errno;
 */
static int log_add_callback(log_fn fn, void *udata, int level)
{
	for (int i = 0; i < MAX_CALLBACKS; i++) {
		if (!Logger.callbacks[i].fn) {
			Logger.callbacks[i] = (callback_t) { fn, udata, level };
			return 0;
		}
	}
	return -1;
}

/**
 * @brief 添加文件日志
 *
 * @param [in] fp
 * @param [in] level
 * @return int 0: success; <0: failed;
 */
int log_add_fp(FILE *fp, int level)
{
	return log_add_callback(file_callback, fp, level);
}

/**
 * @brief 删除文件日志
 *
 * @param [in] fp
 */
void log_del_fp(FILE *fp)
{
	for (int i = 0; i < MAX_CALLBACKS; i++) {
		if (Logger.callbacks[i].fn) {
			if ((FILE *)(Logger.callbacks[i].udata) == fp) {
				Logger.callbacks[i] = (callback_t)
					{ NULL, NULL, 0 };
			}
		}
	}
}

/**
 * @brief 初始化时间戳
 *
 * @param [in] ev
 * @param [in] udata
 */
static void init_event(log_event_t *ev, void *udata)
{
	if (!ev->time) {
		time_t t = time(NULL);
		ev->time = localtime(&t);
	}
	ev->udata = udata;
}

/**
 * @brief 日志输出函数
 *
 * @param [in] level
 * @param [in] file
 * @param [in] line
 * @param [in] fmt
 * @param [in] ...
 */
void log_log(int level, const char *file, int line, const char *fmt, ...)
{
	log_event_t ev = {
		.fmt   = fmt,
		.file  = file,
		.line  = line,
		.level = level,
	};

	lock();

	/** 标准输出 */
	if (!Logger.stdout_quiet && level >= Logger.level) {
		init_event(&ev, stderr);
		va_start(ev.ap, fmt);
		stdout_callback(&ev);
		va_end(ev.ap);
	}

	/** syslog 输出 */
	if (!Logger.syslog_quiet && level >= Logger.level) {
		va_start(ev.ap, fmt);
		syslog_callback(&ev);
		va_end(ev.ap);
	}

	/** 回调函数集合
	 * TODO: 日志循环功能
	 */
	for (int i = 0; i < MAX_CALLBACKS && Logger.callbacks[i].fn; i++) {
		callback_t *cb = &Logger.callbacks[i];
		if (level >= cb->level) {
			init_event(&ev, cb->udata);
			va_start(ev.ap, fmt);
			cb->fn(&ev);
			va_end(ev.ap);
		}
	}

	unlock();
}

void init_log(char *name)
{
	openlog(name, LOG_PID , LOG_SYSLOG | LOG_DAEMON | LOG_CONS);
}

void deinit_log(void)
{
	closelog();
}