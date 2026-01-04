/* kernel.c - Main kernel with null process */
#include "types.h"
#include "serial.h"
#include "string.h"
#include "memory.h"
#include "process.h"
#include "scheduler.h"

#define MAX_INPUT 128
#define NUM_DUMMY_PROCESSES 4
#define SIMULATION_TICKS 30

/* Helper function to print a number */
static void print_number(int n) {
    char buf[12];
    int i = 0;
    int neg = 0;
    
    if (n < 0) {
        neg = 1;
        n = -n;
    }
    
    if (n == 0) {
        serial_putc('0');
        return;
    }
    
    while (n > 0) {
        buf[i++] = '0' + (n % 10);
        n /= 10;
    }
    
    if (neg) serial_putc('-');
    while (i > 0) serial_putc(buf[--i]);
}

/* Print process state as string */
static void print_state(int state) {
    switch (state) {
        case READY:      serial_puts("READY");      break;
        case CURRENT:    serial_puts("RUNNING");    break;
        case TERMINATED: serial_puts("TERMINATED"); break;
        default:         serial_puts("UNKNOWN");    break;
    }
}

/* Display status of all processes */
static void show_process_status(void) {
    serial_puts("  +-----+------------+\n");
    serial_puts("  | PID |   STATE    |\n");
    serial_puts("  +-----+------------+\n");
    
    for (int i = 0; i < NUM_DUMMY_PROCESSES; i++) {
        serial_puts("  |  ");
        print_number(i);
        serial_puts("  | ");
        print_state(get_process_state(i));
        
        /* Padding for alignment */
        int state = get_process_state(i);
        if (state == READY) serial_puts("      ");
        else if (state == CURRENT) serial_puts("    ");
        else if (state == TERMINATED) serial_puts(" ");
        
        serial_puts("|\n");
    }
    serial_puts("  +-----+------------+\n");
}

/* Simulate process execution */
static void simulate_processes(void) {
    serial_puts("\n[SIMULATION] Starting process simulation...\n");
    serial_puts("[SIMULATION] Time quantum = ");
    print_number(TIME_QUANTUM);
    serial_puts(" ticks\n\n");
    
    /* Run simulation for specified ticks */
    for (int tick = 1; tick <= SIMULATION_TICKS; tick++) {
        serial_puts("--- Tick ");
        print_number(tick);
        serial_puts(" ---\n");
        
        /* Get current running process */
        int curr = get_current_process();
        if (curr != -1) {
            serial_puts("  Process ");
            print_number(curr);
            serial_puts(" executing...\n");
        } else {
            serial_puts("  CPU idle - no process running\n");
        }
        
        /* Simulate termination: terminate current process every 5 ticks */
        if (tick % 5 == 0 && curr != -1 && get_process_state(curr) != TERMINATED) {
            serial_puts("  ** Process ");
            print_number(curr);
            serial_puts(" finished its work - terminating **\n");
            process_terminate(curr);
            /* If this was the running process, schedule next */
            schedule();
        }
        
        
        /* Call scheduler tick (handles time quantum) */
        scheduler_tick();
        
        /* Show context switch if it happened */
        int new_curr = get_current_process();
        if (new_curr != curr && new_curr != -1) {
            serial_puts("  [SCHEDULER] Context switch to Process ");
            print_number(new_curr);
            serial_puts("\n");
        }
        
        serial_puts("\n");
    }
    
    serial_puts("[SIMULATION] Simulation complete!\n\n");
}

void kmain(void) {
    char input[MAX_INPUT];
    int pos = 0;
    
    /* Initialize hardware */
    serial_init();
    
    /* Initialize subsystems */
    memory_init();
    process_init();
    scheduler_init();
    
    /* Print welcome message */
    serial_puts("\n");
    serial_puts("========================================\n");
    serial_puts("    kacchiOS - Minimal Baremetal OS\n");
    serial_puts("========================================\n");
    serial_puts("Hello from kacchiOS!\n\n");
    
    /* Create dummy processes */
    serial_puts("[KERNEL] Creating dummy processes...\n");
    for (int i = 0; i < NUM_DUMMY_PROCESSES; i++) {
        int pid = process_create();
        if (pid >= 0) {
            serial_puts("  Created Process ");
            print_number(pid);
            serial_puts(" (stack: 2KB, heap: 4KB)\n");
        } else {
            serial_puts("  Failed to create process!\n");
        }
    }
    serial_puts("\n");
    
    /* Show initial process table */
    serial_puts("[KERNEL] Initial process table:\n");
    show_process_status();
    
    /* Schedule first process */
    serial_puts("\n[KERNEL] Starting scheduler...\n");
    schedule();
    
    serial_puts("[KERNEL] First process scheduled: ");
    print_number(get_current_process());
    serial_puts("\n");
    
    /* Run simulation */
    simulate_processes();
    
    /* Show final process table */
    serial_puts("[KERNEL] Final process table:\n");
    show_process_status();
    
    /* Clean up - terminate remaining processes */
    serial_puts("\n[KERNEL] Cleaning up remaining processes...\n");
    for (int i = 0; i < NUM_DUMMY_PROCESSES; i++) {
        if (get_process_state(i) != TERMINATED) {
            process_terminate(i);
            serial_puts("  Terminated Process ");
            print_number(i);
            serial_puts("\n");
        }
    }
    
    serial_puts("\n[KERNEL] All processes terminated.\n");
    serial_puts("[KERNEL] Entering interactive shell...\n\n");
    
    /* Main Loop - the "null process" */
    while (1) {
        serial_puts("kacchiOS> ");
        pos = 0;
        
        /* Read input line */
        while (1) {
            char c = serial_getc();
            
            /* Handle Enter key */
            if (c == '\r' || c == '\n') {
                input[pos] = '\0';
                serial_puts("\n");
                break;
            }
            /* Handle Backspace */
            else if ((c == '\b' || c == 0x7F) && pos > 0) {
                pos--;
                serial_puts("\b \b");  /* Erase character on screen */
            }
            /* Handle normal characters */
            else if (c >= 32 && c < 127 && pos < MAX_INPUT - 1) {
                input[pos++] = c;
                serial_putc(c);  /* Echo character */
            }
        }
        
        /* Process commands */
        if (pos > 0) {
            if (strcmp(input, "help") == 0) {
                serial_puts("Available commands:\n");
                serial_puts("  help  - Show this help\n");
                serial_puts("  ps    - Show process table\n");
                serial_puts("  run   - Run process simulation again\n");
                serial_puts("  clear - Clear screen\n");
            }
            else if (strcmp(input, "ps") == 0) {
                serial_puts("Process table:\n");
                show_process_status();
            }
            else if (strcmp(input, "run") == 0) {
                /* Reinitialize and run simulation again */
                memory_init();
                process_init();
                scheduler_init();
                
                for (int i = 0; i < NUM_DUMMY_PROCESSES; i++) {
                    process_create();
                }
                schedule();
                simulate_processes();
                show_process_status();
            }
            else if (strcmp(input, "clear") == 0) {
                serial_puts("\033[2J\033[H");  /* ANSI clear screen */
            }
            else {
                serial_puts("Unknown command: ");
                serial_puts(input);
                serial_puts("\nType 'help' for available commands.\n");
            }
        }
    }
}