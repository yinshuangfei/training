#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#include "multicast.h"

#define BUFFER_SIZE 1024

int main()
{
	int sockfd;
	struct sockaddr_in addr;
	struct ip_mreq mreq;
	char buffer[BUFFER_SIZE];

	// 创建UDP套接字
	if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
		pr_stderr("socket creation failed");
		exit(EXIT_FAILURE);
	}

	// 设置地址结构
	memset(&addr, 0, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = htonl(INADDR_ANY);
	addr.sin_port = htons(PORT);

	// 绑定套接字
	if (bind(sockfd, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
		pr_stderr("bind failed");
		close(sockfd);
		exit(EXIT_FAILURE);
	}

	// 加入组播组
	mreq.imr_multiaddr.s_addr = inet_addr(MULTICAST_GROUP);
	mreq.imr_interface.s_addr = htonl(INADDR_ANY);

	if (setsockopt(sockfd, IPPROTO_IP, IP_ADD_MEMBERSHIP, &mreq, sizeof(mreq)) < 0) {
		pr_stderr("setsockopt IP_ADD_MEMBERSHIP failed");
		close(sockfd);
		exit(EXIT_FAILURE);
	}

	pr_info("Waiting for multicast messages...");

	while (1) {
		socklen_t addrlen = sizeof(addr);
		ssize_t n = recvfrom(sockfd, buffer, BUFFER_SIZE, 0,
				     (struct sockaddr *)&addr, &addrlen);
		if (n < 0) {
			pr_stderr("recvfrom failed");
			break;
		}

		buffer[n] = '\0';
		pr_info("Received: %s", buffer);
	}

	// 离开组播组
	setsockopt(sockfd, IPPROTO_IP, IP_DROP_MEMBERSHIP, &mreq, sizeof(mreq));
	close(sockfd);

	return 0;
}
