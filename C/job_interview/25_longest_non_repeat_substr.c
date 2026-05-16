/**
 * @file 25_longest_non_repeat_substr.c
 * @author Alan Yin (yinshuangfei007@163.com)
 * @brief
 * @version 0.0.1
 * @date 2025-07-18
 *
 * @copyright Copyright (c) 2025
 *
 * @node: 最长无重复子串（滑动窗口法）。
 * 给定一个字符串 s ，请你找出其中不含有重复字符的'最长子串'的长度。
 *
 * 寻找字符串中最长无重复字符子串是一个经典问题，主要考察对滑动窗口和
 * 哈希表的应用能力。
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

int lengthOfLongestSubstring_1(char *s)
{
	int n = strlen(s);
	if (n == 0)
		return 0;

	int lastIndex[256]; // ASCII字符最后出现'位置'
	memset(lastIndex, -1, sizeof(lastIndex));

	int maxLen = 0;
	int start = 0; // 窗口起始位置

	for (int end = 0; end < n; end++) {
		char c = s[end];
		// 如果字符已存在且在当前窗口内，移动窗口起始位置
		if (lastIndex[c] != -1 && lastIndex[c] >= start) {
			start = lastIndex[c] + 1;
		}

		// 更新字符最后出现位置
		lastIndex[c] = end;

		// 计算当前窗口长度
		int currentLen = end - start + 1;
		if (currentLen > maxLen) {
			maxLen = currentLen;
		}
	}
	return maxLen;
}

int lengthOfLongestSubstring_2(char *s)
{
	int max_len = 0;
	int left = 0;
	// ASCII码表范围优化, 用来记录每个字符最后一次出现的位置（下标）
	int index[128]; 

	// 初始化字符位置记录
	memset(index, -1, sizeof(index));

	for (int right = 0; s[right] != '\0'; right++) {
		// index[s[right]]: 取出当前字符 s[right] 上一次出现的位置。
		// < left，说明该字符上次出现在窗口左边界的左侧（即已经被甩在窗口后面了），
		// 不算重复，不用管
		// >= left，说明该字符就在当前窗口里！发生了冲突（重复）
		// left = index[s[right]] + 1: 既然发现了重复，我们需要收缩窗口。

		if (index[s[right]] >= left) {
			left = index[s[right]] + 1; // 移动左边界到重复字符的下一位
		}

		index[s[right]] = right; // 更新字符最新位置

		int current_len = right - left + 1;
		max_len = (current_len > max_len) ? current_len : max_len;
	}

	return max_len;
}

int main()
{
	char *test_cases[] = {
		"abcabcbb",
		"bbbbb",
		"pwwkew",
		"assdrtt",
		"abcabcbb"
		"bbbbb"
	};

	int num = sizeof(test_cases) / sizeof(char *);

	for (int i = 0; i < num; i++) {
		printf("测试 (%s): %d, %d\n", 
			test_cases[i],
			lengthOfLongestSubstring_1(test_cases[i]),
			lengthOfLongestSubstring_2(test_cases[i]));
	}

	return 0;
}