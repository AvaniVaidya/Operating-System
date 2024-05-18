#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<time.h>
#include<string.h>
#include<pthread.h>
#include"utility.h"
#define hp_seller_count 1
#define mp_seller_count 3
#define lp_seller_count 6
#define total_seller (hp_seller_count + mp_seller_count + lp_seller_count)
#define concert_row 10
#define concert_col 10
#define simulation_duration 60

// Seller Argument Structure
typedef struct seller_struct {
	char seller_number;
	char seller_type;
	queue *seller_queue;
} seller;

typedef struct customer_struct {
	char cust_no;
	int arrival_time;
} customer;

//Global Variable
int simulation_time;
int N = 5;
char seat_alignment[concert_row][concert_col][5];	//4 to hold L002\0
float response_time_H = 0.0, turn_around_time_H = 0.0, throughput_H = 0.0;
float response_time_M = 0.0, turn_around_time_M = 0.0, throughput_M = 0.0;
float response_time_L = 0.0, turn_around_time_L = 0.0, throughput_L = 0.0;

//Thread Variable
pthread_t seller_thread[total_seller];
pthread_mutex_t thread_mutex_count = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t thread_for_clock_tick_mutex_change_wait = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t reservation_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t thread_mutex_completion = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex_cond_ini = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t  condition_cond_ini  = PTHREAD_COND_INITIALIZER;

//Function Defination
void print_queue(queue *q);
void generate_seller_threads(pthread_t *thread, char seller_type, int no_of_sellers);
void await_for_current_time_slice_completion();
void wakeup_all_seller_threads();
void *sell(void *);
queue * generate_customer_queue(int);
int compare_by_arrival_time(void * data1, void * data2);
int availableSeatFinder(char seller_type);

int thread_count = 0;
int threads_waiting_for_clock_tick = 0;
int active_thread = 0;
int verbose = 0;
int main(int argc, char** argv) {

	if(argc == 2) {
		N = atoi(argv[1]);
	}

	//Initialize Global Variables
	for(int r=0; r<concert_row; r++) {
		for(int c=0; c<concert_col; c++) {
			strncpy(seat_alignment[r][c],"-",1);
		}
	}

	//Create all threads
	generate_seller_threads(seller_thread, 'H', hp_seller_count);
	generate_seller_threads(seller_thread + hp_seller_count, 'M', mp_seller_count);
	generate_seller_threads(seller_thread + hp_seller_count + mp_seller_count, 'L', lp_seller_count);

	//Wait for threads to finish initialization and wait for synchronized clock tick
	while(1) {
		pthread_mutex_lock(&thread_mutex_count);
		if(thread_count == 0) {
			pthread_mutex_unlock(&thread_mutex_count);
			break;
		}
		pthread_mutex_unlock(&thread_mutex_count);
	}

	//Simulate each time quanta/slice as one iteration
	printf("Starting Simulation\n");
	threads_waiting_for_clock_tick = 0;
	wakeup_all_seller_threads(); //For first tick
	
	do {

		//Wake up all thread
		await_for_current_time_slice_completion();
		simulation_time = simulation_time + 1;
		wakeup_all_seller_threads();
		//Wait for thread completion
	} while(simulation_time < simulation_duration);

	//Wakeup all thread so that no more thread keep waiting for clock Tick in limbo
	wakeup_all_seller_threads();

	while(active_thread);

	//Display concert chart
	printf("\n\n");
	printf("Seat Matrix Final\n");
	printf("-------------------------------------------------------\n");

	int h_customers = 0,m_customers = 0,l_customers = 0;
	for(int r=0;r<concert_row;r++) {
		for(int c=0;c<concert_col;c++) {
			if(c!=0)
				printf("\t");
			printf("%5s",seat_alignment[r][c]);
			if(seat_alignment[r][c][0]=='H') h_customers++;
			if(seat_alignment[r][c][0]=='M') m_customers++;
			if(seat_alignment[r][c][0]=='L') l_customers++;
		}
		printf("\n");
	}

	printf("\n\nStatistics for N = %02d\n",N);
	printf("-------------------------------------------------------\n");
	printf("-------------------------------------------------------\n");
	printf("|%3c | Customer Count | No of customers got seat | No of customers returned |\n",' ');
	printf("-------------------------------------------------------\n");
	printf("|%3c | %14d | %24d | %24d |\n",'H',hp_seller_count*N,h_customers,(hp_seller_count*N)-h_customers);
	printf("|%3c | %14d | %24d | %24d |\n",'M',mp_seller_count*N,m_customers,(mp_seller_count*N)-m_customers);
	printf("|%3c | %14d | %24d | %24d |\n",'L',lp_seller_count*N,l_customers,(lp_seller_count*N)-l_customers);
	printf(" ------------------------------------------------------\n");

	printf("\nAverage Response Time per Customer\n");
    printf("H Type Seller: %.2f\n", response_time_H / (hp_seller_count * N));
    printf("M Type Seller: %.2f\n", response_time_M / (mp_seller_count * N));
    printf("L Type Seller: %.2f\n", response_time_L / (lp_seller_count * N));

    printf("\nAverage Turn-around Time per Customer\n");
    printf("H Type Seller: %.2f\n", turn_around_time_H / (hp_seller_count * N));
    printf("M Type Seller: %.2f\n", turn_around_time_M / (mp_seller_count * N));
    printf("L Type Seller: %.2f\n", turn_around_time_L / (lp_seller_count * N));

    printf("\nAverage Throughput per Seller Type\n");
    printf("H Type Seller: %.2f\n", throughput_H / simulation_duration);
    printf("M Type Seller: %.2f\n", throughput_M / simulation_duration);
    printf("L Type Seller: %.2f\n", throughput_L / simulation_duration);

	return 0;
}

void generate_seller_threads(pthread_t *thread, char seller_type, int no_of_sellers){
	//Create all threads
	for(int t_no = 0; t_no < no_of_sellers; t_no++) {
		seller *seller_arg = (seller *) malloc(sizeof(seller));
		seller_arg->seller_number = t_no;
		seller_arg->seller_type = seller_type;
		seller_arg->seller_queue = generate_customer_queue(N);

		pthread_mutex_lock(&thread_mutex_count);
		thread_count++;
		pthread_mutex_unlock(&thread_mutex_count);
		if(verbose)
			printf("Creating thread %c%02d\n",seller_type,t_no);
		pthread_create(thread+t_no, NULL, &sell, seller_arg);
	}
}

void print_queue(queue *q) {
	for(node *ptr = q->head;ptr!=NULL;ptr=ptr->next) {
		customer *cust = (customer * )ptr->data;
		printf("[%d,%d]",cust->cust_no,cust->arrival_time);
	}
}

void await_for_current_time_slice_completion(){
	//Check if all threads has finished their jobs for this time slice
	while(1){
		pthread_mutex_lock(&thread_for_clock_tick_mutex_change_wait);
		if(threads_waiting_for_clock_tick == active_thread) {
			threads_waiting_for_clock_tick = 0;	
			pthread_mutex_unlock(&thread_for_clock_tick_mutex_change_wait);
			break;
		}
		pthread_mutex_unlock(&thread_for_clock_tick_mutex_change_wait);
	}
}
void wakeup_all_seller_threads() {

	pthread_mutex_lock( &mutex_cond_ini );
	if(verbose)
		printf("00:%02d Main Thread Broadcasting Clock Tick\n",simulation_time);
	pthread_cond_broadcast( &condition_cond_ini);
	pthread_mutex_unlock( &mutex_cond_ini );
}

void *sell(void *t_args) {
	//Initializing thread
	seller *args = (seller *) t_args;
	queue * customer_queue = args->seller_queue;
	queue * seller_queue = create_queue();
	char seller_type = args->seller_type;
	int seller_no = args->seller_number + 1;
	
	pthread_mutex_lock(&thread_mutex_count);
	thread_count--;
	active_thread++;
	pthread_mutex_unlock(&thread_mutex_count);

	customer *cust = NULL;
	int random_wait_time = 0;
	
	// Variables to track response time and turn-around time
    int response_time = -1;
    int turn_around_time = -1;

	while(simulation_time < simulation_duration) {
		//Waiting for clock tick
		pthread_mutex_lock(&mutex_cond_ini);
		if(verbose)
			printf("00:%02d %c%02d Waiting for next clock tick\n",simulation_time,seller_type,seller_no);
		
		pthread_mutex_lock(&thread_for_clock_tick_mutex_change_wait);
		threads_waiting_for_clock_tick++;
		pthread_mutex_unlock(&thread_for_clock_tick_mutex_change_wait);
		
		pthread_cond_wait( &condition_cond_ini, &mutex_cond_ini);
		if(verbose)
			printf("00:%02d %c%02d Received Clock Tick\n",simulation_time,seller_type,seller_no);
		pthread_mutex_unlock( &mutex_cond_ini );

		// Sell
		if(simulation_time == simulation_duration) break;
		//All New Customer Came
		while(customer_queue->size > 0 && ((customer *)customer_queue->head->data)->arrival_time <= simulation_time) {
			customer *temp = (customer *) dequeue (customer_queue);
			enqueue(seller_queue,temp);
			printf("00:%02d %c%d Customer No %c%d%02d arrived\n",simulation_time,seller_type,seller_no,seller_type,seller_no,temp->cust_no);
		}
		//Serve next customer
		if(cust == NULL && seller_queue->size>0) {
			cust = (customer *) dequeue(seller_queue);
			printf("00:%02d %c%d Serving Customer No %c%d%02d\n",simulation_time,seller_type,seller_no,seller_type,seller_no,cust->cust_no);
			switch(seller_type) {
				case 'H':
				random_wait_time = (rand()%2) + 1;
				break;
				case 'M':
				random_wait_time = (rand()%3) + 2;
				break;
				case 'L':
				random_wait_time = (rand()%4) + 4;
			}
		}
		if(cust != NULL) {
			//printf("Wait time %d\n",random_wait_time);
			if(random_wait_time == 0) {
				// Update turn-around time
                turn_around_time = simulation_time - cust->arrival_time;
				
				//Selling Seat
				pthread_mutex_lock(&reservation_mutex);

				// Find seat
				int seatIndex = availableSeatFinder(seller_type);
				if(seatIndex == -1) {
					printf("00:%02d %c%d Customer No %c%d%02d has been told Concert Sold Out.\n",simulation_time,seller_type,seller_no,seller_type,seller_no,cust->cust_no);
				} else {
					int row_no = seatIndex/concert_col;
					int col_no = seatIndex%concert_col;
					sprintf(seat_alignment[row_no][col_no],"%c%d%02d",seller_type,seller_no,cust->cust_no);
					printf("00:%02d %c%d Customer No %c%d%02d assigned seat %d,%d \n",simulation_time,seller_type,seller_no,seller_type,seller_no,cust->cust_no,row_no,col_no);
				}
				pthread_mutex_unlock(&reservation_mutex);
				cust = NULL;
			} else {
				random_wait_time--;
			}
		} else {
			//printf("00:%02d %c%02d Waiting for customer\n",sim_time,seller_type,seller_no);
		}
	}

	// Calculate response time if not already calculated
    if (response_time == -1 && seller_queue->size > 0) {
        response_time = simulation_time - ((customer *) seller_queue->head->data)->arrival_time;
    }

	// Update statistics based on seller type
    switch (seller_type) {
        case 'H':
            response_time_H += response_time;
            turn_around_time_H += turn_around_time;
            throughput_H += (float) (hp_seller_count * N);
            break;
        case 'M':
            response_time_M += response_time;
            turn_around_time_M += turn_around_time;
            throughput_M += (float) (mp_seller_count * N);
            break;
        case 'L':
            response_time_L += response_time;
            turn_around_time_L += turn_around_time;
            throughput_L += (float) (lp_seller_count * N);
            break;
    }

	while(cust!=NULL || seller_queue->size > 0) {
		if(cust==NULL)
			cust = (customer *) dequeue(seller_queue);
		printf("00:%02d %c%d Ticket Sale Closed. Customer No %c%d%02d Leaves\n",simulation_time,seller_type,seller_no,seller_type,seller_no,cust->cust_no);
		cust = NULL;
	}

	pthread_mutex_lock(&thread_mutex_count);
	active_thread--;
	pthread_mutex_unlock(&thread_mutex_count);
}

int availableSeatFinder(char seller_type){
	int seatIndex = -1;

	if(seller_type == 'H') {
		for(int row_no = 0;row_no < concert_row; row_no ++ ){
			for(int col_no = 0;col_no < concert_col; col_no ++) {
				if(strcmp(seat_alignment[row_no][col_no],"-") == 0) {
					seatIndex = row_no * concert_col + col_no;
					return seatIndex;
				}
			}
		}
	} else if(seller_type == 'M') {
		int mid = concert_row / 2;
		int row_jump = 0;
		int next_row_no = mid;
		for(row_jump = 0;;row_jump++) {
			int row_no = mid+row_jump;
			if(mid + row_jump < concert_row) {
				for(int col_no = 0;col_no < concert_col; col_no ++) {
					if(strcmp(seat_alignment[row_no][col_no],"-") == 0) {
						seatIndex = row_no * concert_col + col_no;
						return seatIndex;
					}
				}
			}
			row_no = mid - row_jump;
			if(mid - row_jump >= 0) {
				for(int col_no = 0;col_no < concert_col; col_no ++) {
					if(strcmp(seat_alignment[row_no][col_no],"-") == 0) {
						seatIndex = row_no * concert_col + col_no;
						return seatIndex;
					}
				}
			}
			if(mid + row_jump >= concert_row && mid - row_jump < 0) {
				break;
			}
		}
	} else if(seller_type == 'L') {
		for(int row_no = concert_row - 1;row_no >= 0; row_no -- ){
			for(int col_no = concert_col - 1;col_no >= 0; col_no --) {
				if(strcmp(seat_alignment[row_no][col_no],"-") == 0) {
					seatIndex = row_no * concert_col + col_no;
					return seatIndex;
				}
			}
		}
	}

	return -1;
}

queue * generate_customer_queue(int N){
	queue * customer_queue = create_queue();
	char cust_no = 0;
	while(N--) {
		customer *cust = (customer *) malloc(sizeof(customer));
		cust->cust_no = cust_no;
		cust->arrival_time = rand() % simulation_duration;
		enqueue(customer_queue,cust);
		cust_no++;
	}
	sort(customer_queue, compare_by_arrival_time);
	node * ptr = customer_queue->head;
	cust_no = 0;
	while(ptr!=NULL) {
		cust_no ++;
		customer *cust = (customer *) ptr->data;
		cust->cust_no = cust_no;
		ptr = ptr->next;
	}
	return customer_queue;
}

int compare_by_arrival_time(void * data1, void * data2) {
	customer *c1 = (customer *)data1;
	customer *c2 = (customer *)data2;
	if(c1->arrival_time < c2->arrival_time) {
		return -1;
	} else if(c1->arrival_time == c2->arrival_time){
		return 0;
	} else {
		return 1;
	}
}
