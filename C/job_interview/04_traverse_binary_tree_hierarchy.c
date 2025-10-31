/**
 * @file 04_traverse_binary_tree_hierarchy.c
 * @author Alan Yin (yinshuangfei007@163.com)
 * @brief
 * @version 0.0.1
 * @date 2025-05-02
 *
 * @copyright Copyright (c) 2025
 *
 * @node: 二叉树的层次遍历
 * 包含‌队列辅助的 BFS 实现‌和‌递归实现‌两种方法。
 *
 * 队列实现（BFS，Breadth-First Search，宽度优先搜索，推荐方法）‌
 * ‌- 时间复杂度：O(n)
 * - 空间复杂度：O(n)
 *
 * 递归实现（DFS，Depth-First Search，深度优先搜索，按层打印）‌
 * -‌ 时间复杂度：O(n²)
 * - 空间复杂度：O(n)（递归栈）
 */

#include <stdio.h>
#include <stdlib.h>

/** 定义二叉树节点结构体 */
typedef struct TreeNode {
	int val;
	struct TreeNode *left;
	struct TreeNode *right;
} tree_node_t;

/** 定义队列节点（用于BFS） */
typedef struct QueueNode {
	tree_node_t *treeNode;	// 存储二叉树节点
	struct QueueNode *next;	// 队列指针
} queue_node_t;

/** 定义队列结构 */
typedef struct Queue {
	struct QueueNode *front;	// 头
	struct QueueNode *rear;		// 尾
} queue_t;

/**
 * @brief 初始化队列
 *
 * @return queue_t* queue
 */
queue_t *create_queue()
{
	queue_t *q = (queue_t*)malloc(sizeof(queue_t));
	if (NULL == q) {
		return NULL;
	}

	q->front = NULL;
	q->rear = NULL;

	return q;
}

/**
 * @brief 入队
 *
 * @param [in] q
 * @param [in] treeNode
 */
void enqueue(queue_t *q, tree_node_t *treeNode)
{
	queue_node_t *newNode = (queue_node_t*)malloc(sizeof(queue_node_t));
	newNode->treeNode = treeNode;
	newNode->next = NULL;

	if (q->rear == NULL) {
		q->front = newNode;
		q->rear = newNode;
	} else {
		q->rear->next = newNode;
		q->rear = newNode;
	}
}

/**
 * @brief 出队
 *
 * @param [in] q
 * @return tree_node_t*
 */
tree_node_t *dequeue(queue_t *q)
{
	if (q->front == NULL)
		return NULL;

	queue_node_t *temp = q->front;
	tree_node_t *node = temp->treeNode;

	q->front = q->front->next;

	if (q->front == NULL) {
		q->rear = NULL;
	}

	free(temp);
	return node;
}

/**
 * @brief 判断队列是否为空
 *
 * @param [in] q
 * @return int
 */
int isQueueEmpty(queue_t *q)
{
	return q->front == NULL;
}

/**
 * @brief 层次遍历（BFS）
 *
 * 队列实现（BFS，Breadth-First Search，宽度优先搜索，推荐方法）‌
 *
 * @param [in] root
 */
void level_order_traversal(tree_node_t *root)
{
	if (root == NULL)
		return;

	queue_t *q = create_queue();
	if (q == NULL)
		return;

	enqueue(q, root);

	while (!isQueueEmpty(q)) {
		int levelSize = 0;
		queue_node_t *temp = q->front;

		/**
		 * 计算当前层的节点数（可选，用于格式化输出）
		 * */
		while (temp != NULL) {
			levelSize++;
			temp = temp->next;
		}

		/** 按数量出队 */
		for (int i = 0; i < levelSize; i++) {
			tree_node_t *current = dequeue(q);
			printf("%d ", current->val);

			if (current->left != NULL) {
				enqueue(q, current->left);
			}
			if (current->right != NULL) {
				enqueue(q, current->right);
			}
		}

		/** 换行表示一层结束 */
		printf("\n");
	}

	/** 释放队列内存 */
	free(q);
}

/**
 * @brief 创建新节点
 *
 * @param [in] val
 * @return tree_node_t*
 */
tree_node_t *createNode(int val)
{
	tree_node_t *node = (tree_node_t *)malloc(sizeof(tree_node_t));
	node->val = val;
	node->left = NULL;
	node->right = NULL;
	return node;
}

/**
 * @brief 获取二叉树高度
 *
 * @param [in] root
 * @return int 0: success; <0: failed, posix errno;
 */
int getHeight(tree_node_t *root)
{
	if (root == NULL)
		return 0;

	int leftHeight = getHeight(root->left);
	int rightHeight = getHeight(root->right);

	return (leftHeight > rightHeight) ? leftHeight + 1 : rightHeight + 1;
}

/**
 * @brief 打印指定层的节点
 *
 * @param [in] root
 * @param [in] level
 */
void printGivenLevel(tree_node_t *root, int level)
{
	if (root == NULL)
		return;

	if (level == 1) {
		printf("%d ", root->val);
	} else if (level > 1) {
		printGivenLevel(root->left, level - 1);
		printGivenLevel(root->right, level - 1);
	}
}

/**
 * @brief 层次遍历（DFS）
 *
 * 递归实现（DFS，Depth-First Search，深度优先搜索，按层打印）‌
 *
 * @param [in] root
 */
void level_order_recursive(tree_node_t *root)
{
	int h = getHeight(root);

	for (int i = 1; i <= h; i++) {
		printGivenLevel(root, i);
		printf("\n");  // 换行表示一层结束
	}
}

int main(void)
{
	/* 构造测试二叉树：
	         1
	        / \
	       2   3
	      / \   \
	     4   5   6
	              \
	               9
	*/
	tree_node_t *root = createNode(1);
	root->left = createNode(2);
	root->right = createNode(3);
	root->left->left = createNode(4);
	root->left->right = createNode(5);
	root->right->right = createNode(6);
	root->right->right->right = createNode(9);

	printf("BFS层次遍历结果:\n");
	level_order_traversal(root);

	printf("\n递归层次遍历结果:\n");
	level_order_recursive(root);

	return 0;
}
