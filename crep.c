#include <stdio.h>
#include <stdlib.h>
#include "minutil.h"

// Offset by (int)input_len + 1 to point to the first character
// of the input
#define print_loc(path, line, col, input_len, buf)\
	printf("%s:%d:%d:%s\n", path, line + 1, col - (int)input_len + 1, buf);\

typedef struct {
	const char *path;
	int start_line;
	int at_line;
	int at_col;
} Text_Pointer;

void get_line_content(Min_String_Builder sb, Text_Pointer tp, char buf[])
{
	// TODO: If target is on the same line, it read the same line twice.. okay for now
	// Later maybe need to add some sort of data structure to keep already read line
	// Hash table? for now it's overkill I guess. Want to focus on implementing
	// string matching algorithm first
	size_t start = tp.start_line;
	size_t lc_idx = 0;
	while (sb.items[start] != '\n' && start < sb.count) {
		buf[lc_idx++] = sb.items[start];
		start++;
	}
	buf[lc_idx] = '\0';
}

void naive_string_matching(Min_String_Builder sb, Text_Pointer *tp, const char *input)
{
	size_t in_len = strlen(input);
	int idx = 0;
	int char_at = 0;
	while (char_at < (int)sb.count) {
		char c = sb.items[char_at];
		if (c == '\n') {
			tp->at_line = tp->at_line + 1;
			tp->at_col = 1;
			char_at++;
			tp->start_line = char_at;
			continue;
		}
		if (c == input[idx]) {
			if (idx == (int)in_len - 1) {
				char line_content[1024] = "\0";
				get_line_content(sb, *tp, line_content);
				print_loc(tp->path, tp->at_line, tp->at_col, in_len, line_content);
				idx = 0;
			} else idx++;
		} else if (idx != 0) {
			idx = 0;
			continue;
		}
		tp->at_col++;
		char_at++;
	}
}

void construct_dfa(const char *pattern, size_t n_possible_input, size_t n_state,
				   int dfa_table[n_state][n_possible_input])
{
	memset(dfa_table, 0, sizeof(dfa_table[0][0]) * n_state * n_possible_input);
	dfa_table[0][(int)pattern[0]] = 1;

	size_t x = 0;
	for (size_t q = 1; q < n_state; ++q) {
		for (size_t i = 0; i < n_possible_input; ++i) {
			dfa_table[q][i] = dfa_table[x][i];
		}
		if (q < n_state - 1) {
			dfa_table[q][(int)pattern[q]] = q + 1;
		}
		x = dfa_table[q][(int)pattern[q]];
	}

	// Print the table
	/* for (size_t i = 0; i < n_state; ++i) { */
	/* 	for (size_t j = 0; j < n_possible_input; ++j) { */
	/* 		printf("%d ", dfa_table[i][j]); */
	/* 	} */
	/* 	printf("\n"); */
	/* } */

}

typedef struct {
	int *items;
	size_t count;
	size_t cap;
} Occ;

Occ dfa_string_matcher(const char *pattern, Min_String_Builder sb)
{
	size_t ascii_count = 126;
	size_t pat_len = strlen(pattern);
	size_t n_state = pat_len + 1;
	int dfa_table[n_state][ascii_count];
	construct_dfa(pattern, ascii_count, n_state, dfa_table);

	Occ occ = {0};

	// TODO: Handle line numbers and columns number in the actual text
	// Basically add a workaround for the Text_Pointer
	size_t state = 0;
	for (size_t i = 0; i < sb.count; ++i) {
		int code = (int)sb.items[i];
	    state = dfa_table[state][code];
		if (state == pat_len) {
			int curr_occ = i - pat_len + 1;
			da_append(&occ, curr_occ);
		}
	}

	return occ;
}

int main(void)
{
	// TODO: Make it usable as command line tool for later
	Text_Pointer tp = {0};
	tp.path = "./sample.txt";
	tp.at_col = 1;

    Min_String_Builder sb = {0};
	if (!min_read_entire_file(tp.path, &sb)) return 1;

	min_log(MIN_LOG, "Count: %d, Capacity: %d", sb.count, sb.capacity);
	min_log(MIN_LOG, "Starting search...");

	const char *pattern = "Gutenberg";

	Occ occ = dfa_string_matcher(pattern, sb);

	for (size_t i = 0; i < occ.count; ++i) {
		printf("Occ: %d\n", occ.items[i]);
	}

	printf("Col 50: %c\n", sb.items[50]);

	min_free_sb(sb);
	min_log(MIN_LOG, "Total Line: %d", tp.at_line);
	return 0;
}
