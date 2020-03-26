
#include "common.h"
#include "circularbuffer.h"


void putone(struct ringbuffer *buff, char c) {
    buff->blob[buff->head] = c;
    buff->head = (buff->head + 1) & (buff->size - 1);
}


int bufferput(struct ringbuffer *buff, const char *data, int len) {
    int i, err;
    if (bufferspace(buff) < len) {
        L_ERROR("Buffer full");
        errno = ENOBUFS;
        return ERR;
    }
    for (i = 0; i < len; i++) {
        putone(buff, data[i]);
    }
    return OK;
}


