#include <stdio.h>
#include <stdlib.h>
#include "stat.h"
#include "util.h"

// function to compare time the completion time of processes while sorting 
int compare_time(void * data1, void * data2)
{
  process_stat * p_state_1 = (process_stat *) data1;
  process_stat * p_state_2 = (process_stat *) data2;
  if(((((process *)p_state_1->proc)->service_time) - (p_state_1->service_time)) < ((((process *)p_state_2->proc)->service_time) - (p_state_2->service_time))) {
    return -1;
  } else {
    return 1;
  }
}


average_stats shortest_remainingTime_pr(linked_list * input_processes)
{
  int time = 0;
  //Process Queue Creation
  queue * process_queue = create_queue();
  // creation of linked list for managaing the order of processes in preemeption
  linked_list * completed_process = create_linked_list();

  node * process_ptr = input_processes->head;

  process_stat * cur_ptr = NULL;
  printf("\n\n Shortest Remaining Time Algorithm:\n");
  while(time < 100 || cur_ptr != NULL)
  {
    if(cur_ptr != NULL)
    {
      enqueue(process_queue, cur_ptr);
      cur_ptr = NULL;
    }
    //check for incoming new process and enqueue it in the queue
        if(process_ptr != NULL) {
            process * new_process = (process *)(process_ptr->data);
            while(process_ptr !=NULL && new_process->process_arrivalTime <= time) {
                enqueue(process_queue, create_process_stat(new_process));       
                process_ptr = process_ptr->next;
                if(process_ptr != NULL)
                {
                    new_process = (process *)(process_ptr->data);
                }
            }
        // sort all the processes that have arrived based on their remaining running time to completion
        sort(process_queue,compare_time);
    }

    //if there is no scheduled process, then check process queue and schedule it
    if(cur_ptr == NULL && process_queue->size > 0) {
        cur_ptr = (process_stat *) dequeue(process_queue);

        // If the process has not started before quanta 100, 
        //remove the process from the queue and take the next process in queue for execution
        while(time>=100 && cur_ptr->start_time == -1)
        {
            cur_ptr = (process_stat *) dequeue(process_queue);
        }
    }

    if(cur_ptr != NULL) {
              process * proc = cur_ptr->proc;
            //add the currently running process to the Gantt chart
              printf("%c",proc->pid);

            //update the current processes stat
              if(cur_ptr->start_time == -1) {
                  cur_ptr->start_time = time;
              }

              cur_ptr->service_time++;

        if(cur_ptr->service_time >= proc->service_time) {
          cur_ptr->end_time = time;
          add_node(completed_process,cur_ptr);
          cur_ptr = NULL;
        }
      } else {
            printf("_");
        }
        
        //keep increasing the time
          time++;
  }

  return print_policy_stat(completed_process);
}
