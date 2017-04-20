// stack.c
// Implementation of a stack

#include "stack.h"
#include<stdlib.h>
#include<stdint.h>


// Initialize a new stack with maximum size given by capacity.
// Use malloc to allocate enough space for this
// Return the pointer to the stack
// (this is basically an array of "pointers to void")

Stack *stack_init(int capacity)
{
	Stack *s = malloc(sizeof(struct stack_struct));
	s->arr = malloc(sizeof(uint32_t) * capacity);
	s->size=0;
	s->capacity=capacity;
	return s;
}

// Push the pointer onto the stack
void stack_push(Stack *s, void *ptr)
{
	//*ptr is the pointer to be pushed onto the stack
	if(s->size<s->capacity)	//you can only push if the size is less than the capacity
	{
		*(s->arr+s->size-1) = ptr;
		++s->size;	
	}	
}

// Pop a pointer from the stack
void *stack_pop(Stack *s)
{	void *res;
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

