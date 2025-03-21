/************************************************************************************************
 * ndn_interestTable.c
 *
 * Authors:
 *
 * -Igor Paruque ist1102901
 * -Mónica Ferreira ist106520
 *
 * Description: source code for the interest Table
 ***********************************************************************************************/

#include "ndn_interestTable.h"


// Initialize the interest table
void init_interest_table(InterestTable *table) {
    for (int i = 0; i < MAX_ENTRIES; i++) {
        table->entries[i].active = 0;                       // There are no active entries
        memset(table->entries[i].name, 0, MAX_NAME_LENGTH);
        for (int j = 0; j < MAX_INTERFACES; j++) {
            table->entries[i].interfaces[j] = CLOSED;       // All interfaces are closed
        }
    }
}

// Add an interest to the table
int add_interest(InterestTable *table, char *name, InterfaceState initial_state) {
    for (int i = 0; i < MAX_ENTRIES; i++) {
        if (!table->entries[i].active) {                                // If the entry is not active
            strncpy(table->entries[i].name, name, MAX_NAME_LENGTH - 1); // Add Object name
            for (int j = 0; j < MAX_INTERFACES; j++) {
                table->entries[i].interfaces[j] = initial_state;        // Define the initial state for all interfaces
            }
            table->entries[i].active = 1;
            return 0;
        }
    }
    return -1; // Interest table is full
}

// Delete an interest from the table
int remove_interest(InterestTable *table, char *name) {
    for (int i = 0; i < MAX_ENTRIES; i++) {
        // If the entry is active and the object name matches,
        // we found the interest to remove
        if (table->entries[i].active && strcmp(table->entries[i].name, name) == 0) {
            table->entries[i].active = 0;   // Set the entry as inactive
            memset(table->entries[i].name, 0, MAX_NAME_LENGTH); // Clear the object name
            for (int j = 0; j < MAX_INTERFACES; j++) {
                table->entries[i].interfaces[j] = CLOSED; // Close all interfaces
            }
            return 0;
        }
    }
    return -1; // Interest not found
}

// Update the state of an interface for a given interest
void update_interface_state(InterestTable *table, char *name, int interface_index, InterfaceState state) {
    for (int i = 0; i < MAX_ENTRIES; i++) {
        // Find the interest entry with the given object name
        if (table->entries[i].active && strcmp(table->entries[i].name, name) == 0) {
            if (interface_index >= 0 && interface_index < MAX_INTERFACES) {
                table->entries[i].interfaces[interface_index] = state;
            }
            return;
        }
    }
}

// Delete all entries
void clear_interest_table(InterestTable *table) {
    for (int i = 0; i < MAX_ENTRIES; i++) {
        table->entries[i].active = 0;  // Set the entry as inactive
        memset(table->entries[i].name, 0, MAX_NAME_LENGTH);
        for (int j = 0; j < MAX_INTERFACES; j++) {
            table->entries[i].interfaces[j] = CLOSED;  // Close all interfaces
        }
    }
}

// Check if all interfaces are closed
int all_interfaces_closed(InterestTable *table, char *name) {
    
    if (table == NULL || name == NULL) {
        return 1; // Return all interfaces are closed if the table or name is NULL
    }
    

    for (int i = 0; i < MAX_ENTRIES; i++) {
        // Find the interest entry with the given object name
        if (table->entries[i].active && strcmp(table->entries[i].name, name) == 0) {
            for (int j = 0; j < MAX_INTERFACES; j++) {
                if (table->entries[i].interfaces[j] != CLOSED) {
                    return 0; // At least one interface is not closed
                }
            }
            return 1; // All interfaces are closed
        }
    }

    return 1; // Consider all interfaces closed if the interest is not found
}

// Search for an interest in the table
int search_interest(InterestTable *table, char *name) {
    for (int i = 0; i < MAX_ENTRIES; i++) {
        if (table->entries[i].active && strcmp(table->entries[i].name, name) == 0) {
            return 1; // Interest found
        }
    }
    return 0; // Interest not found
}

// Search for the interface that is waiting for the object
int search_waiting_interface(InterestTable *table, char *name) {
    for (int i = 0; i < MAX_ENTRIES; i++) {
        // Find the interest entry with the given object name
        if (table->entries[i].active && strcmp(table->entries[i].name, name) == 0) {
            for (int j = 0; j < MAX_INTERFACES; j++) {
                if (table->entries[i].interfaces[j] == WAITING) {
                    return j; // Return the index of the waiting interface
                }
            }
        }
    }
    return -1;
}

InterfaceState search_interest_interface_state(InterestTable *table, char *name, int interface_index){
    
    if (table == NULL || name == NULL) {
        return CLOSED; // Return closed if the table or name is NULL
    }
    
    for (int i = 0; i < MAX_ENTRIES; i++) {
        // Find the interest entry with the given object name
        if (table->entries[i].active && strcmp(table->entries[i].name, name) == 0) {
            if (interface_index >= 0 && interface_index < MAX_INTERFACES) {
                return table->entries[i].interfaces[interface_index]; // Return the state of the interface
            }
            else {
                return CLOSED; // Return closed if the interface index is invalid
            }
        }
    }
    return CLOSED; // Return closed if the interest is not found
}

// Show all entries in the interest table
void show_interest_table(const InterestTable *table) {
    printf("Interest Table:\n");
    for (int i = 0; i < MAX_ENTRIES; i++) {
        if (table->entries[i].active) {
            printf("Entry %d:\n", i + 1);
            printf("  Object: %s\n", table->entries[i].name);
            printf("  Interfaces: ");
            for (int j = 0; j < MAX_INTERFACES; j++) {
                printf("%d ", table->entries[i].interfaces[j]);
            }
            printf("\n");
        }
    }
}