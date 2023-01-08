#include <stdio.h>
#include <arpa/inet.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>

int main(int argc, char* argv[])
{
        unsigned char mac[6]={0x84,0x7B,0xEB,0x13,0x27,0x97};//目的主机MAC地址，例如: 00:10:20:30:40:50
        unsigned char packet[102];
        struct sockaddr_in addr;
        int sockfd, i,j, on = 1;

        //构建magic packet
        for(i=0;i<6;i++)            
            packet[i] = 0xFF;    
            
        for(i=1;i<17;i++)
            for(j=0;j<6;j++)
                packet[i*6+j] = mac[j];
        
        //UDP
        sockfd = socket(AF_INET, SOCK_DGRAM, 0);
        //广播
        setsockopt(sockfd, SOL_SOCKET, SO_BROADCAST,&on, sizeof(on));
        if(sockfd < 0)
            exit(0);
                
        memset((void*)&addr, 0, sizeof(addr));
        addr.sin_family = AF_INET;
        addr.sin_port = htons(10000);
        addr.sin_addr.s_addr = inet_addr("255.255.255.255");//广播地址
        
        sendto(sockfd, packet, sizeof(packet), 0, (struct sockaddr *)&addr, sizeof(addr));
       
        
        printf("end\n");  
} 
