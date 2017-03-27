/*
* Author: Clare Brown
* Date: 12/1/16
* Description: A program that receives plaintext and a key from another program
* through sockets, encrypts the plaintext into ciphertext, and sends it back
* to the program that sent the plaintext and key.  
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
* A function to verify that the incoming connection is from otp_enc. If the
* socket receives the string "enc" then it is coming from otp_enc. If the 
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
		error("otp_enc_d error: reading from socket", 1);
	if (strcmp(buffer, "enc") == 0){
		// Send a success message back to otp_enc
		charsRead = send(socket, "yes", 4, 0); 
		if (charsRead < 0) 
			error("otp_enc_d error: writing to socket", 1);
		return 1;
	}
	else{
		// Send a failure message back to client
		charsRead = send(socket, "no", 3, 0); 
		if (charsRead < 0) 
			error("otp_enc_d error: writing to socket", 1);
		return 0;
	}
}


/*
* A function to encrypt the plaintext into ciphertext using the key. Sets the 
* plaintext and key characters to a number between 0 and 26 inclusive with space
* being zero and A = 1. Adds those numbers together and finds the remainder 
* when divided by 27. Then changes those numbers back to characters from A-Z and space. 
* Parameters: 
* 	-pointer to a string representing the plaintext
* 	-pointer to a string representing the key
* 	-int representing the length of the plaintext
* Returns: 
* 	-int indicating if true or false, 1 for true, 0 for false
*/
char * encrypt(char * plaintext, char * key, int length){
	char p, k, c;
	int i;
	char * ciphertext = malloc(length*sizeof(char)); //create room for the ciphertext
	memset(ciphertext, '\0', length); //set all values of ciphertext to \0
	
	// Loop through all characters in the plaintext
	for (i = 0; i < length; i++){
		p = plaintext[i]; //get plaintext char
		k = key[i];	//get key char

		// Change plaintext characters to a value between 0 and 26
		if (p == ' ')
			p = p-' '; //change space character to 0
		else
			p = p - '@'; //subtract 64 to bring non-space characters to between 1 and 26
		
		// Change key characters to a value between 0 and 26
		if (k == ' ')
			k = k-' ';
		else
			k = k - '@';

		// Add plaintext and key together and mod 27 the result
		c = p + k;
		c = c % 27;

		// Changeciphertext characters to either a space or a A-Z
		if (c == 0)
			c = c +' ';
		else
			c = c + '@';

		// Verify input, if invalid, set all of ciphertext to \0 and return
		if ( c < 65 || c > 90){
			if (c != 32){
				memset(ciphertext, '\0', length);
				return ciphertext;
			}
		}
		ciphertext[i] = c; //store ciphertext character	
	}
	return ciphertext;
}


/*
* The main function that listens for a connection, accepts up to 5 concurrent connections,
* verifies that they are from otp_enc, receives the plaintext and key, encodes the plaintext,
* and returns the ciphertext to otp_enc.
* Parameters: 
* 	-int representing number of arguments passed
* 	-pointer to string representing the arguments
* Returns: 
* 	-int 1
*/
int main(int argc, char *argv[])
{
	// Variable Declarations
	int listenSocketFD, establishedConnectionFD, portNumber, charsRead, charsWritten, i, lengthText;
	int amountReceived, amountSent;
	socklen_t sizeOfClientInfo;
	char buffer[256];
	char buffer2[1001];
	char plaintextBuff[70000];
	char keyBuff[70000];
	char * ciphertext;
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
		error("otp_enc_d error: opening socket\n", 1);

	// Enable the socket to begin listening
	if (bind(listenSocketFD, (struct sockaddr *)&serverAddress, sizeof(serverAddress)) < 0) //connect socket to port
		error("otp_enc_d error: on binding\n", 1);
	listen(listenSocketFD, 5); //flip the socket on to receive up to 5 connections

	// Look for connections until program is terminated
	while (1){
		// Accept a connection, blocking if one is not available until one connects
		sizeOfClientInfo = sizeof(clientAddress); //get the size of the address for the client that will connect
		establishedConnectionFD = accept(listenSocketFD, (struct sockaddr *)&clientAddress, &sizeOfClientInfo); //accept connection
		if (establishedConnectionFD < 0) 
			error("otp_enc_d error: on accept\n", 1);

		//Create a fork so that parent process can keep listening for new connections
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
					error("otp_enc_d error: reading from socket expected length\n", 1);
				lengthText = atoi(buffer); //change string to integer to store expected length
				
				// Receive plaintext in batches of 1000 characters until have received more than expected length
				// Loop until amount received is greater than the amount to be expected
				amountReceived = 0;
				memset(plaintextBuff, '\0', sizeof(plaintextBuff)); //clear plaintextBuff
				while(amountReceived <= lengthText){
					memset(buffer2, '\0', sizeof(buffer2));
					charsRead = recv(establishedConnectionFD, buffer2, 1000, 0); //read otp_enc's message from the socket
					if (charsRead < 0) 
						error("otp_enc_d error: reading from socket plaintext\n", 1);

					// If all 1000 characters are received, send a success message back to otp_enc 
					// so it will continue to send information
					if (charsRead == 1000){
						memcpy(&plaintextBuff[amountReceived], buffer2, 1000); //copy received data into plaintext at next available point
						charsWritten = send(establishedConnectionFD, "textReceived", 13, 0);
						if (charsWritten < 0) 
							error("otp_enc_d error: writing to socket\n", 1);
						amountReceived +=1000; //increase amount received for while loop
					}
					// If not all characters received, send an error back to otp_enc
					else {
						charsWritten = send(establishedConnectionFD, "noText", 7, 0);
						if (charsWritten < 0) 
							error("otp_enc_d error: writing to socket\n", 1);
					}
					
				}

				// Receive key in batches of 1000 characters until have received more than length
				// Loop until amount received is greater than the amount to be expected
				amountReceived = 0;
				memset(keyBuff, '\0', sizeof(keyBuff)); //clear keyBuff
				while(amountReceived <= lengthText){
					memset(buffer2, '\0', sizeof(buffer2));
					charsRead = recv(establishedConnectionFD, buffer2, 1000, 0); // Read otp_enc's message from the socket
					if (charsRead < 0) 
						error("otp_enc_d error: reading from socket key\n", 1);
					// If all 1000 characters are received, send a success message back to otp_enc 
					// so it will continue to send information
					if (charsRead == 1000){
						memcpy(&keyBuff[amountReceived], buffer2, 1000); //copy received data into key next available point
						charsWritten = send(establishedConnectionFD, "keytextReceived", 16, 0);
						if (charsWritten < 0) 
							error("otp_enc_d error: writing to socket\n", 1);
						amountReceived +=1000;  //increase amount received for while loop
					}
					// If not all characters received, send an error back to otp_enc
					else{
						charsWritten = send(establishedConnectionFD, "noText", 7, 0);
						if (charsWritten < 0) 
							error("otp_enc_d error: writing to socket\n", 1);
					}

				}

				// Encrypt plaintext
				ciphertext = encrypt(plaintextBuff, keyBuff, lengthText);

				// Send ciphertext back if valid input
				if (ciphertext[0] != '\0'){
					// Let otp_enc know if valid input was received and to expect the ciphertext
					charsWritten = send(establishedConnectionFD, "ValidInput", 11, 0);
					if (charsWritten < 0) 
							error("otp_enc_d error: writing to socket\n", 1);
					if (charsWritten < 11)
							fprintf(stderr, "otp_enc_d warning: not all data written to socket!\n");
					
					// Send ciphertext in batches of 1000
					// Loop until amount sent is greater than the length of the ciphertext
					amountSent = 0;
					while (amountSent <= lengthText){
						memset(buffer2, '\0', sizeof(buffer2));
						memcpy(buffer2, &ciphertext[amountSent], 1000); //copy next 1000 characters from ciphertext to buffer2
						charsWritten = send(establishedConnectionFD, buffer2, 1000, 0);
						if (charsWritten < 0) 
							error("otp_enc_d error: writing to socket\n", 1);
						if (charsWritten < 1000)
							fprintf(stderr, "otp_enc_d warning: not all data written to socket!\n");
						
						// Check that otp_enc received the ciphertext
						memset(buffer, '\0', sizeof(buffer));
						charsRead = recv(establishedConnectionFD, buffer, 19, 0); //read data
						if (charsRead < 0){
							error("otp_enc_d error: reading from socket cipher\n", 1);
						}
						if (strcmp(buffer, "ciphertextReceived")==0){
							amountSent+=1000; //increase amount sent for while loop
						}
						else{
							error ("otp_enc_d error: sending ciphertext to otp_enc\n", 1);
						}
					}
					
				}
				// If invalid input, send that to otp_enc
				else{
					charsWritten = send(establishedConnectionFD, "Invalid Input", 14, 0);
					if (charsWritten < 0) 
							error("otp_enc_d error: writing to socket\n", 1);
					if (charsWritten < 14)
							fprintf(stderr, "otp_enc_d warning: not all data written to socket!\n");
				}
				free(ciphertext);
			}
			// Close socket in child
			close(establishedConnectionFD);
			exit(0);
		}
		else {	
			// PARENT PROCESS		
			wait(NULL); //remove child processes that have completed
			close(establishedConnectionFD); //close the existing socket which is connected to the otp_enc
		}
	}
	close(listenSocketFD); //close the listening socket
	return 0; 
}
