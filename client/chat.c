/*********************************
 * 聊天
 * 2020-1 by llc
 * ******************************/
#include"config.h"

void enterchat(int sockfd){
	setbuf(stdin,NULL);
	system("clear");
	printf("===============================\n");
	while(1){
		Msg msg;
	
		memset(&msg,0,sizeof(msg));
	
		scanf("%s",msg.buff);
		msg.cmd = 3;	//群聊信号
	
		//获得本地ip地址
		struct sockaddr_in addr;
		socklen_t addr_len = sizeof(addr);
		getsockname(sockfd,(struct sockaddr*) &addr,&addr_len);
		msg.from_userAddr = addr;
		if(send(sockfd,&msg,sizeof(msg),0) < 0){ //将要发送的消息发送至服务器端
			perror("send msg error");	
			continue;
		}
		
		memset(&msg,0,sizeof(msg));
		if(recv(sockfd,&msg,sizeof(msg),0) < 0){
			perror("recv msg error");
			continue;
		}
		
		if(msg.cmd == 3000){
			printf("%s\n",msg.buff);
		}
		
	} //while
	
}
