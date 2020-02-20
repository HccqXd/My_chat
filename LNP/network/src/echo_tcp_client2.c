#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/socket.h>
#include<netdb.h>
#include<string.h>
#include<arpa/inet.h>
#include<memory.h>
#include<time.h>

int main(int argc,char *argv[]){
	if(argc<3){
		printf("usage: %s ip port\n",argv[0]);
		exit(1);
	}

	/*步骤1：创建socket*/
	int sockfd = socket(AF_INET,SOCK_STREAM,0);
	if(sockfd < 0){
		perror("socket error");
		exit(1);
	}
	
	//往serveraddr中填入地址族类型、port和ip
	struct sockaddr_in serveraddr;
	memset(&serveraddr,0,sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_port = htons(atoi(argv[2]));
	//将ip地址转换成网络字节序后填入sereraddr中
	inet_pton(AF_INET,argv[1],&serveraddr.sin_addr.s_addr);

	/*步骤2：客户端调用connect函数连接到服务器端*/
	if(connect(sockfd,(struct sockaddr*)&serveraddr,sizeof(serveraddr))<0){
		perror("connect error");
		exit(1);
	}

	/*步骤3：调用IO函数(read/write)和服务器端进行双向通信*/
	char buff[512];
	size_t size;
	char *prompt = ">";
	while(1){
		memset(buff,0,sizeof(buff));
		write(STDOUT_FILENO,prompt,1);
		size = read(STDIN_FILENO,buff,sizeof(buff));
		if(size < 0) continue;
		buff[size - 1] = '\0';
		if(write(sockfd,buff,sizeof(buff)) < 0){
			perror("write msg error");
			continue;
		}
		else{
			if(read(sockfd,buff,
					sizeof(buff)) < 0){
				perror("read msg error");
				continue;
			}
			else{
				printf("%s\n",buff);
			}
		}
	}

	/*步骤4：关闭socket*/
	close(sockfd);

	return 0;

}

