#ifndef SCHEDULER_H
#define SCHEDULER_H

/* Time quantum for Round Robin scheduling */
#define TIME_QUANTUM 2

/* Initialize scheduler */
void scheduler_init();

/* Select next process to run */
void schedule();

/* Called on every timer tick */
void scheduler_tick();



#endif
