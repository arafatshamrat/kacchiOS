#include "process.h"
#include "memory.h"

/* Define NULL explicitly */
#define NULL ((void*)0)

/* Process Control Block (PCB) */
typedef struct {
    int pid;            // process id
    int state;          // READY, CURRENT, TERMINATED
    void* stack_ptr;    // stack memory pointer
    void* heap_ptr;     // heap memory pointer
} pcb_t;

/* Process table */
static pcb_t process_table[MAX_PROCESSES];

/* Process manager variables */
static int process_count = 0;
static int current_pid   = -1;

/*
    Initialize process manager
*/
void process_init() {
    process_count = 0;
    current_pid = -1;

    for (int i = 0; i < MAX_PROCESSES; i++) {
        process_table[i].pid = -1;
        process_table[i].state = TERMINATED;
        process_table[i].stack_ptr = NULL;
        process_table[i].heap_ptr  = NULL;
    }
}

/*
    Create a new process
*/
int process_create() {

    if (process_count >= MAX_PROCESSES)
        return -1;

    int pid = process_count;

    /* Initialize PCB */
    process_table[pid].pid   = pid;
    process_table[pid].state = READY;

    /* Allocate stack and heap */
    process_table[pid].stack_ptr = mem_alloc(2048, pid);
    process_table[pid].heap_ptr  = mem_alloc(4096, pid);

    /* Check allocation failure */
    if (process_table[pid].stack_ptr == NULL ||
        process_table[pid].heap_ptr  == NULL) {

        process_table[pid].state = TERMINATED;
        mem_free(pid);
        return -1;
    }

    process_count++;
    return pid;
}

/*
    Change process state
*/
void set_process_state(int pid, int state) {

    if (pid < 0 || pid >= process_count)
        return;

    process_table[pid].state = state;

    if (state == CURRENT)
        current_pid = pid;
}

/*
    Get currently running process
*/
int get_current_process() {
    return current_pid;
}

/*
    Terminate a process
*/
void process_terminate(int pid) {

    if (pid < 0 || pid >= process_count)
        return;

    process_table[pid].state = TERMINATED;

    /* Free all memory of this process */
    mem_free(pid);

    if (current_pid == pid)
        current_pid = -1;
}

int get_process_state(int pid) {
    if (pid < 0 || pid >= MAX_PROCESSES)
        return TERMINATED;

    return process_table[pid].state;
}

