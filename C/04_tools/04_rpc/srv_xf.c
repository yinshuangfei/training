/**
 * @file srv_xf.c
 * @author Alan Yin (yinshuangfei007@163.com)
 * @brief
 * @version 0.0.1
 * @date 2025-04-05
 *
 * @copyright Copyright (c) 2025
 *
 * @node:
 */

#include <netinet/in.h>
#include <errno.h>
#include <syslog.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "server.h"
#include "../../common/utils.h"
#include "../../common/debug.h"
#include "cmd.h"

/**
 * @description: 服务端调用
 * @param {int} fd
 * @param {void} *buf
 * @param {size_t} len
 * @return {*}
 */
ssize_t serv_tcp_read(int fd, void *buf, size_t len)
{
	int rc = 0;
	size_t iolen = 0;

	while (iolen < len) {
		rc = read(fd, buf + iolen, (size_t)(len - iolen));
		if (rc < 0) {
			if (errno == EAGAIN ||
			    errno == EWOULDBLOCK ||
			    errno == EINTR) {
				pr_buff(buf, iolen);
				pr_info("continue read ...");
			} else {
				pr_stderr("read [%d] with error", fd);
			}

			return -errno;
		}
		if (rc == 0) {
			/** 这里 0 表示断开 */
			pr_info("with fd [%d] disconnect [x]", fd);
			return -EHOSTDOWN;
		}
		iolen += rc;
	}
	return iolen;
}

/**
 * @description: 服务端调用
 * @param {int} fd
 * @param {void*} buf
 * @param {size_t} len
 * @return {*}
 */
ssize_t serv_tcp_write(int fd, void *buf, size_t len)
{
	int rc = 0;
	size_t iolen = 0;

	while (iolen < len) {
		rc = write(fd, buf + iolen, (size_t)(len - iolen));
		if (rc <= 0) {
			pr_err("write [%d] with error (%d:%s)\n", fd,
				-errno, strerror(errno));
			return rc;
		}
		iolen += rc;
	}
	return iolen;
}

/**
 * @description: 服务端调用
 * @param {int} fd
 * @param {void} *
 * @param {size_t} *len
 * @return {*}
 */
int server_stream_call_recv_and_unpack(int fd, void *buf, int *len)
{
	uint32_t data_len = 0;
	uint32_t magic = PACKAGE_MAGIC;
	char head_buff[PACKAGE_HEAD_LEN];
	int rc;

	*len = 0;

	rc = serv_tcp_read(fd, head_buff, PACKAGE_HEAD_LEN);
	if (rc < 0) {
		return rc;
	} else if (rc == 0) {
		pr_info("new fd [%d] disconnected [x]", fd);
		epoll_del_fd(fd);
	} else if (rc != PACKAGE_HEAD_LEN) {
		return -EIO;
	}

	if (memcmp(head_buff, &magic, 4) != 0) {
		pr_err("recv magic error (0x%x%x%x%x)",
			*(head_buff + 3),
			*(head_buff + 2),
			*(head_buff + 1),
			*(head_buff + 0));
		return -EIO;
	}

	memcpy(&data_len, head_buff + 4, 4);
	data_len = ntohl(data_len);

	if (data_len > RPC_BUFF_MAX) {
		pr_err("recv msg too long (%d > MAX:%d)", data_len,
			RPC_BUFF_MAX);
		return -EMSGSIZE;
	}

	rc = serv_tcp_read(fd, buf, data_len);
	if (rc < 0)
		return rc;
	else if (rc != data_len)
		return -EIO;

	*len = data_len;
	return 0;
}

int server_stream_call_pack_and_send(int fd, void *buf, int len)
{
	int rc;
	char *send_buf = NULL;
	uint32_t l = htonl((uint32_t)len);
	uint32_t magic = PACKAGE_MAGIC;
	size_t package_len = len + PACKAGE_HEAD_LEN;

	send_buf = malloc(package_len);
	if (send_buf == NULL) {
		pr_err("malloc data (len:%lu) failed", package_len);
		return -ENOMEM;
	}

	memcpy(send_buf, &magic, 4);
	memcpy(send_buf + 4, &l, 4);
	memcpy(send_buf + PACKAGE_HEAD_LEN, buf, len);

	rc = serv_tcp_write(fd, send_buf, package_len);
	if (rc < 0) {
		goto out;
	} else if (rc != package_len) {
		pr_err("send length error (src:%lu, send:%d)",
			package_len, rc);
		rc = -EIO;
		goto out;
	}

	rc = 0;
out:
	free(send_buf);
	return rc;
}