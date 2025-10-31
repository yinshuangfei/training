/**
 * @file 17_length_of_longest_sub_string.c
 * @author Alan Yin (yinshuangfei007@163.com)
 * @brief
 * @version 0.0.1
 * @date 2025-05-14
 *
 * @copyright Copyright (c) 2025
 *
 * @node: 最长无重复子串
 *
 * 寻找字符串中最长无重复字符子串是一个经典问题，主要考察对滑动窗口和
 * 哈希表的应用能力。
 */

#include <stdio.h>
#include <string.h>

int lengthOfLongestSubstring(char *s)
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

// 测试用例
int main()
{
	char *testCases[] = {
		"abcabcbb",
		"bbbbb",
		"pwwkew",
	};

	int numCases = sizeof(testCases) / sizeof(char *);

	for (int i = 0; i < numCases; i++) {
		printf("测试 (%s): %d\n", testCases[i],
			lengthOfLongestSubstring(testCases[i]));
	}

	return 0;
}
