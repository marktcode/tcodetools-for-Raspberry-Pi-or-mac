/*****************************************************************************************

 	Depletion Code Routines  				

	Written by:	Scott Wackrow
	Date:		Dec 1994	

******************************************************************************************

NewDepln:	syntax:	NewDepln ( DepletionCodePointer )

		accepts: a pointer to a depletion code, initilised or not

		does:	if pointer is not initilsed allocates memory
			clears all fields and sets size = 0

		returns:	the pointer to the depletion code

		********************************************

Compare:		syntax:	Compare ( LevelToStartAt, CodePointer, StringPointer )

		accepts:	a level to start comparisions at, a pointer to a code 
			to compare the string with, and a pointer to the string 
			to compare

		does:	recursively checks the string against successively lower 
			prefixs to determine if sections of the string match the 
			prefix or not

		returns:	length of string successfully matched, 0 if no match

		********************************************

DecodeAt:	syntax:	DecodeAt ( LevelToStartAt, CodePointer, StringPointer, DepletionPointer )

		accepts:	a level to start decoding at, a pointer to a code to decode 
			with respect to, a pointer to the string to decode, and a
			pointer to a depletion code to decode the string into

		does:	calls Compare to determine if the string matches various
			prefixs, recording the results in the depletion code

		returns:	length of string successfully encoded or -1 if an error 
			has occured

		********************************************

StringToDepln:	syntax:	StringToDepln (CodePointer, StringPointer, DepletionPointer )

		accepts:	a pointer to a code to use, a pointer to the string to convert
			and a pointer to the depletion code to convert into

		does:	calls DecodeAt at the highest level of the code ( ie: at q )

		returns: 	length of string successfully converted or -1 if an error
			has occured

		********************************************

ChkDeplnCode:	syntax:	ChkDeplnCode ( DepletionPointer, CodePointer )

		accepts:	a pointer to a depletion code to check and a pointer
			to a code to check it against

		does:	checks if the depletion code is valid and doesn't
			already exist in the code set

		returns:	true if valid or false if not valid

		********************************************

Encode:		synatx:	Encode ( DepletionPointer, CodePointer, StringPointer )

		accepts:	a pointer to a depletion code to encode, a pointer to a
			code to encode it by and a pointer to a string to encode
			the depletion code into

		does:	converts the depletion code into a string, if pointer to the
			string is null doesn't create string but still returns length

		returns:	the length of the string encoded

		********************************************

DeplnToLong:	syntax:	DeplnToLong ( DepletionPointer, CodePointer )

		accepts:	a pointer to a depletion code and a pointer to the
			code to convert it with referance to

		does:	converts the depletion code into a long number

		returns:	the long number representation

		********************************************

LongToDepln:	synatx:	LongToDepln ( LongNumber, CodePointer, DepletionPointer )

		accepts:	a long number representing a depletion code, a pointer to
			a code to convert it with referance to, and a pointer to a
			depletion code for the result

		does:	converts the long number representation back into a depletion
			code

		returns:	nothing

		********************************************

CopyDepln:	syntax:	CopyDepln (DepletionPointer1, DepletionPointer2 )

		accepts:	two pointers to depletion codes

		does:	copies the code at the first pointer into the code at the second pointer

		returns:	nothing

		********************************************

CompareDepln:	syntax:	CompareDepln ( DepletionPointer1, DepletionPointer2, LevelToCompareTo )

		accepts:	two pointers to depletion codes, and a level to
 			which to compare them
	
		does:	compares the two codes to a given level ( rather
			specific to the needs of decomposition )

		returns:	true if they are the same, false if they are not

		********************************************

DisposeDepln:	syntax:	DisposeDepln ( PointerTodepletionPointer )

		accepts:	the address ( ie: a pointer ) of a depletion pointer

		does:	frees the memory of the depletion code and sets the 
			pointer to null, a message is displayed if an error occurs

		returns:	nothing

*****************************************************************************************/

DeplnTypeP NewDepln ( deplnP )
DeplnTypeP deplnP;
{

int 	i;

	if ( deplnP == NULL ) 
		deplnP = ( DeplnTypeP ) malloc(sizeof(DeplnType));
	if ( deplnP != NULL ) {
		deplnP->size = 0;
		for (i=0; i<MAXAUG; i++)
			deplnP->w[i] = 0;
	}
	return (deplnP);
}

/****************************************************************************************/

int	Compare(level , CodeP, string)	
int	level;
DepletionSetTypeP	CodeP;
char	*string;
{
int count, countr, length = 0, n, prefsz;
DeplnTypeP DeplnP;

	if (CodeP != NULL) {											/* check code exists */
		DeplnP = CodeP->PDTs[level]; 								/* get the PDT code for this level */
		if (DeplnP->size >= 1) { 									/* now look at PDT code string if it exists ie: size > 0 */			
			for (count = DeplnP->size; count >= 1; count--) { 					/* look at each field starting with the most significant */
				if (count >=  2) { 								/* looking at prefix selection code fields or literal ie: w[0] */
					if ((n = DeplnP->w[count - 1] ) != 0) {					/* if the prefix is selected ie: > 0 */
						countr = 1;
						while (countr <= n) { 					/* check for prefix  up to n times */
							if ((prefsz = Compare(count-1, CodeP, string)) == 0) 
								return(0); 				/* if prefix doesn't match no need to continue*/
							string += prefsz; 					/* exclude section of string just matched */
							length += prefsz;					/* add to length of matched string */
							countr++; 					/* loop */
						}
					}
				}
				else if (*string != CodeP->AlphabetP->a[DeplnP->w[0]] ) 				/* if at w[0] check this matches character in string */
					return (0);							/* if not then not this prefix so return */
				else length++;  								/* if it is this character is valid so increment length */
			} 										/* end of for loop */
		}
		else return (0); 										/* no PDT code at this level */
	}
	return (length);											/* return length successfully matched */
}

/****************************************************************************************/

int DecodeAt(level , CodeP, string, DeplnP)	
int	level;
DepletionSetTypeP	CodeP;
char	*string;
DeplnTypeP		DeplnP;
{
int	i, length = 0,  prfxlen;

	DeplnP = NewDepln(DeplnP);							/* initilise depletion code*/

	while (level > 0) {
		prfxlen = Compare(level, CodeP, string);				/* get length of valid prefix at this level */
		if (prfxlen > 0) {							/* if this prefix was in string ie: prfxlen > 0 */
			if (DeplnP->size == 0) DeplnP->size = level;			/* if first matching prefix set depln size ie: highest included prefix level */
			DeplnP->w[level]++;					/* increment the times the prefix is included */
			if (DeplnP->w[level] == CodeP->e[level]) 			/* check if found maximum possible times ie: e times */
				level--; 						/* if so move to next level down */
			string += prfxlen;						/* exclude section of string just matched */
			length += prfxlen;						/* add to length of matched string */
		}
		else
			 level--;	 						/* prefix not in string so move immediately to next level down */
	}
	DeplnP->w[0] = *string - CodeP->AlphabetP->a[0];				/* make last character an index to the alphabet ie: subtract value in a[0] */
	if ((DeplnP->w[0] < 0) || (DeplnP->w[0] >= CodeP->AlphabetP->Q) ) 			/* check if the index is valid */
		return(-1); 							/* if not return error */
	else { DeplnP->size++; length++; }						/* add to length and make size at least one */
	return(length);								/* return length of valid string */
}

/****************************************************************************************/

int StringToDepln(CodeP, string, DeplnP)
DepletionSetTypeP	CodeP;
char	*string;
DeplnTypeP DeplnP;
{
	return ( DecodeAt (CodeP->q, CodeP, string, DeplnP) );
}

/****************************************************************************************/

int	ChkDeplnCode(DeplnP, CodeP)
DeplnTypeP	DeplnP;
DepletionSetTypeP	CodeP;
{
int 	level, i, cmp;
	
	if (DeplnP->size > CodeP->q + 1)  return(FALSE); 							/* check size not greater than level of augmentation */
	if ((DeplnP->w[0] < 0) ||  (DeplnP->w[0] >= CodeP->AlphabetP->Q ))  return(FALSE); 			/* check index to alphabet is valid */
	if (CodeP->q > 0) { 										/* check if it matches any of the other PDT's */
		for (level = 1; level <= CodeP->q; level++) { 						/* check each level PDT */ 
			cmp = TRUE;								/* assume equal until proven otherwise */
			i = 0; 
			while ((cmp == TRUE) && (i < level) ) {						/* step through each w[i], check up to but not including level */
												/* three cases occur ... */

												/* level being checked exists in both PDT and depln being checked */
				if ((i < DeplnP->size) && (i < (CodeP->PDTs[level])->size)) {
					if (DeplnP->w[i] != (CodeP->PDTs[level])->w[i]) cmp = FALSE; 	/* different */
				}
												/* level being checked exists only in depln, PDT assumed to be 0 */
				else if ((i < DeplnP->size) && (i >= (CodeP->PDTs[level])->size)) {
					if (DeplnP->w[i] != 0) cmp = FALSE;
				}
												/* level being checked exists only in PDT, depln assumed to be 0 */
				else if ((i >= DeplnP->size) && (i < (CodeP->PDTs[level])->size)) {
					if ((CodeP->PDTs[level])->w[i] != 0) cmp = FALSE;
				}
				i++; 								/* on to next w[i] */
			}
			if ((cmp == TRUE)  && (DeplnP->w[level] == CodeP->e[level]))			/* check final level is equal to expansion parameter */
				cmp = FALSE; 

			if (cmp == TRUE) return(FALSE); 						/* if it matches a PDT then not a valid code */
		}
		return(TRUE); 									/* did not match any PDT's so checks ok */
	}
	return(TRUE);
}

/****************************************************************************************/

int	Encode(DeplnP , CodeP, string)	/* encodes  Depln code  pointed to by DeplnP to string via CodeH */
DeplnTypeP	DeplnP;
DepletionSetTypeP	CodeP;
char	*string;
{
int	level, countr, length = 0, n, prefsz;

	if ((DeplnP != NULL) && (CodeP != NULL)) {
		if (DeplnP->size >= 1) { 										/* if there is something to encode */							/* only if depletion code contains fields */
			for (level = DeplnP->size; level >= 1; level--) { 							/* look at each field starting with the most significant */
				if (level >=  2) { 									/* if a prefix selection field vs literal field ie: w[0] */				/* prefix selection code fields... */
					if ((n = DeplnP->w[level - 1] ) != 0) { 						/* if a prefix is selected */
						for (countr = 1; countr <= n; countr++) { 				/* get prefix n times */
							length += (prefsz = Encode(CodeP->PDTs[level - 1], CodeP, string)); 
							if (string != NULL) string += prefsz; 				/* move past encoded section in string, noting length */
						}
					}
				}
				else {  										/* check the literal code in range of alphabet*/
					if ((DeplnP->w[0] >= 0) && (DeplnP->w[0] < CodeP->AlphabetP->Q)) {
						if (string != NULL) 							/* if valid pointer to string insert character from alphabet */
							*string++ = CodeP->AlphabetP->a[DeplnP->w[0]];  
						length++;  							/* increment length */
					}
					else length = 0; 								/* length set to zero */
				}   
			}
		}
	}
	return (length);
}

/****************************************************************************************/

long	DeplnToLong( DeplnP, CodeP)
DeplnTypeP DeplnP;
DepletionSetTypeP	CodeP;
{
int	i;
long	deplnLong = 0;

	if (DeplnP->size > CodeP->q   + 1)  return((long) -1); 						/* error: size of depletion larger than CodeH degree q*/
	for (i = DeplnP->size - 1; i >= 0 ; i--) {								/* for each level in depln starting with most significant */
		if (i > 0) {										/* prefix selection */
			if ((CodeP->e[i] > 0) && ( DeplnP->w[i] <= CodeP->e[i])) {				/* if prefix exists and w[i] in range */
				deplnLong = ( deplnLong * (CodeP->e[i] + 1) ) + DeplnP->w[i];		/* multiply out */
			}
			else return ((long) -1);							/* out of range field */
		}
		else { 										/* literal code */
			if (  ( CodeP->AlphabetP->Q > 0) && ( DeplnP->w[0] < CodeP->AlphabetP->Q)) {
				deplnLong = ( deplnLong * CodeP->AlphabetP->Q  ) + DeplnP->w[0];		/* multiply out */
			}
			else return ((long) -1);							/* out of range field */
		}
	}
	return( (long) deplnLong);									/* return answer */
}

/****************************************************************************************/

void LongToDepln(deplnLong, CodeP, DeplnP)
long		deplnLong;
DeplnTypeP	DeplnP;
DepletionSetTypeP	CodeP;
{
int	i;
long	tempLong;
	NewDepln(DeplnP); 									/* gives DeplnP->size = 0 */
	DeplnP->size = 1; 										/* always have a literal code */
	for (i = 0; i <= CodeP->q ;  i++) {
		if (i == 0) {
			if ( CodeP->AlphabetP->Q != 0) {						/* if dealing with literal */
				tempLong = deplnLong / CodeP->AlphabetP->Q; 				/* divides it down */
				DeplnP->w[0] = deplnLong - (tempLong * CodeP->AlphabetP->Q);		/* takes remainder */
				DeplnP->size = 1;							/* sets size */
				deplnLong = tempLong;						/* store divided long */
			}
		}
		else if (CodeP->e[i] != 0) { 								/* e[0] = Q - 1 */
			tempLong = deplnLong / (CodeP->e[i] + 1); 					/* divides it down */
			DeplnP->w[i] = deplnLong - (tempLong * (CodeP->e[i] + 1));			/* takes remainder */
			if (DeplnP->w[i] != 0) DeplnP->size = i +1;					/* if remainder not zero sets size */
			deplnLong = tempLong;							/* store divided long */
		}
	}
}

/****************************************************************************************/

void CopyDepln(Depln1P,Depln2P)
DeplnTypeP	Depln1P, Depln2P;
{
int	i;
		NewDepln(Depln2P);						/* clear depln */
		Depln2P->size = Depln1P->size;					/* copy size */
		for (i = 0; i < Depln1P->size; i++) 
			Depln2P->w[i] = Depln1P->w[i];				/* copy w[i] */
}

/*************************************************************************************/

int	CompareDepln(Depln1P,Depln2P, level)
DeplnTypeP	Depln1P, Depln2P;
int	level;
{
int 	i = 0;
	if ( Depln1P->size != Depln2P->size ) return (FALSE); 				/* check if differant sizes */
	while ((i <= level) && (i < Depln1P->size)) {
		if ( Depln1P->w[i] != Depln2P->w[i] ) return (FALSE); 			/* compare w[i]'s up to level or size */
		i++;
	}
	return(TRUE);
}

/****************************************************************************************/

void DisposeDepln (deplnPP)
DeplnTypeP	*deplnPP;
{
	if ( *deplnPP != NULL ) {
		free(*deplnPP);
		*deplnPP = NULL;
	}
}

/*************************************************************************************/

