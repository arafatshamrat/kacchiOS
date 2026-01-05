#ifndef PROCESS_H
#define PROCESS_H

#define MAX_PROCESSES 128

#define READY       0
#define CURRENT     1
#define TERMINATED  2

void process_init();
int  process_create();
void process_terminate(int pid);
void set_process_state(int pid, int state);
int  get_current_process();
int get_process_state(int pid);


#endif
