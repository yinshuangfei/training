#ifndef _HXJ_LIST_H_
#define _HXJ_LIST_H_

#include <stdlib.h>

typedef struct _list_node {
    struct _list_node *next;
    struct _list_node *prev;
} list_node;

typedef list_node RDT_list_iterator;

typedef struct _rdt_list {
    list_node head;
} rdt_list;

typedef rdt_list RDT_list;

#define list_begin(list) ((list) -> head.next)
#define list_end(list) (& (list) -> head)
#define list_head(list) (& (list) -> head)

#define list_empty(list) (list_begin (list) == list_end (list))

#define list_next(iterator) ((iterator) -> next)
#define list_prev(iterator) ((iterator) -> prev)

#define list_front(list) ((void *) (list) -> head.next)
#define list_back(list) ((void *) (list) -> head.prev)

void 
list_init(RDT_list * list);

RDT_list_iterator *
list_insert(RDT_list_iterator *position, void *data);

void * 
list_remove(RDT_list_iterator *position);

RDT_list_iterator *
list_move(RDT_list_iterator *position, void * data_first, void * data_last);

size_t
list_size(RDT_list * list);

#endif /* _HXJ_LIST_H_ */