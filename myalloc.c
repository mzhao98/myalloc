/*! \file
 * Implementation of a simple memory allocator.  The allocator manages a small
 * pool of memory, provides memory chunks on request, and reintegrates freed
 * memory back into the pool.
 *
 * Adapted from Andre DeHon's CS24 2004, 2006 material.
 * Copyright (C) California Institute of Technology, 2004-2010.
 * All rights reserved.
 */

#include <stdio.h>
#include <stdlib.h>

#include "myalloc.h"


/*!
 * These variables are used to specify the size and address of the memory pool
 * that the simple allocator works against.  The memory pool is allocated within
 * init_myalloc(), and then myalloc() and free() work against this pool of
 * memory that mem points to.
 */
int MEMORY_SIZE;
unsigned char *mem;





/*!
 * This function initializes both the allocator state, and the memory pool.  It
 * must be called before myalloc() or myfree() will work at all.
 *
 * Note that we allocate the entire memory pool using malloc().  This is so we
 * can create different memory-pool sizes for testing.  Obviously, in a real
 * allocator, this memory pool would either be a fixed memory region, or the
 * allocator would request a memory region from the operating system (see the
 * C standard function sbrk(), for example).
 */

/* Struct header used for indicated size-specified blocks of memory
* Contains two integer elements: int free is a free flag. free = -1 if not free,
* free = 1 if free.
* int size represents size of memory block, not including header size itself.
*/
struct header {

    int free; /*4 bytes*/
    int size; /*4 bytes*/

};
int headersize = sizeof(struct header);
struct header *head; /*initialize head for first header of memory block*/


/*
* init_myalloc() initializes Memory block
* creates a first front header that emcompasses entire memory block.
* front header size is size of memory block - headersize, set to free.
*/
void init_myalloc() {

    mem = (unsigned char *) malloc(MEMORY_SIZE);
    if (mem == 0) {
        fprintf(stderr,
                "init_myalloc: could not get %d bytes from the system\n",
        MEMORY_SIZE);
        abort();
    }

    
    head = (struct header*)mem;
    head->size = MEMORY_SIZE - headersize; /*set size to size of entire memory block*/
    head->free = 1; /*initially set entire memory block to free*/
   
    
}


/*!
 * Attempt to allocate a chunk of memory of "size" bytes.  Return 0 if
 * allocation fails.
 * BEST FIT 
 * Starts from beginning of memory block.
 * If a block of memory that will fit the size passed into the function is found
 * Compare how close its size is to the size needed. Save the best memory block.
 * We use best sufficiently-sized block found.
 * Splits block found and creates new header
 * Updates free flag and size of each changed header. 
 */
unsigned char *myalloc(int size) {
    struct header *best = (struct header*)mem;
    int size_difference = MEMORY_SIZE;
    struct header *helperptr = (struct header*)mem;
        while(1){
            /*Check if helperptr exceeds the allocated space of memory. Return an error if
            *exceeds block of memory allotted.*/
            if ((unsigned char*)helperptr >= mem + MEMORY_SIZE) {
                break;
            }
            else{
                /*Check if current header's memory block is free and have sufficient size*/
                if (helperptr->free == 1 && helperptr->size > size + headersize){
                    /*If free and size sufficient, compare the size of block to size needed. 
                    *Save if best size. */
                    int diff = (helperptr->size) - size;
                    if (diff < size_difference)
                        {
                            size_difference = diff;
                            best = helperptr;
                        }
                    }       
                /*Check until end of memory for best block*/
                helperptr = (struct header*)((unsigned char*)helperptr + headersize 
                    + helperptr->size);
            }
        }
            /*If no block is found, return error*/
            if (size_difference==MEMORY_SIZE){
                return (unsigned char *) 0;
            }
            /*partition this memory block into the size passed
            *into function. Make new header to point to remaining space in memory block.
            *Set new block to free, and set partitioned old block to not-free.*/
            best->free = -1;
            struct header *new = (struct header*)((unsigned char*)best + headersize + size);
            new->size = best->size - headersize - size;
            new->free = 1;
            best->size = size;
            /*return pointer to start of memory block, not including header*/
            unsigned char *resultptr = (unsigned char*)best + headersize;
            return resultptr;
    }

    


/*!
 * Free a previously allocated pointer.  oldptr should be an address returned by
 * myalloc().
 * Frees memory at oldptr location. 
 * Sweeps through total MEMORYSIZE from the begining. If two headers allocating
 * memory blocks are both free, coalesces the two memory blocks, continues to end.
 */
void myfree(unsigned char *oldptr) {
    /* TODO:
     *
     * The unacceptable allocator does nothing -- that's part of why this is
     * unacceptable!
     *
     * Allocations will succeed for a little while...
     */
    struct header *helperptr = (struct header*)oldptr -1;
    helperptr->free = 1; /*Frees memory at oldptr location*/
    helperptr = (struct header*)mem; /*Starting from beginning, sweep through all memory*/
    while(1){
        /*If end of memory is reaches, end sweep.*/
        if ((unsigned char*)helperptr >= mem + MEMORY_SIZE) {
            break;
        }
        else{
            /*Check to make sure memory is not exceeded by nextptr*/
            if ((unsigned char*)helperptr+headersize+helperptr->size< mem+MEMORY_SIZE){
                struct header *nextptr = (struct header*)((unsigned char*)helperptr 
                    + headersize + helperptr->size);
                /*Make a pointer pointing to the next memory block w/ header*/
                if (helperptr->free == 1)
                {
                    if (nextptr->free ==1){
                        /*If both memory blocks are free, coalesce.*/
                        helperptr->size += headersize + nextptr->size;
                        nextptr = helperptr;
                    }
                }
                helperptr = nextptr;
                } else {
                    break;
                }
            }
            
        }
    
    }


/*HERE IS WHERE I IMPLEMENTED FIRST FIT. UNCOMMENT TO TEST FIRST FIT*/


/*!
 * Attempt to allocate a chunk of memory of "size" bytes.  Return 0 if
 * allocation fails.
 * FIRST FIT 
 * Starts from beginning of memory block.
 * If a block of memory that will fit the size passed into the function is found
 * We use first sufficiently-sized block found.
 * Splits block found and creates new header
 * Updates free flag and size of each changed header. 
 */

// unsigned char *myalloc(int size) {
    
//     struct header *helperptr = (struct header*)mem;
//         while(1)
//         {
//             /*Check if helperptr exceeds the allocated space of memory. Return an error if
//             *exceeds block of memory allotted.*/
//             if ((unsigned char*)helperptr + headersize + size >= mem + MEMORY_SIZE) 
//             {
//                 return (unsigned char *) 0;
//             }
//             else
//             {
//                 /*Check if current header's memory block is free and have sufficient size*/
//                 if (helperptr->free == 1 && helperptr->size > size + headersize)
//                 {
//                     /*If free and size sufficient, partition this memory block into the size passed
//                     *into function. Make new header to point to remaining space in memory block.
//                     *Set new block to free, and set partitioned old block to not-free.*/
//                     helperptr->free = -1;
//                     int temp_size = helperptr->size;
//                     struct header *new = (struct header*)((unsigned char*)helperptr 
//                         + headersize + size);
//                     new->size = temp_size - headersize - size;
//                     new->free = 1;
//                     helperptr->size = size;
//                     break;
//                 }
//                 else 
//                 {
//                     /*If block is inappropriate, shift to next block.*/
//                     helperptr = (struct header*)((unsigned char*)helperptr + headersize 
//                         + helperptr->size);
//                 }
//             }
//         }
//         /*return pointer to start of memory block, not including header*/
//         unsigned char *resultptr = (unsigned char*)helperptr + headersize;
//         return resultptr;
// }

    


// /*!
//  * Free a previously allocated pointer.  oldptr should be an address returned by
//  * myalloc().
//  * Frees memory at oldptr location. 
//  * Sweeps through total MEMORYSIZE from the begining. If two headers allocating
//  * memory blocks are both free, coalesces the two memory blocks, continues to end.
//  */
// void myfree(unsigned char *oldptr) {
//     struct header *helperptr = (struct header*)oldptr - 1;
//     helperptr->free = 1; /*Frees memory at oldptr location*/
//     helperptr = (struct header*)mem; /*Starting from beginning, sweep through all memory*/
//     while(1){
//         /*If end of memory is reaches, end sweep.*/
//         if ((unsigned char*)helperptr >= mem + MEMORY_SIZE) 
//         {
//             break;
//         }
//         else
//         {
//             /*Make a pointer pointing to the next memory block w/ header*/
//             struct header *nextptr = (struct header*)((unsigned char*)helperptr 
//                 + headersize + helperptr->size);
            
//             if (helperptr->free == 1)
//             {
                
//                 if (nextptr->free == 1)
//                 {
//                     /*If both memory block are free, coalesce.*/
//                     helperptr->size += headersize + nextptr->size;
//                     nextptr = helperptr;
//                 }
//             }
//             helperptr = nextptr;
//         }
            
//     }
    
// }






/*!
 * Clean up the allocator state.
 * All this really has to do is free the user memory pool. This function mostly
 * ensures that the test program doesn't leak memory, so it's easy to check
 * if the allocator does.
 */
void close_myalloc() {
    free(mem);
}
