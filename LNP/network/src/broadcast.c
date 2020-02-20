#include<netdb.h>
#include<sys/socket.h>
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<arpa/inet.h>

int main(int argc,char *argv[]){
	if(argc < 3){
		fprintf(stderr,"usage: %s ip port\n",argv[0]);
		exit(1);
	}
	
	int sockfd = socket(AF_INET,SOCK_DGRAM,0);
	if(sockfd < 0){
		perror("socket error");
		exit(1);
	}
	int opt = 1;
	//采用广播方式发送
	setsockopt(sockfd,SOL_SOCKET,
			SO_BROADCAST,&opt,sizeof(opt));

	struct sockaddr_in serveraddr;
	memset(&serveraddr,0,sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_port = htons(atoi(argv[2]));
	inet_pton(AF_INET,argv[1],
			&serveraddr.sin_addr.s_addr);
	printf("I will broadcast...\n");
	char *info = "hello llc";
	size_t size = strlen(info) * sizeof(char);
	if(sendto(sockfd,info,size,0,
			(struct sockaddr*)&serveraddr,
				sizeof(serveraddr)) < 0){
		perror("sento error");
		exit(1);
	}
	else{
		printf("broadcast success\n");
	}

	return 0;
}
