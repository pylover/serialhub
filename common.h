#ifndef H_COMMON
#define H_COMMON

#define OK  0

enum {
    FAILURE_CLIPARSE = 1,
    FAILURE_SERIALDEVICE,
    FAILURE_ALLOCATE
};

#define _STR(X) #X
#define STR(X) _STR(X)

#endif
