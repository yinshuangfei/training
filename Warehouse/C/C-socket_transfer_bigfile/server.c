/**
把传输文件的socket和其他功能用的socket分开，也就是用不同的端口。
**/
#include <stdio.h>
#include <sys/types.h>
#include <stdlib.h>
#include <time.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/stat.h>

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>
#include <ctype.h>

#define __FILENAME__ (strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : __FILE__)
#define LOG(fmt, ...) printf("[Debug %s:%d] " fmt "\n", __FILENAME__, __LINE__, ##__VA_ARGS__)
#define EXIT(error) do {perror("[Error] " error); exit(EXIT_FAILURE);} while(0)

int create_server_fd (unsigned int port){
	int serverfd = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (serverfd == -1)
		EXIT("create socket fail");

	struct sockaddr_in server;
	memset(&server, 0, sizeof(server));

	server.sin_family = AF_INET;
	server.sin_port = htons(port);
	server.sin_addr.s_addr = INADDR_ANY;

	if (bind(serverfd,(struct sockaddr *)&server, sizeof(server)) == -1)
		EXIT("bind fail");

	if (listen(serverfd, 10) == -1)
		EXIT("listen fail");

    LOG("Server started, listen port %d\n", port);
	return serverfd;
}

unsigned long get_file_size(const char *path)  
{  
    unsigned long filesize = -1;
    struct stat statbuff;

    if(stat(path, &statbuff) < 0){  
        return filesize;  
    }else{  
        return statbuff.st_size;  
    }
}

/*
 * 与创建套通信
 */
void *process(void* psockfd) {
    int sockfd = *(int*)psockfd;
    char * buff[1024];
    char f_name[256];
    time_t t;
    struct tm * now;
    int fd;
    size_t size;
    int _size = 0;

recv:
    time(&t);
    now = localtime(&t);
    sprintf(f_name, "%d-%d-%d_%d:%d:%d.xxx", 
        now->tm_year + 1900, now->tm_mon + 1, now->tm_mday, now->tm_hour, now->tm_min, now->tm_sec);
    LOG("now %s \n", f_name);

    if (creat(f_name, S_IREAD | S_IWRITE | S_IRGRP | S_IROTH ) == -1 ){
        printf("creat file:%s error, err:%d \n", f_name, -errno);
        close(sockfd);
        return 0;
    }

    fd = open(f_name, O_RDWR);
    while((size = recv(sockfd, buff, sizeof(buff), 0)) > 0){
        LOG("rcve size=%d\n", size);
        write(fd, buff, size);
    }

    close(fd);
    
    if((_size = get_file_size(f_name)) <= 0){
        LOG("file size=%ld, remove it.\n", _size);
        unlink(f_name);
    }

    if(size == 0){
        if(errno == EINTR)
            goto recv;
    }
    LOG("connect close\n");
    close(sockfd);
}

int main ( ){
	int serverfd, connfd;
    pthread_t tid;

    struct sockaddr_in client;
    socklen_t clientlen = sizeof(client);
	unsigned int port = 8880;

    serverfd = create_server_fd(port);

	while (1){
		connfd = accept(serverfd, (struct sockaddr *)&client, &clientlen);
		if (pthread_create(&tid, NULL, process, &connfd) == 0) {
			unsigned char *ip = (unsigned char*)&client.sin_addr.s_addr;
			unsigned short port = client.sin_port;
			LOG("request %u.%u.%u.%u:%5u", ip[0], ip[1], ip[2], ip[3], port);
		} else {
			EXIT("create thread fail");
		}
	}
    return 0;
}