/************************************************************************************************
 * ndn_queue.c
 *
 * Authors:
 *
 * -Igor Paruque ist1102901
 * -Mónica Ferreira ist106520
 *
 * Description: source code for the object queue
 ***********************************************************************************************/

#define _XOPEN_SOURCE 600 //!REMOVE BEFORE COMPILING

 // general purpose libraries
#include <stdlib.h>
#include <string.h>


// networking libraries
#include <netdb.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <signal.h>

// project libraries
#include "ndn_commands.h"
#include "ndn_messages.h"
#include "ndn_node.h"
#include "ndn_queue.h"

objectQueue_t *queueInit(objectQueue_t *queue_ptr, int size){

}

objectQueue_t *insertNew(objectQueue_t *queue_ptr, char *string){

}

objectQueue_t *deleteOld(objectQueue_t *queue_ptr, char *string){

}

int queueSearch(objectQueue_t *queue_ptr, char *string){

}

void clearQueue(objectQueue_t *queue_ptr){

}

