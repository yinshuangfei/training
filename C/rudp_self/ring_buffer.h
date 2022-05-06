#ifndef _RING_BUFFER_H_
#define _RING_BUFFER_H_

/* 
   仅凭 front = rear无法区分队列是空还足满, 因此,为区分队列足空或是满,
   不应填满队列的所有单元,而应把队列元素个数达到 MAX_QUEUE_SIZE时视为队
   列满, 这样就使条件 front = rear 唯一对应队列空的情形。
*/

typedef struct {
    int size;           // buffer 最大长度
    int wr_point;       // write 位置
    int rd_point;       // read 位置
    char *buffer;      // 不占空间
} Ringbuffer ;

void
ringbuff_init(Ringbuffer * rb, int size);

void
ringbuff_free(Ringbuffer * rb);

int
ringbuff_data_size(Ringbuffer * rb);

int
ringbuff_free_size(Ringbuffer * rb);

int
is_ringbuff_full(Ringbuffer * rb);

int
is_ringbuff_empty(Ringbuffer * rb);

int
ringbuff_write(Ringbuffer * rb, char *buff, int len);

int
ringbuff_read(Ringbuffer * rb, char *buff, int len);

void 
ringbuff_dump(Ringbuffer * rb);


#endif /* _RING_BUFFER_H_ */