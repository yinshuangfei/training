/**
 * @file 15_min_coverage_substring.c
 * @author Alan Yin (yinshuangfei007@163.com)
 * @brief
 * @version 0.0.1
 * @date 2025-05-12
 *
 * @copyright Copyright (c) 2025
 *
 * @node: 最小覆盖子串（最小窗口方法）
 *
 * 给定一个字符串 s 和一个字符串 t，请在 s 中找到包含 t 所有字符的最短子串。
 * 题目要求：
 *   给定一个字符串 s 和一个字符串 t，请编写一个函数，返回 s 中包含 t 所有字符的最短子串。
 *   如果不存在这样的子串，返回空字符串。
 *
 * 示例 1：
 * 输入：s = "ADOBECODEBANC", t = "ABC"
 * 输出："BANC"
 * 示例 2：
 * 输入：s = "a", t = "a"
 * 输出："a"
 */

#include <stdio.h>
#include <string.h>
#include <limits.h>
#include <stdlib.h>

#define CHAR_SET_SIZE 128

/**
 * @brief 匹配字符种类数
 *
 * @param [in out] s
 * @param [in out] t
 * @return char* 0: success; <0: failed, posix errno;
 */
char *minWindow_self(char *s, char *t)
{
	if (s == NULL || t == NULL || strlen(s) < strlen(t)) {
		return "";
	}

	/** 统计 t 的频率 */
	int t_freq[CHAR_SET_SIZE] = {0};
	for (int i = 0; t[i]; i++) {
		t_freq[(int)t[i]]++;
	}

	/** 匹配字符种类数 */
	int require = 0;
	for (int i = 0; i < CHAR_SET_SIZE; i++) {
		if (t_freq[i] > 0) {
			require++;
		}
	}

	int left = 0;
	int right = 0;
	int min_len = 0xffff;
	int min_left = 0;
	int formed = 0;		/** 当前窗口中已匹配的字符种类 */
	int window[CHAR_SET_SIZE] = {0};

	/** 扩大窗口，控制右边界 */
	while (s[right]) {
		char c = s[right];
		window[(int)c]++;

		/** 当前字符在 t 中，且频率相同，增加 formed */
		if (t_freq[(int)c] > 0 && t_freq[(int)c] == window[(int)c]) {
			formed++;
		}

		/** 收缩窗口，控制左边界 */
		while (left <= right && formed == require) {
			c = s[left];

			/** 更新最小窗口 */
			if (right - left + 1 < min_len) {
				min_len = right - left + 1;
				min_left = left;
			}

			/** 移动窗口 */
			window[(int)c]--;

			/** 收缩的字符在 t 中，且窗口中的频率不满足条件 */
			if (t_freq[(int)c] > 0 &&
			    window[(int)c] < t_freq[(int)c]) {
				formed--;
			}

			left++;
		}

		right++;
	}

	printf("%.*s \n", min_len, s + min_left);

	/** 准备返回值 */
	if (min_len == 0xffff) {
		return "";
	} else {
		char *ret = calloc(1, min_len + 1);
		strncpy(ret, s + min_left, min_len);
		return ret;
	}
}

/**
 * @brief 匹配字符总数
 *
 * @param [in out] s
 * @param [in out] t
 * @return char* 0: success; <0: failed, posix errno;
 */
char *minWindow(char *s, char *t)
{
	if (s == NULL || t == NULL || strlen(s) < strlen(t))
		return "";

	// 初始化哈希表记录t中字符出现次数
	int tCount[CHAR_SET_SIZE] = {0};
	for (int i = 0; t[i] != '\0'; i++) {
		tCount[t[i]]++;
	}

	int left = 0, right = 0;
	int required = strlen(t); // 需要匹配的字符总数
	int minLen = INT_MAX;
	int minLeft = 0;
	int windowCount[CHAR_SET_SIZE] = {0};
	int formed = 0; // 当前窗口中已匹配的字符数

	while (right < strlen(s)) {
		char c = s[right];
		windowCount[c]++;

		// 如果当前字符在t中，且窗口中的数量不超过t中的数量
		if (tCount[c] > 0 && windowCount[c] <= tCount[c]) {
			formed++;
		}

		// 尝试收缩窗口左边界
		while (formed == required && left <= right) {
			// 更新最小窗口
			if (right - left + 1 < minLen) {
				minLen = right - left + 1;
				minLeft = left;
			}

			char sss[1024] = {0};
			snprintf(sss, right - left + 2, "%s", s + left);
			printf("required:%d, formed:%d, left:%d, right:%d, minLeft:%d, %s\n",
				required, formed, left, right, minLeft, sss);

			char leftChar = s[left];
			windowCount[leftChar]--;

			// 如果移除了t中的关键字符
			if (tCount[leftChar] > 0 &&
			    windowCount[leftChar] < tCount[leftChar]) {
				formed--;
			}
			left++;
		}
		right++;
	}

	// 返回结果
	if (minLen == INT_MAX)
		return "";

	char* result = (char*)malloc(sizeof(char) * (minLen + 1));
	strncpy(result, s + minLeft, minLen);
	result[minLen] = '\0';
	return result;
}

int main()
{
	printf("'%s'\n", minWindow("ADOBECODEBANCCANNNB", "ABCC")); // 输出 "BANC"
	printf("'%s'\n", minWindow_self("ADOBECODEBANCCANNNB", "ABCC")); // 输出 "BANC"
	printf("'%s'\n", minWindow("a", "a"));               // 输出 "a"
	printf("'%s'\n", minWindow("a", "aa"));              // 输出 ""
	return 0;
}
