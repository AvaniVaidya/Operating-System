#include "stat.h"

process_stat * create_process_stat(process* proc) {
	process_stat * ps = (process_stat *) malloc(sizeof(process_stat));
	ps->proc = proc;
	ps->wait_time = 0;
	ps->turnaround_time = 0;
	ps->response_time = 0;

	ps->service_time = 0;
	ps->start_time = -1;
	ps->end_time = -1;
	return ps;
}