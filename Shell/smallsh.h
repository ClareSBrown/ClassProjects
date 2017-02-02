/*
* Program 3: smallsh
* Author: Clare Brown
* ONID: browncla
* Date: 11/16/16
* filename: smallsh.h
* Description: The header file for the shell functions
*/

#include <sys/wait.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <signal.h>

#include "command.h"

/* Function declarations */
void shell_loop();
char * shell_readLine();
struct command * shell_parseLine(char* line);
int shell_execution(struct command *);
int shell_exit();
int shell_cd (struct command *);
int shell_status ();
int shell_start(struct command *);



