/*
**	Emily Hamilton (hamiltem)
**	CS344 - Operating Systems I (Spring 2018)
**	Oregon State University
**	Program 4 - "OTP - One Time Pad"
*/

/*
**	This file contains the implementation code for the encryption-related
**	functions declared in "encryption.h".
*/

#include "encryption.h"


// stripNewlines() replaces all newline characters in arr[] with null characters
void stripNewlines(char arr[]) {
	int len = strlen(arr);
	int i;

	for (i = 0; i < len; i++) {
		if (arr[i] == '\n') {
			arr[i] = '\0';
		}
	}
}


// hasInvalidChars() returns 0 if all characters in the array of characters are between
// A-Z or is a space; it returns 1 if it contains any other characters.
int hasInvalidChars(char arr[]) {
	int len = strlen(arr);
	int i, c;
	
	for (i = 0; i < len; i++) {
		c = (int)arr[i];
		// If c is not a space
		if (c != 32) {
			// If c is less than A or greater than Z
			if (c < 65 || c > 90) {
				return 1;
			}
		}
	}
	return 0;
}


// _translateASCIItoCode() turns an array of n chars (in[]) into an array of ints (out[])
// where A=0, B=1, ..., Z=25, SPACE=26 and saves the result to out[].
void _translateASCIItoCode(char in[], int out[], int n) {
	
	char input;
	int output;
	int i;

	for (i = 0; i < n; i++) {
		input = in[i];
		output = (int)input;
		
		// If space, assign code of 26
		if (output == 32) {
			output = 26;
		}
		// Else, subtract 65
		else {
			output -= 65;
		}

		out[i] = output;
	}
	
}

// _translateCodetoASCII() turns an array of n ints (in[]) where A=0, B=1, ..., Z=25,
// SPACE=26 to an array of chars with regular ASCII values and saves the result to out[].
void _translateCodetoASCII(int in[], char out[], int n) {

	int input;
	char output;
	int i;

	for (i = 0; i < n; i++) {
		input = in[i];
		
		// If space (26), assign ascii value of 32
		if (input == 26) {
			input = 32;
		}
		// Else, add 65
		else {
			input += 65;
		}

		output = (char)input;
		out[i] = output;
	}
}


// _addMOD() adds two integers with a modulo of 27 and returns the result
int _addMOD(int crypt, int key) {
	int modSum;

	modSum = crypt + key;
	if (modSum > 26) {
		modSum -= 27;
	}

	return modSum;
}

// _subMOD() subtracts integer key from crypt with a modulo of 27 and returns the result 
int _subMOD(int crypt, int key) {
	int modDiff;

	modDiff = crypt - key;
	if (modDiff < 0) {
		modDiff += 27;
	}

	return modDiff;
}

// encrypt() takes an array of n chars (original) and uses the key (also an array of n chars)
// to fill the array output[] with encrypted chars per OTP conventions. 
void encrypt(char original[], char key[], char output[], int n) {
	int i, j, result;
	
	// Strip off transmission identifiers
	char *origStripped, *keyStripped;
	origStripped = malloc(sizeof(char) * strlen(original));
	memset(origStripped, '\0', strlen(original));
	keyStripped = malloc(sizeof(char) * strlen(key));
	memset(keyStripped, '\0', strlen(key));
	j = 0;
	for (i = 2; i < strlen(original) - 2; i++) {
		origStripped[j] = original[i];
		j++;
	}
	j = 0;
	for (i = 2; i < strlen(key) - 2; i++) {
		keyStripped[j] = key[i];
		j++;
	}

	// Make temporary arrays of ints
	int *tmpOrig, *tmpKey, *tmpOutput;
	tmpOrig = malloc(sizeof(int) * strlen(origStripped));
	tmpKey = malloc(sizeof(int) * strlen(keyStripped));
	tmpOutput = malloc(sizeof(int) * strlen(origStripped));

	// Translate original chars from ASCII values to A=0, B=1, ..., Z=25, SPACE=26
	_translateASCIItoCode(origStripped, tmpOrig, strlen(origStripped));

	// Translate key chars from ASCII values to A=0, B=1, ..., Z=25, SPACE=26
	_translateASCIItoCode(keyStripped, tmpKey, strlen(keyStripped));

	// Add each char in original with corresponding char in key
	// if result >= 27, then subtract 27
	for (i = 0; i < strlen(origStripped); i++) {
		result = _addMOD(tmpOrig[i], tmpKey[i]);
		tmpOutput[i] = result;
	}

	// Translate result chars from A=0, B=1, ..., Z=25, SPACE=26 to ASCII values
	_translateCodetoASCII(tmpOutput, output, strlen(origStripped));

	// Clean up temporary arrays
	free(origStripped);
	free(keyStripped);
	free(tmpOrig);
	free(tmpKey);
	free(tmpOutput);
}

// decrypt() takes an array of n chars (original) and uses the key (also an array of n chars)
// to fill the array output[] with decrypted chars per OTP conventions. 
void decrypt(char original[], char key[], char output[], int n) {
	int i, j, result;

	// Strip off transmission identifiers
	char *origStripped, *keyStripped;
	origStripped = malloc(sizeof(char) * strlen(original));
	memset(origStripped, '\0', strlen(original));
	keyStripped = malloc(sizeof(char) * strlen(key));
	memset(keyStripped, '\0', strlen(key));
	j = 0;
	for (i = 2; i < strlen(original) - 2; i++) {
		origStripped[j] = original[i];
		j++;
	}
	j = 0;
	for (i = 2; i < strlen(key) - 2; i++) {
		keyStripped[j] = key[i];
		j++;
	}
	
	// Make temporary arrays of n ints
	int *tmpOrig, *tmpKey, *tmpOutput;
	tmpOrig = malloc(sizeof(int) * strlen(origStripped));
	tmpKey = malloc(sizeof(int) * strlen(keyStripped));
	tmpOutput = malloc(sizeof(int) * strlen(origStripped));

	// Translate original chars from ASCII values to A=0, B=1, ..., Z=25, SPACE=26
	_translateASCIItoCode(origStripped, tmpOrig, strlen(origStripped));

	// Translate key chars from ASCII values to A=0, B=1, ..., Z=25, SPACE=26
	_translateASCIItoCode(keyStripped, tmpKey, strlen(keyStripped));

	// Subtract each char in key from corresponding char in cipher tesxt
	// if result < 0, then add 27
	for (i = 0; i < strlen(origStripped); i++) {
		result = _subMOD(tmpOrig[i], tmpKey[i]);
		tmpOutput[i] = result;
	}

	// Translate result chars from A=0, B=1, ..., Z=25, SPACE=26 to ASCII values
	_translateCodetoASCII(tmpOutput, output, strlen(origStripped));

	// Clean up temporary arrays
	free(origStripped);
	free(keyStripped);
	free(tmpOrig);
	free(tmpKey);
	free(tmpOutput);
}
