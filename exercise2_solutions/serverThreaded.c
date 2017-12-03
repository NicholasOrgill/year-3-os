#include <stdio.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <ctype.h>
#include <stdlib.h>
#include <strings.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/stat.h>
#include <assert.h>
#include <errno.h>

#define ERRORCODE 1

// --- Data strcutures ---
typedef struct threadArgs{
    int socket;
} threadArgs;

unsigned long long counter = 0;
char *logFileName;
int fileErased = 0;
pthread_mutex_t mut_counter; //lock for counter
pthread_mutex_t mut_open; //lock for erasing logfile

// --- Implementation ---
void error(char *msg)
{
    perror(msg);
    exit(ERRORCODE);
}

// erase log file
void eraseLogFile(char *logFileName) {
    FILE* f = fopen(logFileName, "w");
    if(f == NULL){
	fprintf(stderr, "Cannot erase logfile\n");
    }
    else {
	fclose(f);
	fileErased = 1;
    }
}

/* the procedure called for each request */
void *processRequest (void *in){

    //parse arguments
    assert(in != NULL);
    threadArgs* args = (threadArgs*)in;


    // check whether logfile has been erased
    pthread_mutex_lock(&mut_open);
    if (!fileErased) {
	eraseLogFile(logFileName);
    }
    pthread_mutex_unlock(&mut_open);
    
    //open file for append
    //opening a file multiple times is thread-safe
    //however, writing is not!
    FILE* f = fopen(logFileName, "a");
    if(f == NULL){
	fprintf(stderr, "Cannot open logfile - not processing request\n");
        close(args->socket);
        free(args);
        pthread_exit(NULL);
    }
  

    FILE* socket_fp = fdopen(args->socket, "r+");
    if(socket_fp == NULL){
	fprintf(stderr, "Could not read data from client\n");
	close(args->socket);
	free(args);
	pthread_exit(NULL);
    }
  
    //read input
    char prefix[23]; //strlen(max ull) = 21; +1 space; +1 '\0' = 23
    int prefix_len = 0;
    char* buffer = NULL;
    size_t size = 0;
    while(getline(&buffer, &size, socket_fp) > 0){
	//lock to make getting the counter and fwrite atomic
	pthread_mutex_lock(&mut_counter);
	{
	    //prepend counter
	    prefix_len = snprintf(prefix, 23, "%llu ", counter);
	    counter++;
	    if (fwrite(prefix, sizeof(char), prefix_len, f) < prefix_len) {
	        fprintf (stderr, "Could not write to logfile!\n");
	    }
	    //write to file
	    else if (fwrite(buffer, sizeof(char), strlen(buffer), f) < strlen(buffer)) {
	        fprintf(stderr, "Could not write to logfile!\n");
	    }
	    if (fflush(f)) {
	        fprintf(stderr, "Could not write to logfile!\n");
	    }
	}
	pthread_mutex_unlock(&mut_counter);

    }
    
    if (errno) {
	fprintf(stderr, "Couldn't read from client\n");
    }
    
    //avoid memory leaks
    free(buffer);
    assert(args != NULL);
    fclose(socket_fp);
    free(args);
    fclose(f);
    
    pthread_exit (NULL);
}


int main(int argc, char *argv[]){
    //checks
    if (argc != 3) {
	fprintf(stderr, "usage %s <port> <logfile>\n", argv[0]);
	exit(ERRORCODE);
    }

    // erase log file
    logFileName = argv[2];
    eraseLogFile(logFileName);
    
    //get port
    errno = 0;
    unsigned long portno = strtoul(argv[1], NULL, 10);
    if(errno != 0 || portno > 65535){
	error("ERROR: Invalid port number");
    }
    //create server socket
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if(sockfd < 0){
	error("ERROR: opening socket");
    }
    //make socket (port) reusable
    if(setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &(int){1}, sizeof(int)) < 0){
	error("ERROR: setsockopt failed");
    }
    struct sockaddr_in serv_addr;
    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(portno);
  
    //bind socket
    if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0){
	error("ERROR: binding socket");
    }

    //ready to accept connections
    listen (sockfd, 5);
    struct sockaddr_in cli_addr;
    socklen_t clilen = sizeof (cli_addr);
   
    //wait for connections
    while(1){
	//create thread arguments
	threadArgs* targs;
	targs = (threadArgs*)malloc(sizeof(threadArgs));
	if(!targs) {
	    fprintf (stderr, "ERROR: memory allocation failed\n");
	    exit(ERRORCODE);
	}
	//accept new connection
	targs->socket = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);
	if(targs->socket < 0){
	    error("ERROR: accept");
	}

	//create separate thread for processing
	pthread_t server_thread;
	pthread_attr_t pthread_attr; //attributes for thread
	if (pthread_attr_init (&pthread_attr)) {
	    fprintf(stderr, "ERROR: creating initial thread attributes failed\n");
	    exit(ERRORCODE);
	}
	if (pthread_attr_setdetachstate(&pthread_attr, PTHREAD_CREATE_DETACHED)) {
	    fprintf(stderr, "ERROR: setting thread attributes failed\n");
	    exit(ERRORCODE);
	}
	int result = pthread_create(&server_thread, &pthread_attr, processRequest, (void*)targs);
	if (result != 0) {
	    fprintf(stderr, "ERROR: thread creation failed!\n");
	    exit(ERRORCODE);
	}
    }

    // not reached
    return 0; 
}
