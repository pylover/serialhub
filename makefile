CC = gcc
CFLAGS=-I.
LIBS=-lm
OBJECTS=logging.o

serialhub: $(OBJECTS) serialhub.c
	$(CC) $(CFLAGS) $(LIBS) -o $@ $^

tty.o: tty.c tty.h


.PHONY: clean

clean:
	rm -f $(OBJECTS) serialhub

