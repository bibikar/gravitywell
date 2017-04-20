// stack.c
// Implementation of a stack

#include "stack.h"

// Initialize a new stack with maximum size given by capacity.
// Use malloc to allocate enough space for this
// Return the pointer to the stack
// (this is basically an array of "pointers to void")
Stack stack_init(int capacity);

// Push the pointer onto the stack
void stack_push(Stack s, void *ptr);

// Pop a pointer from the stack
void *stack_pop(Stack s);

// Get the maximum capacity of this stack
int stack_capacity(Stack s);

// Get the current size of this stack
int stack_size(Stack s);

// Get if this stack is full
// (return true or false)
int stack_full(Stack s);

// Get of this stack is empty
// (return true or false)
int stack_empty(Stack s);

