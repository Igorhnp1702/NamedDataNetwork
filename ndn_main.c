/************************************************************************************************
 * ndn_main.c
 *
 * Authors:
 *
 * -Igor Paruque ist1102901
 * -Mónica Ferreira ist106520
 *
 * Description: source code for the main function
 ***********************************************************************************************/

//#define _XOPEN_SOURCE 600 //!REMOVE BEFORE COMPILING

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
#include "ndn_commands.h"
#include "ndn_messages.h"
#include "ndn_interestTable.h"
#include "ndn_objectStructs.h"



int main(int argc, char **argv){

    
    printf("\n");
    /* check the arguments */

    if (argc < 4 || argc > 6)
    {

        printf("Error: Wrong number of arguments. Please provide 4 to 6 of the following arguments\n\n");
        printf("Usage: ndn [personal cache size] [personal IPv4 address] [personal TCP port] [node server's IPv4 address] [server's UDP port]\n");
        printf("\nor\n");
        printf("Usage: ndn [personal cache size] [personal IPv4 address] [personal TCP port] [node server's IPv4 address]\n");
        printf("\nor\n");
        printf("Usage: ndn [personal cache size] [personal IPv4 address] [personal TCP port] [server's UDP port]\n");
        printf("\nor\n");
        printf("Usage: ndn [personal cache size] [personal IPv4 address] [personal TCP port]\n");
            
        exit(1);
    } // if
    
    char *personal_addr;        // Personal IPv4 addres
    char *personal_port;        // Personal TCP port
    char *server_addr;          // The server's IPv4 address
    char *server_port;          // The server's UDP port      
    int cache_threshold;  
            
    if (argc == 4)
    {
        if(check_cache_size(argv[1]) == 1){

            printf("Cache size has to be a positive number\n");
            exit(1);
        }
        cache_threshold = atoi(argv[1]);
        if(!is_valid_ip(argv[2])){

            printf("Invalid personal IPv4 address. Process terminated\n");
            exit(1);
        }
        personal_addr = argv[2];               

        if(check_ports(argv[3]) == 1){

            printf("Error: invalid port. Process terminated\n");
            exit(1);
        }
        personal_port = argv[3];

        printf("The node server's IPv4 address will default to %s\n", DEFAULT_REGIP);
        server_addr = DEFAULT_REGIP;

        printf("The node server's UDP port will default to %s\n", DEFAULT_REGUDP);
        server_port = DEFAULT_REGUDP;

    } // if 3 arguments

    if (argc == 5)
    {
        if(check_cache_size(argv[1]) == 1){

            printf("Cache size has to be a positive number\n");
            exit(1);
        }
        cache_threshold = atoi(argv[1]);
        
        if(!is_valid_ip(argv[2])){

            printf("Invalid personal IPv4 address. Process terminated\n");
            exit(1);
        }
        personal_addr = argv[2];
       
        if(check_ports(argv[3]) == 1){

            printf("Error: invalid port. Process terminated\n");
            exit(1);
        }
        personal_port = argv[3];

        // test format to see if it's an ip address or not

        if(is_valid_ip(argv[4])){

            server_addr = argv[4];

            printf("The node server's UDP port will default to %s\n", DEFAULT_REGUDP);
            server_port = DEFAULT_REGUDP;

        }else if(check_ports(argv[4]) == 0){ // if test fails
                                   
            server_port = argv[4];

            printf("The node server's IPv4 address will default to %s\n", DEFAULT_REGIP);
            server_addr = DEFAULT_REGIP;

        }
        else{

            printf("Invalid 4th argument. Process terminated\n");
            exit(1);
        }            
        
    } // if 4 arguments

    if (argc == 6)
    {
    
        if(check_cache_size(argv[1]) == 1){

            printf("Cache size has to be a positive number\n");
            exit(1);
        }
        cache_threshold = atoi(argv[1]);
        
        if(!is_valid_ip(argv[2])){

            printf("Invalid personal IPv4 address. Process terminated\n");
            exit(1);
        }
        personal_addr = argv[2];

        if(check_ports(argv[3]) == 1){

            printf("Error: invalid port. Process terminated\n");
            exit(1);
        }
        personal_port = argv[3];
        
        if(!is_valid_ip(argv[4])){

            printf("Invalid IPv4 address for the server. Process terminated\n");
            exit(1);
        }
        server_addr = argv[4];
        
        if(check_ports(argv[5]) == 1){

            printf("Error: invalid port. Process terminated\n");
            exit(1);
        }
        server_port = argv[5];

    } // if 5 arguments

    
    printf("The application was launched successfuly\n\n");
    printf("\nType 'help' to show this help menu.\n\n");
    help_menu();            
        
    
    
    /* Initialize the node */

    struct personal_node *my_node = NULL;

    my_node = personal_init(my_node);

    my_node->personal_addr = personal_addr;
    my_node->personal_tcp = personal_port;
    my_node->udp_address = server_addr;
    my_node->udp_port = server_port;
    my_node->cache_limit = cache_threshold;
    my_node->queue_ptr = queueInit(my_node->queue_ptr, my_node->cache_limit);
    my_node->extern_node = contact_init(my_node->extern_node);

    socklen_t srv_addrlen;        // size of the personal IPv4 address
    struct sockaddr srv_addr;     // the IPv4 address
    
    // Start the event loop
      
    int select_ctrl;
    int parse_return = 0;
    int fd_itr = 0;              // an iterator to go through the FD set
    char buffer[MAX_MSG_LENGTH]; // buffer to receive msg and cmd
    
    //char msg_type[MAX_MSG_CMD_SIZE];
        
    ssize_t nread;               // number of bytes read from a read operation    
    int new_fd;                  // a file descriptor to receive the "NEW" message and extract the id of the node
    int backup_fail = 0;

    nodesLinkedlist_t *aux, *aux2del;
    
    nodeinfo_t *node_aux = NULL;
    node_aux = contact_init(node_aux);

    char *message = NULL;              // pointer to buffer, to use with read operation;
    char *return_msg = NULL;
        
    FD_ZERO(&my_node->crr_scks);                              // Set the set of FD's to zero
    FD_SET(STDIN_FILENO, &my_node->crr_scks);                 // add stdin(keyboard input) to FD set    

    // Infinite cycle where the communications happen

    while (1) {
                    
        //In case old external neigbor disconnected, add new one to FD set
        
        if ((my_node->extern_node->node_fd != -1) && (!FD_ISSET(my_node->extern_node->node_fd, &my_node->crr_scks))) {
            FD_SET(my_node->extern_node->node_fd, &my_node->crr_scks);
            if (my_node->extern_node->node_fd > my_node->max_fd) my_node->max_fd = my_node->extern_node->node_fd;
        }
        
        //remove all FDs with no activity from FD set my_node->rdy_scks
        my_node->rdy_scks = my_node->crr_scks;
    
        printf("\nWaiting for activity...\n\n");
        select_ctrl = select(my_node->max_fd + 1, &my_node->rdy_scks, (fd_set*)NULL, (fd_set*)NULL, (struct timeval*)NULL);


        if (select_ctrl < 0) {         
            printf("Error in select: %s.\nProcess terminated\n", strerror(errno));
            leave(my_node);                                
            free_contact(&(my_node->extern_node));
            my_node->queue_ptr = clearQueue(my_node->queue_ptr);
            my_node->storage_ptr = storageClear(my_node->storage_ptr);
            free(my_node->personal_net);
            free(my_node->backup_addr);
            free(my_node->backup_tcp);
            free_contact(&node_aux);                           
            free(my_node);
            if(message != NULL) free(message);
            exit(1);                
        }
        
        printf("Activity found\n");

        //Go through all FDs and see which ones have activity
        for (fd_itr = 0; fd_itr <= my_node->max_fd + 1; fd_itr++){

            if(FD_ISSET(fd_itr, &my_node->rdy_scks)){
                
                // read and process user input
                if (fd_itr == STDIN_FILENO) {
                    memset(buffer, 0, MAX_MSG_LENGTH); //set the buffer to '\0'                                         
                    fgets(buffer, MAX_MSG_LENGTH-1, stdin);
                    printf("\n-------------------------------------------------------------------\n");
                    printf("Handling your command...");
                    printf("\n--------------------------------------------------------------------\n\n");

                    select_cmd(my_node, buffer);  
                    
                    if(my_node->exit_flag == 1){

                        if(leave(my_node) == 0){

                            free_contact(&(my_node->extern_node));                            
                            my_node->queue_ptr = clearQueue(my_node->queue_ptr);
                            my_node->storage_ptr = storageClear(my_node->storage_ptr);
                            free(my_node->personal_net);
                            free(my_node->backup_addr);
                            free(my_node->backup_tcp);                                                                             
                            free(my_node);
                            free_contact(&node_aux);
                            if(message != NULL) free(message);
                            printf("Exit executed successfully\n");
                            exit(0);
                        }
                    }
                    
                }

                // accept a connection, if you can
                else if (fd_itr == my_node->server_fd && my_node->network_flag == 1) { // atividade no servidor e estou na rede
                    
                    srv_addrlen = sizeof(srv_addr);
                    if((new_fd = accept(my_node->server_fd, &srv_addr, &srv_addrlen)) == -1){
                        printf("Error in accept(). Request rejected\n");
                        //return 1;
                    }                    
                    else{
                        printf("Accepted connection. Expecting ENTRY\n");       // ENTRY message will come from newfd

                        // add new intern node to the list
                        // wait for entry to actualy update the information
                        node_aux->node_fd = new_fd;  
                        
                        my_node->internals_list = insertnode(my_node->internals_list, node_aux);
                        my_node->n_internals++;

                        FD_SET(new_fd, &my_node->crr_scks);                     // prepare to read the ENTRY message
                        if (new_fd > my_node->max_fd) my_node->max_fd = new_fd;
                    }
                    
                }

                // read a message from a node
                
                else if(my_node->network_flag == 1){
                    memset(buffer, 0, MAX_MSG_LENGTH); //set the buffer to '\0'   
                                                            
                    nread = read(fd_itr, buffer, MAX_MSG_LENGTH - 1);
                     
                    if (nread == -1) {
                        printf("Error in read: %s\n", strerror(errno));
                        FD_CLR(fd_itr, &(my_node->crr_scks));
                        close(fd_itr);
                    }                    
                    else if (nread == 0) {  //the fd is disconnected
                        
                        FD_CLR(fd_itr, &(my_node->crr_scks));
                        
                        if (fd_itr == my_node->extern_node->node_fd) {    //Extern neighbor disconnected

                            
                            printf("External neighbor [%s | %s] disconnected!\n\n",
                                my_node->extern_node->node_addr, my_node->extern_node->tcp_port);
                            
                                                                                                                                            
                            if (my_node->anchorflag == 0) { //not an anchor                                                                                                                        
                            
                                
                                close(my_node->extern_node->node_fd);
                                my_node->extern_node->node_fd = -1;   // Reset client_fd to -1
                                
                                //Sends ENTRY message 
                                
                                // Until a successfull connection is achieved with a new extern node, try to send an entry to someone

                                // If the backup node fails, pick an intern to be an anchor with you
                                                                    
                                printf("Sending %s\n\n", entry_str);

                                return_msg = send_entry(&(my_node->extern_node->node_fd), my_node->personal_addr, my_node->personal_tcp,
                                my_node->backup_addr, my_node->backup_tcp); 
                                if(return_msg == NULL){ // alterar retorno do send entry
                                    printf("Failed to send %s to new extern (backup_node)\n", entry_str);
                                    printf("An intern node will be picked to be an anchor with you, if available\n");
                                    backup_fail = 1;
                                    my_node->anchorflag = 1;                                                                       
                                }
                                else{
                                    printf("Expecting SAFE\n\n");

                                    FD_SET(my_node->extern_node->node_fd, &my_node->crr_scks);
                                    if (my_node->extern_node->node_fd > my_node->max_fd) my_node->max_fd = my_node->extern_node->node_fd;

                                    strcpy(my_node->extern_node->node_addr, my_node->backup_addr);
                                    strcpy(my_node->extern_node->tcp_port, my_node->backup_tcp);

                                    printf("New extern's info:\n");

                                    printf("Address: %s\nPort: %s\n\n", 
                                    my_node->extern_node->node_addr, my_node->extern_node->tcp_port);

                                    free(return_msg);
                                    return_msg = NULL;
                                } 
                                                        
                            }
                            if(my_node->n_internals > 0 && my_node->anchorflag == 1) { // anchor partner left

                                                                
                                if(my_node->anchorflag == 1 && backup_fail == 0){ // fallen external was an internal
                                    
                                    my_node->internals_list = removenode(my_node->internals_list, my_node->extern_node->node_fd);
                                    my_node->n_internals--;
                                }
                                close(my_node->extern_node->node_fd);
                                my_node->extern_node->node_fd = -1;   // Reset client_fd to -1


                                aux = my_node->internals_list;
                                while(aux != NULL){

                                    printf("Converting an intern into an extern\n\n");
                                
                                    // search the list for a node to become an external. Start with the first internal 
                                    contact_copy(my_node->extern_node, aux->node);
                                    my_node->extern_node->node_fd = aux->node->node_fd;

                                    //send ENTRY + SAFE
                                    
                                    printf("Sending %s\n\n", entry_str);

                                    return_msg = send_entry(&(my_node->extern_node->node_fd), my_node->personal_addr, my_node->personal_tcp,
                                    my_node->backup_addr, my_node->backup_tcp);
                                    if(return_msg == NULL){
                                        
                                        printf("Failed to convert intern into extern. Connection closed.\n");
                                        my_node->internals_list = removenode(my_node->internals_list, aux->node->node_fd);
                                        my_node->n_internals--;
                                        aux = aux->next; // head = head->next                                                                                    
                                        continue;
                                    }
                                    free(return_msg);
                                    return_msg = NULL;

                                    printf("\nMessage sent to [%s | %s]:\n", aux->node->node_addr, aux->node->tcp_port);
                                    printf("%s %s %s\n\n", entry_str, my_node->personal_addr, my_node->personal_tcp);

                                    
                                    printf("Sending %s\n", safe_str);

                                    return_msg = send_safe(my_node->extern_node->node_fd, aux->node->node_addr, aux->node->tcp_port);
                                    if(return_msg == NULL){
                                        
                                        printf("Failed to convert intern into extern. Connection closed.\n");
                                        aux2del = aux;
                                        aux = aux->next; // head = head->next                            
                                        my_node->internals_list = removenode(my_node->internals_list, aux2del->node->node_fd);
                                        my_node->n_internals--;
                                                    
                                        continue;
                                    }
                                    free(return_msg);
                                    return_msg = NULL; 
                                    
                                    printf("\nMessage sent to [%s | %s]:\n", aux->node->node_addr, aux->node->tcp_port);
                                    printf("%s %s %s\n\n", safe_str, aux->node->node_addr, aux->node->tcp_port);                                    
                                    
                                    backup_fail = 0;
                                    printf("Waiting for response to conversion\n\n");
                                    break;
                                }                                    
                                                                                                
                                // send safe to everybody else
                                
                                aux = my_node->internals_list;
                                
                                if(aux != NULL) printf("Warning other interns\n\n");

                                while(aux != NULL){
                                
                                    if(aux->node->node_fd != my_node->extern_node->node_fd){

                                        printf("Sending %s\n\n", safe_str);

                                        return_msg = send_safe(aux->node->node_fd, my_node->extern_node->node_addr, my_node->extern_node->tcp_port);
                                        if(return_msg == NULL){
                                            printf("Error in send safe: failed to send safe message to [%s | %s]\n", aux->node->node_addr, aux->node->tcp_port);
                                        }else{
                                            free(return_msg);
                                            return_msg = NULL;
                                            
                                            printf("\nMessage sent to [%s | %s]:\n", aux->node->node_addr, aux->node->tcp_port);
                                            printf("%s %s %s\n\n", safe_str, my_node->extern_node->node_addr, my_node->extern_node->tcp_port);
                                        }
                                    }
                                    
                                    aux = aux->next;
                                }                                                                
                            }
                            if(my_node->n_internals == 0 && (backup_fail == 1 || my_node->anchorflag == 1)) {  //is now the only node in the network
                                
                                // become your own extern and clear the backup node

                                
                                memset(my_node->extern_node->tcp_port, 0, MAX_TCP_UDP_CHARS * sizeof(*my_node->extern_node->tcp_port));
                                memset(my_node->extern_node->node_addr, 0, MAX_ADDRESS_SIZE * sizeof(*my_node->extern_node->node_addr));
                                memset(my_node->extern_node->node_buff, 0, MAX_MSG_LENGTH * sizeof(*my_node->extern_node->node_buff));
                                my_node->extern_node->node_fd = -1;
                                strcpy(my_node->extern_node->node_addr, my_node->personal_addr);
                                strcpy(my_node->extern_node->tcp_port, my_node->personal_tcp);

                                
                                memset(my_node->backup_tcp, 0, MAX_TCP_UDP_CHARS * sizeof(*my_node->backup_tcp));
                                memset(my_node->backup_addr, 0, MAX_ADDRESS_SIZE * sizeof(*my_node->backup_addr));  
                                
                                printf("At the moment, you are alone in the network and you are your own extern neighbor\n");
                                
                            }
                            
                        }
                        else{                    
                            
                            //Internal neighbor disconnected
                            
                            //find the node that corresponds to the disconnected fd

                            aux = my_node->internals_list;
                                
                            while(aux != NULL){
                            
                                if(aux->node->node_fd == fd_itr){
                                    
                                    if((strcmp(aux->node->node_addr, "") == 0) || (strcmp(aux->node->tcp_port, "") == 0)){
                                        
                                        printf("Unknown neighbor disconnected\n");
                                    }
                                    else printf("Internal node disconnected: [%s | %s]\n", aux->node->node_addr, aux->node->tcp_port);

                                    break;
                                }
                                aux = aux->next;
                            }  
                            // remove him from the list

                            if(my_node->internals_list != NULL){
                            
                                my_node->internals_list = removenode(my_node->internals_list, aux->node->node_fd);
                                my_node->n_internals--;                            
                            }  
                                                      
                        }                                                       
                        
                    }
                    else{                        
    
                        if(fd_itr == my_node->extern_node->node_fd){ 

                            //the message came from the extern node
                           
                            while((message = parseNstore(buffer, &(my_node->extern_node->node_buff))) != NULL){

                                memset(buffer, 0, MAX_MSG_LENGTH); //set the buffer to '\0'
                                if(parse_tcp(my_node, message, my_node->extern_node) == 1){
                                    printf("Error in main: failed to parse a message\n");
                                                                        
                                }
                                if(message != NULL){
                                    free(message);
                                    message = NULL;
                                }
                            }
                        }                        
                        
                        else{

                            // search the intern node with the file descriptor

                            aux = my_node->internals_list;

                            while(aux != NULL){

                                if(aux->node->node_fd == fd_itr){
                                    
                                    // the message came from an intern node
                                    while((message = parseNstore(buffer, &(aux->node->node_buff))) != NULL){
                                        memset(buffer, 0, MAX_MSG_LENGTH); //set the buffer to '\0'
                                        parse_return = parse_tcp(my_node, message, aux->node);
                                        if(message != NULL){
                                            free(message);
                                            message = NULL;
                                        }

                                        if( parse_return == 1){
                                            printf("Error in main: failed to parse a message\n");
                                            
                                        }
                                        if( parse_return == -1) break;
                                                                                                                                                                      
                                    }
                                    break;
                                } 
                                aux = aux->next;
                            }                                                    
                        }

                    }//else                    
                }//else (read a message from a node)
                else FD_CLR(fd_itr, &(my_node->crr_scks));
            }//if(fdisset)
        }//for(fd_itr)
    }//while(1)    
}//main()

   
    