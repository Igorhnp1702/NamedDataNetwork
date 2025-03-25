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
#include <string.h>

// networking libraries
#include <netdb.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <signal.h>


// project libraries
#include "ndn_objectStructs.h"
#include "ndn_interestTable.h"
#include "ndn_node.h"
#include "ndn_messages.h"





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



void send_interest(int fd2avoid, char *object_name, struct personal_node *slf_node) {
    
    char *ptr;
    char *buffer = NULL;

    // Allocate Buffer's Memory
    if ((buffer = (char *)calloc(MAX_MSG_LENGTH, sizeof(char))) == NULL) {
        printf("Error in send_interest: Failed to allocate memory. Process terminated.\n");
        exit(1);
    }

    // Initialize variables to use for write function
    ssize_t nbytes, nleft, nwritten;
    sprintf(buffer, "%s %s\n", interest_msg_str, object_name);
    nbytes = strlen(buffer);

    //Send interest to everybody, except the node with fd2avoid as the file descritpor

    nodesLinkedlist_t *current;

    //If fd2avoid == -1, the retrieve command was used and I dont have the object, send to everybody

    if(fd2avoid == -1){

        // send to the extern node

        ptr = buffer;
        nleft = nbytes;

        while (nleft > 0) {
            nwritten = write(slf_node->extern_node->node_fd, ptr, nleft);
            if (nwritten <= 0) {
                printf("Error in send_interest: Failed to write to socket for node [%s | %s].\n",
                    slf_node->extern_node->node_addr, slf_node->extern_node->tcp_port);
                break;
            }
            nleft -= nwritten;
            ptr += nwritten;
        }
        
        if (nleft == 0) {
            printf("Message sent to [%s | %s]:\n",
                slf_node->extern_node->node_addr, slf_node->extern_node->tcp_port);
            printf("%s\n\n", buffer);
            
            // Insert interest entry and set the interface to WAITING
            if (!search_interest(slf_node->interests_ptr, slf_node->extern_node, object_name, WAITING)) {
                // Add interest
                slf_node->interests_ptr = add_interest(slf_node->interests_ptr, slf_node->extern_node, object_name, WAITING);
            }           
        }
    
        // send to all other nodes
        current = slf_node->internals_list;
        while(current != NULL){

            if(current->node->node_fd != slf_node->extern_node->node_fd){

                // this intern is different from the extern node

                ptr = buffer;
                nleft = nbytes;

                while (nleft > 0) {
                    nwritten = write(current->node->node_fd, ptr, nleft);
                    if (nwritten <= 0) {
                        printf("Error in send_interest: Failed to write to socket for node [%s | %s].\n",
                            slf_node->extern_node->node_addr, slf_node->extern_node->tcp_port);
                        break;
                    }
                    nleft -= nwritten;
                    ptr += nwritten;
                }
                
                if (nleft == 0) {
                    printf("Interest sent to [%s | %s]:\n", current->node->node_addr, current->node->tcp_port);
                    printf("%s\n\n", buffer);
                                            
                    // If interest is not in the table
                    if (!search_interest(slf_node->interests_ptr, current->node, object_name, WAITING)) {
                        // Add interest
                        slf_node->interests_ptr = add_interest(slf_node->interests_ptr, current->node, object_name, WAITING);
                    }                     
                }
            }
        }

        // add yourself to the interest table and set yourself to ANSWER state

        slf_node->interests_ptr = add_interest(slf_node->interests_ptr, NULL, object_name, ANSWER);
    }
    else{  // received INTEREST message from someone and I don't have the object, send INTEREST to everybody except the source

        if(slf_node->extern_node->node_fd != fd2avoid){

            // extern node is not source, send him and INTEREST

            ptr = buffer;
            nleft = nbytes;

            while (nleft > 0) {
                nwritten = write(slf_node->extern_node->node_fd, ptr, nleft);
                if (nwritten <= 0) {
                    printf("Error in send_interest: Failed to write to socket for node [%s | %s].\n",
                        slf_node->extern_node->node_addr, slf_node->extern_node->tcp_port);
                    break;
                }
                nleft -= nwritten;
                ptr += nwritten;
            }
            if (nleft == 0) {
                printf("Message sent to [%s | %s]:\n",
                    slf_node->extern_node->node_addr, slf_node->extern_node->tcp_port);
                printf("%s\n\n", buffer);
                
                // Insert interest entry and set the interface to ANSWER, if it's not done already
                if (!search_interest(slf_node->interests_ptr, slf_node->extern_node, object_name, WAITING)) {
                    // Add interest
                    slf_node->interests_ptr = add_interest(slf_node->interests_ptr, slf_node->extern_node, object_name, WAITING);
                }           
            }

        }
        current = slf_node->internals_list;
        while (current != NULL) {             

            // Ignore the node that sent the interest and don't send to extern twice

            if ((current->node->node_fd != fd2avoid) && (current->node->node_fd != slf_node->extern_node->node_fd)) {
                ptr = buffer;
                nleft = nbytes;

                // Send the message
                while (nleft > 0) {
                    nwritten = write(current->node->node_fd, ptr, nleft);
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
                    
                    // Insert interest entry and set the interface to ANSWER, if it's not done already
                    if (!search_interest(slf_node->interests_ptr, current->node, object_name, WAITING)) {
                        // Add interest
                        slf_node->interests_ptr = add_interest(slf_node->interests_ptr, current->node, object_name, WAITING);
                    }                    

                }
            }
            current = current->next;
        }
    }

    free(buffer);
    

    return;
}

void send_object(char *object_name, struct personal_node *slf_node) {
    
    char *ptr;
    char *buffer = NULL;
    InterestEntry *aux = slf_node->interests_ptr, *aux2del, *prev;
    ssize_t nbytes, nleft, nwritten;
    int extern_flag = 0, writefail = 0;

    // Allocate Buffer's Memory
    if ((buffer = (char *)calloc(MAX_MSG_LENGTH, sizeof(char))) == NULL) {
        printf("Error in send_object: Failed to allocate memory. Process terminated.\n");
        exit(1);
    }
        
    sprintf(buffer, "%s %s\n", object_str, object_name);    
    nbytes = strlen(buffer);

    if(slf_node->interests_ptr == NULL){

        printf("Error in send_object: The interest table is empty, no one wants an answer for this object\n");
        return;
    }
    
    int counter = 2; // number of matches for the head
                
    if(strcmp(aux->name, object_name) != 0){

        //head has different name
        counter --;
    }
    if(aux->current_state != ANSWER){

        //head has different state
        counter --;
    }

    if(counter == 2){

        // send object to the head and remove it from the interest table

        nleft = nbytes;
        ptr = buffer;

        //To avoid sending this message twice to the extern node
        if(aux->interface_fd == slf_node->extern_node->node_fd) extern_flag++;

        // Send the message
        while (nleft > 0) {
            nwritten = write(aux->interface_fd, ptr, nleft);
            if (nwritten <= 0) {
                printf("Error in send_object: Failed to write to file descriptor: %s\n", strerror(errno));
                writefail = 1;
                break;
            }
            nleft -= nwritten;
            ptr += nwritten;
        }
        if(writefail == 0){
            printf("Message sent to [%s | %s]:\n", aux->interface_addr, aux->interface_tcp);
            printf("%s\n\n", buffer);
        }
        writefail = 0;

        printf("The following interest entry will be removed:\n\n");
        printf("Interface: [%s | %s]\n", aux->interface_addr, aux->interface_tcp);
        printf("Object in question: %s\n", aux->name);
        printf("Current state: %s\n\n", aux->state_str);
        aux2del = aux;
        aux = aux->next;
        free_interest(&aux2del);        
    }

    while(aux != NULL){
                                         
        if(strcmp(aux->name, object_name) != 0){

            //different name, see next entry
            
            prev = aux;
            aux = aux->next;
            continue;
        }
        if(aux->current_state != ANSWER){

            //different state, see next entry
            
            prev = aux;
            aux = aux->next;
            continue;
        }
        if((aux->interface_fd == slf_node->extern_node->node_fd) && (extern_flag = 1)){

            //Do not send this message twice, skip to next interest entry
            prev = aux;
            aux = aux->next;
            continue;
        }

        nleft = nbytes;
        ptr = buffer;

        // Send the message
        while (nleft > 0) {
            nwritten = write(aux->interface_fd, ptr, nleft);
            if (nwritten <= 0) {
                printf("Error in send_object: Failed to write to file descriptor: %s\n", strerror(errno));
                writefail = 1;
                break;
            }
            nleft -= nwritten;
            ptr += nwritten;
        }

        if(writefail == 0){
            printf("Message sent to [%s | %s]:\n", aux->interface_addr, aux->interface_tcp);
            printf("%s\n\n", buffer);
        }
        writefail = 0;
        
        //To avoid sending this message twice to the extern node
        if(aux->interface_fd == slf_node->extern_node->node_fd) extern_flag++;

        printf("The following interest entry will be removed:\n\n");
        printf("Interface: [%s | %s]\n", aux->interface_addr, aux->interface_tcp);
        printf("Object in question: %s\n", aux->name);
        printf("Current state: %s\n\n", aux->state_str);
        aux2del = aux;
        prev->next = aux->next;
        aux = aux->next;
        free_interest(&aux2del);
                
    }
    free(buffer);
               
    return;
}

void send_noobject(char *object_name, struct personal_node *slf_node) {
    
    char *ptr;
    char *buffer = NULL;
    InterestEntry *aux = slf_node->interests_ptr, *aux2del, *prev;
    ssize_t nbytes, nleft, nwritten;
    int extern_flag = 0, writefail = 0;

    // Allocate Buffer's Memory
    if ((buffer = (char *)calloc(MAX_MSG_LENGTH, sizeof(char))) == NULL) {
        printf("Error in send_object: Failed to allocate memory. Process terminated.\n");
        exit(1);
    }
        
    sprintf(buffer, "%s %s\n", noobject_str, object_name);    
    nbytes = strlen(buffer);

    if(slf_node->interests_ptr == NULL){

        printf("Error in send_noobject: The interest table is empty, no one wants an answer for this object\n");
        return ;
    }
    
    int counter = 2; // number of matches for the head
                
    if(strcmp(aux->name, object_name) != 0){

        //head has different name
        counter --;
    }
    if(aux->current_state != ANSWER){

        //head has different state
        counter --;
    }

    if(counter == 2){

        // send object to the head and remove it from the interest table

        nleft = nbytes;
        ptr = buffer;

        //To avoid sending this message twice to the extern node
        if(aux->interface_fd == slf_node->extern_node->node_fd) extern_flag++;

        // Send the message
        while (nleft > 0) {
            nwritten = write(aux->interface_fd, ptr, nleft);
            if (nwritten <= 0) {
                printf("Error in send_noobject: Failed to write to file descriptor: %s\n", strerror(errno));
                writefail = 1;
                break;
            }
            nleft -= nwritten;
            ptr += nwritten;
        }
        if(writefail == 0){
            printf("Message sent to [%s | %s]:\n", aux->interface_addr, aux->interface_tcp);
            printf("%s\n\n", buffer);
        }
        writefail = 0;

        printf("The following interest entry will be removed:\n\n");
        printf("Interface: [%s | %s]\n", aux->interface_addr, aux->interface_tcp);
        printf("Object in question: %s\n", aux->name);
        printf("Current state: %s\n\n", aux->state_str);
        aux2del = aux;
        aux = aux->next;
        free_interest(&aux2del);        
    }

    while(aux != NULL){
                                         
        if(strcmp(aux->name, object_name) != 0){

            //different name, see next entry
            
            prev = aux;
            aux = aux->next;
            continue;
        }
        if(aux->current_state != ANSWER){

            //different state, see next entry
            
            prev = aux;
            aux = aux->next;
            continue;
        }
        if((aux->interface_fd == slf_node->extern_node->node_fd) && (extern_flag = 1)){

            //Do not send this message twice, skip to next interest entry
            prev = aux;
            aux = aux->next;
            continue;
        }

        nleft = nbytes;
        ptr = buffer;

        // Send the message
        while (nleft > 0) {
            nwritten = write(aux->interface_fd, ptr, nleft);
            if (nwritten <= 0) {
                printf("Error in send_object: Failed to write to file descriptor: %s\n", strerror(errno));
                writefail = 1;
                break;
            }
            nleft -= nwritten;
            ptr += nwritten;
        }

        if(writefail == 0){
            printf("Message sent to [%s | %s]:\n", aux->interface_addr, aux->interface_tcp);
            printf("%s\n\n", buffer);
        }
        writefail = 0;
        
        //To avoid sending this message twice to the extern node
        if(aux->interface_fd == slf_node->extern_node->node_fd) extern_flag++;

        printf("The following interest entry will be removed:\n\n");
        printf("Interface: [%s | %s]\n", aux->interface_addr, aux->interface_tcp);
        printf("Object in question: %s\n", aux->name);
        printf("Current state: %s\n\n", aux->state_str);
        aux2del = aux;
        prev->next = aux->next;
        aux = aux->next;
        free_interest(&aux2del);
                
    }
    free(buffer);
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

    nodeinfo_t *temp = NULL; 
    temp = contact_init(temp);
    strcpy(temp->node_addr, slf_node->personal_addr);
    strcpy(temp->tcp_port, slf_node->personal_tcp);
    temp->node_fd = -1;

    //get commnand
    if (sscanf(msg, "%8s", cmd) != 1) {
        printf("Error in parse_tcp: Failed to read message type\n");
        return ++fail_flag;
    }

    //try to read and execute command

    if(strcmp(cmd, interest_msg_str) == 0){

        if(sscanf(msg, "%*s %100s",object_buff) == 1){

            printf("Message received from [%s | %s]:\n", src_node->node_addr, src_node->tcp_port); 
            printf("%s\n\n", msg);

            // search the cache and the storage

            if (queueSearch(slf_node->queue_ptr, object_buff) || storageSearch(slf_node->storage_ptr, object_buff)) {
            
                // We have the object, create the interest entry             

                slf_node->interests_ptr = add_interest(slf_node->interests_ptr, src_node, object_buff, ANSWER);

                // send OBJECT message
                
                send_object(object_buff, slf_node);

            } else {

                // We don't have the object...

                if (has_neighbors(slf_node->internals_list)) {
                    
                    // ...and you have intern neighbors

                    if(search_interest(slf_node->interests_ptr, src_node, object_buff, ANSWER)){

                        //avoid duplicate interest entry

                    }

                    else if(!search_interest(slf_node->interests_ptr, src_node, object_buff, WAITING) && 
                        !search_interest(slf_node->interests_ptr, NULL, object_buff, WAITING)) {
                        
                        // ...and not in interest table

                        // we made sure that the interest entry is not repeated and that no one is
                        // waiting for the object
                        slf_node->interests_ptr = add_interest(slf_node->interests_ptr, src_node, object_buff, ANSWER);
                        // send interest message to neighbors
                        send_interest(src_node->node_fd, object_buff, slf_node);
                    }
                    else {
                        // Interest already in the table
                        // Add the interface and set it to ANSWER
                        // so that we know that we have to send the object through this interface

                        slf_node->interests_ptr = add_interest(slf_node->interests_ptr, src_node, object_buff, ANSWER);
                    }
                
                } else {
                    // ...and you have no other neighbors, send noobject message to the extern node
                    send_noobject(object_buff, slf_node);                   
                }
            }
        }else{
        
            printf("Error in parse_tcp: Failed to read arguments of %s\n", cmd);
            return ++fail_flag;
        }                       
            
    }

    else if(strcmp(cmd, object_str) == 0){

        if(sscanf(msg, "%*s %100s", object_buff) == 1){
            
            printf("Message received from [%s | %s]:\n", src_node->node_addr, src_node->tcp_port);
            printf("%s\n\n", object_buff);
            
            // clear the entry that was in the waiting state, if it exists

            if(search_interest(slf_node->interests_ptr, src_node, object_buff, WAITING)){

                slf_node->interests_ptr = RemoveSingleInterest(slf_node->interests_ptr, src_node, object_buff, WAITING);
            }

            // if im waiting for the object, copy the object to the storages and clear myself from the interest table
            
            if(search_interest(slf_node->interests_ptr, temp, object_buff, WAITING)){

                printf("The object '%s' is now in your possession\n\n", object_buff);
                
                slf_node->storage_ptr = storageInsert(slf_node->storage_ptr, object_buff);

                // Delete the old object in cache to make sure you have space for new object

                if(slf_node->queue_ptr->object_counter == slf_node->queue_ptr->queue_limit){

                    slf_node->queue_ptr = deleteOld(slf_node->queue_ptr);
                }
                
                // and place the object in cache
                slf_node->queue_ptr = insertNew(slf_node->queue_ptr, object_buff);

            }
            slf_node->interests_ptr = RemoveSingleInterest(slf_node->interests_ptr, temp, object_buff, WAITING);
                        
            // check interest table to see if I have to answer someone             

            if(search_interest(slf_node->interests_ptr, NULL, object_buff, ANSWER)){                                                    

                send_object(object_buff, slf_node);                                                       

                // Delete the old object in cache to make sure you have space for new object

                if(slf_node->queue_ptr->object_counter == slf_node->queue_ptr->queue_limit){

                    slf_node->queue_ptr = deleteOld(slf_node->queue_ptr);
                }
                
                // and place the object in cache
                slf_node->queue_ptr = insertNew(slf_node->queue_ptr, object_buff);

                // remove interest from table
                slf_node->interests_ptr = remove_interests(slf_node->interests_ptr, object_buff);
            }
            else {
                // if no one is waiting for the object
                // place the object in cache

                if(slf_node->queue_ptr->object_counter == slf_node->queue_ptr->queue_limit){

                    slf_node->queue_ptr = deleteOld(slf_node->queue_ptr);
                }
                slf_node->queue_ptr = insertNew(slf_node->queue_ptr, object_buff);
                printf("No interfaces waiting for 'OBJECT %s'\n", object_buff);
            }
        }
        else{
            printf("Error in parse_tcp: Failed to read arguments of %s\n", cmd);
            return ++fail_flag;
        }
    }

    else if(strcmp(cmd, noobject_str) == 0){

        if(sscanf(msg, "%*s %100s",object_buff) == 1){
            
            printf("Message received from [%s | %s]:\n", src_node->node_addr, src_node->tcp_port);
            printf("%s\n\n", object_buff);

            // close this interface, if necessary
            if(search_interest(slf_node->interests_ptr, src_node, object_buff, WAITING)){
                slf_node->interests_ptr = update_interface_state(slf_node->interests_ptr,src_node->node_fd, object_buff, CLOSED);
            }
            
            // check if all interfaces are closed for this object

            if (!search_interest(slf_node->interests_ptr, NULL, object_buff, WAITING)){
                // all interfaces for object_buff are closed

                // send NOOBJECT
                send_noobject(object_buff, slf_node);

                // remove interest from table                                
                slf_node->interests_ptr = remove_interests(slf_node->interests_ptr, object_buff);
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