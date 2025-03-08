/************************************************************************************************
 * g23funcs.c
 *
 * Authors:
 *
 * -Igor Paruque ist1102901
 * -Afonso Klier ist196139
 * 
 * Description: source code for the functionalities of the program
 ***********************************************************************************************/

//#define _XOPEN_SOURCE 600 //!REMOVE BEFORE COMPILING

//general purpose libraries
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <errno.h>
#include "g23funcs.h"
#include "g23tcp_udp.h"

//libraries for the network communications
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/select.h>

int udp_flag = 0;

/* ----------------------------utility functions-------------------------------- */

int check_id(char *id){

    int success_flag = 0;

    if(strlen(id) != 2){
        printf("Invalid node id\n");
        return ++success_flag;
    }

    if(0 <= atoi(id) && atoi(id) <= 99) return success_flag;

    else{
        printf("Invalid node id\n");
        return ++success_flag;
    }
}//check_id()

int check_net(char *net){
    
    int success_flag = 0;
    
    if(strlen(net) != 3){
        
        printf("Invalid net\n");
        return ++success_flag;
    }

    if(0 <= atoi(net) && atoi(net) <= 999) return success_flag;
    else{
        printf("Invalid net\n");
        return ++success_flag;
    }
}//check_net()

int check_ports(char *port_str){
    
    int length = strlen(port_str), flag = 0;
    
    if(length > 5) return ++flag;
    
    if(atoi(port_str) > 65536)return ++flag;
    
    else return flag;
}//check_ports()


 int check_name(char *name){

    int flag = 0; //success flag

    if(strlen(name) > 100){
        printf("Content size is too big. Content ignored\n");
        return ++flag;
    }
    else return flag;
}//check_name


 nodeinfo_t *contact_init(nodeinfo_t *contact){

    contact = (nodeinfo_t*)calloc(1, sizeof(nodeinfo_t)); // the block itself

    /* initializing positive integer variables to -1 */
      
    contact->node_id = (char*)calloc(MAX_ID_CHARS, sizeof(char));      // integer from 00 to 99
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
    personal->n_neighbrs = 0;    // counter for the number of internal neighbors
    personal->server_fd = -1;   // file descriptor for a server node
    personal->max_fd = 0;        // the maximum integer assigned to a file descriptor
    personal->client_fd = -1;   // file descriptor for a client node  
    personal->udp_port = NULL;   //UDP server port
    personal->udp_address = NULL;    //UDP server address
    personal->extern_node = NULL;   //extern neighbor node
    personal->backup_node = NULL;   //backup neighbor node

   /* initializing the integers in the routing table */

    personal->route_tab = (int*)malloc(NODE_IDS * sizeof(int));      
    
    for(iter = 0; iter < NODE_IDS; iter++){

        personal->route_tab[iter] = -1;
    }

    /* set the start of the list to NULL */

    personal->contents = NULL; 

    //init neighbrs_fd array

    for (iter = 0; iter < NODE_IDS; iter++) {
        personal->neighbrs_fd[iter] = -1;
    }
    
    //init neighbors array

    personal->neighbrs = (nodeinfo_t **)malloc(NODE_IDS * sizeof(nodeinfo_t *));
    for (iter = 0; iter < NODE_IDS; iter++) {
        personal->neighbrs[iter] = NULL;
    }   

    return personal;
}//personal_init()


 void free_contact(nodeinfo_t *contact){
    
    free(contact->node_id);
    free(contact->network);
    free(contact->tcp_port);
    free(contact->node_addr);    
    free(contact);

    return;
}//free_contact()


 struct personal_node *reset_personal(struct personal_node *personal){
   
    int iter = 0;
    /* reset the routing table */

    for(iter = 0; iter < NODE_IDS; iter++){

        personal->route_tab[iter] = -1;
    }

    /* clear the memory for the contacts */

    for(iter = 0; iter < NODE_IDS; iter++){

        if(personal->neighbrs[iter] != NULL){
            free_contact(personal->neighbrs[iter]);
            personal->neighbrs[iter] = NULL;
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
    personal->n_neighbrs = 0;    // counter for the number of internal neighbors
    personal->server_fd = -1;   // file descriptor for a server node
    personal->max_fd = 0;        // the maximum integer assigned to a file descriptor
    personal->client_fd = -1;   // file descriptor for a client node

    return personal;

}//reset_personal()

 void contact_copy(nodeinfo_t *dest, nodeinfo_t *src) {

    memset(dest->node_id, 0, 3 * sizeof(*dest->node_id));
    strcpy(dest->node_id, src->node_id);

    memset(dest->network, 0, 4 * sizeof(*dest->network));
    strcpy(dest->network, src->network);

    memset(dest->tcp_port, 0, 6 * sizeof(*dest->tcp_port));
    strcpy(dest->tcp_port, src->tcp_port);
    
    memset(dest->node_addr, 0, MAX_ADDRESS_SIZE * sizeof(*dest->node_addr));
    strcpy(dest->node_addr, src->node_addr);

    return;
}//contact_copy()



/*------------------------------------------User inteface funcions------------------------------------------*/

 void select_cmd(struct personal_node *personal, char *input){ 
    
    char net_num[MAX_NET_CHARS]; memset(net_num, 0, sizeof(net_num));            // network number passed in the commands
    char id_num[MAX_ID_CHARS];  memset(id_num, 0, sizeof(id_num));           // node id passed in the commands
    char tcp[MAX_TCP_UDP_CHARS];      memset(tcp, 0, sizeof(tcp));          // a tcp port passed in the commands
    char udp[MAX_TCP_UDP_CHARS];      memset(udp, 0, sizeof(udp));          // the udp port of the node server passed in the commands
    char boot_id[MAX_ID_CHARS];  memset(boot_id, 0, sizeof(boot_id));          // the boot node id passed in the djoin command

    char first_arg[MAX_MSG_CMD_SIZE]; memset(first_arg, 0, sizeof(first_arg));    // should be enough for the first string of the command
    char address[MAX_ADDRESS_SIZE];   memset(address, 0, sizeof(address));    // an IPv4 address passed in the commands
    char content[MAX_CONTENT_NAME];   memset(content, 0, sizeof(content));     // the name of a content passed in the commands

    // parse the first string

    sscanf(input, "%s", first_arg);

    // try to read and execute the desired command

    if(strcmp(first_arg, join_str) == 0){

        if(strcmp(personal->persn_info->network, "") != 0 ){
            printf("The node already belongs to a network. Command ignored\n");
            return;
        }       
        else if(sscanf(input, "%s %s %s", first_arg, net_num, id_num) == 3){
            
            printf("Executing %s...\n\n", first_arg);
            join(personal, net_num, id_num); //join a network and register the personal node in the server
            return;
        }                        
        else{
            printf("Failed to read arguments of %s\n", first_arg); 
            return;
        } 
    }//else if

    else if(strcmp(first_arg, djoin_str) == 0){

        if(strcmp(personal->persn_info->network, "") != 0 ){
            printf("The node already belongs to a network. Command ignored\n");
            return;
        }
        else if(sscanf(input, "%*s %s %s %s %s %s", net_num, id_num, boot_id, address, tcp) == 5){
            
            printf("Executing %s...\n\n", first_arg);
            djoin(personal, net_num, id_num, boot_id, address, tcp); // join a network without registration
            return;
        }       
        else{
            printf("Failed to read arguments of %s\n", first_arg); 
            return;
        } 
    }//else if

    else if(strcmp(first_arg, create_str) == 0){

        if(sscanf(input, "%s %s", first_arg, content) == 2){
            
            printf("Executing %s...\n\n", first_arg);
            personal->contents = create(personal->contents, content); // create and store a content inside the personal node           
            return; 
        }
        else{
            printf("Failed to read arguments of %s\n", first_arg); 
            return;
        } 
    }//else if

    else if(strcmp(first_arg, delete_str) == 0){

        if(sscanf(input, "%s %s", first_arg, content) == 2){
            
            printf("Executing %s...\n\n", first_arg);
            personal->contents = delete(personal->contents, content); // delete a content inside the personal node
            return;            
        }
        else{
            printf("Failed to read arguments of %s\n", first_arg); 
            return;
        } 
    }//else if

    else if(strcmp(first_arg, get_str) == 0){
        
        if(sscanf(input, "%s %s %s", first_arg, id_num, content) == 3){
            
            printf("Executing %s...\n\n", first_arg);
            get(personal, id_num, content); // search and copy a content from another node        
            return; 
        }
        else{
            printf("Failed to read arguments of %s\n", first_arg); 
            return;
        } 
    }//else if

    else if(strcmp(first_arg, show_topology_str) == 0){
        
        printf("Executing %s...\n\n", first_arg);
        show_topology(personal);  // show topology of the personal node
        return;

    }// else if

    else if(strcmp(first_arg, show_names_str) == 0){
        
        printf("Executing %s...\n\n", first_arg);
        show_names(&personal->contents); // show contents of the personal node
        return;

    }//else if

    else if(strcmp(first_arg, clear_names_str) == 0){

        printf("Executing %s...\n\n", first_arg);
        personal->contents = clear_names(personal->contents);  // clear the contents table of the personal node            
        return;

    }// else if

    else if(strcmp(first_arg, show_routing_str) == 0){

        if(strcmp(personal->persn_info->network, "") == 0)
        
        printf("Executing %s...\n\n", first_arg);
        show_routing(personal->route_tab); // show routing table of the personal node
        return;

    }//else if 

    else if(strcmp(first_arg, clear_routing_str) == 0){

        printf("Executing %s...\n\n", first_arg);
        clear_routing(personal->route_tab); // clear the routing table of the personal node
        return;
        
    }// else if

    else if(strcmp(first_arg, leave_str) == 0){

        //remove the personal node from the network
        // reset the struct in order to give the possibility to join or djoin again

        printf("Executing %s...\n\n", first_arg);
        if(strcmp(personal->persn_info->network, "") == 0){
            printf("The node is already without a network.\n");
        }
        leave(personal);

    }//else if

    else if(strcmp(first_arg, exit_str) == 0){

        // remove the personal node from the network  
        // free the memory, close the fds and get out of the program     
        
        printf("Executing %s...\n\n", first_arg);
        leave(personal);
        free(personal->route_tab);
        free(personal->neighbrs);
        free_contact(personal->persn_info);

        if(personal->contents != NULL){
        
            contentList_t *listptr; // pointer to go through the list
            contentList_t *aux;     // auxiliary pointer to delete elements in the lists
            listptr = personal->contents;
            
            while(listptr != NULL){

                aux = listptr;
                listptr = listptr->next;
                free(aux->string);
                free(aux);
                
            }                            
        }
            
        free(personal);
        exit(0);

    }//else if

    else if(strcmp(first_arg, help_str) == 0) {
        
        printf("Executing %s...\n\n", first_arg);
        help_menu();
        return;
    }
    
    else{
        printf("%s does not match any known command. Therefore, the command will be ignored\n", first_arg);
        return;
    }
    
}//select_cmd()


 int join(struct personal_node *personal, char *net, char *id) { 

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


 int djoin(struct personal_node *personal, char *net, char *id, char *bootid, char *bootIP, char *bootTCP){

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
        if(check_id(id) == 1){
            printf("Your desired id is invalid. Command ignored\n");
            printf("Please, insert a two digit id from 00 to 99\n");
            return ++success_flag;
        }
        if(check_id(bootid) == 1){
            printf("The boot id is invalid. Command ignored\n");
            printf("Please, insert a two digit id from 00 to 99\n");
            return ++success_flag;
        }
    
        if(check_ports(bootTCP) == 1){
            printf("The boot TCP port is invalid. Command ignored\n");
            printf("Please insert a 1 to 5 digit TCP port from 0 to 65536");
            return ++success_flag;
        }
    }

    strcpy(personal->persn_info->node_id, id);
    strcpy(personal->persn_info->network, net);

    if (strcmp(id, bootid) == 0) {  //Only node in the network
        personal->anchorflag = 1; //!confirmar
        personal->extern_node = contact_init(personal->extern_node);
        contact_copy(personal->extern_node, personal->persn_info);
        personal->backup_node = contact_init(personal->backup_node);
        contact_copy(personal->backup_node, personal->persn_info);
    }
    else{
        personal->extern_node = contact_init(personal->extern_node);
        strcpy(personal->extern_node->node_id, bootid);
        strcpy(personal->extern_node->network, net);
        strcpy(personal->extern_node->tcp_port, bootTCP);
        strcpy(personal->extern_node->node_addr, bootIP);
        personal->backup_node = contact_init(personal->backup_node);

        //NEW message
        sprintf(msg, "%s %s %s %s\n", new_str, personal->persn_info->node_id, personal->persn_info->node_addr, personal->persn_info->tcp_port);
        //Sends NEW message , receives EXTERN response and updates backup node
        if(send_tcp(personal, personal->extern_node, msg)){
            printf("Failed to join the network\n");
            return 1;
        }
        if (strcmp(personal->persn_info->node_id, personal->backup_node->node_id) == 0) {
            personal->anchorflag = 1;
        }
    }
    printf("Successfuly joined a network\n"); 
    printf("Your node has the following info:\n\n");
    
    printf("ID: %s\nNetwork: %s\nAdress: %s\nPort: %s\n",  
    personal->persn_info->node_id, 
    personal->persn_info->network,
    personal->persn_info->node_addr, 
    personal->persn_info->tcp_port);
       
    return 0;
}//djoin


 contentList_t *create(contentList_t *contents, char *name){ // name size <= 100, alphanumeric chars only

    if(check_name(name) == 1){
        printf("Failed to create a content. Name was not valid\n");
        return contents;
    }
    
    int name_size = strlen(name) + 1; 
    contentList_t *new_node = (contentList_t*)calloc(1, sizeof(contentList_t));
    new_node->string= (char*)calloc(name_size, sizeof(char));
    strcpy(new_node->string, name);

    if(contents == NULL){ // if true, we are creating the first element of the list

        contents = new_node;
        contents->next = NULL;
        
        printf("Content created at the beginning of the list: %s\n", name);
        return contents;
    }

    contentList_t *listptr; // pointer to go through the list
    
    listptr = contents;

	while(listptr->next != NULL){ //reach the last element of the list
		listptr = listptr->next;		
	}
		
	listptr->next = new_node;
	printf("\n%s was inserted in the list\n\n", name);
    return contents;
}//create


 contentList_t *delete(contentList_t *contents, char *name) {
    
    if(check_name(name) == 1){
        printf("Error: Invalid name\n");
        return contents;
    }

    if(contents == NULL){
        printf("Error: No contents available\n");
        return contents;
    }

    contentList_t *listptr; // pointer to go through the list
    contentList_t *aux; // auxiliary pointer to delete elements of the lists
    listptr = contents;

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

    while(listptr != NULL){

        if(listptr->next == NULL && strcmp(listptr->string, name) != 0){            
            printf("Error: The name was not found\n");
            return contents;
        }
        
        else if(strcmp(listptr->string, name) == 0){ 

            aux = listptr;
            listptr = listptr->next;
            free(aux->string);
            free(aux);
            printf("The deletion of %s was successful\n", name);
            if(listptr == NULL){
				printf("\nThe list of contents is now empty\n\n");
			}
            return contents;
        }
        
        else if(strcmp(listptr->next->string, name) == 0){
            
            aux = listptr->next;
            listptr->next = listptr->next->next;
            free(aux->string);
            free(aux);
            printf("The deletion of %s was successful\n", name);
            return contents;
        }
        else listptr = listptr->next;
    }    
    return contents;
}//delete


 int get(struct personal_node *personal, char *dest_id, char *name){

    int success_flag = 0;
    if(strcmp(personal->persn_info->network, "") == 0){
        printf("Error: You are not inside a network\n");
        return ++success_flag;
    }
    
    /* check if the id is valid */

    if(check_id(dest_id) == 1 || strcmp(dest_id, personal->persn_info->node_id) == 0){
        printf("Error: Invalid id\n");
        return ++success_flag;
    }

    /* check the expedition table */
    
    int search_id = atoi(dest_id);  //integer to search the table of intern neighbors
    
    int gateway = -1;                      //id of neighbor to send something to the destination
    char query_buffer[MAX_MSG_LENGTH];     //buffer for the "QUERY" message
    
    memset(query_buffer, 0, sizeof(query_buffer));

    sprintf(query_buffer,"QUERY %s %s %s\n", dest_id, personal->persn_info->node_id, name);    

    if(personal->route_tab[search_id] == search_id){ // destination is equal to the neighbor, but what type of neighbor?

        if(strcmp(personal->extern_node->node_id, dest_id) == 0 || strcmp(personal->backup_node->node_id, dest_id) == 0 ){ // extern/backup neighbor
            printf("Sending: %s\n", query_buffer);
            send_tcp(personal, personal->extern_node, query_buffer);                        
        }
        else{
            printf("Sending: %s\n", query_buffer);
            send_tcp(personal, personal->neighbrs[search_id], query_buffer); // intern neighbor 
        }                               
    }
    else if(personal->route_tab[search_id] != -1){ // destination is initialized and is different from the neighbor

        gateway = personal->route_tab[search_id];
        if(gateway == atoi(personal->extern_node->node_id) || gateway == atoi(personal->backup_node->node_id)){
            printf("Sending: %s\n", query_buffer);
            send_tcp(personal, personal->extern_node, query_buffer);
        }
        else{
            printf("Sending: %s\n", query_buffer);
            send_tcp(personal, personal->neighbrs[gateway], query_buffer);
        }        
    }
    else{ //destination is not a neighbor and it is not initialized
        printf("Sending: %s\n", query_buffer);
        send_tcp(personal, personal->extern_node, query_buffer);

        for(int iter = 0; iter < NODE_IDS; iter++){

            if(personal->neighbrs[iter] != NULL){
                printf("Sending: %s\n", query_buffer);
                send_tcp(personal, personal->neighbrs[iter], query_buffer);                
            }
        }
    }
    return 0;
}//get


 int show_topology(struct personal_node *personal){
    
    int success_flag = 0;
    int printed_interns = 0;

    if(strcmp(personal->persn_info->network, "") != 0){
        printf("\n\nYour node:\n\n");        
        printf("ID: %s\nNetwork: %s\nAdress: %s\nPort: %s\n\n",  
        personal->persn_info->node_id, 
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
        printf("ID: %s\nNetwork: %s\nAdress: %s\nPort: %s\n\n",  
        personal->extern_node->node_id, 
        personal->extern_node->network,
        personal->extern_node->node_addr, 
        personal->extern_node->tcp_port);
        
        
    }
    else {
        printf("\nThere is no external neighbor!\n");
    }
    if (personal->backup_node != NULL) {
        printf("\n\nBackup neighbor: \n");                
        printf("ID: %s\nNetwork: %s\nAdress: %s\nPort: %s\n\n", 
        personal->backup_node->node_id, 
        personal->backup_node->network, 
        personal->backup_node->node_addr, 
        personal->backup_node->tcp_port);
        
        
    }   
    else {
        printf("\nThere is no backup neighbor!\n");   
    }

    int iter;
    
    
    if(personal->neighbrs == NULL){
        printf("\nThere are no internal neighbors\n");        
    }
    else{
        printf("\n\nPrinting internal neighbors: \n\n");

        if(personal->extern_node == NULL && personal->backup_node == NULL){
            
            for(iter = 0; iter < NODE_IDS; iter++){

                if(personal->neighbrs[iter] != NULL){

                    printf("ID: %s\nNetwork: %s\nAdress: %s\nPort: %s\n\n",
                    personal->neighbrs[iter]->node_id,
                    personal->neighbrs[iter]->network,
                    personal->neighbrs[iter]->node_addr,
                    personal->neighbrs[iter]->tcp_port);

                    printed_interns++;
                }//if            
            }//for
        }
        else{
            for(iter = 0; iter < NODE_IDS; iter++){

                if(iter != atoi(personal->extern_node->node_id) &&
                iter != atoi(personal->backup_node->node_id) &&
                iter != atoi(personal->persn_info->node_id)){

                    if(personal->neighbrs[iter] != NULL){

                        printf("ID: %s\nNetwork: %s\nAdress: %s\nPort: %s\n\n",
                        personal->neighbrs[iter]->node_id,
                        personal->neighbrs[iter]->network,
                        personal->neighbrs[iter]->node_addr,
                        personal->neighbrs[iter]->tcp_port);

                        printed_interns++;
                    }//if
                }//if
            }//for
        }        
    }
    
    if(printed_interns == 0){

        printf("There are no internal neighbors\n");
        return ++success_flag;
    }
    return success_flag;
} // show_topology

 int show_names(contentList_t **contents){

    int printed_names = 0; // counter of printed names
    int success_flag = 0; //success signal for the main function

    if(*contents == NULL){
        printf("The node has no contents to print\n");
        return ++success_flag;
    }

    printf("Printing names...\n\n");
    contentList_t *listptr; // pointer to go through the list

    for(listptr = *contents; listptr != NULL; listptr = listptr->next){
        printf("%s\n", listptr->string);
        printed_names++;
    }
    printf("\nNumber of printed names: %d\n", printed_names);
    return success_flag;
}//show_names

 int show_routing(int *routing_tab){

    int iter;
    int printed_pairs = 0;
    int success_flag = 0;

    printf("\nPrinting routing table as:\n\n");
    
    printf("Neighbour ------------> Destination \n");    

    for(iter = 0; iter < NODE_IDS; iter++){

        if(routing_tab[iter] != -1){

            printf("%02d ----------------> %02d\n", routing_tab[iter], iter);
            printed_pairs++;
        }
    }
    if(printed_pairs == 0){
        printf("\nThe routing table is empty\n");
        return ++success_flag;
    }
    else{
        printf("\nNumber of entries showed: %d\n", printed_pairs);
        return success_flag;
    }
}//show_routing


 int clear_routing(int *routing_tab){

    int iter;
    int cleared_pairs = 0;
    int success_flag = 0;

    printf("\nClearing routing table...\n");

    for(iter = 0; iter < NODE_IDS; iter++){

        if(routing_tab[iter] != -1){ // if this entry was initialized, reset it
            
            routing_tab[iter] = -1;
            cleared_pairs++;
        }
    }
    if(cleared_pairs == 0){
        printf("\nThe routing table is empty\n");
        return ++success_flag;
    }
    else{
        printf("\nNumber of cleared entries: %d\n", cleared_pairs);
        return success_flag;
    }
}//clear_routing

 contentList_t *clear_names(contentList_t *contents){

    int cleared_names = 0;  //number of names cleared
  
    if(contents == NULL){
        printf("No contents to clear in this node\n");
        return contents;
    }
    printf("\nClearing names...\n");

    contentList_t *listptr; // pointer to go through the list
    contentList_t *aux;     // auxiliary pointer to delete elements in the lists
    listptr = contents;
    while(listptr != NULL){

        aux = listptr;
        listptr = listptr->next;
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
        sprintf(msg, "%s %s %s\n", unreg_str, personal->persn_info->network, personal->persn_info->node_id);
        send_udp(personal, msg);
        udp_flag = 0;
    }
    
    int i = 0;
    for (i = 3; i <= personal->max_fd; i++) {
        if (FD_ISSET(i, &personal->crr_scks) && (i != personal->server_fd)) {
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
        printf("To use the application, insert one of the following commands:\n\n");
        printf("djoin [desired network] [desired ID] [boot ID] [boot IPv4 address] [boot TCP port]\n");
        printf("join [desired network] [desired ID]\n");
        printf("create [name]\n");
        printf("delete [name]\n");
        printf("get [ID of node where the desired name is] [the desired name]\n");
        printf("st (show topology)\n");
        printf("sn (show names)\n");
        printf("sr (show routing table)\n");
        printf("cr (clear routing)\n");
        printf("cn (clear names)\n");
        printf("leave (to leave the network)\n");
        printf("exit (to exit the application)\n");
        return;
}//help_menu

/*--------------End user interface functions--------------*/