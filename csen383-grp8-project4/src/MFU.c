#include "PAGE.h"

// MFU algorithm
void MFU_FUNCTION(LISTOFPAGES* PMFUL) {
    page* FITMFU = PMFUL->HDL; // Initialize a pointer to the start of the page list
    page* PGMFUEVC = PMFUL->HDL; // Initialize a pointer to the page to be evicted as the first page in the list
    int max = FITMFU->CNTER; // Initialize the maximum count with the count of the first page

    // Loop through the pages to find the most frequently used page
    while(FITMFU) {
        if(FITMFU->CNTER > max){
            PGMFUEVC = FITMFU; // Update the page to be evicted
            max = FITMFU->CNTER; // Update the maximum count
        }
        FITMFU = FITMFU->next; // Move to the next page in the list
    }

    // If debugging is enabled, print information about the evicted page
    if (DEBUG == 1) printf("EVICTED: p[%03d] c:%02d l:%02f\n", PGMFUEVC->pid, PGMFUEVC->CNTER, PGMFUEVC->LONE);

    // Mark the evicted page as not in use
    PGMFUEVC->pid = -1;
    PGMFUEVC->PGENUMBER = -1;
}
