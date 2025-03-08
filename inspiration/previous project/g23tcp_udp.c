/************************************************************************************************
 * g23tcp.c
 *
 * Authors:
 *
 * -Igor Paruque ist1102901
 * -Afonso Klier ist196139
 *
 * Description: source code for the TCP and UDP communication
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
#include "g23tcp_udp.h"


// networking libraries
#include <netdb.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <signal.h>

/* ------------------------TCP Message Handler--------------------------- */

 int send_tcp(struct personal_node *slf_node, nodeinfo_t *dest_node, char *msg) {// slf_node = self node = personal = my_node
    
    //dest_node is a neighbor
    //just check if it's an intern node or an extern node

    int success_flag = 0;

    // blocks of memory to check the arguments of the messages sent by slf_node

    char cmd[MAX_MSG_CMD_SIZE];       //first string of the outgoing message
    memset(cmd, 0, sizeof(cmd));    

    char id1_cmd[MAX_ID_CHARS];     //slot of memory for the id (QUERY => destination)
    memset(id1_cmd, 0, sizeof(id1_cmd));

    char id2_cmd[MAX_ID_CHARS];     //second slot of memory for the id (QUERY => origin)
    memset(id1_cmd, 0, sizeof(id2_cmd));

    char tcp_cmd[MAX_TCP_UDP_CHARS];     //tcp port of the outgoing message 
    memset(tcp_cmd, 0, sizeof(tcp_cmd));

    char ip_cmd[MAX_ADDRESS_SIZE];     //ip address of the outgoing message  
    memset(ip_cmd, 0, sizeof(ip_cmd));

    char content_buff[MAX_CONTENT_NAME + 1];  //content of the outgoing message + \n + \0
    memset(content_buff, 0, sizeof(content_buff));

    char *rcv_msg = NULL; //reaction to our message, needs to be freed after use

    int intern_gateway; //id of gateway neighbor to help me send the message to the destination
    int fd_gateway;     //file descriptor to send the message to    
    int search_id;
    
   //get commnand
    if (sscanf(msg, "%s", cmd) != 1){
        printf("Error in send_tcp: Failed to read command\n");
        return ++success_flag;
    }
    
    /* select the message to be sent */

    if(strcmp(cmd, query_str) == 0){ // send "QUERY", wait for "CONTENT" or "NOCONTENT", or timeout

        if(sscanf(msg, "%*s %s %s %s", id1_cmd, id2_cmd, content_buff) == 3){ 

            if(strcmp(slf_node->extern_node->node_id, dest_node->node_id) == 0){
                
                fd_gateway = slf_node->client_fd;
                if((rcv_msg = tcp_client(&fd_gateway, dest_node->node_addr, dest_node->tcp_port, msg, cmd)) == NULL){
                    
                    printf("Error in send_tcp: Connection failed!\n");
                    return ++success_flag;
                }
                if (strcmp(rcv_msg, "1") == 0) {
                    
                    printf("\nMessage sent to %s:\n", dest_node->node_id);
                    printf("%s\n\n", msg);

                    if (rcv_msg != NULL) { //reset the pointer to the received message
                        free(rcv_msg);
                        rcv_msg = NULL;
                    }
                    return success_flag;
                }
                else{
                    printf("Error in send_tcp: Communication with TCP server failed.\n");
                    if (rcv_msg != NULL) { //reset the pointer to the received message
                        free(rcv_msg);
                        rcv_msg = NULL;
                    }
                    return ++success_flag;
                }                 
            }//if
            
            else{ //QUERY to an intern neighbor

                intern_gateway = atoi(dest_node->node_id);
                fd_gateway = slf_node->neighbrs_fd[intern_gateway];

                if((rcv_msg = tcp_client(&fd_gateway, dest_node->node_addr, dest_node->tcp_port, msg, cmd)) == NULL){
                    printf("Error in send_tcp: Connection failed!\n");
                    return ++success_flag;
                }
                if (strcmp(rcv_msg, "1") == 0){
                    printf("\nMessage sent to %s:\n", dest_node->node_id);
                    printf("%s\n", msg);
                    if (rcv_msg != NULL){ //reset the pointer to the received message
                        free(rcv_msg);
                        rcv_msg = NULL;
                    }
                    return success_flag;
                }
                else{
                    printf("Error in send_tcp: Communication with TCP server failed.\n");
                    if (rcv_msg != NULL){ //reset the pointer to the received message
                        free(rcv_msg);
                        rcv_msg = NULL;
                    }
                    return ++success_flag;
                }
            }//else                           
        }//if
        else{
            printf("Error in send_tcp: Failed to read arguments of %s.\n", cmd);
            return ++success_flag;    
        }
    }
    
    else if(strcmp(cmd, withdraw_str) == 0){ // send "WITHDRAW", receive nothing

        if(sscanf(msg, "%*s %s", id1_cmd) == 1){

            search_id = atoi(id1_cmd);            
            slf_node->route_tab[search_id] = -1;
            printf("\n%02d was removed from the routing table as a destination\n", search_id);

            for(int i = 0; i < NODE_IDS; i++){
               
               if(slf_node->route_tab[i] == search_id){
                    slf_node->route_tab[i] = -1;
                }
            }
            printf("\n%02d was removed from the routing table as a neighbor\n", search_id);
            
            
            if (strcmp(dest_node->node_id, slf_node->extern_node->node_id) == 0) {
                fd_gateway = slf_node->client_fd;
            }
            else {
                fd_gateway = slf_node->neighbrs_fd[atoi(dest_node->node_id)];
            }            

            if((rcv_msg = tcp_client(&fd_gateway, dest_node->node_addr, dest_node->tcp_port, msg, cmd)) == NULL){
                
                printf("Error in send_tcp: Communication with TCP server failed.\n");
                return ++success_flag;
            }
            if (strcmp(rcv_msg, "1") == 0) {
                printf("\nMessage sent to %s:\n", dest_node->node_id);
                printf("%s\n", msg);
                if (rcv_msg != NULL) { //reset the pointer to the received message
                    free(rcv_msg);
                    rcv_msg = NULL;
                }
                return success_flag;
            }
            else{
                printf("Error in send_tcp: Communication with TCP server failed.\n");
                if (rcv_msg != NULL) { //reset the pointer to the received message
                    free(rcv_msg);
                    rcv_msg = NULL;
                }
                return ++success_flag;
            }
        }
        else {
            printf("Error in send_tcp: Failed to read arguments of %s.\n", cmd);
            return success_flag;      
        }   
    }

    else if (strcmp(cmd, new_str) == 0) { // send "NEW", wait for "EXTERN"

        if (sscanf(msg, "%*s %s %s %s",id1_cmd, ip_cmd, tcp_cmd) == 3) {
            if((rcv_msg = tcp_client(&slf_node->client_fd, dest_node->node_addr, dest_node->tcp_port, msg, cmd)) == NULL){
                printf("Error in send_tcp: Connection failed!\n");
                return ++success_flag;
            }
            if (strcmp(rcv_msg, "1") == 0) {
                printf("\nMessage sent to %s:\n", dest_node->node_id);
                printf("%s\n", msg);
                if (rcv_msg != NULL) { //reset the pointer to the received message
                    free(rcv_msg);
                    rcv_msg = NULL;
                }
                return success_flag;
            }
            else{
                printf("Error in send_tcp: Communication with TCP server failed.\n");
                if (rcv_msg != NULL) { //reset the pointer to the received message
                    free(rcv_msg);
                    rcv_msg = NULL;
                }
                return ++success_flag;
            }            
        }   
        else {
            printf("Error in send_tcp: Failed to read arguments of %s.\n", cmd);
            return ++success_flag;      
        }    
    } 
    
    else if(strcmp(cmd, extern_str) == 0){ // send "EXTERN", receive nothing

        if(sscanf(msg, "%*s %s %s %s", id1_cmd, ip_cmd, tcp_cmd) == 3){
            
            if((rcv_msg = tcp_client(&slf_node->neighbrs_fd[atoi(dest_node->node_id)], dest_node->node_addr, dest_node->tcp_port, msg, cmd)) == NULL){
                
                printf("Error in send_tcp: Communication with TCP server failed.\n");
                return ++success_flag;

            }                                      
            if (strcmp(rcv_msg, "1") == 0) {
                printf("\nMessage sent to %s:\n", dest_node->node_id);
                printf("%s\n", msg);
                if (rcv_msg != NULL) { //reset the pointer to the received message
                    free(rcv_msg);
                    rcv_msg = NULL;
                }
                return success_flag;
            }
            else{
                printf("Error in send_tcp: Communication with TCP server failed.\n");
                if (rcv_msg != NULL) { //reset the pointer to the received message
                    free(rcv_msg);
                    rcv_msg = NULL;
                }
                return ++success_flag;
            }
        }
        else {
            printf("Error in send_tcp: Failed to read arguments of %s.\n", cmd);
            return ++success_flag;      
        } 
    }
    return ++success_flag;
} 

 int rcv_tcp(struct personal_node *slf_node, char *msg, int *src_fd){

    int success_flag = 0;

    char cmd[MAX_MSG_CMD_SIZE];     //message command of msg
    memset(cmd, 0, sizeof(cmd));    

    char id1_cmd[MAX_ID_CHARS];     //id present in the msg
    memset(id1_cmd, 0, sizeof(id1_cmd));

    char id2_cmd[MAX_ID_CHARS];     //id present in msg
    memset(id1_cmd, 0, sizeof(id2_cmd));

    char tcp_cmd[MAX_TCP_UDP_CHARS];    //tcp port of msg
    memset(tcp_cmd, 0, sizeof(tcp_cmd));

    char ip_cmd[MAX_ADDRESS_SIZE];    //ip address of msg
    memset(ip_cmd, 0, sizeof(ip_cmd));

    char content_buff[MAX_CONTENT_NAME];  //content of msg
    memset(content_buff, 0, sizeof(content_buff));

    char snd_msg[MAX_MSG_LENGTH];    //buffer to send a response 
    memset(snd_msg, 0, sizeof(snd_msg));

    char snd_cmd[MAX_MSG_CMD_SIZE];     //the message command of our response 
    memset(snd_cmd, 0, sizeof(snd_cmd));

    char *rcv_msg = NULL; //reaction to our response, needs to be freed after use

    int i = 0;          //iterator
    
    int src_node;       // id of neighbor through which the message was received
    char *src_addr;     // his address
    char *src_tcp;      // his tcp port

    int origin;         // node who originated the message
    int search_id;      // integer to check destination in route_tab
    int intern_gateway; // variable to store the id of an intern neighbor
    int fd_gateway;     // variable to store the file descriptor to use
    

    //get commnand
    if (sscanf(msg, "%s", cmd) != 1) {
        printf("Error in rcv_tcp: Failed to read command\n");
        return ++success_flag;
    }

    //try to read and execute command

    if(strcmp(cmd, query_str) == 0){

        if(sscanf(msg, "%*s %s %s %s", id1_cmd, id2_cmd, content_buff) == 3){ // 
            
            //find out who sent the message before answering

            origin = atoi(id2_cmd);

            if(*src_fd == slf_node->client_fd){ // The query reached me through the extern node

                fd_gateway = slf_node->client_fd;
                src_node = atoi(slf_node->extern_node->node_id);
                printf("\nMessage received from %02d:\n", src_node);
                printf("%s\n", msg);

                src_addr = slf_node->extern_node->node_addr;
                src_tcp = slf_node->extern_node->tcp_port;

                if(slf_node->route_tab[origin] != src_node){
                    
                    slf_node->route_tab[origin] = src_node;
                    printf("The routing table was updated\n");
                    printf("To send something to %s, use the node %s\n\n", 
                    id2_cmd, slf_node->extern_node->node_id);

                    if (slf_node->route_tab[src_node] == -1) {
                        slf_node->route_tab[src_node] = src_node;
                        printf("To send something to %02d, use the node %02d\n\n", 
                        src_node, src_node);
                    }
                }
            }            
            else{// who is the intern neighbor that sent me the query message?
                for(i = 0; i < NODE_IDS; i++){
                    if(*src_fd == slf_node->neighbrs_fd[i]){
                        
                        src_node = i;
                        printf("\nMessage received from %02d:\n", src_node);
                        printf("%s\n", msg);

                        src_addr = slf_node->neighbrs[i]->node_addr;
                        src_tcp = slf_node->neighbrs[i]->tcp_port;

                        fd_gateway = slf_node->neighbrs_fd[i];
                        if(slf_node->route_tab[origin] != src_node){
                            
                            slf_node->route_tab[origin] = src_node;
                            printf("The routing table was updated\n");
                            printf("To send something to %s, use the node %s\n\n", 
                            id2_cmd, slf_node->neighbrs[i]->node_id);

                            if (slf_node->route_tab[src_node] == -1) {
                                slf_node->route_tab[src_node] = src_node;
                                printf("To send something to %02d, use the node %02d\n\n", 
                                src_node, src_node);
                            }
                        }
                        break;
                    }
                }
            }
            
            if(strcmp(id1_cmd, slf_node->persn_info->node_id) == 0){ //it's for me, search the content    

                contentList_t *listptr;
                listptr = slf_node->contents;
                int flag = 0;                

                while(listptr != NULL){

                    if(strcmp(content_buff, listptr->string) == 0){
                        
                        flag++;
                        sprintf(snd_msg, "CONTENT %s %s %s\n", id2_cmd, slf_node->persn_info->node_id, listptr->string);

                        printf("Sending %s\n", snd_msg);
                        if((rcv_msg = tcp_client(&fd_gateway, src_addr, src_tcp, snd_msg, "CONTENT")) == NULL){
                            
                            printf("Error in rcv_tcp: Failed to send a message\n");
                            return ++success_flag;
                        }
                        if((strcmp(rcv_msg, "1") == 0)){                            
                            
                            printf("\nMessage sent to %02d:\n", src_node);
                            printf("%s\n", snd_msg);
                            if (rcv_msg != NULL) { //reset the pointer to the received message
                                free(rcv_msg);
                                rcv_msg = NULL;
                            }
                            break;                        
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
                    listptr = listptr->next;                                     
                }
                if(flag == 1){
                    return success_flag; //found the content and sent the message
                }
                else{ //reply with NOCONTENT

                    sprintf(snd_msg, "NOCONTENT %s %s %s\n", id2_cmd, slf_node->persn_info->node_id, content_buff);                                                                                            
                                                    
                    printf("Sending %s\n", snd_msg);
                    if((rcv_msg = tcp_client(&fd_gateway, src_addr, src_tcp, snd_msg, "NOCONTENT")) == NULL){
                        
                        printf("Error in rcv_tcp: Failed to send a message\n");
                        return ++success_flag;
                    }

                    if(strcmp(rcv_msg, "1") == 0){
                        printf("\nMessage sent to %02d:\n", src_node);
                        printf("%s\n", snd_msg);
                        if(rcv_msg != NULL){ //reset the pointer to the received message
                            free(rcv_msg);
                            rcv_msg = NULL;
                        }
                        return success_flag;                           
                    }
                    else{
                        printf("Error in rcv_tcp: Communication with TCP server failed.\n");
                        if (rcv_msg != NULL) { //reset the pointer to the received message
                            free(rcv_msg);
                            rcv_msg = NULL;
                        }
                        return ++success_flag;
                    }                                                                                                            
                }//else
            }//if QUERY for me

            else{ // not for me? Redirect message

                // check route table and see where to redirect message            
                
                search_id = atoi(id1_cmd); //check destination in route_tab                

                if(slf_node->route_tab[search_id] == src_node){ // gateway is equal to the source, discard message
                    printf("rcv_tcp: This received message was discarded: %s\n", msg);
                    return ++success_flag;                
                }
                else if(slf_node->route_tab[search_id] == atoi(slf_node->extern_node->node_id)){ // the gateway to dest is the extern

                    fd_gateway = slf_node->client_fd;
                    if((rcv_msg = tcp_client(&fd_gateway, slf_node->extern_node->node_addr, slf_node->extern_node->tcp_port, msg, cmd)) == NULL){
                        
                        printf("Error in rcv_tcp: Failed to redirect: %s\n", msg);
                        return ++success_flag;
                    }
                    if(strcmp(rcv_msg, "1") == 0){

                        printf("\nMessage sent to %s:\n", slf_node->extern_node->node_id);
                        printf("%s\n", msg);
                        if (rcv_msg != NULL) { //reset the pointer to the received message
                            free(rcv_msg);
                            rcv_msg = NULL;
                        }
                        return success_flag;
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
                else if(slf_node->route_tab[search_id] != -1){ // the gateway is an intern neighbor, but who?

                    intern_gateway = slf_node->route_tab[search_id];
                    fd_gateway = slf_node->neighbrs_fd[intern_gateway];

                    printf("Sending %s\n", msg);
                    if((rcv_msg = tcp_client(&fd_gateway, slf_node->neighbrs[intern_gateway]->node_addr, slf_node->neighbrs[intern_gateway]->tcp_port, msg, cmd)) == NULL){
                        
                        printf("Error in rcv_tcp: Failed to redirect: %s\n", msg);
                        return ++success_flag;
                    }
                    if(strcmp(rcv_msg, "1") == 0){

                        printf("\nMessage sent to %s:\n", slf_node->neighbrs[intern_gateway]->node_id);
                        printf("%s\n", msg);
                        if (rcv_msg != NULL) { //reset the pointer to the received message
                            free(rcv_msg);
                            rcv_msg = NULL;
                        }
                        return success_flag;
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
                else{ //send to everybody

                    //extern node
                    if(src_node != atoi(slf_node->extern_node->node_id)){ 

                        // extern is not source
                        if((rcv_msg = tcp_client(&slf_node->client_fd, slf_node->extern_node->node_addr,
                        slf_node->extern_node->tcp_port, msg, cmd)) == NULL){
                            
                            printf("Error in rcv_tcp: Failed to redirect: %s\n", msg);
                            
                        }
                        else if(strcmp(rcv_msg, "1") == 0){

                            printf("\nMessage sent to %s:\n", slf_node->extern_node->node_id);
                            printf("%s\n", msg);
                            if (rcv_msg != NULL) { //reset the pointer to the received message
                                free(rcv_msg);
                                rcv_msg = NULL;
                            }
                            
                        }
                        else{
                            printf("Error in rcv_tcp: Communication with TCP server failed.\n");
                            if (rcv_msg != NULL) { //reset the pointer to the received message
                                free(rcv_msg);
                                rcv_msg = NULL;
                            }                        
                        }
                    }

                    //all intern nodes
                    for(i = 0; i < NODE_IDS; i++){
                        if(slf_node->neighbrs[i] != NULL && src_node != atoi(slf_node->neighbrs[i]->node_id)){
                           
                            fd_gateway = slf_node->neighbrs_fd[i];
                            if((rcv_msg = tcp_client(&fd_gateway, slf_node->neighbrs[i]->node_addr,
                             slf_node->extern_node->tcp_port, msg, cmd)) == NULL){
                                
                                printf("Error in rcv_tcp: Failed to redirect: %s\n", msg);
                                
                            }
                            else if(strcmp(rcv_msg, "1") == 0){

                                printf("\nMessage sent to %s:\n", slf_node->neighbrs[i]->node_id);
                                printf("%s\n", msg);
                                if (rcv_msg != NULL) { //reset the pointer to the received message
                                    free(rcv_msg);
                                    rcv_msg = NULL;
                                }                                
                            }
                            else{
                                printf("Error in rcv_tcp: Communication with TCP server failed.\n");
                                if (rcv_msg != NULL) { //reset the pointer to the received message
                                    free(rcv_msg);
                                    rcv_msg = NULL;
                                }                        
                            }
                        }//if
                    }//for

                }//else 2nd else               
            }//else - not for me
        }
        else {
            printf("Error in rcv_tcp: Failed to read arguments of %s\n", cmd);
            return ++success_flag;
        }
    }

    else if(strcmp(cmd, content_str) == 0){

        if(sscanf(msg, "%*s %s %s %s", id1_cmd, id2_cmd, content_buff) == 3){
            
            //find out which neighbor sent the message
            
            origin = atoi(id2_cmd);

            if(*src_fd == slf_node->client_fd){ // The message reached me through the extern node
                
                src_node = atoi(slf_node->extern_node->node_id);
                printf("\nMessage received from %02d:\n", src_node);
                printf("%s\n", msg);
 
                if(slf_node->route_tab[origin] != src_node){
                    
                    slf_node->route_tab[origin] = src_node;
                    printf("The routing table was updated\n");
                    printf("To send something to %s, use the node %s\n\n", 
                    id2_cmd, slf_node->extern_node->node_id);

                    if (slf_node->route_tab[src_node] == -1) {
                        slf_node->route_tab[src_node] = src_node;
                        printf("To send something to %02d, use the node %02d\n\n", 
                        src_node, src_node);
                    }
                }
            }            
            else{// who is the intern neighbor that sent me the message?
                for(i = 0; i < NODE_IDS; i++){
                    if(*src_fd == slf_node->neighbrs_fd[i]){
                        
                        src_node = i;
                        printf("\nMessage received from %02d:\n", src_node);
                        printf("%s\n", msg);                    
                        
                        if(slf_node->route_tab[origin] != src_node){
                            
                            slf_node->route_tab[origin] = src_node;
                            printf("The routing table was updated\n");
                            printf("To send something to %s, use the node %s\n\n", 
                            id2_cmd, slf_node->neighbrs[i]->node_id);

                            if (slf_node->route_tab[src_node] == -1) {
                                slf_node->route_tab[src_node] = src_node;
                                printf("To send something to %02d, use the node %02d\n\n", 
                                src_node, src_node);
                            }
                        }
                        break;
                    }
                }
            }
            

            if(strcmp(id1_cmd, slf_node->persn_info->node_id) == 0){ //it's for me

                printf("Your QUERY was answered with a CONTENT message\n");
                return success_flag;
            }
            else{ // it's for someone else. Redirect
                 
                search_id = atoi(id1_cmd); //check destination in route_tab                

                if(slf_node->route_tab[search_id] == src_node){ // gateway is equal to the source, discard message
                    printf("rcv_tcp: This received message was discarded: %s\n", msg);
                    return ++success_flag;                
                }
                else if(slf_node->route_tab[search_id] == atoi(slf_node->extern_node->node_id)){ // the gateway to dest is the extern

                    fd_gateway = slf_node->client_fd;
                    
                    printf("Sending %s\n", msg);
                    if((rcv_msg = tcp_client(&fd_gateway, slf_node->extern_node->node_addr, slf_node->extern_node->tcp_port, msg, cmd)) == NULL){
                        
                        printf("Error in rcv_tcp: Failed to send a message\n");
                        return ++success_flag;
                    }
                    if(strcmp(rcv_msg, "1") == 0){
                        printf("\nMessage sent to %s:\n", slf_node->extern_node->node_id);
                        printf("%s\n", msg);
                        if (rcv_msg != NULL) { //reset the pointer to the received message
                            free(rcv_msg);
                            rcv_msg = NULL;
                        }
                        return success_flag;                           
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
                else if(slf_node->route_tab[search_id] != -1){ // the gateway is an intern neighbor

                    intern_gateway = slf_node->route_tab[search_id];
                    fd_gateway = slf_node->neighbrs_fd[intern_gateway];

                    printf("Sending %s\n", msg);
                    if((rcv_msg = tcp_client(&fd_gateway, slf_node->neighbrs[intern_gateway]->node_addr, slf_node->neighbrs[intern_gateway]->tcp_port, msg, cmd)) == NULL){
                       
                       printf("Error in rcv_tcp: Failed to send a message\n");
                       return ++success_flag;
                    }
                    if(strcmp(rcv_msg, "1") == 0){
                        printf("\nMessage sent to %s:\n", slf_node->neighbrs[intern_gateway]->node_id);
                        printf("%s\n", msg);
                        if (rcv_msg != NULL) { //reset the pointer to the received message
                            free(rcv_msg);
                            rcv_msg = NULL;
                        }
                        return success_flag;                           
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
                else{ //send to everybody

                    //extern node
                    if(src_node != atoi(slf_node->extern_node->node_id)){  
                        fd_gateway = slf_node->client_fd;
                        
                        printf("Sending %s\n", msg);
                        if((rcv_msg = tcp_client(&fd_gateway, slf_node->extern_node->node_addr,
                         slf_node->extern_node->tcp_port, msg, cmd)) == NULL){
                            
                            printf("Failed to send a message\n");
                            
                        }
                        if(strcmp(rcv_msg, "1") == 0){
                            printf("\nMessage sent to %s:\n", slf_node->extern_node->node_id);
                            printf("%s\n", msg);
                            if (rcv_msg != NULL) { //reset the pointer to the received message
                                free(rcv_msg);
                                rcv_msg = NULL;
                            }
                                                    
                        }
                        else{
                            printf("Error in rcv_tcp: Communication with TCP server failed.\n");
                            if (rcv_msg != NULL) { //reset the pointer to the received message
                                free(rcv_msg);
                                rcv_msg = NULL;
                            }                        
                        }
                    } 

                    for(i = 0; i < NODE_IDS; i++){ // all intern nodes, except source

                        if(slf_node->neighbrs[i] != NULL && src_node != atoi(slf_node->neighbrs[i]->node_id)){

                            fd_gateway = slf_node->neighbrs_fd[i];
                            if((rcv_msg = tcp_client(&fd_gateway, slf_node->neighbrs[i]->node_addr,
                             slf_node->extern_node->tcp_port, msg, cmd)) == NULL){
                                
                                printf("Error in rcv_tcp: Failed to redirect: %s\n", msg);
                                
                            }
                            else if(strcmp(rcv_msg, "1") == 0){

                                printf("\nMessage sent to %s:\n", slf_node->neighbrs[i]->node_id);
                                printf("%s\n", msg);
                                if (rcv_msg != NULL) { //reset the pointer to the received message
                                    free(rcv_msg);
                                    rcv_msg = NULL;
                                }                                
                            }
                            else{
                                printf("Error in rcv_tcp: Communication with TCP server failed.\n");
                                if (rcv_msg != NULL) { //reset the pointer to the received message
                                    free(rcv_msg);
                                    rcv_msg = NULL;
                                }                        
                            }
                        }//if
                    }//for

                }//else send to everybody
            }//else it's for someone else

        }else{
            printf("Error in rcv_tcp: Failed to read arguments of %s\n", cmd);
            return ++success_flag;
        }
    }

    else if(strcmp(cmd, nocontent_str) == 0){

        if(sscanf(msg, "%*s %s %s %s", id1_cmd ,id2_cmd, content_buff) == 3){
            
            //find out which neighbor sent the message
            
            origin = atoi(id2_cmd);

            if(*src_fd == slf_node->client_fd){ // The message reached me through the extern node
                
                src_node = atoi(slf_node->extern_node->node_id);
                printf("\nMessage received from %02d:\n", src_node);
                printf("%s\n", msg);
 
                if(slf_node->route_tab[origin] != src_node){
                    
                    slf_node->route_tab[origin] = src_node;
                    printf("The routing table was updated\n");
                    printf("To send something to %s, use the node %s\n\n", 
                    id2_cmd, slf_node->extern_node->node_id);

                    if (slf_node->route_tab[src_node] == -1) {
                        slf_node->route_tab[src_node] = src_node;
                        printf("To send something to %02d, use the node %02d\n\n", 
                        src_node, src_node);
                    }
                }
            }            
            else{// who is the intern neighbor that sent me the message?
                for(i = 0; i < NODE_IDS; i++){
                    if(*src_fd == slf_node->neighbrs_fd[i]){
                        
                        src_node = i;
                        printf("\nMessage received from %02d:\n", src_node);
                        printf("%s\n", msg);                        
                        
                        if(slf_node->route_tab[origin] != src_node){
                            
                            slf_node->route_tab[origin] = src_node;
                            printf("The routing table was updated\n");
                            printf("To send something to %s, use the node %s\n\n", 
                            id2_cmd, slf_node->neighbrs[i]->node_id);

                            if (slf_node->route_tab[src_node] == -1) {
                                slf_node->route_tab[src_node] = src_node;
                                printf("To send something to %02d, use the node %02d\n\n", 
                                src_node, src_node);
                            }
                        }
                        break;
                    }
                }
            }        

            if(strcmp(id1_cmd, slf_node->persn_info->node_id) == 0){ //it's for me

                printf("Your QUERY was answered with a NOCONTENT message\n");
                return success_flag;
            }
            else{ // it's for someone else. Redirect
                 
                search_id = atoi(id1_cmd); //check destination in route_tab                

                 if(slf_node->route_tab[search_id] == src_node){ // gateway is equal to the source, discard message
                    printf("rcv_tcp: This received message was discarded: %s\n", msg);
                    return ++success_flag;                
                }
                else if(slf_node->route_tab[search_id] == atoi(slf_node->extern_node->node_id)){ // the gateway to dest is the extern

                    fd_gateway = slf_node->client_fd;
                    
                    printf("Sending %s\n", msg);
                    if((rcv_msg = tcp_client(&fd_gateway, slf_node->extern_node->node_addr, slf_node->extern_node->tcp_port, msg, cmd)) == NULL){
                        
                        printf("Error in rcv_tcp: Failed to send a message\n");
                        return ++success_flag;
                    }
                    if(strcmp(rcv_msg, "1") == 0){
                        printf("\nMessage sent to %s:\n", slf_node->extern_node->node_id);
                        printf("%s\n", msg);
                        if (rcv_msg != NULL){ //reset the pointer to the received message
                            free(rcv_msg);
                            rcv_msg = NULL;
                        }
                        return success_flag;                           
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
                else if(slf_node->route_tab[search_id] != -1){ // the gateway is an intern neighbor

                    intern_gateway = slf_node->route_tab[search_id];
                    fd_gateway = slf_node->neighbrs_fd[intern_gateway];

                    printf("Sending %s\n", msg);
                    if((rcv_msg = tcp_client(&fd_gateway, slf_node->neighbrs[intern_gateway]->node_addr, slf_node->neighbrs[intern_gateway]->tcp_port, msg, cmd)) == NULL){
                       
                       printf("Error in rcv_tcp: Failed to send a message\n");
                       return ++success_flag;
                    }
                    if(strcmp(rcv_msg, "1") == 0){
                        printf("\nMessage sent to %s:\n", slf_node->neighbrs[intern_gateway]->node_id);
                        printf("%s\n", msg);
                        if (rcv_msg != NULL) { //reset the pointer to the received message
                            free(rcv_msg);
                            rcv_msg = NULL;
                        }
                        return success_flag;                           
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
                else{ //send to everybody

                    //extern node
                    if(src_node != atoi(slf_node->extern_node->node_id)){  
                        fd_gateway = slf_node->client_fd;
                        
                        printf("Sending %s\n", msg);
                        if((rcv_msg = tcp_client(&fd_gateway, slf_node->extern_node->node_addr, slf_node->extern_node->tcp_port, msg, cmd)) == NULL){
                            
                            printf("Error in rcv_tcp: Failed to send a message\n");
                            
                        }
                        if(strcmp(rcv_msg, "1") == 0){
                            printf("\nMessage sent to %s:\n", slf_node->extern_node->node_id);
                            printf("%s\n", msg);
                            if (rcv_msg != NULL) { //reset the pointer to the received message
                                free(rcv_msg);
                                rcv_msg = NULL;
                            }                                                    
                        }
                        else{
                            printf("Error in rcv_tcp: Communication with TCP server failed.\n");
                            if (rcv_msg != NULL) { //reset the pointer to the received message
                                free(rcv_msg);
                                rcv_msg = NULL;
                            }                        
                        }
                    } 

                    for(i = 0; i < NODE_IDS; i++){ // all intern nodes, except source

                        if(slf_node->neighbrs[i] != NULL && src_node != atoi(slf_node->neighbrs[i]->node_id)){

                            fd_gateway = slf_node->neighbrs_fd[i];
                            if((rcv_msg = tcp_client(&fd_gateway, slf_node->neighbrs[i]->node_addr, slf_node->extern_node->tcp_port, msg, cmd)) == NULL){
                                
                                printf("Error in rcv_tcp: Failed to redirect: %s\n", msg);
                                
                            }
                            else if(strcmp(rcv_msg, "1") == 0){

                                printf("\nMessage sent to %s:\n", slf_node->neighbrs[i]->node_id);
                                printf("%s\n", msg);
                                if (rcv_msg != NULL){ //reset the pointer to the received message
                                    free(rcv_msg);
                                    rcv_msg = NULL;
                                }                                
                            }
                            else{
                                printf("Error in rcv_tcp: Communication with TCP server failed.\n");
                                if (rcv_msg != NULL) { //reset the pointer to the received message
                                    free(rcv_msg);
                                    rcv_msg = NULL;
                                }                        
                            }
                        }//if
                    }//for

                }//else send to everybody
            }//else it's for someone else            

        }else {
            printf("Error in rcv_tcp: Failed to read arguments of %s\n", cmd);
            return ++success_flag;
        }
    }
    
    else if(strcmp(cmd, withdraw_str) == 0){

        if(sscanf(msg, "%*s %s", id1_cmd) == 1){            
                        
            printf("\nThe following withdraw was received:\n");
            printf("%s\n", msg);

            //remove from route_tab

            search_id = atoi(id1_cmd);            
            
            if(slf_node->route_tab[search_id] != -1){
                slf_node->route_tab[search_id] = -1;
                printf("\n%02d was removed from the routing table as a destination\n", search_id);
            }

            for(i = 0; i < NODE_IDS; i++){
               
               if(slf_node->route_tab[i] == search_id){
                    slf_node->route_tab[i] = -1;
                }
            }
            printf("\n%02d was removed from the routing table as a neighbor\n", search_id);

            if (*src_fd != slf_node->client_fd){ //extern is not source, send WITHDRAW to him
                printf("Sending %s\n", msg);
                fd_gateway = slf_node->client_fd;
                
                if((rcv_msg = tcp_client(&fd_gateway, slf_node->extern_node->node_addr,
                 slf_node->extern_node->tcp_port, msg, cmd)) == NULL){
                    
                    printf("Error in rcv_tcp: Failed to redirect: %s\n", msg);
                    
                }
                else if(strcmp(rcv_msg, "1") == 0){

                    printf("\nMessage sent to %s:\n", slf_node->extern_node->node_id);
                    printf("%s\n", msg);
                    if (rcv_msg != NULL){ //reset the pointer to the received message
                        free(rcv_msg);
                        rcv_msg = NULL;
                    }                                
                }
                else{
                    printf("Error in rcv_tcp: Communication with TCP server failed.\n");
                    if (rcv_msg != NULL) { //reset the pointer to the received message
                        free(rcv_msg);
                        rcv_msg = NULL;
                    }                        
                }
            }

            for (i = 0; i < NODE_IDS; i++) {
                if(slf_node->neighbrs[i] != NULL){
                    if(*src_fd != slf_node->neighbrs_fd[i]){

                        fd_gateway = slf_node->neighbrs_fd[i];
                        if((rcv_msg = tcp_client(&fd_gateway, slf_node->neighbrs[i]->node_addr,
                        slf_node->neighbrs[i]->tcp_port, msg, cmd)) == NULL){
                            
                            printf("Error in rcv_tcp: Failed to redirect: %s\n", msg);
                            
                        }
                        else if(strcmp(rcv_msg, "1") == 0){

                            printf("\nMessage sent to %s:\n", slf_node->neighbrs[i]->node_id);
                            printf("%s\n", msg);
                            if (rcv_msg != NULL){ //reset the pointer to the received message
                                free(rcv_msg);
                                rcv_msg = NULL;
                            }                                
                        }
                        else{
                            printf("Error in rcv_tcp: Communication with TCP server failed.\n");
                            if (rcv_msg != NULL) { //reset the pointer to the received message
                                free(rcv_msg);
                                rcv_msg = NULL;
                            }                        
                        }

                    }//2nd if
                }// 1st fi
            }//for
            
        }
        else {
            printf("Error in rcv_tcp: Failed to read arguments of %s\n", cmd);
            return ++success_flag;
        }
    }

    else if (strcmp(cmd, new_str) == 0) {

        if (sscanf(msg, "%*s %s %s %s",id1_cmd, ip_cmd, tcp_cmd) == 3){
            
            printf("\nMessage received from a new intern neighbor:\n");
            printf("%s\n", msg);

            if (strcmp(slf_node->persn_info->node_id, slf_node->extern_node->node_id) == 0) {  //Only node in the network

                strcpy(slf_node->extern_node->network, slf_node->persn_info->network);
                strcpy(slf_node->extern_node->node_id, id1_cmd);
                strcpy(slf_node->extern_node->tcp_port, tcp_cmd);
                strcpy(slf_node->extern_node->node_addr, ip_cmd);
                slf_node->client_fd = *src_fd;
                slf_node->anchorflag = 1; 
            }
            else {
                slf_node->neighbrs[atoi(id1_cmd)] = contact_init(slf_node->neighbrs[atoi(id1_cmd)]);
                strcpy(slf_node->neighbrs[atoi(id1_cmd)]->network, slf_node->persn_info->network);
                strcpy(slf_node->neighbrs[atoi(id1_cmd)]->node_id, id1_cmd);
                strcpy(slf_node->neighbrs[atoi(id1_cmd)]->tcp_port, tcp_cmd);
                strcpy(slf_node->neighbrs[atoi(id1_cmd)]->node_addr, ip_cmd);
                slf_node->n_neighbrs++;
                slf_node->neighbrs_fd[atoi(id1_cmd)] = *src_fd;
            }
            sprintf(snd_msg, "%s %s %s %s\n", extern_str, slf_node->extern_node->node_id,
             slf_node->extern_node->node_addr, slf_node->extern_node->tcp_port);
            
            printf("Sending %s\n", snd_msg);
            rcv_msg = tcp_client(src_fd, ip_cmd, tcp_cmd, snd_msg, extern_str);
            if (strcmp(rcv_msg, "1") == 0) {
                printf("\nMessage sent to %s:\n", id1_cmd);
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

    else if(strcmp(cmd, extern_str) == 0){

        if(sscanf(msg, "%*s %s %s %s", id1_cmd, ip_cmd, tcp_cmd) == 3){

            if(*src_fd == slf_node->client_fd){ // The message reached me through the extern node
                
                src_node = atoi(slf_node->extern_node->node_id);
                printf("\nMessage received from %02d:\n", src_node);
                printf("%s\n", msg);
                 
            }            
            else{// who is the intern neighbor that sent me the message?
                for(i = 0; i < NODE_IDS; i++){
                    if(*src_fd == slf_node->neighbrs_fd[i]){
                        
                        src_node = i;
                        printf("\nMessage received from %02d:\n", src_node);
                        printf("%s\n", msg);                                                                        
                        break;
                    }
                }
            }        

            printf("Updating the backup neighbor...\n\n");

            memset(slf_node->backup_node->node_id, 0, 3 * sizeof(*slf_node->backup_node->node_id));
            memset(slf_node->backup_node->network, 0, 4 * sizeof(*slf_node->backup_node->network));
            memset(slf_node->backup_node->tcp_port, 0, 6 * sizeof(*slf_node->backup_node->tcp_port));
            memset(slf_node->backup_node->node_addr, 0, 50 * sizeof(*slf_node->backup_node->node_addr));
            strcpy(slf_node->backup_node->network, slf_node->persn_info->network);
            strcpy(slf_node->backup_node->node_id, id1_cmd);
            strcpy(slf_node->backup_node->tcp_port, tcp_cmd);
            strcpy(slf_node->backup_node->node_addr, ip_cmd);

            printf("New backup neighbor:\n\n");                    
            printf("ID: %s\nNetwork: %s\nAdress: %s\nPort: %s\n\n", 
            slf_node->backup_node->node_id, 
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

/* ------------------------UDP Message Handler--------------------------- */

 int send_udp(struct personal_node *slf_node, char *msg) {

    int success_flag = 0;

    char cmd[MAX_MSG_CMD_SIZE];                //buffer for the first string that identifies the message       
    memset(cmd, 0, sizeof(cmd));

    char network[MAX_NET_CHARS];              //block of memory to store the index of the network      
    memset(network, 0, sizeof(network));

    char ls_id[MAX_ID_CHARS];                 
    memset(ls_id, 0, sizeof(ls_id));

    char ls_ip[MAX_ADDRESS_SIZE];                     
    memset(ls_ip, 0, sizeof(ls_ip));

    char ls_tcp[MAX_TCP_UDP_CHARS];                     
    memset(ls_tcp, 0, sizeof(ls_tcp));

    int free_id[NODE_IDS];          
    memset(free_id, 0, sizeof(free_id));

    char aux_id_str[MAX_ID_CHARS];                 
    memset(aux_id_str, 0, sizeof(aux_id_str));

    char aux_msg[MAX_MSG_LENGTH];                  
    memset(aux_msg, 0, sizeof(aux_msg));

    char conf[MAX_MSG_CMD_SIZE];                      
    memset(conf, 0, sizeof(conf));

    const char delim[2] = "\n";
    char *token;
    int flag = 0;
    int cnt = 0;
    char *rcv_msg = NULL; //pointer to the received message, needs to be freed

    //get commnand
    if (sscanf(msg, "%s", cmd) != 1) {
        printf("Error in send_udp: Failed to read command\n");
        return ++success_flag;
    }
    
    //try to read and execute command


    //join - add a node to the network
    if (strcmp(cmd, nodes_str) == 0) {

        if (sscanf(msg, "%*s %s", network) == 1) {
            rcv_msg = udp_client(slf_node->udp_address, slf_node->udp_port, msg); // Get NODESLIST
            if(rcv_msg == NULL){
                printf("Error in send_udp: Failed to get the NODESLIST\n");
                return ++success_flag;
            }

            if (strcmp(rcv_msg, "0") == 0) {
                printf("Error in send_udp: Failed!\n");
                return ++success_flag;
            }            
            token = strtok(rcv_msg, delim);
            while (token != NULL) {             //Go through all the lines and populate an array with the available ID's
                if (flag > 0) {                                             //Iterating line by line, descarding the first one
                    sscanf(token, "%s %s %s", ls_id, ls_ip, ls_tcp);        //ls_id, ls_ip and ls_tcp are the contacts from the node in that line
                    free_id[atoi(ls_id)] = 1;                               //available ID's array
                }
                token = strtok(NULL, delim);
                flag = 1;
            }

            if (ls_id[0] == '\0') {
                strcpy(ls_id, slf_node->persn_info->node_id);
                strcpy(ls_ip, slf_node->persn_info->node_addr);
                strcpy(ls_tcp, slf_node->persn_info->tcp_port);
            }

            if (free_id[atoi(slf_node->persn_info->node_id)] != 0) {     //If the chosen ID is available, this if statmnent is skipped   
                for (cnt = 0, flag = 0; cnt < 99 && flag == 0; cnt++) {  //If the chosen ID isn't available, the first one available in free_id will be chosen   
                    if (free_id[cnt] == 0) {
                        sprintf(aux_id_str, "%02d", cnt);
                        strcpy(slf_node->persn_info->node_id, aux_id_str);
                        flag = 1;
                        printf("The chosen ID is not available!\nThe assigned ID is: %s\n", slf_node->persn_info->node_id);
                    }
                }
                if (flag == 0) {                                       
                    printf("Error in send_udp: The network is full!\n");
                    return ++success_flag;
                }
            }
            //djoin, TCP connection with neighbor

            memset(slf_node->persn_info->network, 0, 4*sizeof(char));
            if(djoin(slf_node, network, slf_node->persn_info->node_id, ls_id, ls_ip, ls_tcp) == 1) {
                printf("send_udp->join->djoin: Failed!\n");
                return ++success_flag;
            }
            //register node in node server
            if (rcv_msg != NULL) {
                free(rcv_msg);
            }
            sprintf(aux_msg, "%s %s %s %s %s\n", reg_str, slf_node->persn_info->network, slf_node->persn_info->node_id, 
            slf_node->persn_info->node_addr, slf_node->persn_info->tcp_port);

            rcv_msg = udp_client(slf_node->udp_address, slf_node->udp_port, aux_msg);
            if(rcv_msg == NULL){
                printf("Error in send_udp: Failed to register in the network\n");
                return ++success_flag;
            }
            if (strcmp(rcv_msg, "0") == 0) {
                printf("Error in send_udp: Failed!\n");
                return ++success_flag;
            }
            // if (sscanf(conf, "%s", rcv_msg) != 1) {
            //     printf("send_udp: Confirmation failed!\n");
            //     return ++success_flag;
            // } 
            if (strcmp(rcv_msg, okreg_str) != 0) {
                printf("Error in send_udp: Confirmation failed!\n");
                return ++success_flag;
            }

        }
        else {
            printf("Error in send_udp: Failed to read arguments of %s\n", cmd);
            return ++success_flag;
        }
    }
    
    //exit of a node from the network
    else if (strcmp(cmd, unreg_str) == 0) {

        if (sscanf(msg, "%*s %s %s", network, ls_id) == 2) {

            rcv_msg = udp_client(slf_node->udp_address, slf_node->udp_port, msg); // Send UNREG

            if(rcv_msg == NULL){
                printf("Error in send_udp: Failed to unregister the node\n");
                return ++success_flag;
            }

            if (strcmp(rcv_msg, "0") == 0) {
                printf("Error in send_udp: Failed!\n");
                return ++success_flag;
            }
            // else if (sscanf(conf, "%s", rcv_msg) != 1) {
            //     printf("send_udp: Confirmation failed!\n");
            //     return ++success_flag;
            // } 
            else if (strcmp(rcv_msg, okunreg_str) != 0) {
                printf("Error in send_udp: Confirmation failed!\n");
                return ++success_flag;
            }
        }
        else {
            printf("Error in send_udp: Failed to read arguments of %s\n", cmd);
            return ++success_flag;
        }
    }

    if (rcv_msg != NULL) {
        free(rcv_msg);
    }
    return success_flag;
}

/* ----------------------------TCP Client-------------------------------- */

 char *tcp_client(int *fd, char *server_IP, char *server_TCP, char *msg, char *cmd) {

    struct addrinfo srv_criteria, *srv_result; 
    int errflag;
    ssize_t nbytes, nleft, nwritten;
    char *ptr;
    char *buffer = (char*)calloc(MAX_MSG_LENGTH, sizeof(char));
    struct sigaction act;


    if (*fd == -1) {
        *fd = socket(AF_INET, SOCK_STREAM, 0);
        if(*fd == -1){
            printf("Error initializing socket.\n");
            return NULL;
        }
        
        memset(&act, 0, sizeof(act));
        act.sa_handler = SIG_IGN;

        if(sigaction(SIGPIPE, &act, NULL) == -1){
            printf("Error in sigaction().\n");            
            return NULL;
        }

        memset(&srv_criteria, 0, sizeof(srv_criteria));
        srv_criteria.ai_family = AF_INET;       //IPv4 address family
        srv_criteria.ai_socktype = SOCK_STREAM; // TCP socket

        errflag = getaddrinfo(server_IP, server_TCP, &srv_criteria, &srv_result);
        if(errflag != 0){
            printf("Error in getaddrinfo()\n");
            return NULL;
        }

        errflag = connect(*fd, srv_result->ai_addr, srv_result->ai_addrlen);
        if(errflag == -1){
            printf("Error in connect()\n");
            return NULL;
        }

        freeaddrinfo(srv_result);
    }
    ptr = strcpy(buffer, msg);
    nbytes = strlen(msg);
    nleft = nbytes;

    while(nleft > 0){
        nwritten = write(*fd, ptr, nleft);
        if(nwritten <= 0){
            printf("Error in write\n");
            return NULL;
        }
        nleft -= nwritten;
        ptr += nwritten;
    }
   
    memset(buffer, 0, MAX_MSG_LENGTH * sizeof(char)); //set the buffer to '\0' //!lembrar
    strcpy(buffer, "1"); //indicates that the message was sent
   
    return buffer; // returning what was inside the file descriptor
}

/* ----------------------------UDP Client-------------------------------- */

 char *udp_client(char *server_IP, char *server_UDP, char *msg){
    
    struct addrinfo srv_criteria, *srv_result;
    int fd, errflag;
    ssize_t cnt;
    struct sockaddr_in addr_conf;
    socklen_t addrlen_conf;
    char *buffer = (char*)calloc(MAX_NODESLIST, sizeof(char));

    fd = socket(AF_INET, SOCK_DGRAM, 0) ;
    if(fd == -1) return NULL;

    memset(&srv_criteria, 0, sizeof(srv_criteria));
    srv_criteria.ai_family = AF_INET;
    srv_criteria.ai_socktype = SOCK_DGRAM;

    errflag = getaddrinfo(server_IP,server_UDP, &srv_criteria, &srv_result);
    if(errflag != 0) return NULL;

    cnt = sendto(fd, msg, strlen(msg), 0, srv_result->ai_addr, srv_result->ai_addrlen);
    if(cnt == -1) return NULL;

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

/* ----------------------------TCP Server-------------------------------- */

int tcp_server(struct personal_node *slf_node) {    

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

    slf_node->server_fd = socket(AF_INET, SOCK_STREAM, 0); // personal server socket for the intern nodes
    if (slf_node->server_fd < 0) {
        printf("Error in socket()\n");
        return ++success_flag;
        
    }

    slf_node->max_fd = slf_node->server_fd;

    memset(&srv_criteria, 0, sizeof(srv_criteria)); // set all bytes inside "criteria" to zero
    srv_criteria.ai_family = AF_INET;               // family of IPv4 addresses
    srv_criteria.ai_socktype = SOCK_STREAM;         // TCP socket
    srv_criteria.ai_flags = AI_PASSIVE;             // passive socket that waits for requests

    // pass the information in "criteria" to "srv_result" 

    errflag = getaddrinfo(NULL, slf_node->persn_info->tcp_port, &srv_criteria, &srv_result);
    if (errflag != 0) {
        printf("Error in getaddrinfo()\n");
        return ++success_flag;
    }

    // bind the address in "srv_result" to the personal socket

    errflag = bind(slf_node->server_fd, srv_result->ai_addr, srv_result->ai_addrlen);
    if (errflag == -1) {
        printf("Error in bind()\n");
        return ++success_flag;
    }

    freeaddrinfo(srv_result);
    // Set the personal socket to listen to requests. Only 6 requests at a time

    errflag = listen(slf_node->server_fd, MAX_QUEUE_LENGTH_TCP); // the second argument refers to the number of pending connections allowed in the queue
    if (errflag == -1) {
        printf("Error in listen()\n");
        return ++success_flag;
    }

    FD_ZERO(&slf_node->crr_scks);              // Set set of FD's to zero
    FD_SET(STDIN_FILENO, &slf_node->crr_scks); // add stdin(keyboard input) to FD set
    FD_SET(slf_node->server_fd, &slf_node->crr_scks);           // add server socket to FD set

    // Infinite cycle where the communications happen

    ptimeout = NULL;    //select waits for a indeterminate amount of time

    

    while (1) {
    
        printf("\nChecking activity...\n\n");
        //In case old external neigbor disconnected, add new one to FD set
        if ((slf_node->client_fd != -1) && (!FD_ISSET(slf_node->client_fd, &slf_node->crr_scks))) {
            FD_SET(slf_node->client_fd, &slf_node->crr_scks);
            if (slf_node->client_fd > slf_node->max_fd) slf_node->max_fd = slf_node->client_fd;
        }

        //remove all FDs with no activity from FD set slf_node->rdy_scks
        slf_node->rdy_scks = slf_node->crr_scks;
    
        select_ctrl = select(slf_node->max_fd + 1, &slf_node->rdy_scks, (fd_set*)NULL, (fd_set*)NULL, ptimeout);

        if (select_ctrl < 0) {         
            printf("Error in select: %s. Terminating process\n", strerror(errno));
            return ++success_flag;    
        }
        if (select_ctrl == 0) {     //Timeout
            printf("SELECT TIMEOUT!!!\n");
            ptimeout = NULL;
            return 1;
        }

        //Go through all FDs and see which ones have activity
        for (fd_itr = 0; fd_itr < slf_node->max_fd + 1; fd_itr++) {

            if (FD_ISSET(fd_itr, &slf_node->rdy_scks)) {
                if (fd_itr == slf_node->server_fd && strcmp(slf_node->persn_info->network, "") != 0) {
                    srv_addrlen = sizeof(srv_addr);
                    if((new_fd = accept(slf_node->server_fd, &srv_addr, &srv_addrlen)) == -1){
                        printf("Error in accept(). Request rejected\n");
                        return 1;
                    }
                    
                    FD_SET(new_fd, &slf_node->crr_scks);
                    if (new_fd > slf_node->max_fd) slf_node->max_fd = new_fd;
                    timeout.tv_sec = TIMEOUT_VALUE_s;
                    timeout.tv_usec = 0;
                    ptimeout = &timeout;
                    
                }
                else if (fd_itr == STDIN_FILENO) {
                    memset(&buffer, 0, sizeof(buffer)); //set the buffer to '\0'                                         
                    fgets(buffer, MAX_MSG_LENGTH-1, stdin);
                    printf("\n_________________________________________________________________\n");
                    printf("Handling your command...");
                    printf("\n_________________________________________________________________\n\n");

                    select_cmd(slf_node, buffer); 
                    ptimeout = NULL;

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
                        return 1;
                    }

                    else if (nread == 0) {  //the fd is disconnected
                        
                        memset(&buffer, 0, sizeof(buffer));
                        
                        if (fd_itr == slf_node->client_fd) {    //External neighbor disconnected

                            disc_nodeid = atoi(slf_node->extern_node->node_id);

                            printf("External neighbor (Client %s, with FD: %d) disconnected!\n", slf_node->extern_node->node_id, fd_itr);
                            
                            slf_node->client_fd = -1;   // Reset client_fd to -1

                            //send withdraw msg to all internal neighbors
                            if (slf_node->n_neighbrs > 0) {
                                memset(msg, 0, sizeof(msg));
                                sprintf(msg, "%s %02d\n", withdraw_str, atoi(slf_node->extern_node->node_id));
                                for (i = 0; i < NODE_IDS; i++) {
                                    if (slf_node->neighbrs[i] != NULL) {
                                        send_tcp(slf_node, slf_node->neighbrs[i], msg);
                                    }
                                }
                                sleep(0.5);
                            }
                            
                            if (strcmp(slf_node->backup_node->node_id, slf_node->persn_info->node_id) != 0) { //not an anchor                                                        
                                contact_copy(slf_node->extern_node, slf_node->backup_node);
                                memset(slf_node->backup_node->node_id, 0, 3 * sizeof(*slf_node->backup_node->node_id));
                                memset(slf_node->backup_node->network, 0, 4 * sizeof(*slf_node->backup_node->network));
                                memset(slf_node->backup_node->tcp_port, 0, 6 * sizeof(*slf_node->backup_node->tcp_port));
                                memset(slf_node->backup_node->node_addr, 0, 50 * sizeof(*slf_node->backup_node->node_addr));

                                memset(msg, 0, sizeof(msg));
                                sprintf(msg, "%s %s %s %s\n", new_str, slf_node->persn_info->node_id, slf_node->persn_info->node_addr, slf_node->persn_info->tcp_port);
                                //Sends NEW message , receives EXTERN response and updates backup node
                                if(send_tcp(slf_node, slf_node->extern_node, msg) == 1){
                                    printf("Failed to send a message\n");
                                }

                                memset(msg, 0, sizeof(msg));
                                    sprintf(msg, "%s %s %s %s\n", extern_str, slf_node->extern_node->node_id, 
                                        slf_node->extern_node->node_addr, slf_node->extern_node->tcp_port);
                                    for (j = 0; j < NODE_IDS; j++) {
                                        if (slf_node->neighbrs[j] != NULL) {
                                            send_tcp(slf_node, slf_node->neighbrs[j], msg);
                                        }
                                    }

                                if (strcmp(slf_node->persn_info->node_id, slf_node->backup_node->node_id) == 0) {
                                    slf_node->anchorflag = 1;
                                }                            
                            }
                            else if (slf_node->n_neighbrs > 0) { //anchor
                                for (i = 0; i < NODE_IDS; i++) {
                                    if (slf_node->neighbrs[i] != NULL) {
                                        contact_copy(slf_node->extern_node, slf_node->neighbrs[i]);
                                        slf_node->client_fd = slf_node->neighbrs_fd[i];

                                        memset(msg, 0, sizeof(msg));
                                        sprintf(msg, "%s %s %s %s\n", extern_str, slf_node->extern_node->node_id, 
                                            slf_node->extern_node->node_addr, slf_node->extern_node->tcp_port);
                                        for (j = 0; j < NODE_IDS; j++) {
                                            if (slf_node->neighbrs[j] != NULL) {
                                                send_tcp(slf_node, slf_node->neighbrs[j], msg);
                                            }
                                        }
                                        free_contact(slf_node->neighbrs[i]);
                                        slf_node->neighbrs[i] = NULL;
                                        slf_node->n_neighbrs--;
                                        slf_node->neighbrs_fd[i] = -1;
                                        break;
                                    }
                                }
                            }
                            else {  //is now the only node in the network
                                slf_node->anchorflag = 1;
                                contact_copy(slf_node->extern_node, slf_node->persn_info);
                                contact_copy(slf_node->backup_node, slf_node->persn_info);
                            }
                            
                        }
                        else{                     //Internal neighbor disconnected

                            int flag = 0;
                            //find the id of the node that corresponds to the disconnected fd
                            for (i = 0; i < NODE_IDS; i++) {
                                if (slf_node->neighbrs_fd[i] == fd_itr) {
                                    disc_nodeid = i;
                                    flag++;                                    
                                    break;
                                }
                            }
                            
                            if(flag == 1){
                                printf("Client: %02d, with FD: %d, disconnected!\n", i, fd_itr);
                                

                                if(slf_node->neighbrs[disc_nodeid] != NULL){
                                    free_contact(slf_node->neighbrs[disc_nodeid]);
                                    slf_node->neighbrs[disc_nodeid] = NULL;
                                    slf_node->n_neighbrs--;
                                    slf_node->neighbrs_fd[disc_nodeid] = -1;
                                    
                                }
                                

                                //send WHITHDRAW msg
                                memset(msg, 0, sizeof(msg));
                                sprintf(msg, "%s %02d\n", withdraw_str, disc_nodeid);

                                if(send_tcp(slf_node, slf_node->extern_node, msg) == 1){
                                    printf("Failed to send a message\n"); //msg for external neighbor 
                                }
                                for (i = 0; i < NODE_IDS; i++) {
                                    if (slf_node->neighbrs[i] != NULL) {
                                        if(send_tcp(slf_node, slf_node->neighbrs[i], msg) == 1){//msg for internal neighbors
                                            printf("Failed to send a message\n");
                                        }     
                                    }
                                }
                            }            

                        }                               
                        slf_node->route_tab[disc_nodeid] = -1;
                        printf("%02d was removed from the routing table as a destination\n", disc_nodeid);

                        for(int i = 0; i < NODE_IDS; i++){
                        
                            if(slf_node->route_tab[i] == disc_nodeid){
                                slf_node->route_tab[i] = -1;
                            }
                        }
                        printf("%02d was removed from the routing table as a neighbor\n", disc_nodeid);
                        FD_CLR(fd_itr, &slf_node->crr_scks);                                                        
                    }
                    else{                        
                        
                        if(rcv_tcp(slf_node, buffer, &fd_itr) == 1){
                            printf("Failed to send %s\n", buffer);
                        }
                    }//else
                    ptimeout = NULL;
                }
            }//if
        }//for
    }//while

}//tcp_server
