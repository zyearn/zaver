Zaver
=====

yet another fast and efficient HTTP server

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
* dynamic content
* other HTTP/1.1 features
* memory pool
* timer(e.g. close socket when timeout)

## more details

http://lifeofzjs.com/blog/2015/05/16/how-to-write-a-server/

