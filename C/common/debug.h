/**
 * @file debug.h
 * @author Alan Yin (yinshuangfei007@163.com)
 * @brief
 * @version 0.0.1
 * @date 2025-04-05
 *
 * @copyright Copyright (c) 2025
 *
 * @node:
 */

static inline void pr_line(int width)
{
	if (width < 20)
		width = 20;

	if (width > 160)
		width = 160;

	for (int i = 0; i < width; i++)
		printf("-");

	printf("\n");
}

/**
 * @brief 任意字节一行输出二进制数据
 *
 * @param [in] buff
 * @param [in] len
 */
static inline void pr_buff(char *buff, int len)
{
	int i = 0;
	int row_cnt = 4;

	if (0 == len) {
		return;
	}

	if (len / row_cnt > 0) {
		for (i = 0; i < len - len % row_cnt; i += row_cnt) {
			if (i != 0 && i % row_cnt == 0) {
				printf("\n");
			}

			if (i % row_cnt == 0) {
				printf("[%03d/%03d] 0x", i + 1, len);
			}

			for (int j = 0; j < row_cnt; j++)
				printf("%02x", buff[i + j] & 0xff);
		}
		printf("\n");
	}

	if (len % row_cnt != 0) {
		printf("[%03d/%03d] 0x", i + 1, len);

		for (int j = 0; j < len % row_cnt; j++)
			printf("%02x", buff[i + j] & 0xff);

		printf("\n");
	}
}

static inline void print_raw_ptr(char *buf, int len)
{
	pr_line(80);
	for (int i = 0; i < len; i++) {
		if (i == 0 && i % 16 == 0)
			printf("[0x%04x]", i);
		else if (i % 16 == 0)
			printf("\n[0x%04x]", i);
		printf(" 0x%02x", buf[i] & 0xff);
	}
	printf("\n");
	pr_line(80);
}