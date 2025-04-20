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
#include <pthread.h>

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

	int head_ready;
	int head_total_size;
	int head_recv_size;

	int body_ready;
	int body_total_size;
	int body_recv_size;

	char head[PACKAGE_HEAD_LEN];
	char *data;
} client_t;

typedef struct {
	int count;
	pthread_mutex_t mutex;
} client_pool_t;

client_t clients[MAX_EVENTS];
client_pool_t client_pool;

static void switch_cmd(client_t *client);

/**
 * @brief 初始化用户池用户
 *
 * @param [in out] client
 */
void init_client(client_t *client)
{
	client->head_ready = 0;
	client->head_total_size = PACKAGE_HEAD_LEN;
	client->head_recv_size = 0;
	client->body_ready = 0;
	client->body_total_size = -1;
	client->body_recv_size = 0;

	// if (NULL != client->data) {
	// 	pr_err("client->data is not NULL (p:%p)", client->data);
	// 	SAFE_FREE(client->data);
	// 	client->data = NULL;
	// }
}

/**
 * @brief 根据 fd 查找 client
 *
 * @param [in] fd
 * @return client_t* client: success, NULL: failed;
 */
client_t *find_client(int fd)
{
	pthread_mutex_lock(&client_pool.mutex);
	for (int i = 0; i < MAX_EVENTS; i++) {
		if (clients[i].fd == fd) {
			pthread_mutex_unlock(&client_pool.mutex);
			return &clients[i];
		}
	}
	pthread_mutex_unlock(&client_pool.mutex);
	return NULL;
}

/**
 * @brief 添加客户端至用户池
 *
 * @param [in] fd
 * @param [in] addr
 * @return int 0: success, <0: failed, posix errno;
 */
int add_client(int fd, struct sockaddr_in addr)
{
	pthread_mutex_lock(&client_pool.mutex);
	if (client_pool.count >= MAX_EVENTS) {
		pthread_mutex_unlock(&client_pool.mutex);
		pr_stderr("Max clients reached");
		return -EPERM;
	}

	clients[client_pool.count].fd = fd;
	clients[client_pool.count].addr = addr;
	init_client(&clients[client_pool.count]);
	// clients[client_pool.count].data = NULL;

	pr_info("after add fd [%d], index:%d, client_pool.count:%d, data prt:%p", fd,
		client_pool.count, client_pool.count + 1, clients[client_pool.count].data);

	client_pool.count++;
	pthread_mutex_unlock(&client_pool.mutex);

	return 0;
}

/**
 * @brief 移除客户端
 *
 * @param [in out] fd
 */
void remove_client(int fd)
{
	pthread_mutex_lock(&client_pool.mutex);
	for (int i = 0; i < MAX_EVENTS; i++) {
		if (clients[i].fd == fd) {
			pr_info("free fd [%d] with data index:%d, <ptr:%p>", fd,
				i, clients[i].data);

			SAFE_FREE(clients[i].data);

			clients[i].fd = -1;
			memset(&clients[i].addr, 0, sizeof(clients[i].addr));

			// 将最后一个客户端移动到当前位置，覆盖要移除的客户端
			if (i < client_pool.count - 1) {
				clients[i] = clients[client_pool.count - 1];

				/** 删除下一个，重点 */
				init_client(&clients[client_pool.count - 1]);
				clients[client_pool.count - 1].fd = -1;
				memset(&clients[client_pool.count - 1].addr, 0, sizeof(clients[0].addr));
				clients[client_pool.count - 1].data = NULL;
			}

			client_pool.count--;
			pr_info("after remove, client_pool.count:%d", client_pool.count);
			break;
		}
	}
	pthread_mutex_unlock(&client_pool.mutex);
}

/**
 * @brief 从 epoll 中删除套接字监控，并关闭套接字
 *
 * @param [in] fd
 */
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
		pr_stderr("fcntl (F_GETFL) failed");
		return -errno;
	}

	/** 添加非阻塞标志 */
	if (fcntl(sockfd, F_SETFL, flags | O_NONBLOCK) == -1) {
		pr_stderr("fcntl (F_SETFL) failed");
		return -errno;
	}

	return 0;
}

/**
 * @brief 初始化套接字
 *
 * @param [in] port
 * @return int 0: success; <0: failed, posix errno;
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
 * @brief 初始化 epoll
 *
 * @param [in] port
 * @return int 0: success; <0: failed, posix errno;
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
				epoll_del_fd(events[i].data.fd);
				remove_client(events[i].data.fd);
				continue;
			}

			if (0 == (events[i].events & EPOLLIN)) {
				pr_warn("recv unknown event 0x%03x",
					events[i].events);
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
				// pr_info("fd [%d] has event [0x%03x]",
				// 	event.data.fd, event.events);

				epoll_recv(efd, events[i].data.fd);
			}
		}
	}

	return 0;
}

/**
 * @brief 接受 RPC 头部
 *
 * @param [in] client
 * @return int 0: success; <0: failed, posix errno;
 */
int recv_head(client_t *client)
{
	int rc = 0;
	int fd = 0;
	uint32_t data_len = 0;

	if (NULL == client) {
		pr_info("client is invalide (NULL)");
		return -EINVAL;
	}

	/** Head buff is OK already */
	if (client->head_total_size == client->head_recv_size) {
		return 0;
	}

	fd = client->fd;

recv_head:
	rc = read(fd, client->head + client->head_recv_size,
		  (size_t)(client->head_total_size - client->head_recv_size));
	if (rc < 0) {
		if (errno == EAGAIN ||
		    errno == EWOULDBLOCK ||
		    errno == EINTR) {
			pr_info("continue read head ...");
		} else {
			/** 例如：EBADF */
			pr_stderr("read [%d] with error", fd);
			epoll_del_fd(fd);
			remove_client(fd);
		}

		/**
		 * 接收失败
		 * 若进入 EAGAIN 分支，后有读取事件, epoll 会再次触发读取
		 */
		return -errno;
	}

	/** 接受到 0, 表示客户端主动断开，退出 */
	if (rc == 0) {
		pr_info("client [%d] trigger disconnect", fd);
		epoll_del_fd(fd);
		remove_client(fd);
		return -EPERM;
	}

	pr_info("head recv: fd [%d] [%d/%d]", fd, rc,
		client->head_total_size - client->head_recv_size);

	client->head_recv_size += rc;

	if (client->head_recv_size == client->head_total_size) {
		client->head_ready = 1;

		memcpy(&data_len, client->head + 4, 4);
		data_len = ntohl(data_len);

		pr_buff(client->head, client->head_recv_size);
		pr_info("body size:%d, fd [%d]", data_len, fd);

		if (data_len > 0xffffffff) {
			pr_err("recv msg too long (%d > MAX:%d)", data_len,
				0xffffffff);
			epoll_del_fd(fd);
			remove_client(fd);
			return -EINVAL;
		}

		pthread_mutex_lock(&client_pool.mutex);
		if (NULL == client->data) {
			client->data = calloc(1, data_len);
			if (NULL == client->data) {
				pthread_mutex_unlock(&client_pool.mutex);
				pr_stderr("calloc failed");
				epoll_del_fd(fd);
				remove_client(fd);
				return -ENOMEM;
			}

			pr_info("calloc fd [%d] with data <ptr:%p>", client->fd,
				client->data);
		} else {
			pr_info("last calloc fd [%d] with data <ptr:%p>", client->fd,
				client->data);
		}
		pthread_mutex_unlock(&client_pool.mutex);

		client->body_total_size = data_len;
		return 0;
	} else {
		pr_info("Head buff recv no finished");
		goto recv_head;
	}
}

/**
 * @brief 接受 RPC 数据
 *
 * @param [in] client
 * @return int 0: success; <0: failed, posix errno;
 */
int recv_data(client_t *client)
{
	int rc = 0;
	int fd = 0;
	int len;

	if (NULL == client) {
		pr_info("client is invalide (NULL)");
		return -EINVAL;
	}

	/** Data buff is OK already */
	if (client->body_total_size == client->body_recv_size) {
		return 0;
	}

	fd = client->fd;

	while (1) {
		len = client->body_total_size - client->body_recv_size;

		rc = read(client->fd, client->data + client->body_recv_size,
			  len);
		if (rc < 0) {
			if (errno == EAGAIN ||
			    errno == EWOULDBLOCK ||
			    errno == EINTR) {
				pr_info("have no data, return and epoll wait ... again");
			} else {
				pr_stderr("read [%d] with error", fd);
				epoll_del_fd(fd);
				remove_client(fd);
			}

			/**
			 * 接收失败
			 * 若进入 EAGAIN 分支，后有读取事件, epoll 会再次触发读取
			 */
			return -errno;
		}

		/** 接受到 0, 表示客户端主动断开，退出 */
		if (rc == 0) {
			pr_info("client trigger disconnect");
			epoll_del_fd(fd);
			remove_client(fd);
			return -EPERM;
		}

		// pr_info("data recv: [curr:%d, remain:%d, total:%d]", rc,
		// 	client->body_total_size - client->body_recv_size,
		// 	client->body_total_size);

		client->body_recv_size += rc;

		if (client->body_recv_size == client->body_total_size) {
			/** 数据准备好 */
			client->body_ready = 1;
			// pr_info("data ready");
			break;
		} else if (client->body_recv_size < client->body_total_size) {
			// pr_info("data recv no finished");
			continue;
		} else {
			pr_err("data len is too long (recv:%d > total:%d)",
				client->body_recv_size,
				client->body_total_size);
			epoll_del_fd(fd);
			remove_client(fd);
			return -EINVAL;
		}
	}

	return 0;
}

void epoll_recv(int efd, int client_fd)
{
	int rc = 0;
	client_t *client = NULL;

	client = find_client(client_fd);
	if (NULL == client) {
		pr_err("can't find client with fd [%d]", client_fd);
		return;
	}

	if (client->fd != client_fd) {
		pr_err("client->fd:%d != fd:%d", client->fd, client_fd);
		abort();
	}

	/** 读取协议头 */
	if (0 == client->head_ready) {
		rc = recv_head(client);
		if (0 != rc) {
			return;
		}
	}

	/** 读取数据体 */
	if (0 == client->body_ready) {
		rc = recv_data(client);
		if (0 != rc) {
			return;
		}
	}

	if (client->head_ready && client->body_ready) {
		// pr_info("data ready, switch_cmd");
		switch_cmd(client);
	}
}

static void switch_cmd(client_t *client)
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
	sprintf(msg, "server say: random - %ld", random());
	server_stream_call_pack_and_send(client->fd, msg, strlen(msg) + 1);

	pthread_mutex_lock(&client_pool.mutex);
	init_client(client);
	pthread_mutex_unlock(&client_pool.mutex);

	/** 处理完请求后，不主动断开，等待客户端主动断开 */
}

int main_server(void)
{
	int rc = 0;

	pthread_mutex_init(&client_pool.mutex, NULL);
	client_pool.count = 0;

	rc = init_server_epoll(RPC_PORT);

	pthread_mutex_destroy(&client_pool.mutex);
	return rc;
}
