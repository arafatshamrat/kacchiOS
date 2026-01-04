#ifndef MEMORY_H
#define MEMORY_H

#define RAM_SIZE 65536
#define MAX_BLOCKS 128

void memory_init();
void* mem_alloc(int size, int pid);
void mem_free(int pid);

#endif
