/*****************************************************************************************

 	Depletion Set Routines  				

	Written by:	Scott Wackrow
	Date:		Dec 1994	

******************************************************************************************

NewCode:	syntax:	NewCode ( CodePointer )

		accepts:	a pointer to a code set, initilised or not

		does:	if pointer not initilised allocates memory
			initlises alphabet and all PDTs, set q=0 and
			sets all expansion parameters = 0

		returns:	the pointer to the code

		********************************************

LoadCode:	syntax:	LoadCode ( StringPointer, CodePointer )

		accepts:	a pointer to a string and a pointer to an initilised
			code

		does:	loads the code set from the file with the name
			pointed to by the string pointer

		returns:	nothing

		********************************************

SaveCode:	syntax:	SaveCode ( StringPointer, CodePointer )

		accepts:	a pointer to a string and a pointer to a code set

		does:	saves the code set to the file with the name pointed
			to be the string pointer

		returns:	nothing

		********************************************

MaximalCode:	syntax:	MaximalCode ( CodePointer, deplationPointer )

		accepts:	a pointer to a code set and a pointer to a depletion
			code

		does:	finds the first maximal length code word and places
			it in the depletion code pointed to

		returns:	nothing

		********************************************

CodeSize:		syntax:	CodeSize ( CodePointer )

		accepts:	a pointer to a code set

		does:	calculates how many members will be in the code set

		returns:	number of members

		********************************************

WordSize:	syntax:	WordSize ( DepletionPointer, CodePointer )

		accepts:	a pointer to a depletion code and a pointer to a code
			set to encode by

		does:	counts the length of the word after encoding the
			depletion code into a string, calls encode with a null
			string pointer

		returns:	the length of the word

		********************************************

NextRankWord:	syntax:	NextRankWord ( CurrentLong, CodePointer, DepletionPointer )

		accepts:	a long representation of the last word or -1 if this is the 
			first, a pointer to the code set and a pointer to a depletion 
			code to work with

		does:	finds the next word in the code set in length rank order, use with
			small sets where a full listing is required
			
		returns:	next rank word in long number representation or -1
			if there are no more

		********************************************

FirstRankWord:	syntax:	FirstRankWord ( CurrentLong, CodePointer, DepletionPointer )

		accepts:	a long representation of the last word or -1 if this is the 
			first, a pointer to the code set and a pointer to a depletion 
			code to work with

		does:	finds the next word in the code set in length rank order, use with
			large sets where a full listing is not required - high speed version
			of NextRankWord
			
		returns:	next rank word in long number representation or -1
			if there are no more

		********************************************

DecomposeString:	syntax:	DecomposeString ( StringPointer, LengthOfString, CodePointer )

		accepts:	a pointer to the string to decompose, the length of the
			string and a pointer to a code set to decompose into

		does:	decomposes the string pointed to into a new code set replacing the
			code pointed to, calls DecodeAt to encode sections of the string

		returns: 	nothing

		********************************************

Augment:		syntax:	Augment ( CodePointer, DepletionPointer, ExpansionParameter)

		accepts:	a pointer to a code set, a pointer to a depletion code, and an
			expansion parameter

		does:	augments the depletion code with the given expansion parameter
			onto the code set

		returns:	nothing

		********************************************

ShowPDTs:	syntax:	ShowPDTs ( CodePointer )

		accepts:	a pointer to a code set

		does:	displays all the PDTs of the code set

		returns:	nothing

		********************************************

CopyCode:	syntax:	CopyCode ( CodePointer1, CodePointer2 )

		accepts:	two pointers to code sets

		does:	re initilises the code set pointed to by the second pointer
			and copies the code from the first pointer to it

		returns:	nothing

		********************************************

DisposeCode:	syntax:	DisposeCode ( PointerToCodePointer )

		accepts:	the address ( ie: a pointer ) of a code set

		does:	frees the memory of the code set and sets the 
			pointer to null, a message is displayed if an error occurs

		returns:	nothing

		********************************************

TryNextSet:	syntax:	TryNextSet ( CodePointer, IntgerBound, StringPointer, Boolean)

		accepts:	a pointer to a code set, a bound for maximal length
			codes to find, a pointer string to compare maximal length
			 codes to, and a boolean value as to display style

		does:	with all possible expansion parameters goes through 
			all possible prefix's that can be used to form a set with
			maximal length equal to bound.  Recursively tries all
			possible ways of forming subsections within this bound,
			if boolean is true displays charcteristic of all sets with
			maximal length equal to bound, if boolean is false then
			displays only set whose maximal codes match the
			string pointed to.  

		returns:	nothing

		********************************************

DisplayAllLengths:	syntax:	DisplayAllLengths ( CodePointer ) 

		accepts:	a pointer to a code set

		does:	for all maximal code words in the set displays the 
			expansion parameters and the prefixs

		returns:	nothing

		********************************************

DisplayIfMatches:	syntax:	DisplayIfMatches ( CodePointer, StringPointer ) 

		accepts:	a pointer to a code set and a pointer to a string

		does:	for all maximal code words in the set that match 
			the string pointed to display the expansion parameters
			 and the prefixs formatted as s1 e1 s2 e2 ... sn en 

		returns:	nothing

**************************************************************************************/
DepletionSetTypeP NewCode ( DepletionSetTypeP );
void LoadCode ( char *, DepletionSetTypeP );
void SaveCode (char *, DepletionSetTypeP);
void MaximalCode (DepletionSetTypeP, DeplnTypeP);
long CodeSize(DepletionSetTypeP);
int	WordSize(DeplnTypeP , DepletionSetTypeP);	/* returns the word size */
long NextRankWord(long, DepletionSetTypeP, DeplnTypeP);
long FirstRankWord( long, DepletionSetTypeP, DeplnTypeP );
void DecomposeString(char *, int, DepletionSetTypeP);		/* assume string bounded by a null */	
void Augment (DepletionSetTypeP, DeplnTypeP, int);
void ShowPDTs(DepletionSetTypeP);
void CopyCode(DepletionSetTypeP, DepletionSetTypeP);
void DisposeCode(DepletionSetTypeP *);
void TryNextSet (DepletionSetTypeP, int, char *, int);
void DisplayAllLengths (DepletionSetTypeP);
void DisplayIfMatches(DepletionSetTypeP, char*); 	/* display set parameters if reference string matches */


DepletionSetTypeP NewCode ( codeP )
DepletionSetTypeP codeP;
{

int 	i;

	if ( codeP == NULL ) {
		codeP = ( DepletionSetTypeP ) malloc(sizeof(DepletionSetType));
		if ( codeP != NULL ) {
			for (i=0; i<MAXAUG + 1; i++) 
				codeP->PDTs[i] = NULL;
			codeP->AlphabetP = NULL;
		}
	}
	if ( codeP != NULL ) {
		codeP->q = 0;	
		for (i=0; i<MAXAUG + 1; i++) {
			codeP->e[i] = 0;
			codeP->PDTs[i] = NewDepln(codeP->PDTs[i]);
		}
		codeP->AlphabetP = NewAlphabet(codeP->AlphabetP);
	}	
	return (codeP);
}

/****************************************************************************************/

void LoadCode ( file, codeP )
DepletionSetTypeP codeP;
char *file;
{

FILE 	*fp;
int	i, j;
 char t[20];
	fp = fopen ( file, "r" );
	if (fp == NULL) {
		printf("error: invalid file\n");
		exit(-1);
	}
	//if (fscanf ( fp, "%d", &codeP->AlphabetP->Q ) != 1) 
	//printf("Error reading alphabet size\n");
	fscanf(fp, "%19s", t);
	codeP->AlphabetP->Q = atoi(t);
//	printf("read alphabet size %d\n", codeP->AlphabetP->Q);
	for ( i=0; i < codeP->AlphabetP->Q; i++) {
	  fscanf(fp, "%19s", t);
	  codeP->AlphabetP->a[i] = t[0];
	  //fscanf ( fp, "% c\n", &codeP->AlphabetP->a[i] );
//	  printf("read alphabet symbol '%c'\n", codeP->AlphabetP->a[i]);
	};
	fscanf ( fp, "%d\n", &codeP->q );
//	printf("read code size %d\n", codeP->q);
	for ( i=1; i <= codeP->q; i++ ) {
		fscanf ( fp, "%d\n", &codeP->e[i] );
		fscanf ( fp, "%d\n", &codeP->PDTs[i]->size );
		for ( j=0; j < codeP->PDTs[i]->size; j++)
			fscanf ( fp, "%d\n", &codeP->PDTs[i]->w[j] );
	}

	fclose (fp);
}

/****************************************************************************************/

void SaveCode (file, codeP)
char *file;
DepletionSetTypeP codeP;
{

FILE	*fp;
int 	i, j;

	fp = fopen ( file, "w" );

	fprintf ( fp, "%d\n", codeP->AlphabetP->Q );
	for ( i=0; i < codeP->AlphabetP->Q; i++)
		fprintf ( fp, "%c\n", codeP->AlphabetP->a[i] );
	fprintf ( fp, "%d\n", codeP->q );
	//printf("codeP->q = %d, alphabet Q = %d\n", codeP->q, codeP->AlphabetP->Q);
	for ( i=1; i <= codeP->q; i++ ) {
		fprintf ( fp, "%d\n", codeP->e[i] );
		fprintf ( fp, "%d\n", codeP->PDTs[i]->size );
		for ( j=0; j < codeP->PDTs[i]->size; j++)
			fprintf ( fp, "%d\n", codeP->PDTs[i]->w[j] );
	}

	fclose (fp);
}

/****************************************************************************************/

void MaximalCode (codeP, deplnP)
DepletionSetTypeP codeP;
DeplnTypeP deplnP;
{
int i;
	NewDepln(deplnP);
	if (codeP != NULL) {
		deplnP->size = codeP ->q + 1;					/* set size to level of augmentation */
		for ( i=0; i <= codeP->q; i++)
			deplnP->w[i] = codeP->e[i];					/* set all w[i]'s to maximum ie expansion parameter */
	}
}

/****************************************************************************************/

long	CodeSize(CodeP)
DepletionSetTypeP	CodeP;
{
int 	level;
long	size = 0;
	
	if (CodeP != 0L) {
		size = CodeP->AlphabetP->Q - 1;
		for (level = 1; level <= CodeP->q; level++) size *= ( CodeP->e[level] + 1);
		size += 1;	/* plus one */
	}
	return(size);   								/* size = (Q -1) prod(e[i] + 1)  + 1 */
}

/****************************************************************************************/

int	WordSize(DeplnP , CodeP)	/* returns the word size */
DeplnTypeP	DeplnP;
DepletionSetTypeP	CodeP;
{
	return( Encode(DeplnP , CodeP, NULL));
}

/****************************************************************************************/

long	NextRankWord(num, CodeP, DeplnP)
long	num;
DepletionSetTypeP	CodeP;
DeplnTypeP	DeplnP;
{
int	reflength = 1, maxlength, length, done = FALSE;
long	MaxDepln;

	if (CodeP != 0) {
		if (num >= 0) {
			LongToDepln(num++, CodeP, DeplnP);					/* calculate current depletion and go to next num*/
			reflength = WordSize(DeplnP , CodeP);					/* calculate current word length as reference */
		}
		else num = 0;
		MaximalCode(CodeP, DeplnP);						/* returns first of the maximal codes in DeplnP*/
		maxlength = WordSize(DeplnP , CodeP);						/* calculate longest word size in set */
		MaxDepln = DeplnToLong( DeplnP, CodeP) + CodeP->AlphabetP->Q;			/* upper limit to search */
		while (done == FALSE) { 							/* stay in till done == true */
			while ((num < MaxDepln) && (done == FALSE)) {				/* while num between current and max */
				LongToDepln(num, CodeP, DeplnP);				/* convert number to a depletion */
				if ( ChkDeplnCode(DeplnP, CodeP) == TRUE ) { 			/* check for a valid code */
					if (WordSize(DeplnP , CodeP) == reflength) done =TRUE; 	/* if word length same as reference finished */
					else num++; 					/* otherwise move on */
				}
				else num++;						/* not valid so move on */
			}	
			if (done == FALSE) { 						/* if cant find another word of this length */
				reflength++;						/* step up to next word length */
				num = 0;							/* prepare to start search again */
				if (reflength > maxlength) {					/* check if any more word possible */
					done = TRUE; 					/* exhausted search */
					num = -1;					/* return a -1 */
				}
			}
		}
		return((long) num);								/* return next word as a long */
	}
	else return((long) -1L);								/* return error as no code set */
}

long FirstRankWord ( CurrentLong, CodeP, DeplnP )
long CurrentLong;
DepletionSetTypeP CodeP;
DeplnTypeP DeplnP;
{

int Found = FALSE, Length, Carry, i;
char String[MAXPRFXLEN];

	if ( CurrentLong > -1 ) {
		LongToDepln( CurrentLong, CodeP, DeplnP );
		Length = Encode( DeplnP, CodeP, String );
	}
	else {
		String[0] = CodeP->AlphabetP->a[0] - 1;
		String[1] = 0; // NULL
		Length = 1;
	}

	while ( Found == FALSE ) {
		Carry = 1;
		for (i = Length - 1; i >= 0; i-- ) {
			if ( String[i] - CodeP->AlphabetP->a[0] + Carry >= CodeP->AlphabetP->Q ) {
				String[i] = CodeP->AlphabetP->a[0];
				Carry = 1;
				if ( i == 0 ) {
					String[Length++] = CodeP->AlphabetP->a[0];
					String[Length] = 0; // NULL;
					MaximalCode( CodeP, DeplnP );
					if ( Length > Encode( DeplnP, CodeP, NULL ) )
						return(-1);
				}
			}
			else {
				String[i] += Carry;
				Carry = 0;
			}
		}
		if ( DecodeAt( CodeP->q, CodeP, String, DeplnP ) == Length )
			Found = TRUE;
	}
	
	return( DeplnToLong( DeplnP, CodeP ) );
}
/****************************************************************************************/

void DecomposeString(string, lengthstring, CodeP)		/* assume string bounded by a null */	
char	*string;
int	lengthstring;
DepletionSetTypeP	CodeP;
{
int	done, codecount = 0, expansion, codelength, lastexpansion;
char	*endstringptr, *stringptr;
DeplnType		lastDepln, depletion;

	endstringptr = string + lengthstring;
	CodeP->q = 0;									/* start at level 0 */
	if (lengthstring > 1) codecount = 2;							/* can assume that there is more than 1 code at level 0 */
	while (codecount > 1) {								/* increment through all levels so long as there are codes to work with */
		stringptr = string;								/* start at beginning  of  string */
		expansion = 1;								/* reset expansion */
		NewDepln(&lastDepln);							/* reset last depletion */
		codecount = 0;								/* reset counter */
		done = FALSE;
		while (done == FALSE) {							/* go through whole string from beginning */
			codelength = DecodeAt(CodeP->q, CodeP, stringptr, &depletion);		/* decode string at current level till end */
			stringptr += codelength;
			codecount++;							/* increment counter */

			if (endstringptr == stringptr) { 					/* at end of string */
				if (codecount > 1) Augment(CodeP, &lastDepln, expansion);	/* inserts the PDTs starting with the PDT[0] = literal code */
				done = TRUE;						/* done */
			}
			else {
				if (TRUE == CompareDepln(&lastDepln, &depletion, CodeP->q) ) 
					 expansion++;					/* increment */
				else { 							/* new depletion code so make a note */ 
					CopyDepln(&depletion, &lastDepln);  			/* save depletion value*/
					expansion = 1;					/* reset expansion counter to one*/
				}
			}
		}
	}
}

/****************************************************************************************/

void Augment (codeP, deplnP, expansion)
DepletionSetTypeP	codeP;
DeplnTypeP	deplnP;
int		expansion;
{
int i;

	codeP->q++;
	codeP->e[codeP->q] = expansion;
	CopyDepln(deplnP, codeP->PDTs[codeP->q]);
}

/****************************************************************************************/

void ShowPDTs(codeP)
DepletionSetTypeP	codeP;
{
int i, j;

	printf(" q = %d\n", codeP->q);
	for (i=1; i<=codeP->q; i++) {
		printf(" e[%d] = %d\n", i, codeP->e[i]);
		printf(" PDT[%d]->size = %d\n", i, codeP->PDTs[i]->size);
		for (j=0; j<codeP->PDTs[i]->size; j++)
			printf(" PDT[%d]->w[%d] = %d\n", i, j,codeP->PDTs[i]->w[j]);
	}
}

/****************************************************************************************/

void CopyCode(Code1P, Code2P)
DepletionSetTypeP	Code1P, Code2P;
{
int	i;

	NewCode(Code2P);
	CopyAlphabet(Code1P->AlphabetP, Code2P->AlphabetP);				/* copy the alphabet */
	Code2P->q = Code1P->q;							/* copy level of augmentation */
	for (i = 1; i <= Code2P->q; i++) {
		 Code2P->e[i] = Code1P->e[i];					/* copy expansion paramters */
		CopyDepln(Code1P->PDTs[i], Code2P->PDTs[i]);				/* copy PDTs */
	}
}

/****************************************************************************************/

void DisposeCode(CodePP)
DepletionSetTypeP	*CodePP;
{	
int i;
	if (*CodePP != NULL) {
		DisposeAlphabet(&((*CodePP)->AlphabetP));				/* dispose alphabet */
		for (i=0; i<MAXAUG + 1; i++)
			DisposeDepln(&((*CodePP)->PDTs[i]));				/* dispose PDTs */
		free(*CodePP);							/* dispose code */
		*CodePP = NULL;							/* set codeP = NULL */
	}
}
/****************************************************************************************/

void TryNextSet (CodeP, bound, refstring, Showall)
DepletionSetTypeP	CodeP;
int	bound, Showall;
char	*refstring;
{ 
char		prefix[MAXPRFXLEN];
DepletionSetTypeP	NewCodeP = 0L;
DeplnType	Depletion;
int	expansion = 1, prefixlength, DoExpnLoop = TRUE, DoWordLoop;
long	longnumber;

	NewCodeP = NewCode(NewCodeP);								/* create temporary code area*/
	while (DoExpnLoop == TRUE) {								/* cycle through possible expansion parameters */
		longnumber = -1;									/* reset current rankword to get shortest */
		DoWordLoop = TRUE;					
		while (DoWordLoop == TRUE) {							/* keep going until overflow bound */
			if ((longnumber = FirstRankWord(longnumber, CodeP, &Depletion)) >= 0) {		/* get next longest word from code as prefix */
				prefixlength = WordSize(&Depletion , CodeP);				/* get size of prefix */
				if (bound > prefixlength * expansion) { 					/* if augmenting won't fill bound size*/
					CopyCode(CodeP,NewCodeP); 				/* make a copy of the code */
					Augment(NewCodeP, &Depletion, expansion);			/* augment with prefix */
					TryNextSet(NewCodeP, bound - expansion * prefixlength, refstring, Showall ); 	/* recursively scansets with remaining bound */
				}
				else if (bound == prefixlength * expansion) {				/* if augmenting will fill bound size exactly */
					CopyCode(CodeP,NewCodeP); 				/* make a copy of the code */
					Augment(NewCodeP, &Depletion, expansion);			/* augment with prefix */
					if (Showall == TRUE)
						DisplayAllLengths(NewCodeP);			/* display maximal characteristics */
					else
						DisplayIfMatches(NewCodeP, refstring);		/* display characterisics of matching strings only */
				}
				else 								/* if augmenting will overflow bound */
					DoWordLoop = FALSE; 					/* only after all possible prefixes have been tried */
			}
			else 
				DoWordLoop = FALSE; 						/* gone through all possible prefixs */
		}
		expansion += 1;									/* try greater expansion parameter */
		longnumber = -1;									/* reset current rankword */
		longnumber = FirstRankWord(longnumber, CodeP, &Depletion); 				/* find smallest word */
		prefixlength = WordSize(&Depletion , CodeP);						/* get length of smallest word */
		if (bound < expansion * prefixlength)							/* if not possible to augment within bound */
		 	DoExpnLoop = FALSE; 							/* finished all posible sets */
	} /* end of loop for each expansion parameter	*/
	DisposeCode(&NewCodeP);									/* dispose temporary code area */  

}

/*************************************************************************************/

void DisplayAllLengths (codeP) 
DepletionSetTypeP	codeP;
{
char		prefix[MAXPRFXLEN];
DeplnType	depln;
int	 	i, j, length;

	MaximalCode(codeP, &depln);						/* get first maximal length code */
	for (j = 1; j <= codeP->AlphabetP->Q; j++) { 					/* for each of the maximal length codes */
		length = Encode(&depln, codeP, prefix);				/* convert code to string */
		prefix[length] = 0;							/* set null at end of string */
		printf("%s\t( ", prefix);						/* display the string */

		for (i = 1; i <= codeP->q; i++) {					/* for each level of augmentation */
			if (i > 1) printf(", ");						/* display with commas between them */
			printf("%d", codeP->e[i] );					/* the expansion parameters */
		}
		printf( " )\t( ");
		for (i = 1; i <= codeP->q; i++) {					/* for each level of augmentation */
			if (i > 1) printf(", ");						/* display with commas between them */
			length = Encode(codeP->PDTs[i], codeP, prefix); 			/* convert PDTs to prefix */
			prefix[length] = 0;						/* set null at end of string */
			printf("%s", prefix);					/* the prefix's */
		}
		printf( " )\n");							/* display closing bracket and new line */
			
		depln.w[0]++;							/* get ready for next maximal length code */
	}
}

/*************************************************************************************/

void DisplayIfMatches(CodeP, refstring) 							/* display set parameters if reference string matches */
DepletionSetTypeP	CodeP;
char	refstring[MAXPRFXLEN];
{
char		codestring[MAXPRFXLEN];
DeplnType	Depln;
int	 	i, j, length;

	MaximalCode(CodeP, &Depln);						/* gets first of the maximal codes in Depletion*/
	for (i = 1; i <= CodeP->AlphabetP->Q; i++) {					/* for each of the maximal length codes */			
		length = Encode(&Depln, CodeP, codestring);
		codestring[length] = 0;
		if ( strcmp(refstring, codestring) == 0L ) {
			for (j= 1; j <= CodeP->q; j++) {				/* for each level of augmentation */
				length = Encode(CodeP->PDTs[j], CodeP, codestring); 	/* convert PDTs to prefix */
				codestring[length] = 0;				/* set null at end of string */
				printf("%s %d ", codestring, CodeP->e[j] );		/* show the prefixs  an expansions*/
			}
			printf( "\n");						/* display new line */
		}
		Depln.w[0]++;							/* get ready for next maximal length code */
	}
}

/*************************************************************************************/
