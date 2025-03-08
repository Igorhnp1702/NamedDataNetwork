/************************************************************************************************
 * ndn_netfuncs.c
 *
 * Authors:
 *
 * -Igor Paruque ist1102901
 * -Mónica Ferreira ist106520
 *
 * Description: header file for the network functionalities
 ***********************************************************************************************/

#ifndef ndn_netfuncs_header
#define ndn_netfuncs_header

#define MAX_INTERNALS 99


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
 *  -n_internals  = counter for the number of internal neighbors;
 *  -udp_fd      = file descriptor to communicate with the node server
 *  -server_fd   = file descriptor to communicate with the intern nodes
 *  -max_fd      = the maximum integer assigned to a file descriptor in this node's FD set
 *  -client_fd   = file descriptor to communicate with the extern node
 *  -internal_fds = array that relates internal neighbrs id's with their fd. The indexes match the nodes's ids
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
   nodeinfo_t *persn_info;          // contact of the personal node
   int anchorflag;                  // flag that says whether the node is an anchor or not (anchor = backup to itself)
   int n_internals;                 // counter for the number of internal neighbors
   int udp_fd;                      // file descriptor to communicate with the server of nodes//!não esta a ser utilizado
   int server_fd;                   // file descriptor to communicate with the intern nodes
   int max_fd;                      // the maximum integer assigned to a file descriptor in this node's FD set
   int client_fd;                   // file descriptor to communicate with the extern node
   char *udp_port;                  // UDP port of the server of nodes
   char *udp_address;               // UDP address of the server of nodes
   fd_set rdy_scks;                 // set of file descriptors with activity to handle (rdy = ready)
   fd_set crr_scks;                 // set of file descriptors in use (crr = current)
   queryList_t *qryhistory;         // history of queries sent (qry = querry)
   contentList_t *contents;         // linked list of the contents of the node
   nodeinfo_t *extern_node;         // contact of the extern neighbor node
   nodeinfo_t *backup_node;         // contact of the backup neighbor node
   nodeinfo_t **neighbrs;           // array of contacts of intern neighbors. 
};

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

 #endif