#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "ring_buffer.h"

void
ringbuff_init(Ringbuffer * rb, int size) {
    assert(size > 0);

    rb->size = size;
    rb->wr_point = 0;
    rb->rd_point = 0;
    rb->buffer = calloc(1, size);
}

void
ringbuff_free(Ringbuffer * rb) {
    assert(rb);
    assert(rb->buffer);

    free(rb->buffer);
}

int
ringbuff_data_size(Ringbuffer * rb) {
    int size = rb->wr_point - rb->rd_point;
    if (size >= 0)
        return size;
    else 
        return size + rb->size;
}

int
ringbuff_free_size(Ringbuffer * rb) {
    return ( rb->size - 1 - ringbuff_data_size(rb) );
}

int
is_ringbuff_full(Ringbuffer * rb) {
    return ringbuff_data_size(rb) == (rb->size - 1);
}

int
is_ringbuff_empty(Ringbuffer * rb) {
    return rb->wr_point == rb->rd_point;
}

int
ringbuff_write(Ringbuffer * rb, char *buff, int len) {
    int point, count = 0;
    int free_size = ringbuff_free_size(rb);
    
    /* 放不下不让放 */
    if (len > free_size) {
        len = free_size;    // 自动截断
    }
    
    point = rb->wr_point;
    /* 需要回头 */
    if (point + len > rb->size -1 ){
        int firstpart_len = rb->size - point;

        memcpy(rb->buffer + point, buff, firstpart_len);
        buff += firstpart_len;
        len -= firstpart_len;
        
        point = 0;
        count += firstpart_len;
    }

    memcpy(rb->buffer + point, buff, len);
    rb->wr_point = point + len;

    count += len;

    return count;
}

int
ringbuff_read(Ringbuffer * rb, char *buff, int len) {
    int point, count = 0;
    int data_size = ringbuff_data_size(rb);

    if (len > data_size)
        len = data_size;

    point = rb->rd_point;
    /* 需要回头 */
    if (point + len > rb->size -1 ){
        int firstpart_len = rb->size - point;

        memcpy(buff, rb->buffer + point, firstpart_len);
        buff += firstpart_len;
        len -= firstpart_len;
        
        point = 0;
        count += firstpart_len;
    }

    memcpy(buff, rb->buffer + point, len);
    rb->rd_point = point + len;

    count += len;

    return count;
}

void 
ringbuff_dump(Ringbuffer * rb) {
    int line_size = 10;
    int i;
    printf("|");
    for (i = 0; i < rb->size; i++) {
        if (i != 0 && i % line_size == 0)
            printf("\n|");

        if(is_ringbuff_empty(rb))
            printf("%4d:       | ", i);
        else if(
            (rb->wr_point > rb->rd_point && i >= rb->wr_point) ||
            (rb->wr_point > rb->rd_point && i < rb->rd_point) ||
            (rb->wr_point < rb->rd_point && i < rb->rd_point && i >= rb->wr_point )
            )
            printf("%4d:       | ", i);
        else
            printf("%4d: - %c - | ", i, rb->buffer[i] == '\0' ? '*' : rb->buffer[i]);
    }
    printf("\n");
}

void ringbuff_test() {
    Ringbuffer rb;
    ringbuff_init(&rb, 30);
    char b[100] = { 0 };

    printf("init, left:%d\n", ringbuff_free_size(&rb));
    ringbuff_dump(&rb);

    printf("write:%d, ", ringbuff_write(&rb, "tdr", 3));
    printf("left:%d\n", ringbuff_free_size(&rb));
    ringbuff_dump(&rb);

    printf("write:%d, ", ringbuff_write(&rb, "aw24sdfsdfd", 10));
    printf("left:%d\n", ringbuff_free_size(&rb));
    ringbuff_dump(&rb);

    printf("read:%d, (%s), ", ringbuff_read(&rb, b, 20), b);
    printf("left:%d\n", ringbuff_free_size(&rb));
    ringbuff_dump(&rb);

    printf("write:%d, ", ringbuff_write(&rb, "aw24sdfsdfd", 10));
    printf("left:%d\n", ringbuff_free_size(&rb));
    ringbuff_dump(&rb);

    printf("write:%d, ", ringbuff_write(&rb, "t546t&^234sfsdfd", sizeof("t546t&^234sfsdfd")));
    printf("left:%d\n", ringbuff_free_size(&rb));
    ringbuff_dump(&rb);

    printf("ringbuff_data_size= \t%d\n", ringbuff_data_size(&rb));
    printf("ringbuff_free_size= \t%d\n", ringbuff_free_size(&rb));
    printf("is_ringbuff_full= \t%d\n", is_ringbuff_full(&rb));
    printf("is_ringbuff_empty= \t%d\n", is_ringbuff_empty(&rb));
    printf("wr_point= \t\t%d\n", rb.wr_point);
    printf("rd_point= \t\t%d\n", rb.rd_point);
    
}