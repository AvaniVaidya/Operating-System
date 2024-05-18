#include "PAGE.h"

// LRU algorithm
void LRU_FUNCTION(LISTOFPAGES* PLLRU) {
    page* FITLRU = PLLRU->HDL; // Initialize a pointer to the start of the page list
    page* LRU_PGEVVTON = PLLRU->HDL; // Initialize a pointer to the page to be evicted as the first page in the list
    int lru = FITLRU->LONE; // Initialize the LRU timestamp with the timestamp of the first page

    // Loop through the pages to find the one with the least recently used timestamp
    while(FITLRU) {
        if(FITLRU->LONE < lru){
            LRU_PGEVVTON = FITLRU; // Update the page to be evicted
            lru = FITLRU->LONE; // Update the LRU timestamp
        }
        FITLRU = FITLRU->next; // Move to the next page in the list
    }

    // If debugging is enabled, print information about the evicted page
    if (DEBUG == 1) printf("EVICTED ones :: p[%03d] c:%02d l:%02f\n", LRU_PGEVVTON->pid, LRU_PGEVVTON->CNTER, LRU_PGEVVTON->LONE);

    // Mark the evicted page as not in use
    LRU_PGEVVTON->pid = -1;
    LRU_PGEVVTON->PGENUMBER = -1;
}
