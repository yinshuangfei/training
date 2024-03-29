/* 
 *  fork_test.c 
 *  version 1 
 *  Created on: 2010-5-29 
 *      Author: wangth 
 */  
#include <unistd.h>  
#include <stdio.h>   

int main ()   
{   
    pid_t fpid; //fpid表示fork函数返回的值  

    int count=0;  
    fpid=fork();   
    if (fpid < 0)   
        printf("error in fork!\n");   
    else if (fpid == 0) {   
        printf("i am the child process, my process id is %d \n",getpid());   
        printf("我是爹的儿子\n");//对某些人来说中文看着更直白。  
        count++;  
        sleep(5);
    }   
    else {   
        printf("i am the parent process, my process id is %d\n",getpid());   
        printf("我是孩子他爹\n");  
        count++;  
    
        sleep(10);
    }   
    printf("统计结果是: %d,我是%d\n",count,getpid());  
    return 0;  
}