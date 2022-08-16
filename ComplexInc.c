/*****************************************************************************************

 	Complexity Tree Routines  				
	
	Written by:	Scott Wackrow
	Date:		Dec 1994	
	Modified:		May 1995		LoadTree, load and check alphabet size
					SaveTree, save alphabet size

******************************************************************************************

LoadTree:	usage:	LoadTree ( StringPointer, PointerToTreePointer, boolean, AlphabetSize )

		accepts:	pointer to a string containing the filename, pointer to a
			tree pointer - expected to be empty, a boolean value
			as to wether to show headings or not, and the size of the
			alphabet being used

		does:	load the complexity tree from the file named, showing
			progress if boolean value true, calls load node, checks if
			this tree stores complexity values calculated with this 
			size alphabet

		returns:	nothing

		********************************************

LoadNode:	usage:	LoadNode ( PointerToTreePointer, NodePointer, boolean )

		accepts:	pointer to the tree pointer, a pointer to the node to
			insert in the tree and a boolean value 

		does:	inserts node pointed to into binary complexity tree
			pointed to, if boolean true will show progress '.'
			recursively calls until found position to insert

		returns:	nothing
	
		********************************************

SaveTree:	usage:	SaveTree ( StringPointer, TreePointer, boolean, AlphabetSize )

		accepts:	pointer to a string containing the filename, a pointer
			to the complexity tree to save, boolean value about
			displaying progress, and Size of the alphabet used

		does:	saves complexity tree in file named, showing progress
		 	if boolean value true, calls save node

		returns:	nothing

		********************************************

SaveNode:	usage:	SaveNode ( FilePointer, TreePointer, boolean )

		accepts: 	pointer to the file to save node in, pointer to the
			complexity tree, boolean value for progress indicator

		does:	recursively performs an in oreder traversal of the tree
			showing progress if boolean is true

		returns:	nothing

		********************************************

DisposeTree:	usage:	DisposeTree ( PointerToTreePointer )

		accepts:	pointer to a complexity tree pointer

		does:	frees memory used by the complexity tree, and sets
			the pointer to the tree to NULL

		returns:	nothing

		********************************************

FndInsert:	usage:	FndInsert ( PointerToTreePointer, CodePointer, Tcomplexity )

		accepts:	pointer to a complexity tree pointer, pointer to a code
			set and a T complexity value

		does:	searches the complexity tree and if structure found returns
			its complexity, otherwise if there is a complexity value to be
			added, does so
		
		returns:	complexity value if found otherwise 0 or complexity value passed
			in if an insertion was made

		********************************************

CompareStruct:	usage:	CompareStruct ( CodePointer, TreePointer )

		accepts:	pointer to a code set, and pointer to a complexity
			tree

		does:	compares the structures of each and finds if the code structure is 
			less than, greater than or equal to the structure in the tree

		returns:	-1 if less than, 0 if equal, or 1 if greater than

		********************************************

EnlargeSet:	usage:	EnlargeSet ( CodePointer, Level, boolean )

		accepts: 	pointer to the code set, a level for recursive call set to 1 when
			first calling, and a boolean for showing structure

		does:	goes through all levels of augmentation, if the expansion parameter
			is factorisable will factorise into two parts, then uses a recursive call
			to check and expand the new factors

		returns:	the complexity of the structure of the code set

		********************************************

MinSearch:	usage:	MinSearch ( NewCodePointer, CodePointer )

		accepts:	two pointers to code sets, the first being a newly initilised set

		does:	finds the first smallest code of the same structure as the code 
			set pointed to and inserts it at the new code set pointer

		returns:	the length of the longest word of the new set as an integer

		********************************************

LeastNum:	usage:	LeastNum ( FirstInteger, SecondInteger )

		accepts:	two integer numbers

		does:	if the first number is zero returns the second number,
			otherwise returns the smallest of the two numbers

		returns:	the smallest number

		********************************************

factor:		usage:	factor ( Number, LastFactor )

		accepts:	the number to find factors of and the last factor found

		does:	finds the next factor of the number

		returns:	the next factor or 0 if there are no more

		********************************************

ShowStructure:	usage:	ShowStructure ( CodePointer )

		accepts:	a pointer to a code set

		does:	displays the structure of the code set

		returns:	nothing

*****************************************************************************************/
void LoadTree(char *, ComplexityP *, int, int);
void LoadNode(ComplexityP *, ComplexityP, int);
void SaveTree(char *, ComplexityP, int, int );
void SaveNode(FILE *, ComplexityP, int);								/* in order traversal */
void DisposeTree (ComplexityP *);
int	FndInsert(ComplexityP *, DepletionSetTypeP , int );
int	CompareStruct(DepletionSetTypeP, ComplexityP);
int	EnlargeSet(DepletionSetTypeP, int, int);
int	MinSearch(DepletionSetTypeP, DepletionSetTypeP );
int	LeastNum(int, int);
int	factor(int, int);
void ShowStructure(DepletionSetTypeP); /* temp routine */





void LoadTree(file, treePP, heading, AlphabetSize)
char *file;
ComplexityP *treePP;
int heading, AlphabetSize;
{
FILE *fp;
ComplexityType node;
int i;
	fp = fopen(file, "r");							/* open file */
	if (fp != NULL) {								/* test if file opened */
		if ( heading == 1 ) 
			printf( "loading tree ");					/* display heading */
		fscanf(fp, "%d\n", &i);						/* load size of alphabet */
		if ( i != AlphabetSize ) {
			printf("\nerror: Tree for alphabet of size %d expected, tree of alphabet size %d specified\n", AlphabetSize, i );
			exit(-1);
		}
		while ( feof(fp) == FALSE ) {						/* until end of file */
			fscanf(fp, "%d\n", &node.q);
			for (i=1; i<node.q+1; i++)
				fscanf(fp, "%d\n", &node.e[i]);
			fscanf(fp, "%d\n", &node.MinComplexity);			/* load node details */
			LoadNode(treePP, &node, heading);				/* insert node into tree */
		}
		fclose(fp);							/* close the file */
		if (heading==TRUE) printf("\ttree loaded\n\n");				/* finish heading */
	}
}

/****************************************************************************************/

void LoadNode(treePP, nodeP, heading)
ComplexityP *treePP, nodeP;
int heading;
{
int i, result;
	if (*treePP == NULL) { 							/* if tree / subtree empty */
		if (heading==TRUE) printf(".");
		*treePP = (ComplexityP) malloc(sizeof(ComplexityType));			/* allocate memory */
		(*treePP)->q = nodeP->q;						/* copy level of augmentation */
		for (i = 1; i<= nodeP->q; i++)
			 (*treePP)->e[i] = nodeP->e[i];				/* copy structure */
		(*treePP)->MinComplexity = nodeP->MinComplexity;			/* insert complexity */
		(*treePP)->NextR = (*treePP)->NextL = NULL;				/* zero pointers */
	}
	else { 									/* continue search */
		result = CompareStruct(nodeP, *treePP);				/* compare the code and tree structures */
		if (result < 0)
	 		LoadNode(&(*treePP)->NextL, nodeP, heading);			/* if code structure less than tree structure go left */
		else if (result > 0) 
			LoadNode(&(*treePP)->NextR, nodeP, heading);			/* if code structure greater than tree structure go right */
	}
}

/****************************************************************************************/

void SaveTree(file, treeP, heading, AlphabetSize )
char *file;
ComplexityP treeP;
int heading, AlphabetSize;
{
FILE *fp;

	if (heading == TRUE) printf("ready to merge trees\n");
	LoadTree(file, &treeP, heading, AlphabetSize );					/* load tree and merge changes */
	if (heading==TRUE) printf("saving tree ");					/* show heading */
	fp = fopen(file, "w");							/* open the file */
	fprintf(fp, "%d\n", AlphabetSize );						/* save the alphabet size */
	SaveNode(fp, treeP, heading);						/* save the tree */
	fclose(fp);								/* close the file */
	if (heading==TRUE)printf("\ttree saved\n\n");					/* finish the heading */
}

/****************************************************************************************/

void SaveNode(fp, treeP, heading)								/* in order traversal */
FILE *fp;
ComplexityP treeP;
int heading;
{
int i;

	if (treeP != NULL) {								/* while stuff to save */
		if (heading==TRUE) printf(".");					/* working indicator */
		fprintf(fp, "%d\n", treeP->q);
		for (i=1; i<treeP->q+1; i++)
			fprintf(fp, "%d\n", treeP->e[i]);
		fprintf(fp, "%d\n", treeP->MinComplexity);				/* save current node */
		SaveNode(fp, treeP->NextL, heading);					/* save left node */
		SaveNode(fp, treeP->NextR, heading);					/* save right node */
	}
}

/****************************************************************************************/

void DisposeTree (treePP)
ComplexityP *treePP;
{
	if (*treePP != NULL) {							/* while not null */
		DisposeTree(&((*treePP)->NextL));					/* dispose of children */
		DisposeTree(&((*treePP)->NextR));
		free (*treePP); 							/* free node */
		*treePP = NULL;							/* reset pointer */
	}
}

/****************************************************************************************/

int	FndInsert(treePP, codeP , Tcomplexity )
ComplexityP	*treePP;
DepletionSetTypeP	codeP;
int Tcomplexity;
{
int	complexity = 0, result, i;

	if (*treePP == NULL) { 							/* if tree / subtree empty */
		if (Tcomplexity != 0) {						/* if complexity data to insert */
			*treePP = (ComplexityP) malloc(sizeof(ComplexityType));		/* allocate memory */
			(*treePP)->q = codeP->q;					/* copy level of augmentation */
			for (i = 1; i<= codeP->q; i++)
				 (*treePP)->e[i] = codeP->e[i];			/* copy structure */
			complexity = (*treePP)->MinComplexity = Tcomplexity;		/* insert complexity */
			(*treePP)->NextR = (*treePP)->NextL = NULL;			/* zero pointers */
		}
	}
	else { 									/* continue search */
		result = CompareStruct(codeP, *treePP);				/* compare the code and tree structures */
		if (result < 0)
	 		complexity = FndInsert(&(*treePP)->NextL, codeP , Tcomplexity);	/* if code structure less than tree structure go left */
		else if (result > 0) 
			complexity = FndInsert(&(*treePP)->NextR, codeP , Tcomplexity);	/* if code structure greater than tree structure go right */
		else 
			complexity = (*treePP)->MinComplexity;			/* if structure the same return the found complexity */
	}
	return(complexity);							/* if already found in tree then don't add */
}

/****************************************************************************************/

int	CompareStruct(codeP, treeP)
DepletionSetTypeP	codeP;
ComplexityP	treeP;
{
int	i;

	if ((treeP != NULL ) && ( codeP != NULL )) {					/* if pointers not null */
		if (codeP->q > treeP->q) return(1);					/* structure with highest level of augmentation */
		else if (codeP->q < treeP->q) return(-1);				/* is on the right side of the tree */
		else {								/* if augmentation the same */
			for (i = 1; i <= treeP->q; i++) {				/* for all levels */
				if (treeP->e[i] < codeP->e[i]) return(-1);		/* structure with lowest level of expansion */
				else if (treeP->e[i] > codeP->e[i]) return(1);		/* is on the right side of the tree */
			}
		}
	}
	return(0);									/* otherwise they are the same */
}

/****************************************************************************************/

int	EnlargeSet(CodeP, level, show)
DepletionSetTypeP	CodeP;
int	level, show;
{
int	i,j, newexpansion, BestNum = 0, NewNum;
DepletionSetTypeP	TempCode1P = 0L, TempCode2P = 0L, BestCodeP = 0L;
char	prefix[MAXPRFXLEN];

	BestCodeP = NewCode(BestCodeP);										/* initilise new code */
	CopyAlphabet(CodeP->AlphabetP, BestCodeP->AlphabetP);								/* copy alphabet for use */

	NewNum = MinSearch(BestCodeP, CodeP) ;									/* do best code search */
	if (show == 2 || show == 3 ) ShowStructure(BestCodeP);								/* display the structure of the first new set */
	BestNum = LeastNum(BestNum, NewNum); 									/* returns whichever is the smaller */

	if (CodeP->q > 0) {												/* if the code is augmented ... */
		TempCode1P = NewCode(TempCode1P);								/* create two temporary work areas */
		TempCode2P = NewCode(TempCode2P);
		for ( i = level; i <= CodeP->q; i++) {									/* for each level of augmentation */
			if (CodeP->e[i] >= 3) { 									/* if expansion parameter factorisable, ie: at least 4 */
				newexpansion = 1;									/* start looking for factors */
				while (newexpansion != 0) {								/* while there are still factors */
					newexpansion = factor((CodeP->e[i] + 1), newexpansion); 				/* get the next factor */
					if (newexpansion != 0)  {							/* if we have a valid factor ... */
						CopyCode(CodeP, TempCode1P);					/* create a copy of the origional code */
						for (j = CodeP->q; j >= i; j--) {					/* for all levels of augmentation above this one */
							TempCode1P->e[j + 1] = TempCode1P->e[j]; 			/* move PDTs and e's up one level */ 
							CopyDepln(TempCode1P->PDTs[j], TempCode1P->PDTs[j+1] );	/* to make space for new level */
						}
						TempCode1P->e[i+1] = (( TempCode1P->e[i] + 1) / newexpansion ) - 1;	/* old level expansion is second factor */
						TempCode1P->e[i] = newexpansion - 1; 				/* new level expansion is first factor */			
						TempCode1P->PDTs[i+1]->w[i] = TempCode1P->e[i]; 			/* old level PDT includes new level first factor times */
														/* eg: e = 9 => 10 : 2/5   => 1/4 */
						TempCode1P->PDTs[i+1]->size = i + 1;					/* set size of old level PDT to include new level */
						TempCode1P->q++;							/* increase level of augmentation for new level added*/
							
						CopyCode(TempCode1P, TempCode2P);					/* copy code with the new level already in */
						TempCode2P->e[i] = TempCode1P->e[i+1];				/* new level expansion is second factor */
						TempCode2P->e[i + 1] = TempCode1P->e[i];				/* old level expansion is first factor */
						TempCode2P->PDTs[i+1]->w[i] = TempCode2P->e[i];			/* old level PDT includes new level second factor times */
						TempCode2P->PDTs[i+1]->size = i + 1;					/* set size of old level PDT to include new level */
						for (j = i + 2; j <= TempCode1P->q; j++) { 				/* correct all levels above those adjusted */
							Encode(CodeP->PDTs[j - 1], CodeP, &prefix[0]); 			/* get each origional prefix */
							DecodeAt(j -1 , TempCode1P, &prefix[0], TempCode1P->PDTs[j]); 	/* decode to first new set */
							DecodeAt(j -1 , TempCode2P, &prefix[0], TempCode2P->PDTs[j]); 	/* decode to second new set */
						}

						if (show==TRUE || show == 2 ) ShowStructure(TempCode1P);				/* display the structure of the first new set */
						NewNum = EnlargeSet(TempCode1P, i +1, show);				/* enlarge new factors in first new set */
						BestNum = LeastNum(BestNum, NewNum); 				/* keep track of smallest complexity */
	
						if (TempCode2P->e[i] != TempCode2P->e[i + 1]) {				/* if the two factors are differant */
							if (show==TRUE || show == 2 ) ShowStructure(TempCode2P);			/* display the structure of the second new set */
							NewNum = EnlargeSet(TempCode2P, i +1, show);			/* enlarge new factors in second new set */
							BestNum = LeastNum(BestNum, NewNum);	 		/* keep track of smallest complexity */
						}
					} /* if valid factor */
				} /* while more factors */
			} /* if expansion factorisable */
		} /* for each level of augmentation */
		DisposeCode(&TempCode1P);										/* dispose temporary work areas */
		DisposeCode(&TempCode2P);
	}

	DisposeCode(&BestCodeP);											/* dispose working code set */
	return(BestNum);												/* return minimum complexity */
}

/****************************************************************************************/
/************************************************************
  * pre: assumes CodeH, enters with level = q
  * search returns any same set with a smaller entropy
  * NewCode is assumed to be freshly inited, returns the first smallest code of same structure.
  * CodeH contains q and e parameters
  * returns the length of the longest word of the new set as an integer
  ************************************************************/

int	MinSearch(NewCodeP, CodeP )
DepletionSetTypeP	NewCodeP, CodeP;
{
DeplnType		Depletion;
DepletionSetTypeP	tempCodeP = 0L, saveCodeP = 0L;
int	maxlength, ref = 0, i, limit, prefixlength, Lastprefixlength = 0;
long	LastDeplnNum = -1;

	if (NewCodeP->q < CodeP->q) {										/* if newcode augmented less than code set... */
		i = NewCodeP->q + 1; 										/* scan through remaining expansion parameters in code set */
		while ((i <= CodeP->q ) && (CodeP->e[i] == 1) ) i++;
		if ( i > CodeP->q ) { 										/* if all e's are ones, can do a quicker job as follows */
			for ( i = NewCodeP->q + 1; i <= CodeP->q; i++) { 							/* for each extra level of augmentation in code set */
				LastDeplnNum = -1;								/* reset current rankword to get shorest */
				FirstRankWord(LastDeplnNum, NewCodeP, &Depletion); 					/* get shortest word from new set */
				Augment(NewCodeP,  &Depletion, 1);  							/* augment with shortest word once ie: e=1*/
			}
			MaximalCode(NewCodeP, &Depletion);								/* gets first of the maximal codes from new set */
			ref = WordSize(&Depletion , NewCodeP);							/* set longest word size in set to be returned*/
		}
		else if ( i <= CodeP->q ) { 										/* found at least one expansion parameter which is not 1 */
			limit = CodeSize(NewCodeP);									/* set limit to size of set */
			if (limit  > (CodeP->q - NewCodeP->q ) )							/* if limit greater than differance in levels of augmentation */
			 	limit = CodeP->q;									/* then only need to go up to augmentation level in code set */
			tempCodeP = NewCode(tempCodeP);								/* create work areas */
			saveCodeP = NewCode(saveCodeP);		
			for ( i = NewCodeP->q + 1; i <= limit; i++) {							/* for each word in set or until augmentation levels are equal */
				LastDeplnNum = FirstRankWord(LastDeplnNum, NewCodeP, &Depletion);			/* get next short word from the new set */
	
				if (Lastprefixlength != (prefixlength = WordSize(&Depletion , NewCodeP)) ) { 			/* if word length is differant from the last time */			
	
					Lastprefixlength = prefixlength;						/* note the word length for next time */
					CopyCode(NewCodeP, tempCodeP);						/* copy to tempCode for working with */
					Augment(tempCodeP,  &Depletion, CodeP->e[NewCodeP->q + 1]);			/* augment tempcode by same expansion as in code set */
					maxlength = MinSearch(tempCodeP, CodeP);					/* recursive call using tempCode */
					if (ref == 0)  {								/* if first time in the loop */
						ref = maxlength; 
						CopyCode(tempCodeP, saveCodeP); 					/* save a copy of the code */
					}
					else if (maxlength < ref) {							/* or if found a better code */
						ref = maxlength;		
						CopyCode(tempCodeP, saveCodeP); 					/* save a copy of the better code */
					}			
				} /* end of shortcut testing */
			}
			CopyCode(saveCodeP, NewCodeP);								/* transfer the best of the codes ready to return */
			DisposeCode(&tempCodeP);									/* dispose work areas */
			DisposeCode(&saveCodeP);
		}
	}
	else if (NewCodeP->q == CodeP->q) { 										/* if equally augmented ie: base case for recursion */
		MaximalCode(NewCodeP, &Depletion);									/* gets first of the maximal codes from the new set */
		ref = WordSize(&Depletion , NewCodeP);								/* return size of longest word in set */
	} 
	return(ref);
}


/****************************************************************************************/

int	LeastNum(BestNum, NewNum)
int	BestNum, NewNum;
{
	if ((BestNum == 0) || ( NewNum < BestNum ) ) {					/* if bestnum is zero ie base case return newnum */
		return(NewNum);							/* else return smallest number */
	}
	else return(BestNum);
}

/****************************************************************************************/
int	factor(number, last)
int	number, last;
{
int	factr;

	while (last < number) {							/* while still untried numbers */
		last++;								/* increment number being tried */
		factr = number / last;						/* divide by new value*/
		if (number - (factr  * last) == 0) return(last);				/* if remainder is zero return new factor */
		else if (factr < last)  return(0);					/* if past halfway then dont need to go any futher */
	}
	return(0);									/* no more factors */
}


/****************************************************************************************/
void ShowStructure(CodeP) /* temp routine */
DepletionSetTypeP	CodeP;
{
char	prefix[MAXPRFXLEN];
int	i, len;

	printf("m\tk_m\tp_m\n");							/* show heading */
	for (i = 1; i <= CodeP->q; i++) {
		len = Encode(CodeP->PDTs[i], CodeP, prefix);				/* encode the PDT */
		prefix[len] = 0;							/* mark end of prefix string */
		printf("%d\t%d\t%s\n", i, CodeP->e[i], prefix);				/* display details */
	}
	printf("\n");								/* print new line */
}

/****************************************************************************************/
