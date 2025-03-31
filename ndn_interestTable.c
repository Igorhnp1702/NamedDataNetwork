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

// general purpose libraries
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>

// project libraries
#include "ndn_interestTable.h"
#include "ndn_messages.h"
#include "ndn_node.h"

// Initialize the interest table
InterestEntry *init_interest_table(InterestEntry *head) {   
    
    head = NULL;
    return head;
    
}

// Add an interest to the table
InterestEntry *add_interest(InterestEntry *head, nodeinfo_t *src_node, char *name, InterfaceState initial_state, int retrieve_flag) {
    
    InterestEntry *new_entry = NULL, *aux;    
    

    if((new_entry = (InterestEntry*)calloc(1, sizeof(InterestEntry))) == NULL){

        printf("Error in add_interest: Failed to allocate memory. Process terminated");
        exit(1);
    }
    new_entry->current_state = initial_state;
    new_entry->state_str = NULL;
    if((new_entry->state_str = (char*)calloc(MAX_STATE_CHARS, sizeof(char))) == NULL){

        printf("Error in add_interest: Failed to allocate memory. Process terminated");
        exit(1);
    }

    if(initial_state == ANSWER)       strcpy(new_entry->state_str, "ANSWER");
    else if(initial_state == WAITING) strcpy(new_entry->state_str, "WAITING");
    else if(initial_state == CLOSED)  strcpy(new_entry->state_str, "CLOSED");
    
    else{

        printf("Invalid interface state. The interest was not added");
        free(new_entry->state_str);
        free(new_entry);
        return head;
    }
    
    new_entry->interface_addr = NULL;
    if((new_entry->interface_addr = (char*)calloc(MAX_ADDRESS_SIZE, sizeof(char))) == NULL){

        printf("Error in add_interest: Failed to allocate memory. Process terminated");
        exit(1);
    }

    new_entry->interface_tcp = NULL;
    if((new_entry->interface_tcp = (char*)calloc(MAX_TCP_UDP_CHARS, sizeof(char))) == NULL){

        printf("Error in add_interest: Failed to allocate memory. Process terminated");
        exit(1);
    }

    new_entry->name = NULL;
    if((new_entry->name = (char*)calloc(MAX_NAME_LENGTH, sizeof(char))) == NULL){

        printf("Error in add_interest: Failed to allocate memory. Process terminated");
        exit(1);
    }
    strcpy(new_entry->name, name);

    new_entry->next = NULL;
    new_entry->interface_fd = src_node->node_fd;             
    strcpy(new_entry->interface_addr, src_node->node_addr);    
    strcpy(new_entry->interface_tcp, src_node->tcp_port);
    
    if(head == NULL){

        head = new_entry;
        if(retrieve_flag == 0){

            printf("New entry in the interest table:\n\n");        
            printf("Interface: [%s | %s]\n", new_entry->interface_addr, new_entry->interface_tcp);
            printf("Object in question: %s\n", name);
            printf("State: %s\n\n", new_entry->state_str);
        }
        
        return head;
    }
    
    // insert interest entry at the end of the list

    aux = head;

    while (aux->next != NULL) aux = aux->next;
    
    aux->next = new_entry;

    if(retrieve_flag == 0){

        printf("New entry in the interest table:\n\n");    
        printf("Interface: [%s | %s]\n", new_entry->interface_addr, new_entry->interface_tcp);
        printf("Object in question: %s\n", name);
        printf("Current state: %s\n\n", new_entry->state_str);
    }
    
    return head;    
}

void free_interest(InterestEntry **interest_block){

    free((*interest_block)->interface_addr);
    free((*interest_block)->interface_tcp);
    free((*interest_block)->name);
    free((*interest_block)->state_str);
    free((*interest_block));

    
}

InterestEntry *RemoveSingleInterest(InterestEntry *head, nodeinfo_t *target_node, char *name2del, InterfaceState target_state, int retrieve_flag){

    if(head == NULL){

        //printf("Error in RemoveSingleInterest: There are no interests to remove\n");
        return head;
    }

    InterestEntry *aux, *aux2del;
    int counter = 4; // number of matches for the head

    if(strcmp(head->interface_addr, target_node->node_addr) != 0){

        //head has different address
        counter --;
    }
    if(strcmp(head->interface_tcp, target_node->tcp_port) != 0){

        //head has different port
        counter --;
    }            
    
    if(strcmp(head->name, name2del) != 0){

        //head has different name
        counter --;
    }
    if(head->current_state != target_state){

        //head has different state
        counter --;
    }

    if(counter == 4){
        
        // the head will be removed and NULL will be returned

        if(retrieve_flag == 0){

            printf("The following interest entry will be removed:\n\n");
            printf("Interface: [%s | %s]\n", head->interface_addr, head->interface_tcp);
            printf("Object in question: %s\n", head->name);
            printf("Current state: %s\n\n", head->state_str);
        }
       
        aux2del = head;
        head = head->next;
        free_interest(&aux2del);
        return head;
    }

    aux = head;

    if(aux == NULL) return head;

    while(aux->next != NULL){
        
        
        if(strcmp(aux->next->interface_addr, target_node->node_addr) != 0){

            //different address, see next entry
                        
            aux = aux->next;
            continue;
        }
        if(strcmp(aux->next->interface_tcp, target_node->tcp_port) != 0){

            //different port, see next entry
            
            
            aux = aux->next;
            continue;
        }            
        
        if(strcmp(aux->next->name, name2del) != 0){

            //different name, see next entry
            
            
            aux = aux->next;
            continue;
        }
        if(aux->next->current_state != target_state){

            //different state, see next entry
            
            
            aux = aux->next;
            continue;
        }
        if(retrieve_flag == 0){

            printf("The following interest entry will be removed:\n\n");
            printf("Interface: [%s | %s]\n", aux->next->interface_addr, aux->next->interface_tcp);
            printf("Object in question: %s\n", aux->next->name);
            printf("Current state: %s\n\n", aux->next->state_str);
        }
       
        aux2del = aux->next;
        aux->next = aux->next->next;                
        free_interest(&aux2del);
        return head;
        
    }
    
    //printf("Error in RemoveSingleInterest: Failed to remove desired interest entry\n");
    return head;
}

// Delete all interests associated with 'name2del'
 InterestEntry *remove_interests(InterestEntry *head, char *name2del, int retrieve_flag) {
    
    // check if the list is empty

    if(head == NULL){

        //printf("Error in remove interest: There are no interests to remove\n");
        return head;
    }
    InterestEntry *aux, *aux2del;

    // To remove first

    while(strcmp(head->name, name2del) == 0){

        if(retrieve_flag == 0){

            printf("The following interest entry will be removed:\n\n");
            printf("Interface: [%s | %s]\n", head->interface_addr, head->interface_tcp);
            printf("Object in question: %s\n", head->name);
            printf("Current state: %s\n\n", head->state_str);
        }
        
        aux2del = head;
        head = head->next;
        free_interest(&aux2del);
        if(head == NULL)break;

    }    
        
    aux = head;
    
    if(aux == NULL) return head;

    while(aux->next != NULL){

        if(strcmp(aux->next->name, name2del) == 0){

            if(retrieve_flag == 0){

                printf("The following interest entry will be removed:\n\n");
                printf("Interface: [%s | %s]\n", aux->next->interface_addr, aux->next->interface_tcp);
                printf("Object in question: %s\n", aux->next->name);
                printf("Current state: %s\n\n", aux->next->state_str);
            }
            
            aux2del = aux->next;
            aux->next = aux->next->next;
            free_interest(&aux2del);

        }else aux = aux->next;

    }
    return head;
}

// Update the state of an interface for a given interest
InterestEntry *update_interface_state(InterestEntry *head, int src_fd ,char *name, InterfaceState new_state) {
    
    if((new_state != ANSWER) && (new_state != CLOSED) && (new_state != WAITING)){
       
        printf("Error in update_interface_state: Invalid state was passed.\n");
        printf("Update canceled\n\n");
        return head;
    }
    
    // check if the list is empty

    if(head == NULL){

        printf("Error in update_interface_state: There are no interfaces to update\n");
        return head;
    }

    InterestEntry *aux = head;

    while(aux != NULL){

        if(aux->interface_fd != src_fd){ 

            // aux does not match the interface I want, continue searching
            aux = aux->next;
            continue;
        }

        if(strcmp(aux->name, name) != 0){

            // aux has the interface but not the name I want, continue
            aux = aux->next;
            continue;
        }

        // Has the interface and the name, hopefully not duplicated

        printf("Change in the interest table\n\n");
        printf("Previous state of interface [%s | %s] for object '%s': %s\n\n", aux->interface_addr, aux->interface_tcp, name, aux->state_str);

        aux->current_state = new_state;

        if(new_state == ANSWER)       strcpy(aux->state_str, "ANSWER");
        else if(new_state == WAITING) strcpy(aux->state_str, "WAITING");
        else if(new_state == CLOSED)  strcpy(aux->state_str, "CLOSED");
                
        printf("Updated state of interface [%s | %s] for object '%s': %s\n\n", aux->interface_addr, aux->interface_tcp, name, aux->state_str);
        
        return head;
        
    }

    printf("Error in update_interface_state: target interface was not found\n");
    return head;
}

// Delete all entries
InterestEntry *clear_interest_table(InterestEntry *head) {
    
    if(head == NULL){
        
        return head;
    }

    InterestEntry *aux = head, *aux2del;

    while(aux != NULL){

        aux2del = aux;
        aux = aux->next;
        free_interest(&aux2del);
    }
    head = NULL;
    return head;
}

// Show all entries in the interest table
void show_interest_table(InterestEntry *head){
    
    if(head == NULL){

        printf("No interests to show");
        return;
    }

    printf("The interest table goes as follows:\n\n");

    InterestEntry *aux = head;

    while(aux != NULL){

        printf("Interface: [%s | %s]\n", aux->interface_addr, aux->interface_tcp);
        printf("Object in question: %s\n", aux->name);
        printf("Current state: %s\n\n", aux->state_str);
        aux = aux->next;
    }
    return;

}

// To check if all interfaces are closed for object 'name', try to find one interface with 'WAITING' as
// the current state. 
// If you find one, not all interfaces are closed.
// If you don't find one, they are either closed or in ANSWER state, in which case you send NOOBJECT and delete 
// the entries from the table

// Search for an interest in the table to see if we are duplicating or not
int search_interest(InterestEntry *head, nodeinfo_t *target_node, char *name, InterfaceState target_state) {
    
    // by passing NULL to the target_node, we only search entries with 'name' and 'target_state'
    if(head == NULL){

        //no interest to search, the table is empty
        return 0;        
    }
    InterestEntry *aux = head;

    while(aux != NULL){
        
        if(target_node != NULL){
            if(strcmp(aux->interface_addr, target_node->node_addr) != 0){

                //different address, see next entry
                aux = aux->next;
                continue;
            }
            if(strcmp(aux->interface_tcp, target_node->tcp_port) != 0){

                //different port, see next entry
                aux = aux->next;
                continue;
            }            
        }
        if(strcmp(aux->name, name) != 0){

            //different name, see next entry
            aux = aux->next;
            continue;
        }
        if(aux->current_state != target_state){

            //different state, see next entry
            aux = aux->next;
            continue;
        }
        // interest found, return 1
        return 1;
    }
    return 0;
}





