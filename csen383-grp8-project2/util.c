#include <stdio.h>
#include <stdlib.h>
#include "util.h"
#include "generate_process.h"
#include "stat.h"



linked_list * create_linked_list()
{
	linked_list * new_linkedList =  (linked_list*) malloc(sizeof(linked_list));
	new_linkedList->head = NULL;
	new_linkedList->tail = NULL;
	new_linkedList->size = 0;
	return new_linkedList;
}

node* create_node(void* data)
{
	node* new_node = (node*) malloc(sizeof(node));
	new_node->data = data;
	new_node->next = NULL;
	new_node->prev = NULL;
	return new_node;
}

void add_node(linked_list * lList, void * data)
{
	node * new_node = create_node(data);
	if(lList->size == 0)
	{
		lList->head = new_node;
		lList->tail = new_node;
		lList->size = 1 ;
	} else {
		new_node->prev = lList->tail;
		lList->tail->next = new_node;
		lList->tail = new_node;
		lList->size += 1;
	}
}

void remove_node(linked_list* lList, void * data)
{
	node* current_node = lList->head;

	while(current_node != NULL && current_node->data != data) {
		current_node = current_node->next;
	}

	if(current_node != NULL) {
		if(current_node->prev != NULL) {
			current_node->prev->next = current_node->next;
		}
		if(current_node->next != NULL) {
			current_node->next->prev = current_node->prev;
		}
		if(lList->head == current_node) {
			lList->head = current_node->next;
		}
		if(lList->tail == current_node) {
			lList->tail = current_node->prev;
		}
		lList->size --;
		free(current_node);
	}
}

void remove_head(linked_list* lList)
{
	node * current_node = lList->head;
	if(current_node != NULL) {
		lList->head = current_node->next;
		if(lList->tail == current_node) {
			lList->tail = current_node->prev;
		}
		lList->size --;
		free(current_node);
	}
}

void sort(linked_list *lList, int (*cmp)(void *data1, void *data2)) {
	node *i = lList->head;
	while(i!=NULL) {
		node *j = i->next;
		while(j!=NULL) {
			void * p1 = i->data;
			void * p2 = j->data;
			if((*cmp)(p1,p2) > 0) {
				swap_nodes(i,j);
			}
			j=j->next;
		}
		i = i->next;
	}
}

void swap_nodes(node *a, node *b) {
	void * temp = a->data;
	a->data = b->data;
	b->data = temp;
}

queue * create_queue() {
	return create_linked_list();
}

void enqueue(queue* q, void * data)
{
	node* new_node = create_node(data);	//Creates a new node to add in queue

	new_node->prev = q->tail;
	if(q->tail != NULL) {
		q->tail->next = new_node;
		q->tail = new_node;
	} else {
		q->tail = new_node;
		q->head = new_node;
	}
	q->size += 1;
}

void* dequeue(queue* q)
{
	if(q->head != NULL) {
		node * current_node = q->head;	
		void * data = current_node->data;

		node * next_node = q->head->next;

		if(next_node != NULL) next_node->prev = NULL;
		q->head = next_node; 	

		if(q->tail == current_node) {	
			q->tail = NULL;
		}

		q->size--;
		free(current_node);
		return data;
	}
}

average_stats print_policy_stat(linked_list * lList)
{
	average_stats avg;
	printf("\n");
	printf("-----------------------------------------------------------------------------------------------------------------------------\n");
	printf("Process Name\t| Arrival Time | Start Time | End Time | Run Time | Response Time | Wait Time | Turn Around Time | Priority |\n");
	printf("-----------------------------------------------------------------------------------------------------------------------------\n");
	node * ptr = lList->head;
	float avg_response_time = 0;
	float avg_wait_time = 0;
	float avg_turnaround = 0;
	int process_count = 0;
	while(ptr!=NULL) {
		process_stat *stat = (process_stat *)ptr->data;
		if(stat == NULL) printf("No Stat\n");
		process *proc = (process *)stat->proc;
		if(proc == NULL) printf("No Process\n");
		float process_arrivalTime = proc->process_arrivalTime;
		float service_time = proc->service_time;
		float response_time = stat->start_time - process_arrivalTime;
		float turnaround = stat->end_time - proc->process_arrivalTime + 1;
		float wait_time = turnaround - service_time;
		unsigned char priority = proc->priority;
		avg_response_time += response_time;
		avg_wait_time += wait_time;
		avg_turnaround += turnaround;
		process_count++;
		printf("%16c|%14.1f|%12.1f|%10.1f|%10.1f|%15.1f|%11.1f| %17.1f|%10u|\n",
        proc->pid,process_arrivalTime, stat->start_time, stat->end_time, service_time, response_time, wait_time, turnaround,priority);
		ptr = ptr->next;
	}
	avg.avg_throughput = process_count;
	if(process_count == 0) process_count = 1;
	avg_response_time = avg_response_time / process_count;
	avg_wait_time = avg_wait_time / process_count;
	avg_turnaround = avg_turnaround / process_count;
	printf("-----------------------------------------------------------------------------------------------------------------------------\n");
	printf("%16s|%14.1f|%12.1f|%10.1f|%10.1f|%15.1f|%11.1f| %17.1f|\n"," Average",0.0, 0.0, 0.0,0.0,avg_response_time, avg_wait_time, avg_turnaround);
	printf("-----------------------------------------------------------------------------------------------------------------------------\n");
	avg.avg_response_time = avg_response_time;
	avg.avg_wait_time = avg_wait_time;
	avg.avg_turnaround = avg_turnaround;

	return avg;
}
