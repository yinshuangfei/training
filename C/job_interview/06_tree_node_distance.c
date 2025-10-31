/**
 * @file 06_tree_node_distance.c
 * @author Alan Yin (yinshuangfei007@163.com)
 * @brief
 * @version 0.0.1
 * @date 2025-05-05
 *
 * @copyright Copyright (c) 2025
 *
 * @node: 二叉树中相距最远的两个节点距离（二叉树的直径）
 *
 * 在二叉树中，相距最远的两个节点之间的距离被称为二叉树的直径。这个距离可以通过计算每个
 * 节点的左右子树高度之和来得到。
 *
 */

#include <stdio.h>
#include <stdlib.h>

/** 定义二叉树节点结构体 */
typedef struct TreeNode {
	int val;
	struct TreeNode *left;
	struct TreeNode *right;
} tree_node_t;


int max(int a, int b)
{
	return a > b ? a : b;
}

int diameterOfBinaryTreeHelper(struct TreeNode *root, int *maxDiameter)
{
	if (root == NULL) {
		return 0;
	}

	/** 递归计算左右子树的高度 */
	int leftHeight = diameterOfBinaryTreeHelper(root->left, maxDiameter);
	int rightHeight = diameterOfBinaryTreeHelper(root->right, maxDiameter);

	/** 更新最大直径（左右子树高度之和） */
	*maxDiameter = max(*maxDiameter, leftHeight + rightHeight);

	/** 返回当前子树的高度 */
	return max(leftHeight, rightHeight) + 1;
}

/**
 * @brief 计算最大直径
 *
 * 递归解法（时间复杂度O(n))
 *
 * @param [in] root
 * @return int 0: success; <0: failed, posix errno;
 */
int diameterOfBinaryTree(struct TreeNode *root)
{
	int maxDiameter = 0;
	diameterOfBinaryTreeHelper(root, &maxDiameter);
	return maxDiameter;
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

	printf("二叉树的直径: %d\n", diameterOfBinaryTree(root));

	return 0;
}
