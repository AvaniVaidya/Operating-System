#include <stdio.h>
#include "util.h"
#include "generate_process.h"
#include "stat.h"

int main(int argc,char **argv) {
    int counter = 0;

    // Arrays to hold statistics for different scheduling algorithms and final results
    average_stats fcfs[6], sjf[6], srf[6], rr[6], hpfp[6], hpfnp[6], final[6];

    // Loop to perform 5 iterations
    while (counter < 5){
        
        printf("\n*********************************************************************************\n");
        printf("\t\t\t\tIteration -%d\n",counter+1);
        printf("*********************************************************************************\n");

        // Generate a list of 52 processes
        linked_list * processList = create_process_list(52);
        node * ptr = processList->head;

        //Process Details
        printf("------------------------------------------------------------\n");
        printf("Process Name | Arrival Time | Expected Run Time | Priority |\n");
        printf("------------------------------------------------------------\n");
        while(ptr != NULL) {
            process *proc = (process *)ptr->data;
            printf("%10c | %12.1f | %8.1f | %8d |\n",proc->pid,proc->process_arrivalTime,proc->service_time,proc->priority);
            ptr = ptr->next;
        }
        printf("--------------------------------------------------\n");
        printf("Total Number of Processes = %d\n",processList->size);
        printf("--------------------------------------------------\n");

        
        // Execution of different scheduling algorithms
        fcfs[counter] = first_come_first_serve(processList);
        
        sjf[counter] = shortest_job_first_np(processList);
        
        srf[counter] = shortest_remainingTime_pr(processList);
        
        rr[counter] = round_robin(processList,5);
        
        hpfp[counter] = highest_priority_first_pr(processList);
        
        hpfnp[counter] = highest_priority_first_np(processList);

        counter++;
    }

    //initializing final array
    for (int i = 0; i<6 ; i++)
    {
        final[i].avg_response_time = 0;
        final[i].avg_wait_time = 0;
        final[i].avg_turnaround = 0;
        final[i].avg_throughput = 0;
    }

    // Sum up of all the given stats
    for (int i = 0; i<5 ; i++)
    {
        final[0].avg_response_time += fcfs[i].avg_response_time;
        final[1].avg_response_time += sjf[i].avg_response_time;
        final[2].avg_response_time += srf[i].avg_response_time;
        final[3].avg_response_time += rr[i].avg_response_time;
        final[4].avg_response_time += hpfp[i].avg_response_time;
        final[5].avg_response_time += hpfnp[i].avg_response_time;

        final[0].avg_wait_time += fcfs[i].avg_wait_time;
        final[1].avg_wait_time += sjf[i].avg_wait_time;
        final[2].avg_wait_time += srf[i].avg_wait_time;
        final[3].avg_wait_time += rr[i].avg_wait_time;
        final[4].avg_wait_time += hpfp[i].avg_wait_time;
        final[5].avg_wait_time += hpfnp[i].avg_wait_time;

        final[0].avg_turnaround += fcfs[i].avg_turnaround;
        final[1].avg_turnaround += sjf[i].avg_turnaround;
        final[2].avg_turnaround += srf[i].avg_turnaround;
        final[3].avg_turnaround += rr[i].avg_turnaround;
        final[4].avg_turnaround += hpfp[i].avg_turnaround;
        final[5].avg_turnaround += hpfnp[i].avg_turnaround;

        final[0].avg_throughput += fcfs[i].avg_throughput;
        final[1].avg_throughput += sjf[i].avg_throughput;
        final[2].avg_throughput += srf[i].avg_throughput;
        final[3].avg_throughput += rr[i].avg_throughput;
        final[4].avg_throughput += hpfp[i].avg_throughput;
        final[5].avg_throughput += hpfnp[i].avg_throughput;
    }

    for (int i = 0; i<6 ; i++)
    {
        final[i].avg_response_time /= 5;
        final[i].avg_wait_time /= 5;
        final[i].avg_turnaround /= 5;
        final[i].avg_throughput /= 5;
    }
    printf("\n\n\n");
    
    printf("----------------------------------------------------------------\n");
    printf("The Calculated statistics of the 5 iterations of all algorithms\n");
    printf("----------------------------------------------------------------\n");
    printf("\n");
    printf("----------------------------------------------------------------\n");
    printf("        First-come, first-served (FCFS) [non-preemptive]\n");
    printf("----------------------------------------------------------------\n");
    printf("Average Turnaround Time(TAT) :%.1f\n",final[0].avg_turnaround);
    printf("Average Wait Time(WT) : %.1f\n",final[0].avg_wait_time);
    printf("Average Response Time(RT) : %.1f\n",final[0].avg_response_time);
    printf("Average Throughput :%.1f\n",final[0].avg_throughput);
    printf("\n");
    printf("----------------------------------------------------------------\n");
    printf("        Shortest job first (SJF) [non-preemptive]\n");
    printf("----------------------------------------------------------------\n");
    printf("Average Turnaround Time(TAT) :%.1f\n",final[1].avg_turnaround);
    printf("Average Wait Time(WT) : %.1f\n",final[1].avg_wait_time);
    printf("Average Response Time(RT) : %.1f\n",final[1].avg_response_time);
    printf("Average Throughput :%.1f\n",final[1].avg_throughput);
    printf("\n");
    printf("----------------------------------------------------------------\n");
    printf("        Shortest remaining time (SRT) [preemptive]\n");
    printf("----------------------------------------------------------------\n");
    printf("Average Turnaround Time(TAT) :%.1f\n",final[2].avg_turnaround);
    printf("Average Wait Time(WT) : %.1f\n",final[2].avg_wait_time);
    printf("Average Response Time(RT) : %.1f\n",final[2].avg_response_time);
    printf("Average Throughput :%.1f\n",final[2].avg_throughput);
    printf("\n");
    printf("----------------------------------------------------------------\n");
    printf("        Round robin (RR) [preemptive]\n");
    printf("----------------------------------------------------------------\n");
    printf("Average Turnaround Time(TAT) :%.1f\n",final[3].avg_turnaround);
    printf("Average Wait Time(WT) : %.1f\n",final[3].avg_wait_time);
    printf("Average Response Time(RT) : %.1f\n",final[3].avg_response_time);
    printf("Average Throughput :%.1f\n",final[3].avg_throughput);
    printf("\n");
    printf("----------------------------------------------------------------\n");
    printf("        Highest priority first (HPF) [preemptive]\n");
    printf("----------------------------------------------------------------\n");
    printf("Average Turnaround Time(TAT) :%.1f\n",final[4].avg_turnaround);
    printf("Average Wait Time(WT) : %.1f\n",final[4].avg_wait_time);
    printf("Average Response Time(RT) : %.1f\n",final[4].avg_response_time);
    printf("Average Throughput :%.1f\n",final[4].avg_throughput);
    printf("\n");
    printf("----------------------------------------------------------------\n");
    printf("        Highest priority first (HPF) [non-preemptive]\n");
    printf("----------------------------------------------------------------\n");
    printf("Average Turnaround Time(TAT) :%.1f\n",final[5].avg_turnaround);
    printf("Average Wait Time(WT) : %.1f\n",final[5].avg_wait_time);
    printf("Average Response Time(RT) : %.1f\n",final[5].avg_response_time);
    printf("Average Throughput :%.1f\n",final[5].avg_throughput);
    printf("\n");
    printf("----------------------------------------------------------------\n");
}