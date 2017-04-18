
#include <stdint.h>
#include <stdlib.h>

typedef struct linked_list_struct {
	void *value;
	struct linked_list_struct *ptr;
} ListNode;


// This takes an array of pointers to void! So it can be anything it points to.
ListNode *linkedlist_from_array((void *) *arr, uint32_t len) {
	ListNode *new_linkedlist = malloc(sizeof(ListNode) * len);
	ListNode *ptr = new_linkedlist;
	for (int i = 0; i < len; i++) {
		ptr->value = *(arr + i);
		ptr->ptr = ptr++;
	}
	ptr->ptr = NULL;
	return new_linkedlist;
}

// Appends the second linkedlist to the first one
// Returns the pointer to the new list.
ListNode *linkedlist_append(ListNode *one, ListNode *two) {
	ListNode *ptr = one;
	while (ptr->ptr != NULL) {
		ptr = ptr->ptr;
	}
	ptr->ptr = two;
	return one;
}

// Find the size of the linked list.
uint32_t linkedlist_size(ListNode *list) {
	uint32_t size = 0;
	while (list->ptr != NULL) {
		size++;
		list = list->ptr;
	}
	return size;
}

void linkedlist_traverse(ListNode *list, (*functionPtr)(void *)) {
	while (list->ptr != NULL) {
		(*functionPtr)(list->value);
		list = list->ptr;
	}
}

// Get the nth value in the list.
// Returns the pointer stored in the list,
// or NULL if the list doesn't have an nth item.
void *linkedlist_get(ListNode *list, int32_t n) {
	uint32_t i = 0;
	while (list->ptr != NULL) {
		if (i == n) return list->value;
		list = list->ptr;
		i++;
	}
	return NULL;
}

// Get the portion of the linkedlist starting with n.
ListNode *linkedlist_sublist(ListNode *list, int32_t n) {
	uint32_t i = 0;
	while (list->ptr != NULL) {
		if (i == n) return list;
		list = list->ptr;
		i++;
	}
	return NULL;
}

// Free the entire linked list.
void linkedlist_free(ListNode *list) {
	ListNode *curr = list;
	while ((curr = list) != NULL) {
		// The condition in the while loop first sets curr to next,
		// then checks if curr is NULL.
		list = list->ptr;
		free(curr);
	}
}

// Remove the nth element from the array.
// Returns the pointer to the head of the updated list.
// If n is outside the list, nothing happens.
ListNode *linkedlist_remove(ListNode *head, uint32_t n) {
	if (n == 0) {
		ListNode *rem = head;
		head = head->ptr;
		free(rem);
		return head;
	}
	uint32_t i = 1;
	ListNode *list = head;
	while (list->ptr != NULL) {
		if (i == n) {
			ListNode *rem = list->ptr;
			list->ptr = list->ptr->ptr;
			free(rem);
			return head;
		}	
		i++;
		list = list->ptr;
	}
	return head;
