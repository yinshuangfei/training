/**
 * @file test_main.c
 * @author Alan Yin (yinshuangfei007@163.com)
 * @brief
 * @version 0.0.1
 * @date 2026-05-12
 *
 * @copyright Copyright (c) 2026
 *
 * @node: 跳表
 *
 * 跳表的作用就是结合数组和链表两者的优点：
 * 在保持数据有序的同时，它既有链表灵活的插入删除特性，又能达到接近二分查找的查询效率。
 * 跳表通过在链表的基础上增加多层索引，使得查询操作能够跳过大量的节点，从而大幅提升查询
 * 效率。
 *
 * 它通过“空间换时间”的策略，巧妙地弥补了普通链表查询慢的短板，成为平衡树（如红黑树、
 * AVL树）的一种强大且更易实现的替代方案。
 *
 * 使用场景：
 * - Redis 使用跳表来实现 ZSet
 * - LevelDB 的内存表 (MemTable)
 */

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

// --- 配置参数 ---
#define MAX_LEVEL	16	// 最大层级，2^16 足够覆盖绝大多数数据量
#define PROBABILITY	0.5	// 随机升级的概率 (50%)

// 跳表节点
typedef struct SkipListNode {
	int key;                          // 存储的关键字
	int level;                        // 节点的层级（仅用于调试，实际不需要存储）
	struct SkipListNode **forward;    // 指针数组，指向不同层级的下一个节点
} SkipListNode;

// 跳表结构
typedef struct SkipList {
	int level;            // 当前跳表的最大层级
	SkipListNode *header; // 头节点（哨兵），不存具体数据
} SkipList;

/**
 * 创建新节点
 * @param key 关键字
 * @param level 节点的层级
 */
SkipListNode *createNode(int key, int level)
{
	SkipListNode *newNode = (SkipListNode*)malloc(sizeof(SkipListNode));
	if (!newNode)
		return NULL;

	newNode->key = key;
	newNode->level = level;	/** for debug */

	/**
	 * 为新节点分配 forward 数组，大小为节点的层级
	 * newNode->forward[i]，表示第 i 层的后续节点
	 */
	newNode->forward = (SkipListNode**)malloc(level * sizeof(SkipListNode*));
	for (int i = 0; i < level; i++) {
		newNode->forward[i] = NULL;
	}
	return newNode;
}

/**
 * 创建跳表
 */
SkipList *createSkipList()
{
	SkipList *list = (SkipList*)malloc(sizeof(SkipList));
	list->level = 1;
	// 创建头节点，初始化为最大层级，Key设为-1作为哨兵
	list->header = createNode(-1, MAX_LEVEL);
	return list;
}

/**
 * 随机生成层级算法
 * 模拟抛硬币：如果随机数小于概率，则层级+1，直到达到最大层级或停止
 */
int randomLevel()
{
	int level = 1;
	while ((float)rand() / RAND_MAX < PROBABILITY && level < MAX_LEVEL) {
		level++;
	}
	return level;
}

// --- 核心操作 ---
/**
 * 插入元素
 * @param list 跳表指针
 * @param key 要插入的值
 */
void insert(SkipList *list, int key)
{
	SkipListNode *update[MAX_LEVEL]; // 用于记录每一层需要更新的前驱节点
	SkipListNode *current = list->header;

	// 1. 寻找插入位置，并记录路径
	for (int i = list->level - 1; i >= 0; i--) {
		while (current->forward[i] != NULL &&
		       current->forward[i]->key < key) {
			current = current->forward[i];
		}
		update[i] = current; // 记录第 i 层的最后一个小于 key 的节点
	}

	// 移动到最底层，检查是否已存在该 key
	current = current->forward[0];
	if (current != NULL && current->key == key) {
		printf("Key %d 已存在，跳过插入。\n", key);
		return;
	}

	// 2. 生成新节点的随机层级
	int newLevel = randomLevel();

	// 3. 如果新节点层级高于当前跳表最大层级，更新 update 数组
	if (newLevel > list->level) {
		for (int i = list->level; i < newLevel; i++) {
		update[i] = list->header;
		}
		list->level = newLevel;
	}

	// 4. 创建新节点并重新链接指针
	SkipListNode *newNode = createNode(key, newLevel);
	for (int i = 0; i < newLevel; i++) {
		newNode->forward[i] = update[i]->forward[i];
		update[i]->forward[i] = newNode;
	}
}

/**
 * 删除指定元素
 * @param list 跳表指针
 * @param key 要删除的值
 * @return 删除成功返回 1，未找到目标返回 0
 */
int deleteNode(SkipList *list, int key)
{
	SkipListNode *update[MAX_LEVEL]; // 记录每一层需要更新的前驱节点
	SkipListNode *current = list->header;

	// 1. 寻找目标节点，并记录每一层的路径
	for (int i = list->level - 1; i >= 0; i--) {
		while (current->forward[i] != NULL &&
		       current->forward[i]->key < key) {
			current = current->forward[i];
		}
		update[i] = current;
	}

	// 移动到最底层，检查目标节点是否存在
	current = current->forward[0];
	if (current == NULL || current->key != key) {
		return 0; // 目标节点不存在，删除失败
	}

	// 2. 在每一层断开目标节点的指针（将其从链表中摘除）
	for (int i = 0; i < list->level; i++) {
		// 如果当前层的前驱节点的下一个不是目标节点，说明目标节点没有这一层的索引，直接跳出
		if (update[i]->forward[i] != current) {
			break;
		}
		// 前驱节点直接指向目标节点的下一个节点，完成“跳过”目标节点的操作
		update[i]->forward[i] = current->forward[i];
	}

	// 3. 释放目标节点的内存
	free(current->forward); // 先释放指针数组
	free(current);          // 再释放节点本身

	// 4. 更新跳表的最大层级
	// 如果删除的是当前最高层的唯一节点，需要降低跳表的 level
	while (list->level > 1 &&
	       list->header->forward[list->level - 1] == NULL) {
		list->level--;
	}

	return 1; // 删除成功
}

/**
 * 查找元素
 * @param list 跳表指针
 * @param key 要查找的值
 * @return 找到的节点指针，未找到返回 NULL
 */
SkipListNode *search(SkipList *list, int key)
{
	SkipListNode *current = list->header;

	// 从最高层开始向下查找
	for (int i = list->level - 1; i >= 0; i--) {
		while (current->forward[i] != NULL && current->forward[i]->key < key) {
		current = current->forward[i];
		}
	}

	// 到达最底层，判断下一个节点是否为目标
	current = current->forward[0];
	if (current != NULL && current->key == key) {
		return current;
	}
	return NULL;
}

/**
 * 打印跳表结构
 * 从上到下打印每一层的链表
 */
void printSkipList(SkipList *list)
{
	printf("\n--- 跳表结构 (Level %d) ---\n", list->level);
	for (int i = list->level - 1; i >= 0; i--) {
		SkipListNode *current = list->header->forward[i];
		printf("L%d: ", i);
		while (current != NULL) {
			printf("%d(%d) -> ", current->key, current->level); // 显示节点的 key 和层级
			current = current->forward[i];
		}
		printf("NULL\n");
	}
	printf("---------------------------\n");
}

int main()
{
	srand(time(NULL)); // 初始化随机数种子
	SkipList *sl = createSkipList();

	// 插入测试数据
	int keys[] = {10, 20, 30, 40, 50, 25, 35, 45};
	int n = sizeof(keys) / sizeof(keys[0]);

	for (int i = 0; i < n; i++) {
		insert(sl, keys[i]);
	}

	// 打印结构
	printSkipList(sl);

	// 查找测试
	int target = 25;
	if (search(sl, target)) {
		printf("✅ 找到元素: %d\n", target);
	} else {
		printf("❌ 未找到元素: %d\n", target);
	}

	int target_to_delete = 25;
	if (deleteNode(sl, target_to_delete)) {
		printf("✅ 成功删除元素: %d\n", target_to_delete);
	} else {
		printf("❌ 删除失败，未找到元素: %d\n", target_to_delete);
	}

	// 再次打印，观察 25 是否从所有层级中消失
	printSkipList(sl);

	return 0;
}