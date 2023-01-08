#ifndef _SERIAL_CMD_
#define _SERIAL_CMD_

#include "comm.h"
#include <pthread.h>

#define HEARTREAT_CMD_LEN           7
#define GETSLOT_CMD_LEN             7
#define SEND_VERSION_CMD_LEN        10

struct slot_info {
    int slot;
    int dev_type;
    int vehicle_type
} ;

void * 
heart_beat(void *data);

int 
is_checksum_OK(char * buf, int len);

int
parse_cmd (char * buf, int len);

int
response (char * buf, int len, int fd);

int
send_version_cmd (int fd, int year, int month, int day, int xx, int yyzz);

int 
send_get_slot_cmd(int fd, struct slot_info * info);

int
init_tty (int fd, int Baud, int nBits, char nEvent, int nStop);

#endif /* _SERIAL_CMD_ */