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

 char *node_reg(char *server_IP, char *server_UDP, char *node_IP, char *node_TCP, char *net){ // register the node

    int success = 0;

    struct addrinfo srv_criteria, *srv_result;
    int fd, errflag;
    ssize_t cnt;
    struct sockaddr_in addr_conf;
    socklen_t addrlen_conf;
    char *buffer = (char*)calloc(6, sizeof(char)); // OKREG (5) + \0 (1) = 6
    char *reg_msg[69]; // 5(REG) + 1(space) + 3(net) + 1(space) + 50(IP) + 1(space) + 6(TCP) + 1(\n) + 1(\0) = 19 + strlen(IP)
    memset(reg_msg, "\0", 69);

    fd = socket(AF_INET, SOCK_DGRAM, 0) ;
    if(fd == -1) return NULL;

    memset(&srv_criteria, 0, sizeof(srv_criteria));
    srv_criteria.ai_family = AF_INET;
    srv_criteria.ai_socktype = SOCK_DGRAM;

    errflag = getaddrinfo(server_IP,server_UDP, &srv_criteria, &srv_result);
    if(errflag != 0) return NULL;

    sprintf(reg_msg, "%s %s %s %s\n", reg_str, net, node_IP, node_TCP);

    cnt = sendto(fd, reg_msg, strlen(reg_msg), 0, srv_result->ai_addr, srv_result->ai_addrlen);
    if(cnt == -1) return NULL;

    printf("Registration request sent to the node server: %s\n\n", reg_msg);

    freeaddrinfo(srv_result);

    addrlen_conf = sizeof(struct sockaddr_in);
    cnt = recvfrom(fd, buffer, 6, 0,(struct sockaddr *)&addr_conf, &addrlen_conf); // retrieve the same 6 bytes you allocated for the response
    if(cnt == -1) return NULL;
    buffer[cnt] = '\0';

    if (strcmp(inet_ntoa(addr_conf.sin_addr), server_IP) != 0) { //Verification
        strcpy(buffer, "0");
    }

    close(fd);
    return buffer;
 }

 char *node_unreg(char *server_IP, char *server_UDP, char *node_IP, char *node_TCP, char *net){ // unregister the node

    int success = 0;

    struct addrinfo srv_criteria, *srv_result;
    int fd, errflag;
    ssize_t cnt;
    struct sockaddr_in addr_conf;
    socklen_t addrlen_conf;
    char *buffer = (char*)calloc(6, sizeof(char)); // OKREG (5) + \0 (1) = 6
    char *unreg_msg[71]; // 7(REG) + 1(space) + 3(net) + 1(space) + 50(IP) + 1(space) + 6(TCP) + 1(\n) + 1(\0) = 19 + strlen(IP)
    memset(unreg_msg, "\0", 71);

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
    cnt = recvfrom(fd, buffer, 6, 0,(struct sockaddr *)&addr_conf, &addrlen_conf); // retrieve the same 6 bytes you allocated for the response
    if(cnt == -1) return NULL;
    buffer[cnt] = '\0';

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
    char *buffer = (char*)calloc(MAX_NODESLIST, sizeof(char));
    char *inquiry[11]; // 5(nodes) + 1(space) + 3(net) + 1(\n) + 1(\0) = 11 
    memset(inquiry, "\0", 11);

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
    if(cnt == -1) return NULL;
    buffer[cnt] = '\0';

    if (strcmp(inet_ntoa(addr_conf.sin_addr), server_IP) != 0) { //Verification
        strcpy(buffer, "0");
    }

    close(fd);
    return buffer;
}

char *send_retrieve(){

}

char *send_entry(int *fd, char *mynode_ip, char *mynode_tcp, char *dest_ip, char *dest_tcp){
    
    struct addrinfo srv_criteria, *srv_result; 
    int errflag;
    ssize_t nbytes, nleft, nwritten;
    char *ptr;
    char *buffer = (char*)calloc(MAX_MSG_LENGTH, sizeof(char));
    struct sigaction act;

    struct timeval timeout;
    timeout.tv_sec = 5;  // 5-second timeout
    timeout.tv_usec = 0;
    

    if (*fd == -1) {
        *fd = socket(AF_INET, SOCK_STREAM, 0);
        if(*fd == -1){
            printf("Send entry: Error initializing socket.\n");
            return NULL;
        }
        setsockopt(*fd, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout));
        
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

        errflag = connect(*fd, srv_result->ai_addr, srv_result->ai_addrlen);
        if(errflag == -1){
            printf("Send entry: Error in connect()\n");
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
    char *buffer = (char*)calloc(MAX_MSG_LENGTH, sizeof(char));
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


char *send_object(){

}

char *send_noobject(){
    
}

int parse_tcp(struct personal_node *slf_node, char *msg, int *src_fd){

    int success_flag = 0;

    char cmd[MAX_MSG_CMD_SIZE];     //message command of msg
    memset(cmd, 0, sizeof(cmd));    
        
    char tcp_cmd[MAX_TCP_UDP_CHARS];    //tcp port of msg
    memset(tcp_cmd, 0, sizeof(tcp_cmd));

    char ip_cmd[MAX_ADDRESS_SIZE];    //ip address of msg
    memset(ip_cmd, 0, sizeof(ip_cmd));

    char object_buff[MAX_OBJECT_NAME];  //content of msg
    memset(object_buff, 0, sizeof(object_buff));

    char snd_msg[MAX_MSG_LENGTH];    //buffer to send a response 
    memset(snd_msg, 0, sizeof(snd_msg));

    char snd_cmd[MAX_MSG_CMD_SIZE];     //the message command of our response 
    memset(snd_cmd, 0, sizeof(snd_cmd));

    char *rcv_msg = NULL; //reaction to our response, needs to be freed after use

    int i = 0;          //iterator
               
    int fd_gateway;     // variable to store the file descriptor to use

    nodeinfo_t *new_internal;

    
    

    //get commnand
    if (sscanf(msg, "%s", cmd) != 1) {
        printf("Error in rcv_tcp: Failed to read command\n");
        return ++success_flag;
    }

    //try to read and execute command

    if(strcmp(cmd, interest_str) == 0){

        if(sscanf(msg, "%*s %s",object_buff) == 1){
            
            // search the object in the cache

            // object in cache, send object message

            // object not in cache and you have other neighbors, send interest message to them

            // object not in cach and you have no other neighbors, send noobject message to the source
        }else{
        
            printf("Error in rcv_tcp: Failed to read arguments of %s\n", cmd);
            return ++success_flag;
        }                       
            
    }

    else if(strcmp(cmd, object_str) == 0){

        if(sscanf(msg, "%*s %s", object_buff) == 1){
            
            // check interest table to see if someone is waiting for an answer from me           

            // if someone is waiting for the object, send object message to that node and place the object in cache

        }else{
            printf("Error in rcv_tcp: Failed to read arguments of %s\n", cmd);
            return ++success_flag;
        }
    }

    else if(strcmp(cmd, noobject_str) == 0){

        if(sscanf(msg, "%*s %s",object_buff) == 1){
            
             // check interest table to see if someone is waiting for an answer from me           

            // if someone is waiting for the object, send noobject message to that node

        }else {
            printf("Error in rcv_tcp: Failed to read arguments of %s\n", cmd);
            return ++success_flag;
        }
    }
       
    else if (strcmp(cmd, entry_str) == 0) {

        if (sscanf(msg, "%*s %s %s",ip_cmd, tcp_cmd) == 3){
            
            printf("\nMessage received from a new intern neighbor:\n");
            printf("%s\n", msg);

            if (strcmp(slf_node->extern_node->node_addr, "") == 0) {  //Only node in the network

                strcpy(slf_node->extern_node->network, slf_node->persn_info->network);                
                strcpy(slf_node->extern_node->tcp_port, tcp_cmd);
                strcpy(slf_node->extern_node->node_addr, ip_cmd);
                slf_node->client_fd = *src_fd;
                slf_node->anchorflag = 1; 
            }
            else {
                
                new_internal = contact_init(new_internal);
                strcpy(new_internal->network, slf_node->persn_info->network);
                strcpy(new_internal->node_addr, tcp_cmd);
                strcpy(new_internal->node_addr, ip_cmd);
                new_internal->node_fd = *src_fd;

                insertnode(slf_node->internals_list, new_internal);                                
            }
            //sprintf(snd_msg, "%s %s %s %s\n", e_str, slf_node->extern_node->node_id,slf_node->extern_node->node_addr, slf_node->extern_node->tcp_port);
            
            printf("Sending %s %s %s\n", safe_str, slf_node->extern_node->node_addr, slf_node->extern_node->tcp_port);
            rcv_msg = send_safe(src_fd, ip_cmd, tcp_cmd);
            
            if (strcmp(rcv_msg, "1") == 0) {
                printf("\nMessage sent to %s | %s:\n", ip_cmd, tcp_cmd);
                printf("%s\n", snd_msg);
                if (rcv_msg != NULL){ //reset the pointer to the received message
                    free(rcv_msg);
                    rcv_msg = NULL;
                    return success_flag;
                }                                
            }
            else{
                printf("Error in rcv_tcp: Communication with TCP server failed.\n");
                if (rcv_msg != NULL) { //reset the pointer to the received message
                    free(rcv_msg);
                    rcv_msg = NULL;
                }
                return ++success_flag;                        
            }
        }
        else {
            printf("Error in rcv_tcp: Failed to read arguments of %s\n", cmd);
            return ++success_flag;
        }
    }

    else if(strcmp(cmd, safe_str) == 0){

        if(sscanf(msg, "%*s %s %s",ip_cmd, tcp_cmd) == 2){

            if(*src_fd == slf_node->client_fd){ // The message reached me through the extern node
                
                
                printf("\nMessage received from %s | %s:\n", slf_node->extern_node->node_addr, slf_node->extern_node->tcp_port);
                printf("%s\n", msg);
                 
            }            
            else{// who is the intern neighbor that sent me the message?
                for(i = 0; i < MAX_INTERNALS; i++){
                    
                    if(slf_node->internals_array[i] != NULL){
                        if(*src_fd == slf_node->internals_array[i]->node_fd){
                                                    
                            printf("\nMessage received from %s | %s:\n", slf_node->internals_array[i]->node_addr, slf_node->internals_array[i]->tcp_port);
                            printf("%s\n", msg);                                                                        
                            break;
                        }
                    }
                }
            }        

            printf("Updating the backup neighbor...\n\n");
            
            memset(slf_node->backup_node->network, 0, 4 * sizeof(*slf_node->backup_node->network));
            memset(slf_node->backup_node->tcp_port, 0, 6 * sizeof(*slf_node->backup_node->tcp_port));
            memset(slf_node->backup_node->node_addr, 0, 50 * sizeof(*slf_node->backup_node->node_addr));

            strcpy(slf_node->backup_node->network, slf_node->persn_info->network);            
            strcpy(slf_node->backup_node->tcp_port, tcp_cmd);
            strcpy(slf_node->backup_node->node_addr, ip_cmd);

            printf("New backup neighbor:\n\n");                    
            printf("Network: %s\nAdress: %s\nPort: %s\n\n",             
            slf_node->backup_node->network, 
            slf_node->backup_node->node_addr, 
            slf_node->backup_node->tcp_port);
            
            
        }
        else{
            printf("Error in rcv_tcp: Failed to read arguments of %s\n", cmd);
            return ++success_flag;
        }
    } 
    
    else{
        printf("Error in rcv_tcp: Failed to read command!");
        return ++success_flag;
    }

    if (rcv_msg != NULL) {
        free(rcv_msg);
        rcv_msg = NULL;
    }
    return success_flag;
}
