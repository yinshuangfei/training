/*
 * @Author: Alan Yin
 * @Date: 2024-07-08 10:03:02
 * @LastEditTime: 2024-07-08 19:23:11
 * @LastEditors: Alan Yin
 * @FilePath: /nas_mgmt/rpc/server.h
 * @Description:
 * // -*- mode:C; tab-width:8; c-basic-offset:2; indent-tabs-mode:t -*-
 * // vim: ts=8 sw=2 smarttab
 * Copyright (c) 2024 by HT706, All Rights Reserved.
 */

#ifndef NAS_RPCSERVER_H
#define NAS_RPCSERVER_H

#include "cmd.h"

static int init_sock(int port);
static int init_server_epoll(int port);
static void do_service(int efd, int client_fd, int *client_fds);
static void switch_cmd(rpc_cmd_t *cmd);

int main_server(void);

#endif /** NAS_RPCSERVER_H */