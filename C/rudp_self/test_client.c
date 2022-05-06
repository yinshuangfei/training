#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <strings.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>                      /* netbd.h is needed for struct hostent =) */
#include <errno.h>
// #include <enet/enet.h>

#include "test_client.h"
#include "utils.h"
#include "common.h"
#include "protocol.h"

static int finished;

// ENET_CALLBACK * enet_callback(struct _ENetPacket *packet) {
//     finished = 1;
// }

static int init_udp(struct sockaddr_in *server) {
    int fd;
    struct hostent *he;                     /* structure that will get information about remote host */

    if ((he = gethostbyname(hostname)) == NULL){
        printf("gethostbyname() error\n");
        exit(1);
    }

    if ((fd = socket(AF_INET, SOCK_DGRAM, 0)) == -1){
        printf("socket() error\n");
        exit(1);
    }

    bzero(server, sizeof(struct sockaddr_in));
    server->sin_family = AF_INET;
    server->sin_port = htons(PORT); 
    server->sin_addr = *((struct in_addr *)he->h_addr);  /*he->h_addr passes "*he"'s info to "h_addr" */
    return fd;
}

int client_mode_1() {
    int fd;
    struct sockaddr_in server, client;
    char recvbuf[PROTOCOL_MTU_C];
    socklen_t len = sizeof(struct sockaddr_in);
    int numbytes;

    fd = init_udp(&server);

    /* transfer cmd */
    char *cmd = "1";
    if ((numbytes = sendto(fd, cmd, strlen(cmd), 0, (struct sockaddr *)&server, len)) == -1) {
        close(fd);
        printf("error exit (%d), %s.\n", -errno, strerror(errno));
        return -errno;
    }

    /* recv data */
    int packages = 0;
    while (1) {
        if ((numbytes = recvfrom(fd, recvbuf, PROTOCOL_MTU_C, 0, (struct sockaddr *)&server, &len)) == -1) {
            printf("recvfrom() error\n");
            return -errno;
        }

        if (numbytes == 0) {
            printf("recv end, packages: %d, package size: %d, recycle...\n", packages, PROTOCOL_MTU_C);
            break;
        }

        // if (numbytes < 100)
        //     printf("recv %d (%s)\n", numbytes, recvbuf);
        // else
        //     printf("recv %d\n", numbytes);
        
        packages ++;
    }
    close(fd);
}

int client_mode_2() {
//     if (enet_initialize() != 0) {
//         printf("An error occurred while initializing ENet.\n");
//         return -EXIT_FAILURE;
//     }

//     ENetHost* client = { 0 };
//     client = enet_host_create (NULL /* create a client host */,
//                 1 /* only allow 1 outgoing connection */,
//                 2 /* allow up 2 channels to be used, 0 and 1 */,
//                 0 /* assume any amount of incoming bandwidth */,
//                 0 /* assume any amount of outgoing bandwidth */);
//     if (client == NULL) {
//         printf("An error occurred while trying to create an ENet client host.\n");
//         return -EXIT_FAILURE;
//     }

//     ENetAddress address = { 0 };
//     ENetEvent event = { 0 };
//     ENetPeer* peer = { 0 };

//     /* set address information */
//     enet_address_set_host(&address, hostname);
//     address.port = PORT;

//     /* Initiate the connection, allocating the two channels 0 and 1. */
//     peer = enet_host_connect(client, &address, 2, 0);
//     if (peer == NULL) {
//         printf("No available peers for initiating an ENet connection.\n");
//         enet_host_destroy(client);
//         enet_deinitialize();
//         return -EXIT_FAILURE;
//     }

//     /* Wait up to 3 seconds for the connection attempt to succeed. */
//     if (enet_host_service (client, &event, 300) > 0 &&
//         event.type == ENET_EVENT_TYPE_CONNECT) {
//         printf ("Connection to server %s:%d succeeded.\n", hostname, PORT);
//     } else {
//         /* Either the 3 seconds are up or a disconnect event was */
//         /* received. Reset the peer in the event the 5 seconds   */
//         /* had run out without any significant event.            */
//         enet_peer_reset (peer);
//         printf("Connection to server failed.\n");
//         enet_host_destroy(client);
//         enet_deinitialize();
//         return -EXIT_FAILURE;
//     }

//     ENetPacket * packet;

//     size_t len = (size_t)1024 * 1024 * MODE2_TRANSFER_SIZE; // MB in size
//     size_t total = len;
//     int packages = 0;
//     int ret;

//     char size_str[128];
//     transfer_size(len, size_str);
//     printf("total transfer size: %s\n", size_str);

//     gettimeofday(&t_start, NULL); 
//     while (len != 0) {
//         char raw[PROTOCOL_MTU_ENET];
//         int _len;
        
//         if (len > PROTOCOL_MTU_ENET) {
//             _len = PROTOCOL_MTU_ENET;
//             packet = enet_packet_create (raw, 
//                                         _len, 
//                                         ENET_PACKET_FLAG_RELIABLE);
//         } else {
//             _len = len;
//             packet = enet_packet_create (raw, 
//                                         _len, 
//                                         ENET_PACKET_FLAG_RELIABLE);
//         }
//         enet_peer_send (peer, 0, packet);
        
//         finished = 0;
//         packet->freeCallback = enet_callback;

//         while(finished == 0) {
//             if ((ret = enet_host_service (client, &event, 0)) > 0 ) {
//                 printf("enet_host_service error (%d), force disconnect.\n", ret);
//                 goto out;
//             } else if (ret < 0){
//                 printf("enet_host_service error (%d), force disconnect.\n", ret);
//                 goto out;
//             }
//         }

//         // printf("will send:%d, left:%llu, packages:%d\n", _len, len, packages + 1);
//         len -= _len;
//         packages ++;
//         print_percentage(len, total);
//     }
//     printf("send end, packages: %d, package size: %d, recycle...\n", packages, PROTOCOL_MTU_ENET);
//     gettimeofday(&t_end, NULL ); 
//     calculate_speed(t_start, t_end, total);

// out:
//     /* Disconnect */
//     enet_peer_disconnect(peer, 0);

//     uint8_t disconnected = 0;
//     /* Allow up to 3 seconds for the disconnect to succeed
//      * and drop any packets received packets.
//      */
//     while (enet_host_service(client, &event, 300) > 0) {
//         switch (event.type) {
//         case ENET_EVENT_TYPE_RECEIVE:
//             enet_packet_destroy(event.packet);
//             break;
//         case ENET_EVENT_TYPE_DISCONNECT:
//             puts("Disconnection succeeded.");
//             disconnected = 1;
//             break;
//         }
//     }

//     // Drop connection, since disconnection didn't successed
//     if (!disconnected) {
//         enet_peer_reset(peer);
//     }

//     enet_host_destroy(client);
//     enet_deinitialize();
//     return 0;
}

int client_mode_3() {
    int fd;
    struct sockaddr_in server;
    
    fd = init_udp(&server);

    rdt_t *rdt = create_rdt(fd);
    rdt->sock_addr = (struct sockaddr *)&server;

    char buf[128];
    int data_len;

    while (1) {
        usleep(1000 * 50);
        /* 大于0则接收指定大小的字符串，小于0则测速 */
        printf("Bytes to recv> ");
        fgets(buf, 1024, stdin);

        if ((data_len = atoi(buf)) == 0) 
            continue;

        if (data_len > MAX_PROTOCOL_BUFF_LEN - 1) {
            LOG("number:(%d) too big! reset to %d\n", data_len, MAX_PROTOCOL_BUFF_LEN - 1);
            data_len = MAX_PROTOCOL_BUFF_LEN -1;
        }

        if (data_len < 0) {
            LOG("number:(%d) < 0! reset to %d\n", data_len, -1);
            data_len = -1;
        }

        _do_mode3(rdt, data_len);
    }

    free_rdt(rdt);
    close(fd);
}

static int
_do_mode3(rdt_t *rdt, int len){
    /* transfer cmd */
    int numbytes;
    char cmd[8];
    sprintf(cmd, "%d", len);

    // _internal send msg
    packet_t *packet = make_packet(rdt, cmd, strlen(cmd), RDT_CMD_TEST);
    RDT_list_iterator * position = list_begin(& rdt->send_queue);
    list_insert(position, packet);
    

    char buff[MODE3_MAX_USER_BUFF_LEN];
    int ret;
    bzero(buff, MODE3_MAX_USER_BUFF_LEN);

    int left = len;
    int index = 0;
    while (left > 0) {
        if ((ret = rdt_recv(rdt, buff, MODE3_MAX_USER_BUFF_LEN)) < 0) {
            LOG("server recv error.\n");
        }
        // LOG("client recv:"
        //     "\n-------------------(%d, %d)"
        //     "\n%s"
        //     "\n-------------------\n", ++index, ret, buff);
        left -= ret;
    }
    if (left != 0)
        LOG("left=%d != 0, error!!!\n", left);
    LOG("********  ringbuff_data_size:%d *************\n", ringbuff_data_size(& rdt->recv_buffer));
}
