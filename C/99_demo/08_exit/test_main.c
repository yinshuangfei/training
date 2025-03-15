#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

/**
 * exit 函数
 * void exit(int status);
 *
 * 功能：正常终止程序，执行清理操作。
 * 行为：
 *   - 调用通过 atexit 或 on_exit 注册的函数。
 *   - 刷新所有打开的流（如 stdout、stderr），关闭所有文件描述符。
 *   - 将 status 传递给父进程（通过 wait 或 waitpid 获取）。
 *   - 终止进程。
*/

/**
 * _exit 函数
 * void _exit(int status);
 *
 * 功能：立即终止程序，不执行任何清理操作。
 * 行为：
 *   - 不调用通过 atexit 或 on_exit 注册的函数。
 *   - 不刷新缓冲区，不关闭文件描述符。
 *   - 将 status 传递给父进程。
 *   - 立即终止进程。
*/

void cleanup()
{
	printf("Cleanup function called\n");
}

int main()
{
	atexit(cleanup);  // 注册清理函数
	printf("Exiting program...\n");
	exit(0);  // 正常退出
}

// int main()
// {
// 	atexit(cleanup);  // 注册清理函数
// 	printf("Exiting program...\n");  // 可能不会输出
// 	_exit(0);  // 立即退出
// }