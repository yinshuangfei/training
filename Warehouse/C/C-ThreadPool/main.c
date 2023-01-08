#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <pthread.h>
#include <assert.h>

#include "pool.h"

#define __FILENAME__ (strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : __FILE__)
#define LOG(fmt, ...) printf("[Debug]: " fmt " %s:%d\n", ##__VA_ARGS__, __FILENAME__, __LINE__)
#define EXIT(error) do {perror("[Error] " error); exit(EXIT_FAILURE);} while(0)

void DisplayPoolStatus(CThread_pool_t * pPool);
void * thread_1(void * arg);
void * thread_2(void * arg);
void * thread_3(void * arg);
int nKillThread = 0;

int main ( ){
	int t=0;
	assert(t==0);
    
    CThread_pool_t * pThreadPool = NULL;
    pThreadPool = ThreadPoolConstruct(5, 1);

    int nNumInput = 5;
    char LogInput[] = "My name is OK!";
	
    
    DisplayPoolStatus(pThreadPool);
    
    /*可用AddWorkLimit()替换看执行的效果*/
    pThreadPool->AddWorkUnlimit((void *)pThreadPool, (void *)thread_1, (void *)NULL);

/*
 * 没加延迟发现连续投递任务时pthread_cond_wait()会收不到信号pthread_cond_signal() !!
 * 因为AddWorkUnlimit()进去后调用pthread_mutex_lock()把互斥锁锁上,导致pthread_cond_wait()
 * 收不到信号!!也可在AddWorkUnlimit()里面加个延迟,一般情况可能也遇不到这个问题
 */
    usleep(10);
    pThreadPool->AddWorkUnlimit((void *)pThreadPool, (void *)thread_2, (void *)nNumInput);
    usleep(10);
    pThreadPool->AddWorkUnlimit((void *)pThreadPool, (void *)thread_3, (void *)LogInput);
    usleep(10);
    pThreadPool->AddWorkUnlimit((void *)pThreadPool, (void *)thread_3, (void *)LogInput);
    usleep(10);
    pThreadPool->AddWorkUnlimit((void *)pThreadPool, (void *)thread_3, (void *)LogInput);
    usleep(10);
    pThreadPool->AddWorkUnlimit((void *)pThreadPool, (void *)thread_3, (void *)LogInput);
    usleep(10);
    pThreadPool->AddWorkUnlimit((void *)pThreadPool, (void *)thread_3, (void *)LogInput);
    usleep(10);
    DisplayPoolStatus(pThreadPool);

    //nKillThread = 1;
    usleep(100);    /**< 先让线程退出 */
    DisplayPoolStatus(pThreadPool);
    //nKillThread = 2;
    usleep(100);
    DisplayPoolStatus(pThreadPool);
    nKillThread = 3;
    usleep(100);
    DisplayPoolStatus(pThreadPool);


    nKillThread = 1;
    usleep(100);
    nKillThread = 2;
    sleep(3);
    DisplayPoolStatus(pThreadPool);
    pThreadPool->Destroy((void*)pThreadPool);

    return 0;
}

void * thread_1(void * arg){
    printf("Thread 1 is running !\n");
    while(nKillThread != 1)
        usleep(10);
    return NULL;
}

void * thread_2(void * arg){
    int nNum = (int)arg;
            
    printf("Thread 2 is running !\n");
    printf("Get Number %d\n", nNum);
    while(nKillThread != 2)
        usleep(10);
    return NULL;
}

void * thread_3(void * arg){
    char * pLog = (char *)arg;
            
    printf("Thread 3 is running !\n");
    printf("Get String %s\n", pLog);
    while(nKillThread != 3)
        usleep(10);
    return NULL;
}

void DisplayPoolStatus(CThread_pool_t * pPool){
    static int nCount = 1;
            
    printf("****************************\n");
    printf("nCount = %d\n", nCount++);
    printf("max_thread_num = %d\n", pPool->GetThreadMaxNum((void *)pPool));
    printf("current_pthread_num = %d\n", pPool->GetCurrentThreadNum((void *)pPool));
    printf("current_pthread_task_num = %d\n", pPool->GetCurrentTaskThreadNum((void *)pPool));
    printf("current_wait_queue_num = %d\n", pPool->GetCurrentWaitTaskNum((void *)pPool));
    printf("****************************\n");
}
