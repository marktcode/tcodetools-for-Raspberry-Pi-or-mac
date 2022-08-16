/*************************************************************************************
								
	Program:		Unpack	
	Written by:	Scott Wackrow
	Date:		Dec 1994	

*************************************************************************************/
//#include 	"Incs.c"

#include 	<stdlib.h>
#include	<stdio.h>

#define TRUE 1
#define FALSE 0

#include	"ArgInc.c"


int main (argc, argv)
int argc;
char *argv[];
{
char *file=NULL;
int i, number;
FILE *fp;

/*  search arguments for parameters */

	ShowHelp(argc, argv, "usage: unpack -f filename", FALSE);

	if (ArgsLeft(argc, argv) > 3) {
		printf ("error: too many arguments passed\n");
		exit (-1);
	}
	if (argc==1) { // got no switches
		number = getc(stdin);
		while ( feof(stdin) == FALSE) {
			for (i=0; i<8; i++) {
				printf ("%d", (number >> 7 ) % 2 );
				number = number * 2;
			}
			number = getc(stdin);
		}
		fflush(stdout);
		exit(0);
	
	}
	else {
		if (strncmp(argv[1],"-f",2)) {
			printf ("error: unrecognised switch %s\n", argv[1]); 
			exit (-1);
		}
		for (i=2; i < argc; i++)
			if (argv[i] != NULL)
				file = argv[i];
	
	/*  load the file */
	
		fp = fopen(file, "r");
		if (fp == NULL) {
			printf("error: can't open file\n");
			exit(-1);
		}
	
	/* convert the characters and display them */		
	
		number = getc(fp);
		while ( feof(fp) == FALSE) {
			for (i=0; i<8; i++) {
				printf ("%d", (number >> 7 ) % 2 );
				number = number * 2;
			}
			number = getc(fp);
		}
		fflush(stdout);
		/* close file and exit */
		fclose(fp);
		exit(0);
	}

}

