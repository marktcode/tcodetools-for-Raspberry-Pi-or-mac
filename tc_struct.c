/*************************************************************************************
								
	Program:		Structures	
	Written by:	Scott Wackrow
	Date:		Dec 1994	

*************************************************************************************/
#include 	"Incs.c"

int main (argc, argv)
int argc;
char *argv[];
{
DepletionSetTypeP 	codeP = NULL;
char	*file = NULL;
int	i, heading;

/* pass arguments */

	file = FindFile (argc, argv, DEFAULTCODESET );
	heading = ShowHeading(argc, argv);
	ShowHelp (argc, argv, "usage: structures [ -f filename ] [-h]", FALSE );

	if (ArgsLeft(argc, argv) > 0 ) {
		printf("error: too many arguments passed\n");
		exit(-1);
	}

/* show details */

	printf ("\nfile: %s\n\n", file);

/* open file and show structures */

	codeP = NewCode(codeP);
	LoadCode ( file, codeP );
	if (heading == TRUE) {
		ShowStructure(codeP);
		EnlargeSet ( codeP, 1, 2 );
	}
	else
		EnlargeSet ( codeP, 1, 3 );

/* dispose memory and exit */

	DisposeCode(&codeP);
	exit(0);
}
