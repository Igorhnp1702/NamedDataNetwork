/****************************************************************************
 * ndn_queue.c
 *
 * Authors:
 *
 * -Igor Paruque ist1102901
 * -Mónica Ferreira ist106520
 *
 * Description: header file for the object queue
 ***************************************************************************/

 #ifndef ndn_queue_header
 #define ndn_queue_header

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

objectQueue_t *queueInit(objectQueue_t *queue_ptr, int size);

objectQueue_t *insertNew(objectQueue_t *queue_ptr, char *string);

objectQueue_t *deleteOld(objectQueue_t *queue_ptr, char *string);

int queueSearch(objectQueue_t *queue_ptr, char *string);

void clearQueue(objectQueue_t *queue_ptr);


 #endif