/**
 * @file 26_valid_parentheses.c
 * @author Alan Yin (yinshuangfei007@163.com)
 * @brief
 * @version 0.0.1
 * @date 2026-05-09
 *
 * @copyright Copyright (c) 2026
 *
 * @node: 有效的括号
 *
 * - 遍历字符串：从左到右依次查看每个字符。
 * - 遇到左括号（(, {, [）：直接把它压入栈中，等待匹配。
 * - 遇到右括号（), }, ]）：
 * 	检查栈是否为空。如果栈为空，说明前面没有左括号来和它匹配，直接判定为无效。
 * 	如果栈不为空，取出栈顶的左括号，检查它是否与当前的右括号是同一类型。如果不匹配，
 * 	判定为无效。
 * - 遍历结束：检查栈是否为空。如果栈为空，说明所有左括号都完美匹配并消除了；
 * 	如果栈里还有剩余，说明有左括号没闭合，判定为无效。
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

// 1. 定义栈结构体
typedef struct {
	char *data;       // 栈的数据数组
	int top;          // 栈顶指针
	int capacity;     // 栈的容量
} Stack;

// 2. 栈的基本操作函数
// 初始化栈
Stack *createStack(int capacity)
{
	Stack *stack = (Stack*)malloc(sizeof(Stack));
	stack->data = (char*)malloc(sizeof(char) * capacity);
	stack->top = -1;
	stack->capacity = capacity;
	return stack;
}

// 判断栈是否为空
bool isEmpty(Stack* stack)
{
	return stack->top == -1;
}

// 入栈
void push(Stack* stack, char c)
{
	stack->data[++stack->top] = c;
}

// 出栈
char pop(Stack* stack)
{
	return stack->data[stack->top--];
}

// 获取栈顶元素
char peek(Stack* stack)
{
	return stack->data[stack->top];
}

// 销毁栈，释放内存
void destroyStack(Stack* stack)
{
	free(stack->data);
	free(stack);
}

// 3. 判断括号是否有效的核心逻辑
bool isValid(char* s)
{
	int len = strlen(s);
	// 如果字符串长度为奇数，肯定无法完全匹配，直接返回 false
	if (len % 2 != 0) {
		return false;
	}

	Stack *stack = createStack(len);

	for (int i = 0; i < len; i++) {
		char c = s[i];
		// 遇到左括号，直接入栈
		if (c == '(' || c == '{' || c == '[') {
			push(stack, c);
		} else {
			// 遇到右括号，先检查栈是否为空
			if (isEmpty(stack)) {
				destroyStack(stack);
				return false;
			}

			// 取出栈顶的左括号进行匹配校验
			char top = peek(stack);
			if ((c == ')' && top != '(') ||
			    (c == '}' && top != '{') ||
			    (c == ']' && top != '[')) {
				destroyStack(stack);
				return false;
			}

			// 匹配成功，弹出栈顶元素
			pop(stack);
		}
	}

	// 遍历结束后，栈为空说明所有括号都完美闭合
	bool result = isEmpty(stack);
	destroyStack(stack);
	return result;
}

// 4. 测试代码
int main()
{
	char* test1 = "()";
	char* test2 = "()[]{}";
	char* test3 = "(]";
	char* test4 = "([)]";
	char* test5 = "{[]}";
	char* test6 = "{[()[][][]{(){}}]}";

	printf("%s is valid: %s\n", test1, isValid(test1) ? "true" : "false");
	printf("%s is valid: %s\n", test2, isValid(test2) ? "true" : "false");
	printf("%s is valid: %s\n", test3, isValid(test3) ? "true" : "false");
	printf("%s is valid: %s\n", test4, isValid(test4) ? "true" : "false");
	printf("%s is valid: %s\n", test5, isValid(test5) ? "true" : "false");
	printf("%s is valid: %s\n", test6, isValid(test6) ? "true" : "false");

	return 0;
}