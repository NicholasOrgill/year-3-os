#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char ** argv) {
	int total = 20, current = 0, usage = 10000;
	char* temp;
	char buffer[10000];
	char** lines = malloc(usage*sizeof(char*));
	while(1) {
		if(scanf("%s",buffer) < 0) {
			total = current;
			break;
		} else {
			lines[current] = malloc(1+strlen(buffer));
			strcpy(lines[current], buffer);
			if(++current > usage-1) {
				char** new_array = malloc((100+usage)*sizeof(char*));
				usage+=100;
				for(int i = 0; i < current; i++) {
					new_array[i] = lines[i];
				}
				char** old = lines;
				lines = new_array;
				free(old);
			}
		}
	}

	for (int i = 0; i < current - 1; i++) {
		for (int j = i + 1; j < current; j++) {
			if (strcmp(lines[i], lines[j]) > 0) {
				temp = lines[i];
				lines[i] = lines[j];
				lines[j] = temp;
			}
		}
	}
	for (int i = 0; i < current; i++) {
		puts(lines[i]);
		free(lines[i]);
	}
	free(lines);
	return 0;
}
