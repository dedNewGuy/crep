#include <stdio.h>
#include "minutil.h"

int main(void)
{
	const char *path = "./sample.txt";

    Min_String_Builder sb = {0};
	if (!min_read_entire_file(path, &sb)) return 1;

	min_log(MIN_LOG, "Count: %d, Capacity: %d", sb.count, sb.capacity);

	min_log(MIN_LOG, "Starting search...");
	int at_line = 0;
	char *input = "this";
	size_t idx = 0;
	size_t char_at = 0;
	while (char_at < sb.count) {
		char c = sb.items[char_at];
		// min_log(MIN_LOG, "Comparing %c & %c", c, input[idx]);
		if (c == '\n') {
			at_line = at_line + 1;
			char_at++;
			continue;
		}
		if (c == input[idx]) {
			if (idx == strlen(input) - 1) {
				printf("%d\n", at_line + 1);
				idx = 0;
			} else idx++;
		} else if (idx != 0) {
			idx = 0;
			continue;
		}
		char_at++;
	}
	min_free_sb(sb);
	min_log(MIN_LOG, "Total Line: %d", at_line);
	return 0;
}
