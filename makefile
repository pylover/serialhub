CC = gcc
CFLAGS=-I.
LIBS=-lm
OBJECTS=cli.o

serialhub: $(OBJECTS) logging.h serialhub.c
	$(CC) $(CFLAGS) $(LIBS) -o $@ $^

cli.o: cli.c cli.h
tty.o: tty.c tty.h


.PHONY: clean

clean:
	rm -f $(OBJECTS) serialhub

