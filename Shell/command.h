/*
* Program 3: smallsh
* Author: Clare Brown
* ONID: browncla
* Date: 11/16/16
* filename: command.h
* Description: The header file for the command functions
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Command struct to hold information about input */
struct command{
	char * args[512];
	int argSize;
	int isInFile;
	int isOutFile;
	int isBackground;
	char inFile[255];
	char outFile[255];
	char cmd[50];
};

/* Function declarations */
struct command * init_command(char ** argsIn, int size);
void free_command(struct command *);
void print_command(struct command *);

