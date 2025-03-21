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
#include "ndn_objectStructs.h"

objectQueue_t *queueInit(objectQueue_t *queue_ptr, int limit){

    objectQueue_t *new_queue = NULL;

    if((new_queue = (objectQueue_t*)calloc(1, sizeof(objectQueue_t))) == NULL){

        printf("Error in queueInit: Failed to allocate memory. Process terminated\n");
        exit(1);
    }

    new_queue->queue_limit = limit;
    new_queue->object_counter = 0;
    new_queue->head = NULL;
    new_queue->tail = NULL;
    queue_ptr = new_queue;
    return queue_ptr;
}

objectQueue_t *insertNew(objectQueue_t *queue_ptr, char *string){

    // compare counter with limit before calling this function
    char *copy_string = NULL;
    queueBlock_t *new_block = NULL;

    if((new_block = (queueBlock_t*)calloc(1, sizeof(queueBlock_t))) == NULL){
        
        printf("Error in insertNew: Failed to allocate memory. Process terminated\n");
        exit(1);
    }
    new_block->next = NULL;
    new_block->prev = NULL;

    if((copy_string = (char*)calloc(MAX_NAME_LENGTH, sizeof(char))) == NULL){

        printf("Error in insertNew: Failed to allocate memory. Process terminated\n");
        exit(1);
    }
    strcpy(copy_string, string);
    new_block->name = copy_string;

    if((queue_ptr->head == NULL) && (queue_ptr->tail == NULL)){ // empty list
        queue_ptr->head = new_block;
        queue_ptr->tail = new_block;
        queue_ptr->object_counter++;
        return queue_ptr;
    }

    new_block->next = queue_ptr->head;      // new block points to former first
    queue_ptr->head->prev = new_block;      // former first previous pointer points to new block
    queue_ptr->head = new_block;            // list list pointer will point to new block
    queue_ptr->object_counter++;            // increment the counter

    return queue_ptr;
}

objectQueue_t *deleteOld(objectQueue_t *queue_ptr){

    if((queue_ptr->tail == NULL) && (queue_ptr->head == NULL)) return queue_ptr;
        
    queueBlock_t *aux = queue_ptr->tail;

    queue_ptr->tail = queue_ptr->tail->prev;
    queue_ptr->tail->next = NULL;
    free(aux->name);
    free(aux);
    queue_ptr->object_counter--;
    return queue_ptr;            
}

int queueSearch(objectQueue_t *queue_ptr, char *string){

    if((queue_ptr->tail == NULL) && (queue_ptr->head == NULL)){
        printf("Failed to find %s in cache because it's empty\n", string);
        return 0;
    }

    queueBlock_t *aux = queue_ptr->head;

    while(aux != NULL){

        if((strcmp(aux->name, string)) == 0){

            printf("%s was found in cache\n", string);
            return 1;
        }
        aux = aux->next;
    }
    printf("Failed to find %s in cache\n", string);
    return 0;
}

objectQueue_t *deleteObject(objectQueue_t *queue_ptr, char *string){

    if((queue_ptr->tail == NULL) && (queue_ptr->head == NULL)){
        printf("Failed to delete %s because cache is empty\n", string);
        return queue_ptr;
    }

    queueBlock_t *aux1 = queue_ptr->head, *aux2, *aux3;

    // beginning of the list

    if((strcmp(string, queue_ptr->head->name)) == 0){

        aux1 = queue_ptr->head;
        queue_ptr->head = queue_ptr->head->next;
        queue_ptr->head->prev = NULL;
        free(aux1->name);
        free(aux1);
        queue_ptr->object_counter--;
        printf("%s was deleted from the cache\n", string);
        return queue_ptr;
    }
    
    // end of the list

    if((strcmp(string, queue_ptr->tail->name)) == 0){
        
        aux1 = queue_ptr->tail;
        queue_ptr->tail = queue_ptr->tail->prev;
        queue_ptr->tail->next = NULL;
        free(aux1->name);
        free(aux1);
        queue_ptr->object_counter--;
        printf("%s was deleted from the cache\n", string);
        return queue_ptr;
    }

    // middle of the list

    while(aux1 != NULL){

        if((strcmp(aux1->name, string)) == 0){
                        
            aux2 = aux1->prev;
            aux3 = aux1->next;
            aux2->next = aux3;
            aux3->prev = aux2;
            free(aux1->name);
            free(aux1);
            queue_ptr->object_counter--;
            printf("%s was deleted from the cache\n", string);
            return queue_ptr;
        }
        aux1 = aux1->next;
    }
    printf("Failed to delete %s\n", string);
    return queue_ptr;
}

objectQueue_t *clearQueue(objectQueue_t *queue_ptr){

    if((queue_ptr->tail == NULL) && (queue_ptr->head == NULL)){
        printf("Failed to clear cache because it's empty\n");
        return queue_ptr;
    }

    queueBlock_t *aux1 = queue_ptr->head, *aux2;

    while (aux1 != NULL){
        
        aux2 = aux1;
        aux1 = aux1->next;
        free(aux2->name);
        free(aux2);
        queue_ptr->object_counter--;
    }
    free(queue_ptr);
    queue_ptr = NULL;

    printf("The cache of objects is now empty\n");
    return queue_ptr;        
}

storageList_t *storageInit(storageList_t *head){
    head = NULL; 
    return head;
}

storageList_t *storageInsert(storageList_t *head, char *object){

    storageList_t *new_node = NULL, *aux;
    char *copy_object = NULL;

    if((new_node = (storageList_t*)calloc(1, sizeof(storageList_t))) == NULL){

        printf("Error in storageInsert: Failed to allocate memory. Process terminated\n");
        exit(1);
    }
    new_node->next = NULL;

    if((copy_object = (char*)calloc(MAX_NAME_LENGTH, sizeof(char))) == NULL){

        printf("Error in storageInsert: Failed to allocate memory. Process terminated\n");
        exit(1);
    }
    strcpy(copy_object, object);
    new_node->object = copy_object;
    
    if(head == NULL){

        head = new_node;
        return head;
    }

    aux = head;

    while(aux->next != NULL) aux = aux->next;

    aux->next = new_node;

    return head;
    
}

storageList_t *storageDelete(storageList_t *head, char *object){

    if(head == NULL){
        printf("Object storage is empty\n");
        return head;
    }

    storageList_t *aux = head, *aux_del;

    if((strcmp(head->object, object)) == 0){

        aux_del = head;
        head = head->next;
        free(aux_del->object);
        free(aux);
        return head;
    }

    while(aux->next != NULL){

        if((strcmp(aux->next->object, object)) == 0){

            aux_del = aux->next;
            aux->next = aux->next->next;
            free(aux_del->object);
            free(aux);
            return head;
        }
    }
    printf("%s was not found in object storage\n", object);
    return  head;
}

int storageSearch(storageList_t *head, char *object){

    if(head == NULL){
        printf("Object storage is empty\n");
        return 0;
    }
    storageList_t *aux = head;

    while(aux != NULL){

        if((strcmp(aux->object, object)) == 0){
            printf("%s was found in the object storage\n", object);
            return 1;
        }
        aux = aux->next;
    }
    printf("Failed to find %s in the object storage\n", object);
    return 0;

}

storageList_t *storageClear(storageList_t *head){

    if(head == NULL){
        printf("Object storage is empty\n");
        return head;
    }

    storageList_t *aux = head, *aux_del;

    while(aux != NULL){

        aux_del = aux;
        aux = aux->next;
        free(aux_del->object);
        free(aux_del);
    }
    head = NULL;
    return head;
}

