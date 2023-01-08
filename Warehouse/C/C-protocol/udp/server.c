#include <stdio.h>
#include <string.h>
#include <unistd.h>     /* for close() */
#include <sys/types.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "common.h"
#include "test_server.h"


int main(int argc, char *argv[]) {
    print_help();

    while (1) {
        char cmdbuff[1024];

        printf("server cmd> ");
        // fgets(cmdbuff, 1024, stdin);
        sprintf(cmdbuff, "3\n");

        cmdbuff[strlen(cmdbuff) - 1] = '\0';
        if(strcmp(cmdbuff, "") == 0)  continue;
        if(strcmp(cmdbuff, "quit") == 0)  exit(0);

        int cmd = atoi(cmdbuff);
        switch(cmd) {
        case 1:
            server_mode_1();
            break;
        case 2:
            server_mode_2();
            break;
        case 3:
            server_mode_3();
            break;
        default:
            print_help();
            continue;
        }
    }
    return 0;
}