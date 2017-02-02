/*
* Program 3: smallsh
* Author: Clare Brown
* ONID: browncla
* Date: 11/16/16
* filename: command.c
* Description: The implementation file for the command functions
*/

#include "command.h"


/* 
* A function to initialize the command struct based on the array of inputs
* and the size of the array. 
* Parameters: 
*	-array of pointers to strings
* 	-int representing the size of the array
* Returns:
* 	-a pointer to a command struct
*/
struct command * init_command(char ** argsIn, int size){
	int i;
	int j = 0;
	struct command * tempCommand = malloc(sizeof(struct command));
	char * argsList[512];

	// Initializing items in the struct
	tempCommand->isInFile = 0;
	tempCommand->isOutFile = 0;
	tempCommand->isBackground = 0;

	// If the first item in the input array is NULL, the line entered was an
	// empty line or was a comment starting with #. If this is true, set the cmd 
	// field to an empty string and return because there is nothing else to parse. 
	// Otherwise, the first field in the input array is the command and is set 
	// to the cmd field.
	if (argsIn[0] == NULL){
		strcpy(tempCommand->cmd,"");
		return tempCommand;
	}
	else {
		strcpy(tempCommand->cmd, argsIn[0]);
	}
	
	// Checking each item in the input array, checking if it is a special symbol 
	// (<, >, or &) and storing the appropriate information in the struct
	for (i = 0; i < size; i++){
		// If the input is <, set isInFile to 1 to show that an inFile is expected
		// and if there is something after the <, set its name to be the inFile name. 
		if (strcmp(argsIn[i], "<") == 0){
			tempCommand->isInFile = 1;
			if (argsIn[i+1] != NULL){
				strcpy(tempCommand->inFile, argsIn[i+1]);
				i++;
			}
		}
		// If the input is >, set isOutFile to 1 to show that an outFile is expected
		// and if there is something after the >, set its name to be the outFile name.
		else if (strcmp(argsIn[i], ">") == 0){
			tempCommand->isOutFile = 1;
			if (argsIn[i+1] != NULL){
				strcpy(tempCommand->outFile, argsIn[i+1]);
				i++;
			}
		}
		// If the input is &, set isBackground to 1 to show that this should be a background
		// process. Set the size of the argument array and return the command struct.
		else if (strcmp(argsIn[i], "&") == 0) {
			tempCommand->isBackground = 1;
			tempCommand->argSize = j;
			return tempCommand;
		}
		// Copying the arguments into the args array of the command struct if not a special character
		else if (argsIn[i] != NULL){
			argsList[i] = malloc(128 * sizeof(char));
			strcpy(argsList[i], argsIn[i]);
			tempCommand->args[i] = argsList[i];
			j++; //recording size of argument array
		}
	}
	tempCommand->argSize = j; //setting the size of the argument array
	return tempCommand; //returning the command struct
}


/* 
* A function to free the command struct
* Parameters: 
*	-a pointer to a command stsruct
* Returns:
* 	-void
*/
void free_command(struct command * cmd){
	int i;
	for (i = 0; i < cmd->argSize; i++){
		free(cmd->args[i]);
	}
	free(cmd);
}


/* 
* A function to print the contents of a command struct to help with debugging
* Parameters: 
*	-a pointer to a command struct
* Returns:
* 	-void
*/

void print_command(struct command * cmdIn){
	int i;
	printf("Command: %s\n", cmdIn->cmd);
	printf("Number of arguments: %d\n", cmdIn->argSize);
	printf("Arguments: \n");
	for (i = 0; i < cmdIn->argSize; i++){
		printf("%s ", cmdIn->args[i]);
	}
	printf("\nisInFile: %d\n", cmdIn->isInFile);
	printf("isOutFile: %d\n", cmdIn->isOutFile);
	printf("isBackground: %d\n", cmdIn->isBackground);
	printf("inFile: %s\n", cmdIn->inFile);
	printf("outFile: %s\n", cmdIn->outFile);
}