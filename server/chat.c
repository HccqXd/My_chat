/*********************************
 * 聊天处理
 * 这里只实现客户端和服务器端通信功能
 * 其余功能可以相似扩展
 * 2020-1 by llc
 * ******************************/
#include"config.h"

void enterChat(int sockfd){
	while(1){
		Msg msg;
	
		memset(&msg,0,sizeof(msg));
	
		if(recv(sockfd,&msg,sizeof(msg),0) < 0){ //接收sockfd发送过来的消息
			perror("send msg");
			continue;	
		}

		//初始化数据库
		MYSQL conn;
        	MYSQL_RES *res; //查询得到的结果集
        	MYSQL_ROW row;

        	char *mysql_server = "localhost";
        	char *mysql_user = "root";
        	char *mysql_password = "123";
        	char *mysql_database = "client";

        	mysql_init(&conn);

        	/*连接数据库*/
        	if(!mysql_real_connect(&conn,mysql_server,
                                	mysql_user,mysql_password,
                                	mysql_database,0,NULL,0)){
                	fprintf(stderr,"%s\n",mysql_error(&conn));
                	exit(1);
        	}



		char sql[128]; //sql语句存储变量

		//检查要注册用户名是否已存在
		memset(sql,0,sizeof(sql));
		sprintf(sql,"select * from user where userAddr='%s';",inet_ntoa(msg.from_userAddr.sin_addr));
		if(mysql_query(&conn,sql)||!(res = mysql_store_result(&conn))){
			printf("插入查询失败\n");
			return;
		}
		if(!mysql_num_rows(res)){
			char buf[1024];
			memset(buf,0,sizeof(buf));
			strcpy(buf,"没有该用户，请注册!");
			write(sockfd,buf,sizeof(buf));
			continue;
		}

		row = mysql_fetch_row(res);

		char from_name[20];
		memset(from_name,0,sizeof(from_name));
		strcpy(from_name,row[0]);

		char buff[1024];
		memset(buff,0,sizeof(buff));
		sprintf(buff,"~%s~:%s",from_name,msg.buff);

		memset(&msg,0,sizeof(msg));
		msg.cmd = 3000; //发送聊天消息信号
		strcpy(msg.buff,buff);

		if(send(sockfd,&msg,sizeof(msg),0) < 0){
			perror("send error");
			continue;
		}

		mysql_free_result(res);

		//关闭数据库
		mysql_close(&conn);
	} //while
	
}
