#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <ctype.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <getopt.h>

// investigate the tree pointers.
// awk 'BEGIN{for (i=0; i< 400000; i++) {print n=int(rand()+0.5); print n; print n; for (j=0; j< 100000; j++) {}; fflush()}}' | framesnd 192.168.0.53 9222 | awk '{print i++}'



#include 	"Incs.c"

DepletionSetTypeP codeP = NULL;
DeplnType depln, lastdepln;
int	bitcount = 0;

#define MAXBUFLEN 1024

struct MorseNode
{
	struct MorseNode			*leftPtr, *rightPtr;							// left and right child nodes
	struct MorseNode			*PreviousNodePtr;
	char						string[10];										//  character code
	char						code[10];										//binary node code
	char						morsecode[32];									//
};

// morse letter set
char *letterlist[] = {
"E",
"I",
"S",
"H",
"5",
"4",
"V",
"^S",
"3",
"U",
"F",
"É",
"Ü",
"-D",
"?",
"_",
"2",
"A",
"R",
"L",
"È",
"\"",
"Ä",
"+",
".\n",
"W",
"P",
"pb",
"À",
"@",
"J",
"^J",
"1",
"T",
"N",
"D",
"B",
"6",
"-",
"\n=\n",
"X",
"/",
"K",
"C",
"Ç",
";",
"!",
"Y",
"^H",
"()",
"M",
"G",
"Z",
"7",
",",
"Q",
"^G",
"Ñ",
"O",
"Ö",
"8",
";",
"CH",
"9",
"0"
};


// morse code '0' = dit, '1' = dah
char *codelist[] = {
"0",
"00",
"000",
"0000",
"00000",
"00001",
"0001",
"00010",
"00011",
"001",
"0010",
"00100",
"0011",
"00110",
"001100",
"001101",
"00111",
"01",
"010",
"0100",
"01001",
"010010",
"0101",
"01010",
"010101",
"011",
"0110",
"01100",
"01101",
"011010",
"0111",
"01110",
"01111",
"1",
"10",
"100",
"1000",
"10000",
"100001",
"10001",
"1001",
"10010",
"101",
"1010",
"10100",
"101010",
"101011",
"1011",
"10110",
"101101",
"11",
"110",
"1100",
"11000",
"110011",
"1101",
"11010",
"11011",
"111",
"1110",
"11100",
"111000",
"1111",
"11110",
"11111"
};

//add a node
void add_node( struct MorseNode **NodePtrPtr, char *string, int level, char *code ) {

	struct MorseNode	*NodePtr;
	int	i=0,j=0;

	if (*NodePtrPtr == NULL) {
		NodePtr = malloc( sizeof( struct MorseNode ) );	 //create the root node
		if ( NodePtr == NULL ) fprintf(stderr,"Could not allocate memory!\n" );
		NodePtr->leftPtr = NULL;
		NodePtr->rightPtr = NULL;
//		fprintf(stderr,"pointers %p %p!\n",NodePtr->leftPtr,NodePtr->rightPtr );
		*NodePtrPtr = NodePtr;
	} else NodePtr = *NodePtrPtr;
	
	if (level==strlen(code)) { // store the letter
//		fprintf(stderr,"%s\t%d\t%p\tL:%p\tR:%p\n",string, strlen(code), NodePtr, NodePtr->leftPtr, NodePtr->rightPtr); fflush(stderr);
		strncpy(NodePtr->string,string,strlen(string) );
		strncpy(NodePtr->code,code,strlen(code) );
		// generating the long form of the morse code
		j=0;
		for (i=0; i < strlen(code); i++){
		 if (NodePtr->code[i]=='0') {NodePtr->morsecode[j++]='1'; NodePtr->morsecode[j++]='0';}
		 else if (NodePtr->code[i]=='1') {NodePtr->morsecode[j++]='1'; NodePtr->morsecode[j++]='1'; NodePtr->morsecode[j++]='1'; NodePtr->morsecode[j++]='0';}
		}
		NodePtr->morsecode[j++]='0';NodePtr->morsecode[j++]='0';
		NodePtr->morsecode[j++]=0; NodePtr->morsecode[j++]=0;
//		fprintf(stderr,"%s\t%d\t%s\t%s\t%p\n",string, strlen(code), NodePtr->code, NodePtr->morsecode, NodePtr); fflush(stderr);		
		return;
	}
	if(code[level]=='0'){
		add_node(&NodePtr->leftPtr, string, level+1, code );
	} 
	else if (code[level]=='1') {
		add_node(&NodePtr->rightPtr, string, level+1, code);
	}
}

//output tree in order
void Inorder(struct MorseNode *NodePtr)
{
	if (NodePtr==NULL)  return;
	printf ("%p\tL:%p\tR:%p\n",NodePtr, NodePtr->leftPtr, NodePtr->rightPtr); fflush(stdout);
	if (strlen(NodePtr->string) > 0) printf ("%s\t%s\t%s\n",NodePtr->string, NodePtr->code, NodePtr->morsecode); 
	Inorder(NodePtr->leftPtr);
	Inorder(NodePtr->rightPtr);
}




//output tree in order
void Postorder(struct MorseNode *NodePtr)
{
	if (NodePtr==NULL) return;
	if (strlen(NodePtr->string) > 0) printf ("%s\t%s\t%s\n",NodePtr->string, NodePtr->code, NodePtr->morsecode); 
	printf ("%p\tL:%p\tR:%p\n",NodePtr, NodePtr->leftPtr, NodePtr->rightPtr); fflush(stdout);
	Postorder(NodePtr->rightPtr);
	Postorder(NodePtr->leftPtr);
}

/*void Cleaninorder(struct MorseNode **NodePtrPtr)
{
	if (*NodePtrPtr==(struct MorseNode *)0x3030) {*NodePtrPtr=NULL; return;}
	if (*NodePtrPtr==NULL) return;
	Cleaninorder(*NodePtrPtr->leftPtr);
	Cleaninorder(*NodePtrPtr->rightPtr);
}
*/

struct MorseNode* dit(struct MorseNode *NodePtr) {
		if (NodePtr->leftPtr == NULL)  return NodePtr; 
		else return (NodePtr->leftPtr);
}

struct MorseNode* dah(struct MorseNode *NodePtr) {
		if (NodePtr->rightPtr == NULL)  return NodePtr; 
		else return (NodePtr->rightPtr);
}

void getlet(struct MorseNode *NodePtr) {
		if (strlen(NodePtr->string) !=0) {fprintf (stdout, "%s", NodePtr->string); fflush(stdout);}
}



// get sockaddr, IPv4 or IPv6:
void *get_in_addr(struct sockaddr *sa)
{
	if (sa->sa_family == AF_INET) {
		return &(((struct sockaddr_in*)sa)->sin_addr);
	}

	return &(((struct sockaddr_in6*)sa)->sin6_addr);
}



void usage(void)
{
	fprintf(stderr,
		"CW Tcode decoder for use with morsedemod"
		"Usage: e.g. rtl_sdr -f 27085500 -g 1 -s 2400000  - | ./morsedemod -t .03 -l .00002  -b -h 192.168.1.7 - | ./tcode_decode_udp_mac -h 192.168.1.7"
		"\t -h [ip address] assumes port 9222"
		"\t -e envelope"
		"\tfilename ( '-' dumps samples to stdout)\n\n");
	exit(1);
}

char *optarg;
int optind = 1;
int __getopt_initialized;
static char *nextchar;
int opterr = 1;
int optopt = '?';

int ii = 0;
int	retflg = 0;

int ring[20];
 //set up a dynamic pointer to tree nodes
 	struct MorseNode *pointer = NULL;
	struct MorseNode *treePtr = NULL;   // root node for tree
	


void processring(int deplncode) {

	int j,k;

	ring[ii]= deplncode;
	// lets make sure its a valid depletion code!
	if (((ring[ii]/2 > 0) && (ring[ii]/2<=128) && (ring[ii] != 26) && (ring[ii] != 52) && (ring[ii] != 78)) || (ring[ii] == 25) || (ring[ii] == 51) || (ring[ii] == 77) || (ring[ii] == 103) || (ring[ii] == 129)) {

		j=ii-1+10 % 10;
		k=ii-2+10 % 10;
		if (ring[ii] == 104) {

			if (retflg==0) { getlet(pointer); pointer = treePtr; }  //fprintf(stdout,"00\n"); // end of letter
 			else if (retflg==1) { getlet(pointer); pointer = treePtr;  fprintf(stdout," "); } // fprintf(stderr,"\t%d\n", bitcount); bitcount=0; //  fprintf(stdout,"0000\n"); // end of word
			retflg++; 
			fflush(stdout);
			if ((ring[j] == 25) || (ring[j] == 51) || (ring[j] == 77) || (ring[j] == 103) || (ring[j] == 129)) { retflg = 0; pointer = dah(pointer);}
		} 
		else if ((ring[j] == 104) && (ring[k] == 104)) { // ith not 104 but jth and kth are 104
				if ((ring[ii] == 2) || (ring[ii] == 4) || (ring[ii] == 6) || (ring[ii] == 8)) {if (retflg<=1) pointer=treePtr; retflg = 0; pointer = dit(pointer);}
				else if ((ring[ii] == 28) || (ring[ii] == 30) || (ring[ii] == 32) || (ring[ii] == 34)) {if (retflg<=1) pointer=treePtr; retflg = 0; pointer = dit(pointer);}

				else if ((ring[ii] == 10) ||(ring[ii] == 12) || (ring[ii] == 14) || (ring[ii] == 16) || (ring[ii] == 18) || (ring[ii] == 20) || (ring[ii] == 22) || (ring[ii] == 24)) {if (retflg<=1) pointer=treePtr; retflg = 0; pointer = dah(pointer);} 
				else if ((ring[ii] == 36) || (ring[ii] == 38) || (ring[ii] == 40) || (ring[ii] == 42) || (ring[ii] == 44) || (ring[ii] == 46) || (ring[ii] == 48) || (ring[ii] == 50)) {if (retflg<=1) pointer=treePtr; retflg = 0; pointer = dah(pointer);} 
		}
		else { // all remaining instances cases are single substitutions as follows
		
			if ((ring[ii] == 2) || (ring[ii] == 4) || (ring[ii] == 6) || (ring[ii] == 8) || (ring[ii] == 28) || (ring[ii] == 30) || (ring[ii] == 32) || (ring[ii] == 34)) {retflg = 0; pointer = dit(pointer);} 
			else if ((ring[ii] == 10) ||(ring[ii] == 12) || (ring[ii] == 14) || (ring[ii] == 16) || (ring[ii] == 18) || (ring[ii] == 20) || (ring[ii] == 22) || (ring[ii] == 24)) {retflg = 0; pointer = dah(pointer);}
			else if ((ring[ii] == 36) ||(ring[ii] == 38) || (ring[ii] == 40) || (ring[ii] == 42) || (ring[ii] == 44) || (ring[ii] == 46) || (ring[ii] == 48) || (ring[ii] == 50)) {retflg = 0; pointer = dah(pointer);}
		}
// unconditional substitutions otherwise as follows
		if 	((ring[ii] == 54) || (ring[ii] == 56) || (ring[ii] == 58) || (ring[ii] == 60)) {
			 retflg = 0; pointer = dit(pointer); 
		}
		else if ((ring[ii] == 80) || (ring[ii] == 82) || (ring[ii] == 84) || (ring[ii] == 86) || (ring[ii] == 106) || (ring[ii] == 108) || (ring[ii] == 110) || (ring[ii] == 112)) {
			retflg = 0; pointer = dit(pointer);
		}
		else if ((ring[ii] == 62) || (ring[ii] == 64) || (ring[ii] == 66) || (ring[ii] == 68) || (ring[ii] == 70) || (ring[ii] == 72) || (ring[ii] == 74) || (ring[ii] == 76)) {
			retflg = 0; pointer = dah(pointer);
		}
		else if ((ring[ii] == 88) || (ring[ii] == 90) || (ring[ii] == 92) || (ring[ii] == 94) || (ring[ii] == 96) || (ring[ii] == 98) || (ring[ii] == 100) || (ring[ii] == 102)) {
			retflg = 0; pointer = dah(pointer);
		}
		else if ((ring[ii] == 114) || (ring[ii] == 116) || (ring[ii] == 118) || (ring[ii] == 120) || (ring[ii] == 122) || (ring[ii] == 124) || (ring[ii] == 126) || (ring[ii] == 128)) {
			retflg = 0; pointer = dah(pointer);
		}
			
		ii=(ii+1) % 20; // increment ring counter
	}
}



void decode(char *string, int level ){
char *stringptr, prefix[MAXPRFXLEN];
long lengthstring;
int heading, i, length = 1, expansion = 1;

	NewDepln(&depln);
	NewDepln(&lastdepln);
	stringptr = string;
	lengthstring = strlen(string);
	char message[10];
	
	while (length > 0) {
		length = DecodeAt(level, codeP, stringptr, &depln);
		if (length > 0) {
			stringptr += length;
			if ( CompareDepln( &lastdepln, &depln, codeP->q) == TRUE ) 
				expansion++;
			else {
				if (stringptr > string + length) {
					prefix[Encode(&lastdepln, codeP, prefix)] = 0;
					for (i=0; i<expansion; i++) {
							processring((int)DeplnToLong(&lastdepln, codeP)); 	
//							fprintf(stderr, "%ld\n", DeplnToLong(&lastdepln, codeP));
//							sprintf(message, "%ld", DeplnToLong(&lastdepln, codeP));
//						if ((numbytes = sendto(sockfd, message, strlen(message), 0,p->ai_addr, p->ai_addrlen)) == -1) {
//							perror("framesnd: sendto");
//							exit (1);
//						}
					}
				}
				CopyDepln(&depln, &lastdepln);
				expansion = 1;
			}
		}
	}
	prefix[Encode(&lastdepln, codeP, prefix)] = 0;
	for (i=0; i<expansion; i++){
		processring((int)DeplnToLong(&lastdepln, codeP)); 	
//		fprintf(stderr, "%ld\n", DeplnToLong(&lastdepln, codeP));
//		sprintf(message, "%ld", DeplnToLong(&lastdepln, codeP));
//		if ((numbytes = sendto(sockfd, &message[0], strlen(message), 0,p->ai_addr, p->ai_addrlen)) == -1) {
//			perror("framesnd: sendto");
//			exit (1);
//		}
	
	}

}


/* ========================================================================= */
/* main		to gather the sampled output from guassian filter	                                                    */
/* ========================================================================= */
/*
** All the initialization and action takes place here.
*/

int main ( int argc, char** argv )
{
//	char *file=NULL;
	
    int scanfReturn = 0;
    long counter;
    int	value, sync, lastvalue=0;
    float envelope;
	int opt;
    char string[80];
	int	envelopeFlg=TRUE;

	int sockfd;
	struct addrinfo hints, *servinfo, *p;
	int rv;
	int numbytes;
	struct sockaddr_storage their_addr;
	char buf[MAXBUFLEN];
	socklen_t addr_len;
	char s[INET6_ADDRSTRLEN];


    char* message, hostip[20];
	char t[20];

	int	i,j;




char *nmorse = "2\n0\n1\n2\n12\n1\n1\n4\n1\n0\n"; // defines the T-code set

	while ((opt = getopt(argc, argv, "eh:")) != -1) {
		switch (opt) {
		case 'h': // host ip
			strcpy(&hostip[0], optarg);
			;
			break;
		case 'e': // accepts envelope input
			envelopeFlg=FALSE;
			;
			break;
		default:
			usage();
			break;
		}
	}


// ******** set up morse code tree  ****************
	int N = sizeof letterlist / sizeof *letterlist; 							//number of entries in tree

// create and populate morse tree
	add_node(&treePtr,"",0,""); 
	for (i=0; i < N; i++) add_node(&treePtr,letterlist[i],0,codelist[i]);

// ******** check tree  ****************
//	pointer = treePtr; 
//	Inorder(pointer);

// ******** reverse check  ****************
//	pointer = treePtr; 
//	Postorder(pointer);	
//	exit(0);




// ******** set up the T-code decoder ***************
	pointer = treePtr; 
	codeP = NewCode (codeP);
    int offset = 0, readCharCount;
    
	sscanf(nmorse + offset, "%19s%n", t, &readCharCount); // read code prescription
	offset += readCharCount;
	codeP->AlphabetP->Q = atoi(t); // size of the set
	for ( i=0; i < codeP->AlphabetP->Q; i++) {
	  sscanf(nmorse + offset, "%19s%n", t, &readCharCount);
	  offset += readCharCount;
	  codeP->AlphabetP->a[i] = t[0];
	};
	sscanf(nmorse + offset, "%d%n", &codeP->q, &readCharCount );
    offset += readCharCount;
	for ( i=1; i <= codeP->q; i++ ) {
		sscanf(nmorse + offset, "%d%n", &codeP->e[i], &readCharCount );
        offset += readCharCount;
		sscanf(nmorse + offset, "%d%n", &codeP->PDTs[i]->size, &readCharCount );
        offset += readCharCount;
		for ( j=0; j < codeP->PDTs[i]->size; j++)
			sscanf(nmorse + offset, "%d%n", &codeP->PDTs[i]->w[j], &readCharCount );
            offset += readCharCount;
	}

// ********  character code is now defined ***************

	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_UNSPEC; // set to AF_INET to force IPv4
	hints.ai_socktype = SOCK_DGRAM;
	hints.ai_flags = AI_PASSIVE; // use my IP

	strcpy(&hostip[0], "127.0.0.1"); // default to current machine but accepts -h [hostip]
	if ((rv = getaddrinfo(NULL, "9222", &hints, &servinfo)) != 0) {
		fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
		return 1;
	}

	// loop through all the results and bind to the first we can
	for(p = servinfo; p != NULL; p = p->ai_next) {
		if ((sockfd = socket(p->ai_family, p->ai_socktype,
				p->ai_protocol)) == -1) {
			perror("listener: socket");
			continue;
		}

		if (bind(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
			close(sockfd);
			perror("listener: bind");
			continue;
		}

		break;
	}

	if (p == NULL) {
		fprintf(stderr, "listener: failed to bind socket\n");
		return 2;
	}



// **************loop the loop *********
// zero out ring buffer
   	for (i=0; i <10; i++ ) ring[i]=0;	


	i=0;
	while(scanfReturn != EOF) {

		addr_len = sizeof their_addr;
		if ((numbytes = recvfrom(sockfd, buf, MAXBUFLEN-1 , 0,(struct sockaddr *)&their_addr, &addr_len)) == -1) {
			perror("recvfrom");
			exit(1);
		}
	//	printf("listener: got packet from %s\n", might want to secure where the data is coming from?
			inet_ntop(their_addr.ss_family, get_in_addr((struct sockaddr *)&their_addr), s, sizeof s);
	//	printf("listener: packet is %d bytes long\n", numbytes);
		buf[numbytes] = '\0';
//		printf("%s\n", buf);fflush(stdout);

 //  		scanfReturn = fscanf(stdin," %1023[^\n]", message); // line at a time from stdin

		if (envelopeFlg == TRUE) sscanf(buf, "%d %f", &value, &envelope);
		else sscanf(buf, "%d", &value);
		bitcount++; 
		if ((i==-1) && (value==1)){
			i=0;
		}
		if ((i >= 40) && (value==0) && (lastvalue==0)) { // if we have a long run of 0's? then decode and go into a wait state
			string[i]='0'; 
			string[i+1]=0;
			i = -1;
			decode(string, codeP->q); 
		} // sit on the same 
		else if ((i >= 39) && (value==1) && (lastvalue==1)) { // can only get to this if we have a run of 1's so discard the lot
			i = -1;
		}
		else if ((value==0) && (lastvalue==1)) { /// termination '10' which allows one to break the string and restart
			string[i]='0'; 
			string[i+1]=0; 
			i=0;
			// here is the point at which to decode the string
			decode(string, codeP->q);
//			fprintf(stdout, "%s\n",string); 
//			fflush(stdout);
		} // end of string ouptut string
		else if (value==0) { string[i]='0'; i++; string[i]=0;}
		else if (value==1) { string[i]='1'; i++; string[i]=0;}
		
		lastvalue=value;

	}


	DisposeCode(&codeP);
	freeaddrinfo(servinfo);
	close(sockfd);


	return 0;
}
