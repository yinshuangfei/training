//
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <pthread.h>
#include <assert.h>

#include "pool.h"

void * ThreadPoolRoutine(void * arg); 


/**
 * function:       ThreadPoolAddWorkLimit
 * description:    向线程池投递任务,无空闲线程则阻塞
 * input param:    pthis   线程池指针
 * process 回调函数
 * arg     回调函数参数
 * return Val:     0       成功
 *                  -1      失败
 */     
int
ThreadPoolAddWorkLimit(void * pthis, void * (* process)(void * arg), void * arg){ 
    // int FreeThreadNum = 0;
    // int CurrentPthreadNum = 0;
        //         
    CThread_pool_t * pool = (CThread_pool_t *)pthis;
        //                 
    /*为添加的任务队列节点分配内存*/
    worker_t * newworker  = (worker_t *)malloc(sizeof(worker_t)); 
    if(NULL == newworker) 
        return -1;
        //                                         
    newworker->process  = process;  // 回调函数,在线程ThreadPoolRoutine()中执行
    newworker->arg      = arg;      // 回调函数参数
    newworker->next     = NULL;      

    pthread_mutex_lock(&(pool->queue_lock));
        //                                                                 
    /*插入新任务队列节点*/
    worker_t * member = pool->queue_head;   // 指向任务队列链表整体
    if(member != NULL) {
        while(member->next != NULL) // 队列中有节点
            member = member->next;  // member指针往后移动
    
        member->next = newworker;   // 插入到队列链表尾部
    } else 
        pool->queue_head = newworker; // 插入到队列链表头

    assert(pool->queue_head != NULL);
    pool->current_wait_queue_num++; // 等待队列加1

    /*空闲的线程= 当前线程池存放的线程 - 当前已经执行任务和已分配任务的线程数目和*/
    int FreeThreadNum = pool->current_pthread_num - pool->current_pthread_task_num;
    /*如果没有空闲线程且池中当前线程数不超过可容纳最大线程*/
    if((0 == FreeThreadNum) && (pool->current_pthread_num < pool->max_thread_num)) {
        int CurrentPthreadNum = pool->current_pthread_num;
        /*新增线程*/
        pool->threadid = (pthread_t *)realloc(pool->threadid, 
            (CurrentPthreadNum+1) * sizeof(pthread_t));
        pthread_create(&(pool->threadid[CurrentPthreadNum]),
            NULL, ThreadPoolRoutine, (void *)pool);
        
        /*当前线程池中线程总数加1*/                                   
        pool->current_pthread_num++;
        /*分配任务线程数加1*/
        pool->current_pthread_task_num++;
        pthread_mutex_unlock(&(pool->queue_lock));
        /*发送信号给一个处与条件阻塞等待状态的线程*/
        pthread_cond_signal(&(pool->queue_ready));
        return 0;
    }
    pool->current_pthread_task_num++;
    pthread_mutex_unlock(&(pool->queue_lock));
    /*发送信号给一个处与条件阻塞等待状态的线程*/
    pthread_cond_signal(&(pool->queue_ready));
    //  usleep(10);  //看情况  
    return 0;
}


/**
 * function:       ThreadPoolAddWorkUnlimit
 * description:    向线程池投递任务
 * input param:    pthis   线程池指针
 * process 回调函数
 * arg     回调函数参数
 * return Valr:    0       成功
 *                  -1      失败
 */
int
ThreadPoolAddWorkUnlimit(void * pthis, void * (* process)(void * arg), void * arg)
{
    // int FreeThreadNum = 0;
    // int CurrentPthreadNum = 0;
         
    CThread_pool_t * pool = (CThread_pool_t *)pthis;
                         
    /*给新任务队列节点分配内存*/
    worker_t * newworker = (worker_t *)malloc(sizeof(worker_t));
    if(NULL == newworker)
        return -1;
                                                 
    newworker->process  = process;  // 回调函数
    newworker->arg      = arg;      // 回调函数参数
    newworker->next     = NULL;
                                                                
    pthread_mutex_lock(&(pool->queue_lock));
                                                                         
    /*新节点插入任务队列链表操作*/
    worker_t * member = pool->queue_head;
    if(member != NULL) {
        while(member->next != NULL)
            member = member->next;
        
        member->next = newworker;       // 插入队列链表尾部
    } else 
        pool->queue_head = newworker;   // 插入到头(也就是第一个节点,之前链表没有节点)
        
    assert(pool->queue_head != NULL);
    pool->current_wait_queue_num++;     // 当前等待队列的的任务数目+1
        
    int FreeThreadNum = pool->current_pthread_num - pool->current_pthread_task_num;
        
    /*只判断是否没有空闲线程*/
    if(0 == FreeThreadNum) {
        int CurrentPthreadNum = pool->current_pthread_num;

        pool->threadid = (pthread_t *)realloc(pool->threadid,
            (CurrentPthreadNum+1)*sizeof(pthread_t));
        pthread_create(&(pool->threadid[CurrentPthreadNum]),NULL,
            ThreadPoolRoutine, (void *)pool);
        pool->current_pthread_num++;
        
        if(pool->current_pthread_num > pool->max_thread_num)
            pool->max_thread_num = pool->current_pthread_num;
            
        pool->current_pthread_task_num++;
        pthread_mutex_unlock(&(pool->queue_lock));
        pthread_cond_signal(&(pool->queue_ready));
        return 0;
    }
        
    pool->current_pthread_task_num++;
    pthread_mutex_unlock(&(pool->queue_lock));
    pthread_cond_signal(&(pool->queue_ready));
    //  usleep(10);    
    return 0;   
}

/**
 *  function:       ThreadPoolGetThreadMaxNum
 *  description:    获取线程池可容纳的最大线程数
 *  input param:    pthis   线程池指针
 *  return val:     线程池可容纳的最大线程数
 */     
int ThreadPoolGetThreadMaxNum(void * pthis){
    int num = 0;   
    CThread_pool_t * pool = (CThread_pool_t *)pthis;
                
    pthread_mutex_lock(&(pool->queue_lock));
    num = pool->max_thread_num;
    pthread_mutex_unlock(&(pool->queue_lock));
                                
    return num;
}

/**
 *  function:       ThreadPoolGetCurrentThreadNum
 *  description:    获取线程池存放的线程数
 *  input param:    pthis   线程池指针
 *  return Val:     线程池存放的线程数
 */     
int ThreadPoolGetCurrentThreadNum(void * pthis){
    int num = 0;
    CThread_pool_t * pool = (CThread_pool_t *)pthis;
                
    pthread_mutex_lock(&(pool->queue_lock));
    num = pool->current_pthread_num;
    pthread_mutex_unlock(&(pool->queue_lock));
                                
    return num;       
}

/**
 *  function:       ThreadPoolGetCurrentTaskThreadNum
 *  description:    获取当前正在执行任务和已经分配任务的线程数目和
 *  input param:    pthis   线程池指针
 *  return Val:     当前正在执行任务和已经分配任务的线程数目和
 */   
int
ThreadPoolGetCurrentTaskThreadNum(void * pthis){
    int num = 0;
    CThread_pool_t * pool = (CThread_pool_t *)pthis;
                
    pthread_mutex_lock(&(pool->queue_lock));
    num = pool->current_pthread_task_num;
    pthread_mutex_unlock(&(pool->queue_lock));
                                
    return num;   
}

/**
 *  function:       ThreadPoolGetCurrentWaitTaskNum
 *  description:    获取线程池等待队列任务数
 *  input param:    pthis   线程池指针
 *  return Val:     等待队列任务数
 */     
int
ThreadPoolGetCurrentWaitTaskNum(void * pthis){
    int num = 0;
    CThread_pool_t * pool = (CThread_pool_t *)pthis;
                
    pthread_mutex_lock(&(pool->queue_lock));
    num = pool->current_wait_queue_num;
    pthread_mutex_unlock(&(pool->queue_lock));
                                
    return num;   
}


/**
 * function:       ThreadPoolDestroy
 * description:    销毁线程池
 * input param:    pthis   线程池指针
 * return Val:     0       成功
 *                  -1      失败
 */     
int ThreadPoolDestroy(void * pthis){
    int i;
    CThread_pool_t * pool = (CThread_pool_t *)pthis;
                
    if(pool->shutdown)      // 已销毁
        return -1;
                                        
    pool->shutdown = 1;     // 销毁标志置位
                                                
    /*唤醒所有pthread_cond_wait()等待线程*/
    pthread_cond_broadcast(&(pool->queue_ready));
    for(i=0; i<pool->current_pthread_num; i++)
        pthread_join(pool->threadid[i], NULL);  // 等待所有线程执行结束
                                                                            
    free(pool->threadid);   // 释放

    /*销毁任务队列链表*/
    worker_t * head = NULL;
    while(pool->queue_head != NULL) {
        head = pool->queue_head;
        pool->queue_head = pool->queue_head->next;
        free(head);    
    }

    /*销毁锁*/
    pthread_mutex_destroy(&(pool->queue_lock));
    pthread_cond_destroy(&(pool->queue_ready));

    free(pool);
    pool = NULL;
    return 0;
}


void * ThreadPoolRoutine(void * arg); 
/**
 * function:       ThreadPoolRoutine
 * description:    线程池中运行的线程
 * input param:    arg  线程池指针
 */     
void * 
ThreadPoolRoutine(void * arg)
{
    CThread_pool_t * pool = (CThread_pool_t *)arg;
            
    while(1) {
        /*上锁,pthread_cond_wait()调用会解锁*/
        pthread_mutex_lock(&(pool->queue_lock));
                                            
        /*队列没有等待任务*/
        while((pool->current_wait_queue_num == 0) && (!pool->shutdown)) {
            /*条件锁阻塞等待条件信号*/
            pthread_cond_wait(&(pool->queue_ready), &(pool->queue_lock));
        }
        
        if(pool->shutdown) {
            pthread_mutex_unlock(&(pool->queue_lock));
            pthread_exit(NULL);         // 释放线程
        }
    
        
        assert(pool->current_wait_queue_num != 0);
        assert(pool->queue_head != NULL);
        
        
        pool->current_wait_queue_num--;             // 等待任务减1,准备执行任务
        worker_t * worker = pool->queue_head;       // 去等待队列任务节点头
        pool->queue_head = worker->next;            // 链表后移     
        pthread_mutex_unlock(&(pool->queue_lock));

        (* (worker->process))(worker->arg);         // 执行回调函数
        
        pthread_mutex_lock(&(pool->queue_lock));
        pool->current_pthread_task_num--;           // 函数执行结束
        free(worker);                               // 释放任务结点
        worker = NULL;
        
        
        if((pool->current_pthread_num - pool->current_pthread_task_num) > pool->free_pthread_num) {
            pthread_mutex_unlock(&(pool->queue_lock));
            break;  // 当线程池中空闲线程超过 free_pthread_num 则将线程释放回操作系统
        }
        pthread_mutex_unlock(&(pool->queue_lock));    
    }

    pool->current_pthread_num--;    // 当前线程数减1
    pthread_exit(NULL);             // 释放线程

    return (void *)NULL;
}


/* 创建线程池 */
CThread_pool_t * 
ThreadPoolConstruct(int max_num, int free_num)
{
    int i = 0;
        
    CThread_pool_t * pool = (CThread_pool_t *)malloc(sizeof(CThread_pool_t));
    if(NULL == pool)
        return NULL;
                                
    memset(pool, 0, sizeof(CThread_pool_t));
                                        
    /*初始化互斥锁*/
    pthread_mutex_init(&(pool->queue_lock), NULL);
    /*初始化条件变量*/
    pthread_cond_init(&(pool->queue_ready), NULL);
                                                            
    pool->queue_head                = NULL;
    pool->max_thread_num            = max_num; // 线程池可容纳的最大线程数
    pool->current_wait_queue_num    = 0;
    pool->current_pthread_task_num  = 0;
    pool->shutdown                  = 0;
    pool->current_pthread_num       = 0;
    pool->free_pthread_num          = free_num; // 线程池允许存在最大空闲线程
    pool->threadid                  = NULL;
    pool->threadid                  = (pthread_t *)malloc(max_num*sizeof(pthread_t));

    /*该函数指针赋值*/
    pool->AddWorkUnlimit            = ThreadPoolAddWorkUnlimit;
    pool->AddWorkLimit              = ThreadPoolAddWorkLimit;
    pool->Destroy                   = ThreadPoolDestroy;
    pool->GetThreadMaxNum           = ThreadPoolGetThreadMaxNum;
    pool->GetCurrentThreadNum       = ThreadPoolGetCurrentThreadNum;
    pool->GetCurrentTaskThreadNum   = ThreadPoolGetCurrentTaskThreadNum;
    pool->GetCurrentWaitTaskNum     = ThreadPoolGetCurrentWaitTaskNum;
                                                                                                                                    
    for(i=0; i<max_num; i++) {
        pool->current_pthread_num++;    // 当前池中的线程数
        /*创建线程*/
        pthread_create(&(pool->threadid[i]), NULL, ThreadPoolRoutine, (void *)pool);
        usleep(1000);        
    }

    return pool;
}

/**
 *  function:       ThreadPoolConstructDefault
 *  description:    创建线程池,以默认的方式初始化,未创建线程
 *
 *  return Val:     线程池指针                 
 */     
CThread_pool_t * 
ThreadPoolConstructDefault(void){
    CThread_pool_t * pool = (CThread_pool_t *)malloc(sizeof(CThread_pool_t));
    if(NULL == pool)
        return NULL;
                            
    memset(pool, 0, sizeof(CThread_pool_t));
                                    
    pthread_mutex_init(&(pool->queue_lock), NULL);
    pthread_cond_init(&(pool->queue_ready), NULL);
                                            
    pool->queue_head                = NULL;
    pool->max_thread_num            = DEFAULT_MAX_THREAD_NUM; // 默认值
    pool->current_wait_queue_num    = 0;
    pool->current_pthread_task_num  = 0;
    pool->shutdown                  = 0;
    pool->current_pthread_num       = 0;
    pool->free_pthread_num          = DEFAULT_FREE_THREAD_NUM; // 默认值
    pool->threadid                  = NULL;
    /*该函数指针赋值*/
    pool->AddWorkUnlimit            = ThreadPoolAddWorkUnlimit;
    pool->AddWorkLimit              = ThreadPoolAddWorkLimit;
    pool->Destroy                   = ThreadPoolDestroy;
    pool->GetThreadMaxNum           = ThreadPoolGetThreadMaxNum;
    pool->GetCurrentThreadNum       = ThreadPoolGetCurrentThreadNum;
    pool->GetCurrentTaskThreadNum   = ThreadPoolGetCurrentTaskThreadNum;
    pool->GetCurrentWaitTaskNum     = ThreadPoolGetCurrentWaitTaskNum;
    
    return pool;
}
