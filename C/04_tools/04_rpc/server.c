/**
 * @file server.c
 * @author Alan Yin (yinshuangfei007@163.com)
 * @brief
 * @version 0.0.1
 * @date 2025-04-05
 *
 * @copyright Copyright (c) 2025
 *
 * @node:
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/epoll.h>
#include <fcntl.h>
#include <errno.h>
#include <syslog.h>

#include "server.h"
#include "srv_xf.h"
#include "cmd.h"
#include "../../common/utils.h"
#include "../../common/debug.h"

#define MAX_EVENTS		1024

extern int SIGNAL_TERMINATE;

int efd = 0;

typedef struct {
	int fd;
	struct sockaddr_in addr;
	char head[RPC_BUFF_MAX];
	int head_ready;
	int head_total_size;
	int head_recv_size;
	int body_ready;
	int body_total_size;
	int body_recv_size;
	char *data;
} Client;

typedef struct {
	int count;
	// mutex
} Client_Pool;

Client clients[MAX_EVENTS];
Client_Pool client_pool;

static void switch_cmd(Client *client);

Client *find_client(int fd)
{
	for (int i = 0; i < MAX_EVENTS; i++) {
		if (clients[i].fd == fd) {
			return &clients[i];
		}
	}
	return NULL;
}

void init_client(Client *client)
{
	client->head_ready = 0;
	client->head_total_size = PACKAGE_HEAD_LEN;
	client->head_recv_size = 0;
	client->body_ready = 0;
	client->body_total_size = -1;
	client->body_recv_size = 0;
}

// 添加客户端
int add_client(int fd, struct sockaddr_in addr)
{
	if (client_pool.count >= MAX_EVENTS) {
		pr_stderr("Max clients reached");
		return -EPERM;
	}

	clients[client_pool.count].fd = fd;
	clients[client_pool.count].addr = addr;

	init_client(&clients[client_pool.count]);
	client_pool.count++;

	pr_info("dddddddddddddddd %d", client_pool.count);
	return 0;
}

// 移除客户端
void remove_client(int fd)
{
	for (int i = 0; i < MAX_EVENTS; i++) {
		if (clients[i].fd == fd) {
			pr_info("free fd [%d] with dat ptr:%p", fd, clients[i].data);
			SAFE_FREE(clients[i].data);

			// 将最后一个客户端移动到当前位置，覆盖要移除的客户端
			if (i < client_pool.count - 1) {
				clients[i] = clients[client_pool.count - 1];
			}
			client_pool.count--;
			pr_info("xxxxxxxxxxxxxx %d", client_pool.count);
			break;
		}
	}
}

/**
 * @brief 设置套接字为非阻塞模式
 *
 * @param [in] sockfd
 * @return int 0: success, <0: posix errno; >0: value;
 */
int set_socket_nonblocking(int sockfd)
{
	int flags;

	/** 获取当前文件状态标志 */
	flags = fcntl(sockfd, F_GETFL, 0);
	if (flags == -1) {
		pr_stderr("fcntl(F_GETFL) failed");
		return -errno;
	}

	/** 添加非阻塞标志 */
	if (fcntl(sockfd, F_SETFL, flags | O_NONBLOCK) == -1) {
		pr_stderr("fcntl(F_SETFL) failed");
		return -errno;
	}

	return 0;
}

void epoll_del_fd(int fd)
{
	struct epoll_event event = {0};
	int rc = 0;

	event.data.fd = fd;
	rc = epoll_ctl(efd, EPOLL_CTL_DEL, fd, &event);
	if (rc < 0) {
		pr_stderr("add event failed, fd: %d", fd);
	}

	pr_info("fd [%d] disconnect [x]", fd);
	close(fd);
}

/**
 * @description: 初始化套接字
 * @param {int} port
 * @return {*}
 */
int init_sock(int port)
{
	int rc = 0;
	int opt = 1;
	int listen_fd = 0;
	struct sockaddr_in addr = {0};

	listen_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (listen_fd == -1) {
		pr_err("create socket error (%s)", strerror(errno));
		return -errno;
	}

	rc = setsockopt(listen_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT,
			&opt, sizeof(opt));
	if (rc) {
		pr_err("setsockopt error (%s)", strerror(errno));
		return -errno;
	}

	memset(&addr, 0, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = INADDR_ANY;
	addr.sin_port = htons(port);

	rc = bind(listen_fd, (struct sockaddr *)&addr, sizeof(addr));
	if (rc == -1) {
		pr_err("bind socket error (%s)", strerror(errno));
		return -errno;
	}

	rc = listen(listen_fd, SOMAXCONN);
	if (rc == -1) {
		pr_err("listen socket error (%s)", strerror(errno));
		return -errno;
	}

	pr_info("server listen on fd [%d]", listen_fd);

	return listen_fd;
}

/**
 * @description: 初始化 epoll
 * @param {int} port
 * @return {*}
 */
int init_server_epoll(int port)
{
	int rc = 0;
	int sock_fd = 0;
	int new_sock = 0;
	int ready = 0;
	struct epoll_event event = {0};
	struct epoll_event events[MAX_EVENTS] = {0};
	struct sockaddr_in remote_addr = {0};
	socklen_t addrlen = {0};

	sock_fd = init_sock(port);
	if (sock_fd < 0)
		return sock_fd;

	client_pool.count = 0;

	/** 创建 epoll 实例 */
	efd = epoll_create(MAX_EVENTS);
	if (efd < 0) {
		pr_err("create epoll error (%s)", strerror(errno));
		close(sock_fd);
		return -errno;
	}

	event.events = EPOLLIN;
	event.data.fd = sock_fd;

	rc = epoll_ctl(efd, EPOLL_CTL_ADD, sock_fd, &event);
	if (rc < 0) {
		pr_err("epoll add error (%s)", strerror(errno));
		close(sock_fd);
		close(efd);
		return -errno;
	}

	while (SIGNAL_TERMINATE == 0) {
		ready = epoll_wait(efd, events, MAX_EVENTS, -1);

		if (ready == -1) {
			/** 被信号中断，不打印错误 */
			if (errno != EINTR) {
				pr_err("epoll_wait failed (%d:%s)",
					errno, strerror(errno));
			}
			continue;
		}

		for (int i = 0; i < ready; i++) {
			if (events[i].events & EPOLLERR ||
			    events[i].events & EPOLLHUP) {
				pr_warn("recv err event 0x%03x",
					events[i].events);
				/** 这里不清除监控 fd，等到 read == 0 的时候处理 */
			}

			if (0 == (events[i].events & EPOLLIN)) {
				continue;
			}

			if (events[i].data.fd == sock_fd) {
				new_sock = accept(sock_fd,
					(struct sockaddr *)&remote_addr,
					&addrlen);
				if (new_sock < 0) {
					pr_err("accept failed (%s)",
						strerror(errno));
					continue;
				}

				pr_info("new fd [%d] connected [√]", new_sock);

				rc = set_socket_nonblocking(new_sock);
				if (0 != rc) {
					close(new_sock);
					continue;
				}

				event.events = EPOLLIN | EPOLLET;
				event.data.fd = new_sock;
				rc = epoll_ctl(efd, EPOLL_CTL_ADD, new_sock,
					       &event);
				if (rc < 0) {
					pr_err("add event failed, fd: %d (%s)",
						new_sock, strerror(errno));
					close(new_sock);
					continue;
				}

				if (0 != add_client(new_sock, remote_addr)) {
					epoll_del_fd(new_sock);
					continue;
				}
			} else {
				/**
				 * 边缘触发模式下，事件值为
				 * EPOLLET | EPOLLIN = 0x80000001;
				 */
				pr_info("fd [%d] has event [0x%03x]",
					event.data.fd, event.events);
				recv_data(efd, events[i].data.fd);
			}
		}
	}

	return 0;
}

void recv_data(int efd, int client_fd)
{
	int rc = 0;
	Client *client = NULL;

	client = find_client(client_fd);
	if (NULL == client) {
		pr_err("can't find client with fd [%d]", client_fd);
		return;
	}

	/** 读取协议头 */
	if (0 == client->head_ready) {
		rc = read(client->fd, client->head + client->head_recv_size,
			  (size_t)(client->head_total_size - client->head_recv_size));
		if (rc < 0) {
			if (errno == EAGAIN ||
			    errno == EWOULDBLOCK ||
			    errno == EINTR) {
				pr_buff(client->head, client->head_recv_size);
				pr_info("continue read head ...");
			} else {
				/** 例如：EBADF */
				pr_stderr("read [%d] with error", client->fd);
				epoll_del_fd(client_fd);
				remove_client(client_fd);
			}

			return;
		}

		if (rc == 0) {
			/** 这里 0 表示断开，从这里退出 */
			pr_info("recv no head data");
			epoll_del_fd(client_fd);
			remove_client(client_fd);
			return;
		}

		pr_info("head recv: [%d/%d]", rc,
			client->head_total_size - client->head_recv_size);

		client->head_recv_size += rc;
		if (client->head_recv_size == client->head_total_size) {
			client->head_ready = 1;

			uint32_t data_len = 0;
			memcpy(&data_len, client->head + 4, 4);
			data_len = ntohl(data_len);

			pr_buff(client->head, client->head_recv_size);
			pr_info("body size:%d", data_len);

			if (data_len > 0xffffffff) {
				pr_err("recv msg too long (%d > MAX:%d)", data_len,
					0xffffffff);
				epoll_del_fd(client_fd);
				remove_client(client_fd);
				return;
			}

			client->data = calloc(1, data_len);
			if (NULL == client->data) {
				pr_stderr("calloc failed");
				epoll_del_fd(client_fd);
				remove_client(client_fd);
				return;
			}

			pr_info("calloc fd [%d] with data ptr:%p", client->fd,
				client->data);

			client->body_total_size = data_len;
		} else {
			pr_info("head recv no finished");
			return;
		}
	}

	/** 读取数据体 */
	if (0 == client->body_ready) {
		while (1) {
			int len;

			len = client->body_total_size - client->body_recv_size;

			rc = read(client->fd, client->data + client->body_recv_size,
				  len);
			if (rc < 0) {
				if (errno == EAGAIN ||
				    errno == EWOULDBLOCK ||
				    errno == EINTR) {
					// pr_buff(client->data, client->body_recv_size);
					pr_info("have no data, return and epoll wait ... again");
				} else {
					pr_stderr("read [%d] with error", client->fd);
					epoll_del_fd(client_fd);
					remove_client(client_fd);
				}

				return;
			}

			if (rc == 0) {
				/** 这里 0 表示断开，从这里退出 */
				pr_info("recv no data");
				epoll_del_fd(client_fd);
				remove_client(client_fd);
				return;
			}

			pr_info("data recv: [curr:%d, remain:%d, total:%d]", rc,
				client->body_total_size - client->body_recv_size,
				client->body_total_size);

			client->body_recv_size += rc;

			if (client->body_recv_size == client->body_total_size) {
				/** 数据准备好 */
				client->body_ready = 1;
				pr_info("data ready");
				break;
			} else if (client->body_recv_size < client->body_total_size) {
				pr_info("data recv no finished");
				continue;
			} else {
				pr_err("data len is too long (recv:%d>total:%d)",
					client->body_recv_size,
					client->body_total_size);
				epoll_del_fd(client_fd);
				remove_client(client_fd);
				return;
			}
		}
	}

	if (client->head_ready && client->body_ready) {
		pr_info("data ready, switch_cmd");
		switch_cmd(client);
	}
}

static void switch_cmd(Client *client)
{
	// pr_info("recv: %s", cmd->msg);

	// switch (cmd->cmd)
	// {
	// case RPC_CMD_WRITE_NODELIST:
	// 	cmd->rc = 0;
	// 	strcpy(cmd->msg, "this is server msg RPC_CMD_WRITE_NODELIST!");
	// 	break;
	// default:
	// 	break;
	// }

	// pr_info("client [%d] say:%s", client->fd, client->data);

	char msg[1024] = {0};
	sprintf(msg, "server say:%s", "client->data");
	server_stream_call_pack_and_send(client->fd, msg, strlen(msg) + 1);
	init_client(client);

	/** 处理完请求后，不主动断开，等待客户端主动断开 */
}

int main_server(void)
{
	int rc = 0;

	rc = init_server_epoll(RPC_PORT);
	if (rc)
		return rc;

	return 0;
}
