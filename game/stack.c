// stack.c
// Implementation of a stack

#include "stack.h"
#include<stdint.h>


// Initialize a new stack with maximum size given by capacity.
// Use malloc to allocate enough space for this
// Return the pointer to the stack
// (this is basically an array of "pointers to void")

void stack_init(Stack *s, uint16_t *arr, int capacity)
{
	s->arr = arr;
	s->size=0;
	s->capacity=capacity;
	for (int i = 0; i < capacity; i++) {
		stack_push(s, i);
	}
}

// Push the data onto the stack
void stack_push(Stack *s, uint16_t data)
{
	//*data is the data to be pushed onto the stack
	if(s->size<s->capacity)	//you can only push if the size is less than the capacity
	{
		*(s->arr+s->size-1) = data;
		++s->size;	
	}	
}

// Pop a data from the stack
uint16_t stack_pop(Stack *s)
{	uint16_t res;
	if(s->size!=0)	//you can only pop as long as the size is not 0
	{
		(res) = *(s->arr+s->size-1);
	}
	return res;	
}

// Get the maximum capacity of this stack
int stack_capacity(Stack *s)
{
	return s->capacity;
}

// Get the current size of this stack
int stack_size(Stack *s)
{
	return s->size;
}

// Get if this stack is full
// (return true or false)
int stack_full(Stack *s)
{
	if(s->size==s->capacity)
		return 1;
	else 
		return 0;
}


// Get of this stack is empty
// (return true or false)
int stack_empty(Stack *s)
{
	if(s->size==0)
		return 1;
	else 
		return 0;
}

