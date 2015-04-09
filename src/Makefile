all: zaver 

zaver: zaver.c
	gcc -o zaver zaver.c util.c http.c rio.c epoll.c threadpool.c -lpthread 

clean:
	rm zaver
