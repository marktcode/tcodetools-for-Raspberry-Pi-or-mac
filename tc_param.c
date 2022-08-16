/*************************************************************************************
								
	Program:		SetParameters	
	Written by:	Scott Wackrow
	Date:		Dec 1994	

*************************************************************************************/
#include 	"Incs.c"

int main (argc, argv)
int argc;
char *argv[];
{
 
int i, len;
char *file = NULL, prefix[MAXPRFXLEN] ;
DepletionSetTypeP codeP = NULL;

/*  search arguments for parameters */
	
	file = FindFile (argc, argv, DEFAULTCODESET );
	ShowHelp(argc, argv, "usage: tc_param [-f filename]", FALSE);

	if (ArgsLeft(argc, argv) > 0) {
		printf ("error: too many arguments passed\n");
		exit (-1);
	}

/* display whats received */

	printf ("\nfilename: %s\n\n", file );

/*  load the file */

	codeP = NewCode (codeP);
	LoadCode ( file, codeP );

/* display the alphabet */

	printf ("A = {");

	for (i=0; i < codeP->AlphabetP->Q - 1; i++)
		printf ("%c,", codeP->AlphabetP->a[i]);
	printf ("%c}, |A| = %d\n\n", codeP->AlphabetP->a[codeP->AlphabetP->Q-1], codeP->AlphabetP->Q);

/* display set size */

	printf ("X = {x_i}, |X| = |{x_i}| = %ld\n\n", CodeSize(codeP));

/* display augmentation */

	printf ("set augmentations: |{k_m}| = |{p_m}| = N = %d, m = 1,...,N\n\n", codeP->q);
	ShowStructure(codeP);

/* display maximal codes */

	MaximalCode(codeP, codeP->PDTs[codeP->q+1]);
	len = Encode(codeP->PDTs[codeP->q+1], codeP, prefix);
	printf ("\nMaximal codes u \\in X, |u| = %d:\n", len);
	for (i=1; i<=codeP->AlphabetP->Q; i++) {
		len = Encode(codeP->PDTs[codeP->q+1], codeP, prefix);
		prefix[len] = 0;
		printf ("%s\n", prefix);
		codeP->PDTs[codeP->q+1]->w[0]++;
	}
	DisposeCode(&codeP);
	exit(0);
}
