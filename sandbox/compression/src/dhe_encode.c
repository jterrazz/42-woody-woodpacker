#include "dhe.h"

#include "intro.c"

char *new_data = (char *)dhe_encode(data, &new_len);
if (new_data == NULL) {
	dprintf(STDERR_FILENO, "DHE encode failed\n");
	exit(1);
}

#include "end.c"
