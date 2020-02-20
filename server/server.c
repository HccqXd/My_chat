/**************************************
 * 服务器端主程序
 * 2020.1 by llc
 * ***********************************/
#include"config.h"

void reg(int client_socket);
void login(int client_socket);

int sockfd;

void sig_handler(int signo){ //服务器异常处理
	if(signo == SIGINT){
		printf("server close\n");
		close(sockfd);
		exit(1);
	}
}

void* hanld_client(void* fd){
	int client_socket = (int)(*((int*)fd));
	Msg msg;
	char buf[1024];
	int ret = read(client_socket,&msg,sizeof(msg));
	printf("%d\n",msg.cmd);
	if(ret == -1){
		perror("read  msg error");
		close(client_socket);
	}
	else{
	switch(msg.cmd){
		case 1:	//处理客户端注册
		{
			msg.cmd=1003; //发送允许注册信号
			if(send(client_socket,&msg,sizeof(msg),0) < 0){
				perror("send msg error");
				exit(1);
			}
			memset(buf,0,sizeof(buf));
			memset(&msg,0,sizeof(msg));
			reg(client_socket);
			break;	
		}
				
		case 2: //处理客户端登录
		{	
			msg.cmd=2003; //发送信号
			if(send(client_socket,&msg,sizeof(msg),0) < 0){
				perror("send msg error");
				exit(1);
			}
			memset(buf,0,sizeof(buf));
			memset(&msg,0,sizeof(msg));
			login(client_socket);
			break;
		}
		default:
			break;
		
	}
	}	
}

int main(int argc,char* argv[]){
	MYSQL *conn;
	MYSQL_RES *res;
	MYSQL_ROW row;

	char *mysql_server = "localhost";
	char *mysql_user = "root";
	char *mysql_password = "123";
	char *mysql_database = "client";

	conn = mysql_init(NULL);

	/*连接数据库*/
	if(!mysql_real_connect(conn,mysql_server,
				mysql_user,mysql_password,
				mysql_database,0,NULL,0)){
		fprintf(stderr,"%s\n",mysql_error(conn));
		exit(1);
	}
	
	printf("数据库准备就绪...\n");

	mysql_close(conn);

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

	/*步骤2：调用bind函数将scoket和地址进行绑定*/
	struct sockaddr_in serveraddr;
	memset(&serveraddr,0,sizeof(serveraddr));

	serveraddr.sin_family = AF_INET;
	serveraddr.sin_port = htons(8888);
	serveraddr.sin_addr.s_addr = INADDR_ANY;
	if(bind(sockfd,(struct sockaddr*)&serveraddr,sizeof(serveraddr)) < 0){
		perror("bind error");
		exit(1);
	}

	/*步骤3：调用listen函数启动监听（指定port监听）*/
	if(listen(sockfd,100) < 0){
		perror("listen error");
		exit(1);
	}

	/*步骤4：调用accept函数从队列中获得一个客户端的请求连接*/
	struct sockaddr_in clientaddr;
	socklen_t clientaddr_len = sizeof(clientaddr);
	while(1){
		int fd = accept(sockfd,(struct sockaddr*)&clientaddr,
				&clientaddr_len);
		if(fd < 0){
			perror("accept error");
			continue;
		}
		
		//创建一个线程处理客户端的请求，主线程依然负责监听
		pthread_t id;
		pthread_create(&id,NULL,hanld_client,&fd);

		pthread_detach(id); //线程分离

	}

	close(sockfd);

	return 0;
}
