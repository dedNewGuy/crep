#include <stdio.h>
#include "minutil.h"

int main(void)
{
	const char *path = "shakespear-smol.txt";

    Min_String_Builder sb = {0};
	if (!min_read_entire_file(path, &sb)) return 1;

	min_log(MIN_LOG, "Count: %d, Capacity: %d", sb.count, sb.capacity);

	int at_line = 0;
	for (size_t i = 0; i < sb.count; ++i) {
		char c = sb.items[i];
		if (c == '\n') at_line++;
	}
	
	min_free_sb(sb);

	min_log(MIN_LOG, "Total Line: %d", at_line);
	return 0;
}
