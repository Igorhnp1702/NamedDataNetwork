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
#define TIMEOUT_VALUE_s 5  //!repor!!!   // timeout value in seconds 

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

// set of strings for the messages

// interactions with the node server
#define reg_str "REG"           // UDP msg REG
#define okreg_str "OKREG"       // UDP msg OKREG
#define unreg_str "UNREG"       // UDP msg UNREG
#define okunreg_str "OKUNREG"   // UDP msg OKUNREG
#define nodes_str "NODES"       // UDP msg NODES

// interactions with other nodes for tree maintenance
#define entry_str "ENTRY"       // TCP msg ENTRY
#define safe_str "SAFE"         // TCP msg SAFE

// interactions with other nodes to exchange objects
#define query_str "INTEREST"    // TCP msg INTEREST
#define object_str "OBJECT"     // TCP msg OBJECT
#define noobject_str "NOOBJECT" // TCP msg NOOBJECT

int udp_flag = 0;

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





int is_valid_ip(const char *ip);

int check_num_string(char *num_string);

#endif

 