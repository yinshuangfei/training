#ifndef _PROTOCOL_H_
#define _PROTOCOL_H_

#include <pthread.h>
#include <event.h>

#include "list.h"
#include "common.h"
#include "ring_buffer.h"

/**
 * RDT: reliable data transmit
 * UDT: unreliable data transmit
 * 
 * 可能遇到的问题：（应对方法）
 * 1.bit位错误；（checksum）
 * 2.包丢失；（超时重传）
 * 3.ack丢失；（超时重传）
 * 3.乱序；（缓存）
 * 4.重包；（序列号）
 * 5.窗口长度：seq_len >= 2 * win_len
 * 
 * 加速传输：
 * 1.multiple inflight packets;
 * 2.selective repeat;
 * 
 * 超时时长定义：
 * RTT
 * 
 * 协议特性：
 * 1.流模式;
 * 2.ack机制;
 * 3.处理重复ack/packet;
 * 3.连接管理;
 * 4.窗口管理;
 * 5.有序到达;
 * 6.线程安全;
 * 7.拥塞机制;
 * 
 **/

// #define RDT_MTU 1024
#define RDT_MTU                 1024
#define SEQ_NUM_MAX             4294967295
#define MAX_PROTOCOL_BUFF_LEN   4 * 1024 * 1024   /* TCP 默认 212992 = 208*1024 */
#define RDT_WINDOW_SIZE         10
#define RDT_RETSEND_TIMEOUT_MAX 1000 * 1000  /* 微秒 */
#define PACKET_EXPIRE_TIME      100             /* 微秒 */
#define PACKET_RESEND_RETRY_MAX 10

/** packet
 * 操作系统的字节对齐会增加结构体的占用空间
*/
typedef struct _packet {
    list_node packet_list;
    enum {
        RDT_CMD_NULL,       // 0
	    RDT_CMD_ACK,        // 1
        RDT_CMD_SEND,       // 2
        RDT_CMD_TEST,       // 3
    } cmd;

    int pkt_size;           /*  */
    int len;                /* data length */
    u_int32_t seq;                /* 4 */
    u_int32_t ack;                /* 4 */
    u_int32_t ts;            /* 4 */     // timestamp in millisec
    char sync;

    u_int32_t wnd;          // 窗口大小
    u_int32_t una;          //

    char data[1];           /* 1, important */
} packet_t;
// __attribute__((packed)) packet_t;

typedef struct _unack {
    list_node packet_list;
    u_int32_t seq;
} unack_t;

/**
 * @brief 
 * rcv_wup: 滑动窗口的左边沿, 即落在滑动窗口中的最小的一个序号, 
 rcv_wup+rcv_wnd即为滑动窗口的右边沿, rcv_wup+rcv_wnd-rcv_nxt即为滑动窗口的空白部分.
 它的初始值为0，在移动滑动窗口时被更新。
 * snd_wnd: 是发送窗口的大小，直接取值于来自对端的数据报的首部。
 * snd_una: 表示当前正等待ACK的第一个序号，而发送窗口实际上是在每次收到来自对端的ACK后，
 都会更新，所以，实际上snd_una成了发送窗口的左边沿。
 * 
 */
typedef struct _rdt {
    int fd;                         // 套接字 fd
    struct sockaddr *sock_addr;     // 发送地址的 sockaddr

    enum {
	    MODE_PACKET,                /* 数据包模式 */
	    MODE_STREAM,                /* 流模式 */
    } mode;

    int mtu;
    int resend_retry;
    u_int32_t current;
    u_int32_t current_send_seq;
    u_int32_t wnd;                  // 本端窗口大小
    u_int32_t wnd_size;             /* 窗口大小 */
    u_int32_t wnd_base;
    u_int32_t send_wnd_base;             /* 发送窗口 */
    u_int32_t recv_wnd_base;             /* 接收窗口 */
    // u_int32_t remt_wnd;             /* remote window*/
    // u_int32_t cnt_wnd;              /*current window*/

    u_int32_t send_una;
    

    size_t send_queue_len;
    size_t recv_queue_len;
    rdt_list send_queue;            // 发送队列，保存未发送的用户的数据包
    rdt_list recv_queue;            // 接收队列，保存接收的用户的数据包
    rdt_list flight_send_queue;
    rdt_list unack_list;

    Ringbuffer send_buffer;         // 发送buff
    Ringbuffer recv_buffer;         // 发送buff

    int terminate_worker;
    pthread_t flush_worker;
    pthread_t send_worker;
    pthread_t recv_worker;
    
    struct event_base * base;
    struct event * event;

} rdt_t;

u_int32_t 
get_current();

rdt_t *
create_rdt(int fd);

void
free_rdt(rdt_t *rdt);

void
rdt_dump (rdt_t *rdt);

void
flush_buffer (rdt_t *rdt);

void
flush_queue (rdt_t *rdt);

void
insert_to_unack_list (RDT_list * list, void *data);

u_int32_t
get_and_update_seq (rdt_t *rdt);

u_int32_t
get_and_update_current (rdt_t *rdt);

packet_t *
make_packet(rdt_t *rdt, char *buff, int len, int cmd);

void 
free_packet(packet_t * packet);

void
packet_list_dump (rdt_list * list, char * name);

RDT_list_iterator *
find_packet(rdt_list * list, u_int32_t ack);

void 
resend_expire_packet (rdt_t *rdt);

int 
udt_send(rdt_t *rdt, char *buff, int len);

int 
udt_recv(rdt_t *rdt, char *buff, int len);

int 
rdt_send(rdt_t *rdt, char *buff, int len);

int 
rdt_recv(rdt_t *rdt, char *buff, int len);

static void *
send_worker_thread(void * data);

static void *
recv_worker_thread(void * data);

#endif /* _PROTOCOL_H_ */