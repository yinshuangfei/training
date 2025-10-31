/**
 * @file 24_queue_implemented_stack.c
 * @author Alan Yin (yinshuangfei007@163.com)
 * @brief
 * @version 0.0.1
 * @date 2025-07-11
 *
 * @copyright Copyright (c) 2025
 *
 * @node: 用队列实现栈
 * 请你仅使用两个队列实现一个后入先出（LIFO）的栈，并支持普通栈的全部四种操作
 * 	（push、top、pop 和 empty）。
 *
 * 思路：push操作直接入队q1，pop和top操作时将q1中除最后一个元素外的所有元素转移到q2，
 *      然后处理最后一个元素，最后交换 q1 和 q2 的引用。
 */

#include <stdio.h>
#include <stdlib.h>

#define MAX_SIZE 100

// typedef struct _queue {

// } queue_t;

// typedef struct _stack {
// 	queue_t *q1;
// 	queue_t *q2;
// } stack_t;

typedef struct {
    int data[MAX_SIZE];
    int front;
    int rear;
} Queue;

void initQueue(Queue *q) {
    q->front = q->rear = -1;
}

int isEmpty(Queue *q) {
    return q->front == -1;
}

void enqueue(Queue *q, int value) {
    if (q->rear == MAX_SIZE - 1) {
        printf("Queue is full\n");
        return;
    }

    if (isEmpty(q)) {
        q->front = q->rear = 0;
    } else {
        q->rear++;
    }

    q->data[q->rear] = value;
}

int dequeue(Queue *q) {
    if (isEmpty(q)) {
        printf("Queue is empty\n");
        return -1;
    }

    int value = q->data[q->front];
    if (q->front == q->rear) {
        q->front = q->rear = -1;
    } else {
        q->front++;
    }

    return value;
}

typedef struct {
    Queue q1;
    Queue q2;
} Stack;

void push(Stack *s, int value) {
    enqueue(&s->q1, value);
}

int pop(Stack *s) {
    if (isEmpty(&s->q1)) {
        printf("Stack is empty\n");
        return -1;
    }

    while (s->q1.front != s->q1.rear) {
        enqueue(&s->q2, dequeue(&s->q1));
    }

    int value = dequeue(&s->q1);

    Queue temp = s->q1;
    s->q1 = s->q2;
    s->q2 = temp;

    return value;
}

int main() {
    Stack s;
    initQueue(&s.q1);
    initQueue(&s.q2);

    push(&s, 1);
    push(&s, 2);
    push(&s, 3);

    printf("%d\n", pop(&s)); // 3
    printf("%d\n", pop(&s)); // 2
    printf("%d\n", pop(&s)); // 1
    printf("%d\n", pop(&s)); // Stack is empty

    return 0;
}
