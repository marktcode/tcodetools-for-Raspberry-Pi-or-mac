
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

#define EMPTY -1

#define PROGRESSNONE 0
#define PROGRESSAUG 1
#define PROGRESSPERCENT 2
#define PROGRESSLENGTH 3
#define PROGRESSPOINT 4

#define MAXIT 100
#define EULER 0.5772156649
#define FPMIN 5.0e-308
#define EPS 5.0e-16

#define FALSE 0
#define TRUE 1

#define NORMAL 0  // as it used to be
#define RUNIT 1  // take the maximum run of the prefix
#define ORDERLY 2  // Take the shortest prefix and max run

  // Command Line Flags
  int FromFile = FALSE;
  char *FileName = NULL;
  int RemoveLast = FALSE;
  int RemovedLast = FALSE;
  long Progress = 0;
  long NextProgress = 0;
  int ProgressType = PROGRESSNONE;
  int DisplayAug = FALSE;
  int DisplayList = FALSE;
  int CalcAlphabet = FALSE;
  int FullDebug = FALSE;
  int VerboseDisplay = FALSE;
  int InterpolateAug = FALSE;
  int CalculateTInfo = FALSE;
  int ReverseData = FALSE;
  int Binary = FALSE;
  int Kolmogorov = FALSE;


  // Data Array
  long DataLen;
  char *Data;

  char tempData;
  // Linked List
  long Head = EMPTY, Tail = EMPTY;
  long *Next, *Last;

  // Augmentation Data
  long AugCount = 0;
  long Prefix = EMPTY, ShortestPrefix = EMPTY, K = 0;
  long LastK = 1;

  // Complexity
  double Complexity = 0.0;
  double LastComplexity = 0.0, MinComplexity = 0.0;
  int StartFlag = FALSE, whichprocess = NORMAL;

  // T-information
  double LastInfo = 0;
  long LastInfoPos = 0;
  long OldPos = 0;


  double SaveComplexity = 0.0;
  double SaveInfo = 0.0 ;
  long  SaveInfoPos = 0;

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

long FileSize(char *FileName) {
  struct stat stbuf;

  if (stat(FileName, &stbuf) == -1)
    return(-1);
  else
    return(stbuf.st_size);
}

void LoadFile() {

  FILE *f;

  // Allocate memory
  DataLen = FileSize(FileName);

  // Load Data
  Data = (char *)malloc(DataLen);
  f = fopen(FileName, "r");
  fread(Data, 1, DataLen, f);
  fclose(f);

  // Remove last character if new line
  if ((RemoveLast == TRUE) && (Data[DataLen - 1] == '\n')) {
    RemovedLast = TRUE;
    DataLen--;
  }

}

void Initialise() {

  long Loop;

  if (FullDebug)
    printf("Requesting Linked List Memory\n");

  // Allocate memory
  Next = (long *)malloc(DataLen * sizeof(long));
  Last = (long *)malloc(DataLen * sizeof(long));

  if ((Next == NULL) || (Last == NULL))
    printf("Error Allocating Memory for linked list\n");

  if (FullDebug)
    printf("Linked List Memory Allocated\n");

  // Initilise pointers
  Head = 0;
  Tail = DataLen - 1;

  if (FullDebug)
    printf("Head and Tail Initialised\n");

  for (Loop = 0; Loop < DataLen; Loop++) {
    Next[Loop] = Loop + 1;
    Last[Loop] = Loop - 1;
  }
  Next[DataLen - 1] = EMPTY;
  Last[0] = EMPTY;

  if (FullDebug)
    printf("Linked List Initialised\n");

}

void CalculateAlphabet() {

  int Alphabet[512], AlphaIndex;
  long Loop, AlphabetCount = 0;

  if (FullDebug)
    printf("Initialising ALphabet\n");

  for (Loop = 0; Loop < 512; Loop++)
    Alphabet[Loop] = FALSE;

  if (FullDebug)
    printf("Analysing Data\n");

  for (Loop = 0; Loop < DataLen; Loop++)
    Alphabet[(int)Data[Loop] + 256] = TRUE;

  if (FullDebug)
    printf("Counting Alphabet\n");

  for (Loop = 0; Loop < 512; Loop++)
    if (Alphabet[Loop] == TRUE)
      AlphabetCount++;

  if (VerboseDisplay)
    printf("Alphabet : %ld characters\n", AlphabetCount);

  if (FullDebug)
    printf("Finalising Alphabet\n");

}

void DisplayLinks() {

  long Loop;

  printf("Next [");
  for (Loop = 0; Loop < DataLen; Loop++)
    if (Loop < DataLen - 1)
      printf("%ld,", Next[Loop]);
    else
      printf("%ld] Head = %ld\n", Next[Loop], Head);

  printf("Last [");
  for (Loop = 0; Loop < DataLen; Loop++)
    if (Loop < DataLen - 1)
      printf("%ld,", Last[Loop]);
    else
      printf("%ld] Tail = %ld\n", Last[Loop], Tail);

}

long Length(long Pos) {

  if (Next[Pos] != EMPTY)
    return Next[Pos] - Pos;
  else
    return DataLen - Pos;

}

int Compare(long FirstPos, long SecondPos) {

  long Len, Loop;

  // Test same length
  if (Length(FirstPos) != Length(SecondPos))
    return FALSE;
  else {
    // Get bound to test
    Len = Length(FirstPos) - 1;
    // Test last character
    if (Data[FirstPos + Len] != Data[SecondPos + Len])
      return FALSE;
    // Test remaining characters
    for (Loop = 0; Loop < Len; Loop++)
      if (Data[FirstPos + Loop] != Data[SecondPos + Loop])
        return FALSE;
    return TRUE;
  }

}

void Display(long Pos) {

  long Len, Loop;

  Len = Length(Pos);
  for (Loop = 0; Loop < Len; Loop++)
    if (Loop != Len - 1)
      printf("%c ", Data[Pos + Loop]);
    else
      printf("%c", Data[Pos + Loop]);

}

void DisplayPrefix() {

  printf("K = %ld\tPrefix = ", K);
  Display(Prefix);
  printf("\n");

}

void DisplayAll() {

  long Pos;

  printf("Data [");
  Pos = Head;
  while (Pos != EMPTY) {
    Display(Pos);
    Pos = Next[Pos];
    if (Pos != EMPTY)
      printf(".");
  }
  if (Head == Tail)
    printf("]\n");
  else {
    printf("]\tK = %ld\tPrefix = ", K);
    Display(Prefix);
    printf("\n");
  }

}

void SelectPrefix() {

  long Pos;

  if (Tail != EMPTY) {
    Prefix = Last[Tail];
    Pos = Prefix;
    K = 0;
    while ((Pos != EMPTY) && Compare(Pos, Prefix)) {
      K++;
      Pos = Last[Pos];
    }
  }
}

void SelectMaxRunPrefix() {

  long Pos, remK;

	remK=0;
  	if (Tail != EMPTY) {
    	Prefix = Last[Tail];

    	Pos = Prefix;
	while (Pos != EMPTY) {
    	K = 0;
    	while ((Pos != EMPTY) && Compare(Pos, Prefix)) {
      		K++;
    		Pos = Last[Pos];
    	}
    	if (Pos != EMPTY) Pos = Last[Pos];
    	if (K >= remK) remK = K;
    }
  	K = remK;
  }
}

void SelectShortestMaxRunPrefix() {

  long Pos, remK;

	remK=0;
  	if (Tail != EMPTY) {
    	if (Prefix == EMPTY) Prefix = Last[Tail];
    	else Prefix = ShortestPrefix;

    	Pos = Head; // lets go in forward direction
	while (Pos != EMPTY) {
    	K = 0;
    	while ((Pos != EMPTY) && Compare(Pos, Prefix)) {
      		K++;
    		Pos = Next[Pos];
    	}
    	if (Pos != EMPTY) Pos = Next[Pos];
    	if (K >= remK) remK = K;
    }
  	K = remK;
  }
}

void Combine(long FirstPos, long LastPos) {

  long Following, Pos, NextPos;

  Following = Next[LastPos];

  Pos = Next[FirstPos];
  while ((Pos != EMPTY) && (Pos <= LastPos)) {
    NextPos = Next[Pos];
    Next[Pos] = EMPTY;
    Last[Pos] = EMPTY;
    Pos = NextPos;
  }

  Next[FirstPos] = Following;
  if (Following == EMPTY) {
    OldPos = DataLen - Tail;
    Tail = FirstPos;
  }
  else
    Last[Following] = FirstPos;

}

void Augment() {

  long Pos, FirstPos, LastPos;
  int Matching = FALSE;
  long Matched = 0;

  // Calculate complexity
  AugCount++;
  LastComplexity = Complexity;
  LastK = K;
  Complexity += log((double)K + 1)/log(2.0);
  Pos = Head;
  while (Pos != EMPTY) {
    if (Compare(Pos, Prefix) == TRUE)
      if (Matching == TRUE) {
        Matched++;
        if (Matched > K) {
          Matching = FALSE;
          LastPos = Pos;
          Pos = Next[Pos];
          Combine(FirstPos, LastPos);
        }
        else
          Pos = Next[Pos];
      }
      else {
        Matching = TRUE;
        Matched = 1;
        FirstPos = Pos;
        Pos = Next[Pos];
      }
    else {
      if (Matching == TRUE) {
        Matching = FALSE;
        LastPos = Pos;
        Pos = Next[Pos];
        Combine(FirstPos, LastPos);
      }
      else
        Pos = Next[Pos];
    }
  }

}


void AugmentShortestMaxRunPrefix() {

  long Pos, FirstPos, LastPos, LowestLength, Endstop;
  int Matching = FALSE;
  long Matched = 0;


  LowestLength = DataLen;
  // Calculate complexity
  AugCount++;
  LastComplexity = Complexity;
  LastK = K;
  Complexity += log((double)K + 1)/log(2.0);
  Pos = Head;

  while (Pos != EMPTY) {
    if (Compare(Pos, Prefix) == TRUE)
      if (Matching == TRUE) {
        Matched++;
        if ((Matched > K) || (Pos > Last[Tail])) {		// got a match but now past the tail
          Matching = FALSE;
          LastPos = Pos;
          Pos = Next[Pos];
          Combine(FirstPos, LastPos);
          if ((LastPos < Last[Tail]) && (Length(FirstPos) <= LowestLength)) {
          	LowestLength = Length(FirstPos);
          	ShortestPrefix = FirstPos;
//          	printf("firstpos1 %d\n",FirstPos);
          }
        }
        else
          Pos = Next[Pos];
      }
      else {						// first time a match, set firstpos
        Matching = TRUE;
        Matched = 1;
        FirstPos = Pos;
        Pos = Next[Pos];
      }
    else {
      if (Matching == TRUE) {
        Matching = FALSE;
        LastPos = Pos;
        Pos = Next[Pos];
        Combine(FirstPos, LastPos);
    	if ((LastPos < Last[Tail]) && (Length(FirstPos) <= LowestLength)) {
    		LowestLength = Length(FirstPos);
          	ShortestPrefix = FirstPos;
//          	printf("firstpos2 %d\n",FirstPos);
         }

      }
      else {
    	if ((Next[Pos] != EMPTY) && (Length(Pos) <= LowestLength)) {
    		LowestLength = Length(Pos);
    		ShortestPrefix = Pos;
//          	printf("firstpos3 %d\n",Pos);
        }

        Pos = Next[Pos];
      }
    }
  }

//  printf("\n length %d \n", LowestLength);				// output length of current (Length(FirstPos))

}

void DisplayProgress() {

  long Pos, ThisK, ThisPos;
  double Percentage;
  double ThisComplexity = 0.0, ThisInfo;

  Pos = DataLen - Tail;
  Percentage = (double)(Pos * 100) / (double)DataLen;

  switch (ProgressType) {
  case PROGRESSAUG :
    if (AugCount % Progress == 0) {
      if (InterpolateAug)
	ThisK = 1;
      else
	ThisK = LastK;
      while (ThisK <= LastK) {
	ThisPos =  ThisK * ((Pos - OldPos) / LastK) + OldPos;
	if (StartFlag == TRUE) {
	  ThisComplexity = LastComplexity+log((double)ThisK + 1)/log(2.0);
	}
	else
	  {
	    ThisComplexity = 0;
	    StartFlag = TRUE;
	  }
	if (VerboseDisplay)  printf("Progress : %ld bytes (%.2f %%), %ld TAL\tT-complexity : %f Taugs", ThisPos, (double)(ThisPos * 100) / (double)DataLen, AugCount, ThisComplexity);

	else  printf("%ld\t%.2f\t%ld\t%f", ThisPos, (double)(ThisPos * 100) / (double)DataLen, AugCount, ThisComplexity);

	if (CalculateTInfo) {
	  ThisInfo = invlogint(ThisComplexity);
	  if (VerboseDisplay)
	    printf(", T-information: %f nats, T-entropy: %f nats/symbol", ThisInfo, (ThisInfo - LastInfo) / (ThisPos - LastInfoPos));
	  else
	    printf("\t%f\t%f", ThisInfo, (ThisInfo - LastInfo) / (ThisPos - LastInfoPos));
	  LastInfoPos = ThisPos;
	  LastInfo = ThisInfo;
	}
	printf("\n");
	ThisK++;
      }
    };
    break;

  case PROGRESSPERCENT :
      while (NextProgress <= Percentage) {
        if (VerboseDisplay) {
          printf("Progress : %ld bytes (%.2f %%), %ld TAL\tT-complexity : %f Taugs", Pos, (double)NextProgress, AugCount, Complexity);
        }
        else
        {
          printf("%ld\t%.2f\t%ld\t%f", Pos, (double)NextProgress, AugCount, Complexity);
        }
        if (CalculateTInfo) {
            ThisInfo = invlogint(Complexity);
            if (VerboseDisplay)
              printf(", T-information: %f nats, T-entropy: %f nats/symbol", ThisInfo, (ThisInfo - LastInfo) / (Pos - LastInfoPos));
            else
              printf("\t%f\t%f", ThisInfo, (ThisInfo - LastInfo) / (Pos - LastInfoPos));
            LastInfoPos = Pos;
            LastInfo = ThisInfo;
        }
        printf("\n");
        NextProgress += Progress;
      }
      break;

    case PROGRESSLENGTH :
      while (NextProgress <= Pos) {
        if (VerboseDisplay) {
          printf("Progress : %ld bytes (%.2f %%), %ld TAL\tT-complexity : %f Taugs", NextProgress, Percentage, AugCount, Complexity);
        }
        else
        {
          printf("%ld\t%.2f\t%ld\t%f", NextProgress, Percentage, AugCount, Complexity);
        }
        if (CalculateTInfo) {
            ThisInfo = invlogint(Complexity);
            if (VerboseDisplay)
              printf(", T-information: %f nats, T-entropy: %f nats/symbol", ThisInfo, (ThisInfo - LastInfo) / (NextProgress - LastInfoPos));
            else
              printf("\t%f\t%f", ThisInfo, (ThisInfo - LastInfo) / (NextProgress - LastInfoPos));
            LastInfoPos = NextProgress;
            LastInfo = ThisInfo;
        }
        printf("\n");
        NextProgress += Progress;
      }
      break;

    case PROGRESSPOINT :
      while (NextProgress <= Pos)  {
//		if (VerboseDisplay) {
//		  printf("Progress : %d bytes \tT-complexity : %f Taugs", Pos);
//		}
//		else
//		{
//		  printf("%d\t%f", Pos);
//		}
        if (NextProgress < DataLen) {

			SaveComplexity=Complexity;
            SaveInfo = invlogint(Complexity);
            SaveInfoPos = Pos;
        }
        else {
        	if (VerboseDisplay)
              printf("T-complexity 1:  %f, T-complexity 2: %f, T-information 1:  %f, T-information 2: %f, T-entropy 1: %f, T-entropy 2: %f\n",SaveComplexity, Complexity, SaveInfo, invlogint(Complexity), SaveInfo/ SaveInfoPos, (invlogint(Complexity) - SaveInfo) / (Pos - SaveInfoPos));
            else
              printf("%f\t%f\t%f\t%f\t%f\t%f\n",SaveComplexity, Complexity, SaveInfo, invlogint(Complexity), SaveInfo/ SaveInfoPos, (invlogint(Complexity) - SaveInfo) / (Pos - SaveInfoPos));
        }
            LastInfoPos = Pos;
            LastInfo = ThisInfo;
//        }
        if (NextProgress < DataLen)	{NextProgress = DataLen;}
        else NextProgress +=1;
      }
      break;
  }

}

void Finalise() {

  // Deallocate memory
  if (FromFile)
    free(Data);
  free(Next);
  free(Last);

}

int main(int argc, char *argv[]) {

  int Loop;
  long	i;

  // Read arguments from command line
  for (Loop = 1; Loop < argc; Loop++) {
    if ((strcmp(argv[Loop], "?")== 0)|| (strcmp(argv[Loop], "-?")== 0) || (strcmp(argv[Loop], "-h")== 0)) {
     printf("\t Deterministic Information Theory computations V1.4\n");
    printf("\t Copyright (C) 1998 M. R. Titchener, University of Auckland\n");
    printf("\t\tExecutable Program and its Source licenced under the \n");
    printf("\t\tGNU LESSER GENERAL PUBLIC LICENSE,\n");
    printf("\t\t\tas set out in the associate file COPYING.\n");
    printf("\t \n");
    printf("\t Algorithm by\t\tM. R. Titchener\t\t1997\n");
    printf("\t Implemented by\t\tS. Wackrow\t\tMar. 1998\n");
    printf("\t Amended\t\tU. Guenther\t\tAug. 1999\t\n");
    printf("\t\t\t\t\t\t(-v, -i, -h, and -ti flags added) \n");
    printf("\t Amended\t\tM. R. Titchener\tDec. 2000\t\n");
    printf("\t\t\t\t\t\t(-fr & summary output)\n\n");
    printf("\t Amended\t\tM. R. Titchener\tNov. 2004\t\n");
    printf("\t\t\t\t\t\t(-b & correction to InvLogInt)\n\n");
    printf("\t -f\tFileName\tGets data from 'fileName'\n");
    printf("\t -fr\tFileName\tGets data from 'fileName' in reverse order\n");
    printf("\t -R\t\t\tAssumes reverse order of input string\n");
    printf("\t -r\t\t\tRemoves carriage-retn if last char of input file\n");
    printf("\t -b\t\t\tGives T-information and T-entropy in (bits); default is (nats)\n");
    printf("\t -pa\tnumber\tShows progress every 'number' of T-augmentations\n");
    printf("\t -pl\tnumber\tShows progress approx every 'number' of bytes\n");
    printf("\t -pp\tnumber\tShows progress approx every 'number' percent\n");
    printf("\t -d\tDisplays the T-augmentation process\n");
    printf("\t -o\tTakes the shortest available prefix and maximum runs\n");
    printf("\t -m\tTakes the next prefix but with maximum runs\n");
    printf("\t -v\tRuns the display in verbose mode, i.e., labels the output\n");
    printf("\t\t\tfor -pa, -pl, and -pp\n");
    printf("\t -i\tIn the case of -pa, the T-complexity is calculated after\n");
    printf("\t\t\teach T-prefix appended, even if the T-expansion\n");
    printf("\t\t parameter is greater than 1.\n");
      return 0;
    }
    else if ((strcmp(argv[Loop], "-f") == 0) || (strcmp(argv[Loop], "-fr")== 0 )){
      FromFile = TRUE;
      if  (strcmp(argv[Loop], "-fr")== 0) ReverseData = TRUE;
      else ReverseData = FALSE;

      if (argc > (Loop+1)) {
        FileName = argv[Loop + 1];
        Loop++;
      }
      else
      {
        printf("No filenname specified!\n");
        return -1;
      }
    }
    else if (strcmp(argv[Loop], "-r") == 0)  {
      RemoveLast = TRUE;
    }
    else if (strcmp(argv[Loop], "-R") == 0)  {
      ReverseData = TRUE;
    }
    else if (strcmp(argv[Loop], "-o") == 0)  {
      whichprocess = ORDERLY;
    }
    else if (strcmp(argv[Loop], "-m") == 0)  {
      whichprocess = RUNIT;
    }
    else if (strcmp(argv[Loop], "-pa") == 0) {
      ProgressType = PROGRESSAUG;
      sscanf(argv[Loop + 1], "%ld", &Progress);
      Loop++;
    }
    else if (strcmp(argv[Loop], "-pp") == 0) {
      ProgressType = PROGRESSPERCENT;
      sscanf(argv[Loop + 1], "%ld", &Progress);
      Loop++;
    }
    else if (strcmp(argv[Loop], "-pl") == 0) {
      ProgressType = PROGRESSLENGTH;
      sscanf(argv[Loop + 1], "%ld", &Progress);
      Loop++;
    }
    else if (strcmp(argv[Loop], "-d") == 0) {
      DisplayAug = TRUE;
    }
    else if (strcmp(argv[Loop], "-l") == 0) {
      DisplayList = TRUE;
    }
    else if (strcmp(argv[Loop], "-fd") == 0) {
      FullDebug = TRUE;
    }
    else if (strcmp(argv[Loop], "-a") == 0) {
      CalcAlphabet = TRUE;
    }
    else if (strcmp(argv[Loop], "-v") == 0) {
      VerboseDisplay = TRUE;
    }
    else if (strcmp(argv[Loop], "-i") == 0) {
      InterpolateAug = TRUE;
    }
    else if (strcmp(argv[Loop], "-ti") == 0) {
      CalculateTInfo = TRUE;
    }
    else if (strcmp(argv[Loop], "-b") == 0) {
      Binary = TRUE;
    }
    else if (strcmp(argv[Loop], "-k") == 0) {
      Kolmogorov = TRUE;
    }
    else if (strcmp(argv[Loop], "-n") == 0) {
      ProgressType = PROGRESSPOINT;
      sscanf(argv[Loop + 1], "%ld", &NextProgress);
      Loop++;
    }
    else {
      if (Data == NULL)
        Data = argv[Loop];
      else {
        printf("Error in parameter list\n");
        return -1;
      }
    }
  }

  // Initialise
  if ((FromFile == FALSE) && (Data == NULL)) {
    printf("Error : No data specified\n");
    return -1;
  }

  if (FullDebug)
    printf("Full Debug Mode On\n");




  if (FromFile == TRUE) {
    LoadFile();
    if (VerboseDisplay) {
      printf("File : %s\tSize : %ld", FileName, DataLen);
     if (RemovedLast == TRUE)
       printf(" (Removed new line from end of file)\n");
     else
       printf("\n");
    }
  }
  else {
    DataLen = (long) strlen(Data);
    if (VerboseDisplay) {
      printf("File : stdin\tSize : %ld\n", DataLen);
    }
  }
  if (ReverseData == TRUE) {
	   for (i=0; i < DataLen/2; i++) { tempData = Data[i]; Data[i]  = Data[DataLen-1 - i]; Data[DataLen-1  - i] = tempData;}
  }

  if (CalcAlphabet == TRUE)
    CalculateAlphabet();

  if (Kolmogorov == FALSE) {
  if (FullDebug)
    printf("Ready to Initialise Linked List\n");

  Initialise();

  if (FullDebug)
    printf("Intialisation complete\n");

  switch (ProgressType) {
    case PROGRESSAUG :
      if (VerboseDisplay)
        printf("Summary information every %ld T-augmentations\n", Progress);
      break;
    case PROGRESSPERCENT :
      printf("Summary information every %ld percent\n", Progress);
      break;
    case PROGRESSLENGTH :
      printf("Summary information every %ld bytes\n", Progress);
      break;
  }

  if ((VerboseDisplay) && (DisplayAug || DisplayList || (ProgressType != PROGRESSNONE)))
    printf("\n");

  // Process
  if (whichprocess == NORMAL) {
	  while (Head != Tail) {
	    SelectPrefix();
		if (DisplayAug == TRUE)
		  DisplayAll();
		if (DisplayList == TRUE)
		  DisplayLinks();
		if (ProgressType != PROGRESSNONE)
		  DisplayProgress();
		Augment();
	  }
  }
  else if (whichprocess == RUNIT) {
	  while (Head != Tail) {
		SelectMaxRunPrefix();
		if (DisplayAug == TRUE)
		  DisplayAll();
		if (DisplayList == TRUE)
		  DisplayLinks();
		if (ProgressType != PROGRESSNONE)
		  DisplayProgress();
		Augment();
	  }

  }
  else if (whichprocess == ORDERLY) {
	  while (Head != Tail) {
		SelectShortestMaxRunPrefix();
		if (DisplayAug == TRUE)
		  DisplayAll();
		if (DisplayList == TRUE)
		  DisplayLinks();
		if (ProgressType != PROGRESSNONE)
		  DisplayProgress();
		AugmentShortestMaxRunPrefix();
	  }

  }

  // Finalise
  if (DisplayAug == TRUE)
    DisplayAll();
  if (DisplayList == TRUE)
    DisplayLinks();
  if (ProgressType != PROGRESSNONE)
    DisplayProgress();
  if (VerboseDisplay) {
    printf("\n");
    printf("T-complexity = %.2f", Complexity);
    if (CalculateTInfo) {
     printf(" T-information = %.2f",invlogint(Complexity));
     printf(" Average T-entropy = %.5f",invlogint(Complexity)/DataLen);
    }
    printf("\n");
  }
  else if (ProgressType == PROGRESSNONE) printf("%.2f\t%.2f\t%.5f\n",Complexity,invlogint(Complexity), invlogint(Complexity)/DataLen);



 }
 else { //Kolmogorov
 	Initialise();
	Complexity=0.0;


  // Process
	while (Head != Tail) {
	    SelectPrefix();
		Augment();
	}


    MinComplexity = Complexity;

    Initialise();
    Complexity=0.0;
    for (i=0; i < DataLen/2; i++) { tempData = Data[i]; Data[i]  = Data[DataLen-1 - i]; Data[DataLen-1  - i] = tempData;}


    // Process
	 while (Head != Tail) {
	    SelectPrefix();
		Augment();
	 }

    if (Complexity > MinComplexity) Complexity = MinComplexity ;
    printf("%.2f\t%.2f\t%.5f\n",Complexity,invlogint(Complexity), invlogint(Complexity)/DataLen);

 }

  Finalise();
  return 0;
}
