/*
 * @Author: Alan Yin
 * @Date: 2024-07-08 11:48:40
 * @LastEditTime: 2024-07-08 19:13:16
 * @LastEditors: Alan Yin
 * @FilePath: /nas_mgmt/rpc/client.c
 * @Description:
 * // -*- mode:C; tab-width:8; c-basic-offset:2; indent-tabs-mode:t -*-
 * // vim: ts=8 sw=2 smarttab
 * Copyright (c) 2024 by HT706, All Rights Reserved.
 */

#include <sys/socket.h>
#include <arpa/inet.h>		// for inet_aton
#include <syslog.h>
#include <errno.h>

#include "client.h"
#include "tcp_rw.h"
#include "../../common/utils.h"

/**
 * @description: 连接服务器
 * @param {char} *host
 * @param {int} port
 * @return {*}
 */
int tcp_connect(const char *host, int port)
{
	int fd, rc;
	struct sockaddr_in addr;

	memset(&addr, 0, sizeof(addr));

	addr.sin_family = AF_INET;
	addr.sin_port = htons(port);
	rc = inet_aton(host, &addr.sin_addr);
	if (rc == -1) {
		pr_err("inet_aton %s error (%s)", host, strerror(errno));
		return -errno;
	}

	fd = socket(AF_INET, SOCK_STREAM, 0);
	if (fd == -1) {
		pr_err("create socket error (%s)", strerror(errno));
		return -errno;
	}

	rc = connect(fd, (struct sockaddr*)&addr, sizeof(addr));
	if (rc == -1) {
		pr_err("connect to %s error (%s)", host, strerror(errno));
		return -errno;
	}
	return fd;
}

int rpc_call(char *server, int cmd, char *arg)
{
	int rc = 0;
	int sock_fd = 0;
	int send_len = 0;
	int recv_len = 0;
	char recv_buf[RPC_BUFF_MAX] = {0};
	rpc_cmd_t send_cmd = {0};
	rpc_cmd_t *recv_cmd = NULL;

	send_cmd.cmd = cmd;
	strcpy(send_cmd.msg, "hahaha99999999 wetwrwewer");

	sock_fd = tcp_connect(server, RPC_PORT);
	if (sock_fd < 0)
		return sock_fd;

	send_len = sizeof(rpc_cmd_t);
	if (send_len > RPC_BUFF_MAX) {
		pr_err("send msg too long (%d > MAX:%d)", send_len,
			RPC_BUFF_MAX);
		return -EMSGSIZE;
	}

	rc = tcp_stream_call(sock_fd, (char *)&send_cmd, send_len, recv_buf,
			     &recv_len);
	if (rc < 0)
		goto out;

	recv_cmd = (rpc_cmd_t *)recv_buf;
	if (recv_cmd->len != 0) {
		pr_err("server exec error (%d:%s)", recv_cmd->len,
			strerror(-recv_cmd->len));
		return recv_cmd->len;
	}

	pr_info("%s", recv_cmd->msg);
out:
	close(sock_fd);
	return 0;
}
