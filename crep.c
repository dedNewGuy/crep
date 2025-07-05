#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#define min_return_defer(value) do { result = (value); goto defer } while(0); \

typedef struct {
	char *items;
	size_t count;
	size_t capacity;
} Min_String_Builder;

void min_sb_append(Min_String_Builder *sb, char *item)
{
	
}

int min_read_file(const char *path, Min_String_Builder *sb)
{
	size_t buf_size = 32 * 1024;
	char *buf = malloc(buf_size * sizeof(char));
	if (buf == NULL) {
		perror("crep");
		return 0;
	}
	FILE *file = fopen(path, "r");
    if (file == NULL) {
		perror("crep");
		return 0;
	}

	size_t n = fread(buf, 1, buf_size, file); 
	while (n > 0) {
		printf("%s", buf);
		n = fread(buf, 1, buf_size, file); 
	}
	if (ferror(file)) {
		
	}

	fclose(file);
}

int main(void)
{
	const char *path = "sample-1.txt";

    Min_String_Builder buf = {0};
	min_read_file(path, &buf);

	free(buf.items);
	return 0;
}
