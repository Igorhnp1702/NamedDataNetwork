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

 #include "ndn_messages.h"

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

 int node_reg(char *server_IP, char *server_UDP, char *node_IP, char *node_TCP, char *net){ // register the node

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
    if(fd == -1) return success;

    memset(&srv_criteria, 0, sizeof(srv_criteria));
    srv_criteria.ai_family = AF_INET;
    srv_criteria.ai_socktype = SOCK_DGRAM;

    errflag = getaddrinfo(server_IP,server_UDP, &srv_criteria, &srv_result);
    if(errflag != 0) return success;

    sprintf(reg_msg, "%s %s %s %s\n", reg_str, net, node_IP, node_TCP);

    cnt = sendto(fd, reg_msg, strlen(reg_msg), 0, srv_result->ai_addr, srv_result->ai_addrlen);
    if(cnt == -1) return success;

    printf("Registration request sent to the node server: %s\n\n", reg_msg);

    freeaddrinfo(srv_result);

    addrlen_conf = sizeof(struct sockaddr_in);
    cnt = recvfrom(fd, buffer, 6, 0,(struct sockaddr *)&addr_conf, &addrlen_conf); // retrieve the same 6 bytes you allocated for the response
    if(cnt == -1) return success;
    buffer[cnt] = '\0';

    if (strcmp(inet_ntoa(addr_conf.sin_addr), server_IP) != 0) { //Verification
        strcpy(buffer, "0");
    }

    close(fd);
    return ++success;
 }

 int node_unreg(char *server_IP, char *server_UDP, char *node_IP, char *node_TCP, char *net){ // unregister the node

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
    if(fd == -1) return success;

    memset(&srv_criteria, 0, sizeof(srv_criteria));
    srv_criteria.ai_family = AF_INET;
    srv_criteria.ai_socktype = SOCK_DGRAM;

    errflag = getaddrinfo(server_IP,server_UDP, &srv_criteria, &srv_result);
    if(errflag != 0) return success;

    sprintf(unreg_msg, "%s %s %s %s\n", unreg_str, net, node_IP, node_TCP);

    cnt = sendto(fd, unreg_msg, strlen(unreg_msg), 0, srv_result->ai_addr, srv_result->ai_addrlen);
    if(cnt == -1) return success;

    printf("Unregistration request sent to the node server: %s\n\n", unreg_msg);

    freeaddrinfo(srv_result);

    addrlen_conf = sizeof(struct sockaddr_in);
    cnt = recvfrom(fd, buffer, 6, 0,(struct sockaddr *)&addr_conf, &addrlen_conf); // retrieve the same 6 bytes you allocated for the response
    if(cnt == -1) return success;
    buffer[cnt] = '\0';

    if (strcmp(inet_ntoa(addr_conf.sin_addr), server_IP) != 0) { //Verification
        strcpy(buffer, "0");
    }

    close(fd);
    return ++success;
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

