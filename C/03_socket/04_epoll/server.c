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
#include <stdatomic.h>
#include <pthread.h>
#include <time.h>

#include "../common.h"
#include "server.h"
#include "../../common/utils.h"
#include "../../common/debug.h"

#define MAX_EVENTS		1024

extern int SIGNAL_TERMINATE;

/** 客户连接 */
struct _client_t {
	int fd;
	struct sockaddr_in addr;
	int recv_len;
	char data[MAX_DATASIZE];
};

/** 客户连接池 */
struct _client_pool_t {
	int count;
	pthread_mutex_t mutex;
	client_t clients[MAX_EVENTS];
};

client_pool_t client_pool;
int efd = 0;

/**
 * @brief 初始化用户池用户
 *
 * @param [in out] client
 */
void init_client(client_t *client)
{
	client->recv_len = 0;
	memset(client->data, 0, sizeof(client->data));
}

/**
 * @brief 根据 fd 查找 client
 *
 * @param [in] fd
 * @return client_t* client: success, NULL: failed;
 */
static client_t *find_client(int fd)
{
	pthread_mutex_lock(&client_pool.mutex);
	for (int i = 0; i < MAX_EVENTS; i++) {
		if (client_pool.clients[i].fd == fd) {
			pthread_mutex_unlock(&client_pool.mutex);
			return &client_pool.clients[i];
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
 * @return int 0: success, <0: failed, posix errno; >0: other success, value;
 */
static int add_client(int fd, struct sockaddr_in addr)
{
	pthread_mutex_lock(&client_pool.mutex);
	if (client_pool.count >= MAX_EVENTS) {
		pthread_mutex_unlock(&client_pool.mutex);
		pr_err("Max clients (%d) reached", MAX_EVENTS);
		return -EPERM;
	}

	client_pool.clients[client_pool.count].fd = fd;
	client_pool.clients[client_pool.count].addr = addr;
	init_client(&client_pool.clients[client_pool.count]);
	client_pool.count++;

	pthread_mutex_unlock(&client_pool.mutex);

	pr_dbg("client_pool count: %d", client_pool.count);

	return 0;
}

/**
 * @brief 移除客户端
 *
 * @param [in out] fd
 */
static void remove_client(int fd)
{
	pthread_mutex_lock(&client_pool.mutex);
	for (int i = 0; i < MAX_EVENTS; i++) {
		if (client_pool.clients[i].fd == fd) {
			/** 将最后一个客户端移动到当前位置，覆盖要移除的客户端 */
			if (i < client_pool.count - 1) {
				client_pool.clients[i] =
				    client_pool.clients[client_pool.count - 1];
			}
			client_pool.count--;
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
static void epoll_del_fd(int fd)
{
	int rc = 0;
	struct epoll_event event = {0};

	event.data.fd = fd;
	rc = epoll_ctl(efd, EPOLL_CTL_DEL, fd, &event);
	if (rc < 0) {
		pr_stderr("del event failed, fd [%d]", fd);
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
static int set_socket_nonblocking(int sockfd)
{
	int flags;

	/** 获取当前文件状态标志 */
	flags = fcntl(sockfd, F_GETFL, 0);
	if (-1 == flags) {
		pr_stderr("fcntl (F_GETFL) failed");
		return -errno;
	}

	/** 添加非阻塞标志 */
	if (-1 == fcntl(sockfd, F_SETFL, flags | O_NONBLOCK)) {
		pr_stderr("fcntl (F_SETFL) failed");
		return -errno;
	}

	return 0;
}

/**
 * @brief 初始化套接字
 *
 * @param [in] port
 * @return int 0: success; <0: failed, posix errno; >0: other success, value;
 */
static int init_sock(int port)
{
	int rc = 0;
	int opt = 1;
	int listen_fd = 0;
	struct sockaddr_in addr = {0};

	listen_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (listen_fd == -1) {
		pr_stderr("create socket error");
		return -errno;
	}

	rc = setsockopt(listen_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT,
			&opt, sizeof(opt));
	if (rc) {
		pr_stderr("setsockopt error");
		return -errno;
	}

	memset(&addr, 0, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = INADDR_ANY;
	addr.sin_port = htons(port);

	rc = bind(listen_fd, (struct sockaddr *)&addr, sizeof(addr));
	if (rc == -1) {
		pr_stderr("bind socket error");
		return -errno;
	}

	rc = listen(listen_fd, SOMAXCONN);
	if (rc == -1) {
		pr_stderr("listen socket error");
		return -errno;
	}

	pr_info("server listen on fd [%d]", listen_fd);

	return listen_fd;
}

/**
 * @brief 发送定长数据
 *
 * @param [in] fd
 * @param [in] buf
 * @param [in] len
 * @return ssize_t 0: success; <0: failed, posix errno;
 */
ssize_t serv_fix_write(int fd, void *buf, size_t len)
{
	int rc = 0;
	size_t iolen = 0;
	int max_retry = 30;
	int cnt = 1;

	while (iolen < len) {
		rc = write(fd, buf + iolen, (size_t)(len - iolen));
		if (rc < 0) {
			if (errno == EAGAIN ||
			    errno == EWOULDBLOCK ||
			    errno == EINTR) {

				/** 客户端没有把数据取走 */
				if (cnt <= max_retry) {
					pr_info("buff no space left, retry [%d/%d]",
						cnt, max_retry);
					cnt++;
					/** 等待 10 ms */
					usleep(10000);
					continue;
				} else {
					pr_err("buff no space left, reached max retry [%d]",
						max_retry);
					return -errno;
				}

			} else {
				/** 这里表示客户端异常断开，退出 */
				pr_stderr("write [%d] with error", fd);
				return -errno;
			}
		}
		iolen += rc;
	}
	return 0;
}

void do_server_demo(client_t *client)
{
	time_t raw_time;
	struct tm *time_info;
	char time_str[80];
	int rc = 0;
	char msg[MAX_DATASIZE] = {0};

	pr_info("client [%d] say: <%s>", client->fd, client->data);

	/** 1. 获取当前时间（秒数，UTC 1970-01-01 00:00:00 起） */
	time(&raw_time);
	/** 2. 转换为本地时间结构体 */
	time_info = localtime(&raw_time);
	/** 3. 格式化为字符串 */
	strftime(time_str, sizeof(time_str), "%Y-%m-%d %H:%M:%S", time_info);

	snprintf(msg, sizeof(msg), "server say, time is:%s\n", time_str);

	rc = serv_fix_write(client->fd, msg, sizeof(msg));
	if (0 != rc) {
		/** 发送失败 */
		epoll_del_fd(client->fd);
		remove_client(client->fd);
		return;
	}

	init_client(client);
	/** 处理完请求后，不主动断开，等待客户端主动断开 */
}

/**
 * @brief
 *
 * @param [in] efd
 * @param [in] client_fd
 */
static void epoll_recv(int efd, int client_fd)
{
	int rc = 0;
	client_t *client = NULL;
	char buff[MAX_DATASIZE] = {0};

	client = find_client(client_fd);
	if (NULL == client) {
		pr_err("can't find client with fd [%d]", client_fd);
		return;
	}

	while (1) {
		/**
		 * 这里为了能无限接收，使用局部变量 buff
		 * 为提升性能，可在控制好传入 in_buff 长度的情况下，
		 * 直接复制到 in_buff 内 */
		rc = read(client->fd, buff, sizeof(buff));
		if (rc < 0) {
			if (errno == EAGAIN ||
			    errno == EWOULDBLOCK ||
			    errno == EINTR) {
				pr_info("have no data, this recv loop end, deal it");
				break;
			} else {
				/** 这里表示客户端异常断开，退出 */
				pr_stderr("read [%d] with error", client->fd);
				epoll_del_fd(client_fd);
				remove_client(client_fd);
				return;
			}
		}

		if (rc == 0) {
			/** 接受到 0, 表示客户端主动断开，退出 */
			pr_info("client trigger disconnect");
			epoll_del_fd(client_fd);
			remove_client(client_fd);
			return;
		}

		/** 数据长度过长 */
		if (client->recv_len + rc > sizeof(client->data)) {
			pr_err("data len is too long (recv:%d > total:%lu)",
				client->recv_len + rc,
				sizeof(client->data));
			epoll_del_fd(client_fd);
			remove_client(client_fd);
			return;
		}

		pr_info("data recv: [curr:%d, prev received:%d]", rc,
			client->recv_len);

		memcpy(client->data + client->recv_len, buff, rc);
		client->recv_len += rc;
	}

	pr_info("data ready, len:%d, do_server_demo", client->recv_len);

	do_server_demo(client);
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
		pr_stderr("create epoll error");
		close(sock_fd);
		return -errno;
	}

	event.events = EPOLLIN;
	event.data.fd = sock_fd;

	/** 添加对监听套接字的监控 */
	rc = epoll_ctl(efd, EPOLL_CTL_ADD, sock_fd, &event);
	if (rc < 0) {
		pr_stderr("epoll add error");
		close(sock_fd);
		close(efd);
		return -errno;
	}

	while (SIGNAL_TERMINATE == 0) {
		/** 程序收到中断信号， epoll_wait 会退出 */
		ready = epoll_wait(efd, events, MAX_EVENTS, -1);

		if (ready == -1) {
			/** 被信号中断，不打印错误 */
			if (errno != EINTR) {
				pr_stderr("epoll_wait failed");
			}
			continue;
		}

		for (int i = 0; i < ready; i++) {
			if (events[i].events & EPOLLERR ||
			    events[i].events & EPOLLHUP) {
				/**
				 * 这里不清除监控 fd，等到 read == 0 的时候处理,
				 * 避免重复操作
				 */
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

			/** 监听套接字 */
			if (events[i].data.fd == sock_fd) {
				new_sock = accept(sock_fd,
					(struct sockaddr *)&remote_addr,
					&addrlen);
				if (new_sock < 0) {
					pr_stderr("accept failed");
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
					pr_stderr("add event failed, fd [%d]",
						  new_sock);
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

				epoll_recv(efd, events[i].data.fd);
			}
		}
	}

	return 0;
}

/**
 * @brief 循环
 *
 * @return int 0: success, <0: failed, posix errno; >0: other success, value;
 */
int server_loop(void)
{
	int rc = 0;

	/** 初始化用户池 */
	client_pool.count = 0;
	pthread_mutex_init(&client_pool.mutex, NULL);

	rc = init_server_epoll(SERVER_PORT);
	if (rc)
		return rc;

	/** 销毁用户池 */
	pthread_mutex_destroy(&client_pool.mutex);

	return 0;
}
