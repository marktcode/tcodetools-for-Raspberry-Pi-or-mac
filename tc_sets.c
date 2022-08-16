/*************************************************************************************
								
	Program:		ScanSets	
	Written by:	Scott Wackrow
	Date:		Jan 1995	

*************************************************************************************/
#include 	"Incs.c"

int main (argc, argv)
int argc;
char *argv[];
{
char *file=NULL, *string, prefix[MAXPRFXLEN];
int heading, i, length, prfxlen;
DepletionSetTypeP codeP = NULL;
DeplnType depln;

/*  search arguments for parameters */

	file = FindFile (argc, argv, DEFAULTCODESET );
	heading = ShowHeading(argc, argv);
	ShowHelp(argc, argv, "usage: scansets [ -f filename ] [ -h ] string", TRUE);

	if (ArgsLeft(argc, argv) > 1) {								/* check number left */
		printf ("error: too many arguments passed\n");
		exit (-1);
	}
	for (i=0; i<argc; i++)
		if (argv[i] != NULL) 
			string = argv[i];							/* get scansize */

/* display whats received */

	if (heading == TRUE) {
 		printf ("\nFile: %s\n", file );
		printf ("String = %s\n", string);
	}

/*  load the file */

	codeP = NewCode (codeP);
	LoadCode ( file, codeP );

/* scan all sets for maximal length strings of given length */

	DecomposeString (string, strlen(string), codeP);				/* decompose the string */
	MaximalCode (codeP, &depln);					/* gets first of the maximal codes in Depletion*/
	length = Encode(&depln, codeP, string); 				/* gets the first maximal length string */

	if (length > 0) {			
		if (heading == TRUE) {
			printf("\nThe string is one of a set\n");			/* display the strings */
			for (i=1; i<=codeP->AlphabetP->Q; i++) {			/* display maximal codes */
				prfxlen = Encode(&depln, codeP, prefix);
				prefix[prfxlen] = 0;
				printf ("%s\n", prefix);
				depln.w[0]++;
			}
			printf ("\n|u| = %d\n\n", prfxlen);
		}
			
		codeP->q = 0; 						/* reset augmentation level*/
		TryNextSet(codeP, length - 1, string, heading );			/* now start search for the string in all other codes */
	}

/* save and dispose */

	DisposeCode(&codeP);
	exit(0);
}
