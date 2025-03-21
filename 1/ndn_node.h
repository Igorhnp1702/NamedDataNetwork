/************************************************************************************************
 * ndn_node.h
 *
 * Authors:
 *
 * -Igor Paruque ist1102901
 * -Mónica Ferreira ist106520
 *
 * Description: header file for node management
 ***********************************************************************************************/

#ifndef ndn_node_header
#define ndn_node_header


#include "ndn_interestTable.h"
#include "ndn_objectStructs.h"

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
 *  -node_buff = buffer to store unfinished mesages
 ***************************************************************************/
typedef struct any_node{
char *tcp_port;  // tcp port of the node; integer from 0 to 65 536   
char *node_addr; // IPv4 address with undefined size
char *node_buff; // buffer to store unfinished mesages
int  node_fd;   // the file descriptor assigned to this node
int  safe_flag;
int  entry_flag;
int  msg_object_flag;

} nodeinfo_t;


typedef struct nodesLinkedlist_t{
    nodeinfo_t *node;
    struct nodesLinkedlist_t *next;
} nodesLinkedlist_t;


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
 *  -n_internals = counter for the number of internal neighbors;
 *  -udp_fd      = file descriptor to communicate with the node server
 *  -max_fd      = the maximum integer assigned to a file descriptor in this node's FD set
 *  -client_fd   = file descriptor to communicate with the extern node
 *  -udp_port    = UDP port of the server of nodes
 *  -udp_address = UDP address of the server of nodes
 *  -rdy_scks    = set of file descriptors to read from
 *  -crr_scks    = set of file descriptors to read from
 *  -queue_ptr   = ptr for the object queue
 *  -extern_node = contact of the extern neighbor node
 *  -backup_node = contact of the backup neighbor node
 *  -neighbrs    = array of contacts of intern neighbors. 
 *                 The indexes match the nodes's ids
 ****************************************************************************/
struct personal_node{

int anchorflag;                     // flag that says whether the node is an anchor or not (anchor = backup to itself)
int network_flag;
int join_flag;
int n_internals;                    // counter for the number of internal neighbors
int max_fd;                         // the maximum integer assigned to a file descriptor in this node's FD set
int cache_limit;
int server_fd;                      // to accept connections with other nodes
char *personal_addr;
char *personal_tcp;
char *personal_net;
char *backup_addr;
char *backup_tcp;
char *udp_port;                     // UDP port of the server of nodes
char *udp_address;                  // UDP address of the server of nodes
fd_set rdy_scks;                    // set of file descriptors with activity to handle (rdy = ready)
fd_set crr_scks;                    // set of file descriptors in use (crr = current)
storageList_t * storage_ptr;
objectQueue_t *queue_ptr;           // double linked list of the contents of the node
InterestTable *interest_table;      // table of interests
nodeinfo_t *extern_node;            // contact of the extern neighbor node
nodesLinkedlist_t *internals_list; // list of contacts of internal neighbors
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
 *  -Allocate memory to store the contact of the node IN THE NODE'S MEMBERS
 *  -Please call the calloc function to allocate memory to the node itself
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
void free_contact(nodeinfo_t **contact);

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

void reset_contact(nodeinfo_t **contact);

nodesLinkedlist_t *insertnode(nodesLinkedlist_t *head, nodeinfo_t *new_node);

nodesLinkedlist_t *Listinit(nodesLinkedlist_t *head); //-Please call the calloc function to allocate memory to the node itself

nodesLinkedlist_t *removenode(nodesLinkedlist_t *head, int old_fd);

nodesLinkedlist_t *clearlist(nodesLinkedlist_t *head);

int has_neighbors(nodesLinkedlist_t *internals_list);

#endif