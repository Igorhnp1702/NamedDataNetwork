/****************************************************************************
 * ndn_utils.c
 *
 * Authors:
 *
 * -Igor Paruque ist1102901
 * -Mónica Ferreira ist106520
 *
 * Description: header file for the utility functions
 ***************************************************************************/

 #ifndef ndn_utils_header
 #define ndn_utils_header

/**************************************************************************
 * typedef struct queueBlock_t
 * 
 * Description:
 * 
 *  -Structure of variables to define a block of the object queue
 * 
 * Members of the struct
 * 
 * -name = name of the object
 * -next = pointer to the next object in the queue
 * -prev = pointer to the previous object in the queue
 * 
 **************************************************************************/
typedef struct queueBlock_t{
    char *name;
    struct queueBlock_t *next;
    struct queueBlock_t *prev;
}queueBlock_t;

 /**************************************************************************
 * typedef struct objectQueue_t
 * 
 * Description: 
 * 
 *  -Structure of variables to build a double linked lists of strings
 * 
 * Members of the struct
 * 
 *  -queue_size = number of objects in the queue; 
 *  -head = pointer to the first object;
 *  -tail = pointer to the last object;
 * 
 **************************************************************************/
typedef struct objectQueue_t{
    int queue_size;            
    queueBlock_t *head; 
    queueBlock_t *tail; 
}objectQueue_t;
  
// Insert function signature for queue management (init, delete, insert, search, clear)

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
objectQueue_t *create(objectQueue_t *queue_ptr, char *name);

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
 objectQueue_t *delete(objectQueue_t *queue_ptr, char *name); 

/***************************************************************
 * retrieve()
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
int retrieve(struct personal_node *personal, char *name);

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
int show_names(objectQueue_t **queue_ptr);

/***************************************************************
 * show_interest_table()
 *
 *  Description:
 *
 *  -Shows all the entries in the personal node's interest table;
 *
 *  Arguments:
 *
 *  -personal = a pointer to the interest table;
 *
 *  Return: 0 on success; 1 on failure;
 **************************************************************/
int show_interest_table(struct personal_node *personal);

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
objectQueue_t *clear_names(objectQueue_t *queue_ptr);

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

 #endif