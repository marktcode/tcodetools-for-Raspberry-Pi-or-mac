/*************************************************************************************
								
	Program:		ReAugment	
	Written by:	Scott Wackrow
	Date:		Dec 1994	

*************************************************************************************/
#include 	"Incs.c"

int main (argc, argv)
int argc;
char *argv[];
{
char *file = NULL;
DepletionSetTypeP codeP = NULL;

/*  search arguments for parameters */

	file = FindFile (argc, argv, DEFAULTCODESET );
	ShowHelp(argc, argv, "usage: reaugment [ -f filename ]", FALSE);

	if (ArgsLeft(argc, argv) > 0) {
		printf ("error: too many arguments passed\n");
		exit (-1);
	}

/*  load the old file */

	codeP = NewCode (codeP);
	LoadCode ( LASTDEAUGFILE, codeP );


/* display whats received */

	printf ("\nFile: %s\n", file );
	printf ("restored to level %d\n\n", codeP->q);
	
/* save file */

	SaveCode(file, codeP);
	DisposeCode(&codeP);
	exit(0);
}