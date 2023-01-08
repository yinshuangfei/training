#include<stdlib.h>
#include<stdio.h>
#include<time.h>

/**
int daemon(int nochdir, int noclose);

当nochdir为0时，daemon将更改进城的根目录为root(“/”), 否则继续使用当前工作目录;
当noclose为0时，daemon将进城的STDIN, STDOUT, STDERR都重定向到/dev/null, 否则依然使用原来的设备;

deamon()调用了fork()，如果fork成功，那么父进程就调用_exit(2)退出，
所以看到的错误信息 全部是子进程产生的。如果成功函数返回0，否则返回-1并设置errno。
*/

int main() {
    daemon(1,1);
    
    FILE *file;
    
    time_t rawtime;
    struct tm *timeinfo;

    while (1) {
        time(&rawtime); 
        timeinfo = localtime(&rawtime); 
        sleep(1);
        
        printf("%s", timeinfo);
        file = fopen("/root/ysf/daemon/file.txt", "a");
        fprintf(file,"当前时间为:%s", asctime(timeinfo));
        fclose(file);
    }
    
    return 0;
}

