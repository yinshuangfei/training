/**
 * @file test_main.c
 * @author Alan Yin (yinshuangfei007@163.com)
 * @brief
 * @version 0.0.1
 * @date 2025-03-15
 *
 * @copyright Copyright (c) 2025
 *
 * @node:
 */

#include <sys/types.h>
#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include "../../common/utils.h"

int main(int argc, char *argv[])
{
	int pipefd[2];
	pid_t cpid;
	char buf;

	if (argc != 2) {
		fprintf(stderr, "Usage: %s <string>\n", argv[0]);
		exit(EXIT_FAILURE);
	}

	/**
	 * Pipe 是 Unix/Linux 系统中用于进程间通信（IPC，Inter-Process Communication）
	 * 的一种机制。它创建一个管道（Pipe），管道是一个单向的数据通道，允许一个进程
	 * 向管道写入数据，另一个进程从管道读取数据。
	 * - 管道是一种特殊的文件，存在于内核中，没有磁盘上的文件表示。
	 * - 管道是单向的，数据只能从一个方向流动（从写端到读端）。
	 * - 管道有一个固定大小的缓冲区（通常为 4KB 或 64KB），当缓冲区满时，写操作会
	 * 阻塞；当缓冲区为空时，读操作会阻塞。
	 * - 管道通常用于父子进程或兄弟进程之间的通信。
	 *
	 * pipefd：一个长度为 2 的整数数组，pipefd[0] 是管道的读端，pipefd[1] 是管道
	 * 的写端。
	*/
	if (pipe(pipefd) == -1) {
		pr_stderr("pipe");
		exit(EXIT_FAILURE);
	}

	cpid = fork();
	if (cpid == -1) {
		pr_stderr("pipe");
		exit(EXIT_FAILURE);
	}

	if (cpid == 0) {
		/* Child reads from pipe */
		close(pipefd[1]);          /* Close unused write end */

		while (read(pipefd[0], &buf, 1) > 0)
			write(STDOUT_FILENO, &buf, 1);

		write(STDOUT_FILENO, "\n", 1);
		close(pipefd[0]);
		_exit(EXIT_SUCCESS);
	} else {
		/* Parent writes argv[1] to pipe */
		close(pipefd[0]);          /* Close unused read end */
		write(pipefd[1], argv[1], strlen(argv[1]));
		close(pipefd[1]);          /* Reader will see EOF */
		wait(NULL);                /* Wait for child */
		exit(EXIT_SUCCESS);
	}
}
