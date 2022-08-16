/**************************************************************************************
	Structures used in library routines

	Written by:	Scott Wackrow
	Date:		Dec 1994	

****************************************************************************************
			Alphabet					
***************************************************************************************/

#define	MAXALPHA	26							/* max number of members in alphabet */

typedef struct AlphabetType { 							/* actually the structure of a pascal string */
	char	Q;								/* size Q = |A| of base */
	char	a[MAXALPHA];							/* base character list A={0,1,...[9]} or {a,b,c...,[z]} */
} AlphabetType, *AlphabetTypeP;

/**************************************************************************************
			Depletion Code				
**************************************************************************************/

#define	MAXAUG		32							/* max number of augmentations allowed */

typedef struct DeplnType {								/* Depletion codes */
	int			size;						/* size indicates the largest occupied cell to minimise parsing*/
	int			w[MAXAUG];					/* array of expansion codes */
} DeplnType, *DeplnTypeP;

/**************************************************************************************
			Depletion Set				
**************************************************************************************/

typedef struct DepletionSetType {				
	int			q;						/* degree */
	int			e[MAXAUG + 1];					/* expansion factors */
	DeplnTypeP		PDTs[MAXAUG + 1];					/* pointers to prefixes depletion codes */
	AlphabetTypeP		AlphabetP;					/* pointer to alphabet */
} DepletionSetType, *DepletionSetTypeP;

/**************************************************************************************
			Complexity Tree				
**************************************************************************************/

typedef struct ComplexityType {							/* complexity */
	int			q;						/* degree */
	int			e[MAXAUG + 1];					/* expansion factors */
	int			MinComplexity;					/* T-complexity*/
	struct	ComplexityType	*NextL;
	struct	ComplexityType	*NextR;						/* pointers */
} ComplexityType, *ComplexityP;

/**************************************************************************************
			Other Constants				
**************************************************************************************/

#define 	MAXPRFXLEN	1000
#define 	DEFAULTCODESET	"alphabet"
#define	COMPLEXITYTREE	"/usr/bin/tcode/treefile"
#define	LASTDEAUGFILE	"deaugment.last"

/*************************************************************************************/


