#include <stdlib.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/ip.h>
#include <netinet/ip_icmp.h>
#include <time.h>
#include <errno.h>
#include <sys/time.h>

#include "../../common/utils.h"

int main(int argc, char *argv[])
{
	if (argc != 2) {
		fprintf(stderr, "Usage: %s <destination>\n", argv[0]);
		return -EINVAL;
	}


	pr_warn("%s is not alive", argv[1]);


	return 0;
}