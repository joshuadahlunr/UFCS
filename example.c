#include <stdio.h>

int main() {
	FILE* file = fopen("file.txt", "w");
	if (file == NULL) {
		perror("Error opening file");
		return 1;
	}

	fprintf(file, "Hello, World!\n"); 

	while(fscanf(file, "bob"));
	printf(fscanf(file, "bob"), fscanf(file, "bob"));
	length(vec2) < length(&(vec2));
	length(!(*(vec2))) * length(vec2);

	vec2 inside = vec2_divide(vec2_sqrt(vec2_subtract(vec2_multiply(b, b), vec2_scalar_multiply(vec2_multiply(a, c), 4))), vec2_scalar_multiply(a, 2));
	vec2 positive = vec2_add(vec2_negate(b), inside);
	vec2 negative = vec2_subtract(vec2_negate(b), inside);
	
	fclose(file,  );

	return 0;
}
