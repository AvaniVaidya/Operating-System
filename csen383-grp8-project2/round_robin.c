#include <stdio.h>
#include <stdlib.h>
#include "stat.h"
#include "util.h"

average_stats round_robin(linked_list * processes, int time_slice) {
	int time = 0;

    //Process Queue Creation
	queue *process_queue = (queue *)create_queue();
	node * proc_ptr = processes->head;
	if(processes->head == NULL) {
		fprintf(stderr,"No Process to schedule\n");
	}
	
	process_stat * scheduled_process = NULL;

	linked_list *ll = create_linked_list();
	printf("\nRound Robin Algorithm:\n");
	node * current_node = NULL;
	int cur_run = 0;

    //keep checking while time quanta is less than 100 or the process queue is not empty
	while(time <100 || process_queue->size > 0) {
		//check for incoming new process and do enqueue.
		if(proc_ptr != NULL && time < 100) {
			process * new_process = (process *)(proc_ptr->data);
			while(proc_ptr!=NULL && new_process->process_arrivalTime <= time) {
				enqueue(process_queue,create_process_stat(new_process));
				proc_ptr = proc_ptr->next;
				if(proc_ptr!=NULL)
					new_process = (process *)(proc_ptr->data);
			}
		}

		//check process queue and schedule it if there is no scheduled process now
		if(current_node == NULL) {
			cur_run = 0;
			current_node = process_queue->head;
		} else if(cur_run == time_slice) {
			cur_run = 0;
			current_node = current_node->next;
			if(current_node == NULL) {
				current_node = process_queue->head;
			}
		}

		if(current_node != NULL) {
			scheduled_process = (process_stat *) current_node->data;
			process * proc = scheduled_process->proc;

			if(time >=100) {
				if(scheduled_process->start_time == -1) {
                    //Do not start any new process, remove it from processQueue
					free(scheduled_process);
					node * next = current_node->next;
					remove_node(process_queue,current_node->data);
					current_node = next;
					cur_run = 0;
					continue;
				}
			}
            //add the currently running process to the Gantt chart
			printf("%c",proc->pid);
			cur_run++;
            //update the current processes stat
			if(scheduled_process->start_time == -1) {
				scheduled_process->start_time = time;
			}
			scheduled_process->service_time++;

			if(scheduled_process->service_time >= proc->service_time) {
				scheduled_process->end_time = time;
				add_node(ll,scheduled_process);
				node * next = current_node -> next;
				remove_node(process_queue, current_node->data);
				current_node = next;
				cur_run = 0;
			}
		} else {
			printf("_");
		}
        //keep increasing the time
		time++;
	}

	return print_policy_stat(ll);
	
}
