/**
 * @file 25_min_no_comlicate_str.c
 * @author Alan Yin (yinshuangfei007@163.com)
 * @brief
 * @version 0.0.1
 * @date 2025-07-18
 *
 * @copyright Copyright (c) 2025
 *
 * @node: 给定一个字符串 s ，请你找出其中不含有重复字符的 最长 子串 的长度。
 *
 * 输入: s = "abcabcbb"
 * 输出: 3
 * 解释: 因为无重复字符的最长子串是 "abc"，所以其长度为 3。
 *
 * 输入: s = "bbbbb"
 * 输出: 1
 * 解释: 因为无重复字符的最长子串是 "b"，所以其长度为 1。
 *
 * 输入: s = "pwwkew"
 * 输出: 3
 * 解释: 因为无重复字符的最长子串是 "wke"，所以其长度为 3。
 *       请注意，你的答案必须是 子串 的长度，"pwke" 是一个子序列，不是子串。
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int lengthOfLongestSubstring(char *s)
{
	int max_len = 0;
	int left = 0;
	int index[128]; // ASCII码表范围优化

	// 初始化字符位置记录
	memset(index, -1, sizeof(index));

	for (int right = 0; s[right] != '\0'; right++) {
		if (index[s[right]] >= left) {
			left = index[s[right]] + 1; // 移动左边界到重复字符的下一位
		}

		index[s[right]] = right; // 更新字符最新位置
		int current_len = right - left + 1;
		max_len = (current_len > max_len) ? current_len : max_len;
	}

	printf("max_len:%d\n", max_len);

	return max_len;
}

int main()
{
	lengthOfLongestSubstring("assdrtt");
	lengthOfLongestSubstring("abcabcbb");
	lengthOfLongestSubstring("bbbbb");
	lengthOfLongestSubstring("pwwkew");
	return 0;
}