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

#define _XOPEN_SOURCE 600 //!REMOVE BEFORE COMPILING

// project libraries

#include "ndn_node.h"
#include "ndn_objectStructs.h"



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
 int djoin(struct personal_node *personal, char *connectIP, char *connectTCP);

/***************************************************************
 * create()
 *
 *  Description:
 *
 *  -Insert "name" in the linked list of contents;
 *
 *  Arguments:
 *
 *  -queue_head = the head of the linked list of contents;
 *  -name = name of content;
 *
 * Return: A pointer to the updated list of contents;
 **************************************************************/
storageList_t *create(storageList_t *storage_head, char *object);


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
void show_names(storageList_t *storage_ptr);

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

int check_cache_size(char *size_str);


#endif

 