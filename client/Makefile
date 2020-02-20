MYNAME = makefile
CC = gcc

objects = client.o register.o interface.o login.o chat.o  

client: $(objects)
	cc -o client $(objects) -I /usr/include/mysql/ -l mysqlclient -lpthread

client.o: client.c config.h
	cc -c client.c  -I /usr/include/mysql/ -l mysqlclient -lpthread

register.o: register.c config.h
	cc -c register.c -I /usr/include/mysql/ -l mysqlclient -lpthread 

interface.o: interface.c config.h
	cc -c interface.c -I /usr/include/mysql/ -l mysqlclient -lpthread
login.o:login.c config.h
	cc -c login.c -I /usr/include/mysql/ -l mysqlclient -lpthread
chat.o:chat.c config.h
	cc -c chat.c -I /usr/include/mysql/ -l mysqlclient -lpthread
#比较稳健的clean做法，表示clean是一个伪目标
.PHONY: clean

#前面-的意思是：也许某些文件出现问题，忽略，继续执行
clean:
	-rm client $(objects) 

