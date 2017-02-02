/*
* Program 3: smallsh
* Author: Clare Brown
* ONID: browncla
* Date: 11/16/16
* filename: smallsh.c
* Description: The implementation file for the shell functions
*/

#include "smallsh.h"

/*  Global variables  */
int exit_status = 0;  //used to hold the number for the exit status
char exit_string[50]=""; //used to store the string for printing the exit status
pid_t backgroundPID[30]; //used to store the pids of the processes running in the background

/* 
* A function that maintains the shell by looping through the following steps:
* checking if any background have ended, prompting the user for input, retrieving 
* the input with the shell_readLine function, parsing the input with the shell_parseLine 
* function, and executing the commands with the shell_execution function.
* Parameters: 
*	-none
* Returns:
* 	-void
*/
void shell_loop(){
	char *line;
	struct command * cmdIn;
	int status, j, i;
	pid_t wpid;
	struct sigaction sa;
	
	// Setting up the parent process and background processes to ignore the SIGINT signal
	// by using SIG_IGN as the handler for SIGINT
	sa.sa_handler = SIG_IGN; 
	sigaction(SIGINT, &sa, NULL);

	// Main loop of shell
	do {
		// Checking if any background processes have finished by looping through an array
		// of the pids of the processes running in the background and calling waitpid. By
		// using WNOHANG, if the process has not finished, it will immediately return with
		// a 0. Otherwise, a number greater than 0 indicates that the process has finished.
		for (j = 0; j < 30; j++){
			if (backgroundPID[j] != 0){
				if (wpid = waitpid(backgroundPID[j], &status, WNOHANG) > 0){
					if (WEXITSTATUS(status) != 0)
						exit_status = WEXITSTATUS(status); //storing the exit status if exited normally
					else if (WIFSIGNALED(status) !=0)
						exit_status = WTERMSIG(status); //storing the exit status if exited by signal

					// Displaying that background process is done
					if (exit_status == 0 || exit_status == 1)
						sprintf(exit_string, "exit value %d", exit_status);
					else
						sprintf(exit_string, "terminated by signal %d", exit_status);
					printf("background pid %d is done: %s\n", backgroundPID[j], exit_string);
					backgroundPID[j] = 0; //setting value in array of pids to 0 for later use
				}
			}
		}

		// Promting the user for input, reading it, parsing it, and executing it
		printf(": ");
		fflush(stdout);
		line = shell_readLine();
		cmdIn = shell_parseLine(line);
		status = shell_execution(cmdIn); //If returns 0, program exits
		free(line);
		free_command(cmdIn);
	} while (status);
	
	
}


/* 
* A function that reads the line inputed by the user. To handle the variable expansion of $$, 
* checks to see if $$ is included in the string and replaces it with the current pid if necessary. 
* Parameters: 
*	-none
* Returns:
* 	-pointer to a string of characters
*/
char * shell_readLine(){
	size_t bufferSize = 2048; //restricting the number of characters
	size_t expandedSize = 2058; //adding 10 for the expansion of $$
	char *buffer = malloc(sizeof(char)*bufferSize);
	char *expandedString = malloc(sizeof(char)*expandedSize);
	int pid;
	char pidStr[10];
	char *p;

	getline(&buffer, &bufferSize, stdin); // Getting line from input

	// Checking if $$ is in string, if not returns the string
	if (!(p = strstr(buffer, "$$")))
		return buffer;

	// Otherwise, gets the pid of current process and inserts it where $% is
	pid = getpid();
	sprintf(pidStr, "%d", pid);
	strncpy(expandedString, buffer, p-buffer);
	expandedString[p-buffer] = '\0';
	sprintf(expandedString + (p-buffer), "%s%s", pidStr, p+strlen("$$"));

	// Returning the updated string
	return expandedString;
}


/* 
* A function that parses the line found from the shell_readLine function. Places each string
* separated by a space into an array of c-style strings. Then passes that array to the 
* init_command function to place the correct items in the correct places. 
* Parameters: 
*	-pointer to a c-style string
* Returns:
* 	-pointer to a command struct
*/
struct command * shell_parseLine(char* line){
	int argsSize = 512; //restricting the number of arguments
	int i = 0;
	int j;
	char **args = malloc(sizeof(char)*argsSize);
	char *input;

	// Checking if allocation worked
	if (!args){
		fprintf(stderr, "parseLine: Allocation Error\n");
		exit (1);
	}

	// If the line does not start with # or a new line character move through
	// the line using strtok and count the number of arguments. 
	if (line[0] != 35 && line[0] != '\n'){
		input = strtok(line, "\n ");
		while (input != NULL && i < argsSize){
			args[i] = input;
			i++;
			input = strtok(NULL, "\n ");
		}
	}
	// Setting last (or first in the case of a new line/#) item in argument array to NULL
	args[i] = NULL;

	// Set up the command struct
	struct command * cmd = init_command(args, i);

	return cmd;
}


/* 
* A function that executes the command stored in the command struct. Checks if there is a
* command and if the command is a built-in command (cd, status, exit). If it is not a built-in
* command, the command struct is passed to shell_start which handles forking and execution.
* Parameters: 
*	-pointer to a command struct
* Returns:
* 	-int indicating the result of the command
*/
int shell_execution(struct command * cmdIn){
	if (strcmp(cmdIn->cmd, "") == 0) 
		return 1;
	else if (strcmp(cmdIn->cmd, "exit") == 0)
		return shell_exit();
	else if (strcmp(cmdIn->cmd, "cd") == 0)
		return shell_cd(cmdIn);
	else if (strcmp(cmdIn->cmd, "status") == 0)
		return shell_status();
	else
		return shell_start(cmdIn);
}


/* 
* A function that handles the built-in command exit. This command can only be run in the foreground
* and so all other foreground processes are exited before it is called. The only processes that need
* to be cleaned up are the background processes. The function loops through the pids of background
* processes stored in backgroundPID and kills them with SIGTERM. It then returns 0 to break out of the
* loop in the shell_loop function to end the shell.
* Parameters: 
*	-none
* Returns:
* 	-the int 0 indicating that it has killed all of the background child processes.
*/
int shell_exit(){
	int j;

	// Cleaning up all background child processes before ending
	for (j = 0; j < 30; j++){
			if (backgroundPID[j] != 0){
				kill(backgroundPID[j], SIGTERM);
			}
	}
	return 0;
}


/* 
* A function that handles the built-in command cd. If there are no arguments for the cd
* command, the directory is changed to the home directory. If there is an argument, the
* directory is changed to that directory. If there is an error, the exit_status is set to
* 1 and an error statment is printed. If there is no error, the exit_status is set to 0.
* Parameters
*	-pointer to a command struct
* Returns:
* 	-int indicating command was run
*/
int shell_cd (struct command * cmdIn){
	if (cmdIn->argSize == 1){
		chdir(getenv("HOME"));	
	}
	else if (chdir(cmdIn->args[1]) != 0){
		fprintf(stderr,"Cannot change directory\n");
		exit_status = 1;
	}

	exit_status = 0;
	return 5;
}


/* 
* A function that combines the exit_status global variable with the appropriate starting
* string and prints it. 
* Parameters: 
*	-none
* Returns:
* 	-int greater than 0 indicating that the action is complete
*/
int shell_status(){
	
	if (exit_status == 0 || exit_status == 1)
		sprintf(exit_string, "exit value %d", exit_status);
	else
		sprintf(exit_string, "terminated by signal %d", exit_status);
	printf("%s\n", exit_string);
	exit_status = 0;
	return 6;
}


/* 
* A function that handles when the command given is not a built-in command. The function
* creates a child process, redirects file pointers as necessary, and sets the processes as
* foreground or background as necessary.  
* Parameters: 
*	-pointer to a command struct
* Returns:
* 	-int indicating that the function is complete
*/
int shell_start(struct command *cmdIn){
	pid_t forkPid = -5;
	pid_t wpid;
	int status = -5;
	int fileStatus, result;
	int i=0;
	int j;
	struct sigaction sa;

	// Forking the get a child process
	forkPid = fork();
	
	// Checking if fork was successful
	if (forkPid < 0){
		perror("Fork failure\n");
		exit (1);		
	}

	// If fork is successful, perform redirections and call execvp
	else if (forkPid == 0){

		// Redirecting input file if necessary
		if (cmdIn->isInFile == 1){
			// Checking if input file is valid
			fileStatus = open(cmdIn->inFile, O_RDONLY);
			if (fileStatus == -1){
				fprintf(stderr, "Cannot open file for input\n");
				exit_status = 1;
				exit(1);
			}
			// Checking if redirection is successful
			result = dup2(fileStatus, 0);
			if (result == -1){
				fprintf(stderr,"dup2 error\n");
				exit_status = 1;
				exit(1);
			}
		}

		// Redirecting output file if necessary
		if (cmdIn->isOutFile == 1)	{
			// Checking if output file is valid
			fileStatus = open(cmdIn->outFile, O_WRONLY | O_CREAT | O_TRUNC, 0644);
			if (fileStatus == -1){
				fprintf(stderr,"Cannot open file for output\n");
				exit_status = 1;
				exit(1);
			}
			// Checking if redirection is successful
			result = dup2(fileStatus, 1);
			if (result == -1){
				fprintf(stderr,"dup2 error \n");
				exit_status = 1;
				exit(1);
			}
		}

		// Redirecting stdout and stdin to /dev/null if command is background process so output doesn't print
		if (cmdIn->isBackground == 1){
			fileStatus = open("/dev/null", O_WRONLY);
			if (fileStatus == -1){
				fprintf(stderr,"C\n");
				exit_status = 1;
				exit(1);
			}
			// Checking if redirection of stdout is successful
			result = dup2(fileStatus, STDOUT_FILENO);
			if (result == -1){
				fprintf(stderr,"dup2 error \n");
				exit_status = 1;
				exit(1);
			}
			// Checking if redirection of stdin is successful
			result = dup2(fileStatus, STDOUT_FILENO);
			if (result == -1){
				fprintf(stderr,"dup2 error \n");
				exit_status = 1;
				exit(1);
			}
		}
		// If not a background process, set SIGINT to default to allow for control-c 
		// to kill the foreground processes
		else {
			sa.sa_handler = SIG_DFL;
			sigaction(SIGINT, &sa, NULL);
		}

		// Running execvp. If it fails, print an error
		if (execvp(cmdIn->cmd, cmdIn->args) == -1){
			fprintf(stderr,"Cannot find file or directory\n");
			exit(1);
		}
	}

	// If it is a parent process, wait for the child process
	else {
		// If the process is not a background process, wait for child process to end
		if (cmdIn->isBackground == 0){
			do {
				wpid = waitpid(forkPid, &status, 0);
			} while (!WIFEXITED(status) && !WIFSIGNALED(status));
			// If exited normally, get the exit status
			if (WIFEXITED(status) != 0){
				exit_status = WEXITSTATUS(status);
			}
			// If exited by signal, get exit status and print status
			else if (WIFSIGNALED(status) != 0){
				exit_status = WTERMSIG(status);
				sprintf(exit_string, "terminated by signal %d", exit_status);
				printf("%s\n", exit_string);

			}
		}

		// If process is background, print the pid and add the pid to backgroundPID array
		else {
			printf("background pid is %d\n", forkPid);
			while (backgroundPID[i] != 0)
				i++;
			backgroundPID[i] = forkPid;
		}
	}
	return 1;
}