#include "PAGE.h"

// FCFS algorithm
void FCFS_FUNCTION(LISTOFPAGES* plFCFS) {
    page* PITFCF = plFCFS->HDL; // Initialize a pointer to the start of the page list
    page* PG_FCFFSEVICtion = plFCFS->HDL; // Initialize a pointer to the page to be evicted as the first page in the list

    // Loop through the pages to find the one with the earliest arrival time
    while(PITFCF) {
        if(PITFCF->FTBOUGHT < PG_FCFFSEVICtion->FTBOUGHT) {
            PG_FCFFSEVICtion = PITFCF;
        }
        PITFCF = PITFCF->next; // Move to the next page in the list
    }

    // If debugging is enabled, print information about the evicted page
    if (DEBUG == 1) printf("EVICTED ones :: p[%03d] c:%02d l:%02f\n", PG_FCFFSEVICtion->pid, PG_FCFFSEVICtion->CNTER, PG_FCFFSEVICtion->LONE);

    // Mark the evicted page as not in use
    PG_FCFFSEVICtion->pid = -1;
    PG_FCFFSEVICtion->PGENUMBER = -1;
}
