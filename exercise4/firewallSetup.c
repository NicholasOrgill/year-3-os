#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

void valid_line(char *line) {
	char *port;
	char *exec;
	int port_no;
	
	port = strtok(line, " ");
	exec = strtok(NULL, "\n");
	port_no = atoi(port);
	
	if (port_no != 80) {
		fprintf(stderr, "ERROR: Ill-formed file\n");
		exit(1);
	}
	
	if (access(exec, F_OK) == -1) {
		fprintf(stderr, "ERROR: Cannot execute file\n");
		exit(1);
	}
}

int main(int argc, char **argv) {
	FILE *fp;
	
	//assume two args required unless proven otherwise
	int msg_concat = 1;
	
	//assume valid until proven otherwise
	int valid = 1;
	
	//check validity of argument count
	if (!strcmp(argv[1], "L")) {
		if (argc != 2) {
			valid = 0;
		} else {
			msg_concat = 0;	
		}
	} else if (!strcmp(argv[1], "W")) {
		if (argc != 3) {
			valid = 0;
		} else if (access(argv[2], F_OK) == -1) {
			valid = 0;
		} else {
			FILE *txtfp;
			txtfp = fopen(argv[2], "r");
			char l[128];
			while(fgets(l, sizeof(line), txtfp) != NULL) {
				valid_line(l);
			}
		}
	} else {
		valid = 0;
	}
	
	//error for invalid
	if (valid == 0) {
		fprintf(stderr, "Usage: firewallSetup L to list rules, or firewallSetup W <filename>.txt to set rules given by <filename>.txt\n");
		exit(1);
	}
	
	fp = fopen("/proc/firewallExtension", "w");
	if (msg_concat == 1) {
		fprintf(fp, "%s %s", argv[1], argv[2]);
	} else {
		fprintf(fp, "%s", argv[1]);
	}
	fflush(fp);
	fclose(fp);
	
	return 0;
}
