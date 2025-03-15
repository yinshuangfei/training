#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/ip.h>
#include <netinet/ip_icmp.h>
#include <time.h>
#include <errno.h>
#include <sys/time.h>
#include <netdb.h>		/** getprotobyname */

#include "../../common/utils.h"

/***
 * 127.0.0.1: 不检测 icmp_cksum
 * 126.0.0.1: 检测 icmp_cksum
 * 140.82.113.3: github 检测 icmp_cksum
*/

#define DATASIZE	64
#define PKTSIZE		(sizeof(struct icmp) + DATASIZE)

int checksum(void *b, int len)
{
	unsigned short *buf = b;
	unsigned int sum = 0;

	for (sum = 0; len > 1; len -= 2)
		sum += *buf++;

	if (len == 1) {
		sum += *(unsigned char*)buf;
	}

	sum = (sum >> 16) + (sum & 0xffff);
	sum += (sum >> 16);
	sum = ~sum;

	return sum;
}

void create_packet(struct icmp *icmp_pkt, struct timeval *tv)
{
	struct timeval *icpm_tv = NULL;

	icmp_pkt->icmp_type = ICMP_ECHO;
	icmp_pkt->icmp_code = 0;
	icmp_pkt->icmp_id = getpid();
	icmp_pkt->icmp_seq = 0;
	icmp_pkt->icmp_cksum = 0;

	/** 数据段存放发送时间 */
	icpm_tv = (struct timeval *)icmp_pkt->icmp_data;
	icpm_tv->tv_sec = tv->tv_sec;
	icpm_tv->tv_usec = tv->tv_usec;

	icmp_pkt->icmp_cksum = checksum((unsigned short*)icmp_pkt, PKTSIZE);

	pr_dbg("send icmp, id:%d, sec:%ld, usec:%ld",
		icmp_pkt->icmp_id,
		icpm_tv->tv_sec,
		icpm_tv->tv_usec);
}

int send_packet(int sockfd, struct sockaddr_in *dest_addr, struct timeval *tv)
{
	int rc = 0;
	ssize_t size = 0;
	char send_buf[PKTSIZE] = {0};

	create_packet((struct icmp *)send_buf, tv);

	size = sendto(sockfd, send_buf, PKTSIZE, 0, (struct sockaddr*)dest_addr,
		      sizeof(*dest_addr));
	if (size < 0) {
		pr_err("sendto error (%d:%s)", errno, strerror(errno));
		return -errno;
	}

	return 0;
}

int receive_packet(int sockfd, struct sockaddr_in *dest_addr,
		   struct timeval *tv)
{
	int rc = 0;
	char recv_buf[sizeof(struct ip) + PKTSIZE];
	socklen_t addr_len = sizeof(struct sockaddr_in);
	ssize_t size = 0;
	struct ip *ip_hdr = NULL;
	struct icmp *icmp_hdr = NULL;
	struct timeval *icpm_tv = NULL;
	struct timeval recv_tv, diff_tv;

	size = recvfrom(sockfd, recv_buf, sizeof(recv_buf), 0,
			(struct sockaddr*)dest_addr, &addr_len);
	if (size < 0) {
		pr_err("recvfrom error (%d:%s)", errno, strerror(errno));
		return -errno;
	}

	ip_hdr = (struct ip *)recv_buf;
	icmp_hdr = (struct icmp*)(recv_buf + (ip_hdr->ip_hl * 4));
	icpm_tv = (struct timeval *)icmp_hdr->icmp_data;

	pr_dbg("recv icmp, id:%d, sec:%ld, usec:%ld",
		icmp_hdr->icmp_id,
		icpm_tv->tv_sec,
		icpm_tv->tv_usec);

	if ((icmp_hdr->icmp_type == ICMP_ECHO ||
	     icmp_hdr->icmp_type == ICMP_ECHOREPLY) &&
	    icmp_hdr->icmp_id == getpid() &&
	    tv->tv_sec == icpm_tv->tv_sec &&
	    tv->tv_usec == icpm_tv->tv_usec) {
		/** 计算时间差 */
		rc = gettimeofday(&recv_tv, NULL);
		if (0 != rc) {
			pr_err("gettimeofday error (%d:%s)", errno,
				strerror(errno));
			return -errno;
		}

		timersub(&recv_tv, icpm_tv, &diff_tv);

		pr_info("Received reply from %s: type:%d, ttl:%d, time=%.3fms",
			inet_ntoa(dest_addr->sin_addr),
			icmp_hdr->icmp_type,
			ip_hdr->ip_ttl,
			diff_tv.tv_sec * 1000 + (float)diff_tv.tv_usec / 1000);
	} else {
		pr_err("Received unexpected packet from %s, icmp_type:%d, icmp_id:%d, sec:%ld, usec:%ld",
			inet_ntoa(dest_addr->sin_addr),
			icmp_hdr->icmp_type,
			icmp_hdr->icmp_id,
			icpm_tv->tv_sec,
			icpm_tv->tv_usec);
	}

	return rc;
}

int ping(char *ipaddr)
{
	int rc = 0;
	int sockfd;
	struct sockaddr_in dest_addr;
	struct timeval tv;
	struct timeval tv_timeout;

	/**
	 * 使用 getprotobyname 的方法如下：
	struct protoent *protocol = getprotobyname("icmp");
	if (protocol == NULL) {
		pr_err("getprotobyname error (%d:%s)", errno, strerror(errno));
		return -errno;
	}
	sockfd = socket(AF_INET, SOCK_RAW, protocol->p_proto);
	*/
	sockfd = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
	if (sockfd < 0) {
		pr_err("socket error (%d:%s)", errno, strerror(errno));
		return -errno;
	}

	/** 设置超时时间为1秒 */
	tv_timeout.tv_sec = 3;
	tv_timeout.tv_usec = 0;
	rc = setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO,
			&tv_timeout, sizeof(tv_timeout));
	if (0 != rc) {
		pr_err("setsockopt error (%d:%s)", errno, strerror(errno));
		return -errno;;
	}

	memset(&dest_addr, 0, sizeof(dest_addr));
	dest_addr.sin_family = AF_INET;

	/** 返回值大于 0 */
	rc = inet_pton(AF_INET, ipaddr, &dest_addr.sin_addr);
	if (rc <= 0) {
		pr_err("Invalid address/ Address not supported");
		return -errno;
	}

	rc = gettimeofday(&tv, NULL);
	if (0 != rc) {
		pr_err("gettimeofday error (%d:%s)", errno, strerror(errno));
		return -errno;;
	}

	rc = send_packet(sockfd, &dest_addr, &tv);
	if (0 != rc)
		goto out;

	rc = receive_packet(sockfd, &dest_addr, &tv);
out:
	close(sockfd);
	return rc;
}

int main(int argc, char *argv[])
{
	if (argc != 2) {
		fprintf(stderr, "Usage: %s <destination>\n", argv[0]);
		return -EINVAL;
	}

	if (0 == ping(argv[1])) {
		pr_info("%s is alive", argv[1]);
	} else {
		pr_warn("%s is not alive", argv[1]);
	}

	return 0;
}