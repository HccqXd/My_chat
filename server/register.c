#include"config.h"

/******************************
 * 客户注册处理
 * 2020.1 by llc
 * ****************************/
void reg(int client_socket){
	char buf[1024];
	memset(buf,0,sizeof(buf));
	while(1){
	if(recv(client_socket,buf,sizeof(buf),0) < 0){
		perror("recv buf error");
	}
	
	if(strlen(buf)){
	Msg msg;
	User client_user;
	memset(&client_user,0,sizeof(client_user));
	memcpy(&client_user,buf,sizeof(buf));	

	printf("%s进行注册\n",client_user.userName);

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
	sprintf(sql,"select * from user where userName='%s';",client_user.userName);
	if(mysql_query(&conn,sql)||!(res = mysql_store_result(&conn))){
		printf("插入查询失败\n");
		return;
	}
	if(mysql_num_rows(res)){
		msg.cmd = 1002;
		printf("%s已存在\n",client_user.userName);
		write(client_socket,&msg,sizeof(msg));
		return;
	}
	mysql_free_result(res);
	//不存在则插入数据库
	memset(sql,0,sizeof(sql));
	sprintf(sql,"INSERT INTO user(userName,password,userAddr,sockfd) VALUES('%s','%s','%s',%d);",client_user.userName,client_user.password,
			inet_ntoa(client_user.userAddr.sin_addr),client_user.sockfd);
	if(!mysql_query(&conn,sql)){ //c中，成功返回0
			msg.cmd = 1001;
	                printf("%s注册成功!\n",client_user.userName);
        	        write(client_socket,&msg,sizeof(msg));

	}
	

	//关闭数据库
	mysql_close(&conn);
	close(client_socket);
	break;
	}//if
}//while
}
