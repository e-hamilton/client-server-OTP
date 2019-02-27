/*
**	Emily Hamilton (hamiltem)
**	CS344 - Operating Systems I (Spring 2018)
**	Oregon State University
**	Program 4 - "OTP - One Time Pad"
*/

/*
**	"keygen.c"
** 	Usage:	keygen [#chars in key]
**	Prints the specified number of random capital letters to stdout
**	(which can be redirected at the command line).
*/

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int main(int argc, char **argv) {
	// This program needs exactly one argument!
	if (argc != 2) {
		fprintf(stderr, "KEYGEN: Incorrect number of arguments. "
			"Terminating.\n");
		exit(EXIT_FAILURE);
	}

	int i, length, tmp, res, max, min;
	
	length = atoi(argv[1]);
	
	srand((int) time(NULL));	// Set seed for rand()
	
	// Task: Print <length> random chars + 1 newline to stdout
	
	max = 91;	// One greater than 'Z' (91 assigned to ' ')
	min = 65;	// 'A'

	for (i = 0; i < length; i++) {
		tmp = min + rand() / (RAND_MAX / (max - min + 1) + 1);
		// If 91, replace tmp with a space
		if (tmp == 91) {
			tmp = 32;	// 32 = space
		}
		// Print character to stdout
		res = putc(tmp, stdout);
		if (res == EOF) {
			fprintf(stderr, "Unable to write char <%c>.\n",
				tmp);
			exit(EXIT_FAILURE);
		}
	}

	// Last char is newline
	res = putc('\n', stdout);
	if (res == EOF) {
		fprintf(stderr, "Unable to write char <newline>.\n");
		exit(EXIT_FAILURE);
	}

	return 0;
}
