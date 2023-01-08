#include <pthread.h>

#include "comm.h"
#include "cmd.h"

#define DOMAIN "MAIN"

#define BUFF_LEN    128

#define TTY_NAME_0  "/dev/ttyAMA0" 
#define TTY_NAME_1  "/dev/ttyAMA0" 
#define TTY_NAME_2  "/dev/ttyAMA2" 
#define TTY_NAME_3  "/dev/ttyAMA3" 


int
main (int argc, char * argv[]) {
    int ret;
    int tty_fd = -1;
    char r_buf[BUFF_LEN];

    DEBUG_LOG(is_dbg, DOMAIN, "daemon start, argc:(%d)\n", argc);

    // tty_fd = open(TTY_NAME_1, O_RDWR|O_NOCTTY|O_NDELAY);
    tty_fd = open(TTY_NAME_1, O_RDWR);
    if (0 > tty_fd) {
        daemon_log(LOG_ERR, "open tty:%s failed, %s\n", TTY_NAME_1, strerror(errno));
        goto err;
    }

    if (0 != init_tty (tty_fd, 115200, 8, 'N', 1)) {
        goto err;
    }

    /* --slot */
    if (argc > 1 && strcmp(argv[1], "--slot") == 0) {
        struct slot_info info;

        if ((ret = send_get_slot_cmd(tty_fd, &info)) != 0) {
            printf("get slot info error.\n");
            close(tty_fd);
            exit(-1);
        }

        printf("slot:%d \tdev_type:%d \tvehicle_type:%d\n", info.slot, info.dev_type, info.vehicle_type);
        close(tty_fd);
        exit(0);
    }

    init_daemon();

    pthread_t th_heartbeat_id;
    if (pthread_create(&th_heartbeat_id, NULL, heart_beat, &tty_fd) != 0) {
        daemon_log(LOG_ERR, "create process thread:'%s' failed.\n", "heart_beat");
        goto err;
    }

    /* 主线程接收请求 */
    while (1) {
        sleep(1);
        int cmd = -1;

        memset(r_buf, 0, sizeof(r_buf));

        ret = read(tty_fd, r_buf, sizeof(r_buf));
        if (0 > ret) {
            daemon_log(LOG_ERR, "read error (%d), %s\n", ret, strerror(errno));
            continue;
        }

        if ( 0 != response(r_buf, ret, tty_fd)) {
            continue;
        }
    }

    DEBUG_LOG(is_dbg, DOMAIN, "successful end.\n");
    close(tty_fd);
    return -1;

err:
    close(tty_fd);    
    return 0;
}


