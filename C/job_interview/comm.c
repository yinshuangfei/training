/**
 * @file comm.c
 * @author Alan Yin (yinshuangfei007@163.com)
 * @brief
 * @version 0.0.1
 * @date 2025-05-02
 *
 * @copyright Copyright (c) 2025
 *
 * @node:
 */

#include "comm.h" 

/** init */
struct Node *init_linked_list(int data[], int num)
{
	if (num < 1)
		return NULL;

	int i;
	struct Node *tmp;
	struct Node *head = calloc(num, sizeof(struct Node));
	tmp = head;

	for (i = 0; i < num - 1; i++) {
		tmp->val = data[i];
		tmp->next = tmp + 1;
		tmp = tmp + 1;
	}

	tmp->val = data[num - 1];
	tmp->next = NULL;

	return head;
}

/** print */
void pr_linked_list(char *tag, struct Node *tmp)
{
	while (tmp != NULL) {
		printf("%s val = %d\n", tag == NULL ? "null": tag, tmp->val);
		tmp = tmp->next;
	}
}