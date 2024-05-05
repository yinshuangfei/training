/*
 * @Author: Alan Yin
 * @Date: 2024-05-05 22:06:11
 * @LastEditTime: 2024-05-05 23:24:37
 * @LastEditors: Alan Yin
 * @FilePath: /myOS/include/screen.h
 * @Description:
 * // -*- mode:C; tab-width:8; c-basic-offset:2; indent-tabs-mode:t -*-
 * // vim: ts=8 sw=2 smarttab
 * Copyright (c) 2024 by Alan Yin, All Rights Reserved.
 */
#ifndef _SCREEN_H_
#define _SCREEN_H_

void init_video(void);
void putch(char c);
void puts_color_str(char* str, unsigned color);

#endif /** _SCREEN_H_ */