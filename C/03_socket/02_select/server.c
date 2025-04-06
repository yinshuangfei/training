/*
 * @Author: Alan Yin
 * @Date: 2024-05-19 22:11:33
 * @LastEditTime: 2024-05-30 23:53:01
 * @LastEditors: Alan Yin
 * @FilePath: /windows_cifs/training/C/03_socket/02_select/server.c
 * @Description:
 * // -*- mode:C; tab-width:8; c-basic-offset:2; indent-tabs-mode:t -*-
 * // vim: ts=8 sw=2 smarttab
 * Copyright (c) 2024 by Alan Yin, All Rights Reserved.
 */

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/select.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>

#include "../common.h"

struct param_t {
	int fd;
	fd_set *set;
};

static int socket_init(void)
{
	int sock_fd = -1;
	int rc = 0;
	int on = 1;
	struct sockaddr_in local_addr;

	sock_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (-1 == sock_fd) {
		pr_err("socket init error (%d:%s)", -errno, strerror(errno));
		return -errno;
	}

	// 设置套接字端口可重用，修复了当 Socket 服务器重启时"地址已在使用
	// (Address already in use)"的错误
	rc = setsockopt(sock_fd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));
	if (-1 == rc) {
		pr_err("setsockopt error (%d:%s)", -errno, strerror(errno));
		return -errno;
	}

	local_addr.sin_family = AF_INET;
	local_addr.sin_port = htons(SERVER_PORT);
	local_addr.sin_addr.s_addr = INADDR_ANY;
	bzero(&(local_addr.sin_zero), 8);

	rc = bind(sock_fd, (struct sockaddr *)&local_addr,
		  sizeof(struct sockaddr));
	if (-1 == rc) {
		pr_err("socket bind error (%d:%s)", -errno, strerror(errno));
		return -errno;
	}

	rc = listen(sock_fd, MY_MAX_CONNECTED);
	if (-1 == rc) {
		pr_err("socket listen error (%d:%s)", -errno, strerror(errno));
		return -errno;
	}

	pr_info("Server PID is: %d, FD is: %d\n"
		"Server listening on IP: %s\n"
		"Server listening on Port: %d, waiting client ...",
		getpid(),
		sock_fd,
		(char *)inet_ntoa(local_addr.sin_addr),
		SERVER_PORT);

	return sock_fd;
}

void *msg_service(void *data)
{
	struct param_t *param = (struct param_t *)data;
	int client_fd = param->fd;
	fd_set *readfds = param->set;

	char buff[MAX_DATASIZE];
	char resend_buff[MAX_DATASIZE];
	int size;

	pr_info("new thread [%ld], work on fd [%d]", pthread_self(), client_fd);

	while (1) {
		/** 对端发送大量数据时，recv 会多次接收返回 */
		size = recv(client_fd, buff, sizeof(buff), 0);
		if (-1 == size) {
			pr_stderr("[TID:%ld] socket receive error",
				  pthread_self());
			FD_CLR(client_fd, readfds);
			break;
		} else if (0 == size) {
			pr_info("[TID:%ld] the connect [%d] shutdown, now close the connection",
				pthread_self(), client_fd);
			FD_CLR(client_fd, readfds);
			break;
		} else {
			buff[size] = '\0';
			pr_info("[TID:%ld] [Received ID:%d] len:%d, content: %s",
				pthread_self(), client_fd, size, buff);

			sprintf(resend_buff, "[Received ID:%d] recv content: "
				"%.*s\n", client_fd, MAX_DATASIZE - 100, buff);

			if (-1 == send(client_fd, resend_buff,
				       strlen(resend_buff), 0)) {
				pr_stderr("socket send error");
				FD_CLR(client_fd, readfds);
				break;
			}
		}
	}

	close(client_fd);
	return NULL;
}

int accept_fn(int sock_fd)
{
	int client_fd;
	int sin_size = sizeof(struct sockaddr_in);
	struct sockaddr_in remote_addr;

	/** 程序开始监听，在这个地方会阻塞，不消耗 cpu */
	client_fd = accept(sock_fd, (struct sockaddr *)&remote_addr,
				(socklen_t *)&sin_size);
	if (-1 == client_fd) {
		pr_err("socket accept error (%d:%s)", -errno,
			strerror(errno));
		return -errno;
	}

	pr_info("received a connection from %s, the socket ID is %d",
		(char *)inet_ntoa(remote_addr.sin_addr),
		client_fd);

	return client_fd;
}

int main(void)
{
	int rc;
	int i;
	int sock_fd;
	int client_fd;
	pthread_t tid;
	struct param_t param;

	/** Broken pipe, 必须设置 */
	signal(SIGPIPE, SIG_IGN);		/** 13 */

	sock_fd = socket_init();
	if (sock_fd < 0) {
		exit(sock_fd);
	}

	/** select有最大连接数限制，只能有 FD_SETSIZE(1024) 个 */
	/** 创建文件描述符表 */
	fd_set readfds;			// 母本
	FD_ZERO(&readfds);		// 清空

	/** 每次给select使用，因为select会擦除掉一部分 */
	fd_set readfds_temp;		// 用来备份原集合的
	FD_ZERO(&readfds_temp);		// 清空

	/** 记录表中最大的文件描述符，因为文件描述符从 0 到大开始排序 */
	int max_fd = 0;
	/** 将要监视的文件描述符放到表中 */
	FD_SET(sock_fd, &readfds);
	// 记录最大文件描述符
	max_fd = (max_fd > sock_fd ? max_fd : sock_fd);

	while (1) {
		/**
		 * select 每次返回后 会将没有就绪的文件描述符擦除，所以每次调用都需要
		 * 重置这个集合
		 * */
		readfds_temp = readfds;

		/**
		 * 第一个参数 nfds 是一个整数值，是指集合中所有文件描述符的范围，即所
		 * 有文件描述符的最大值加 1
		 * 1. 当返回为 -1 时，所有描述符集清0。
		 * 2. 当返回为 0 时，表示超时。
		 * 3. 当返回为正数时，表示已经准备好的描述符数。
		 * */
		rc = select(max_fd + 1, &readfds_temp, NULL, NULL, NULL);
		if (rc == -1) {
			pr_err("select error (%d:%s)", -errno, strerror(errno));
			break;
		}

		for (i = 0; i < max_fd + 1 && rc > 0; i++) {

			/** 检查对应 bit 为是否有效 */
			if (FD_ISSET(i, &readfds_temp)) {
				if (i == sock_fd) {
					client_fd = accept_fn(sock_fd);
					if (client_fd < 0)
						continue;

					// 将 client_fd 也加入到要监视的集合中
					FD_SET(client_fd, &readfds);
					// 记录最大文件描述符
					max_fd = max_fd > client_fd ?
						 max_fd : client_fd;
				} else {
					/** 客户端发来消息 */
					param.fd = i;
					param.set = &readfds;

					/** TODO: 这里不应该用线程 */
					rc = pthread_create(&tid, NULL,
							    msg_service,
							    (void *)(&param));
					if (rc < 0) {
						pr_stderr("pthread create error");
					}
				}
			}
		}
	}

	pr_info("Service ended, bye...");
	close(sock_fd);
	exit(0);
}
