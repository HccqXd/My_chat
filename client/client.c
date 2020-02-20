/*************************
 * 客户端通信
 * 2020.1 by llc
 * ***********************/

#include"config.h"

void reg(int sockfd);
void mainInterface();
void loginInterface(char *userName);
void login(int sockfd);

int main(int argc,char *argv[]){
	/*步骤1、创建socket*/   	
	int sockfd = socket(AF_INET,SOCK_STREAM,0);
	if(sockfd < 0){
        	perror("socket error");
        	exit(1);
	}       
	
	struct sockaddr_in serveraddr;
	memset(&serveraddr,0,sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_port = htons(8888);
	//将ip地址转换成网络字节序后填入serveraddr中
	inet_pton(AF_INET,"127.0.0.1",&serveraddr.sin_addr.s_addr);

	/*步骤2：和服务器端建立连接*/
	if(connect(sockfd,(struct sockaddr*)&serveraddr,sizeof(serveraddr)) < 0){
		perror("connect error");
		exit(1);
	}
	
	mainInterface();
	
	char c;
	scanf("%c",&c);
	switch(c){
		case '1':{
			Msg msg;
			memset(&msg,0,sizeof(msg));
			msg.cmd = 1;
			char buf[1024];
			memset(buf,0,sizeof(buf));
			memcpy(buf,&msg,sizeof(msg));
			if(send(sockfd,buf,sizeof(buf),0) < 0){
				perror("send error");			
			}
			
			memset(buf,0,sizeof(buf));
			//读取服务器的注册回应
			if(recv(sockfd,buf,sizeof(buf),0) < 0){
				perror("recv error");
			}
			memset(&msg, 0 , sizeof(msg));
			memcpy(&msg, buf , sizeof(buf));
			printf("%d\n",msg.cmd);
			if(msg.cmd == 1003)
				reg(sockfd);
			break;
		}
		case '2':{
			Msg msg;
			memset(&msg,0,sizeof(msg));
			msg.cmd = 2;
			char buf[1024];
			memset(buf,0,sizeof(buf));
			memcpy(buf,&msg,sizeof(msg));
			if(send(sockfd,buf,sizeof(buf),0) < 0){
				perror("send error");			
			}
			
			memset(buf,0,sizeof(buf));
			//读取服务器的登录回应
			if(recv(sockfd,buf,sizeof(buf),0) < 0){
				perror("recv error");
			}
			memset(&msg, 0 , sizeof(msg));
			memcpy(&msg, buf , sizeof(buf));
			printf("%d\n",msg.cmd);
			if(msg.cmd == 2003)
				login(sockfd);
			break;
		}	
		default:
			perror("command error");
			break;	
	}
	




	close(sockfd);
	
	return 0;
}

