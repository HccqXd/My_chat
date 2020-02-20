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

int sockfd;

void sig_handler(int signo){
	if(signo==SIGINT){
		printf("server close\n");
		close(sockfd);
		exit(1);
	}
}

/*输出连接上来的客户端相关信息*/
void out_addr(struct sockaddr_in *clientaddr){
	//将端口从网络字节序转换成主机字节序
	int port = ntohs(clientaddr->sin_port);
	char ip[16];
	memset(ip,0,sizeof(ip));
	//将ip地址从网络字节序转换成点分十进制
	inet_ntop(AF_INET,&clientaddr->sin_addr.s_addr,ip,sizeof(ip));
	printf("client:%s(%d) connected\n",ip,port);
}

void do_service(int fd){
	//获得系统时间
	long t = time(0); 
	char *s = ctime(&t); 
	size_t size = strlen(s) * sizeof(char);
	//将服务器端获得的系统时间写回到客户端
	if(write(fd,s,size) != size){
		perror("write error");
	}
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
	while(1){
		int fd = accept(sockfd,(struct sockaddr*)&clientaddr,
				&clientaddr_len);
		if(fd<0){
			perror("accept error");
			continue;
		}

		/*步骤5：调用IO函数(read/write)和连接的客户端进行双向通信*/
       		out_addr(&clientaddr);
       		do_service(fd);

       		/*步骤6：关闭socket*/
       		close(fd);

	}


	return 0;
}
