#include "stat.h"
#include "util.h"
#include "hpf_utils.h"
#include <stdio.h>
#include <stdlib.h>

/*
This function executes the Non-preemptive Highest Priority First (HPF) scheduling algorithm using FCFS.
It processes a linked list of process entries, organizing them according to their priority levels.
Four separate queues are established to categorize processes by their distinct priorities. The function employs the priority_comp function to maintain order within these queues.
The function actively schedules processes from the queue with the highest priority that contains entries, without preempting the currently running process.
Additionally, it computes and aggregates various performance metrics such as response time, waiting time, and turnaround time, ultimately encapsulating these metrics within an average_stats structure.
*/

average_stats highest_priority_first_np(linked_list * procs)
{
    int quanta = 0;

    // Create separate linked lists to store completed processes for each priority level
    linked_list * linked_list_1 = create_linked_list();
    linked_list * linked_list_2 = create_linked_list();
    linked_list * linked_list_3 = create_linked_list();
    linked_list * linked_list_4 = create_linked_list();

    // Create separate queues for each priority level
    queue * proc_queue_1 = create_queue();
    queue * proc_queue_2 = create_queue();
    queue * proc_queue_3 = create_queue();
    queue * proc_queue_4 = create_queue();

    // Get the head of the input process list
    node * proc_pointer = procs->head;
    if(procs->head == NULL) {
        fprintf(stderr,"No Process to schedule\n");
    }
    printf("\nHighest Priority First Non-preemptive (FCFS):\n");

    process_stat * scheduled_proc = NULL;
    while(quanta<100 || scheduled_proc!=NULL )
    {
        // Enqueue new processes based on priority
        if(proc_pointer != NULL) {
            process * new_process = (process *)(proc_pointer->data);
            while(proc_pointer !=NULL && new_process->process_arrivalTime <= quanta) {
                // Enqueue the process into the corresponding priority queue
                if(new_process->priority == 1) enqueue(proc_queue_1,create_process_stat(new_process));
                if(new_process->priority == 2) enqueue(proc_queue_2,create_process_stat(new_process));
                if(new_process->priority == 3) enqueue(proc_queue_3,create_process_stat(new_process));
                if(new_process->priority == 4) enqueue(proc_queue_3,create_process_stat(new_process));
                // Sort the processes within each priority queue based on arrival time
                sort(proc_queue_1,compare_priority);
                sort(proc_queue_2,compare_priority);
                sort(proc_queue_3,compare_priority);
                sort(proc_queue_4,compare_priority);
                proc_pointer = proc_pointer->next;
                if(proc_pointer!=NULL)
                {
                    new_process = (process *)(proc_pointer->data);
                }
            }
        }

        // If no process is scheduled, dequeue from the highest-priority non-empty queue
        if(scheduled_proc == NULL) {
            if (proc_queue_1->size > 0) scheduled_proc = (process_stat *) dequeue(proc_queue_1);
            else if (proc_queue_2->size > 0) scheduled_proc = (process_stat *) dequeue(proc_queue_2);
            else if (proc_queue_3->size > 0) scheduled_proc = (process_stat *) dequeue(proc_queue_3);
            else if (proc_queue_4->size > 0) scheduled_proc = (process_stat *) dequeue(proc_queue_4);

            // If the current time is greater than or equal to 100 and the process has not started yet,
            // set the scheduled process to NULL and continue to the next iteration
            if (quanta>=100 && scheduled_proc->start_time == -1){
                scheduled_proc = NULL;
                continue;
            }
        }

        if(scheduled_proc != NULL) {
            process * proc = scheduled_proc->proc;

            printf("%c",proc->pid);

            // If the process has not started yet, set its start time to the current time
            if(scheduled_proc->start_time == -1) {
                scheduled_proc->start_time = quanta;
            }

            // Increment the service time of the currently running process
            scheduled_proc->service_time++;

            // If the process has completed its service time, set its end time and add it to the corresponding priority-based linked list
            if(scheduled_proc->service_time >= proc->service_time) {
                scheduled_proc->end_time = quanta + 1;
                if(scheduled_proc->proc->priority == 1) add_node(linked_list_1,scheduled_proc);
                else if(scheduled_proc->proc->priority == 2) add_node(linked_list_2,scheduled_proc);
                else if(scheduled_proc->proc->priority == 3) add_node(linked_list_3,scheduled_proc);
                else if(scheduled_proc->proc->priority == 4) add_node(linked_list_4,scheduled_proc);
                scheduled_proc = NULL;
            }
        } else {
            // If there is no scheduled process, print an underscore to represent idle time
            printf("_");
        }

        // Increment the time quantum
        quanta++;
    }

    // Calculate and print the average statistics for each priority queue
    average_stats average1,average2,average3,average4,average;
    printf("\nFor Priority Queue 1");
    average1 = print_policy_stat(linked_list_1);
    printf("\nFor Priority Queue 2");
    average2 = print_policy_stat(linked_list_2);
    printf("\nFor Priority Queue 3");
    average3 = print_policy_stat(linked_list_3);
    printf("\nFor Priority Queue 4");
    average4 = print_policy_stat(linked_list_4);

    // Calculate the overall average statistics by taking the average of the statistics from each priority queue
    average.avg_response_time = (average1.avg_response_time + average2.avg_response_time + average3.avg_response_time + average4.avg_response_time)/4 ;
    average.avg_wait_time = (average1.avg_wait_time + average2.avg_wait_time + average3.avg_wait_time + average4.avg_wait_time)/4 ;
    average.avg_turnaround = (average1.avg_turnaround + average2.avg_turnaround + average3.avg_turnaround + average4.avg_turnaround)/4 ;
    average.avg_throughput = (average1.avg_throughput + average2.avg_throughput + average3.avg_throughput + average4.avg_throughput) ;

    // Print the overall average statistics for the Highest Priority First Non-preemptive (FCFS) algorithm
    printf("\nThe average times of High Priority First Non-preemptive (FCFS) for all queues:\n");
    printf("Average Turnaround Time(TAT) :%.1f\n",average.avg_turnaround);
    printf("Average Wait Time(WT) : %.1f\n",average.avg_wait_time);
    printf("Average Response Time(RT) : %.1f\n",average.avg_response_time);

    return average;
}