#include "PAGE.h"

// LFU algorithm
void LFU_FUNCTION(LISTOFPAGES* pLFUl) {
    page* FITLFU = pLFUl->HDL; // Initialize a pointer to the start of the page list
    page* PG_LFUEVICTN = pLFUl->HDL; // Initialize a pointer to the page to be evicted as the first page in the list
    int min = FITLFU->CNTER; // Initialize the minimum reference count with the count of the first page

    // Loop through the pages to find the one with the least frequent usage
    while(FITLFU) {
        if(FITLFU->CNTER < min){
            PG_LFUEVICTN = FITLFU; // Update the page to be evicted
            min = FITLFU->CNTER; // Update the minimum reference count
        }
        FITLFU = FITLFU->next; // Move to the next page in the list
    }

    // If debugging is enabled, print information about the evicted page
    if (DEBUG == 1) printf("EVICTED ones :: p[%03d] c:%02d l:%02f\n", PG_LFUEVICTN->pid, PG_LFUEVICTN->CNTER, PG_LFUEVICTN->LONE);

    // Mark the evicted page as not in use
    PG_LFUEVICTN->pid = -1;
    PG_LFUEVICTN->PGENUMBER = -1;
}
