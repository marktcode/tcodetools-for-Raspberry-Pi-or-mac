/* sqtp assumes spodio formats, that is input and output spod formats
  could add  fileio module so that it more generally handles input... but then we cn rely on spod to do that!
*/




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




  // Data Array
	long DataLen;
	int *Data;
	int string = FALSE;

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

  int done, Partition, defaultviewing = FALSE, Raster = FALSE, Surface=TRUE, String=FALSE, newsettings=FALSE;
  int Loop, Range, Startoffset, Start, Finish, DataOut=FALSE, LogPart=FALSE, BestDir=FALSE,  val, autoset = TRUE, minset=FALSE, maxset=FALSE ;
  long	i, j, k, l, m, offset, count2;
  long	Window = 10000, Shift = 1000;
  float xscale = 9.0, yscale = 3.0 ,zscale = 1.0 , xoffset = 0.0, yoffset = 0.0 ,zoffset = 0.0;
  float nxscale = 9.0 , nyscale = 3.0 , nzscale = 1.0 , nxoffset = 0.0, nyoffset = 0.0, nzoffset = 0.0;
  float Threshold, level, Scale=1.0, *Input, avg, max, min, width = 0.0;
  float *Rasterlist, *MaxList;
  double Complexity1, Complexity2,  maxComplexity;
  char temp;
  char object, tempobject, colour, label[100];


      String = FALSE; // default no strig input
      Surface = TRUE;
      Raster = FALSE;
      defaultviewing = TRUE;


  // Read arguments from command line
	for (Loop = 1; Loop < argc; Loop++) {
		if ((strcmp(argv[Loop], "?")== 0)|| (strcmp(argv[Loop], "-?")== 0) || (strcmp(argv[Loop], "-h")== 0)) {
			printf("\t spod compatible sqtp (quick t-partition V3.0\n");
			printf("\t Copyright (C) 2007 M. R. Titchener, University of Auckland\n");
			printf("\t\tExecutable Program and its Source licenced under the \n");
			printf("\t\tGNU LESSER GENERAL PUBLIC LICENSE,\n");
			printf("\t\t\tas set out in the associate file COPYING.TXT\n");
			printf("\t\tIncludes code by M.R.Titchener, U.Speidel, J. Yang \n");
			printf("\t\t\t     -h \t prints this info\n");
			printf("\t\t\t     -p n s\t sets partition range +/- n and increment s \n");
			printf("\t\t\t     -w n s\t sets window to n and wndow shift increment to s\n");
			printf("\t\t\t     -i \t outputs in (nats) else in (taugs)\n");
			printf("\t\t\t     -r \t computes the minimum complexity parsing over both directions\n");
			printf("\t\t\t     -R \t computes total over Rastered window \n");
			printf("\t\t\t     -ud \t Information units decinats/decibits \n");
			printf("\t\t\t     -uc \t Information units centinats/centibits \n");
			printf("\t\t\t     -um \t Information units millinats/millibits \n");
			printf("\t\t\t     -a \t calculates average position of raster \n");
			printf("\t\t\t     -b \t outputs in (bits) \n");
			printf("\t\t\t     -c \t outputs default viewing parameters\n");
			printf("\t\t\t     -width \t processes the fraction of the total dynamic range\n");
			printf("\t \n");
			return 0;
		}
		else if (strcmp(argv[Loop], "-w") == 0) {  // window size
			sscanf(argv[Loop + 1], "%ld", &Window);
			Loop++;
			sscanf(argv[Loop + 1], "%ld", &Shift);
			Loop++;
		}
		else if (strcmp(argv[Loop], "-p") == 0) {
			sscanf(argv[Loop + 1], "%d", &Start);
			Loop++;
			sscanf(argv[Loop + 1], "%d", &Finish);
			Loop++;
			if (Finish - Start < 10  )  { printf ("\nIn switch '-p %d %d'; %d must be 10 or more than %d\n", Start, Finish, Finish, Start); exit (1); }
		}
		else if (strcmp(argv[Loop], "-r") == 0) { // minimum complexity for both directions
			BestDir=TRUE;
		}
		else if (strcmp(argv[Loop], "-R") == 0) { // Raster calculation
			Surface=FALSE;
			Raster=TRUE;
			String=TRUE;
		}
		else if ((strcmp(argv[Loop], "-RS") == 0) || ((strcmp(argv[Loop], "-SR") == 0))) { // and raster
			Surface=TRUE;
			Raster=TRUE;
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
		else if (strcmp(argv[Loop], "-width") == 0)  {
			sscanf(argv[Loop + 1], "%f", &width);
			Loop++;
		}
		else if (strcmp(argv[Loop], "-xyz") == 0) {  // newsettings
			newsettings=TRUE;
			sscanf(argv[Loop + 1], "%f", &nxscale);
			Loop++;
			sscanf(argv[Loop + 1], "%f", &nyscale);
			Loop++;
			sscanf(argv[Loop + 1], "%f", &nzscale);
			Loop++;
			sscanf(argv[Loop + 1], "%f", &nxoffset);
			Loop++;
			sscanf(argv[Loop + 1], "%f", &nyoffset);
			Loop++;
			sscanf(argv[Loop + 1], "%f", &nzoffset);
			Loop++;
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

	int graphnum = 1;
	done = scanf("%c",&object);	// get first character
	while ((done != EOF) && (graphnum != 0)) {
		while (( object != 'Y') && ( object != 'y') &&  ( object != 'Z') && ( object != 'z'))  { done = scanf("%c",&object); }
		done = scanf("%c",&tempobject); // read next
		if ( tempobject == ':') {
			graphnum--;
			done = scanf("%c:",&colour);
			i = 0;
			while  (( EOF != done ) && (i < 80)) {
				done = scanf("%c",&label[i]);
				if ( ':' == (char) label[i]) break;
				i++;
			}
			label[i+1] = 0;
		}
	}

	done = scanf(" %ld: %ld:",&DataLen, &count2);

	if (count2 > 1) { printf("\nIncorrect size for array second parameter\n"); exit (1); }
	done = scanf(" %f: %f: %f:", &xscale, &yscale, &zscale);
	done = scanf(" %f: %f: %f", &xoffset, &yoffset, &zoffset);

	if (newsettings==FALSE) {
		nxscale=xscale;
		nyscale=yscale;
		nzscale=zscale;
		nxoffset=xoffset;
		nyoffset=yoffset;
		nzoffset=zoffset;
	}





	Input = (float *) malloc(DataLen * sizeof(int));
	Data = (int *) malloc(DataLen * sizeof(int));

	avg = 0.0;
	if ((DataLen >= 1) && (count2 == 1)) {

		printf("%c:%c:%s :%ld:1 :%g:%g:%g :%g:%g:%g\n",object, colour, label, DataLen, nxscale, yscale, zscale, nxoffset, nyoffset, nzoffset); //echo graph but with a ball
		for (i = 0; i < DataLen; i++) {
			scanf("%f", &Input[i]);
			if (i==0) max = min = Input[0];
			else {
				if (max < Input[i]) max =  Input[i];
				if (min > Input[i]) min =  Input[i];
			}
			avg += Input[i];
			printf ("%g ", Input[i]);
		}

		if (((object == 'y') || (object == 'z')) && (DataLen > 500)) printf ("\np:.:points\n");

		avg = avg/(float) DataLen;
		printf("\nb:m:\\ \n\n#sqtp -p %d %d -w %ld %ld \n", Start, Finish, Window, Shift);

		if (DataLen) printf("#min: %g,  avg: %g, max: %g, Datalength: %ld\n", min, avg, max, DataLen);

		if (Start == -Finish ) {
			Range = 2 * Finish; // is symmetric
			Startoffset = 0;
		}
		else {
			if (-Start > Finish) { Range = - 2 * Start; Startoffset = 0;   }		// use Start to define scaling range
			else if (-Start < Finish) { Range = 2 * Finish;  Startoffset = Start + Finish; } // use Finish to define scaling range
		}

		float floatrange;
		if (width) { floatrange = max-min; max = avg+floatrange*width; min = avg - floatrange*width; }
		for (i = 0; i < DataLen; i++) Data[i++] = (int) (( (Input[i] - min) * Range)/(max-min) - Startoffset) ;
		free(Input);


		if (Surface==TRUE) {
			HashTableSize = Window + 255;  										// approximate the maximum number of different codewords that might occur during whole process

			  // Allocate memory
			  EntryList = (MATCHLISTENTRY*) malloc(sizeof (MATCHLISTENTRY) * HashTableSize);
			  PrimeList = (CODEWORD *)malloc ((Window + 2) * sizeof (CODEWORD)); // two extra nodes, head, end nodes in string list.

			  Dynamics = (char *) malloc(DataLen);
			  Dynamicsrev = (char *) malloc(Window);
			  if ((EntryList == NULL) || (PrimeList == NULL) || (Dynamics == NULL) || (Dynamicsrev == NULL)) {printf("Error Allocating Memory for linked list\n"); return -1;}
				j=0; for (i = 0; i <= (DataLen - Window); i+= Shift) j++;
				printf("s:p:partitiondata: %ld:%d: %g:%g:%g: %g:%g:%g\n", j, (Finish-Start) + 1, nxscale, nyscale, nzscale, nxoffset, nyoffset, nzoffset);


				MaxList = (float *) malloc (j * sizeof (float)); // record maximum.


				for (Partition = 0; Partition <= Finish - Start; Partition++) {
					for (i=0; i < DataLen; i++) {
						Dynamics[i] = (Data[i] >= Partition);
					}

					k=0;
				// calculate complexities
				   for (i = 0; i <= (DataLen - Window); i+= Shift) {
						Complexity1 = ftdSelAugment(&Dynamics[i], Window);
						if (BestDir) { // minimum complexity for both forward and reverse directions
							for (l=0; l< Window; l++) Dynamicsrev[Window-1-l]=Dynamics[i+l];
							Complexity2 = ftdSelAugment(&Dynamicsrev[0], Window);
							//printf("\n%g %g %g\n",Complexity1,Complexity2,Complexity1-Complexity2);
							if (Complexity1 > Complexity2 ) Complexity1 = Complexity2;
						}
						if (Info == FALSE) printf("%.5f ", Complexity1);
						else {
							if (Binary == TRUE) printf("%.5f ", Scale*(invlogint(Complexity1))*log(2.0)/Window);//millibits/sample
							else printf("%.5f ",  Scale*invlogint(Complexity1)/Window); //millinats/sample
						}
						if (Partition == 0)  MaxList[k] = Complexity1;
						else if (Complexity1 > MaxList[k]) MaxList[k] = Complexity1;
						k++;
					}



					printf("\n");
				} // end of Partition

			printf("\n\nz:r:maxent: %ld:1: %g:%g:%g: %g:%g:%g\n", j, nxscale, nyscale, nzscale, nxoffset, nyoffset, nzoffset);
			if (Info == FALSE) for (i = 0; i < j ; i++) printf ("%g ", MaxList[i]);
			else {
				if (Binary == TRUE) for (i = 0; i < j ; i++) printf("%.5f ", MaxList[i] = Scale*(invlogint(MaxList[i]))*log(2.0)/Window);//millibits/sample
				else for (i = 0; i < j ; i++) printf("%.5f ",  MaxList[i] = Scale*invlogint(MaxList[i])/Window); //millinats/sample
			}

			printf("b:r:\\ \n");


			free(Dynamicsrev);
			free(Dynamics);
			free (PrimeList);
			free (EntryList);

		} // end surface

		// ********* raster calculation *************
		if (Raster==TRUE) { //calculate the total entropy
				HashTableSize = (Finish - Start + 1) *  Window + 255;
			  // Allocate memory
			  EntryList = (MATCHLISTENTRY*) malloc(sizeof (MATCHLISTENTRY) * HashTableSize);
			  PrimeList = (CODEWORD *)malloc (((Finish - Start + 1) * Window + 2) * sizeof (CODEWORD));

			  Dynamics = (char *) malloc((Finish - Start + 1) * Window);
			  Dynamicsrev = (char *) malloc((Finish - Start + 1) * Window);
			  if ((EntryList == NULL) || (PrimeList == NULL) || (Dynamics == NULL) || (Dynamicsrev == NULL)) {printf("Error Allocating Memory for linked list\n"); return -1;}
				j=0; for (i = 0; i <= (DataLen - Window); i+= Shift) j++;
			  printf("\n\nz:g:partitionraster: %ld:1 :%g:%g:%g :%g:%g:%g\n", j , nxscale, 0.0, 5.0 * nzscale , nxoffset, nyoffset, nzoffset);

				Rasterlist = (float *) malloc (j * sizeof(float) );
				k = 0;
			  for (m = 0; m <= DataLen-Window  ; m+= Shift) { // set window position

					j=0; l= (Finish - Start + 1) * Window;
					for (Partition = 0; Partition <= Finish - Start; Partition++) { // raster over the whole range
						for (i=m; i < (m+Window); i++) {
							Dynamicsrev[--l] = Dynamics[j++] = (Data[i] >= Partition);
						} // step along data
					} // raster step
					Complexity1 = ftdSelAugment(&Dynamics[0], (Finish - Start + 1) *  Window);
					if (BestDir) { // find the minimum complexity out of forward and reverse directions
						Complexity2 = ftdSelAugment(&Dynamicsrev[0], (Finish - Start + 1) *  Window);
						//printf("\n%g %g %g\n",Complexity1,Complexity2,Complexity1-Complexity2);
						if (Complexity1 > Complexity2 ) Complexity1 = Complexity2; //take the smallest value
					}
					if (Info == FALSE) printf("%.5f ", Rasterlist[k++] = Complexity1);
					 else {
							if (Binary == TRUE) printf("%.5f ", Rasterlist[k++] = Scale*(invlogint(Complexity1))*log(2.0)/((Finish - Start + 1) * Window));//bits/sample
							else printf("%.5f ", Rasterlist[k++] = Scale*invlogint(Complexity1)/((Finish - Start + 1) * Window)); //nats/sample
					 }
			  } // window shift
			printf("\nv:f:off\nb:g:\\ \n");
			free(Dynamicsrev);
			free(Dynamics);
			free (PrimeList);
			free (EntryList);
		}// end of raster stuff


		if ((Surface == TRUE) && (Raster == TRUE)) {// output kurve
			  j=0; for (i = 0; i <= (DataLen - Window); i+= Shift) j++; // this needs to be improved!

			  printf("\ny:g:partn_raster: %ld:1 :%g:%g:%g :%g:%g:%g\n", j, nxscale, 5.0 * nzscale, 0.0  , nxoffset, nyoffset, nzoffset);
			  for (i = 0; i < j ; i++) printf ("%g ", Rasterlist[i]);
			  printf("\nv:f:off\nb:g:\\ \n");

			  printf("\nk:y:maxent_vs_raster: %ld:2 :%g:%g:%g :%g:%g:%g\n", j, nxscale, 5.0*nzscale, nzscale  , nxoffset, nyoffset, nzoffset);
			  for (i = 0; i < j ; i++) printf ("%g ", Rasterlist[i]);
			  printf("\n");
			  for (i = 0; i < j ; i++) printf ("%g ", MaxList[i]);
			  printf("\np:.:points\nw 1.0\n");

		//	  j=0; for (i = 0; i <= (DataLen - Window); i+= Shift) j++; // this needs to be improved!
			  printf("\nk:y:maxent_vs_raster: %ld:2 :%g:%g:%g :%g:%g:%g\n", j, nxscale, 5.0*nzscale, nzscale , nxoffset, nyoffset, nzoffset);
			  for (i = 0; i < j ; i++) printf ("%g ", Rasterlist[i]);
			  printf("\n");
			  for (i = 0; i < j ; i++) printf ("%g ", MaxList[i]);
			  printf("\nw .3\n");

		}
	} // end of count1>1 && count2 =1;

	if (defaultviewing == TRUE) printf("\nO: 339:329\nL: 298:29: 353:1: 0:0\nC: 1.0:1.0:0.0: 1.0:0.0:0.0: 0.0\nI -1.0 0.0 0.01 0.0 -1.0\n");


	fflush(stdout);

	if (Raster==TRUE) free(Rasterlist);
	if (Surface==TRUE) free(MaxList);

	free(Data);
    return 0;
}
