#ifndef _UDP_TEST_COMMON_H_
#define _UDP_TEST_COMMON_H_

#define PORT 1236               /* Port that will be opened */

// All mode tansfer size in MB
#define MODE1_TRANSFER_SIZE    100
#define MODE2_TRANSFER_SIZE    1024 * 5
#define MODE3_TRANSFER_SIZE    1

/*** MODE 1 ***/
/* Max number of bytes of data 
   max = 2^16-1-8-20=65507
   optimize = 1472
*/
#define PROTOCOL_MTU        4 * 1024
#define PROTOCOL_MTU_S      PROTOCOL_MTU
#define PROTOCOL_MTU_C      PROTOCOL_MTU

// #define SIZE_MAX            (size_t)17446744073709551615

/*** MODE 2 ***/
#define PROTOCOL_MTU_ENET   512 * 1024
#define MAX_CLIENTS         32    /* Enet MAX_CLIENTS */

/*** MODE 3 ***/
#define MODE3_MAX_USER_BUFF_LEN  1024 * 64

static struct timeval t_start, t_end;
static size_t percentage_pre;
static int print_flag;

static void print_help() {
    printf("===== Test mode =====\n");
    printf("1. auto recv from server (no ack, not stable)\n");
    printf("2. auto recv from server (enet, stable protocol)\n");
    printf("3. auto recv from server (kcp, stable protocol)\n");
    // printf("    please select   \n");
}

/* calculate speed */
static void calculate_speed(struct timeval start, struct timeval end, size_t total) {
    long timeuse = 1000000 * ( end.tv_sec - start.tv_sec ) + end.tv_usec - start.tv_usec;
    printf("***************** Summary *****************\n");
    printf("time spend in microsecond (%lu us)\n", timeuse);

    float speed = total * 1000 * 1000 / timeuse;
    printf("transfer speed %.2f B/s, ", speed);
    
    /* human speed */
    if (speed < 1024)
        printf("human speed %.2f B/s\n", speed);
    else if ((speed = speed / 1024) < 1024)
        printf("human speed %.2f KB/s\n", speed);
    else if ((speed = speed / 1024) < 1024)
        printf("human speed %.2f MB/s\n", speed);
    else if ((speed = speed / 1024) < 1024)
        printf("human speed %.2f GB/s\n", speed);
    else 
        printf("human speed %.2f TB/s\n", speed / 1024);

    printf("***************** Test End ****************\n\n");
}

static char * transfer_size(size_t size, char *buf) {
    /* human speed */
    float _size;
    if (size < 1024 )
        sprintf(buf, "%lu B", size);
    else if ((_size = (float)size / 1024) < 1024)
        sprintf(buf, "%.2f KB", _size);
    else if ((_size = (float)_size / 1024) < 1024)
        sprintf(buf, "%.2f MB", _size);
    else if ((_size = (float)_size / 1024) < 1024)
        sprintf(buf, "%.2f GB", _size);
    else {
        sprintf(buf, "%.2f TB",(float) _size / 1024);
    }
}

/* print percentage */
static void print_percentage(size_t len, size_t total) {
    if (len > total)
        return ;

    size_t pect = (total - len) * 100 / total;
    if (percentage_pre != pect)
        print_flag = 1;
    
    if (print_flag) {
        printf("current percentage %3ld%%\n", pect);
        percentage_pre = pect;
        print_flag = 0;
    }
}

#endif /* _UDP_TEST_COMMON_H_ */