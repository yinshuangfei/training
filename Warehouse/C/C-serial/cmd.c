
#include "cmd.h"

#define DOMAIN "CMD"


void * 
heart_beat(void *data) {
    pthread_detach(pthread_self());
    
    int ret;
    int fd = *(int *)data;
    char cmd[HEARTREAT_CMD_LEN];

    cmd[0] = 0xaa;
    cmd[1] = 0x55;
    cmd[2] = 0x3c;  // src
    cmd[3] = 0x44;  // des
    cmd[4] = 0x01;  // len
    cmd[5] = 0x01;  // cmd
    cmd[6] = 0x81;
    
    while (1) {
        usleep(1000000);
        DEBUG_LOG(is_dbg, DOMAIN, "send heart beat.\n");

#if defined(__DEBUG__)
        for (int i = 0 ; i < HEARTREAT_CMD_LEN; i++) 
            printf("0x%02x ", cmd[i] & 0xff);

        printf("\n");
#endif

        ret = write(fd, cmd, HEARTREAT_CMD_LEN);
        if (0 >= ret) {
            daemon_log(LOG_ERR, "write error (%d), %s\n", ret, strerror(errno));
            sleep(5);
        }
    }
}

/**
  ok, return 1, otherwise return -1;
*/
int 
is_checksum_OK(char * buf, int len) {
    unsigned int check_sum = 0;
    if (len < 7) {
        daemon_log(LOG_ERR, "check sum error, len (%d) less then 7.\n", len);
        return -1;
    }

    for (int i = 0 ; i < len - 1; i++) {
        check_sum += ( buf[i] & 0xff );
    }
    
    if (buf[len - 1] != check_sum) {
        daemon_log(LOG_ERR, "check sum error, recv:%d, calculate:%d.\n", buf[len - 1], check_sum);
        return -1;
    }

    return 1;
}

int
parse_cmd (char * buf, int len) {
    return buf[5] & 0xff;
}

int
response (char * buf, int len, int fd) {
    int ret = -1;
    int cmd;

    if (is_checksum_OK(buf, len) != 1) {
        return ret;
    }

    cmd = parse_cmd(buf, len);

    switch (cmd) {
        case 0x01:
            break;
        case 0x02:
            break;
        case 0x03:
            ret = send_version_cmd(fd, 2022, 04, 12, 1, 13);
            break;
        default:
            break;
    }

    return ret;
}

int
send_version_cmd (int fd, int year, int month, int day, int xx, int yyzz) {
    int ret;
    char cmd[SEND_VERSION_CMD_LEN], buff[256];
    unsigned int check_sum = 0;

    cmd[0] = 0xaa;
    cmd[1] = 0x55;
    cmd[2] = 0x3c;  // src
    cmd[3] = 0x44;  // des
    cmd[4] = 0x04;  // len
    cmd[5] = 0x03;  // cmd

    // int year = 2022, month = 4, day = 22;
    if (year < 2000)
        year = 2022;
    if (month < 1)
        month = 1;
    if (day < 1)
        day = 1;

    unsigned int reserve_bit = 0x01 & 0x00;                      //                0001 = 0x1
    unsigned int year_bit = 0x7e & ((year - 2000) << 1);         // 0000,0000,0111,1110 = 0x7e

    unsigned int month_bit = 0x0780 & (month << 7);              // 0000,0111,1000,0000 = 0x0780
    unsigned int day_bit = 0xf800 & (day << 11);                 // 1111,1000,0000,0000 = 0xf800

    unsigned int date = reserve_bit & year_bit & month_bit & day_bit;

    cmd[6] = date & 0xff;
    cmd[7] = (date & 0xff00) >> 8;

    /* 版本号格式为 Vx.yy.zz, 
        x 为第一字段, version_low_bit
        yy 为第二字段, version_high_bit / 10
        zz 为第三字段, version_high_bit % 10 
    */
    int low = xx;
    int mid = 0;
    int high = yyzz;        
    unsigned char version_low_bit = 0x03 & low;                  //            0000,0011 = 0x03, x
    unsigned char version_mid_reserve_bit = 0xfc & mid;          //            1111,1100 = 0xfc, 保留值，为0；
    unsigned char version_high_bit = 0xff & high;             //            1111,1111 = 0xff,

    cmd[8] = version_low_bit & version_mid_reserve_bit;
    cmd[9] = version_high_bit;

    for (int i = 0 ; i < SEND_VERSION_CMD_LEN - 1; i++) {
        check_sum += ( cmd[i] & 0xff );
    }
    cmd[SEND_VERSION_CMD_LEN - 1] = check_sum;

    ret = write(fd, cmd, SEND_VERSION_CMD_LEN);
    if (0 >= ret) {
        daemon_log(LOG_ERR, "write cmd error (%d), %s\n", ret, strerror(errno));
        return -1;
    }

    return 0;
}

int 
send_get_slot_cmd(int fd, struct slot_info * info) {
    int ret;
    char cmd[GETSLOT_CMD_LEN], buff[256];
    unsigned int check_sum = 0;

    cmd[0] = 0xaa;
    cmd[1] = 0x55;
    cmd[2] = 0x3c;  // src
    cmd[3] = 0x44;  // des
    cmd[4] = 0x01;  // len
    cmd[5] = 0x02;  // cmd
    cmd[6] = 0x00;

    for (int i = 0 ; i < GETSLOT_CMD_LEN - 1; i++) {
        check_sum += ( cmd[i] & 0xff );
    }
    cmd[GETSLOT_CMD_LEN - 1] = check_sum;

    ret = write(fd, cmd, GETSLOT_CMD_LEN);
    if (0 >= ret) {
        daemon_log(LOG_ERR, "write cmd error (%d), %s\n", ret, strerror(errno));
        return -1;
    }

    ret = read(fd, buff, 256);
    if (7 > ret) {
        if (0 > ret)
            daemon_log(LOG_ERR, "read error (%d), %s\n", ret, strerror(errno));
        else
            daemon_log(LOG_ERR, "recv cmd len error (len:%d).\n", ret);
        return -1;
    }

    if (1 != is_checksum_OK(buff, ret)) {
        return -1;
    }
 
    info->slot = buff[6];
    info->dev_type = buff[7];
    info->vehicle_type = buff[8];

    return 0;
}

int
init_tty (int fd, int Baud, int nBits, char nEvent, int nStop) {
    struct termios options;

    memset(&options, 0, sizeof(options));

    /* get current param */
    if (0 != tcgetattr(fd, &options)) {
        daemon_log(LOG_ERR, "tcgetattr execute failed, %s\n", strerror(errno));
        return -1;
    }

    /* CREAD 开启串行数据接收，CLOCAL并打开本地连接模式 */
    options.c_cflag |= ( CLOCAL | CREAD );
    /* 使用CSIZE做位屏蔽 */
    options.c_cflag &= ~CSIZE;

    /* 设置数据位 */
    switch (nBits) {
        case 5:
            options.c_cflag |= CS5;
            break;
        case 6:
            options.c_cflag |= CS6;
            break;
        case 7:
            options.c_cflag |= CS7;
            break;
        case 8:
            options.c_cflag |= CS8;
            break;
        default:
            options.c_cflag |= CS8;
            break;
    }

    /* 校验位 */
    switch (nBits) {
        case 'O':   /* odd */
            options.c_cflag |= PARENB;
            options.c_cflag |= PARODD;
            options.c_iflag |= ( INPCK | ISTRIP );
            break;
        case 'E':   /* even */
            options.c_cflag |= PARENB;
            options.c_cflag &= ~PARODD;
            options.c_iflag |= ( INPCK | ISTRIP );
            break;
        case 'N':   /* none */
        default :
            options.c_cflag &= ~PARENB;
            break;
    }

    /* 设置波特率 */
    switch (nBits) {
        case 9600:
            cfsetispeed(&options, B9600);
	        cfsetospeed(&options, B9600);
            break;
        case 38400:
            cfsetispeed(&options, B38400);
	        cfsetospeed(&options, B38400);
            break;
        case 115200:
        default:
            cfsetispeed(&options, B115200);
	        cfsetospeed(&options, B115200);
            break;
    }

    /* 设置停止位; */
    switch (nStop) {
        case 2:
            options.c_cflag |= CSTOPB;
            break;
        case 1:
        default:
            options.c_cflag &= ~CSTOPB;
            break;
    }

	/* 非规范模式读取时的超时时间；*/
	options.c_cc[VTIME] = 0;

	/* 非规范模式读取时的最小字符数*/
	options.c_cc[VMIN]  = 0;

    /* tcflush清空终端未完成的输入/输出请求及数据；TCIFLUSH表示清空正收到的数据，且不读取出来 */
    tcflush(fd, TCIFLUSH);

    if (0 != tcsetattr(fd, TCSANOW, &options)) {
        daemon_log(LOG_ERR, "tcsetattr execute failed, %s\n", strerror(errno));
        return -1;
    }

    return 0;
}