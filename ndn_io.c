/************************************************************************************************
 * ndn_io.c
 *
 * Authors:
 *
 * -Igor Paruque ist1102901
 * -Mónica Ferreira ist106520
 *
 * Description: source code for input/output functions
 ***********************************************************************************************/

#include <arpa/inet.h>
#include "ndn_io.h"
#include "ndn_netfuncs.h"


/*------------------------------------------Argument checking functions------------------------------------------*/

int is_valid_ip(const char *ip) {
    struct sockaddr_in sa;
    return inet_pton(AF_INET, ip, &(sa.sin_addr)) != 0;  // IPv4 check
}

int check_net(char *net){
    
    int success_flag = 0;
    
    if(strlen(net) != 3){
        
        printf("Invalid net\n");
        return ++success_flag;
    }

    if(0 <= atoi(net) && atoi(net) <= 999) return success_flag;
    else{
        printf("Invalid net\n");
        return ++success_flag;
    }
}//check_net()

int check_ports(char *port_str){
    
    int length = strlen(port_str), flag = 0;
    
    if(length > 5) return ++flag;
    
    if(atoi(port_str) > 65536)return ++flag;
    
    else return flag;
}//check_ports()


int check_name(char *name){

    int flag = 0; //success flag

    if(strlen(name) > 100){
        printf("Content size is too big. Content ignored\n");
        return ++flag;
    }
    else return flag;
}//check_name

/*------------------------------------------User inteface funcions------------------------------------------*/

void select_cmd(struct personal_node *personal, char *input){ 
    
    char net_num[MAX_NET_CHARS];      memset(net_num, 0, sizeof(net_num));     // network number passed in the commands    
    char tcp[MAX_TCP_UDP_CHARS];      memset(tcp, 0, sizeof(tcp));             // a tcp port passed in the commands
    char udp[MAX_TCP_UDP_CHARS];      memset(udp, 0, sizeof(udp));             // the udp port of the node server passed in the commands    

    char cmd_str1[MAX_MSG_CMD_SIZE];  memset(cmd_str1, 0, sizeof(cmd_str1));   // should be enough for the first string of the command
    char cmd_str2[MAX_MSG_CMD_SIZE];  memset(cmd_str2, 0, sizeof(cmd_str2));   // should be enough for the first string of the command
    char cmd_str3[MAX_MSG_CMD_SIZE];  memset(cmd_str3, 0, sizeof(cmd_str3));   // should be enough for the first string of the command
    char address[MAX_ADDRESS_SIZE];   memset(address, 0, sizeof(address));     // an IPv4 address passed in the commands
    char content[MAX_CONTENT_NAME];   memset(content, 0, sizeof(content));     // the name of a content passed in the commands

    // parse the first string

    sscanf(input, "%s %s %s", cmd_str1, cmd_str2, cmd_str3);

    // try to read and execute the desired command

    if(strcmp(cmd_str1, join_str) == 0 || strcmp(cmd_str1, join_str_short) == 0){

        if(strcmp(personal->persn_info->network, "") != 0 ){
            printf("The node already belongs to a network. Command ignored\n");
            return;
        }       
        else if(sscanf(input, "%*s %s", net_num) == 1){
            
            printf("Executing %s...\n\n", join_str);
            join(personal, net_num); //join a network and register the personal node in the server
            return;
        }                        
        else{
            printf("Failed to read arguments of %s\n", join_str); 
            return;
        } 
    }// if

    else if(strcmp(cmd_str1, direct_join_str_short) == 0){

        if(strcmp(personal->persn_info->network, "") != 0 ){
            printf("The node already belongs to a network. Command ignored\n");
            return;
        }
        else if(sscanf(input, "%*s %s %s %s", net_num, address, tcp) == 3){
            
            printf("Executing %s %s...\n\n", direct_str, join_str);
            djoin(personal, net_num, address, tcp); // join a network without registration
            return;
        }       
        else{
            printf("Failed to read arguments of %s %s\n", direct_str, join_str); 
            return;
        } 
    }//else if

    else if((strcmp(cmd_str1, direct_str) == 0 && strcmp(cmd_str2, join_str) == 0)){

        if(strcmp(personal->persn_info->network, "") != 0 ){
            printf("The node already belongs to a network. Command ignored\n");
            return;
        }
        else if(sscanf(input, "%*s %*s %s %s %s", net_num, address, tcp) == 3){
            
            printf("Executing %s %s...\n\n", direct_str, join_str);
            djoin(personal, net_num, address, tcp); // join a network without registration
            return;
        }       
        else{
            printf("Failed to read arguments of %s %s\n", direct_str, join_str); 
            return;
        } 
    }//else if

    else if(strcmp(cmd_str1, create_str) == 0 || strcmp(cmd_str1, create_str_short) == 0){

        if(sscanf(input, "%*s %s", content) == 1){
            
            printf("Executing %s...\n\n", create_str);
            personal->contents = create(personal->contents, content); // create and store a content inside the personal node           
            return; 
        }
        else{
            printf("Failed to read arguments of %s\n", create_str); 
            return;
        } 
    }//else if

    else if(strcmp(cmd_str1, delete_str) == 0 || strcmp(cmd_str1, delete_str_short) == 0){

        if(sscanf(input, "%*s %s", content) == 1){
            
            printf("Executing %s...\n\n", delete_str);
            personal->contents = delete(personal->contents, content); // delete a content inside the personal node
            return;            
        }
        else{
            printf("Failed to read arguments of %s\n", delete_str); 
            return;
        } 
    }//else if

    else if(strcmp(cmd_str1, retrieve_str) == 0 || strcmp(cmd_str1, retrieve_str_short) == 0){
        
        if(sscanf(input, "%*s %s", content) == 1){
            
            printf("Executing %s...\n\n", retrieve_str);
            retrieve(personal, content); // search and copy a content from another node        
            return; 
        }
        else{
            printf("Failed to read arguments of %s\n", retrieve_str); 
            return;
        } 
    }//else if

    else if(strcmp(cmd_str1, show_topology_str_short) == 0){
        
        printf("Executing %s %s...\n\n", show_str, topology_str);
        show_topology(personal);  // show topology of the personal node
        return;

    }// else if

    else if(strcmp(cmd_str1, show_str) == 0){
        
        if(strcmp(cmd_str2, topology_str) == 0){
            printf("Executing %s %s...\n\n", show_str, topology_str);
            show_topology(personal);  // show topology of the personal node
            return;
        }

        if(strcmp(cmd_str2, names_str) == 0){
            printf("Executing %s %s...\n\n", show_str, names_str);
            show_names(&personal->contents); // show contents of the personal node
            return;
        }

        if(strcmp(cmd_str2, interest_str) == 0 && strcmp(cmd_str3, table_str) == 0){

            printf("Executing %s %s %s...\n\n", show_str, interest_str, table_str);
            show_names(&personal->contents); // show contents of the personal node
            return;
        }
        
    }// else if

    else if(strcmp(cmd_str1, show_names_str_short) == 0){
        
        printf("Executing %s...\n\n", cmd_str1);
        show_names(&personal->contents); // show contents of the personal node
        return;

    }//else if sn

    else if(strcmp(cmd_str1, clear_names_str_short) == 0){

        printf("Executing %s...\n\n", cmd_str1);
        personal->contents = clear_names(personal->contents);  // clear the contents table of the personal node            
        return;

    }// else if cn

    else if(strcmp(cmd_str1, show_interest_table_str_short) == 0){

        if(strcmp(personal->persn_info->network, "") == 0)
        
        printf("Executing %s %s %s...\n\n", show_str, interest_str, table_str);
        show_routing(personal->route_tab); // show routing table of the personal node
        return;

    }//else if si
    
    else if(strcmp(cmd_str1, leave_str) == 0 || strcmp(cmd_str1, leave_str_short) == 0){

        //remove the personal node from the network
        // reset the struct in order to give the possibility to join or djoin again

        printf("Executing %s...\n\n", leave_str);
        if(strcmp(personal->persn_info->network, "") == 0){
            printf("The node is already without a network.\n");
        }
        leave(personal);

    }//else if leave

    else if(strcmp(cmd_str1, exit_str) == 0 || strcmp(cmd_str1, exit_str_short) == 0){

        // remove the personal node from the network  
        // free the memory, close the fds and get out of the program     
        
        printf("Executing %s...\n\n", exit_str);
        leave(personal);        
        free(personal->internals_array);
        free_contact(personal->persn_info);

        if(personal->contents != NULL){
        
            objectQueue_t *queue_ptr; // pointer to go through the list
            objectQueue_t *aux;     // auxiliary pointer to delete elements in the lists
            queue_ptr = personal->contents;
            
            while(queue_ptr != NULL){

                aux = queue_ptr;
                queue_ptr = queue_ptr->next;
                free(aux->string);
                free(aux);
                
            }                            
        }
            
        free(personal);
        exit(0);

    }//else if exit

    else if(strcmp(cmd_str1, help_str) == 0 || strcmp(cmd_str1, help_str_short) == 0) {
        
        printf("Executing %s...\n\n", help_str);
        help_menu();
        return;
    }
    
    else{
        printf("Error: Invalid command\n");
        printf("Type h or help to get a list of valid commands\n\n");
        return;
    }
    
}//select_cmd()
