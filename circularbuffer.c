
#include "common.h"
#include "circularbuffer.h"

#include <stdlib.h>


int bufferallocate(Buffer *buff) {
    char *p = malloc(buff->size);
    if (p == NULL) {
        return FAILURE_ALLOCATE;
    }

    buff->buffer = p; 
    return OK;
}


void bufferfree(Buffer *buff) {
    free(buff->buffer);
}
