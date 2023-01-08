 
#include <stdio.h>  
#include <string.h>  
#include <sys/socket.h>  
#include <arpa/inet.h>  
#include <unistd.h>  
#include <netinet/in.h>  
#include <stdlib.h> 
#include "fcntl.h"
  
#define MAXLINE 1024

size_t transfer(int sockfd, char * buff, size_t size, int i){
    printf("send once size=%ld, i=%d...\n", size, i);
    send(sockfd, buff, size, 0);
}

int main (){
    int sockfd;  
    char file_url[128];  
    char *IP = "192.168.3.210";
    unsigned short port = 8880;
    char * endword = "*#*#i_want_end#*#*";

    char recvline[MAXLINE];
    struct sockaddr_in servaddr; 

   /* 初始化套接字 */
    if((sockfd = socket(AF_INET,SOCK_STREAM,0)) < 0){
        printf("socket error\n");  
        return -1;
    }

    bzero(&servaddr,sizeof(servaddr));  
    servaddr.sin_family = AF_INET;  
    servaddr.sin_port = htons(port);

    // 设置IP
    if(inet_pton(AF_INET,IP,&servaddr.sin_addr) <= 0){
        printf("inet_pton error\n");  
        return -1;
    }

    // 连接套接字
    if(connect(sockfd,(struct sockaddr *)&servaddr,sizeof(servaddr)) < 0){
        printf("connect error\n");  
        return -1;
    }
    
    printf("please input filename to transfer ('quit' to exit):\n");

    int i = 0;
    while ( scanf("%s", file_url) >= 0 && strcmp("quit", file_url) != 0 ) {
        if (access(file_url, 0) != 0) {
            printf("file: '%s' not exist, continue ...\n", file_url);
            continue; 
        }

        int fd = open(file_url, O_RDONLY);
        int size = 0;
        while( (size = read(fd, recvline, MAXLINE)) > 0 ){
            transfer(sockfd, recvline, size, ++i);
        }
        close(fd);
        bzero(file_url, sizeof(file_url));

        transfer(sockfd, "", size, ++i);
        printf("transfer successed!\n");
        break;
        // printf("please input filename to transfer ('quit' to exit):\n");
    }

    return 0;
 }


