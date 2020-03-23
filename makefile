CC = gcc
CFLAGS = -I.
LIBS = -lm
OBJECTS = settings.o cli.o tty.o

serialhub: $(OBJECTS) logging.h main.c
	$(CC) $(CFLAGS) $(LIBS) -o $@ $^

cli.o: cli.c cli.h
tty.o: tty.c tty.h
settings.o: settings.c settings.h

.PHONY: clean 
clean:
	rm -f $(OBJECTS) serialhub

.PHONY: test 
test: serialhub
	./serialhub /dev/ttyACM0

