#include <sys/types.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <stdio.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <assert.h>
#include <glib.h>
#include <unistd.h>

#include "protocol.h"
#include "list.h"
#include "utils.h"
#include "ring_buffer.h"

struct timeval 
resent_timeout_tv = {0, RDT_RETSEND_TIMEOUT_MAX};

/* 选中的超时packet,作为retry的标记 */
u_int32_t blame_packet_for_expire = 0;

u_int32_t 
get_current() {
    struct timeval tv;
    gettimeofday(&tv, NULL);

    return tv.tv_sec * 1000 + tv.tv_usec / 1000;  /* 毫秒 */
}

u_int32_t
time_diff (u_int32_t now, u_int32_t past) {
    return now - past;
}

void
readhand_cb (evutil_socket_t fd, short what, void * arg) {
    rdt_t * rdt = (rdt_t *)arg;
    // event_add(rdt->event, &resent_timeout_tv);
}

/* 创建rdt, 每个连接通道一个 */
rdt_t *
create_rdt (int fd) {
    rdt_t *rdt = malloc(sizeof(rdt_t));
    rdt->fd = fd;
    rdt->sock_addr = NULL;
    rdt->mode = MODE_STREAM;
    rdt->mtu = RDT_MTU;
    rdt->resend_retry = 0;

    srand((unsigned)time(NULL));
    rdt->current_send_seq = (u_int32_t) (rand() % SEQ_NUM_MAX) + 1;
    // rdt->wnd_base = rdt->current_send_seq;
    rdt->wnd_size = RDT_WINDOW_SIZE;
    
    rdt->send_wnd_base = rdt->current_send_seq;
    rdt->recv_wnd_base = 0;
    // rdt->remt_wnd = 0;
    // rdt->cnt_wnd = 0;

    rdt->send_queue_len = 0;
    rdt->recv_queue_len = 0;
    
    list_init(&rdt->send_queue);
    list_init(&rdt->recv_queue);
    list_init(&rdt->flight_send_queue);
    list_init(&rdt->unack_list);

    /* init ringbuffer */
    ringbuff_init(&rdt->send_buffer, MAX_PROTOCOL_BUFF_LEN);
    ringbuff_init(&rdt->recv_buffer, MAX_PROTOCOL_BUFF_LEN);

    pthread_mutex_init(&rdt->mutex, NULL);

    rdt->terminate_worker = 0;
    pthread_t tid;
    if (pthread_create(&tid, NULL, send_worker_thread, rdt) == 0) {
        rdt->send_worker = tid;
    } else {
        EXIT("create process 'send_worker_thread' thread fail.");
    }

    if (pthread_create(&tid, NULL, recv_worker_thread, rdt) == 0) {
        rdt->recv_worker = tid;
    } else {
        EXIT("create process 'recv_worker_thread' thread fail.");
    }

    // event_init();
    // rdt->base = event_base_new();
    // rdt->event = event_new(rdt->base, rdt->fd, EV_READ, readhand_cb, rdt);

    // event_add(rdt->event, &resent_timeout_tv);
    // event_base_dispatch(rdt->base);

    return rdt;
}

void
free_rdt (rdt_t *rdt) {
    assert(rdt);

    ringbuff_free(&rdt->send_buffer);
    ringbuff_free(&rdt->recv_buffer);

    // event_free(rdt->event);
    // event_base_free(rdt->base);

    rdt->terminate_worker = 1;
    pthread_join(rdt->flush_worker, NULL);

    free(rdt);
}

void
rdt_dump (rdt_t *rdt) {
    LOG("send_buffer size= \t%d\n", ringbuff_data_size(&rdt->send_buffer));
    LOG("send_queue  size= \t%ld\n", list_size(&rdt->send_queue));
    LOG("            mtu = \t%d\n", rdt->mtu);
}

/* 将写 send_buffer 的数据下刷至 send_queue 中 */
void
flush_buffer (rdt_t *rdt) {
    assert(rdt);

    int numbytes;
    char buff[rdt->mtu];
    char *tmp = buff;
    packet_t *packet;

    while (1) {
        /* 计算窗口大小 */
        u_int32_t last_wnd = rdt->send_wnd_base + rdt->wnd_size - 1;
        if (rdt->current_send_seq > last_wnd) {
            // sleep(1);
            LOG("current_send_seq > last_wnd, waiting ...\n");
            
            /* 检测定时器 */
            resend_expire_packet(rdt);

            // packet_list_dump(& rdt->send_queue, "send_queue");
            // packet_list_dump(& rdt->flight_send_queue, "flight_send_queue");
            // dump_unack_list(& rdt->unack_list, "unack_list");

            break;
        }

        if ((numbytes = ringbuff_read(&rdt->send_buffer, tmp, rdt->mtu)) > 0) {
            packet = make_packet(rdt, tmp, numbytes, RDT_CMD_SEND);

            RDT_list_iterator * position = list_begin(& rdt->send_queue);
            pthread_mutex_lock(& rdt->mutex);
            list_insert(position, packet);
            pthread_mutex_unlock(& rdt->mutex);

            tmp = buff;
        } else {
            break;
        }
    }
}

/**
 * @brief 根据窗口大小调用不可靠通道发送数据
 * 
 * @param rdt 
 */
void
flush_queue (rdt_t *rdt) {
    RDT_list_iterator * position;
    RDT_list_iterator * head = list_head(& rdt->send_queue);
        
    /* 从队尾取packet, 放入flight_send_queue */
    for (position = head; position->prev != head; ) {
        RDT_list_iterator * tail =  position->prev;
        RDT_list_iterator * flight_position = list_begin(& rdt->flight_send_queue);

        packet_t *packet = list_remove(tail);
        if (packet->cmd == RDT_CMD_SEND) {
            pthread_mutex_lock(& rdt->mutex);
            list_insert (flight_position, packet);
            pthread_mutex_unlock(& rdt->mutex);
            

            // 添加至ack未接受列表
            unack_t * unack = malloc(sizeof(unack));
            unack->seq = packet->seq;
            pthread_mutex_lock(& rdt->mutex);
            seq_insert_to_list(& rdt->unack_list, unack);
            pthread_mutex_unlock(& rdt->mutex);

            // dump_unack_list(& rdt->unack_list, "unack_list");

            // TODO 设置定时器

        }
        
        // 执行发送操作，放在条件判断外
        udt_send (rdt, (char *)packet, packet->pkt_size);

        if (packet->cmd == RDT_CMD_ACK) {
            free_packet(packet);
        }
            
    }

    // packet_list_dump(& rdt->send_queue, "send_queue");
    // packet_list_dump(& rdt->flight_send_queue, "flight_send_queue");
    // packet_list_dump(& rdt->recv_queue, "recv_queue");
}


void
seq_insert_to_list (RDT_list * list, void *data) {
    RDT_list_iterator * position;
    unack_t * node = (unack_t *) data;

    if (list_empty(list)) {
        list_insert(list_begin(list), data);
        return ;
    }
        
    for ( position = list_begin(list);
          position != list_end(list);
          position = list_next(position)) {
        if (node->seq < ((unack_t *)position)->seq) {
            list_insert(position, data);
            return ;
        }
    }

    // 大于所有的数
    list_insert(list_end(list), data);
}

RDT_list_iterator *
find_unack(rdt_list * list, u_int32_t ack) {
    RDT_list_iterator * position;

    for ( position = list_begin(list);
          position != list_end(list);
          position = list_next(position)) {
        if (((unack_t *)position)->seq == ack)
            return position;
    }
    return NULL;
}

void dump_unack_list (RDT_list * list, char *name) {
    RDT_list_iterator * position;
    int index = 0;
    LOG("============== list element ============== (%s)\n", name);
    for ( position = list_begin(list);
          position != list_end(list);
          position = list_next(position)) {
        LOG("index:%d \tseq: %4d \n", ++index, ((unack_t *)position)->seq);
    }
    LOG("========================================== (%s)\n", name);
}

u_int32_t
get_and_update_seq (rdt_t *rdt) {
    u_int32_t tmp = rdt->current_send_seq;

    pthread_mutex_lock(& rdt->mutex);
    rdt->current_send_seq = ++ rdt->current_send_seq % SEQ_NUM_MAX;
    pthread_mutex_unlock(& rdt->mutex);

    return tmp;
}

u_int32_t
get_and_update_current (rdt_t *rdt) {
    pthread_mutex_lock(& rdt->mutex);
    rdt->current = get_current();
    pthread_mutex_unlock(& rdt->mutex);
    return rdt->current;
}

/**
 * @brief 数据封包
 * rdt: rdt 数据结构
 * buff: 数据buff
 * len: 数据长度
 */
packet_t *
make_packet(rdt_t *rdt, char *buff, int len, int cmd) {
    packet_t * packet = malloc(sizeof(packet_t) + len);

    packet->ts = get_and_update_current(rdt);
    packet->len = len;
    packet->pkt_size = sizeof(packet_t) + len;

    if (cmd == RDT_CMD_SEND) {
        packet->seq = get_and_update_seq(rdt);
    }
    packet->cmd = cmd;

    if (buff != NULL)
        memcpy(packet->data, buff, len);

    return packet;
}

/*  */
void 
free_packet(packet_t * packet) {
    free(packet);
}


void
packet_list_dump (rdt_list * list, char * name) {
    RDT_list_iterator * position;

    LOG("============== list element ============== (%s)\n", name);
    for ( position = list_begin(list);
          position != list_end(list);
          position = list_next(position)) {

        // LOG("pkt_size: %4d \tlen: %4d \tseq: %10d \tts: %10d \tack:%d \tcmd:%d \tcontnt:%s\n",
        LOG("pkt_size: %4d \tlen: %4d \tseq: %10d \tts: %10d \tack:%d \tcmd:%d\n", 
            ((packet_t *)position)->pkt_size,
            ((packet_t *)position)->len,
            ((packet_t *)position)->seq,
            ((packet_t *)position)->ts,
            ((packet_t *)position)->ack,
            ((packet_t *)position)->cmd
            // ((packet_t *)position)->data
        );
    }
    LOG("========================================== (%s)\n", name);
}

RDT_list_iterator *
find_packet(rdt_list * list, u_int32_t ack) {
    RDT_list_iterator * position;

    for ( position = list_begin(list);
          position != list_end(list);
          position = list_next(position)) {
        if (((packet_t *)position)->seq == ack)
            return position;
    }
    return NULL;
}

// TODO
void 
resend_expire_packet (rdt_t *rdt) {
    rdt_list * list = &rdt->flight_send_queue;
    RDT_list_iterator * position;

    pthread_mutex_lock(& rdt->mutex);
    for ( position = list_back(list);
          position != list_end(list);
          position = list_prev(position)) {
        /* 超时重传 */
        if ( time_diff ( get_current(), ((packet_t *)position)->ts) > PACKET_EXPIRE_TIME ) {
            ((packet_t *)position)->ts = get_and_update_current(rdt);

            LOG("resend seq:%u\n", ((packet_t *)position)->seq);
            udt_send (rdt, (char *)position, ((packet_t *)position)->pkt_size);

            if ( !blame_packet_for_expire )
                blame_packet_for_expire = ((packet_t *)position)->seq;

            if ( blame_packet_for_expire && 
                 blame_packet_for_expire == ((packet_t *)position)->seq ) {
                rdt->resend_retry ++;
                if (rdt->resend_retry > PACKET_RESEND_RETRY_MAX) {
                    LOG("loss seq:%u , disconnect session !!!!!\n", blame_packet_for_expire);
                    // TODO
                    exit(-1);
                }
            }
        }
    }
    pthread_mutex_unlock(& rdt->mutex);
}

/* 不可靠发送 */
int 
udt_send (rdt_t *rdt, char *buff, int len) {
    socklen_t sin_size = sizeof(struct sockaddr_in);
    return sendto(rdt->fd, buff, len, 0, rdt->sock_addr, sin_size);
}

/* 不可靠接受 */
int 
udt_recv (rdt_t *rdt, char *buff, int len) {
    socklen_t sin_size = sizeof(struct sockaddr_in);
    return recvfrom(rdt->fd, buff, len, 0, rdt->sock_addr, &sin_size);
}

/* 可靠发送 */
int 
rdt_send (rdt_t *rdt, char *buff, int len) {
    assert(rdt);
    assert(&rdt->send_buffer);

    int numbytes;

    // TODO sleep
    while (len > ringbuff_free_size(&rdt->send_buffer)) {
        flush_buffer(rdt);
        usleep(5);
        LOG("send ringbuff wait flush ...\n\t(input len:%d, ringb_data_size:%d, ringb_free_size:%d)\n", 
            len, ringbuff_data_size(&rdt->send_buffer), ringbuff_free_size(&rdt->send_buffer));
    }

    if ((numbytes = ringbuff_write(&rdt->send_buffer, buff, len)) != len) {
        LOG("user data len:%d not equal send len:%d !!!\n", len, numbytes);
    }
    return numbytes;
}

/* 可靠接受 */
int 
rdt_recv (rdt_t *rdt, char *buff, int len) {
    assert(rdt);
    assert(&rdt->recv_buffer);

    int numbytes;

    // TODO sleep
    while (0 == ringbuff_data_size(&rdt->recv_buffer)) {
        usleep(5);
    }

    if ((numbytes = ringbuff_read(&rdt->recv_buffer, buff, len)) != len) {
        // LOG("user data len:%d not equal recv len:%d !!!\n", len, numbytes);
    }
    return numbytes;
}

/**
 * @brief 处理'读'和'写'任务
 * 
 * @param data 
 * @return void* 
 */
static void *
send_worker_thread(void * data) {
    rdt_t * rdt = (rdt_t *)data;
    usleep(1000 * 10);
    LOG("[Worker] I am send_worker_thread, tid=%lu\n", rdt->send_worker);
    
    while (1 != rdt->terminate_worker) {
        usleep(1);
        flush_buffer(rdt);

        if (!list_empty(& rdt->send_queue)) {
            flush_queue(rdt);
        }
    }
}

static void *
recv_worker_thread(void * data) {
    rdt_t * rdt = (rdt_t *)data;
    usleep(1000 * 10);
    LOG("[Worker] I am recv_worker_thread, tid=%lu\n", rdt->recv_worker);
    
    while (1 != rdt->terminate_worker) {
        usleep(1);

        int numbytes;
        int packet_len = rdt->mtu + sizeof(packet_t);
        char recv_buff[packet_len];
        bzero(recv_buff, packet_len);

        /* receiver recv packet */
        if((numbytes = udt_recv(rdt, recv_buff, packet_len)) == -1) {
            LOG("udt_recv on recv_worker error!");
            continue;
        }

        packet_t * recv_packet = (packet_t *)recv_buff;

        /* ack packet (发送方收到) */
        if ( recv_packet->cmd == RDT_CMD_ACK ) {
            /* 已接收到的重复应答，跳过 */
            if ( recv_packet->ack < rdt->send_wnd_base)
                continue;

            RDT_list_iterator * position = find_packet(& rdt->flight_send_queue, recv_packet->ack);
            if (NULL != position) {
                // LOG("recv_ack ack:%u, successful send seq:%u len:%d\n", recv_packet->ack, ((packet_t *)position)->seq, ((packet_t *)position)->len);
                /* 清除重试计数 */
                if ( blame_packet_for_expire && 
                    blame_packet_for_expire == ((packet_t *)position)->seq ) {
                    rdt->resend_retry = 0;
                    blame_packet_for_expire = 0;
                }

                RDT_list_iterator * unack = find_unack(& rdt->unack_list, ((packet_t *)position)->seq);
                if (NULL != unack) {
                    /* wnd_base 找到连续的 */
                    if (((unack_t *)unack)->seq == rdt->send_wnd_base) {
                        RDT_list_iterator * next_unack = unack->next;

                        if (next_unack != list_end(& rdt->unack_list)) {
                            rdt->send_wnd_base = ((unack_t *)next_unack)->seq;
                        } else {
                            rdt->send_wnd_base ++;
                        }
                        pthread_mutex_lock(& rdt->mutex);
                        free(list_remove(unack));
                        pthread_mutex_unlock(& rdt->mutex);
                    } 
                    /* wnd_base 未找到连续的 */
                    else {
                        pthread_mutex_lock(& rdt->mutex);
                        free(list_remove(unack));
                        pthread_mutex_unlock(& rdt->mutex);
                    }
                }

                pthread_mutex_lock(& rdt->mutex);
                list_remove(position);
                pthread_mutex_unlock(& rdt->mutex);

                free_packet((packet_t *)position);
            }

            // packet_list_dump(& rdt->recv_queue, "recv_queue");

            // packet_list_dump(& rdt->send_queue, "send_queue");
            // packet_list_dump(& rdt->flight_send_queue, "flight_send_queue");
            // dump_unack_list(& rdt->unack_list, "unack_list");

            continue;
        }

        // for test
        ((packet_t *)recv_buff)->data[((packet_t *)recv_buff)->len] = '\0';

        /* data packet (接收方收到) */
        if ( recv_packet->cmd == RDT_CMD_SEND || recv_packet->cmd == RDT_CMD_TEST ) {
            if ( recv_packet->cmd == RDT_CMD_SEND ) {
                /* data packet, send ack */
                packet_t * packet = make_packet(rdt, NULL, 0, RDT_CMD_ACK);
                packet->ack = recv_packet->seq;
                RDT_list_iterator * position = list_begin(& rdt->send_queue);

                pthread_mutex_lock(& rdt->mutex);
                list_insert(position, packet);
                pthread_mutex_unlock(& rdt->mutex);

                // LOG("recv seq:%u \tlen: %d\n", recv_packet->seq, recv_packet->len);

                /* 更新接收窗口 */
                if (0 == rdt->recv_wnd_base) {
                    rdt->recv_wnd_base = recv_packet->seq + 1;  // first seq + 1
                    LOG("first recv_wnd_base=%u\n", rdt->recv_wnd_base);

                    /* return data to user recv buff */
                    push_to_recv_buff(rdt, recv_packet);
                    continue;
                }

                /* 丢弃已收到的包 */
                if ( recv_packet->seq < rdt->recv_wnd_base || 
                     rdt->recv_wnd_base + rdt->wnd_size < recv_packet->seq ) {
                    LOG("discard seq:%u \tlen:%d\n", recv_packet->seq, recv_packet->len);
                    continue;
                }

                /* 对收包进行排序 */
                if ( recv_packet->seq == rdt->recv_wnd_base ) {
                    push_to_recv_buff(rdt, recv_packet);
                    rdt->recv_wnd_base ++;

                    RDT_list_iterator * next_packet;
                    RDT_list_iterator * to_be_remove;
                    for ( next_packet = list_begin(& rdt->recv_queue);
                          next_packet != list_end(& rdt->recv_queue);
                          ) {
                        if (((packet_t *)next_packet)->seq == rdt->recv_wnd_base) {
                            push_to_recv_buff(rdt, (packet_t *)next_packet);
                            rdt->recv_wnd_base ++;

                            to_be_remove = next_packet;
                            next_packet = list_next(next_packet);

                            pthread_mutex_lock(& rdt->mutex);
                            list_remove(to_be_remove);
                            pthread_mutex_unlock(& rdt->mutex);

                            free_packet((packet_t *)to_be_remove);
                        }
                    }
                } 
                /* recv_wnd_base 未找到连续的 */
                else {
                    packet_t * tmp_pkt = malloc(recv_packet->pkt_size);
                    memcpy(tmp_pkt, recv_packet, recv_packet->pkt_size);
                    
                    pthread_mutex_lock(& rdt->mutex);
                    seq_insert_to_list(& rdt->recv_queue, tmp_pkt);
                    pthread_mutex_unlock(& rdt->mutex);
                }

                // packet_list_dump(& rdt->recv_queue, "recv_queue");
                // LOG("recv_wnd_base:%u \trecv_buff:%d\n", rdt->recv_wnd_base, ringbuff_data_size(& rdt->recv_buffer));
                continue;
            }

            // push RDT_CMD_TEST data to user
            push_to_recv_buff(rdt, recv_packet);
            // LOG("recv_worker \n"
            //     "\tpkt len:%d \tdata len:%d \tseq:%lu \tcmd:%d \tcontnt:%s\n", 
            //         numbytes, ((packet_t *)recv_buff)->len, 
            //                     ((packet_t *)recv_buff)->seq,
            //                     ((packet_t *)recv_buff)->cmd,
            //                     ((packet_t *)recv_buff)->data);
        }
    }
}

int
push_to_recv_buff (rdt_t *rdt, packet_t * packet) {
    assert(packet);

    int data_size = packet->len;
    if (0 == ringbuff_free_size(& rdt->recv_buffer)) {
        LOG("ringbuff_free_size == 0, discard\n");
        return 0;
    }

    int w_size = ringbuff_write(& rdt->recv_buffer, packet->data, packet->len);
    if(data_size != w_size)
        LOG("ringbuff_free_size < recv data size, discard\n");
    
    return w_size;
}
