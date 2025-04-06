/*
 * @Author: Alan Yin
 * @Date: 2024-08-10 00:18:33
 * @LastEditTime: 2024-08-10 00:18:39
 * @LastEditors: Alan Yin
 * @FilePath: /C/99_demo/03_rpc/client_main.c
 * @Description:
 * @// -*- mode:C; tab-width:8; c-basic-offset:2; indent-tabs-mode:t -*-
 * @// vim: ts=8 sw=2 smarttab
 * @Copyright (c) 2024 by Alan Yin, All Rights Reserved.
 */
#include <pthread.h>

#include "../../common/utils.h"

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

#include "cmd.h"
#include "cli_xf.h"

#define MAX_DATASIZE 4096

int loop_test(void)
{
	int sock_fd;
	int rc;
	char buff[MAX_DATASIZE*1000] = {0};
	char recv_buff[MAX_DATASIZE] = {0};
	int recv_size;

	struct sockaddr_in server_addr, client_addr;
	const char *server_ip = "192.168.81.66";	// Remote server IP
	int server_port = RPC_PORT;			// Remote server port
	const char *local_ip = "192.168.81.66";		// Local IP address to bind to

reconnect:
	sock_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (-1 == sock_fd) {
		pr_stderr("socket init error");
		return -errno;
	}

	memset(&client_addr, 0, sizeof(client_addr));
	client_addr.sin_family = AF_INET;
	client_addr.sin_port = 0;
	client_addr.sin_addr.s_addr = inet_addr(local_ip);
	bzero(&(client_addr.sin_zero), 8);

	rc = bind(sock_fd, (struct sockaddr*)&client_addr, sizeof(client_addr));
	if (rc < 0) {
		pr_stderr("Bind failed");
		return 1;
	}

	memset(&server_addr, 0, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(server_port);
	server_addr.sin_addr.s_addr = inet_addr(server_ip);
	bzero(&(server_addr.sin_zero), 8);

	rc = connect(sock_fd, (struct sockaddr*)&server_addr,
		     sizeof(server_addr));
	if (rc < 0) {
		pr_stderr("socket connect error, exit now.");
		return 1;
	}

	pr_info("Successfully connected to the server at %s:%d from local IP %s",
		server_ip, server_port, local_ip);
	pr_info("client PID is: %d", getpid());

	while (1) {
		// rc = tcp_stream_call(sock_fd, buff, strlen(buff) + 1, recv_buff,
		rc = tcp_stream_call(sock_fd, buff, sizeof(buff), recv_buff,
				     &recv_size);
		if (EHOSTDOWN == -rc) {
			pr_info("reconnect server ...");
			close(sock_fd);
			goto reconnect;
		} else if (0 != rc) {
			pr_err("tcp_stream_call error ...");
			break;
		}

		pr_info("recv seccess, len%d, buff:%s", recv_size, recv_buff);
		close(sock_fd);
		goto reconnect;

		/** 在子进程收到信息显示后，再执行 */
		usleep(1000);
	}

	return 0;
}

void *worker(void *args)
{
	loop_test();
	return NULL;
}

int main(int argc, char *argv[])
{
// 	int sock_fd;
// 	int size;
// 	int rc;
// 	char buff[MAX_DATASIZE*1000] = {0};
// 	char recv_buff[MAX_DATASIZE] = {0};
// 	int recv_size;

// 	struct sockaddr_in server_addr, client_addr;
// 	const char *server_ip = "192.168.81.66";	// Remote server IP
// 	int server_port = RPC_PORT;			// Remote server port
// 	const char *local_ip = "192.168.81.66";		// Local IP address to bind to

// reconnect:
// 	sock_fd = socket(AF_INET, SOCK_STREAM, 0);
// 	if (-1 == sock_fd) {
// 		pr_stderr("socket init error");
// 		return -errno;
// 	}

// 	memset(&client_addr, 0, sizeof(client_addr));
// 	client_addr.sin_family = AF_INET;
// 	client_addr.sin_port = 0;
// 	client_addr.sin_addr.s_addr = inet_addr(local_ip);
// 	bzero(&(client_addr.sin_zero), 8);

// 	rc = bind(sock_fd, (struct sockaddr*)&client_addr, sizeof(client_addr));
// 	if (rc < 0) {
// 		pr_stderr("Bind failed");
// 		return 1;
// 	}

// 	memset(&server_addr, 0, sizeof(server_addr));
// 	server_addr.sin_family = AF_INET;
// 	server_addr.sin_port = htons(server_port);
// 	server_addr.sin_addr.s_addr = inet_addr(server_ip);
// 	bzero(&(server_addr.sin_zero), 8);

// 	rc = connect(sock_fd, (struct sockaddr*)&server_addr,
// 		     sizeof(server_addr));
// 	if (rc < 0) {
// 		pr_stderr("socket connect error, exit now.");
// 		return 1;
// 	}

// 	pr_info("Successfully connected to the server at %s:%d from local IP %s",
// 		server_ip, server_port, local_ip);
// 	pr_info("client PID is: %d", getpid());
// 	pr_info("Input 'exit' to exit.");

// 	while (1) {
// 		printf("> ");

// 		/** 去除回车 \n */
// 		fgets(buff, MAX_DATASIZE, stdin);
// 		buff[strlen(buff) - 1] = '\0';

// 		// 检测是否要离开
// 		if (0 == strcmp("exit", buff) || 0 == strcmp("q", buff)) {
// 			pr_info("Welcome to use next time.");
// 			close(sock_fd);
// 			exit(0);
// 		}

// 		// rc = tcp_stream_call(sock_fd, buff, strlen(buff) + 1, recv_buff,
// 		rc = tcp_stream_call(sock_fd, buff, sizeof(buff), recv_buff,
// 				     &recv_size);
// 		if (EHOSTDOWN == -rc) {
// 			pr_info("reconnect server ...");
// 			close(sock_fd);
// 			goto reconnect;
// 		} else if (0 != rc) {
// 			break;
// 		}

// 		pr_info("recv seccess, len%d, buff:%s", recv_size, recv_buff);
// 		close(sock_fd);
// 		goto reconnect;

// 		/** 在子进程收到信息显示后，再执行 */
// 		usleep(1000);
// 	}

// 	close(sock_fd);
// 	pr_info("my service is satisfied, bye...");
// 	exit(0);

#define MAX_THR 10
	pthread_t tids[MAX_THR];
	int rc = 0;

	for (int i = 0; i < MAX_THR; i++) {
		rc = pthread_create(&tids[i], NULL, worker, NULL);
		if (-1 == rc) {
			pr_stderr("pthread_create failed");
			exit(EXIT_FAILURE);
		}
	}

	for (int i = 0; i < MAX_THR; i++) {
		rc = pthread_join(tids[i], NULL);
		if (-1 == rc) {
			pr_stderr("pthread_join failed");
			exit(EXIT_FAILURE);
		}
	}

	return 0;
}
