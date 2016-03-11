Zaver
=====

Yet another fast and efficient HTTP server.

## purpose

Zaver的目的是帮助开发者理解基于epoll的高性能服务器是如何开发的。Nginx是一个非常好的服务器开发学习范例，但是它的代码规模较大也较复杂，让许多人望而却步。Zaver用非常少的代码展示了像Nginx这类高性能服务器的核心结构，为开发者进一步学习网络编程打下基础。

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

