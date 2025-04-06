/**
 * @file srv_xf.h
 * @author Alan Yin (yinshuangfei007@163.com)
 * @brief
 * @version 0.0.1
 * @date 2025-04-05
 *
 * @copyright Copyright (c) 2025
 *
 * @node:
 */

#ifndef SRV_XF_H
#define SRV_XF_H

ssize_t serv_tcp_read(int fd, void *buf, size_t len);
ssize_t serv_tcp_write(int fd, void *buf, size_t len);

int server_stream_call_recv_and_unpack(int fd, void *buf, int *len);
int server_stream_call_pack_and_send(int fd, void *buf, int len);

#endif /** SRV_XF_H */