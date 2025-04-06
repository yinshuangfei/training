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

int init_sock(char *server_ip, int server_port, char *local_ip)
{
	int sock_fd;
	int rc = 0;
	struct sockaddr_in server_addr, client_addr;

	/**
	 * 创建套接字
	 * Step 1: Create a socket
	 */
	sock_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (-1 == sock_fd) {
		pr_stderr("socket init error");
		return -errno;
	}

	/**
	 * 指定客户端 IP 地址
	 * Step 2: Set up the client address structure
	 */
	memset(&client_addr, 0, sizeof(client_addr));
	client_addr.sin_family = AF_INET;
	/**
	 * 使用特定端口可能会报错：Address already in use（被占用），为 0 则由系统
	 * 自己分配一个未占用的端口号。
	 * Use ephemeral port（短暂端口）, OS will pick an available one
	 */
	client_addr.sin_port = 0;
	/** 以下方式自由选择，设置本地使用什么 IP 地址连接 */
#if 0
	client_addr.sin_addr.s_addr = inet_addr(local_ip);
	bzero(&(client_addr.sin_zero), 8);
#else
	/** 将点分十进制的地址 local_ip 转换为接口类型为 AF_INET 的二进制网络地址 */
	rc = inet_pton(AF_INET, local_ip, &client_addr.sin_addr);
	if (rc <= 0) {
		pr_stderr("Invalid local IP address");
		return -errno;
	}
#endif
	/**
	 * Step 3: Bind the socket to the local IP address
	 */
	rc = bind(sock_fd, (struct sockaddr*)&client_addr, sizeof(client_addr));
	if (rc < 0) {
		pr_stderr("Bind failed");
		return -errno;
	}

	/**
	 * 填写套接字描述符
	 * Step 4: Set up the server address structure
	 */
	memset(&server_addr, 0, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	/** 将端口号转换为标准格式 */
	server_addr.sin_port = htons(server_port);
	/** 以下方式自由选择 */
#if 0
	server_addr.sin_addr.s_addr = inet_addr(server_ip);
	bzero(&(server_addr.sin_zero), 8);
#else
	rc = inet_pton(AF_INET, server_ip, &server_addr.sin_addr);
	if (rc <= 0) {
		pr_stderr("Invalid server IP address");
		return -errno;
	}
#endif
	/** 连接套接字
	 * Step 5: Connect to the remote server
	 */
	rc = connect(sock_fd, (struct sockaddr*)&server_addr,
		     sizeof(server_addr));
	if (rc < 0) {
		pr_stderr("socket connect (%s:%d) error, exit now", server_ip,
			  server_port);
		return -errno;
	}

	return sock_fd;
}

void interactive_mode(int sock_fd)
{
	int size;
	int pid;
	char buff[MAX_DATASIZE];

	/** 子线程用于接受 */
	if (0 == (pid = fork())) {
		while (1) {
			/** 数据过大，会多次接受 */
			if (-1 == (size = recv(sock_fd, buff, sizeof(buff),
					       0))) {
				pr_stderr("socket receive error, exit now");
				exit(1);
			} else if (size == 0) {
				pr_info("server disconnected, child exit now");
				exit(1);
			} else {
				buff[size] = '\0';
				printf("[recv:%d] %s\n", size, buff);
			}
		}
	}
	/** 父线程用于发送 */
	else {
		pr_info("Input 'exit' or 'q' to exit.");

		while (1) {
			printf("> ");

			/** 去除回车 \n */
			fgets(buff, MAX_DATASIZE, stdin);
			buff[strlen(buff) - 1] = '\0';

			// 检测是否要离开
			if (0 == strcmp("exit", buff) ||
			    0 == strcmp("q", buff)) {
				pr_info("Welcome to use next time");
				close(sock_fd);
				kill(pid, 9);
				exit(0);
			}
			// 发送消息, 模拟发送大量数据
			// if (-1 == send(sock_fd, buff, strlen(buff), 0)) {
			if (-1 == send(sock_fd, buff, sizeof(buff), 0)) {
				pr_err("socket send error, try again");
			}

			/** 在子进程收到信息显示后，再执行 */
			usleep(1000);
		}
	}
}

void loop_mode(int sock_fd)
{
	int size;
	int pid;
	char buff[MAX_DATASIZE];

	/** 子线程用于接受 */
	if (0 == (pid = fork())) {
		while (1) {
			/** 数据过大，会多次接受 */
			if (-1 == (size = recv(sock_fd, buff, sizeof(buff),
					       0))) {
				pr_stderr("socket receive error, exit now");
				exit(1);
			} else if (size == 0) {
				pr_info("server disconnected, child exit now");
				exit(1);
			} else {
				buff[size] = '\0';
				printf("[recv:%d] %s\n", size, buff);
			}
		}
	}
	/** 父线程用于发送 */
	else {
		while (1) {
			sprintf(buff, "random: %ld", random());

			if (-1 == send(sock_fd, buff, sizeof(buff), 0)) {
				pr_err("socket send error, try again");
			}

			/** 防止服务端粘包 */
			usleep(20000);
		}
	}
}

int main(int argc, char *argv[])
{
	int sock_fd;

	/** Remote server IP */
	char *server_ip = "192.168.81.66";
	/** Remote server port */
	int server_port = SERVER_PORT;
	/** Local IP address to bind to */
	char *local_ip = "192.168.81.66";

	sock_fd = init_sock(server_ip, server_port, local_ip);
	if (sock_fd < 0) {
		return sock_fd;
	}

	pr_info("Successfully connected to the server at %s:%d from local IP %s",
		server_ip, server_port, local_ip);
	pr_info("client PID is: %d", getpid());

	// interactive_mode(sock_fd);
	loop_mode(sock_fd);

	close(sock_fd);
	pr_info("my service is satisfied, bye...");
	exit(0);
}
