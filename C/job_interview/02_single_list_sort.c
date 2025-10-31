/**
 * @file single_list_sort.c
 * @author Alan Yin (yinshuangfei007@163.com)
 * @brief
 * @version 0.0.1
 * @date 2025-05-02
 *
 * @copyright Copyright (c) 2025
 *
 * @node_t: 单链表排序, 归并排序
 */

#include "comm.h"

/**
 * @brief 合并两个有序链表, 递归式
 *
 * @param [in] head1
 * @param [in] head2
 * @return node_t* result
 */
node_t *merge_by_recursive(node_t *head1, node_t *head2)
{
	if (!head1)
		return head2;

	if (!head2)
		return head1;

	node_t *result = NULL;

	if (head1->val <= head2->val) {
		result = head1;
		result->next = merge_by_recursive(head1->next, head2);
	} else {
		result = head2;
		result->next = merge_by_recursive(head1, head2->next);
	}
	return result;
}

/**
 * @brief 合并两个有序链表, 循环式
 *
 * @param [in] l1
 * @param [in] l2
 * @return node_t* result
 */
node_t *merge_by_loop(node_t *l1, node_t *l2)
{
	node_t dummy;				/** 虚拟头节点 */
	node_t *tail = &dummy;

	while (l1 && l2) {
		if (l1->val <= l2->val) {
			tail->next = l1;
			l1 = l1->next;
		} else {
			tail->next = l2;
			l2 = l2->next;
		}
		tail = tail->next;
	}

	tail->next = l1 ? l1 : l2;		/** 连接剩余部分 */
	return dummy.next;
}

/**
 * @brief 归并排序主函数
 *
 * @param [in] head
 * @return node_t* result
 */
node_t *merge_sort(node_t *head)
{
	/** 递归终止条件 */
	if (!head || !head->next)
		return head;

	/**
	 * 快慢指针找中点
	 * 慢指针：每次步进 1；
	 * 快指针：每次步进 2；
	 *  */
	node_t *slow = head, *fast = head->next;
	while (fast && fast->next) {
		slow = slow->next;
		fast = fast->next->next;
	}

	node_t *mid = slow->next;		/** 中点 */
	slow->next = NULL;			/** 断开链表 */

	node_t *left = merge_sort(head);	/** 递归排序左半部分 */
	node_t *right = merge_sort(mid);	/** 递归排序右半部分 */

	return merge_by_recursive(left, right);	/** 合并 */
}

int main(void)
{
	struct Node *tmp, *head, *next = NULL;

	int data[] = {4, 2, 1, 6, 9};
	int num = sizeof(data) / sizeof(int);

	head = init_linked_list(data, num);
	if (NULL == head) {
		return -1;
	}

	tmp = head;
	pr_linked_list("org", tmp);

	tmp = merge_sort(head);
	pr_linked_list("new", tmp);

	return 0;
}