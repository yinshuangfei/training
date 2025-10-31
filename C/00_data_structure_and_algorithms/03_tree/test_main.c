#include <stdio.h>
#include <stdlib.h>

/** 二叉树节点结构体 */
typedef struct TreeNode {
	int data;
	struct TreeNode* left;
	struct TreeNode* right;
} TreeNode;

/**
 * @brief 创建新节点
 *
 * @param [in out] data
 * @return TreeNode* 0: success; <0: failed, posix errno;
 */
TreeNode* createNode(int data)
{
	TreeNode* newNode = (TreeNode*)malloc(sizeof(TreeNode));
	if (newNode == NULL) {
		printf("内存分配失败\n");
		exit(1);
	}
	newNode->data = data;
	newNode->left = NULL;
	newNode->right = NULL;
	return newNode;
}

/**
 * @brief 前序遍历：根 -> 左 -> 右
 *
 * @param [in] root
 */
void preorderTraversal(TreeNode* root)
{
	if (root != NULL) {
		printf("%d ", root->data);
		preorderTraversal(root->left);
		preorderTraversal(root->right);
	}
}

/**
 * @brief 中序遍历：左 -> 根 -> 右
 *
 * @param [in] root
 */
void inorderTraversal(TreeNode* root) {
	if (root != NULL) {
		inorderTraversal(root->left);
		printf("%d ", root->data);
		inorderTraversal(root->right);
	}
}

/**
 * @brief 后序遍历：左 -> 右 -> 根
 *
 * @param [in] root
 */
void postorderTraversal(TreeNode* root) {
	if (root != NULL) {
		postorderTraversal(root->left);
		postorderTraversal(root->right);
		printf("%d ", root->data);
	}
}

/**
 * @brief 释放二叉树内存
 *
 * @param [in] root
 */
void freeTree(TreeNode* root)
{
	if (root != NULL) {
		freeTree(root->left);
		freeTree(root->right);
		free(root);
	}
}

int main()
{
	/* 构建一个简单的二叉树
	       1
	      / \
	     2   3
	    / \
	   4   5
	*/
	TreeNode* root = createNode(1);
	root->left = createNode(2);
	root->right = createNode(3);
	root->left->left = createNode(4);
	root->left->right = createNode(5);

	printf("前序遍历: ");
	preorderTraversal(root);
	printf("\n");

	printf("中序遍历: ");
	inorderTraversal(root);
	printf("\n");

	printf("后序遍历: ");
	postorderTraversal(root);
	printf("\n");

	// 释放内存
	freeTree(root);

	return 0;
}
