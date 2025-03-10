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
#include "ndn_commands.h"
#include "ndn_node.h"


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


//-------------------------------------------- user commands --------------------------------------------//

int join(struct personal_node *personal, char *net) { 

    int success_flag = 0;

    if(strcmp(personal->persn_info->network, "") != 0){
        printf("Error in join: The node is already inside a network. Command ignored\n");
        return ++success_flag;
    }
    if(check_net(net) == 1){
        printf("Error in join: The network is invalid. Command ignored\n");
        printf("Please insert a three digit network from 000 to 999\n");
        return ++success_flag;
    }

    char msg[MAX_MSG_LENGTH];      memset(msg, 0, sizeof(msg));
    udp_flag++; 
    
    struct addrinfo srv_criteria, *srv_result;
    int fd, errflag;
    ssize_t cnt;
    struct sockaddr_in addr_conf;
    socklen_t addrlen_conf;
    char *buffer = (char*)calloc(MAX_NODESLIST, sizeof(char));

    fd = socket(AF_INET, SOCK_DGRAM, 0) ;
    if(fd == -1) return NULL;

    memset(&srv_criteria, 0, sizeof(srv_criteria));
    srv_criteria.ai_family = AF_INET;
    srv_criteria.ai_socktype = SOCK_DGRAM;

    errflag = getaddrinfo(personal->udp_address, personal->udp_port, &srv_criteria, &srv_result);
    if(errflag != 0) return NULL;

    cnt = sendto(fd, msg, strlen(msg), 0, srv_result->ai_addr, srv_result->ai_addrlen);
    if(cnt == -1) return NULL;

    freeaddrinfo(srv_result);

    addrlen_conf = sizeof(struct sockaddr_in);
    cnt = recvfrom(fd, buffer, MAX_NODESLIST, 0,(struct sockaddr *)&addr_conf, &addrlen_conf);
    if(cnt == -1) return -1;
    buffer[cnt] = '\0';

    if (strcmp(inet_ntoa(addr_conf.sin_addr), personal->udp_address) != 0) { //Verification
        strcpy(buffer, "0");
    }

    close(fd);
    token = strtok(buffer, delim);
            while (token != NULL) {             //Go through all the lines and populate an array with the available ID's
                if (flag > 0) {                                             //Iterating line by line, descarding the first one
                    sscanf(token, "%s %s %s", ls_id, ls_ip, ls_tcp);        //ls_id, ls_ip and ls_tcp are the contacts from the node in that line
                    free_id[atoi(ls_id)] = 1;                               //available ID's array
                }
                token = strtok(NULL, delim);
                flag = 1;
            }

            if (ls_id[0] == '\0') {
                strcpy(ls_id, slf_node->persn_info->node_id);
                strcpy(ls_ip, slf_node->persn_info->node_addr);
                strcpy(ls_tcp, slf_node->persn_info->tcp_port);
            }

            if (free_id[atoi(slf_node->persn_info->node_id)] != 0) {     //If the chosen ID is available, this if statmnent is skipped   
                for (cnt = 0, flag = 0; cnt < 99 && flag == 0; cnt++) {  //If the chosen ID isn't available, the first one available in free_id will be chosen   
                    if (free_id[cnt] == 0) {
                        sprintf(aux_id_str, "%02d", cnt);
                        strcpy(slf_node->persn_info->node_id, aux_id_str);
                        flag = 1;
                        printf("The chosen ID is not available!\nThe assigned ID is: %s\n", slf_node->persn_info->node_id);
                    }
                }
                if (flag == 0) {                                       
                    printf("Error in send_udp: The network is full!\n");
                    return ++success_flag;
                }
            }
            //djoin, TCP connection with neighbor

            memset(slf_node->persn_info->network, 0, 4*sizeof(char));
            if(djoin(slf_node, network, slf_node->persn_info->node_id, ls_id, ls_ip, ls_tcp) == 1) {
                printf("send_udp->join->djoin: Failed!\n");
                return ++success_flag;
            }
            //register node in node server
            if (rcv_msg != NULL) {
                free(rcv_msg);
            }
    
    djoin();
    //either print the successfull registration, or an error message

    sprintf(msg, "%s %s\n", nodes_str, net);
    success_flag = send_udp(personal, msg);
    return success_flag;

}//join


int djoin(struct personal_node *personal, char *net, char *connectIP, char *connectTCP){

    char msg[MAX_MSG_LENGTH];   memset(msg, 0, sizeof(msg));
    int success_flag = 0;

    if(strcmp(personal->persn_info->network, "") != 0){
        printf("Error in direct join: The node is already inside a network. Command ignored\n");
        return ++success_flag;
    }
    if(check_net(net) == 1){
        printf("Error in direct join: The network is invalid. Command ignored\n");
        printf("Please insert a three digit network from 000 to 999\n");
        return ++success_flag;
    }

    if(udp_flag == 0){
           
        if(check_ports(connectTCP) == 1){
            printf("Error in direct join: The boot TCP port is invalid. Command ignored\n");
            printf("Please insert a 1 to 5 digit TCP port from 0 to 65536");
            return ++success_flag;
        }
    }
    
    strcpy(personal->persn_info->network, net);

    if (strcmp(connectIP, "0.0.0.0") == 0) {  //First node of the network
        personal->anchorflag = 1; //!confirmar
        personal->extern_node = contact_init(personal->extern_node);
        contact_copy(personal->extern_node, personal->persn_info);
        personal->backup_node = contact_init(personal->backup_node);
        contact_copy(personal->backup_node, personal->persn_info);
    }
    else{
        personal->extern_node = contact_init(personal->extern_node);        
        strcpy(personal->extern_node->network, net);
        strcpy(personal->extern_node->tcp_port, connectTCP);
        strcpy(personal->extern_node->node_addr, connectIP);
        personal->backup_node = contact_init(personal->backup_node);

        //ENTRY message
        sprintf(msg, "%s %s %s\n", entry_str, personal->persn_info->node_addr, personal->persn_info->tcp_port);
        //Sends ENTRY message , receives SAFE response and updates backup node
        if(send_tcp(personal, personal->extern_node, msg)){
            printf("Error in direct join: Failed to join the network\n");
            return 1;
        }
        // check if it's an anchor
        if (strcmp(personal->persn_info->node_addr, personal->backup_node->node_addr) == 0 && 
        strcmp(personal->persn_info->tcp_port, personal->backup_node->tcp_port) == 0) {
            personal->anchorflag = 1;
        }
    }
    printf("Successfuly joined a network\n"); 
    printf("Your node has the following info:\n\n");
    
    printf("Network: %s\nAdress: %s\nPort: %s\n",       
    personal->persn_info->network,
    personal->persn_info->node_addr, 
    personal->persn_info->tcp_port);
    
    return 0;
}//djoin


objectQueue_t *create(objectQueue_t *queue_ptr, char *name){ // name size <= 100, alphanumeric chars only

    if(check_name(name) == 1){
        printf("Error in create: invalid name\n");
        return contents;
    }
    
    int name_size = strlen(name) + 1; 
    queueBlock_t *new_node = (queueBlock_t*)calloc(1, sizeof(queueBlock_t));
    new_node->name = (char*)calloc(name_size, sizeof(char));
    strcpy(new_node->name, name);

    if(queue_ptr == NULL){ // if true, we are creating the first element of the list

        queue_ptr = new_node;
        contents->next = NULL;
        
        printf("Content created at the beginning of the list: %s\n", name);
        return contents;
    }

    objectQueue_t *queue_ptr; // pointer to go through the list
    
    queue_ptr = contents;

    while(queue_ptr->next != NULL){ //reach the last element of the list
        queue_ptr = queue_ptr->next;		
    }
        
    queue_ptr->next = new_node;
    printf("\n%s was inserted in the list\n\n", name);
    return contents;
}//create


objectQueue_t *delete(objectQueue_t *contents, char *name) {
    
    if(check_name(name) == 1){
        printf("Error in delete: Invalid name\n");
        return contents;
    }

    if(contents == NULL){
        printf("Error in delete: No contents available\n");
        return contents;
    }

    objectQueue_t *queue_ptr; // pointer to go through the list
    objectQueue_t *aux; // auxiliary pointer to delete elements of the lists
    queue_ptr = contents;

    if(strcmp(contents->string, name) == 0){ //delete at the head of the list

        aux = contents;
        contents = contents->next;
        free(aux->string);
        free(aux);
        printf("%s was successfuly deleted\n", name);
        if(contents == NULL){
            printf("\nThe list of contents is now empty\n\n");
        }		
        return contents;
    }

    while(queue_ptr != NULL){

        if(queue_ptr->next == NULL && strcmp(queue_ptr->string, name) != 0){            
            printf("Error in delete: The name was not found\n");
            return contents;
        }
        
        else if(strcmp(queue_ptr->string, name) == 0){ 

            aux = queue_ptr;
            queue_ptr = queue_ptr->next;
            free(aux->string);
            free(aux);
            printf("The deletion of %s was successful\n", name);
            if(queue_ptr == NULL){
                printf("\nThe list of contents is now empty\n\n");
            }
            return contents;
        }
        
        else if(strcmp(queue_ptr->next->string, name) == 0){
            
            aux = queue_ptr->next;
            queue_ptr->next = queue_ptr->next->next;
            free(aux->string);
            free(aux);
            printf("The deletion of %s was successful\n", name);
            return contents;
        }
        else queue_ptr = queue_ptr->next;
    }    
    return contents;
}//delete


int retrieve(struct personal_node *personal, char *name){

    int success_flag = 0;
    if(strcmp(personal->persn_info->network, "") == 0){
        printf("Error in retrieve: You are not inside a network\n");
        return ++success_flag;
    }
    
    /* check the expedition table */
    
    int search_id = atoi(dest_id);  //integer to search the table of intern neighbors
    
    int gateway = -1;                      //id of neighbor to send something to the destination
    char retrieve_buffer[MAX_MSG_LENGTH];     //buffer for the "QUERY" message
    
    memset(retrieve_buffer, 0, sizeof(retrieve_buffer));

    sprintf(retrieve_buffer,"RETRIEVE %s %s\n", name);    

    if(personal->route_tab[search_id] == search_id){ // destination is equal to the neighbor, but what type of neighbor?

        if(strcmp(personal->extern_node->node_id, dest_id) == 0 || strcmp(personal->backup_node->node_id, dest_id) == 0 ){ // extern/backup neighbor
            printf("Sending: %s\n", retrieve_buffer);
            send_tcp(personal, personal->extern_node, retrieve_buffer);                        
        }
        else{
            printf("Sending: %s\n", retrieve_buffer);
            send_tcp(personal, personal->internals_array[search_id], retrieve_buffer); // intern neighbor 
        }                               
    }
    else if(personal->route_tab[search_id] != -1){ // destination is initialized and is different from the neighbor

        gateway = personal->route_tab[search_id];
        if(gateway == atoi(personal->extern_node->node_id) || gateway == atoi(personal->backup_node->node_id)){
            printf("Sending: %s\n", retrieve_buffer);
            send_tcp(personal, personal->extern_node, retrieve_buffer);
        }
        else{
            printf("Sending: %s\n", retrieve_buffer);
            send_tcp(personal, personal->internals_array[gateway], retrieve_buffer);
        }        
    }
    else{ //destination is not a neighbor and it is not initialized
        printf("Sending: %s\n", retrieve_buffer);
        send_tcp(personal, personal->extern_node, retrieve_buffer);

        for(int iter = 0; iter < MAX_INTERNALS; iter++){

            if(personal->internals_array[iter] != NULL){
                printf("Sending: %s\n", retrieve_buffer);
                send_tcp(personal, personal->internals_array[iter], retrieve_buffer);                
            }
        }
    }
    return 0;
}//retrieve


int show_topology(struct personal_node *personal){
    
    int success_flag = 0;
    int printed_interns = 0;

    if(strcmp(personal->persn_info->network, "") != 0){
        printf("\n\nYour node:\n\n");        
        printf("Network: %s\nAdress: %s\nPort: %s\n\n",           
        personal->persn_info->network,
        personal->persn_info->node_addr, 
        personal->persn_info->tcp_port);
        
    }
    else{
        printf("Error in show topology: You are not inside a network\n");
        return ++success_flag;
    }
    
    if (personal->extern_node != NULL) {
        printf("\n\nExternal neighbor: \n");        
        printf("Network: %s\nAdress: %s\nPort: %s\n\n",          
        personal->extern_node->network,
        personal->extern_node->node_addr, 
        personal->extern_node->tcp_port);
        
        
    }
    else {
        printf("\nThere is no external neighbor!\n");
    }
    if (personal->backup_node != NULL) {
        printf("\n\nBackup neighbor: \n");                
        printf("Network: %s\nAdress: %s\nPort: %s\n\n",         
        personal->backup_node->network, 
        personal->backup_node->node_addr, 
        personal->backup_node->tcp_port);
        
        
    }   
    else {
        printf("\nThere is no backup neighbor!\n");   
    }

    int iter;
        
    if(personal->internals_array == NULL){
        printf("\nThere are no internal neighbors\n");        
    }
    else{
        printf("\n\nPrinting internal neighbors: \n\n");        
            
        for(iter = 0; iter < MAX_INTERNALS; iter++){

            if(personal->internals_array[iter] != NULL){

                printf("ID: %s\nNetwork: %s\nAdress: %s\nPort: %s\n\n",                    
                personal->internals_array[iter]->network,
                personal->internals_array[iter]->node_addr,
                personal->internals_array[iter]->tcp_port);

                printed_interns++;
            }//if            
        }//for                    
    }
    
    if(printed_interns == 0){ // the array is initialized

        printf("There are no internal neighbors\n");
        return ++success_flag;
    }
    return success_flag;
} // show_topology

int show_names(objectQueue_t **contents){

    int printed_names = 0; // counter of printed names
    int success_flag = 0; //success signal for the main function

    if(*contents == NULL){
        printf("The node has no contents to print\n");
        return ++success_flag;
    }

    printf("Printing names...\n\n");
    objectQueue_t *queue_ptr; // pointer to go through the list

    for(queue_ptr = *contents; queue_ptr != NULL; queue_ptr = queue_ptr->next){
        printf("%s\n", queue_ptr->string);
        printed_names++;
    }
    printf("\nNumber of printed names: %d\n", printed_names);
    return success_flag;
}//show_names

int show_interest_table (struct personal_node *personal) { 

    int success_flag = 0;

    if(strcmp(personal->persn_info->network, "") == 0){
        printf("Error in show interest table: You are not inside a network\n");
        return ++success_flag;
    }

    printf("\nInterest Table: \n\n");
    for (int i = 0; i < 10; i++) {
        if (personal->interest_tab[i].node_id[0] != '\0') {
            printf("Entry %d:\n", i + 1);
            printf("ID: %s\nNetwork: %s\nAddress: %s\nPort: %s\n\n",
                personal->interest_tab[i].node_id,
                personal->interest_tab[i].network,
                personal->interest_tab[i].node_addr,
                personal->interest_tab[i].tcp_port);
        }
    }

    return success_flag;
}//show_interest_table  

objectQueue_t *clear_names(objectQueue_t *contents){

    int cleared_names = 0;  //number of names cleared

    if(contents == NULL){
        printf("No contents to clear in this node\n");
        return contents;
    }
    printf("\nClearing names...\n");

    objectQueue_t *queue_ptr; // pointer to go through the list
    objectQueue_t *aux;     // auxiliary pointer to delete elements in the lists
    queue_ptr = contents;
    while(queue_ptr != NULL){

        aux = queue_ptr;
        queue_ptr = queue_ptr->next;
        free(aux->string);
        free(aux);
        cleared_names++;
    }    
    printf("\nNumber of cleared names: %d\n", cleared_names);
    contents = NULL;
    return contents;    
}//clear_names()


int leave(struct personal_node *personal) {
    
    if(strcmp(personal->persn_info->network, "") == 0){        
        //personal = reset_personal(personal);
        printf("Error in leave command: You are not inside a network\n");
        return 1;
    }
    
    if(udp_flag == 1){
        char msg[MAX_MSG_LENGTH];
        memset(msg, 0, sizeof(msg));
        sprintf(msg, "%s %s\n", unreg_str, personal->persn_info->network);
        send_udp(personal, msg);
        udp_flag = 0;
    }
    
    int i = 0;
    for (i = 3; i <= personal->max_fd; i++) {
        if (FD_ISSET(i, &personal->crr_scks)) {
            FD_CLR(i, &personal->crr_scks);
            close(i);
        }
    }

    strcpy(personal->persn_info->network, "");
    personal = reset_personal(personal);
    return 0;
}//leave

void help_menu() {
        printf("*****************HELP MENU*****************\n\n");
        printf("To use the application, insert one of the following commands:\n");
        printf("Note: curved brackets => valid abreviations; square brackets => arguments\n\n");
        printf("join (j) [desired network] \n");
        printf("direct join (dj) [desired network] [connect IPv4 address] [connect TCP port]\n");        
        printf("create (c) [name]\n");
        printf("delete (dl) [name]\n");
        printf("retrieve (r) [name]\n");
        printf("show topology (st)\n");
        printf("show names (sn)\n");
        printf("show interest table (si)\n");        
        printf("clear names (cn)\n");
        printf("leave (l)\n");
        printf("exit (x)\n");
        return;
}//help_menu

/*--------------End user interface functions--------------*/