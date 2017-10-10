#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char ** argv) {
	int usage = 0;
	char temp [21];
	char words [20][21];
	while(fgets(temp,21,stdin)) {
		if (temp[strlen(temp)-1] == '\n') {
			temp[strlen(temp)-1] = '\0';
		}
		strcpy(words[usage],temp);
		usage++;
	}

	for (int i = 0; i < usage - 1; i++) {
		for (int j = i + 1; j < usage; j++) {
			if (strcmp(words[i], words[j]) > 0) {
				strcpy(temp,words[i]);
				strcpy(words[i],words[j]);
				strcpy(words[j],temp);
			}
		}
	}
	for (int i = 0; i < usage; i++) {
		puts(words[i]);
	}
	return 0;
}
