CC = gcc
CFLAGS=-I.
LIBS=-lm
OBJECTS=main.o tty.o

serialhub: $(OBJECTS)
	$(CC) $(CFLAGS) $(LIBS) -o $@ $^

tty.o: tty.c tty.h
	$(CC) $(CFLAGS) -c -o $@ $<

main.o: main.c tty.h
	$(CC) $(CFLAGS) -c -o $@ $<



.PHONY: clean

clean:
	rm -f $(OBJECTS) serialhub
