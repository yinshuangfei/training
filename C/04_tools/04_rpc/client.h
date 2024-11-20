/*
 * @Author: Alan Yin
 * @Date: 2024-07-08 12:33:54
 * @LastEditTime: 2024-07-08 14:10:44
 * @LastEditors: Alan Yin
 * @FilePath: /nas_mgmt/rpc/client.h
 * @Description:
 * // -*- mode:C; tab-width:8; c-basic-offset:2; indent-tabs-mode:t -*-
 * // vim: ts=8 sw=2 smarttab
 * Copyright (c) 2024 by HT706, All Rights Reserved.
 */

#ifndef RPC_CLIENT_H
#define RPC_CLIENT_H

#include "cmd.h"

int rpc_call(char *server, int cmd, char *arg);

#endif /** RPC_CLIENT_H */