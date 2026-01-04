#include "scheduler.h"
#include "process.h"

/*
    Scheduler internal variables
*/
static int time_slice = 0;   // how long current process has run
static int last_pid   = -1;  // last scheduled process

/*
    Initialize scheduler
*/
void scheduler_init() {
    time_slice = 0;
    last_pid = -1;
}

/*
    Pick next READY process using Round Robin
*/
void schedule() {

    int start = last_pid + 1;

    for (int i = 0; i < MAX_PROCESSES; i++) {

        int pid = (start + i) % MAX_PROCESSES;

        /* Skip invalid or terminated processes */
        if (pid < 0)
            continue;

        /* Select READY process */
        if (get_process_state(pid) == READY) {

            /* If another process was running, make it READY */
            int curr = get_current_process();
            if (curr != -1 && curr != pid) {
                set_process_state(curr, READY);
            }

            /* Set new process as CURRENT */
            set_process_state(pid, CURRENT);
            last_pid = pid;
            time_slice = 0;
            return;
        }
    }
    /* If no READY process found, CPU stays idle */
}

/*
    Called on every timer interrupt
*/
void scheduler_tick() {

    int curr = get_current_process();

    /* No running process */
    if (curr == -1)
        return;

    time_slice++;

    /* Time quantum expired */
    if (time_slice >= TIME_QUANTUM) {

        /* Move current process back to READY */
        set_process_state(curr, READY);

        /* Schedule next process */
        schedule();
    }
}
