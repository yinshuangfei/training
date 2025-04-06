/**
 * @file cli_xf.h
 * @author Alan Yin (yinshuangfei007@163.com)
 * @brief
 * @version 0.0.1
 * @date 2025-04-05
 *
 * @copyright Copyright (c) 2025
 *
 * @node:
 */
#ifndef CLI_XF_H
#define CLI_XF_H

#include "cmd.h"

int tcp_stream_call(int fd, char *send_buf, int send_len, char *recv_buf,
		    int *recv_len);

#endif /** CLI_XF_H */