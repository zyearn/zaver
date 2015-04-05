all: zaver 

zaver: zaver.c
	gcc -W -Wall -lpthread -o zaver zaver.c util.c http.c rio.c epoll.c

clean:
	rm zaver
