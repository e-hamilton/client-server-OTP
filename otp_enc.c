/*
**	Emily Hamilton (hamiltem)
**	CS344 - Operating Systems I (Spring 2018)
**	Oregon State University
**	Program 4 - "OTP - One Time Pad"
*/

/*
**	"otp_enc.c"
** 	Usage:	otp_enc [plain text file] [key file] [otp_enc_d's port]
**	Sends contents of plain text file and key file to otp_enc_d
** 	(daemon), which returns an encrypted message. otp_enc then prints
** 	out the results to stdout.
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

#include "encryption.h"
#include "utilities.h"

#define h_addr h_addr_list[0]
#define MAX_MESSAGE 70100

int main(int argc, char *argv[])
{
	int socketFD, portNumber, charsWritten, charsRead;
	struct sockaddr_in serverAddress;
	struct hostent* serverHostInfo;
	char* textfile;
	char* keyfile;
	char buffer[256];
    
    // Reject incorrect number of args
	if (argc < 4) { 
		fprintf(stderr,"USAGE: %s [plaintext] [key] [port]\n", argv[0]); 
		exit(1);
	}

	// Record arguments
	textfile = argv[1];
	keyfile = argv[2];

	// Set up the server address struct
	memset((char*)&serverAddress, '\0', sizeof(serverAddress)); // Clear out the address struct
	portNumber = atoi(argv[3]); // Get the port number, convert to an integer from a string
	serverAddress.sin_family = AF_INET; // Create a network-capable socket
	serverAddress.sin_port = htons(portNumber); // Store the port number
	serverHostInfo = gethostbyname("localhost"); // Convert the machine name into a special form of address
	memcpy((char*)&serverAddress.sin_addr.s_addr, (char*)serverHostInfo->h_addr, serverHostInfo->h_length); // Copy in the address

	// Set up the socket
	socketFD = socket(AF_INET, SOCK_STREAM, 0); // Create the socket
	if (socketFD < 0) hardError("CLIENT: ERROR opening socket", 1);
	
	// Connect to server
	if (connect(socketFD, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) < 0) // Connect socket to address
		hardError("CLIENT: ERROR connecting", 1);

	// Get message from arg[1] and key from arg[2]
	memset(buffer, '\0', sizeof(buffer)); // Clear out the buffer array
	struct fileContents* textContents;
	struct fileContents* keyContents;
	textContents = readFromFile(textfile, 'E');		// Message code 'E' = encrypt
	keyContents = readFromFile(keyfile, 'K');		// Message code 'K' = key

	// Verify key is at least as long as plaintext
	if (keyContents->length < textContents->length) {
		//Error!
			freeFileContents(textContents);
			freeFileContents(keyContents);
			free(textContents);
			free(keyContents);
			fprintf(stderr, "ERROR: Key \"%s\" is too short to use with \"%s\".\n", keyfile, textfile);
			exit(1);
	}

	// Send message to server
	charsWritten = send(socketFD, textContents->message, strlen(textContents->message), 0); // Write to the server
	if (charsWritten < 0) hardError("CLIENT: ERROR writing to socket", 1);
	if (charsWritten < strlen(buffer)) softError("CLIENT: WARNING: Not all data written to socket!\n");

	// Get return message from server
	memset(buffer, '\0', sizeof(buffer)); // Clear out the buffer again for reuse
	charsRead = recv(socketFD, buffer, sizeof(buffer) - 1, 0); // Read data from the socket, leaving \0 at end
	if (charsRead < 0) hardError("CLIENT: ERROR reading from socket", 1);

	if (strcmp(buffer, "SUCCESS") == 0) {
		//printf("CLIENT: I received a success message from the server: <%s>\n", buffer);

		// Send key to server ONLY if server replied successfully
		charsWritten = send(socketFD, keyContents->message, strlen(keyContents->message), 0); // Write to the server
		if (charsWritten < 0) hardError("CLIENT: ERROR writing to socket", 1);
		if (charsWritten < strlen(buffer)) softError("CLIENT: WARNING: Not all data written to socket!\n");

		// Get return message from server
		char* completeMessage = malloc(sizeof(char) * MAX_MESSAGE);
		memset(completeMessage, '\0', MAX_MESSAGE);
		int totalCharsRead = 0;
		while (strstr(completeMessage, "$$") == NULL) {
			memset(buffer, '\0', sizeof(buffer)); // Clear out the buffer again for reuse
			charsRead = recv(socketFD, buffer, sizeof(buffer) - 1, 0); // Read data from the socket, leaving \0 at end
			if (charsRead < 0) hardError("CLIENT: ERROR reading from socket", 1);
			totalCharsRead += charsRead;
			if (totalCharsRead < MAX_MESSAGE) {
				strcat(completeMessage, buffer);	// tack on contents of buffer
			}
			else {
				softError("CLIENT: Had to stop reading from server; reached maximum message length.");
				break;
			}
		}
		if (strcmp(completeMessage, "FAIL$$") != 0) {		// If it did NOT fail...
			// Remove sentinel "$$"
			completeMessage[strlen(completeMessage) - 1] = '\0';
			completeMessage[strlen(completeMessage) - 1] = '\0';
			//printf("CLIENT: I received a success message from the server: <%s>\n", completeMessage);
			printf("%s\n", completeMessage);
		}
		else {
			fprintf(stderr, "Error: Could not contact otp_enc_d on port %d\n", argv[3]);
			freeFileContents(textContents);
			freeFileContents(keyContents);
			free(textContents);
			free(keyContents);
			close(socketFD);
			exit(2);
		}
	}
	else {
		fprintf(stderr, "Error: Could not contact otp_enc_d on port %d\n", argv[3]);
		freeFileContents(textContents);
		freeFileContents(keyContents);
		free(textContents);
		free(keyContents);
		close(socketFD);
		exit(2);
	}

	freeFileContents(textContents);
	freeFileContents(keyContents);
	free(textContents);
	free(keyContents);
	close(socketFD); // Close the socket
	return 0;
}