#ifndef _TEST_SERVER_H_
#define _TEST_SERVER_H_

#include "protocol.h"

static int init_udp(struct sockaddr_in *server);

int server_mode_1();
static void _do_mode_1(struct sockaddr_in *client, int sockfd);

int server_mode_2();
int server_mode_3();

static int
_do_server_mode_3(rdt_t *rdt, int len);

#endif /* _TEST_SERVER_H_ */