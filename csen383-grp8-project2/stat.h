#ifndef _stat_h_
#define _stat_h_
#include "generate_process.h"
#include <stdlib.h>
/*
UNUSED: unused process
EMBRYO: a new process is currently being created
SLEEPING: process blocked for I/O
RUNNING: currently executing process
ZOMBIE: a process whose execution is completed but it still has an entry in the process table
*/
enum State {UNUSED, EMBRYO, SLEEPING, RUNNING, ZOMBIE};

struct process_stat_s {
    process * proc;
    float wait_time;
    float turnaround_time;
    float response_time;

    float start_time;
    float end_time;
    float service_time;

    enum State state;
};
typedef struct process_stat_s process_stat;

process_stat * create_process_stat(process* proc);

struct stat_s {
    linked_list * process_queue;
    linked_list * time_chart;
    float avg_turnaround_time;
    float avg_waiting_time;
    float avg_response_time;
    float throughput;
    int total_quanta;
};
typedef struct stat_s scheduling_stat;

struct average_stats_s{
    float avg_response_time;
    float avg_wait_time;
    float avg_turnaround;
    float avg_throughput;
};

typedef struct average_stats_s average_stats;

average_stats first_come_first_serve(linked_list * processes);
average_stats shortest_job_first_np(linked_list * processes);
average_stats shortest_remainingTime_pr(linked_list * processes);
average_stats round_robin(linked_list * processes,int time_slice);
average_stats highest_priority_first_pr(linked_list * processes);
average_stats highest_priority_first_np(linked_list * processes);

average_stats print_policy_stat(linked_list * ll);
#endif
