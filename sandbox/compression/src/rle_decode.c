#include "rle.h"

#include "intro.c"

char *new_data = (char *)rle_decode(data, &new_len);
if (new_data == NULL) {
	dprintf(STDERR_FILENO, "RLE decode failed\n");
	exit(1);
}

#include "end.c"
