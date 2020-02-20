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
#include<fcntl.h>
#include"vector_fd.h"

VectorFD *vfd;

int sockfd;

void sig_handler(int signo){
	if(signo==SIGINT){
		printf("server close\n");
		close(sockfd);
		//销毁动态数组
		destroy_vector_fd(vfd);
		exit(1);
	}
}

/*
 *fd对应于某个连接的客户端
 *和某一个连接的客户端进行双向通信(非阻塞方式)
 * */

void do_service(int fd){
	/*和客户端进行读写操作(双向通信)*/
	char buff[512];
	memset(buff,0,sizeof(buff));
	/*
	 *因为采用非阻塞方式，若读不到数据直接返回
	 *直接服务于下一个客户端
	 *因此不需要判断size小于0的情况
	 */
	size_t size = read(fd,buff,sizeof(buff));
	if(size == 0){
		char info[] = "client closed";
		write(STDOUT_FILENO,info,sizeof(info));
		//从动态数组中删除对应的fd
		remove_fd(vfd,fd);
		//关闭对应客户端的socket
		close(fd);
	}
	else if(size > 0){
		write(STDOUT_FILENO,buff,sizeof(buff));
		if(write(fd,buff,size) != size){
			if(errno == EPIPE){ //客户端关闭连接
				perror("write error");
				remove_fd(vfd,fd);
				close(fd);
			}
		}
	}
}


void *th_fn(void *arg){
	int i;
	while(1){
		i = 0;
		//遍历动态数组中的socket描述符
		for(;i < vfd->counter;i++){
			do_service(get_fd(vfd,i));
		}
	}
	return (void*)0;
}

void out_addr(struct sockaddr_in *clientaddr){
	char ip[16];
	memset(ip,0,sizeof(ip));
	int port = ntohs(clientaddr->sin_port);
	inet_ntop(AF_INET,
		&clientaddr->sin_addr.s_addr,ip,sizeof(ip));
	printf("%s(%d) connected!\n",ip,port);
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
	
	//创建放置套接字描述符fd的动态数组
	vfd = create_vector_fd();

	//设置线程的分离属性
	pthread_t th;
	pthread_attr_t attr;
	pthread_attr_init(&attr);
	pthread_attr_setdetachstate(&attr,
			PTHREAD_CREATE_DETACHED);

	int err;
	if((err = pthread_create(&th,&attr,
			th_fn,(void*)0)) != 0){
		perror("pthread create error");
		exit(1);
	}
	pthread_attr_destroy(&attr);

	/*
	 *1）主控线程获得客户端的连接，将新的socket描述符放置到动态数组中
	 *2）启动的子线程负责遍历动态数组中socket描述符
	 *   并和对应的客户端进行双向通信
	 *   (采用非阻塞方式读写)
	 * */
	struct sockaddr_in clientaddr;
        socklen_t len = sizeof(clientaddr);

	while(1){
		int fd = accept(sockfd,
			(struct sockaddr*)&clientaddr,&len);
		if(fd<0){
			perror("accept error");
			continue;
		}
		out_addr(&clientaddr);
		//将读写修改为非阻塞方式
		int val;
		fcntl(fd,F_GETFL,&val);
		val |= O_NONBLOCK;
		fcntl(fd,F_SETFL,val);
		//将返回新的socket描述符加入到动态数组中
		add_fd(vfd,fd);
	}


	return 0;
}
