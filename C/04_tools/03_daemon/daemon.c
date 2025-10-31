/*
 * @Author: Alan Yin
 * @Date: 2024-07-07 15:33:04
 * @LastEditTime: 2024-07-07 15:33:10
 * @LastEditors: Alan Yin
 * @FilePath: /training/C/04_tools/03_daemon/main.c
 * @Description:
 * @// -*- mode:C; tab-width:8; c-basic-offset:2; indent-tabs-mode:t -*-
 * @// vim: ts=8 sw=2 smarttab
 * @Copyright (c) 2024 by Alan Yin, All Rights Reserved.
 */

#include <sys/resource.h>	// for getrlimit()
#include <sys/stat.h>		// for umask()
#include <syslog.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>

#include "daemon.h"
#include "../../common/utils.h"

#define LOCKMODE (S_IRUSR|S_IWUSR|S_IRGRP|S_IROTH)

/*
 * @description: 申请文件锁
 * @param {int} fd
 * @return {*}
 */
static int lockfile(int fd)
{
	struct flock fl;
	fl.l_type = F_WRLCK;
	fl.l_start = 0;
	fl.l_whence = SEEK_SET;
	fl.l_len = 0;
	return fcntl(fd, F_SETLK, &fl);
}

/**
 * @description: 删除 PID 文件
 * @return {*}
 */
static void remove_pidfile()
{
	if (-1 == unlink(EXIT_REMOVED_PIDFILE))
		syslog(LOG_ERR, "failed to remove PID file: %s, %s\n",
			EXIT_REMOVED_PIDFILE, strerror(errno));
}

/*
 * @description: 检测进程是否已经执行
 * @param {char} *lfile
 * @return {*}
 */
int already_running(char *pidfile)
{
	int fd;
	char buf[16];

	fd = open(pidfile, O_RDWR|O_CREAT, LOCKMODE);
	if (fd < 0) {
		syslog(LOG_ERR, "can't open %s: %s", pidfile, strerror(errno));
		exit(EXIT_FAILURE);
	}

	if (lockfile(fd) < 0) {
		if (errno == EACCES || errno == EAGAIN) {
			close(fd);
			return 1;
		}
		syslog(LOG_ERR, "can't lock %s: %s", pidfile, strerror(errno));
		exit(EXIT_FAILURE);
	}

	ftruncate(fd, 0);
	sprintf(buf, "%ld", (long)getpid());
	write(fd, buf, strlen(buf) + 1);

	atexit(remove_pidfile);

	/** keep file open */
	return 0;
}

/*
 * @description: glibc daemon() 函数调用, 有 bug，简单场景使用
 * @return {*}
 */
void simple_daemonize(void)
{
	/**
	 * If nochdir is zero, daemon() changes the process's current working
	 * directory to the root directory ("/"); otherwise, the current working
	 * directory is left unchanged
	 * */
	int nochdir = 0;

	/**
	 * If noclose is zero, daemon() redirects standard input, standard
	 * output and standard error to /dev/null; otherwise, no changes are
	 * made to these file descriptors.
	 * This is equivalent to "closing the file descriptors".
	 * */
	int noclose = 0;

	/**
	 * glibc call to daemonize this process without a double fork
	 * $ man 3 daemon
	 */
	if (daemon(nochdir, noclose)) {
		pr_err("call daemon error (%d:%s)", -errno, strerror(errno));
		exit(EXIT_FAILURE);
	}
}

/*
 * @description: 变成守护进程
 * @param {char} *cmd
 * @return {*}
 */
void daemonize(const char *cmd, void(*init_signal)(void))
{
	int i, fd0, fd1, fd2;
	pid_t pid;
	struct rlimit rl;

	if (getrlimit(RLIMIT_NOFILE, &rl) < 0) {
		pr_err("%s: can't get file limit", cmd);
		exit(EXIT_FAILURE);
	}

	/**
	 * The first fork will change our pid but the sid and pgid will be the
	 * calling process.
	 * */
	if ((pid = fork()) < 0) {
		pr_err("%s: can't fork", cmd);
		exit(EXIT_FAILURE);
	} else if (pid != 0) { /* parent */
		exit(EXIT_SUCCESS);
	}

	/*
	* Run the process in a new session without a controlling
	* terminal. The process group ID will be the process ID
	* and thus, the process will be the process group leader.
	* After this call the process will be in a new session,
	* and it will be the progress group leader in a new
	* process group.
	*/
	if (setsid() < 0) {
		pr_err("%s: can't setsid", cmd);
		exit(EXIT_FAILURE);
	}

	/** Catch, ignore and handle signals */
	if (init_signal)
		init_signal();

	/* Fork off for the second time*/
	if ((pid = fork()) < 0) {
		pr_err("%s: can't fork", cmd);
		exit(EXIT_FAILURE);
	} else if (pid != 0) { /* parent */
		exit(EXIT_SUCCESS);
	}

	/* Set new file permissions */
	umask(0);

	/* Change the working directory to the root directory */
	/* or another appropriated directory */
	if (chdir("/") < 0) {
		pr_err("%s: can't change directory to /", cmd);
		exit(EXIT_FAILURE);
	}

	/**
	 * Close all open file descriptors.
	 *
	 * #include <unistd.h>
	 *
	 * for (maxfd = sysconf(_SC_OPEN_MAX); 0 <= maxfd; maxfd--) {
	 *         close(maxfd);
	 * }
	 */
	if (rl.rlim_max == RLIM_INFINITY)
		rl.rlim_max = 1024;

	/** 这里可能关闭了之前打开的描述符，例如 syslog 打开的套接字描述符 */
	for (i = 0; i < rl.rlim_max; i++)
		close(i);

	/**
	 * Attach file descriptors 0, 1, and 2 to /dev/null.
	 * 这里不需要关闭 fd.
	 * 需要特别注意，当使用套接字时，关闭 0|1|2 文件描述符，套接字使用 0|1|2
	 * 文件描述符，会导致在 sprintf(stderr, ...) 时将输出内容误发送到套接字上。
	 */

	fd0 = open("/dev/null", O_RDWR);
	fd1 = dup(0);
	fd2 = dup(0);

	/**
	 * If open file descriptors 0, 1, and 2 error, write to the log file.
	 */
	if (fd0 != 0 || fd1 != 1 || fd2 != 2) {
		openlog(cmd, LOG_CONS, LOG_DAEMON);
		syslog(LOG_ERR, "unexpected file descriptors %d %d %d",
			fd0, fd1, fd2);
		closelog();
		exit(EXIT_FAILURE);
	}
}