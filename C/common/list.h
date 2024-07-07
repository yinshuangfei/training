/*
 * @Author: Alan Yin
 * @Date: 2024-05-27 08:48:03
 * @LastEditTime: 2024-07-02 21:33:08
 * @LastEditors: Alan Yin
 * @FilePath: /training/C/common/list.h
 * @Description:
 * // -*- mode:C; tab-width:8; c-basic-offset:2; indent-tabs-mode:t -*-
 * // vim: ts=8 sw=2 smarttab
 * Copyright (c) 2024 by Alan Yin, All Rights Reserved.
 */


#ifndef __YSF_LIST_H__
#define __YSF_LIST_H__

#include <stddef.h>		// for offsetof

struct list_head {
	struct list_head* next;
	struct list_head* prev;
};

static inline void INIT_LIST_HEAD(struct list_head* list)
{
	list->next = list;
	list->prev = list;
}

/**
 * container_of - cast a member of a structure out to the containing structure
 * @ptr:        the pointer to the member.
 * @type:       the type of the container struct this is embedded in.
 * @member:     the name of the member within the struct.
 *
 */
#define container_of(ptr, type, member) ({				\
		const typeof(((type *)0)->member)*__mptr = (ptr);	\
		(type *)((char *)__mptr - offsetof(type,member));	\
	})

/**
 * list_entry - get the struct for this entry
 * @ptr:        the &struct list_head pointer.
 * @type:       the type of the struct this is embedded in.
 * @member:     the name of the list_struct within the struct.
 */
#define list_entry(ptr, type, member)					\
	container_of(ptr, type, member)

/**
 * list_first_entry - get the first element from a list
 * @ptr:        the list head to take the element from.
 * @type:       the type of the struct this is embedded in.
 * @member:     the name of the list_struct within the struct.
 *
 * Note, that list is expected to be not empty.
 */
#define list_first_entry(ptr, type, member)				\
	list_entry((ptr)->next, type, member)

/**
 * list_first_entry_or_null - get the first element from a list
 * @ptr:        the list head to take the element from.
 * @type:       the type of the struct this is embedded in.
 * @member:     the name of the list_struct within the struct.
 *
 * Note that if the list is empty, it returns NULL.
 */
#define list_first_entry_or_null(ptr, type, member)			\
	(!list_empty(ptr) ?						\
	 list_first_entry(ptr, type, member) : NULL)

/**
 * @description: 将 new 添加到 prev 和 next 之间
 * @param {list_head*} new
 * @param {list_head*} prev
 * @param {list_head*} next
 * @return {*}
 */
static inline void __list_add(struct list_head* new, struct list_head* prev,
			      struct list_head* next)
{
	next->prev = new;
	new->next = next;
	new->prev = prev;
	prev->next = new;
}

/**
 * @description: 将 new 添加到 head 后面
 * 即将 new 添加到 head 和 head->next 之间
 * @param {list_head} *new
 * @param {list_head} *head
 * @return {*}
 */
static inline void list_add(struct list_head *new, struct list_head *head)
{
	__list_add(new, head, head->next);
}

/**
 * @description: 将 new 添加到末尾
 * 即将 new 添加到 head->prev 和 head 之间
 * @param {list_head} *new
 * @param {list_head} *head
 * @return {*}
 */
static inline void list_add_tail(struct list_head *new, struct list_head *head)
{
	__list_add(new, head->prev, head);
}

/**
 * @description: 连接 prev 和 next 两个节点
 * @param {list_head} *prev
 * @param {list_head} *next
 * @return {*}
 */
static inline void __list_del(struct list_head *prev, struct list_head *next)
{
	next->prev = prev;
	prev->next = next;
}

/**
 * @description: 删除 entry 节点，不删除前后指针关系
 * @param {list_head} *entry
 * @return {*}
 */
static inline void __list_del_entry(struct list_head *entry)
{
	__list_del(entry->prev, entry->next);
}

/**
 * @description: 删除 entry 节点，删除前后指针关系
 * @param {list_head} *entry
 * @return {*}
 */
static inline void list_del(struct list_head *entry)
{
	__list_del(entry->prev, entry->next);
	entry->next = NULL;
	entry->prev = NULL;
}

/**
 * @description: 将 entry 节点从链表中取出，添加到链表末尾
 * @param {list_head} *entry
 * @param {list_head} *head
 * @return {*}
 */
static inline void list_move_tail(struct list_head *entry,
				  struct list_head *head)
{
	__list_del_entry(entry);
	list_add_tail(entry, head);
}

/**
 * @description: 将 entry 从链表中取出，初始化为链表
 * @param {list_head} *entry
 * @return {*}
 */
static inline void list_del_init(struct list_head *entry)
{
	__list_del_entry(entry);
	INIT_LIST_HEAD(entry);
}

/**
 * @description: 链表是否为空
 * @param {list_head*} head
 * @return {*}
 */
static inline int list_empty(const struct list_head *head)
{
	return head->next == head;
}

/**
 * list_for_each_entry - iterate over list of given type
 * @pos:        the type * to use as a loop cursor.
 * @head:       the head for your list.
 * @member:     the name of the list_struct within the struct.
 */
#define list_for_each_entry(pos, head, member)				\
	for (pos = list_entry((head)->next, typeof(*pos), member);	\
	     &pos->member != (head);					\
	     pos = list_entry(pos->member.next, typeof(*pos), member))

/**
 * list_for_each_entry_safe - iterate over list of given type safe against
 * removal of list entry
 * @pos:        the type * to use as a loop cursor.
 * @tmp:        another type * to use as temporary storage
 * @head:       the head for your list.
 * @member:     the name of the list_struct within the struct.
 */
#define list_for_each_entry_safe(pos, tmp, head, member)		\
	for (pos = list_entry((head)->next, typeof(*pos), member),	\
	     tmp = list_entry(pos->member.next, typeof(*pos), member);	\
	     &pos->member != (head);					\
	     pos = tmp,							\
	     tmp = list_entry(tmp->member.next, typeof(*tmp), member))

#endif /** __YSF_LIST_H__ */
