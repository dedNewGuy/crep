#include <stdio.h>
#include "minutil.h"

int main(void)
{
	const char *path = "./sample.txt";

    Min_String_Builder sb = {0};
	if (!min_read_entire_file(path, &sb)) return 1;

	min_log(MIN_LOG, "Count: %d, Capacity: %d", sb.count, sb.capacity);

	min_log(MIN_LOG, "Starting search...");

	char *input = "Etext";
	size_t in_len = strlen(input);
	size_t idx = 0;
	int at_line = 0;
	int at_col = 0;

	size_t char_at = 0;
	while (char_at < sb.count) {
		char c = sb.items[char_at];
		// min_log(MIN_LOG, "Comparing %c & %c", c, input[idx]);
		if (c == '\n') {
			at_line = at_line + 1;
			at_col = 0;
			char_at++;
			continue;
		}
		if (c == input[idx]) {
			if (idx == in_len - 1) {
				int last_char_at = char_at;
				while (sb.items[last_char_at - 1] != '\n' && last_char_at > 0) {
					last_char_at--;
				}
				char line_content[1024] = "\0";
				int lc_idx = 0;
				while (sb.items[last_char_at] != '\n' && last_char_at < (int)sb.count) {
					line_content[lc_idx++] = sb.items[last_char_at];
					last_char_at++;
				}
				line_content[++lc_idx] = '\0';
				printf("%s:%d:%d:%s\n", path, at_line + 1, at_col - (int)in_len + 2, line_content);
				idx = 0;
			} else idx++;
		} else if (idx != 0) {
			idx = 0;
			continue;
		}
		at_col++;
		char_at++;
	}
	min_free_sb(sb);
	min_log(MIN_LOG, "Total Line: %d", at_line);
	return 0;
}
