/**
 * @file 05_max_continue_inc_num_str.c
 * @author Alan Yin (yinshuangfei007@163.com)
 * @brief
 * @version 0.0.1
 * @date 2025-05-05
 *
 * @copyright Copyright (c) 2025
 *
 * @node: 最大连续递增数字串
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

/**
 * @brief 基本遍历法
 *
 * @param [in] s
 */
void findMaxIncreasingDigits(char *s)
{
	int max = 0, index = 0, len = 0, start = 0;

	for (int i = 0; i < strlen(s); i++) {
		if (s[i] >= '0' && s[i] <= '9') {
			if (i == 0 ||
			    /** 字符是否是增序
			     * 普通增序：s[i-1] >= s[i]
			     * 严格增序：s[i-1] + 1 != s[i]
			     */
			    (s[i-1] + 1 != s[i])) {
				if (len > max) {
					max = len;
					start = index;
				}
				index = i;
				len = 1;
			} else {
				len++;
			}
		}
	}

	for (int i = start; i < start + max; i++) {
		printf("%c", s[i]);
	}
	printf("\n");
}
/**
 * @brief 动态规划法
 *
 * @param [in] s
 */
int longestIncreasingDigits(const char* str)
{
	int n = strlen(str);
	int dp[n];

	/** 使用动态规划思想，记录每个位置的最长递增数字串长度 */
	for(int i = 0; i < n; i++) {
		dp[i] = 1;
	}

	int maxLength = 1;
	for (int i = 1; i < n; i++) {
		/** 字符是否是增序
		 * 普通增序：str[i] > str[i-1]
		 * 严格增序：str[i-1] + 1 == str[i]
		 */
		if (str[i-1] + 1 == str[i] &&
		    isdigit(str[i]) &&
		    isdigit(str[i-1])) {
			dp[i] = dp[i-1] + 1;
			maxLength = maxLength > dp[i] ? maxLength : dp[i];
		}
	}

	return maxLength;
}

int main(void)
{
	char s[] = "addfad3sl456789DF3456ld345eA";
	findMaxIncreasingDigits(s);
	printf("最长连续递增数字串长度: %d\n", longestIncreasingDigits(s));
	return 0;
}