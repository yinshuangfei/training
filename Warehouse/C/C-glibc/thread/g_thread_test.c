#include <glib.h>

static gpointer worker_thread(gpointer data)
{
    int num = *(int *)data;

    printf("thread start!\n");

    printf("data = %d!\n", num);

    printf("thread end!\n");
}


void main()
{
    printf("main start!\n");
    int loop = 10;

    /* g_thread_new() 函数执行完就立即执行线程 */
    GThread *t = g_thread_new("cfs_loop", worker_thread, &loop);

    sleep(3);
    printf("main end!\n");
}

