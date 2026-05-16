/**
 * @file 27_minimum_stack2.c
 * @author Alan Yin (yinshuangfei007@163.com)
 * @brief
 * @version 0.0.1
 * @date 2026-05-13
 *
 * @copyright Copyright (c) 2026
 *
 * @node:
 */

#include <stdio.h>
#include <stdlib.h>

#define MAX_SIZE 30000
// 定义一个求较小值的宏
#define MIN(a, b) ((a) < (b) ? (a) : (b))

typedef struct {
    int *data;      // 数据栈：存放正常元素
    int *min_data;  // 辅助栈：存放当前栈内的最小元素
    int top;        // 栈顶指针（同时控制 data 和 min_data）
} MinStack;

// 初始化栈结构
MinStack* minStackCreate() {
    MinStack* obj = (MinStack*)malloc(sizeof(MinStack));
    // 为数据栈和辅助栈分配空间
    obj->data = (int*)malloc(sizeof(int) * MAX_SIZE);
    obj->min_data = (int*)malloc(sizeof(int) * MAX_SIZE);
    obj->top = 0; // 初始栈顶为 0
    return obj;
}

// 压栈操作
void minStackPush(MinStack* obj, int val) {
    // 1. 数据栈正常压入
    obj->data[obj->top] = val;

    // 2. 辅助栈压入“当前值”和“上一个最小值”中的较小者
    if (obj->top == 0) {
        // 如果是第一个元素，直接压入
        obj->min_data[obj->top] = val;
    } else {
        // 否则，取较小者压入，保证 min_data 栈顶始终是当前最小值
        obj->min_data[obj->top] = MIN(obj->min_data[obj->top - 1], val);
    }
    obj->top++;
}

// 弹栈操作
void minStackPop(MinStack* obj) {
    if (obj->top > 0) {
        // 数据栈和辅助栈同步弹出（只需移动栈顶指针）
        obj->top--;
    }
}

// 获取栈顶元素
int minStackTop(MinStack* obj) {
    // 注意：因为 push 后 top 已经自增，所以栈顶元素下标是 top - 1
    return obj->data[obj->top - 1];
}

// 获取最小元素
int minStackGetMin(MinStack* obj) {
    // 直接返回辅助栈的栈顶
    return obj->min_data[obj->top - 1];
}

// 释放内存
void minStackFree(MinStack* obj) {
    free(obj->data);
    free(obj->min_data);
    free(obj);
}

// --- 测试 Main 函数 ---
int main() {
    MinStack* obj = minStackCreate();

    minStackPush(obj, -2);
    minStackPush(obj, 0);
    minStackPush(obj, -3);

    printf("getMin: %d\n", minStackGetMin(obj)); // 返回 -3
    minStackPop(obj);
    printf("top: %d\n", minStackTop(obj));       // 返回 0
    printf("getMin: %d\n", minStackGetMin(obj)); // 返回 -2

    minStackFree(obj);
    return 0;
}