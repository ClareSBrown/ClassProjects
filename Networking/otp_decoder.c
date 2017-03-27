/*
* Author: Clare Brown
* Date: 12/1/16
* Description: A program that sends ciphertext and a key to another program
* through sockets and receives the decrypted ciphertext back in the form
* of plaintext or a message stating that the input is invalid   
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 


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
* The main function that sets up a connection, waits for it to be verfied, sends
* the ciphertext and key data to be encrypted, and receives the plaintext back
* Parameters: 
* 	-int representing number of arguments passed
* 	-pointer to string representing the arguments
* Returns: 
* 	-int 1
*/
int main(int argc, char *argv[])
{
	// Variable Declarations
	int socketFD, portNumber, charsWritten, charsRead, i, endKey, endCipher, amountSent, amountReceived;
	struct sockaddr_in serverAddress;
	struct hostent* serverHostInfo;
	char buffer[256];
	char buffer2[1001];
	char plaintextBuff[70000];
	char keyBuff[70000];
	char cipherBuff[70000];
	
	FILE * f1, *f2; 

    // Check usage & args
	if (argc < 4) { 
		fprintf(stderr,"USAGE: %s ciphertext key port\n", argv[0]); 
		exit(1); 
	} 

	// Set up the server address struct
	memset((char*)&serverAddress, '\0', sizeof(serverAddress)); //clear out the address struct
	portNumber = atoi(argv[3]); //get the port number, convert to an integer from a string
	serverAddress.sin_family = AF_INET; //create a network-capable socket
	serverAddress.sin_port = htons(portNumber); //store the port number
	serverHostInfo = gethostbyname("localhost"); //convert the machine name into a special form of address
	if (serverHostInfo == NULL) { 
		error("otp_dec error: no such host\n", 1); 
	}
	memcpy((char*)&serverAddress.sin_addr.s_addr, (char*)serverHostInfo->h_addr, serverHostInfo->h_length); // Copy in the address

	// Set up the socket
	socketFD = socket(AF_INET, SOCK_STREAM, 0); 
	if (socketFD < 0) 
		error("otp_dec error: opening socket\n", 1);
	
	// Connect to server
	if (connect(socketFD, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) < 0){ // Connect socket to address
		fprintf(stderr, "otp_dec error: connecting to port %d\n", portNumber);
		exit(1);
	}

	// Send "dec" to server to verify that it is the correct server
	charsWritten = send(socketFD, "dec", 4, 0); // Write to the server
	if (charsWritten < 0) 
		error("otp_dec error: writing to socket\n", 1);
	if (charsWritten < 4) 
		fprintf(stderr, "otp_dec warning: Not all data written to socket!\n");

	// Get return message from server
	memset(buffer, '\0', sizeof(buffer)); // Clear out the buffer again for reuse
	charsRead = recv(socketFD, buffer, sizeof(buffer) - 1, 0); // Read data from the socket, leaving \0 at end
	if (charsRead < 0) 
		error("otp_dec error: reading from socket\n", 1);
	
	// If the message received was yes, it is the correct server 
	// Send the plaintext and key
	if (strcmp(buffer, "yes") == 0){
		
		// Read ciphertext from file
		memset(cipherBuff, '\0', sizeof(cipherBuff));
		f1 = fopen(argv[1], "r");
		if (f1 == NULL){
			error("otp_dec error: opening ciphertext file\n", 1);
		}
		else{
			fgets(cipherBuff, sizeof(cipherBuff), f1);
		}

		// Get the length of the ciphertext
		endCipher = strlen(cipherBuff)-1;
		fclose(f1);

		// Read key from file
		memset(keyBuff, '\0', sizeof(keyBuff));
		f1 = fopen(argv[2], "r");
		if (f1 == NULL){
			error("otp_dec error: opening key file\n", 1);
		}
		else{
			fgets(keyBuff, sizeof(keyBuff), f1);
		}

		// Get length of the key
		endKey = strlen(keyBuff)-1;;
		fclose(f1);

		// Verfiy that the key is long enough
		if (endKey < endCipher){
			fprintf(stderr, "otp_dec error: key '%s' is too short\n", argv[2]);
		}
		else{
			// Send the number of expected characters to the server
			memset(buffer, '\0', sizeof(buffer));
			i = snprintf(buffer, sizeof(buffer), "%d", endCipher);

			charsWritten = send(socketFD, buffer, i , 0);
			if (charsWritten < 0) 
				error("otp_dec error: writing to socket\n", 1);
			if (charsWritten < i)
				fprintf(stderr, "otp_dec warning: not all data written to socket!\n");

			// Send the ciphertext in batches of 1000 characters
			// Loop until amount sent is greater than the amount to be sent
			amountSent = 0;
			while (amountSent <= endCipher){
				memset(buffer2, '\0', sizeof(buffer2));
				memcpy(buffer2, &cipherBuff[amountSent], 1000);  //copy next 1000 characters from ciphertext to buffer2
				charsWritten = send(socketFD, buffer2, 1000, 0);
				if (charsWritten < 0) 
					error("otp_dec error: writing to socket\n", 1);
				if (charsWritten < 1000)
					fprintf(stderr, "otp_dec warning: not all data written to socket!\n");
				
				// Check that otp_dec_d received the ciphertext
				memset(buffer, '\0', sizeof(buffer));
				charsRead = recv(socketFD, buffer, 13, 0); //read data
				if (charsRead < 0){
					error("otp_dec error: reading from socket ciphertext\n", 1);
				}
				if (strcmp(buffer, "textReceived")==0){
					amountSent+=1000;
				}
				else
					error ("otp_dec error: sending ciphertext to otp_dec_d\n", 1);
			}

			// Send the key in batches of 1000 characters
			// Loop until amount sent is greater than the amount to be sent
			amountSent = 0;
			while (amountSent <= endCipher){
				memset(buffer2, '\0', sizeof(buffer2));
				memcpy(buffer2, &keyBuff[amountSent], 1000);   //copy next 1000 characters from key to buffer2
				charsWritten = send(socketFD, buffer2, 1000, 0);
				if (charsWritten < 0) 
					error("otp_dec error: writing to socket\n", 1);
				if (charsWritten < 1000)
					fprintf(stderr, "otp_dec warning: not all data written to socket!\n");
				
				// Check that otp_dec_d received the key
				memset(buffer, '\0', sizeof(buffer));
				charsRead = recv(socketFD, buffer, 16, 0); //read data
				if (charsRead < 0){
					error("otp_dec error: reading from socket key\n", 1);
				}
				if (strcmp(buffer, "keytextReceived")==0){
					amountSent+=1000; //increase amount sent for while loop
				}
				else
					error ("otp_dec error: sending key to otp_dec_d\n", 1);
			}
			
			// Check if input was valid
			memset(buffer, '\0', sizeof(buffer)); //clear out the buffer again for reuse
			charsRead = recv(socketFD, buffer, 11, 0); //read data from the socket, leaving \0 at end
			if (charsRead < 0) 
				error("otp_dec error: reading from socket\n", 1);
			// If it is valid, receive the plaintext
			if (strcmp(buffer, "ValidInput") == 0){
				// Receive plaintext in batches of 1000
				amountReceived = 0;
				memset(plaintextBuff, '\0', sizeof(plaintextBuff)); //clear plaintextBuffer
				
				while(amountReceived <= endCipher){
					memset(buffer2, '\0', sizeof(buffer2));
					charsRead = recv(socketFD, buffer2, 1000, 0); //read otp_dec_d's message from the socket
					if (charsRead < 0) 
						error("otp_dec error: reading from socket plaintext\n", 1);
					// Checking if all 1000 characters were received
					if (charsRead == 1000){
						memcpy(&plaintextBuff[amountReceived], buffer2, 1000); //copy received data into plaintext at the next available spot
						
						// Send confirmation back to otp_dec_d to indicate text has been received
						charsWritten = send(socketFD, "plaintextReceived", 19, 0);
						if (charsWritten < 0) 
							error("otp_dec error: writing to socket\n", 1);
						amountReceived +=1000;
					}
					else {
						charsWritten = send(socketFD, "noText", 7, 0);
						if (charsWritten < 0) 
							error("otp_dec error: writing to socket\n", 1);
					}

				}
				fprintf(stdout, "%s\n", plaintextBuff); //print plaintext results
				
			}
			// If invalid input, print error and exit
			else{
				fprintf(stderr, "otp_dec error: %s contains bad characters\n", argv[1]);
				exit(1);
			}

		}
	}
	// If not correct port for otp_dec_d, print error and exit
	else{
		fprintf(stderr, "otp_dec error: cannot contact otp_dec_d on port %d\n", portNumber);
		exit(2);
	}
	close(socketFD); // Close the socket
	return 0;
}
