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
#include <signal.h>

// networking libraries
#include <netdb.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <arpa/inet.h>

// project libraries
#include "ndn_commands.h"
#include "ndn_messages.h"

/*------------------------------------------Argument checking functions------------------------------------------*/

int is_valid_ip(const char *ip) { // 1 means success

    if(strlen(ip) > 15) return 0;
    struct sockaddr_in sa;
    return inet_pton(AF_INET, ip, &(sa.sin_addr)) == 1;  // IPv4 check
}

int check_net(char *net){ // 0 means success
    
    int success_flag = 0;
    
    if(strlen(net) != 3){
                
        return ++success_flag;
    }
    while(*net){

        if(*net < '0' || *net > '9'){        
            return ++success_flag;
        }
        net++; 
    }
    return success_flag;
        
}//check_net()

int check_ports(char *port_str){ // 0 means success
    
    int length = strlen(port_str), flag = 0;
    
    if(length > 5) return ++flag;

    while(*port_str){

        if(*port_str < '0' || *port_str > '9'){        
            return ++flag;
        }
        port_str++; 
    }            
    return flag;
}//check_ports()


int check_name(char *name){ // 0 means success

    int flag = 0; //success flag

    if(strlen(name) > 100){        
        return ++flag;
    }
    else return flag;
}//check_name

int check_cache_size(char *size_str){

    if(strlen(size_str) > 9) return 1;
    while(*size_str){

        if(*size_str < '0' || *size_str > '9'){        
            return 1;
        }
        size_str++; 
    }
    return 0;
}

/*------------------------------------------User inteface funcions------------------------------------------*/

void select_cmd(struct personal_node *personal, char *input){ 
    
    char net_num[MAX_NET_CHARS + 1];      memset(net_num, 0, sizeof(net_num));     // network number passed in the commands    
    char tcp[MAX_TCP_UDP_CHARS + 1];      memset(tcp, 0, sizeof(tcp));             // a tcp port passed in the commands
    char udp[MAX_TCP_UDP_CHARS + 1];      memset(udp, 0, sizeof(udp));             // the udp port of the node server passed in the commands    

    char cmd_str1[MAX_MSG_CMD_SIZE];    memset(cmd_str1, 0, sizeof(cmd_str1));   // should be enough for the first string of the command
    char cmd_str2[MAX_MSG_CMD_SIZE];    memset(cmd_str2, 0, sizeof(cmd_str2));   // should be enough for the first string of the command
    char cmd_str3[MAX_MSG_CMD_SIZE];    memset(cmd_str3, 0, sizeof(cmd_str3));   // should be enough for the first string of the command
    char address[MAX_ADDRESS_SIZE];     memset(address, 0, sizeof(address));     // an IPv4 address passed in the commands
    char object_string[MAX_NAME_LENGTH];    memset(object_string, 0, sizeof(object_string));     // the name of a content passed in the commands

    // parse the first string

    sscanf(input, "%8s %8s %8s", cmd_str1, cmd_str2, cmd_str3);

    // try to read and execute the desired command

    if(strcmp(cmd_str1, join_str) == 0 || strcmp(cmd_str1, join_str_short) == 0){

        if(personal->network_flag == 1){
            printf("The node already belongs to a network. Command ignored\n");
            return;
        }       
        else if(sscanf(input, "%*s %4s", net_num) == 1){
            
            printf("Executing %s...\n\n", join_str);

            if(join(personal, net_num) == 1){ //join a network and register the personal node in the server

                leave(personal);
            }
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
        else if(sscanf(input, "%*s %16s %6s", address, tcp) == 2){
            
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
        else if(sscanf(input, "%*s %*s %16s %6s", address, tcp) == 2){
            
            printf("Executing %s %s...\n\n", direct_str, join_str);
            djoin(personal, address, tcp); // join a network without registration
            return;
        }       
        else{
            printf("Failed to read arguments of %s %s\n", direct_str, join_str); 
            return;
        } 
    }//else if direct join

    else if(strcmp(cmd_str1, create_str) == 0 || strcmp(cmd_str1, create_str_short) == 0){

        if(sscanf(input, "%*s %101s", object_string) == 1){
            
            printf("Executing %s...\n\n", create_str);

            if(check_name(object_string) == 1){
                printf("Error in create: The name to be created is too long. Command ignored\n");
                return;
            }

            personal->storage_ptr = create(personal->storage_ptr, object_string); // create and store a content inside the personal node           
            return; 
        }
        else{
            printf("Failed to read arguments of %s\n", create_str); 
            return;
        } 
    }//else if create or c

    else if(strcmp(cmd_str1, delete_str) == 0 || strcmp(cmd_str1, delete_str_short) == 0){

        if(sscanf(input, "%*s %101s", object_string) == 1){
            
            printf("Executing %s...\n\n", delete_str);

            if(check_name(object_string) == 1){
                printf("Error in delete: The name to be deleted is too long. Command ignored\n");
                return;
            }

            personal->storage_ptr = storageDelete(personal->storage_ptr, object_string); // delete a content inside the personal node
            return;            
        }
        else{
            printf("Failed to read arguments of %s\n", delete_str); 
            return;
        } 
    }//else if delete or d

    else if(strcmp(cmd_str1, retrieve_str) == 0 || strcmp(cmd_str1, retrieve_str_short) == 0){                       
        
        if(sscanf(input, "%*s %101s", object_string) == 1){            
            
            
            printf("Executing %s...\n\n", retrieve_str);

            if(check_name(object_string) == 1){
                printf("Error in retrieve: The name to be retrieved is too long. Command ignored\n");
                return;
            }
            
            // VER SE TENHO O OBJETO EM CACHE 
            // SE NAO TIVER SEND INTERST 
            // SE TIVER RETORNAR O OBJETO

            if(personal->network_flag != 1){

                printf("Although you are not inside a network, local object locations will be checked\n\n");
                
                if (queueSearch(personal->queue_ptr, object_string)) {
                
                    printf("Object %s was found in cache\n\n", object_string);
                    return;
                                 
                } 
                else if (storageSearch(personal->storage_ptr, object_string)) {
                    
                    printf("Object %s was found in private storage\n\n", object_string);
                    return;
                }
                printf("The object was not found locally\n");
                return;
            }
            
            if (queueSearch(personal->queue_ptr, object_string)) {
                
                printf("Object %s was found in cache\n\n", object_string);
                return;
                             
            } 
            else if (storageSearch(personal->storage_ptr, object_string)) {
                
                printf("Object %s was found in private storage\n\n", object_string);
                return;
            }            
            else {
                
                printf("Sending INTEREST message to everybody\n\n");
                send_interest(-1, object_string, personal);
            }            
            return; 
        }
        else{
            printf("Failed to read arguments of %s\n", retrieve_str); 
            return;
        } 
    }//else if retrieve or r

    else if(strcmp(cmd_str1, show_topology_str_short) == 0){
        
        printf("Executing %s %s...\n\n", show_str, topology_str);
        show_topology(personal);  // show topology of the personal node
        return;

    }// else if st

    else if(strcmp(cmd_str1, show_names_str_short) == 0){
        
        printf("Executing %s...\n\n", cmd_str1);
        show_names(personal->storage_ptr); // show names of the personal node
        return;

    }//else if sn

    else if(strcmp(cmd_str1, show_cache_str_short) == 0){
        
        printf("Executing %s...\n\n", cmd_str1);
        show_cache(personal->queue_ptr); // show names of the personal node
        return;

    }//else if sn

    else if(strcmp(cmd_str1, show_interest_table_str_short) == 0){

        if(personal->network_flag == 0){
            printf("You are not inside a network. No interests to show\n");

        }
        else{
            printf("Executing %s %s %s...\n\n", show_str, interest_str, table_str);
            show_interest_table(personal->interests_ptr); // show the interest table
        }                        
        return;

    }//else if si

    else if(strcmp(cmd_str1, show_str) == 0){
        
        if(strcmp(cmd_str2, topology_str) == 0){
            printf("Executing %s %s...\n\n", show_str, topology_str);
            show_topology(personal);  // show topology of the personal node
            return;
        }

        if(strcmp(cmd_str2, names_str) == 0){
            printf("Executing %s %s...\n\n", show_str, names_str);
            show_names(personal->storage_ptr); // show names of the personal node
            return;
        }

        if(strcmp(cmd_str2, cache_str) == 0){
            printf("Executing %s %s...\n\n", show_str, cache_str);
            show_cache(personal->queue_ptr); // show names of the personal node
            return;
        }

        if(strcmp(cmd_str2, interest_str) == 0 && strcmp(cmd_str3, table_str) == 0){

            printf("Executing %s %s %s...\n\n", show_str, interest_str, table_str);
            show_interest_table(personal->interests_ptr); // show the interest table
            return;
        }
        
    }// else if show commands long

    else if(strcmp(cmd_str1, clear_cache_str_short) == 0){

        printf("Executing %s...\n\n", cmd_str1);
        personal->queue_ptr = clearQueue(personal->queue_ptr);  // clear the contents table of the personal node            
        return;

    }// else if cn
    
    else if(strcmp(cmd_str1, clear_names_str_short) == 0){

        printf("Executing %s...\n\n", cmd_str1);
        personal->storage_ptr = storageClear(personal->storage_ptr);  // clear the contents table of the personal node            
        return;

    }// else if cn

    else if(strcmp(cmd_str1, clear_str) == 0){
                
        if(strcmp(cmd_str2, names_str) == 0){
            printf("Executing %s %s...\n\n", clear_str, names_str);
            personal->storage_ptr = storageClear(personal->storage_ptr);  // clear the contents table of the personal node            
            return;
        }

        if(strcmp(cmd_str2, cache_str) == 0){
            printf("Executing %s %s...\n\n", clear_str, cache_str);
            personal->queue_ptr = clearQueue(personal->queue_ptr);  // clear the contents table of the personal node            
            return;
        }
               
    }// else if clear commands long
                
    else if(strcmp(cmd_str1, leave_str) == 0 || strcmp(cmd_str1, leave_str_short) == 0){

        //remove the personal node from the network
        // reset the struct in order to give the possibility to join or djoin again

        printf("Executing %s...\n\n", leave_str);
        if(personal->network_flag == 0){
            printf("The node is already without a network.\n");
            return;
        }
        if(leave(personal) == 0) printf("left the network successfully\n");
        
        return;

    }//else if leave or l

    else if(strcmp(cmd_str1, exit_str) == 0 || strcmp(cmd_str1, exit_str_short) == 0){

        // remove the personal node from the network  
        // free the memory, close the fds and get out of the program     
        
        printf("Executing %s...\n\n", exit_str);
        
        personal->exit_flag = 1;        
                
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
    char picked_ip[16];
    char picked_tcp[6];
    char *return_msg;
                
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
    if((strcmp("127.0.0.1", personal->personal_addr)) == 0){
        printf("You cannot use the loopback address to reach the node server. Command ignored\n");
        return ++success_flag;
    }
    
    // inquire the server about the nodes in the network
            
    nodesLinkedlist_t *nodeslist = server_inquiry(personal->udp_address, personal->udp_port, net);

    if(nodeslist == NULL){
        printf("Error in join: failed to inquire the server\n\n");
        return ++success_flag;
    }            

    if(((strcmp(nodeslist->node->node_addr, "")) != 0) && ((strcmp(nodeslist->node->tcp_port, "")) != 0)){ // if the network is not empty, pick the first node
        
        if(((is_valid_ip(nodeslist->node->node_addr)) == 1) && ((check_ports(nodeslist->node->tcp_port)) == 0)){

        
            strcpy(picked_ip, nodeslist->node->node_addr);
            strcpy(picked_tcp, nodeslist->node->tcp_port);
                                                
            // try to connect

            if(djoin(personal, picked_ip, picked_tcp) == 1) {
                printf("Error in join: Failed to connect to a node in the desired network\n");
                nodeslist = clearlist(nodeslist);
                return ++success_flag;
            }
        }else{
            printf("Error in join: Invalid node found in NODESLIST %s", net);
            nodeslist = clearlist(nodeslist);
            return ++success_flag;
        }
        
    }
    else{ // empty network, 
        
        printf("The network is empty. You are the first to join\n");

        memset(personal->personal_net, 0, 4*sizeof(char));
        if(djoin(personal, "0.0.0.0", personal->personal_tcp) == 1) {
            printf("Error in join: Failed to enter the network\n");
            nodeslist = clearlist(nodeslist);
            return ++success_flag;
        }
    }
    
    // try to register the node
    return_msg = node_reg(personal->udp_address, personal->udp_port, personal->personal_addr, personal->personal_tcp, net);
    if(return_msg == NULL){
        
        printf("Error in join: Failed to register the node\n");
        nodeslist = clearlist(nodeslist);        
        return ++success_flag;
    }
    strcpy(personal->personal_net,net);
    personal->join_flag = 1;
             
    nodeslist = clearlist(nodeslist);
    free(return_msg);
     
    return success_flag;

}//join


int djoin(struct personal_node *personal, char *connectIP, char *connectTCP){

    char buffer[MAX_MSG_LENGTH];   memset(buffer, 0, sizeof(buffer));
    int success_flag = 0;

    if(personal->network_flag == 1){
        printf("Error in direct join: The node is already inside a network. Command ignored\n");
        return ++success_flag;
    }        
           
    if(is_valid_ip(connectIP) != 1){
        printf("Error in direct join: Invalid IPv4 address. Command ignored\n");
        printf("Please insert 4 numbers between 0 and 255 seperated by dots\n");        
        return ++success_flag;
    }

    if(check_ports(connectTCP) == 1){
        printf("Error in direct join: Invalid TCP port. Command ignored\n");
        printf("Please insert a 1 to 5 digit TCP port from 0 to 65535\n");        
        return ++success_flag;
    }
    
    int errflag;                  // flag to check for errors in function calls    
    struct addrinfo *srv_result;  // list of address structures    
    struct addrinfo srv_criteria; // necessary criteria to select the address structures from the list
    struct sigaction act;
    char msg_type[MAX_MSG_CMD_SIZE]; memset(msg_type, 0, MAX_MSG_CMD_SIZE);
    char *return_msg;
    //nleft = MAX_MSG_LENGTH;

    act.sa_handler = SIG_IGN;
    memset(&act, 0, sizeof(act));
    

    if(sigaction(SIGPIPE, &act, NULL) == -1){
        printf("Error in sigaction()\n");
        printf("This node cannot belong to a network.\n");
        printf("Because of this, the process will be terminated\n");
        close(personal->server_fd);
        personal->server_fd = -1;  
        personal->exit_flag = 1;               
        return 1;
    }
        
    personal->server_fd = socket(AF_INET, SOCK_STREAM, 0); // personal server socket for the intern nodes
    if (personal->server_fd < 0) {        
        printf("Error in socket()\n");
        printf("This node cannot belong to a network.\n");
        printf("Because of this, the process will be terminated\n");
        personal->exit_flag = 1;
        // free and close everything
        return 1;        
    }

    FD_SET(personal->server_fd, &personal->crr_scks);           // add server socket to FD set

    personal->max_fd = personal->server_fd;

    memset(&srv_criteria, 0, sizeof(srv_criteria)); // set all bytes inside "criteria" to zero
    srv_criteria.ai_family = AF_INET;               // family of IPv4 addresses
    srv_criteria.ai_socktype = SOCK_STREAM;         // TCP socket
    srv_criteria.ai_flags = AI_PASSIVE;             // passive socket that waits for requests

    // pass the information in "criteria" to "srv_result" 

    errflag = getaddrinfo(NULL, personal->personal_tcp, &srv_criteria, &srv_result);
    if (errflag != 0) {
        printf("Error in getaddrinfo()\n");
        printf("This node cannot belong to a network.\n");
        printf("Because of this, the process will be terminated\n");
        personal->exit_flag = 1;
        FD_CLR(personal->server_fd, &personal->crr_scks);
        close(personal->server_fd);        
        // free and close everything
        return 1;
    }

    // bind the address in "srv_result" to the personal socket

    errflag = bind(personal->server_fd, srv_result->ai_addr, srv_result->ai_addrlen);
    if (errflag == -1) {
        printf("Error in bind()\n");
        printf("This node cannot belong to a network.\n");
        printf("Because of this, the process will be terminated\n");
        personal->exit_flag = 1;
        FD_CLR(personal->server_fd, &personal->crr_scks);
        close(personal->server_fd);
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
        FD_CLR(personal->server_fd, &personal->crr_scks);
        close(personal->server_fd);
        personal->exit_flag = 1;
        return 1;
    }
    

    if (strcmp(connectIP, "0.0.0.0") == 0) {  //First node of the network
        
        if(strcmp(connectTCP, personal->personal_tcp) != 0){
            printf("If you're the first node, please use your port. Command ignored\n");
            FD_CLR(personal->server_fd, &personal->crr_scks);
            close(personal->server_fd);
            return 1;
        }
        
        strcpy(personal->extern_node->node_addr, personal->personal_addr);
        strcpy(personal->extern_node->tcp_port, personal->personal_tcp);
        personal->anchorflag = 1;                    
        personal->network_flag = 1;
        printf("From now on, you are alone in the network and you are your own extern neighbor\n");                
        
    }

    else if((strcmp(connectTCP, personal->personal_tcp) == 0) && (strcmp(connectIP, personal->personal_addr)) == 0){
        
        printf("You can't connect to yourself.\n");
        printf("If you're the first node, please use 0.0.0.0 as the connect IPv4 address and make sure to use your port.\n");
        printf("Command ignored\n");
        FD_CLR(personal->server_fd, &personal->crr_scks);
        close(personal->server_fd);
        return 1;
    }
    else{
        printf("Sending %s\n\n", entry_str);       
        
        //Sends ENTRY message
        return_msg = send_entry(&(personal->extern_node->node_fd), personal->personal_addr, personal->personal_tcp, 
        connectIP, connectTCP); 
        if(return_msg == NULL){
                        
            printf("Error in direct join: Failed to join the network\n");
            FD_CLR(personal->server_fd, &personal->crr_scks);
            close(personal->server_fd);                                    
            return 1;
        }
        
        printf("\nMessage sent to [%s | %s]\n", connectIP, connectTCP);
        printf("%s %s %s\n\n", entry_str, personal->personal_addr, personal->personal_tcp);        
        free(return_msg);        

        //personal->extern_node = contact_init(personal->extern_node);                
        strcpy(personal->extern_node->tcp_port, connectTCP);
        strcpy(personal->extern_node->node_addr, connectIP);

        printf("New extern neighbor:\n");
        printf("Address:%s\nPort:%s\n", personal->extern_node->node_addr, personal->extern_node->tcp_port);
        //personal->extern_node->safe_flag = 1;
        personal->network_flag = 1;
    }
    return 0;
}//djoin
    


storageList_t *create(storageList_t *storage_head, char *name){ // name size <= 100, alphanumeric chars only

    if(check_name(name) == 1){
        printf("Error in create: invalid name\n");
        return storage_head;
    }

    storage_head = storageInsert(storage_head, name);
    printf("Object '%s' was created your personal storage\n\n", name);
    return storage_head;
            
}//create

void show_names(storageList_t *storage_ptr){

    if(storage_ptr == NULL){
        printf("No names to show\n");
        return;
    }

    int counter = 1;
    storageList_t *aux = storage_ptr;

    printf("Showing names\n\n");
    while(aux != NULL){
        printf("Name %d: %s\n", counter, aux->object);
        counter++;
        aux = aux->next;
    }
    return;
}

void show_cache(objectQueue_t *queue_ptr){

    if(queue_ptr->head == NULL){
        printf("No names to show inside the cache\n");
        return;
    }

    int counter = 1;
    queueBlock_t *aux = queue_ptr->head;

    printf("Showing names in cache\n\n");
    while(aux != NULL){
        printf("Name %d: %s\n", counter, aux->name);
        counter++;
        aux = aux->next;
    }
    return;
}


int show_topology(struct personal_node *personal){
    
    int success_flag = 0;
    int known_neighbors = 0, unknown_neighbors = 0, total_neighbors = 0;

    nodesLinkedlist_t *aux;

    if(personal == NULL){
        printf("The node is not initialized\n");
        return ++success_flag;
    }    

    if(personal->network_flag == 1){

        if(personal->join_flag == 1){
            printf("Your node is inside the following network: %s\n\n", personal->personal_net);
        }
        printf("\n\nYour node's info:\n\n");        
        printf("Adress: %s\nPort: %s\n\n",        
        personal->personal_addr, 
        personal->personal_tcp);
        
    }
    else{
        printf("Error in show topology: You are not inside a network\n");
        return ++success_flag;
    }
                   
    printf("\n\nExtern neighbor's info: \n");        
    printf("Adress: %s\nPort: %s\n\n",                  
    personal->extern_node->node_addr, 
    personal->extern_node->tcp_port);
                                    
    
    if ((strcmp(personal->backup_addr, "") != 0) && (strcmp(personal->backup_tcp, "") != 0)) {
        printf("\n\nBackup neighbor's info: \n");                
        printf("Adress: %s\nPort: %s\n\n",                  
        personal->backup_addr, 
        personal->backup_tcp);
                
    }   
    else {
        printf("\nThere is no backup neighbor!\n");   
    }    
        
    if(personal->internals_list == NULL){
          
        printf("\nThere are no intern neighbors\n");        
                
    }
    else{
        printf("\n\nPrinting intern neighbors: \n\n");        
            
        aux = personal->internals_list;
        while(aux != NULL){
                            
            if((strcmp(aux->node->node_addr, "") != 0) && (strcmp(aux->node->tcp_port, "") != 0)){

                printf("Adress: %s\nPort: %s\n\n",                                    
                    aux->node->node_addr,
                    aux->node->tcp_port);
    
                known_neighbors++;
                total_neighbors++;
            }else{                
                unknown_neighbors++;
                printf("%d unknown neighbor(s) detected\n\n", unknown_neighbors);
                total_neighbors++;
            }
                                    
            aux = aux->next;
        }        
        printf("\nNumber of known interns: %d\n", known_neighbors);
        printf("Number of unknown interns: %d\n", unknown_neighbors);
        printf("Number of total interns: %d\n", total_neighbors);
    }
        
    return success_flag;
} // show_topology


int leave(struct personal_node *personal) {
            
    char *return_msg;
    
    if(personal->network_flag == 1){
        if(personal->join_flag == 1){

            return_msg = node_unreg(personal->udp_address, personal->udp_port, personal->personal_addr, personal->personal_tcp, personal->personal_net);
            if (return_msg == NULL){

                printf("Error in leave: Failed to unregister the node. Leaving anyway\n");                

            }
            else free(return_msg);
            // check return value of node_unreg
            personal->join_flag = 0;
            strcpy(personal->personal_net, "");
        }
        
        // close connections 
        int i;
        for (i = 3; i <= personal->max_fd; i++) {
            if (FD_ISSET(i, &personal->crr_scks)) {
                FD_CLR(i, &personal->crr_scks);
                close(i);
            }
        }
        personal->network_flag = 0;
        
    }    
    personal = reset_personal(personal);
    
    return 0;
}//leave

void help_menu(void) {

        printf("--------------------------------------------HELP MENU-------------------------------------\n\n");
        printf("To use the application, insert one of the following commands:\n\n");        
        printf("join (j) [desired network] \n");
        printf("direct join (dj) [connect IPv4 address] [connect TCP port]\n");        
        printf("create (c) [name]\n");
        printf("delete (dl) [name]\n");
        printf("retrieve (r) [name]\n");
        printf("show topology (st)\n");
        printf("show interest table (si)\n");
        printf("show names (sn)\n");                
        printf("clear names (cn)\n");
        printf("show cache (sc)\n");
        printf("clear cache (cc)\n");
        printf("leave (l)\n");
        printf("exit (x)\n");
        printf("\nA few notes:\n\n");
        printf("-curved brackets => valid abreviations;\n");
        printf("-square brackets => command arguments;\n");
        printf("-all the commands and their arguments are case sensitive;\n\n");
        printf("\n----------------------------------------------------------------------------------------\n\n");
        return;
}//help_menu

/*--------------End user interface functions--------------*/