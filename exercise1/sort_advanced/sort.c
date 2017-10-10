#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char ** argv) {
	int usage = 0;
	int size = 100;
	char* buffer = malloc(1025);
	char** words = malloc(100*sizeof(char*));
	while(fgets(buffer,1025,stdin)) {
		if(usage == size-1) {
			char** new_array = realloc(words,(100+size)*sizeof(char*));
			size+=100;
			char** old = words;
			words = new_array;
			free(old);
		}
		if (buffer[strlen(buffer)-1] == '\n') {
			buffer[strlen(buffer)-1] = '\0';
		}
		words[usage] = buffer;
		usage++;
	}

	for (int i = 0; i < usage - 1; i++) {
		for (int j = i + 1; j < usage; j++) {
			if (strcmp(words[i], words[j]) > 0) {
				buffer = words[i];
				words[i] = words[j];
				words[j] = buffer;
			}
		}
	}
	for (int i = 0; i < usage; i++) {
		puts(words[i]);
	}
	return 0;
}
