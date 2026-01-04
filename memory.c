#include "memory.h"
#define NULL ((void*)0)

/*
    Fake RAM to simulate physical memory
*/
static char RAM[RAM_SIZE];

/*
    Structure to keep track of memory blocks
*/
typedef struct {
    int start;      // starting index in RAM
    int size;       // size of block in bytes
    int free;       // 1 = free, 0 = used
    int pid;        // owner process id
} mem_block_t;

/*
    Memory block table
*/
static mem_block_t blocks[MAX_BLOCKS];
static int block_count = 0;

/*
    Initialize memory manager
    Initially whole RAM is free
*/
void memory_init() {
    blocks[0].start = 0;
    blocks[0].size  = RAM_SIZE;
    blocks[0].free  = 1;
    blocks[0].pid   = -1;

    block_count = 1;
}

/*
    Allocate memory using First-Fit policy
*/
void* mem_alloc(int size, int pid) {
    for (int i = 0; i < block_count; i++) {

        // Check if block is free and has enough space
        if (blocks[i].free == 1 && blocks[i].size >= size) {

            // Create a new free block with remaining memory
            blocks[block_count].start = blocks[i].start + size;
            blocks[block_count].size  = blocks[i].size - size;
            blocks[block_count].free  = 1;
            blocks[block_count].pid   = -1;

            // Allocate current block to process
            blocks[i].size = size;
            blocks[i].free = 0;
            blocks[i].pid  = pid;

            block_count++;

            // Return pointer to allocated memory
            return (void*)&RAM[blocks[i].start];
        }
    }

    // No suitable memory block found
    return NULL;
}

/*
    Free all memory blocks owned by a process
*/
void mem_free(int pid) {
    for (int i = 0; i < block_count; i++) {
        if (blocks[i].free == 0 && blocks[i].pid == pid) {
            blocks[i].free = 1;
            blocks[i].pid  = -1;
        }
    }
}
