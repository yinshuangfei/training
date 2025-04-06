/**
 * @file cli_xf.c
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

#include "cli_xf.h"
#include "../../common/utils.h"

/**
 * @description: 阻塞式接收定长数据
 * 客户端调用
 * @param {int} fd
 * @param {void} *buf
 * @param {size_t} len
 * @return {*}
 */
static ssize_t bio_tcp_read(int fd, void *buf, size_t len)
{
	int rc = 0;
	size_t iolen = 0;

	while (iolen < len) {
		rc = read(fd, buf + iolen, (size_t)(len - iolen));
		if (rc == 0) {
			pr_err("disconnect with server on fd [%d]\n", fd);
			return -EHOSTDOWN;
		} else if (rc < 0) {
			pr_err("read [%d] with error (%d:%s)\n", fd, -errno,
				strerror(errno));
			return -errno;
		}
		iolen += rc;
	}
	return iolen;
}

/**
 * @description: 阻塞式发送定长数据
 * 客户端调用
 * @param {int} fd
 * @param {void} *buf
 * @param {size_t} len
 * @return {*}
 */
static ssize_t bio_tcp_write(int fd, void *buf, size_t len)
{
	int rc = 0;
	size_t iolen = 0;

	while (iolen < len) {
		rc = write(fd, buf + iolen, (size_t)(len - iolen));
		if (rc == 0) {
			pr_err("write [%d] length error (zero)\n", fd);
			return iolen;
		} else if (rc < 0) {
			pr_err("write [%d] with error (%d:%s)\n", fd,
				-errno, strerror(errno));
			return -errno;
		}
		iolen += rc;
	}
	return iolen;
}

/**
 * @description: 打包，发送数据包
 * 客户端调用
 * @param {int} fd
 * @param {void*} buf
 * @param {size_t} len
 * @return {*}
 */
static int stream_call_pack_and_send(int fd, void *buf, size_t len)
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

	rc = bio_tcp_write(fd, send_buf, package_len);
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

/**
 * @description: 接收数据包，解包
 * 客户端调用
 * @param {int} fd
 * @param {void} *
 * @param {size_t} *len
 * @return {*}
 */
static int stream_call_recv_and_unpack(int fd, void *buf, int *len)
{
	uint32_t data_len = 0;
	uint32_t magic = PACKAGE_MAGIC;
	char head_buff[PACKAGE_HEAD_LEN];
	int rc;

	*len = 0;

	rc = bio_tcp_read(fd, head_buff, PACKAGE_HEAD_LEN);
	if (rc < 0)
		return rc;
	else if (rc != PACKAGE_HEAD_LEN)
		return -EIO;

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

	rc = bio_tcp_read(fd, buf, data_len);
	if (rc < 0)
		return rc;
	else if (rc != data_len)
		return -EIO;

	*len = data_len;
	return 0;
}

/**
 * @description: RPC 调用, 使用后释放 recv_buf
 * @param {int} fd
 * @param {size_t} send_len
 * @param {void} *send_buf
 * @param {size_t} *recv_len
 * @param {void} *
 * @return {*}
 */
int tcp_stream_call(int fd, char *send_buf, int send_len, char *recv_buf,
		    int *recv_len)
{
	int rc;

	rc = stream_call_pack_and_send(fd, (void *)send_buf, send_len);
	if (rc)
		return rc;

	rc = stream_call_recv_and_unpack(fd, (void *)recv_buf, recv_len);
	if (rc)
		return rc;

	return 0;
}
