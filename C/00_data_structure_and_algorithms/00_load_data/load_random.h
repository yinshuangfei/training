/**
 * @file load_random.h
 * @author Alan Yin (yinshuangfei007@163.com)
 * @brief
 * @version 0.0.1
 * @date 2025-01-15
 *
 * @copyright Copyright (c) 2025
 *
 * @node:
 */

#ifndef __LOAD_RANDOM__
#define __LOAD_RANDOM__

/**
 * @brief
 *
 * @param [in] filename		文件名
 * @param [out] data		读取的数据
 * @param [out] total		读取的总条目
 * @param [in] creat_size	数据条目
 * @param [in] creat_max	最大数值
 * @param [in] debug		调试模式
 * @return int
 */
int load_data(char *filename, long **data, long *total, long creat_size,
	      long creat_max, int debug);

#endif /** __LOAD_RANDOM__ */