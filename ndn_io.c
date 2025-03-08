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


int is_valid_ip(const char *ip) {
    struct sockaddr_in sa;
    return inet_pton(AF_INET, ip, &(sa.sin_addr)) != 0;  // IPv4 check
}

int check_num(char *num_string){

    
}