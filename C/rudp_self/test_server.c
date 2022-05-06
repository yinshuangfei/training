#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/time.h>
#include <errno.h>
// #include <enet/enet.h>

#include "test_server.h"
#include "utils.h"
#include "common.h"
#include "protocol.h"
#include "ring_buffer.h"

/**
struct timeval {  
　　long tv_sec; // 秒数  
　　long tv_usec; //微秒数  
}
from <sys/time.h>
**/

socklen_t sin_size = sizeof(struct sockaddr_in);

static int init_udp(struct sockaddr_in *server) {
    int sockfd;                 /* socket descriptors */
    
    /* Creating UDP socket */
    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) == -1) {
        perror("Creating socket failed.");
        exit(1);
    }

    bzero(server, sizeof(*server));
    server->sin_family = AF_INET;
    server->sin_port = htons(PORT);
    server->sin_addr.s_addr = htonl (INADDR_ANY);

    if (bind(sockfd, (struct sockaddr *)server, sizeof(struct sockaddr)) == -1) {
        perror("Bind error.");
        exit(1);
    }

    return sockfd;
}

int server_mode_1() {
    struct sockaddr_in server;  /* server's address information */
    struct sockaddr_in client;  /* client's address information */
    char recvmsg[PROTOCOL_MTU_S];  /* buffer for message */
    char sendmsg[PROTOCOL_MTU_S];
    int num;
    int sockfd;

    socklen_t sin_size = sizeof(struct sockaddr_in);
    sockfd = init_udp(&server);

    while (1) {
        printf("mode 1 wait to stransfer ...\n");
        /* recv cmd from client */
        num = recvfrom(sockfd, recvmsg, PROTOCOL_MTU_S, 0, (struct sockaddr *)&client, &sin_size);
        if (num < 0){
            close(sockfd);
            perror("server recvfrom error\n");
            return -1;
        }

        recvmsg[num] = '\0';
        printf("We got a message:(%s) from %s\n", recvmsg, inet_ntoa(client.sin_addr) );

        int cmd = atoi(recvmsg);
        if ( cmd == 1)
            _do_mode_1(&client, sockfd);
        else
            printf("unknown cmd, continue ...\n");
    }
    close(sockfd); /* close listenfd */
    return 0;
}

static void _do_mode_1(struct sockaddr_in *client, int sockfd) {
    char recvmsg[PROTOCOL_MTU_S];  /* buffer for message */
    char sendmsg[PROTOCOL_MTU_S];
    size_t len = (size_t)1024 * 1024 * MODE1_TRANSFER_SIZE; // MB in size
    size_t total = len;
    int len_send;
    int packages = 0;
    socklen_t sin_size = sizeof(struct sockaddr_in);

    char size_str[128];
    transfer_size(len, size_str);
    printf("total transfer size: %s\n", size_str);

    gettimeofday(&t_start, NULL); 
    while (len > 0) {
        char raw[PROTOCOL_MTU_S];
        if (len > PROTOCOL_MTU_S) {
            if ((len_send = sendto(sockfd, raw, PROTOCOL_MTU_S, 0, (struct sockaddr *)client, sin_size)) == -1) {
                printf("error exit (%d), %s.\n", -errno, strerror(errno));
                printf("send end, packages: %d, package size: %d\n", packages, PROTOCOL_MTU_S);
                return ;
            }
        } else {
            if ((len_send = sendto(sockfd, raw, len, 0, (struct sockaddr *)client, sin_size)) == -1) {
                printf("error exit(%d), %s.\n", -errno, strerror(errno));
                printf("send end, packages: %d, package size: %d\n", packages, PROTOCOL_MTU_S);
                return ;
            }
        }

        len -= len_send;
        packages ++;
        // print_percentage(len, total);
    }

    /* end transfer */
    sendto(sockfd, sendmsg, 0, 0, (struct sockaddr *)client, sin_size);

    printf("send end, packages: %d, package size: %d, recycle...\n", packages, PROTOCOL_MTU_S);
    gettimeofday(&t_end, NULL ); 
    calculate_speed(t_start, t_end, total);
}

int server_mode_2() {
    // printf("mode 2 start stransfer ...\n");
    // ENetHost *server;
    // ENetPeer *peer;
    // ENetEvent event;

    // if (enet_initialize () != 0) {
    //     printf("An error occurred while initializing ENet.\n");
    //     return -EXIT_FAILURE;
    // }

    // ENetAddress address = {0};
    // address.host = ENET_HOST_ANY;   /* Bind the server to the default localhost.     */
    // address.port = PORT;

    // server = enet_host_create(&address, MAX_CLIENTS, 2, 0, 0);
    // if (server == NULL) {
    //     printf("An error occurred while trying to create an ENet server host.\n");
    //     return -EXIT_FAILURE;
    // }

    // /* Wait up to 1000 milliseconds for an event. (WARNING: blocking) */
    // int packages = 0;
    // while (enet_host_service(server, &event, 1000) >= 0) {
    //     switch (event.type) {
    //         case ENET_EVENT_TYPE_CONNECT: /*1*/
    //             printf("A new client connected from %x:%u.\n",  event.peer->address.host, event.peer->address.port);
    //             /* Store any relevant client information here. */
    //             event.peer->data = "Client information xxx";
    //             break;

    //         case ENET_EVENT_TYPE_RECEIVE: /*3*/
    //             // printf("A packet of length %lu containing %s was received from %s on channel %u.\n",
    //             //         event.packet->dataLength,
    //             //         event.packet->data,
    //             //         event.peer->data,
    //             //         event.channelID);
    //             // printf("len:%d, packages:%d\n", event.packet->dataLength, packages + 1);
    //             /* Clean up the packet now that we're done using it. */
    //             enet_packet_destroy (event.packet);
    //             packages++;
    //             break;

    //         case ENET_EVENT_TYPE_DISCONNECT: /*2*/
    //             printf("%s disconnected.\n", event.peer->data);
    //             /* Reset the peer's client information. */
    //             event.peer->data = NULL;
    //             printf("packages=%d\n", packages);
    //             packages = 0;
    //             break;

    //         case ENET_EVENT_TYPE_NONE: /*0*/
    //             printf("none\n");
    //             break;
    //         default:
    //             printf("type:%d\n", event.type);
    //     }
    // }

    // enet_host_destroy(server);
    // enet_deinitialize();
    // return 0;
}

int server_mode_3() {
    struct sockaddr_in server;  /* server's address information */
    struct sockaddr_in client;  /* client's address information */

    int sockfd;
    int num = 0;

    sockfd = init_udp(&server);

    rdt_t *rdt = create_rdt(sockfd);
    rdt->sock_addr = (struct sockaddr *)&client;
    printf("\ncurrent_send_seq: %u\n", rdt->current_send_seq);

    while (1) {
        printf("mode 3 start stransfer ...\n");
        char buf[rdt->mtu];
        int len;

        if ((len = rdt_recv(rdt, buf, rdt->mtu)) < 0) {
            LOG("server recv error.\n");
        }

        char *ptr = malloc(len + 1);
        memcpy(ptr, buf, len);
        ptr[len] = '\0';

        LOG("len=%d content=%s\n", len, ptr);

        if (atoi(ptr) > 0 ) {
            _do_server_mode_3(rdt, atoi(ptr));
            continue;
        }
    }

    close(sockfd);
    return 0;
}

static int
_do_server_mode_3(rdt_t *rdt, int len) {
    /* prepare and make data  */
    char raw[len + 1];
    char * tmp = raw;
    int numbytes;
    int i;
    
    for (i = 0; i < len; i ++) {
        char a = 'A' + (i % 58);
        memcpy(tmp++, &a, 1);
    }
    raw[len] = '\0';
    // LOG("mk data len:%d (%s)\n", len, raw);

    if((numbytes = rdt_send(rdt, raw, len)) < 0 ) {
        LOG("send error\n");
    } else if (numbytes != len) {
        LOG("data len error, raw len:%d, send len:%d\n", len, numbytes);
    }
}

