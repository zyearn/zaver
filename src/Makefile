all: zaver 

zaver: zaver.c
	gcc -o zaver zaver.c util.c http.c epoll.c rio.c threadpool.c -lpthread 

clean:
	rm zaver
