#include "PAGE.h"

int main(int Argumnt1, char* Argumnt2[]) {
    int TimeStamp = 0; /// simulator timestamp

    // Paging options
    int *PGCoptn = malloc(sizeof(int) * 4);
    PGCoptn[0] = 5;
    PGCoptn[1] = 11;
    PGCoptn[2] = 17;
    PGCoptn[3] = 31;

    // Hit and Miss counters
    int hits = 0;
    int misses = 0;

    // Pointer to the Page
    page *Page_pointer;

    // Function pointer to store the selected page replacement algorithm
    void (*AlgoFunction)(LISTOFPAGES*);

    // Check command line arguments for the selected algorithm
    if (Argumnt1 != 2) {
        printf("*******Page Replacement Algo are: FCFS, LRU, LFU, MFU, or Random.\n");
        return -1;
    }

    // Checking which function to call based on the algorithm name provided
    if (strcmp(Argumnt2[1], "FCFS") == 0) {
        AlgoFunction = FCFS_FUNCTION;
    } else if (strcmp(Argumnt2[1], "LRU") == 0) {
        AlgoFunction = LRU_FUNCTION;
    } else if (strcmp(Argumnt2[1], "LFU") == 0) {
        AlgoFunction = LFU_FUNCTION;
    } else if (strcmp(Argumnt2[1], "MFU") == 0) {
        AlgoFunction = MFU_FUNCTION;
    } else if (strcmp(Argumnt2[1], "Random") == 0) {
        AlgoFunction = R_FUNCTION;
    } else {
        printf("####################################################################\n");
        printf("These are the options to run for simulation: FCFS, LRU, LFU, MFU, or Random.\n");
        printf("####################################################################\n");
        return -1;
    }

    int swappingInProcess = 0;

    // Seed the random number generator
    srand(0);

    int i;
    for (i = 0; i < 5; i++) {
        printf("------======= Running simulator %d ========-------\n", i + 1);

        LISTOFPAGES pl;
        Intializing_PGELIst(&pl);

        process Q[Total_PROCESS];

        int i;
        for (i = 0; i < Total_PROCESS; i++) {
            // Initialize process attributes
            Q[i].pid = i;
            Q[i].PGECNTER = PGCoptn[rand() % 4];
            Q[i].TIMEARR = rand() % 60;
            Q[i].DRUTION = rand() % PROCss_DuraTN; // Maximum process duration
            Q[i].PGCRR = 0; // All processes begin with page 0
        }

        // Sort the process queue by arrival time
        qsort(Q, Total_PROCESS, sizeof(process), CMP_ARRtime);

        int index = 0; // Index to the start of the process queue

        for (TimeStamp = 0; TimeStamp < TotaL_DURATION; TimeStamp++) {
            // Looking for new processes at the start of every second
            while (index < Total_PROCESS && Q[index].TIMEARR <= TimeStamp) {
                // To check at least four pages
                if (PageWhoRFree(&pl, 4)) {
                    // If it's present, bring it into memory
                    page *p = PAGEfrreeeg(&pl);
                    p->pid = Q[index].pid;
                    p->PGENUMBER = Q[index].PGCRR;
                    p->FTBOUGHT = 1.0 * TimeStamp;
                    p->CNTER = 1;
                    p->LONE = TimeStamp;
                    printf("Page::: %d for process id::: %d brought in at ::: %f\n", Q[index].PGCRR, Q[index].pid, p->FTBOUGHT);
                    swappingInProcess++;
                    index++;
                } else
                    break;
            }
            
            int i;
            for (i = 0; i < 10; i++) {
                int j;
                for (j = 0; j < index; j++) if (Q[j].DRUTION > 0) {
                    Q[j].PGCRR = PGNUMNXT(Q[j].PGCRR, Q[j].PGECNTER);
                    if (MEMinPGES(&pl, Q[j].pid, Q[j].PGCRR)) {
                        Page_pointer = IDwhozPGrFree(&pl, Q[j].pid, Q[j].PGCRR);
                        if (Page_pointer == NULL) {
                            printf("There is a bug, got null: pid %d page:: %d\n", Q[j].pid, Q[j].PGCRR);
                            return -1;
                        }
                        Page_pointer->CNTER++;
                        Page_pointer->LONE = TimeStamp;
                        hits++;
                        continue;
                    }
                    // If we are here, that means we referred a page that is not in memory. So we need to bring it in.
                    page *pageeeg = PAGEfrreeeg(&pl);
                    if (!pageeeg) {
                        printf("####################################################################\n");
                        printf("Memory is full, Page list:\n");
                        PageeDisplayingg(&pl);
                        AlgoFunction(&pl);
                        PageeDisplayingg(&pl);
                        pageeeg = PAGEfrreeeg(&pl);
                    }
                    pageeeg->pid = Q[j].pid;
                    pageeeg->PGENUMBER = Q[j].PGCRR;
                    pageeeg->FTBOUGHT = TimeStamp + (0.1 * i);
                    pageeeg->LONE = TimeStamp + (0.1 * i);
                    pageeeg->CNTER = 0;
                    printf("Page::: %d for process %d brought in at %f\n", Q[j].PGCRR, Q[j].pid, pageeeg->FTBOUGHT);
                    swappingInProcess++;
                    misses++;
                }
            }
            
            int j;
            for (j = 0; j < index; j++) if (Q[j].DRUTION > 0) {
                Q[j].DRUTION--;
                if (Q[j].DRUTION == 0) {
                    printf("####################################################################\n");
                    printf("Process id %d is done. Memory is getting free...\n", Q[j].pid);
                    printf("####################################################################\n");
                    MEMRFree(&pl, Q[j].pid);
                }
            }
            usleep(900); // Sleep for 900 microseconds (0.9 ms)
        }
    }

    // Calculate the hit-miss ratio
    float hitMissRatio = (float) hits / (hits + misses);

    // Output results
    printf("Average number of processes that were successfully swapped in: %d\n", (swappingInProcess / 5));
    printf("Hit Ratio: %.6f\n", hitMissRatio);
}