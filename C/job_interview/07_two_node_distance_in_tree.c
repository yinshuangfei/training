/**
 * @file 07_two_node_distance_in_tree.c
 * @author Alan Yin (yinshuangfei007@163.com)
 * @brief
 * @version 0.0.1
 * @date 2025-05-07
 *
 * @copyright Copyright (c) 2025
 *
 * @node: 在二叉树中计算两个指定节点之间的最短距离
 *
 * LCA: Least Common Ancestors, 最近公共祖先
 *
 * 通常需要以下步骤：
 * - 找到两个节点的最近公共祖先（LCA）：最短路径会经过LCA。
 * - 计算从LCA到两个节点的路径长度：最短距离是这两个路径长度之和。
 */

#include <stdio.h>
#include <stdlib.h>

/** 二叉树节点结构 */
typedef struct TreeNode {
	int val;
	struct TreeNode *left;
	struct TreeNode *right;
} TreeNode;

/**
 * @brief 创建新节点
 *
 * @param [in] val
 * @return TreeNode* 0: success; <0: failed, posix errno;
 */
TreeNode *createNode(int val)
{
	TreeNode *node = (TreeNode *)malloc(sizeof(TreeNode));
	node->val = val;
	node->left = node->right = NULL;
	return node;
}

/**
 * @brief 查找最近公共祖先（LCA）
 *
 * @param [in] root
 * @param [in] p
 * @param [in] q
 * @return TreeNode* 0: success; <0: failed, posix errno;
 */
TreeNode *findLCA(TreeNode* root, int p, int q)
{
	if (root == NULL || root->val == p || root->val == q) {
		return root;
	}

	TreeNode *left = findLCA(root->left, p, q);
	TreeNode *right = findLCA(root->right, p, q);

	if (left && right) {
		return root;
	}

	return left ? left : right;
}

/**
 * @brief 计算从节点到目标节点的距离
 *
 * @param [in] root
 * @param [in] target
 * @param [in] level
 * @return int 0: success; <0: failed, posix errno;
 */
int findLevel(TreeNode *root, int target, int level)
{
	if (root == NULL)
		return -1;

	if (root->val == target)
		return level;

	int left = findLevel(root->left, target, level + 1);
	if (left != -1)
		return left;

	return findLevel(root->right, target, level + 1);
}

/**
 * @brief 计算两个节点之间的最短距离
 *
 * @param [in] root
 * @param [in] p
 * @param [in] q
 * @return int 0: success; <0: failed, posix errno;
 */
int shortestDistance(TreeNode *root, int p, int q)
{
	/** 查找LCA */
	TreeNode *lca = findLCA(root, p, q);

	/** 计算从 LCA 到 p 和 q 的距离 */
	int d1 = findLevel(lca, p, 0);
	int d2 = findLevel(lca, q, 0);

	return d1 + d2;
}

int main(void)
{
	/* 构造测试二叉树：
	           1
	        /     \
	       2       3
	      / \     / \
	     4   5   6   7
	                / \
	               8   9
	*/
	TreeNode *root = createNode(1);
	root->left = createNode(2);
	root->right = createNode(3);
	root->left->left = createNode(4);
	root->left->right = createNode(5);
	root->right->left = createNode(6);
	root->right->right = createNode(7);
	root->right->right->left = createNode(8);
	root->right->right->right = createNode(9);

	int p = 9;
	int q = 6;

	int distance = shortestDistance(root, p, q);
	printf("最短距离: %d\n", distance);

	// 释放内存（简单示例，实际需递归释放所有节点）
	free(root->right->right->left);
	free(root->right->right->right);
	free(root->right->left);
	free(root->right->right);
	free(root->left->left);
	free(root->left->right);
	free(root->left);
	free(root->right);
	free(root);

	return 0;
}