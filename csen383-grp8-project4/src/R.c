#include "PAGE.h"

// Random page replacement algorithm
void R_FUNCTION(LISTOFPAGES* PRM) {
    page* FITR = PRM->HDL; // Initialize a pointer to the first page in the page list
    page* PGRNDMEVC = PRM->HDL; // Initialize a pointer to track the page for eviction
    int CNTER = 0; // Counter to keep track of the current page number
    int r = rand() % Toal_PAGE; // Generate a random number 'r' within the range of total pages

    // Loop through the pages in the list
    while (FITR) {
        if (FITR->pid > 0 && CNTER < r) { // Check if the page is in use and if the counter is less than the random number 'r'
            PGRNDMEVC = FITR; // Update the page to be evicted
        }
        FITR = FITR->next; // Move to the next page
        CNTER++; // Increment the counter
    }

    if (DEBUG == 1) {
        printf("EVICTED ones :: p[%03d] c:%02d l:%02f\n", PGRNDMEVC->pid, PGRNDMEVC->CNTER, PGRNDMEVC->LONE);
    }

    PGRNDMEVC->pid = -1; // Mark the page as not in use
    PGRNDMEVC->PGENUMBER = -1; // Reset the page number
}
