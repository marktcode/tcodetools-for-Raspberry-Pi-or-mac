/*************************************************************************************
								
	Program:		Encode	
	Written by:	Scott Wackrow
	Date:		Jan 1995	

*************************************************************************************/
#include 	"Incs.c"

int main (argc, argv)
int argc;
char *argv[];
{
char *file=NULL, prefix[MAXPRFXLEN];
int heading, level = 0, PDT = -1, expansion = 1, i, length = 0, position = 0;
DepletionSetTypeP codeP = NULL;
DeplnType depln;

/*  search arguments for parameters */

	file = FindFile (argc, argv, DEFAULTCODESET );
	heading = ShowHeading(argc, argv);
	level = (int) FindNumber(argc, argv, 'n');
	ShowHelp(argc, argv, "usage: Encode [ -f filename ] [ -h ] [ -n level ] PDT [ expansion ]", TRUE);

	if (ArgsLeft(argc, argv) > 2) {
		printf ("error: too many arguments passed\n");
		exit (-1);
	}
	if (level < 0) {
		printf ("error: invalid level\n");
		exit (-1);
	}

	for (i=1; i<argc; i++)
		if (argv[i] != NULL)
			if ( PDT == -1 )						/* if PDT not yet found then this is it */
				sscanf(argv[i], "%d", &PDT);
			else							/* otherwise its the expansion parameter */
				sscanf(argv[i], "%d", &expansion);

	if (PDT < 0) {
		printf ("error: invalid PDT\n");
		exit (-1);
	}

	if (expansion < 1) {
		printf ("error: invalid expansion\n");
		exit (-1);
	}

/* display whats received */

	if (heading == TRUE) {
		printf ("\nFile: %s\n", file );
		printf("PDT: %d\n", PDT);
		printf("expansion: %d\n", expansion);	
	}

/*  load the file */

	codeP = NewCode (codeP);
	LoadCode ( file, codeP );

/* encode the PDT */

	if (level == 0 || level > codeP->q )							/* set level to encode at */
		level = codeP->q;
	codeP->q = level;
	if (heading == TRUE)								/* display the level */
		printf("level: %d\n\n", level);

	LongToDepln ( PDT, codeP, &depln);							/* convert PDT to depln */
	if ( ChkDeplnCode(&depln, codeP) == FALSE ) {						/* check if code is valid */
		printf ("error: invalid PDT\n");
		exit (-1);
	}
	
	for (i=1; i<= expansion; i++) {								/* get prefix expansion times */
			length = Encode (&depln, codeP, &prefix[position] );
			position += length;
	}
	prefix[position] = 0;								/* mark end of prefix's */
	if (heading == TRUE)								/* display encoded version */
		printf("prefix = %s\n\n", prefix);
	else
		printf ("%s", prefix);

/* dispose working space */
	
	DisposeCode(&codeP);
	exit(0);
}
