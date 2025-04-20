/**
 * @file server.h
 * @author Alan Yin (yinshuangfei007@163.com)
 * @brief
 * @version 0.0.1
 * @date 2025-04-05
 *
 * @copyright Copyright (c) 2025
 *
 * @node:
 */

#ifndef NAS_RPCSERVER_H
#define NAS_RPCSERVER_H

#include "cmd.h"

void epoll_del_fd(int fd);

int init_sock(int port);
int init_server_epoll(int port);
void epoll_recv(int efd, int client_fd);
// static void switch_cmd(rpc_cmd_t *cmd);

int main_server(void);

#endif /** NAS_RPCSERVER_H */