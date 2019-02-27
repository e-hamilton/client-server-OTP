/*
**	Emily Hamilton (hamiltem)
**	CS344 - Spring 2018
**	Program 4
*/

/*
**	This file contains the declarations of the various utility
**	functions defined in "utilities.c". Full descriptions of the
**	functions are included above each function definition in
**	"utilities.c".
*/

#ifndef UTILITIES_H
#define UTILITIES_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// fileContents is a struct used to implement a data structure similar
// to a dynamic array used for reading strings from a file
struct fileContents {
	char* message;
	int length;
	int size;
};

void hardError(const char* msg, const int exitStatus);

void softError(const char* msg);

struct fileContents* readFromFile(const char* filename, char codeMode);

/* File contents dynamic string array methods */
void initFileContents(struct fileContents *fc, int startingSize);
void freeFileContents(struct fileContents *fc);
void insertFileContents(struct fileContents *fc, char c);

#endif	// UTILITIES_H
