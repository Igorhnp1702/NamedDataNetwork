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

 #include <stdio.h>
 #include <string.h>

 #define MAX_ENTRIES 10
 #define MAX_NAME_LENGTH 101
 #define MAX_INTERFACES 10


// Interfaces' States
typedef enum {
    ANSWER,   
    WAITING,  
    CLOSED
} InterfaceState;

// Structure of an entry in the interest table
typedef struct {
    char name[MAX_NAME_LENGTH];                // Object name
    InterfaceState interfaces[MAX_INTERFACES]; // Interfaces' states
    int active;                                // 1 = Active, 0 = Inactive
} InterestEntry;

// Structure of the interest table
typedef struct {
    InterestEntry entries[MAX_ENTRIES];
} InterestTable;




void init_interest_table(InterestTable *table);
int add_interest(InterestTable *table, char *name, InterfaceState initial_state);
int remove_interest(InterestTable *table, char *name);
void update_interface_state(InterestTable *table, char *name, int interface_index, InterfaceState state);
void clear_interest_table(InterestTable **table);
int all_interfaces_closed(InterestTable *table, char *name);
int search_interest(InterestTable *table, char *name);
int search_waiting_interface(InterestTable *table, char *name);
InterfaceState search_interest_interface_state(InterestTable *table, char *name, int interface_index);
void show_interest_table(InterestTable *table);



 #endif