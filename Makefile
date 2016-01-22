# @version 0.0.0
# @author jiashun Zhu (zhujiashun2010@gmail.com) 
# @date 2015.04.22 created by jiashunZhu

CC=gcc
CFLAGS=-g -O2 -Wall -Wextra -Isrc $(OPTFLAGS)
LIBS=-ldl -lpthread $(OPTLIBS)
PREFIX?=/usr/local
LINK=gcc -o

SOURCES=$(wildcard src/**/*.c src/*.c)
OBJECTS=$(patsubst %.c,objs/%.o,$(SOURCES))

# for tests
OBJECTS_WITHOUT_ZAVER=$(filter-out objs/src/zaver.o,$(OBJECTS))

TEST_SRC=$(wildcard tests/*_test.c)
TESTS=$(patsubst %.c,%,$(TEST_SRC))

TARGET=objs/zaver

all: $(TARGET) tests
zaver: $(TARGET)

dev: CFLAGS=-g -Wall -Isrc -Wextra $(OPTFLAGS)
dev: all

$(TARGET): build $(OBJECTS)
	$(LINK) $(TARGET) $(OBJECTS) $(LIBS)

objs/%.o: %.c
	$(CC) -c $(CFLAGS) -o $@ $<

tests/%: tests/%.c 
	$(CC) $(CFLAGS) -o $@ $< $(OBJECTS_WITHOUT_ZAVER) $(LIBS)

build:
	@mkdir -p objs/src

# 当前目录下有tests这个文件或目录，所以如果不加PHONY，则总是最新的，不会执行下面的指令
.PHONY: tests
tests: $(TESTS)
	@echo "compile test succeed"
	

valgrind:
	VALGRIND="valgrind --log-file=/tmp/valgrind-%p.log" $(MAKE)

# The Cleaner
clean:
	rm -rf objs $(TESTS)
	rm -f tests/tests.log
	rm -rf `find . -name "*.dSYM" -print`

# The Install
install: all
	install -d $(DESTDIR)/$(PREFIX)/lib/
	install $(TARGET) $(DESTDIR)/$(PREFIX)/lib/
