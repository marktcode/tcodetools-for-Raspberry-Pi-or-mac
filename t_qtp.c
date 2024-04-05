#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>


#define MAX_STEPSAMPLE_INTERVAL 65536  // The maximum interval (2^16) for step information sample

typedef struct CODEWORDLIST  // match lists links same codewords based on IDs. string list links all codewords
{
    unsigned long CodeWordID;
    struct CODEWORDLIST *NextCodeWordInMatchList;
    struct CODEWORDLIST *PreviousCodeWordInMatchList;
    struct CODEWORDLIST *NextCodeWordInStringList;
    struct CODEWORDLIST *PreviousCodeWordInStringList;

}CODEWORD;

typedef struct MATCHLISTENTRY_DEF  // entry list stores heads of all match lists. These are dummy heads, no real data is stored here
{
    unsigned long CodeWordID;
    struct CODEWORDLIST *NextCodeWordInMatchList;
    struct CODEWORDLIST *PreviousCodeWordInMatchList;

}MATCHLISTENTRY;



#define EMPTY -1


#define FALSE 0
#define TRUE 1
//#define NULL 0


#define MAXPART 32767

#define MAXIT 100
#define EULER 0.5772156649
#define FPMIN 5.0e-308
#define EPS 5.0e-16

 int	Info = FALSE;
 int	Binary = FALSE;


  // Command Line Flags
  int FromFile = FALSE,  FromFileRaw = FALSE;
  char *FileName = NULL;


  // Data Array
	long DataLen;
	int *Data;
	char *Dynamics, *Dynamicsrev;
	char *Headr[1000];

	long Head, Tail;
	long *Next, *Last;
	long AugCount = 0;
	long Prefix = EMPTY, K = 0, LastK;
	double Complexity, LastComplexity = 0.0;


    CODEWORD *PrimeListEnd;
    MATCHLISTENTRY *EntryList;
	long HashTableSize;


    long Tprefix_N_Indicate;
    long NextAvailableEntry;
    long FirstNewEntry_PerPass;
    CODEWORD *PrimeList;

  // T-information
  long OldPos = 0;

long FileSize(char *FileName) {
  struct stat stbuf;

  if (stat(FileName, &stbuf) == -1)
    return(-1);
  else
    return(stbuf.st_size);
}

/* void LoadFile(void) {

  FILE *f;
  float input, max = 0.0, min = 0.0, avg = 0.0;
	int val;
	long	j;

	  DataLen = 0;
	  f = fopen(FileName, "r");

	  while (EOF != fscanf(f, "%f", &input)) {
		avg += input;
	  	if (DataLen == 0) {max = min = input;}
	  	else {
			if (input > max) max = input;
			if (input < min) min = input;
			DataLen++;
		}
	  }
	  fclose(f);

	  avg = avg/(float) DataLen;
		max -= avg;
		min -= avg;
	  	if (max - min < 0) max = -min; // cover the range;
		Data = (char *) malloc(DataLen * sizeof(int));
	  	j = 0;
	  	max = max/10000.0; // scale max to 10000
	  	  if (DataLen) printf("#%f %f %f %d\n", max, avg, min, DataLen);
	  f = fopen(FileName, "r"); // now read in the data again
	  while (EOF != fscanf(f, "%f", &input)) {
		val = (int) (((float) input - avg)/max); // printf("debug %f %d\n", input, val);
		Data[j++] = (char) (val >> 8) & 255  ;
		Data[j++] = (char) val & 255;
	  }
	  fclose(f);

}
*/


double ei(double x)
{
        void nrerror(char error_text[]);
        int k;
        double fact,prev,sum,term;

        if (x <= 0.0) nrerror("Bad argument in ei");
        if (x < FPMIN) return log(x)+EULER;
        if (x <= -log(EPS)) {
                sum=0.0;
                fact=1.0;
                for (k=1;k<=MAXIT;k++) {
                        fact *= x/k;
                        term=fact/k;
                        sum += term;
                        if (term < EPS*sum) break;
                }
                if (k > MAXIT) nrerror("series failed in ei");
                return sum+log(x)+EULER;
        } else {
                sum=0.0;
                term=1.0;
                for (k=1;k<=MAXIT;k++) {
                        prev=term;
                        term *= k/x;
                        if (term < EPS) break;
                        if (term < prev) sum += term;
                        else {
                                sum -= prev;
                                break;
                        }
                }
                return exp(x)*(1.0+sum)/x;
        }
}

void nrerror(char error_text[])
{
        printf("%s\n", error_text);
        exit(1);
}


double logint(double x) {
        return ei(log(x));
}

double invlogint(double x) {
        double lb, ub, tmp1, tmp2, g1;
        lb = 1.0+EPS;
        if (x < logint(lb)) {
                nrerror("argument too small for fast algorithm");
        }
        ub = 2.0;
        while (logint(ub) < x) {
                lb = ub;
                ub *= 2;
        }
        tmp1 = logint(lb);
        tmp2 = logint(ub);
        g1 = 1/log(lb);
        /* x is now between logint(lb) and logint(ub) */
        /* printf("lb:%g ub:%g tmp1:%g tmp2:%g g1:%g\n",lb,ub,tmp1,tmp2,g1); */
        while (tmp2-tmp1 > EPS) {
                /* printf("lb:%g ub:%g tmp1:%g tmp2:%g g1:%g\n",lb,ub,tmp1,tmp2,g1);
                printf("Iteration\n"); */
                ub = (x - tmp1) * (ub - lb)/(tmp2 - tmp1) + lb;
                tmp2 = logint(ub);
                lb += (x - tmp1)/g1;
                tmp1 = logint(lb);
                g1 = 1/log(lb);
        }
        if (Binary) return (ub-.7582221)/log(2.0);
        else return (ub-.7582221);
}

/******************************************************************************
 * New calc
 *
 ******************************************************************************/
/*

double nwSelAugment(char *buffer, long bufflen) { //back to old bound

  long Pos, FirstPos, LastPos;
  int Matching = FALSE;
  long Matched = 0;
  long Len, Len2, LenPrefix, tPos, Following, NexttPos;
  int	comp, dn;
  long	Loop, Loop2;

  // Linked List
  long Head, Tail;

  // Augmentation Data
  long Prefix = EMPTY, K = 0;
  double Complexity = 0;

	int	k;


	Head = 0;
	Tail = bufflen - 1;
	for (k = 0; k < bufflen; k++) {
	    Next[k] = k + 1;
	    Last[k] = k - 1;
	}
	Next[bufflen - 1] = EMPTY;
	Last[0] = EMPTY;


while (Head != Tail) {
	Matching = FALSE;
	Matched = 0;
//debug	printf("Tail %d\n",Tail);
	// select prefix
	  if (Tail != EMPTY) {
	    tPos = Prefix = Last[Tail];
	    K = 0;

		  if (Next[tPos] != EMPTY)
		    Len = Next[tPos] - tPos;
		  else
		    Len = bufflen - tPos;

		  if (Next[Prefix] != EMPTY)
		    Len2 = Next[Prefix] - Prefix;
		  else
		    Len2 = bufflen - Prefix;

		  if ( Len  != Len2)  dn =  FALSE;
		  else {
		    // Test remaining characters
		  	dn = TRUE;
		    Loop2 = 0;
		    for (Loop = Len - 1; Loop >= Len/2 ; Loop--) {
				if (buffer[tPos + Loop2] != buffer[Prefix + Loop2])  dn = FALSE;
				else if (buffer[tPos + Loop] != buffer[Prefix + Loop])  dn = FALSE;
				else Loop2++;
		     }
		  }
//debug	printf("Got here dn %d\n",dn);


	    while ((tPos != EMPTY) && (dn)) {
	      tPos = Last[tPos];
	      K++;

		  if (Next[tPos] != EMPTY)
		    Len = Next[tPos] - tPos;
		  else
		    Len = bufflen - tPos;

		  if (Next[Prefix] != EMPTY)
		    Len2 = Next[Prefix] - Prefix;
		  else
		    Len2 = bufflen - Prefix;

//		  dn = TRUE; // is superfluous
		  if ( Len  != Len2)  dn =  FALSE;
		  else {
		    // Test remaining characters
		    Loop2 = 0;
		    for (Loop = Len - 1; Loop >= Len/2 ; Loop--) {
				if (buffer[tPos + Loop2] != buffer[Prefix + Loop2])  dn = FALSE;
				else if (buffer[tPos + Loop] != buffer[Prefix + Loop])  dn = FALSE;
				else Loop2++;
		     }
		  }

	    }

//debug	 printf("Got past while  dn %d   tPos %d   K %d \n",dn, tPos, K);


	  }

// augment
	  // Calculate complexity
	  Complexity += log((float)K + 1);
	  Pos = Head;
		  if (Next[Prefix] != EMPTY)
		    Len2 = Next[Prefix] - Prefix;
		  else
		    Len2 = bufflen - Prefix;

	  while (Pos != EMPTY)  {
		  if (Next[Pos] != EMPTY)
		    Len = Next[Pos] - Pos;
		  else
		    Len = bufflen - Pos;


		  if ( Len  != Len2)
		    comp = FALSE;
		  else {
			  Loop2 = 0;
			  comp = TRUE;
		    // Test characters
		    for (Loop = Len - 1; Loop >= Len/2 ; Loop--) {
				if (buffer[Pos + Loop2] != buffer[Prefix + Loop2])  comp = FALSE;
				if (buffer[Pos + Loop] != buffer[Prefix + Loop])  comp = FALSE;
				Loop2++;
		     }
		  }
//debug	 if (Pos == bufflen - 1) printf("comp %d \n",comp);

		 if(comp == TRUE) {
	      if (Matching == TRUE) {
	        Matched++;
	        if (Matched > K) {
	          Matching = FALSE;
	          LastPos = Pos;
	          Pos = Next[Pos];

//  Combine(FirstPos, LastPos);
			  Following = Next[LastPos];

			  tPos = Next[FirstPos];
			  while ((tPos != EMPTY) && (tPos <= LastPos)) {
			    if (Pos == EMPTY) NexttPos = EMPTY;
			    else NexttPos = Next[Pos];

			    Next[tPos] = EMPTY;
			    Last[tPos] = EMPTY;
			    tPos = NexttPos;
			  }

			  Next[FirstPos] = Following;
			  if (Following == EMPTY) {
			    Tail = FirstPos;
			  }
			  else
			    Last[Following] = FirstPos;
// end Combine
	        }
	        else
	          Pos = Next[Pos];
	      }
	      else { // if  Matching == FALSE
	        Matching = TRUE;
	        Matched = 1;
	        FirstPos = Pos;
	        Pos = Next[Pos];
	      }
		}
	    else {  // if  comp == FALSE
	      if (Matching == TRUE) {
	        Matching = FALSE;
	        LastPos = Pos;
	        Pos = Next[Pos];

//  Combine(FirstPos, LastPos);
			  Following = Next[LastPos];

			  tPos = Next[FirstPos];
			  while ((tPos != EMPTY) && (tPos <= LastPos)) {
			    if (Pos == EMPTY) NexttPos = EMPTY;
			    else NexttPos = Next[Pos];

			    Next[tPos] = EMPTY;
			    Last[tPos] = EMPTY;
			    tPos = NexttPos;
			  }

			  Next[FirstPos] = Following;
			  if (Following == EMPTY) {
			    Tail = FirstPos;
			  }
			  else
			    Last[Following] = FirstPos;
// end Combine

	      }
	      else  // if  Matching == FALSE
	        Pos = Next[Pos];
	    }  // end  if  comp
//debug printf("Pos %d\n",Pos);
	  } // end while Pos != EMPTY
//debug	  printf("Pos %d\n",Pos);
	}
	return(Complexity/log(2.0));
}
*/
/******************************************************************************
 * New quickcalc
 *
 ******************************************************************************/


double ftdSelAugment ( char *buffer, long bufflen)
{

    long Loop, StepCount, HashCodeTemp, Length_SuffixString_ParsedIntoOneCodeword, StepInfoPrintCount;
	long TprefixLength, HashCode_Tprefix, Count_TprefixMatch, TempID, TempEntry;

	char *pc_TprefixAddress;
	CODEWORD *CodeWordBegin, *PrimeListTemp, *RealEndOfPrimeList, *Node_Tprefix, *HeadOfCurrentMatchList;
	CODEWORD *CodeWordInMatchList;

    Complexity = 0;
    Tprefix_N_Indicate = 256;												// entry reserved for p^k_n
    NextAvailableEntry = 257; 												// points to next available entry
    FirstNewEntry_PerPass = NextAvailableEntry;  							// entry for first new codeword in each pass
    EntryList[Tprefix_N_Indicate].CodeWordID = 0;

    PrimeListEnd = NULL;


/********* Initialize head node *********/
    PrimeList[0].NextCodeWordInStringList = PrimeList + 1;
    PrimeList[0].PreviousCodeWordInStringList = NULL;

/********* Initialize end node *********/
    PrimeList[bufflen].NextCodeWordInStringList = NULL;
    PrimeList[bufflen].PreviousCodeWordInStringList = PrimeList + bufflen - 1;

    for (Loop = 0; Loop < 256; Loop++)  									// initialize first 256 entries (0--255)
    {
        EntryList[Loop].PreviousCodeWordInMatchList = (CODEWORD*)(EntryList + Loop);
        EntryList[Loop].CodeWordID = 0;
    }

    for (Loop= 1; Loop < bufflen ; Loop++)   							// create lists. Pointers in NextCodeWordInMatchList and NextCodeWordInStringList should have same values
    {
        PrimeList[Loop].NextCodeWordInStringList = PrimeList + Loop + 1;
        PrimeList[Loop].PreviousCodeWordInStringList = PrimeList + Loop - 1;
    }
    PrimeListEnd = PrimeList + bufflen;

    StepCount = 1;   													// includes literal character

    PrimeListTemp = PrimeList;
    for (Loop= 0; Loop < bufflen - 1; Loop++)
    {
        HashCodeTemp = (unsigned char)(buffer[Loop]);
        PrimeListTemp->NextCodeWordInStringList->CodeWordID = HashCodeTemp;

																			/*********** Add to the list **********/
        PrimeListTemp->NextCodeWordInStringList->PreviousCodeWordInMatchList =EntryList[HashCodeTemp].PreviousCodeWordInMatchList;
        PrimeListTemp->NextCodeWordInStringList->NextCodeWordInMatchList = (CODEWORD*)&(EntryList[HashCodeTemp]);
        EntryList[HashCodeTemp].PreviousCodeWordInMatchList->NextCodeWordInMatchList = PrimeListTemp->NextCodeWordInStringList;
        EntryList[HashCodeTemp].PreviousCodeWordInMatchList = PrimeListTemp->NextCodeWordInStringList;
																			/*************************************/

        PrimeListTemp++;
    }
    PrimeListTemp = PrimeList;

    Length_SuffixString_ParsedIntoOneCodeword = 0;   					// compute length of substring (suffix of string) that has been parsed as one codeword after each run
    RealEndOfPrimeList = PrimeListEnd;

    StepInfoPrintCount = 0;   											// To work with StepInfoDisplayInterval

    				/*********** start T-decomposition ***********/

    for (;;)
    {
        if (PrimeList->NextCodeWordInStringList->NextCodeWordInStringList == NULL) // Check if process has finished (i.e. if these is only one codeword in string)
            break;

        K = 1;   														// count the number of the adjacent codewords that match the T-prerix.
        StepCount++;

         			/*********** (1)Get the T-prefix ********** */
        Node_Tprefix = PrimeListEnd->PreviousCodeWordInStringList;
        pc_TprefixAddress = buffer + (Node_Tprefix - PrimeList) - 1;
        TprefixLength = RealEndOfPrimeList - Node_Tprefix;
        HashCode_Tprefix = (Node_Tprefix->CodeWordID);
        HeadOfCurrentMatchList = Node_Tprefix->NextCodeWordInMatchList;  /*Point to the dummy head of the hash list.*/

        			/*********** (2)Find how many adjacent copies preceding the T-prefix at the end of the string. So we can got the T-expansion parameter ***********/
        while (Node_Tprefix->PreviousCodeWordInStringList->PreviousCodeWordInStringList != NULL)
        {
            if (Node_Tprefix->PreviousCodeWordInMatchList == Node_Tprefix->PreviousCodeWordInStringList)
            {
                Node_Tprefix = Node_Tprefix->PreviousCodeWordInStringList;
                K++;
            }
            else
                break;
        }
        RealEndOfPrimeList = Node_Tprefix;

        Node_Tprefix->PreviousCodeWordInMatchList->NextCodeWordInMatchList = HeadOfCurrentMatchList;
        HeadOfCurrentMatchList->PreviousCodeWordInMatchList = Node_Tprefix->PreviousCodeWordInMatchList;
        Node_Tprefix->PreviousCodeWordInStringList->NextCodeWordInStringList = PrimeListEnd;
        PrimeListEnd->PreviousCodeWordInStringList = Node_Tprefix->PreviousCodeWordInStringList;

        Complexity += log((double)(K + 1));

        			/*********** (3)Search from the beginning of the match list to find the codeword that matches the T-prefix ***********/
        CodeWordInMatchList = HeadOfCurrentMatchList;  /*The head of the match list.*/

        if (CodeWordInMatchList ->NextCodeWordInMatchList == HeadOfCurrentMatchList) 	// if list does not exist
          	continue;
        CodeWordInMatchList = CodeWordInMatchList->NextCodeWordInMatchList;
        Count_TprefixMatch = 0;  													// The number of the adjacent copies of T-prefix found. It will be set to 0 again
                                                                             			// once the new codeword does not mathc the T-prefix
        																				// Now search really begins from begining of corresponding match list
        																				// The match list helps to locate the codeword in the string list
        while (CodeWordInMatchList != HeadOfCurrentMatchList)
        {
            PrimeListTemp = CodeWordInMatchList;
            Count_TprefixMatch = 1;  													// number of adjacent codewords matching the T-prefix
            CodeWordBegin = PrimeListTemp; 									// The node that pointing to the possible beginning of a new codeword
            PrimeListTemp = PrimeListTemp->NextCodeWordInStringList;

            for (;;)  																	// matched codeword found. Here we need remove some codewords from the match list and string list
                        																// New merged codeword will be added to the corresponding match list. Check in the string list
            {
                if (HashCode_Tprefix == PrimeListTemp->CodeWordID)
                { 																		// The current codeword DOES match the T-prefix
                    Count_TprefixMatch++;
                    if (Count_TprefixMatch <= K)
                    {
                        PrimeListTemp = PrimeListTemp->NextCodeWordInStringList;
                    }
                    else  																// number of the adjacent matching codewords exceeds the T-expansion parameter
                    {
                        																// move the pointer to the next code in the match list
                        CodeWordInMatchList = PrimeListTemp->NextCodeWordInMatchList;
                        																// delete the node in the string list
                        CodeWordBegin->NextCodeWordInStringList = PrimeListTemp->NextCodeWordInStringList;
                        PrimeListTemp->NextCodeWordInStringList->PreviousCodeWordInStringList = CodeWordBegin;
                        if ( EntryList[Tprefix_N_Indicate].CodeWordID < FirstNewEntry_PerPass) // no entry for this new codeword
                        {
                            EntryList[Tprefix_N_Indicate].CodeWordID = NextAvailableEntry;
                            TempID = NextAvailableEntry;
                            EntryList[NextAvailableEntry].PreviousCodeWordInMatchList = (CODEWORD*)(EntryList + NextAvailableEntry);
                            EntryList[NextAvailableEntry].CodeWordID= 0;
                            NextAvailableEntry++;
                        }
                        else
                        {
                            TempID = EntryList[Tprefix_N_Indicate].CodeWordID;
                        }
                        CodeWordBegin->CodeWordID = TempID;

																						/*********** Add to the lists ***********/
                        CodeWordBegin->PreviousCodeWordInMatchList =EntryList[TempID].PreviousCodeWordInMatchList;
                        CodeWordBegin->NextCodeWordInMatchList = (CODEWORD*)(&(EntryList[TempID]));
                        EntryList[TempID].PreviousCodeWordInMatchList->NextCodeWordInMatchList = CodeWordBegin;
                        EntryList[TempID].PreviousCodeWordInMatchList = CodeWordBegin;

                        break;
                    }
                }
                else   																	// current codeword does not match the T-prefix
                {
                    TempEntry = PrimeListTemp->CodeWordID;
                    CodeWordInMatchList = PrimeListTemp->PreviousCodeWordInStringList->NextCodeWordInMatchList;
                    for (Loop = 0; Loop < Count_TprefixMatch; Loop++)  					// Remove codewords from string list, the first matched codeword remains
                    {
                        if (EntryList[TempEntry].CodeWordID < FirstNewEntry_PerPass) 	// entry for the new codeword
                        {
                            EntryList[TempEntry].CodeWordID = NextAvailableEntry;
                            TempEntry = NextAvailableEntry;
                            EntryList[TempEntry].PreviousCodeWordInMatchList = (CODEWORD*)(EntryList + TempEntry);
                            EntryList[TempEntry].CodeWordID= 0;

                            NextAvailableEntry++;
                        }
                        else
                        {
                            TempEntry = EntryList[TempEntry].CodeWordID;
                        }
                    }
                    CodeWordBegin->CodeWordID = TempEntry;  							// new ID
                    { 																	// if not the last node in the match list
                        PrimeListTemp->PreviousCodeWordInMatchList->NextCodeWordInMatchList = PrimeListTemp->NextCodeWordInMatchList;
                        PrimeListTemp->NextCodeWordInMatchList->PreviousCodeWordInMatchList = PrimeListTemp->PreviousCodeWordInMatchList;
                    }

																						/*********** Add to the lists ***********/
                    CodeWordBegin->PreviousCodeWordInMatchList =EntryList[TempEntry].PreviousCodeWordInMatchList;
                    CodeWordBegin->NextCodeWordInMatchList = (CODEWORD*)(&(EntryList[TempEntry]));
                    EntryList[TempEntry].PreviousCodeWordInMatchList->NextCodeWordInMatchList = CodeWordBegin;
                    EntryList[TempEntry].PreviousCodeWordInMatchList = CodeWordBegin;
																						/****************************************/
                    CodeWordBegin->NextCodeWordInStringList = PrimeListTemp->NextCodeWordInStringList;
                    PrimeListTemp->NextCodeWordInStringList->PreviousCodeWordInStringList = CodeWordBegin;

                    break;
                }
            }																			// end of for (;;)
        }
        FirstNewEntry_PerPass = NextAvailableEntry;
    }


    return Complexity/log(2.0);
}






//******************* main *********************/
int main(int argc, char *argv[]) {

  int *Dataint, Partition, Division, defaultviewing = FALSE, Raster = FALSE, Surface=TRUE, Maximum=FALSE, String=FALSE;
  int Loop, PRange = 50, PIncr = 1, DataOut=FALSE, LogPart=FALSE, BestDir=FALSE,  val, autoset = TRUE, minset=FALSE, midset=FALSE, maxset=FALSE ;
  long	i, j, k, l, m, offset;
  long	Window = 10000, Shift = 1000;
  float Threshold, level, Scale=1.0, input, avg, max, min, mid;
  float xscale = 7.0, yscale = 3.0 ,zscale = 1.0 , xoffset = 0.0, yoffset = 0.0 ,zoffset = 0.0;
  float *first,  *second;
  FILE *f;
    double Complexity1, Complexity2,  maxComplexity;
    char temp;

  // Read arguments from command line
  for (Loop = 1; Loop < argc; Loop++) {
    if ((strcmp(argv[Loop], "?")== 0)|| (strcmp(argv[Loop], "-?")== 0) || (strcmp(argv[Loop], "-h")== 0)) {
     printf("\t Partition Flow files V2.0\n");
    printf("\t Copyright (C) 1998 M. R. Titchener, University of Auckland\n");
    printf("\t\tExecutable Program and its Source licenced under the \n");
    printf("\t\tGNU GENERAL PUBLIC LICENSE v3,\n");
    printf("\t\t\tas set out in the associate file LICENSE.\n");
    printf("\t\tIncludes routines by M.R.Titchener, U.Speidel, J. Yang \n");
    printf("\t\t\t     -f [or -F] file \t processes input file of floating point data\n");
    printf("\t\t\t     -s \t assumes input file contains a string of symbols\n");
    printf("\t\t\t     -h \t prints this info\n");
    printf("\t\t\t     -p n s\t sets partition range +/- n and increment s \n");
    printf("\t\t\t     -ps s \t sets partition range +/-1000 and increment s \n");
    printf("\t\t\t     -w n s \t sets window to n and wndow shift increment to s\n");
    printf("\t\t\t     -ws s \t sets the window to 2 x s and wndow shift increment to s\n");
    printf("\t\t\t     -i \t outputs in (nats) else in (taugs)\n");
    printf("\t\t\t     -b \t outputs in (bits) \n");
    printf("\t\t\t     -r \t computes the minimum complexity parsing over both directions\n");
    printf("\t\t\t     -R \t computes total over Rastered window \n");
    printf("\t\t\t     -M \t computes Max entropy over Rastered window \n");
    printf("\t\t\t     -ud \t Information units decinats/decibits \n");
    printf("\t\t\t     -uc \t Information units centinats/centibits \n");
    printf("\t\t\t     -um \t Information units millinats/millibits \n");
    printf("\t\t\t     -mid n.n \t sets mid position of raster \n");
    printf("\t\t\t     -max n.n \t sets max position of raster \n");
    printf("\t\t\t     -min n.n \t sets min position of raster \n");
    printf("\t\t\t     -a \t calculates average position of raster \n");
    printf("\t\t\t     -c \t outputs default viewing parameters\n");
    printf("\t\t\t     -s \t assumes file contains a string\n");
    printf("\t \n");
      return 0;
    }
    else if ((strcmp(argv[Loop], "-f") == 0) || (strcmp(argv[Loop], "-F")== 0 )){
      FromFile = TRUE;
      if (argc > (Loop+1)) {
        FileName = argv[Loop + 1];
        Loop++;
      }
      else
      {
        printf("No filenname specified!\n");
        return -1;
      }
      String=FALSE;
    }
    else if (strcmp(argv[Loop], "-s") == 0) {
      FromFile = TRUE;
      if (argc > (Loop+1)) {
        FileName = argv[Loop + 1];
        Loop++;
      }
      else
      {
        printf("No filenname specified!\n");
        return -1;
      }
      String = TRUE;
      Surface = FALSE;
      Raster = FALSE;
      Maximum = FALSE;
      defaultviewing = FALSE;
    }
    else if (strcmp(argv[Loop], "-w") == 0) {  // window size
      sscanf(argv[Loop + 1], "%ld", &Window);
      Loop++;
      sscanf(argv[Loop + 1], "%ld", &Shift);
      Loop++;
    }
    else if (strcmp(argv[Loop], "-ws") == 0) {  // shift window increment
      sscanf(argv[Loop + 1], "%ld", &Shift);
      Loop++;
      Window = 2 * Shift;
    }
    else if (strcmp(argv[Loop], "-p") == 0) {  // set a symmetrical partition range : default is 50
      sscanf(argv[Loop + 1], "%d", &PRange);
      Loop++;
      sscanf(argv[Loop + 1], "%d", &PIncr);
      Loop++;
    }
    else if (strcmp(argv[Loop], "-ps") == 0) {  // set a symmetrical partition range : default is 50
	  PRange = 1000;
      sscanf(argv[Loop + 1], "%d", &PIncr);
      Loop++;
    }
     else if (strcmp(argv[Loop], "-r") == 0) { // minimum complexity for both directions
      BestDir=TRUE;
    }
     else if (strcmp(argv[Loop], "-R") == 0) { // Raster calculation
      Surface=FALSE;
      Raster=TRUE;
      Maximum=FALSE;
    }
     else if (strcmp(argv[Loop], "-RS") == 0) { // maximum value
      Surface=TRUE;
      Raster=TRUE;
      Maximum=FALSE;
    }
     else if (strcmp(argv[Loop], "-RM") == 0) { // maximum value
      Surface=FALSE;
      Raster=TRUE;
      Maximum=FALSE;
    }
     else if (strcmp(argv[Loop], "-M") == 0) { // maximum value
      Surface=FALSE;
      Raster=FALSE;
      Maximum=TRUE;
    }
     else if (strcmp(argv[Loop], "-RMS") == 0) { // maximum value
      Surface=TRUE;
      Raster=TRUE;
      Maximum=TRUE;
    }
    else if (strcmp(argv[Loop], "-i") == 0)  { //output T-information
      Info = TRUE;
    }
    else if (strcmp(argv[Loop], "-b") == 0)  {
      Binary = TRUE;
    }
    else if (strcmp(argv[Loop], "-a") == 0)  {
      autoset = FALSE;
    }
    else if (strcmp(argv[Loop], "-c") == 0)  {
      defaultviewing = TRUE;
    }
    else if (strcmp(argv[Loop], "-mid") == 0)  {
      sscanf(argv[Loop + 1], "%f", &mid);
      Loop++;
	  midset=TRUE;
    }
    else if (strcmp(argv[Loop], "-max") == 0)  {
      sscanf(argv[Loop + 1], "%f", &max);
      Loop++;
	  maxset=TRUE;
    }
    else if (strcmp(argv[Loop], "-min") == 0)  {
      sscanf(argv[Loop + 1], "%f", &min);
      Loop++;
	  minset=TRUE;
    }
    else if (strcmp(argv[Loop], "-xyz") == 0) {  //
      sscanf(argv[Loop + 1], "%f", &xscale);
      Loop++;
      sscanf(argv[Loop + 1], "%f", &yscale);
      Loop++;
      sscanf(argv[Loop + 1], "%f", &zscale);
      Loop++;
      sscanf(argv[Loop + 1], "%f", &xoffset);
      Loop++;
      sscanf(argv[Loop + 1], "%f", &yoffset);
      Loop++;
      sscanf(argv[Loop + 1], "%f", &zoffset);
      Loop++;
//      printf("\ntest %f %f %f %f %f %f\n", xscale, xscale, yscale, xoffset, yoffset, zoffset);
    }
    else if (strcmp(argv[Loop], "-ud") == 0)  { //deci-range
      Scale=10.0;
    }
    else if (strcmp(argv[Loop], "-uc") == 0)  { //centi-range
      Scale=100.0;
    }
    else if (strcmp(argv[Loop], "-um") == 0)  { //milli-range
      Scale=1000.0;
    }

    }

    if ((FromFile == TRUE) && (String == FALSE))  {
 	  DataLen = 0;


	  f = fopen(FileName, "r");

	  while (EOF != fscanf(f, "%f", &input)) {
		avg += input;
	  	if (DataLen == 0) {
	  		if (maxset == FALSE) max = input;
	  		if (minset == FALSE) min = input;
	  	}
	  	else {
			if ((input > max) & (maxset == FALSE)) max = input;
			if ((input < min) & (minset == FALSE))  min = input;
		}
		DataLen++;
	  }
	  fclose(f);


		avg = avg/(float) DataLen;
		printf("#qtp -f %s -p %d -ps %d -w %ld -ws %ld \n",FileName, PRange, PIncr, Window, Shift);
		if (midset==FALSE) {
			if (autoset==FALSE) mid=(max+min)/2;
			else mid = avg;
		}

		if (DataLen) printf("#%f %f %f %f %ld\n", min, avg, max, mid, DataLen);
		max -= mid;
		min -= mid;
		max = fabs(max-min)/2000.0; // scale max to 1000
//		Data = (char *) malloc(2*DataLen);

		Data = (int *) malloc(DataLen * sizeof(int));
		j = 0;
	  f = fopen(FileName, "r"); // now read in the data again
	  while (EOF != fscanf(f, "%f", &input)) {
		Data[j++] = (int) (((float) input - mid)/max); // printf("debug %f %d\n", input, val);
//		Data[j++] = (char) (val >> 8) & 255  ;
//		Data[j++] = (char) val & 255;
	  }
	  fclose(f);
	}
	else if ((FromFile == TRUE) && (String == TRUE)) { // string to process
	  	struct stat stbuf;

	  if (stat(FileName, &stbuf) == -1)
		return(-1);
		else {
			DataLen = stbuf.st_size;

			Dynamics = (char *)malloc(DataLen);
			f = fopen(FileName, "r");
			fread(Dynamics, 1, DataLen, f);
			fclose(f);
		}
		HashTableSize = DataLen + 255;
		EntryList = (MATCHLISTENTRY*) malloc(sizeof (MATCHLISTENTRY) * HashTableSize);
		PrimeList = (CODEWORD *)malloc ((DataLen + 2) * sizeof (CODEWORD)); // two extra nodes, head, end nodes in string list.

		Dynamics = (char *) malloc(DataLen);
		Dynamicsrev = (char *) malloc(DataLen);
		if ((EntryList == NULL) || (PrimeList == NULL) || (Dynamics == NULL) || (Dynamicsrev == NULL)) {printf("Error Allocating Memory for linked list\n"); return -1;}
		Complexity = ftdSelAugment(&Dynamics[0], DataLen);
		if (Info == FALSE) printf("%.5f\n", Complexity);
		 else {
			if (Binary == TRUE) printf("%.5f\n", Scale*(invlogint(Complexity))*log(2.0)/DataLen);//millibits/sample
			else printf("%.5f\n", Scale*invlogint(Complexity)/DataLen); //millinats/sample
		 }

		free(Dynamicsrev);
		free(Dynamics);
		free (PrimeList);
		free (EntryList);

	}


	if (Surface==TRUE) {
	    HashTableSize = Window + 255;  										// approximate the maximum number of different codewords that might occur during whole process

          // Allocate memory
    	  EntryList = (MATCHLISTENTRY*) malloc(sizeof (MATCHLISTENTRY) * HashTableSize);
	      PrimeList = (CODEWORD *)malloc ((Window + 2) * sizeof (CODEWORD)); // two extra nodes, head, end nodes in string list.

   		  Dynamics = (char *) malloc(DataLen);
   		  Dynamicsrev = (char *) malloc(Window);
  		  if ((EntryList == NULL) || (PrimeList == NULL) || (Dynamics == NULL) || (Dynamicsrev == NULL)) {printf("Error Allocating Memory for linked list\n"); return -1;}
			j=0; for (i = 0; i <= (DataLen - Window); i+= Shift) j++;
			printf("s:partiondata\n%ld %d\n%f %f %f\n%f %f %f\n", j, (2*PRange)/PIncr + 1, xscale, yscale, zscale, xoffset, yoffset, zoffset);

			for (Partition = -PRange; Partition <= PRange; Partition += PIncr) { // assumes PRange
				for (i=0; i < DataLen; i++) {
					Dynamics[i] = (Data[i] >= Partition);
				}
			// calculate complexities
			   for (i = 0; i <= (DataLen - Window); i+= Shift) {
					Complexity1 = ftdSelAugment(&Dynamics[i], Window);
					if (BestDir) { // minimum complexity for both forward and reverse directions
						for (l=0; l< Window; l++) Dynamicsrev[Window-1-l]=Dynamics[i+l];
						Complexity2 = ftdSelAugment(&Dynamicsrev[0], Window);
						//printf("\n%f %f %f\n",Complexity1,Complexity2,Complexity1-Complexity2);
						if (Complexity1 > Complexity2 ) Complexity1 = Complexity2;
					}
					if (Info == FALSE) printf("%.5f ", Complexity1);
					 else {
						if (Binary == TRUE) printf("%.5f ", Scale*(invlogint(Complexity1))*log(2.0)/Window);//millibits/sample
						else printf("%.5f ",  Scale*invlogint(Complexity1)/Window); //millinats/sample
					 }
				}


				printf("\n");
			} // end of Partition

		free(Dynamicsrev);
		free(Dynamics);
		free (PrimeList);
		free (EntryList);

	} // end surface

    // ********* raster calculation *************
	if (Raster==TRUE) { //calculate the total entropy
			HashTableSize = (2*PRange/PIncr + 1) *  Window + 255;
		  // Allocate memory
		  EntryList = (MATCHLISTENTRY*) malloc(sizeof (MATCHLISTENTRY) * HashTableSize);
		  PrimeList = (CODEWORD *)malloc (((2*PRange/PIncr + 1) * Window + 2) * sizeof (CODEWORD));

		  Dynamics = (char *) malloc((2*PRange/PIncr + 1) * Window);
		  Dynamicsrev = (char *) malloc((2*PRange/PIncr + 1) * Window);
		  if ((EntryList == NULL) || (PrimeList == NULL) || (Dynamics == NULL) || (Dynamicsrev == NULL)) {printf("Error Allocating Memory for linked list\n"); return -1;}
			j=0; for (i = 0; i <= (DataLen - Window); i+= Shift) j++;
		  printf("z:g:partionraster\n%ld 1\n%f %f %f\n%f %f %f\n", j , xscale, yscale, zscale * 2.5, xoffset, yoffset, zoffset);

		    first = (float *) malloc (j * sizeof(float) );
			k = 0;
		  for (m = 0; m <= DataLen-Window  ; m+= Shift) { // set window position

				j=0; l= (2*PRange/PIncr + 1) * Window;
				for (Partition = -PRange; Partition <= PRange; Partition += PIncr) { // raster over the whole range
					for (i=m; i < (m+Window); i++) {
						Dynamicsrev[--l] = Dynamics[j++] = (Data[i] >= Partition);
					} // step along data
				} // raster step
				Complexity1 = ftdSelAugment(&Dynamics[0], (2*PRange/PIncr + 1) *  Window);
				if (BestDir) { // find the minimum complexity out of forward and reverse directions
					Complexity2 = ftdSelAugment(&Dynamicsrev[0], (2*PRange/PIncr + 1) *  Window);
					//printf("\n%f %f %f\n",Complexity1,Complexity2,Complexity1-Complexity2);
					if (Complexity1 > Complexity2 ) Complexity1 = Complexity2; //take the smallest value
				}
				if (Info == FALSE) printf("%.5f ", first[k++] = Complexity1);
				 else {
						if (Binary == TRUE) printf("%.5f ", first[k++] = Scale*(invlogint(Complexity1))*log(2.0)/((2*PRange/PIncr + 1) * Window));//bits/sample
						else printf("%.5f ", first[k++] = Scale*invlogint(Complexity1)/((2*PRange/PIncr + 1) * Window)); //nats/sample
				 }
		  } // window shift
				printf("\nv:f:off\n");
		free(Dynamicsrev);
		free(Dynamics);
		free (PrimeList);
		free (EntryList);
	}// end of raster stuff

	// **************** calculate the maximum for the raster ***************
	if (Maximum==TRUE) { //calculate the maximum entropy

		  // Allocate memory
		  HashTableSize = Window + 255;
		  EntryList = (MATCHLISTENTRY*) malloc(sizeof (MATCHLISTENTRY) * HashTableSize);
		  PrimeList = (CODEWORD *)malloc ((Window + 2) * sizeof (CODEWORD));
		  Dynamics = (char *) malloc(Window);
		  Dynamicsrev = (char *) malloc(Window);
		  if ((EntryList == NULL) || (PrimeList == NULL) || (Dynamics == NULL) || (Dynamicsrev == NULL)) {printf("Error Allocating Memory for linked list\n"); return -1;}
		  j=0; for (i = 0; i <= (DataLen - Window); i+= Shift) j++; // This needs to be sorted
		  printf("z:r:maxentropy\n%ld 1\n%f %f %f\n%f %f %f\n", j, xscale, yscale, zscale  , xoffset, yoffset, zoffset);

		  k = 0;
		  second = (float *) malloc (j * sizeof(float));

		  for (m = 0; m <= DataLen-Window  ; m+= Shift) { // set window position
				maxComplexity = 0.0;

				for (Partition = -PRange; Partition <= PRange; Partition += PIncr) { // raster over the whole range
					j=0; l= Window;
					for (i=m; i < (m+Window); i++) {
						Dynamicsrev[--l] = (Dynamics[j++] = (Data[i] >= Partition));
						//printf("%d", Dynamics[j-1]);
					} // step along data
					if (Partition == 0) { maxComplexity = ftdSelAugment(&Dynamics[0], Window); }
					else {
						Complexity1 = ftdSelAugment(&Dynamics[0], Window);
						if (BestDir) { // minimum complexity for both forward and reverse directions
							Complexity2 = ftdSelAugment(&Dynamicsrev[0], Window);
							if (Complexity1 < Complexity2 ) Complexity1 = Complexity2;
						}
						if (Complexity1 > maxComplexity) maxComplexity = Complexity1;
					} // raster step
				} // partition increment
				if (Info == FALSE) printf("%.5f ", second[k++] = maxComplexity);
				 else {
						if (Binary == TRUE) printf("%.5f ", second[k++] = Scale*(invlogint(maxComplexity))*log(2.0)/Window);//millibits/sample
						else printf("%.5f ", second[k++] = Scale*invlogint(maxComplexity)/Window); //millinats/sample
				}
		}// window shift
				printf("\n");
		free(Dynamicsrev);
		free(Dynamics);
		free (PrimeList);
		free (EntryList);
	} // end of maxcalc

	if ((Maximum == TRUE) & (Raster == TRUE)) {// output kurve
		  j=0; for (i = 0; i <= (DataLen - Window); i+= Shift) j++; // this needs to be improved!

		  printf("y:g:raster\n%ld 1\n%f %f %f\n%f %f %f\n", j, xscale, 2.5 * zscale, yscale  , xoffset, yoffset, zoffset);
		  for (i = 0; i < j ; i++) printf ("%f ", first[i]);
		  printf("\n");

		  printf("k:p:maxent_vs_raster\n%ld 2\n%f %f %f\n%f %f %f\n", j, xscale, 2.5 * zscale, zscale  , xoffset, yoffset, zoffset);
		  for (i = 0; i < j ; i++) printf ("%f ", second[i]);
		  printf("\n");
		  for (i = 0; i < j ; i++) printf ("%f ", first[i]);
		  printf("\np:.:points\nw 1.0\n");

		  j=0; for (i = 0; i <= (DataLen - Window); i+= Shift) j++; // this needs to be improved!
		  printf("k:p:maxent_vs_raster\n%ld 2\n%f %f %f\n%f %f %f\n", j, xscale, 2.5 * zscale, zscale  , xoffset, yoffset, zoffset);
		  for (i = 0; i < j ; i++) printf ("%f ", second[i]);
		  printf("\n");
		  for (i = 0; i < j ; i++) printf ("%f ", first[i]);
		  printf("\nw .3\n");
	}

	if (defaultviewing == TRUE) printf("\nO 339 329\nL 298 29\nC -7.300 1.10 0.00 0.00\nI -1.0 0.0 0.01 0.0 -1.0\n");

	free(Data);
  return 0;
}
