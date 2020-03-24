
#include "common.h"
#include "circularbuffer.h"

#include <unistd.h>


int buffer_readinto(struct ringbuffer *b, int fd) {
    char buff[CHUNKSIZE];
    int result;

    while (1) {
        result = read(fd, buff, CHUNKSIZE);
        if (result <= 0) {
            return result;
        }
        
        //result = buffer_put(b, buff, result);
        // TODO: continue here
    }
    return OK;
}
