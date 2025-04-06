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

typedef struct _client_t client_t;
typedef struct _client_pool_t client_pool_t;

void init_client(client_t *client);
void do_server_demo(client_t *client);
ssize_t serv_fix_write(int fd, void *buf, size_t len);
int init_server_epoll(int port);
int server_loop(void);

#endif /** NAS_RPCSERVER_H */