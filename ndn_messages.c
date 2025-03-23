/************************************************************************************************
 * ndn_messages.c
 *
 * Authors:
 *
 * -Igor Paruque ist1102901
 * -Mónica Ferreira ist106520
 *
 * Description: source code for the messages
 ***********************************************************************************************/

#define _XOPEN_SOURCE 600 //!REMOVE BEFORE COMPILING

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
#include "ndn_messages.h"
#include "ndn_interestTable.h"
#include "ndn_objectStructs.h"


 char *node_reg(char *server_IP, char *server_UDP, char *node_IP, char *node_TCP, char *net){ // register the nod    

    struct addrinfo srv_criteria, *srv_result;
    int fd, errflag;
    ssize_t cnt;
    struct sockaddr_in addr_conf;
    socklen_t addrlen_conf;
    char *buffer = NULL;
    if((buffer = (char*)calloc(strlen(okreg_str) + 1, sizeof(char))) == NULL){
        printf("Error in send_safe: Failed to allocate memory. process terminated\n");
        exit(1);
    }
    // OKREG (5) + \0 (1) = 6
    char reg_msg[69]; // 5(REG) + 1(space) + 3(net) + 1(space) + 50(IP) + 1(space) + 6(TCP) + 1(\n) + 1(\0) = 19 + strlen(IP)
    memset(reg_msg, 0, 69*sizeof(char));

    fd = socket(AF_INET, SOCK_DGRAM, 0) ;
    if(fd == -1) return NULL;

    memset(&srv_criteria, 0, sizeof(srv_criteria));
    srv_criteria.ai_family = AF_INET;
    srv_criteria.ai_socktype = SOCK_DGRAM;

    errflag = getaddrinfo(server_IP,server_UDP, &srv_criteria, &srv_result);
    if(errflag != 0) return NULL;

    sprintf(reg_msg, "%s %s %s %s\n", reg_str, net, node_IP, node_TCP);

    cnt = sendto(fd, reg_msg, strlen(reg_msg), 0, srv_result->ai_addr, srv_result->ai_addrlen); // Send reg
    if(cnt == -1) return NULL;

    printf("Registration request sent to the node server: %s\n\n", reg_msg);

    freeaddrinfo(srv_result);

    addrlen_conf = sizeof(struct sockaddr_in);
    cnt = recvfrom(fd, buffer, strlen(okreg_str)+1, 0,(struct sockaddr *)&addr_conf, &addrlen_conf); // recieve okreg
    if(cnt == -1){
        printf("Error in node_reg: Failed to receive confirmation from the server\n");
        return NULL;
    }
         
    printf("Response from the node server: %s\n", buffer);

    close(fd);
    return buffer;
 }

 char *node_unreg(char *server_IP, char *server_UDP, char *node_IP, char *node_TCP, char *net){ // unregister the node    

    struct addrinfo srv_criteria, *srv_result;
    int fd, errflag;
    ssize_t cnt;
    struct sockaddr_in addr_conf;
    socklen_t addrlen_conf;
    char *buffer = NULL;
    if((buffer = (char*)calloc(MAX_MSG_LENGTH, sizeof(char))) == NULL){
        printf("Error in send_safe: Failed to allocate memory. process terminated\n");
        exit(1);
    }
    // OKUNREG (5) + \0 (1) = 6
    char unreg_msg[71]; // 7(REG) + 1(space) + 3(net) + 1(space) + 50(IP) + 1(space) + 6(TCP) + 1(\n) + 1(\0) = 19 + strlen(IP)
    memset(unreg_msg, 0, 71*sizeof(char));

    fd = socket(AF_INET, SOCK_DGRAM, 0) ;
    if(fd == -1) return NULL;

    memset(&srv_criteria, 0, sizeof(srv_criteria));
    srv_criteria.ai_family = AF_INET;
    srv_criteria.ai_socktype = SOCK_DGRAM;

    errflag = getaddrinfo(server_IP,server_UDP, &srv_criteria, &srv_result);
    if(errflag != 0) return NULL;

    sprintf(unreg_msg, "%s %s %s %s\n", unreg_str, net, node_IP, node_TCP);

    cnt = sendto(fd, unreg_msg, strlen(unreg_msg), 0, srv_result->ai_addr, srv_result->ai_addrlen);
    if(cnt == -1) return NULL;

    printf("Unregistration request sent to the node server: %s\n\n", unreg_msg);

    freeaddrinfo(srv_result);

    addrlen_conf = sizeof(struct sockaddr_in);
    cnt = recvfrom(fd, buffer, strlen(okunreg_str)+1, 0,(struct sockaddr *)&addr_conf, &addrlen_conf); // retrieve the same 6 bytes you allocated for the response
    if(cnt == -1){
        printf("Error in node_reg: Failed to receive confirmation from the server\n");
        return NULL;
    } 
    
    printf("Response from the node server: %s\n", buffer);

    close(fd);
    return buffer;
 }

 
nodesLinkedlist_t *server_inquiry(char *server_IP, char *server_UDP, char *net){// request the list of nodes in the network

    struct addrinfo srv_criteria, *srv_result;
    int fd, errflag, counter = 0;
    ssize_t cnt = 0;
    struct sockaddr_in addr_conf;
    socklen_t addrlen_conf;
    char nodeslist_bff[MAX_NODESLIST]; memset(nodeslist_bff, 0, MAX_NODESLIST);
    
    nodesLinkedlist_t *serverlist = NULL; serverlist = Listinit(serverlist);    
    nodeinfo_t *temp = NULL; temp = contact_init(temp);
    
    
    char *inquiry = NULL; 
    if((inquiry = (char*)calloc(11, sizeof(char))) == NULL){ // 5(nodes) + 1(space) + 3(net) + 1(\n) + 1(\0) = 11 
        
        printf("Error in server_inquiry: failed to allocate memory. Process terminated\n");
        exit(1);
    }
    

    char *first_line = NULL;           
       
    char delim[2] = "\n";
    char *one_line;

    fd = socket(AF_INET, SOCK_DGRAM, 0) ;
    if(fd == -1){
        return NULL;
        free(first_line);        
        free(inquiry);
        free_contact(&temp);
    } 
    
    memset(&srv_criteria, 0, sizeof(srv_criteria));
    srv_criteria.ai_family = AF_INET;
    srv_criteria.ai_socktype = SOCK_DGRAM;

    errflag = getaddrinfo(server_IP,server_UDP, &srv_criteria, &srv_result);
    if(errflag != 0) return NULL;

    sprintf(inquiry, "%s %s\n", nodes_str, net);

    cnt = sendto(fd, inquiry, strlen(inquiry), 0, srv_result->ai_addr, srv_result->ai_addrlen);
    if(cnt == -1){

        close(fd);
        free(first_line);                
        free(inquiry);
        free_contact(&temp);
        return NULL;
    } 

    printf("Inquiry sent to the node server: %s %s\n\n", nodes_str, net);

    freeaddrinfo(srv_result);

    addrlen_conf = sizeof(struct sockaddr_in);
    
    cnt = recvfrom(fd, nodeslist_bff, MAX_NODESLIST, 0,(struct sockaddr *)&addr_conf, &addrlen_conf);
    
    if(cnt == -1){
       
        printf("Failed to get the list of nodes from the server\n");
        
        close(fd);               
        free(inquiry);
        free_contact(&temp);
        return NULL;
    }
    first_line = strtok(nodeslist_bff,delim);
    printf("Response from the node server\n\n");
    printf("%s\n\n", first_line);

    while((one_line = strtok(NULL, delim)) != NULL){

        if((sscanf(one_line, "%15s %5s", temp->node_addr, temp->tcp_port)) == 2){

            serverlist = insertnode(serverlist, temp);
            counter++;
            printf("%s %s\n", temp->node_addr, temp->tcp_port);                    
        }else{
            printf("Failed to read one line of NODESLIST\n");
        }                
    }

    if(counter == 0){ // serverlist is null
        printf("Zero nodes reported\n");
        close(fd);                
        free(inquiry);
        free_contact(&temp);
        if((serverlist = (nodesLinkedlist_t*)calloc(1, sizeof(nodesLinkedlist_t))) == NULL){
            printf("Error in server_inquiry: failed to allocate memory. Process terminated\n");
            exit(1);
        }
        serverlist->node = contact_init(serverlist->node);
        return serverlist;
        
    }

    close(fd);    
    free(inquiry);
    free_contact(&temp);
    return serverlist;
}



char *send_entry(int *fd, char *mynode_ip, char *mynode_tcp, char *dest_ip, char *dest_tcp){
    
    //On server side, the node binds the socket to the desired ip and port
    //on client side, you use the desired ip and a random port given by the OS
    // The intern node receives messages in a random port
    // The extern node receives messages in the desired port
    
    struct addrinfo srv_criteria, *srv_result;    
    int errflag;
    ssize_t nbytes, nleft, nwritten;
    char *ptr;
    char *buffer = NULL;
    if((buffer = (char*)calloc(MAX_MSG_LENGTH, sizeof(char))) == NULL){
        printf("Error in send_safe: Failed to allocate memory. process terminated\n");
        exit(1);
    }
    struct sigaction act;

    // struct timeval timeout;
    // timeout.tv_sec = 5;  // 5-second timeout
    // timeout.tv_usec = 0;
    

    if (*fd == -1) {
        *fd = socket(AF_INET, SOCK_STREAM, 0);
        if(*fd == -1){
            printf("Send entry: Error initializing socket.\n");
            free(buffer);
            return NULL;
        }
        //setsockopt(*fd, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout));
        
        memset(&act, 0, sizeof(act));
        act.sa_handler = SIG_IGN;

        if(sigaction(SIGPIPE, &act, NULL) == -1){
            printf("Send entry: Error in sigaction().\n");
            close(*fd);
            *fd = -1;
            free(buffer);            
            return NULL;
        }

        memset(&srv_criteria, 0, sizeof(srv_criteria));
        srv_criteria.ai_family = AF_INET;       //IPv4 address family
        srv_criteria.ai_socktype = SOCK_STREAM; // TCP socket
        
        errflag = getaddrinfo(dest_ip, dest_tcp, &srv_criteria, &srv_result);
        if(errflag != 0){
            printf("Send entry: Error in getaddrinfo()\n");
            close(*fd);
            *fd = -1;
            free(buffer);
            freeaddrinfo(srv_result);
            return NULL;
        }

        errno = 0;
        errflag = connect(*fd, srv_result->ai_addr, srv_result->ai_addrlen);
        if(errflag == -1){
            printf("Send entry: Error in connect(): %s\n", strerror(errno));
            close(*fd);
            *fd = -1;
            free(buffer);
            freeaddrinfo(srv_result);
            return NULL;
        }

        freeaddrinfo(srv_result);
    }

    sprintf(buffer, "%s %s %s\n", entry_str, mynode_ip, mynode_tcp);
    ptr = buffer;
    nbytes = strlen(buffer);
    nleft = nbytes;

    while(nleft > 0){
        nwritten = write(*fd, ptr, nleft);
        if(nwritten <= 0){
            printf("Send entry: Error in write\n");
            close(*fd);
            *fd = -1;
            free(buffer);
            return NULL;
        }
        nleft -= nwritten;
        ptr += nwritten;
    }
   
    memset(buffer, 0, MAX_MSG_LENGTH * sizeof(char)); //set the buffer to '\0' //!lembrar
    strcpy(buffer, "1"); //indicates that the message was sent
   
    return buffer; // returning what was inside the file descriptor

}

char *send_safe(int fd, char *ext_ip, char *ext_tcp){

    char *ptr;
    char *buffer = NULL;
    if((buffer = (char*)calloc(MAX_MSG_LENGTH, sizeof(char))) == NULL){
        printf("Error in send_safe: Failed to allocate memory. process terminated\n");
        exit(1);
    }
    ssize_t nbytes, nleft, nwritten;

    sprintf(buffer, "%s %s %s\n", safe_str, ext_ip, ext_tcp);
    ptr = buffer;
    nbytes = strlen(buffer);
    nleft = nbytes;

    while(nleft > 0){
        nwritten = write(fd, ptr, nleft);
        if(nwritten <= 0){
            printf("Send safe: Error in write\n");
            close(fd);
            free(buffer);            
            return NULL;
        }
        nleft -= nwritten;
        ptr += nwritten;
    }
   
    memset(buffer, 0, MAX_MSG_LENGTH * sizeof(char)); //set the buffer to '\0' //!lembrar
    strcpy(buffer, "1"); //indicates that the message was sent
   
    return buffer; // returning what was inside the file descriptor

}



void send_interest(int fd, char *object_name, struct personal_node *slf_node) {
    
    char *ptr;
    char *buffer = NULL;

    // Allocate Buffer's Memory
    if ((buffer = (char *)calloc(MAX_MSG_LENGTH, sizeof(char))) == NULL) {
        printf("Error in send_interest: Failed to allocate memory. Process terminated.\n");
        exit(1);
    }

    ssize_t nbytes, nleft, nwritten;
    sprintf(buffer, "%s %s\n", interest_msg_str, object_name);
    nbytes = strlen(buffer);

    

    nodesLinkedlist_t *current = slf_node->internals_list;
    while (current != NULL) {
        int internal_fd = current->node->node_fd;

        // Ignore the node that sent the interest
        if (internal_fd != fd) {
            ptr = buffer;
            nleft = nbytes;

            // Send the message
            while (nleft > 0) {
                nwritten = write(fd, ptr, nleft);
                if (nwritten <= 0) {
                    printf("Error in send_interest: Failed to write to socket for node [%s | %s].\n",
                           current->node->node_addr, current->node->tcp_port);
                    break;
                }
                nleft -= nwritten;
                ptr += nwritten;
            }

            if (nleft == 0) {
                printf("Interest sent to internal node [%s | %s]: %s\n",
                       current->node->node_addr, current->node->tcp_port, object_name);
                
                // If interest is not in the table
                if (!search_interest(slf_node->interest_table, object_name)) {
                    // Add interest
                    add_interest(slf_node->interest_table, object_name, WAITING);
                }
                // Set interface to WAITING
                update_interface_state(slf_node->interest_table, object_name, internal_fd, WAITING);

            }
        }

        current = current->next;
    }

    free(buffer);
    

    return;
}

char *send_object(int fd, char *object_name, struct personal_node *slf_node) {
    
    char *ptr;
    char *buffer = NULL;

    // Allocate Buffer's Memory
    if ((buffer = (char *)calloc(MAX_MSG_LENGTH, sizeof(char))) == NULL) {
        printf("Error in send_object: Failed to allocate memory. Process terminated.\n");
        exit(1);
    }

    ssize_t nbytes, nleft, nwritten;
    
    sprintf(buffer, "%s %s\n", object_str, object_name);
    ptr = buffer;
    nbytes = strlen(buffer);
    nleft = nbytes;

    // Send the message
    while (nleft > 0) {
        nwritten = write(fd, ptr, nleft);
        if (nwritten <= 0) {
            printf("Send Object: Error in write\n");
            free(buffer);
            return NULL;
        }
        nleft -= nwritten;
        ptr += nwritten;
    }

    update_interface_state(slf_node->interest_table, object_name, fd, ANSWER);
    

    return buffer; // returning what was inside the file descriptor
}

char *send_noobject(int fd, char *object_name, struct personal_node *slf_node) {
    
    char *ptr;
    char *buffer = NULL;

    // Allocate Buffer's Memory
    if ((buffer = (char *)calloc(MAX_MSG_LENGTH, sizeof(char))) == NULL) {
        printf("Error in send_noobject: Failed to allocate memory. Process terminated.\n");
        exit(1);
    }

    ssize_t nbytes, nleft, nwritten;
    
    sprintf(buffer, "%s %s\n", noobject_str, object_name);
    ptr = buffer;
    nbytes = strlen(buffer);
    nleft = nbytes;

    // Send the message
    while (nleft > 0) {
        nwritten = write(fd, ptr, nleft);
        if (nwritten <= 0) {
            printf("Send Object: Error in write\n");
            free(buffer);
            return NULL;
        }
        nleft -= nwritten;
        ptr += nwritten;
    }

    // Close the interface that is sending the NOOBJECT message
    update_interface_state(slf_node->interest_table, object_name, fd, CLOSED);
    
    // Close all interfaces that are waiting for the object
    for (int i = 0; i < MAX_ENTRIES; i++) {
        if (slf_node->interest_table->entries[i]->active &&
            strcmp(slf_node->interest_table->entries[i]->name, object_name) == 0) {
            for (int j = 0; j < MAX_INTERFACES; j++) {

                // Check if the interface is waiting for the object
                if (slf_node->interest_table->entries[i]->interfaces[j] == WAITING) {

                    // Send NOOBJECT to the interface
                    ptr = buffer;
                    nleft = nbytes;

                    while (nleft > 0) {
                        nwritten = write(j, ptr, nleft);
                        if (nwritten <= 0) {
                            printf("Error in send_noobject: Failed to forward NOOBJECT to interface [%d].\n", j);
                            break;
                        }
                        nleft -= nwritten;
                        ptr += nwritten;
                    }
                    // Set the interface to CLOSED
                    update_interface_state(slf_node->interest_table, object_name, j, CLOSED);
                }
            }
            break; // Found the entry, no need to continue
        }
    }
    
    
    memset(buffer, 0, MAX_MSG_LENGTH * sizeof(char)); //set the buffer to '\0' //!lembrar
    strcpy(buffer, "1"); //indicates that the message was sent

    return buffer; // returning what was inside the file descriptor
}





int parse_tcp(struct personal_node *slf_node, char *msg, nodeinfo_t *src_node){

    int fail_flag = 0;

    char cmd[MAX_MSG_CMD_SIZE];         //message command of msg
    memset(cmd, 0, sizeof(cmd));    
        
    char tcp_cmd[MAX_TCP_UDP_CHARS];    //tcp port of msg
    memset(tcp_cmd, 0, sizeof(tcp_cmd));

    char ip_cmd[MAX_ADDRESS_SIZE];      //ip address of msg
    memset(ip_cmd, 0, sizeof(ip_cmd));

    char object_buff[MAX_NAME_LENGTH];  //object of msg
    memset(object_buff, 0, sizeof(object_buff));

    char snd_msg[MAX_MSG_LENGTH];       //buffer to send a response 
    memset(snd_msg, 0, sizeof(snd_msg));

    char snd_cmd[MAX_MSG_CMD_SIZE];     //the message command of our response 
    memset(snd_cmd, 0, sizeof(snd_cmd));

    char *return_msg = NULL;            //return from all the send commands

    nodesLinkedlist_t *aux;

    //get commnand
    if (sscanf(msg, "%8s", cmd) != 1) {
        printf("Error in parse_tcp: Failed to read message type\n");
        return ++fail_flag;
    }

    //try to read and execute command

    if(strcmp(cmd, interest_msg_str) == 0){

        if(sscanf(msg, "%*s %100s",object_buff) == 1){

            printf("Received INTEREST: %s\n", object_buff);
            // search the object in the cache
            if (queueSearch(slf_node->queue_ptr, object_buff)) {
                // object in cache, send object message
                return_msg = send_object(src_node->node_fd, object_buff, slf_node);
                if (return_msg != NULL) {
                    free(return_msg);
                    return_msg = NULL;
                }
            } else {

                // object not in cache...
                if (has_neighbors(slf_node->internals_list)) {
                    // ...and you have other neighbors
                    if (!search_interest(slf_node->interest_table, object_buff)) {
                        // ...and not in interest table
                        add_interest(slf_node->interest_table, object_buff, WAITING);
                        // send interest message to neighbors
                        send_interest(src_node->node_fd, object_buff, slf_node);
                    }
                    else {
                        // Interest already in the table
                        // Update a interface para ANSWER
                        // so that we know that we have to send the object through this interface
                        int waiting_interface = search_waiting_interface(slf_node->interest_table, object_buff);
                        if (waiting_interface != -1) {
                            update_interface_state(slf_node->interest_table, object_buff, waiting_interface, ANSWER);
                        }
                    }
                
                } else {
                    // ...and you have no other neighbors, send noobject message to the source
                    return_msg = send_noobject(src_node->node_fd, object_buff, slf_node);
                    if (return_msg != NULL) {
                        free(return_msg);
                        return_msg = NULL;
                    }
                }
            }
        }else{
        
            printf("Error in parse_tcp: Failed to read arguments of %s\n", cmd);
            return ++fail_flag;
        }                       
            
    }

    else if(strcmp(cmd, object_str) == 0){

        if(sscanf(msg, "%*s %100s", object_buff) == 1){
            
            printf("Received OBJECT: %s\n", object_buff);
            // check interest table to see if someone is waiting for an answer from me 
            int waiting_interface = search_waiting_interface(slf_node->interest_table, object_buff);
            if (waiting_interface != -1) {
                // if someone is waiting for the object
                // send object message to that node
                return_msg = send_object(waiting_interface, object_buff, slf_node);
                if (return_msg != NULL) {
                    free(return_msg);
                    return_msg = NULL;
                }

                // and place the object in cache
                insertNew(slf_node->queue_ptr, object_buff);

                // remove interest from table
                remove_interest(slf_node->interest_table, object_buff);
            }
            else {
                // if no one is waiting for the object
                // place the object in cache
                insertNew(slf_node->queue_ptr, object_buff);
                //ou printf("No interfaces waiting for OBJECT: %s\n", object_buff);
            }
        }
        else{
            printf("Error in parse_tcp: Failed to read arguments of %s\n", cmd);
            return ++fail_flag;
        }
    }

    else if(strcmp(cmd, noobject_str) == 0){

        if(sscanf(msg, "%*s %100s",object_buff) == 1){
            
            printf("Received NOOBJECT: %s\n", object_buff);

            // Send NOOBJECT to all interfaces that are waiting for the object
            return_msg = send_noobject(src_node->node_fd, object_buff, slf_node);
            if (return_msg != NULL) {
                free(return_msg);
                return_msg = NULL;
            }

            // check if all interfaces are closed
            if (all_interfaces_closed(slf_node->interest_table, object_buff)) {
                // remove interest from table
                remove_interest(slf_node->interest_table, object_buff);
            }
        }
    

        else {
            printf("Error in parse_tcp: Failed to read arguments of %s\n", cmd);
            return ++fail_flag;
        }
    }
       
    else if (strcmp(cmd, entry_str) == 0) {

        if (sscanf(msg, "%*s %15s %5s", ip_cmd, tcp_cmd) == 2){
            
            printf("\nMessage received from a new intern neighbor:\n");
            printf("%s\n\n", msg);

            if (strcmp(slf_node->extern_node->node_addr, "") == 0) {  //I was alone in the network. Answer with SAFE, followed by an ENTRY

                //src_node is the new intern node
                
                strcpy(src_node->tcp_port, tcp_cmd);
                strcpy(src_node->node_addr, ip_cmd);
                                                              

                printf("Sending %s %s %s\n\n", safe_str, ip_cmd, tcp_cmd);
                return_msg = send_safe(src_node->node_fd, ip_cmd, tcp_cmd);
                
                if (return_msg != NULL) {

                    printf("\nMessage sent to [%s | %s]:\n", ip_cmd, tcp_cmd);
                    printf("%s %s %s\n\n", safe_str, ip_cmd, tcp_cmd);
                    
                    free(return_msg);
                    return_msg = NULL;                        
                                                   
                }
                else{
                    printf("Error in parse_tcp.\n");
                    printf("Error accepting new internal: Failed to send SAFE message. Connection closed\n");
                    slf_node->internals_list = removenode(slf_node->internals_list, src_node->node_fd);
                    slf_node->n_internals--;
                    close(src_node->node_fd);
                    FD_CLR(src_node->node_fd, &(slf_node->crr_scks));
                    
                    return ++fail_flag;                        
                }

                printf("Sending %s %s %s\n\n", entry_str, slf_node->personal_addr, slf_node->personal_tcp);
                
                return_msg = send_entry(&(src_node->node_fd), slf_node->personal_addr, slf_node->personal_tcp,
                                        ip_cmd, tcp_cmd);
                
                if (return_msg != NULL) {

                    printf("\nMessage sent to [%s | %s]:\n", ip_cmd, tcp_cmd);
                    printf("%s %s %s\n\n", entry_str, slf_node->personal_addr, slf_node->personal_tcp);
                    
                    free(return_msg);
                    return_msg = NULL;                        
                                                  
                }
                else{
                    printf("Error in parse_tcp.\n");
                    printf("Error connecting to new external: Failed to send ENTRY message. Connection closed\n");
                    slf_node->internals_list = removenode(slf_node->internals_list, src_node->node_fd);
                    slf_node->n_internals--;
                    close(src_node->node_fd);
                    FD_CLR(src_node->node_fd, &(slf_node->crr_scks));
                    return ++fail_flag;                        
                }
                strcpy(slf_node->extern_node->tcp_port, tcp_cmd);
                strcpy(slf_node->extern_node->node_addr, ip_cmd);
                slf_node->extern_node->node_fd = src_node->node_fd;                
                slf_node->anchorflag = 1; 

            }
            else {
                
                strcpy(src_node->tcp_port, tcp_cmd);
                strcpy(src_node->node_addr, ip_cmd);                

                // if ENTRY came from my extern, put him in the interns list

                if((strcmp(slf_node->extern_node->node_addr, ip_cmd) == 0) && 
                   (strcmp(slf_node->extern_node->tcp_port, tcp_cmd) == 0)){
                    
                    slf_node->internals_list = insertnode(slf_node->internals_list, slf_node->extern_node);
                    slf_node->n_internals++;
                }
                
                printf("\nSending %s %s %s\n\n", safe_str, slf_node->extern_node->node_addr, slf_node->extern_node->tcp_port);
                return_msg = send_safe(src_node->node_fd, slf_node->extern_node->node_addr, slf_node->extern_node->tcp_port);
                
                if (strcmp(return_msg, "1") == 0) {
                    printf("\nMessage sent to [%s | %s]:\n", ip_cmd, tcp_cmd);
                    printf("%s %s %s\n\n", safe_str, slf_node->extern_node->node_addr, slf_node->extern_node->tcp_port);
                    
                    if (return_msg != NULL){ //reset the pointer to the received message
                        free(return_msg);
                        return_msg = NULL;
                        return fail_flag;
                    }                                
                }
                else{
                    printf("Error in parse_tcp: Communication with TCP server failed.\n");
                    if (return_msg != NULL) { //reset the pointer to the received message
                        free(return_msg);
                        return_msg = NULL;
                        printf("Error accepting new internal: Failed to send SAFE message. Connection closed\n");
                        slf_node->internals_list = removenode(slf_node->internals_list, src_node->node_fd);
                        close(src_node->node_fd);
                        FD_CLR(src_node->node_fd, &(slf_node->crr_scks));
                    }
                    return ++fail_flag;                        
                }
            }                        
        }
        else {
            printf("Error in parse_tcp: Failed to read arguments of %s\n", cmd);
            return ++fail_flag;
        }
    }

    else if(strcmp(cmd, safe_str) == 0){

        if(sscanf(msg, "%*s %15s %5s",ip_cmd, tcp_cmd) == 2){

            if(src_node->node_fd == slf_node->extern_node->node_fd){ // The message reached me through the extern node
                
                
                printf("\nMessage received from [%s | %s]:\n", slf_node->extern_node->node_addr, slf_node->extern_node->tcp_port);
                printf("%s\n\n", msg);                
                 
            }            
            else{
                // entrance of the 2nd node, while im the first (beginning of the list) or
                // anchor node disappeared, while im the other anchor, and I had to choose someone to be the 2nd anchor (anywhere in the list)
                
                aux = slf_node->internals_list;
                while(aux != NULL){
                            
                    if(aux->node->node_fd == src_node->node_fd){
                        printf("Internal node converted to external: [%s | %s]\n\n", aux->node->node_addr, aux->node->tcp_port);
                        break;
                    }
                    aux = aux->next;
                }                                  
            }        

            printf("Updating the backup neighbor...\n\n");
                        
            memset(slf_node->backup_tcp, 0, MAX_TCP_UDP_CHARS * sizeof(*slf_node->backup_tcp));
            memset(slf_node->backup_addr, 0, MAX_ADDRESS_SIZE * sizeof(*slf_node->backup_addr));
                       
            strcpy(slf_node->backup_tcp, tcp_cmd);
            strcpy(slf_node->backup_addr, ip_cmd);

            if((strcmp(slf_node->backup_addr, slf_node->personal_addr)) == 0 &&
                (strcmp(slf_node->backup_tcp, slf_node->personal_tcp)) == 0){

                slf_node->anchorflag = 1;
                
            }

            printf("New backup neighbor:\n\n");                    
            printf("Adress: %s\nPort: %s\n\n",                         
            slf_node->backup_addr, 
            slf_node->backup_tcp);            
            
        }
        else{
            printf("Error in parse_tcp: Failed to read arguments of %s\n", cmd);
            return ++fail_flag;
        }
    } 
    
    else{
        printf("Error in parse_tcp: Failed to read command!\n");
        return ++fail_flag;
    }

    if (return_msg != NULL) {
        free(return_msg);
        return_msg = NULL;
    }
    return fail_flag;
}

char *parseNstore(char msg_bffr[], char **node_bffr){
    
    char *str_ptr = NULL;
    const char delim[2] = "\n";
    char *token = NULL;
    char *one_cmd; 
    if((one_cmd = (char*)calloc(MAX_MSG_LENGTH, sizeof(char))) == NULL){
        
        printf("Failed to allocate memory in parseNstore. Process terminated\n");
        exit(1);
    }   
    memset(one_cmd, 0, MAX_MSG_LENGTH);
    char cmds_left[MAX_MSG_LENGTH]; memset(cmds_left, 0, MAX_MSG_LENGTH);
    int msg_found = 0;


    if((strcmp(*node_bffr, "")) != 0){  // if the node's buffer has content, read it
                
        if(((str_ptr = strchr(*node_bffr, '\n')) != NULL) && ((token=strtok(*node_bffr,delim)) != NULL)){  // if there's a final part of a message, store it
            
            msg_found = 1;
            snprintf(one_cmd, MAX_MSG_LENGTH,"%s",token);
            
            //strtok only replaces the next occurence (from left to right) of the \n

            while((str_ptr = strchr(str_ptr + 1, '\n')) != NULL){
                
                // You still have full messages to extract
                
                if((token = strtok(NULL, delim)) != NULL){
            
                    //extract the remainder and put it back where it was, along with the line feed characters
                    strcat(cmds_left, token);
                    strcat(cmds_left, "\n");
                }
            }
            if((token = strtok(NULL, delim)) != NULL){ 
                
                //if there is still something to extract after strchr returns NULL, 
                //extract the remainder and put it back where it was, along with the line feed characters

                strcat(cmds_left, token);                
             
            }
            strcpy(*node_bffr, cmds_left);// keep the remainder
                        
            
        }
        else{
            // if not, put the beginning of the message in one_cmd and
            // try to concatenate with the contents from the file descriptor
            // this guy shouldn't have the \n character

            snprintf(one_cmd, strlen(*node_bffr),"%s",*node_bffr);
            memset(*node_bffr, 0, MAX_MSG_LENGTH);
        }         
    
        if(msg_found == 1){ 
            
            // The next command was found and 
            // the node's buffer was preserved
            //Return the command

            return one_cmd;

        }
        //concatenate the contents in the file descriptor with the contents in the buffer
    }
                       
    
    //Find msgs in msg_buffer

    if((strcmp(msg_bffr, "")) != 0){  // if the message buffer has content, read it
        if (((str_ptr = strchr(msg_bffr, '\n')) != NULL) && ((token = strtok(msg_bffr,delim)) != NULL)) { // \n found
        
            strcat(one_cmd, token); // here, token should be the missing half of one_cmd
            msg_found = 1;

            //strtok modifies *node_bffr, we need to restore it

            while((str_ptr = strchr(str_ptr + 1, '\n')) != NULL){ 
                
                if((token = strtok(NULL, delim)) != NULL){
            
                    //extract the remainder and put it back where it was, along with the line feed characters
                    strcat(cmds_left, token);
                    strcat(cmds_left, "\n");
                }
            }
            if((token = strtok(NULL, delim)) != NULL){ //after strchr returns NULL, there is still the unfinished message to extract
            
                //extract the remainder and put it back where it was, along with the line feed characters
                strcat(cmds_left, token);
                    
            }
                        
            strcpy(*node_bffr, cmds_left);// keep the remainder            
        }
        else{
            
            // no message found
            // store the remainder in the node's buffer and return NULL
            strcat(one_cmd, msg_bffr); 
            strcpy(*node_bffr, one_cmd);
            return NULL;
            
        }
    }
    if(msg_found == 1) return one_cmd;
    free(one_cmd);
    return NULL;
                             
}