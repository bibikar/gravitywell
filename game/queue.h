// FiFo.h
// Runs on LM4F120/TM4C123
// Provide functions that implement the Software FiFo Buffer
// Last Modified: 4/12/2016 
// Student names: Sameer Bibikar, Sneha Pendharkar
// Last modification date: 4/17/2017


#include <stdint.h>

struct queue_struct {
	uint32_t *arr;
	int begin; // The beginning of the queue (inclusive)
	int end; // The end of the queue (exclusive)
	int fr;
	int capacity;
};

typedef struct queue_struct Queue;

// *********** FIFO_Init**********
// Initializes a software FIFO of a
// fixed size and sets up indexes for
// put and get operations
void queue_init(Queue *q, uint32_t *arr, int capacity);

// *********** FIFO_Put**********
// Adds an element to the FIFO
// Input: Character to be inserted
// Output: 1 for success and 0 for failure
//         failure is when the buffer is full
void queue_offer(Queue q, uint32_t data);
void queue_put(Queue q, uint32_t data);

// *********** FIFO_Get**********
// Gets an element from the FIFO
// Input: Pointer to a character that will get the character read from the buffer
// Output: 1 for success and 0 for failure
//         failure is when the buffer is empty
uint32_t queue_poll(Queue q);
uint32_t queue_get(Queue q);

int queue_free(Queue q);
int queue_capacity(Queue q);
int queue_size(Queue q);
