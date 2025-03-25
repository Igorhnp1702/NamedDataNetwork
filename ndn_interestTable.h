/************************************************************************************************
 * ndn_interestTable.h
 *
 * Authors:
 *
 * -Igor Paruque ist1102901
 * -Mónica Ferreira ist106520
 *
 * Description: header file for the interest table
 ***********************************************************************************************/

 #ifndef ndn_interestTable_header
 #define ndn_interestTable_header

 #define ANSWER_STR "ANSWER"
 #define WAITING_STR "WAITING"
 #define CLOSED_STR "CLOSED"

 

// Interfaces' States
typedef enum State{    
    ANSWER,   
    WAITING,  
    CLOSED
} InterfaceState;

// Structure of a node in the interest table list
typedef struct InterestEntry{
    char *name;                // Object name
    char *interface_addr;
    char *interface_tcp;
    char *state_str;
    int interface_fd;
    InterfaceState current_state;
    struct InterestEntry *next;
} InterestEntry;

#include "ndn_node.h"

InterestEntry *init_interest_table(InterestEntry *head); // Initialize the list with the interests

InterestEntry *add_interest(InterestEntry *head, nodeinfo_t *src_node, char *name, InterfaceState initial_state); // add an interest to the interest table

InterestEntry *RemoveSingleInterest(InterestEntry *head, nodeinfo_t *target_node, char *name2del, InterfaceState target_state);

InterestEntry *remove_interests(InterestEntry *table, char *name2del); // remove all interests from the interest table related to 'name2del'

void free_interest(InterestEntry **interest_block); // free the memory allocated for the interest

InterestEntry *update_interface_state(InterestEntry *head, int src_fd ,char *name, InterfaceState new_state); // update the state of an interest

InterestEntry *clear_interest_table(InterestEntry *table); // clear the interest table from memory

void show_interest_table(InterestEntry *head); // Display the interest table on screen

int search_interest(InterestEntry *head, nodeinfo_t *target_node, char *name, InterfaceState target_state); // search the interest in the interest table

#endif