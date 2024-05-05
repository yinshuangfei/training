/*
 * @Author: Alan Yin
 * @Date: 2024-05-03 16:48:26
 * @LastEditTime: 2024-05-05 23:17:17
 * @LastEditors: Alan Yin
 * @FilePath: /myOS/windows_cifs/training/myOS/include/types.h
 * @Description:
 * // -*- mode:C; tab-width:8; c-basic-offset:2; indent-tabs-mode:t -*-
 * // vim: ts=8 sw=2 smarttab
 * Copyright (c) 2024 by Alan Yin, All Rights Reserved.
 */

#ifndef _TYPES_H_
#define _TYPES_H_

#ifndef USE_SYS
#define NULL 0
typedef unsigned long		size_t;
#endif

typedef unsigned long long	u64;
typedef signed long long	s64;
typedef unsigned int		u32;
typedef signed int		s32;
typedef unsigned short		u16;
typedef signed short		s16;
typedef unsigned char		u8;
typedef signed char		s8;

#endif /** _TYPES_H_ */