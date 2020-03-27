CC = gcc
#KERNEL = $(shell uname -r)
#CFLAGS = -I. -I/usr/src/linux-headers-$(KERNEL)/include
CFLAGS =-Wall -I.
LIBS = -lm
OBJECTS = common.o cli.o tty.o mux.o
PREFIX := /usr/local

serialhub: $(OBJECTS) main.c
	$(CC) $(CFLAGS) $(LIBS) -o $@ $^

common.o: common.c common.h
cli.o: cli.c cli.h common.h 
tty.o: tty.c tty.h common.h 
mux.o: mux.c mux.h common.h 

.PHONY: clean 
clean:
	rm -f $(OBJECTS) serialhub

.PHONY: test 
test: serialhub
	./serialhub /dev/ttyUSB0

.PHONY: install
install: serialhub
	install -m 755 serialhub $(DESTDIR)$(PREFIX)/bin
