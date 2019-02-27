/*
**	Emily Hamilton (hamiltem)
**	CS344 - Spring 2018
**	Program 4
*/

/*
**	This file contains the implementation code for the various utility
**	functions declared in "utilities.h".
*/


#include "utilities.h"

// Prints error message and exits
void hardError(const char* msg, const int exitStatus) {
	fprintf(stderr, "%s\n", msg);
	exit(exitStatus);
}

// Prints error message and DOESN'T exit
void softError(const char* msg) {
	fprintf(stderr, "%s\n", msg);
}

// Creates fileContents struct on the heap and reads chars
// from filename into it. Returns pointer to fileContents
// struct when done. Parameter codeMode should be 'E' for
// a message that needs to be encoded, 'D' for one that
// needs to be decoded, and 'K' for a key
struct fileContents* readFromFile(const char* filename, char codeMode) {
	FILE* fp;
	char c;
	struct fileContents *fc;

	// Make space for fileContents struct & initialize it
	fc = malloc(sizeof(struct fileContents));
	initFileContents(fc, 256);

	fp = fopen(filename, "r");
	if (fp == NULL) {
		freeFileContents(fc);
		free(fc);
		fprintf(stderr, "%s: ", filename);
		hardError("Unable to open file.", 1);
	}

	// 'E' passed in for encode; 'D' passed in for decode; 
	// 'K' passed in for key
	insertFileContents(fc, codeMode);
	insertFileContents(fc, '_');

	// Read each char from file until EOF and insert it into
	// fileContents struct. Not checking for validity of input
	// at this stage.
	while ((c = fgetc(fp)) != EOF) {
		// Check for invalid chars
		if ((int)c < 65 || (int)c > 90) {
			if ((int)c != 32 && (int)c != 10) {
				//fprintf(stderr, "Unexpected ASCII char code in message: %d\n", (int)c);
				// Error & Quit
				freeFileContents(fc);
				free(fc);
				fclose(fp);
				hardError("readFromFile(): Error! This file contains invalid characters.", 1);
			}
		}
		insertFileContents(fc, c);
	}
	// Replace trailing newline with first $ sentinel
	fc->message[fc->length - 1] = '$';
	// Add second $ sentinel
	insertFileContents(fc, '$');
	// Add trailing null terminator
	insertFileContents(fc, '\0');

	fclose(fp);

	return fc;
}

/* File contents dynamic string array methods */

void initFileContents(struct fileContents *fc, int startingSize) {
	fc->message = (char *)malloc(sizeof(char) * startingSize);
	if (fc->message == NULL) {
		hardError("initFileContents(): Unable to allocate memory.", 1);
	}
	fc->length = 0;
	fc->size = startingSize;
}

void freeFileContents(struct fileContents *fc) {
	free(fc->message);
	fc->message = NULL;
	fc->length = 0;
	fc->size = 0;
}

void insertFileContents(struct fileContents *fc, char c) {
	if (fc->length == fc->size) {
		fc->size *= 2;
		fc->message = (char *)realloc(fc->message, (sizeof(char) * fc->size));
		if (fc->message == NULL) {
			hardError("insertFileContents(): Unable to reallocate memory.", 1);
		}
	}
	fc->message[fc->length] = c;
	(fc->length)++;
}
