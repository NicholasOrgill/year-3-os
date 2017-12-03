#include <stdio.h>
#include <sys/socket.h>
#include <netdb.h> 
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include<signal.h>
#include <unistd.h>
#include <errno.h>

//defines
#define ERRORCODE 1

int sockfd = -1;


// displays error messages from system calls
void error(char *msg){
    perror(msg);
    exit(ERRORCODE);
}

void sig_handler(int signo)
{
  if (signo == SIGINT) {
    printf("Received SIGINT\n");
    if (sockfd > 0) {
      close(sockfd);
    }
    exit(0);
  }
  if (signo == SIGTERM) {
    printf("received TERM\n");
    if (sockfd > 0) {
      close(sockfd);
    }
    exit(0);
  }
}


int main(int argc, char *argv[]){

    // setup signal handler
    if (signal(SIGINT, sig_handler) == SIG_ERR) {
	printf("\ncan't catch SIGINT\n");
	return 1;
    }

    if (signal(SIGTERM, sig_handler) == SIG_ERR) {
	printf("\ncan't catch SIGTERM\n");
	return 1;
    }

    //check arguments
    if (argc != 3) {
	fprintf(stderr, "usage %s <hostname> <port>\n", argv[0]);
	exit(ERRORCODE);
    }

    //create socket
    errno = 0;
    unsigned long portno = strtoul(argv[2], NULL, 10);
    if(errno != 0 || portno > 65535){
	error("ERROR: Invalid port number");
    }
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if(sockfd < 0){
	error("ERROR: opening socket");
    }

    //setup connection
    struct hostent *server = NULL;
    server = gethostbyname (argv[1]);
    if(server == NULL) {
	fprintf (stderr, "ERROR: no such host\n");
	exit (1);
    }
    struct sockaddr_in serv_addr;
    bzero ((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy ((char *)server->h_addr, 
	   (char *)&serv_addr.sin_addr.s_addr,
	   server->h_length);
    serv_addr.sin_port = htons(portno);

    //connect to the server
    if(connect (sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0){
	error("ERROR: connection failed");
    }

    //get input from stdin
    char* buffer = NULL;
    size_t size = 0;
    int n = 0;
    while(getline(&buffer, &size, stdin) > 0){
	//send input
	n = write(sockfd, buffer, strlen(buffer));
	if(n < strlen(buffer)){
	    fprintf(stderr, "ERROR: couldn't write string %s to socket\n", buffer);
	    exit(ERRORCODE);
	}
    }

    //close connection
    close (sockfd);
    return 0;
}
