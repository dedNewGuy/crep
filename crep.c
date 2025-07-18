#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <ctype.h>
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

	// [NOTE] ONLY USED BY Naive implementation. This function is not use anymore
	// since I don't use naive implementation but merely as a reference for me in the future
	size_t start = tp.start_line;
	size_t lc_idx = 0;
	while (sb.items[start] != '\n' && start < sb.count) {
		buf[lc_idx++] = sb.items[start];
		start++;
	}
	buf[lc_idx] = '\0';
}

// NAIVE CAUSE SEGFAULT WHEN READING MULTIPLE FILES. IM NOT BOTHERED TO DEBUG THIS.
// Prolly will if it matters. Main implementation will use DFA anyway.
void naive_string_matching(Min_String_Builder sb, Text_Pointer *tp, const char *input)
{
	tp->at_line = 0;	
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

void dfa_string_matcher(size_t pat_len, size_t n_possible_input, int (*dfa_table)[n_possible_input], Min_String_Builder sb, Text_Pointer *tp)
{
	tp->at_line = 1;
	
	size_t state = 0;
	size_t buf_len = 2 * 1024;
	char buf[buf_len];
	bool is_read = false;
	memset(buf, 0, buf_len);
	for (size_t i = 0; i < sb.count; ++i, tp->at_col++) {
		char ch = sb.items[i];
		int code = (int)ch;
		if (code < 0 || code >= (int)n_possible_input) continue;
		buf[tp->at_col - 1] = ch;
	    state = dfa_table[state][code];
		if (state == pat_len) {
			if (!is_read) {
				size_t tmp_i = i;
				size_t tmp_at_col = tp->at_col - 1; 			
				while (sb.items[tmp_i] != '\n') {
					buf[tmp_at_col] = sb.items[tmp_i];
					tmp_i++;
					tmp_at_col++;
				}
				buf[tmp_at_col] = '\0';
				is_read = true;
			}
			printf("%s:%d:%zu:%s\n", tp->path, tp->at_line, tp->at_col - pat_len + 1, buf);
		}
		if (ch == '\n') {
			tp->at_line++;
			tp->at_col = 0;
			is_read = false;
			memset(buf, 0, buf_len);
		}
	}
}

int main(int argc, char **argv)
{
	if (argc < 3) {
		fprintf(stderr, "Usage: crep <pattern> <file...>\n");
		return 1;
	}

	const char *pattern = argv[1];
	size_t pat_len = strlen(pattern);
	size_t n_possible_input = 127;
	size_t n_state = strlen(pattern) + 1;
	int dfa_table[n_state][n_possible_input];
	construct_dfa(pattern, n_possible_input, n_state, dfa_table);
	
	Text_Pointer tp = {0};
	tp.at_col = 1;
	
	for (int i = 2; i < argc; ++i) {
		tp.path = argv[i];
		Min_String_Builder sb = {0};
		if (!min_read_entire_file(tp.path, &sb)) return 1;
		dfa_string_matcher(pat_len, n_possible_input, dfa_table, sb, &tp);
		/* naive_string_matching(sb, &tp, pattern);*/
		min_free_sb(sb);
	}

	return 0;
}
