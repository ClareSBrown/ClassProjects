/*
* Author: Clare Brown
* Date: 12/1/16
* Description: A program that receives ciphertext and a key from another program
* through sockets, decrypts the ciphertext into plaintext, and sends it back
* to the program that sent the ciphertext and key.  
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>

/*
* A function to handle when there are errors. 
* Parameters: 
* 	-pointer to a string with the error message to be printed
* 	-int representing the error code to be used
* Returns: 
* 	-void
*/
void error(const char *msg, int i) { 
	fprintf(stderr, "%s", msg); 
	exit(i); 
} 


/*
* A function to verify that the incoming connection is from otp_dec. If the
* socket receives the string "dec" then it is coming from otp_dec. If the 
* verification is successful, "yes" is sent back to the sender, otherwise
* no is sent back to the sender.
* Parameters: 
* 	-int representing the file descriptor for the socket
* Returns: 
* 	-int indicating if true or false, 1 for true, 0 for false
*/
int handshake(int socket){
	char buffer[256];
	int charsRead;
	memset(buffer, '\0', 256);

	// Read the client's message from the socket
	charsRead = recv(socket, buffer, 255, 0); 
	if (charsRead < 0) 
		error("otp_dec_d error: reading from socket\n", 1);
	if (strcmp(buffer, "dec") == 0){
		// Send a Success message back to otp_dec
		charsRead = send(socket, "yes", 4, 0); // Send success back
		if (charsRead < 0) 
			error("otp_dec_d error: writing to socket\n", 1);
		return 1;
	}
	else{
		// Send a failure message back to the client
		charsRead = send(socket, "no", 3, 0); 
		if (charsRead < 0) 
			error("otp_dec_d error: writing to socket\n", 1);
		return 0;
	}
}


/*
* A function to decrpyt the ciphertext into plaintext using the key. Sets the 
* ciphertext and key characters to a number between 0 and 26 inclusive with space
* being zero and A = 1. Adds those numbers together and finds the remainder 
* when divided by 27. Then changes those numbers back to characters from A-Z and space. 
* Parameters: 
* 	-pointer to a string representing the ciphertext
* 	-pointer to a string representing the key
* 	-int representing the length of the ciphertext
* Returns: 
* 	-int indicating if true or false, 1 for true, 0 for false
*/
char * decrypt(char * ciphertext, char * key, int length){
	char p, k, c;
	int i;
	char * plaintext = malloc(length*sizeof(char)); //create room for the plaintext
	memset(plaintext, '\0', length); //set all values of plaintext to \0
	for (i = 0; i < length; i++){
		c = ciphertext[i]; //get ciphertext char
		k = key[i]; //get key char
		
		// Change ciphertext characters to a value between 0 and 26
		if (c == ' ')
			c = c-' ';  //change space character to 0
		else
			c = c - '@'; //subtract 64 to bring non-space characters to between 1 and 26
		
		// Change key characters to a value between 0 and 26
		if (k == ' ')
			k = k-' ';
		else
			k = k - '@';

		// Add ciphertext and key together and add 27 if result is negative
		// This will bring all the characters back into the 0-26 range
		p = c - k;	
		if (p < 0){
			p = p + 27;
		}

		// Change plaintext characters to either a space or a A-Z
		if (p == 0)
			p = p +' ';
		else
			p = p + '@';

		// Verify input, if invalid, set all of plaintext to \0 and return
		if ( p < 65 || p > 90){
			if (p != 32){
				memset(plaintext, '\0', length);
				return plaintext;
			}
		}
		plaintext[i] = p; //storing plaintext character
	}
	return plaintext;
}


/*
* The main function that listens for a connection, accepts up to 5 concurrent connections,
* verifies that they are from otp_dec, receives the ciphertext and key, encodes the ciphertext,
* and returns the plaintext to otp_dec.
* Parameters: 
* 	-int representing number of arguments passed
* 	-pointer to string representing the arguments
* Returns: 
* 	-int 1
*/
int main(int argc, char *argv[])
{
	// Variable declarations
	int listenSocketFD, establishedConnectionFD, portNumber, charsRead, charsWritten, i, lengthText;
	int amountReceived, amountSent;
	socklen_t sizeOfClientInfo;
	char buffer[256];
	char buffer2[1001];
	char cipherBuff[70000];
	char keyBuff[70000];
	char * plaintext;
	struct sockaddr_in serverAddress, clientAddress;
	pid_t pid;

	// Check usage & args
	if (argc < 2) { 
		fprintf(stderr,"USAGE: %s port\n", argv[0]); 
		exit(1); 
	} 

	// Set up the address struct for server to listen on
	memset((char *)&serverAddress, '\0', sizeof(serverAddress)); //clear out the address struct
	portNumber = atoi(argv[1]); //get the port number, convert to an integer from a string
	serverAddress.sin_family = AF_INET; //create a network-capable socket
	serverAddress.sin_port = htons(portNumber); //store the port number
	serverAddress.sin_addr.s_addr = inet_addr("127.0.0.1"); //restrict access to localhost

	// Set up the socket
	listenSocketFD = socket(AF_INET, SOCK_STREAM, 0); 
	if (listenSocketFD < 0) 
		error("otp_dec_d error: opening socket\n", 1);

	// Enable the socket to begin listening
	if (bind(listenSocketFD, (struct sockaddr *)&serverAddress, sizeof(serverAddress)) < 0) // Connect socket to port
		error("otp_dec_d error: on binding\n", 1);
	listen(listenSocketFD, 5); //flip the socket on to receive up to 5 connections

	// Look for connections until program is terminated
	while (1){
		// Accept a connection, blocking if one is not available until one connects
		sizeOfClientInfo = sizeof(clientAddress); //get the size of the address for the client that will connect
		establishedConnectionFD = accept(listenSocketFD, (struct sockaddr *)&clientAddress, &sizeOfClientInfo); // Accept
		if (establishedConnectionFD < 0) 
			error("otp_dec_d error: on accept\n", 1);
		
		//Create a fork so that the parent process can keep listening for new connections
		pid = fork();

		// CHILD PROCESS
		if (pid == 0){
			close(listenSocketFD); //close parent socket in child
			
			// Check that it is correct connection
			i = handshake(establishedConnectionFD);
			
			// If handshake was successful, wait for other responses
			if (i == 1){

				// Receive and store the amount of characters expected to be received
				memset(buffer, '\0', sizeof(buffer));
				charsRead = recv(establishedConnectionFD, buffer, sizeof(buffer)-1, 0);
				if (charsRead < 0) 
					error("otp_dec_d error: reading from socket expected length\n", 1);
				lengthText = atoi(buffer); //change string to integer to store expected length
				
				// Receive ciphertext in batches of 1000 characters until have received more than expected length
				// Loop until amount received is greater than the amount to be expected
				amountReceived = 0;
				memset(cipherBuff, '\0', sizeof(cipherBuff)); //clear cipherBuff
				while(amountReceived <= lengthText){
					memset(buffer2, '\0', sizeof(buffer2));
					charsRead = recv(establishedConnectionFD, buffer2, 1000, 0); // Read otp_dec's message from the socket
					if (charsRead < 0) 
						error("otp_dec_d error: reading from socket cipher\n", 1);
					
					// If all 1000 characters are received, send a success message back to otp_dec 
					// so it will continue to send information
					if (charsRead == 1000){
						memcpy(&cipherBuff[amountReceived], buffer2, 1000); //copy received data into ciphertext at next available point
						charsWritten = send(establishedConnectionFD, "textReceived", 13, 0);
						if (charsWritten < 0) 
							error("otp_dec_d error: writing to socket\n", 1);
						amountReceived +=1000; //increase amount received for while loop
					}
					// If not all characters received, send an error back to otp_dec
					else {
						charsWritten = send(establishedConnectionFD, "noText", 7, 0);
						if (charsWritten < 0) 
							error("otp_dec_d error: writing to socket\n", 1);
					}
					
				}

				// Receive key in batches of 1000 characters until have received more than length
				// Loop until amount received is greater than the amount to be expected
				amountReceived = 0;
				memset(keyBuff, '\0', sizeof(keyBuff)); //clear keyBuff
				while(amountReceived <= lengthText){
					memset(buffer2, '\0', sizeof(buffer2));
					charsRead = recv(establishedConnectionFD, buffer2, 1000, 0); // Read otp_dec's message from the socket
					if (charsRead < 0) 
						error("otp_dec_d error: reading from socket key\n", 1);
					// If all 1000 characters are received, send a success message back to otp_dec 
					// so it will continue to send information
					if (charsRead == 1000){
						memcpy(&keyBuff[amountReceived], buffer2, 1000); //copy received data into key at next available point
						charsWritten = send(establishedConnectionFD, "keytextReceived", 16, 0);
						if (charsWritten < 0) 
							error("otp_dec_d error: writing to socket\n", 1);
						amountReceived +=1000; //increase amount received for while loop
					}
					// If not all characters received, send an error back to otp_dec
					else{
						charsWritten = send(establishedConnectionFD, "noText", 7, 0);
						if (charsWritten < 0) 
							error("otp_dec_d error: writing to socket\n", 1);
					}
				}

				// Decrypt ciphertext
				plaintext = decrypt(cipherBuff, keyBuff, lengthText);

				// Send plaintext back if valid input
				if (plaintext[0] != '\0'){
					// Let otp_dec know if valid input was received and to expect the plaintext
					charsWritten = send(establishedConnectionFD, "ValidInput", 11, 0);
					if (charsWritten < 0) 
							error("otp_dec_d error: writing to socket\n", 1);
					if (charsWritten < 11)
							fprintf(stderr, "otp_dec_d warning: not all data written to socket!\n");
					
					// Send plaintext in batches of 1000
					// Loop until amount sent is greater than the length of the ciphertext
					amountSent = 0;
					while (amountSent <= lengthText){
						memset(buffer2, '\0', sizeof(buffer2));
						memcpy(buffer2, &plaintext[amountSent], 1000); ///copy next 1000 characters from plaintext to buffer2
						charsWritten = send(establishedConnectionFD, buffer2, 1000, 0);
						if (charsWritten < 0) 
							error("otp_dec_d error: writing to socket\n", 1);
						if (charsWritten < 1000)
							fprintf(stderr, "otp_dec_d warning: not all data written to socket!\n");
						
						// Check that otp_dec received the plaintext
						memset(buffer, '\0', sizeof(buffer));
						charsRead = recv(establishedConnectionFD, buffer, 19, 0); //read data
						if (charsRead < 0){
							error("otp_dec_d error: reading from socket cipher\n", 1);
						}
						if (strcmp(buffer, "plaintextReceived")==0){
							amountSent+=1000; //increase amount sent for while loop
						}
						else{
							error ("otp_dec_d error: sending plaintext to otp_dec\n", 1);
						}
					}
					
				}
				// If invalid input, send that to otp_dec
				else{
					charsWritten = send(establishedConnectionFD, "Invalid Input", 14, 0);
					if (charsWritten < 0) 
							error("otp_dec_d error: writing to socket\n", 1);
					if (charsWritten < 14)
							fprintf(stderr, "otp_dec_d warning: not all data written to socket!\n");
				}
				free(plaintext);
			}
			// Close socket in child
			close(establishedConnectionFD);
			exit(0);
		}
		else {	
			// PARENT PROCESS		
			wait(NULL); //remove child processes that have completed
			close(establishedConnectionFD); // Close the existing socket which is connected to the client
		}
	}
	close(listenSocketFD); // Close the listening socket
	return 0; 
}
