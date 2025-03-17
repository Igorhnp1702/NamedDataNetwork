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
     
    buffer[cnt] = '\0';

    printf("Response from the node server: %s\n", buffer);

    if (strcmp(inet_ntoa(addr_conf.sin_addr), server_IP) != 0) { //Verification
        strcpy(buffer, "0");
    }

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
    buffer[cnt] = '\0';

    printf("Response from the node server: %s\n", buffer);

    if (strcmp(inet_ntoa(addr_conf.sin_addr), server_IP) != 0) { //Verification
        strcpy(buffer, "0");
    }

    close(fd);
    return buffer;
 }

 
char *server_inquiry(char *server_IP, char *server_UDP, char *net){// request the list of nodes in the network

    struct addrinfo srv_criteria, *srv_result;
    int fd, errflag;
    ssize_t cnt;
    struct sockaddr_in addr_conf;
    socklen_t addrlen_conf;
    char *buffer = NULL;
    if((buffer = (char*)calloc(MAX_NODESLIST, sizeof(char))) == NULL){
        printf("Error in send_safe: Failed to allocate memory. process terminated\n");
        exit(1);
    }
    char inquiry[11]; // 5(nodes) + 1(space) + 3(net) + 1(\n) + 1(\0) = 11 
    memset(inquiry, 0, 11*sizeof(char));

    fd = socket(AF_INET, SOCK_DGRAM, 0) ;
    if(fd == -1) return NULL;

    memset(&srv_criteria, 0, sizeof(srv_criteria));
    srv_criteria.ai_family = AF_INET;
    srv_criteria.ai_socktype = SOCK_DGRAM;

    errflag = getaddrinfo(server_IP,server_UDP, &srv_criteria, &srv_result);
    if(errflag != 0) return NULL;

    sprintf(inquiry, "%s %s\n", nodes_str, net);

    cnt = sendto(fd, inquiry, strlen(inquiry), 0, srv_result->ai_addr, srv_result->ai_addrlen);
    if(cnt == -1) return NULL;

    printf("Inquiry sent to the node server: %s %s\n\n", nodes_str, net);

    freeaddrinfo(srv_result);

    addrlen_conf = sizeof(struct sockaddr_in);
    cnt = recvfrom(fd, buffer, MAX_NODESLIST, 0,(struct sockaddr *)&addr_conf, &addrlen_conf);
    if(cnt == -1){
        printf("Failed to get the list of nodes from the server\n");
        return NULL;
    }
    
    buffer[cnt] = '\0';

    if (strcmp(inet_ntoa(addr_conf.sin_addr), server_IP) != 0) { //Verification
        strcpy(buffer, "0");
    }

    close(fd);
    return buffer;
}

// char *send_retrieve(){

// }

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
            return NULL;
        }
        //setsockopt(*fd, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout));
        
        memset(&act, 0, sizeof(act));
        act.sa_handler = SIG_IGN;

        if(sigaction(SIGPIPE, &act, NULL) == -1){
            printf("Send entry: Error in sigaction().\n");            
            return NULL;
        }

        memset(&srv_criteria, 0, sizeof(srv_criteria));
        srv_criteria.ai_family = AF_INET;       //IPv4 address family
        srv_criteria.ai_socktype = SOCK_STREAM; // TCP socket
        
        errflag = getaddrinfo(dest_ip, dest_tcp, &srv_criteria, &srv_result);
        if(errflag != 0){
            printf("Send entry: Error in getaddrinfo()\n");
            return NULL;
        }

        errno = 0;
        errflag = connect(*fd, srv_result->ai_addr, srv_result->ai_addrlen);
        if(errflag == -1){
            printf("Send entry: Error in connect(): %s\n", strerror(errno));
            close(*fd);
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
            return NULL;
        }
        nleft -= nwritten;
        ptr += nwritten;
    }
   
    memset(buffer, 0, MAX_MSG_LENGTH * sizeof(char)); //set the buffer to '\0' //!lembrar
    strcpy(buffer, "1"); //indicates that the message was sent
   
    return buffer; // returning what was inside the file descriptor

}


// char *send_object(){

// }

// char *send_noobject(){
    
// }

int parse_tcp(struct personal_node *slf_node, char *msg, int *src_fd){

    int fail_flag = 0;

    char cmd[MAX_MSG_CMD_SIZE];         //message command of msg
    memset(cmd, 0, sizeof(cmd));    
        
    char tcp_cmd[MAX_TCP_UDP_CHARS];    //tcp port of msg
    memset(tcp_cmd, 0, sizeof(tcp_cmd));

    char ip_cmd[MAX_ADDRESS_SIZE];      //ip address of msg
    memset(ip_cmd, 0, sizeof(ip_cmd));

    char object_buff[MAX_OBJECT_NAME];  //object of msg
    memset(object_buff, 0, sizeof(object_buff));

    char snd_msg[MAX_MSG_LENGTH];       //buffer to send a response 
    memset(snd_msg, 0, sizeof(snd_msg));

    char snd_cmd[MAX_MSG_CMD_SIZE];     //the message command of our response 
    memset(snd_cmd, 0, sizeof(snd_cmd));

    char *return_msg = NULL;            //return from all the send commands

    nodeinfo_t *new_internal;
    

    nodesLinkedlist_t *aux;

    //get commnand
    if (sscanf(msg, "%s", cmd) != 1) {
        printf("Error in parse_tcp: Failed to read message type\n");
        return ++fail_flag;
    }

    //try to read and execute command

    if(strcmp(cmd, interest_msg_str) == 0){

        if(sscanf(msg, "%*s %s",object_buff) == 1){
            
            // search the object in the cache

            // object in cache, send object message

            // object not in cache and you have other neighbors, send interest message to them

            // object not in cach and you have no other neighbors, send noobject message to the source
        }else{
        
            printf("Error in parse_tcp: Failed to read arguments of %s\n", cmd);
            return ++fail_flag;
        }                       
            
    }

    else if(strcmp(cmd, object_str) == 0){

        if(sscanf(msg, "%*s %s", object_buff) == 1){
            
            // check interest table to see if someone is waiting for an answer from me           

            // if someone is waiting for the object, send object message to that node and place the object in cache

        }else{
            printf("Error in parse_tcp: Failed to read arguments of %s\n", cmd);
            return ++fail_flag;
        }
    }

    else if(strcmp(cmd, noobject_str) == 0){

        if(sscanf(msg, "%*s %s",object_buff) == 1){
            
            // check interest table to see if someone is waiting for an answer from me           

            // if someone is waiting for the object, send noobject message to that node

        }else {
            printf("Error in parse_tcp: Failed to read arguments of %s\n", cmd);
            return ++fail_flag;
        }
    }
       
    else if (strcmp(cmd, entry_str) == 0) {

        if (sscanf(msg, "%*s %s %s", ip_cmd, tcp_cmd) == 2){
            
            printf("\nMessage received from a new intern neighbor:\n");
            printf("%s\n", msg);

            if (strcmp(slf_node->extern_node->node_addr, "") == 0) {  //I was alone in the network answer with SAFE, followed by an ENTRY

                //new intern
                new_internal = NULL;
                new_internal = contact_init(new_internal);
                strcpy(new_internal->tcp_port, tcp_cmd);
                strcpy(new_internal->node_addr, ip_cmd);
                new_internal->node_fd = *src_fd;

                slf_node->internals_list = insertnode(slf_node->internals_list, new_internal);
                slf_node->n_internals++;                                

                printf("Sending %s %s %s\n", safe_str, ip_cmd, tcp_cmd);
                return_msg = send_safe(*src_fd, ip_cmd, tcp_cmd);
                
                if (return_msg != NULL) {

                    printf("\nMessage sent to %s | %s:\n", ip_cmd, tcp_cmd);
                    printf("%s %s %s\n", safe_str, ip_cmd, tcp_cmd);
                    
                    free(return_msg);
                    return_msg = NULL;                        
                                                   
                }
                else{
                    printf("Error in parse_tcp.\n");
                    printf("Error accepting new internal: Failed to send SAFE message. Connection closed\n");
                    close(*src_fd);
                    FD_CLR(*src_fd, &(slf_node->crr_scks));
                    return ++fail_flag;                        
                }

                printf("Sending %s %s %s\n", entry_str, slf_node->personal_addr, slf_node->personal_tcp);
                
                return_msg = send_entry(src_fd, slf_node->personal_addr, slf_node->personal_tcp,
                                        ip_cmd, tcp_cmd);
                
                if (return_msg != NULL) {

                    printf("\nMessage sent to %s | %s:\n", ip_cmd, tcp_cmd);
                    printf("%s %s %s\n", entry_str, slf_node->personal_addr, slf_node->personal_tcp);
                    
                    free(return_msg);
                    return_msg = NULL;                        
                                                  
                }
                else{
                    printf("Error in parse_tcp.\n");
                    printf("Error connecting to new external: Failed to send ENTRY message. Connection closed\n");
                    close(*src_fd);
                    FD_CLR(*src_fd, &(slf_node->crr_scks));
                    return ++fail_flag;                        
                }
                strcpy(slf_node->extern_node->tcp_port, tcp_cmd);
                strcpy(slf_node->extern_node->node_addr, ip_cmd);
                slf_node->extern_node->node_fd = *src_fd;                
                slf_node->anchorflag = 1; 

            }
            else {
                
                new_internal = NULL;
                new_internal = contact_init(new_internal);                
                strcpy(new_internal->node_addr, tcp_cmd);
                strcpy(new_internal->node_addr, ip_cmd);
                new_internal->node_fd = *src_fd;

                slf_node->internals_list = insertnode(slf_node->internals_list, new_internal);
                slf_node->n_internals++;
                
                printf("Sending %s %s %s\n", safe_str, slf_node->extern_node->node_addr, slf_node->extern_node->tcp_port);
                return_msg = send_safe(*src_fd, ip_cmd, tcp_cmd);
                
                if (strcmp(return_msg, "1") == 0) {
                    printf("\nMessage sent to %s | %s:\n", ip_cmd, tcp_cmd);
                    printf("%s %s %s\n", safe_str, slf_node->extern_node->node_addr, slf_node->extern_node->tcp_port);
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
                        close(*src_fd);
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

        if(sscanf(msg, "%*s %s %s",ip_cmd, tcp_cmd) == 2){

            if(*src_fd == slf_node->extern_node->node_fd){ // The message reached me through the extern node
                
                
                printf("\nMessage received from %s | %s:\n", slf_node->extern_node->node_addr, slf_node->extern_node->tcp_port);
                printf("%s\n", msg);
                 
            }            
            else{
                // entrance of the 2nd node, while im the first (beginning of the list) or
                // anchor node disappeared, while im the other anchor, and I had to choose someone to be the 2nd anchor (anywhere in the list)
                
                aux = slf_node->internals_list;
                while(aux != NULL){
                            
                    if(aux->node->node_fd == *src_fd){
                        printf("Internal node disconected: %s | %s\n", aux->node->node_addr, aux->node->tcp_port);
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
        printf("Error in parse_tcp: Failed to read command!");
        return ++fail_flag;
    }

    if (return_msg != NULL) {
        free(return_msg);
        return_msg = NULL;
    }
    return fail_flag;
}

char *parseNstore(char *msg_bffr, char *node_bffr, int fd){

    ssize_t bytes_read;               // number of bytes read from a read operation
    ssize_t bytes_left;               // number of bytes left to fill the buffer capacity
    char *scan_ptr;
    const char delim[2] = "\n";
    char *token;
    char one_cmd[MAX_MSG_LENGTH];   memset(one_cmd, 0, MAX_MSG_LENGTH);
    char cmds_left[MAX_MSG_LENGTH]; memset(one_cmd, 0, MAX_MSG_LENGTH);

    scan_ptr = msg_bffr;

    if(( token=strtok(node_bffr,delim)) != NULL){
        strcpy(one_cmd, token);
        while((token = strtok(NULL, delim)) != NULL){
            strcat(cmds_left, token);
            cmds_left[strlen(token+1)] = '\n';
        }
        
        strcpy(node_bffr, cmds_left);// keep the remainder
    }
    
    while (((bytes_read = read(fd, scan_ptr, MAX_MSG_LENGTH - 1)) > 0) && (bytes_read < bytes_left)) { //read msg to buffer
            
        if((token = strtok(scan_ptr,delim)) != NULL){ //command found
            strcpy(one_cmd, token);
            while((token = strtok(NULL, delim)) != NULL){
                strcat(cmds_left, token);
                cmds_left[strlen(token+1)] = '\n';
            }
            
            strcpy(node_bffr, cmds_left);// keep the remainder
            
            break;                
        } 
        scan_ptr += bytes_read - 1;
        
        bytes_left = MAX_MSG_LENGTH - bytes_read;
        if (*(scan_ptr) == '\n') break;
    }

    return one_cmd;
}