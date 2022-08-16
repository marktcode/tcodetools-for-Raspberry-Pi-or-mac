/*************************************************************************************
								
	Program:		Augment	
	Written by:	Scott Wackrow
	Date:		Dec 1994	
	Updated:		Jan 1995

*************************************************************************************/
#include 	"Incs.c"

int main (argc, argv)
int argc;
char *argv[];
{
DepletionSetTypeP 	codeP = NULL;
DeplnType	depln;
char	*file = NULL, *prefix = NULL, firstPDT[20];
int	i, heading, expansion = 1, len, list = FALSE;
long number = 0;

/* pass arguments */

	file = FindFile (argc, argv, DEFAULTCODESET );
	heading = ShowHeading(argc, argv);
	ShowHelp (argc, argv, "usage: augment [ -f filename ] [ -h ] [ -n PDT | prefix [ expansion parameter ] ] | s1 e1 ... sn en | -n PDT1 e1 ... PDTn en", TRUE );
	number = (long) FindNumber (argc, argv, 'n');

	if (number == 0) {										/* if a prefix entered then */
		if ( ArgsLeft(argc, argv) > 2 ) {							/* check arguments left */
			if ( ArgsLeft(argc, argv) % 2 != 0) {						/* if a list then check in pairs */
				printf ("error: invalid parameter count\n");
				exit (-1);
			}
			else
				list = TRUE;
		}
		else {										/* if not a list... */
			for ( i=1; i<argc; i++)							/* go through all arguments that aren't null */
				if ( argv[i] != NULL )
					if (prefix == NULL )						/* if prefix not yet found then this is it */
						prefix = argv[i];
					else							/* otherwise its the expansion parameter */
						sscanf(argv[i], "%d", &expansion);
			if (expansion < 1 ) {							/* check expansion parameter */
				printf ("error: invalid expansion parameter\n");
				exit (-1);
			}
		}
	}
	else {											/* if PDT entered instead of prefix */
		if (ArgsLeft(argc,argv) > 1 ) {								/* check arguments left */
			if ( ArgsLeft(argc, argv) % 2 != 1) {						/* if a list then check in pairs */
				printf ("error: invalid parameter count\n");
				exit (-1);
			}
			else
				list = TRUE;
		}
		else {										/* if not a list... */
			for ( i=1; i<argc; i++)							/* go through all arguments that aren't null */
				if ( argv[i] != NULL )
					sscanf(argv[i], "%d", &expansion);				/* get the expansion parameter */

			if (expansion < 1 ) {							/* check expansion parameter */
				printf ("error: invalid expansion parameter\n");
				exit (-1);
			}
		}
	}

/* show details */

	if (heading == TRUE) {
		printf ("\nfile: %s\n", file);
		if (list == TRUE )
			printf ("augmenting from list:\n");
		else {
			if (number == 0)								/* if prefix entered */
				printf ("prefix: %s\n", prefix);
			else									/* if PDT entered */
				printf ("PDT = %d\n", number);
			printf ("expansion: %d\n\n", expansion);
		}
	}

/* load the file */

	codeP = NewCode(codeP);
	LoadCode (file, codeP);

/* check the prefix is valid and save to file */

	NewDepln(&depln);										/* initilise the depln code */
	if (list == FALSE) {
		if (number == 0) {									/* if prefix then ... */
		  printf("prefix '%s', strtodepln = %d\n", prefix,
			 StringToDepln(codeP, prefix, &depln));
			if (strlen(prefix) != StringToDepln ( codeP, prefix, &depln)) {			/* convert string to depln and check all used */
				printf ("error: invalid prefix1\n");
				exit (-1);
			}
		}
		else 										/* if PDT in long number form */
			LongToDepln(number, codeP, &depln);						/* convert to depln */
		if ( ChkDeplnCode(&depln, codeP) == FALSE ) {						/* check if code is valid */
			if (number == 0)								/* if not show invalid */
				printf("error: invalid prefix2\n");
			else
				printf("error: invalid PDT\n");
			exit(-1);
		}
		else
			Augment(codeP, &depln, expansion);						/* if valid augment */
	}
	else {											/* if a list of parameters */
		if (number != 0) {									/* if PDTs entered */
			sprintf (firstPDT, "%d", number);						/* convert back to string */
			prefix = firstPDT;								/* set as first PDT / prefix */
		}
		for (i=1; i<argc; i++) 								/* for each argument */
			if (argv[i] != NULL)								/* check its not null */
				if (prefix == NULL)							/* if we havent got the next prefix/PDT yet */
					prefix = argv[i];						/* get the prefix/PDT */
				else {								/* otherwise get the expansion */
					sscanf(argv[i], "%d", &expansion);				/* got a prefix/PDT and expansion so */
					if (number == 0) {						/* if prefix then ... */
						if (strlen(prefix)!=StringToDepln(codeP, prefix, &depln)){ 	/* convert string to depln and check all used */
							printf ("error: invalid prefix3\n");
							exit (-1);
						}
					}
					else {							/* if PDT in long number form */
						sscanf (prefix, "%d", &number);			/* convert string to long number */
						LongToDepln(number, codeP, &depln);			/* convert long number to depln */
					}
					if ( ChkDeplnCode(&depln, codeP) == FALSE ) {			/* check if code is valid */
						if (number == 0)					/* if not show invalid */
							printf("error: invalid prefix4\n");
						else
							printf("error: invalid PDT\n");
						exit(-1);
					}
					else
						Augment(codeP, &depln, expansion);			/* augment */
					if (heading == TRUE)					/* list augmentations */
						if (number ==0)		
							printf("s%d = %s\te%d = %d\n", codeP->q, prefix, codeP->q, expansion); 
						else
							printf("PDT%d = %s\te%d = %d\n", codeP->q, prefix, codeP->q, expansion);
					prefix = NULL;
				}
		if (heading == TRUE)
			printf("\n");
	}
			
	SaveCode ( file, codeP );							/* save code */
	DisposeCode(&codeP);
	exit (0);
}
