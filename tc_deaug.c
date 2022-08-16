/*************************************************************************************
								
	Program:		DeAugment	
	Written by:	Scott Wackrow
	Date:		Dec 1994	

*************************************************************************************/
#include 	"Incs.c"

int main (argc, argv)
int argc;
char *argv[];
{
char *file = NULL;
int number, heading;
DepletionSetTypeP codeP = NULL;

/*  search arguments for parameters */

	file = FindFile (argc, argv, DEFAULTCODESET );
	number = (int) FindNumber (argc, argv, 'n');
	heading = ShowHeading(argc, argv);
	ShowHelp(argc, argv, "usage: deaugment [ -f filename ] [ -h ]  [ -n number of levels ]", FALSE);

	if (ArgsLeft(argc, argv) > 0) {
		printf ("error: too many arguments passed\n");
		exit (-1);
	}
	if (number < 0) {
		printf ("error: invalid number of levels\n");
		exit (-1);
	}

/*  load the file */

	codeP = NewCode (codeP);
	LoadCode ( file, codeP );

	if (number == 0 || number > codeP->q)					/* calculate levels to remove */
		number = codeP->q;

/* display whats received */

	if (heading == TRUE) {
		printf ("\nFile: %s\n", file );
		printf ("levels:%d\n\n", number);
	}

/* save a backup */

	SaveCode( LASTDEAUGFILE , codeP);

/* deaugment */

	codeP->q -= number;

/* save file */

	SaveCode(file, codeP);
	DisposeCode(&codeP);
	exit(0);
}