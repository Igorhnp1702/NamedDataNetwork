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
//#include "g23tcp_udp.h"
//#include "g23funcs.h"



int main(int argc, char **argv){

    printf("\n");
    /* check the arguments */

    if (argc < 3 || argc > 6)
    {

        printf("Please provide 2 to 4 of the following arguments in the correct order\n\n");
        printf("Usage: cot [personal IPv4 address] [personal TCP port] [server's IPv4 address] [server's UDP port]\n");
        exit(1);
    } // if

    char *personal_addr; // Personal IPv4 addres
    char *personal_port; // Personal TCP port
    char *server_addr;   // The server's IPv4 address
    char *server_port;   // The server's UDP port

    if (argc == 3)
    {

        personal_addr = argv[1];

        personal_port = argv[2];

        printf("The node server's IPv4 address will default to %s\n", DEFAULT_REGIP);
        server_addr = DEFAULT_REGIP;

        printf("The node server's UDP port will default to %s\n", DEFAULT_REGUDP);
        server_port = DEFAULT_REGUDP;
    } // if

    if (argc == 4)
    {

        personal_addr = argv[1];

        personal_port = argv[2];

        server_addr = argv[3];

        printf("The node server's UDP port will default to %s\n", DEFAULT_REGUDP);
        server_port = DEFAULT_REGUDP;
    } // if

    if (argc == 5)
    {

        personal_addr = argv[1];

        personal_port = argv[2];

        server_addr = argv[3];

        server_port = argv[4];
    } // if

    printf("The application was launched successfuly\n\n");
    printf("\nType 'help' to show this help menu.\n\n");
    help_menu();    
    printf("\nIf the program stopped, it's either:\n\n");
    printf("-waiting for a command, or;\n");
    printf("-it froze;\n\n");
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

   
    