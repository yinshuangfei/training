/*
 * @Author: Alan Yin
 * @Date: 2024-05-19 22:11:33
 * @LastEditTime: 2024-05-30 23:53:09
 * @LastEditors: Alan Yin
 * @FilePath: /windows_cifs/training/C/03_socket/03_poll/server.c
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
#include <poll.h>

#include "../common.h"

/**
 * https://blog.csdn.net/weixin_43757283/article/details/119185222
*/

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

	while (1) {
		size = recv(client_fd, buff, sizeof(buff), 0);
		if (-1 == size) {
			pr_err("socket receive error (%d:%s)", -errno,
				strerror(errno));
			FD_CLR(client_fd, readfds);
			break;
		} else if (0 == size) {
			pr_info("the connect [%d] shutdown, now close the "
				"connection.", client_fd);
			FD_CLR(client_fd, readfds);
			break;
		} else {
			buff[size] = '\0';
			pr_info("[Received ID:%d] content: %s", client_fd,
				buff);

			sprintf(resend_buff, "[Received ID:%d] recv content: "
				"%.*s\n", client_fd, MAX_DATASIZE - 100, buff);

			if (-1 == send(client_fd, resend_buff,
				       strlen(resend_buff), 0)) {
				pr_err("socket send error, try again");
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
	int max = 0;
	struct param_t param;

	/** Broken pipe, 必须设置 */
	signal(SIGPIPE, SIG_IGN);		/** 13 */

	/**
	 * poll() 没有最大文件描述符数量的限制（但是数量过大后性能也是会下降）
	 * poll() 和 select() 同样存在一个缺点就是，包含大量文件描述符的数组被整体复
	 * 制于用户态和内核的地址空间之间，而不论这些文件描述符是否就绪，它的开销随着文
	 * 件描述符数量的增加而线性增大.
	 */
	struct pollfd fds_array[1024];

	sock_fd = socket_init();
	if (sock_fd < 0) {
		exit(sock_fd);
	}

	/* 将数组中每个元素都设置为-1，相当于清空fd */
	for (i = 0; i < ARRAY_SIZE(fds_array); i++) {
		fds_array[i].fd = -1;
	}

	/* 将listen_fd和期待POLLIN存入结构体数组第0个元素中 */
	fds_array[0].fd = sock_fd;
	fds_array[0].events = POLLIN;

	/* 当前结构体中，最大的下标是0 */
	max = 0;

	while (1) {
		/**
		 * 第二个参数 nfds 是一个整数值，是指集合中所有文件描述符的范围，即所
		 * 有文件描述符的最大值加 1；
		 * timeout 指定为负数值表示无限超时；
		 * timeout 为 0 指示 poll 调用立即返回并列出准备好 I/O 的文件描述符；
		 *
		 * 返回值如下：
		 * 1. poll() 返回结构体中 revents 域不为0的文件描述符个数；
		 * 2. 返回 0：说明在超时前没有任何事件发生；
		 * 3. 返回 -1：说明失败，并设置errno为下列值之一：
		 * 	EBADF：一个或多个结构体中指定的文件描述符无效。
		 * 	EFAULT：指针指向的地址超出进程的地址空间。
		 * 	EINTR：请求的事件之前产生一个信号，调用可以重新发起。
		 * 	EINVAL：参数超出PLIMIT_NOFILE值。
		 * 	ENOMEM：可用内存不足，无法完成请求。
		 * */
		rc = poll(fds_array, max + 1, -1);
		if (rc == -1) {
			pr_err("poll error (%d:%s)", -errno, strerror(errno));
			break;
		}

		if (rc == 0) {
			continue;
		}

		/* 判断是不是 sock_fd 的消息 */
		if (fds_array[0].revents & POLLIN) {
			client_fd = accept_fn(sock_fd);
			if (client_fd < 0)
				continue;

			/**
			 * 在把 client_fd 放到数组中的空位中 （元素的值为-1的地方）
			 */
			int found = 0;

			for (int i = 0; i < ARRAY_SIZE(fds_array); i++) {
				if (fds_array[i].fd < 0) {
					pr_info("accept new client [%d] and add"
						" it to array", client_fd);
					fds_array[i].fd = client_fd;
					fds_array[i].events = POLLIN;
					found = 1;
					/** 更新结构体数组中的当前最大下标 */
					max = i > max ? i : max;
					break;
				}
			}

			if (!found) {
				pr_err("accept new client [%d], but full, so "
					"refuse", client_fd);
				close(client_fd);
			}
		} else {
			/** TODO: 客户端发来消息 */
			// param.fd = i;
			// param.set = &readfds;

			// rc = pthread_create(&tid, NULL,
			// 			msg_service,
			// 			(void *)(&param));
			// if (rc < 0) {
			// 	pr_err("pthread create error "
			// 		"(%d:%s)",
			// 		-errno,
			// 		strerror(errno));
			// }
		}
	}

	pr_info("Service ended, bye...");
	close(sock_fd);
	exit(0);
}
