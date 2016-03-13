Zaver
=====

Yet another fast and efficient HTTP server.

## purpose

The purpose of Zaver is to help developers understand how to write a high performance server based on epoll. Although Nginx is a good learning example, its complexity and huge code base make people discouraged. Zaver uses as few codes as possible to demonstrate the core structure of high performance server like Nginx. Developers can lay a solid foundation by learning Zaver for further study in network programming.

The purpose of Zaver is to help developers understand how to write a high performance server based on epoll. Although Nginx is a good learning example, its complexity and huge code base make people discouraged. Zaver uses as few codes as possible to demonstrate the core structure of high performance server like Nginx. Developers can lay a solid foundation by learning Zaver for further study in network programming.

## programming model

* epoll
* non-blocking I/O
* thread-pool

## compile and run

```
make
./objs/zaver -c zaver.conf
```

## support

* HTTP persistent connection
* browser cache

## todo

* ~~add command line parameter~~
* ~~add conf file~~
* sendfile
* proxy
* other HTTP/1.1 features
* memory pool
* timer(e.g. close socket when timeout)

## more details

http://lifeofzjs.com/blog/2015/05/16/how-to-write-a-server/

