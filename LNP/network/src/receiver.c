#include<sys/socket.h>
#include<arpa/inet.h>
#include<netdb.h>
#include<unistd.h>
#include<string.h>
#include<stdlib.h>
#include<stdio.h>
#include<signal.h>
#include<time.h>

int sockfd;

void sig_handler(int signo){
	if(signo == SIGINT){
		printf("receiver will exited\n");
		close(sockfd);
		exit(1);
	}
}

int main(int argc,char *argv[]){
	if(argc < 2){
		fprintf(stderr,"usage:%s port\n",argv[0]);
		exit(1);
	}

	if(signal(SIGINT,sig_handler) == SIG_ERR){
		perror("signal sigint error");
		exit(1);
	}

	sockfd = socket(AF_INET,SOCK_DGRAM,0);
	if(sockfd < 0){
		perror("socket error");
		exit(1);
	}

	struct sockaddr_in serveraddr;
	memset(&serveraddr,0,sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_port = htons(atoi(argv[1]));
	serveraddr.sin_addr.s_addr = INADDR_ANY;
	if(bind(sockfd,(struct sockaddr*)&serveraddr,
				sizeof(serveraddr)) < 0){
		perror("bind error");
		exit(1);
	}

	char buffer[1024];
	struct sockaddr_in clientaddr;
	socklen_t len = sizeof(clientaddr);
	while(1){
		memset(buffer,0,sizeof(buffer));
		memset(&clientaddr,0,sizeof(clientaddr));
		if(recvfrom(sockfd,buffer,sizeof(buffer),0,
			(struct sockaddr*)&clientaddr,&len) < 0){
			perror("recvfrom error");
			exit(1);
		}
		else{
			char ip[16];
			inet_ntop(AF_INET,&clientaddr.sin_addr.s_addr,
					ip,sizeof(ip));
			int port = ntohs(clientaddr.sin_port);
			printf("%s(%d):%s\n",ip,port,buffer);
		}
	}
	return 0;
}
