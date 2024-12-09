# serialhub

Forward TCP, UDP and Unix connections to serial interface(aka serial-over-ip).


### Install

#### Dependencies
- https://github.com/pylover/clog
- https://github.com/pylover/caio
- https://github.com/pylover/yacap


```bash
mkdir -p build && cd build && cmake ..
make install
```

### Quickstart

```bash
serialhub /dev/ttyUSB0  # Default baudrate is 115200
serialhub --baudrate 9600 /dev/ttyUSB0
serialhub --tcpport 3000 --unixfile /tmp/serialhub.s /dev/ttyUSB0
```

#### Help

```bash
./serialhub --help
```


### Systemd

```bash
make systemd
```

Create a file named `/etc/systemd/system/serialhub.service`.

```systemd
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
```

Then:

```bash
systemctl daemon-reload
systemctl enable serialhub.service
service serialhub start
```

Check the service using:

```bash
ss -lnp | grep serialhub
```

