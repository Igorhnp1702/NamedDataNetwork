/************************************************************************************************
 * ndn_main.c
 *
 * Authors:
 *
 * -Igor Paruque ist1102901
 * -Mónica Ferreira ist106520
 *
 * Description: header file for the input/output functions
 ***********************************************************************************************/

#ifndef ndn_io_header
#define ndn_io_header

#define DEFAULT_REGIP "193.136.138.142" // default IPv4 address of the node server
#define DEFAULT_REGUDP "59000"          // default UDP port of the node server
#define NODE_IDS 100                    // amount of different ids the nodes can have: 00 - 99  
#define MAX_QUEUE_LENGTH_TCP 6          // maximium size of the tcp queue
#define MAX_NODESLIST 4000              // size of the buffer for the NODESLIST response
#define MAX_MSG_LENGTH 200              // size of the buffer for the messages (incoming/outgoing)
#define MAX_USR_CMD_LENGTH 150          // size of the buffer for the commands
#define MAX_CONTENT_NAME 101            // size of the buffer for the contents (max chars = 100 + null terminator)
#define MAX_ADDRESS_SIZE 50             // size of the buffer for non_determinístic ip address
#define MAX_ID_CHARS 3                  // number of chars required for the id (with '\0')
#define MAX_NET_CHARS 4                 // number of chars required for the network number (with '\0')
#define MAX_TCP_UDP_CHARS 6             // number of chars required for the tcp/udp ports(with '\0')
#define MAX_MSG_CMD_SIZE 10             // size of the biggest message protocol (NOCONTENT = 9 bytes + null terminator)
#define TIMEOUT_VALUE_s 5  //!repor!!!             // timeout value in seconds 

// set of strings for the commands

#define exit_str  "exit"        // user cmd exit
#define leave_str  "leave"      // user cmd leave
#define join_str  "join"       // user cmd join
#define djoin_str  "djoin"      // user cmd djoin
#define create_str  "create"    // user cmd create
#define delete_str  "delete"    // user cmd delete
#define show_topology_str "st" // user cmd st (show topology)
#define show_names_str "sn"    // user cmd sn (show names)
#define show_routing_str "sr"  // user cmd sr (show routing)
#define clear_routing_str "cr" // user cmd cr (clear routing)
#define clear_names_str "cn"   // user cmd cn (clear names)
#define load "load"            // user cmd load
#define get_str  "get"          // user cmd get
#define help_str "help"        // user cmd help

// set of strings for the messages

#define reg_str "REG"             // UDP msg REG
#define okreg_str "OKREG"         // UDP msg OKREG
#define unreg_str "UNREG"         // UDP msg UNREG
#define okunreg_str "OKUNREG"     // UDP msg OKUNREG
#define nodes_str "NODES"         // UDP msg NODES
#define query_str "QUERY"         // TCP msg QUERY
#define content_str "CONTENT"     // TCP msg CONTENT
#define nocontent_str "NOCONTENT" // TCP msg NOCONTENT
#define withdraw_str "WITHDRAW"   // TCP msg WITHDRAW
#define new_str "NEW"             // TCP msg NEW
#define extern_str "EXTERN"       // TCP msg EXTERN

int is_valid_ip(const char *ip);

int check_num_string(char *num_string);

#endif

 