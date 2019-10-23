#include "mtf.h"

#include "intro.c"

char *new_data = (char *)mtf_encode(data, &new_len);
if (new_data == NULL) {
	dprintf(STDERR_FILENO, "MTF encode failed\n");
	exit(1);
}

#include "end.c"
