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
#define show_interest_table_str "si"  // user cmd si (show interest table)
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


/**************************************************************************
 * nodeinfo_t
 *
 *  Description:
 *
 *  -Data type in the form of a struct that holds the contact of a node;
 *
 *  Members of the struct:
 *
 *  -network = index of the network; integer from 000 to 999;
 *  -node_id = tcp port of the node; integer from 0 to 65 536;
 *  -node_addr = index of the node; integer from 00 to 99;
 *  -node_port = IPv4 address with undefined size;
 ***************************************************************************/
 typedef struct any_node
 {
    char *network;   // index of the network; integer from 000 to 999
    char *tcp_port;  // tcp port of the node; integer from 0 to 65 536
    char *node_id;   // index of the node; integer from 00 to 99
    char *node_addr; // IPv4 address with undefined size
 } nodeinfo_t;
 
 /**************************************************************************
  * struct Linkedlist
  * 
  * Description: 
  * 
  *  -Structure of variables to build linked lists of strings
  * 
  * Members of the struct
  * 
  *  -string = dynamic array of chars; 
  *  -next = pointer to the next string; 
  **************************************************************************/
 struct Linkedlist{
    char *string;            // dynamic array of chars
    struct Linkedlist *next; // pointer to the next name
 };
 
 /**************************************************************************
  * contentList_t
  * 
  * Description: Data type for the list of contents;
  ***************************************************************************/
 typedef struct Linkedlist contentList_t;
 
 /***************************************************************************
  * queryList_t
  * 
  * Description: Data type for the list of queries sent;
  ***************************************************************************/
 typedef struct Linkedlist queryList_t;
 
 /*****************************************************************************
  * personal_node
  *
  *  Description:
  *
  *  -Structure of data that will hold all variables regarding the personal node;
  *
  *  Members of the struct:
  *
  *  -persn_info  = contact of the personal node;
  *  -anchorflag  = flag that indicates if the node is an anchor or not
  *  -n_neighbrs  = counter for the number of internal neighbors;
  *  -udp_fd      = file descriptor to communicate with the node server
  *  -server_fd   = file descriptor to communicate with the intern nodes
  *  -max_fd      = the maximum integer assigned to a file descriptor in this node's FD set
  *  -client_fd   = file descriptor to communicate with the extern node
  *  -neighbrs_fd = array that relates internal neighbrs id's with their fd. The indexes match the nodes's ids
  *  -route_tab   = expedition table. The indexes of the array are the destinations
  *  -udp_port    = UDP port of the server of nodes
  *  -udp_address = UDP address of the server of nodes
  *  -rdy_scks    = set of file descriptors to read from
  *  -crr_scks    = set of file descriptors to read from
  *  -contents    = linked list of the contents of the node
  *  -extern_node = contact of the extern neighbor node
  *  -backup_node = contact of the backup neighbor node
  *  -neighbrs    = array of contacts of intern neighbors. 
  *                   The indexes match the nodes's ids
  ****************************************************************************/
 struct personal_node
 {
    nodeinfo_t *persn_info;    // contact of the personal node
    int anchorflag;            // flag that says whether the node is an anchor or not
    int n_neighbrs;            // counter for the number of internal neighbors
    int udp_fd;                // file descriptor to communicate with the server of nodes//!não esta a ser utilizado
    int server_fd;             // file descriptor to communicate with the intern nodes
    int max_fd;                // the maximum integer assigned to a file descriptor in this node's FD set
    int client_fd;             // file descriptor to communicate with the extern node
    int neighbrs_fd[NODE_IDS]; // array that relates internal neighbrs id's with their fd. The indexes match the nodes's ids
    int *route_tab;            // expedition table. The indexes of the array are the destinations
    char *udp_port;            // UDP port of the server of nodes
    char *udp_address;         // UDP address of the server of nodes
    fd_set rdy_scks;           // set of file descriptors with activity to handle
    fd_set crr_scks;           // set of file descriptors in use
    queryList_t *qryhistory;   // history of queries sent
    contentList_t *contents;   // linked list of the contents of the node
    nodeinfo_t *extern_node;   // contact of the extern neighbor node
    nodeinfo_t *backup_node;   // contact of the backup neighbor node
    nodeinfo_t **neighbrs;     // array of contacts of intern neighbors. The indexes match the nodes's ids
 };


int is_valid_ip(const char *ip);

int check_num_string(char *num_string);

#endif

 