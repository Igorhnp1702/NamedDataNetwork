#################################################################################################
# Makefile for the Computer Networking project
# 
# Authors:
#
# -Igor Paruque ist1102901
# -Mónica Ferreira ist1106520
#
# Command to clean object files: make clean
#################################################################################################
CC = gcc

CFLAGS = -g -Wall -std=c99

APPNAME = ndn

SRCDIR = 
OBJDIR = 

OBJFILES = ${OBJDIR}ndn_main.o ${OBJDIR}ndn_node.o ${OBJDIR}ndn_commands.o  ${OBJDIR}ndn_messages.o ${OBJDIR}ndn_interestTable.o ${OBJDIR}ndn_objectStructs.o


ndn: ${OBJFILES}
	${CC} ${CFLAGS} -o ${APPNAME} ${OBJFILES}

${OBJDIR}ndn_main.o: ${SRCDIR}ndn_main.c ${SRCDIR}ndn_node.h ${SRCDIR}ndn_commands.h ${SRCDIR}ndn_messages.h ${SRCDIR}ndn_interestTable.h ${SRCDIR}ndn_objectStructs.h 
	${CC} ${CFLAGS} -c ${SRCDIR}ndn_main.c -o ${OBJDIR}ndn_main.o

${OBJDIR}ndn_node.o: ${SRCDIR}ndn_node.c ${SRCDIR}ndn_node.h ${SRCDIR}ndn_messages.h 
	${CC} ${CFLAGS} -c ${SRCDIR}ndn_node.c -o ${OBJDIR}ndn_node.o

${OBJDIR}ndn_commands.o: ${SRCDIR}ndn_commands.c ${SRCDIR}ndn_commands.h ${SRCDIR}ndn_node.h ${SRCDIR}ndn_messages.h 
	${CC} ${CFLAGS} -c ${SRCDIR}ndn_commands.c -o ${OBJDIR}ndn_commands.o

${OBJDIR}ndn_messages.o: ${SRCDIR}ndn_messages.c ${SRCDIR}ndn_messages.h ${SRCDIR}ndn_node.h  ${SRCDIR}ndn_interestTable.h ${SRCDIR}ndn_objectStructs.h 
	${CC} ${CFLAGS} -c ${SRCDIR}ndn_messages.c -o ${OBJDIR}ndn_messages.o

${OBJDIR}ndn_interestTable.o: ${SRCDIR}ndn_interestTable.c ${SRCDIR}ndn_interestTable.h 
	${CC} ${CFLAGS} -c ${SRCDIR}ndn_interestTable.c -o ${OBJDIR}ndn_interestTable.o

${OBJDIR}ndn_objectStructs.o: ${SRCDIR}ndn_objectStructs.c ${SRCDIR}ndn_objectStructs.h ${SRCDIR}ndn_messages.h ${SRCDIR}ndn_node.h ${SRCDIR}ndn_commands.h 
	${CC} ${CFLAGS} -c ${SRCDIR}ndn_objectStructs.c -o ${OBJDIR}ndn_objectStructs.o
	
clean:	
	rm -f ${OBJFILES}

all: 
	make clean
	make ndn

compclean:
	make ndn
	make clean

cleanall:
	rm -f ${OBJFILES} ${APPNAME}

val:
	valgrind --leak-check=full --show-leak-kinds=all -s ./ndn $(ARG1) $(ARG2) $(ARG3) $(ARG4) $(ARG5)

# To provide arguments with make val, type ARG1=[first arg] ARG2=[second arg] and so on, without spaces

# Some program calls:

# ./ndn 10 127.0.0.1 51000 
# ./ndn 10 193.236.216.177 51000
