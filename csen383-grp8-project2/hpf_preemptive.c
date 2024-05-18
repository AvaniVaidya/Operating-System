#include "stat.h"
#include "util.h"
#include "hpf_utils.h"
#include <stdio.h>
#include <stdlib.h>

// Comparison function for sorting processes by priority and then by arrival time
int compare_priority(void * d1, void * d2)
{
  process_stat * process1 = (process_stat *) d1;
	process_stat * process2 = (process_stat *) d2;
	if(((((process *)process1->proc)->priority)) < ((((process *)process2->proc)->priority))) {
		return -1;
	}
  else if (((((process *)process1->proc)->priority)) == ((((process *)process2->proc)->priority)) &&  (((process *)process1->proc)->process_arrivalTime < (((process *)process2->proc)->process_arrivalTime)))
  {
    return -1;
  }
  else {
		return 1;
	}
}

/*
This function executes the Highest Priority First (HPF) scheduling algorithm.
It processes a linked list of process entries, organizing them according to their priority levels.
Four separate queues are established to categorize processes by their distinct priorities. The function employs the priority_comp function to maintain order within these queues.
The function actively schedules processes from the queue with the highest priority that contains entries, preempting current processes when a new process of higher priority is introduced.
Additionally, it computes and aggregates various performance metrics such as response time, waiting time, and turnaround time, ultimately encapsulating these metrics within an average_stats structure.
*/

average_stats highest_priority_first_pr(linked_list * procs)
{
  int quanta = 0;

  linked_list * linked_list_1 = create_linked_list();
  linked_list * linked_list_2 = create_linked_list();
  linked_list * linked_list_3 = create_linked_list();
  linked_list * linked_list_4 = create_linked_list();

  queue * proc_queue_1 = create_queue();
  queue * proc_queue_2 = create_queue();
  queue * proc_queue_3 = create_queue();
  queue * proc_queue_4 = create_queue();

  node * proc_pointer = procs->head;
  if(procs->head == NULL) {
		fprintf(stderr,"No Process to schedule\n");
	}
  printf("\nHighest Priority First Preemptive:\n");

  process_stat * scheduled_proc = NULL;
  while(quanta<100 || scheduled_proc!=NULL )
  {
     // Schedule or enqueue processes based on priority
    if(scheduled_proc!=NULL)
    {
      if(scheduled_proc->proc->priority == 1) enqueue(proc_queue_1,scheduled_proc);
      if(scheduled_proc->proc->priority == 2) enqueue(proc_queue_2,scheduled_proc);
      if(scheduled_proc->proc->priority == 3) enqueue(proc_queue_3,scheduled_proc);
      if(scheduled_proc->proc->priority == 4) enqueue(proc_queue_3,scheduled_proc);
    }

		if(proc_pointer != NULL) {
			process * new_process = (process *)(proc_pointer->data);
			while(proc_pointer !=NULL && new_process->process_arrivalTime <= quanta) {
        if(new_process->priority == 1) enqueue(proc_queue_1,create_process_stat(new_process));
        if(new_process->priority == 2) enqueue(proc_queue_2,create_process_stat(new_process));
        if(new_process->priority == 3) enqueue(proc_queue_3,create_process_stat(new_process));
        if(new_process->priority == 4) enqueue(proc_queue_3,create_process_stat(new_process));
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

    // If no process is scheduled, dequeue from the highest-priority queue
    if(scheduled_proc == NULL) {
      if (proc_queue_1->size > 0) scheduled_proc = (process_stat *) dequeue(proc_queue_1);
      else if (proc_queue_2->size > 0) scheduled_proc = (process_stat *) dequeue(proc_queue_2);
      else if (proc_queue_3->size > 0) scheduled_proc = (process_stat *) dequeue(proc_queue_3);
      else if (proc_queue_4->size > 0) scheduled_proc = (process_stat *) dequeue(proc_queue_4);

      if (quanta>=100 && scheduled_proc->start_time == -1){
        scheduled_proc = NULL;
        continue;
      }
		}
    if(scheduled_proc != NULL) {
  			process * proc = scheduled_proc->proc;

  			
  			printf("%c",proc->pid);

  			
  			if(scheduled_proc->start_time == -1) {
  				scheduled_proc->start_time = quanta;
  			}

  			scheduled_proc->service_time++;

        if(scheduled_proc->service_time >= proc->service_time) {
          scheduled_proc->end_time = quanta;
          if(scheduled_proc->proc->priority == 1) add_node(linked_list_1,scheduled_proc);
          else if(scheduled_proc->proc->priority == 2) add_node(linked_list_2,scheduled_proc);
          else if(scheduled_proc->proc->priority == 3) add_node(linked_list_3,scheduled_proc);
          else if(scheduled_proc->proc->priority == 4) add_node(linked_list_4,scheduled_proc);
          scheduled_proc = NULL;
        }
      }else {
    			printf("_");
  		}
  		
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

    average.avg_response_time = (average1.avg_response_time + average2.avg_response_time + average3.avg_response_time + average4.avg_response_time)/4 ;
    average.avg_wait_time = (average1.avg_wait_time + average2.avg_wait_time + average3.avg_wait_time + average4.avg_wait_time)/4 ;
    average.avg_turnaround = (average1.avg_turnaround + average2.avg_turnaround + average3.avg_turnaround + average4.avg_turnaround)/4 ;
    average.avg_throughput = (average1.avg_throughput + average2.avg_throughput + average3.avg_throughput + average4.avg_throughput) ;

    printf("\nThe average times of High Priority First Preemptive for all queues:\n");
    printf("Average Turnaround Time(TAT) :%.1f\n",average.avg_turnaround);
    printf("Average Wait Time(WT) : %.1f\n",average.avg_wait_time);
    printf("Average Response Time(RT) : %.1f\n",average.avg_response_time);
    
    return average;
}