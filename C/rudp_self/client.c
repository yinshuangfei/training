#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <strings.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>                      /* netbd.h is needed for struct hostent =) */

#include "common.h"
#include "test_client.h"


int main(int argc, char *argv[])
{
    char *host = "127.0.0.1";
    hostname = host;

    print_help();
    while (1) {
        /* input cmd */
        char cmdbuff[1024];

        printf("client cmd> ");
        // fgets(cmdbuff, 1024, stdin);
        sprintf(cmdbuff, "3\n");

        cmdbuff[strlen(cmdbuff) - 1] = '\0';
        if(strcmp(cmdbuff, "") == 0)  continue;

        int cmd = atoi(cmdbuff);
        switch(cmd) {
        case 1:
            client_mode_1();
            break;
        case 2:
            client_mode_2();
            break;
        case 3:
            client_mode_3();
            break;
        default:
            print_help();
            continue;
        }
    }
    return 0;
}