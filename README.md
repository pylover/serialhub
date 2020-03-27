# serialhub

Forward TCP and Unix connections to serial interface.


```bash
make
./serialhub /dev/ttyUSB0  # Default baudrate: 115200
./serialhub --baudrate 9600 /dev/ttyUSB0
./serialhub --tcpport 3000 --unixfile /tmp/serialhub.s /dev/ttyUSB0
./serialhub --help
```

