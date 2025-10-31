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
	char buffer[BUFFER_SIZE];

	// 创建UDP套接字
	if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
		pr_stderr("socket creation failed");
		exit(EXIT_FAILURE);
	}

	// 设置组播TTL (Time To Live)
	unsigned char ttl = 1; // 限制在本地网络
	if (setsockopt(sockfd, IPPROTO_IP, IP_MULTICAST_TTL, &ttl, sizeof(ttl)) < 0) {
		pr_stderr("setsockopt IP_MULTICAST_TTL failed");
		close(sockfd);
		exit(EXIT_FAILURE);
	}

	// 设置组播地址结构
	memset(&addr, 0, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = inet_addr(MULTICAST_GROUP);
	addr.sin_port = htons(PORT);

	printf("Enter messages to multicast (Ctrl+C to quit):\n");

	while (1) {
		printf("> ");
		fgets(buffer, BUFFER_SIZE, stdin);

		// 发送组播消息
		if (sendto(sockfd, buffer, strlen(buffer), 0,
		    (struct sockaddr *)&addr, sizeof(addr)) < 0) {
			pr_stderr("sendto failed");
			break;
		}
	}

	close(sockfd);
	return 0;
}
