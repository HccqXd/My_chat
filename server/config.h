#ifndef __CONFIG_H__
#define __CONFIG_H__

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<netdb.h>
#include<sys/socket.h>
#include<arpa/inet.h>
#include<unistd.h>
#include<errno.h>
#include<memory.h>
#include<signal.h>
#include<time.h>
#include<sys/types.h>
#include<sys/wait.h>
#include<pthread.h>
#include<fcntl.h>
#include<mysql.h>

//消息结构体
typedef struct{
	char buff[512]; //数据
	int cmd;	//消息类型
	struct sockaddr_in from_userAddr;; //发送者ip
	char toName[20]; //接收者姓名
}Msg;

//用户信息结构体
typedef struct{
	char userName[20]; //用户名
	char password[20]; //用户密码
	struct sockaddr_in userAddr; //用户ip地址
	int sockfd; //当前用户套接字
}User;



#endif
