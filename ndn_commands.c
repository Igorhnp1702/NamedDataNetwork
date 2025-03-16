/************************************************************************************************
 * ndn_io.c
 *
 * Authors:
 *
 * -Igor Paruque ist1102901
 * -Mónica Ferreira ist106520
 *
 * Description: source code for the commands
 ***********************************************************************************************/

 #define _XOPEN_SOURCE 600 //!REMOVE BEFORE COMPILING

// general purpose libraries
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
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




/*------------------------------------------Argument checking functions------------------------------------------*/

int is_valid_ip(const char *ip) {
    struct sockaddr_in sa;
    return inet_pton(AF_INET, ip, &(sa.sin_addr)) != 0;  // IPv4 check
}

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
    
    if(atoi(port_str) < 0 || 65535 < atoi(port_str) ) return ++flag;
    
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

/*------------------------------------------User inteface funcions------------------------------------------*/

void select_cmd(struct personal_node *personal, char *input){ 
    
    char net_num[MAX_NET_CHARS];      memset(net_num, 0, sizeof(net_num));     // network number passed in the commands    
    char tcp[MAX_TCP_UDP_CHARS];      memset(tcp, 0, sizeof(tcp));             // a tcp port passed in the commands
    char udp[MAX_TCP_UDP_CHARS];      memset(udp, 0, sizeof(udp));             // the udp port of the node server passed in the commands    

    char cmd_str1[MAX_MSG_CMD_SIZE];  memset(cmd_str1, 0, sizeof(cmd_str1));   // should be enough for the first string of the command
    char cmd_str2[MAX_MSG_CMD_SIZE];  memset(cmd_str2, 0, sizeof(cmd_str2));   // should be enough for the first string of the command
    char cmd_str3[MAX_MSG_CMD_SIZE];  memset(cmd_str3, 0, sizeof(cmd_str3));   // should be enough for the first string of the command
    char address[MAX_ADDRESS_SIZE];   memset(address, 0, sizeof(address));     // an IPv4 address passed in the commands
    char content[MAX_OBJECT_NAME];    memset(content, 0, sizeof(content));     // the name of a content passed in the commands

    // parse the first string

    sscanf(input, "%s %s %s", cmd_str1, cmd_str2, cmd_str3);

    // try to read and execute the desired command

    if(strcmp(cmd_str1, join_str) == 0 || strcmp(cmd_str1, join_str_short) == 0){

        if(personal->network_flag == 1){
            printf("The node already belongs to a network. Command ignored\n");
            return;
        }       
        else if(sscanf(input, "%*s %s", net_num) == 1){
            
            printf("Executing %s...\n\n", join_str);
            join(personal, net_num); //join a network and register the personal node in the server
            return;
        }                        
        else{
            printf("Failed to read arguments of %s\n", join_str); 
            return;
        } 
    }//if join or j

    else if(strcmp(cmd_str1, direct_join_str_short) == 0){

        if(personal->network_flag == 1){
            printf("The node already belongs to a network. Command ignored\n");
            return;
        }
        else if(sscanf(input, "%*s %s %s", address, tcp) == 2){
            
            printf("Executing %s %s...\n\n", direct_str, join_str);
            djoin(personal, address, tcp); // join a network without registration
            return;
        }       
        else{
            printf("Failed to read arguments of %s %s\n", direct_str, join_str); 
            return;
        } 
    }//else if dj

    else if((strcmp(cmd_str1, direct_str) == 0 && strcmp(cmd_str2, join_str) == 0)){

        if(personal->network_flag == 1){
            printf("The node already belongs to a network. Command ignored\n");
            return;
        }
        else if(sscanf(input, "%*s %*s %s %s", address, tcp) == 2){
            
            printf("Executing %s %s...\n\n", direct_str, join_str);
            djoin(personal, address, tcp); // join a network without registration
            return;
        }       
        else{
            printf("Failed to read arguments of %s %s\n", direct_str, join_str); 
            return;
        } 
    }//else if direct join

    // else if(strcmp(cmd_str1, create_str) == 0 || strcmp(cmd_str1, create_str_short) == 0){

    //     if(sscanf(input, "%*s %s", content) == 1){
            
    //         printf("Executing %s...\n\n", create_str);
    //         personal->queue_ptr = create(personal->queue_ptr, content); // create and store a content inside the personal node           
    //         return; 
    //     }
    //     else{
    //         printf("Failed to read arguments of %s\n", create_str); 
    //         return;
    //     } 
    // }//else if create or c

    // else if(strcmp(cmd_str1, delete_str) == 0 || strcmp(cmd_str1, delete_str_short) == 0){

    //     if(sscanf(input, "%*s %s", content) == 1){
            
    //         printf("Executing %s...\n\n", delete_str);
    //         personal->queue_ptr = delete(personal->queue_ptr, content); // delete a content inside the personal node
    //         return;            
    //     }
    //     else{
    //         printf("Failed to read arguments of %s\n", delete_str); 
    //         return;
    //     } 
    // }//else if delete or d

    // else if(strcmp(cmd_str1, retrieve_str) == 0 || strcmp(cmd_str1, retrieve_str_short) == 0){
        
    //     if(sscanf(input, "%*s %s", content) == 1){
            
    //         printf("Executing %s...\n\n", retrieve_str);
    //         retrieve(personal, content); // search and copy a content from another node        
    //         return; 
    //     }
    //     else{
    //         printf("Failed to read arguments of %s\n", retrieve_str); 
    //         return;
    //     } 
    // }//else if retreive or r

    else if(strcmp(cmd_str1, show_topology_str_short) == 0){
        
        printf("Executing %s %s...\n\n", show_str, topology_str);
        show_topology(personal);  // show topology of the personal node
        return;

    }// else if st

    else if(strcmp(cmd_str1, show_str) == 0){
        
        if(strcmp(cmd_str2, topology_str) == 0){
            printf("Executing %s %s...\n\n", show_str, topology_str);
            show_topology(personal);  // show topology of the personal node
            return;
        }

        // if(strcmp(cmd_str2, names_str) == 0){
        //     printf("Executing %s %s...\n\n", show_str, names_str);
        //     show_names(&personal->queue_ptr); // show names of the personal node
        //     return;
        // }

        // if(strcmp(cmd_str2, interest_str) == 0 && strcmp(cmd_str3, table_str) == 0){

        //     printf("Executing %s %s %s...\n\n", show_str, interest_str, table_str);
        //     show_names(&personal->queue_ptr); // show names of the personal node
        //     return;
        // }
        
    }// else if show commands long

    // else if(strcmp(cmd_str1, show_names_str_short) == 0){
        
    //     printf("Executing %s...\n\n", cmd_str1);
    //     show_names(&personal->queue_ptr); // show names of the personal node
    //     return;

    // }//else if sn

    // else if(strcmp(cmd_str1, clear_names_str_short) == 0){

    //     printf("Executing %s...\n\n", cmd_str1);
    //     personal->contents = clear_names(personal->contents);  // clear the contents table of the personal node            
    //     return;

    // }// else if cn

    // else if(strcmp(cmd_str1, show_interest_table_str_short) == 0){

    //     if(strcmp(personal->persn_info->network, "") == 0)
        
    //     printf("Executing %s %s %s...\n\n", show_str, interest_str, table_str);
    //     show_routing(personal->route_tab); // show routing table of the personal node
    //     return;

    // }//else if si
    
    else if(strcmp(cmd_str1, leave_str) == 0 || strcmp(cmd_str1, leave_str_short) == 0){

        //remove the personal node from the network
        // reset the struct in order to give the possibility to join or djoin again

        printf("Executing %s...\n\n", leave_str);
        if(personal->network_flag == 0){
            printf("The node is already without a network.\n");
            return;
        }
        leave(personal);
        return;

    }//else if leave or l

    else if(strcmp(cmd_str1, exit_str) == 0 || strcmp(cmd_str1, exit_str_short) == 0){

        // remove the personal node from the network  
        // free the memory, close the fds and get out of the program     
        
        printf("Executing %s...\n\n", exit_str);
        leave(personal);                
        free_contact(personal->persn_info);

        // if(personal->queue_ptr != NULL){
        
        //     objectQueue_t *queue_ptr; // pointer to go through the list
        //     objectQueue_t *aux;     // auxiliary pointer to delete elements in the lists
        //     queue_ptr = personal->queue_ptr;
            
        //     while(queue_ptr != NULL){

        //         aux = queue_ptr;
        //         queue_ptr = queue_ptr->next;
        //         free(aux->string);
        //         free(aux);
                
        //     }                            
        // }
            
        free(personal);
        exit(0);

    }//else if exit

    else if(strcmp(cmd_str1, help_str) == 0 || strcmp(cmd_str1, help_str_short) == 0) {
        
        printf("Executing %s...\n\n", help_str);
        help_menu();
        return;
    }
    
    else{
        printf("Error: Invalid command\n");
        printf("Type h or help to get a list of valid commands\n\n");
        return;
    }
    
}//select_cmd()


//-------------------------------------------- user commands --------------------------------------------//

int join(struct personal_node *personal, char *net) { 

    int success_flag = 0;
    char picked_ip[51];
    char picked_tcp[6];
    nodesLinkedlist_t *serverlist = NULL;
    serverlist = Listinit(serverlist);
    nodeinfo_t *aux = NULL;
    aux = contact_init(aux);
        
    // check the arguments

    if(personal->network_flag == 1){
        printf("Error in join: The node is already inside a network. Command ignored\n");
        return ++success_flag;
    }
    if(check_net(net) == 1){
        printf("Error in join: The network is invalid. Command ignored\n");
        printf("Please insert a three digit network from 000 to 999\n");
        return ++success_flag;
    }
    
    // inquire the server about the nodes in the network
            
    char *nodeslist = server_inquiry(personal->udp_address, personal->udp_port, net);

    if(nodeslist == NULL){
        printf("Error in join: failed to inquire the server\n\n");
        return ++success_flag;
    }
    
    const char delim[2] = "\n";
    char *token;
    int n_nodes = 0;
    
    // print the response from the node server, line by line, and count the amount of nodes
    // copy the nodes to a temporary list

    printf("Response from the node server:\n\n");

    token = strtok(nodeslist, delim);
    printf("%s\n\n", token); // first line (nodeslist net\n)
    while (token != NULL) {                                         
        
        token = strtok(NULL, delim);
        
        if(token == NULL) break;
        if((sscanf(token, "%s %s", aux->node_addr, aux->tcp_port)) != 2){
            
            printf("Skipped a line in nodeslist\n");
        }
        else{
            printf("%s\n", token);
            serverlist = insertnode(serverlist, aux);            
            n_nodes++;
        }                
    }

    

    if(n_nodes > 0){ // if the network is not empty, pick the first node
        
        printf("Number of nodes reported in the network: %d\n\n", n_nodes);

        strcpy(picked_ip, serverlist->node->node_addr);
        strcpy(picked_tcp, serverlist->node->tcp_port);
                                             
        // try to connect

        if(djoin(personal, picked_ip, picked_tcp) == 1) {
            printf("Error in join: Failed to connect to a node in the desired network\n");
            return ++success_flag;
        }
    }
    else{ // empty network, 
        
        printf("The network is empty. You are the first to join\n");

        memset(personal->persn_info->network, 0, 4*sizeof(char));
        if(djoin(personal, "0.0.0.0", personal->persn_info->tcp_port) == 1) {
            printf("Error in join: Failed to enter the network\n");
            return ++success_flag;
        }
    }

    // try to register the node

    if(strcmp(node_reg(personal->udp_address, personal->udp_port, personal->persn_info->node_addr, personal->persn_info->tcp_port, net), "1") != 0){
        
        printf("Error in join: Failed to register the node\n");
        node_unreg(personal->udp_address, personal->udp_port, personal->persn_info->node_addr, personal->persn_info->tcp_port, net);
        return ++success_flag;
    }
    personal->join_flag = 1; 
    free(nodeslist);
    free_contact(aux);
    serverlist = clearlist(serverlist);
     
    return success_flag;

}//join


int djoin(struct personal_node *personal, char *connectIP, char *connectTCP){

    char buffer[MAX_MSG_LENGTH];   memset(buffer, 0, sizeof(buffer));
    int success_flag = 0;

    if(personal->network_flag == 1){
        printf("Error in direct join: The node is already inside a network. Command ignored\n");
        return ++success_flag;
    }        
           
    if(check_ports(connectTCP) == 1){
        printf("Error in direct join: The boot TCP port is invalid. Command ignored\n");
        printf("Please insert a 1 to 5 digit TCP port from 0 to 65536");
        return ++success_flag;
    }
    

    int errflag;                  // flag to check for errors in function calls    
    struct addrinfo *srv_result;  // list of address structures
    struct addrinfo srv_criteria; // necessary criteria to select the address structures from the list
    ssize_t nread;               // number of bytes read from a read operation
    ssize_t nleft;               // number of bytes left to fill the buffer capacity
    char *scan_ptr;
    nleft = MAX_MSG_LENGTH;

    struct timeval timeout;
    timeout.tv_sec = 5;  // 5-second timeout
    timeout.tv_usec = 0;
    
    personal->server_fd = socket(AF_INET, SOCK_STREAM, 0); // personal server socket for the intern nodes
    if (personal->server_fd < 0) {
        printf("Error in socket()\n");
        printf("This node cannot belong to a network.\n");
        printf("Because of this, the process will be terminated\n");
        // free and close everything
        return 1;        
    }
    setsockopt(personal->server_fd, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout));
    FD_SET(personal->server_fd, &personal->crr_scks);           // add server socket to FD set

    personal->max_fd = personal->server_fd;

    memset(&srv_criteria, 0, sizeof(srv_criteria)); // set all bytes inside "criteria" to zero
    srv_criteria.ai_family = AF_INET;               // family of IPv4 addresses
    srv_criteria.ai_socktype = SOCK_STREAM;         // TCP socket
    srv_criteria.ai_flags = AI_PASSIVE;             // passive socket that waits for requests

    // pass the information in "criteria" to "srv_result" 

    errflag = getaddrinfo(NULL, personal->persn_info->tcp_port, &srv_criteria, &srv_result);
    if (errflag != 0) {
        printf("Error in getaddrinfo()\n");
        printf("This node cannot belong to a network.\n");
        printf("Because of this, the process will be terminated\n");
        // free and close everything
        return 1;
    }

    // bind the address in "srv_result" to the personal socket

    errflag = bind(personal->server_fd, srv_result->ai_addr, srv_result->ai_addrlen);
    if (errflag == -1) {
        printf("Error in bind()\n");
        printf("This node cannot belong to a network.\n");
        printf("Because of this, the process will be terminated\n");
        // free and close everything
        return 1;
    }

    freeaddrinfo(srv_result);
    // Set the personal socket to listen to requests. Only 6 requests at a time

    errflag = listen(personal->server_fd, MAX_QUEUE_LENGTH_TCP); // the second argument refers to the number of pending connections allowed in the queue
    if (errflag == -1) {
        printf("Error in listen()\n");
        printf("This node cannot belong to a network.\n");
        printf("Because of this, the process will be terminated\n");
        // free and close everything
        return 1;
    }


    if (strcmp(connectIP, "0.0.0.0") == 0) {  //First node of the network
        personal->anchorflag = 1;             //!confirmar
        personal->extern_node = contact_init(personal->extern_node);        
        personal->backup_node = contact_init(personal->backup_node);
        
    }
    else{
                
        //Sends ENTRY message
        if(send_entry(&(personal->client_fd), personal->persn_info->node_addr, personal->persn_info->tcp_port, 
                      connectIP, connectTCP) == NULL){
                        
            printf("Error in direct join: Failed to join the network\n");
            // unregister from the node server, if necessary
            // clear all data
            //return 1;
            return 1;
        }
        else{
            printf("\nENTRY message delivered to %s | %s\n", connectIP, connectTCP);
            printf("Updating external neighbor\n");
        }

        personal->extern_node = contact_init(personal->extern_node);                
        strcpy(personal->extern_node->tcp_port, connectTCP);
        strcpy(personal->extern_node->node_addr, connectIP);

        //Receives SAFE message

        personal->backup_node = contact_init(personal->backup_node);
        
        errno = 0;
        scan_ptr = buffer;

        while (((nread = read(personal->client_fd, scan_ptr, MAX_MSG_LENGTH - 1)) > 0) && (nread < nleft)) { //read msg to buffer
            scan_ptr += nread - 1;
            nleft = MAX_MSG_LENGTH;
            if (*(scan_ptr) == '\n') break;
        }

        if (nread == -1) {
            printf("Error in djoin: read returned %s while reading SAFE message. The message was incomplete.\n", strerror(errno));
            // unregister from the node server, if necessary
            // clear all data
            //return 1;
        }
        
        sscanf(buffer, "%*s %s %s", personal->backup_node->node_addr, personal->backup_node->tcp_port);

        // check if it's an anchor
        if (strcmp(personal->persn_info->node_addr, personal->backup_node->node_addr) == 0 && 
            strcmp(personal->persn_info->tcp_port, personal->backup_node->tcp_port) == 0) {
            
            personal->anchorflag = 1;
        }
    }
    printf("Successfuly joined a network\n"); 
    printf("Your node has the following info:\n\n");
    personal->network_flag = 1;
    
    if(personal->join_flag == 1){
        printf("Network: %s\nAdress: %s\nPort: %s\n",       
        personal->persn_info->network,
        personal->persn_info->node_addr, 
        personal->persn_info->tcp_port);
        return 0;    
    }
    printf("Adress: %s\nPort: %s\n",     
    personal->persn_info->node_addr, 
    personal->persn_info->tcp_port);
    
    return 0;
}//djoin


// objectQueue_t *create(objectQueue_t *queue_ptr, char *name){ // name size <= 100, alphanumeric chars only

//     if(check_name(name) == 1){
//         printf("Error in create: invalid name\n");
//         return contents;
//     }
    
//     int name_size = strlen(name) + 1; 
//     queueBlock_t *new_node = (queueBlock_t*)calloc(1, sizeof(queueBlock_t));
//     new_node->name = (char*)calloc(name_size, sizeof(char));
//     strcpy(new_node->name, name);

//     if(queue_ptr == NULL){ // if true, we are creating the first element of the list

//         queue_ptr = new_node;
//         contents->next = NULL;
        
//         printf("Content created at the beginning of the list: %s\n", name);
//         return contents;
//     }

//     objectQueue_t *queue_ptr; // pointer to go through the list
    
//     queue_ptr = contents;

//     while(queue_ptr->next != NULL){ //reach the last element of the list
//         queue_ptr = queue_ptr->next;		
//     }
        
//     queue_ptr->next = new_node;
//     printf("\n%s was inserted in the list\n\n", name);
//     return queue_ptr;
// }//create


// objectQueue_t *delete(objectQueue_t *contents, char *name) {
    
//     if(check_name(name) == 1){
//         printf("Error in delete: Invalid name\n");
//         return contents;
//     }

//     if(contents == NULL){
//         printf("Error in delete: No contents available\n");
//         return contents;
//     }

//     objectQueue_t *queue_ptr; // pointer to go through the list
//     objectQueue_t *aux; // auxiliary pointer to delete elements of the lists
//     queue_ptr = contents;

//     if(strcmp(contents->string, name) == 0){ //delete at the head of the list

//         aux = contents;
//         contents = contents->next;
//         free(aux->string);
//         free(aux);
//         printf("%s was successfuly deleted\n", name);
//         if(contents == NULL){
//             printf("\nThe list of contents is now empty\n\n");
//         }		
//         return contents;
//     }

//     while(queue_ptr != NULL){

//         if(queue_ptr->next == NULL && strcmp(queue_ptr->string, name) != 0){            
//             printf("Error in delete: The name was not found\n");
//             return contents;
//         }
        
//         else if(strcmp(queue_ptr->string, name) == 0){ 

//             aux = queue_ptr;
//             queue_ptr = queue_ptr->next;
//             free(aux->string);
//             free(aux);
//             printf("The deletion of %s was successful\n", name);
//             if(queue_ptr == NULL){
//                 printf("\nThe list of contents is now empty\n\n");
//             }
//             return contents;
//         }
        
//         else if(strcmp(queue_ptr->next->string, name) == 0){
            
//             aux = queue_ptr->next;
//             queue_ptr->next = queue_ptr->next->next;
//             free(aux->string);
//             free(aux);
//             printf("The deletion of %s was successful\n", name);
//             return contents;
//         }
//         else queue_ptr = queue_ptr->next;
//     }    
//     return contents;
// }//delete


// int retrieve(struct personal_node *personal, char *name){

//     int success_flag = 0;
//     if(strcmp(personal->persn_info->network, "") == 0){
//         printf("Error in retrieve: You are not inside a network\n");
//         return ++success_flag;
//     }
    
//     /* check the expedition table */
    
//     int search_id = atoi(dest_id);  //integer to search the table of intern neighbors
    
//     int gateway = -1;                      //id of neighbor to send something to the destination
//     char retrieve_buffer[MAX_MSG_LENGTH];     //buffer for the "QUERY" message
    
//     memset(retrieve_buffer, 0, sizeof(retrieve_buffer));

//     sprintf(retrieve_buffer,"RETRIEVE %s %s\n", name);    

//     if(personal->route_tab[search_id] == search_id){ // destination is equal to the neighbor, but what type of neighbor?

//         if(strcmp(personal->extern_node->node_id, dest_id) == 0 || strcmp(personal->backup_node->node_id, dest_id) == 0 ){ // extern/backup neighbor
//             printf("Sending: %s\n", retrieve_buffer);
//             send_tcp(personal, personal->extern_node, retrieve_buffer);                        
//         }
//         else{
//             printf("Sending: %s\n", retrieve_buffer);
//             send_tcp(personal, personal->internals_array[search_id], retrieve_buffer); // intern neighbor 
//         }                               
//     }
//     else if(personal->route_tab[search_id] != -1){ // destination is initialized and is different from the neighbor

//         gateway = personal->route_tab[search_id];
//         if(gateway == atoi(personal->extern_node->node_id) || gateway == atoi(personal->backup_node->node_id)){
//             printf("Sending: %s\n", retrieve_buffer);
//             send_tcp(personal, personal->extern_node, retrieve_buffer);
//         }
//         else{
//             printf("Sending: %s\n", retrieve_buffer);
//             send_tcp(personal, personal->internals_array[gateway], retrieve_buffer);
//         }        
//     }
//     else{ //destination is not a neighbor and it is not initialized
//         printf("Sending: %s\n", retrieve_buffer);
//         send_tcp(personal, personal->extern_node, retrieve_buffer);

//         for(int iter = 0; iter < MAX_INTERNALS; iter++){

//             if(personal->internals_array[iter] != NULL){
//                 printf("Sending: %s\n", retrieve_buffer);
//                 send_tcp(personal, personal->internals_array[iter], retrieve_buffer);                
//             }
//         }
//     }
//     return 0;
// }//retrieve


int show_topology(struct personal_node *personal){
    
    int success_flag = 0;
    int printed_interns = 0;

    nodesLinkedlist_t *aux;

    if(personal == NULL){
        printf("The node is not initialized\n");
        return ++success_flag;
    }

    if(personal->persn_info == NULL){
        printf("The node's information is not initialized\n");
        return ++success_flag;
    }

    if(personal->network_flag != 0){
        printf("\n\nYour node:\n\n");        
        printf("Network: %s\nAdress: %s\nPort: %s\n\n",           
        personal->persn_info->network,
        personal->persn_info->node_addr, 
        personal->persn_info->tcp_port);
        
    }
    else{
        printf("Error in show topology: You are not inside a network\n");
        return ++success_flag;
    }
    if(personal->extern_node == NULL){
        printf("The external node's information is not initialized\n");
        return ++success_flag;
    }
    if (strcmp(personal->extern_node->network, "") != 0) {
        printf("\n\nExternal neighbor: \n");        
        printf("Network: %s\nAdress: %s\nPort: %s\n\n",          
        personal->extern_node->network,
        personal->extern_node->node_addr, 
        personal->extern_node->tcp_port);
        
        
    }
    else {
        printf("\nThere is no external neighbor!\n");
    }

    if (strcmp(personal->backup_node->network, "") != 0) {
        printf("\n\nBackup neighbor: \n");                
        printf("Network: %s\nAdress: %s\nPort: %s\n\n",         
        personal->backup_node->network, 
        personal->backup_node->node_addr, 
        personal->backup_node->tcp_port);
        
        
    }   
    else {
        printf("\nThere is no backup neighbor!\n");   
    }    
        
    if(personal->internals_list == NULL){
        if(strcmp(personal->internals_list->node->network, "") == 0){
            
            printf("\nThere are no internal neighbors\n");        
        }
        
    }
    else{
        printf("\n\nPrinting internal neighbors: \n\n");        
            
        aux = personal->internals_list;
        while(aux != NULL){
                            
            printf("Network: %s\nAdress: %s\nPort: %s\n\n",                    
                aux->node->network,
                aux->node->node_addr,
                aux->node->tcp_port);

            printed_interns++;                        
            aux = aux->next;
        }
        printf("Number of internals: %d\n", printed_interns);
    }
        
    return success_flag;
} // show_topology


int leave(struct personal_node *personal) {
    
    // if(personal->network_flag == 0){        
    //     //personal = reset_personal(personal);
    //     int i = 0;
    //     for (i = 3; i <= personal->max_fd; i++) {
    //         if (FD_ISSET(i, &personal->crr_scks)) {
    //             FD_CLR(i, &personal->crr_scks);
    //             close(i);
    //         }
    //     }        
    //     personal = reset_personal(personal); 
    //     return 1;
    // }    
    
    if(personal->join_flag == 1){
        if ((node_unreg(personal->udp_address, personal->udp_port, personal->persn_info->node_addr, 
            personal->persn_info->tcp_port, personal->persn_info->network)) == NULL){

            printf("Error in leave: Failed to unregister the node. The connections are still open.\n");
            return 1;

        }
        // check return value of node_unreg
        personal->join_flag = 0;
        strcpy(personal->persn_info->network, "");
    }
    
    // close connections 
    int i = 0;
    for (i = 3; i <= personal->max_fd; i++) {
        if (FD_ISSET(i, &personal->crr_scks)) {
            FD_CLR(i, &personal->crr_scks);
            close(i);
        }
    }
    personal->network_flag = 0;    
    personal = reset_personal(personal);
    printf("leave executed successfully\n");
    return 0;
}//leave

void help_menu() {
        printf("*****************HELP MENU*****************\n\n");
        printf("To use the application, insert one of the following commands:\n");
        printf("Note: curved brackets => valid abreviations; square brackets => arguments\n\n");
        printf("join (j) [desired network] \n");
        printf("direct join (dj) [connect IPv4 address] [connect TCP port]\n");        
        // printf("create (c) [name]\n");
        // printf("delete (dl) [name]\n");
        // printf("retrieve (r) [name]\n");
        printf("show topology (st)\n");
        // printf("show names (sn)\n");
        // printf("show interest table (si)\n");        
        // printf("clear names (cn)\n");
        printf("leave (l)\n");
        printf("exit (x)\n");
        return;
}//help_menu

/*--------------End user interface functions--------------*/