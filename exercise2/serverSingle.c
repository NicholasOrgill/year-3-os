#include <stdio.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>

#define BUFFERLENGTH 1024

int client_socket = -1;
FILE *fp = NULL;
int line_no;

void sig_handler(int signo) {
    if(signo == SIGTERM || signo == SIGINT) {
    	if(client_socket >= 0) close(client_socket);
	if(fp != NULL) fclose(fp);
        exit(0);
    }
}

int main(int argc, char **argv) {
    struct sockaddr_in addr, client_addr;
    int port, line_no;
    char buffer[BUFFERLENGTH];
    char *logfile;
    line_no = 0;

    signal(SIGTERM, sig_handler);
    signal(SIGINT, sig_handler);

    //arg length check
    if (argc != 3) {
        fprintf(stderr, "No port included.\n");
        sig_handler(SIGTERM);
    }

    //store logfile
    logfile = argv[2];
    //get port from argv
    port = atoi(argv[1]);
    if((port < 0) || (port > 65535)) {
        fprintf(stderr, "Illegal port number.\n");
        sig_handler(SIGTERM);
    }

    //create socket
    int sock = socket(AF_INET, SOCK_STREAM, 0);

    //checks socket is working
    if (sock < 0) {
        fprintf(stderr, "Failed to create socket.\n");
        sig_handler(SIGTERM);
    }

    //basic stuff
    memset((char *) &addr, 0, sizeof(addr));
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);

    //bind error check
    if (bind(sock, (struct sockaddr *) &addr, sizeof(addr)) < 0) {
        fprintf(stderr, "Failed to bind to socket.\n");
        sig_handler(SIGTERM);
    }
    //start listening
    listen(sock, 5);

    //obtain size of client_addr
    socklen_t client_size = sizeof(client_addr);
    //loop until ctrl-c is pressed
    while (1) {

        //flush buffer
        memset(buffer, 0, sizeof(*buffer) * BUFFERLENGTH);

        //perform accept on client socket
        client_socket = accept(sock, (struct sockaddr *) &client_addr, &client_size);

        //check new socket accepted
        if (client_socket < 0) {
            fprintf(stderr,"Failed to accept.\n");
            sig_handler(SIGTERM);
        }

        //reads in the request
	int position = 0;
	char c;

	read(client_socket, &c, sizeof(c));
        while(c != EOF && c != '\0') {
		buffer[position] = c;
		position++;
                read(client_socket, &c, sizeof(c));
	}
	buffer[position-1] = '\0';
	position = 0;
	fp = fopen(logfile,"a+");
	fprintf(fp, "%d ", line_no);
	while((c = buffer[position]) != '\0') {
	    fprintf(fp, "%c", c);
	    if(c == '\n') fprintf(fp, "%d ", ++line_no);
            position++;
	}
	fprintf(fp, "\n");
        fclose(fp);   
    }
}

