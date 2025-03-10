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
#include "ndn_queue.h"

nodeinfo_t *contact_init(nodeinfo_t *contact){

    contact = (nodeinfo_t*)calloc(1, sizeof(nodeinfo_t)); // the block itself

    /* initializing positive integer variables to -1 */
          
    contact->network = (char*)calloc(MAX_NET_CHARS, sizeof(char));      // integer from 000 to 999
    contact->tcp_port = (char*)calloc(MAX_TCP_UDP_CHARS, sizeof(char));     // tcp port; integer from 0 to 65 536
    contact->node_addr = (char*)calloc(MAX_ADDRESS_SIZE, sizeof(char));   // IPv4 address
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

    personal->queue_ptr = NULL; 
            
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

    /* clear the memory for the internal nodes */

    for(iter = 0; iter < MAX_INTERNALS; iter++){

        if(personal->internals_array[iter] != NULL){
            free_contact(personal->internals_array[iter]);
            personal->internals_array[iter] = NULL;
        } 
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