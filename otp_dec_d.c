/*
**	Emily Hamilton (hamiltem)
**	CS344 - Operating Systems I (Spring 2018)
**	Oregon State University
**	Program 4 - "OTP - One Time Pad"
*/

/*
**	"otp_dec_d.c"
** 	Usage:	otp_dec [open port]
**	otp_dec_d is a daemon for otp_dec clients. When a client connects to its
** 	port, otp_dec_d accepts a message and checks that the message code equals
**	"D". If the code is incorrect, it returns the message "FAIL" and goes back
**	to listening for connections; if it matches, it returns the message 
**	"SUCCESS" and accepts another message for the client to send a key. When
**	it receives the key, it uses it to decrypt the first message and sends
**	the results to the client.
**	otp_dec_d can handle concurrent connections.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <sys/wait.h>
#include <netinet/in.h>

#include "encryption.h"
#include "utilities.h"

#define MAX_MESSAGE 70100

int main(int argc, char *argv[])
{
	int listenSocketFD, establishedConnectionFD, portNumber, charsRead, pid;
	socklen_t sizeOfClientInfo;
	char buffer[256];
	struct sockaddr_in serverAddress, clientAddress;

	if (argc < 2) { 
		fprintf(stderr,"USAGE: %s [port]\n", argv[0]); 
		exit(1); // Check usage & args
	}

	// Set up the address struct for this process (the server)
	memset((char *)&serverAddress, '\0', sizeof(serverAddress)); // Clear out the address struct
	portNumber = atoi(argv[1]); // Get the port number, convert to an integer from a string
	serverAddress.sin_family = AF_INET; // Create a network-capable socket
	serverAddress.sin_port = htons(portNumber); // Store the port number
	serverAddress.sin_addr.s_addr = INADDR_ANY; // Any address is allowed for connection to this process

	// Set up the socket
	listenSocketFD = socket(AF_INET, SOCK_STREAM, 0); // Create the socket
	if (listenSocketFD < 0) hardError("ERROR opening socket", 1);	//Error & exit


	// Enable the socket to begin listening
	if (bind(listenSocketFD, (struct sockaddr *)&serverAddress, sizeof(serverAddress)) < 0) // Connect socket to port
		hardError("ERROR on binding", 1);

	// Start listening!
	listen(listenSocketFD, 5); // Flip the socket on - it can now receive up to 5 connections

	// Continuously accept new connections...
	while (1) {
		char* completeMessage, *keyMessage, *decryptedMsg;
		int totalCharsRead, err, status, termChild;

		// Attempt to wait for any existing child processes
		while((termChild = waitpid(-1, &status, WNOHANG)) > 0) {
			//printf("Terminated child: %d\n", termChild);
		}

		// Accept a connection, blocking if one is not available until one connects
		sizeOfClientInfo = sizeof(clientAddress); // Get the size of the address for the client that will connect
		establishedConnectionFD = accept(listenSocketFD, (struct sockaddr *)&clientAddress, &sizeOfClientInfo); // Accept
		if (establishedConnectionFD < 0) softError("ERROR on accept");

		// Fork child process to deal with encryption
		pid = fork();

		switch(pid) {
			case -1:	// fork() error
						fprintf(stderr, "Error: Couldn't fork new process./n");
						break;
			case 0:		// This is the child process.
						// Get the message from the client and display it
								
						completeMessage = malloc(sizeof(char) * MAX_MESSAGE);
						totalCharsRead = 0;
						err = 0;
						while (strstr(completeMessage, "$$") == NULL) {
							memset(buffer, '\0', 256);
							charsRead = recv(establishedConnectionFD, buffer, 255, 0); // Read the client's message from the socket
							if (charsRead < 0) softError("ERROR reading from socket");
							totalCharsRead += charsRead;
							if (totalCharsRead < MAX_MESSAGE) {
								strcat(completeMessage, buffer);
							}
							else {
								softError("SERVER: Had to stop reading from client; reached maximum message length.");
								err = 1;
								break;
							}
						}
						//printf("SERVER: I received this MESSAGE from the client: \"%s\"\n", completeMessage);
						if (completeMessage[0] != 'D') {
							fprintf(stderr, "SERVER: Wrong message code: %c\n", completeMessage[0]);
							err = 1;
						}
						// Send message back to the client
						if (err == 0) {
							charsRead = send(establishedConnectionFD, "SUCCESS", 7, 0);
						}
						else {
							charsRead = send(establishedConnectionFD, "FAIL", 4, 0);
						}
						if (charsRead < 0) softError("ERROR writing to socket");


						// If previous read was successful, get the key from the 
						// client and display it
						if (err == 0) {
							keyMessage = malloc(sizeof(char) * MAX_MESSAGE);
							totalCharsRead = 0;
							err = 0;
							while (strstr(keyMessage, "$$") == NULL) {
								memset(buffer, '\0', 256);
								charsRead = recv(establishedConnectionFD, buffer, 255, 0); // Read the client's message from the socket
								if (charsRead < 0) softError("ERROR reading from socket");
								totalCharsRead += charsRead;
								if (totalCharsRead < MAX_MESSAGE) {
									strcat(keyMessage, buffer);
								}
								else {
									softError("SERVER: Had to stop reading from client; reached maximum message length.");
									err = 1;
									break;
								}
							}
							//printf("SERVER: I received this KEY from the client: \"%s\"\n", keyMessage);

							// Send message back to the client
							if (err == 0) {
								// Encrypt and send back
								decryptedMsg = malloc(sizeof(char) * MAX_MESSAGE);
								memset(decryptedMsg, '\0', MAX_MESSAGE);
								decrypt(completeMessage, keyMessage, decryptedMsg, strlen(completeMessage));
								strcat(decryptedMsg, "$$");
								//printf("\n\nSERVER: Encrypted Message: <%s>\n\n", decryptedMsg);

								charsRead = send(establishedConnectionFD, decryptedMsg, strlen(decryptedMsg), 0);
								if (charsRead < strlen(decryptedMsg)) softError("SERVER: WARNING: Not all data written to socket!");
							}
							else {
								charsRead = send(establishedConnectionFD, "FAIL$$", 6, 0);
							}
							if (charsRead < 0) softError("ERROR writing to socket");
						}
						close(establishedConnectionFD);
						exit(0);
						break;	//Should never be encountered, but whatever. I'm paranoid.
			default:	// This is the parent process
						// Check again for any child processes that need to be snuffed out.
						while((termChild = waitpid(-1, &status, WNOHANG)) > 0) {
							//printf("Terminated child: %d\n", termChild);
						}			
						close(establishedConnectionFD);
		}
	}

	close(listenSocketFD); // Close the listening socket

	return 0; 
}
