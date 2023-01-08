#include<stdio.h>
#include<stdlib.h>
#include<errno.h>
#include<string.h>
#include<netdb.h>
#include<sys/types.h>
#include<netinet/in.h>
#include<sys/socket.h>
#include<sys/wait.h>
#include<unistd.h>
#include<arpa/inet.h>

#define SERVER_PORT 12316
#define MAX_DATASIZE 4096
#define SERVER_IP_ADDR "192.168.1.12"

void 
main ( int argc, char *argv[] ) {
	int sock_fd;
	int sin_size;
	int size;
	int pid;
	char buff[MAX_DATASIZE];
	struct sockaddr_in server_addr;
	
	//创建套接字
	if (-1 == ( sock_fd=socket(AF_INET,SOCK_STREAM,0))){
		printf("socket init error, exit now.\n");
		exit(1);
	}

	//填写套接字描述符
	server_addr.sin_family=AF_INET;//internet连接的默认值
	server_addr.sin_port=htons(SERVER_PORT);//将端口号转换为标准格式，为0则由系统自己分配一个未占用的端口号
	server_addr.sin_addr.s_addr=inet_addr(SERVER_IP_ADDR);
	bzero(&(server_addr.sin_zero),8);//sockaddr_in结构体的剩余字节处用0填充
	
	//连接套接字
	if ( -1 == connect( sock_fd,( struct sockaddr * )&server_addr,sizeof(struct sockaddr))){
		printf("socket connect error, exit now.\n");
		exit(1);
	}
	
	
	//提示信息
	printf("my pid is: %d, I'm connected.\n",getpid());
	printf("Input 'exit' to exit.\n");
	
	
	//子线程用于接受
	if ( 0 == (pid=fork())){
		while ( 1 ){
			if ( -1 == (size=recv(sock_fd,buff,sizeof(buff),0))){
				printf("socket receive error, exit now.\n");
				exit(1);
			}
			else{
				buff[size]='\0';
				printf("%s\n",buff);
			}
		}
	}
	//父线程用于发送
	else{
		while (1) {
			printf(">");
			gets(buff);
			//检测是否要离开
			if ( 0 == strcmp("exit",buff) ){  
				printf("Welcome to use next time.\n");
				close(sock_fd);
				kill(pid,9);
				exit(0);
			}
			//发送消息
			if ( -1 == send(sock_fd,buff,strlen(buff),0)){
				printf("socket send error, try again.\n");
			}
		}
	
	}
			
	close(sock_fd);
	printf("my service is satisfied, bye...\n");
	exit(0);
}







