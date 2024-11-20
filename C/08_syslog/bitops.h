/*
 * @Author: Alan Yin
 * @Date: 2024-07-18 19:47:50
 * @LastEditTime: 2024-07-18 19:52:59
 * @LastEditors: Alan Yin
 * @FilePath: \Code\training\C\08_syslog\bitops.h
 * @Description:
 * @// -*- mode:C; tab-width:8; c-basic-offset:2; indent-tabs-mode:t -*-
 * @// vim: ts=8 sw=2 smarttab
 * @Copyright (c) 2024 by Alan Yin, All Rights Reserved.
 */
/*
 * Soft:        Keepalived is a failover program for the LVS project
 *              <www.linuxvirtualserver.org>. It monitor & manipulate
 *              a loadbalanced server pool using multi-layer checks.
 *
 * Part:        bitops.h include file.
 *
 * Author:      Alexandre Cassen, <acassen@linux-vs.org>
 *
 *              This program is distributed in the hope that it will be useful,
 *              but WITHOUT ANY WARRANTY; without even the implied warranty of
 *              MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 *              See the GNU General Public License for more details.
 *
 *              This program is free software; you can redistribute it and/or
 *              modify it under the terms of the GNU General Public License
 *              as published by the Free Software Foundation; either version
 *              2 of the License, or (at your option) any later version.
 *
 * Copyright (C) 2001-2017 Alexandre Cassen, <acassen@gmail.com>
 */

#ifndef _BITOPS_H
#define _BITOPS_H

#include <limits.h>
#include <stdbool.h>

/* Defines */
#define BIT_PER_LONG	(CHAR_BIT * sizeof(unsigned long))
#define BIT_MASK(idx)	(1UL << ((idx) % BIT_PER_LONG))
#define BIT_WORD(idx)	((idx) / BIT_PER_LONG)

/* Helpers */
static inline void __set_bit(unsigned idx, unsigned long *bmap)
{
	*bmap |= BIT_MASK(idx);
}

static inline void __clear_bit(unsigned idx, unsigned long *bmap)
{
	*bmap &= ~BIT_MASK(idx);
}

static inline bool __test_bit(unsigned idx, const unsigned long *bmap)
{
	return !!(*bmap & BIT_MASK(idx));
}

static inline bool __test_and_set_bit(unsigned idx, unsigned long *bmap)
{
	if (__test_bit(idx, bmap))
		return true;

	__set_bit(idx, bmap);

	return false;
}

static inline void __set_bit_array(unsigned idx, unsigned long bmap[])
{
	bmap[BIT_WORD(idx)] |= BIT_MASK(idx);
}

static inline void __clear_bit_array(unsigned idx, unsigned long bmap[])
{
	bmap[BIT_WORD(idx)] &= ~BIT_MASK(idx);
}

static inline bool __test_bit_array(unsigned idx, const unsigned long bmap[])
{
	return !!(bmap[BIT_WORD(idx)] & BIT_MASK(idx));
}

static inline bool __test_and_set_bit_array(unsigned idx, unsigned long bmap[])
{
	if (__test_bit_array(idx, bmap))
		return true;

	__set_bit_array(idx, bmap);

	return false;
}

/* Bits */
enum global_bits {
	LOG_CONSOLE_BIT,		// LOG_CONS
	NO_SYSLOG_BIT,			// 不输出至系统日志
	LOG_FILE_OR_CONSOLE_BIT,	// 输出至控制台（stderr）
	DO_NULL_BIT,			// 不做任何动作
};

#endif
