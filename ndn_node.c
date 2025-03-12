/************************************************************************************************
 * ndn_netfuncs.c
 *
 * Authors:
 *
 * -Igor Paruque ist1102901
 * -Mónica Ferreira ist106520
 *
 * Description: source code for node management
 ***********************************************************************************************/

// general purpose libraries
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <sys/types.h>
#include <errno.h>

// networking libraries
#include <netdb.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <signal.h>

// project libraries
#include "ndn_commands.h"
#include "ndn_node.h"
//#include "ndn_queue.h"



nodeinfo_t *contact_init(nodeinfo_t *contact){

    contact = (nodeinfo_t*)calloc(1, sizeof(nodeinfo_t)); // the block itself

    /* initializing positive integer variables to -1 */
          
    contact->network = (char*)calloc(MAX_NET_CHARS, sizeof(char));      // integer from 000 to 999
    contact->tcp_port = (char*)calloc(MAX_TCP_UDP_CHARS, sizeof(char));     // tcp port; integer from 0 to 65 536
    contact->node_addr = (char*)calloc(MAX_ADDRESS_SIZE, sizeof(char));   // IPv4 address
    contact->node_buff = (char*)calloc(MAX_MSG_LENGTH, sizeof(char));
    contact->node_fd = -1;
    //!ponderar inicializar tudo com NULL em vez de 0
    return contact;
}//contact_init()


struct personal_node *personal_init(struct personal_node *personal){

    int iter = 0;

    personal = (struct personal_node*)calloc(1, sizeof(struct personal_node));

    personal->persn_info = contact_init(personal->persn_info);

    personal->anchorflag = -1;   // flag that says whether the node is an anchor or not
    personal->n_internals = 0;    // counter for the number of internal neighbors    
    personal->max_fd = 0;        // the maximum integer assigned to a file descriptor
    personal->client_fd = -1;   // file descriptor for a client node  
    personal->udp_port = NULL;   //UDP server port
    personal->udp_address = NULL;    //UDP server address
    personal->extern_node = NULL;   //extern neighbor node
    personal->backup_node = NULL;   //backup neighbor node
   
    /* set the start of the list to NULL */

    //personal->queue_ptr = NULL; 
            
    //init neighbors array

    personal->internals_array = (nodeinfo_t **)malloc(MAX_INTERNALS * sizeof(nodeinfo_t *));
    for (iter = 0; iter < MAX_INTERNALS; iter++) {
        personal->internals_array[iter] = NULL;
    }   

    return personal;
}//personal_init()


void free_contact(nodeinfo_t *contact){
    
    free(contact->node_buff);
    free(contact->network);
    free(contact->tcp_port);
    free(contact->node_addr);    
    free(contact);

    return;
}//free_contact()


struct personal_node *reset_personal(struct personal_node *personal){
   
    int iter = 0;
    nodesLinkedlist_t *aux1, *aux2;

    /* clear the memory for the internal nodes */

    aux1 = personal->internals_list;
    
    while(aux1 != NULL){
        aux2 = aux1;
        aux1 = aux1->next;
        free_contact(aux2->node);
        free(aux2);
    }
    
    /*clear extern and backup nodes*/
    if (personal->extern_node != NULL){        
        free_contact(personal->extern_node);
        personal->extern_node = NULL;
    }
            
    if (personal->backup_node != NULL){
        free_contact(personal->backup_node);
        personal->backup_node = NULL;
    }

    personal->anchorflag = -1;   // flag that says whether the node is an anchor or not
    personal->n_internals = 0;    // counter for the number of internal neighbors    
    personal->max_fd = 0;        // the maximum integer assigned to a file descriptor
    personal->client_fd = -1;   // file descriptor for a client node

    return personal;

}//reset_personal()

void contact_copy(nodeinfo_t *dest, nodeinfo_t *src) {

    memset(dest->network, 0, 4 * sizeof(*dest->network));
    strcpy(dest->network, src->network);

    memset(dest->tcp_port, 0, 6 * sizeof(*dest->tcp_port));
    strcpy(dest->tcp_port, src->tcp_port);
    
    memset(dest->node_addr, 0, MAX_ADDRESS_SIZE * sizeof(*dest->node_addr));
    strcpy(dest->node_addr, src->node_addr);

    memset(dest->node_buff, 0, MAX_MSG_LENGTH * sizeof(*dest->node_buff));
    strcpy(dest->node_buff, src->node_buff);

    return;
}//contact_copy()

nodesLinkedlist_t *Listinit(nodesLinkedlist_t *head){

    nodesLinkedlist_t *newlist_head = (nodesLinkedlist_t*)calloc(1, sizeof(nodesLinkedlist_t));

    newlist_head->node = contact_init(newlist_head->node);
    newlist_head->next = NULL;
    head = newlist_head;

    return head;

}

nodesLinkedlist_t *insertnode(nodesLinkedlist_t *head, nodeinfo_t *new_node){

    // Check if the list is initialized

    if(head == NULL){
        printf("Error in insertnode: list is not initialized");
        return NULL;
    }
    
    nodesLinkedlist_t *aux, *new_block;    

    // Write the info in memory

    new_block = (nodesLinkedlist_t*)calloc(1, sizeof(nodesLinkedlist_t));
    new_block->node = contact_init(new_block->node);
    contact_copy(new_block->node, new_node);
    new_block->next = NULL;

    // Go through the list and insert at the end

    aux = head;

    while(aux->next != NULL){
        aux = aux->next;
    }

    aux->next = new_block;
    printf("Successfully inserted %s | %s in the neighbor list\n", new_node->node_addr, new_node->tcp_port);
    return head;

}

nodesLinkedlist_t *removenode(nodesLinkedlist_t *head, int old_fd){

    if(head == NULL){
		printf("\nThe list is empty\n\n");
		return head;
	}

    nodesLinkedlist_t *listptr, *aux;
	listptr = head;

	if(head->node->node_fd == old_fd){ //if the head has the desired node

		printf("\n%s | %s was removed from the internals list\n\n", head->node->node_addr, head->node->tcp_port);
        aux = head;
		head = head->next;
		free_contact(aux->node);		
		free(aux);		
		if(head == NULL){
			printf("\nThe list of internals is now empty\n\n");
		}
		return head;			
	}
	
	while(listptr != NULL){

		if(listptr->next == NULL && listptr->node->node_fd == old_fd){
			printf("\nThe name was not found\n\n");
			return head;
		}
		else if(listptr->node->node_fd == old_fd){ //first or last node, and it contains the name

			printf("\n%s | %s was removed from the internals list\n\n", listptr->node->node_addr, listptr->node->tcp_port);
            aux = listptr;
			listptr = listptr->next;			
			free_contact(aux->node);
			free(aux);
			
			if(listptr == NULL){
				printf("\nThe list of internals is now empty\n\n");
			}
			return head;
		}		
		else if(*(listptr)->next->node->node_fd == old_fd){
			
			printf("\n%s | %s was removed from the internals list\n\n", listptr->next->node->node_addr, listptr->next->node->tcp_port);
            aux = listptr->next;
			listptr->next = listptr->next->next;			
			free_contact(aux->node);
			free(aux);
			
			return head;
			
		}
		else listptr = listptr->next;
	}
	return head;
}

void clearnodelist(nodesLinkedlist_t *head){

}