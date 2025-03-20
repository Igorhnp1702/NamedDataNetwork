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
 #define MAX_NAME_LENGTH 100
 #define MAX_INTERFACES 10


// Interfaces' States
typedef enum {
    ANSWER,   
    WAITING,  
    CLOSED
} InterfaceState;

// Structure of an entry in the interest table
typedef struct {
    char name[MAX_NAME_LENGTH];          // Object name
    InterfaceState interfaces[MAX_INTERFACES]; // Interfaces' states
} InterestEntry;

// Structure of the interest table
typedef struct {
    InterestEntry entries[MAX_ENTRIES];
} InterestTable;






 #endif