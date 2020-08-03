CC = gcc
#KERNEL = $(shell uname -r)
#CFLAGS = -I. -I/usr/src/linux-headers-$(KERNEL)/include
CFLAGS =-Wall -I.
LIBS = -lm
OBJECTS = common.o cli.o tty.o tcp.o unix.o mux.o
PREFIX := /usr/local

serialhub: $(OBJECTS) main.c
	$(CC) $(CFLAGS) $(LIBS) -o $@ $^

common.o: common.c common.h
cli.o: cli.c cli.h common.h 
tty.o: tty.c tty.h common.h 
tcp.o: tcp.c tcp.h mux.h
unix.o: unix.c unix.h mux.h
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


define SYSTEMDSERVICE
[Unit]
Description=Serial port multiplexer
After=network.target

[Service]
ExecStart=/usr/local/bin/serialhub \
    --baudrate 115200 \
    --unixfile /run/serialhub.s /dev/ttyACM0
Restart=on-failure
User=root
Group=dialout
UMask=003
KillSignal=SIGINT

[Install]
WantedBy=multi-user.target
endef

export SYSTEMDSERVICE


.PHONY: systemd
systemd: install
	@echo "$$SYSTEMDSERVICE" | \
		sudo tee /etc/systemd/system/serialhub.service > /dev/null
	sudo systemctl daemon-reload
	sudo systemctl enable serialhub.service
	sudo service serialhub start

