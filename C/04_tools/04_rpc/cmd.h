/*
 * @Author: Alan Yin
 * @Date: 2024-07-08 13:33:09
 * @LastEditTime: 2024-07-08 19:04:28
 * @LastEditors: Alan Yin
 * @FilePath: /nas_mgmt/rpc/cmd.h
 * @Description:
 * // -*- mode:C; tab-width:8; c-basic-offset:2; indent-tabs-mode:t -*-
 * // vim: ts=8 sw=2 smarttab
 * Copyright (c) 2024 by HT706, All Rights Reserved.
 */

#ifndef NAS_RPC_CMD_H
#define NAS_RPC_CMD_H

#include <netinet/in.h>

#define RPC_PORT		7689
#define RPC_BUFF_MAX		8192

enum {
	RPC_CMD_WRITE_NODELIST = 0,
};

typedef struct rpc_cmd {
	int cmd;		// 命令号
	int rc;			// 返回值
	int len;		// 数据长度
	char ip[INET_ADDRSTRLEN];
	char msg[128];
	char data[0];		// 数据
} rpc_cmd_t;


#endif /** NAS_RPC_CMD_H */