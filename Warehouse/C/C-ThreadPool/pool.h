#ifndef POOL_H
#define POOL_H

#include <pthread.h>

/*线程池可容纳最大线程数*/
#define DEFAULT_MAX_THREAD_NUM      100

/*线程池允许最大的空闲线程，超过则将线程释放回操作系统*/
#define DEFAULT_FREE_THREAD_NUM     10

typedef struct worker_t         worker_t;
typedef struct CThread_pool_t   CThread_pool_t;

// 线程池任务结点
struct worker_t {
    void * (* process)(void * arg); /*回调函数*/
    int    paratype;                /*函数类型(预留)*/
    void * arg;                     /*回调函数参数*/
    struct worker_t * next;         /*链接下一个任务节点*/
};

/*线程控制器*/
struct CThread_pool_t {
    pthread_mutex_t queue_lock;     /*互斥锁*/
    pthread_cond_t  queue_ready;    /*条件变量*/
                
    worker_t * queue_head;          /*任务节点链表 保存所有投递的任务*/
    int shutdown;                   /*线程池销毁标志 1-销毁*/
    pthread_t * threadid;           /*线程ID数组*/
                                
    int max_thread_num;             /*线程池可容纳最大线程数*/
    int current_pthread_num;        /*当前线程池存放的线程*/
    int current_pthread_task_num;   /*当前已经执行任务和已分配任务的线程数目和*/
    int current_wait_queue_num;     /*当前等待队列的的任务数目*/
    int free_pthread_num;           /*线程池允许最大的空闲线程数*/

    /**
     * function:       ThreadPoolAddWorkUnlimit
     * description:    向线程池投递任务
     * input param:    pthis   线程池指针
     * process 回调函数
     * arg     回调函数参数
     * return Valr:    0       成功
     *                  -1      失败
     */
    int (* AddWorkUnlimit)(void * pthis, void * (* process)(void * arg), void * arg);
    
    /**
     * function:       ThreadPoolAddWorkLimit
     * description:    向线程池投递任务,无空闲线程则阻塞
     * input param:    pthis   线程池指针
     * process 回调函数
     * arg     回调函数参数
     * return Val:     0       成功
     *                  -1      失败
     */ 
    int (* AddWorkLimit)(void * pthis, void * (* process)(void * arg), void * arg);
    
    /**
     * function:       ThreadPoolGetThreadMaxNum
     * description:    获取线程池可容纳的最大线程数
     * input param:    pthis   线程池指针
     */
    int (* GetThreadMaxNum)(void * pthis);
    
    /**
     * function:       ThreadPoolGetCurrentThreadNum
     * description:    获取线程池存放的线程数
     * input param:    pthis   线程池指针
     * return Val:     线程池存放的线程数
     */
    int (* GetCurrentThreadNum)(void * pthis);
    
    /**
     * function:       ThreadPoolGetCurrentTaskThreadNum
     * description:    获取当前正在执行任务和已经分配任务的线程数目和
     * input param:    pthis   线程池指针
     * return Val:     当前正在执行任务和已经分配任务的线程数目和
     */
    int (* GetCurrentTaskThreadNum)(void * pthis);
    
    /**
     * function:       ThreadPoolGetCurrentWaitTaskNum
     * description:    获取线程池等待队列任务数
     * input param:    pthis   线程池指针
     * return Val:     等待队列任务数
     */
    int (* GetCurrentWaitTaskNum)(void * pthis);
    
    /**
     * function:       ThreadPoolDestroy
     * description:    销毁线程池
     * input param:    pthis   线程池指针
     * return Val:     0       成功
     * -1      失败
    */
    int (* Destroy)(void * pthis); 
};

/**
 * function:       ThreadPoolConstruct
 * description:    构建线程池
 * input param:    max_num   线程池可容纳的最大线程数
 * free_num  线程池允许存在的最大空闲线程,超过则将线程释放回操作系统
 * return Val:     线程池指针                 
 */     
CThread_pool_t * ThreadPoolConstruct(int max_num, int free_num);

/**
 * function:       ThreadPoolConstructDefault
 * description:    创建线程池,以默认的方式初始化,未创建线程
 * return Val:     线程池指针                 
 */     
CThread_pool_t * ThreadPoolConstructDefault(void);

#endif 
