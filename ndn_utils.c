/************************************************************************************************
 * ndn_utils.c
 *
 * Authors:
 *
 * -Igor Paruque ist1102901
 * -Mónica Ferreira ist106520
 *
 * Description: source code for the utility functions
 ***********************************************************************************************/

 // general purpose libraries
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>

// project libraries
#include "ndn_io.h"
#include "ndn_netfuncs.h"
#include "ndn_utils.h"


//-------------------------------------------- user commands --------------------------------------------//

int join(struct personal_node *personal, char *net) { 

    int success_flag = 0;

    if(strcmp(personal->persn_info->network, "") != 0){
        printf("The node is already inside a network. Command ignored\n");
        return ++success_flag;
    }
    if(check_net(net) == 1){
        printf("The network is invalid. Command ignored\n");
        printf("Please insert a three digit network from 000 to 999\n");
        return ++success_flag;
    }

    char msg[MAX_MSG_LENGTH];      memset(msg, 0, sizeof(msg));
    udp_flag++; 

    strcpy(personal->persn_info->node_id, id);

    sprintf(msg, "%s %s\n", nodes_str, net);
    success_flag = send_udp(personal, msg);
    return success_flag;

}//join


int djoin(struct personal_node *personal, char *net, char *connectIP, char *connectTCP){

    char msg[MAX_MSG_LENGTH];   memset(msg, 0, sizeof(msg));
    int success_flag = 0;

    if(strcmp(personal->persn_info->network, "") != 0){
        printf("The node is already inside a network. Command ignored\n");
        return ++success_flag;
    }
    if(check_net(net) == 1){
        printf("The network is invalid. Command ignored\n");
        printf("Please insert a three digit network from 000 to 999\n");
        return ++success_flag;
    }

    if(udp_flag == 0){
           
        if(check_ports(connectTCP) == 1){
            printf("The boot TCP port is invalid. Command ignored\n");
            printf("Please insert a 1 to 5 digit TCP port from 0 to 65536");
            return ++success_flag;
        }
    }
    
    strcpy(personal->persn_info->network, net);

    if (strcmp(connectIP, "0.0.0.0") == 0) {  //First node of the network
        personal->anchorflag = 1; //!confirmar
        personal->extern_node = contact_init(personal->extern_node);
        contact_copy(personal->extern_node, personal->persn_info);
        personal->backup_node = contact_init(personal->backup_node);
        contact_copy(personal->backup_node, personal->persn_info);
    }
    else{
        personal->extern_node = contact_init(personal->extern_node);        
        strcpy(personal->extern_node->network, net);
        strcpy(personal->extern_node->tcp_port, connectTCP);
        strcpy(personal->extern_node->node_addr, connectIP);
        personal->backup_node = contact_init(personal->backup_node);

        //ENTRY message
        sprintf(msg, "%s %s %s\n", entry_str, personal->persn_info->node_addr, personal->persn_info->tcp_port);
        //Sends ENTRY message , receives SAFE response and updates backup node
        if(send_tcp(personal, personal->extern_node, msg)){
            printf("Failed to join the network\n");
            return 1;
        }
        // check if it's an anchor
        if (strcmp(personal->persn_info->node_addr, personal->backup_node->node_addr) == 0 && 
        strcmp(personal->persn_info->tcp_port, personal->backup_node->tcp_port) == 0) {
            personal->anchorflag = 1;
        }
    }
    printf("Successfuly joined a network\n"); 
    printf("Your node has the following info:\n\n");
    
    printf("Network: %s\nAdress: %s\nPort: %s\n",       
    personal->persn_info->network,
    personal->persn_info->node_addr, 
    personal->persn_info->tcp_port);
    
    return 0;
}//djoin


objectQueue_t *create(objectQueue_t *queue_ptr, char *name){ // name size <= 100, alphanumeric chars only

    if(check_name(name) == 1){
        printf("Failed to create a content. Name was not valid\n");
        return contents;
    }
    
    int name_size = strlen(name) + 1; 
    queueBlock_t *new_node = (queueBlock_t*)calloc(1, sizeof(queueBlock_t));
    new_node->name = (char*)calloc(name_size, sizeof(char));
    strcpy(new_node->name, name);

    if(queue_ptr == NULL){ // if true, we are creating the first element of the list

        queue_ptr = new_node;
        contents->next = NULL;
        
        printf("Content created at the beginning of the list: %s\n", name);
        return contents;
    }

    objectQueue_t *queue_ptr; // pointer to go through the list
    
    queue_ptr = contents;

    while(queue_ptr->next != NULL){ //reach the last element of the list
        queue_ptr = queue_ptr->next;		
    }
        
    queue_ptr->next = new_node;
    printf("\n%s was inserted in the list\n\n", name);
    return contents;
}//create


objectQueue_t *delete(objectQueue_t *contents, char *name) {
    
    if(check_name(name) == 1){
        printf("Error: Invalid name\n");
        return contents;
    }

    if(contents == NULL){
        printf("Error: No contents available\n");
        return contents;
    }

    objectQueue_t *queue_ptr; // pointer to go through the list
    objectQueue_t *aux; // auxiliary pointer to delete elements of the lists
    queue_ptr = contents;

    if(strcmp(contents->string, name) == 0){ //delete at the head of the list

        aux = contents;
        contents = contents->next;
        free(aux->string);
        free(aux);
        printf("%s was successfuly deleted\n", name);
        if(contents == NULL){
            printf("\nThe list of contents is now empty\n\n");
        }		
        return contents;
    }

    while(queue_ptr != NULL){

        if(queue_ptr->next == NULL && strcmp(queue_ptr->string, name) != 0){            
            printf("Error: The name was not found\n");
            return contents;
        }
        
        else if(strcmp(queue_ptr->string, name) == 0){ 

            aux = queue_ptr;
            queue_ptr = queue_ptr->next;
            free(aux->string);
            free(aux);
            printf("The deletion of %s was successful\n", name);
            if(queue_ptr == NULL){
                printf("\nThe list of contents is now empty\n\n");
            }
            return contents;
        }
        
        else if(strcmp(queue_ptr->next->string, name) == 0){
            
            aux = queue_ptr->next;
            queue_ptr->next = queue_ptr->next->next;
            free(aux->string);
            free(aux);
            printf("The deletion of %s was successful\n", name);
            return contents;
        }
        else queue_ptr = queue_ptr->next;
    }    
    return contents;
}//delete


int retrieve(struct personal_node *personal, char *name){

    int success_flag = 0;
    if(strcmp(personal->persn_info->network, "") == 0){
        printf("Error: You are not inside a network\n");
        return ++success_flag;
    }
    
    /* check the expedition table */
    
    int search_id = atoi(dest_id);  //integer to search the table of intern neighbors
    
    int gateway = -1;                      //id of neighbor to send something to the destination
    char retrieve_buffer[MAX_MSG_LENGTH];     //buffer for the "QUERY" message
    
    memset(retrieve_buffer, 0, sizeof(retrieve_buffer));

    sprintf(retrieve_buffer,"RETRIEVE %s %s\n", name);    

    if(personal->route_tab[search_id] == search_id){ // destination is equal to the neighbor, but what type of neighbor?

        if(strcmp(personal->extern_node->node_id, dest_id) == 0 || strcmp(personal->backup_node->node_id, dest_id) == 0 ){ // extern/backup neighbor
            printf("Sending: %s\n", retrieve_buffer);
            send_tcp(personal, personal->extern_node, retrieve_buffer);                        
        }
        else{
            printf("Sending: %s\n", retrieve_buffer);
            send_tcp(personal, personal->internals_array[search_id], retrieve_buffer); // intern neighbor 
        }                               
    }
    else if(personal->route_tab[search_id] != -1){ // destination is initialized and is different from the neighbor

        gateway = personal->route_tab[search_id];
        if(gateway == atoi(personal->extern_node->node_id) || gateway == atoi(personal->backup_node->node_id)){
            printf("Sending: %s\n", retrieve_buffer);
            send_tcp(personal, personal->extern_node, retrieve_buffer);
        }
        else{
            printf("Sending: %s\n", retrieve_buffer);
            send_tcp(personal, personal->internals_array[gateway], retrieve_buffer);
        }        
    }
    else{ //destination is not a neighbor and it is not initialized
        printf("Sending: %s\n", retrieve_buffer);
        send_tcp(personal, personal->extern_node, retrieve_buffer);

        for(int iter = 0; iter < MAX_INTERNALS; iter++){

            if(personal->internals_array[iter] != NULL){
                printf("Sending: %s\n", retrieve_buffer);
                send_tcp(personal, personal->internals_array[iter], retrieve_buffer);                
            }
        }
    }
    return 0;
}//retrieve


int show_topology(struct personal_node *personal){
    
    int success_flag = 0;
    int printed_interns = 0;

    if(strcmp(personal->persn_info->network, "") != 0){
        printf("\n\nYour node:\n\n");        
        printf("Network: %s\nAdress: %s\nPort: %s\n\n",           
        personal->persn_info->network,
        personal->persn_info->node_addr, 
        personal->persn_info->tcp_port);
        
    }
    else{
        printf("Error: You are not inside a network\n");
        return ++success_flag;
    }
    
    if (personal->extern_node != NULL) {
        printf("\n\nExternal neighbor: \n");        
        printf("Network: %s\nAdress: %s\nPort: %s\n\n",          
        personal->extern_node->network,
        personal->extern_node->node_addr, 
        personal->extern_node->tcp_port);
        
        
    }
    else {
        printf("\nThere is no external neighbor!\n");
    }
    if (personal->backup_node != NULL) {
        printf("\n\nBackup neighbor: \n");                
        printf("Network: %s\nAdress: %s\nPort: %s\n\n",         
        personal->backup_node->network, 
        personal->backup_node->node_addr, 
        personal->backup_node->tcp_port);
        
        
    }   
    else {
        printf("\nThere is no backup neighbor!\n");   
    }

    int iter;
        
    if(personal->internals_array == NULL){
        printf("\nThere are no internal neighbors\n");        
    }
    else{
        printf("\n\nPrinting internal neighbors: \n\n");        
            
        for(iter = 0; iter < MAX_INTERNALS; iter++){

            if(personal->internals_array[iter] != NULL){

                printf("ID: %s\nNetwork: %s\nAdress: %s\nPort: %s\n\n",                    
                personal->internals_array[iter]->network,
                personal->internals_array[iter]->node_addr,
                personal->internals_array[iter]->tcp_port);

                printed_interns++;
            }//if            
        }//for                    
    }
    
    if(printed_interns == 0){ // the array is initialized

        printf("There are no internal neighbors\n");
        return ++success_flag;
    }
    return success_flag;
} // show_topology

int show_names(objectQueue_t **contents){

    int printed_names = 0; // counter of printed names
    int success_flag = 0; //success signal for the main function

    if(*contents == NULL){
        printf("The node has no contents to print\n");
        return ++success_flag;
    }

    printf("Printing names...\n\n");
    objectQueue_t *queue_ptr; // pointer to go through the list

    for(queue_ptr = *contents; queue_ptr != NULL; queue_ptr = queue_ptr->next){
        printf("%s\n", queue_ptr->string);
        printed_names++;
    }
    printf("\nNumber of printed names: %d\n", printed_names);
    return success_flag;
}//show_names

int show_interest_table (struct personal_node *personal) { 

    int success_flag = 0;

    if(strcmp(personal->persn_info->network, "") == 0){
        printf("Error: You are not inside a network\n");
        return ++success_flag;
    }

    printf("\nInterest Table: \n\n");
    for (int i = 0; i < 10; i++) {
        if (personal->interest_tab[i].node_id[0] != '\0') {
            printf("Entry %d:\n", i + 1);
            printf("ID: %s\nNetwork: %s\nAddress: %s\nPort: %s\n\n",
                personal->interest_tab[i].node_id,
                personal->interest_tab[i].network,
                personal->interest_tab[i].node_addr,
                personal->interest_tab[i].tcp_port);
        }
    }

    return success_flag;
}//show_interest_table  

objectQueue_t *clear_names(objectQueue_t *contents){

    int cleared_names = 0;  //number of names cleared

    if(contents == NULL){
        printf("No contents to clear in this node\n");
        return contents;
    }
    printf("\nClearing names...\n");

    objectQueue_t *queue_ptr; // pointer to go through the list
    objectQueue_t *aux;     // auxiliary pointer to delete elements in the lists
    queue_ptr = contents;
    while(queue_ptr != NULL){

        aux = queue_ptr;
        queue_ptr = queue_ptr->next;
        free(aux->string);
        free(aux);
        cleared_names++;
    }    
    printf("\nNumber of cleared names: %d\n", cleared_names);
    contents = NULL;
    return contents;    
}//clear_names()


int leave(struct personal_node *personal) {
    
    if(strcmp(personal->persn_info->network, "") == 0){        
        personal = reset_personal(personal);
        return 1;
    }
    
    if(udp_flag == 1){
        char msg[MAX_MSG_LENGTH];
        memset(msg, 0, sizeof(msg));
        sprintf(msg, "%s %s\n", unreg_str, personal->persn_info->network);
        send_udp(personal, msg);
        udp_flag = 0;
    }
    
    int i = 0;
    for (i = 3; i <= personal->max_fd; i++) {
        if (FD_ISSET(i, &personal->crr_scks)) {
            FD_CLR(i, &personal->crr_scks);
            close(i);
        }
    }

    strcpy(personal->persn_info->network, "");
    personal = reset_personal(personal);
    return 0;
}//leave

void help_menu() {
        printf("*****************HELP MENU*****************\n\n");
        printf("To use the application, insert one of the following commands:\n");
        printf("Note: curved brackets => valid abreviations; square brackets => arguments\n\n");
        printf("join (j) [desired network] \n");
        printf("direct join (dj) [desired network] [connect IPv4 address] [connect TCP port]\n");        
        printf("create (c) [name]\n");
        printf("delete (dl) [name]\n");
        printf("retrieve (r) [name]\n");
        printf("show topology (st)\n");
        printf("show names (sn)\n");
        printf("show interest table (si)\n");        
        printf("clear names (cn)\n");
        printf("leave (l)\n");
        printf("exit (x)\n");
        return;
}//help_menu

/*--------------End user interface functions--------------*/