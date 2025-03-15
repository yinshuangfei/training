#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <time.h>

#include "../../common/utils.h"

int SIGNAL_TERMINATE = 0;
int count = 0;

static void dispatch_handler(int sig)
{
	switch(sig) {
	case SIGTERM:
		SIGNAL_TERMINATE = 1;
		pr_info("Received SIGTERM signal.");
		break;
	case SIGINT:
		SIGNAL_TERMINATE = 1;
		pr_info("Received SIGINT signal.");
		break;
	case SIGQUIT:
		count++;
		pr_info("Received SIGQUIT signal.");
		break;
	default:
		pr_info("Unhandled signal (%d) %s", sig, strsignal(sig));
		break;
	}
}

static void init_signal(void)
{
	/** ignore signals */
	signal(SIGCHLD, SIG_IGN);		/** 20 */
	signal(SIGHUP, SIG_IGN);		/**  1 */

	/** Implement a working signal handler */
	/** 对于任何情况下都需要立即停止程序的调试场景，注释 SIGINT 行 */
	signal(SIGINT, dispatch_handler);	/**  2, Crlt + C */
	signal(SIGQUIT, dispatch_handler);	/**  3 */
	signal(SIGTERM, dispatch_handler);	/** 15 */
}

static void print_siginfo(siginfo_t *si)
{
	timer_t *tidp;
	int or;

	/** timerid 的地址 */
	tidp = si->si_value.sival_ptr;

	pr_info("sival_ptr:%p, val:0x%lx", tidp, (long)*tidp);

	/** 定时器超时没有执行的次数 */
	or = timer_getoverrun(*tidp);
	if (or == -1) {
		pr_stderr("timer_getoverrun failed");
		exit(EXIT_FAILURE);
	} else {
		pr_info("overrun count = %d", or);
	}
}

static void handler(int sig, siginfo_t *si, void *uc)
{
	/* Note: calling printf() from a signal handler is not safe
	(and should not be done in production programs), since
	printf() is not async-signal-safe; see signal-safety(7).
	Nevertheless, we use printf() here as a simple way of
	showing that the handler was called. */

	/**
	 * Async-signal-safe 函数是指在信号处理函数中可以安全调用的函数。信号处理函数
	 * 可能在程序的任意时刻被调用，如果调用非 async-signal-safe 的函数，可能会
	 * 导致死锁、数据竞争或其他未定义行为。
	 * 以下是一些常见的 非 async-signal-safe 函数，不能在信号处理函数中调用：
	 *
	 * 函数类别	  示例函数
	 * 标准 I/O	 printf, scanf, fopen, fclose
	 * 动态内存管理	  malloc, free, realloc
	 * 线程操作	  pthread_mutex_lock, pthread_cond_wait
	 * 复杂系统调用	  system, popen
	 *
	 * 为什么某些函数不是 Async-Signal-Safe?
	 * - 锁竞争：例如，printf 内部可能使用锁，如果在信号处理函数中调用，可能会导致
	 * 死锁。
	 * - 不可重入：某些函数（如 malloc）可能修改全局状态，导致不可预测的行为。
	 * - 资源管理：某些函数可能分配或释放资源，导致资源泄漏或重复释放。
	*/

	/**
	 * ！！！不可在不阻塞或忽略定时器信号的情况下，在句柄中执行超过定时周期的任务，
	 * 因为定时器的信号将持续占据信号处理的位置，阻止进程收取其他信号，从而无法响应
	 * 其他信号！！！
	 * sleep(5);
	 */

	/**
	 * 若选择在句柄中忽略该信号，要重新生效，则需要重新注册该信号
	 * 用于需要在句柄中长时间处理，且忽略定时器信号的操作
	*/
	/** 取消注释（1） */
	// signal(sig, SIG_IGN);

	/** do something */
	print_siginfo(si);

	/** 取消注释（2） */
	// struct sigaction sa;
	// sa.sa_flags = SA_SIGINFO;
	// sa.sa_sigaction = handler;
	// sigemptyset(&sa.sa_mask);
	// if (sigaction(SIGRTMIN, &sa, NULL) == -1) {
	// 	pr_stderr("sigaction failed");
	// 	exit(EXIT_FAILURE);
	// }
}

static void block_timer(sigset_t *mask)
{
	/** Block timer signal temporarily */
	printf("Blocking signal %d\n", SIGRTMIN);
	sigemptyset(mask);
	sigaddset(mask, SIGRTMIN);
	if (sigprocmask(SIG_SETMASK, mask, NULL) == -1) {
		pr_stderr("sigprocmask failed");
		exit(EXIT_FAILURE);
	}
}

static void unblock_timer(sigset_t *mask)
{
	/* Unlock the timer signal, so that timer notification
	can be delivered */
	printf("Unblocking signal %d\n", SIGRTMIN);
	if (sigprocmask(SIG_UNBLOCK, mask, NULL) == -1) {
		pr_stderr("sigprocmask failed");
		exit(EXIT_FAILURE);
	}
}

static void init_timer(timer_t *timerid, sigset_t *mask,
		       long long freq_nanosecs)
{
	struct sigevent sev;
	struct itimerspec its;
	struct sigaction sa;

	/** Establish handler for timer signal */
	printf("Establishing handler for signal %d\n", SIGRTMIN);
	sa.sa_flags = SA_SIGINFO;
	sa.sa_sigaction = handler;
	sigemptyset(&sa.sa_mask);
	if (sigaction(SIGRTMIN, &sa, NULL) == -1) {
		pr_stderr("sigaction failed");
		exit(EXIT_FAILURE);
	}

	block_timer(mask);

	/** Create the timer */
	sev.sigev_notify = SIGEV_SIGNAL;
	sev.sigev_signo = SIGRTMIN;
	sev.sigev_value.sival_ptr = timerid;
	if (timer_create(CLOCK_REALTIME, &sev, timerid) == -1) {
		pr_stderr("timer_create failed");
		exit(EXIT_FAILURE);
	}

	/** Start the timer */
	its.it_value.tv_sec = freq_nanosecs / 1000000000;
	its.it_value.tv_nsec = freq_nanosecs % 1000000000;
	its.it_interval.tv_sec = its.it_value.tv_sec;
	its.it_interval.tv_nsec = its.it_value.tv_nsec;

	if (timer_settime(timerid, 0, &its, NULL) == -1) {
		pr_stderr("timer_settime failed");
		exit(EXIT_FAILURE);
	}

	/**
	 * Sleep for a while; meanwhile, the timer may expire multiple times
	 **/
	printf("Sleeping for %d seconds\n", 2);
	sleep(2);

	unblock_timer(mask);
}

void destroy_timer(timer_t *timerid)
{
	timer_delete(*timerid);
}

int main(int argc, char *argv[])
{
	timer_t timerid;
	sigset_t mask;
	int blocked = -1;

	init_signal();

	init_timer(&timerid, &mask, (long long)500000000);

	while (0 == SIGNAL_TERMINATE) {
		/**
		 * 如果你尝试调用 sleep(0.5)，编译器会将 0.5 截断为 0，因为 sleep
		 * 函数的参数是 unsigned int
		 * */
		usleep(500000);
		pr_info("I am running");

		/**
		 * 阻塞定时器，定时器的信号将不会传递至该进程，句柄将不会被调用，定时
		 * 器的超时次数会被记录
		 * */
		if (count > 0 && count % 2 == 1 && blocked != 1) {
			pr_info("block_timer count:%d", count);
			block_timer(&mask);

			blocked = 1;
		}

		/**
		 * 取消阻塞定时器，定时器的信号传递至该进程，句柄继续被调用
		 */
		if (count > 0 && count % 2 == 0 && blocked != 0) {
			pr_info("unblock_timer count:%d", count);
			unblock_timer(&mask);

			blocked = 0;
		}
	}

	destroy_timer(&timerid);

	pr_info("exit success");

	exit(EXIT_SUCCESS);
}
