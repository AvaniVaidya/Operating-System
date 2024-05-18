#ifndef _PROCESS_H_
#define _PROCESS_H_

#include "util.h"

struct process_s {
	unsigned char pid;
	float process_arrivalTime;
	float service_time;
	unsigned char priority; 
};

typedef struct process_s process;

process * create_process(char pid, float process_arrivalTime, float service_time, unsigned char priority);

linked_list * create_process_list(int n);

#endif