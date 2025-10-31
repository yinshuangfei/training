/**
 * @file 19_stack_implemented_queue.c
 * @author Alan Yin (yinshuangfei007@163.com)
 * @brief
 * @version 0.0.1
 * @date 2025-05-14
 *
 * @copyright Copyright (c) 2025
 *
 * @node: 用栈实现队列
 * Google、微软等大厂常考的经典面试题，考察对数据结构的理解和编码能力。
 *
 * 思路：push操作直接压入输入栈，pop/peek时若输出栈为空则将输入栈元素全部转移。
 * 时间复杂度分析：push为O(1)，pop/peek均摊O(1)。
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#define INITIAL_CAPACITY 4
#define GROWTH_FACTOR 2

// 动态栈结构
typedef struct {
	int *data;
	int top;
	int capacity;
} Stack;

// 初始化栈
void stackInit(Stack *s)
{
	s->data = (int *)malloc(INITIAL_CAPACITY * sizeof(int));
	s->top = -1;
	s->capacity = INITIAL_CAPACITY;
}

// 栈是否为空
bool stackEmpty(Stack *s)
{
	return s->top == -1;
}

// 栈扩容
void stackResize(Stack *s)
{
	s->capacity *= GROWTH_FACTOR;
	s->data = (int *)realloc(s->data, s->capacity * sizeof(int));
	if (!s->data) {
		perror("Stack resize failed");
		exit(EXIT_FAILURE);
	}
}

// 入栈
void stackPush(Stack *s, int val)
{
	if (s->top == s->capacity - 1) {
		stackResize(s);
	}
	s->data[++s->top] = val;
}

// 出栈
int stackPop(Stack *s)
{
	if (stackEmpty(s)) {
		fprintf(stderr, "Stack underflow\n");
		exit(EXIT_FAILURE);
	}
	return s->data[s->top--];
}

// 查看栈顶
int stackPeek(Stack *s)
{
	if (stackEmpty(s)) {
		fprintf(stderr, "Stack is empty\n");
		exit(EXIT_FAILURE);
	}
	return s->data[s->top];
}

// 释放栈
void stackFree(Stack *s)
{
	free(s->data);
}

// 队列结构
typedef struct {
	Stack in;
	Stack out;
} MyQueue;

// 创建队列
MyQueue* myQueueCreate()
{
	MyQueue *q = (MyQueue *)malloc(sizeof(MyQueue));
	stackInit(&q->in);
	stackInit(&q->out);
	return q;
}

// 入队
void myQueuePush(MyQueue* obj, int x)
{
	stackPush(&obj->in, x);
}

// 转移元素从输入栈到输出栈
void transfer(MyQueue *obj)
{
	while (!stackEmpty(&obj->in)) {
		stackPush(&obj->out, stackPop(&obj->in));
	}
}

// 出队
int myQueuePop(MyQueue* obj)
{
	if (stackEmpty(&obj->out)) {
		transfer(obj);
	}
	return stackPop(&obj->out);
}

// 查看队首
int myQueuePeek(MyQueue* obj)
{
	if (stackEmpty(&obj->out)) {
		transfer(obj);
	}
	return stackPeek(&obj->out);
}

// 队列是否为空
bool myQueueEmpty(MyQueue* obj)
{
	return stackEmpty(&obj->in) && stackEmpty(&obj->out);
}

// 释放队列
void myQueueFree(MyQueue* obj)
{
	stackFree(&obj->in);
	stackFree(&obj->out);
	free(obj);
}

// 测试用例
int main()
{
	MyQueue *q = myQueueCreate();

	// 测试基本功能
	myQueuePush(q, 1);
	myQueuePush(q, 2);
	printf("Peek: %d\n", myQueuePeek(q));  // 1
	printf("Pop: %d\n", myQueuePop(q));    // 1
	printf("Empty: %s\n", myQueueEmpty(q) ? "true" : "false"); // false

	// 测试边界条件
	myQueuePush(q, 3);
	printf("Pop: %d\n", myQueuePop(q));    // 2
	printf("Pop: %d\n", myQueuePop(q));    // 3
	printf("Empty: %s\n", myQueueEmpty(q) ? "true" : "false"); // true

	// 测试大量数据
	for (int i = 0; i < 1000; i++) {
		myQueuePush(q, i);
	}

	for (int i = 0; i < 1000; i++) {
		if (myQueuePop(q) != i) {
			printf("Test failed at %d\n", i);
			break;
		}
	}

	printf("Massive data test passed\n");

	myQueueFree(q);
	return 0;
}
