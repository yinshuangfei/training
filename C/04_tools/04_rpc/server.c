/*
 * @Author: Alan Yin
 * @Date: 2024-07-08 10:02:58
 * @LastEditTime: 2024-07-08 19:28:36
 * @LastEditors: Alan Yin
 * @FilePath: /nas_mgmt/rpc/server.c
 * @Description:
 * // -*- mode:C; tab-width:8; c-basic-offset:2; indent-tabs-mode:t -*-
 * // vim: ts=8 sw=2 smarttab
 * Copyright (c) 2024 by HT706, All Rights Reserved.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/epoll.h>
#include <fcntl.h>
#include <errno.h>
#include <syslog.h>

#include "server.h"
#include "tcp_rw.h"
#include "../../common/utils.h"

#define MAX_EVENTS		1024

extern int SIGNAL_TERMINATE;

/**
 * @description: 初始化套接字
 * @param {int} port
 * @return {*}
 */
static int init_sock(int port)
{
	int rc = 0;
	int opt = 1;
	int listen_fd = 0;
	struct sockaddr_in addr = {0};

	listen_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (listen_fd == -1) {
		syslog(LOG_ERR, "create socket error (%s)", strerror(errno));
		return -errno;
	}

	rc = setsockopt(listen_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
	if (rc) {
		syslog(LOG_ERR, "setsockopt error (%s)", strerror(errno));
		return -errno;
	}

	memset(&addr, 0, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = INADDR_ANY;
	addr.sin_port = htons(port);
	rc = bind(listen_fd, (struct sockaddr *)&addr, sizeof(addr));
	if (rc == -1) {
		syslog(LOG_ERR, "bind socket error (%s)", strerror(errno));
		return -errno;
	}

	rc = listen(listen_fd, SOMAXCONN);
	if (rc == -1) {
		syslog(LOG_ERR, "listen socket error (%s)", strerror(errno));
		return -errno;
	}

	return listen_fd;
}

/**
 * @description: 初始化 epoll
 * @param {int} port
 * @return {*}
 */
static int init_server_epoll(int port)
{
	int rc = 0;
	int efd = 0;
	int sock_fd = 0;
	int new_sock = 0;
	int ready = 0;
	int flag = 0, j = 0;
	int client_fds[MAX_EVENTS] = {0};
	struct epoll_event event = {0};
	struct epoll_event events[MAX_EVENTS] = {0};
	struct sockaddr_in local_addr = {0};
	struct sockaddr_in remote_addr = {0};
	socklen_t addrlen = {0};

	sock_fd = init_sock(port);
	if (sock_fd < 0)
		return sock_fd;

	/** 初始化客户端链接*/
	for (int i = 0; i < MAX_EVENTS; i++) {
		client_fds[i] = -1;
	}

	/** 创建 epoll 实例 */
	efd = epoll_create(MAX_EVENTS);
	if (efd < 0) {
		syslog(LOG_ERR, "create epoll error (%s)", strerror(errno));
		return -errno;
	}

	event.events = EPOLLIN;
	event.data.fd = sock_fd;

	rc = epoll_ctl(efd, EPOLL_CTL_ADD, sock_fd, &event);
	if (rc < 0) {
		syslog(LOG_ERR, "epoll add error (%s)", strerror(errno));
		return -errno;
	}

	while (SIGNAL_TERMINATE == 0) {
		ready = epoll_wait(efd, events, MAX_EVENTS, 1000);

		if (ready == -1) {
			/** 被信号中断，不打印错误 */
			if (errno != EINTR)
				syslog(LOG_ERR, "epoll_wait failed (%d:%s)",
					errno, strerror(errno));
			continue;
		}

		for (int i = 0; i < ready; i++) {
			if (!(events[i].events & EPOLLIN)) {
				continue;
			}

			if (events[i].data.fd == sock_fd) {
				new_sock = accept(sock_fd,
					(struct sockaddr *)&remote_addr,
					&addrlen);
				if (new_sock < 0) {
					syslog(LOG_ERR, "accept failed (%s)",
						strerror(errno));
					continue;
				}

				/** 控制最大连接数 */
				flag = -1;
				for (j = 0; j < MAX_EVENTS; j++) {
					if (client_fds[j] == -1) {
						client_fds[j] = new_sock;
						flag = j;
						break;
					}
				}

				if (flag == -1)
					syslog(LOG_ERR, "the client pool is full");

				event.events = EPOLLIN;
				event.data.fd = new_sock;
				rc = epoll_ctl(efd, EPOLL_CTL_ADD, new_sock,
					       &event);
				if (rc < 0) {
					syslog(LOG_ERR, "add event failed, fd: %d (%s)",
						new_sock, strerror(errno));
					continue;
				}
			} else {
				do_service(efd, events[i].data.fd, client_fds);
			}
		}
	}

	return 0;
}

static void do_service(int efd, int client_fd, int *client_fds)
{
	int rc = 0;
	int j = 0;
	char recv_buf[RPC_BUFF_MAX] = {0};
	int send_len = 0;
	int recv_len = 0;
	rpc_cmd_t *cmd = NULL;

	rc = server_stream_recv_and_unpack(client_fd, recv_buf, &recv_len);
	if (rc == -EHOSTDOWN) {
		for (j = 0; j < MAX_EVENTS; j++) {
			if (client_fds[j] == client_fd) {
				client_fds[j] = -1;
				break;
			}
		}

		if (j == MAX_EVENTS)
			syslog(LOG_ERR, "client with fd[%d] not find in array",
				client_fd);

		close(client_fd);
		return;
	} else if (rc < 0) {
		return;
	}

	cmd = (rpc_cmd_t *)recv_buf;
	/** 处理任务, 返回值在结构体内部赋值 */
	switch_cmd(cmd);

	/** 预留，防止以后使用忘记判断有效长度 */
	send_len = sizeof(rpc_cmd_t);
	if (send_len > RPC_BUFF_MAX) {
		syslog(LOG_ERR, "send msg too long (%d > MAX:%d)", send_len,
			RPC_BUFF_MAX);
		cmd->rc = -EMSGSIZE;
	}

	server_stream_pack_and_send(client_fd, cmd, send_len);
}

static void switch_cmd(rpc_cmd_t *cmd)
{
	pr_info("recv: %s", cmd->msg);

	switch (cmd->cmd)
	{
	case RPC_CMD_WRITE_NODELIST:
		cmd->rc = 0;
		strcpy(cmd->msg, "this is server msg RPC_CMD_WRITE_NODELIST!");
		break;
	default:
		break;
	}
}

int main_server(void)
{
	int rc = 0;

	rc = init_server_epoll(RPC_PORT);
	if (rc)
		return rc;

	return 0;
}
