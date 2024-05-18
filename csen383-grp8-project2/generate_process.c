#include <stdio.h>
#include <stdlib.h>
#include "generate_process.h"
#include "util.h"
#include <time.h>

int compare(void * data1, void * data2) {
	process *p1 = (process *)data1;
	process *p2 = (process *)data2;
	if(p1->process_arrivalTime < p2->process_arrivalTime) {
		return -1;
	} else if(p1->process_arrivalTime == p2->process_arrivalTime){
		return 0;
	} else {
		return 1;
	}
}

linked_list * create_process_list(int n) {
	linked_list * process_list = create_linked_list();
	char pid = 'A';
	unsigned char priority;
	float process_arrivalTime, service_time;

   /* Intializes random number generator */
   int seed = time(NULL); 
    srand(seed); 

	if(process_list == NULL) fprintf(stderr,"Unable to create Linked List\n");

	while(n--) {
		process_arrivalTime = rand() % 100;	// will return num between 0 and 99

        service_time = (rand() % 11); // will return num between 0 and 10
        if (service_time == 0) {
            service_time += 0.1; // service_time = 0.1 .. 10 
        }

        priority = rand() % 5; 
        if (priority == 0) priority += 1; // priority between 1 .. 4

        /*
		service_time = ( rand() % 10 ) + 1; 
		priority = ((rand() % 4) + 1 );
		*/

		process *p = create_process(pid,process_arrivalTime,service_time,priority);

		add_node(process_list,p);
		pid++;
	}

	sort(process_list,compare); // sort based on arrival time

	node * ptr = process_list->head;
	pid = 'A';
	while(ptr!=NULL) {
		((process *)ptr->data)->pid = pid;
		if(pid == 'Z') pid = 'a' - 1;
		pid++;
		ptr=ptr->next;
	}

	return process_list;
}

process * create_process(char pid, float process_arrivalTime, float service_time, unsigned char priority) {
	process * proc = (process *) malloc(sizeof(process));
	proc->pid = pid;
	proc->process_arrivalTime = process_arrivalTime;
	proc->service_time = service_time;
	proc->priority = priority;
	return proc;
}