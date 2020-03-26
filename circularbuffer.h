#ifndef H_CIRCULARBUFFER
#define H_CIRCULARBUFFER


struct ringbuffer{
    char *blob;
    int head;
    int tail;
    int size;   // Power of 2
};



/* Return count in buffer.  */
#define CIRC_CNT(head,tail,size) (((head) - (tail)) & ((size)-1))

/* Return space available, 0..size-1.  We always leave one free char
   as a completely full buffer has head == tail, which is the same as
   empty.  */
#define bufferspace(b) CIRC_CNT((b->tail), ((b->head)+1), (b->size))
#define bufferused(b) CIRC_CNT(b->head, b->tail, b->size)

// Not used yet
///* Return count up to the end of the buffer.  Carefully avoid
//   accessing head and tail more than once, so they can change
//   underneath us without returning inconsistent results.  */
//#define CIRC_CNT_TO_END(head,tail,size) \
//	({int end = (size) - (tail); \
//	  int n = ((head) + end) & ((size)-1); \
//	  n < end ? n : end;})
//
///* Return space available up to the end of the buffer.  */
//#define CIRC_SPACE_TO_END(head,tail,size) \
//	({int end = (size) - 1 - (head); \
//	  int n = (end + (tail)) & ((size)-1); \
//	  n <= end ? n : end+1;})

int bufferput(struct ringbuffer *buff, const char *data, int len);

#endif



