/*
 * @Author: Alan Yin
 * @Date: 2024-08-10 00:01:50
 * @LastEditTime: 2024-08-10 14:09:42
 * @LastEditors: Alan Yin
 * @FilePath: /C/99_demo/03_rpc/server_main.c
 * @Description:
 * @// -*- mode:C; tab-width:8; c-basic-offset:2; indent-tabs-mode:t -*-
 * @// vim: ts=8 sw=2 smarttab
 * @Copyright (c) 2024 by Alan Yin, All Rights Reserved.
 */
/*
 * @Author: Alan Yin
 * @Date: 2024-07-06 22:45:09
 * @LastEditTime: 2024-07-08 11:14:47
 * @LastEditors: Alan Yin
 * @FilePath: /nas_mgmt/rpc/main.c
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

#include "server.h"
#include "../../common/utils.h"

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
	int rc = 0;
	char *cmd = NULL;

	/** 获取程序名称 */
	if ((cmd = strrchr(argv[0], '/')) == NULL)
		cmd = argv[0];
	else
		cmd++;

	init_signal();

	/** 运行任务 */
	rc = main_server();
	if (rc) {
		pr_err("%s exit with error (%d:%s)", cmd, -rc,
			strerror(-rc));
	} else {
		pr_err("%s exit successful", cmd);
	}

	return 0;
}
