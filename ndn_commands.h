/************************************************************************************************
 * ndn_commands.h
 *
 * Authors:
 *
 * -Igor Paruque ist1102901
 * -Mónica Ferreira ist106520
 *
 * Description: header file for the commands
 ***********************************************************************************************/

#ifndef ndn_commands_header
#define ndn_commands_header

#define DEFAULT_REGIP "193.136.138.142"  // default IPv4 address of the node server
#define DEFAULT_REGUDP "59000"           // default UDP port of the node server
#define MAX_QUEUE_LENGTH_TCP 6           // maximium size of the tcp queue
#define MAX_NODESLIST 4000               // size of the buffer for the NODESLIST response
#define MAX_MSG_LENGTH 200               // size of the buffer for the messages (incoming/outgoing)
#define MAX_USR_CMD_LENGTH 150           // size of the buffer for the commands
#define MAX_CONTENT_NAME 101             // size of the buffer for the contents (max chars = 100 + null terminator)
#define MAX_ADDRESS_SIZE 50              // size of the buffer for non_determinístic ip address
#define MAX_NET_CHARS 4                  // number of chars required for the network number (with '\0')
#define MAX_TCP_UDP_CHARS 6              // number of chars required for the tcp/udp ports(with '\0')
#define MAX_MSG_CMD_SIZE 10              // size of the biggest message protocol (NOCONTENT = 9 bytes + null terminator)
 
// set of strings for the mandatory commands

#define join_str "join"                     // user cmd join
#define join_str_short "j"                  // short for join
#define direct_str "direct"          // user cmd djoin
#define direct_join_str_short "dj"          // short for direct join
#define create_str  "create"                // user cmd create
#define create_str_short "c"                // short for
#define leave_str "leave"                   // user cmd leave
#define leave_str_short "l"                 // short for
#define delete_str  "delete"                // user cmd delete
#define delete_str_short "dl"               // short for
#define retrieve_str  "retrieve"            // user cmd get
#define retrieve_str_short "r"              // short for
#define show_str "show"                     // partial cmd string
#define topology_str "topology"             // partial cmd string
#define show_topology_str_short "st"        // short for show topology
#define names_str "names"                   // partial cmd string
#define show_names_str_short "sn"                 // short for
#define interest_str "interest"             // partial cmd string
#define table_str "table"                   // partial cmd string
#define show_interest_table_str_short "si"  //short for show interest table
#define exit_str "exit"                     // user cmd exit
#define exit_str_short "x"                  //short for exit

// set of strings for the optional commands

#define help_str "help"                     // user cmd help
#define help_str_short "h"                  // short for help
#define clear_str "clear"                   // partial cmd string
#define clear_names_str_short "cn"          // short for clear names

int server_on = 0;

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
int join(struct personal_node *personal, char *net);

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
 int djoin(struct personal_node *personal, char *net, char *connectIP, char *connectTCP);

// /***************************************************************
//  * create()
//  *
//  *  Description:
//  *
//  *  -Insert "name" in the linked list of contents;
//  *
//  *  Arguments:
//  *
//  *  -queue_head = the head of the linked list of contents;
//  *  -name = name of content;
//  *
//  * Return: A pointer to the updated list of contents;
//  **************************************************************/
// objectQueue_t *create(objectQueue_t *queue_ptr, char *name);

// /***************************************************************
//  * delete()
//  *
//  *  Description:
//  *
//  *  -Remove "name" from the linked list of contents;
//  *
//  *  Arguments:
//  *
//  *  -contents = a pointer to the linked list of contents;
//  *  -name = name of content;
//  *
//  *  Return: A pointer to the updated list of contents;
//  **************************************************************/
//  objectQueue_t *delete(objectQueue_t *queue_ptr, char *name); 

// /***************************************************************
//  * retrieve()
//  *
//  *  Description:
//  *
//  *  -Searches for content of name "name" located in node "dest_id".
//  *
//  *  Arguments:
//  *
//  *  -personal = a pointer to the personal node;
//  *  -dest_id = the index of the node where "name" is being searched;
//  *  -name = name of content;
//  *
//  *  Return: 0 on success; 1 on failure;
//  **************************************************************/
// int retrieve(struct personal_node *personal, char *name);

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

// /***************************************************************
//  * show_names()
//  *
//  *  Description:
//  *
//  *  -Show the names of all contents located in the personal node's list;
//  *
//  *  Arguments:
//  *
//  *  -personal = a pointer to the list of contents;
//  *
//  *  Return: 0 on success; 1 on failure;
//  **************************************************************/
// int show_names(objectQueue_t **queue_ptr);

// /***************************************************************
//  * show_interest_table()
//  *
//  *  Description:
//  *
//  *  -Shows all the entries in the personal node's interest table;
//  *
//  *  Arguments:
//  *
//  *  -personal = a pointer to the interest table;
//  *
//  *  Return: 0 on success; 1 on failure;
//  **************************************************************/
// int show_interest_table(struct personal_node *personal);

// /***************************************************************
//  * clear_names()
//  *
//  *  Description:
//  *
//  *  -Clear all the names in the contents table
//  *
//  *  Arguments:
//  *
//  *  -The array of contents
//  *
//  *  Return: A pointer to the updated list of contents
//  **************************************************************/
// objectQueue_t *clear_names(objectQueue_t *queue_ptr);

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
                        
int is_valid_ip(const char *ip);

int check_num_string(char *num_string);

#endif

 