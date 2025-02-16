#include <stdio.h>
#include "../../common/utils.h"

struct test {
	int v;
} X;

static struct s_test {
	int v;
} S_X;

/** 设置当前文件中的变量
 * X：所有文件中有效
 * S_X：仅该文件中有效
 */
void __set_x(int val)
{
	X.v = val;
	S_X.v = val;
}

/** 库打印函数（与进程打印函数同名） */
void pr_x()
{
	pr_info("lib p:%p, val:%d, static_p:%p, static_val:%d",
		&X, X.v, &S_X, S_X.v);
}

/** 库打印函数（与进程打印函数同名） */
void pr_coredump()
{
	pr_info("lib p:%p, val:%d, static_p:%p, static_val:%d",
		&X, X.v, &S_X, S_X.v);
}

/** 库特有打印函数 */
void lib_pr_xx()
{
	pr_info("spec lib p:%p, val:%d, static_p:%p, static_val:%d",
		&X, X.v, &S_X, S_X.v);
}

void lib_do_x()
{
	/** 调用：进程中非库定义的函数 */
	pr_x();

	/** 调用：库定义的函数 */
	__set_x(30);

	/** 调用：进程中非库定义的函数 */
	pr_x();

	/** 调用：库定义的函数 */
	lib_pr_xx();
}

/** 出现两个入参不同的调用，但能编译通过 */
void lib_coredump()
{
	/** 调用：进程中非库定义的函数 */
	pr_coredump(&(X.v), &(S_X.v));

	/** 调用：进程中非库定义的函数 (!!!coredump!!!) */
	pr_coredump();
}