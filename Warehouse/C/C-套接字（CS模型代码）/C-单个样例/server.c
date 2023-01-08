#include<stdio.h>
#include<stdlib.h>
#include<errno.h>
#include<string.h>
#include<sys/types.h>
#include<netinet/in.h>
#include<sys/socket.h>
#include<sys/wait.h>
#include<unistd.h>
#include<arpa/inet.h>

#define SERVER_PORT 12316
#define MAX_CONNECTED 20
#define MAX_DATASIZE 4096
#define SERVER_IP_ADDR "192.168.1.12"

int sock_fd,client_fd;
int sin_size;
int size;
char buff[MAX_DATASIZE];
char resend_buff[MAX_DATASIZE];
struct sockaddr_in local_addr;
struct sockaddr_in remote_addr;

void socket_init(void);


void socket_init(void){
	//创建套接字
	if (-1 == ( sock_fd=socket(AF_INET,SOCK_STREAM,0))){
		printf("socket init error, exit now.\n");
		exit(1);
	}

	//填写套接字描述符
	local_addr.sin_family=AF_INET;//internet连接的默认值
	local_addr.sin_port=htons(SERVER_PORT);//将端口号转换为标准格式，为0则由系统自己分配一个未占用的端口号
	local_addr.sin_addr.s_addr=INADDR_ANY;//系统自动填入本机IP地址
	//local_addr.sin_addr.s_addr=inet_addr(SERVER_IP_ADDR);//将IP地址转换为可用的地址
	bzero(&(local_addr.sin_zero),8);//sockaddr_in结构体的剩余字节处用0填充
	
	//绑定套接字
	if ( -1 == bind( sock_fd,( struct sockaddr * )&local_addr,sizeof(struct sockaddr))){
		printf("socket bind error, exit now.\n");
		exit(1);
	}
	
	//设置监听
	if ( -1 == listen(sock_fd,MAX_CONNECTED)){
		printf("socket listen error, exit now.\n");
		exit(1);
	}
	
	//提示语句
	printf("my pid is: %d; \nmy IP is :%s;\nI'm listening on port:%d, waiting for you ... \n", \
			getpid(),(char *)inet_ntoa(local_addr.sin_addr),SERVER_PORT);
}


void main ( void ) {

	socket_init();
	
	//循环，提供服务
	while (1){
		//程序开始监听，在这个地方会阻塞，不消耗cpu
		sin_size=sizeof(struct sockaddr_in);
		if ( -1 == (client_fd = accept( sock_fd,(struct sockaddr *)&remote_addr,&sin_size))){
			printf("socket accept error, exit now.\n");
			continue;
		}
	
		//子进程
		if( 0 == fork()){
			//提示信息
			printf("Son process(PID=%d): received a connection from %s, the socket ID is %d\n",\
				getpid(),(char *)inet_ntoa(remote_addr.sin_addr),client_fd);
			
			while ( 1 ){
				if ( -1 == (size=recv(client_fd,buff,sizeof(buff),0))){
					printf("socket receive error, exit now.\n");
					exit(1);
				}
				//检测连接是否断开
				else if( 0 == size){
					printf("the connetc shutdown, now close the connection.\n");
					break;
				}
				else{
					buff[size]='\0';
					printf("[Received ID:%d] content:%s\n",client_fd,buff);
					sprintf(resend_buff,"[Received ID:%d] content:%s\n",client_fd,buff);
					//发送给其他人
					if ( -1 == send(client_fd,resend_buff,strlen(resend_buff),0)){
						printf("socket send error, try again.\n");
					}
				}
			}				
			close(client_fd);
			exit(0);
		}
		//父进程
		else{
			printf("Father process(PID=%d): I will not provode the tille sevice.\n",getpid());
		}
	}
	
	printf("Service ended, bye...\n");
	close(sock_fd);
	exit(0);
}







