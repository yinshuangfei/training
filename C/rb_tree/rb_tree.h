/*
 * @Author: Alan Yin
 * @Date: 2023-07-22 19:38:08
 * @LastEditTime: 2024-06-27 22:22:04
 * @LastEditors: Alan Yin
 * @FilePath: /training/C/rb_tree/rb_tree.h
 * @Description:
 * // -*- mode:C; tab-width:8; c-basic-offset:2; indent-tabs-mode:t -*-
 * // vim: ts=8 sw=2 smarttab
 * Copyright (c) 2024 by HT706, All Rights Reserved.
 */

#ifndef _RB_TREE_H_
#define _RB_TREE_H_

/**
基本特性：
1.根节点是黑色的
2.叶子节点（NULL）是黑色的,叶子节点只为空
3.每条路径上的黑色节点数相同
4.没有连续的红色节点
5.结点的颜色只能是红色或者黑色，确保没有一条路径会比其他路径长出俩倍

基本操作：
旋转(左旋和右旋)、添加、删除

x是父节点，对x左旋，x变左子节点

https://blog.csdn.net/qq_40989769/article/details/113941467
*/

#include <stdio.h>
#include <stdlib.h>

enum color {
    RED,
    BLACK,
};

typedef int Type;

typedef struct RB_node {
    int col;
    Type key;
    struct RB_node * left;
    struct RB_node * right;
    struct RB_node * parent;
} rb_node, * rb_tree;

typedef struct RB_root {
    rb_node * node;
} rb_root;

rb_root * create_rbtree();

int insert_rbtree();

int delete_rbtree();

int print_rbtree(rb_root root);

int destroy_rbtree();

#endif /** _RB_TREE_H_  */