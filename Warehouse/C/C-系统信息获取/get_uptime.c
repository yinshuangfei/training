#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/stat.h>
#include <sys/sysinfo.h>
 
int main(int argc, char* argv[]){
    struct sysinfo info;
    
    if(sysinfo(&info) != 0)
    {
        printf("error\n");
        return -1;
    } else {
        printf("%ld seconds\n", (long)info.uptime);
    }

    return 0;
}
