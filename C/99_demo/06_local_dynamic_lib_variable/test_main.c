#include <stdio.h>

#include "../../common/utils.h"

/**
 * 进程和库中的全局变量同名:
 * - 若没有加 static 则表示同一个对象;
 * - 若加 static 则表示不同对象；（直接访问时，库中的变量被覆盖，会直接访问到进程中非库
 *   定义的变量，相当于OO编程中的private；若要访问库中的变量，需要设置对应的 get 函数）
 */
struct test {
	int v;
} X;

static struct s_test {
	int v;
} S_X;

/**
 * 进程打印函数（库打印函数同名）
 * 直接访问时，库中的同名函数无法调用，被进程中非库定义的函数覆盖；即使在库的内部函数中
 * 调用，也会调用到进程中定义的函数；
 */
void pr_x()
{
	pr_info("main p:%p, val:%d, static_p:%p, static_val:%d",
		&X, X.v, &S_X, S_X.v);
}

/** 进程打印函数（库打印函数同名） */
void pr_coredump(int *a, int *b)
{
	pr_info("main p:%p, val:%d, static_p:%p, static_val:%d, a:%d, b:%d",
		&X, X.v, &S_X, S_X.v, *a, *b);
}

int main(void)
{
	X.v = 10;
	S_X.v = 10;

	/** 调用：进程中非库定义的函数 */
	pr_x();

	/** 调用：库定义的函数 */
	lib_do_x();

	/** 调用：进程中非库定义的函数 */
	pr_x();

	/** 调用：进程中非库定义的函数 */
	pr_coredump(&(X.v), &(S_X.v));

	/** 调用：库定义的函数 */
	lib_coredump();
}
