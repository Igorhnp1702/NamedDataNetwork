/************************************************************************************************
 * ndn_main.c
 *
 * Authors:
 *
 * -Igor Paruque ist1102901
 * -Mónica Ferreira ist106520
 *
 * Description: source code for the main function
 ***********************************************************************************************/

// general purpose libraries
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

// project libraries
#include "ndn_netfuncs.h"
#include "ndn_io.h"
#include "ndn_utils.h"



int main(int argc, char **argv){

    
    printf("\n");
    /* check the arguments */

    if (argc < 4 || argc > 6)
    {

        printf("Error: Wrong number of arguments. Please provide 4 to 6 of the following arguments\n\n");
        printf("Usage: ndn [personal cache size] [personal IPv4 address] [personal TCP port] [node server's IPv4 address] [server's UDP port]\n");
        printf("\nor\n");
        printf("Usage: ndn [personal cache size] [personal IPv4 address] [personal TCP port] [node server's IPv4 address]\n");
        printf("\nor\n");
        printf("Usage: ndn [personal cache size] [personal IPv4 address] [personal TCP port] [server's UDP port]\n");
        printf("\nor\n");
        printf("Usage: ndn [personal cache size] [personal IPv4 address] [personal TCP port]\n");
            
        exit(1);
    } // if
    
    char *personal_addr;        // Personal IPv4 addres
    char *personal_port;        // Personal TCP port
    char *server_addr;          // The server's IPv4 address
    char *server_port;          // The server's UDP port
    int cache_size;             // integer variable to store the cache size

    errno = 0;                      // reset error indicator
    char *num_string_check;  // for the strtol function
        
    if (argc == 4)
    {

        cache_size = strtol(argv[1], num_string_check, 10);

        if(cache_size < 0 || errno == ERANGE){
            printf("Error in cache size: number out of range\n");
            printf("Pick a number between 0 and 4294967295\n");
            printf("Process terminated\n");
            exit(1);
        }
        else if(*num_string_check != "\0"){
            printf("Error in cache size: non-numeric character detected\n");
            printf("Process terminated\n");
        }

        if(!is_valid_ip(argv[2])){
            printf("Error in personal IPv4 address: the address is invalid\n");
            printf("Process terminated\n");
            exit(1);
        }

        personal_addr = argv[2];
        
        if(strtol(argv[3], num_string_check, 10) < 0 || errno == ERANGE){
            printf("Error in personal port: number out of range\n");
            printf("Pick a number between 0 and 4294967295 (65565 might be the MAXPORT)\n");
            printf("Process terminated\n");
            exit(1);
        }
        else if(*num_string_check != "\0"){
            printf("Error in personal port: non-numeric character detected\n");
            printf("Process terminated\n");
            exit(1);
        }

        personal_port = argv[3];

        printf("The node server's IPv4 address will default to %s\n", DEFAULT_REGIP);
        server_addr = DEFAULT_REGIP;

        printf("The node server's UDP port will default to %s\n", DEFAULT_REGUDP);
        server_port = DEFAULT_REGUDP;

    } // if 3 arguments

    if (argc == 5)
    {
        cache_size = strtol(argv[1], num__string_check, 10);

        if(cache_size < 0 || errno == ERANGE){
            printf("Error in cache size: number out of range\n");
            printf("Pick a number between 0 and 4294967295\n");
            printf("Process terminated\n");
            exit(1);
        }
        else if(*num_string_check != "\0"){
            printf("Error in cache size: non-numeric character detected\n");
            printf("Process terminated\n");
        }

        if(!is_valid_ip(argv[2])){
            printf("Error in personal IPv4 address: the address is invalid\n");
            printf("Process terminated\n");
            exit(1);
        }
        
        personal_addr = argv[2];

        if(strtol(argv[3], num_string_check, 10) < 0 || errno == ERANGE){
            printf("Error in personal port: number out of range\n");
            printf("Pick a number between 0 and 4294967295 (65565 might be the MAXPORT)\n");
            printf("Process terminated\n");
            exit(1);
        }
        else if(*num_string_check != "\0"){
            printf("Error in personal port: non-numeric character detected\n");
            printf("Process terminated\n");
            exit(1);
        }

        personal_port = argv[3];

        // test format to see if it's an ip address or not

        if(is_valid_ip(argv[4])){

            server_addr = argv[4];

            printf("The node server's UDP port will default to %s\n", DEFAULT_REGUDP);
            server_port = DEFAULT_REGUDP;

        }else{ // if test fails
            
            if(strtol(argv[4], num_string_check, 10) < 0 || errno == ERANGE){
                printf("Error in node server's port: number out of range\n");
                printf("Pick a number between 0 and 4294967295 (65565 might be the MAXPORT)\n");
                printf("Process terminated\n");
                exit(1);
            }
            else if(*num_string_check != "\0"){
                printf("Error in node server's port: non-numeric character detected\n");
                printf("Process terminated\n");
                exit(1);
            }
            
            server_port = argv[4];

            printf("The node server's IPv4 address will default to %s\n", DEFAULT_REGIP);
            server_addr = DEFAULT_REGIP;

        }            
        
    } // if 4 arguments

    if (argc == 6)
    {

        cache_size = strtol(argv[1], num_string_check, 10);

        if(cache_size < 0 || errno == ERANGE){
            printf("Error in cache size: number out of range\n");
            printf("Pick a number between 0 and 4294967295\n");
            printf("Process terminated\n");
            exit(1);
        }
        else if(*num_string_check != "\0"){
            printf("Error in cache size: non-numeric character detected\n");
            printf("Process terminated\n");
        }

        if(!is_valid_ip(argv[2])){
            printf("Error in personal IPv4 address: the address is invalid\n");
            printf("Process terminated\n");
            exit(1);
        }

        personal_addr = argv[2];

        if(strtol(argv[3], num_string_check, 10) < 0 || errno == ERANGE){
            printf("Error in personal port: number out of range\n");
            printf("Pick a number between 0 and 4294967295 (65565 might be the MAXPORT)\n");
            printf("Process terminated\n");
            exit(1);
        }
        else if(*num_string_check != "\0"){
            printf("Error in personal port: non-numeric character detected\n");
            printf("Process terminated\n");
            exit(1);
        }
        personal_port = argv[3];

        if(!is_valid_ip(argv[2])){
            printf("Error in node server's IPv4 address: the address is invalid\n");
            printf("Process terminated\n");
            exit(1);
        }

        server_addr = argv[4];

        if(strtol(argv[4], num_string_check, 10) < 0 || errno == ERANGE){
            printf("Error in node server's port: number out of range\n");
            printf("Pick a number between 0 and 4294967295 (65565 might be the MAXPORT)\n");
            printf("Process terminated\n");
            exit(1);
        }
        else if(*num_string_check != "\0"){
            printf("Error in node server's port: non-numeric character detected\n");
            printf("Process terminated\n");
            exit(1);
        }
        server_port = argv[5];

    } // if 5 arguments

    
    printf("The application was launched successfuly\n\n");
    printf("\nType 'help' to show this help menu.\n\n");
    help_menu();    
    printf("\nIf the program stopped, it's waiting for activity:\n");    
    printf("\n_________________________________________________________________\n\n");    
    
    
    /* Prepare the node */

    struct personal_node *my_node = NULL;

    my_node = personal_init(my_node);

    strcpy(my_node->persn_info->node_addr, personal_addr);
    strcpy(my_node->persn_info->tcp_port, personal_port);

    my_node->udp_address = server_addr;
    my_node->udp_port = server_port;

    if(tcp_server(my_node) == 1){ //caso haja algum erro no início desta função, sai suavemente 
        leave(my_node);
        free(my_node->neighbrs);
        free(my_node->route_tab);
        free_contact(my_node->persn_info);

        if(my_node->contents != NULL){
        
            contentList_t *listptr; // pointer to go through the list
            contentList_t *aux;     // auxiliary pointer to delete elements in the lists
            listptr = my_node->contents;
            
            while(listptr != NULL){

                aux = listptr;
                listptr = listptr->next;
                free(aux->string);
                free(aux);
                
            }                            
        }

        free(my_node);
        exit(1);
    } 
}//main()

   
    