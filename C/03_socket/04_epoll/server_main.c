/**
 * @file server_main.c
 * @author Alan Yin (yinshuangfei007@163.com)
 * @brief
 * @version 0.0.1
 * @date 2025-04-05
 *
 * @copyright Copyright (c) 2025
 *
 * @node:
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
 * @brief 设置信号句柄
 *
 * @param [in] sig
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
	case SIGPIPE:
		syslog(LOG_INFO, "Received SIGPIPE signal.");
		break;
	default:
		syslog(LOG_INFO, "Unhandled signal (%d) %s", sig,
			strsignal(sig));
		break;
	}
}

/**
 * @brief 初始化信号, Catch, ignore and handle signals
 *
 */
static void init_signal(void)
{
	/** ignore signals */
	signal(SIGCHLD, SIG_IGN);		/** 20 */
	signal(SIGHUP, SIG_IGN);		/**  1 */

	/** Broken pipe, 必须设置 */
	signal(SIGPIPE, SIG_IGN);		/** 13 */

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
	if ((cmd = strrchr(argv[0], '/')) == NULL) {
		cmd = argv[0];
	} else {
		cmd++;
	}

	init_signal();

	/** 运行任务 */
	rc = server_loop();
	if (rc) {
		pr_err("%s exit with error (%d:%s)", cmd, -rc, strerror(-rc));
	} else {
		pr_info("%s exit successful", cmd);
	}

	return 0;
}
