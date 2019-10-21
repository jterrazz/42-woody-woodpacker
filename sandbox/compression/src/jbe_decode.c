#include "jbe.h"

#include "intro.c"

char *new_data = (char *)jbe_decode(data, &new_len);
if (new_data == NULL) {
	dprintf(STDERR_FILENO, "JBE decode failed\n");
	exit(1);
}

#include "end.c"
