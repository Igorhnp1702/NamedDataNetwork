/************************************************************************************************
 * g23tcp.h
 *
 *  Authors:
 *
 *  -Igor Paruque ist1102901
 *  -Afonso Klier ist196139
 * 
 *  Description: header file for g23tcp_udp.c
 ***********************************************************************************************/

#ifndef _G23TCP_UDPH_
#define _G23TCP_UDPH_

//#define _XOPEN_SOURCE 600 //!REMOVE BEFORE COMPILING

#include "g23funcs.h"

/*******************************************************************
 * send_tcp()
 *
 *  Description:
 *
 *  -Sends a message over TCP and if there is a response from the destinations, it handles it. 
 *
 *  Arguments:
 *
 *  -slf_node: personal node.
 *  -dest_node: Node where the message will be sent to
 *  -msg: message to be sent (with '\0' at the end of the string).
 *
 *  Return: 0 = success; 1 = error
 ***************************************************************/
 int send_tcp(struct personal_node *slf_node, nodeinfo_t *dest_node, char *msg);

/*******************************************************************
 * rcv_tcp()
 *
 *  Description:
 *
 *  
 *  -To be called when the personal node receives a message over TCP.
 *  -Processes the message and if needed sends a response to the server.
 *
 *  Arguments:
 *
 *  -src_fd = file descriptor that sent (pass by reference)
 *  -slf_node: personal node.
 *  -msg: message received (with '\0' at the end of the string).
 *
 *  Return: 0 = success; 1 = error
 ***************************************************************/
 int rcv_tcp(struct personal_node *slf_node, char *msg, int *src_fd);

/*******************************************************************
 * send_udp()
 *
 *  Description:
 *
 *  - -Sends a message over UDP and handles the responce from the server
 *
 *  Arguments:
 *
 *  -slf_node: personal node.
 *  -msg: message received (with '\0' at the end of the string).
 *
 *  Return: 0 = success; 1 = error
 ***************************************************************/
 int send_udp(struct personal_node *slf_node, char *msg);

/*******************************************************************
 * tcp_client()
 *
 *  Description:
 *
 *  -TCP Client;
 *  -All strings (arguments and return) including '\0' at the end)
 *  -
 *
 *  Arguments:
 * 
 *  -fd = file descriptor to be used to send the message (pass by reference)
 *  -server_IP: ip of the TCP server to which "msg" will be sent.
 *  -server_TCP: TCP port of the TCP server to which "msg" will be sent.
 *  -msg: message to be sent (with '\0' at the end of the string).
 *  -cmd: first argument of "msg"
 *
 *  Return: a pointer to a string wich contains the return message from the server.
 *          (ends with '\0'), or NULL in case of error
 ***************************************************************/
 char *tcp_client(int *fd, char *server_IP, char *server_TCP, char *msg, char *cmd);

/*******************************************************************
 * udp_client()
 *
 *  Description:
 *
 *  -UDP Client;
 *  -All strings (arguments and return) including '\0' at the end)
 *
 *  Arguments:
 *
 *  -server_IP: ip of the UDP server to which "msg" will be sent.
 *  -server_UDP: UDP port of the UDP server to which "msg" will be sent.
 *  -msg: message to be sent (with '\0' at the end of the string).
 *
 *  Return: a pointer to a string wich contains the return message from the server.
 *          (ends with '\0'), or NULL in case of error
 ***************************************************************/
 char *udp_client(char *server_IP, char *server_UDP, char *msg);

/*******************************************************************
 * tcp_server()
 *
 *  Description:
 *
 *  -TCP Server; 
 *  -Also handles inputs from stdin
 *  -Calls different funcions depending on the msg received
 *
 *  Arguments:
 *
 *  -slf_node: personal node
 *
 *  Return: 0 = success; 1 = error
 ***************************************************************/
 int tcp_server(struct personal_node *slf_node);

 #endif