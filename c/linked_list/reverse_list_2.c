#include <stdio.h>
#include <stdlib.h>

typedef struct _link {
    int x;
    struct _link * next;
} link;

/** init */
link * init_link_list(int data[], int num){
    if (num < 1) return NULL;

    int i;
    link * tmp;
    link * head = calloc(num, sizeof(link *));
    tmp = head;

    for(i = 0; i < num - 1; i++ ){
        tmp->x = data[i];
        tmp->next = tmp + 1;
        tmp = tmp + 1;
    }
    tmp->x = data[num-1];
    tmp->next = NULL;

    return head;
}

/** print */
void p_link(link * l){
    int i = 0;
    while(l){
        printf("index=%d,p=%p, x=%d, next=%p \n", i++, l, l->x, l->next);
        l = l->next;
    }
    printf("\n");
}

/** 链表反转
    *     *     *
  curr    p    next
    |     |     |
    |     |    p节点下一个节点的缓存值
    |   存储curr的next值
  存储当前节点
*/
link * reverse(link * head){
    if (head->next == NULL)
        return head;
        
    link * curr = head;
    link * p = head->next;
    link * next = NULL;

    curr->next = NULL;
    
    while(p){
        next = p->next;
        p->next = curr;
        curr = p;
        p = next;
    }

    return curr;
}

int main(){
    printf("start ... \n");
    int data[] = {4, 2, 1, 6, 9};
    int num = sizeof(data) / sizeof(int);

    link * tmp, *head;

    head = init_link_list(data, num);

    tmp = head;
    p_link(tmp);

    tmp = reverse(head);
    p_link(tmp);

    return 0;
}