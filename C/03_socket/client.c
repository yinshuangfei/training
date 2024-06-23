/*
 * @Author: Alan Yin
 * @Date: 2024-05-19 22:11:29
 * @LastEditTime: 2024-05-28 23:46:25
 * @LastEditors: Alan Yin
 * @FilePath: /windows_cifs/training/C/03_socket/client.c
 * @Description:
 * // -*- mode:C; tab-width:8; c-basic-offset:2; indent-tabs-mode:t -*-
 * // vim: ts=8 sw=2 smarttab
 * Copyright (c) 2024 by Alan Yin, All Rights Reserved.
 */

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <unistd.h>
#include <arpa/inet.h>

#include "common.h"

#define MAX_DATASIZE 4096

int main(int argc, char *argv[])
{
	int sock_fd;
	int size;
	int pid;
	char buff[MAX_DATASIZE];
	struct sockaddr_in server_addr;

	/** 创建套接字 */
	sock_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (-1 == sock_fd) {
		pr_err("socket init error (%d:%s)\n", -errno, strerror(errno));
		return -errno;
	}

	/** 填写套接字描述符 */
	// internet 连接的默认值
	server_addr.sin_family = AF_INET;
	// 将端口号转换为标准格式，为 0 则由系统自己分配一个未占用的端口号
	server_addr.sin_port = htons(SERVER_PORT);
	server_addr.sin_addr.s_addr = inet_addr(SERVER_IP_ADDR);
	// sockaddr_in 结构体的剩余字节处用 0 填充
	bzero(&(server_addr.sin_zero), 8);

	/** 连接套接字 */
	if (-1 == connect(sock_fd, (struct sockaddr *)&server_addr,
			  sizeof(struct sockaddr))) {
		pr_info("socket connect error, exit now.");
		exit(1);
	}

	pr_info("client PID is: %d", getpid());
	pr_info("Input 'exit' to exit.");

	/** 子线程用于接受 */
	if (0 == (pid = fork())) {
		while (1) {
			if (-1 == (size = recv(sock_fd, buff, sizeof(buff),
					       0))) {
				pr_err("socket receive error, exit now.");
				exit(1);
			} else if (size == 0) {
				pr_info("server disconnected, child exit now.");
				exit(1);
			} else {
				buff[size] = '\0';
				printf("%s", buff);
			}
		}
	}
	/** 父线程用于发送 */
	else {
		while (1) {
			printf("> ");

			/** 去除回车 \n */
			fgets(buff, MAX_DATASIZE, stdin);
			buff[strlen(buff) - 1] = '\0';

			// 检测是否要离开
			if (0 == strcmp("exit", buff)) {
				pr_info("Welcome to use next time.");
				close(sock_fd);
				kill(pid, 9);
				exit(0);
			}
			// 发送消息
			if (-1 == send(sock_fd, buff, strlen(buff), 0)) {
				pr_err("socket send error, try again.");
			}

			/** 在子进程收到信息显示后，再执行 */
			usleep(1000);
		}

	}

	close(sock_fd);
	pr_info("my service is satisfied, bye...\n");
	exit(0);
}







