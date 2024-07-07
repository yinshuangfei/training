/*
 * @Author: Alan Yin
 * @Date: 2024-07-06 22:45:09
 * @LastEditTime: 2024-07-07 15:37:57
 * @LastEditors: Alan Yin
 * @FilePath: /training/C/04_tools/03_daemon/main.c
 * @Description:
 * @// -*- mode:C; tab-width:8; c-basic-offset:2; indent-tabs-mode:t -*-
 * @// vim: ts=8 sw=2 smarttab
 * @Copyright (c) 2024 by Alan Yin, All Rights Reserved.
 */

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <syslog.h>
#include <signal.h>

#include "daemon.h"
#include "../../common/utils.h"

#define LOCKFILE "/var/run/demo_daemon.pid"

char *EXIT_REMOVED_PIDFILE = LOCKFILE;
int SIGNAL_TERMINATE = 0;

/**
 * @description: 设置信号句柄
 * @param {int} sign
 * @return {*}
 */
static void dispatch_handler(int sig)
{
	switch(sig) {
	case SIGTERM:
		SIGNAL_TERMINATE = 1;
		syslog(LOG_INFO, "Received SIGTERM signal.");
		break;
	case SIGINT:
		SIGNAL_TERMINATE = 1;
		syslog(LOG_INFO, "Received SIGINT signal.");
		break;
	case SIGQUIT:
		syslog(LOG_INFO, "Received SIGQUIT signal.");
		break;
	default:
		syslog(LOG_INFO, "Unhandled signal (%d) %s", sig,
			strsignal(sig));
		break;
	}
}

/**
 * @description: 初始化信号, Catch, ignore and handle signals
 * @return {*}
 */
static void init_signal(void)
{
	/** ignore signals */
	signal(SIGCHLD, SIG_IGN);		/** 20 */
	signal(SIGHUP, SIG_IGN);		/**  1 */

	/** Implement a working signal handler */
	/** 对于任何情况下都需要立即停止程序的调试场景，注释 SIGINT 行 */
	signal(SIGINT, dispatch_handler);	/**  2, Crlt + C */
	signal(SIGQUIT, dispatch_handler);	/**  3 */
	signal(SIGTERM, dispatch_handler);	/** 15 */
}

int main(int argc, char *argv[])
{
	char *cmd = NULL;
	int foreground = 0;

	/** 获取程序名称 */
	if ((cmd = strrchr(argv[0], '/')) == NULL)
		cmd = argv[0];
	else
		cmd++;

	for (int i = 0; i < argc; i++)
		if (0 == strcmp(argv[i], "-f"))
			foreground = 1;

	if (foreground)
		init_signal();
	else
		daemonize(cmd, init_signal);

	/** open up the system log */
	openlog(cmd, LOG_PID, LOG_DAEMON);
	syslog(LOG_INFO, "starting");

	/**
	 * 放在这里的原因：
	 * 1.程序经过 daemonize 后，持有的文件锁已经失效；
	 * 2.若程序直接被函数调用执行，没有连接 TTY, 报错信息依然可以查询;
	 * 这里的返回值已经无法到达调用的 TTY 界面.
	 * */
	if (already_running(LOCKFILE)) {
		syslog(LOG_ERR, "daemon already running");
		exit(EXIT_FAILURE);
	}

	/** our process is now a daemon! */
	while (SIGNAL_TERMINATE == 0) {
		sleep(5);
		syslog(LOG_INFO, "%s running", cmd);
	}

	syslog(LOG_INFO, "%s exit successful", cmd);

	/** close the system log */
	closelog();
	return 0;
}
