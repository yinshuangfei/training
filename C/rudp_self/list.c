#include "list.h"


void 
list_init(RDT_list * list) {
    list->head.next = & list->head;
    list->head.prev = & list->head;
}
/**
 * @brief data 插入至 position 的前面
 * 
 */
RDT_list_iterator *
list_insert(RDT_list_iterator *position, void *data) {
    RDT_list_iterator *result = (RDT_list_iterator *) data;
    result->prev = position->prev;
    result->next = position;

    position->prev->next = result;
    position->prev = result;

    return result;
}

void * 
list_remove(RDT_list_iterator *position) {
    position->next->prev = position->prev;
    position->prev->next = position->next;

    return position;
}

/* 将first至last的片段移动至position的前面 */
RDT_list_iterator *
list_move(RDT_list_iterator *position, void * data_first, void * data_last) {
    RDT_list_iterator *first = (RDT_list_iterator *) data_first,
                      *last = (RDT_list_iterator *) last;
    
    first->prev->next = last->next;
    last->next->prev = first->prev;

    first->prev = position->prev;
    last->next = position;

    first->prev->next = first;
    position->prev = last;

    return first;
}

size_t
list_size(RDT_list * list) {
    size_t size = 0;
    RDT_list_iterator *position;

    for (position = list_begin(list); 
         position != list_end(list);
         position = list_next(position)) {
        ++ size;
    }

    return size;
}
