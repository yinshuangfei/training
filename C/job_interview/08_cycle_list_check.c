/**
 * @file 08_cycle_list_check.c
 * @author Alan Yin (yinshuangfei007@163.com)
 * @brief
 * @version 0.0.1
 * @date 2025-05-07
 *
 * @copyright Copyright (c) 2025
 *
 * @node: 检测链表是否包含环的完整代码示例，采用快慢指针法（Floyd判圈算法）
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

// 链表节点结构体
typedef struct ListNode {
	int data;
	struct ListNode* next;
} ListNode;

// 创建新节点
ListNode* createNode(int data)
{
	ListNode* newNode = (ListNode*)malloc(sizeof(ListNode));
	if (newNode == NULL) {
		printf("内存分配失败\n");
		exit(1);
	}
	newNode->data = data;
	newNode->next = NULL;
	return newNode;
}

// 检测链表是否有环
bool hasCycle(ListNode* head)
{
	if (head == NULL || head->next == NULL) {
		return false;
	}

	ListNode* slow = head;      // 慢指针，每次走一步
	ListNode* fast = head->next; // 快指针，每次走两步

	while (slow != fast) {

		if (fast == NULL || fast->next == NULL) {
			return false; // 快指针到达链表末尾，无环
		}
		slow = slow->next;
		fast = fast->next->next;
	}

	return true; // 快慢指针相遇，有环
}

// 释放链表内存
void freeList(ListNode* head) {
	ListNode* current = head;
	while (current != NULL) {
		ListNode* temp = current;
		current = current->next;
		free(temp);
	}
}

int main()
{
	// 创建一个有环的链表：1->2->3->4->5->2(指向节点2形成环)
	ListNode* head = createNode(1);
	head->next = createNode(2);
	head->next->next = createNode(3);
	head->next->next->next = createNode(4);
	head->next->next->next->next = createNode(5);
	head->next->next->next->next->next = head->next; // 形成环

	if (hasCycle(head)) {
		printf("链表有环\n");
	} else {
		printf("链表无环\n");
	}

	// 注意：由于链表有环，不能直接调用freeList，否则会无限循环
	// 实际应用中需要先断开环再释放内存

	// 手动释放示例链表的内存（先断开环）
	head->next->next->next->next->next = NULL;
	freeList(head);

	return 0;
}
