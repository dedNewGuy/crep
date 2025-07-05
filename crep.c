#include <assert.h>
#include <errno.h>
#include <string.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdarg.h>


#define MIN_ASSERT assert
#define MIN_FREE free
#define min_return_defer(value) do { result = (value); goto defer; } while(0); \

typedef enum {
	MIN_LOG,
	MIN_WARNING,
	MIN_ERROR,
} Min_Log_Level;

void min_log(Min_Log_Level log_level, const char *fmt, ...)
{
	switch (log_level) {
	case MIN_LOG:
		fprintf(stderr, "[LOG] ");
		break;
	case MIN_WARNING:
		fprintf(stderr, "[WARNING] ");
		break;
	case MIN_ERROR:
		fprintf(stderr, "[ERROR] ");
		break;
	default:
		MIN_ASSERT(0 && "unreachable");
	}

	va_list args;
	va_start(args, fmt);
	vfprintf(stderr, fmt, args);
	va_end(args);
	fprintf(stderr, "\n");
}

typedef struct {
	char *items;
	size_t count;
	size_t capacity;
} Min_String_Builder;

void min_sb_append(Min_String_Builder *sb, char *item)
{
	
}

bool min_read_file(const char *path, Min_String_Builder *sb)
{
	bool result = true;
	size_t buf_size = 32 * 1024;
	char *buf = malloc(buf_size * sizeof(char));
	MIN_ASSERT(buf != NULL && "Not enough memory");
	FILE *file = fopen(path, "r");
    if (file == NULL) {
		min_log(MIN_ERROR, "Failed to open file path %s: %s", path, strerror(errno));
		min_return_defer(false);
	}

	size_t n = fread(buf, 1, buf_size, file); 
	while (n > 0) {
		printf("%s", buf);
		n = fread(buf, 1, buf_size, file); 
	}
	if (ferror(file)) {
		min_log(MIN_ERROR, "Failed to read file %s: %s", path, strerror(errno));
		min_return_defer(false);
	}

 defer:
	MIN_FREE(buf);
	if (file) fclose(file);
	return result;
}

int main(void)
{
	const char *path = "sample-1.txt";

    Min_String_Builder buf = {0};
	if (!min_read_file(path, &buf)) return 1;

	return 0;
}
