/*************************************************************************************
								
	Program:		ListSet	
	Written by:	Scott Wackrow
	Date:		Dec 1994	

*************************************************************************************/
#include 	"Incs.c"

int main (argc, argv)
int argc;
char *argv[];
{
char *file=NULL, prefix[MAXPRFXLEN];
int heading, setsize, len, count = 0, number = 0;
DepletionSetTypeP codeP = NULL;
DeplnType depln;

/*  search arguments for parameters */

	file = FindFile (argc, argv, DEFAULTCODESET );
	heading = ShowHeading(argc, argv);
	ShowHelp(argc, argv, "usage: listset [ -f filename ] [ -h ]", FALSE);

	if (ArgsLeft(argc, argv) > 0) {
		printf ("error: too many arguments passed\n");
		exit (-1);
	}

/* display whats received */

	if (heading == TRUE) printf ("\nFile: %s\n\n", file );

/*  load the file */

	codeP = NewCode (codeP);
	LoadCode ( file, codeP );

/* list the set */

	NewDepln(&depln);									/*initilise a working area */
	setsize = CodeSize(codeP);
	if (heading == TRUE) printf ("Set size = %d\n\ni\tPDT[i]\tWord[i]\n", setsize);			/* show set size */
	while (count < setsize) {
		LongToDepln((long) number, codeP, &depln);					/* convert long to a depln code */
		if (TRUE == ChkDeplnCode(&depln, codeP)) {					/* if its a valid code */
			count++;								/* increment number found */
			len = Encode (&depln, codeP, prefix);					/* encode into string */
			prefix[len] = 0;							/* set end of string mark */
			if (heading == TRUE)
				printf ("%d\t%d\t%s\n", count, number, prefix);			/* show full details */
			else
				printf ("%s\n", prefix);					/* show basic details */
		}
		number++;								/* increment long number */
	}
	printf ("\n");									/* print new line */
	DisposeCode(&codeP);
	exit(0);
}