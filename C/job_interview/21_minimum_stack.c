/**
 * @file 20_minimum_stack.c
 * @author Alan Yin (yinshuangfei007@163.com)
 * @brief
 * @version 0.0.1
 * @date 2025-05-14
 *
 * @copyright Copyright (c) 2025
 *
 * @node: 最小栈
 *
 * 最小栈是要求能在 O(1) 时间内获取栈中最小元素的特殊栈结构，这是Google、Facebook
 * 等大厂常见的面试题。
 */

#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <stdbool.h>

#define INIT_CAPACITY 4
#define GROWTH_FACTOR 2

typedef struct {
	int *data;
	int top;
	int capacity;
} Stack;

// 初始化栈
void stackInit(Stack *s)
{
	s->data = (int *)malloc(INIT_CAPACITY * sizeof(int));
	s->top = -1;
	s->capacity = INIT_CAPACITY;
}

// 判断栈是否为空
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

// 最小栈结构
typedef struct {
	Stack data_stack;
	Stack min_stack;
} MinStack;

// 创建最小栈
MinStack* minStackCreate()
{
	MinStack *obj = (MinStack *)malloc(sizeof(MinStack));
	stackInit(&obj->data_stack);
	stackInit(&obj->min_stack);
	return obj;
}

// 入栈
void minStackPush(MinStack* obj, int val)
{
	stackPush(&obj->data_stack, val);

	// 更新最小栈
	if (stackEmpty(&obj->min_stack) ||
	    val <= stackPeek(&obj->min_stack)) {
		stackPush(&obj->min_stack, val);
	}
}

// 出栈
void minStackPop(MinStack* obj)
{
	if (stackEmpty(&obj->data_stack))
		return;

	int val = stackPop(&obj->data_stack);

	// 如果弹出的是当前最小值，则同时弹出最小栈
	if (val == stackPeek(&obj->min_stack)) {
		stackPop(&obj->min_stack);
	}
}

// 获取栈顶元素
int minStackTop(MinStack* obj)
{
	return stackPeek(&obj->data_stack);
}

// 获取最小值
int minStackGetMin(MinStack* obj)
{
	return stackPeek(&obj->min_stack);
}

// 释放最小栈
void minStackFree(MinStack* obj)
{
	stackFree(&obj->data_stack);
	stackFree(&obj->min_stack);
	free(obj);
}

// 测试用例
int main()
{
	MinStack* obj = minStackCreate();

	minStackPush(obj, -2);
	minStackPush(obj, 0);
	minStackPush(obj, -3);

	printf("Current min: %d\n", minStackGetMin(obj)); // -3
	minStackPop(obj);
	printf("Top element: %d\n", minStackTop(obj));     // 0
	printf("Current min: %d\n", minStackGetMin(obj));  // -2

	minStackPush(obj, -5);
	printf("Current min: %d\n", minStackGetMin(obj));  // -5
	minStackPop(obj);
	printf("Current min: %d\n", minStackGetMin(obj));  // -2

	minStackFree(obj);
	return 0;
}
