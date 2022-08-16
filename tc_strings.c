/*************************************************************************************
								
	Program:		ScanStrings	
	Written by:	Scott Wackrow
	Date:		Dec 1994	
	Modified:		May 1995		Optional loading of complexity tree

*************************************************************************************/
#include 	"Incs.c"

int main (argc, argv)
int argc;
char *argv[];
{
char *file=NULL, *tree=NULL, string[MAXPRFXLEN];
int heading, scansize = 0, i, complexity, number, sum = 0, count = 0;
int treechanged = FALSE, finished = FALSE, complhistogram[MAXPRFXLEN];
DepletionSetTypeP codeP = NULL;
ComplexityP complexitytreeP = NULL;

/*  search arguments for parameters */

	file = FindFile (argc, argv, DEFAULTCODESET );
	tree = FindTree (argc, argv, NULL );
	heading = ShowHeading(argc, argv);
	ShowHelp(argc, argv, "usage: scanstrings [ -f filename ] [ -t complexity tree ] [ -h ] stringsize", TRUE);

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
		printf ("String size = %d\n", scansize);
	}

/*  load the file */

	codeP = NewCode (codeP);
	LoadCode ( file, codeP );
	if ( tree != NULL )
		LoadTree( tree, &complexitytreeP, heading, codeP->AlphabetP->Q );

/* scan all fixed length codes */

	for (i = 0; i < scansize; i++) {
		string[i] = codeP->AlphabetP->a[0]; 						/* init string characters to first alphabet letter*/
		complhistogram[i+1] = 0;							/* init complexity histogram to 0 */
	}
	string[scansize] = 0;								/* set end of string */

	while (finished == FALSE) { 								/* size is reset when the counting is done */

		DecomposeString(string, scansize, codeP); 					/* decompose maximal string to a new set*/
		complexity = FndInsert(&complexitytreeP, codeP , 0 );				/* try finding in complexity tree */
		if (complexity == 0)  {							/* if not found then ... */
			complexity = EnlargeSet(codeP, 1, FALSE);				/* then do it rest starting from level 1, and show = FALSE*/
			treechanged = TRUE;
			FndInsert(&complexitytreeP, codeP , complexity );			/* Insert the resulting complexity in tree */
		}
		complhistogram[complexity] += 1;						/* increase histogram count for that complexity */
		if (heading == TRUE) printf("%s\tH(u) = %d\n", string, complexity);			/* display string and complexity */

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

/* display histogram material */

	if ( heading == TRUE ) printf("\nHistogram\ncompl\tcount\n\n");
	for (i = 1; i <= scansize; i++) {
		printf("%d\t%d\n", i, complhistogram[i]);
		sum += i * complhistogram[i];
		count += complhistogram[i];
	} 
	if (heading == TRUE) printf("\nAverage = %d / %d = %f\n\n", sum, count, (float) sum / (float) count );

/* save and dispose */

	if (treechanged == TRUE)
		if ( tree != NULL )
			SaveTree( tree, complexitytreeP, heading, codeP->AlphabetP->Q );
	DisposeCode(&codeP);
	DisposeTree(&complexitytreeP);
	exit(0);
}