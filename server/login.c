/******************************
 * 客户登陆处理
 * 2020-1 by llc
 * ****************************/
#include"config.h"

void enterChat(int socket);

void login(int client_socket){
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

	printf("%s正在登录\n",client_user.userName);

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
	if(!mysql_num_rows(res)){
		memset(buf,0,sizeof(buf));
		strcpy(buf,"没有该用户，请注册!");
		write(client_socket,buf,sizeof(buf));
		return;
	}

	row = mysql_fetch_row(res);
	printf("%s\n",client_user.password);
	printf("%s\n",row[1]);
	if(strcmp(row[1],client_user.password) == 0){
		memset(&msg,0,sizeof(msg));
		msg.cmd = 2000;
		send(client_socket,&msg,sizeof(msg),0);
		enterChat(client_socket);
	}
	mysql_free_result(res);
	
	//关闭数据库
	mysql_close(&conn);
	break;
	}//if
}//while
}
