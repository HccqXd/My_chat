#include"config.h"

/******************************
 * 客户登录
 * 2020.1 by llc
 * ****************************/
void enterchat(int sockfd);

void login(int sockfd){
	User user;
	char buf[1024];
	Msg msg;
	
	memset(&user,0,sizeof(user));
	system("clear");
	printf("\t**********用户登录************\n");
	printf("\t请输入用户名:");
	scanf("%s",user.userName);
	printf("\n");
	
	printf("\t请输入用户密码:");
	scanf("%s",user.password);
	printf("\n");
	
	//获得本地ip地址
	struct sockaddr_in addr;
	socklen_t addr_len = sizeof(addr);
	getsockname(sockfd,(struct sockaddr*) &addr,&addr_len);
	
	user.userAddr = addr;
	user.sockfd = sockfd;
	
	//向服务器端发送注册信息
	memset(buf , 0 , sizeof(buf));
	memcpy(buf , &user , sizeof(user));
	if(send(sockfd,buf,sizeof(buf),0) < 0){
		perror("send user error");
	}
	
	memset(buf , 0 , sizeof(buf));
	//读取服务器的注册回应
	if(recv(sockfd,buf,sizeof(buf),0) < 0){
		perror("recv error");
	}
	memset(&msg, 0 , sizeof(msg));
	memcpy(&msg, buf , sizeof(buf));
	printf("%d\n",msg.cmd);
	
	if(msg.cmd == 2000){
		printf("登录成功\n");
		sleep(1);
		enterchat(sockfd);
	}
			
}
