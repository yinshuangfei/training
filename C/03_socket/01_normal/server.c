/*
 * @Author: Alan Yin
 * @Date: 2024-05-19 22:11:33
 * @LastEditTime: 2024-05-29 00:28:31
 * @LastEditors: Alan Yin
 * @FilePath: /windows_cifs/training/C/03_socket/01_normal/server.c
 * @Description:
 * // -*- mode:C; tab-width:8; c-basic-offset:2; indent-tabs-mode:t -*-
 * // vim: ts=8 sw=2 smarttab
 * Copyright (c) 2024 by Alan Yin, All Rights Reserved.
 */

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>

#include "../common.h"

#define MAX_DATASIZE 4096

static int socket_init(void)
{
	int sock_fd = -1;
	int rc = 0;
	int on = 1;
	struct sockaddr_in local_addr;

	/** 创建套接字 */
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

	/** 填写套接字描述符 */
	// internet连接的默认值
	local_addr.sin_family = AF_INET;
	// 将端口号转换为标准格式，为 0 则由系统自己分配一个未占用的端口号
	local_addr.sin_port = htons(SERVER_PORT);
	// 接受任何 IP 地址的访问
	// 将点分十进制 IP 地址转换为机器识别的地址
	// local_addr.sin_addr.s_addr = inet_addr(SERVER_IP_ADDR);
	local_addr.sin_addr.s_addr = INADDR_ANY;
	// sockaddr_in结构体的剩余字节处用0填充
	bzero(&(local_addr.sin_zero), 8);

	/** 绑定套接字 */
	rc = bind(sock_fd, (struct sockaddr *)&local_addr,
		  sizeof(struct sockaddr));
	if (-1 == rc) {
		pr_err("socket bind error (%d:%s)", -errno, strerror(errno));
		return -errno;
	}

	/** 设置监听 */
	rc = listen(sock_fd, MY_MAX_CONNECTED);
	if (-1 == rc) {
		pr_err("socket listen error (%d:%s)", -errno, strerror(errno));
		return -errno;
	}

	/** 提示语句 */
	pr_info("Server PID is: %d, FD is: %d\n"
		"Server listening on IP: %s\n"
		"Server listening on Port: %d, waiting client ...",
		getpid(),
		sock_fd,
		(char *)inet_ntoa(local_addr.sin_addr),
		SERVER_PORT);

	return sock_fd;
}

void msg_service(int client_fd)
{
	char buff[MAX_DATASIZE];
	char resend_buff[MAX_DATASIZE];
	int size;

	while (1) {
		size = recv(client_fd, buff, sizeof(buff), 0);
		if (-1 == size) {
			pr_err("socket receive error (%d:%s)", -errno,
				strerror(errno));
			exit(-errno);
		}
		/** 检测连接是否断开 */
		else if (0 == size) {
			pr_info("the connect [%d] shutdown, now close the "
				"connection.", client_fd);
			break;
		} else {
			buff[size] = '\0';
			pr_info("[Received ID:%d] content: %s", client_fd,
				buff);

			sprintf(resend_buff, "[Received ID:%d] recv content: "
				"%.*s\n", client_fd, MAX_DATASIZE - 100, buff);

			/** 发送给其他人 */
			if (-1 == send(client_fd, resend_buff,
				       strlen(resend_buff), 0)) {
				pr_err("socket send error, try again");
			}
		}
	}

	close(client_fd);
}

int main(void)
{
	int sock_fd;
	int client_fd;
	int sin_size = sizeof(struct sockaddr_in);
	struct sockaddr_in remote_addr;

	sock_fd = socket_init();
	if (sock_fd < 0) {
		exit(sock_fd);
	}

	while (1) {
		/** 程序开始监听，在这个地方会阻塞，不消耗 cpu */
		client_fd = accept(sock_fd, (struct sockaddr *)&remote_addr,
				   (socklen_t *)&sin_size);
		if (-1 == client_fd) {
			pr_err("socket accept error (%d:%s)", -errno,
			       strerror(errno));
			continue;
		}

		/** 子进程 */
		if (0 == fork()) {
			pr_info("Son process(PID=%d): received a connection "
				"from %s, the socket ID is %d", getpid(),
				(char *)inet_ntoa(remote_addr.sin_addr),
				client_fd);

			msg_service(client_fd);
		}
		/** 父进程 */
		else {
			pr_info("Father process(PID=%d): I will not provode the "
				"tille sevice", getpid());
		}
	}

	pr_info("Service ended, bye...");
	close(sock_fd);
	exit(0);
}
