#include "mtf.h"

#include "intro.c"

char *new_data = (char *)mtf_decode(data, &new_len);
if (new_data == NULL) {
	dprintf(STDERR_FILENO, "MTF decode failed\n");
	exit(1);
}

#include "end.c"
