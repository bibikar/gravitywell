// FiFo.c
// Runs on LM4F120/TM4C123
// Provide functions that implement the Software FiFo Buffer
// Last Modified: 4/10/2017 
// Student names: Sneha Pendharkar, Sameer Bibikar
// Last modification date: change this to the last modification date or look very silly

#include <stdint.h>
#include <stdlib.h>
#include "../tm4c123gh6pm.h"
void DisableInterrupts(void); // Disable interrupts
void EnableInterrupts(void);  // Enable interrupts


struct queue_struct {
	uint32_t *arr;
	int begin; // The beginning of the queue (inclusive)
	int end; // The end of the queue (exclusive)
	int fr;
	int capacity;
};

typedef struct queue_struct Queue;


// *********** FiFo_Init**********
// Initializes a software FIFO of a
// fixed size and sets up indexes for
// put and get operations
Queue *queue_init(int capacity) {
	Queue *q = malloc(sizeof(struct queue_struct));
	q->arr = malloc(sizeof(uint32_t) * capacity);
	q->begin = 0;
	q->end = -1;
	q->fr = capacity;
	q->capacity = capacity;
	return q;
}

// *********** queue_offer **********
// Adds an element to the FIFO
// Input: Data to be inserted
// Won't insert if there isn't any space
void queue_offer(Queue *q, uint32_t data) {
	// If we have no space left in the queue return failure
	if (q->fr == 0) return;
	// If the queue end is the last element set it to zero
	if (q->end == q->capacity - 1) q->end = 0;
	// Otherwise increment it.
	else (q->end)++;
	// Keep track of fr space, decrement
	DisableInterrupts();
	(q->fr)--;
	EnableInterrupts();
	// Actually put the data in the queue.
	*(q->arr + q->end) = data;
}

void queue_put(Queue *q, uint32_t data) {
	queue_offer(q, data);
}

// *********** queue_poll **********
// Gets an element from the FIFO
// Input: Pointer to a character that will get the character read from the buffer
// Output: 1 for success and 0 for failure
//         failure is when the buffer is empty
int32_t queue_poll(Queue *q)
{
	// If the queue is empty, return failure
	if (q->fr == q->capacity) return -1;
	// Actually get data from the queue
	uint32_t retval = *(q->arr + q->begin);
	// If the queue start is the last element, set it to zero
	if (q->begin == q->capacity - 1) q->begin = 0;
	// Otherwise, increment it.
	else (q->begin)++;
	// Keep track of fr space, increment
	DisableInterrupts();
	(q->fr)++;
	EnableInterrupts();
	// Return success
	return retval;
}

uint32_t queue_get(Queue *q) {
	return queue_poll(q);
}

int queue_fr(Queue *q) {
	return q->fr;
}

int queue_capacity(Queue *q) {
	return q->capacity;
}

int queue_size(Queue *q) {
	return q->capacity - q->fr;
}

