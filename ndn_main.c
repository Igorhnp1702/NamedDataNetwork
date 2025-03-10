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
#include "ndn_netfuncs.h"
#include "ndn_io.h"
#include "ndn_utils.h"



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
    int cache_size;             // integer variable to store the cache size

    errno = 0;                      // reset error indicator
    char *num_string_check;  // for the strtol function
        
    if (argc == 4)
    {

        cache_size = strtol(argv[1], num_string_check, 10);

        if(cache_size < 0 || errno == ERANGE){
            printf("Error in cache size: number out of range\n");
            printf("Pick a number between 0 and 4294967295\n");
            printf("Process terminated\n");
            exit(1);
        }
        else if(*num_string_check != "\0"){
            printf("Error in cache size: non-numeric character detected\n");
            printf("Process terminated\n");
        }

        if(!is_valid_ip(argv[2])){
            printf("Error in personal IPv4 address: the address is invalid\n");
            printf("Process terminated\n");
            exit(1);
        }

        personal_addr = argv[2];
        
        if(strtol(argv[3], num_string_check, 10) < 0 || strtol(argv[3], num_string_check, 10) > 65535){
            printf("Error in personal port: number out of range\n");
            printf("Pick a number between 0 and 65535\n");
            printf("Process terminated\n");
            exit(1);
        }
        else if(*num_string_check != "\0"){
            printf("Error in personal port: non-numeric character detected\n");
            printf("Process terminated\n");
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
        cache_size = strtol(argv[1], num_string_check, 10);

        if(cache_size < 0 || errno == ERANGE){
            printf("Error in cache size: number out of range\n");
            printf("Pick a number between 0 and 4294967295\n");
            printf("Process terminated\n");
            exit(1);
        }
        else if(*num_string_check != "\0"){
            printf("Error in cache size: non-numeric character detected\n");
            printf("Process terminated\n");
        }

        if(!is_valid_ip(argv[2])){
            printf("Error in personal IPv4 address: the address is invalid\n");
            printf("Process terminated\n");
            exit(1);
        }
        
        personal_addr = argv[2];

        if(strtol(argv[3], num_string_check, 10) < 0 || errno == ERANGE){
            printf("Error in personal port: number out of range\n");
            printf("Pick a number between 0 and 4294967295 (65565 might be the MAXPORT)\n");
            printf("Process terminated\n");
            exit(1);
        }
        else if(*num_string_check != "\0"){
            printf("Error in personal port: non-numeric character detected\n");
            printf("Process terminated\n");
            exit(1);
        }

        personal_port = argv[3];

        // test format to see if it's an ip address or not

        if(is_valid_ip(argv[4])){

            server_addr = argv[4];

            printf("The node server's UDP port will default to %s\n", DEFAULT_REGUDP);
            server_port = DEFAULT_REGUDP;

        }else{ // if test fails
            
            if(strtol(argv[4], num_string_check, 10) < 0 || errno == ERANGE){
                printf("Error in node server's port: number out of range\n");
                printf("Pick a number between 0 and 4294967295 (65565 might be the MAXPORT)\n");
                printf("Process terminated\n");
                exit(1);
            }
            else if(*num_string_check != "\0"){
                printf("Error in node server's port: non-numeric character detected\n");
                printf("Process terminated\n");
                exit(1);
            }
            
            server_port = argv[4];

            printf("The node server's IPv4 address will default to %s\n", DEFAULT_REGIP);
            server_addr = DEFAULT_REGIP;

        }            
        
    } // if 4 arguments

    if (argc == 6)
    {

        cache_size = strtol(argv[1], num_string_check, 10);

        if(cache_size < 0 || errno == ERANGE){
            printf("Error in cache size: number out of range\n");
            printf("Pick a number between 0 and 4294967295\n");
            printf("Process terminated\n");
            exit(1);
        }
        else if(*num_string_check != "\0"){
            printf("Error in cache size: non-numeric character detected\n");
            printf("Process terminated\n");
        }

        if(!is_valid_ip(argv[2])){
            printf("Error in personal IPv4 address: the address is invalid\n");
            printf("Process terminated\n");
            exit(1);
        }

        personal_addr = argv[2];

        if(strtol(argv[3], num_string_check, 10) < 0 || errno == ERANGE){
            printf("Error in personal port: number out of range\n");
            printf("Pick a number between 0 and 4294967295 (65565 might be the MAXPORT)\n");
            printf("Process terminated\n");
            exit(1);
        }
        else if(*num_string_check != "\0"){
            printf("Error in personal port: non-numeric character detected\n");
            printf("Process terminated\n");
            exit(1);
        }
        personal_port = argv[3];

        if(!is_valid_ip(argv[2])){
            printf("Error in node server's IPv4 address: the address is invalid\n");
            printf("Process terminated\n");
            exit(1);
        }

        server_addr = argv[4];

        if(strtol(argv[4], num_string_check, 10) < 0 || errno == ERANGE){
            printf("Error in node server's port: number out of range\n");
            printf("Pick a number between 0 and 4294967295 (65565 might be the MAXPORT)\n");
            printf("Process terminated\n");
            exit(1);
        }
        else if(*num_string_check != "\0"){
            printf("Error in node server's port: non-numeric character detected\n");
            printf("Process terminated\n");
            exit(1);
        }
        server_port = argv[5];

    } // if 5 arguments

    
    printf("The application was launched successfuly\n\n");
    printf("\nType 'help' to show this help menu.\n\n");
    help_menu();    
    printf("\nIf the program stopped, it's waiting for activity:\n");    
    printf("\n_________________________________________________________________\n\n");    
    
    
    /* Initialize the node */

    struct personal_node *my_node = NULL;

    my_node = personal_init(my_node);

    strcpy(my_node->persn_info->node_addr, personal_addr);
    strcpy(my_node->persn_info->tcp_port, personal_port);

    my_node->udp_address = server_addr;
    my_node->udp_port = server_port;


    // Start the event loop

    int success_flag = 0;

    int errflag;                  // flag to check for errors in function calls
    socklen_t srv_addrlen;        // size of the personal IPv4 address
    struct addrinfo *srv_result;  // list of address structures
    struct addrinfo srv_criteria; // necessary criteria to select the address structures from the list
    struct sockaddr srv_addr;     // the IPv4 address

    struct timeval timeout;      // structure to set timeout value
    struct timeval *ptimeout;    // pointer to timeval struct
    int fd_itr = 0;              // an iterator to go through the FD set
    ssize_t nread;               // number of bytes read from a read operation
    ssize_t nleft;               // number of bytes left to fill the buffer capacity
    int new_fd;                  // a file descriptor to receive the "NEW" message and extract the id of the node
    char buffer[MAX_MSG_LENGTH]; // buffer to receive msg and cmd
    char *ptr_bffr;              // pointer to buffer, to use with read operation;
    int i = 0, j = 0;            // iterators
    int disc_nodeid = -1;         // id of the node that left the network
    int select_ctrl = 0;         // flag to control select()
    char msg[MAX_MSG_LENGTH];    // msg to be sent by tcp_client
 
    //Init

    my_node->server_fd = socket(AF_INET, SOCK_STREAM, 0); // personal server socket for the intern nodes
    if (my_node->server_fd < 0) {
        printf("Error in socket()\n");
        return ++success_flag;
        
    }

    my_node->max_fd = my_node->server_fd;

    memset(&srv_criteria, 0, sizeof(srv_criteria)); // set all bytes inside "criteria" to zero
    srv_criteria.ai_family = AF_INET;               // family of IPv4 addresses
    srv_criteria.ai_socktype = SOCK_STREAM;         // TCP socket
    srv_criteria.ai_flags = AI_PASSIVE;             // passive socket that waits for requests

    // pass the information in "criteria" to "srv_result" 

    errflag = getaddrinfo(NULL, my_node->persn_info->tcp_port, &srv_criteria, &srv_result);
    if (errflag != 0) {
        printf("Error in getaddrinfo()\n");
        return ++success_flag;
    }

    // bind the address in "srv_result" to the personal socket

    errflag = bind(my_node->server_fd, srv_result->ai_addr, srv_result->ai_addrlen);
    if (errflag == -1) {
        printf("Error in bind()\n");
        return ++success_flag;
    }

    freeaddrinfo(srv_result);
    // Set the personal socket to listen to requests. Only 6 requests at a time

    errflag = listen(my_node->server_fd, MAX_QUEUE_LENGTH_TCP); // the second argument refers to the number of pending connections allowed in the queue
    if (errflag == -1) {
        printf("Error in listen()\n");
        return ++success_flag;
    }

    FD_ZERO(&my_node->crr_scks);              // Set set of FD's to zero
    FD_SET(STDIN_FILENO, &my_node->crr_scks); // add stdin(keyboard input) to FD set
    FD_SET(my_node->server_fd, &my_node->crr_scks);           // add server socket to FD set

    // Infinite cycle where the communications happen

    ptimeout = NULL;    //select waits for a indeterminate amount of time

    

    while (1) {
    
        printf("\nChecking activity...\n\n");
        //In case old external neigbor disconnected, add new one to FD set
        if ((my_node->client_fd != -1) && (!FD_ISSET(my_node->client_fd, &my_node->crr_scks))) {
            FD_SET(my_node->client_fd, &my_node->crr_scks);
            if (my_node->client_fd > my_node->max_fd) my_node->max_fd = my_node->client_fd;
        }

        //remove all FDs with no activity from FD set my_node->rdy_scks
        my_node->rdy_scks = my_node->crr_scks;
    
        select_ctrl = select(my_node->max_fd + 1, &my_node->rdy_scks, (fd_set*)NULL, (fd_set*)NULL, (struct timeval*)NULL);

        if (select_ctrl < 0) {         
            printf("Error in select: %s. Terminating process\n", strerror(errno));
            return ++success_flag;    
        }
        
        //Go through all FDs and see which ones have activity
        for (fd_itr = 0; fd_itr < my_node->max_fd + 1; fd_itr++) {

            if (FD_ISSET(fd_itr, &my_node->rdy_scks)) {
                
                if (fd_itr == STDIN_FILENO) {
                    memset(&buffer, 0, sizeof(buffer)); //set the buffer to '\0'                                         
                    fgets(buffer, MAX_MSG_LENGTH-1, stdin);
                    printf("\n_________________________________________________________________\n");
                    printf("Handling your command...");
                    printf("\n_________________________________________________________________\n\n");

                    select_cmd(my_node, buffer);                     
                }
                else if (fd_itr == my_node->server_fd && strcmp(my_node->persn_info->network, "") != 0) { // atividade no servidor e estou na rede
                    srv_addrlen = sizeof(srv_addr);
                    if((new_fd = accept(my_node->server_fd, &srv_addr, &srv_addrlen)) == -1){
                        printf("Error in accept(). Request rejected\n");
                        //return 1;
                    }
                    
                    FD_SET(new_fd, &my_node->crr_scks);
                    if (new_fd > my_node->max_fd) my_node->max_fd = new_fd;
                    
                }
                
                else {
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
                        //return 1;
                    }

                    else if (nread == 0) {  //the fd is disconnected
                        
                        memset(&buffer, 0, sizeof(buffer));
                        
                        if (fd_itr == my_node->client_fd) {    //External neighbor disconnected

                            disc_nodeid = atoi(my_node->extern_node->node_id);

                            printf("External neighbor (Interface %s/%s) disconnected!\n", my_node->extern_node->node_addr, my_node->extern_node->tcp_port);
                            
                            my_node->client_fd = -1;   // Reset client_fd to -1

                            //send withdraw msg to all internal neighbors
                            if (my_node->n_neighbrs > 0) {
                                memset(msg, 0, sizeof(msg));
                                sprintf(msg, "%s %02d\n", withdraw_str, atoi(my_node->extern_node->node_id));
                                for (i = 0; i < MAX_INTERNALS; i++) {
                                    if (my_node->neighbrs[i] != NULL) {
                                        send_tcp(my_node, my_node->neighbrs[i], msg);
                                    }
                                }
                                sleep(0.5);
                            }
                            
                            if (strcmp(my_node->backup_node->node_id, my_node->persn_info->node_id) != 0) { //not an anchor                                                        
                                contact_copy(my_node->extern_node, my_node->backup_node);
                                memset(my_node->backup_node->node_id, 0, 3 * sizeof(*my_node->backup_node->node_id));
                                memset(my_node->backup_node->network, 0, 4 * sizeof(*my_node->backup_node->network));
                                memset(my_node->backup_node->tcp_port, 0, 6 * sizeof(*my_node->backup_node->tcp_port));
                                memset(my_node->backup_node->node_addr, 0, 50 * sizeof(*my_node->backup_node->node_addr));

                                memset(msg, 0, sizeof(msg));
                                sprintf(msg, "%s %s %s %s\n", new_str, my_node->persn_info->node_id, my_node->persn_info->node_addr, my_node->persn_info->tcp_port);
                                //Sends NEW message , receives EXTERN response and updates backup node
                                if(send_tcp(my_node, my_node->extern_node, msg) == 1){
                                    printf("Failed to send a message\n");
                                }

                                memset(msg, 0, sizeof(msg));
                                    sprintf(msg, "%s %s %s %s\n", extern_str, my_node->extern_node->node_id, 
                                        my_node->extern_node->node_addr, my_node->extern_node->tcp_port);
                                    for (j = 0; j < MAX_INTERNALS; j++) {
                                        if (my_node->neighbrs[j] != NULL) {
                                            send_tcp(my_node, my_node->neighbrs[j], msg);
                                        }
                                    }

                                if (strcmp(my_node->persn_info->node_id, my_node->backup_node->node_id) == 0) {
                                    my_node->anchorflag = 1;
                                }                            
                            }
                            else if (my_node->n_neighbrs > 0) { //anchor
                                for (i = 0; i < MAX_INTERNALS; i++) {
                                    if (my_node->neighbrs[i] != NULL) {
                                        contact_copy(my_node->extern_node, my_node->neighbrs[i]);
                                        my_node->client_fd = my_node->neighbrs_fd[i];

                                        memset(msg, 0, sizeof(msg));
                                        sprintf(msg, "%s %s %s %s\n", extern_str, my_node->extern_node->node_id, 
                                            my_node->extern_node->node_addr, my_node->extern_node->tcp_port);
                                        for (j = 0; j < MAX_INTERNALS; j++) {
                                            if (my_node->neighbrs[j] != NULL) {
                                                send_tcp(my_node, my_node->neighbrs[j], msg);
                                            }
                                        }
                                        free_contact(my_node->neighbrs[i]);
                                        my_node->neighbrs[i] = NULL;
                                        my_node->n_neighbrs--;
                                        my_node->neighbrs_fd[i] = -1;
                                        break;
                                    }
                                }
                            }
                            else {  //is now the only node in the network
                                my_node->anchorflag = 1;
                                contact_copy(my_node->extern_node, my_node->persn_info);
                                contact_copy(my_node->backup_node, my_node->persn_info);
                            }
                            
                        }
                        else{                     //Internal neighbor disconnected

                            int flag = 0;
                            //find the id of the node that corresponds to the disconnected fd
                            for (i = 0; i < MAX_INTERNALS; i++) {
                                if (my_node->neighbrs_fd[i] == fd_itr) {
                                    disc_nodeid = i;
                                    flag++;                                    
                                    break;
                                }
                            }
                            
                            if(flag == 1){
                                printf("Client: %02d, with FD: %d, disconnected!\n", i, fd_itr);
                                

                                if(my_node->neighbrs[disc_nodeid] != NULL){
                                    free_contact(my_node->neighbrs[disc_nodeid]);
                                    my_node->neighbrs[disc_nodeid] = NULL;
                                    my_node->n_neighbrs--;
                                    my_node->neighbrs_fd[disc_nodeid] = -1;
                                    
                                }
                                

                                //send WHITHDRAW msg
                                memset(msg, 0, sizeof(msg));
                                sprintf(msg, "%s %02d\n", withdraw_str, disc_nodeid);

                                if(send_tcp(my_node, my_node->extern_node, msg) == 1){
                                    printf("Failed to send a message\n"); //msg for external neighbor 
                                }
                                for (i = 0; i < MAX_INTERNALS; i++) {
                                    if (my_node->neighbrs[i] != NULL) {
                                        if(send_tcp(my_node, my_node->neighbrs[i], msg) == 1){//msg for internal neighbors
                                            printf("Failed to send a message\n");
                                        }     
                                    }
                                }
                            }            

                        }                               
                        my_node->route_tab[disc_nodeid] = -1;
                        printf("%02d was removed from the routing table as a destination\n", disc_nodeid);

                        for(int i = 0; i < MAX_INTERNALS; i++){
                        
                            if(my_node->route_tab[i] == disc_nodeid){
                                my_node->route_tab[i] = -1;
                            }
                        }
                        printf("%02d was removed from the routing table as a neighbor\n", disc_nodeid);
                        FD_CLR(fd_itr, &my_node->crr_scks);                                                        
                    }
                    else{                        
                        
                        if(rcv_tcp(my_node, buffer, &fd_itr) == 1){
                            printf("Failed to send %s\n", buffer);
                        }
                    }//else
                    ptimeout = NULL;
                }
            }//if
        }//for
    }//while

    if(tcp_server(my_node) == 1){ //caso haja algum erro no início desta função, sai suavemente 
        leave(my_node);
        free(my_node->neighbrs);    
        free_contact(my_node->persn_info);

        if(my_node->contents != NULL){
        
            objectQueue_t *queue_ptr; // pointer to go through the list
            objectQueue_t *aux;     // auxiliary pointer to delete elements in the lists
            queue_ptr = my_node->contents;
            
            while(queue_ptr != NULL){

                aux = queue_ptr;
                queue_ptr = queue_ptr->next;
                free(aux->string);
                free(aux);
                
            }                            
        }

        free(my_node);
        exit(1);
    } 
}//main()

   
    