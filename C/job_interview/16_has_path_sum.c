/**
 * @file 16_has_path_sum.c
 * @author Alan Yin (yinshuangfei007@163.com)
 * @brief
 * @version 0.0.1
 * @date 2025-05-14
 *
 * @copyright Copyright (c) 2025
 *
 * @node: 路径总和：判断二叉树中是否存在路径和等于目标值的路径
 */

#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>

// 二叉树节点结构体
typedef struct TreeNode {
	int val;
	struct TreeNode *left;
	struct TreeNode *right;
} TreeNode;

// 辅助函数：创建新节点
TreeNode* newNode(int val)
{
	TreeNode* node = (TreeNode*)malloc(sizeof(TreeNode));
	if (node == NULL) {
		fprintf(stderr, "内存分配失败\n");
		exit(EXIT_FAILURE);
	}

	node->val = val;
	node->left = NULL;
	node->right = NULL;
	return node;
}

// 递归判断是否存在路径和等于目标值
bool hasPathSumHelper(TreeNode* root, int targetSum, int currentSum)
{
	if (root == NULL) {
		return false;
	}

	currentSum += root->val;

	// 如果是叶子节点，检查当前和是否等于目标值
	if (root->left == NULL && root->right == NULL) {
		return currentSum == targetSum;
	}

	// 递归检查左右子树
	return hasPathSumHelper(root->left, targetSum, currentSum) ||
		hasPathSumHelper(root->right, targetSum, currentSum);
}

// 主函数
bool hasPathSum(TreeNode* root, int targetSum)
{
	return hasPathSumHelper(root, targetSum, 0);
}

// 释放二叉树内存
void freeTree(TreeNode* root)
{
	if (root == NULL)
		return;

	freeTree(root->left);
	freeTree(root->right);
	free(root);
}

// 测试代码
int main(void)
{
	/* 示例树结构：
	      5
	     / \
	    4   8
	   /   / \
	  11  13  4
	 /  \      \
	7    2      1
	*/
	TreeNode* root = newNode(5);
	root->left = newNode(4);
	root->right = newNode(8);
	root->left->left = newNode(11);
	root->right->left = newNode(13);
	root->right->right = newNode(4);
	root->left->left->left = newNode(7);
	root->left->left->right = newNode(2);
	root->right->right->right = newNode(1);

	// 测试用例
	int testCases[] = {22, 26, 18, 0};
	int numCases = sizeof(testCases) / sizeof(testCases[0]);

	for (int i = 0; i < numCases; i++) {
		int target = testCases[i];
		printf("目标值 %d: ", target);

		if (hasPathSum(root, target)) {
			printf("存在路径和等于%d的路径\n", target);
		} else {
			printf("不存在路径和等于%d的路径\n", target);
		}
	}

	// 释放内存
	freeTree(root);

	return 0;
}
