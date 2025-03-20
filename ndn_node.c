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
#include "ndn_node.h"
#include "ndn_messages.h"
//#include "ndn_queue.h"



nodeinfo_t *contact_init(nodeinfo_t *contact){
    
    // initialize with NULL before calling this function
    
    if((contact = (nodeinfo_t*)calloc(1, sizeof(nodeinfo_t))) == NULL){
        printf("Error in contact_init: Failed to allocate memory\n");
        exit(1);
    }
    
    /* initializing positive integer variables to -1 */
             
    if((contact->tcp_port = (char*)calloc(MAX_TCP_UDP_CHARS, sizeof(char))) == NULL){
        printf("Error in contact_init: Failed to allocate memory\n");
        exit(1);
    }
    if((contact->node_addr = (char*)calloc(MAX_ADDRESS_SIZE, sizeof(char))) == NULL){
        printf("Error in contact_init: Failed to allocate memory\n");
        exit(1);
    }
    if((contact->node_buff = (char*)calloc(MAX_MSG_LENGTH, sizeof(char))) == NULL){
        printf("Error in contact_init: Failed to allocate memory\n");
        exit(1);
    }
    
    contact->node_fd = -1;
    contact->entry_flag = 0;
    contact->safe_flag = 0;
    contact->msg_object_flag = 0;
    
    return contact;
}//contact_init()


struct personal_node *personal_init(struct personal_node *personal){

    if((personal = (struct personal_node*)calloc(1, sizeof(struct personal_node))) == NULL){
        printf("Error in personal_init: Failed to allocate memory. Process terminated\n");
        exit(1);
    }
    
    if((personal->personal_net = (char*)calloc(MAX_NET_CHARS, sizeof(char))) == NULL){
        printf("Error in personal_init: Failed to allocate memory. Process terminated\n");
        exit(1);
    }

    if((personal->backup_addr = (char*)calloc(MAX_ADDRESS_SIZE, sizeof(char))) == NULL){
        printf("Error in personal_init: Failed to allocate memory. Process terminated\n");
        exit(1);
    }

    if((personal->backup_tcp = (char*)calloc(MAX_TCP_UDP_CHARS, sizeof(char))) == NULL){
        printf("Error in personal_init: Failed to allocate memory. Process terminated\n");
        exit(1);
    }
    personal->anchorflag = -1;   // flag that says whether the node is an anchor or not
    personal->network_flag = 0;
    personal->join_flag = 0;
    personal->n_internals = 0;    // counter for the number of internal neighbors    
    personal->max_fd = 0;        // the maximum integer assigned to a file descriptor        
    personal->personal_addr = NULL;
    personal->personal_tcp = NULL;  
    personal->udp_port = NULL;   //UDP server port
    personal->udp_address = NULL;    //UDP server address
    personal->extern_node = NULL;   //extern neighbor node
    
   
    /* set the start of the list to NULL */

    //personal->queue_ptr = NULL; 
            
    //init neighbors list

    personal->internals_list = NULL;
    Listinit(personal->internals_list);
    
    return personal;
}//personal_init()


void free_contact(nodeinfo_t **contact){
    
    free((*contact)->node_addr);
    free((*contact)->tcp_port);    
    free((*contact)->node_buff);            
    free(*contact);

    return;
}//free_contact()

void reset_contact(nodeinfo_t **contact){
    memset((*contact)->node_addr, 0, MAX_ADDRESS_SIZE);
    memset((*contact)->tcp_port, 0, MAX_TCP_UDP_CHARS);
    memset((*contact)->node_buff, 0, MAX_MSG_LENGTH);
    (*contact)->node_fd = -1;
    (*contact)->entry_flag = 0;
    (*contact)->safe_flag = 0;
    (*contact)->msg_object_flag = 0;
    return;

}


struct personal_node *reset_personal(struct personal_node *personal){
           
    personal->internals_list = clearlist(personal->internals_list);
    
    /*clear extern and backup nodes*/
    if (personal->extern_node != NULL){
        close(personal->extern_node->node_fd);
        reset_contact(&(personal->extern_node));
        
    }
            
    memset(personal->backup_addr, 0, MAX_ADDRESS_SIZE);
    memset(personal->backup_tcp, 0, MAX_TCP_UDP_CHARS);

    personal->anchorflag = -1;   // flag that says whether the node is an anchor or not
    personal->network_flag = 0;
    personal->join_flag = 0;
    personal->n_internals = 0;    // counter for the number of internal neighbors    
    personal->max_fd = 0;        // the maximum integer assigned to a file descriptor
    

    return personal;

}//reset_personal()

void contact_copy(nodeinfo_t *dest, nodeinfo_t *src) {

    
    memset(dest->tcp_port, 0, 6 * sizeof(*dest->tcp_port));
    strcpy(dest->tcp_port, src->tcp_port);
    
    memset(dest->node_addr, 0, MAX_ADDRESS_SIZE * sizeof(*dest->node_addr));
    strcpy(dest->node_addr, src->node_addr);

    memset(dest->node_buff, 0, MAX_MSG_LENGTH * sizeof(*dest->node_buff));
    strcpy(dest->node_buff, src->node_buff);

    dest->node_fd = src->node_fd;

    return;
}//contact_copy()

nodesLinkedlist_t *Listinit(nodesLinkedlist_t *head){
    
    // if((head = (nodesLinkedlist_t*)calloc(1, sizeof(nodesLinkedlist_t))) == NULL){
    //     printf("Error in Listinit: calloc failed. Process terminated");
    //     exit(1);
    // }    
    
    // head->node = NULL;             
    
    // head->node = contact_init(head->node);
    // head->next = NULL;
    
    head = NULL;
    return head;

}

nodesLinkedlist_t *insertnode(nodesLinkedlist_t *head, nodeinfo_t *new_node){
        
    nodesLinkedlist_t *aux, *new_block;    

    // Write the info in memory

    if((new_block = (nodesLinkedlist_t*)calloc(1, sizeof(nodesLinkedlist_t))) == NULL){
        
        printf("Error in insertnode: Failed to allocate memory");
        exit(1);
    }
    // if((new_block->node = (nodeinfo_t*)calloc(1, sizeof(nodeinfo_t))) == NULL){
        
    //     printf("Error in insertnode: Failed to allocate memory: process terminated");
    //     exit(1);
    // }
    // contact_init(new_block->node);
    // contact_copy(new_block->node, new_node);
    new_block->node = new_node;
    new_block->next = NULL;

    if(head == NULL){
        head = new_block;
        return head;
    }

    // Go through the list and insert at the end

    aux = head;

    while(aux->next != NULL){
        aux = aux->next;
    }

    aux->next = new_block;
    printf("\nSuccessfully inserted %s | %s in a list\n", new_node->node_addr, new_node->tcp_port);
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
		free_contact(&(aux->node));		
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
		else if(listptr->node->node_fd == old_fd){ //first or last node, and it contains the fd

			printf("\n%s | %s was removed from the internals list\n\n", listptr->node->node_addr, listptr->node->tcp_port);
            aux = listptr;
			listptr = listptr->next;			
			free_contact(&(aux->node));
			free(aux);
			
			if(listptr == NULL){
				printf("\nThe list of internals is now empty\n\n");
			}
			return head;
		}		
		else if(listptr->next->node->node_fd == old_fd){
			
			printf("\n%s | %s was removed from the internals list\n\n", listptr->next->node->node_addr, listptr->next->node->tcp_port);
            aux = listptr->next;
			listptr->next = listptr->next->next;			
			free_contact(&(aux->node));
			free(aux);
			
			return head;
			
		}
		else listptr = listptr->next;
	}
	return head;
}

nodesLinkedlist_t *clearlist(nodesLinkedlist_t* head){

    if(head == NULL) return head;
    nodesLinkedlist_t *aux1, *aux2;

    /* clear the memory for the internal nodes */
    
    aux1 = head;
    
    while(aux1 != NULL){
        aux2 = aux1;
        aux1 = aux1->next;
        free_contact(&(aux2->node));
        free(aux2);
    }
    head = NULL;
    return head;
}
