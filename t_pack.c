/*************************************************************************************
								
	Program:		Pack	
	Written by:	Scott Wackrow
	Date:		Dec 1994	

*************************************************************************************/
#include 	"Incs.c"

int main (argc, argv)
int argc;
char *argv[];
{
char *file=NULL;
int i, bit, character = 0, bitcount = 1;
FILE *fp;

/*  search arguments for parameters */

	ShowHelp(argc, argv, "usage: t_pack filename", TRUE);

	if (ArgsLeft(argc, argv) > 1) {
		printf ("error: too many arguments passed\n");
		exit (-1);
	}

	for (i=1; i<argc; i++)
		if (argv[i] != NULL)
			file = argv[i];

/*  load the file */

	fp = fopen(file, "r");
	if (fp == NULL) {
		printf("error: can't open file\n");
		exit(-1);
	}

/* convert the characters and display them */		

	bit = getc(fp);
	while ( feof(fp) == FALSE) {
		character *= 2;
		character += bit - 48;
		if (bitcount == 8) {
			printf("%c", character);
			character = 0;
			bitcount = 1;
		}
		else
			bitcount++;
		bit = getc(fp);
	}
	if (bitcount > 2)
		printf("%c", character);


/* close file and exit */

	fclose(fp);
	exit(0);
}

