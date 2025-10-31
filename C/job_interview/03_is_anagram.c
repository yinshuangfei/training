/**
 * @file 03_is_anagram.c
 * @author Alan Yin (yinshuangfei007@163.com)
 * @brief
 * @version 0.0.1
 * @date 2025-05-02
 *
 * @copyright Copyright (c) 2025
 *
 * @node: 判断两个字符串是否为排列（Anagrams，即字符顺序不同但内容相同）
 */

#include <stdio.h>
#include <stdbool.h>
#include <string.h>

bool isAnagram(const char *s1, const char *s2)
{
	if (strlen(s1) != strlen(s2)) {
		// 长度不同直接返回false
		return false;
	}

	int count[256] = {0};  // 覆盖所有ASCII字符

	// 统计s1的字符频率
	for (int i = 0; s1[i] != '\0'; i++) {
		count[(unsigned char)s1[i]]++;
	}

	// 减去s2的字符频率
	for (int i = 0; s2[i] != '\0'; i++) {
		count[(unsigned char)s2[i]]--;
		if (count[(unsigned char)s2[i]] < 0) {
			return false;  // 出现不一致的字符
		}
	}

	return true;
}

int main()
{
	const char* s1 = "listen";
	const char* s2 = "silent";

	printf("%s\n", isAnagram(s1, s2) ? "true" : "false");

	return 0;
}
