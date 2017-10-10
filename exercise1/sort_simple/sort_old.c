#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char ** argv) {
	int total = 20;
	char* temp;
	char** names = malloc(total*sizeof(char*));
	for(int i = 0; i < total; i++) {
		names[i] = malloc(total);
		if(scanf("%s",names[i]) < 0) {
			total = i;
			free(names[i]);
			break;
		}
	}

	for (int i = 0; i < total - 1; i++) {
		for (int j = i + 1; j < total; j++) {
			if (strcmp(names[i], names[j]) > 0) {
				temp = names[i];
				names[i] = names[j];
				names[j] = temp;
			}
		}
	}
	for (int i = 0; i < total; i++) {
		puts(names[i]);
		free(names[i]);
	}
	free(names);
	return 0;
}
