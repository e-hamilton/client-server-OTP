/*
**	Emily Hamilton (hamiltem)
**	CS344 - Operating Systems I (Spring 2018)
**	Oregon State University
**	Program 4 - "OTP - One Time Pad"
*/

/*
**	This file contains the declarations of the encryption-related
**	functions defined in "encryption.c". Full descriptions of the
**	functions are included above each function definition in
**	"encryption.c".
*/

#ifndef ENCRYPTION_H
#define ENCRYPTION_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void stripNewlines(char arr[]);

int hasInvalidChars(char arr[]);

void _translateASCIItoCode(char in[], int out[], int n);

void _translateCodetoASCII(int in[], char out[], int n);

int _addMOD(int crypt, int key);

int _subMOD(int crypt, int key);

void encrypt(char original[], char key[], char output[], int n);

void decrypt(char original[], char key[], char output[], int n);

#endif	// ENCRYPTION_H
