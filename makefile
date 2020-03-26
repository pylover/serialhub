CC = gcc
#KERNEL = $(shell uname -r)
#CFLAGS = -I. -I/usr/src/linux-headers-$(KERNEL)/include
CFLAGS = -I.
LIBS = -lm
OBJECTS = circularbuffer.o common.o cli.o tty.o connection.o

serialhub: $(OBJECTS) main.c
	$(CC) $(CFLAGS) $(LIBS) -o $@ $^

common.o: common.c common.h
cli.o: cli.c cli.h common.h 
tty.o: tty.c tty.h common.h 
connection.o: connection.c connection.h circularbuffer.h common.h 
circularbuffer.o: circularbuffer.c circularbuffer.h common.h

.PHONY: clean 
clean:
	rm -f $(OBJECTS) serialhub

.PHONY: test 
test: serialhub
	./serialhub /dev/ttyUSB0

.PHONY: t 
t: serialhub
	./serialhub /dev/ttyACM0

