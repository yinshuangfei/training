/**
 * @file reverse_list.c
 * @author Alan Yin (yinshuangfei007@163.com)
 * @brief
 * @version 0.0.1
 * @date 2025-05-02
 *
 * @copyright Copyright (c) 2025
 *
 * @node: 链表翻转
 */

#include "comm.h"

struct Node *reverse(struct Node *head)
{
	if (head == NULL)
		return NULL;

#if defined(M1)
	printf("M1\n");
	/**
	 * @brief 方法1：
	 * pre  <-  pos  <-  next
	 * NULL <-  [0]  <-  [1]
	 */
	struct Node *pre = NULL;
	struct Node *pos = head;
	struct Node *next = NULL;

	while (pos != NULL) {
		next = pos->next;
		pos->next = pre;
		pre = pos;
		pos = next;
	}

	return pre;

#elif defined(M2)
	printf("M2\n");
	/**
	 * @brief 方法2：
	 * pre  <-  pos
	 *          curr
	 * NULL <-  [0]   <-  [1]
	 */
	struct Node *pre = NULL;
	struct Node *curr = NULL;
	struct Node *pos = head;

	while (pos != NULL) {
		curr = pos;
		pos = pos->next;
		curr->next = pre;
		pre = curr;
	}

	return pre;

#else
	printf("M3\n");
	/**
	 * @brief 方法3：
	 * pre  <-  curr  <-  next
	 * [0]  <-  [1]   <-  [2]
	 * start
	 * 下一个位置用 curr 表示，有值才进入
	 */
	node_t *curr = head->next;
	node_t *next = NULL;
	node_t *pre = head;

	pre->next = NULL;

	while (curr != NULL) {
		next = curr->next;
		curr->next = pre;
		pre = curr;
		curr = next;
	}

	return pre;
#endif
}

int main(void)
{
	struct Node *tmp, *head, *next = NULL;

	/** 倒序初始化 */
	for (int i = 0; i < 6; i++) {
		tmp = calloc(1, sizeof(struct Node *));
		if (NULL == tmp)
			return -1;

		tmp->val = i;
		tmp->next = next;
		next = tmp;
	}
	head = next;

	/** 数组初始化
	int data[] = {4, 2, 1, 6, 9};
	int num = sizeof(data) / sizeof(int);

	head = init_link_list(data, num);
	if (NULL == head) {
		return -1;
	}
	*/

	tmp = head;
	pr_linked_list("org", tmp);

	tmp = reverse(head);
	pr_linked_list("new", tmp);

	return 0;
}
