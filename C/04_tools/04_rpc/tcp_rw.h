/*
 * @Author: Alan Yin
 * @Date: 2024-07-08 13:16:39
 * @LastEditTime: 2024-07-08 17:55:01
 * @LastEditors: Alan Yin
 * @FilePath: /nas_mgmt/rpc/tcp_rw.h
 * @Description:
 * // -*- mode:C; tab-width:8; c-basic-offset:2; indent-tabs-mode:t -*-
 * // vim: ts=8 sw=2 smarttab
 * Copyright (c) 2024 by HT706, All Rights Reserved.
 */

#ifndef NAS_TCP_RW_H
#define NAS_TCP_RW_H

#include <stdint.h>		// for uint64_t

#include "cmd.h"

#define PACKAGE_HEAD_LEN	sizeof(uint64_t)
#define PACKAGE_MAGIC		0xe0e1e2e3

int server_stream_recv_and_unpack(int fd, void *buf, int *len);
int server_stream_pack_and_send(int fd, void *buf, int len);

int tcp_stream_call(int fd, char *send_buf, int send_len, char *recv_buf,
		    int *recv_len);

#endif /** NAS_TCP_RW_H */