/*************************************************************************************
								
	Program:		ListStrings	
	Written by:	Scott Wackrow
	Date:		Dec 1994	

*************************************************************************************/
#include 	"Incs.c"

int main (argc, argv)
int argc;
char *argv[];
{
char *file=NULL, string[MAXPRFXLEN];
int heading, scansize,number, i, finished = FALSE;
DepletionSetTypeP codeP = NULL;

/*  search arguments for parameters */

	file = FindFile (argc, argv, DEFAULTCODESET );
	heading = ShowHeading(argc, argv);
	ShowHelp(argc, argv, "usage: liststrings [ -f filename ] [ -h ] stringsize", TRUE);

	if (ArgsLeft(argc, argv) > 1) {								/* check number left */
		printf ("error: too many arguments passed\n");
		exit (-1);
	}
	for (i=0; i<argc; i++)
		if (argv[i] != NULL) 
			sscanf(argv[i], "%d", &scansize);					/* get scansize */

	if (scansize <= 0) {									/* check scansize valid */
		printf("error: invalid string length\n");
		exit (-1);
	}

/* display whats received */

	if (heading == TRUE) {
 		printf ("\nFile: %s\n", file );
		printf ("String size = %d\n\n", scansize);
	}

/*  load the file */

	codeP = NewCode (codeP);
	LoadCode ( file, codeP );

/* list codes */
	for (i = 0; i < scansize; i++) 
		string[i] = codeP->AlphabetP->a[0]; 						/* init string characters to first alphabet letter*/
	string[scansize] = 0;

	while (finished == FALSE) {
		printf("%s\n", string);
		number = 1;								/* set carry flag for first character */
		for (i = scansize - 1; i >= 0; i--) { 						/* increment string */
			if (string[i] - codeP->AlphabetP->a[0] + number >= codeP->AlphabetP->Q) {	/* if character greater or equal to highest letter */
				string[i] = codeP->AlphabetP->a[0];				/* reset character to first letter */
				number = 1; 						/* set carry flag */
				if (i == 0) finished = TRUE; 					/* if the last character then finished */
			}
			else {								/* if character not greater or equal to highest letter */
				string[i] += number;					/* increase the letter by the carry */
				number = 0;						/* clear the carry flag */
			}
		}
	}
	if (heading == TRUE) printf("\n");

/* dispose */

	DisposeCode(&codeP);
	exit(0);
}
