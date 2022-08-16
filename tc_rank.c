/*************************************************************************************
								
	Program:		RankList2	
	Written by:	Scott Wackrow
	Date:		Dec 1994	

*************************************************************************************/
#include 	"Incs.c"

int main (argc, argv)
int argc;
char *argv[];
{
char *file=NULL, prefix[MAXPRFXLEN];
int heading, number, setsize, len, i=1;
DepletionSetTypeP codeP = NULL;
DeplnType depln;
long longnumber = -1;

/*  search arguments for parameters */

	file = FindFile (argc, argv, DEFAULTCODESET );
	heading = ShowHeading(argc, argv);
	number = (int) FindNumber(argc, argv, 'n');
	ShowHelp(argc, argv, "usage: ranklist2 [ -f filename ] [ -h ] [-n position ]", FALSE);

	if (number == 0) {
		if (ArgsLeft(argc, argv) > 0) {
			printf ("error: too many arguments passed\n");
			exit (-1);
		}
	}
	else {
		if (ArgsLeft(argc, argv) <= 1) {
			for (i=1; i < argc; i++)
				if (argv[i] != NULL) 
					sscanf(argv[i], "%d", &longnumber);
		}
		else {
			printf ("error: too many arguments passed\n");
			exit(-1);
		}
	}

/* display whats received */

	if (heading == TRUE) printf ("\nFile: %s\n\n", file );

/*  load the file */

	codeP = NewCode (codeP);
	LoadCode ( file, codeP );

/* list the set */

	NewDepln(&depln);								/* initilise some working space */

	setsize = CodeSize(codeP);							/* find set size */
	if (heading == TRUE)
		printf ("Set size = %d\n\ni\tPDT[i]\tWord[i]\n", setsize);			/* show set size */	

	if (number == 0) {								/* list the whole set */
		while ( (longnumber = FirstRankWord(longnumber, codeP, &depln) ) >= 0) {	/* get next rank word */
			len = Encode (&depln, codeP, prefix);				/* encode it to a string */
			prefix[len] = 0;						/* mark end of string */
			if (heading == TRUE)
				printf ("%d\t%d\t%s\n", i++, longnumber, prefix);	/* show all details */
			else
				printf ("%s\n", prefix);				/* show basic details */
		}
		printf ("\n");							/* print new line */
	}
	else {									/* list just one word */
		if (number < 0)
			number = number + setsize + 1;
		if (number < 1) 
			number = 1;
		if (number > setsize)
			number = setsize;
		for (i=0; i < number; i++)						/* until we reach the nth word */
			longnumber = FirstRankWord(longnumber, codeP, &depln);		/* get the next word */

		len = Encode (&depln, codeP, prefix);					/* encode it to a string */
		prefix[len] = 0;							/* mark end of string */
		if (heading == TRUE)
			printf ("%d\t%d\t%s\n\n", i, longnumber, prefix);		/* show all details */
		else
			printf ("%s %d\n", prefix, longnumber);					/* show basic details */
	}
	DisposeCode(&codeP);
	exit(0);
}
