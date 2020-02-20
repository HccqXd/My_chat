#include"../include/msg.h"
#include<stdio.h>
#include<stdlib.h>
#include<netdb.h>
#include<sys/socket.h>
#include<arpa/inet.h>
#include<unistd.h>
#include<string.h>
#include<errno.h>
#include<memory.h>
#include<signal.h>
#include<time.h>
#include<sys/types.h>
#include<sys/wait.h>
#include<pthread.h>

int sockfd;

void sig_handler(int signo){
	if(signo==SIGINT){
		printf("server close\n");
		close(sockfd);
		exit(1);
	}
}

void do_service(int fd){
	/*和客户端进行读写操作(双向通信)*/
	char buff[512];
	while(1){
		memset(buff,0,sizeof(buff));
		size_t size;
		if((size = read_msg(fd,
				buff,sizeof(buff)))<0){
			perror("protocal error");
			break;
		}
		else if(size == 0){
			break;
		}
		else{
			printf("%s\n",buff);
			if(write_msg(fd,buff,sizeof(buff)) < 0){
				if(errno == EPIPE){
					break;
				}
				perror("protocal error");
			}
		}
	}
}

void out_fd(int fd){
	struct sockaddr_in addr;
	socklen_t len = sizeof(addr);
	//从fd中获得连接的客户端相关信息并放置到sockaddr_in中
	if(getpeername(fd,
			(struct sockaddr*)&addr,&len) < 0){
		perror("getpeername error");
		return;
	}
	char ip[16];
	memset(ip,0,sizeof(ip));
	int port = ntohs(addr.sin_port);
	inet_ntop(AF_INET,
			&addr.sin_addr.s_addr,ip,sizeof(ip));
	printf("%16s(%5d) closed!\n",ip,port);
}

void *th_fn(void *arg){
	int fd = (int)arg;
	do_service(fd);
	out_fd(fd);
	close(fd);

	return (void*)0;
}
int main(int argc,char* argv[])
{
	if(argc<2){
		printf("usage:%s #port\n",argv[0]);
		exit(1);
	}
	if(signal(SIGINT,sig_handler)==SIG_ERR){
		perror("signal sigint error");
		exit(1);
	}


	/*步骤1：创建socket*/
	sockfd = socket(AF_INET,SOCK_STREAM,0);
	if(sockfd < 0){
		perror("socket error");
		exit(1);	
	}

	/*步骤2：调用bind函数将socket和地址(包括ip、port)进行绑定
	 * */
	struct sockaddr_in serveraddr;
	memset(&serveraddr,0,sizeof(serveraddr));
	//往地址中填入port、internet地址族类型、ip
	serveraddr.sin_family = AF_INET; //IPV$
	serveraddr.sin_port = htons(atoi(argv[1])); //端口需要网络字节序
	serveraddr.sin_addr.s_addr = INADDR_ANY;
	if(bind(sockfd,(struct sockaddr*) &serveraddr,sizeof(serveraddr))<0){
		perror("bind error");
		exit(1);
	}

	/*步骤3：调用listen函数启动监听(指定port监听)*/
	if(listen(sockfd,10)<0){
	 	perror("listen error");
		exit(1);
	}

	/*步骤4：调用accept函数从队列中获得一个客户端的请求连接*/
	struct sockaddr_in clientaddr;
	socklen_t clientaddr_len = sizeof(clientaddr);

	//设置线程的分离属性
	pthread_attr_t attr;
	pthread_attr_init(&attr);
	pthread_attr_setdetachstate(&attr,
			PTHREAD_CREATE_DETACHED);
	while(1){
		//主控线程负责调用accept去获得客户端的连接
		int fd = accept(sockfd,NULL,NULL);
		if(fd<0){
			perror("accept error");
			continue;
		}

		/*步骤5：启动子线程用IO函数(read/write)和
		 *	 连接的客户端进行双向通信
		 * */
		pthread_t th;
		int err;
		//以分离状态启动子线程
		if((err = pthread_create(&th,&attr,th_fn,(void*)fd)) != 0){
			perror("pthread create error");
		}
		pthread_attr_destroy(&attr);
	}


	return 0;
}
