#include <linux/module.h>
#include <linux/kernel.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

MODULE_AUTHOR("Nicholas Orgill");
MODULE_DESCRIPTION("Extensions to the firewall") ;
MODULE_LICENSE("GPL");

struct list {
	int port;
	char *exec;
	list *next;
};

struct list *l;

void listFirewallRules() {
	char line[256];
	FILE *fp = fopen("/var/log/kern.log", "a");
	if(fp == NULL) {
		fprintf(stderr, "Unable to open kernel log\n");
		exit(1);
	}
	
	//read through linked list
	while(l->next != NULL) {
		fprintf(fp, "%d %s", l->port, l->rule);
	}
	fclose(fp);
}

void writeFirewallRules(char *filename) {
	struct list *new_list;
	struct list *last_item;
	new_list->next = NULL;
	last_item = new_list;
	char line[256];
	FILE *newfile = fopen(filename, "r");
	
	if(newfile == NULL) {
		fprintf(stderr, "Invalid file\n");
		exit(1);
	}
	
	
	
	while(fgets(line, sizeof(line), newfile)) {
		char *port = strtok(line, " ");
		char *file = strtok(NULL, "\n");
		int p = atoi(port);
		if(p != 80) {
			fprintf(stderr, "Invalid rules - port number\n");
			exit(1);
		}
		if(access(file, F_OK) == -1) {
			fprintf(stderr, "Invalid rules - executable\n");
			exit(1);
		}
		
		//overwrite rules in linked list
		while(last_item->next != NULL) {
			last_item = last_item->next;
		}
		last_item->port = kmalloc(sizeof(int), GFP_KERNEL);
		last_item->port = p;
		last_item->exec = kmalloc(sizeof(char)*strlen(file), GFP_KERNEL);
		strcpy(last_item->exec, file);
		last_item->next = NULL;		
		
	}
	fclose(logfile);
	fclose(newfile);
}

int main(int argc, char **argv) {
	
	if(argc < 2) {
		fprintf(stderr, "Usage: firewallSetup L / W <filename>\n");
		exit(1);
	}
	if(strcmp(argv[1], "L") == 0 && argc == 2) listFirewallRules();
	else if(strcmp(argv[1], "W") == 0 && argc == 3) writeFirewallRules(argv[2]);
	else {
		fprintf(stderr, "Usage: firewallSetup L / W <filename>\n");
		exit(1);
	}
	return 0;
}
