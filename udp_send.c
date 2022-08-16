/*
** ptxsnd.c -- sends frames till EOF received
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

int main(int argc, char *argv[])
{
	int sockfd;
	struct addrinfo hints, *servinfo, *p;
	int rv;
	int numbytes;
    
    int scanfReturn = 0;
    int count = 0; // now down sampling
    char* message;
    char messageArray[1024];
    
    message = messageArray;
  
	if (argc != 3) {
		fprintf(stderr,"usage: echo \"frame\" | framesnd hostname portnumber\n");
		exit(1);
	}
	
	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_DGRAM;

	if ((rv = getaddrinfo(argv[1], argv[2], &hints, &servinfo)) != 0) {
		fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
		return 1;
	}

	// loop through all the results and make a socket
	for(p = servinfo; p != NULL; p = p->ai_next) {
		if ((sockfd = socket(p->ai_family, p->ai_socktype,
				p->ai_protocol)) == -1) {
			perror("udpsend: socket");
			continue;
		}

		break;
	}

	if (p == NULL) {
		fprintf(stderr, "udpsend: failed to bind socket\n");
		return 2;
	}
    
    
	// Read to eof
	count=0;
	while (! feof(stdin)) { messageArray[count] = getchar(); count++;}
	
    
		if ((numbytes = sendto(sockfd, message, count-1, 0,
				 p->ai_addr, p->ai_addrlen)) == -1) {
			perror("framesnd: sendto");
			exit(1);
		}

   
	freeaddrinfo(servinfo);

	close(sockfd);

	return 0;
}
