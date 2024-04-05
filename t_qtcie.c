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

#define MAXIT 100
#define EULER 0.5772156649
#define FPMIN 5.0e-308
#define EPS 5.0e-16

#define FALSE 0
#define TRUE 1

  // type definitions
  typedef struct TCodeWord {
           struct TCodeWord *NextCodeWordOfSameLength;
           struct TCodeWord *NextCodeWordInString;
           struct TCodeWord *PreviousCodeWordInString;
           struct TCodeWord *PreviousCodeWordOfSameLength;
	   long Position;
	   long Length;
       } STRU_TCodeWord;

  typedef struct LinkedListArrayEntry {
           long Index;
	   struct LinkedListArrayEntry *NextEntry;
	   struct TCodeWord *Codeword;
	   struct TCodeWord *LastInsert;
       } STRU_LinkedListArrayEntry;

int Compare(struct TCodeWord *FirstPos, struct TCodeWord *SecondPos);
  // pointer to array for the first codeword of length L and the last codeword of length L inserted
  struct LinkedListArrayEntry *CodeWordsOfLengthL;
  // T-expansion index counter
  long TExpansionIndex;
  // Pointer to previous codeword of same length
  struct TCodeWord *PreviousCodeWordOfSameLength = NULL;
  // Pointer to beginning end end of string linked list
  struct TCodeWord *DataStringStart;
  struct TCodeWord *DataStringEnd;
  struct TCodeWord *TailCodeWord;

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
  long outputlevel = -1;
  int OutputShannon = FALSE;

  long AlphabetCount = 2;
  long plen;


  // Data Array
  long DataLen;
  char *Data;

  char tempData;

  // pointers to codeword under investigation
  struct TCodeWord *Current, *Last;
  // tail position
  long TailPosition;


  // Augmentation Data
  long AugCount = 0;
  struct TCodeWord *Prefix;
  long K = 0;
  long LastK = 1;

  // Complexity
  double Complexity = 0.0;
  double LastComplexity = 0.0;
  int StartFlag = FALSE;

  // T-information
  double LastInfo = 0;
  long LastInfoPos = 0;
  long OldPos = 0;

  // H1
  double H1 = 1.0;
  double LastH1 = 1.0;



  struct LinkedListArrayEntry *getArrayEntryForLength(long length) {
      struct LinkedListArrayEntry *Entry1;
      struct LinkedListArrayEntry *Entry2;
      Entry1 = CodeWordsOfLengthL;
      while (Entry1->Index < length) {
        Entry2 = Entry1->NextEntry;
	if (Entry2 != NULL) {
	    if (Entry2->Index < length) {
	        Entry1 = Entry2;
	    }
	    else
	    {
	        if (Entry2->Index == length) {
		  return Entry2;
		}
	        else {
		  return Entry1;
                }
	    }
	}
	else
	{
	    return Entry1;
	}
      }
      return Entry1;
      // need to figure out from Entry1->Index as to whether there is an entry
  }

  struct LinkedListArrayEntry *setArrayStartEntryForLength(long length, struct TCodeWord *Codeword) {
    struct LinkedListArrayEntry *Entry1, *Entry2;
    Entry1 = getArrayEntryForLength(length);
    // does entry already exist?
    if (Entry1->Index != length) {
       // no, do insert
       Entry2 = (struct LinkedListArrayEntry*) malloc(sizeof(struct LinkedListArrayEntry));
       Entry2->NextEntry = Entry1->NextEntry;
       Entry2->Index = length;
       Entry2->LastInsert = NULL;
       Entry1->NextEntry = Entry2;
       Entry1 = Entry2;
    }
    Entry1->Codeword = Codeword;
    return Entry1;
  }

  struct LinkedListArrayEntry *setArrayInsertEntryForLength(long length, struct TCodeWord *Codeword) {
    struct LinkedListArrayEntry *Entry1, *Entry2;
    Entry1 = getArrayEntryForLength(length);
    // does entry already exist?
    if (Entry1->Index != length) {
       // no, do insert
       Entry2 = (struct LinkedListArrayEntry*) malloc(sizeof(struct LinkedListArrayEntry));
       Entry2->NextEntry = Entry1->NextEntry;
       Entry2->Index = length;
       Entry1->NextEntry = Entry2;
       Entry1 = Entry2;
       Entry1->Codeword = Codeword;
    }
    Entry1->LastInsert = Codeword;
    return Entry1;
  }

void dumpStringPositionList() {
  struct TCodeWord *dummy;
  printf("String linked list positions: ");
  dummy = DataStringStart;
  while (dummy != NULL) {
     printf("%ld ",dummy->Position);
     dummy = dummy->NextCodeWordInString;
  }
  printf("\n");
}


void dumpLengthPositionList(long cwl) {
  struct LinkedListArrayEntry *list;
  struct TCodeWord *dummy;
  list = getArrayEntryForLength(cwl);
  printf("Length linked list positions for length %ld: ",list->Index);
  dummy = list->Codeword;
  while (dummy != NULL) {
     printf("%ld* ",dummy->Position);
     dummy = dummy->NextCodeWordOfSameLength;
  }
  printf("\n");
}


  void resetInsertPointers() {
    struct LinkedListArrayEntry *list;
    list = CodeWordsOfLengthL;
    while (list != NULL) {
      list->LastInsert = NULL;
      list = list->NextEntry;
    }
    // printf("Insert pointers reset\n");
  }

  struct TCodeWord *removeFromLengthList(struct TCodeWord *codeword) {
    struct LinkedListArrayEntry *entry;
    // if (FullDebug) printf("\nentering removeFromLengthList()\n");

    // first make sure the FORWARD pointers to this codeword are changed to the next one

    entry = getArrayEntryForLength(codeword->Length);
    if (codeword->PreviousCodeWordOfSameLength != NULL) {
      // if the present codeword is not the first codeword in that list...
      codeword->PreviousCodeWordOfSameLength->NextCodeWordOfSameLength = codeword->NextCodeWordOfSameLength;
    }
    else
    {
      // if it is the first codeword in list, set first entry to next codeword of the same length as the former
      // length of the codeword we're trying to remove
      entry->Codeword = codeword->NextCodeWordOfSameLength;
    }

    // in any case, reset stray insert pointers!
    if ((entry->LastInsert != NULL) && (entry->LastInsert->Position == codeword->Position)) {
        entry->LastInsert = NULL; // This pointer can't stay here!
    }

    // now worry about backward pointers - there is only one to worry about, really

    if (codeword->NextCodeWordOfSameLength != NULL) {
      codeword->NextCodeWordOfSameLength->PreviousCodeWordOfSameLength = codeword->PreviousCodeWordOfSameLength;
    }
    // if (FullDebug) printf("\nexiting removeFromLengthList()\n");
    return codeword->NextCodeWordOfSameLength;
  }

  void removeFromStringList(struct TCodeWord *codeword) {
    struct LinkedListArrayEntry *entry;
    // no need to test here for nullness as left codewords always survive in mergers
    codeword->PreviousCodeWordInString->NextCodeWordInString = codeword->NextCodeWordInString;
    if (codeword->NextCodeWordInString != NULL) {
      codeword->NextCodeWordInString->PreviousCodeWordInString = codeword->PreviousCodeWordInString;
    }
  }

  void mergeCodeWords(struct TCodeWord *firstCodeWordInMergedString) {
    struct LinkedListArrayEntry *Entry, *NewEntry;
    struct TCodeWord *NextCW, *InsertPointer, *NextSameLength;
    // if (FullDebug) printf("\nentering mergeCodeWords()\n");
    TExpansionIndex = 1;
    // remove first codeword from linked list for its length
    NextSameLength = removeFromLengthList(firstCodeWordInMergedString);
    // while next codeword is also the T-prefix and TExpansionIndex < K,
    // merge with next codeword. Do not insert in linked list for larger length yet!
    // Remove from linked list for codewords of length of the T-prefix
    NextCW = firstCodeWordInMergedString->NextCodeWordInString;
    while ((TExpansionIndex < K) && (Compare(Prefix,NextCW))) {
      TExpansionIndex++;
      // do the merge:
      // add length of newly found T-prefix copy to merged codeword
      firstCodeWordInMergedString->Length += NextCW->Length;
      // take newly found T-prefix copy out of linked list for codewords of T-prefix length
      NextSameLength = removeFromLengthList(NextCW);
      // take newly found T-prefix copy out of linked list for codewords in the string T-prefix length
      removeFromStringList(NextCW);
      free(NextCW);
      NextCW = firstCodeWordInMergedString->NextCodeWordInString;
    }
    // merge with following codeword & remove that codeword from linked list for its length
    firstCodeWordInMergedString->Length += NextCW->Length;
    if (NextCW->Length == Prefix->Length) { // if suffix has T-prefix length, we need to advance Last (eventually)
        NextSameLength = removeFromLengthList(NextCW);
    }
    else removeFromLengthList(NextCW); // if not, we just remove the codeword from its own length list
    if (Last != NULL) {
      Last->NextCodeWordOfSameLength = NextSameLength;
    }
    else
    {
      Entry = setArrayStartEntryForLength(Prefix->Length,NextSameLength);
    }
    // remove merged codeword from the string linked list
    removeFromStringList(NextCW);
    free(NextCW);

    // insert resulting merged codeword into appropriate linked list for its length

    Entry = getArrayEntryForLength(firstCodeWordInMergedString->Length);

    if (Entry->Index != firstCodeWordInMergedString->Length) {
       //if (FullDebug) printf("\n No array entry for this length - creating...");
       // no inserts have been made in this list, ever. Add array element
       NewEntry = setArrayStartEntryForLength(firstCodeWordInMergedString->Length, firstCodeWordInMergedString);
       NewEntry->LastInsert = firstCodeWordInMergedString;
       firstCodeWordInMergedString->NextCodeWordOfSameLength = NULL;
       firstCodeWordInMergedString->PreviousCodeWordOfSameLength = NULL;
    }
    else
    {
       // linked list for this length was used before. Is it empty?
       // if (FullDebug) printf("\n Array entry exists...");
       if (Entry->Codeword == NULL) { // yes, empty, add codeword at start
         // if (FullDebug) printf("\n List for array entry is empty...");
         Entry->Codeword = firstCodeWordInMergedString;
         Entry->LastInsert = firstCodeWordInMergedString;
         firstCodeWordInMergedString->NextCodeWordOfSameLength = NULL;
	 firstCodeWordInMergedString->PreviousCodeWordOfSameLength = NULL;
	 return;
       }
       else
       { // not empty. Does the last insert pointer point to the start of the list (reset by removal of an element)?
         if (Entry->LastInsert == NULL) {
	    // OK, if the position of the codeword that we want to insert is before the first element in the list, insert
	    if (firstCodeWordInMergedString->Position < Entry->Codeword->Position) {
              firstCodeWordInMergedString->NextCodeWordOfSameLength = Entry->Codeword;
	      Entry->Codeword->PreviousCodeWordOfSameLength = firstCodeWordInMergedString;
              firstCodeWordInMergedString->PreviousCodeWordOfSameLength = NULL;
              Entry->Codeword = firstCodeWordInMergedString;
              Entry->LastInsert = firstCodeWordInMergedString;
	      return;
	    }
	    else
	    { // if not, move the insert pointer to the first codeword - we will search later for the right spot to insert
	      Entry->LastInsert = Entry->Codeword;
	    }
	 }
	 else // list was not reset
	 { // is last insert after the position of the codeword we want to insert (old insert from a previous pass)?
	   if (Entry->LastInsert->Position > firstCodeWordInMergedString->Position) {
	     // treat as if it was Entry->LastInsert was NULL
  	     // OK, if the position of the codeword that we want to insert is before the first element in the list, insert
  	     if (firstCodeWordInMergedString->Position < Entry->Codeword->Position) {
	       firstCodeWordInMergedString->NextCodeWordOfSameLength = Entry->Codeword;
               Entry->Codeword->PreviousCodeWordOfSameLength = firstCodeWordInMergedString;
	       firstCodeWordInMergedString->PreviousCodeWordOfSameLength = NULL;
               Entry->Codeword = firstCodeWordInMergedString;
               Entry->LastInsert = firstCodeWordInMergedString;
	       return;
	     }
	     else
	     { // if not, move the insert pointer to the first codeword
	       Entry->LastInsert = Entry->Codeword;
	     }
	   }
	 }
       }



       if (Entry->LastInsert != firstCodeWordInMergedString) {
         // printf("\n Insert point needs to be searched for...");

         while (Entry->LastInsert->NextCodeWordOfSameLength != NULL) {
           if (Entry->LastInsert->NextCodeWordOfSameLength->Position > firstCodeWordInMergedString->Position) {
             break;
           }
           else
           {
             Entry->LastInsert = Entry->LastInsert->NextCodeWordOfSameLength;
           }
         }

	 // insert point found - are we at the end of the list now?
	 if (Entry->LastInsert->NextCodeWordOfSameLength == NULL) {
	   firstCodeWordInMergedString->NextCodeWordOfSameLength = NULL;
	 }
	 else
	 {
	   firstCodeWordInMergedString->NextCodeWordOfSameLength = Entry->LastInsert->NextCodeWordOfSameLength;
	   Entry->LastInsert->NextCodeWordOfSameLength->PreviousCodeWordOfSameLength = firstCodeWordInMergedString;
	 }
	 Entry->LastInsert->NextCodeWordOfSameLength = firstCodeWordInMergedString;
	 firstCodeWordInMergedString->PreviousCodeWordOfSameLength = Entry->LastInsert;
	 Entry->LastInsert = firstCodeWordInMergedString;
         // if (FullDebug) printf("\n Inserted!");
       }
    }
    // if (FullDebug) printf("\nexiting mergeCodeWords()\n");
  }



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
        return ub;
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
  struct TCodeWord *codeword;

  if (FullDebug)
    printf("Initializing Linked List Memory\n");

  // initialise linked list
  DataStringStart = (struct TCodeWord *) malloc(sizeof(struct TCodeWord));
  DataStringStart->PreviousCodeWordInString = NULL;
  DataStringStart->PreviousCodeWordOfSameLength = NULL;
  DataStringStart->Position = 0;
  DataStringStart->Length = 1;
  Last = DataStringStart;

  for (Loop = 1; Loop < DataLen; Loop++) {
    codeword = (struct TCodeWord *) malloc(sizeof(struct TCodeWord));
    codeword->NextCodeWordInString = NULL;
    codeword->NextCodeWordOfSameLength = NULL;
    codeword->PreviousCodeWordInString = Last;
    codeword->PreviousCodeWordOfSameLength = Last;
    codeword->Length = 1;
    codeword->Position = Loop;
    Last->NextCodeWordInString = codeword;
    Last->NextCodeWordOfSameLength = codeword;
    Last = codeword;
  }
  Last = DataStringStart;

  DataStringEnd = codeword;
  CodeWordsOfLengthL = (struct LinkedListArrayEntry*) malloc(sizeof(struct LinkedListArrayEntry));
  CodeWordsOfLengthL->NextEntry = NULL;
  CodeWordsOfLengthL->Index = 1;
  CodeWordsOfLengthL->Codeword = DataStringStart;
  CodeWordsOfLengthL->LastInsert = NULL;
  TailPosition = DataStringEnd->Position;
  TailCodeWord = DataStringEnd;

  if (FullDebug) {
     dumpStringPositionList();
     dumpLengthPositionList(1);
    printf("Linked List Initialised\n");
  }

}

void CalculateAlphabet() {

  int Alphabet[512], AlphaIndex;
  long Loop;

  AlphabetCount = 0;

  if (FullDebug)
    printf("Initialising ALphabet\n");

  for (Loop = 0; Loop < 512; Loop++)
    Alphabet[Loop] = FALSE;

  if (FullDebug)
    printf("Analysing Data\n");

  for (Loop = 0; Loop < DataLen; Loop++)
    Alphabet[(int)Data[Loop]+256] = TRUE;

  if (FullDebug)
    printf("Counting Alphabet\n");

  for (Loop = 0; Loop < 512; Loop++)
    if (Alphabet[Loop] == TRUE) {
      AlphabetCount++;
    }

  if (AlphabetCount < 2) {
    AlphabetCount = 2;
  }

  H1 = -log(1/(double)AlphabetCount)/log(2.0);

  if (VerboseDisplay)
    printf("Alphabet : %ld characters\n", AlphabetCount);

  if (FullDebug)
    printf("Finalising Alphabet\n");

}

/* void DisplayLinks() {

  long Loop;

  printf("Next [");
  for (Loop = 0; Loop < DataLen; Loop++)
    if (Loop < DataLen - 1)
      printf("%d,", Next[Loop]);
    else
      printf("%d] Head = %d\n", Next[Loop], Head);

  printf("Last [");
  for (Loop = 0; Loop < DataLen; Loop++)
    if (Loop < DataLen - 1)
      printf("%d,", Last[Loop]);
    else
      printf("%d] Tail = %d\n", Last[Loop], Tail);

} */

int Compare(struct TCodeWord *FirstPos, struct TCodeWord *SecondPos) {

  long Len, Loop;

  if (FullDebug) printf("\nentering Compare()\n");
/*  // Test same length
  if (FirstPos->Length != SecondPos->Length) {
    // if (FullDebug) printf("\neexiting Compare()\n");
    return FALSE;
  }
  else { */
    // Get bound to test
    Len = FirstPos->Length - 1;
    // Test last character
    if (Data[FirstPos->Position + Len] != Data[SecondPos->Position + Len]){
      // if (FullDebug) printf("\neexiting Compare()\n");
      return FALSE;
    }
    // Test remaining characters
    for (Loop = 0; Loop < Len; Loop++)
      if (Data[FirstPos->Position + Loop] != Data[SecondPos->Position + Loop]) {
        // if (FullDebug) printf("\neexiting Compare()\n");
        return FALSE;
      }
    // if (FullDebug) printf("\neexiting Compare()\n");
    return TRUE;
 // }
}

void Display(struct TCodeWord *codeword) {

  long Pos, Len, Loop;

  Pos = codeword->Position;
  Len = codeword->Length;
  for (Loop = 0; Loop < Len; Loop++)
    if (Loop != Len - 1)
      printf("%c ", Data[Pos + Loop]);
    else
      printf("%c", Data[Pos + Loop]);

}

void DisplayPrefix() {

  printf("K = %ld\tPrefix = ", K);
  Display(Prefix);
  printf(" from %ld length %ld ", Prefix->Position, Prefix->Length);
  printf("\n");

}

void DisplayAll() {

  long Pos;
  printf("K = %ld\tPrefix = ", K);
  Display(Prefix);
  printf("\n");

}


void SelectPrefix() {

  struct TCodeWord *TPrefixCandidate;

  if (FullDebug) {
    printf("\n\n**********************************************************\n\n");
    printf("%ld\n",TailPosition);
  }

  Prefix = TailCodeWord->PreviousCodeWordInString;
  K = 1;
  Prefix->NextCodeWordOfSameLength = NULL;
  removeFromLengthList(Prefix);
  TailPosition = Prefix->Position;
  TailCodeWord = Prefix;
  Prefix->NextCodeWordOfSameLength = NULL;
  TPrefixCandidate = Prefix->PreviousCodeWordInString;
  while ((TPrefixCandidate != NULL) && Compare(Prefix, TPrefixCandidate)) {
      K++;
      TailPosition = TPrefixCandidate->Position;
      TailCodeWord = TPrefixCandidate;
      TPrefixCandidate->NextCodeWordOfSameLength = NULL;
      removeFromLengthList(TPrefixCandidate);
      TPrefixCandidate = TPrefixCandidate->PreviousCodeWordInString;
  }

}

void Augment() {

  long Pos, FirstPos, LastPos;
  long kloop;
  int Matching = FALSE;
  long Matched = 0;
  double pprobability;
  double lastsum;
  struct LinkedListArrayEntry *entry;
  struct TCodeWord *NextDummy, *PreviousDummy;

  // printf("\nentering Augment()\n");
  // Calculate complexity
  AugCount++;
  LastComplexity = Complexity;
  LastK = K;
  Complexity += log((double)K + 1)/log(2.0);

  // Calculate H1 entropy
  plen = Prefix->Length;
  pprobability = 1/pow((double)AlphabetCount,(double) plen);
  LastH1 = H1;
  lastsum = LastH1*pprobability;
  H1 = H1 + lastsum;
  for (kloop = 2; kloop <= (double)K; kloop++) {
    lastsum = lastsum*pprobability;
    H1 = H1 + lastsum;
  }

  entry = getArrayEntryForLength(Prefix->Length);

  if ((TailPosition > 0) && (Prefix->Length == entry->Index) && (entry->Codeword != NULL)) { // any codewords of prefix length in the string?
     // yes, so follow the linked list for the prefix length and merge any T-prefixes we find
     // resetInsertPointers();
     // if (FullDebug) printf("Found at positions ");
     Current = entry->Codeword;
     Last = NULL;
     while ((Current != NULL) && (Current->Position < TailPosition)) { // QUESTION: can we drop second condition?

       if (Compare(Current,Prefix)) { // T-prefix found, start merger
          // printf("Beginning of T-prefix run (level %d) found at current pos:%d (%d)\n",AugCount, Current->Position, TailPosition);
          // if (FullDebug) printf("+");
          mergeCodeWords(Current);
	  if (Last != NULL) {
            Current = Last->NextCodeWordOfSameLength;
	  }
	  else
	  { // start at beginning of list
	    Current = entry->Codeword;
	  }
       }
       else
       {
          Last = Current;
	  Current = Current->NextCodeWordOfSameLength;
       }
       // if (FullDebug) printf(" ");
     }
     // if (FullDebug) printf("\n");
  }
//  if (FullDebug) printf("\nexiting Augment()\n");
//  if (FullDebug) dumpStringPositionList();
}

void DisplayProgress() {

  long Pos, ThisK, ThisPos;
  double Percentage;
  double ThisComplexity, ThisInfo;

  Pos = TailPosition;
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
	if (VerboseDisplay)  printf("Progress : %ld bytes (%.2f %%), %ld TAL\tT-complexity : %f Taugs", (DataLen - TailPosition), (double)(ThisPos * 100) / (double) (DataLen - TailPosition), AugCount, ThisComplexity);

	else  printf("%ld\t%.2f\t%ld\t%f", (DataLen - TailPosition), (double)(ThisPos * 100) / (double)(DataLen - TailPosition), AugCount, ThisComplexity);

	if (CalculateTInfo) {
	  ThisInfo = invlogint(ThisComplexity);
	  if (VerboseDisplay)
	    printf(", T-information: %f nats, T-entropy: %f nats/symbol", ThisInfo, (ThisInfo - LastInfo) / (ThisPos - LastInfoPos));
	  else
	    printf("\t%f\t%f", ThisInfo, (ThisInfo - LastInfo) / (ThisPos - LastInfoPos));
	  LastInfoPos = ThisPos;
	  LastInfo = ThisInfo;
	}
	if (OutputShannon) {

printf("\t%ld:%ld:%ld:%f:%f:%f:%f:%f",AlphabetCount,plen,K,H1,(H1-LastH1)/plen,H1/plen,H1*log(2)/plen,(ThisInfo
- LastInfo) / ((double)(ThisPos - LastInfoPos)*H1*log(2)/plen));
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

  }

}

void Finalise() {

  // Deallocate memory
  if (FromFile)
    free(Data);
  // need to free linked list also!
}

int main(int argc, char *argv[]) {

  int Loop;
  long	i;

  // Read arguments from command line
  for (Loop = 1; Loop < argc; Loop++) {
    if ((strcmp(argv[Loop], "?")== 0)|| (strcmp(argv[Loop], "-?")== 0) || (strcmp(argv[Loop], "-h")== 0)) {
     printf("\t Deterministic Information Theory computations V1.3\n");
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
    printf("\t -f\tFileName\tGets data from 'fileName'\n");
    printf("\t -fr\tFileName\tGets data from 'fileName' in reverse order\n");
    printf("\t -r\t\t\tRemoves carriage-retn if last char of input file\n");
    printf("\t -pa\tnumber\tShows progress every 'number' of T-augmentations\n");
    printf("\t -pl\tnumber\tShows progress approx every 'number' of bytes\n");
    printf("\t -pp\tnumber\tShows progress approx every 'number' percent\n");
    printf("\t -d\tDisplays the T-augmentation process\n");
    printf("\t -v\tRuns the display in verbose mode, i.e., labels the output\n");
    printf("\t\t\tfor -pa, -pl, and -pp\n");
    printf("\t -i\tIn the case of -pa, the T-complexity is calculated after\n");
    printf("\t\t\teach T-prefix appended, even if the T-expansion\n");
    printf("\t\t parameter is greater than 1.\n");
    printf("\t -h\tdisplays this help message\n");
    printf("\t -h1\tin the case of -pa, outputs H1 Shannon entropy\n");
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
    else if (strcmp(argv[Loop], "-q") == 0) {
      	sscanf(argv[Loop + 1], "%ld", &outputlevel);
      	Loop++;
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
    else if (strcmp(argv[Loop], "-h1") == 0) {
      OutputShannon = TRUE;
    }
    else if (strcmp(argv[Loop], "-ti") == 0) {
      CalculateTInfo = TRUE;
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
    if (ReverseData == TRUE) {
         for (i=0; i < DataLen/2; i++) { tempData = Data[i]; Data[i]  = Data[DataLen-1 - i]; Data[DataLen-1  - i] = tempData;}
    }
  }
  else {
    DataLen = (long) strlen(Data);
    if (VerboseDisplay) {
      printf("File : stdin\tSize : %ld\n", DataLen);
    }
  }
  if (CalcAlphabet == TRUE)
    CalculateAlphabet();

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
  while (TailPosition > 0) {
    // printf("\n\nTail Position: %d:\n",TailPosition);
    SelectPrefix();
    if (outputlevel+1 > 0) {
    	outputlevel--;
    }
    if ( outputlevel == 0 ) {
    	DisplayAll();
    }
    if (DisplayAug == TRUE)
      DisplayAll();
     /* if (DisplayList == TRUE)
      DisplayLinks(); */
    Augment();
    if (ProgressType != PROGRESSNONE)
      DisplayProgress();
  }

  // Finalise
  /* if (DisplayAug == TRUE)
     DisplayAll();
  if (DisplayList == TRUE)
    DisplayLinks(); */
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
  else if (ProgressType == PROGRESSNONE) printf("%.2f (taugs)\t%.2f (nats)\t%.5f(nats/symbol)\n",Complexity,invlogint(Complexity), invlogint(Complexity)/DataLen);
  Finalise();
  return 0;
}
