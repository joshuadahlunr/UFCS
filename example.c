#include <stdio.h>

int main() {
	FILE* file = fopen("file.txt", "w");
	if (file == NULL) {
		perror("Error opening file");
		return 1;
	}

	fprintf(file, "Hello, World!\n"); 

	while(file.fscanf("bob"));
	printf( fscanf(file, "bob"), fscanf(file, "bob"));
	length(vec2) < length(&vec2);
	length(*vec2) * length(vec2);
	
	fclose(file);

	return 0;
}