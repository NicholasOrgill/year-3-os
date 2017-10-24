#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>

#define BUFFERLENGTH 1024

void sig_handler(int signo)
{
  if (signo == SIGINT)
    printf("received SIGINT\n");
}

int main(int argc, char **argv) {
    struct sockaddr_in server_addr;
    int port;
    char buffer[BUFFERLENGTH];

    signal(SIGINT, sig_handler);
    signal(SIGTERM, sig_handler);

    //arg length check
    if (argc != 3) {
       fprintf (stderr, "No port or address included.\n");
       exit(1);
    }

    //get port from argv
    port = atoi(argv[2]);
    if ((port < 0) || (port > 65535)) {
	   fprintf (stderr, "Illegal port number.\n");
	   exit(1);
    }

    //create socket
    int sock = socket (AF_INET, SOCK_STREAM, 0);

    //check socket is working 
    if (sock < 0) {
        fprintf(stderr, "Failed to open socket.\n");
        exit(1);
    }

    //get server data
    struct hostent *server = gethostbyname(argv[1]);
    if (!server) {
        fprintf(stderr, "Invalid host.\n");
        exit(1);
    }

    //basic stuff
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    server_addr.sin_addr.s_addr = INADDR_ANY;

    //make connection to server
    if (connect (sock, (struct sockaddr *) &server_addr, sizeof (server_addr)) < 0) {
        fprintf(stderr, "Failed to connect.\n");
        exit(1);
    }

    memset(buffer, 0, BUFFERLENGTH*sizeof(char));

	char c;
	c = getc(stdin);
        while(c != EOF && c != '\0') {
		write(sock,&c ,sizeof(char));
                c = getc(stdin);
	}
	write(sock, "\0", sizeof(char));
	
    close(sock);
    return 0;
}
