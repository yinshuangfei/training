#ifndef _TEST_CLIENT_H_
#define _TEST_CLIENT_H_

#include "protocol.h"

char *hostname;

int client_mode_1();
int client_mode_2();
int client_mode_3();

static int
_do_mode3(rdt_t *rdt, int len);

#endif /* _TEST_CLIENT_H_ */