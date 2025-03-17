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
// #include "ndn_interestTable.h"
// #include "ndn_queue.h"



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
            
    if (argc == 4)
    {
        
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

        }else{ // if test fails
                                   
            server_port = argv[4];

            printf("The node server's IPv4 address will default to %s\n", DEFAULT_REGIP);
            server_addr = DEFAULT_REGIP;

        }            
        
    } // if 4 arguments

    if (argc == 6)
    {
    
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
    printf("\n_________________________________________________________________\n\n");    
    
    
    /* Initialize the node */

    struct personal_node *my_node = NULL;

    my_node = personal_init(my_node);

    my_node->personal_addr = personal_addr;
    my_node->personal_tcp = personal_port;
    my_node->udp_address = server_addr;
    my_node->udp_port = server_port;

    my_node->extern_node = contact_init(my_node->extern_node);

    socklen_t srv_addrlen;        // size of the personal IPv4 address
    struct sockaddr srv_addr;     // the IPv4 address
    
    // Start the event loop
      
    int select_ctrl;
    int fd_itr = 0;              // an iterator to go through the FD set
    char buffer[MAX_MSG_LENGTH]; // buffer to receive msg and cmd
    char msg_type[MAX_MSG_CMD_SIZE];
        
    ssize_t nread;               // number of bytes read from a read operation
    ssize_t nleft;               // number of bytes left to fill the buffer capacity
    int new_fd;                  // a file descriptor to receive the "NEW" message and extract the id of the node

    nodesLinkedlist_t *aux;

    char *ptr_bffr;              // pointer to buffer, to use with read operation;
        
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
            

            // if(my_node->queue_ptr != NULL){
            
            //     objectQueue_t *queue_ptr; // pointer to go through the list
            //     objectQueue_t *aux;     // auxiliary pointer to delete elements in the lists
            //     queue_ptr = my_node->contents;
                
            //     while(queue_ptr != NULL){

            //         aux = queue_ptr;
            //         queue_ptr = queue_ptr->next;
            //         free(aux->string);
            //         free(aux);
                    
            //     }                            
            // }
                
            free(my_node);
            exit(1);                
        }
        
        printf("Activity found\n");

        //Go through all FDs and see which ones have activity
        for (fd_itr = 0; fd_itr < my_node->max_fd + 1; fd_itr++){

            if(FD_ISSET(fd_itr, &my_node->rdy_scks)){
                
                // read and process user input
                if (fd_itr == STDIN_FILENO) {
                    memset(&buffer, 0, sizeof(buffer)); //set the buffer to '\0'                                         
                    fgets(buffer, MAX_MSG_LENGTH-1, stdin);
                    printf("\n_________________________________________________________________\n");
                    printf("Handling your command...");
                    printf("\n_________________________________________________________________\n\n");

                    select_cmd(my_node, buffer);                                        
                    
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
                        FD_SET(new_fd, &my_node->crr_scks);                     // prepare to read the ENTRY message
                        if (new_fd > my_node->max_fd) my_node->max_fd = new_fd;
                    }
                    
                }

                // read a message from a node
                
                else{
                    memset(&buffer, 0, sizeof(buffer)); //set the buffer to '\0'
                    ptr_bffr = buffer;
                    nleft = MAX_MSG_LENGTH;     //last byte needs to be free for '\0'

                    while (((nread = read(fd_itr, ptr_bffr, MAX_MSG_LENGTH - 1)) > 0) && (nread < nleft)) { //read msg to buffer
                        ptr_bffr += nread - 1;
                        nleft = MAX_MSG_LENGTH;
                        if (*(ptr_bffr) == '\n') break;
                    }

                    if (nread == -1) {
                        printf("Error in read: %s\n", strerror(errno));
                        FD_CLR(fd_itr, &(my_node->crr_scks));                        
                    }                    
                    else if (nread == 0) {  //the fd is disconnected
                        
                        memset(&buffer, 0, sizeof(buffer));
                        
                        if (fd_itr == my_node->extern_node->node_fd) {    //External neighbor disconnected

                            
                            printf("External neighbor (Interface %s/%s) disconnected!\n",
                                 my_node->extern_node->node_addr, my_node->extern_node->tcp_port);
                            
                            my_node->extern_node->node_fd = -1;   // Reset client_fd to -1                            
                            
                            if (my_node->anchorflag == 0) { //not an anchor                                                                                                                        
                                
                                //Sends ENTRY message , receives SAFE response and updates backup node
                                
                                // Until a successfull connection is achieved with a new extern node, try to send an entry to someone

                                // If the backup node fails, pick an intern to be an anchor with you
                                                                    
                                printf("Sending %s %s %s\n", entry_str, my_node->personal_addr, my_node->personal_tcp);

                                if(strcmp(send_entry(&(my_node->extern_node->node_fd), my_node->personal_addr, my_node->personal_tcp,
                                            my_node->backup_addr, my_node->backup_tcp), "1") != 0){
                                    printf("Failed to send %s to new extern (backup_node)\n", entry_str);
                                    printf("An intern node will be picked to be an anchor with you, if available\n");
                                    my_node->anchorflag = 1;
                                    close(my_node->extern_node->node_fd);
                                    my_node->extern_node->node_fd = -1;
                                    
                                }
                                else{
                                    printf("Expecting SAFE\n");
                                    
                                    while (((nread = read(my_node->extern_node->node_fd, ptr_bffr, MAX_MSG_LENGTH - 1)) > 0) && (nread < nleft)) { //read msg to buffer
                                        ptr_bffr += nread - 1;
                                        nleft = MAX_MSG_LENGTH;
                                        if (*(ptr_bffr) == '\n') break;
                                    }                                                                        
                            
                                    if (nread == -1) {
                                        printf("Error in main: read returned %s while reading SAFE message.\n", strerror(errno));
                                        printf("An intern node will be picked to be an anchor with you, if available\n");
                                        my_node->anchorflag = 1;
                                        close(my_node->extern_node->node_fd);
                                        my_node->extern_node->node_fd = -1;
                                        continue;
                                        
                                    }

                                    sscanf(buffer, "%s", msg_type);
                                    if(strcmp(msg_type, safe_str) != 0){
                                        printf("Error in main: new extern did not send SAFE message\n");
                                        printf("An intern node will be picked to be an anchor with you, if available\n");
                                        printf("Connection with new extern was closed\n");
                                        my_node->anchorflag = 1;            
                                        close(my_node->extern_node->node_fd);
                                        my_node->extern_node->node_fd = -1;
                                        continue;
                                    }
                                    printf("Message received %s\n", buffer);

                                    strcpy(my_node->extern_node->node_addr, my_node->backup_addr);
                                    strcpy(my_node->extern_node->tcp_port, my_node->backup_tcp);

                                    // try to get the new backup node

                                    if((parse_tcp(my_node, buffer, &(my_node->extern_node->node_fd))) == 1 ){
                                        printf("Error in main: Failed to read the new extern node's interface\n");
                                        printf("Connection with new extern was closed\n");            
                                        close(my_node->extern_node->node_fd);
                                        my_node->extern_node->node_fd = -1;
                                        continue;
                                    }                                                                                                                                                                                                                        
                                    
                                    // // send safe to everybody else

                                    // aux = my_node->internals_list;

                                    // while(aux != NULL){
                                    
                                    //     if(strcmp(send_safe(aux->node->node_fd, my_node->extern_node->node_addr, my_node->extern_node->tcp_port), "1") != 0){
                                    //         printf("Error in send_safe: failed to send safe message to %s | %s\n", aux->node->node_addr, aux->node->tcp_port);
                                    //     }
                                    //     aux = aux->next;
                                    // }
                                    
                                    if (strcmp(my_node->personal_addr, my_node->backup_addr) == 0 &&
                                        strcmp(my_node->personal_tcp, my_node->backup_tcp) == 0) {
                                        
                                        my_node->anchorflag = 1;
                                    }
                                }
                                                        
                            }
                            if (my_node->n_internals > 1 && my_node->anchorflag == 1) { //has internals beyond the anchor partner
                                
                                if(my_node->anchorflag == 1 && my_node->extern_node->node_fd == -1){ // is an anchor without the partner

                                    aux = my_node->internals_list;
                                    while(aux != NULL){

                                        printf("Converting an intern into an extern\n\n");
                                    
                                        // search the list for a node to become an external. Start with the first internal 
                                        contact_copy(my_node->extern_node, aux->node);
                                        my_node->extern_node->node_fd = aux->node->node_fd;

                                        //send him an entry message followed by a safe message

                                        printf("Sending %s %s %s\n", entry_str, my_node->personal_addr, my_node->personal_tcp);

                                        if(strcmp(send_entry(&(my_node->extern_node->node_fd), my_node->personal_addr, my_node->personal_tcp,
                                            my_node->backup_addr, my_node->backup_tcp), "1") != 0){
                                            
                                            printf("Failed to convert intern into extern. Connection closed.\n");
                                            FD_CLR(aux->node->node_fd, &(my_node->crr_scks));
                                            aux = removenode(aux, aux->node->node_fd); // head = head->next
                                            my_node->n_internals--;
                                            close(my_node->extern_node->node_fd);
                                            my_node->extern_node->node_fd = -1;
                                            continue;
                                        }

                                        printf("Sending %s %s %s\n", safe_str, aux->node->node_addr, aux->node->tcp_port);

                                        if((strcmp(send_safe(my_node->extern_node->node_fd, aux->node->node_addr, aux->node->tcp_port), "1")) != 0){
                                            
                                            printf("Failed to convert intern into extern. Connection closed.\n");
                                            FD_CLR(aux->node->node_fd, &(my_node->crr_scks));
                                            aux = removenode(aux, aux->node->node_fd); // head = head->next
                                            my_node->n_internals--;
                                            close(my_node->extern_node->node_fd);
                                            my_node->extern_node->node_fd = -1;
                                            continue;
                                        }                                        
                                        printf("Conversion successfull\n");
                                        break;
                                    }
                                    //Delete that node from the internals list
                                    aux = removenode(aux, aux->node->node_fd); // head = head->next
                                }
                                

                                // send safe to everybody else
                               
                                aux = my_node->internals_list;
                                
                                while(aux != NULL){
                                
                                    if(strcmp(send_safe(aux->node->node_fd, my_node->backup_addr, my_node->backup_tcp), "1") != 0){
                                        printf("Error in send safe: failed to send safe message to %s | %s\n", aux->node->node_addr, aux->node->tcp_port);
                                    }
                                    aux = aux->next;
                                }                                                                
                            }
                            if(my_node->n_internals == 0) {  //is now the only node in the network
                                //my_node->anchorflag = 1;
                                // clear the external and the backup node

                                
                                memset(my_node->extern_node->tcp_port, 0, MAX_TCP_UDP_CHARS * sizeof(*my_node->extern_node->tcp_port));
                                memset(my_node->extern_node->node_addr, 0, MAX_ADDRESS_SIZE * sizeof(*my_node->extern_node->node_addr));
                                memset(my_node->extern_node->node_buff, 0, MAX_MSG_LENGTH * sizeof(*my_node->extern_node->node_buff));
                                my_node->extern_node->node_fd = -1;                                
                                
                                memset(my_node->backup_tcp, 0, MAX_TCP_UDP_CHARS * sizeof(*my_node->backup_tcp));
                                memset(my_node->backup_addr, 0, MAX_ADDRESS_SIZE * sizeof(*my_node->backup_addr));                                                             
                                
                            }
                            
                        }
                        else{                     //Internal neighbor disconnected

                            
                            //find the node that corresponds to the disconnected fd

                            aux = my_node->internals_list;
                                
                            while(aux != NULL){
                            
                                if(aux->node->node_fd == fd_itr){
                                    printf("Internal node disconected: %s | %s\n", aux->node->node_addr, aux->node->tcp_port);
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
                        
                        FD_CLR(fd_itr, &(my_node->crr_scks));
                    }
                    else{                        
                        printf("Message read from a node: %s\n", buffer);
                        if(parse_tcp(my_node, buffer, &fd_itr) == 1){
                            printf("Error in main: failed to parse a message\n");
                        }
                    }//else                    
                }//else (read a message from a node)
            }//if(fdisset)
        }//for(fd_itr)
    }//while(1)    
}//main()

   
    