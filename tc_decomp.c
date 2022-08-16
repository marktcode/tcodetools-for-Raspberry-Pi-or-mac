/*************************************************************************************
								
	Program:		Decomposition	
	Written by:	Scott Wackrow
	Date:		Dec 1994	

*************************************************************************************/
#include 	"Incs.c"

int main (argc, argv)
int argc;
char *argv[];
{
char *file=NULL, *string;
int heading, i;
DepletionSetTypeP codeP = NULL;

/*  search arguments for parameters */

	file = FindFile (argc, argv, DEFAULTCODESET );
	heading = ShowHeading(argc, argv);
	ShowHelp(argc, argv, "usage: decomposition [ -f filename ] [ -h ] string", TRUE);

	if (ArgsLeft(argc, argv) > 1) {
		printf ("error: too many arguments passed\n");
		exit (-1);
	}

	for (i=1; i<argc; i++)
		if (argv[i] != NULL)
			string = argv[i];

/* display whats received */

	if (heading == TRUE) printf ("\nFile: %s\n\n", file );

/*  load the file */

	codeP = NewCode (codeP);
	LoadCode ( file, codeP );

/* decompose the string */

	if (heading == TRUE) {
		printf("string: %s\n", string);
		printf("length: %d\n\n", strlen(string));	
	}
	DecomposeString(string, strlen(string), codeP);
	
	SaveCode (file, codeP);
	DisposeCode(&codeP);
	exit(0);
}
