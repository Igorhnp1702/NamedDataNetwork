/************************************************************************************************
 * ndn_queue.c
 *
 * Authors:
 *
 * -Igor Paruque ist1102901
 * -Mónica Ferreira ist106520
 *
 * Description: source code for the utility functions
 ***********************************************************************************************/

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
#include "ndn_node.h"
#include "ndn_queue.h"

