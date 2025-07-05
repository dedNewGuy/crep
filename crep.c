#include <stdio.h>
#include "minutil.h"

int main(void)
{
	const char *path = "shakespear-smol.txt";

    Min_String_Builder sb = {0};
	if (!min_read_file(path, &sb)) return 1;

	min_log(MIN_LOG, "Count: %d, Capacity: %d", sb.count, sb.capacity);
	printf("%s\n", sb.items);
	
	min_free_sb(sb);
	return 0;
}
