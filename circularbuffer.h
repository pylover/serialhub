#ifndef H_CIRCULARBUFFER
#define H_CIRCULARBUFFER


typedef struct {
    char *buffer;
    int head;
    int tail;
    int size;   // Power of 2
} Buffer;


/* Return count in buffer.  */
#define CIRC_CNT(b) (((b->head) - (b->tail)) & ((b->size)-1))

/* Return space available, 0..size-1.  We always leave one free char
   as a completely full buffer has head == tail, which is the same as
   empty.  */
#define CIRC_SPACE(b) CIRC_CNT((b->tail),((b->head)+1),(b->size))

/* Return count up to the end of the buffer.  Carefully avoid
   accessing head and tail more than once, so they can change
   underneath us without returning inconsistent results.  */
#define CIRC_CNT_TO_END(b) \
	({int end = (b->size) - (b->tail); \
	  int n = ((b->head) + end) & ((b->size)-1); \
	  n < end ? n : end;})

/* Return space available up to the end of the buffer.  */
#define CIRC_SPACE_TO_END(b) \
	({int end = (b->size) - 1 - (b->head); \
	  int n = (end + (b->tail)) & ((b->size)-1); \
	  n <= end ? n : end+1;})



int bufferallocate(Buffer *buff);

#endif