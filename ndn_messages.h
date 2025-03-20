
/************************************************************************************************
 * ndn_messages.h
 *
 * Authors:
 *
 * -Igor Paruque ist1102901
 * -Mónica Ferreira ist106520
 *
 * Description: header file for the messages
 ***********************************************************************************************/

 #ifndef ndn_messages_header
 #define ndn_messages_header
 
 #define DEFAULT_REGIP "193.136.138.142"  // default IPv4 address of the node server
 #define DEFAULT_REGUDP "59000"           // default UDP port of the node server
 #define MAX_QUEUE_LENGTH_TCP 6           // maximium size of the tcp queue
 #define MAX_NODESLIST 6000               // size of the buffer for the NODESLIST response
 #define MAX_MSG_LENGTH 200               // size of the buffer for the messages (incoming/outgoing)
 #define MAX_USR_CMD_LENGTH 150           // size of the buffer for the commands
 #define MAX_OBJECT_NAME 101              // size of the buffer for the contents (max chars = 100 + null terminator)
 #define MAX_ADDRESS_SIZE 16              // size of the buffer for non_determinístic ip address
 #define MAX_NET_CHARS 4                  // number of chars required for the network number (with '\0')
 #define MAX_TCP_UDP_CHARS 6              // number of chars required for the tcp/udp ports(with '\0')
 #define MAX_MSG_CMD_SIZE 9               // size of the biggest message protocol (NOOBJECT = 8 bytes + null terminator)


// set of strings for the messages

// interactions with the node server
#define reg_str "REG"               // UDP msg REG
#define okreg_str "OKREG"           // UDP msg OKREG
#define unreg_str "UNREG"           // UDP msg UNREG
#define okunreg_str "OKUNREG"       // UDP msg OKUNREG
#define nodes_str "NODES"           // UDP msg NODES
#define nodeslist_str "NODESLIST"   // UDP msg NODESLIST 

// interactions with other nodes for tree maintenance
#define entry_str "ENTRY"       // TCP msg ENTRY
#define safe_str "SAFE"         // TCP msg SAFE

// interactions with other nodes to exchange objects
#define interest_msg_str "INTEREST"    // TCP msg INTEREST
#define object_str "OBJECT"     // TCP msg OBJECT
#define noobject_str "NOOBJECT" // TCP msg NOOBJECT

// project libraries
#include "ndn_node.h"



nodesLinkedlist_t *server_inquiry(char *server_IP, char *server_UDP, char *msg);// request the list of nodes in the network

char *node_reg(char *server_IP, char *server_UDP, char *node_IP, char *node_TCP, char *net);

char *node_unreg(char *server_IP, char *server_UDP, char *node_IP, char *node_TCP, char *net);

char* send_entry(int *fd, char *mynode_ip, char *mynode_tcp, char *dest_ip, char *dest_tcp);

char *send_safe(int fd, char *ext_ip, char *ext_tcp);

int parse_tcp(struct personal_node *slf_node, char *msg, nodeinfo_t *src_node);

char *parseNstore(char msg_bffr[], char **node_bffr);

#endif
