# serialhub

Forward TCP and Unix connections to serial interface.


### Install

```bash
make
sudo make install
```

### Quickstart

```bash
serialhub /dev/ttyUSB0  # Default baudrate: 115200
serialhub --baudrate 9600 /dev/ttyUSB0
serialhub --tcpport 3000 --unixfile /tmp/serialhub.s /dev/ttyUSB0
```

#### Help

```bash
./serialhub --help
Usage: serialhub [OPTION...] DEVICE
Serial Interface multiplexer

  -b, --baudrate=BAUDRATE    Baudrate, default: 115200
  -p, --tcpport=PORT         Listen port. default: 5600
      --tcpbacklog=TCP_BACKLOG   TCP backlog, default: 1, see listen(2)
  -t, --tcpbind=ADDRESS      Listen address for TCP. default: 0.0.0.0
      --unixbacklog=UNIX_BACKLOG   Unix socket  backlog, default: 1, see
                             listen(2)
  -u, --unixfile=UNIXFILE    Unix domain socket file to listen, default:
                             serialhub.socket
  -?, --help                 Give this help list
      --usage                Give a short usage message
  -V, --version              Print program version

Mandatory or optional arguments to long options are also mandatory or optional
for any corresponding short options.

Report bugs to http://github.com/pylover/serialhub.
```


### Systemd

Create a file named `/etc/systemd/system/serialhub.service`.

```systemd
[Unit]
Description=Serial port multiplexer
After=network.target

[Service]
ExecStart=/usr/local/bin/serialhub --baudrate 115200 /dev/ttyAMA0
Restart=on-failure
KillSignal=SIGINT

[Install]
WantedBy=multi-user.target

```

Then:

```bash
systemctl daemon-reload
systemctl enable serialhub.service
serivce serialhub start
```

Check the service using:

```bash
ss -lnp | grep serialhub
```

