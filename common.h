#ifndef H_COMMON
#define H_COMMON

#define OK  0

enum {
    FAILURE_CLIPARSE = -1,
    FAILURE_SERIALDEVICE = -2,
    FAILURE_ALLOCATE = -3,
    FAILURE_TCPLISTEN = -4,
    FAILURE_TCPBIND = -5,
    FAILURE_INVALIDTCPADDRESS = -6,
};

#define _STR(X) #X
#define STR(X) _STR(X)

#endif
