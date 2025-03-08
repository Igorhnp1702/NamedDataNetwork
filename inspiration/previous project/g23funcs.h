/************************************************************************************************
 * g23funcs.h
 *
 *  Authors:
 *
 *  -Igor Paruque ist1102901
 *  -Afonso Klier ist196139
 *
 *  Description: function declarations for g23funcs.c
 ***********************************************************************************************/

#ifndef _G23FUNCSH_
#define _G23FUNCSH_

// #define _XOPEN_SOURCE 600 //!REMOVE BEFORE COMPILING


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

#include <sys/select.h>

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
   fd_set rdy_scks;           // set of file descriptors with activity to handle (rdy = ready)
   fd_set crr_scks;           // set of file descriptors in use (crr = current)
   queryList_t *qryhistory;   // history of queries sent (qry = querry)
   contentList_t *contents;   // linked list of the contents of the node
   nodeinfo_t *extern_node;   // contact of the extern neighbor node
   nodeinfo_t *backup_node;   // contact of the backup neighbor node
   nodeinfo_t **neighbrs;     // array of contacts of intern neighbors. The indexes match the nodes's ids
};

/**************************************************************************
 * check_id()
 *
 *  Description:
 *
 *  -Check if the id is valid;
 *
 *  Arguments:
 *
 *  -id = index of a node;
 *
 *  Return: 0 = valid; 1 = invalid
 *************************************************************************/
 int check_id(char *id);

/***************************************************************************
 * check_name()
 *
 * Description:
 *
 *  -Check if the name passed in the command is valid
 *
 *  Arguments:
 *
 *  -name = a pointer to a string containing the name
 *
 *  Return: 0 on success; 1 on failure
 ****************************************************************************/
 int check_name(char *name);

/**************************************************************************
 * check_net()
 * 
 * Description:
 * 
 *  -Check if the network passed in the command is valid;
 * 
 *  Arguments:
 * 
 *  -name = a pointer to a string containing the network;
 *
 *  Return: 0 on success; 1 on failure
 ****************************************************************************/
 int check_net(char *net);

 /**************************************************************************
 * check_ports()
 * 
 * Description:
 * 
 *  -Check if the TCP port passed in the command is valid;
 * 
 *  Arguments:
 * 
 *  -name = a pointer to a string containing the port;
 *
 *  Return: 0 on success; 1 on failure
 ****************************************************************************/
 int check_ports(char *port_str);
                        
/******************************************************************
 * personal_init()
 *
 *  Description:
 *
 *  -Allocate the memory for the personal node and set it to zero;
 *
 *  Arguments:
 *
 *  -personal = A pointer to the personal node set to NULL;
 *
 *  Return: A pointer to the initialized personal node;
 *****************************************************************/
 struct personal_node *personal_init(struct personal_node *personal);

/*****************************************************************
 * contact_init()
 *
 *  Description:
 *
 *  -Allocate memory to store the contact of the node
 *
 *  Arguments:
 *
 *  -contact = A pointer to an entry in the array of neighbrs that is set to NULL;
 *
 *  Return: A pointer to the initialized entry;
 ****************************************************************/
 nodeinfo_t *contact_init(nodeinfo_t *contact);

/*****************************************************************
 * reset_personal()
 *
 *  Description:
 *
 *  -Reset the variables in the personal node to;
 *
 *  Arguments:
 *
 *  -personal = A pointer to the personal node;
 *
 *  Return: A pointer to the personal node with reset variables
 ****************************************************************/
 struct personal_node *reset_personal(struct personal_node *personal);

/*****************************************************************
 * free_contact()
 *
 *  Description:
 *
 *  -Free the memory that contains the contact of the node
 *
 *  Arguments:
 *
 *  -contact = A pointer to a nodes's contact;
 *
 *  Return: void;
 ****************************************************************/
 void free_contact(nodeinfo_t *contact);

/*****************************************************************
 * contact_copy()
 *
 *  Description:
 *
 *  -Copy contents from contact node src to dest.
 *  - IMPORTANT: Nodes must be initialized with contact_init().
 *  -            Before calling this function.
 *
 *  Arguments:
 *
 *  -dest = A pointer to source node;
 *  -src  = A pointer to destination node;
 *
 *  Return: pointer to dest node;
 ****************************************************************/
 void contact_copy(nodeinfo_t *dest, nodeinfo_t *src);

/*------------------------------------User inteface funcions----------------------------------------*/

/****************************************************************
 * select_cmd()
 *
 *  Description:
 *
 *  -The function executes a command read from a buffer;
 *
 *  Argument:
 *
 *  -personal = A pointer to the personal node;
 *  -input = A string with the desired input;
 *
 *  Return: void;
 ***************************************************************/
 void select_cmd(struct personal_node *personal, char *input);

/***************************************************************
 * join()
 *
 * Description:
 *
 *  -Entry of a node with "id" as identifier in the network;
 *  -If "id" is already in use by another node, an unique id will be assigned;
 *  -Rgistration of the node in the server
 *  -The node will link to a node of his choosing;
 *
 * Arguments:
 *
 *  -personal = a pointer to the personal node
 *  -net = network identifier;
 *  -id = node identifier;
 *
 * Return: 0 on success, 1 on failure;
 **************************************************************/
 int join(struct personal_node *personal, char *net, char *id);

/***************************************************************
 * djoin()
 *
 * Description:
 *
 *  -Entry of the personal node with "id" as the index in the network "net";
 *  -Entry of a node with "id" as identifier in the network;
 *  -The node will be connected to another node passed in the argument;
 * (with the boot parameters);
 *  -The node server will be bypassed;
 *  -If "bootid" is equal to "id" then a network with only one node will be created;
 *
 * Arguments:
 *
 *  -personal = the structure of variables of the personal node
 *  -net = network identifier;
 *  -id = node;
 *  -bootid = second node index;
 *  -bootIP = second node IP address;
 *  -bootTCP = second node TCP port;
 *
 * Return: 0 on success; 1 on failure;
 **************************************************************/
 int djoin(struct personal_node *personal, char *net, char *id, char *bootid, char *bootIP, char *bootTCP);

/***************************************************************
 * create()
 *
 *  Description:
 *
 *  -Insert "name" in the linked list of contents;
 *
 *  Arguments:
 *
 *  -contents = the head of the linked list of contents;
 *  -name = name of content;
 *
 * Return: A pointer to the updated list of contents;
 **************************************************************/
contentList_t *create(contentList_t *contents, char *name);

/***************************************************************
 * delete()
 *
 *  Description:
 *
 *  -Remove "name" from the linked list of contents;
 *
 *  Arguments:
 *
 *  -contents = a pointer to the linked list of contents;
 *  -name = name of content;
 *
 *  Return: A pointer to the updated list of contents;
 **************************************************************/
 contentList_t *delete(contentList_t *contents, char *name); 

/***************************************************************
 * get()
 *
 *  Description:
 *
 *  -Searches for content of name "name" located in node "dest_id".
 *
 *  Arguments:
 *
 *  -personal = a pointer to the personal node;
 *  -dest_id = the index of the node where "name" is being searched;
 *  -name = name of content;
 *
 *  Return: 0 on success; 1 on failure;
 **************************************************************/
 int get(struct personal_node *personal, char *dest_id, char *name);

/***************************************************************
 * show_topology()
 *
 *  Description:
 *
 *  -Show indexes, tcp ports and addresses of all neighbours
 *
 *  Arguments:
 *
 *  -personal = a pointer to the personal node;
 *
 *  Return: 0 on success; 1 on failure;
 **************************************************************/
 int show_topology(struct personal_node *personal);

/***************************************************************
 * show_names()
 *
 *  Description:
 *
 *  -Show the names of all contents located in the personal node's list;
 *
 *  Arguments:
 *
 *  -personal = a pointer to the list of contents;
 *
 *  Return: 0 on success; 1 on failure;
 **************************************************************/
 int show_names(contentList_t **contents);

/***************************************************************
 * show_routing()
 *
 *  Description:
 *
 *  -Shows the personal node's expedition table;
 *
 *  Arguments:
 *
 *  -personal = a pointer to the expedition table;
 *
 *  Return: 0 on success; 1 on failure;
 **************************************************************/
 int show_routing(int *routing_tab);

/**************************************************************
 * clear_routing()
 *
 *  Description:
 *
 *  -Clear the routing table of the personal node
 *
 *  Arguments:
 *
 -A pointer to the personal node
 *
 *  Return: 0 on success; 1 on failure;
 **************************************************************/
 int clear_routing(int *routing_tab);

/***************************************************************
 * clear_names()
 *
 *  Description:
 *
 *  -Clear all the names in the contents table
 *
 *  Arguments:
 *
 *  -The array of contents
 *
 *  Return: A pointer to the updated list of contents
 **************************************************************/
 contentList_t *clear_names(contentList_t *contents);

/***************************************************************
 * leave()
 *
 *  Description:
 *
 *  -Remove the node from the current network;
 *
 *  Arguments:
 *
 *  -personal = a pointer to the personal node;
 *
 * Return: 0 on success; 1 on failure;
 **************************************************************/
 int leave(struct personal_node *personal);

/***************************************************************
 * help_menu()
 *
 *  Description:
 *
 *  -prints help menu, commands explanation;
 * 
 *  Arguments: void
 * 
 * Return: void
 **************************************************************/
 void help_menu(void);

/*--------------End user interface functions--------------*/

#endif
