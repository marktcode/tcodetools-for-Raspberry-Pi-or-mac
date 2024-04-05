/*************************************************************************************

	Program:		NewAlphabet
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
int	i, size=0, heading;

/* pass arguments */

	file = FindFile (argc, argv, DEFAULTCODESET );
	size = (int) FindNumber (argc, argv, 'n');
	heading = ShowHeading(argc, argv);
	ShowHelp (argc, argv, "usage: new [ -f filename ] [ -n size ] [ elements ]", FALSE );

	if (size == 0 && ArgsLeft(argc, argv) == 0 ) {
		size = 2;
	}

/* show details */

	if (heading == TRUE) {
		printf ("\nfile: %s\n", file);
		printf ("size: %d\n\n", (size == 0) ? ArgsLeft(argc, argv) : size);
	}

/* create and save */

	codeP = NewCode(codeP);
	if ( size == 0)
		AlphabetFromArgs(argc, argv, codeP->AlphabetP);
	else
		GenerateAlphabet(codeP->AlphabetP, size);
	SaveCode(file, codeP);
	printf("new alphabet, size %ld\n", CodeSize(codeP));
	DisposeCode(&codeP);
	exit(0);
}
