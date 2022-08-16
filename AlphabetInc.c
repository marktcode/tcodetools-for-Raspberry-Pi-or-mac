/**************************************************************************************

 	Alphabet Routines  				

	Written by:	Scott Wackrow
	Date:		Dec 1994	

***************************************************************************************

NewAlphabet:	syntax:	NewAlphabet ( AlphabetPointer )

		accepts:	a pointer to an alphabet, initilised or not

		does:	if pointer not initilised allocates memory
			clears all alphabet characters and sets Q = 0

		returns:	the pointer to the alphabet

		********************************************

GenerateAlphabet:	syntax:	GenerateAlphabet (AlphabetPointer, SizeOfAlphabet)

		accepts:	a pointer to an initlised alphabet, and a size of the
			alphabet to create

		does:	generates an alphabet, if size is less than ten creates
			numbers, if greater it creates letters.  If size is more 
			then MAXALPHA an error is displayed

		returns:	nothing

		********************************************

AlphabetFromArgs:	syntax:	AlphabetFromArgs (ArgumentCount, ArgumentList, AlphabetPointer)

		accepts:	a count of arguments in the list and a pointer to the start 
			of an array of pointers to arrays of characters (strings) and
			a pointer to the initilised alphabet

		does:	takes items from list and inserts them in the alphabet, ignores
			all null pointers in the list

		returns:	nothing

		********************************************

CopyAlphabet:	syntax:	CopyAlphabet (AlphabetPointer1, AlphabetPointer2)

		accepts: 	pointers to two initlised alphabets

		does:	re initilises the second alphabet and copies the alphabet
		 	at the first pointer into that of the second pointer

		returns:	nothing

		********************************************

DisposeAlphabet: 	syntax:	DisposeAlphabet (PointerToAlphabetPointer)

		accepts:	the address ( ie: a pointer ) of an alphabet pointer

		does:	frees the memory of the alphabet and sets the alphabet
			pointer to null, a message is displayed if an error occurs

		returns:	nothing

**************************************************************************************/
AlphabetTypeP NewAlphabet ( AlphabetTypeP );
void GenerateAlphabet ( AlphabetTypeP, int );
void AlphabetFromArgs ( int, char **, AlphabetTypeP);
void CopyAlphabet(AlphabetTypeP, AlphabetTypeP);
void DisposeAlphabet (AlphabetTypeP	*);




AlphabetTypeP NewAlphabet ( AlphabetP )
AlphabetTypeP AlphabetP;
{

int 	i;

	if ( AlphabetP == NULL )
		AlphabetP = ( AlphabetTypeP ) malloc(sizeof(AlphabetType));
	if ( AlphabetP != NULL ) {
		AlphabetP->Q = 0;
		for (i=0; i<MAXALPHA ; i++)
			AlphabetP->a[i] = 0;	
	}
	return (AlphabetP);
}

/*************************************************************************************/

void GenerateAlphabet ( AlphabetP, size )
AlphabetTypeP AlphabetP;
int	size;
{
int	i;

	if (size > 10 )
		if (size > MAXALPHA) {
			printf ("error: can't create alphabet that large\n");
			exit (-1);
		}
		else
			for ( i=0; i<size; i++)
				AlphabetP->a[i] = i + 'a';
	else
		for ( i=0; i<size; i++)
			AlphabetP->a[i] = i + '0';
	AlphabetP->Q = size;
}

/*************************************************************************************/

void AlphabetFromArgs ( argc, argv, AlphabetP)
int argc;
char *argv[];
AlphabetTypeP AlphabetP;
{
int	i;

	for ( i=1; i < argc; i++)							/* ignore name of program */
		if (argv[i] != NULL)
			AlphabetP->a[AlphabetP->Q++] = (argv[i])[0];
}

/*************************************************************************************/

void CopyAlphabet(alphabet1P, alphabet2P)
AlphabetTypeP alphabet1P, alphabet2P;
{
int i;
	NewAlphabet(alphabet2P);							/* clears the alphabet */
	alphabet2P->Q = alphabet1P->Q;						/* copies Q */
	for (i=0; i<alphabet1P->Q; i++)
		alphabet2P->a[i] = alphabet1P->a[i];					/* copies letters */
}

/*************************************************************************************/

void DisposeAlphabet (alphabetPP)
AlphabetTypeP	*alphabetPP;
{
	if ( *alphabetPP != NULL ) {
		free(*alphabetPP);
		*alphabetPP = NULL;
	}
}

/*************************************************************************************/
