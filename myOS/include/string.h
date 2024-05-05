/*
 * @Author: Alan Yin
 * @Date: 2024-05-05 22:14:04
 * @LastEditTime: 2024-05-05 23:28:28
 * @LastEditors: Alan Yin
 * @FilePath: /myOS/include/string.h
 * @Description:
 * // -*- mode:C; tab-width:8; c-basic-offset:2; indent-tabs-mode:t -*-
 * // vim: ts=8 sw=2 smarttab
 * Copyright (c) 2024 by Alan Yin, All Rights Reserved.
 */
#ifndef _STRING_H_
#define _STRING_H_

#include <types.h>

void *memcpy(void *dest, const void *src, size_t cnt);
unsigned short *memsetw(unsigned short *dest, unsigned short val, size_t count);

#endif /** _STRING_H_ */