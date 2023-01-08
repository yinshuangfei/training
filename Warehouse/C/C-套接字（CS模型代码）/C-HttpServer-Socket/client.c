#include <stdio.h>  
#include <string.h>  
#include <sys/socket.h>  
#include <arpa/inet.h>  
#include <unistd.h>  
#include <netinet/in.h>  
#include <stdlib.h>  
#include <errno.h>  
  
#define MAXLINE 1024  
  
int main(int argc, char * argv[])  
{  
    int sockfd,n;  
    char recvline[MAXLINE];  
    struct sockaddr_in servaddr;  
    char dns[32];  
    char url[128];  

    if (argc < 2) {
	printf("param less then 1\n");
	return -1;
    }

    char *IP = argv[1];
    printf("ip is: %s\n", argv[1]);  

    char *buf = "GET / HTTP/1.1\r\n\  
Host: 192.168.3.214\r\n\  
Proxy-Connection: keep-alive\r\n\  
Cache-Control: max-age=0\r\n\  
Accept: text/html,application/xhtml+xml,application/xml;q=0.9,image/webp,*/*;q=0.8\r\n\  
User-Agent: xxx/5.0 (feifei 6.3; WOW64) AppleXXX/537.36 (KHTML, like Gecko) web_feifeifei/35.0.1916.153 Safari/537.36\r\n\  
Accept-Encoding: gzip,deflate,sdch\r\n\  
Accept-Language: zh-CN,zh;q=0.8,en;q=0.6\r\n\  
\r\n"; 

    /* 初始化套接字 */
    if((sockfd = socket(AF_INET,SOCK_STREAM,0)) < 0){
        printf("socket error\n");  
        return -1;
    }

    bzero(&servaddr,sizeof(servaddr));  
    servaddr.sin_family = AF_INET;  
    servaddr.sin_port = htons(8888);

    // 设置IP
    if(inet_pton(AF_INET,IP,&servaddr.sin_addr) <= 0){
        printf("inet_pton error\n");  
        return -1;
    }

    // 连接套接字
    if(connect(sockfd,(struct sockaddr *)&servaddr,sizeof(servaddr)) < 0){
        printf("connect error(%d)\n", errno);  
        return -1;
    }

    // 发送请求
    write(sockfd,buf,strlen(buf));  
    printf("%s\n\n",buf);

    //接受请求
    while((n = read(sockfd,recvline,MAXLINE)) > 0)  
    {  
        recvline[n] = 0;  
        if(fputs(recvline,stdout) == EOF)  
            printf("fputs error\n");  
    }  
    if(n < 0) { 
        printf("read error\n"); 
        exit(1);
    }
    printf("all ok now\n");  
    exit(0);  
} 
