#include "jbe.h"

#include "intro.c"

char *new_data = (char *)jbe_encode(data, &new_len);
if (new_data == NULL) {
	dprintf(STDERR_FILENO, "JBE encode failed\n");
	exit(1);
}

#include "end.c"
