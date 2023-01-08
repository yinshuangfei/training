#include <stdio.h>   
#include <time.h>
#include "time.h"
/* Remote version of "printime" */   

char ** printtime_1_svc(char **msg,struct svc_req *req)  
{  
    static char * result;
    static char tmp_char[100];  
    time_t rawtime;
               
    FILE *f = fopen("/tmp/rpc_result", "a+");  
    if (f ==NULL) {  
        strcpy(tmp_char,"Error");  
        result = tmp_char;;  
        return (&result);  
    }  
    
    fprintf(f, "%s", *msg);  
    fclose(f);  
    time(&rawtime);  
    sprintf(tmp_char,"Current time is :%s",ctime(&rawtime));  
    result =tmp_char;  
    return (&result);  
}
