#################################################################################################
# Makefile for the Computer Networking project
# 
# Authors:
#
# -Igor Paruque ist1102901
# -Afonso Klier ist196139
#
# Command to clean object files: make clean
#################################################################################################
CC = gcc

CFLAGS = -g -Wall -std=c99

APPNAME = ndn

SRCDIR = 
OBJDIR = 

OBJFILES = ${OBJDIR}ndn_main.o ${OBJDIR}ndn_node.o ${OBJDIR}ndn_commands.o  ${OBJDIR}ndn_messages.o ${OBJDIR}ndn_queue.o


all: ndn

ndn: ${OBJFILES}
	${CC} ${CFLAGS} -o ${APPNAME} ${OBJFILES}

${OBJDIR}ndn_main.o: ${SRCDIR}ndn_main.c ${SRCDIR}ndn_node.h ${SRCDIR}ndn_commands.h ${SRCDIR}ndn_messages.h # ${SRCDIR}ndn_queue.h ${SRCDIR}ndn_interestTable.h
	${CC} ${CFLAGS} -c ${SRCDIR}ndn_main.c -o ${OBJDIR}ndn_main.o

${OBJDIR}ndn_node.o: ${SRCDIR}ndn_node.c ${SRCDIR}ndn_node.h ${SRCDIR}ndn_commands.h  # check ndn_node dependencies
	${CC} ${CFLAGS} -c ${SRCDIR}ndn_node.c -o ${OBJDIR}ndn_node.o

${OBJDIR}ndn_commands.o: ${SRCDIR}ndn_commands.c ${SRCDIR}ndn_commands.h ${SRCDIR}ndn_node.h ${SRCDIR}ndn_messages.h # check ndn_commands dependencies
	${CC} ${CFLAGS} -c ${SRCDIR}ndn_commands.c -o ${OBJDIR}ndn_commands.o

${OBJDIR}ndn_messages.o: ${SRCDIR}ndn_messages.c ${SRCDIR}ndn_messages.h # check ndn_messages dependencies
	${CC} ${CFLAGS} -c ${SRCDIR}ndn_messages.c -o ${OBJDIR}ndn_messages.o

#${OBJDIR}ndn_queue.o: ${SRCDIR}g23tcp_udp.c ${SRCDIR}g23tcp_udp.h ${SRCDIR}g23funcs.h # check ndn_queue dependencies
#	${CC} ${CFLAGS} -c ${SRCDIR}g23tcp_udp.c -o ${OBJDIR}g23tcp_udp.o
#
#${OBJDIR}ndn_interestTable.o: ${SRCDIR}g23tcp_udp.c ${SRCDIR}g23tcp_udp.h ${SRCDIR}g23funcs.h # check ndn_interestTable dependencies
#	${CC} ${CFLAGS} -c ${SRCDIR}g23tcp_udp.c -o ${OBJDIR}g23tcp_udp.o
	
clean:	
	rm -f ${OBJFILES}

cleanall:
	rm -f ${OBJFILES} ${APPNAME}

val:
	valgrind --leak-check=full --show-leak-kinds=all -s ./ndn $(ARG1) $(ARG2) $(ARG3) $(ARG4) $(ARG5)

# To provide arguments with make val, type ARG1=[first arg] ARG2=[second arg] and so on, without spaces
