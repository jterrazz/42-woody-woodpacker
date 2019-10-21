#include "dhe.h"

#include "intro.c"

char *new_data = (char *)dhe_decode(data, &new_len);
if (new_data == NULL) {
	dprintf(STDERR_FILENO, "DHE decode failed\n");
	exit(1);
}

#include "end.c"
