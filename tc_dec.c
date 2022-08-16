/*************************************************************************************
								
	Program:		Decode	
	Written by:	Scott Wackrow
	Date:		Jan 1995	

*************************************************************************************/
#include 	"Incs.c"



int main (argc, argv)
int argc;
char *argv[];
{
char *file=NULL, *string, *stringptr, prefix[MAXPRFXLEN];
long lengthstring = 0;
int heading, level = 0, i, length = 1, expansion = 1, stdio = FALSE;
DepletionSetTypeP codeP = NULL;
DeplnType depln, lastdepln;


/*  search arguments for parameters */

	file = FindFile (argc, argv, DEFAULTCODESET );
	heading = ShowHeading(argc, argv);
	level = (int) FindNumber(argc, argv, 'n');
	lengthstring = FindNumber(argc, argv, 's');
	ShowHelp(argc, argv, "usage: tc_dec [ -f alphabetfilename ] [ -h ] [ -n level ] [-s sizeofinput] | [ string ]", FALSE);

	if (ArgsLeft(argc, argv) > 1) {
		printf ("error: too many arguments passed\n");
		ShowHelp(argc, argv, "usage: tc_dec [ -f alphabetfilename ] [ -h ] [ -n level ] [-s sizeofinput] | [ string ]", TRUE);
		exit (-1);
	}
	if (level < 0) {
		printf ("error: invalid level\n");
		exit (-1);
	}

	if (lengthstring == 0) {
		for (i=1; i<argc; i++)
			if (argv[i] != NULL)
				string = argv[i];

/* display whats received */

		lengthstring = strlen(string);
		if (heading == TRUE) {
			printf ("\nAlphaFile: %s\n", file );
			printf("string: %s\n", string);
			printf("length: %ld\n\n", lengthstring);	
		}
	}
	else { //standard input but with length lengthstring
		string = (char *)malloc(lengthstring); 
		for (i=0; i < lengthstring; i++ ) scanf ("%c", &string[i]);
		stdio=TRUE;
	}

/*  load the alphabet file */

	codeP = NewCode (codeP);
	LoadCode ( file, codeP );

/* decode the string */

	if (level == 0 || level > codeP->q )							/* set level to decode at */
		level = codeP->q;
	if (heading == TRUE)								/* display the level */
		printf("level: %d\n\n", level);

	NewDepln(&depln);
	NewDepln(&lastdepln);
	stringptr = string;
	while (length > 0) {
		length = DecodeAt(level, codeP, stringptr, &depln);
		if (length > 0) {
			stringptr += length;
			if ( CompareDepln( &lastdepln, &depln, codeP->q) == TRUE ) 
				expansion++;
			else {
				if (stringptr > string + length) {
					prefix[Encode(&lastdepln, codeP, prefix)] = 0;
					if (heading == TRUE )
						printf ("word = %s\tDT = %ld\tn = %d\n", prefix, DeplnToLong(&lastdepln, codeP), expansion);
					else
						printf ("%s\t%ld\t%d\n", prefix, DeplnToLong(&lastdepln, codeP), expansion);	
				}
				CopyDepln(&depln, &lastdepln);
				expansion = 1;
			}
		}
	}
	prefix[Encode(&lastdepln, codeP, prefix)] = 0;
	if (heading == TRUE )
		printf ("word = %s\tDT = %ld\tn = %d\n\n", prefix, DeplnToLong(&lastdepln, codeP), expansion);
	else
		printf ("%s\t%ld\t%d\n", prefix, DeplnToLong(&lastdepln, codeP), expansion);	

/* dispose working space */
	
	if (stdio==TRUE) free(string);
	DisposeCode(&codeP);
	exit(0);
}
