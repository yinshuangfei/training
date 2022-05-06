#include<stdio.h>
#include<stdlib.h>

struct Node {
    int val ;
    struct Node * next ;
};


struct Node * reverse( struct Node * head ){
    if ( head == NULL)
        return NULL;

    struct Node * pre = NULL;
    struct Node * cur = NULL;
    struct Node * p = head;

    while( p != NULL ){
        cur = p -> next;
        p->next = pre;
        pre = p;
        p = cur;

    }
    return pre;
}

void main( ) {
    struct Node * node1 = malloc( sizeof( struct Node *) );
    struct Node * node2 = malloc( sizeof( struct Node *) );
    struct Node * node3 = malloc( sizeof( struct Node *) );
    struct Node * node4 = malloc( sizeof( struct Node *) );
    struct Node * node5 = malloc( sizeof( struct Node *) );

    node1 -> val = 1;
    node2 -> val = 2;
    node3 -> val = 3;
    node4 -> val = 4;
    node5 -> val = 5;

    node1 -> next = node2;
    node2 -> next = node3;
    node3 -> next = node4;
    node4 -> next = node5;
    node5 -> next = NULL;

    struct Node * tmp = node1;
    while(tmp != NULL){
        printf("val = %d\n",tmp -> val);
        tmp = tmp -> next;
    }

    tmp = reverse(node1);

    while(tmp != NULL){
        printf("val = %d\n",tmp -> val);
        tmp = tmp -> next;
    }

    printf("end\n"); 

}
