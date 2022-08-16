/**************************************************************************************

	Routines for scanning arguments passed

	Written by:	Scott Wackrow
	Date:		Dec 1994	
	Modified:		May 1995		FindTree, added for optional loading of 
						the complexity tree

***************************************************************************************
FindFile:		syntax:	FindFile (  ArgumentCount, ArgumentList, StringPointer )

		accepts:	a count of arguments in the list and a pointer to the start 
			of an array of pointers to arrays of characters (strings) and
			a pointer to the default file name

		does:	searches the argument list for a -f filename, ignoring any
			null pointers and setting pointers to null if found

		returns:	pointer to the file name

		********************************************

FindTree:		syntax:	FindTree (  ArgumentCount, ArgumentList, StringPointer )

		accepts:	a count of arguments in the list and a pointer to the start 
			of an array of pointers to arrays of characters (strings) and
			a pointer to the default tree name

		does:	searches the argument list for a -t treename, ignoring any
			null pointers and setting pointers to null if found

		returns:	pointer to the tree name

		********************************************

FindNumber:	syntax:	FindNumber (  ArgumentCount, ArgumentList, letter)

		accepts:	a count of arguments in the list and a pointer to the start
			of an array of pointers to arrays of characters (strings)

		does:	searches the argument list for a -n number, ignoring any
			null pointers and setting pointers to null if found

		returns:	the number found or 0 if no number found

		********************************************

ArgsLeft:		syntax:	ArgsLeft ( ArgumentCount, ArgumentList )

		accepts:	a count of arguments in the list and a pointer to the start
			of an array of pointers to arrays of characters (strings)

		does:	searches the argument list counting all non null pointers
			excluding the program name ( ie: first argument )

		returns:	number of non null pointers

		********************************************

ShowHelp:	syntax:	ShowHelp ( ArgumentCount, ArgumentList, StringPointer, Boolean)

		accepts:	a count of arguments in the list, a pointer to the start
			of an array of pointers to arrays of characters (strings),
			a pointer to the help string to display, and a boolean value
			indicating to display if there are no arguments left

		does:	searches the argument list for a ?, if it found then displays
			string pointed to then exists.  if boolean true then will also
			display and exit if arguments left = 0

		returns:	nothing

		********************************************
		
ShowHeadings:	syntax:	ShowHeading(argc, argv)


		accepts:	a count of arguments in the list and a pointer to the start
			of an array of pointers to arrays of characters (strings)

		does:	searches the argument list for a -h, if found set pointer
			to null

		returns:	if found returns true, if not returns false
	
**************************************************************************************/
char *FindFile (  int, char	**, char *);
char *FindTree (  int, char	**, char *);
long int FindNumber ( int, char **, char);
int ArgsLeft(int, char **);
void ShowHelp(int, char **, char *, int);
int ShowHeading(int, char **);







char *FindFile (  argc, argv, string)
int	argc;
char	*argv[];
char	*string;
{
int 	i;
char	*file = NULL;

	for ( i=0; i < argc; i++ ) {							/*  search arguments for parameters */
		if (argv[i] != NULL) {							/* don't use null values */
			if ((argv[i])[0] == '-' && (argv[i])[1] == 'f' && (argv[i])[2] == 0){	/* check for -f flag */
				if (file == NULL) {					/* check no file already specified */
					if (i+1 < argc) {				/* check if no file name after -f */
						file = argv[i+1];
						argv[i] = NULL;
						argv[i+1] = NULL;
					}
					else {
						printf ("error: no file name specified after -f\n");
						exit (-1);
					}
				} else {
					printf ("error: more than one file specified\n");
					exit (-1);
				}
			}
		}
	}
	if (file == NULL)
		return (string);
	else
		return (file);
}

/*************************************************************************************/

char *FindTree (  argc, argv, string)
int	argc;
char	*argv[];
char	*string;
{
int 	i;
char	*tree = NULL;

	for ( i=0; i < argc; i++ ){							/*  search arguments for parameters */
		if (argv[i] != NULL) {							/* don't use null values */
			if ((argv[i])[0] == '-' && (argv[i])[1] == 't' && (argv[i])[2] == 0){	/* check for -t flag */
				if (tree == NULL){					/* check no tree already specified */
					if (i+1 < argc) {				/* check if no tree name after -t */
						tree = argv[i+1];
						argv[i] = NULL;
						argv[i+1] = NULL;
					} else {
						printf ("error: no tree name specified after -t\n");
						exit (-1);
					}
				} else {
					printf ("error: more than one tree specified\n");
					exit (-1);
				}
			}
		}
	}
	if (tree == NULL)
		return (string);
	else
		return (tree);
}

/*************************************************************************************/

long int FindNumber (  argc, argv, letr )
int	argc;
char *argv[], letr;
{
long int	i;
char	*number = NULL;

	for ( i=0; i < argc; i++ ) {							/*  search arguments for parameters */
		if (argv[i] != NULL) {							/* don't use null values */
			if ((argv[i])[0] == '-' && (argv[i])[1] == letr && (argv[i])[2] == 0) {	/* check for -n flag */
				if (number == NULL)	{			/* check no number already specified */
					if (i+1 < argc) {				/* check if no number name after -n */
						number = argv[i+1];
						argv[i] = NULL;
						argv[i+1] = NULL;
					}
					else {
						printf ("error: no number specified after -%c\n", letr);
						exit (-1);
					}
				} else {
					printf ("error: more than one number specified\n");
					exit (-1);
				}
			}
		}
	}
	if (number == NULL)
		return ((long int) 0);
	else {
		sscanf (number, "%ld", &i);
		return ((long int)i);
	}
}

/*************************************************************************************/

int ArgsLeft(argc, argv)
int argc;
char *argv[];
{
int i, count = 0;

	for (i=1; i<argc; i++)							/* don't count program name ie:argv[0] */
		if (argv[i] != NULL)
			count++;
	return(count);
}

/*************************************************************************************/

void ShowHelp(argc, argv, string, onnoargs)
int argc;
char *argv[];
char *string;
int onnoargs;
{
int 	i;

	if (onnoargs == TRUE && ArgsLeft(argc, argv) == 0) {
		printf ("%s\n", string);
		exit (-1);
	}
	for ( i=0; i < argc; i++ ) {							/*  search arguments for parameters */
		if (argv[i] != NULL) {							/* don't use null values */
			if ((argv[i])[0] == '?' && (argv[i])[1] == 0 ) {			/* check for ? flag */
				printf ("%s\n", string);
				exit (0);
			}
		}
	}
}

/*************************************************************************************/

int ShowHeading(argc, argv)
int argc;
char *argv[];
{
int i, show = FALSE;

	for ( i=0; i < argc; i++ ) {							/*  search arguments for parameters */
		if (argv[i] != NULL) {							/* don't use null values */
			if ((argv[i])[0] == '-' && (argv[i])[1] == 'h' && (argv[i])[2] == 0) {	/* check for -h flag */
				argv[i] = NULL;
				show = TRUE;
			}
		}
		return (show);
	}
}

/*************************************************************************************/
