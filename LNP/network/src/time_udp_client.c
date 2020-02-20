#include<stdio.h>
#include<stdlib.h>
#include<netdb.h>
#include<sys/socket.h>
#include<arpa/inet.h>
#include<unistd.h>
#include<string.h>
#include<memory.h>
#include<signal.h>
#include<time.h>

int main(int argc,char *argv[]){
	if(argc < 3){
		printf("usage: %s ip port\n",argv[0]);
		exit(1);
	}

	/*步骤1：创建socket*/
	int sockfd = socket(AF_INET,SOCK_DGRAM,0);
	if(sockfd < 0){
		perror("socket error");
		exit(1);
	}

	/*步骤2：调用recvfrom和sendto等函数
	 *	 和服务器端双向同信
	 * */
	struct sockaddr_in serveraddr;
	memset(&serveraddr,0,sizeof(serveraddr));
	serveraddr.sin_family = AF_INET; //IPV4
	serveraddr.sin_port = htons(atoi(argv[2])); //port
	inet_pton(AF_INET,argv[1],
			&serveraddr.sin_addr.s_addr); //ip
	char buffer[1024] = "hello llc";
	//向服务器端发送数据报文
	if(sendto(sockfd,buffer,sizeof(buffer),0,
		(struct sockaddr*)&serveraddr,
			sizeof(serveraddr)) < 0){
		perror("sendto error");
		exit(1);
	}
	else{
		//接收服务器端发送的数据报文
		memset(buffer,0,sizeof(buffer));
		if(recv(sockfd,buffer,sizeof(buffer),0) < 0){
			perror("recv error");
			exit(1);
		}
		else{
			printf("%s",buffer);
		}
	}
	close(sockfd);

	return 0;
}
