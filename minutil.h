#ifndef MIN_UTIL_H_
#define MIN_UTIL_H_

#include <stdlib.h>
#include <assert.h>
#include <stdarg.h>
#include <errno.h>
#include <string.h>
#include <stdbool.h>

#define MIN_REALLOC realloc
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

#define MIN_DA_INIT_CAP 256

#define min_da_append_many(da, new_items, new_items_count)				\
	do {																\
		if ((da)->count + new_items_count > (da)->capacity) {			\
			if ((da)->capacity == 0) {									\
				(da)->capacity = MIN_DA_INIT_CAP;						\
			}															\
			while ((da)->count + new_items_count > (da)->capacity) {	\
				(da)->capacity *= 2;									\
			}															\
			(da)->items = MIN_REALLOC((da)->items, (da)->capacity*sizeof(*(da)->items)); \
			MIN_ASSERT((da)->items != NULL && "NOT ENOUGH MEMORY");		\
		}																\
		memcpy((da)->items + (da)->count, new_items, new_items_count*sizeof(*(da)->items));	\
		(da)->count += new_items_count;									\
	} while (0)															\

typedef struct {
	char *items;
	size_t count;
	size_t capacity;
} Min_String_Builder;

#define min_sb_append_buf(sb, buf, buf_size) min_da_append_many(sb, buf, buf_size)
#define min_free_sb(sb) MIN_FREE(sb.items)

bool min_read_entire_file(const char *path, Min_String_Builder *sb)
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
		min_sb_append_buf(sb, buf, n);
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


#endif // MIN_UTIL_H_
