// Provides implementations of the malloc(), calloc(), and free() functions 
// for dynamic memory allocation. Here are some comments on the code:

#define _DEFAULT_SOURCE
#define _BSD_SOURCE 
#include <malloc.h> 
#include <stdio.h> 

// Include any other headers we need here
#include <unistd.h>
#include <string.h>

// NOTE: You should NOT include <stdlib.h> in your final implementation

#include <debug.h> // definition of debug_printf


typedef struct block {
    size_t size;
    struct block *next;
} block_t;


// global variable for the head of the linked list
block_t *head = NULL;

void *mymalloc(size_t size) {
    block_t *current, *prev;
    void *result;

    if (size <= 0) {
        return NULL;
    }

    // if head is null, this is the first call to malloc
    if (head == NULL) {
      
        head = sbrk(0);
        if (sbrk(sizeof(block_t) + size) == (void*) -1) {
            return NULL;
        }
        head->size = size;
        head->next = NULL;
    } else {
        // traverse the linked list to find a free block
        current = head;
        prev = NULL;
        while (current != NULL && (current->size < size)) {
            prev = current;
            current = current->next;
        }
        // if no free block was found, allocate a new one
        if (current == NULL) {
            prev->next = sbrk(0);
            if (sbrk(sizeof(block_t) + size) == (void*) -1) {
                return NULL;
            }
            prev->next->size = size;
            prev->next->next = NULL;
            current = prev->next;
        } else {
            // found a free block, update the linked list
            if (current->size == size) {
                // exact match, remove the block from the list
                if (prev != NULL) {
                    prev->next = current->next;
                } else {
                    head = current->next;
                }
            } else {
                // split the block, allocate the right amount of memory
                block_t *new_block = (block_t*)((void*)current + sizeof(block_t) + size);
                new_block->size = current->size - size - sizeof(block_t);
                new_block->next = current->next;
                if (prev != NULL) {
                    prev->next = new_block;
                } else {
                    head = new_block;
                }
            }
        }
    }

    result = (void*)(current + 1);
    return result;
}

void *mycalloc(size_t nmemb, size_t size) {
    size_t total_size = nmemb * size;
    void *result = mymalloc(total_size);
    if (result != NULL) {
        // initialize the memory to zero
        memset(result, 0, total_size);
    }
    return result;
}

void myfree(void *ptr) {
    if (ptr == NULL) {
        return;
    }

    block_t *current = (block_t*)ptr - 1;
    current->next = head;
    head = current;
}
