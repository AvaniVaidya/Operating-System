#include <stdio.h>
#include <stdlib.h>
#include "stat.h"
#include "util.h"

// Function to compare the service time of processes while sorting 
int compare_service_time(void * data1, void * data2)
{
    process_stat * p_state_1 = (process_stat *) data1;
    process_stat * p_state_2 = (process_stat *) data2;
    if(((process *)p_state_1->proc)->service_time < ((process *)p_state_2->proc)->service_time) {
        return -1;
    } else {
        return 1;
    }
}

average_stats shortest_job_first_np(linked_list * input_processes)
{
    int time = 0;
    queue * process_queue = create_queue();
    linked_list * completed_process = create_linked_list();
    node * process_ptr = input_processes->head;
    process_stat * cur_ptr = NULL;

    printf("\n\n Shortest Job First Algorithm:\n");
    while(time < 100 || cur_ptr != NULL)
    {
        // If there is a currently running process, enqueue it back into the process queue
        if(cur_ptr != NULL)
        {
            enqueue(process_queue, cur_ptr);
            cur_ptr = NULL;
        }

        // Check for new processes that have arrived and enqueue them in the process queue
        if(process_ptr != NULL) {
            process * new_process = (process *)(process_ptr->data);
            while(process_ptr != NULL && new_process->process_arrivalTime <= time) {
                enqueue(process_queue, create_process_stat(new_process));       
                process_ptr = process_ptr->next;
                if(process_ptr != NULL)
                {
                    new_process = (process *)(process_ptr->data);
                }
            }
            // Sort all the processes in the process queue based on their service time
            sort(process_queue, compare_service_time);
        }

        // If there is no currently running process, dequeue the process with the shortest service time from the process queue
        if(cur_ptr == NULL && process_queue->size > 0) {
            cur_ptr = (process_stat *) dequeue(process_queue);
            // If the current time is greater than or equal to 100 and the process has not started yet,
            // dequeue the next process from the queue
            while(time >= 100 && cur_ptr->start_time == -1)
            {
                cur_ptr = (process_stat *) dequeue(process_queue);
            }
        }

        // If there is a currently running process, execute it
        if(cur_ptr != NULL) {
            process * proc = cur_ptr->proc;
            printf("%c", proc->pid);

            // If the process has not started yet, set its start time to the current time
            if(cur_ptr->start_time == -1) {
                cur_ptr->start_time = time;
            }

            // Increment the service time of the currently running process
            cur_ptr->service_time++;

            // If the process has completed its service time, set its end time and add it to the completed process list
            if(cur_ptr->service_time >= proc->service_time) {
                cur_ptr->end_time = time + 1;
                add_node(completed_process, cur_ptr);
                cur_ptr = NULL;
            }
        } else {
            // If there is no currently running process, print an underscore to represent idle time
            printf("_");
        }

        // Increment the time
        time++;
    }

    // Print the statistics for the completed processes and return the average statistics
    return print_policy_stat(completed_process);
}