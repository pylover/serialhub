CC = gcc
#KERNEL = $(shell uname -r)
#CFLAGS = -I. -I/usr/src/linux-headers-$(KERNEL)/include
CFLAGS = -I.
LIBS = -lm
OBJECTS = circularbuffer.o networking.o settings.o cli.o tty.o connection.o

serialhub: $(OBJECTS) main.c
	$(CC) $(CFLAGS) $(LIBS) -o $@ $^

cli.o: cli.c cli.h common.h settings.h
tty.o: tty.c tty.h common.h settings.h 
settings.o: settings.c settings.h
networking.o: networking.c networking.h common.h settings.h
connection.o: connection.c connection.h circularbuffer.h
circularbuffer.o: circularbuffer.c circularbuffer.h common.h

.PHONY: clean 
clean:
	rm -f $(OBJECTS) serialhub

.PHONY: test 
test: serialhub
	./serialhub /dev/ttyACM0

