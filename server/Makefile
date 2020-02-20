MYNAME = makefile
CC = gcc

objects = server.o register.o login.o chat.o

server: $(objects)
	cc -o server $(objects) -I /use/include/mysql/ -l mysqlclient -lpthread

server.o: server.c config.h
	cc -c server.c -I /usr/include/mysql -l mysqlclient -lpthread 

register.o: register.c config.h
	cc -c register.c -I /usr/include/mysql -l mysqlclient -lpthread
login.o:login.c config.h
	cc -c login.c -I /usr/include/mysql -l mysqlclient -lpthread
chat.o:chat.c config.h
	cc -c chat.c -I /usr/include/mysql -l mysqlclient -lpthread
#比较稳健的clean做法，表示clean是一个伪目标
.PHONY: clean

#前面-的意思是：也许某些文件出现问题，忽略，继续执行
clean:
	-rm server $(objects) 

